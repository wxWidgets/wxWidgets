/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/window_qt.cpp
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/qt/window_qt.h"

wxQtWidget::wxQtWidget( wxWindow *window, QWidget *parent )
    : wxQtEventForwarder< wxWindow, QFrame >( window, parent )
{
}

#if wxUSE_ACCEL
wxQtShortcutHandler::wxQtShortcutHandler( wxWindow *window )
    : wxQtSignalForwarder< wxWindow >( window )
{
}

void wxQtShortcutHandler::activated()
{
    int command = sender()->property("wxQt_Command").toInt();
    
    GetSignalHandler()->QtHandleShortcut( command );
}
#endif // wxUSE_ACCEL
