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
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxDirDialogNameStr));

    void SetPath(const wxString& path) override;

    virtual int ShowModal() override;

private:
    // The real implementations of ShowModal(), one using the traditional shell
    // function and another one using IFileOpenDialog.
    int ShowSHBrowseForFolder(WXHWND owner);
    int ShowIFileOpenDialog(WXHWND owner);

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDirDialog);
};

#endif
    // _WX_DIRDLG_H_
