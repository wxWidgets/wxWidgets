/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/window_qt.cpp
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/qt/window_qt.h"

wxQtWidget::wxQtWidget( wxWindow *parent, wxWindow *handler )
    : wxQtEventSignalHandler< QWidget, wxWindow >( parent, handler )
{
}

#if wxUSE_ACCEL
wxQtShortcutHandler::wxQtShortcutHandler( wxWindow *window )
    : wxQtSignalHandler< wxWindow >( window )
{
}

void wxQtShortcutHandler::activated()
{
    int command = sender()->property("wxQt_Command").toInt();

    GetHandler()->QtHandleShortcut( command );
}
#endif // wxUSE_ACCEL
