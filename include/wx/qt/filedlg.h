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
    wxFileDialog() { }
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

    virtual wxString GetPath() const wxOVERRIDE;
    virtual void GetPaths(wxArrayString& paths) const wxOVERRIDE;
    virtual wxString GetFilename() const wxOVERRIDE;
    virtual void GetFilenames(wxArrayString& files) const wxOVERRIDE;
    virtual int GetFilterIndex() const wxOVERRIDE;

    virtual void SetMessage(const wxString& message) wxOVERRIDE;
    virtual void SetPath(const wxString& path) wxOVERRIDE;
    virtual void SetDirectory(const wxString& dir) wxOVERRIDE;
    virtual void SetFilename(const wxString& name) wxOVERRIDE;
    virtual void SetWildcard(const wxString& wildCard) wxOVERRIDE;
    virtual void SetFilterIndex(int filterIndex) wxOVERRIDE;

    virtual bool SupportsExtraControl() const wxOVERRIDE { return true; }

    virtual QFileDialog *GetQFileDialog() const;

private:

    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
};

#endif // _WX_QT_FILEDLG_H_
