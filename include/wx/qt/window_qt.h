/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/window_qt.h
// Author:      Javier Torres
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_WIDGET_QT_H_
#define _WX_QT_WIDGET_QT_H_

#include "wx/window.h"
#include "wx/qt/winevent_qt.h"
#include <QtWidgets/QWidget>

class WXDLLIMPEXP_CORE wxQtWidget : public wxQtEventSignalHandler< QWidget, wxWindow >
{

    public:
        wxQtWidget( wxWindow *parent, wxWindow *handler );
};

#if wxUSE_ACCEL || defined( Q_MOC_RUN )
class WXDLLIMPEXP_CORE wxQtShortcutHandler : public QObject, public wxQtSignalHandler< wxWindow >
{

public:
    wxQtShortcutHandler( wxWindow *window );

public:
    void activated();
};
#endif // wxUSE_ACCEL

#endif
