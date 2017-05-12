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

//-------------------------------------------------------------------------
// wxFileDialog
//-------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFileDialog: public wxFileDialogBase
{
public:
    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = wxFD_DEFAULT_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& sz = wxDefaultSize,
                 const wxString& name = wxFileDialogNameStr);

    virtual void GetPaths(wxArrayString& paths) const wxOVERRIDE;
    virtual void GetFilenames(wxArrayString& files) const wxOVERRIDE;
    virtual bool SupportsExtraControl() const wxOVERRIDE { return true; }
    void MSWOnInitDialogHook(WXHWND hwnd);

    virtual int ShowModal() wxOVERRIDE;

    // wxMSW-specific implementation from now on
    // -----------------------------------------

    // called from the hook procedure on CDN_INITDONE reception
    virtual void MSWOnInitDone(WXHWND hDlg);

    // called from the hook procedure on CDN_SELCHANGE.
    void MSWOnSelChange(WXHWND hDlg);

protected:

    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE;
    virtual void DoCentre(int dir) wxOVERRIDE;
    virtual void DoGetSize( int *width, int *height ) const wxOVERRIDE;
    virtual void DoGetPosition( int *x, int *y ) const wxOVERRIDE;

private:
    wxArrayString m_fileNames;

    // remember if our SetPosition() or Centre() (which requires special
    // treatment) was called
    bool m_bMovedWindow;
    int m_centreDir;        // nothing to do if 0

    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
    wxDECLARE_NO_COPY_CLASS(wxFileDialog);
};

#endif // _WX_FILEDLG_H_

