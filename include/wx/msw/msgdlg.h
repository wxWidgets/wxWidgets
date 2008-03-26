/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/msgdlg.h
// Purpose:     wxMessageDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGBOXDLG_H_
#define _WX_MSGBOXDLG_H_

class WXDLLIMPEXP_CORE wxMessageDialog : public wxMessageDialogBase
{
public:
    wxMessageDialog(wxWindow *parent,
                    const wxString& message,
                    const wxString& caption = wxMessageBoxCaptionStr,
                    long style = wxOK|wxCENTRE,
                    const wxPoint& WXUNUSED(pos) = wxDefaultPosition)
        : wxMessageDialogBase(parent, message, caption, style)
    {
        m_hook = NULL;
    }


    virtual int ShowModal();

private:
    static WXLRESULT wxCALLBACK HookFunction(int code, WXWPARAM, WXLPARAM);

    WXHANDLE m_hook; // HHOOK used to position the message box

    DECLARE_DYNAMIC_CLASS(wxMessageDialog)
    DECLARE_NO_COPY_CLASS(wxMessageDialog)
};


#endif // _WX_MSGBOXDLG_H_
