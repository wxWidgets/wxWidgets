/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/filedlg.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKFILEDLG_H_
#define _WX_GTKFILEDLG_H_

#include "wx/gtk/filectrl.h"    // for wxGtkFileChooser

//-------------------------------------------------------------------------
// wxFileDialog
//-------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFileDialog: public wxFileDialogBase
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
    virtual ~wxFileDialog();

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

    virtual int ShowModal() override;

    virtual bool AddShortcut(const wxString& directory, int flags = 0) override;
    virtual bool SupportsExtraControl() const override { return true; }

    // Implementation only.
    void GTKSelectionChanged(const wxString& filename);


protected:
    // override this from wxTLW since the native
    // form doesn't have any m_wxwindow
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) override;


private:
    void OnFakeOk( wxCommandEvent &event );
    void OnSize(wxSizeEvent&);
    virtual void AddChildGTK(wxWindowGTK* child) override;

    wxGtkFileChooser    m_fc;

    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_GTKFILEDLG_H_
