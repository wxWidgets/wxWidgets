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

template <typename QtWidget>
class wxQtEventForwarder : public QtWidget
{
public:
    wxQtEventForwarder(QWidget *parent) : QtWidget(parent){ }

protected:
    //Must be implemented by all classes using this template
    //to return the window where the events will be sent
    virtual wxWindow *GetEventReceiver() = 0;

    /* Not implemented here: wxHelpEvent, wxIdleEvent wxJoystickEvent,
     * wxMouseCaptureLostEvent, wxMouseCaptureChangedEvent,
     * wxPowerEvent, wxScrollWinEvent, wxSysColourChangedEvent */

    //wxActivateEvent
    virtual void changeEvent ( QEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleChangeEvent(this, event) )
            QtWidget::changeEvent(event);
        else
            event->accept();
    }

    //wxCloseEvent
    virtual void closeEvent ( QCloseEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleCloseEvent(this, event) )
            QtWidget::closeEvent(event);
        else
            event->accept();
    }

    //wxContextMenuEvent
    virtual void contextMenuEvent ( QContextMenuEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleCMenuEvent(this, event) )
            QtWidget::contextMenuEvent(event);
        else
            event->accept();
    }

    //wxDropFilesEvent
    //virtual void dropEvent ( QDropEvent * event ) { }

    //wxMouseEvent
    virtual void enterEvent ( QEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleEnterEvent(this, event) )
            QtWidget::enterEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusInEvent ( QFocusEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleFocusEvent(this, event) )
            QtWidget::focusInEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusOutEvent ( QFocusEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleFocusEvent(this, event) )
            QtWidget::focusOutEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void hideEvent ( QHideEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleShowEvent(this, event) )
            QtWidget::hideEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyPressEvent ( QKeyEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleKeyEvent(this, event) )
            QtWidget::keyPressEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyReleaseEvent ( QKeyEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleKeyEvent(this, event) )
            QtWidget::keyReleaseEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void leaveEvent ( QEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleEnterEvent(this, event) )
            QtWidget::leaveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseDoubleClickEvent ( QMouseEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleMouseEvent(this, event) )
            QtWidget::mouseDoubleClickEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseMoveEvent ( QMouseEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleMouseEvent(this, event) )
            QtWidget::mouseMoveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mousePressEvent ( QMouseEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleMouseEvent(this, event) )
            QtWidget::mousePressEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseReleaseEvent ( QMouseEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleMouseEvent(this, event) )
            QtWidget::mouseReleaseEvent(event);
        else
            event->accept();
    }
    
    //wxMoveEvent
    virtual void moveEvent ( QMoveEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleMoveEvent(this, event) )
            QtWidget::moveEvent(event);
        else
            event->accept();
    }
    
    //wxEraseEvent then wxPaintEvent
    virtual void paintEvent ( QPaintEvent * event )
    {
        if ( !GetEventReceiver()->QtHandlePaintEvent(this, event) )
            QtWidget::paintEvent(event);
        else
            event->accept();

        // Extra: Paint the wxClientDC part for both Qt and wx handling
        // This has to be here to be able to call after Qt paints if wx
        // doesn't handle the event.
        GetEventReceiver()->QtPaintClientDCPicture( this );
    }

    //wxSizeEvent
    virtual void resizeEvent ( QResizeEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleResizeEvent(this, event) )
            QtWidget::resizeEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void showEvent ( QShowEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleShowEvent(this, event) )
            QtWidget::showEvent(event);
        else
            event->accept();
    }
    
    //wxMouseEvent
    virtual void wheelEvent ( QWheelEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleWheelEvent(this, event) )
            QtWidget::wheelEvent(event);
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
};

#endif
