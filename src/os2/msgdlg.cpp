/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.cpp
// Purpose:     wxMessageDialog
// Author:      David Webster
// Modified by:
// Created:     10/10/99
// RCS-ID:      $$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "msgdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/app.h"
#include "wx/msgdlg.h"
#endif

#include "wx/os2/private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

wxMessageDialog::wxMessageDialog(
  wxWindow*                         pParent
, const wxString&                   rsMessage
, const wxString&                   rsCaption
, long                              lStyle
, const wxPoint&                    pPos
)
{
    m_sCaption     = rsCaption;
    m_sMessage     = rsMessage;
    m_lDialogStyle = lStyle;
    m_pParent      = NULL; // pParent;
} // end of wxMessageDialog::wxMessageDialog

int wxMessageDialog::ShowModal()
{
    HWND                            hWnd = 0;
    ULONG                           ulStyle = MB_OK;
    int                             nAns = wxOK;

    if (!wxTheApp->GetTopWindow())
    {
        //
        // when the message box is shown from wxApp::OnInit() (i.e. before the
        // message loop is entered), this must be done or the next message box
        // will never be shown - just try putting 2 calls to wxMessageBox() in
        // OnInit() to see it
        //
        while (wxTheApp->Pending())
            wxTheApp->Dispatch();
    }

    if (m_pParent)
        hWnd = (HWND) m_pParent->GetHWND();
    else
        hWnd = HWND_DESKTOP;
    if (m_lDialogStyle & wxYES_NO)
    {
        if (m_lDialogStyle & wxCANCEL)
            ulStyle = MB_YESNOCANCEL;
        else
            ulStyle = MB_YESNO;

        if (m_lDialogStyle & wxNO_DEFAULT)
            ulStyle |= MB_DEFBUTTON2;
    }

    if (m_lDialogStyle & wxOK)
    {
        if (m_lDialogStyle & wxCANCEL)
            ulStyle = MB_OKCANCEL;
        else
            ulStyle = MB_OK;
    }
    if (m_lDialogStyle & wxICON_EXCLAMATION)
        ulStyle |= MB_ICONEXCLAMATION;
    else if (m_lDialogStyle & wxICON_HAND)
        ulStyle |= MB_ICONHAND;
    else if (m_lDialogStyle & wxICON_INFORMATION)
        ulStyle |= MB_ICONEXCLAMATION;
    else if (m_lDialogStyle & wxICON_QUESTION)
        ulStyle |= MB_ICONQUESTION;

    if (hWnd != HWND_DESKTOP)
        ulStyle |= MB_APPLMODAL;
    else
        ulStyle |= MB_SYSTEMMODAL;

    //
    // This little line of code is get message boxes under OS/2 to
    // behve like the other ports.  In OS/2 if the parent is a window
    // it displays, clipped, in the window.  This centers it on the
    // desktop, like the other ports but still allows control over modality
    //
    hWnd = HWND_DESKTOP;

    ULONG                           ulAns = ::WinMessageBox( hWnd
                                                            ,hWnd
                                                            ,(PSZ)m_sMessage.c_str()
                                                            ,(PSZ)m_sCaption.c_str()
                                                            ,0L
                                                            ,ulStyle);
    switch (ulAns)
    {
        case MBID_CANCEL:
            nAns = wxID_CANCEL;
            break;
        case MBID_OK:
            nAns = wxID_OK;
            break;
        case MBID_YES:
            nAns = wxID_YES;
            break;
        case MBID_NO:
            nAns = wxID_NO;
            break;
        default:
           nAns = wxID_CANCEL;
    }
    return nAns;
} // end of wxMessageDialog::ShowModal

