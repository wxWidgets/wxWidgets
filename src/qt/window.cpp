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
#include "wx/menu.h"
#include "wx/tooltip.h"
#include "wx/scrolbar.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"
#include "wx/qt/window_qt.h"

#include <QtGui/QPicture>
#include <QtGui/QPainter>
#include <QtGui/QGridLayout>
#include <QtGui/QApplication>

#define VERT_SCROLLBAR_POSITION 0, 1
#define HORZ_SCROLLBAR_POSITION 1, 0

//##############################################################################

wxBEGIN_EVENT_TABLE( wxWindow, wxWindowBase )
wxEND_EVENT_TABLE()


// We use the QObject property capabilities to store the wxWindow pointer, so we
// don't need to use a separate lookup table. We also want to use it in the proper
// way and not use/store store void pointers e.g.:
// qVariantSetValue( variant, static_cast< void * >( window ));
// static_cast< wxWindow * >( qVariantValue< void * >( variant ));
// so we declare the corresponding Qt meta type:

Q_DECLARE_METATYPE( const wxWindow * )

static const char WINDOW_POINTER_PROPERTY_NAME[] = "wxWindowPointer";

// We accept a 'const wxWindow *' to indicate that the pointer is only stored:

/* static */ void wxWindow::QtStoreWindowPointer( QWidget *widget, const wxWindow *window )
{
    QVariant variant;
    qVariantSetValue( variant, window );
    widget->setProperty( WINDOW_POINTER_PROPERTY_NAME, variant );
}

/* static */ wxWindow *wxWindow::QtRetrieveWindowPointer( const QWidget *widget )
{
    QVariant variant = widget->property( WINDOW_POINTER_PROPERTY_NAME );
    return const_cast< wxWindow * >( qVariantValue< const wxWindow * >( variant ));
}




static wxWindow *s_capturedWindow = NULL;

/* static */ wxWindow *wxWindowBase::DoFindFocus()
{
    wxWindow *window = NULL;
    QWidget *qtWidget = QApplication::focusWidget();
    if ( qtWidget != NULL )
        window = wxWindow::QtRetrieveWindowPointer( qtWidget );

    return window;
}


wxWindow::wxWindow()
{
    m_horzScrollBar = NULL;
    m_vertScrollBar = NULL;

    m_qtPicture = new QPicture();
    m_qtPaintBuffer = NULL;

    m_mouseInside = false;

    m_qtShortcutHandler = new wxQtShortcutHandler( this );
    m_processingShortcut = false;
}


wxWindow::wxWindow(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name)
{
    Create( parent, id, pos, size, style, name );
}


wxWindow::~wxWindow()
{
    SendDestroyEvent();

    if ( s_capturedWindow == this )
        s_capturedWindow = NULL;

    DestroyChildren(); // This also destroys scrollbars

    delete m_qtPicture;
    delete m_qtPaintBuffer;
}


bool wxWindow::Create( wxWindow * parent, wxWindowID id, const wxPoint & pos,
        const wxSize & size, long style, const wxString &name )
{
    // If the underlying control hasn't been created then this most probably means
    // that a generic control, like wxPanel, is being created, so we need a very
    // simple control as a base:

    if ( GetHandle() == NULL )
        m_qtWindow = new wxQtWidget( parent, this );

    if ( !wxWindowBase::CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
        return false;

    QtStoreWindowPointer( GetHandle(), this );

    parent->AddChild( this );

    DoMoveWindow( pos.x, pos.y, size.GetWidth(), size.GetHeight() );

//    // Create layout for built-in scrolling bars
//    if ( QtGetScrollBarsContainer() )
//    {
//        QGridLayout *scrollLayout = new QGridLayout();
//        scrollLayout->setContentsMargins( 0 , 0, 0, 0 );
//        scrollLayout->setSpacing( 0 );
//        QtGetScrollBarsContainer()->setLayout( scrollLayout );
//
//        // Container at top-left
//        // Scrollbars are lazily initialized
//        m_qtContainer = new wxQtWidget( this, GetHandle() );
//        scrollLayout->addWidget( m_qtContainer, 0, 0 );
//        m_qtContainer->setFocus();
//    }

    
//    // Use custom Qt window flags (allow to turn on or off
//    // the minimize/maximize/close buttons and title bar)
//    Qt::WindowFlags qtFlags = GetHandle()->windowFlags();
//
//    qtFlags |= Qt::CustomizeWindowHint;
//    qtFlags |= Qt::WindowTitleHint;
//    qtFlags |= Qt::WindowSystemMenuHint;
//    qtFlags |= Qt::WindowMinMaxButtonsHint;
//    qtFlags |= Qt::WindowCloseButtonHint;
//
//    GetHandle()->setWindowFlags( qtFlags );
//
//    SetWindowStyleFlag( style );
//
//    m_backgroundColour = wxColour( GetHandle()->palette().color( GetHandle()->backgroundRole() ) );
//    m_foregroundColour = wxColour( GetHandle()->palette().color( GetHandle()->foregroundRole() ) );

    return ( true );
}

bool wxWindow::Show( bool show )
{
    if ( !wxWindowBase::Show( show ))
        return false;

    // Show can be called before the underlying window is created:

    QWidget *qtWidget = GetHandle();
    if ( qtWidget != NULL )
    {
        qtWidget->setVisible( show );
        return true;
    }
    else
        return false;

    wxSizeEvent event(GetSize(), GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);
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

/* static */ void wxWindow::QtReparent( QWidget *child, QWidget *parent )
{
    // Backup the attributes which will be changed during the reparenting:

//    QPoint position = child->pos();
//    bool isVisible = child->isVisible();
    Qt::WindowFlags windowFlags = child->windowFlags();

    child->setParent( parent );

    // Restore the attributes:

    child->setWindowFlags( windowFlags );
//    child->move( position );
//    child->setVisible( isVisible );
}

bool wxWindow::Reparent( wxWindowBase *parent )
{
    if ( !wxWindowBase::Reparent( parent ))
        return false;

    QtReparent( GetHandle(), parent->GetHandle() );

    return true;
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
    // QCursor::setPos takes global screen coordinates, so translate it:

    ClientToScreen( &x, &y );
    QCursor::setPos( x, y );
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

void wxWindow::DoGetTextExtent(const wxString& string, int *x, int *y, int *descent,
        int *externalLeading, const wxFont *font ) const
{
    QFontMetrics fontMetrics( font != NULL ? font->GetHandle() : GetHandle()->font() );

    if ( x != NULL )
        *x = fontMetrics.width( wxQtConvertString( string ));

    if ( y != NULL )
        *y = fontMetrics.height();

    if ( descent != NULL )
        *descent = fontMetrics.descent();

    if ( externalLeading != NULL )
        *externalLeading = fontMetrics.lineSpacing();
}

/* Returns a scrollbar for the given orientation, or NULL if the scrollbar
 * has not been previously created and create is false */
wxScrollBar *wxWindow::QtGetScrollBar( int orientation ) const
{
    wxCHECK_MSG( QtGetScrollBarsContainer(), NULL, "This window can't have scrollbars" );
    wxCHECK_MSG( CanScroll( orientation ), NULL, "Window can't scroll in that orientation" );

    wxScrollBar *scrollBar = NULL;

    if ( orientation == wxHORIZONTAL )
        scrollBar = m_horzScrollBar;
    else
        scrollBar = m_vertScrollBar;
    
    return scrollBar;
}

void wxWindow::SetScrollbar( int orientation, int pos, int thumbvisible, int range, bool refresh )
{
    wxCHECK_RET( CanScroll( orientation ), "Window can't scroll in that orientation" );
    
    //If range is zero, don't create the scrollbar
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );

    // Create a new scrollbar if needed
    if ( !scrollBar && range != 0 )
    {
        QGridLayout *scrollLayout = qobject_cast< QGridLayout* >( QtGetScrollBarsContainer()->layout() );
        wxCHECK_RET( scrollLayout, "Window without scrolling layout" );
        
        scrollBar = new wxScrollBar( const_cast< wxWindow* >( this ), wxID_ANY,
                                     wxDefaultPosition, wxDefaultSize,
                                     orientation == wxHORIZONTAL ? wxSB_HORIZONTAL : wxSB_VERTICAL);

        // Connect scrollbar events to this window
        scrollBar->Bind( wxEVT_SCROLL_LINEUP, &wxWindow::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_LINEDOWN, &wxWindow::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_PAGEUP, &wxWindow::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_PAGEDOWN, &wxWindow::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_TOP, &wxWindow::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_BOTTOM, &wxWindow::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_THUMBTRACK, &wxWindow::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_THUMBRELEASE, &wxWindow::QtOnScrollBarEvent, this );
        
        // Let Qt handle layout
        if ( orientation == wxHORIZONTAL )
        {
            scrollLayout->addWidget( scrollBar->GetHandle(), HORZ_SCROLLBAR_POSITION );
            m_horzScrollBar = scrollBar;
        }
        else
        {
            scrollLayout->addWidget( scrollBar->GetHandle(), VERT_SCROLLBAR_POSITION );
            m_vertScrollBar = scrollBar;
        }
    }
    
    // Configure the scrollbar if it exists. If range is zero we can get here with
    // scrollBar == NULL and it is not a problem
    if ( scrollBar )
    {
        scrollBar->SetScrollbar( pos, thumbvisible, range, thumbvisible, refresh );
        if ( HasFlag( wxALWAYS_SHOW_SB ) && ( range == 0 ) )
        {
            // Disable instead of hide
            scrollBar->GetHandle()->show();
            scrollBar->GetHandle()->setEnabled( false );
        }
        else
            scrollBar->GetHandle()->setEnabled( true );            
    }
}

void wxWindow::SetScrollPos( int orientation, int pos, bool WXUNUSED( refresh ))
{
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_RET( scrollBar, "Invalid scrollbar" );

    scrollBar->SetThumbPosition( pos );
}

int wxWindow::GetScrollPos( int orientation ) const
{
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );
    
    return scrollBar->GetThumbPosition();
}

int wxWindow::GetScrollThumb( int orientation ) const
{
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );
    
    return scrollBar->GetThumbSize();
}

int wxWindow::GetScrollRange( int orientation ) const
{
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );
    
    return scrollBar->GetRange();
}

// Handle event from scrollbars
void wxWindow::QtOnScrollBarEvent( wxScrollEvent& event )
{
    wxEventType windowEventType = 0;

    // Map the scroll bar event to the corresponding scroll window event:

    wxEventType scrollBarEventType = event.GetEventType();
    if ( scrollBarEventType == wxEVT_SCROLL_TOP )
        windowEventType = wxEVT_SCROLLWIN_TOP;
    else if ( scrollBarEventType == wxEVT_SCROLL_BOTTOM )
        windowEventType = wxEVT_SCROLLWIN_BOTTOM;
    else if ( scrollBarEventType == wxEVT_SCROLL_PAGEUP )
        windowEventType = wxEVT_SCROLLWIN_PAGEUP;
    else if ( scrollBarEventType == wxEVT_SCROLL_PAGEDOWN )
        windowEventType = wxEVT_SCROLLWIN_PAGEDOWN;
    else if ( scrollBarEventType == wxEVT_SCROLL_LINEUP )
        windowEventType = wxEVT_SCROLLWIN_LINEUP;
    else if ( scrollBarEventType == wxEVT_SCROLL_LINEDOWN )
        windowEventType = wxEVT_SCROLLWIN_LINEDOWN;
    else if ( scrollBarEventType == wxEVT_SCROLL_THUMBTRACK )
        windowEventType = wxEVT_SCROLLWIN_THUMBTRACK;
    else if ( scrollBarEventType == wxEVT_SCROLL_THUMBRELEASE )
        windowEventType = wxEVT_SCROLLWIN_THUMBRELEASE;

    if ( windowEventType != 0 )
    {
        wxScrollWinEvent event( windowEventType, event.GetPosition(), event.GetOrientation() );
        ProcessWindowEvent( event );
    }
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

void wxWindow::SetWindowStyleFlag( long style )
{
    wxWindowBase::SetWindowStyleFlag( style );
    
//    wxMISSING_IMPLEMENTATION( "wxWANTS_CHARS, wxTAB_TRAVERSAL" );
//    // wxFULL_REPAINT_ON_RESIZE: Qt::WResizeNoErase (marked obsolete)
//    // wxTRANSPARENT_WINDOW, wxCLIP_CHILDREN: Used in window for
//    //   performance, apparently not needed.
//
//    // wxWANTS_CHARS: Need to reimplement event()
//    //   See: http://doc.qt.nokia.com/latest/qwidget.html#events
//    // wxTAB_TRAVERSAL: reimplement focusNextPrevChild()
//
//    Qt::WindowFlags qtFlags = GetHandle()->windowFlags();
//
//    // For this to work Qt::CustomizeWindowHint must be set (done in Create())
//    if ( HasFlag( wxCAPTION ) )
//    {
//        // Enable caption bar and all buttons. This behavious
//        // is overwritten by subclasses (wxTopLevelWindow).
//        qtFlags |= Qt::WindowTitleHint;
//        qtFlags |= Qt::WindowSystemMenuHint;
//        qtFlags |= Qt::WindowMinMaxButtonsHint;
//        qtFlags |= Qt::WindowCloseButtonHint;
//    }
//    else
//    {
//        // Disable caption bar, include all buttons to be effective
//        qtFlags &= ~Qt::WindowTitleHint;
//        qtFlags &= ~Qt::WindowSystemMenuHint;
//        qtFlags &= ~Qt::WindowMinMaxButtonsHint;
//        qtFlags &= ~Qt::WindowCloseButtonHint;
//    }
//
//    GetHandle()->setWindowFlags( qtFlags );
//
//    // Validate border styles
//    int numberOfBorderStyles = 0;
//    if ( HasFlag( wxBORDER_NONE ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_STATIC ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_SIMPLE ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_RAISED ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_SUNKEN ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_THEME ))
//        numberOfBorderStyles++;
//    wxCHECK_RET( numberOfBorderStyles <= 1, "Only one border style can be specified" );
//
//    // Borders only supported for QFrame's
//    QFrame *qtFrame = qobject_cast< QFrame* >( QtGetContainer() );
//    wxCHECK_RET( numberOfBorderStyles == 0 || qtFrame,
//                 "Borders not supported for this window type (not QFrame)" );
//
//    if ( HasFlag( wxBORDER_NONE ) )
//    {
//        qtFrame->setFrameStyle( QFrame::NoFrame );
//    }
//    else if ( HasFlag( wxBORDER_STATIC ) )
//    {
//        wxMISSING_IMPLEMENTATION( "wxBORDER_STATIC" );
//    }
//    else if ( HasFlag( wxBORDER_SIMPLE ) )
//    {
//        qtFrame->setFrameStyle( QFrame::Panel );
//        qtFrame->setFrameShadow( QFrame::Plain );
//    }
//    else if ( HasFlag( wxBORDER_RAISED ) )
//    {
//        qtFrame->setFrameStyle( QFrame::Panel );
//        qtFrame->setFrameShadow( QFrame::Raised );
//    }
//    else if ( HasFlag( wxBORDER_SUNKEN ) )
//    {
//        qtFrame->setFrameStyle( QFrame::Panel );
//        qtFrame->setFrameShadow( QFrame::Sunken );
//    }
//    else if ( HasFlag( wxBORDER_THEME ) )
//    {
//        qtFrame->setFrameStyle( QFrame::StyledPanel );
//        qtFrame->setFrameShadow( QFrame::Plain );
//    }
}

void wxWindow::SetExtraStyle( long exStyle )
{
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
    s_capturedWindow = this;
}


void wxWindow::DoReleaseMouse()
{
    GetHandle()->releaseMouse();
    s_capturedWindow = NULL;
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

    

void wxWindow::DoSetSize(int x, int y, int width, int height, int sizeFlags )
{
    int currentX, currentY;
    GetPosition( &currentX, &currentY );
    if ( x == wxDefaultCoord && !( sizeFlags & wxSIZE_ALLOW_MINUS_ONE ))
        x = currentX;
    if ( y == wxDefaultCoord && !( sizeFlags & wxSIZE_ALLOW_MINUS_ONE ))
        y = currentY;

    // Should we use the best size:

    if (( width == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_WIDTH )) ||
        ( height == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_HEIGHT )))
    {
        const wxSize BEST_SIZE = GetBestSize();
        if ( width == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_WIDTH ))
            width = BEST_SIZE.x;
        if ( height == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_HEIGHT ))
            height = BEST_SIZE.y;
    }
    DoMoveWindow( x, y, width, height );
}


void wxWindow::DoGetClientSize(int *width, int *height) const
{
    QRect geometry = GetHandle()->geometry();
    *width = geometry.width();
    *height = geometry.height();
}

    
void wxWindow::DoSetClientSize(int width, int height)
{
    QWidget *qtWidget = GetHandle();
    QRect geometry = qtWidget->geometry();
    geometry.setWidth( width );
    geometry.setHeight( height );
    qtWidget->setGeometry( geometry );
}

wxSize wxWindow::DoGetBestSize() const
{
    return wxQtConvertSize( GetHandle()->sizeHint() );
}

void wxWindow::DoMoveWindow(int x, int y, int width, int height)
{
    QWidget *qtWidget = GetHandle();

    qtWidget->move( x, y );
    qtWidget->resize( width, height );
}


#if wxUSE_TOOLTIPS
void wxWindow::DoSetToolTip( wxToolTip *tip )
{
    wxWindowBase::DoSetToolTip( tip );
    
    if ( tip != NULL )
        GetHandle()->setToolTip( wxQtConvertString( tip->GetTip() ));
    else
        GetHandle()->setToolTip( QString() );
}
#endif // wxUSE_TOOLTIPS


#if wxUSE_MENUS
bool wxWindow::DoPopupMenu(wxMenu *menu, int x, int y)
{
    menu->GetHandle()->exec( GetHandle()->mapToGlobal( QPoint( x, y ) ) );

    return ( true );
}
#endif // wxUSE_MENUS

#if wxUSE_ACCEL
void wxWindow::SetAcceleratorTable( const wxAcceleratorTable& accel )
{
    wxWindowBase::SetAcceleratorTable( accel );
    
    // Disable previously set accelerators
    while ( !m_qtShortcuts.isEmpty() )
        delete m_qtShortcuts.takeFirst();
    
    // Create new shortcuts (use GetHandle() so all events inside
    // the window are handled, not only in the container subwindow)
    m_qtShortcuts = accel.ConvertShortcutTable( GetHandle() );
    
    // Connect shortcuts to window
    Q_FOREACH( QShortcut *s, m_qtShortcuts )
    {
        QObject::connect( s, SIGNAL( activated() ), m_qtShortcutHandler, SLOT( activated() ) );
        QObject::connect( s, SIGNAL( activatedAmbiguously() ), m_qtShortcutHandler, SLOT( activated() ) );
    }
}
#endif // wxUSE_ACCEL

bool wxWindow::QtHandlePaintEvent ( QWidget *handler, QPaintEvent * WXUNUSED( event ))
{
    /* If this window has scrollbars, only let wx handle the event if it is
     * for the client area (the scrolled part). Events for the whole window
     * (including scrollbars and maybe status or menu bars are handled by Qt */
    
    if ( m_qtContainer && handler != m_qtContainer )
    {
        return false;
    }
    else
    {
        if ( !m_qtPicture->isNull() )
        {
            // Data from wxClientDC, paint it
            QPainter p( GetHandle() );
            p.drawImage( QPoint( 0, 0 ), *m_qtPaintBuffer );
            p.end();
            QtPaintClientDCPicture( handler );

            return true;
        }
        else
        {
            // Real paint event, send it
            if ( m_qtPaintBuffer )
            {
                delete m_qtPaintBuffer;
                m_qtPaintBuffer = 0;
            }
            wxEraseEvent erase;
            ProcessWindowEvent(erase);
            wxPaintEvent paint;
            bool handled = ProcessWindowEvent(paint);

            if ( m_qtPaintBuffer )
            {
                QPainter p( GetHandle() );
                p.drawImage( QPoint( 0, 0 ), *m_qtPaintBuffer );
                p.end();
            }

            return handled;
        }
    }
}

void wxWindow::QtPaintClientDCPicture( QWidget *handler )
{
    if ( m_qtContainer && handler != m_qtContainer )
        return;

    // Paint wxClientDC data
    QPainter painter( handler );
    painter.drawPicture( 0, 0, *m_qtPicture );

    // Reset picture
    m_qtPicture->setData( NULL, 0 );
}

bool wxWindow::QtHandleResizeEvent ( QWidget *WXUNUSED( handler ), QResizeEvent *event )
{
    wxSizeEvent e( wxQtConvertSize( event->size() ) );

    return ProcessWindowEvent( e );
}

bool wxWindow::QtHandleWheelEvent ( QWidget *WXUNUSED( handler ), QWheelEvent *event )
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

static void FillKeyboardModifiers( Qt::KeyboardModifiers modifiers, wxKeyboardState *state )
{
    state->SetControlDown( modifiers.testFlag( Qt::ControlModifier ) );
    state->SetShiftDown( modifiers.testFlag( Qt::ShiftModifier ) );
    state->SetAltDown( modifiers.testFlag( Qt::AltModifier ) );
    state->SetMetaDown( modifiers.testFlag( Qt::MetaModifier ) );
}

bool wxWindow::QtHandleKeyEvent ( QWidget *WXUNUSED( handler ), QKeyEvent *event )
{
#if wxUSE_ACCEL
    if ( m_processingShortcut )
    {
        /* Enter here when a shortcut isn't handled by Qt.
         * Return true to avoid Qt-processing of the event
         * Instead, use the flag to indicate that it wasn't processed */
        m_processingShortcut = false;
        
        return true;
    }
#endif // wxUSE_ACCEL

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
    FillKeyboardModifiers( event->modifiers(), &e );

    handled = ProcessWindowEvent( e );

    // On key presses, send the EVT_CHAR event
    if ( !handled && event->type() == QEvent::KeyPress )
    {
#if wxUSE_ACCEL
        // Check for accelerators
        if ( !m_processingShortcut )
        {
            /* The call to notify() will try to execute a shortcut. If it fails
             * it will call keyPressEvent() in our wxQtWidget which calls back
             * to this function. We use the m_processingShortcut flag to avoid
             * processing that recursive call and return back to this one. */
            m_processingShortcut = true;
            
            QApplication::instance()->notify( GetHandle(), event );
            
            handled = m_processingShortcut;
            m_processingShortcut = false;
            
            if ( handled )
                return true;
        }
#endif // wxUSE_ACCEL

        e.SetEventType( wxEVT_CHAR );

        // Translated key code (including control + letter -> 1-26)
        int translated = 0;
        if ( !event->text().isEmpty() )
            translated = event->text().at( 0 ).toAscii();
        if ( translated )
            e.m_keyCode = translated;
        
        handled = ProcessWindowEvent( e );
    }
    
    return handled;
}

bool wxWindow::QtHandleMouseEvent ( QWidget *handler, QMouseEvent *event )
{
    // Convert event type
    wxEventType wxType = 0;
    switch ( event->type() )
    {
        case QEvent::MouseButtonDblClick:
            switch ( event->button() )
            {
                case Qt::LeftButton:
                    wxType = wxEVT_LEFT_DCLICK;
                    break;
                case Qt::RightButton:
                    wxType = wxEVT_RIGHT_DCLICK;
                    break;
                case Qt::MidButton:
                    wxType = wxEVT_MIDDLE_DCLICK;
                    break;
                case Qt::XButton1:
                    wxType = wxEVT_AUX1_DCLICK;
                    break;
                case Qt::XButton2:
                    wxType = wxEVT_AUX2_DCLICK;
                    break;
                case Qt::NoButton:
                case Qt::MouseButtonMask: // Not documented ?
                    return false;
            }
            break;
        case QEvent::MouseButtonPress:
            switch ( event->button() )
            {
                case Qt::LeftButton:
                    wxType = wxEVT_LEFT_DOWN;
                    break;
                case Qt::RightButton:
                    wxType = wxEVT_RIGHT_DOWN;
                    break;
                case Qt::MidButton:
                    wxType = wxEVT_MIDDLE_DOWN;
                    break;
                case Qt::XButton1:
                    wxType = wxEVT_AUX1_DOWN;
                    break;
                case Qt::XButton2:
                    wxType = wxEVT_AUX2_DOWN;
                case Qt::NoButton:
                case Qt::MouseButtonMask: // Not documented ?
                    return false;
            }
            break;
        case QEvent::MouseButtonRelease:
            switch ( event->button() )
            {
                case Qt::LeftButton:
                    wxType = wxEVT_LEFT_UP;
                    break;
                case Qt::RightButton:
                    wxType = wxEVT_RIGHT_UP;
                    break;
                case Qt::MidButton:
                    wxType = wxEVT_MIDDLE_UP;
                    break;
                case Qt::XButton1:
                    wxType = wxEVT_AUX1_UP;
                    break;
                case Qt::XButton2:
                    wxType = wxEVT_AUX2_UP;
                case Qt::NoButton:
                case Qt::MouseButtonMask: // Not documented ?
                    return false;
            }
            break;
        case QEvent::MouseMove:
            wxType = wxEVT_MOTION;
            break;
        default:
            // Unknown event type
            wxFAIL_MSG( "Unknown mouse event type" );
    }

    // Fill the event
    QPoint mousePos = event->pos();
    wxMouseEvent e( wxType );
    e.m_clickCount = -1;
    e.SetPosition( wxQtConvertPoint( mousePos ) );

    // Mouse buttons
    wxQtFillMouseButtons( event->buttons(), &e );

    // Keyboard modifiers
    FillKeyboardModifiers( event->modifiers(), &e );

    bool handled = ProcessWindowEvent( e );

    // Determine if mouse is inside the widget
    bool mouseInside = true;
    if ( mousePos.x() < 0 || mousePos.x() > handler->width() ||
        mousePos.y() < 0 || mousePos.y() > handler->height() )
        mouseInside = false;
    
    if ( e.GetEventType() == wxEVT_MOTION )
    {
        /* Qt doesn't emit leave/enter events while the mouse is grabbed
        * and it automatically grabs the mouse while dragging. In that cases
        * we emulate the enter and leave events */

        // Mouse enter/leaves
        if ( m_mouseInside != mouseInside )
        {
            if ( mouseInside )
                e.SetEventType( wxEVT_ENTER_WINDOW );
            else
                e.SetEventType( wxEVT_LEAVE_WINDOW );

            ProcessWindowEvent( e );
            m_mouseInside = mouseInside;
        }
    }

    m_mouseInside = mouseInside;
    
    return handled;
}

bool wxWindow::QtHandleEnterEvent ( QWidget *handler, QEvent *event )
{
    wxMouseEvent e( event->type() == QEvent::Enter ? wxEVT_ENTER_WINDOW : wxEVT_LEAVE_WINDOW );
    e.m_clickCount = 0;
    e.SetPosition( wxQtConvertPoint( handler->mapFromGlobal( QCursor::pos() ) ) );
    
    // Mouse buttons
    wxQtFillMouseButtons( QApplication::mouseButtons(), &e );
    
    // Keyboard modifiers
    FillKeyboardModifiers( QApplication::keyboardModifiers(), &e );
    
    return ProcessWindowEvent( e );
}

bool wxWindow::QtHandleMoveEvent ( QWidget *handler, QMoveEvent *event )
{
    if ( GetHandle() != handler )
        return false;
    
    wxMoveEvent e( wxQtConvertPoint( event->pos() ) );

    return ProcessWindowEvent( e );
}

bool wxWindow::QtHandleShowEvent ( QWidget *handler, QEvent *event )
{
    if ( GetHandle() != handler )
        return false;
    
    wxShowEvent e;
    e.SetShow( event->type() == QEvent::Show );

    return ProcessWindowEvent( e );
}

bool wxWindow::QtHandleChangeEvent ( QWidget *handler, QEvent *event )
{
    if ( GetHandle() != handler )
        return false;

    if ( event->type() == QEvent::ActivationChange )
    {
        wxActivateEvent e( wxEVT_ACTIVATE, handler->isActiveWindow() );

        return ProcessWindowEvent( e );
    }
    else
        return false;
}

bool wxWindow::QtHandleCloseEvent ( QWidget *handler, QCloseEvent *WXUNUSED( event ) )
{
    if ( GetHandle() != handler )
        return false;
    
    wxCloseEvent e( wxEVT_CLOSE_WINDOW );

    return ProcessWindowEvent( e );
}

bool wxWindow::QtHandleContextMenuEvent ( QWidget *handler, QContextMenuEvent *event )
{
    wxContextMenuEvent e( wxEVT_CONTEXT_MENU );
    e.SetPosition( wxQtConvertPoint( event->globalPos() ) );
    
    return ProcessWindowEvent( e );
}

bool wxWindow::QtHandleFocusEvent ( QWidget *handler, QFocusEvent *event )
{
    wxFocusEvent e( event->gotFocus() ? wxEVT_SET_FOCUS : wxEVT_KILL_FOCUS );

    bool handled = ProcessWindowEvent( e );

    wxWindow *parent = GetParent();
    if ( event->gotFocus() && parent )
    {
        wxChildFocusEvent childEvent( this );
        parent->ProcessWindowEvent( childEvent );
    }
    
    return handled;
}

#if wxUSE_ACCEL
void wxWindow::QtHandleShortcut ( int command )
{
    if (command != -1)
    {
        wxCommandEvent menu_event( wxEVT_COMMAND_MENU_SELECTED, command );
        bool ret = ProcessWindowEvent( menu_event );
        
        if ( !ret )
        {
            // if the accelerator wasn't handled as menu event, try
            // it as button click (for compatibility with other
            // platforms):
            wxCommandEvent button_event( wxEVT_COMMAND_BUTTON_CLICKED, command );
            ProcessWindowEvent( button_event );
        }
    }
}
#endif // wxUSE_ACCEL

QWidget *wxWindow::GetHandle() const
{
    return m_qtWindow;
}

QWidget *wxWindow::QtGetScrollBarsContainer() const
{
    return m_qtWindow;
}

QPicture *wxWindow::QtGetPicture() const
{
    return m_qtPicture;
}

QImage *wxWindow::QtGetPaintBuffer()
{
    if ( !m_qtPaintBuffer )
    {
        m_qtPaintBuffer = new QImage( wxQtConvertSize( GetClientSize() ),
                                      QImage::Format_ARGB32_Premultiplied );
        m_qtPaintBuffer->fill( GetBackgroundColour().GetHandle().rgb() );
    }

    return m_qtPaintBuffer;
}
