/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.cpp
// Purpose:     wxMessageDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "msgdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/msw/private.h"

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

wxMessageDialog::wxMessageDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& caption,
                                 long style,
                                 const wxPoint& WXUNUSED(pos))
{
    m_caption = caption;
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
}

int wxMessageDialog::ShowModal()
{
    if ( !wxTheApp->GetTopWindow() )
    {
        // when the message box is shown from wxApp::OnInit() (i.e. before the
        // message loop is entered), this must be done or the next message box
        // will never be shown - just try putting 2 calls to wxMessageBox() in
        // OnInit() to see it
        while ( wxTheApp->Pending() )
            wxTheApp->Dispatch();
    }

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
        msStyle |= MB_ICONINFORMATION;
    else if (m_dialogStyle & wxICON_QUESTION)
        msStyle |= MB_ICONQUESTION;

    if (hWnd)
        msStyle |= MB_APPLMODAL;
    else
        msStyle |= MB_TASKMODAL;

    int msAns = MessageBox(hWnd, (LPCTSTR)m_message.c_str(),
                           (LPCTSTR)m_caption.c_str(), msStyle);
    int ans = wxOK;
    switch (msAns)
    {
        case IDCANCEL:
            ans = wxID_CANCEL;
            break;
        case IDOK:
            ans = wxID_OK;
            break;
        case IDYES:
            ans = wxID_YES;
            break;
        case IDNO:
            ans = wxID_NO;
            break;
    }
    return ans;
}

