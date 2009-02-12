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

/*
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

wxNSTextFieldControl::wxNSTextFieldControl( wxTextCtrl *wxPeer, WXWidget w ) : wxWidgetCocoaImpl(wxPeer, w)
{
    m_textField = (NSTextField*) w;
    [m_textField setDelegate: w];
}

wxNSTextFieldControl::~wxNSTextFieldControl()
{
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
    
    if ( style & wxTE_PASSWORD )
        v =[[wxNSSecureTextField alloc] initWithFrame:r];    
    else
        v= [[wxNSTextField alloc] initWithFrame:r];
    
    if ( style & wxNO_BORDER )
    {
        [v setBezeled:NO];
        [v setBordered:NO];
    }

    [v setBezeled:NO];
    [v setBordered:NO];
    //[v setBezeled:NO];
    //[v setEditable:NO];
    //[v setDrawsBackground:NO];
    
    wxWidgetCocoaImpl* c = new wxNSTextFieldControl( wxpeer, v );
    return c;
}


#endif // wxUSE_TEXTCTRL
