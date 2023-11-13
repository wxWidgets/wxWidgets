/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/qt/winevent_qt.h
// Purpose:     QWidget to wxWindow event handler
// Author:      Javier Torres, Peter Most
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

// redeclare wxEVT_TEXT_ENTER here instead of including "wx/textctrl.h"
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_TEXT_ENTER, wxCommandEvent);

class QPaintEvent;


class wxQtSignalHandler
{
protected:
    wxQtSignalHandler( wxEvtHandler *handler )
    {
        m_handler = handler;
    }

    bool EmitEvent( wxEvent &event ) const
    {
        // We're only called with the objects of class (or derived from)
        // wxWindow, so the cast is safe.
        wxWindow* const handler = static_cast<wxWindow *>(GetHandler());
        event.SetEventObject( handler );
        return handler->HandleWindowEvent( event );
    }

    virtual wxEvtHandler *GetHandler() const
    {
        return m_handler;
    }

    // A hack for wxQtEventSignalHandler<>::keyPressEvent() handler for the
    // convenience of wxTextCtrl-like controls to emit the wxEVT_TEXT_ENTER
    // event if the control has wxTE_PROCESS_ENTER flag.
    bool HandleKeyPressEvent(QWidget* widget, QKeyEvent* e)
    {
        // We're only called with the objects of class (or derived from)
        // wxWindow, so the cast is safe.
        wxWindow* const handler = static_cast<wxWindow *>(GetHandler());

        if ( handler->HasFlag(wxTE_PROCESS_ENTER) )
        {
            if ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter )
            {
                wxCommandEvent event( wxEVT_TEXT_ENTER, handler->GetId() );
                event.SetString( GetValueForProcessEnter() );
                event.SetEventObject( handler );
                return handler->HandleWindowEvent( event );
            }
        }

        return handler->QtHandleKeyEvent(widget, e);
    }

    // Controls supporting wxTE_PROCESS_ENTER flag (e.g. wxTextCtrl, wxComboBox and wxSpinCtrl)
    // should override this to return the control value as string when enter is pressed.
    virtual wxString GetValueForProcessEnter() { return wxString(); }

private:
    wxEvtHandler *m_handler;
};

template < typename Widget, typename Handler >
class wxQtEventSignalHandler : public Widget, public wxQtSignalHandler
{
public:
    wxQtEventSignalHandler( wxWindow *parent, Handler *handler )
        : Widget( parent != nullptr ? parent->GetHandle() : nullptr )
        , wxQtSignalHandler( handler )
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
            return static_cast<Handler*>(wxQtSignalHandler::GetHandler());
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

        this->GetHandler()->QtHandleContextMenuEvent(this, event);

        // Notice that we are simply accepting the event and deliberately not
        // calling Widget::contextMenuEvent(event); here because the context menu
        // is supposed to be shown from a wxEVT_CONTEXT_MENU handler and not from
        // QWidget::contextMenuEvent() overrides (and we are already in one of
        // these overrides to perform QContextMenuEvent --> wxContextMenuEvent
        // translation).
        // More importantly, the default implementation of contextMenuEvent() simply
        // ignores the context event, which means that the event will be propagated
        // to the parent widget again which is undesirable here because the event may
        // have already been propagated at the wxWidgets level.

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

        if ( !this->HandleKeyPressEvent(this, event) )
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

// RAII wrapper for blockSignals(). It blocks signals in its constructor and in
// the destructor it restores the state to what it was before the constructor ran.
class wxQtEnsureSignalsBlocked
{
public:
    // Use QObject instead of QWidget to avoid including <QWidget> from here.
    wxQtEnsureSignalsBlocked(QObject *widget) :
        m_widget(widget)
    {
        m_restore = m_widget->blockSignals(true);
    }

    ~wxQtEnsureSignalsBlocked()
    {
        m_widget->blockSignals(m_restore);
    }

private:
    QObject* const m_widget;
    bool m_restore;

    wxDECLARE_NO_COPY_CLASS(wxQtEnsureSignalsBlocked);
};

#endif
