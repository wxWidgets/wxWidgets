/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/window.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <QtGui/QPicture>
#include <QtGui/QPainter>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QShortcut>

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/menu.h"
    #include "wx/scrolbar.h"
#endif // WX_PRECOMP

#include "wx/window.h"
#include "wx/tooltip.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"


#define VERT_SCROLLBAR_POSITION 0, 1
#define HORZ_SCROLLBAR_POSITION 1, 0

#define TRACE_QT_WINDOW "qtwindow"

// Base Widget helper (no scrollbar, used by wxWindow)

class wxQtWidget : public wxQtEventSignalHandler< QWidget, wxWindowQt >
{
    public:
        wxQtWidget( wxWindowQt *parent, wxWindowQt *handler );
};

wxQtWidget::wxQtWidget( wxWindowQt *parent, wxWindowQt *handler )
    : wxQtEventSignalHandler< QWidget, wxWindowQt >( parent, handler )
{
}

// Scroll Area helper (container to show scroll bars for wxScrolledWindow):

class wxQtScrollArea : public wxQtEventSignalHandler< QScrollArea, wxWindowQt >
{

    public:
        wxQtScrollArea( wxWindowQt *parent, wxWindowQt *handler );
};

wxQtScrollArea::wxQtScrollArea( wxWindowQt *parent, wxWindowQt *handler )
    : wxQtEventSignalHandler< QScrollArea, wxWindowQt >( parent, handler )
{
}

#if wxUSE_ACCEL || defined( Q_MOC_RUN )
class wxQtShortcutHandler : public QObject, public wxQtSignalHandler< wxWindowQt >
{

public:
    wxQtShortcutHandler( wxWindowQt *window );

public:
    void activated();
};

wxQtShortcutHandler::wxQtShortcutHandler( wxWindowQt *window )
    : wxQtSignalHandler< wxWindowQt >( window )
{
}

void wxQtShortcutHandler::activated()
{
    int command = sender()->property("wxQt_Command").toInt();

    GetHandler()->QtHandleShortcut( command );
}
#endif // wxUSE_ACCEL

//##############################################################################

#ifdef __WXUNIVERSAL__
    wxIMPLEMENT_ABSTRACT_CLASS(wxWindow, wxWindowBase);
#endif // __WXUNIVERSAL__

// We use the QObject property capabilities to store the wxWindow pointer, so we
// don't need to use a separate lookup table. We also want to use it in the proper
// way and not use/store store void pointers e.g.:
// qVariantSetValue( variant, static_cast< void * >( window ));
// static_cast< wxWindow * >( qVariantValue< void * >( variant ));
// so we declare the corresponding Qt meta type:

Q_DECLARE_METATYPE( const wxWindow * )

static const char WINDOW_POINTER_PROPERTY_NAME[] = "wxWindowPointer";

// We accept a 'const wxWindow *' to indicate that the pointer is only stored:

/* static */ void wxWindowQt::QtStoreWindowPointer( QWidget *widget, const wxWindowQt *window )
{
    QVariant variant;
    qVariantSetValue( variant, window );
    widget->setProperty( WINDOW_POINTER_PROPERTY_NAME, variant );
}

/* static */ wxWindowQt *wxWindowQt::QtRetrieveWindowPointer( const QWidget *widget )
{
    QVariant variant = widget->property( WINDOW_POINTER_PROPERTY_NAME );
    return const_cast< wxWindowQt * >( ( variant.value< const wxWindow * >() ));
}




static wxWindowQt *s_capturedWindow = NULL;

/* static */ wxWindowQt *wxWindowBase::DoFindFocus()
{
    wxWindowQt *window = NULL;
    QWidget *qtWidget = QApplication::focusWidget();
    if ( qtWidget != NULL )
        window = wxWindowQt::QtRetrieveWindowPointer( qtWidget );

    return window;
}

void wxWindowQt::Init()
{
    m_horzScrollBar = NULL;
    m_vertScrollBar = NULL;

    m_qtPicture = new QPicture();
    m_qtPainter = new QPainter();

    m_mouseInside = false;

#if wxUSE_ACCEL
    m_qtShortcutHandler = new wxQtShortcutHandler( this );
    m_processingShortcut = false;
    m_qtShortcuts = NULL;
#endif
    m_qtWindow = NULL;
    m_qtContainer = NULL;
}

wxWindowQt::wxWindowQt()
{
    Init();

}


wxWindowQt::wxWindowQt(wxWindowQt *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name)
{
    Init();

    Create( parent, id, pos, size, style, name );
}


wxWindowQt::~wxWindowQt()
{
    SendDestroyEvent();

    if ( s_capturedWindow == this )
        s_capturedWindow = NULL;

    DestroyChildren(); // This also destroys scrollbars

    delete m_qtPicture;
    delete m_qtPainter;

#if wxUSE_ACCEL
    m_qtShortcutHandler->deleteLater();
    delete m_qtShortcuts;
#endif

    // Delete only if the qt widget was created or assigned to this base class
    if (m_qtWindow)
    {
        wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::~wxWindow %s m_qtWindow=%p"), GetName(), m_qtWindow);
        // Avoid sending further signals (i.e. if deleting the current page)
        m_qtWindow->blockSignals(true);
        // Reset the pointer to avoid handling pending event and signals
        QtStoreWindowPointer( GetHandle(), NULL );
        // Delete QWidget when control return to event loop (safer)
        m_qtWindow->deleteLater();
        m_qtWindow = NULL;
    }
    else
    {
        wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::~wxWindow %s m_qtWindow is NULL"), GetName());
    }
}


bool wxWindowQt::Create( wxWindowQt * parent, wxWindowID id, const wxPoint & pos,
        const wxSize & size, long style, const wxString &name )
{
    // If the underlying control hasn't been created then this most probably means
    // that a generic control, like wxPanel, is being created, so we need a very
    // simple control as a base:

    if ( GetHandle() == NULL )
    {
        if ( style & (wxHSCROLL | wxVSCROLL) )
        {
            m_qtContainer = new wxQtScrollArea( parent, this );
            m_qtWindow = m_qtContainer;
            // Create the scroll bars if needed:
            if ( style & wxHSCROLL )
                QtSetScrollBar( wxHORIZONTAL );
            if ( style & wxVSCROLL )
                QtSetScrollBar( wxVERTICAL );
        }
        else
            m_qtWindow = new wxQtWidget( parent, this );
    }

    
    GetHandle()->setMouseTracking(true);
    if ( !wxWindowBase::CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
        return false;

    parent->AddChild( this );

    wxPoint p;
    if ( pos != wxDefaultPosition )
        p = pos;

    DoMoveWindow( p.x, p.y, size.GetWidth(), size.GetHeight() );

    PostCreation();

    return ( true );
}

void wxWindowQt::PostCreation(bool generic)
{
    if ( m_qtWindow == NULL )
    {
        // store pointer to the QWidget subclass (to be used in the destructor)
        m_qtWindow = GetHandle();
    }
    wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::Create %s m_qtWindow=%p"), GetName(), m_qtWindow);

    // set the background style after creation (not before like in wxGTK)
    // (only for generic controls, to use qt defaults elsewere)
    if (generic)
        QtSetBackgroundStyle();
    else
        SetBackgroundStyle(wxBG_STYLE_SYSTEM);

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

    // Set the default color so Paint Event default handler clears the DC:
    SetBackgroundColour(wxColour(GetHandle()->palette().background().color()));
    SetForegroundColour(wxColour(GetHandle()->palette().foreground().color()));

    GetHandle()->setFont( wxWindowBase::GetFont().GetHandle() );
}

void wxWindowQt::AddChild( wxWindowBase *child )
{
    // Make sure all children are children of the inner scroll area widget (if any):

    if ( QtGetScrollBarsContainer() )
        QtReparent( child->GetHandle(), QtGetScrollBarsContainer()->viewport() );

    wxWindowBase::AddChild( child );
}

bool wxWindowQt::Show( bool show )
{
    if ( !wxWindowBase::Show( show ))
        return false;

    // Show can be called before the underlying window is created:

    QWidget *qtWidget = GetHandle();
    if ( qtWidget == NULL )
    {
        return false;
    }

    qtWidget->setVisible( show );

    wxSizeEvent event(GetSize(), GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);

    return true;
}


void wxWindowQt::SetLabel(const wxString& label)
{
    GetHandle()->setWindowTitle( wxQtConvertString( label ));
}


wxString wxWindowQt::GetLabel() const
{
    return ( wxQtConvertString( GetHandle()->windowTitle() ));
}

void wxWindowQt::DoEnable(bool enable)
{
    GetHandle()->setEnabled(enable);
}

void wxWindowQt::SetFocus()
{
    GetHandle()->setFocus();
}

/* static */ void wxWindowQt::QtReparent( QWidget *child, QWidget *parent )
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

bool wxWindowQt::Reparent( wxWindowBase *parent )
{
    if ( !wxWindowBase::Reparent( parent ))
        return false;

    QtReparent( GetHandle(), parent->GetHandle() );

    return true;
}


void wxWindowQt::Raise()
{
    GetHandle()->raise();
}

void wxWindowQt::Lower()
{
    GetHandle()->lower();
}


void wxWindowQt::WarpPointer(int x, int y)
{
    // QCursor::setPos takes global screen coordinates, so translate it:

    ClientToScreen( &x, &y );
    QCursor::setPos( x, y );
}

void wxWindowQt::Update()
{
    wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::Update %s"), GetName());
    // send the paint event to the inner widget in scroll areas:
    if ( QtGetScrollBarsContainer() )
    {
        QtGetScrollBarsContainer()->viewport()->update();
    } else {
        GetHandle()->update();
    }
}

void wxWindowQt::Refresh( bool WXUNUSED( eraseBackground ), const wxRect *rect )
{
    QWidget *widget;

    // get the inner widget in scroll areas:
    if ( QtGetScrollBarsContainer() )
    {
        widget = QtGetScrollBarsContainer()->viewport();
    } else {
        widget = GetHandle();
    }

    if ( widget != NULL )
    {
        if ( rect != NULL )
        {
            wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::Refresh %s rect %d %d %d %d"),
                       GetName(),
                       rect->x, rect->y, rect->width, rect->height);
            widget->update( wxQtConvertRect( *rect ));
        }
        else
        {
            wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::Refresh %s"), GetName());
            widget->update();
        }
    }
}

bool wxWindowQt::SetCursor( const wxCursor &cursor )
{
    if (!wxWindowBase::SetCursor(cursor))
        return false;

    GetHandle()->setCursor(cursor.GetHandle());
    
    return true;
}

bool wxWindowQt::SetFont( const wxFont &font )
{
    // SetFont may be called before Create, so the font is stored
    // by the base class, and set in PostCreation

    if (GetHandle())
    {
        GetHandle()->setFont( font.GetHandle() );
        return true;
    }

    return wxWindowBase::SetFont(font);
}


int wxWindowQt::GetCharHeight() const
{
    return ( GetHandle()->fontMetrics().height() );
}


int wxWindowQt::GetCharWidth() const
{
    return ( GetHandle()->fontMetrics().averageCharWidth() );
}

void wxWindowQt::DoGetTextExtent(const wxString& string, int *x, int *y, int *descent,
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
wxScrollBar *wxWindowQt::QtGetScrollBar( int orientation ) const
{
    wxCHECK_MSG( CanScroll( orientation ), NULL, "Window can't scroll in that orientation" );

    wxScrollBar *scrollBar = NULL;

    if ( orientation == wxHORIZONTAL )
        scrollBar = m_horzScrollBar;
    else
        scrollBar = m_vertScrollBar;

    return scrollBar;
}

/* Returns a new scrollbar for the given orientation, or set the scrollbar
 * passed as parameter */
wxScrollBar *wxWindowQt::QtSetScrollBar( int orientation, wxScrollBar *scrollBar )
{
    QScrollArea *scrollArea = QtGetScrollBarsContainer();
    wxCHECK_MSG( scrollArea, NULL, "Window without scrolling area" );

    // Create a new scrollbar if needed
    if ( !scrollBar )
    {
        scrollBar = new wxScrollBar( const_cast< wxWindowQt* >( this ), wxID_ANY,
                                     wxDefaultPosition, wxDefaultSize,
                                     orientation == wxHORIZONTAL ? wxSB_HORIZONTAL : wxSB_VERTICAL);

        // Connect scrollbar events to this window
        scrollBar->Bind( wxEVT_SCROLL_LINEUP, &wxWindowQt::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_LINEDOWN, &wxWindowQt::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_PAGEUP, &wxWindowQt::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_PAGEDOWN, &wxWindowQt::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_TOP, &wxWindowQt::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_BOTTOM, &wxWindowQt::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_THUMBTRACK, &wxWindowQt::QtOnScrollBarEvent, this );
        scrollBar->Bind( wxEVT_SCROLL_THUMBRELEASE, &wxWindowQt::QtOnScrollBarEvent, this );
    }

    // Let Qt handle layout
    if ( orientation == wxHORIZONTAL )
    {
        scrollArea->setHorizontalScrollBar( scrollBar->GetQScrollBar() );
        m_horzScrollBar = scrollBar;
    }
    else
    {
        scrollArea->setVerticalScrollBar( scrollBar->GetQScrollBar() );
        m_vertScrollBar = scrollBar;
    }
    return scrollBar;
}


void wxWindowQt::SetScrollbar( int orientation, int pos, int thumbvisible, int range, bool refresh )
{
    wxCHECK_RET( CanScroll( orientation ), "Window can't scroll in that orientation" );

    //If not exist, create the scrollbar
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    if ( scrollBar == NULL )
        scrollBar = QtSetScrollBar( orientation );

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

void wxWindowQt::SetScrollPos( int orientation, int pos, bool WXUNUSED( refresh ))
{
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    if ( scrollBar )
        scrollBar->SetThumbPosition( pos );
}

int wxWindowQt::GetScrollPos( int orientation ) const
{
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );

    return scrollBar->GetThumbPosition();
}

int wxWindowQt::GetScrollThumb( int orientation ) const
{
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );

    return scrollBar->GetThumbSize();
}

int wxWindowQt::GetScrollRange( int orientation ) const
{
    wxScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );

    return scrollBar->GetRange();
}

// Handle event from scrollbars
void wxWindowQt::QtOnScrollBarEvent( wxScrollEvent& event )
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
        wxScrollWinEvent e( windowEventType, event.GetPosition(), event.GetOrientation() );
        ProcessWindowEvent( e );
    }
}

// scroll window to the specified position
void wxWindowQt::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    // check if this is a scroll area (scroll only inner viewport)
    QWidget *widget;
    if ( QtGetScrollBarsContainer() )
        widget = QtGetScrollBarsContainer()->viewport();
    else
        widget = GetHandle();
    // scroll the widget or the specified rect (not children)
    if ( rect != NULL )
        widget->scroll( dx, dy, wxQtConvertRect( *rect ));
    else
        widget->scroll( dx, dy );
}


#if wxUSE_DRAG_AND_DROP
void wxWindowQt::SetDropTarget( wxDropTarget * WXUNUSED( dropTarget ) )
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}
#endif

void wxWindowQt::SetWindowStyleFlag( long style )
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

    if ( !GetHandle() )
        return;

    Qt::WindowFlags qtFlags = GetHandle()->windowFlags();

    if ( HasFlag( wxFRAME_NO_TASKBAR ) )
    {
//        qtFlags &= ~Qt::WindowType_Mask;
        if ( (style & wxSIMPLE_BORDER) || (style & wxNO_BORDER) ) {
            qtFlags = Qt::ToolTip | Qt::FramelessWindowHint;
        }
        else
            qtFlags |= Qt::Dialog;
    }
    else
    if ( ( (style & wxSIMPLE_BORDER) || (style & wxNO_BORDER) )
         != qtFlags.testFlag( Qt::FramelessWindowHint ) )
    {
        qtFlags ^= Qt::FramelessWindowHint;
    }

    GetHandle()->setWindowFlags( qtFlags );
}

void wxWindowQt::SetExtraStyle( long exStyle )
{
    long exStyleOld = GetExtraStyle();
    if ( exStyle == exStyleOld )
        return;

    // update the internal variable
    wxWindowBase::SetExtraStyle(exStyle);

    if (!m_qtWindow)
        return;

    Qt::WindowFlags flags = m_qtWindow->windowFlags();

    if (!(exStyle & wxWS_EX_CONTEXTHELP) != !(flags & Qt::WindowContextHelpButtonHint))
    {
        flags ^= Qt::WindowContextHelpButtonHint;
        m_qtWindow->setWindowFlags(flags);
    }
}



void wxWindowQt::DoClientToScreen( int *x, int *y ) const
{
    QPoint screenPosition = GetHandle()->mapToGlobal( QPoint( *x, *y ));
    *x = screenPosition.x();
    *y = screenPosition.y();
}


void wxWindowQt::DoScreenToClient( int *x, int *y ) const
{
    QPoint clientPosition = GetHandle()->mapFromGlobal( QPoint( *x, *y ));
    *x = clientPosition.x();
    *y = clientPosition.y();
}


void wxWindowQt::DoCaptureMouse()
{
    wxCHECK_RET( GetHandle() != NULL, wxT("invalid window") );
    GetHandle()->grabMouse();
    s_capturedWindow = this;
}


void wxWindowQt::DoReleaseMouse()
{
    wxCHECK_RET( GetHandle() != NULL, wxT("invalid window") );
    GetHandle()->releaseMouse();
    s_capturedWindow = NULL;
}

wxWindowQt *wxWindowBase::GetCapture()
{
    return s_capturedWindow;
}


void wxWindowQt::DoGetPosition(int *x, int *y) const
{
    QWidget *qtWidget = GetHandle();
    *x = qtWidget->x();
    *y = qtWidget->y();
}


void wxWindowQt::DoGetSize(int *width, int *height) const
{
    QSize size = GetHandle()->frameSize();
    QRect rect = GetHandle()->frameGeometry();
    wxASSERT( size.width() == rect.width() );
    wxASSERT( size.height() == rect.height() );

    if (width)  *width = rect.width();
    if (height) *height = rect.height();
}



void wxWindowQt::DoSetSize(int x, int y, int width, int height, int sizeFlags )
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

    int w, h;
    GetSize(&w, &h);
    if (width == -1)
        width = w;
    if (height == -1)
        height = h;

    DoMoveWindow( x, y, width, height );

    // An annoying feature of Qt
    // if a control is created with size of zero, it is set as hidden by qt
    // if it is then resized, in some cases it remains hidden, so it
    // needs to be shown here
    if (m_qtWindow && !m_qtWindow->isVisible() && IsShown())
        m_qtWindow->show();
}


void wxWindowQt::DoGetClientSize(int *width, int *height) const
{
    QRect geometry = GetHandle()->geometry();
    if (width)  *width = geometry.width();
    if (height) *height = geometry.height();
}


void wxWindowQt::DoSetClientSize(int width, int height)
{
    QWidget *qtWidget = GetHandle();
    QRect geometry = qtWidget->geometry();
    geometry.setWidth( width );
    geometry.setHeight( height );
    qtWidget->setGeometry( geometry );
}

void wxWindowQt::DoMoveWindow(int x, int y, int width, int height)
{
    QWidget *qtWidget = GetHandle();

    qtWidget->move( x, y );
    qtWidget->resize( width, height );
}


#if wxUSE_TOOLTIPS
void wxWindowQt::DoSetToolTip( wxToolTip *tip )
{
    wxWindowBase::DoSetToolTip( tip );

    if ( tip != NULL )
        GetHandle()->setToolTip( wxQtConvertString( tip->GetTip() ));
    else
        GetHandle()->setToolTip( QString() );
}
#endif // wxUSE_TOOLTIPS


#if wxUSE_MENUS
bool wxWindowQt::DoPopupMenu(wxMenu *menu, int x, int y)
{
    menu->GetHandle()->exec( GetHandle()->mapToGlobal( QPoint( x, y ) ) );

    return ( true );
}
#endif // wxUSE_MENUS

#if wxUSE_ACCEL
void wxWindowQt::SetAcceleratorTable( const wxAcceleratorTable& accel )
{
    wxWindowBase::SetAcceleratorTable( accel );

    // Disable previously set accelerators
    while ( !m_qtShortcuts->isEmpty() )
        delete m_qtShortcuts->takeFirst();

    // Create new shortcuts (use GetHandle() so all events inside
    // the window are handled, not only in the container subwindow)
    delete m_qtShortcuts;
    m_qtShortcuts = accel.ConvertShortcutTable( GetHandle() );

    // Connect shortcuts to window
    Q_FOREACH( QShortcut *s, *m_qtShortcuts )
    {
        QObject::connect( s, &QShortcut::activated, m_qtShortcutHandler, &wxQtShortcutHandler::activated );
        QObject::connect( s, &QShortcut::activatedAmbiguously, m_qtShortcutHandler, &wxQtShortcutHandler::activated );
    }
}
#endif // wxUSE_ACCEL

bool wxWindowQt::SetBackgroundStyle(wxBackgroundStyle style)
{
    if (!wxWindowBase::SetBackgroundStyle(style))
        return false;
    // NOTE this could be called before creation, so it will be delayed:
    return QtSetBackgroundStyle();
}

bool wxWindowQt::QtSetBackgroundStyle()
{
    QWidget *widget;
    // if it is a scroll area, don't make transparent (invisible) scroll bars:
    if ( QtGetScrollBarsContainer() )
        widget = QtGetScrollBarsContainer()->viewport();
    else
        widget = GetHandle();
    // check if the control is created (wxGTK requires calling it before):
    if ( widget != NULL )
    {
        if (m_backgroundStyle == wxBG_STYLE_PAINT)
        {
            // wx paint handler will draw the invalidated region completely:
            widget->setAttribute(Qt::WA_OpaquePaintEvent);
        }
        else if (m_backgroundStyle == wxBG_STYLE_TRANSPARENT)
        {
            widget->setAttribute(Qt::WA_TranslucentBackground);
            // avoid a default background color (usually black):
            widget->setStyleSheet("background:transparent;");
        }
        else if (m_backgroundStyle == wxBG_STYLE_SYSTEM)
        {
            // let Qt erase the background by default
            // (note that wxClientDC will not work)
            //widget->setAutoFillBackground(true);
            // use system colors for background (default in Qt)
            widget->setAttribute(Qt::WA_NoSystemBackground, false);
        }
        else if (m_backgroundStyle == wxBG_STYLE_ERASE)
        {
            // erase events will be fired, if not handled, wx will clear the DC
            widget->setAttribute(Qt::WA_OpaquePaintEvent);
            // Qt should not clear the background (default):
            widget->setAutoFillBackground(false);
        }
    }
    return true;
}


bool wxWindowQt::IsTransparentBackgroundSupported(wxString* WXUNUSED(reason)) const
{
    return true;
}

bool wxWindowQt::SetTransparent(wxByte alpha)
{
    // For Qt, range is between 1 (opaque) and 0 (transparent)
    GetHandle()->setWindowOpacity(alpha/255.0);
    return true;
}


bool wxWindowQt::QtHandlePaintEvent ( QWidget *handler, QPaintEvent *event )
{
    /* If this window has scrollbars, only let wx handle the event if it is
     * for the client area (the scrolled part). Events for the whole window
     * (including scrollbars and maybe status or menu bars are handled by Qt */

    if ( QtGetScrollBarsContainer() && handler != QtGetScrollBarsContainer() )
    {
        return false;
    }
    else
    {
        // use the Qt event region:
        m_updateRegion.QtSetRegion( event->region() );

        // Prepare the Qt painter for wxWindowDC:
        bool ok = false;
        if ( QtGetScrollBarsContainer() )
        {
            // QScrollArea can only draw in the viewport:
            ok = m_qtPainter->begin( QtGetScrollBarsContainer()->viewport() );
        }
        if ( !ok )
        {
            // Start the paint in the widget itself
            ok =  m_qtPainter->begin( GetHandle() );
        }

        if ( ok )
        {
            bool handled;

            if ( m_qtPicture->isNull() )
            {
                // Real paint event (not for wxClientDC), prepare the background
                switch ( GetBackgroundStyle() )
                {
                    case wxBG_STYLE_TRANSPARENT:
                        if (IsTransparentBackgroundSupported())
                        {
                            // Set a transparent background, so that overlaying in parent
                            // might indeed let see through where this child did not
                            // explicitly paint. See wxBG_STYLE_SYSTEM for more comment
                        }
                        break;
                    case wxBG_STYLE_ERASE:
                        {
                            // the background should be cleared by qt auto fill
                            // send the erase event (properly creating a DC for it)
                            wxPaintDC dc( this );
                            dc.SetDeviceClippingRegion( m_updateRegion );

                            wxEraseEvent erase( GetId(), &dc );
                            if ( ProcessWindowEvent(erase) )
                            {
                                // background erased, don't do it again
                                break;
                            }
                            // Ensure DC is cleared if handler didn't and Qt will not do it
                            if ( UseBgCol() && !GetHandle()->autoFillBackground() )
                            {
                                wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::QtHandlePaintEvent %s clearing DC to %s"),
                                           GetName(), GetBackgroundColour().GetAsString()
                                           );
                                dc.SetBackground(GetBackgroundColour());
                                dc.Clear();
                            }
                        }
                        // fall through
                    case wxBG_STYLE_SYSTEM:
                        if ( GetThemeEnabled() )
                        {
                            // let qt render the background:
                            // commented out as this will cause recursive painting
                            // this should be done outside using setBackgroundRole
                            // setAutoFillBackground or setAttribute
                            //wxWindowDC dc( (wxWindow*)this );
                            //widget->render(m_qtPainter);
                        }
                        break;
                    case wxBG_STYLE_PAINT:
                        // nothing to do: window will be painted over in EVT_PAINT
                        break;

                    case wxBG_STYLE_COLOUR:
                        wxFAIL_MSG( "unsupported background style" );
                }

                // send the paint event (wxWindowDC will draw directly):
                wxPaintEvent paint( GetId() );
                paint.SetEventObject(this);
                handled = ProcessWindowEvent(paint);
                m_updateRegion.Clear();
            }
            else
            {
                // Data from wxClientDC, paint it
                m_qtPicture->play( m_qtPainter );
                // Reset picture
                m_qtPicture->setData( NULL, 0 );
                handled = true;
            }

            // commit changes of the painter to the widget
            m_qtPainter->end();

            return handled;
        }
        else
        {
            // Painter didn't begun, not handled by wxWidgets:
            wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::QtHandlePaintEvent %s Qt widget painter begin failed"), GetName() );
            return false;
        }
    }
}

bool wxWindowQt::QtHandleResizeEvent ( QWidget *WXUNUSED( handler ), QResizeEvent *event )
{
    wxSizeEvent e( wxQtConvertSize( event->size() ) );

    return ProcessWindowEvent( e );
}

bool wxWindowQt::QtHandleWheelEvent ( QWidget *WXUNUSED( handler ), QWheelEvent *event )
{
    wxMouseEvent e( wxEVT_MOUSEWHEEL );
    e.SetPosition( wxQtConvertPoint( event->pos() ) );

    e.m_wheelAxis = ( event->orientation() == Qt::Vertical ) ? wxMOUSE_WHEEL_VERTICAL : wxMOUSE_WHEEL_HORIZONTAL;
    e.m_wheelRotation = event->delta();
    e.m_linesPerAction = 3;
    e.m_wheelDelta = 120;

    return ProcessWindowEvent( e );
}


bool wxWindowQt::QtHandleKeyEvent ( QWidget *WXUNUSED( handler ), QKeyEvent *event )
{
    // qt sends keyup and keydown events for autorepeat, but this is not
    // normal for wx which only sends repeated keydown events
    // discard repeated keyup events
    if ( event->isAutoRepeat() && event->type() == QEvent::KeyRelease )
        return true;

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
    e.m_keyCode = wxQtConvertKeyCode( event->key(), event->modifiers() );

#if wxUSE_UNICODE
    if ( event->text().isEmpty() )
        e.m_uniChar = 0;
    else
        e.m_uniChar = event->text().at( 0 ).unicode();
#endif // wxUSE_UNICODE

    e.m_rawCode = event->nativeVirtualKey();
    e.m_rawFlags = event->nativeModifiers();

    // Modifiers
    wxQtFillKeyboardModifiers( event->modifiers(), &e );

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
            translated = event->text().at( 0 ).toLatin1();
        if ( translated )
            e.m_keyCode = translated;

        handled = ProcessWindowEvent( e );
    }

    return handled;
}

bool wxWindowQt::QtHandleMouseEvent ( QWidget *handler, QMouseEvent *event )
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
                default:
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
                default:
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
                default:
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
    wxQtFillKeyboardModifiers( event->modifiers(), &e );

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

bool wxWindowQt::QtHandleEnterEvent ( QWidget *handler, QEvent *event )
{
    wxMouseEvent e( event->type() == QEvent::Enter ? wxEVT_ENTER_WINDOW : wxEVT_LEAVE_WINDOW );
    e.m_clickCount = 0;
    e.SetPosition( wxQtConvertPoint( handler->mapFromGlobal( QCursor::pos() ) ) );

    // Mouse buttons
    wxQtFillMouseButtons( QApplication::mouseButtons(), &e );

    // Keyboard modifiers
    wxQtFillKeyboardModifiers( QApplication::keyboardModifiers(), &e );

    return ProcessWindowEvent( e );
}

bool wxWindowQt::QtHandleMoveEvent ( QWidget *handler, QMoveEvent *event )
{
    if ( GetHandle() != handler )
        return false;

    wxMoveEvent e( wxQtConvertPoint( event->pos() ) );

    return ProcessWindowEvent( e );
}

bool wxWindowQt::QtHandleShowEvent ( QWidget *handler, QEvent *event )
{
    if ( GetHandle() != handler )
        return false;

    wxShowEvent e;
    e.SetShow( event->type() == QEvent::Show );

    return ProcessWindowEvent( e );
}

bool wxWindowQt::QtHandleChangeEvent ( QWidget *handler, QEvent *event )
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

bool wxWindowQt::QtHandleCloseEvent ( QWidget *handler, QCloseEvent *WXUNUSED( event ) )
{
    if ( GetHandle() != handler )
        return false;

    return Close();
}

bool wxWindowQt::QtHandleContextMenuEvent ( QWidget *WXUNUSED( handler ), QContextMenuEvent *event )
{
    wxContextMenuEvent e( wxEVT_CONTEXT_MENU );
    e.SetPosition( wxQtConvertPoint( event->globalPos() ) );

    return ProcessWindowEvent( e );
}

bool wxWindowQt::QtHandleFocusEvent ( QWidget *WXUNUSED( handler ), QFocusEvent *event )
{
    wxFocusEvent e( event->gotFocus() ? wxEVT_SET_FOCUS : wxEVT_KILL_FOCUS );

    bool handled = ProcessWindowEvent( e );

    wxWindowQt *parent = GetParent();
    if ( event->gotFocus() && parent )
    {
        wxChildFocusEvent childEvent( this );
        parent->ProcessWindowEvent( childEvent );
    }

    return handled;
}

#if wxUSE_ACCEL
void wxWindowQt::QtHandleShortcut ( int command )
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

QWidget *wxWindowQt::GetHandle() const
{
    return m_qtWindow;
}

QScrollArea *wxWindowQt::QtGetScrollBarsContainer() const
{
    return m_qtContainer;
}

QPicture *wxWindowQt::QtGetPicture() const
{
    return m_qtPicture;
}

QPainter *wxWindowQt::QtGetPainter()
{
    return m_qtPainter;
}
