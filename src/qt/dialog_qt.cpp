/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dialog_qt.cpp
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/qt/dialog_qt.h"

wxQtDialog::wxQtDialog( wxWindow *parent, wxDialog *handler )
    : wxQtEventSignalHandler< QDialog, wxDialog >( parent, handler )
{
}
