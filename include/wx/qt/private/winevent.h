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

#include "wx/window.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/utils.h"

#include <QtCore/QEvent>
#include <QtGui/QPaintEvent>

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
        // Set immediatelly as it is used to check if wxWindow is alive
        wxWindow::QtStoreWindowPointer( this, handler );

        // Handle QWidget destruction signal AFTER it gets deleted
        QObject::connect( this, &QObject::destroyed, this,
                          &wxQtEventSignalHandler::HandleDestroyedSignal );

    }

    void HandleDestroyedSignal()
    {
        wxQtHandleDestroyedSignal(this);
    }

    virtual Handler *GetHandler() const
    {
        // Only process the signal / event if the wxWindow is not destroyed
        if ( !wxWindow::QtRetrieveWindowPointer( this ) )
        {
            wxLogDebug( wxT("%s win pointer is NULL (wxWindow is deleted)!"),
                        Widget::staticMetaObject.className()
                        );
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
    virtual void changeEvent ( QEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::changeEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleChangeEvent(this, event) )
            Widget::changeEvent(event);
        else
            event->accept();
    }

    //wxCloseEvent
    virtual void closeEvent ( QCloseEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::closeEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleCloseEvent(this, event) )
            Widget::closeEvent(event);
        else
            event->accept();
    }

    //wxContextMenuEvent
    virtual void contextMenuEvent ( QContextMenuEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::contextMenuEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleContextMenuEvent(this, event) )
            Widget::contextMenuEvent(event);
        else
            event->accept();
    }

    //wxDropFilesEvent
    //virtual void dropEvent ( QDropEvent * event ) { }

    //wxMouseEvent
    virtual void enterEvent ( QEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::enterEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleEnterEvent(this, event) )
            Widget::enterEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusInEvent ( QFocusEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::focusInEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleFocusEvent(this, event) )
            Widget::focusInEvent(event);
        else
            event->accept();
    }

    //wxFocusEvent.
    virtual void focusOutEvent ( QFocusEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::focusOutEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleFocusEvent(this, event) )
            Widget::focusOutEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void hideEvent ( QHideEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::hideEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleShowEvent(this, event) )
            Widget::hideEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyPressEvent ( QKeyEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::keyPressEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleKeyEvent(this, event) )
            Widget::keyPressEvent(event);
        else
            event->accept();
    }

    //wxKeyEvent
    virtual void keyReleaseEvent ( QKeyEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::keyReleaseEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleKeyEvent(this, event) )
            Widget::keyReleaseEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void leaveEvent ( QEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::leaveEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleEnterEvent(this, event) )
            Widget::leaveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseDoubleClickEvent ( QMouseEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::mouseDoubleClickEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseDoubleClickEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseMoveEvent ( QMouseEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::mouseMoveEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseMoveEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mousePressEvent ( QMouseEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::mousePressEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mousePressEvent(event);
        else
            event->accept();
    }

    //wxMouseEvent
    virtual void mouseReleaseEvent ( QMouseEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::mouseReleaseEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleMouseEvent(this, event) )
            Widget::mouseReleaseEvent(event);
        else
            event->accept();
    }
    
    //wxMoveEvent
    virtual void moveEvent ( QMoveEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::moveEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleMoveEvent(this, event) )
            Widget::moveEvent(event);
        else
            event->accept();
    }
    
    //wxEraseEvent then wxPaintEvent
    virtual void paintEvent ( QPaintEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::paintEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandlePaintEvent(this, event) )
            Widget::paintEvent(event);
        else
            event->accept();
    }

    //wxSizeEvent
    virtual void resizeEvent ( QResizeEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::resizeEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleResizeEvent(this, event) )
            Widget::resizeEvent(event);
        else
            event->accept();
    }

    //wxShowEvent
    virtual void showEvent ( QShowEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::showEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleShowEvent(this, event) )
            Widget::showEvent(event);
        else
            event->accept();
    }
    
    //wxMouseEvent
    virtual void wheelEvent ( QWheelEvent * event )
    {
        if ( !this->GetHandler() )
            wxLogDebug( wxT("%s::wheelEvent for invalid handler!"),
                        Widget::staticMetaObject.className() );
        else if ( !this->GetHandler()->QtHandleWheelEvent(this, event) )
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
