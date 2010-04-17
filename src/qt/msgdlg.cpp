/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/msgdlg.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/msgdlg.h"

wxMessageDialog::wxMessageDialog()
{
}

wxMessageDialog::wxMessageDialog(wxWindow *parent, const wxString& message,
                const wxString& caption,
                long style,
                const wxPoint& pos)
{
}

QMessageBox *wxMessageDialog::GetHandle() const
{
    return NULL;
}

