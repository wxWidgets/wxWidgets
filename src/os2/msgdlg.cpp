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
#include "wx/msgdlg.h"
#endif

#include "wx/os2/private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

wxMessageDialog::wxMessageDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        long style, const wxPoint& pos)
{
    m_caption = caption;
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
}

int wxMessageDialog::ShowModal()
{
    HWND hWnd = 0;
    if (m_parent) hWnd = (HWND) m_parent->GetHWND();
    unsigned int msStyle = MB_OK;
    if (m_dialogStyle & wxYES_NO)
    {
        if (m_dialogStyle & wxCANCEL)
            msStyle = MB_YESNOCANCEL;
        else
            msStyle = MB_YESNO;

        if (m_dialogStyle & wxNO_DEFAULT)
            msStyle |= MB_DEFBUTTON2;
    }

    if (m_dialogStyle & wxOK)
    {
        if (m_dialogStyle & wxCANCEL)
            msStyle = MB_OKCANCEL;
        else
            msStyle = MB_OK;
    }
    if (m_dialogStyle & wxICON_EXCLAMATION)
        msStyle |= MB_ICONEXCLAMATION;
    else if (m_dialogStyle & wxICON_HAND)
        msStyle |= MB_ICONHAND;
    else if (m_dialogStyle & wxICON_INFORMATION)
        msStyle |= MB_INFORMATION;
    else if (m_dialogStyle & wxICON_QUESTION)
        msStyle |= MB_ICONQUESTION;

    if (hWnd)
        msStyle |= MB_APPLMODAL;
    else
        msStyle |= MB_SYSTEMMODAL;

	int msAns = WinMessageBox(HWND_DESKTOP, hWnd, m_message.c_str(), m_caption.c_str(), 0, msStyle | MB_MOVEABLE);
    int ans = wxOK;
    switch (msAns)
    {
        case MBID_CANCEL:
            ans = wxID_CANCEL;
            break;
        case MBID_OK:
            ans = wxID_OK;
            break;
        case MBID_YES:
            ans = wxID_YES;
            break;
        case MBID_NO:
            ans = wxID_NO;
            break;
    }
    return ans;
}

