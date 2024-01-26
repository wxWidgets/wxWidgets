/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/colordlg.h
// Purpose:     wxColourDialog class
// Author:      Julian Smart
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
    wxColourDialog(wxWindow *parent, const wxColourData *data = nullptr)
    {
        Init();

        Create(parent, data);
    }

    bool Create(wxWindow *parent, const wxColourData *data = nullptr);

    wxColourData& GetColourData() { return m_colourData; }

    // override some base class virtuals
    virtual void SetTitle(const wxString& title) override;
    virtual wxString GetTitle() const override;

    virtual int ShowModal() override;

    // wxMSW-specific implementation from now on
    // -----------------------------------------

    // called from the hook procedure on WM_INITDIALOG reception
    virtual void MSWOnInitDone(WXHWND hDlg);

    // called from the hook procedure
    void MSWCheckIfCurrentChanged(WXCOLORREF currentCol);

protected:
    // common part of all ctors
    void Init();

    virtual void DoGetPosition( int *x, int *y ) const override;
    virtual void DoGetSize(int *width, int *height) const override;
    virtual void DoGetClientSize(int *width, int *height) const override;
    virtual void DoMoveWindow(int x, int y, int width, int height) override;
    virtual void DoCentre(int dir) override;

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
