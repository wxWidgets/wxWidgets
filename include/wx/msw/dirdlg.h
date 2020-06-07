/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dirdlg.h
// Purpose:     wxDirDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRDLG_H_
#define _WX_DIRDLG_H_

class WXDLLIMPEXP_CORE wxDirDialog : public wxDirDialogBase
{
public:
    wxDirDialog(wxWindow *parent,
                const wxString& message = wxDirSelectorPromptStr,
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxDirDialogNameStr);

    void SetPath(const wxString& path) wxOVERRIDE;

    // can be used only when wxDD_MULTIPLE flag is not set
    wxString GetPath() const wxOVERRIDE;

    // should be used only when wxDD_MULTIPLE flag is set
    void GetPaths(wxArrayString& paths) const wxOVERRIDE;

    virtual int ShowModal() wxOVERRIDE;

private:
    // Used for wxDD_MULTIPLE
    wxArrayString m_paths;

    // The real implementations of ShowModal(), used for Windows versions
    // before and since Vista.
    int ShowSHBrowseForFolder(WXHWND owner);
    int ShowIFileOpenDialog(WXHWND owner);

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDirDialog);
};

#endif
    // _WX_DIRDLG_H_
