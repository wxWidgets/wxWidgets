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
    int                             nX          = rPos.x;
    int                             nY          = rPos.y;
    int                             nWidth      = rSize.x;
    int                             nHeight     = rSize.y;
    long                            lsStyle     = 0L;
    long                            lGroupStyle = 0L;

    SetName(rsName);
#if wxUSE_VALIDATORS
    SetValidator(rValidator);
#endif

    if (pParent)
        pParent->AddChild(this);

    SetBackgroundColour(pParent->GetBackgroundColour());
    SetForegroundColour(pParent->GetForegroundColour());

    if (vId == -1)
        m_windowId = (int)NewControlId();
    else
        m_windowId = vId;


    m_windowStyle = lStyle ;

    if (m_windowStyle & wxRB_GROUP)
        lGroupStyle = WS_GROUP;

    lsStyle = lGroupStyle | BS_AUTORADIOBUTTON | WS_VISIBLE ;

    if (m_windowStyle & wxCLIP_SIBLINGS )
        lsStyle |= WS_CLIPSIBLINGS;
    m_hWnd = (WXHWND)::WinCreateWindow ( GetHwndOf(pParent)
                                        ,WC_BUTTON
                                        ,rsLabel.c_str()
                                        ,lsStyle
                                        ,0, 0, 0, 0
                                        ,GetWinHwnd(pParent)
                                        ,HWND_TOP
                                        ,(HMENU)m_windowId
                                        ,NULL
                                        ,NULL
                                       );
    wxCHECK_MSG(m_hWnd, FALSE, wxT("Failed to create radiobutton"));

    if (rsLabel != wxT(""))
    {
        int                         nLabelWidth;
        int                         nLabelHeight;

        GetTextExtent( rsLabel
                      ,&nLabelWidth
                      ,&nLabelHeight
                      ,NULL
                      ,NULL
                      ,&this->GetFont()
                     );
        if (nWidth < 0)
            nWidth = (int)(nLabelWidth + RADIO_SIZE);
        if (nHeight<0)
        {
            nHeight = (int)(nLabelHeight);
            if (nHeight < RADIO_SIZE)
                nHeight = RADIO_SIZE;
        }
    }
    else
    {
        if (nWidth < 0)
            nWidth = RADIO_SIZE;
        if (nHeight < 0)
            nHeight = RADIO_SIZE;
    }

    //
    // Subclass again for purposes of dialog editing mode
    //
    SubclassWin((WXHWND)m_hWnd);
    SetFont(pParent->GetFont());
    SetSize( nX
            ,nY
            ,nWidth
            ,nHeight
           );
    return FALSE;
} // end of wxRadioButton::Create

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

