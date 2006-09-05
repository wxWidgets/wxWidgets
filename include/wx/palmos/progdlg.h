///////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/progdlg.h
// Purpose:     wxProgressDialog interface
// Author:      Wlodzimierz ABX Skiba
// Modified by: 
// Created:     29.12.2004
// RCS-ID:      $Id$
// Copyright:   (c) Wlodzimierz Skiba
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _PREFCONF_H_
#define _PREFCONF_H_

#include "wx/defs.h"

#if wxUSE_PROGRESSDLG

#include "wx/dialog.h"

class WXDLLEXPORT wxProgressDialog : public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxProgressDialog)
public:
    wxProgressDialog(const wxString &title,
                     wxString const &message,
                     int maximum = 100,
                     wxWindow *parent = NULL,
                     int style = wxPD_APP_MODAL | wxPD_AUTO_HIDE);
    virtual ~wxProgressDialog();

    virtual bool Update(int value, const wxString& newmsg = wxEmptyString, bool *skip = NULL);
    void Resume();
    virtual bool Show( bool show = true );

    Boolean Callback(/*PrgCallbackData */ void *data);

private:

    /*ProgressType*/ void *m_prgFrame;
    wxString m_msg;
    int m_max,m_cur;
    bool m_canSkip;
    bool m_activeSkip;

    // Virtual function hiding supression
    virtual void Update() { wxDialog::Update(); }

    DECLARE_NO_COPY_CLASS(wxProgressDialog)
};

#endif // wxUSE_PROGRESSDLG

#endif // _PREFCONF_H_

