/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/msgdlg.h
// Purpose:     wxMessageDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
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
                    const wxPoint& pos = wxDefaultPosition);

    virtual int ShowModal();

    // customization of the message box
    virtual bool SetYesNoLabels(const wxString& yes,const wxString& no);
    virtual bool SetYesNoCancelLabels(const wxString& yes, const wxString& no, const wxString& cancel);
    virtual bool SetOKLabel(const wxString& ok);
    virtual bool SetOKCancelLabels(const wxString& ok, const wxString& cancel);

protected:
    // not supported for message dialog
    virtual void DoSetSize(int WXUNUSED(x), int WXUNUSED(y),
                           int WXUNUSED(width), int WXUNUSED(height),
                           int WXUNUSED(sizeFlags) = wxSIZE_AUTO) {}

    // labels for the buttons
    wxString m_yes,
             m_no,
             m_ok,
             m_cancel;

    DECLARE_DYNAMIC_CLASS(wxMessageDialog)
};

#endif // _WX_MSGBOXDLG_H_
