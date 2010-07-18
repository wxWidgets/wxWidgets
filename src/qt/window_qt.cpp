/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/window_qt.cpp
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/qt/window_qt.h"

wxQtWidget::wxQtWidget( wxWindow *window, QWidget *parent )
: wxQtEventForwarder< QWidget >( parent )
{
    m_wxWindow = window;
}

wxWindow *wxQtWidget::GetEventHandler() const
{
    return m_wxWindow;
}
