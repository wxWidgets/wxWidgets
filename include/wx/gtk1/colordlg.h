/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/colordlg.h
// Purpose:     wxColourDialog
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/06/04
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __COLORDLG_H__
#define __COLORDLG_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "colordlg.h"
#endif

#include "wx/setup.h"
#include "wx/gdicmn.h"
#include "wx/dialog.h"
#include "wx/cmndata.h"


class WXDLLEXPORT wxColourDialog : public wxDialog
{
public:
    wxColourDialog() {}
    wxColourDialog(wxWindow *parent,
                   wxColourData *data = (wxColourData *)NULL);
    virtual ~wxColourDialog() {}

    bool Create(wxWindow *parent, wxColourData *data = (wxColourData *)NULL);

    wxColourData &GetColourData() { return m_data; }

    virtual int ShowModal();

protected:
    // implement some base class methods to do nothing to avoid asserts and
    // GTK warnings, since this is not a real wxDialog.
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) {}
    virtual void DoMoveWindow(int x, int y, int width, int height) {}

    // copy data between the dialog and m_colourData:
    void ColourDataToDialog();
    void DialogToColourData();
    
    wxColourData m_data;

    DECLARE_DYNAMIC_CLASS(wxColourDialog)
};

#endif
