/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:     wxRadioButton
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/radiobut.h"
#include "wx/brush.h"
#endif

#include "wx/os2/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)

void wxRadioButton::Init()
{
    m_bFocusJustSet = FALSE;
} // end of wxRadioButton::Init

void wxRadioButton::Command (
  wxCommandEvent&                   rEvent
)
{
    SetValue ((rEvent.GetInt() != 0) );
    ProcessCommand (rEvent);
} // end of wxRadioButton::Command

bool wxRadioButton::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsLabel
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
#if wxUSE_VALIDATORS
, const wxValidator&                rValidator
#endif
, const wxString&                   rsName
)
{
    if ( !CreateControl( pParent
                        ,vId
                        ,rPos
                        ,rSize
                        ,lStyle
#if wxUSE_VALIDATORS
                        ,rValidator
#endif
                        ,rsName))
        return FALSE;

    long                            lSstyle = HasFlag(wxRB_GROUP) ? WS_GROUP : 0;

    lSstyle |= BS_AUTORADIOBUTTON;

    if (HasFlag(wxCLIP_SIBLINGS))
        lSstyle |= WS_CLIPSIBLINGS;

    if (!OS2CreateControl( _T("BUTTON")
                          ,lSstyle
                          ,rPos
                          ,rSize
                          ,rsLabel
                          ,0
                         ))
        return FALSE;

    if (HasFlag(wxRB_GROUP))
        SetValue(TRUE);

    SetFont(*wxSMALL_FONT);
    SetSize( rPos.x
            ,rPos.y
            ,rSize.x
            ,rSize.y
           );
    return TRUE;
} // end of wxRadioButton::Create

wxSize wxRadioButton::DoGetBestSize() const
{
    static int                      snRadioSize = 0;

    if (!snRadioSize)
    {
        wxScreenDC                  vDC;

        vDC.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        snRadioSize = vDC.GetCharHeight();
    }

    wxString                        sStr = GetLabel();
    int                             nRadioWidth;
    int                             nRadioHeight;

    if (!sStr.empty())
    {
        GetTextExtent( sStr
                      ,&nRadioWidth
                      ,&nRadioHeight
                     );
        nRadioWidth += snRadioSize + GetCharWidth();
        if (nRadioHeight < snRadioSize)
            nRadioHeight = snRadioSize;
    }
    else
    {
        nRadioWidth  = snRadioSize;
        nRadioHeight = snRadioSize;
    }
    return wxSize( nRadioWidth
                  ,nRadioHeight
                 );
} // end of wxRadioButton::DoGetBestSize

//
// Get single selection, for single choice list items
//
bool wxRadioButton::GetValue() const
{
    return((::WinSendMsg((HWND) GetHWND(), BM_QUERYCHECK, (MPARAM)0L, (MPARAM)0L) != 0));
} // end of wxRadioButton::GetValue

bool wxRadioButton::OS2Command(
  WXUINT                            wParam
, WXWORD                            wId
)
{
    if (wParam == BN_CLICKED)
    {
        wxCommandEvent              rEvent( wxEVT_COMMAND_RADIOBUTTON_SELECTED
                                           ,m_windowId
                                          );

        rEvent.SetEventObject(this);
        ProcessCommand(rEvent);
        return TRUE;
    }
    else
        return FALSE;
} // end of wxRadioButton::OS2Command

void wxRadioButton::SetFocus()
{
    // when the radio button receives a WM_SETFOCUS message it generates a
    // BN_CLICKED which is totally unexpected and leads to catastrophic results
    // if you pop up a dialog from the radio button event handler as, when the
    // dialog is dismissed, the focus is returned to the radio button which
    // generates BN_CLICKED which leads to showing another dialog and so on
    // without end!
    //
    // to aviod this, we drop the pseudo BN_CLICKED events generated when the
    // button gains focus
    m_bFocusJustSet = TRUE;

    wxControl::SetFocus();
}

void wxRadioButton::SetLabel(
  const wxString&                   rsLabel
)
{
    ::WinSetWindowText((HWND)GetHWND(), (const char *)rsLabel.c_str());
} // end of wxRadioButton::SetLabel

void wxRadioButton::SetValue(
  bool                              bValue
)
{
    ::WinSendMsg((HWND)GetHWND(), BM_SETCHECK, (MPARAM)bValue, (MPARAM)0);
} // end of wxRadioButton::SetValue

