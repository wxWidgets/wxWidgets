/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/filedlg.h
// Purpose:     wxFileDialog class using Standard File Package
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_FILEDLG_H_
#define _WX_RETRO68_FILEDLG_H_

//-------------------------------------------------------------------------
// wxFileDialog
//-------------------------------------------------------------------------

// This class uses the native System 7 Standard File dialogs, which only
// support selecting a single file and filtering the files by name using the
// first wildcard filter only.
class WXDLLIMPEXP_CORE wxFileDialog : public wxFileDialogBase
{
public:
    wxFileDialog() = default;
    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxASCII_STR(wxFileSelectorPromptStr),
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxASCII_STR(wxFileSelectorDefaultWildcardStr),
                 long style = wxFD_DEFAULT_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& sz = wxDefaultSize,
                 const wxString& name = wxASCII_STR(wxFileDialogNameStr))
    {
        Create(parent, message, defaultDir, defaultFile, wildCard,
               style, pos, sz, name);
    }

    virtual void GetPaths(wxArrayString& paths) const override;
    virtual void GetFilenames(wxArrayString& files) const override;

    virtual int ShowModal() override;

    virtual bool Show(bool WXUNUSED(show) = true) override { return false; }

private:
    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
};

#endif // _WX_RETRO68_FILEDLG_H_
