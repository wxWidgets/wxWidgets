/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/combobox.mm
// Purpose:     wxChoice
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"
#include "wx/evtloop.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/dcclient.h"
#endif

#include "wx/osx/private/available.h"
#include "wx/osx/cocoa/private/textimpl.h"

// work in progress

@interface wxNSTableDataSource : NSObject <NSComboBoxDataSource>
{
    wxNSComboBoxControl* impl;
}

- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox;
- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index;

@end


@implementation wxNSComboBox

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
    if ( impl && impl->ShouldSendEvents() )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer ) {
            wxCommandEvent event(wxEVT_TEXT, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            event.SetString( static_cast<wxComboBox*>(wxpeer)->GetValue() );
            wxpeer->HandleWindowEvent( event );
        }
    }
}

- (void)controlTextDidEndEditing:(NSNotification *) aNotification
{
    wxUnusedVar(aNotification);
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl )
    {
        wxNSTextFieldControl* timpl = dynamic_cast<wxNSTextFieldControl*>(impl);
        if ( timpl )
            timpl->UpdateInternalSelectionFromEditor(self.WXFieldEditor);
        impl->DoNotifyFocusLost();
    }
}

- (void)comboBoxWillPopUp:(NSNotification *)notification
{
    wxUnusedVar(notification);
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if( impl && impl->ShouldSendEvents() )
    {
        wxComboBox* wxpeer = static_cast<wxComboBox*>(impl->GetWXPeer());
        if( wxpeer )
        {
            wxCommandEvent event(wxEVT_COMBOBOX_DROPDOWN, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            wxpeer->GetEventHandler()->ProcessEvent( event );
        }
    }
}

- (void)comboBoxWillDismiss:(NSNotification *)notification
{
    wxUnusedVar(notification);
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if( impl && impl->ShouldSendEvents() )
    {
        wxComboBox* wxpeer = static_cast<wxComboBox*>(impl->GetWXPeer());
        if( wxpeer )
        {
            wxCommandEvent event(wxEVT_COMBOBOX_CLOSEUP, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            wxpeer->GetEventHandler()->ProcessEvent( event );
        }
    }
}

- (void)comboBoxSelectionDidChange:(NSNotification *)notification
{
    wxUnusedVar(notification);
    wxNSComboBoxControl* const
        impl = (wxNSComboBoxControl* ) wxWidgetImpl::FindFromWXWidget( self );
    if ( impl && impl->ShouldSendEvents())
    {
        wxComboBox* wxpeer = static_cast<wxComboBox*>(impl->GetWXPeer());
        if ( wxpeer ) {
            const int sel = wxpeer->GetSelection();
            const wxString& val = wxpeer->GetString(sel);

            // We need to manually set the new value because at this time it
            // still contains the old value, but we want GetValue() to return
            // the new one if it's called from an event handler invoked below.
            impl->SetStringValue(val);

            wxCommandEvent event(wxEVT_COMBOBOX, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            event.SetInt( sel );
            event.SetString( val );
            wxpeer->HandleWindowEvent( event );

            wxCommandEvent eventText(wxEVT_TEXT, wxpeer->GetId());
            eventText.SetEventObject( wxpeer );
            eventText.SetString( val );
            wxpeer->HandleWindowEvent( eventText );
        }
    }
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

@end

wxNSComboBoxControl::wxNSComboBoxControl( wxComboBox *wxPeer, WXWidget w )
    : wxNSTextFieldControl(wxPeer, wxPeer, w)
{
    m_comboBox = (NSComboBox*)w;
}

wxNSComboBoxControl::~wxNSComboBoxControl()
{
}

void wxNSComboBoxControl::mouseEvent(WX_NSEvent event, WXWidget slf, void *_cmd)
{
    // NSComboBox has its own event loop, which reacts very badly to our synthetic
    // events used to signal when a wxEvent is posted, so during that time we switch
    // the wxEventLoop::WakeUp implementation to a lower-level version
    
    bool reset = false;
    wxEventLoop* const loop = (wxEventLoop*) wxEventLoopBase::GetActive();

    if ( loop != nullptr && [event type] == NSLeftMouseDown )
    {
        reset = true;
        loop->OSXUseLowLevelWakeup(true);
    }
    
    wxOSX_EventHandlerPtr superimpl = (wxOSX_EventHandlerPtr) [[slf superclass] instanceMethodForSelector:(SEL)_cmd];
    superimpl(slf, (SEL)_cmd, event);
 
    if ( reset )
    {
        loop->OSXUseLowLevelWakeup(false);
    }
}

int wxNSComboBoxControl::GetSelectedItem() const
{
    return [m_comboBox indexOfSelectedItem];
}

void wxNSComboBoxControl::SetSelectedItem(int item)
{
    SendEvents(false);

    if ( item != wxNOT_FOUND )
    {
        wxASSERT_MSG( item >= 0 && item < [m_comboBox numberOfItems],
                      "Inavlid item index." );
        [m_comboBox selectItemAtIndex: item];
    }
    else // remove current selection (if we have any)
    {
        const int sel = GetSelectedItem();
        if ( sel != wxNOT_FOUND )
            [m_comboBox deselectItemAtIndex:sel];
    }

    SendEvents(true);
}

int wxNSComboBoxControl::GetNumberOfItems() const
{
    return [m_comboBox numberOfItems];
}

void wxNSComboBoxControl::InsertItem(int pos, const wxString& item)
{
    wxCFStringRef itemLabel( item );
    NSString* const cocoaStr = itemLabel.AsNSString();

    if ( m_wxPeer->HasFlag(wxCB_SORT) )
    {
        NSArray* const objectValues = m_comboBox.objectValues;

        pos = [objectValues indexOfObject: cocoaStr
                            inSortedRange: NSMakeRange(0, objectValues.count)
                            options: NSBinarySearchingInsertionIndex
                            usingComparator: ^(id obj1, id obj2)
                                {
                                    return [obj1 caseInsensitiveCompare: obj2];
                                }];
    }

    [m_comboBox insertItemWithObjectValue:cocoaStr atIndex:pos];
}

void wxNSComboBoxControl::RemoveItem(int pos)
{
    SendEvents(false);

    // Explicitly deselect item being removed
    int selIdx = [m_comboBox indexOfSelectedItem];
    if (selIdx!= -1 && selIdx == pos)
    {
        [m_comboBox deselectItemAtIndex:selIdx];
    }

    [m_comboBox removeItemAtIndex:pos];
    SendEvents(true);
}

void wxNSComboBoxControl::Clear()
{
    SendEvents(false);
    [m_comboBox removeAllItems];
    [m_comboBox setStringValue:@""];
    SendEvents(true);
}

wxString wxNSComboBoxControl::GetStringAtIndex(int pos) const
{
    return wxCFStringRef::AsString([m_comboBox itemObjectValueAtIndex:pos]);
}

int wxNSComboBoxControl::FindString(const wxString& text) const
{
    NSInteger nsresult = [m_comboBox indexOfItemWithObjectValue:wxCFStringRef( text ).AsNSString()];

    int result;
    if (nsresult == NSNotFound)
        result = wxNOT_FOUND;
    else
        result = (int) nsresult;
    return result;
}

void wxNSComboBoxControl::Popup()
{
    id ax = NSAccessibilityUnignoredDescendant(m_comboBox);
    [ax setAccessibilityExpanded: YES];
}

void wxNSComboBoxControl::Dismiss()
{
    id ax = NSAccessibilityUnignoredDescendant(m_comboBox);
    [ax setAccessibilityExpanded: NO];
}

void wxNSComboBoxControl::SetEditable(bool editable)
{
    [m_comboBox setEditable:editable];

    // When the combobox isn't editable, make sure it is still selectable so the text can be copied
    if ( !editable )
        [m_comboBox setSelectable:YES];
}

wxWidgetImplType* wxWidgetImpl::CreateComboBox( wxComboBox* wxpeer, 
                                    wxWindowMac* WXUNUSED(parent), 
                                    wxWindowID WXUNUSED(id), 
                                    wxMenu* WXUNUSED(menu),
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSComboBox* v = [[wxNSComboBox alloc] initWithFrame:r];
    if (WX_IS_MACOS_AVAILABLE(10, 13))
        [v setNumberOfVisibleItems:999];
    else
        [v setNumberOfVisibleItems:13];

    wxNSComboBoxControl* c = new wxNSComboBoxControl( wxpeer, v );

    if (style & wxCB_READONLY)
        c->SetEditable(false);

    return c;
}

wxSize wxComboBox::DoGetBestSize() const
{
    int lbWidth = GetCount() > 0 ? 20 : 100;  // some defaults
    wxSize baseSize = wxWindow::DoGetBestSize();
    int lbHeight = baseSize.y;
    int wLine;
    
    {
        wxInfoDC dc(const_cast<wxComboBox*>(this));
        
        // Find the widest line
        for(unsigned int i = 0; i < GetCount(); i++)
        {
            wxString str(GetString(i));
            
            wxCoord width, height ;
            dc.GetTextExtent( str , &width, &height);
            wLine = width ;
            
            lbWidth = wxMax( lbWidth, wLine ) ;
        }
        
        // Add room for the popup arrow
        lbWidth += 2 * lbHeight ;
    }
    
    return wxSize( lbWidth, lbHeight );
}

#endif // wxUSE_COMBOBOX
