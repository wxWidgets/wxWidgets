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
    virtual void enterEvent ( QEvent * event )
    {
        if ( !GetEventReceiver()->QtHandleEnterEvent(this, event) )
            QtWidget::enterEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    //virtual void focusInEvent ( QFocusEvent * event ) { }

    //wxFocusEvent.
    //virtual void focusOutEvent ( QFocusEvent * event ) { }

    //wxShowEvent
    //virtual void hideEvent ( QHideEvent * event ) { }

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
    virtual void moveEvent ( QMoveEvent * event ) { }
    
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
    //virtual void showEvent ( QShowEvent * event ) { }
    
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
