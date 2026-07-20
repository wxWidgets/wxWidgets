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
    }

    int ShowModal() override;

    static wxFont GetMessageFont();

protected:
    void DoCentre(int dir) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMessageDialog);
};

#endif // _WX_WINUI_MSGBOXDLG_H_
