/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/textctrl.mm
// Purpose:     wxTextCtrl
// Author:      Stefan Csomor
// Modified by: Ryan Norton (MLTE GetLineLength and GetLineText)
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TEXTCTRL

#include "wx/textctrl.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/button.h"
    #include "wx/menu.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/toplevel.h"
#endif

#ifdef __DARWIN__
    #include <sys/types.h>
    #include <sys/stat.h>
#else
    #include <stat.h>
#endif

#if wxUSE_STD_IOSTREAM
    #if wxUSE_IOSTREAMH
        #include <fstream.h>
    #else
        #include <fstream>
    #endif
#endif

#include "wx/filefn.h"
#include "wx/sysopt.h"
#include "wx/thread.h"
#include "wx/textcompleter.h"

#include "wx/osx/private.h"
#include "wx/osx/cocoa/private/textimpl.h"

@interface NSView(EditableView)
- (BOOL)isEditable;
- (void)setEditable:(BOOL)flag;
- (BOOL)isSelectable;
- (void)setSelectable:(BOOL)flag;
@end

// An object of this class is created before the text is modified
// programmatically and destroyed as soon as this is done. It does several
// things, like ensuring that the control is editable to allow setting its text
// at all and eating any unwanted focus loss events from textDidEndEditing:
// which don't really correspond to focus change.
class wxMacEditHelper
{
public :
    wxMacEditHelper( NSView* textView )
    {
        m_viewPreviouslyEdited = ms_viewCurrentlyEdited;
        ms_viewCurrentlyEdited =
        m_textView = textView;
        m_formerEditable = YES;
        if ( textView )
        {
            m_formerEditable = [textView isEditable];
            m_formerSelectable = [textView isSelectable];
            [textView setEditable:YES];
        }
    }

    ~wxMacEditHelper()
    {
        if ( m_textView )
        {
            [m_textView setEditable:m_formerEditable];
            [m_textView setSelectable:m_formerSelectable];
        }

        ms_viewCurrentlyEdited = m_viewPreviouslyEdited;
    }

    // Returns true if this view is the one currently being changed by the
    // program.
    static bool IsCurrentlyEditedView(NSView* v)
    {
        return v == ms_viewCurrentlyEdited;
    }

    // Returns true if this editor is the one currently being modified.
    static bool IsCurrentEditor(wxNSTextFieldEditor* e)
    {
        return e == [(NSTextField*)ms_viewCurrentlyEdited currentEditor];
    }

protected :
    BOOL m_formerEditable ;
    BOOL m_formerSelectable;
    NSView* m_textView;

    // The original value of ms_viewCurrentlyEdited when this object was
    // created.
    NSView* m_viewPreviouslyEdited;

    static NSView* ms_viewCurrentlyEdited;
} ;

NSView* wxMacEditHelper::ms_viewCurrentlyEdited = nil;

// an NSFormatter used to implement SetMaxLength() and ForceUpper() methods
@interface wxTextEntryFormatter : NSFormatter
{
    int maxLength;
    wxTextEntry* field;
    bool forceUpper;
}

@end

@implementation wxTextEntryFormatter

- (id)init
{
    if ( self = [super init] )
    {
        maxLength = 0;
        forceUpper = false;
    }
    return self;
}

- (void) setMaxLength:(int) maxlen
{
    maxLength = maxlen;
}

- (void) forceUpper
{
    forceUpper = true;
}

- (NSString *)stringForObjectValue:(id)anObject
{
    if(![anObject isKindOfClass:[NSString class]])
        return nil;
    return [NSString stringWithString:anObject];
}

- (BOOL)getObjectValue:(id *)obj forString:(NSString *)string errorDescription:(NSString  **)error
{
    *obj = [NSString stringWithString:string];
    return YES;
}

- (BOOL)isPartialStringValid:(NSString **)partialStringPtr proposedSelectedRange:(NSRangePointer)proposedSelRangePtr
              originalString:(NSString *)origString originalSelectedRange:(NSRange)origSelRange errorDescription:(NSString **)error
{
    if ( maxLength > 0 )
    {
        if ( [*partialStringPtr length] > maxLength )
        {
            field->SendMaxLenEvent();
            return NO;
        }
    }

    // wxTextEntryFormatter is always associated with single-line text entry (through wxNSTextFieldControl)
    // so all new line characters should be replaced with spaces (like it is done in single-line NSCell).
    NSString* lineStr = [*partialStringPtr stringByReplacingOccurrencesOfString: @"\n" withString: @" "];

    NSString* newStr = forceUpper ? [lineStr uppercaseString] : lineStr;

    if ( ![*partialStringPtr isEqual:newStr] )
    {
        *partialStringPtr = newStr;
        return NO;
    }

    return YES;
}

- (void) setTextEntry:(wxTextEntry*) tf
{
    field = tf;
}

@end

@implementation wxNSSecureTextField

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (void)controlTextDidChange:(NSNotification *)aNotification
{
    wxUnusedVar(aNotification);
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl )
        impl->controlTextDidChange();
}

- (void)controlTextDidEndEditing:(NSNotification *)aNotification
{
    wxUnusedVar(aNotification);
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl )
        impl->DoNotifyFocusLost();
}

- (BOOL)control:(NSControl*)control textView:(NSTextView*)textView doCommandBySelector:(SEL)commandSelector
{
    wxUnusedVar(textView);

    BOOL handled = NO;

    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( control );
    if ( impl  )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer )
        {
            if (commandSelector == @selector(insertNewline:))
            {
                if ( wxpeer->GetWindowStyle() & wxTE_PROCESS_ENTER )
                {
                    wxCommandEvent event(wxEVT_TEXT_ENTER, wxpeer->GetId());
                    event.SetEventObject( wxpeer );
                    wxTextWidgetImpl* impl = (wxNSTextFieldControl * ) wxWidgetImpl::FindFromWXWidget( self );
                    wxTextEntry * const entry = impl->GetTextEntry();
                    event.SetString( entry->GetValue() );
                    handled = wxpeer->HandleWindowEvent( event );
                }
                else
                {
                    wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(wxpeer), wxTopLevelWindow);
                    if ( tlw && tlw->GetDefaultItem() )
                    {
                        wxButton *def = wxDynamicCast(tlw->GetDefaultItem(), wxButton);
                        if ( def && def->IsEnabled() )
                        {
                            wxCommandEvent event(wxEVT_BUTTON, def->GetId() );
                            event.SetEventObject(def);
                            def->Command(event);
                            handled = YES;
                        }
                     }
                }
            }
            else if ( commandSelector == @selector(noop:) )
            {
                // We are called with this strange "noop:" selector when an
                // attempt to paste the text into a password text control using
                // Cmd+V is made. Check if we're really in this case and, if
                // we're, do paste as otherwise nothing happens by default
                // which is annoying, it is pretty common and useful to paste
                // passwords, e.g. from a password manager.
                NSEvent* cevent = [[NSApplication sharedApplication] currentEvent];
                if ( [cevent type] == NSKeyDown &&
                        [cevent keyCode] == 9 /* V */ &&
                            ([cevent modifierFlags] & NSCommandKeyMask) == NSCommandKeyMask )
                {
                    wxTextWidgetImpl* impl = (wxNSTextFieldControl * ) wxWidgetImpl::FindFromWXWidget( self );
                    wxTextEntry * const entry = impl->GetTextEntry();
                    entry->Paste();
                    handled = YES;
                }
            }
            else if (wxpeer->GetWindowStyle() & wxTE_PASSWORD)
            {
                // Make TAB and ESCAPE work on password fields. The general fix done in wxWidgetCocoaImpl::DoHandleKeyEvent()
                // does not help for password fields because wxWidgetCocoaImpl::keyEvent() is not executed
                // when TAB is pressed, only when it is released.
                wxKeyEvent wxevent(wxEVT_KEY_DOWN);
                if (commandSelector == @selector(insertTab:))
                {
                    wxevent.m_keyCode = WXK_TAB;
                }
                else if (commandSelector == @selector(insertBacktab:))
                {
                    wxevent.m_keyCode = WXK_TAB;
                    wxevent.SetShiftDown(true);
                }
                else if (commandSelector == @selector(selectNextKeyView:))
                {
                    wxevent.m_keyCode = WXK_TAB;
                    wxevent.SetRawControlDown(true);
                }
                else if (commandSelector == @selector(selectPreviousKeyView:))
                {
                    wxevent.m_keyCode = WXK_TAB;
                    wxevent.SetShiftDown(true);
                    wxevent.SetRawControlDown(true);
                }
                else if (commandSelector == @selector(cancelOperation:))
                {
                    wxevent.m_keyCode = WXK_ESCAPE;
                }
                if (wxevent.GetKeyCode() != WXK_NONE)
                {
                    wxKeyEvent eventHook(wxEVT_CHAR_HOOK, wxevent);
                    if (wxpeer->OSXHandleKeyEvent(eventHook) && !eventHook.IsNextEventAllowed())
                        handled = YES;
                    else if (impl->DoHandleKeyNavigation(wxevent))
                        handled = YES;
                }
            }
        }
    }

    return handled;
}

@end

// Borrows from "ScalingScrollView.h" and "ScalingScrollView.h" in the TextEdit sources.
@interface wxNSTextScrollView : NSScrollView
- (void)setScaleFactor:(CGFloat)factor;
- (CGFloat)scaleFactor;

- (IBAction)zoomToActualSize:(id)sender;
- (IBAction)zoomIn:(id)sender;
- (IBAction)zoomOut:(id)sender;
@end

@implementation wxNSTextScrollView

- (CGFloat)scaleFactor {
    return [self magnification];
}

- (void)setScaleFactor:(CGFloat)newScaleFactor {
    //[[self animator] setMagnification:newScaleFactor];
    [self setMagnification:newScaleFactor];
}

/* Action methods
*/
- (IBAction)zoomToActualSize:(id)sender {
    [[self animator] setMagnification:1.0];
}

- (IBAction)zoomIn:(id)sender {
    CGFloat scaleFactor = [self scaleFactor];
    scaleFactor = (scaleFactor > 0.4 && scaleFactor < 0.6) ? 1.0 : scaleFactor * 2.0;
    [[self animator] setMagnification:scaleFactor];
}

- (IBAction)zoomOut:(id)sender {
    CGFloat scaleFactor = [self scaleFactor];
    scaleFactor = (scaleFactor > 1.8 && scaleFactor < 2.2) ? 1.0 : scaleFactor / 2.0;
    [[self animator] setMagnification:scaleFactor];
}

/* Reassure AppKit that ScalingScrollView supports live resize content preservation, even though it's a subclass that could have modified NSScrollView in such a way as to make NSScrollView's live resize content preservation support inoperative. By default this is disabled for NSScrollView subclasses.
*/
- (BOOL)preservesContentDuringLiveResize {
    return [self drawsBackground];
}

- (void)viewDidEndLiveResize
{
    [super viewDidEndLiveResize];
}

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

@end

@implementation wxNSTextFieldEditor

- (void) keyDown:(NSEvent*) event
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( (WXWidget) [self delegate] );
    lastKeyDownEvent = event;
    if ( impl == NULL || !impl->DoHandleKeyEvent(event) )
        [super keyDown:event];
    lastKeyDownEvent = nil;
}

- (void) keyUp:(NSEvent*) event
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( (WXWidget) [self delegate] );
    if ( impl == NULL || !impl->DoHandleKeyEvent(event) )
        [super keyUp:event];
}

- (void) flagsChanged:(NSEvent*) event
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( (WXWidget) [self delegate] );
    if ( impl == NULL || !impl->DoHandleKeyEvent(event) )
        [super flagsChanged:event];
}

- (BOOL) performKeyEquivalent:(NSEvent*) event
{
    BOOL retval = [super performKeyEquivalent:event];
    return retval;
}

- (void) insertText:(id) str
{
    // We should never generate char events for the text being inserted
    // programmatically.
    if ( !wxMacEditHelper::IsCurrentEditor(self) )
    {
        NSString *text = [str isKindOfClass:[NSAttributedString class]] ? [str string] : str;
        wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( (WXWidget) [self delegate] );
        if ( impl && lastKeyDownEvent && impl->DoHandleCharEvent(lastKeyDownEvent, text) )
            return;
    }

    [super insertText:str];
}

- (BOOL) resignFirstResponder
{
    return [super resignFirstResponder];
}

- (BOOL) becomeFirstResponder
{
    // we need the stored text field, as at this point the delegate is not yet set
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( (WXWidget) textField );

    BOOL r = [super becomeFirstResponder];
    if ( impl != NULL && r )
        impl->DoNotifyFocusSet();

    return r;
}

- (void) setTextField:(NSTextField*) field
{
    textField = field;
}


@end

@implementation wxNSTextView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (void)textDidChange:(NSNotification *)aNotification
{
    wxUnusedVar(aNotification);
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl )
        impl->controlTextDidChange();
}


- (void)changeColor:(id)sender
{
   // Define this just to block the color change messages - these are sent from
   // the shared color/font panel resulting in unwanted changes of color when
   // shared color panel is used (as when using wxColourPickerCtrl for example).
}


- (void) setEnabled:(BOOL) flag
{
    // from Technical Q&A QA1461
    if (flag) {
        [self setTextColor: [NSColor controlTextColor]];

    } else {
        [self setTextColor: [NSColor disabledControlTextColor]];
    }

    [self setSelectable: flag];
    [self setEditable: flag];
}

- (BOOL) isEnabled
{
    return [self isEditable];
}

- (void)textDidEndEditing:(NSNotification *)aNotification
{
    wxUnusedVar(aNotification);

    if ( wxMacEditHelper::IsCurrentlyEditedView(self) )
    {
        // This notification is generated as the result of calling our own
        // wxTextCtrl method (e.g. WriteText()) and doesn't correspond to any
        // real focus loss event so skip generating it.
        return;
    }

    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl )
        impl->DoNotifyFocusLost();
}

-(BOOL)textView:(NSTextView *)aTextView clickedOnLink:(id)link atIndex:(NSUInteger)charIndex
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( aTextView );
    if ( impl  )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer )
        {
            wxMouseEvent evtMouse( wxEVT_LEFT_DOWN );
            wxTextUrlEvent event( wxpeer->GetId(), evtMouse, (long int)charIndex, (long int)charIndex );
            event.SetEventObject( wxpeer );
            wxpeer->HandleWindowEvent( event );
        }
    }
    return NO;
}

- (BOOL)_handleClipboardEvent:(wxEventType)type
{
    wxWidgetImpl *impl = wxWidgetImpl::FindFromWXWidget(self);
    wxWindow* wxpeer = impl ? impl->GetWXPeer() : NULL;
    if ( wxpeer )
    {
        wxClipboardTextEvent evt(type, wxpeer->GetId());
        evt.SetEventObject(wxpeer);
        return wxpeer->HandleWindowEvent(evt);
    }
    return false;
}

- (void)copy:(id)sender
{
    if ( ![self _handleClipboardEvent:wxEVT_TEXT_COPY] )
        [super copy:sender];
}

- (void)cut:(id)sender
{
    if ( ![self _handleClipboardEvent:wxEVT_TEXT_CUT] )
        [super cut:sender];
}

- (void)paste:(id)sender
{
    if ( ![self _handleClipboardEvent:wxEVT_TEXT_PASTE] )
        [super paste:sender];
}

- (void)viewDidEndLiveResize
{
    [super viewDidEndLiveResize];

    // Force a re-layout to fix issues with resizing after setting zoom/magnification.
    NSTextStorage* storage = [self textStorage];
    NSMutableAttributedString* oldString = [storage mutableCopy];

    [self setAlignment:NSRightTextAlignment];
    [self setAlignment:NSLeftTextAlignment];
    [self didChangeText];

    [[self textStorage] setAttributedString:oldString];
}

@end

@implementation wxNSTextField

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (id) initWithFrame:(NSRect) frame
{
    if ( self = [super initWithFrame:frame] )
    {
        fieldEditor = nil;
    }
    return self;
}

- (void) dealloc
{
    [fieldEditor release];
    [super dealloc];
}

- (void) setFieldEditor:(wxNSTextFieldEditor*) editor
{
    if ( editor != fieldEditor )
    {
        [editor retain];
        [fieldEditor release];
        fieldEditor = editor;
    }
}

- (wxNSTextFieldEditor*) fieldEditor
{
    return fieldEditor;
}

- (void) setEnabled:(BOOL) flag
{
    [super setEnabled: flag];

    if (![self drawsBackground]) {
        // Static text is drawn incorrectly when disabled.
        // For an explanation, see
        // http://www.cocoabuilder.com/archive/message/cocoa/2006/7/21/168028
        if (flag) {
            [self setTextColor: [NSColor controlTextColor]];
        } else {
            [self setTextColor: [NSColor secondarySelectedControlColor]];
        }
    }
}

- (void)controlTextDidChange:(NSNotification *)aNotification
{
    wxUnusedVar(aNotification);
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl )
        impl->controlTextDidChange();
}

- (NSArray *)control:(NSControl *)control textView:(NSTextView *)textView completions:(NSArray *)words
 forPartialWordRange:(NSRange)charRange indexOfSelectedItem:(NSInteger*)index
{
    NSMutableArray* matches = [NSMutableArray array];

    wxTextWidgetImpl* impl = (wxNSTextFieldControl * ) wxWidgetImpl::FindFromWXWidget( self );
    wxTextEntry * const entry = impl->GetTextEntry();
    wxTextCompleter * const completer = entry->OSXGetCompleter();
    if ( completer )
    {
        const wxString prefix = entry->GetValue();
        if ( completer->Start(prefix) )
        {
            const wxString
                wordStart = wxCFStringRef::AsString(
                              [[textView string] substringWithRange:charRange]
                            );

            for ( ;; )
            {
                const wxString s = completer->GetNext();
                if ( s.empty() )
                    break;

                // Normally the completer should return only the strings
                // starting with the prefix, but there could be exceptions
                // and, for compatibility with MSW which simply ignores all
                // entries that don't match the current text control contents,
                // we ignore them as well. Besides, our own wxTextCompleterFixed
                // doesn't respect this rule and, moreover, we need to extract
                // just the rest of the string anyhow.
                wxString completion;
                if ( s.StartsWith(prefix, &completion) )
                {
                    // We discarded the entire prefix above but actually we
                    // should include the part of it that consists of the
                    // beginning of the current word, otherwise it would be
                    // lost when completion is accepted as OS X supposes that
                    // our matches do start with the "partial word range"
                    // passed to us.
                    const wxCFStringRef fullWord(wordStart + completion);
                    [matches addObject: fullWord.AsNSString()];
                }
            }
        }
    }

    return matches;
}

- (BOOL)control:(NSControl*)control textView:(NSTextView*)textView doCommandBySelector:(SEL)commandSelector
{
    wxUnusedVar(textView);
    wxUnusedVar(control);

    BOOL handled = NO;

    // send back key events wx' common code knows how to handle

    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl  )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer )
        {
            if (commandSelector == @selector(insertNewline:))
            {
                [textView insertNewlineIgnoringFieldEditor:self];
                handled = YES;
            }
            else if ( commandSelector == @selector(insertTab:))
            {
                [textView insertTabIgnoringFieldEditor:self];
                handled = YES;
            }
            else if ( commandSelector == @selector(insertBacktab:))
            {
                [textView insertTabIgnoringFieldEditor:self];
                handled = YES;
            }
        }
    }

    return handled;
}

- (void)controlTextDidEndEditing:(NSNotification *)aNotification
{
    wxUnusedVar(aNotification);
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl )
    {
        wxNSTextFieldControl* timpl = dynamic_cast<wxNSTextFieldControl*>(impl);
        if ( timpl )
            timpl->UpdateInternalSelectionFromEditor(fieldEditor);
        impl->DoNotifyFocusLost();
    }
}
@end


bool wxNSTextBase::ShouldHandleKeyNavigation(const wxKeyEvent &event) const
{
    // Text controls must be allowed to handle the key even if wxWANTS_CHARS is not set, provided wxTE_PROCESS_TAB
    // is set. To make Shift+TAB work with text controls we must process it here regardless of wxTE_PROCESS_TAB.
    // For Ctrl(+Shift)+TAB to work as navigation key consistently in all types of text fields we must process it here as well.
    return (!m_wxPeer->HasFlag(wxTE_PROCESS_TAB) || event.HasAnyModifiers());
}

// wxNSTextViewControl

// Official Apple docs suggest to use FLT_MAX when embedding an NSTextView
// object inside an NSScrollView, see here:
// https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/TextUILayer/Tasks/TextInScrollView.html
// However, when using FLT_MAX, "setAlignment" doesn't work any more; using
// 1000000 instead of FLT_MAX fixes this
#define MAX_WIDTH 1000000

wxNSTextViewControl::wxNSTextViewControl( wxTextCtrl *wxPeer, WXWidget w, long style )
    : wxNSTextBase(wxPeer, w)
{
    wxNSTextScrollView* sv = (wxNSTextScrollView*) w;
    m_scrollView = sv;

    const bool hasHScroll = (style & wxHSCROLL) != 0;
    m_useCharWrapping = (style & wxTE_CHARWRAP) != 0;

    [m_scrollView setHasVerticalScroller:YES];
    [m_scrollView setHasHorizontalScroller:hasHScroll];
    [m_scrollView setHasVerticalScroller:(style & wxTE_NO_VSCROLL)? NO: YES];
    NSSize contentSize = [m_scrollView contentSize];
    NSRect viewFrame = NSMakeRect(
            0, 0,
            hasHScroll ? MAX_WIDTH : contentSize.width, contentSize.height
        );

    wxNSTextView* const tv = [[wxNSTextView alloc] initWithFrame: viewFrame];
    m_textView = tv;
    [tv setVerticallyResizable:YES];
    [tv setHorizontallyResizable:hasHScroll];
    [tv setAutoresizingMask:NSViewWidthSizable];

    if ( hasHScroll )
    {
        [[tv textContainer] setContainerSize:NSMakeSize(MAX_WIDTH, MAX_WIDTH)];
        [[tv textContainer] setWidthTracksTextView:NO];
    }

    if ( style & wxTE_RIGHT)
    {
        [tv setAlignment:NSRightTextAlignment];
    }
    else if ( style & wxTE_CENTRE)
    {
        [tv setAlignment:NSCenterTextAlignment];
    }

    if ( !wxPeer->HasFlag(wxTE_RICH | wxTE_RICH2) )
    {
        [tv setRichText:NO];
        [tv setUsesFontPanel:NO];
    }

    if ( wxPeer->HasFlag(wxTE_AUTO_URL) )
    {
        [tv setAutomaticLinkDetectionEnabled:YES];
    }

    [m_scrollView setDocumentView: tv];

    [tv setDelegate: tv];

    m_undoManager = nil;
    if ( wxPeer && wxPeer->GetParent() && wxPeer->GetParent()->GetPeer() && wxPeer->GetParent()->GetPeer()->GetWXWidget() )
        m_undoManager = [wxPeer->GetParent()->GetPeer()->GetWXWidget() undoManager];

    [tv setAllowsUndo:YES];

    InstallEventHandler(tv);
}

wxNSTextViewControl::~wxNSTextViewControl()
{
    if (m_textView)
        [m_textView setDelegate: nil];
}

bool wxNSTextViewControl::CanFocus() const
{
    // since this doesn't work (return false), we hardcode
    // if (m_textView)
    //    return [m_textView canBecomeKeyView];
    return true;
}

void wxNSTextViewControl::insertText(NSString* str, WXWidget slf, void *_cmd)
{
    NSString *text = [str isKindOfClass:[NSAttributedString class]] ? [(id)str string] : str;
    if ( m_lastKeyDownEvent ==NULL || !DoHandleCharEvent(m_lastKeyDownEvent, text) )
    {
        wxOSX_TextEventHandlerPtr superimpl = (wxOSX_TextEventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
        superimpl(slf, (SEL)_cmd, str);
    }
}

wxString wxNSTextViewControl::GetStringValue() const
{
    wxString result;
    if (m_textView)
    {
        result = wxMacConvertNewlines13To10(
            wxCFStringRef::AsString([m_textView string], m_wxPeer->GetFont().GetEncoding()));
    }
    return result;
}

void wxNSTextViewControl::SetStringValue( const wxString &str)
{
    wxMacEditHelper helper(m_textView);

    if (m_textView)
    {
        wxString st(wxMacConvertNewlines10To13(str));
        [m_textView setString: wxCFStringRef( st , m_wxPeer->GetFont().GetEncoding() ).AsNSString()];
        if ( m_wxPeer->HasFlag(wxTE_AUTO_URL) )
        {
            // Make sure that any URLs in the new text are highlighted.
            [m_textView checkTextInDocument:nil];
        }
        // Some text styles have to be updated manually.
        DoUpdateTextStyle();
    }
}

void wxNSTextViewControl::Copy()
{
    if (m_textView)
        [m_textView copy:nil];

}

void wxNSTextViewControl::Cut()
{
    if (m_textView)
        [m_textView cut:nil];
}

void wxNSTextViewControl::Paste()
{
    if (m_textView)
        [m_textView paste:nil];
}

bool wxNSTextViewControl::CanPaste() const
{
    return true;
}

void wxNSTextViewControl::SetEditable(bool editable)
{
    if (m_textView)
        [m_textView setEditable: editable];
}

long wxNSTextViewControl::GetLastPosition() const
{
    wxCHECK( m_textView, 0 );
    return [[m_textView string] length];
}

void wxNSTextViewControl::GetSelection( long* from, long* to) const
{
    if (m_textView)
    {
        NSRange range = [m_textView selectedRange];
        *from = range.location;
        *to = range.location + range.length;
    }
}

void wxNSTextViewControl::SetSelection( long from , long to )
{
    long textLength = [[m_textView string] length];
    if ((from == -1) && (to == -1))
    {
        from = 0 ;
        to = textLength ;
    }
    else
    {
        from = wxMin(textLength,wxMax(from,0)) ;
        if ( to == -1 )
            to = textLength;
        else
            to = wxMax(0,wxMin(textLength,to)) ;
    }

    NSRange selrange = NSMakeRange(from, to-from);
    [m_textView setSelectedRange:selrange];
    [m_textView scrollRangeToVisible:selrange];
}

bool wxNSTextViewControl::PositionToXY(long pos, long *x, long *y) const
{
    wxCHECK( m_textView, false );
    wxCHECK_MSG( pos >= 0, false, wxS("Invalid character position") );

    NSString* txt = [m_textView string];
    if ( pos > [txt length] )
        return false;

    // Last valid position is past the last character
    // of the text control, so add one virtual character
    // at the end to make calculations easier.
    txt = [txt stringByAppendingString:@"x"];

    long nline = 0;
    NSRange lineRng;
    for ( long i = 0; i <= pos; nline++ )
    {
        lineRng = [txt lineRangeForRange:NSMakeRange(i, 0)];
        i = NSMaxRange(lineRng);
    }

    if ( y )
        *y = nline-1;

    if ( x )
        *x = pos - lineRng.location;

    return true;
}

long wxNSTextViewControl::XYToPosition(long x, long y) const
{
    wxCHECK( m_textView, -1 );
    wxCHECK_MSG( x >= 0 && y >= 0, -1, wxS("Invalid line/column number") );

    NSString* txt = [m_textView string];
    const long txtLen = [txt length];
    long nline = -1;
    NSRange lineRng;
    long i = 0;
    // We need to enter the counting loop at least once,
    // even for empty text string.
    do
    {
        nline++;
        lineRng = [txt lineRangeForRange:NSMakeRange(i, 0)];
        i = NSMaxRange(lineRng);
    } while ( i < txtLen && nline < y );
    // In the last line, the last valid position is after
    // the last real character, so we need to count
    // this additional virtual character.
    // In any other line, the last valid position is at
    // the new line character which is already counted.
    if ( i == txtLen )
    {
        const bool endsWithNewLine = txtLen > 0 &&
                                     [txt characterAtIndex:txtLen-1] == '\n';
        // If text ends with new line, simulated character
        // is placed in the additional virtual line.
        if ( endsWithNewLine )
        {
            //  Creating additional virtual line makes sense only
            // if we look for a position beyond the last real line.
            if ( nline < y )
            {
                nline++;
                lineRng = NSMakeRange(i, 1);
            }
        }
        else
        {
            // Just extended actual range to count
            // additional virtual character
            lineRng.length++;
        }
    }
    // Return error if control contains
    // less lines than given y position.
    if ( nline != y )
        return  -1;

    // Return error if given x position
    // is past the line.
    if ( x >= lineRng.length )
        return  -1;

    return lineRng.location + x;
}

void wxNSTextViewControl::ShowPosition(long pos)
{
    if ( !m_textView )
        return;

    wxCHECK_RET( pos >= 0, wxS("Invalid character position") );
    [m_textView scrollRangeToVisible:NSMakeRange(pos, 1)];
}

void wxNSTextViewControl::WriteText(const wxString& str)
{
    wxString st(wxMacConvertNewlines10To13(str));
    wxMacEditHelper helper(m_textView);
    NSEvent* formerEvent = m_lastKeyDownEvent;
    m_lastKeyDownEvent = nil;
    [m_textView insertText:wxCFStringRef( st , m_wxPeer->GetFont().GetEncoding() ).AsNSString()];
    m_lastKeyDownEvent = formerEvent;
    // Some text styles have to be updated manually.
    DoUpdateTextStyle();
}

void wxNSTextViewControl::controlTextDidChange()
{
    wxNSTextBase::controlTextDidChange();
    // Some text styles have to be updated manually.
    DoUpdateTextStyle();
}

bool wxNSTextViewControl::CanUndo() const
{
    if ( !m_undoManager )
        return false;

    return [m_undoManager canUndo];
}

void wxNSTextViewControl::Undo()
{
    if ( !m_undoManager )
        return;

    [m_undoManager undo];
}

bool wxNSTextViewControl::CanRedo() const
{
    if ( !m_undoManager )
        return false;

    return [m_undoManager canRedo];
}

void wxNSTextViewControl::Redo()
{
    if ( !m_undoManager )
        return;

    [m_undoManager redo];
}

void wxNSTextViewControl::EmptyUndoBuffer()
{
    if ( !m_undoManager )
        return;

    [m_undoManager removeAllActions];
}

void wxNSTextViewControl::DoUpdateTextStyle()
{
    if ( m_useCharWrapping )
    {
        // Set line wrapping at any position
        // by applying a style to the entire text.
        NSTextStorage* storage = [m_textView textStorage];
        NSMutableParagraphStyle* style = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
        [style setLineBreakMode:NSLineBreakByCharWrapping];
        [storage addAttribute:NSParagraphStyleAttributeName value:style range:NSMakeRange(0, [storage length])];
        [style release];
    }
}

void wxNSTextViewControl::SetFont( const wxFont & font , const wxColour& WXUNUSED(foreground) , long WXUNUSED(windowStyle), bool WXUNUSED(ignoreBlack) )
{
    if ([m_textView respondsToSelector:@selector(setFont:)])
        [m_textView setFont: font.OSXGetNSFont()];
}

static double PointsFromInternalUnits(int units, double unitsPerPoint)
{
  return (double)units / unitsPerPoint;
}

static int InternalUnitsFromPoints(double pts, double unitsPerPoint)
{
  return wxRound(pts * unitsPerPoint);
}

NSString * const wxNSParagraphUserDataAttributeName = @"wxParagraphUserData";

bool wxNSTextViewControl::GetStyle(long position, wxTextAttr& style)
{
    if ( m_textView && position >= 0 )
    {   
        NSFont* font = NULL;
        NSColor* bgcolor = NULL;
        NSColor* fgcolor = NULL;
        NSNumber* ultype = NULL;
        NSColor* ulcolor = NULL;
        NSParagraphStyle* paraStyle = NULL;
        int strikethroughVal = 0;
        int superscriptVal = 0;
        long paragraphUserData = 0;

        // NOTE: It appears that other platforms accept GetStyle with the position == length
        // but that NSTextStorage does not accept length as a valid position.
        // Therefore we return the default control style in that case.
        if ( position < (long) [[m_textView string] length] )
        {
            NSTextStorage* storage = [m_textView textStorage];
            font = [storage attribute:NSFontAttributeName atIndex:position effectiveRange:NULL];
            bgcolor = [storage attribute:NSBackgroundColorAttributeName atIndex:position effectiveRange:NULL];
            fgcolor = [storage attribute:NSForegroundColorAttributeName atIndex:position effectiveRange:NULL];
            ultype = [storage attribute:NSUnderlineStyleAttributeName atIndex:position effectiveRange:NULL];
            ulcolor = [storage attribute:NSUnderlineColorAttributeName atIndex:position effectiveRange:NULL];
            paraStyle = [storage attribute:NSParagraphStyleAttributeName atIndex:position effectiveRange:NULL];
            strikethroughVal = [[storage attribute:NSStrikethroughStyleAttributeName atIndex:position effectiveRange:NULL] integerValue];
            superscriptVal = [[storage attribute:NSSuperscriptAttributeName atIndex:position effectiveRange:NULL] integerValue];
            paragraphUserData = [[storage attribute:wxNSParagraphUserDataAttributeName atIndex:position effectiveRange:NULL] longValue];
        }
        else
        {
            NSDictionary* attrs = [m_textView typingAttributes];
            font = [attrs objectForKey:NSFontAttributeName];
            bgcolor = [attrs objectForKey:NSBackgroundColorAttributeName];
            fgcolor = [attrs objectForKey:NSForegroundColorAttributeName];
            ultype = [attrs objectForKey:NSUnderlineStyleAttributeName];
            ulcolor = [attrs objectForKey:NSUnderlineColorAttributeName];
        }

        if (font)
            style.SetFont(wxFont(font));

        if (bgcolor)
            style.SetBackgroundColour(wxColour(bgcolor));

        if (fgcolor)
            style.SetTextColour(wxColour(fgcolor));

        wxTextAttrUnderlineType underlineType = wxTEXT_ATTR_UNDERLINE_NONE;
        if ( ultype )
        {
            NSInteger ulval = [ultype integerValue];
            switch ( ulval )
            {
                case NSUnderlineStyleSingle:
                    underlineType = wxTEXT_ATTR_UNDERLINE_SOLID;
                    break;
                case NSUnderlineStyleDouble:
                    underlineType = wxTEXT_ATTR_UNDERLINE_DOUBLE;
                    break;
                case NSUnderlineStyleSingle | NSUnderlinePatternDot:
                    underlineType = wxTEXT_ATTR_UNDERLINE_SPECIAL;
                    break;
                default:
                    underlineType = wxTEXT_ATTR_UNDERLINE_NONE;
                    break;
            }
        }

        wxColour underlineColour;
        if ( ulcolor )
            underlineColour = wxColour(ulcolor);

        if ( underlineType != wxTEXT_ATTR_UNDERLINE_NONE )
            style.SetFontUnderlined(underlineType, underlineColour);

        int textEffects = 0;

        // It's not obvious whether strikethrough is denoted
        // definitively by wxTEXT_ATTR_EFFECT_STRIKETHROUGH
        // or by wxTEXT_ATTR_FONT_STRIKETHROUGH, so set both.
        style.SetFontStrikethrough(strikethroughVal != NSUnderlineStyleNone);
        switch ( strikethroughVal )
        {
            case NSUnderlineStyleSingle:
                textEffects |= wxTEXT_ATTR_EFFECT_STRIKETHROUGH;
                break;
            case NSUnderlineStyleDouble:
                textEffects |= wxTEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH;
                break;
            default:
                break;
        }

        if ( superscriptVal > 0 )
            textEffects |= wxTEXT_ATTR_EFFECT_SUPERSCRIPT;
        if ( superscriptVal < 0 )
            textEffects |= wxTEXT_ATTR_EFFECT_SUBSCRIPT;

        style.SetParagraphUserData(paragraphUserData);

        int styleAlignment = [paraStyle alignment];
        switch ( styleAlignment )
        {
            case NSRightTextAlignment:
                style.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
                break;
            case NSCenterTextAlignment:
                style.SetAlignment(wxTEXT_ALIGNMENT_CENTER);
                break;
            case NSJustifiedTextAlignment:
                style.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);
                break;
            case NSLeftTextAlignment:
                style.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
                break;
            case NSNaturalTextAlignment:
                style.SetAlignment(wxTEXT_ALIGNMENT_DEFAULT);
                break;
        }

        style.SetParagraphSpacingBefore(InternalUnitsFromPoints([paraStyle paragraphSpacingBefore], style.GetUnitsPerPoint()));
        style.SetParagraphSpacingAfter(InternalUnitsFromPoints([paraStyle paragraphSpacing], style.GetUnitsPerPoint()));

        style.SetLeftIndent(InternalUnitsFromPoints([paraStyle firstLineHeadIndent], style.GetUnitsPerPoint()),
                            InternalUnitsFromPoints([paraStyle headIndent], style.GetUnitsPerPoint()));
        style.SetRightIndent(InternalUnitsFromPoints(-[paraStyle tailIndent], style.GetUnitsPerPoint()));

        style.SetLineSpacing(wxRound([paraStyle lineHeightMultiple] * 10.0));
        style.SetLineSpacingMode(wxTEXT_ATTR_LINE_SPACING_MODE_MULTIPLE);
        if ( [paraStyle minimumLineHeight] != 0.0 )
        {
            style.SetLineSpacing(InternalUnitsFromPoints([paraStyle minimumLineHeight], style.GetUnitsPerPoint()));
            style.SetLineSpacingMode(wxTEXT_ATTR_LINE_SPACING_MODE_EXACT);
        }

        if ( [paraStyle baseWritingDirection] == NSWritingDirectionRightToLeft ) // NSWritingDirectionAttributeName
            textEffects |= wxTEXT_ATTR_EFFECT_RTL;

        if ( [paraStyle hyphenationFactor] == 0.0 )
            textEffects |= wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION;

        style.SetTextEffects(textEffects);
        style.SetTextEffectFlags(wxTEXT_ATTR_EFFECT_SUPERSCRIPT | wxTEXT_ATTR_EFFECT_SUBSCRIPT |
                                 wxTEXT_ATTR_EFFECT_RTL | wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);

        NSArray* tabArray = [paraStyle tabStops];
        NSUInteger count = [tabArray count];
        if ( count > 0 )
        {
            wxArrayInt tabs;
            for (int i = 0; i < count; i++)
            {
                NSTextTab* tab = [tabArray objectAtIndex:i];
                if ( tab )
                {
                    tabs.Add( InternalUnitsFromPoints([tab location], style.GetUnitsPerPoint()) );
                }
            }
            style.SetTabs( tabs );
        }

      return true;
    }

    return false;
}

void wxNSTextViewControl::SetStyle(long start,
                                long end,
                                const wxTextAttr& style)
{
    if ( !m_textView )
        return;

    NSTextStorage* storage = [m_textView textStorage];

    NSRange range = NSMakeRange(start, end-start);
    if ( ![m_textView shouldChangeTextInRange:range replacementString:nil] ) // for Undo
        return;

    @try
    {
        int textsEffects = style.GetTextEffects();

        if ( style.IsCharacterStyle() )
        {
            // It's not obvious whether strikethrough is denoted
            // definitively by wxTEXT_ATTR_EFFECT_STRIKETHROUGH
            // or by wxTEXT_ATTR_FONT_STRIKETHROUGH, so handle both.
            int strikeThroughStyle = NSUnderlineStyleNone; // Should be type NSUnderlineStyle, but that fails xcode6.4 test.
            if ( style.HasTextEffects() )
            {
                if ( textsEffects & wxTEXT_ATTR_EFFECT_STRIKETHROUGH )
                    strikeThroughStyle = NSUnderlineStyleSingle;
                else if ( textsEffects & wxTEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH )
                    strikeThroughStyle = NSUnderlineStyleDouble;
            }
            if ( strikeThroughStyle == NSUnderlineStyleNone && style.GetFontStrikethrough() )
                strikeThroughStyle = NSUnderlineStyleSingle;

            int underlineStyle = NSUnderlineStyleNone; // Should be type NSUnderlineStyle, but that fails xcode6.4 test.
            switch ( style.GetUnderlineType() )
            {
                case wxTEXT_ATTR_UNDERLINE_SOLID:
                    underlineStyle = NSUnderlineStyleSingle;
                    break;
                case wxTEXT_ATTR_UNDERLINE_DOUBLE:
                    underlineStyle = NSUnderlineStyleDouble;
                    break;
                case wxTEXT_ATTR_UNDERLINE_SPECIAL:
                    underlineStyle = NSUnderlineStyleSingle | NSUnderlinePatternDot;
                    break;
                default:
                    underlineStyle = NSUnderlineStyleNone;
                    break;
            }

            int superSubScriptLevel = 0;
            if ( style.HasTextEffects() )
            {
                if ( textsEffects & wxTEXT_ATTR_EFFECT_SUPERSCRIPT )
                    superSubScriptLevel = 1;
                else if ( textsEffects & wxTEXT_ATTR_EFFECT_SUBSCRIPT )
                    superSubScriptLevel = -1;
            }

            if ( start == end )
            {
                NSMutableDictionary* attrs = [m_textView.typingAttributes mutableCopy];
                if ( style.HasFont() )
                {
                    wxTextAttr oldStyle;
                    GetStyle(start, oldStyle);
                    oldStyle.Apply(style);
                    [attrs setValue:oldStyle.GetFont().OSXGetNSFont() forKey:NSFontAttributeName];
                }
                if ( style.HasBackgroundColour() )
                    [attrs setValue:style.GetBackgroundColour().OSXGetNSColor() forKey:NSBackgroundColorAttributeName];
                if ( style.HasTextColour() )
                    [attrs setValue:style.GetTextColour().OSXGetNSColor() forKey:NSForegroundColorAttributeName];
                if ( style.HasFontUnderlined() )
                {
                    [attrs setValue:[NSNumber numberWithInt:underlineStyle] forKey:NSUnderlineStyleAttributeName];
                    wxColour colour = style.GetUnderlineColour();
                    if ( colour.IsOk() )
                      [attrs setValue:colour.OSXGetNSColor() forKey:NSUnderlineColorAttributeName];
                }
                if ( style.HasFontStrikethrough() )
                    [attrs setValue:[NSNumber numberWithInt:strikeThroughStyle] forKey:NSStrikethroughStyleAttributeName];
                if ( style.HasTextEffect(wxTEXT_ATTR_EFFECT_SUPERSCRIPT) || style.HasTextEffect(wxTEXT_ATTR_EFFECT_SUBSCRIPT) )
                    [attrs setValue:[NSNumber numberWithInt:superSubScriptLevel] forKey:NSSuperscriptAttributeName];

                [m_textView setTypingAttributes:attrs];
            }
            else // Set the attributes just for this range.
            {
                if ( style.HasFontFaceName() || style.HasFontFamily() || style.HasFontEncoding() || style.HasFontSize() )
                {
                    if ( ( style.HasFontFaceName() || style.HasFontFamily() || style.HasFontEncoding() ) && style.HasFontSize() )
                    {
                        // Change all the text in the range to have the exact same font, size, etc.
                        [storage addAttribute:NSFontAttributeName value:style.GetFont().OSXGetNSFont() range:range];
                    }
                    else
                    {
                        // Change just the specified attribute(s), and leave everything else alone.
                        NSRange foundRange, searchRange = range;
                        for ( unsigned int maxSelRange = NSMaxRange(range); searchRange.location < maxSelRange;
                             searchRange = NSMakeRange(NSMaxRange(foundRange), maxSelRange - NSMaxRange(foundRange)) )
                        {
                            NSFont *font = [storage attribute: NSFontAttributeName
                                               atIndex: searchRange.location
                                               longestEffectiveRange: &foundRange
                                               inRange: searchRange];

                            if ( font != nil )
                            {
                                NSFont *newFont = nil;

                                if ( style.HasFontSize() )
                                    newFont = [[NSFontManager sharedFontManager] convertFont:font toSize:style.GetFractionalFontSize()];
                                else if ( style.HasFontFaceName() )
                                    newFont = [[NSFontManager sharedFontManager] convertFont:font
                                                                                    toFamily:wxCFStringRef(style.GetFontFaceName()).AsNSString()];

                                if ( newFont != nil )
                                    [storage addAttribute:NSFontAttributeName value:newFont range:foundRange];
                            }
                        }
                    }
                }
                if ( style.HasFontWeight() )
                    [storage applyFontTraits:((style.GetFontWeight() >= wxFONTWEIGHT_BOLD) ? NSBoldFontMask : NSUnboldFontMask) range:range];
                if ( style.HasFontItalic() )
                    [storage applyFontTraits:((style.GetFontStyle() >= wxFONTSTYLE_ITALIC) ? NSItalicFontMask : NSUnitalicFontMask) range:range];
                if ( style.HasBackgroundColour() )
                    [storage addAttribute:NSBackgroundColorAttributeName value:style.GetBackgroundColour().OSXGetNSColor() range:range];
                if ( style.HasTextColour() )
                    [storage addAttribute:NSForegroundColorAttributeName value:style.GetTextColour().OSXGetNSColor() range:range];
                if ( style.HasFontUnderlined() )
                {
                    [storage addAttribute:NSUnderlineStyleAttributeName value:[NSNumber numberWithInt:underlineStyle] range:range];
                    wxColour colour = style.GetUnderlineColour();
                    if ( colour.IsOk() )
                      [storage addAttribute:NSUnderlineStyleAttributeName value:colour.OSXGetNSColor() range:range];
                }
                if ( style.HasFontStrikethrough() )
                    [storage addAttribute:NSStrikethroughStyleAttributeName value:[NSNumber numberWithInt:strikeThroughStyle] range:range];
                if ( style.HasTextEffect(wxTEXT_ATTR_EFFECT_SUPERSCRIPT) || style.HasTextEffect(wxTEXT_ATTR_EFFECT_SUBSCRIPT) )
                    [storage addAttribute:NSSuperscriptAttributeName value:[NSNumber numberWithInt:superSubScriptLevel] range:range];
            }
        }

        if ( style.IsParagraphStyle() || style.HasTextEffect(wxTEXT_ATTR_EFFECT_RTL) || style.HasTextEffect(wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION) )
        {
            NSRange range = NSMakeRange(start, end-start);
            NSUInteger startIndex = 0;
            NSUInteger endIndex = 0;

            [[[m_textView textStorage] mutableString] getParagraphStart:&startIndex end:&endIndex contentsEnd:nil forRange:range];

            range.location = startIndex;
            range.length = endIndex - startIndex;

            if ( ![m_textView shouldChangeTextInRange:range replacementString:nil] ) // for Undo
                return;

            if ( style.HasAlignment() )
            {
                switch ( style.GetAlignment() )
                {
                    case wxTEXT_ALIGNMENT_RIGHT:
                        [m_textView setAlignment:NSRightTextAlignment range:range];
                        break;
                    case wxTEXT_ALIGNMENT_CENTER:
                        [m_textView setAlignment:NSCenterTextAlignment range:range];
                        break;
                    case wxTEXT_ALIGNMENT_JUSTIFIED:
                        [m_textView setAlignment:NSJustifiedTextAlignment range:range];
                        break;
                    case wxTEXT_ALIGNMENT_LEFT:
                        [m_textView setAlignment:NSLeftTextAlignment range:range];
                        break;
                    case wxTEXT_ALIGNMENT_DEFAULT:
                        [m_textView setAlignment:NSNaturalTextAlignment range:range];
                        break;
                }
            }

            NSMutableParagraphStyle* paraStyle;
            {
                NSDictionary* attributes = [storage attributesAtIndex:startIndex effectiveRange: NULL];
                NSParagraphStyle* immutableStyle = [attributes valueForKey: NSParagraphStyleAttributeName];
                if ( immutableStyle )
                {
                    paraStyle = [immutableStyle mutableCopy];
                }
                else
                {
                    paraStyle = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
                }
            }

            if ( style.HasParagraphSpacingBefore() )
                [paraStyle setParagraphSpacingBefore:PointsFromInternalUnits(style.GetParagraphSpacingBefore(), style.GetUnitsPerPoint())];
            if ( style.HasParagraphSpacingAfter() )
                [paraStyle setParagraphSpacing:PointsFromInternalUnits(style.GetParagraphSpacingAfter(), style.GetUnitsPerPoint())];

            if ( style.HasLeftIndent() )
            {
                [paraStyle setFirstLineHeadIndent:PointsFromInternalUnits(style.GetLeftIndent(), style.GetUnitsPerPoint())];
                [paraStyle setHeadIndent:PointsFromInternalUnits(style.GetLeftSubIndent(), style.GetUnitsPerPoint())];
            }
            if ( style.HasRightIndent() )
                [paraStyle setTailIndent:PointsFromInternalUnits(-style.GetRightIndent(), style.GetUnitsPerPoint())];

            if ( style.HasLineSpacing() )
            {
                [paraStyle setLineSpacing:0]; // Space "between" lines; not really what we mean by line spacing here.

                switch ( style.GetLineSpacingMode() )
                {
                    case wxTEXT_ATTR_LINE_SPACING_MODE_MULTIPLE:
                        [paraStyle setLineHeightMultiple:style.GetLineSpacing() / 10.0];
                        [paraStyle setMinimumLineHeight:0];
                        [paraStyle setMaximumLineHeight:0];
                        break;

                    case wxTEXT_ATTR_LINE_SPACING_MODE_EXACT:
                        [paraStyle setLineHeightMultiple:1];
                        [paraStyle setMinimumLineHeight:PointsFromInternalUnits(style.GetLineSpacing(), style.GetUnitsPerPoint())];
                        [paraStyle setMaximumLineHeight:[paraStyle minimumLineHeight]];
                        break;
                }
            }

            if ( style.HasTextEffect(wxTEXT_ATTR_EFFECT_RTL) )
            {
                if ( textsEffects & wxTEXT_ATTR_EFFECT_RTL )
                    [paraStyle setBaseWritingDirection:NSWritingDirectionRightToLeft];
                else
                    [paraStyle setBaseWritingDirection:NSWritingDirectionLeftToRight];
            }

            if ( style.HasTextEffect(wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION) )
            {
                if ( textsEffects & wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION )
                    [paraStyle setHyphenationFactor:0.0];
                else
                    [paraStyle setHyphenationFactor:1.0];
            }

            if ( style.HasTabs() )
            {
                while ([[paraStyle tabStops] count] > 0)
                {
                    NSTextTab* tab = [[paraStyle tabStops] objectAtIndex:0];
                    [paraStyle removeTabStop:tab];
                }

                wxArrayInt tabs = style.GetTabs();
                for (int i = 0; i < tabs.size(); i++)
                {
                    long position = tabs[i];
                    NSTextTab* tab = [[NSTextTab alloc]
                                      initWithType:NSLeftTabStopType
                                      location:PointsFromInternalUnits(position, style.GetUnitsPerPoint())];

                    [paraStyle addTabStop:tab];
                }
            }

            [storage removeAttribute:NSParagraphStyleAttributeName range:range];
            [storage addAttribute:NSParagraphStyleAttributeName value:paraStyle range:range];
            [paraStyle release];

            if ( style.HasParagraphUserData() )
            {
                [storage removeAttribute:wxNSParagraphUserDataAttributeName range:range];
                [storage addAttribute:wxNSParagraphUserDataAttributeName value:[NSNumber numberWithLong:style.GetParagraphUserData()] range:range];
            }
        }
    }
    @catch (NSException *exception)
    {
        NSLog(@"%@", exception);
    }

    if ( start != end )
        [m_textView didChangeText]; // for Undo

    // This puts the style change operation as its own item in the Undo stack.
    if ( start != end && !style.IsParagraphStyle() && ![m_textView isCoalescingUndo] && CanUndo() )
    {
        Undo();
        Redo();
    }
}

void wxNSTextViewControl::CheckSpelling(bool check)
{
    if (m_textView)
    {
        [m_textView setContinuousSpellCheckingEnabled: check];
        [m_textView setAutomaticSpellingCorrectionEnabled: check];
    }
}

void wxNSTextViewControl::EnableAutomaticQuoteSubstitution(bool enable)
{
    if (m_textView)
        [m_textView setAutomaticQuoteSubstitutionEnabled:enable];
}

void wxNSTextViewControl::EnableAutomaticDashSubstitution(bool enable)
{
    if (m_textView)
        [m_textView setAutomaticDashSubstitutionEnabled:enable];
}

wxSize wxNSTextViewControl::GetBestSize() const
{
    if (m_textView && [m_textView layoutManager])
    {
        NSRect rect = [[m_textView layoutManager] usedRectForTextContainer: [m_textView textContainer]];
        return wxSize((int)(rect.size.width + [m_textView textContainerInset].width),
                      (int)(rect.size.height + [m_textView textContainerInset].height));
    }
    return wxSize(0,0);
}

void wxNSTextViewControl::SetJustification()
{
    if ( !m_textView )
        return;

    NSTextAlignment align;

    if ( m_wxPeer->HasFlag(wxTE_RIGHT) )
        align = NSRightTextAlignment;
    else if ( m_wxPeer->HasFlag(wxTE_CENTRE) )
        align = NSCenterTextAlignment;
    else // wxTE_LEFT == 0
        align = NSLeftTextAlignment;

    [m_textView setAlignment:align];
}

long wxNSTextViewControl::GetScaleFactor() const
{
    if ( ![m_scrollView isKindOfClass:[wxNSTextScrollView class]] )
        return 100;

    return wxRound([(wxNSTextScrollView*)m_scrollView scaleFactor] * 100.0);
}

void wxNSTextViewControl::SetScaleFactor(long factorPct)
{
    if ( ![m_scrollView isKindOfClass:[wxNSTextScrollView class]] )
        return;

    if ( GetScaleFactor() == factorPct )
        return;

    factorPct = std::abs(factorPct);
    if ( factorPct == 0 )
        factorPct = 100;

    CGFloat zoomFactor = factorPct / 100.0;

    // If zooming in (making text bigger), first go even bigger,
    // then smaller, so that the code below fixes drawing.
    if ( GetScaleFactor() < factorPct )
    {
        static bool bInRecursion = false;
        if ( !bInRecursion )
        {
            bInRecursion = true;
            SetScaleFactor( factorPct + 1 );
            bInRecursion = false;
        }
    }

    // Setting this to NO then back to YES seems to help the text view correctly
    // redraw the text with the proper paragraph alignment after magnification/zoom,
    // at least when the texted is zoomed out (made smaller).
    BOOL wasHorizontallyResizable = [m_textView isHorizontallyResizable];
    if ( !wasHorizontallyResizable )
        [m_textView setHorizontallyResizable:NO];

    [(wxNSTextScrollView*)m_scrollView setScaleFactor:zoomFactor];

    if ( !wasHorizontallyResizable )
        [m_textView setHorizontallyResizable:YES];
}

// wxNSTextFieldControl

wxNSTextFieldControl::wxNSTextFieldControl( wxTextCtrl *text, WXWidget w )
    : wxNSTextBase(text, w)
{
    Init(w);
}

wxNSTextFieldControl::wxNSTextFieldControl(wxWindow *wxPeer,
                                           wxTextEntry *entry,
                                           WXWidget w)
    : wxNSTextBase(wxPeer, entry, w)
{
    Init(w);
}

void wxNSTextFieldControl::Init(WXWidget w)
{
    NSTextField <NSTextFieldDelegate> *tf = (NSTextField <NSTextFieldDelegate>*) w;
    m_textField = tf;
    [m_textField setDelegate: tf];
    m_selStart = m_selEnd = 0;
    m_hasEditor = [w isKindOfClass:[NSTextField class]];

    GetFormatter(); // we always need to at least replace new line characters with spaces
}

wxNSTextFieldControl::~wxNSTextFieldControl()
{
    if (m_textField)
        [m_textField setDelegate: nil];
}

wxString wxNSTextFieldControl::GetStringValue() const
{
    return wxCFStringRef::AsString([m_textField stringValue], m_wxPeer->GetFont().GetEncoding());
}

void wxNSTextFieldControl::SetStringValue( const wxString &str)
{
    wxMacEditHelper helper(m_textField);
    [m_textField setStringValue: wxCFStringRef( str , m_wxPeer->GetFont().GetEncoding() ).AsNSString()];
}

wxTextEntryFormatter* wxNSTextFieldControl::GetFormatter()
{
    // We only ever call setFormatter with wxTextEntryFormatter, so the cast is
    // safe.
    wxTextEntryFormatter*
        formatter = (wxTextEntryFormatter*)[m_textField formatter];
    if ( !formatter )
    {
        formatter = [[[wxTextEntryFormatter alloc] init] autorelease];
        [formatter setTextEntry:GetTextEntry()];
        [m_textField setFormatter:formatter];
    }

    return formatter;
}

void wxNSTextFieldControl::SetMaxLength(unsigned long len)
{
    [GetFormatter() setMaxLength:len];
}

void wxNSTextFieldControl::ForceUpper()
{
    [GetFormatter() forceUpper];
}

void wxNSTextFieldControl::Copy()
{
    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        [editor copy:nil];
    }
}

void wxNSTextFieldControl::Cut()
{
    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        [editor cut:nil];
    }
}

void wxNSTextFieldControl::Paste()
{
    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        [editor paste:nil];
    }
}

bool wxNSTextFieldControl::CanPaste() const
{
    return true;
}

void wxNSTextFieldControl::SetEditable(bool editable)
{
    [m_textField setEditable:editable];
}

long wxNSTextFieldControl::GetLastPosition() const
{
    return [[m_textField stringValue] length];
}

void wxNSTextFieldControl::GetSelection( long* from, long* to) const
{
    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        NSRange range = [editor selectedRange];
        *from = range.location;
        *to = range.location + range.length;
    }
    else
    {
        *from = m_selStart;
        *to = m_selEnd;
    }
}

void wxNSTextFieldControl::SetSelection( long from , long to )
{
    long textLength = [[m_textField stringValue] length];
    if ((from == -1) && (to == -1))
    {
        from = 0 ;
        to = textLength ;
    }
    else
    {
        from = wxMin(textLength,wxMax(from,0)) ;
        if ( to == -1 )
            to = textLength;
        else
            to = wxMax(0,wxMin(textLength,to)) ;
    }

    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        [editor setSelectedRange:NSMakeRange(from, to-from)];
    }

    // the editor might still be in existence, but we might be already passed our 'focus lost' storage
    // of the selection, so make sure we copy this
    m_selStart = from;
    m_selEnd = to;
}

bool wxNSTextFieldControl::PositionToXY(long pos, long *x, long *y) const
{
    wxCHECK_MSG( pos >= 0, false, wxS("Invalid character position") );

    if ( pos > [[m_textField stringValue] length] )
        return false;

    if ( y )
        *y = 0;

    if ( x )
        *x = pos;

    return true;
}

long wxNSTextFieldControl::XYToPosition(long x, long y) const
{
    wxCHECK_MSG( x >= 0 && y >= 0, -1, wxS("Invalid line/column number") );

    // Last valid position is after the last character.
    if ( y != 0 || x > [[m_textField stringValue] length] )
        return -1;

    return x;
}

void wxNSTextFieldControl::ShowPosition(long pos)
{
    wxCHECK_RET( pos >= 0, wxS("Invalid character position") );
    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        [editor scrollRangeToVisible:NSMakeRange(pos, 1)];
    }
}

void wxNSTextFieldControl::WriteText(const wxString& str)
{
    NSEvent* formerEvent = m_lastKeyDownEvent;
    m_lastKeyDownEvent = nil;
    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        wxMacEditHelper helper(m_textField);
        BOOL hasUndo = [editor respondsToSelector:@selector(setAllowsUndo:)];
        if ( hasUndo )
            [(NSTextView*)editor setAllowsUndo:NO];
        [editor insertText:wxCFStringRef( str , m_wxPeer->GetFont().GetEncoding() ).AsNSString()];
        if ( hasUndo )
            [(NSTextView*)editor setAllowsUndo:YES];
    }
    else
    {
        wxString val = GetStringValue() ;
        long start , end ;
        GetSelection( &start , &end ) ;
        val.Remove( start , end - start ) ;
        val.insert( start , str ) ;
        SetStringValue( val ) ;
        SetSelection( start + str.length() , start + str.length() ) ;
    }
    m_lastKeyDownEvent = formerEvent;
}

void wxNSTextFieldControl::controlAction(WXWidget WXUNUSED(slf),
    void* WXUNUSED(_cmd), void *WXUNUSED(sender))
{
    wxWindow* wxpeer = (wxWindow*) GetWXPeer();
    if ( wxpeer && (wxpeer->GetWindowStyle() & wxTE_PROCESS_ENTER) )
    {
        wxCommandEvent event(wxEVT_TEXT_ENTER, wxpeer->GetId());
        event.SetEventObject( wxpeer );
        event.SetString( GetTextEntry()->GetValue() );
        wxpeer->HandleWindowEvent( event );
    }
}

void wxNSTextFieldControl::SetInternalSelection( long from , long to )
{
    m_selStart = from;
    m_selEnd = to;
}

void wxNSTextFieldControl::UpdateInternalSelectionFromEditor( wxNSTextFieldEditor* fieldEditor )
{
    if ( fieldEditor )
    {
        NSRange range = [fieldEditor selectedRange];
        SetInternalSelection(range.location, range.location + range.length);
    }
}

// as becoming first responder on a window - triggers a resign on the same control, we have to avoid
// the resign notification writing back native selection values before we can set our own

static WXWidget s_widgetBecomingFirstResponder = nil;

bool wxNSTextFieldControl::becomeFirstResponder(WXWidget slf, void *_cmd)
{
    s_widgetBecomingFirstResponder = slf;
    bool retval = wxWidgetCocoaImpl::becomeFirstResponder(slf, _cmd);
    s_widgetBecomingFirstResponder = nil;
    return retval;
}

bool wxNSTextFieldControl::resignFirstResponder(WXWidget slf, void *_cmd)
{
    if ( slf != s_widgetBecomingFirstResponder )
    {
        NSText* editor = [m_textField currentEditor];
        if ( editor )
        {
            NSRange range = [editor selectedRange];
            m_selStart = range.location;
            m_selEnd = range.location + range.length;
        }
    }
    return wxWidgetCocoaImpl::resignFirstResponder(slf, _cmd);
}

bool wxNSTextFieldControl::SetHint(const wxString& hint)
{
    wxCFStringRef hintstring(hint);
    [[m_textField cell] setPlaceholderString:hintstring.AsNSString()];
    return true;
}

void wxNSTextFieldControl::SetJustification()
{
    if ( !m_textField )
        return;

    NSTextAlignment align;

    if ( m_wxPeer->HasFlag(wxTE_RIGHT) )
        align = NSRightTextAlignment;
    else if ( m_wxPeer->HasFlag(wxTE_CENTRE) )
        align = NSCenterTextAlignment;
    else // wxTE_LEFT == 0
        align = NSLeftTextAlignment;

    [m_textField setAlignment:align];
}

//
//
//
wxWidgetImplType* wxWidgetImpl::CreateTextControl( wxTextCtrl* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    const wxString& WXUNUSED(str),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxWidgetCocoaImpl* c = NULL;

    if ( style & wxTE_MULTILINE )
    {
        wxNSTextScrollView* v = nil;
        v = [[wxNSTextScrollView alloc] initWithFrame:r];
        c = new wxNSTextViewControl( wxpeer, v, style );
        c->SetNeedsFocusRect( true );
    }
    else
    {
        NSTextField* v = nil;
        if ( style & wxTE_PASSWORD )
            v = [[wxNSSecureTextField alloc] initWithFrame:r];
        else
            v = [[wxNSTextField alloc] initWithFrame:r];

        if ( style & wxTE_RIGHT)
        {
            [v setAlignment:NSRightTextAlignment];
        }
        else if ( style & wxTE_CENTRE)
        {
            [v setAlignment:NSCenterTextAlignment];
        }

        NSTextFieldCell* cell = [v cell];
        [cell setWraps:NO];
        [cell setScrollable:YES];

        c = new wxNSTextFieldControl( wxpeer, wxpeer, v );

        if ( (style & wxNO_BORDER) || (style & wxSIMPLE_BORDER) )
        {
            // under 10.7 the textcontrol can draw its own focus
            // even if no border is shown, on previous systems
            // we have to emulate this
            [v setBezeled:NO];
            [v setBordered:NO];
        }
        else
        {
            // use native border
            c->SetNeedsFrame(false);
        }
    }

    return c;
}


#endif // wxUSE_TEXTCTRL
