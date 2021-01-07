/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/qt/winevent_qt.h
// Purpose:     QWidget to wxWindow event handler
// Author:      Javier Torres, Peter Most
// Modified by:
// Created:     21.06.10
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_EVENTSIGNALFORWARDER_H_
#define _WX_QT_EVENTSIGNALFORWARDER_H_

#include <QtCore/QEvent>
#include <QtGui/QCloseEvent>

#include "wx/log.h"
#include "wx/window.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/utils.h"

class QPaintEvent;

template< typename Handler >
class wxQtSignalHandler
{
protected:
    wxQtSignalHandler( Handler *handler )
    {
        m_handler = handler;
    }

    void EmitEvent( wxEvent &event ) const
    {
        wxWindow *handler = GetHandler();
        event.SetEventObject( handler );
        handler->HandleWindowEvent( event );
    }

    virtual Handler *GetHandler() const
    {
        return m_handler;
    }

private:
    Handler *m_handler;
};

template < typename Widget, typename Handler >
class wxQtEventSignalHandler : public Widget, public wxQtSignalHandler< Handler >
{
public:
    wxQtEventSignalHandler( wxWindow *parent, Handler *handler )
        : Widget( parent != NULL ? parent->GetHandle() : NULL )
        , wxQtSignalHandler< Handler >( handler )
    {
        // Set immediately as it is used to check if wxWindow is alive
        wxWindow::QtStoreWindowPointer( this, handler );

        // Handle QWidget destruction signal AFTER it gets deleted
        QObject::connect( this, &QObject::destroyed, this,
                          &wxQtEventSignalHandler::HandleDestroyedSignal );

        Widget::setMouseTracking(true);
    }

    void HandleDestroyedSignal()
    {
    }

    virtual Handler *GetHandler() const wxOVERRIDE
    {
        // Only process the signal / event if the wxWindow is not destroyed
        if ( !wxWindow::QtRetrieveWindowPointer( this ) )
        {
            return NULL;
        }
        else
            return wxQtSignalHandler< Handler >::GetHandler();
    }

protected:
    /* Not implemented here: wxHelpEvent, wxIdleEvent wxJoystickEvent,
     * wxMouseCaptureLostEvent, wxMouseCaptureChangedEvent,
     * wxPowerEvent, wxScrollWinEvent, wxSysColourChangedEvent */

    //wxActivateEvent
    virtual void changeEvent ( QEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleChangeEvent(this, event) )
            Widget::changeEvent(event);
        else
            event->accept();
    }

    //wxCloseEvent
    virtual void closeEvent ( QCloseEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleCloseEvent(this, event) )
            Widget::closeEvent(event);
        else
            event->ignore();
    }

    //wxContextMenuEvent
    virtual void contextMenuEvent ( QContextMenuEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleContextMenuEvent(this, event) )
            Widget::contextMenuEvent(event);
        else
            event->accept();
    }

    //wxDropFilesEvent
    //virtual void dropEvent ( QDropEvent * event ) { }

    //wxMouseEvent
    virtual void enterEvent ( QEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleEnterEvent(this, event) )
            Widget::enterEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusInEvent ( QFocusEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleFocusEvent(this, event) )
            Widget::focusInEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusOutEvent ( QFocusEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleFocusEvent(this, event) )
            Widget::focusOutEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void hideEvent ( QHideEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleShowEvent(this, event) )
            Widget::hideEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyPressEvent ( QKeyEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleKeyEvent(this, event) )
            Widget::keyPressEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyReleaseEvent ( QKeyEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleKeyEvent(this, event) )
            Widget::keyReleaseEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void leaveEvent ( QEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleEnterEvent(this, event) )
            Widget::leaveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseDoubleClickEvent ( QMouseEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseDoubleClickEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseMoveEvent ( QMouseEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseMoveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mousePressEvent ( QMouseEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mousePressEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseReleaseEvent ( QMouseEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseReleaseEvent(event);
        else
            event->accept();
    }

    //wxMoveEvent
    virtual void moveEvent ( QMoveEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMoveEvent(this, event) )
            Widget::moveEvent(event);
        else
            event->accept();
    }

    //wxEraseEvent then wxPaintEvent
    virtual void paintEvent ( QPaintEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandlePaintEvent(this, event) )
            Widget::paintEvent(event);
        else
            event->accept();
    }

    //wxSizeEvent
    virtual void resizeEvent ( QResizeEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleResizeEvent(this, event) )
            Widget::resizeEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void showEvent ( QShowEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleShowEvent(this, event) )
            Widget::showEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void wheelEvent ( QWheelEvent * event ) wxOVERRIDE
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleWheelEvent(this, event) )
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

};

#endif
