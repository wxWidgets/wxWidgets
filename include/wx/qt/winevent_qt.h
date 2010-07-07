/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/qt/winevent_qt.h
// Purpose:     QWidget to wxWindow event forwarding class template
// Author:      Javier Torres
// Modified by:
// Created:     21.06.10
// RCS-ID:      $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_EVENTFORWARDER_H_
#define _WX_QT_EVENTFORWARDER_H_

#include "wx/window.h"
#include "wx/qt/converter.h"

#include <QtCore/QEvent>
#include <QtGui/QPaintEvent>

template <typename T> class WindowEventForwarder : public T
{
public:
    WindowEventForwarder<T>(QWidget *parent) : T(parent) { }
    
protected:
    //Must be implemented by all classes using this template
    //to return the window where the events will be sent
    virtual wxWindow *GetEventReceiver() = 0;

    /* Not implemented here: wxChildFocusEvent, wxHelpEvent, wxIdleEvent
        * wxJoystickEvent, wxMouseCaptureLostEvent, wxMouseCaptureChangedEvent,
        * wxPowerEvent, wxScrollWinEvent, wxSysColourChangedEvent */

    //wxActivateEvent
    //virtual void changeEvent ( QEvent * event ) { }
    
    //wxCloseEvent
    //virtual void closeEvent ( QCloseEvent * event ) { }
    
    //wxContextMenuEvent
    //virtual void contextMenuEvent ( QContextMenuEvent * event ) { }
    
    //wxDropFilesEvent
    //virtual void dropEvent ( QDropEvent * event ) { }
    
    //wxMouseEvent
    //virtual void enterEvent ( QEvent * event ) { }
    
    //wxFocusEvent.
    //virtual void focusInEvent ( QFocusEvent * event ) { }
    
    //wxFocusEvent.
    //virtual void focusOutEvent ( QFocusEvent * event ) { }
    
    //wxShowEvent
    //virtual void hideEvent ( QHideEvent * event ) { }
    
    //wxKeyEvent
    //virtual void keyPressEvent ( QKeyEvent * event ) { }
    
    //wxKeyEvent
    //virtual void keyReleaseEvent ( QKeyEvent * event ) { }
    
    //wxMouseEvent
    //virtual void leaveEvent ( QEvent * event ) { }
    
    //wxMouseEvent
    //virtual void mouseDoubleClickEvent ( QMouseEvent * event ) { }
    
    //wxMouseEvent
    //virtual void mouseMoveEvent ( QMouseEvent * event ) { }
    
    //wxMouseEvent
    //virtual void mousePressEvent ( QMouseEvent * event ) { }
    
    //wxMouseEvent
    //virtual void mouseReleaseEvent ( QMouseEvent * event ) { }
    
    //wxMoveEvent
    //virtual void moveEvent ( QMoveEvent * event ) { }
    
    //wxEraseEvent then wxPaintEvent
    virtual void paintEvent ( QPaintEvent * event )
    {
        event->accept();

        if ( !GetEventReceiver()->HandleQtPaintEvent(this, event) )
            T::paintEvent(event);
    }
    
    //wxSizeEvent
    virtual void resizeEvent ( QResizeEvent * event )
    {
        event->accept();

        wxSizeEvent e(wxQtConvertSize(event->size()));
        if (!GetEventReceiver()->ProcessWindowEvent(e))
            T::resizeEvent(event);
    }
    
    //wxShowEvent
    //virtual void showEvent ( QShowEvent * event ) { }
    
    //wxMouseEvent
    virtual void wheelEvent ( QWheelEvent * event )
    {
        event->accept();

        wxMouseEvent e(wxEVT_MOUSEWHEEL);
        e.m_wheelAxis = (event->orientation() == Qt::Vertical) ? 0 : 1;
        e.m_wheelRotation = event->delta();
        e.m_linesPerAction = 3;
        e.m_wheelDelta = 120;

        if (!GetEventReceiver()->ProcessWindowEvent(e))
            T::wheelEvent(event);
    }

    /* Unused Qt events
    virtual void actionEvent ( QActionEvent * event ) { }
    virtual void dragEnterEvent ( QDragEnterEvent * event ) { }
    virtual void dragLeaveEvent ( QDragLeaveEvent * event ) { }
    virtual void dragMoveEvent ( QDragMoveEvent * event ) { }
    virtual void inputMethodEvent ( QInputMethodEvent * event ) { }
    virtual bool macEvent ( EventHandlerCallRef caller, EventRef event ) { }
    virtual bool qwsEvent ( QWSEvent * event ) { }
    virtual void tabletEvent ( QTabletEvent * event ) { }
    virtual bool winEvent ( MSG * message, long * result ) { }
    virtual bool x11Event ( XEvent * event ) { } */
};

#endif
