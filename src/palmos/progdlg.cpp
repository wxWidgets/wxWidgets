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

//****************
//* NEEDS DEBUGING
//****************

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

//    /* uint16_t */  data->bitmapId = 0;
//    /* DmOpenRef */ data->bitmapDatabase = 0;
    /* char * */    data->textP = "test";
//    /* status_t */  data->error;
//    /* uint16_t */  data->canceled;
    /* uint16_t */  data->textChanged = false;
    /* uint16_t */  data->displaySkipBtn = true;
//    /* uint16_t */  data->skipped:1;
//    /* uint32_t */  data->timeout;
    /* uint32_t */  data->barMaxValue = (uint32_t)dialog->GetMaxValue();
    /* uint32_t */  data->barCurValue = (uint32_t)dialog->GetCurValue();
    /* uint16_t */  data->delay = false ;

    /* NOT USED
    data->spareBits1:10;
    data->padding1;
    data->padding2;
    data->barMessage;
    data->barFlags;
    data->spareBits2:15;
    */

    return true;
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
                  m_max(maximum)
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
        SetMessage(newmsg);
    m_cur = value;
    // PrgUpdateDialog crashes, needs debugging
    // PrgUpdateDialog(m_prgFrame, 0, 1, newmsg.ToAscii(), true);
    return true;
}

void wxProgressDialog::Resume()
{
}

bool wxProgressDialog::Show(bool show)
{
    return false;
}

#endif // wxUSE_PROGRESSDLG
