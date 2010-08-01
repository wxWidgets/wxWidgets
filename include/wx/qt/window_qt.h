/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/window_qt.h
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_WIDGET_QT_H_
#define _WX_QT_WIDGET_QT_H_

#include "wx/window.h"
#include "wx/qt/winevent_qt.h"
#include <QtGui/QFrame>

class WXDLLIMPEXP_CORE wxQtWidget : public wxQtEventForwarder< wxWindow, QFrame >
{
    Q_OBJECT

    public:
        wxQtWidget( wxWindow *window, QWidget *parent );
};

#if wxUSE_ACCEL || defined( Q_MOC_RUN )
class WXDLLIMPEXP_CORE wxQtShortcutHandler : public QObject, public wxQtSignalForwarder< wxWindow >
{
    Q_OBJECT
    public:
        wxQtShortcutHandler( wxWindow *window );
    
    public Q_SLOTS:
        void activated();
};
#endif // wxUSE_ACCEL

#endif
