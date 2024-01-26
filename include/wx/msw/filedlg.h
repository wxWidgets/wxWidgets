/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/filedlg.h
// Purpose:     wxFileDialog class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLG_H_
#define _WX_FILEDLG_H_

class wxFileDialogMSWData;

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
    virtual ~wxFileDialog();

    virtual void GetPaths(wxArrayString& paths) const override;
    virtual void GetFilenames(wxArrayString& files) const override;
    virtual bool AddShortcut(const wxString& directory, int flags = 0) override;
    virtual bool SupportsExtraControl() const override { return true; }

    virtual int ShowModal() override;

protected:

    virtual void DoMoveWindow(int x, int y, int width, int height) override;
    virtual void DoCentre(int dir) override;
    virtual void DoGetSize( int *width, int *height ) const override;
    virtual void DoGetPosition( int *x, int *y ) const override;

private:
    // Allow it to call MSWOnXXX() functions below.
    friend class wxFileDialogMSWData;

    // called when the dialog is created
    void MSWOnInitDialogHook(WXHWND hwnd);

    // called when the dialog initialization is fully done
    void MSWOnInitDone(WXHWND hDlg);

    // called when the currently selected file changes in the dialog
    void MSWOnSelChange(const wxString& selectedFilename);

    // called when the currently selected type of files changes in the dialog
    void MSWOnTypeChange(int nFilterIndex);

    // called when the dialog is accepted, i.e. a file is chosen in it
    void MSWOnFileOK();

    // The real implementation of ShowModal() using traditional common dialog
    // functions.
    int ShowCommFileDialog(WXHWND owner);

    // And another one using IFileDialog.
    int ShowIFileDialog(WXHWND owner);

    // Get the data object, allocating it if necessary.
    wxFileDialogMSWData& MSWData();


    wxArrayString m_fileNames;

    // Extra data, possibly null if not needed, use MSWData() to access it if
    // it should be created on demand.
    wxFileDialogMSWData* m_data = nullptr;


    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
    wxDECLARE_NO_COPY_CLASS(wxFileDialog);
};

#endif // _WX_FILEDLG_H_

