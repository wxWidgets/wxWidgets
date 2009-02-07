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

- (id)initWithFrame:(NSRect)frame
{
    [super initWithFrame:frame];
    [self setDelegate: self];
    [self setTarget: self];
//    [self setAction: @selector(enterAction:)];
    return self;
}

// use our common calls
- (void) setTitle:(NSString *) title
{
    [self setStringValue: title];
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

- (void) enterAction: (id) sender
{
    if ( impl )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer && (wxpeer->GetWindowStyle() & wxTE_PROCESS_ENTER) ) {
            wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, wxpeer->GetId());
            event.SetEventObject( wxpeer );
            event.SetString( static_cast<wxTextCtrl*>(wxpeer)->GetValue() );
            wxpeer->HandleWindowEvent( event );
        }
    }
}
*/
@end

wxNSTextFieldControl::wxNSTextFieldControl( wxTextCtrl *wxPeer, WXWidget w ) : wxWidgetCocoaImpl(wxPeer, w)
{
}

wxNSTextFieldControl::~wxNSTextFieldControl()
{
}

wxString wxNSTextFieldControl::GetStringValue() const 
{
    wxCFStringRef cf( (CFStringRef) [[(wxNSTextField*) m_osxView stringValue] retain] );
    return cf.AsString(m_wxPeer->GetFont().GetEncoding());
}
void wxNSTextFieldControl::SetStringValue( const wxString &str) 
{
    [(wxNSTextField*) m_osxView setStringValue: wxCFStringRef( str , m_wxPeer->GetFont().GetEncoding() ).AsNSString()];
}
void wxNSTextFieldControl::Copy() 
{
}

void wxNSTextFieldControl::Cut() 
{
}

void wxNSTextFieldControl::Paste() 
{
}

bool wxNSTextFieldControl::CanPaste() const 
{ 
    return false;
}

void wxNSTextFieldControl::SetEditable(bool editable) 
{
    [(wxNSTextField*) m_osxView setEditable:editable];
}

void wxNSTextFieldControl::GetSelection( long* from, long* to) const 
{
}

void wxNSTextFieldControl::SetSelection( long from , long to )
{
}

void wxNSTextFieldControl::WriteText(const wxString& str) 
{
    // temp hack to get logging working early
    wxString former = GetStringValue();
    SetStringValue( former + str );
}

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
    wxNSTextField* v = [[wxNSTextField alloc] initWithFrame:r];
    
    if ( style & wxNO_BORDER )
    {
        [v setBezeled:NO];
        [v setBordered:NO];
    }

    //[v setBezeled:NO];
    //[v setEditable:NO];
    //[v setDrawsBackground:NO];
    
    wxWidgetCocoaImpl* c = new wxNSTextFieldControl( wxpeer, v );
    return c;
}


#endif // wxUSE_TEXTCTRL
