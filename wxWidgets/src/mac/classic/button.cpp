/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/button.cpp
// Purpose:     wxButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/button.h"

#ifndef WX_PRECOMP
    #include "wx/panel.h"
#endif

#include "wx/stockitem.h"

IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)

#include "wx/mac/uma.h"
// Button

static const int kMacOSXHorizontalBorder = 2 ;
static const int kMacOSXVerticalBorder = 4 ;

bool wxButton::Create(wxWindow *parent, wxWindowID id, const wxString& lbl,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    wxString label(lbl);
    if (label.empty() && wxIsStockID(id))
        label = wxGetStockLabel(id);

    if ( !wxButtonBase::Create(parent, id, pos, size, style, validator, name) )
        return false;

    Rect bounds ;
    Str255 title ;

    if ( UMAHasAquaLayout() )
    {
        m_macHorizontalBorder = kMacOSXHorizontalBorder;
        m_macVerticalBorder = kMacOSXVerticalBorder;
    }

    MacPreControlCreate( parent , id ,  label , pos , size ,style, validator , name , &bounds , title ) ;

    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , 1,
          kControlPushButtonProc , (long) this ) ;
    wxASSERT_MSG( (ControlHandle) m_macControl != NULL , wxT("No valid mac control") ) ;

    MacPostControlCreate() ;

    return true;
}

void wxButton::SetDefault()
{
    wxTopLevelWindow *tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
    wxButton *btnOldDefault = NULL;
    if ( tlw )
    {
        btnOldDefault = wxDynamicCast(tlw->GetDefaultItem(), wxButton);
        tlw->SetDefaultItem(this);
    }

    Boolean inData;
    if ( btnOldDefault && btnOldDefault->m_macControl )
    {
          inData = 0;
        ::SetControlData( (ControlHandle) btnOldDefault->m_macControl , kControlButtonPart ,
                           kControlPushButtonDefaultTag , sizeof( Boolean ) , (char*)(&inData) ) ;
    }
    if ( (ControlHandle) m_macControl )
    {
          inData = 1;
        ::SetControlData(  (ControlHandle) m_macControl , kControlButtonPart ,
                           kControlPushButtonDefaultTag , sizeof( Boolean ) , (char*)(&inData) ) ;
    }
}

wxSize wxButton::DoGetBestSize() const
{
    wxSize sz = GetDefaultSize() ;

    int wBtn = m_label.length() * 8 + 12 + 2 * kMacOSXHorizontalBorder ;

    if (wBtn > sz.x) sz.x = wBtn;

    return sz ;
}

wxSize wxButton::GetDefaultSize()
{
    int wBtn = 70 ;
    int hBtn = 20 ;

    if ( UMAHasAquaLayout() )
    {
        wBtn += 2 * kMacOSXHorizontalBorder ;
        hBtn += 2 * kMacOSXVerticalBorder ;
    }

    return wxSize(wBtn, hBtn);
}

void wxButton::Command (wxCommandEvent & event)
{
    if ( (ControlHandle) m_macControl )
    {
        HiliteControl(  (ControlHandle) m_macControl , kControlButtonPart ) ;
        unsigned long finalTicks ;
        Delay( 8 , &finalTicks ) ;
        HiliteControl(  (ControlHandle) m_macControl , 0 ) ;
    }
    ProcessCommand (event);
}

void wxButton::MacHandleControlClick( WXWidget WXUNUSED(control) , wxInt16 controlpart , bool WXUNUSED(mouseStillDown) )
{
    if ( controlpart != kControlNoPart )
    {
        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, m_windowId );
        event.SetEventObject(this);
        ProcessCommand(event);
    }
}
