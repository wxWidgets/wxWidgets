/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/colordlg.h
// Purpose:     wxColourDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLORDLG_H_
#define _WX_COLORDLG_H_

#include "wx/dialog.h"

// ----------------------------------------------------------------------------
// wxColourDialog: dialog for choosing a colours
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxColourDialog : public wxDialog
{
public:
    wxColourDialog() { Init(); }
    wxColourDialog(wxWindow *parent, const wxColourData *data = NULL)
    {
        Init();

        Create(parent, data);
    }

    bool Create(wxWindow *parent, const wxColourData *data = NULL);

    wxColourData& GetColourData() { return m_colourData; }

    // override some base class virtuals
    virtual void SetTitle(const wxString& title) wxOVERRIDE;
    virtual wxString GetTitle() const wxOVERRIDE;

    virtual int ShowModal() wxOVERRIDE;

    // wxMSW-specific implementation from now on
    // -----------------------------------------

    // called from the hook procedure on WM_INITDIALOG reception
    virtual void MSWOnInitDone(WXHWND hDlg);

    // called from the hook procedure
    void MSWCheckIfCurrentChanged(WXCOLORREF currentCol);

protected:
    // common part of all ctors
    void Init();

    virtual void DoGetPosition( int *x, int *y ) const wxOVERRIDE;
    virtual void DoGetSize(int *width, int *height) const wxOVERRIDE;
    virtual void DoGetClientSize(int *width, int *height) const wxOVERRIDE;
    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE;
    virtual void DoCentre(int dir) wxOVERRIDE;

    wxColourData        m_colourData;
    wxString            m_title;

    // Currently selected colour, used while the dialog is being shown.
    WXCOLORREF m_currentCol;

    // indicates that the dialog should be centered in this direction if non 0
    // (set by DoCentre(), used by MSWOnInitDone())
    int m_centreDir;

    // true if DoMoveWindow() had been called
    bool m_movedWindow;


    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxColourDialog);
};

#endif // _WX_COLORDLG_H_
