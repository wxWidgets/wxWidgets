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

#include <QtWidgets/QGestureEvent>
#include <QtGui/QCursor>

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
        : Widget( parent != nullptr ? parent->GetHandle() : nullptr )
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

    virtual Handler *GetHandler() const override
    {
        // Only process the signal / event if the wxWindow is not destroyed
        if ( !wxWindow::QtRetrieveWindowPointer( this ) )
        {
            return nullptr;
        }
        else
            return wxQtSignalHandler< Handler >::GetHandler();
    }

protected:
    /* Not implemented here: wxHelpEvent, wxIdleEvent wxJoystickEvent,
     * wxMouseCaptureLostEvent, wxMouseCaptureChangedEvent,
     * wxPowerEvent, wxScrollWinEvent, wxSysColourChangedEvent */

    //wxActivateEvent
    virtual void changeEvent ( QEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleChangeEvent(this, event) )
            Widget::changeEvent(event);
        else
            event->accept();
    }

    //wxCloseEvent
    virtual void closeEvent ( QCloseEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleCloseEvent(this, event) )
            Widget::closeEvent(event);
        else
            event->ignore();
    }

    //wxContextMenuEvent
    virtual void contextMenuEvent ( QContextMenuEvent * event ) override
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
    virtual void enterEvent ( QEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleEnterEvent(this, event) )
            Widget::enterEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusInEvent ( QFocusEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleFocusEvent(this, event) )
            Widget::focusInEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusOutEvent ( QFocusEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleFocusEvent(this, event) )
            Widget::focusOutEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void hideEvent ( QHideEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleShowEvent(this, event) )
            Widget::hideEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyPressEvent ( QKeyEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleKeyEvent(this, event) )
            Widget::keyPressEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyReleaseEvent ( QKeyEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleKeyEvent(this, event) )
            Widget::keyReleaseEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void leaveEvent ( QEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleEnterEvent(this, event) )
            Widget::leaveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseDoubleClickEvent ( QMouseEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseDoubleClickEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseMoveEvent ( QMouseEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseMoveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mousePressEvent ( QMouseEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mousePressEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseReleaseEvent ( QMouseEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseReleaseEvent(event);
        else
            event->accept();
    }

    //wxMoveEvent
    virtual void moveEvent ( QMoveEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleMoveEvent(this, event) )
            Widget::moveEvent(event);
        else
            event->accept();
    }

    //wxEraseEvent then wxPaintEvent
    virtual void paintEvent ( QPaintEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandlePaintEvent(this, event) )
            Widget::paintEvent(event);
        else
            event->accept();
    }

    //wxSizeEvent
    virtual void resizeEvent ( QResizeEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleResizeEvent(this, event) )
            Widget::resizeEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void showEvent ( QShowEvent * event ) override
    {
        if ( !this->GetHandler() )
            return;

        if ( !this->GetHandler()->QtHandleShowEvent(this, event) )
            Widget::showEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void wheelEvent ( QWheelEvent * event ) override
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

    virtual bool event(QEvent *event)
    {
        if (event->type() == QEvent::Gesture)
        {
            return gestureEvent(static_cast<QGestureEvent*>(event), event);
        }

        return Widget::event(event);
    }

    bool gestureEvent(QGestureEvent *gesture, QEvent *event)
    {
        if (QGesture *tah = gesture->gesture(Qt::TapAndHoldGesture))
        {
            //  Set the policy so that accepted gestures are taken by the first window that gets them
            tah->setGestureCancelPolicy ( QGesture::CancelAllInContext );
            tapandholdTriggered(static_cast<QTapAndHoldGesture *>(tah), event);
        }

        if (QGesture *pan = gesture->gesture(Qt::PanGesture))
        {
            panTriggered(static_cast<QPanGesture *>(pan), event);
        }

        if (QGesture *pinch = gesture->gesture(Qt::PinchGesture))
        {
            pinchTriggered(static_cast<QPinchGesture *>(pinch), event);
        }

        return true;
    }

    void tapandholdTriggered(QTapAndHoldGesture *gesture, QEvent *event)
    {
        wxWindow *win = wxWindow::QtRetrieveWindowPointer( this );

        if (gesture->state() == Qt::GestureFinished)
        {
            if ( win )
            {
                wxLongPressEvent ev(win->GetId());
                ev.SetPosition( wxQtConvertPoint( gesture->position().toPoint() ) );

                ev.SetGestureEnd();
                win->ProcessWindowEvent( ev );
                event->accept();
            }

        }
        else if (gesture->state() == Qt::GestureStarted)
        {
            event->accept();
        }
        else
        {
            event->accept();
        }
    }

    void panTriggered(QPanGesture *gesture, QEvent *event)
    {
        wxWindow *win = wxWindow::QtRetrieveWindowPointer( this );

        if (win)
        {
            wxPanGestureEvent evp(win->GetId());
            QPoint pos = QCursor::pos();
            evp.SetPosition( wxQtConvertPoint( pos ) );

            QPoint offset = gesture->offset().toPoint();
            QPoint offset_last = gesture->lastOffset().toPoint();
            QPoint delta(offset.x() - offset_last.x(), offset.y() - offset_last.y());

            evp.SetDelta( wxQtConvertPoint( delta ) );

            switch(gesture->state())
            {
                case Qt::GestureStarted:
                    evp.SetGestureStart();
                    break;
                case Qt::GestureFinished:
                case Qt::GestureCanceled:
                    evp.SetGestureEnd();
                    break;
                default:
                    break;
            }

            win->ProcessWindowEvent( evp );

            event->accept();
        }
    }

    void pinchTriggered(QPinchGesture *gesture, QEvent *event)
    {
        wxWindow *win = wxWindow::QtRetrieveWindowPointer( this );
        if (win)
        {

            qreal this_sf = gesture->scaleFactor();
            QPoint center_point = gesture->centerPoint().toPoint();

            wxZoomGestureEvent evp(win->GetId());
            evp.SetPosition( wxQtConvertPoint( center_point ) );
            evp.SetZoomFactor( this_sf);

            switch(gesture->state())
            {
                case Qt::GestureStarted:
                    evp.SetGestureStart();
                    break;
                case Qt::GestureFinished:
                case Qt::GestureCanceled:
                    evp.SetGestureEnd();
                    break;
                default:
                    break;
            }

            win->ProcessWindowEvent( evp );

            event->accept();

        }
    }
};

#endif
