///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/progdlg.cpp
// Purpose:     wxProgressDialog implementation
// Author:      Wlodzimierz ABX Skiba
// Modified by:
// Created:     29.12.2004
// RCS-ID:      $Id$
// Copyright:   (c) Wlodzimierz Skiba
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "progdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include  "wx/string.h"
#endif //WX_PRECOMP

#if wxUSE_PROGRESSDLG

#include "wx/progdlg.h"
#include "wx/msgdlg.h"

IMPLEMENT_CLASS(wxProgressDialog, wxDialog)

static Boolean wxProgressCallback(PrgCallbackData *data)
{
    if(!data)
        return false;

    wxProgressDialog *dialog = (wxProgressDialog *)data->userDataP;

    if(!dialog)
        return false;

    return dialog->Callback(data);
}

wxProgressDialog::wxProgressDialog(const wxString &title,
                                   wxString const &message,
                                   int maximum,
                                   wxWindow *parent,
                                   int style)
                 :wxDialog(parent, wxID_ANY, title),
                  m_prgFrame(NULL),
                  m_msg(message),
                  m_cur(0),
                  m_max(maximum),
                  m_canSkip((style & wxPD_CAN_SKIP )==wxPD_CAN_SKIP)
{
    wxString prgTitle = title.Mid(0, progressMaxTitle);

    m_prgFrame = PrgStartDialog(prgTitle.ToAscii(), wxProgressCallback, this);
}

wxProgressDialog::~wxProgressDialog()
{
    if(m_prgFrame)
    {
        PrgStopDialog(m_prgFrame, false);
        m_prgFrame = NULL;
    }
}

bool wxProgressDialog::Update(int value, const wxString& newmsg, bool *skip)
{
    if(!m_prgFrame)
        return false;
    if(!newmsg.empty())
        m_msg = newmsg;
    m_cur = value;

    EventType event;

    do
    {
        EvtGetEvent(&event, 0);
        Boolean handled = PrgHandleEvent(m_prgFrame, &event);
        if (!PrgHandleEvent(m_prgFrame, &event))
            if( PrgUserCancel(m_prgFrame) )
                return false;
    }
    while(event.eType != sysEventNilEvent);

    PrgUpdateDialog(m_prgFrame, 0, 0, "", true);

    m_activeSkip = m_canSkip && true;

    return true;
}

void wxProgressDialog::Resume()
{
}

bool wxProgressDialog::Show(bool show)
{
    return false;
}

Boolean wxProgressDialog::Callback(PrgCallbackData *data)
{
    strncpy( data->textP, m_msg.ToAscii() , data->textLen - 1 );
    data->textChanged = true;
    data->displaySkipBtn = m_canSkip;
    data->barMaxValue = (uint32_t)m_max;
    data->barCurValue = (uint32_t)m_cur;
    data->delay = (m_max == m_cur);

    return true;
}

#endif // wxUSE_PROGRESSDLG
