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
#include <QtGui/QWidget>
#include <QtGui/QScrollBar>
#include "wx/qt/winevent_qt.h"

class WXDLLIMPEXP_CORE wxQtWidget : public WindowEventForwarder< QWidget >
{
    Q_OBJECT

    public:
        wxQtWidget( wxWindow *window, QWidget *parent );
        
    protected:
        virtual wxWindow *GetEventReceiver();

    private:
        wxWindow *m_wxWindow;

};

class WXDLLIMPEXP_CORE wxQtScrollBarEventForwarder : public QScrollBar
{
    Q_OBJECT

    public:
        wxQtScrollBarEventForwarder( wxWindow *window,
                                     Qt::Orientation orient,
                                     QWidget *parent = 0 );

    private Q_SLOTS:
        void OnActionTriggered( int action );
        void OnSliderReleased();
        
    private:
        wxWindow *m_wxWindow;    
};

#endif
