/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/numdlgg.h
// Purpose:     wxNumberEntryDialog class
// Author:      John Labenski
// Created:     07.02.04 (extracted from textdlgg.cpp)
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __NUMDLGH_G__
#define __NUMDLGH_G__

#include "wx/defs.h"

#if wxUSE_NUMBERDLG

#include "wx/dialog.h"

#if wxUSE_SPINCTRL
    class WXDLLIMPEXP_FWD_CORE wxSpinCtrl;
#else
    class WXDLLIMPEXP_FWD_CORE wxTextCtrl;
#endif // wxUSE_SPINCTRL

// ----------------------------------------------------------------------------
// wxNumberEntryDialog: a dialog with spin control, [ok] and [cancel] buttons
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNumberEntryDialog : public wxDialog
{
public:
    wxNumberEntryDialog()
    {
        m_value = m_min = m_max = 0;
    }

    wxNumberEntryDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& prompt,
                        const wxString& caption,
                        long value, long min, long max,
                        const wxPoint& pos = wxDefaultPosition)
    {
        Create(parent, message, prompt, caption, value, min, max, pos);
    }

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& prompt,
                const wxString& caption,
                long value, long min, long max,
                const wxPoint& pos = wxDefaultPosition);

    long GetValue() const { return m_value; }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Shown over its parent, as a ContentDialog, when the program asked for
    // that presentation of its dialogs (wxWinUISetDialogPresentation()).
    int ShowModal() override;
#endif

    // implementation only
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

protected:

#if wxUSE_SPINCTRL
    wxSpinCtrl *m_spinctrl;
#else
    wxTextCtrl *m_spinctrl;
#endif // wxUSE_SPINCTRL

    long m_value, m_min, m_max;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    wxString m_winuiMessage;
    wxString m_winuiPrompt;
#endif

private:
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxNumberEntryDialog);
    wxDECLARE_NO_COPY_CLASS(wxNumberEntryDialog);
};

// ----------------------------------------------------------------------------
// function to get a number from user
// ----------------------------------------------------------------------------

WXDLLIMPEXP_CORE long
    wxGetNumberFromUser(const wxString& message,
                        const wxString& prompt,
                        const wxString& caption,
                        long value = 0,
                        long min = 0,
                        long max = 100,
                        wxWindow *parent = nullptr,
                        const wxPoint& pos = wxDefaultPosition);

#endif // wxUSE_NUMBERDLG

#endif // __NUMDLGH_G__
