/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/button_qt.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/qt/button_qt.h"

wxQtButton::wxQtButton( const QString &text, QWidget *parent )
    : QPushButton( text, parent )
{
}


