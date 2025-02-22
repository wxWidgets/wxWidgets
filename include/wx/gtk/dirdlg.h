/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dirdlg.h
// Purpose:     wxDirDialog
// Author:      Francesco Montorsi
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKDIRDLG_H_
#define _WX_GTKDIRDLG_H_

typedef struct _GtkFileChooser GtkFileChooser;

//-------------------------------------------------------------------------
// wxDirDialog
//-------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDirDialog : public wxDirDialogBase
{
    typedef wxDirDialogBase BaseType;
public:
    wxDirDialog() = default;

    wxDirDialog(wxWindow *parent,
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxDirDialogNameStr));
    bool Create(wxWindow *parent,
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxDirDialogNameStr));
    ~wxDirDialog();

    virtual int ShowModal() override;
    virtual void EndModal(int retCode) override;
    void SetPath(const wxString& path) override;

    // Implementation only.

    void GTKOnAccept();
    void GTKOnCancel();

protected:
    // override this from wxTLW since the native
    // form doesn't have any m_wxwindow
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) override;


private:
    void GTKAccept();
    GtkFileChooser* m_fileChooser = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxDirDialog);
};

#endif // _WX_GTKDIRDLG_H_
