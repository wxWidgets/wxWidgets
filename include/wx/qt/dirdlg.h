/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dirdlg.h
// Author:      Sean D'Epagnier
// Copyright:   (c) 2014 Sean D'Epagnier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DIRDLG_H_
#define _WX_QT_DIRDLG_H_

#include <QtWidgets/QFileDialog>

class WXDLLIMPEXP_CORE wxDirDialog : public wxDirDialogBase
{
public:
    wxDirDialog() { }

    wxDirDialog(wxWindow *parent,
                const wxString& message = wxDirSelectorPromptStr,
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxDirDialogNameStr);

    bool Create(wxWindow *parent,
                const wxString& message = wxDirSelectorPromptStr,
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxDirDialogNameStr);

public:     // overrides from wxGenericDirDialog

    wxString GetPath() const wxOVERRIDE;
    void SetPath(const wxString& path) wxOVERRIDE;

    virtual QFileDialog *GetHandle() const;

private:

    wxDECLARE_DYNAMIC_CLASS(wxDirDialog);
};

#endif // _WX_QT_DIRDLG_H_
