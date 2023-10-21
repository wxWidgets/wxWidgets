/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/dirdlg.h
// Purpose:     wxDirDialog class
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRDLG_H_
#define _WX_DIRDLG_H_

#if wxOSX_USE_COCOA
    DECLARE_WXCOCOA_OBJC_CLASS(NSOpenPanel);
#endif

class WXDLLIMPEXP_CORE wxDirDialog : public wxDirDialogBase
{
public:
    wxDirDialog() { Init(); }

    wxDirDialog(wxWindow *parent,
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxT(""),
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxDirDialogNameStr))
    {
        Init();

        Create(parent,message,defaultPath,style,pos,size,name);
    }

    void Create(wxWindow *parent,
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxT(""),
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxDirDialogNameStr));

#if wxOSX_USE_COCOA
    ~wxDirDialog();
#endif

    virtual int ShowModal() override;

    // MacOS 10.11 has removed the titlebar from the dialog, so this is provided
    // only for compatibility with older versions
    virtual void SetTitle(const wxString& title) override;

#if wxOSX_USE_COCOA
    virtual void ShowWindowModal() override;
    virtual void ModalFinishedCallback(void* panel, int returnCode) override;
#endif

private:
#if wxOSX_USE_COCOA
    // Create and initialize NSOpenPanel that we use in both ShowModal() and
    // ShowWindowModal().
    WX_NSOpenPanel OSXCreatePanel() const;
#endif

    // Common part of all ctors.
    void Init();

    wxString m_title;

    wxDECLARE_DYNAMIC_CLASS(wxDirDialog);
};

#endif // _WX_DIRDLG_H_
