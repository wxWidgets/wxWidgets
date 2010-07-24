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

// TODO: Check whether wxQtEventForwarder and wxQtSignalForwarder can be merged
// into one class (wxQtNotificationForwarder or wxQtEventSignalForwarder), because
// we probably need a way to forward both events and signals.

template <typename Handler, typename Widget>
class wxQtEventForwarder : public Widget
{
public:
    wxQtEventForwarder(Handler *handler, QWidget *parent)
        : Widget(parent)
    {
        m_handler = handler;
    }

protected:
    Handler *GetEventHandler() const
    {
        return m_handler;
    }

    /* Not implemented here: wxHelpEvent, wxIdleEvent wxJoystickEvent,
     * wxMouseCaptureLostEvent, wxMouseCaptureChangedEvent,
     * wxPowerEvent, wxScrollWinEvent, wxSysColourChangedEvent */

    //wxActivateEvent
    virtual void changeEvent ( QEvent * event )
    {
        if ( !GetEventHandler()->QtHandleChangeEvent(this, event) )
            Widget::changeEvent(event);
        else
            event->accept();
    }

    //wxCloseEvent
    virtual void closeEvent ( QCloseEvent * event )
    {
        if ( !GetEventHandler()->QtHandleCloseEvent(this, event) )
            Widget::closeEvent(event);
        else
            event->accept();
    }

    //wxContextMenuEvent
    virtual void contextMenuEvent ( QContextMenuEvent * event )
    {
        if ( !GetEventHandler()->QtHandleContextMenuEvent(this, event) )
            Widget::contextMenuEvent(event);
        else
            event->accept();
    }

    //wxDropFilesEvent
    //virtual void dropEvent ( QDropEvent * event ) { }

    //wxMouseEvent
    virtual void enterEvent ( QEvent * event )
    {
        if ( !GetEventHandler()->QtHandleEnterEvent(this, event) )
            Widget::enterEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusInEvent ( QFocusEvent * event )
    {
        if ( !GetEventHandler()->QtHandleFocusEvent(this, event) )
            Widget::focusInEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusOutEvent ( QFocusEvent * event )
    {
        if ( !GetEventHandler()->QtHandleFocusEvent(this, event) )
            Widget::focusOutEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void hideEvent ( QHideEvent * event )
    {
        if ( !GetEventHandler()->QtHandleShowEvent(this, event) )
            Widget::hideEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyPressEvent ( QKeyEvent * event )
    {
        if ( !GetEventHandler()->QtHandleKeyEvent(this, event) )
            Widget::keyPressEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyReleaseEvent ( QKeyEvent * event )
    {
        if ( !GetEventHandler()->QtHandleKeyEvent(this, event) )
            Widget::keyReleaseEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void leaveEvent ( QEvent * event )
    {
        if ( !GetEventHandler()->QtHandleEnterEvent(this, event) )
            Widget::leaveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseDoubleClickEvent ( QMouseEvent * event )
    {
        if ( !GetEventHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseDoubleClickEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseMoveEvent ( QMouseEvent * event )
    {
        if ( !GetEventHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseMoveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mousePressEvent ( QMouseEvent * event )
    {
        if ( !GetEventHandler()->QtHandleMouseEvent(this, event) )
            Widget::mousePressEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseReleaseEvent ( QMouseEvent * event )
    {
        if ( !GetEventHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseReleaseEvent(event);
        else
            event->accept();
    }
    
    //wxMoveEvent
    virtual void moveEvent ( QMoveEvent * event )
    {
        if ( !GetEventHandler()->QtHandleMoveEvent(this, event) )
            Widget::moveEvent(event);
        else
            event->accept();
    }
    
    //wxEraseEvent then wxPaintEvent
    virtual void paintEvent ( QPaintEvent * event )
    {
        if ( !GetEventHandler()->QtHandlePaintEvent(this, event) )
            Widget::paintEvent(event);
        else
            event->accept();

        // Extra: Paint the wxClientDC part for both Qt and wx handling
        // This has to be here to be able to call after Qt paints if wx
        // doesn't handle the event.
        GetEventHandler()->QtPaintClientDCPicture( this );
    }

    //wxSizeEvent
    virtual void resizeEvent ( QResizeEvent * event )
    {
        if ( !GetEventHandler()->QtHandleResizeEvent(this, event) )
            Widget::resizeEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void showEvent ( QShowEvent * event )
    {
        if ( !GetEventHandler()->QtHandleShowEvent(this, event) )
            Widget::showEvent(event);
        else
            event->accept();
    }
    
    //wxMouseEvent
    virtual void wheelEvent ( QWheelEvent * event )
    {
        if ( !GetEventHandler()->QtHandleWheelEvent(this, event) )
            Widget::wheelEvent(event);
        else
            event->accept();
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

private:
    Handler *m_handler;
};



template < typename Handler >
class wxQtSignalForwarder
{
public:
    wxQtSignalForwarder( Handler *handler )
    {
        m_handler = handler;
    }

    Handler *GetSignalHandler() const
    {
        return m_handler;
    }

private:
    Handler *m_handler;
};


#endif
