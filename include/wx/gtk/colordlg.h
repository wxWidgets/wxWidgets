/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/colordlg.h
// Purpose:     wxColourDialog
// Author:      Vaclav Slavik
// Created:     2004/06/04
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_COLORDLG_H_
#define _WX_GTK_COLORDLG_H_

#include "wx/dialog.h"

class WXDLLIMPEXP_CORE wxColourDialog : public wxDialog
{
public:
    wxColourDialog() = default;
    wxColourDialog(wxWindow *parent,
                   const wxColourData *data = nullptr);
    virtual ~wxColourDialog() = default;

    bool Create(wxWindow *parent, const wxColourData *data = nullptr);

    wxColourData &GetColourData() { return m_data; }

    virtual int ShowModal() override;

protected:
    // implement some base class methods to do nothing to avoid asserts and
    // GTK warnings, since this is not a real wxDialog.
    virtual void DoSetSize(int WXUNUSED(x), int WXUNUSED(y),
                           int WXUNUSED(width), int WXUNUSED(height),
                           int WXUNUSED(sizeFlags) = wxSIZE_AUTO) override {}
    virtual void DoMoveWindow(int WXUNUSED(x), int WXUNUSED(y),
                              int WXUNUSED(width), int WXUNUSED(height)) override {}

    // copy data between the dialog and m_colourData:
    void ColourDataToDialog();
    void DialogToColourData();

    wxColourData m_data;

    wxDECLARE_DYNAMIC_CLASS(wxColourDialog);
};

#endif
