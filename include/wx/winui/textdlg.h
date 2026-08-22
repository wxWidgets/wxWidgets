/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/textdlg.h
// Purpose:     wxTextEntryDialog for wxWinUI
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_TEXTDLG_H_
#define _WX_WINUI_TEXTDLG_H_

#include "wx/defs.h"

#if wxUSE_TEXTDLG

#include "wx/dialog.h"
#include "wx/textctrl.h"

#include <memory>

class wxWinUITextEntryPeerState;

#if wxUSE_VALIDATORS
    #include "wx/valtext.h"
#endif

extern WXDLLIMPEXP_DATA_CORE(const char) wxGetTextFromUserPromptStr[];
extern WXDLLIMPEXP_DATA_CORE(const char) wxGetPasswordFromUserPromptStr[];

#define wxTextEntryDialogStyle (wxOK | wxCANCEL | wxCENTRE)

class WXDLLIMPEXP_CORE wxTextEntryDialog : public wxDialog
{
public:
    wxTextEntryDialog();
    wxTextEntryDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption = wxASCII_STR(wxGetTextFromUserPromptStr),
                      const wxString& value = wxEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize sz = wxDefaultSize)
    {
        Create(parent, message, caption, value, style, pos, sz);
    }

    virtual ~wxTextEntryDialog();

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption = wxASCII_STR(wxGetTextFromUserPromptStr),
                const wxString& value = wxEmptyString,
                long style = wxTextEntryDialogStyle,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize sz = wxDefaultSize);

    int ShowModal() override;

    void SetValue(const wxString& val);
    wxString GetValue() const { return m_value; }

    void SetHint(const wxString& hint);

    void SetMaxLength(unsigned long len);

    void ForceUpper();

    // Implementation-only deterministic seam for the WinUI peer contract.
    // It is used by tests to exercise a ForceUpper() call made while the
    // modal surface is already open, without physical input.
    bool WinUISetPeerValueForTesting(const wxString& value);
    wxString WinUIGetPeerValueForTesting() const;

#if wxUSE_VALIDATORS
    void SetTextValidator(const wxTextValidator& validator);
    void SetTextValidator(wxTextValidatorStyle style = wxFILTER_NONE);
    wxTextValidator* GetTextValidator() { return m_validator; }
#endif // wxUSE_VALIDATORS

    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;

    void OnOK(wxCommandEvent& event);

protected:
    // Initialized here and not in the constructors: the non-default
    // constructor only calls Create(), and an uninitialized m_validator made
    // the destructor delete a garbage pointer -- an immediate crash when
    // lucky, a delayed heap corruption when not.
    wxTextCtrl* m_textctrl = nullptr;
    wxString m_value;
    long m_dialogStyle = 0;

    wxWindow* m_winuiParent = nullptr;
    wxString m_message;
    wxString m_caption;
    wxString m_hint;
    wxPoint m_pos;
    wxSize m_size;
    unsigned long m_maxLength = 0;
    bool m_forceUpper = false;
    bool m_isPassword = false;
    std::shared_ptr<wxWinUITextEntryPeerState> m_peerState;

#if wxUSE_VALIDATORS
    wxTextValidator* m_validator = nullptr;
#endif

private:
    wxDECLARE_DYNAMIC_CLASS(wxTextEntryDialog);
    wxDECLARE_NO_COPY_CLASS(wxTextEntryDialog);
};

class WXDLLIMPEXP_CORE wxPasswordEntryDialog : public wxTextEntryDialog
{
public:
    wxPasswordEntryDialog() = default;
    wxPasswordEntryDialog(wxWindow *parent,
                          const wxString& message,
                          const wxString& caption = wxASCII_STR(wxGetPasswordFromUserPromptStr),
                          const wxString& value = wxEmptyString,
                          long style = wxTextEntryDialogStyle,
                          const wxPoint& pos = wxDefaultPosition)
    {
        Create(parent, message, caption, value, style, pos);
    }

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption = wxASCII_STR(wxGetPasswordFromUserPromptStr),
                const wxString& value = wxEmptyString,
                long style = wxTextEntryDialogStyle,
                const wxPoint& pos = wxDefaultPosition);

private:
    wxDECLARE_DYNAMIC_CLASS(wxPasswordEntryDialog);
    wxDECLARE_NO_COPY_CLASS(wxPasswordEntryDialog);
};

WXDLLIMPEXP_CORE wxString
    wxGetTextFromUser(const wxString& message,
                      const wxString& caption = wxASCII_STR(wxGetTextFromUserPromptStr),
                      const wxString& default_value = wxEmptyString,
                      wxWindow *parent = nullptr,
                      wxCoord x = wxDefaultCoord,
                      wxCoord y = wxDefaultCoord,
                      bool centre = true);

WXDLLIMPEXP_CORE wxString
    wxGetPasswordFromUser(const wxString& message,
                          const wxString& caption = wxASCII_STR(wxGetPasswordFromUserPromptStr),
                          const wxString& default_value = wxEmptyString,
                          wxWindow *parent = nullptr,
                          wxCoord x = wxDefaultCoord,
                          wxCoord y = wxDefaultCoord,
                          bool centre = true);

#endif // wxUSE_TEXTDLG

#endif // _WX_WINUI_TEXTDLG_H_
