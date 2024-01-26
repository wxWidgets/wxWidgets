/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/filedlg.h
// Author:      Sean D'Epagnier
// Copyright:   (c) 2014 Sean D'Epagnier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FILEDLG_H_
#define _WX_QT_FILEDLG_H_

class QFileDialog;

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
                 const wxString& name = wxASCII_STR(wxFileDialogNameStr));
    bool Create(wxWindow *parent,
                 const wxString& message = wxASCII_STR(wxFileSelectorPromptStr),
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxASCII_STR(wxFileSelectorDefaultWildcardStr),
                 long style = wxFD_DEFAULT_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& sz = wxDefaultSize,
                 const wxString& name = wxASCII_STR(wxFileDialogNameStr));

    virtual wxString GetPath() const override;
    virtual void GetPaths(wxArrayString& paths) const override;
    virtual wxString GetFilename() const override;
    virtual void GetFilenames(wxArrayString& files) const override;
    virtual int GetFilterIndex() const override;

    virtual void SetMessage(const wxString& message) override;
    virtual void SetPath(const wxString& path) override;
    virtual void SetDirectory(const wxString& dir) override;
    virtual void SetFilename(const wxString& name) override;
    virtual void SetWildcard(const wxString& wildCard) override;
    virtual void SetFilterIndex(int filterIndex) override;

    virtual bool SupportsExtraControl() const override { return true; }

    virtual QFileDialog *GetQFileDialog() const;

private:

    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
};

#endif // _WX_QT_FILEDLG_H_
