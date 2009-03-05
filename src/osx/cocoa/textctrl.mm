/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/textctrl.mm
// Purpose:     wxTextCtrl
// Author:      Stefan Csomor
// Modified by: Ryan Norton (MLTE GetLineLength and GetLineText)
// Created:     1998-01-01
// RCS-ID:      $Id: textctrl.cpp 54820 2008-07-29 20:04:11Z SC $
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

#include "wx/osx/private.h"
#include "wx/osx/cocoa/private/textimpl.h"

@interface wxNSSecureTextField : NSSecureTextField
{
    wxWidgetCocoaImpl* impl;
}

- (void) setImplementation:(wxWidgetCocoaImpl*) item;
- (wxWidgetCocoaImpl*) implementation;
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

- (wxWidgetCocoaImpl*) implementation
{
    return impl;
}

- (void) setImplementation:(wxWidgetCocoaImpl*) item
{
    impl = item;
}

- (void)controlTextDidChange:(NSNotification *)aNotification
{
    if ( impl )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer ) {
            wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            event.SetString( static_cast<wxTextCtrl*>(wxpeer)->GetValue() );
            wxpeer->HandleWindowEvent( event );
        }
    }
}

@end

@interface wxNSTextView : NSScrollView
{
    wxWidgetCocoaImpl* impl;
}

- (void) setImplementation:(wxWidgetCocoaImpl*) item;
- (wxWidgetCocoaImpl*) implementation;
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

- (wxWidgetCocoaImpl*) implementation
{
    return impl;
}

- (void) setImplementation:(wxWidgetCocoaImpl*) item
{
    impl = item;
}


- (void)controlTextDidChange:(NSNotification *)aNotification
{
    if ( impl )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer ) {
            wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            event.SetString( static_cast<wxTextCtrl*>(wxpeer)->GetValue() );
            wxpeer->HandleWindowEvent( event );
        }
    }
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

- (wxWidgetCocoaImpl*) implementation
{
    return impl;
}

- (void) setImplementation:(wxWidgetCocoaImpl*) item
{
    impl = item;
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
    if ( impl )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer ) {
            wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            event.SetString( static_cast<wxTextCtrl*>(wxpeer)->GetValue() );
            wxpeer->HandleWindowEvent( event );
        }
    }
}

typedef BOOL (*wxOSX_insertNewlineHandlerPtr)(NSView* self, SEL _cmd, NSControl *control, NSTextView* textView, SEL commandSelector);

- (BOOL)control:(NSControl*)control textView:(NSTextView*)textView doCommandBySelector:(SEL)commandSelector
{
    if (commandSelector == @selector(insertNewline:))
    {
        if ( impl  )
        {
            wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
            if ( wxpeer && wxpeer->GetWindowStyle() & wxTE_PROCESS_ENTER ) 
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, wxpeer->GetId());
                event.SetEventObject( wxpeer );
                event.SetString( static_cast<wxTextCtrl*>(wxpeer)->GetValue() );
                wxpeer->HandleWindowEvent( event );
            }
        }
    }
    
    return NO;
}
/*
- (void)controlTextDidEndEditing:(NSNotification *)aNotification
{
    if ( impl )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer ) {
            wxFocusEvent event(wxEVT_KILL_FOCUS, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            event.SetWindow( wxpeer );
            wxpeer->HandleWindowEvent( event );
        }
    }
}
*/
@end

// wxNSTextViewControl

wxNSTextViewControl::wxNSTextViewControl( wxTextCtrl *wxPeer, WXWidget w ) : wxWidgetCocoaImpl(wxPeer, w)
{
    m_scrollView = (NSScrollView*) w;
    [(wxNSTextField*)w setImplementation: this];
    
    [m_scrollView setHasVerticalScroller:YES];
    [m_scrollView setHasHorizontalScroller:NO];
    [m_scrollView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    NSSize contentSize = [m_scrollView contentSize];
    
    m_textView = [[NSTextView alloc] initWithFrame: NSMakeRect(0, 0,
            contentSize.width, contentSize.height)];
    [m_textView setVerticallyResizable:YES];
    [m_textView setHorizontallyResizable:NO];
    [m_textView setAutoresizingMask:NSViewWidthSizable];
    
    [m_scrollView setDocumentView: m_textView];

    [m_textView setDelegate: w];
}

wxNSTextViewControl::~wxNSTextViewControl()
{
    if (m_textView)
        [m_textView setDelegate: nil];
}

wxString wxNSTextViewControl::GetStringValue() const 
{
    if (m_textView) 
    {
        wxCFStringRef cf( (CFStringRef) [[m_textView string] retain] );
        return cf.AsString(m_wxPeer->GetFont().GetEncoding());
    }
    return wxEmptyString;
}
void wxNSTextViewControl::SetStringValue( const wxString &str) 
{
    if (m_textView)
        [m_textView setString: wxCFStringRef( str , m_wxPeer->GetFont().GetEncoding() ).AsNSString()];
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
    NSRange selrange = NSMakeRange(from, to-from);
    [m_textView setSelectedRange:selrange];
    [m_textView scrollRangeToVisible:selrange];
}

void wxNSTextViewControl::WriteText(const wxString& str) 
{
    // temp hack to get logging working early
    wxString former = GetStringValue();
    SetStringValue( former + str );
    SetSelection(GetStringValue().length(), GetStringValue().length());
}

// wxNSTextFieldControl

wxNSTextFieldControl::wxNSTextFieldControl( wxTextCtrl *wxPeer, WXWidget w ) : wxWidgetCocoaImpl(wxPeer, w)
{
    m_textField = (NSTextField*) w;
    [m_textField setDelegate: w];
}

wxNSTextFieldControl::~wxNSTextFieldControl()
{
    if (m_textField)
        [m_textField setDelegate: nil];
}

wxString wxNSTextFieldControl::GetStringValue() const 
{
    wxCFStringRef cf( (CFStringRef) [[m_textField stringValue] retain] );
    return cf.AsString(m_wxPeer->GetFont().GetEncoding());
}
void wxNSTextFieldControl::SetStringValue( const wxString &str) 
{
    [m_textField setStringValue: wxCFStringRef( str , m_wxPeer->GetFont().GetEncoding() ).AsNSString()];
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

void wxNSTextFieldControl::GetSelection( long* from, long* to) const 
{
    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        NSRange range = [editor selectedRange];
        *from = range.location;
        *to = range.location + range.length;
    }
}

void wxNSTextFieldControl::SetSelection( long from , long to )
{
    NSText* editor = [m_textField currentEditor];
    if ( editor )
    {
        [editor setSelectedRange:NSMakeRange(from, to-from)];
    }
}

void wxNSTextFieldControl::WriteText(const wxString& str) 
{
    // temp hack to get logging working early
    wxString former = GetStringValue();
    SetStringValue( former + str );
    SetSelection(GetStringValue().length(), GetStringValue().length());
}

void wxNSTextFieldControl::controlAction(WXWidget slf, void* _cmd, void *sender)
{
    wxWindow* wxpeer = (wxWindow*) GetWXPeer();
    if ( wxpeer && (wxpeer->GetWindowStyle() & wxTE_PROCESS_ENTER) ) 
    {
        wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, wxpeer->GetId());
        event.SetEventObject( wxpeer );
        event.SetString( static_cast<wxTextCtrl*>(wxpeer)->GetValue() );
        wxpeer->HandleWindowEvent( event );
    }
}

//
//
//

wxWidgetImplType* wxWidgetImpl::CreateTextControl( wxTextCtrl* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& str,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    NSTextField* v = nil;
    wxWidgetCocoaImpl* c = NULL;
    
    if ( style & wxTE_MULTILINE || style & wxTE_RICH || style & wxTE_RICH2 )
    {
        v = [[wxNSTextView alloc] initWithFrame:r];
        c = new wxNSTextViewControl( wxpeer, v );
        static_cast<wxNSTextViewControl*>(c)->SetStringValue(str);
    }
    else 
    {
        if ( style & wxTE_PASSWORD )
            v = [[wxNSSecureTextField alloc] initWithFrame:r];
        else
            v = [[wxNSTextField alloc] initWithFrame:r];
        
        if ( style & wxNO_BORDER )
        {
            // FIXME: How can we remove the native control's border?
            // setBordered is separate from the text ctrl's border.
        }
        
        [v setBezeled:NO];
        [v setBordered:NO];
        
        c = new wxNSTextFieldControl( wxpeer, v );
        [v setImplementation: c];
        static_cast<wxNSTextFieldControl*>(c)->SetStringValue(str);
    }
    
    return c;
}


#endif // wxUSE_TEXTCTRL
