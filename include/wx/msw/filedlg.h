/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/filedlg.h
// Purpose:     wxFileDialog class
// Author:      Julian Smart
// Modified by:
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

    virtual void GetPaths(wxArrayString& paths) const wxOVERRIDE;
    virtual void GetFilenames(wxArrayString& files) const wxOVERRIDE;
    virtual bool AddShortcut(const wxString& directory, int flags = 0) wxOVERRIDE;
    virtual bool SupportsExtraControl() const wxOVERRIDE { return true; }

    virtual int ShowModal() wxOVERRIDE;

protected:

    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE;
    virtual void DoCentre(int dir) wxOVERRIDE;
    virtual void DoGetSize( int *width, int *height ) const wxOVERRIDE;
    virtual void DoGetPosition( int *x, int *y ) const wxOVERRIDE;

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
    wxFileDialogMSWData* m_data;


    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
    wxDECLARE_NO_COPY_CLASS(wxFileDialog);
};

#endif // _WX_FILEDLG_H_

