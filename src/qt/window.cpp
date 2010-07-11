/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/window.cpp
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/window.h"
#include "wx/tooltip.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"
#include "wx/qt/window_qt.h"

#include <QtGui/QGridLayout>

#define VERT_SCROLLBAR_POSITION 0, 1
#define HORZ_SCROLLBAR_POSITION 1, 0

//##############################################################################

BEGIN_EVENT_TABLE( wxWindow, wxWindowBase )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS( wxWindow, wxWindowBase )

static wxWindow *s_capturedWindow = NULL;

/* static */ wxWindow *wxWindowBase::DoFindFocus()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return NULL;
}


wxWindow::wxWindow()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

}


wxWindow::wxWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name)
{
    Create( parent, id, pos, size, style, name );
}

bool wxWindow::Create( wxWindow * parent, wxWindowID WXUNUSED( id ),
    const wxPoint & pos, const wxSize & size,
    long WXUNUSED( style ), const wxString & WXUNUSED( name ))
{
    // Should have already been created in the derived class in most cases
    
    if (GetHandle() == NULL) {
        // Window has not been created yet (wxPanel subclass, plain wxWindow, etc.)
        
        QWidget *qtParent = NULL;
        if ( parent != NULL ) {
            qtParent = parent->QtGetContainer();
            parent->AddChild(this);
        }
        
        m_qtWindow = new wxQtWidget(this, qtParent);
        
    }

    // Create layout for built-in scrolling bars
    if ( QtGetScrollBarsContainer() )
    {
        QGridLayout *scrollLayout = new QGridLayout();
        scrollLayout->setContentsMargins( 0, 0, 0, 0 );
        QtGetScrollBarsContainer()->setLayout( scrollLayout );

        // Container at top-left
        // Scrollbars are lazily initialized
        m_qtContainer = new wxQtWidget( this, GetHandle() );
        scrollLayout->addWidget( m_qtContainer, 0, 0 );
        m_qtContainer->setFocus();
    }

    Move(pos);
    SetSize(size);

    return ( true );
}

bool wxWindow::Show( bool show )
{
    GetHandle()->setVisible( show );

    return wxWindowBase::Show( show );
}


void wxWindow::SetLabel(const wxString& label)
{
    GetHandle()->setWindowTitle( wxQtConvertString( label ));
}


wxString wxWindow::GetLabel() const
{
    return ( wxQtConvertString( GetHandle()->windowTitle() ));
}
    
void wxWindow::SetFocus()
{
    GetHandle()->setFocus();
}


void wxWindow::Raise()
{
    GetHandle()->raise();
}

void wxWindow::Lower()
{
    GetHandle()->lower();
}


void wxWindow::WarpPointer(int x, int y)
{
    GetHandle()->move( x, y );
}

void wxWindow::Update()
{
    GetHandle()->update();
}

void wxWindow::Refresh( bool WXUNUSED( eraseBackground ), const wxRect *rect )
{
    if ( rect != NULL )
        GetHandle()->update( wxQtConvertRect( *rect ));
    else
        GetHandle()->update();
}

    
bool wxWindow::SetFont( const wxFont &font )
{
    GetHandle()->setFont( font.GetHandle() );
    
    return ( true );
}


int wxWindow::GetCharHeight() const
{
    return ( GetHandle()->fontMetrics().height() );
}


int wxWindow::GetCharWidth() const
{
    return ( GetHandle()->fontMetrics().averageCharWidth() );
}

/* Returns a scrollbar for the given orientation, or NULL if the scrollbar
 * has not been previously created and create is false */
QScrollBar *wxWindow::QtGetScrollBar( int orientation, bool create ) const
{
    wxCHECK_MSG( QtGetScrollBarsContainer(), NULL, "This window can't have scrollbars" );
    
    QLayoutItem *qtLayoutItem = NULL;
    QScrollBar *qtScrollBar = NULL;
    
    QGridLayout *scrollLayout = qobject_cast< QGridLayout* >( QtGetScrollBarsContainer()->layout() );
    wxCHECK_MSG( scrollLayout, NULL, "Window without scrolling layout" );
    
    if ( orientation == wxHORIZONTAL )
        qtLayoutItem = scrollLayout->itemAtPosition( HORZ_SCROLLBAR_POSITION );
    else
        qtLayoutItem = scrollLayout->itemAtPosition( VERT_SCROLLBAR_POSITION );
    
    if ( qtLayoutItem == NULL )
    {
        // No item at that position, create scrollbar
        if ( create )
        {
            qtScrollBar = new wxQtScrollBarEventForwarder( const_cast< wxWindow * >( this ),
                orientation == wxHORIZONTAL ? Qt::Horizontal : Qt::Vertical );

            if ( orientation == wxHORIZONTAL )
                scrollLayout->addWidget( qtScrollBar, HORZ_SCROLLBAR_POSITION );
            else
                scrollLayout->addWidget( qtScrollBar, VERT_SCROLLBAR_POSITION );
        }
    }
    else
    {
        // Cast the item to a scrollbar
        qtScrollBar = qobject_cast< QScrollBar* >( qtLayoutItem->widget() );
        wxCHECK_MSG( qtScrollBar, NULL, "Invalid scrolling layout" );
    }

    return qtScrollBar;
}

void wxWindow::SetScrollbar( int orientation, int pos, int thumbvisible, int range, bool WXUNUSED( refresh ))
{
    //If range is zero, don't create the scrollbar
    QScrollBar *qtScrollBar = QtGetScrollBar( orientation, range != 0 );
    
    // Configure the scrollbar
    if (range != 0 )
    {
        wxCHECK_RET( qtScrollBar, "Invalid scrollbar" );
        
        qtScrollBar->setRange( 0, range - thumbvisible );
        qtScrollBar->setPageStep( thumbvisible );
        qtScrollBar->setValue( pos );
        qtScrollBar->show();
    }
    else if ( qtScrollBar )
    {
        // If range is zero and scrollbar exists, hide it
        qtScrollBar->hide();
    }
}

void wxWindow::SetScrollPos( int orientation, int pos, bool WXUNUSED( refresh ))
{
    QScrollBar *qtScrollBar = QtGetScrollBar( orientation );
    wxCHECK_RET( qtScrollBar, "Invalid scrollbar" );

    qtScrollBar->setValue( pos );
}

int wxWindow::GetScrollPos( int orientation ) const
{
    QScrollBar *qtScrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( qtScrollBar, 0, "Invalid scrollbar" );
    
    return qtScrollBar->value();
}

int wxWindow::GetScrollThumb( int orientation ) const
{
    QScrollBar *qtScrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( qtScrollBar, 0, "Invalid scrollbar" );
    
    return qtScrollBar->pageStep();
}

int wxWindow::GetScrollRange( int orientation ) const
{
    QScrollBar *qtScrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( qtScrollBar, 0, "Invalid scrollbar" );
    
    return qtScrollBar->maximum();
}


    // scroll window to the specified position

void wxWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    if ( rect != NULL )
        GetHandle()->scroll( dx, dy, wxQtConvertRect( *rect ));
}
    

void wxWindow::SetDropTarget( wxDropTarget *dropTarget )
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

void wxWindow::DoGetTextExtent(const wxString& string, int *x, int *y, int *descent,
    int *externalLeading, const wxFont *font ) const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}



void wxWindow::DoClientToScreen( int *x, int *y ) const
{
    QPoint screenPosition = GetHandle()->mapToGlobal( QPoint( *x, *y ));
    *x = screenPosition.x();
    *y = screenPosition.y();
}

    
void wxWindow::DoScreenToClient( int *x, int *y ) const
{
    QPoint clientPosition = GetHandle()->mapFromGlobal( QPoint( *x, *y ));
    *x = clientPosition.x();
    *y = clientPosition.y();
}
    

void wxWindow::DoCaptureMouse()
{
    GetHandle()->grabMouse();
}


void wxWindow::DoReleaseMouse()
{
    GetHandle()->releaseMouse();
}

wxWindow *wxWindowBase::GetCapture()
{
    return s_capturedWindow;
}


void wxWindow::DoGetPosition(int *x, int *y) const
{
    QPoint position = GetHandle()->pos();
    *x = position.x();
    *y = position.y();
}


void wxWindow::DoGetSize(int *width, int *height) const
{
    QSize size = GetHandle()->frameSize();
    *width = size.width();
    *height = size.height();
}

    
void wxWindow::DoGetClientSize(int *width, int *height) const
{
    QSize size = QtGetContainer()->size();
    *width = size.width();
    *height = size.height();
}


void wxWindow::DoSetSize(int x, int y, int width, int height, int sizeFlags )
{
    wxMISSING_IMPLEMENTATION( "sizeFlags" );

    DoMoveWindow( x, y, width, height );
}

    
void wxWindow::DoSetClientSize(int width, int height)
{
    GetHandle()->resize( width, height );
}


void wxWindow::DoMoveWindow(int x, int y, int width, int height)
{
    QWidget *qtWidget = GetHandle();

    if (x == wxDefaultCoord)
        x = GetHandle()->x();
    if (y == wxDefaultCoord)
        y = GetHandle()->y();

    if (width == wxDefaultCoord)
        width = GetHandle()->width();
    if (height == wxDefaultCoord)
        height = GetHandle()->height();
    
    qtWidget->move( x, y );
    qtWidget->resize( width, height );
}


#if wxUSE_TOOLTIPS
void wxWindow::DoSetToolTip( wxToolTip *tip )
{
    wxWindowBase::DoSetToolTip( tip );
    
    GetHandle()->setToolTip( wxQtConvertString( tip->GetTip() ));
}
#endif // wxUSE_TOOLTIPS


#if wxUSE_MENUS
bool wxWindow::DoPopupMenu(wxMenu *menu, int x, int y)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
    return ( false );
}
#endif // wxUSE_MENUS

bool wxWindow::QtHandlePaintEvent ( QWidget *receiver, QPaintEvent * WXUNUSED( event ))
{
    /* If this window has scrollbars, only let wx handle the event if it is
     * for the client area (the scrolled part). Events for the whole window
     * (including scrollbars and maybe status or menu bars are handled by Qt */
    
    if ( m_qtContainer && receiver != m_qtContainer )
    {
        return false;
    }
    else
    {
        // Send event
        wxEraseEvent erase;
        ProcessWindowEvent(erase);
        wxPaintEvent paint;
        ProcessWindowEvent(paint);

        return ProcessWindowEvent(paint);
    }
}

bool wxWindow::QtHandleResizeEvent ( QWidget *WXUNUSED( receiver ), QResizeEvent *event )
{
    wxSizeEvent e( wxQtConvertSize( event->size() ) );

    return ProcessWindowEvent( e );
}

bool wxWindow::QtHandleWheelEvent ( QWidget *WXUNUSED( receiver ), QWheelEvent *event )
{
    wxMouseEvent e( wxEVT_MOUSEWHEEL );
    e.m_wheelAxis = ( event->orientation() == Qt::Vertical ) ? 0 : 1;
    e.m_wheelRotation = event->delta();
    e.m_linesPerAction = 3;
    e.m_wheelDelta = 120;
    
    return ProcessWindowEvent( e );
}

/* Auxiliar function for key events. Returns the wx keycode for a qt one.
 * The event is needed to check it flags (numpad key or not) */
static wxKeyCode ConvertQtKeyCode( QKeyEvent *event )
{
    /* First treat common ranges and then handle specific values
     * The macro takes Qt first and last codes and the first wx code
     * to make the conversion */
    #define WXQT_KEY_GROUP( firstQT, lastQT, firstWX ) \
        if ( key >= firstQT && key <= lastQT ) \
            return (wxKeyCode)(key - (firstQT - firstWX));

    int key = event->key();

    if ( event->modifiers().testFlag( Qt::KeypadModifier ) )
    {
        // This is a numpad event
        WXQT_KEY_GROUP( Qt::Key_0, Qt::Key_9, WXK_NUMPAD0 )
        WXQT_KEY_GROUP( Qt::Key_F1, Qt::Key_F4, WXK_NUMPAD_F1 )
        WXQT_KEY_GROUP( Qt::Key_Left, Qt::Key_Down, WXK_NUMPAD_LEFT )

        // * + , - . /
        WXQT_KEY_GROUP( Qt::Key_Asterisk, Qt::Key_Slash, WXK_NUMPAD_MULTIPLY )

        switch (key)
        {
            case Qt::Key_Space:
                return WXK_NUMPAD_SPACE;
            case Qt::Key_Tab:
                return WXK_NUMPAD_TAB;
            case Qt::Key_Enter:
                return WXK_NUMPAD_ENTER;
            case Qt::Key_Home:
                return WXK_NUMPAD_HOME;
            case Qt::Key_PageUp:
                return WXK_NUMPAD_PAGEUP;
            case Qt::Key_PageDown:
                return WXK_NUMPAD_PAGEDOWN;
            case Qt::Key_End:
                return WXK_NUMPAD_END;
            case Qt::Key_Insert:
                return WXK_NUMPAD_INSERT;
            case Qt::Key_Delete:
                return WXK_NUMPAD_DELETE;
            case Qt::Key_Clear:
                return WXK_NUMPAD_BEGIN;
            case Qt::Key_Equal:
                return WXK_NUMPAD_EQUAL;
        }

        // All other possible numpads button have no equivalent in wx
        return (wxKeyCode)0;
    }
        
    // ASCII (basic and extended) values are the same in Qt and wx
    WXQT_KEY_GROUP( 32, 255, 32 );

    // Arrow keys
    WXQT_KEY_GROUP( Qt::Key_Left, Qt::Key_Down, WXK_LEFT )
    
    // F-keys (Note: Qt has up to F35, wx up to F24)
    WXQT_KEY_GROUP( Qt::Key_F1, Qt::Key_F24, WXK_F1 )
    
    // * + , - . /
    WXQT_KEY_GROUP( Qt::Key_Asterisk, Qt::Key_Slash, WXK_MULTIPLY )

    // Special keys in wx. Seems most appropriate to map to LaunchX
    WXQT_KEY_GROUP( Qt::Key_Launch0, Qt::Key_LaunchF, WXK_SPECIAL1 )

    // All other cases
    switch ( key )
    {
        case Qt::Key_Backspace:
            return WXK_BACK;
        case Qt::Key_Tab:
            return WXK_TAB;
        case Qt::Key_Return:
            return WXK_RETURN;
        case Qt::Key_Escape:
            return WXK_ESCAPE;
        case Qt::Key_Cancel:
            return WXK_CANCEL;
        case Qt::Key_Clear:
            return WXK_CLEAR;
        case Qt::Key_Shift:
            return WXK_SHIFT;
        case Qt::Key_Alt:
            return WXK_ALT;
        case Qt::Key_Control:
            return WXK_CONTROL;
        case Qt::Key_Menu:
            return WXK_MENU;
        case Qt::Key_Pause:
            return WXK_PAUSE;
        case Qt::Key_CapsLock:
            return WXK_CAPITAL;
        case Qt::Key_End:
            return WXK_END;
        case Qt::Key_Home:
            return WXK_HOME;
        case Qt::Key_Select:
            return WXK_SELECT;
        case Qt::Key_SysReq:
            return WXK_PRINT;
        case Qt::Key_Execute:
            return WXK_EXECUTE;
        case Qt::Key_Insert:
            return WXK_INSERT;
        case Qt::Key_Help:
            return WXK_HELP;
        case Qt::Key_NumLock:
            return WXK_NUMLOCK;
        case Qt::Key_ScrollLock:
            return WXK_SCROLL;
        case Qt::Key_PageUp:
            return WXK_PAGEUP;
        case Qt::Key_PageDown:
            return WXK_PAGEDOWN;
        case Qt::Key_Meta:
            return WXK_WINDOWS_LEFT;
    }

    // Missing wx-codes: WXK_START, WXK_LBUTTON, WXK_RBUTTON, WXK_MBUTTON
    // WXK_SPECIAL(17-20), WXK_WINDOWS_RIGHT, WXK_WINDOWS_MENU, WXK_COMMAND
    // WXK_SNAPSHOT
    
    return (wxKeyCode)0;
    
    #undef WXQT_KEY_GROUP
}

bool wxWindow::QtHandleKeyEvent ( QWidget *WXUNUSED( receiver ), QKeyEvent *event )
{
    bool handled = false;

    // Build the event
    wxKeyEvent e( event->type() == QEvent::KeyPress ? wxEVT_KEY_DOWN : wxEVT_KEY_UP );
    // TODO: m_x, m_y
    e.m_keyCode = ConvertQtKeyCode( event );

    if ( event->text().isEmpty() )
        e.m_uniChar = 0;
    else
        e.m_uniChar = event->text().at( 0 ).unicode();
    
    e.m_rawCode = event->nativeVirtualKey();
    e.m_rawFlags = event->nativeModifiers();

    // Modifiers
    e.m_controlDown = event->modifiers().testFlag( Qt::ControlModifier );
    e.m_shiftDown = event->modifiers().testFlag( Qt::ShiftModifier );
    e.m_altDown = event->modifiers().testFlag( Qt::AltModifier );
    e.m_metaDown = event->modifiers().testFlag( Qt::MetaModifier );

    handled = ProcessWindowEvent( e );

    // On key presses, send the EVT_CHAR event
    // TODO: Check accelerators
    if ( event->type() == QEvent::KeyPress )
    {
        e.SetEventType( wxEVT_CHAR );

        // Translated key code (including control + letter -> 1-26)
        int translated = 0;
        if ( !event->text().isEmpty() )
            translated = event->text().at( 0 ).toAscii();
        if ( translated )
            e.m_keyCode = translated;
        
        handled |= ProcessWindowEvent( e );
    }
    
    return handled;
}

QWidget *wxWindow::GetHandle() const
{
    return m_qtWindow;
}

QWidget *wxWindow::QtGetContainer() const
{
    if ( m_qtContainer )
        return m_qtContainer;
    else
        return GetHandle();
}

QWidget *wxWindow::QtGetScrollBarsContainer() const
{
    return m_qtWindow;
}
