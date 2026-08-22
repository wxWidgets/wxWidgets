/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/msgdlg.h
// Purpose:     wxMessageDialog for wxWinUI
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_MSGBOXDLG_H_
#define _WX_WINUI_MSGBOXDLG_H_

class WXDLLIMPEXP_CORE wxMessageDialog : public wxMessageDialogBase
{
public:
    wxMessageDialog(wxWindow *parent,
                    const wxString& message,
                    const wxString& caption = wxASCII_STR(wxMessageBoxCaptionStr),
                    long style = wxOK|wxCENTRE,
                    const wxPoint& WXUNUSED(pos) = wxDefaultPosition)
        : wxMessageDialogBase(parent, message, caption, style)
    {
        // Keep the same wxWindow lifetime contract as the other WinUI
        // common dialogs. The visible surface is provided by the presenter,
        // but its nested loop must still receive wxEVT_DESTROY if the
        // wxMessageDialog object is destroyed while it is open.
        wxDialog::Create(m_parent, wxID_ANY, caption,
                         wxDefaultPosition, wxDefaultSize,
                         wxDEFAULT_DIALOG_STYLE);
    }

    int ShowModal() override;

    static wxFont GetMessageFont();

protected:
    void DoCentre(int dir) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMessageDialog);
};

#endif // _WX_WINUI_MSGBOXDLG_H_
