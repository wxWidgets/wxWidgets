/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/dirdlg.h
// Purpose:     wxDirDialog class using Standard File Package
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_DIRDLG_H_
#define _WX_RETRO68_DIRDLG_H_

// Standard File Package doesn't support selecting folders directly, so this
// class uses the standard "Save" dialog and returns the folder in which the
// user chose to "save" the file, which is a traditional way of doing it.
class WXDLLIMPEXP_CORE wxDirDialog : public wxDirDialogBase
{
public:
    wxDirDialog() = default;

    wxDirDialog(wxWindow *parent,
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxDirDialogNameStr))
    {
        Create(parent, message, defaultPath, style, pos, size, name);
    }

    bool Create(wxWindow *parent,
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& WXUNUSED(pos) = wxDefaultPosition,
                const wxSize& WXUNUSED(size) = wxDefaultSize,
                const wxString& WXUNUSED(name) = wxASCII_STR(wxDirDialogNameStr))
    {
        // Don't create any wx window, the native dialog is used instead.
        m_parent = parent;
        m_windowStyle = style;
        m_message = message;
        m_path = defaultPath;
        return true;
    }

    virtual int ShowModal() override;

    virtual bool Show(bool WXUNUSED(show) = true) override { return false; }

private:
    wxDECLARE_DYNAMIC_CLASS(wxDirDialog);
};

#endif // _WX_RETRO68_DIRDLG_H_
