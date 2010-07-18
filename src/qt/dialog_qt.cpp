/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dialog_qt.cpp
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/qt/dialog_qt.h"

wxQtDialog::wxQtDialog( wxDialog *dialog, QWidget *parent )
: wxQtEventForwarder< QDialog >( parent )
{
    m_dialog = dialog;
}

wxWindow *wxQtDialog::GetEventHandler() const
{
    return m_dialog;
}
