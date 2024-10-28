/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/window.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtGui/QPicture>
#include <QtGui/QPainter>
#include <QtGui/QWindow>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QMainWindow>
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
#include "wx/dnd.h"
#include "wx/tooltip.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/compat.h"
#include "wx/qt/private/winevent.h"


#define TRACE_QT_WINDOW "qtwindow"

namespace
{
inline QWidget* wxQtGetDrawingWidget(QAbstractScrollArea* qtContainer,
                                     QWidget* qtWidget)
{
    if ( qtContainer && qtContainer->viewport() )
        return qtContainer->viewport();

    return qtWidget;
}
}

extern wxSize wxQtGetBestSize(QWidget* qtWidget)
{
    auto size = qtWidget->sizeHint();
    // best effort to ensure a correct size (note that some qt controls
    // implement just one or both size hints)
    size = size.expandedTo(qtWidget->minimumSizeHint());
    return wxQtConvertSize(size);
}


// Base Widget helper (no scrollbar, used by wxWindow)

class wxQtWidget : public wxQtEventSignalHandler< QWidget, wxWindowQt >
{
    public:
        wxQtWidget( wxWindowQt *parent, wxWindowQt *handler );

        virtual QSize sizeHint() const override
        {
            // Make sure the window has a valid initial size because the default size of a
            // generic control (e.g. wxPanel) is (0, 0) when created. Quoting the Qt docs:
            //
            //   "Setting the size to QSize(0, 0) will cause the widget
            //    to not appear on screen. This also applies to windows."
            //
            // The value 20 seems to be the default for wxPanel under wxMSW.
            return QSize(20, 20);
        }

};

wxQtWidget::wxQtWidget( wxWindowQt *parent, wxWindowQt *handler )
    : wxQtEventSignalHandler< QWidget, wxWindowQt >( parent, handler )
{
}

// Scroll Area helper (container to show scroll bars for wxScrolledWindow):

class wxQtScrollArea : public wxQtEventSignalHandler< QScrollArea, wxWindowQt >
{
public:
    wxQtScrollArea(wxWindowQt *parent, wxWindowQt *handler);

    bool event(QEvent *e) override;

    void OnActionTriggered(int action);
    void OnSliderReleased();
};

wxQtScrollArea::wxQtScrollArea( wxWindowQt *parent, wxWindowQt *handler )
    : wxQtEventSignalHandler< QScrollArea, wxWindowQt >( parent, handler )
{
    auto sb = horizontalScrollBar();
    if ( sb )
    {
        connect( sb, &QScrollBar::actionTriggered, this, &wxQtScrollArea::OnActionTriggered );
        connect( sb, &QScrollBar::sliderReleased, this, &wxQtScrollArea::OnSliderReleased );
    }

    sb = verticalScrollBar();

    if ( sb )
    {
        connect( sb, &QScrollBar::actionTriggered, this, &wxQtScrollArea::OnActionTriggered );
        connect( sb, &QScrollBar::sliderReleased, this, &wxQtScrollArea::OnSliderReleased );
    }
}

bool wxQtScrollArea::event(QEvent *e)
{
    wxWindowQt* handler = GetHandler();
    if ( handler && handler->HasCapture() )
    {
        switch ( e->type() )
        {
            case QEvent::MouseButtonRelease:
            case QEvent::MouseButtonDblClick:
            case QEvent::MouseMove:
            case QEvent::Wheel:
            case QEvent::TouchUpdate:
            case QEvent::TouchEnd:
                return viewportEvent(e);
            default:
                break;
        }
    }
    //  QGesture events arrive without mouse capture
    else if ( handler )
    {
        switch ( e->type() )
        {
            case QEvent::Gesture:
            {
                QScrollArea::event(e);

                if ( QScrollBar *vBar = verticalScrollBar() )
                    vBar->triggerAction( QAbstractSlider::SliderMove );
                if ( QScrollBar *hBar = horizontalScrollBar() )
                    hBar->triggerAction( QAbstractSlider::SliderMove );

                return true;
            }

            default:
                break;
        }
    }

    return QScrollArea::event(e);
}

void wxQtScrollArea::OnActionTriggered( int action )
{
    wxEventType eventType = wxEVT_NULL;
    switch( action )
    {
        case QAbstractSlider::SliderSingleStepAdd:
            eventType = wxEVT_SCROLLWIN_LINEDOWN;
            break;
        case QAbstractSlider::SliderSingleStepSub:
            eventType = wxEVT_SCROLLWIN_LINEUP;
            break;
        case QAbstractSlider::SliderPageStepAdd:
            eventType = wxEVT_SCROLLWIN_PAGEDOWN;
            break;
        case QAbstractSlider::SliderPageStepSub:
            eventType = wxEVT_SCROLLWIN_PAGEUP;
            break;
        case QAbstractSlider::SliderToMinimum:
            eventType = wxEVT_SCROLLWIN_TOP;
            break;
        case QAbstractSlider::SliderToMaximum:
            eventType = wxEVT_SCROLLWIN_BOTTOM;
            break;
        case QAbstractSlider::SliderMove:
            eventType = wxEVT_SCROLLWIN_THUMBTRACK;
            break;
        default:
            return;
    }

    if ( GetHandler() )
    {
        auto sb = static_cast<QScrollBar*>(sender());
        if ( sb )
        {
            wxScrollWinEvent e( eventType, sb->sliderPosition(),
                                wxQtConvertOrientation( sb->orientation() ) );
            EmitEvent( e );
        }
    }
}

void wxQtScrollArea::OnSliderReleased()
{
    if ( GetHandler() )
    {
        auto sb = static_cast<QScrollBar*>(sender());
        if ( sb )
        {
            wxScrollWinEvent e( wxEVT_SCROLLWIN_THUMBRELEASE, sb->sliderPosition(),
                                wxQtConvertOrientation( sb->orientation() ) );
            EmitEvent( e );
        }
    }
}

#if wxUSE_ACCEL || defined( Q_MOC_RUN )

class wxQtShortcutHandler : public QObject
{
public:
    explicit wxQtShortcutHandler( wxWindow *handler ) : m_handler(handler) { }

    void activated()
    {
        const int command = sender()->property("wxQt_Command").toInt();

        m_handler->QtHandleShortcut( command );
    }

private:

    wxWindow* const m_handler;
};

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
    variant.setValue( window );
    widget->setProperty( WINDOW_POINTER_PROPERTY_NAME, variant );
}

/* static */ wxWindowQt *wxWindowQt::QtRetrieveWindowPointer( const QWidget *widget )
{
    QVariant variant = widget->property( WINDOW_POINTER_PROPERTY_NAME );
    return const_cast< wxWindowQt * >( ( variant.value< const wxWindow * >() ));
}

/* static */
void wxWindowQt::QtSendSetCursorEvent(wxWindowQt* win, const wxPoint& posClient)
{
    const wxRect rect(win->GetClientAreaOrigin(), win->GetClientSize());

    if ( rect.Contains(posClient) )
    {
        wxSetCursorEvent event( posClient.x , posClient.y );
        event.SetId(win->GetId());
        event.SetEventObject(win);

        const bool processedEvtSetCursor = win->HandleWindowEvent(event);
        if ( processedEvtSetCursor && event.HasCursor() )
        {
            win->SetCursor(event.GetCursor());
        }
        else
        {
            // Notice that GetCursor() can return an invalid cursor even if the window already
            // has a valid cursor set at the Qt level. Don't override it if this is the case.
            const bool hasCursor = win->GetHandle()->testAttribute(Qt::WA_SetCursor);

            if ( !hasCursor || (!wxIsBusy() && !win->GetParent()) )
            {
                win->SetCursor( *wxSTANDARD_CURSOR );
            }
        }
    }
}

static wxWindowQt *s_capturedWindow = nullptr;

/* static */ wxWindowQt *wxWindowBase::DoFindFocus()
{
    wxWindowQt *window = nullptr;
    QWidget *qtWidget = QApplication::focusWidget();
    if ( qtWidget != nullptr )
        window = wxWindowQt::QtRetrieveWindowPointer( qtWidget );

    return window;
}

void wxWindowQt::Init()
{
    m_qtPainter.reset(new QPainter());

    m_mouseInside = false;

#if wxUSE_ACCEL
    m_qtShortcutHandler.reset(new wxQtShortcutHandler(this));
    m_processingShortcut = false;
#endif
    m_qtWindow = nullptr;
    m_qtContainer = nullptr;

    m_pendingClientSize = wxDefaultSize;
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
    if ( !m_qtWindow )
    {
        wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::~wxWindow %s m_qtWindow is null"), GetName());
        return;
    }

    // Delete only if the qt widget was created or assigned to this base class
    wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::~wxWindow %s m_qtWindow=%p"), GetName(), m_qtWindow);

    if ( !IsBeingDeleted() )
    {
        SendDestroyEvent();
    }

    // Avoid processing pending events which quite often would lead to crashes after this.
    QCoreApplication::removePostedEvents(m_qtWindow);

    // Block signals because the handlers access members of a derived class.
    m_qtWindow->blockSignals(true);

    if ( s_capturedWindow == this )
        s_capturedWindow = nullptr;

    DestroyChildren(); // This also destroys scrollbars

    if (m_qtWindow)
        QtStoreWindowPointer( GetHandle(), nullptr );

#if wxUSE_DRAG_AND_DROP
    SetDropTarget(nullptr);
#endif

    delete m_qtWindow;
}


bool wxWindowQt::Create( wxWindowQt * parent, wxWindowID id, const wxPoint & pos,
        const wxSize & size, long style, const wxString &name )
{
    // If the underlying control hasn't been created then this most probably means
    // that a generic control, like wxPanel, is being created, so we need a very
    // simple control as a base:

    bool isGeneric = false;

    if ( GetHandle() == nullptr )
    {
        isGeneric = true;

        if ( style & (wxHSCROLL | wxVSCROLL) )
        {
            m_qtWindow =
            m_qtContainer = new wxQtScrollArea( parent, this );
        }
        else
        {
            m_qtWindow = new wxQtWidget( parent, this );
        }
    }
    else
    {
        m_qtContainer = dynamic_cast<QAbstractScrollArea*>(m_qtWindow);
    }

    if ( m_qtContainer )
    {
        // If wx[HV]SCROLL is not given, the corresponding scrollbar is not shown
        // at all. Otherwise it may be shown only on demand (default) or always, if
        // the wxALWAYS_SHOW_SB is specified.
        Qt::ScrollBarPolicy horzPolicy = (style & wxHSCROLL)
                                          ? HasFlag(wxALWAYS_SHOW_SB)
                                              ? Qt::ScrollBarAlwaysOn
                                              : Qt::ScrollBarAsNeeded
                                          : Qt::ScrollBarAlwaysOff;
        Qt::ScrollBarPolicy vertPolicy = (style & wxVSCROLL)
                                          ? HasFlag(wxALWAYS_SHOW_SB)
                                              ? Qt::ScrollBarAlwaysOn
                                              : Qt::ScrollBarAsNeeded
                                          : Qt::ScrollBarAlwaysOff;

        m_qtContainer->setHorizontalScrollBarPolicy( horzPolicy );
        m_qtContainer->setVerticalScrollBarPolicy( vertPolicy );
    }

    if ( !wxWindowBase::CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
        return false;

    if ( parent )
        parent->AddChild( this );

    wxPoint p;
    if ( pos != wxDefaultPosition )
        p = pos;

    wxSize initialSize = size;
    initialSize.SetDefaults( IsTopLevel() && !GetHandle()->inherits("QDialog")
                                 ? wxTopLevelWindowBase::GetDefaultSize()
                                 : wxQtGetBestSize( GetHandle() ) );

    DoMoveWindow( p.x, p.y, initialSize.GetWidth(), initialSize.GetHeight() );

    PostCreation( isGeneric );

    return true;
}

void wxWindowQt::PostCreation(bool generic)
{
    if ( m_qtWindow == nullptr )
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
    wxWindowBase::SetBackgroundColour(wxColour(GetHandle()->palette().window().color()));
    wxWindowBase::SetForegroundColour(wxColour(GetHandle()->palette().windowText().color()));

    GetHandle()->setFont( wxWindowBase::GetFont().GetHandle() );

    if ( !IsThisEnabled() )
        DoEnable(false);

    // The window might have been hidden before Create() and it needs to remain
    // hidden in this case.
    GetHandle()->setVisible(m_isShown);

    wxWindowCreateEvent event(this);
    HandleWindowEvent(event);
}

void wxWindowQt::AddChild( wxWindowBase *child )
{
    // Make sure all children are children of the inner scroll area widget (if any):

    if ( m_qtContainer )
        QtReparent( child->GetHandle(), m_qtContainer->viewport() );

    wxWindowBase::AddChild( child );
}

bool wxWindowQt::Show( bool show )
{
    if ( !wxWindowBase::Show( show ))
        return false;

    // Show can be called before the underlying window is created:

    if ( QWidget *qtWidget = GetHandle() )
    {
        qtWidget->setVisible( show );
    }

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
    if ( GetHandle() )
        GetHandle()->setEnabled(enable);
}

void wxWindowQt::SetFocus()
{
    if ( !GetHandle()->isActiveWindow() )
        GetHandle()->activateWindow();

    GetHandle()->setFocus();
}

/* static */ void wxWindowQt::QtReparent( QWidget *child, QWidget *parent )
{
    child->setParent( parent, child->windowFlags() );
}

bool wxWindowQt::Reparent( wxWindowBase *parent )
{
    if ( !wxWindowBase::Reparent( parent ))
        return false;

    QtReparent( GetHandle(), static_cast<wxWindow*>(parent)->QtGetParentWidget() );

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

    QWidget* const widget = wxQtGetDrawingWidget(m_qtContainer, GetHandle());

    widget->repaint();
}

void wxWindowQt::Refresh( bool WXUNUSED( eraseBackground ), const wxRect *rect )
{
    QWidget* const widget = wxQtGetDrawingWidget(m_qtContainer, GetHandle());

    if ( widget != nullptr )
    {
        if ( rect != nullptr )
        {
            wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::Refresh %s rect %d %d %d %d"),
                       GetName(),
                       rect->x, rect->y, rect->width, rect->height);
            widget->update( wxQtConvertRect( *rect ));

            wxWindowList& children = GetChildren();
            if ( !children.empty() )
            {
                wxRect parentRect = *rect;
                ClientToScreen(&parentRect.x, &parentRect.y);

                for ( auto childWin : children )
                {
                    wxRect childRect = childWin->GetScreenRect();
                    childRect.Intersect(parentRect);
                    if ( !childRect.IsEmpty() )
                    {
                        childWin->ScreenToClient(&childRect.x, &childRect.y);
                        childWin->RefreshRect(childRect);
                    }
                }
            }
        }
        else
        {
            wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::Refresh %s"), GetName());
            widget->update();

            wxWindowList& children = GetChildren();
            for ( auto childWin : children )
            {
                childWin->Refresh();
            }
        }
    }
}

bool wxWindowQt::SetCursor( const wxCursor &cursor )
{
    if (!wxWindowBase::SetCursor(cursor))
        return false;

    if ( cursor.IsOk() )
        GetHandle()->setCursor(cursor.GetHandle());
    else
        GetHandle()->unsetCursor();

    return true;
}

bool wxWindowQt::SetFont( const wxFont &font )
{
    // SetFont may be called before Create, so the font is stored
    // by the base class, and set in PostCreation

    if (GetHandle())
    {
        GetHandle()->setFont( font.GetHandle() );
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

double wxWindowQt::GetContentScaleFactor() const
{
    if (GetHandle())
    {
        QWidget* npw = GetHandle()->nativeParentWidget();

        if (npw)
        {
            QWindow *win = npw->windowHandle();
            return win->devicePixelRatio();
        }
    }

    return qApp->devicePixelRatio();
}

double wxWindowQt::GetDPIScaleFactor() const
{
    return GetContentScaleFactor();
}

wxSize wxWindowQt::GetDPI() const
{
    return MakeDPIFromScaleFactor(GetDPIScaleFactor());
}

void wxWindowQt::DoGetTextExtent(const wxString& string, int *x, int *y, int *descent,
        int *externalLeading, const wxFont *font ) const
{
    if ( x )
        *x = 0;
    if ( y )
        *y = 0;
    if ( descent )
        *descent = 0;
    if ( externalLeading )
        *externalLeading = 0;

    // We can skip computing the string width and height if it is empty, but
    // not its descent and/or external leading, which still needs to be
    // returned even for an empty string.
    if ( string.empty() && !descent && !externalLeading )
        return;

    QFontMetrics fontMetrics( font != nullptr ? font->GetHandle() : GetHandle()->font() );

    if ( x != nullptr )
        *x = wxQtGetWidthFromMetrics(fontMetrics, wxQtConvertString( string ));

    if ( y != nullptr )
        *y = fontMetrics.height();

    if ( descent != nullptr )
        *descent = fontMetrics.descent();

    if ( externalLeading != nullptr )
        *externalLeading = fontMetrics.lineSpacing();
}

QWidget *wxWindowQt::QtGetClientWidget() const
{
    auto frame = wxDynamicCast(this, wxFrame);
    if ( frame )
        return frame->GetQMainWindow()->centralWidget();

    return wxQtGetDrawingWidget(m_qtContainer, GetHandle());
}

/* Returns a scrollbar for the given orientation */
QScrollBar *wxWindowQt::QtGetScrollBar( int orientation ) const
{
    wxCHECK_MSG( m_qtContainer, nullptr, "Window without scrolling area" );

    if ( orientation == wxHORIZONTAL )
    {
        return m_qtContainer->horizontalScrollBar();
    }

    return m_qtContainer->verticalScrollBar();
}


void wxWindowQt::SetScrollbar( int orientation, int pos, int thumbvisible, int range, bool WXUNUSED(refresh) )
{
    wxCHECK_RET(GetHandle(), "Window has not been created");

    // may return nullptr if the window is not scrollable in that orientation
    // or if it's not scrollable at all.
    QScrollBar *scrollBar = QtGetScrollBar( orientation );

    // Configure the scrollbar if it exists. If range is zero we can get here with
    // scrollBar == nullptr and it is not a problem
    if ( scrollBar )
    {
        scrollBar->setRange( 0, range - thumbvisible );
        scrollBar->setPageStep( thumbvisible );
        {
            wxQtEnsureSignalsBlocked blocker(scrollBar);
            scrollBar->setValue(pos);
        }
        scrollBar->show();

        if ( HasFlag(wxALWAYS_SHOW_SB) && (range == 0) )
        {
            // Disable instead of hide
            scrollBar->setEnabled( false );
        }
        else
            scrollBar->setEnabled( true );
    }

}

void wxWindowQt::SetScrollPos( int orientation, int pos, bool WXUNUSED( refresh ))
{
    QScrollBar *scrollBar = QtGetScrollBar( orientation );
    if ( scrollBar )
        scrollBar->setValue( pos );
}

int wxWindowQt::GetScrollPos( int orientation ) const
{
    QScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );

    return scrollBar->value();
}

int wxWindowQt::GetScrollThumb( int orientation ) const
{
    QScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );

    return scrollBar->pageStep();
}

int wxWindowQt::GetScrollRange( int orientation ) const
{
    QScrollBar *scrollBar = QtGetScrollBar( orientation );
    wxCHECK_MSG( scrollBar, 0, "Invalid scrollbar" );

    return scrollBar->maximum();
}

// scroll window to the specified position
void wxWindowQt::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    // check if this is a scroll area (scroll only inner viewport)
    QWidget* const widget = wxQtGetDrawingWidget(m_qtContainer, GetHandle());

    // scroll the widget or the specified rect (not children)
    if ( rect != nullptr )
        widget->scroll( dx, dy, wxQtConvertRect( *rect ));
    else
        widget->scroll( dx, dy );
}


#if wxUSE_DRAG_AND_DROP
void wxWindowQt::SetDropTarget( wxDropTarget *dropTarget )
{
    if ( m_dropTarget == dropTarget )
        return;

    if ( m_dropTarget != nullptr )
    {
        m_dropTarget->Disconnect();
        delete m_dropTarget;
    }

    m_dropTarget = dropTarget;

    if ( m_dropTarget != nullptr )
    {
        m_dropTarget->ConnectTo(m_qtWindow);
    }
}
#endif

void wxWindowQt::SetWindowStyleFlag( long style )
{
    wxWindowBase::SetWindowStyleFlag( style );

//    wxMISSING_IMPLEMENTATION( "wxWANTS_CHARS, wxTAB_TRAVERSAL" );
//    // wxFULL_REPAINT_ON_RESIZE: Qt::WResizeNoErase (marked obsolete)
//    // wxCLIP_CHILDREN: Used in window for
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

wxSize wxWindowQt::GetWindowBorderSize() const
{
    wxCoord border;
    switch ( GetBorder() )
    {
        case wxBORDER_STATIC:
        case wxBORDER_SIMPLE:
        case wxBORDER_SUNKEN:
        case wxBORDER_RAISED:
        case wxBORDER_THEME:
            border = 1;
            break;

        default:
            wxFAIL_MSG( wxT("unknown border style") );
            wxFALLTHROUGH;

        case wxBORDER_NONE:
            border = 0;
    }

    return 2 * wxSize(border, border);
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
    wxCHECK_RET( GetHandle() != nullptr, wxT("invalid window") );
    s_capturedWindow = this;
}


void wxWindowQt::DoReleaseMouse()
{
    wxCHECK_RET( GetHandle() != nullptr, wxT("invalid window") );
    s_capturedWindow = nullptr;
}

void wxWindowQt::QtReleaseMouseAndNotify()
{
    s_capturedWindow = nullptr;

    while ( auto qtWidget = QWidget::mouseGrabber() )
    {
        qtWidget->releaseMouse();
    }

    NotifyCaptureLost();
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

namespace
{
inline wxSize wxQtSetClientSize(QWidget* qtWidget, int width, int height)
{
    // There doesn't seem to be any way to change Qt frame size directly, so
    // change the widget size, but take into account the extra margins
    // corresponding to the frame decorations.
    const QSize frameSize = qtWidget->frameSize();
    const QSize innerSize = qtWidget->geometry().size();
    const QSize frameSizeDiff = frameSize - innerSize;

    int clientWidth = std::max(width - frameSizeDiff.width(), 0);
    int clientHeight = std::max(height - frameSizeDiff.height(), 0);

    qtWidget->resize(clientWidth, clientHeight);

    return wxSize(clientWidth, clientHeight);
}
}

void wxWindowQt::DoGetSize(int *width, int *height) const
{
    const QSize size =  GetHandle()->frameSize();

    if (width)  *width = size.width();
    if (height) *height = size.height();
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

    if ( !GetHandle()->isVisible() && QtGetClientWidget() != GetHandle() )
    {
        if ( width != -1 && height != -1 )
        {
            m_pendingSize = true;
        }
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
    QWidget *qtWidget = QtGetClientWidget();
    wxCHECK_RET(qtWidget, "window must be created");

    const QSize size = (m_pendingClientSize != wxDefaultSize)
        ? wxQtConvertSize(m_pendingClientSize)
        : qtWidget->geometry().size();

    if (width)  *width = size.width();
    if (height) *height = size.height();
}


void wxWindowQt::DoSetClientSize(int width, int height)
{
    QWidget *qtWidget = QtGetClientWidget();
    wxCHECK_RET( qtWidget, "window must be created" );

    if ( qtWidget != GetHandle() )
    {
        const QSize frameSize = GetHandle()->frameSize();
        const QSize innerSize = GetHandle()->geometry().size();
        const QSize frameSizeDiff = frameSize - innerSize;

        const int clientWidth = width + frameSizeDiff.width();
        const int clientHeight = height + frameSizeDiff.height();

        GetHandle()->resize(clientWidth, clientHeight);
    }

    QRect geometry = qtWidget->geometry();
    geometry.setWidth( width );
    geometry.setHeight( height );
    qtWidget->setGeometry( geometry );
}

void wxWindowQt::DoMoveWindow(int x, int y, int width, int height)
{
    QWidget *qtWidget = GetHandle();

    qtWidget->move( x, y );

    const auto clientSize = wxQtSetClientSize(qtWidget, width, height);

    if (!qtWidget->isVisible() && clientSize.x > 0 && clientSize.y > 0)
    {
        m_pendingClientSize = clientSize;
    }
}

#if wxUSE_TOOLTIPS
void wxWindowQt::QtApplyToolTip(const wxString& text)
{
    GetHandle()->setToolTip(wxQtConvertString(text));
}

void wxWindowQt::DoSetToolTip( wxToolTip *tip )
{
    if ( m_tooltip == tip )
        return;

    wxWindowBase::DoSetToolTip( tip );

    if ( m_tooltip )
        m_tooltip->SetWindow(this);
    else
        QtApplyToolTip(wxString());
}
#endif // wxUSE_TOOLTIPS


#if wxUSE_MENUS
bool wxWindowQt::DoPopupMenu(wxMenu *menu, int x, int y)
{
    menu->UpdateUI();

    QPoint pt;
    if (x == wxDefaultCoord && y == wxDefaultCoord)
        pt = QCursor::pos();
    else
        pt = GetHandle()->mapToGlobal(QPoint(x, y));

    menu->GetHandle()->exec(pt);

    return true;
}
#endif // wxUSE_MENUS

#if wxUSE_ACCEL
void wxWindowQt::SetAcceleratorTable( const wxAcceleratorTable& accel )
{
    wxCHECK_RET(GetHandle(), "Window has not been created");

    wxWindowBase::SetAcceleratorTable( accel );

    // Disable previously set accelerators
    for ( wxVector<QShortcut*>::const_iterator it = m_qtShortcuts.begin();
          it != m_qtShortcuts.end(); ++it )
    {
        delete *it;
    }

    m_qtShortcuts = accel.ConvertShortcutTable(GetHandle());

    // Connect shortcuts to window
    for ( wxVector<QShortcut*>::const_iterator it = m_qtShortcuts.begin();
          it != m_qtShortcuts.end(); ++it )
    {
        QObject::connect( *it, &QShortcut::activated, m_qtShortcutHandler.get(), &wxQtShortcutHandler::activated );
        QObject::connect( *it, &QShortcut::activatedAmbiguously, m_qtShortcutHandler.get(), &wxQtShortcutHandler::activated );
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
    // if it is a scroll area, don't make transparent (invisible) scroll bars:
    QWidget* const widget = wxQtGetDrawingWidget(m_qtContainer, GetHandle());

    // check if the control is created (wxGTK requires calling it before):
    if ( widget != nullptr )
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


namespace
{

void wxQtChangeRoleColour(QPalette::ColorRole role,
                          QWidget* widget,
                          const wxColour& colour)
{
    QPalette palette = widget->palette();
    palette.setColor(role, colour.GetQColor());
    widget->setPalette(palette);
}

} // anonymous namespace

bool wxWindowQt::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxWindowBase::SetBackgroundColour(colour) )
        return false;

    if ( colour.IsOk() )
    {
        QWidget *widget = QtGetParentWidget();
        wxQtChangeRoleColour(widget->backgroundRole(), widget, colour);
    }

    return true;
}

bool wxWindowQt::SetForegroundColour(const wxColour& colour)
{
    if (!wxWindowBase::SetForegroundColour(colour))
        return false;

    QWidget *widget = QtGetParentWidget();
    wxQtChangeRoleColour(widget->foregroundRole(), widget, colour);

    return true;
}

void wxWindowQt::SetDoubleBuffered(bool on)
{
    wxCHECK_RET( GetHandle(), "invalid window" );

    // Quoting from the Qt docs:
    // ---------------------------
    // Indicates that the widget wants to draw directly onto the screen. Widgets
    // with this attribute set do not participate in composition management, i.e.
    // they cannot be semi-transparent or shine through semi-transparent overlapping
    // widgets. Note: This flag is only supported on X11 and it disables double buffering.
    // On Qt for Embedded Linux, the flag only works when set on a top-level widget and it
    // relies on support from the active screen driver. This flag is set or cleared by the
    // widget's author. To render outside of Qt's paint system, e.g., if you require native
    // painting primitives, you need to reimplement QWidget::paintEngine() to return 0 and
    // set this flag.

    GetHandle()->setAttribute(Qt::WA_PaintOnScreen, !on);
}

bool wxWindowQt::IsDoubleBuffered() const
{
    return !GetHandle()->testAttribute(Qt::WA_PaintOnScreen);
}

bool wxWindowQt::QtHandlePaintEvent ( QWidget *handler, QPaintEvent *event )
{
    /* If this window has scrollbars, only let wx handle the event if it is
     * for the client area (the scrolled part). Events for the whole window
     * (including scrollbars and maybe status or menu bars are handled by Qt */

    if ( (m_qtContainer &&
          m_qtContainer != handler) || handler != GetHandle() )
    {
        return false;
    }

    // use the Qt event region:
    m_updateRegion.QtSetRegion( event->region() );

    // Prepare the Qt painter:
    QWidget* const widget = wxQtGetDrawingWidget(m_qtContainer, GetHandle());

    // Start the paint in the viewport if _widget_ is a scroll area, because
    // QAbstractScrollArea can only draw in it. Start the paint in the widget
    // itself otherwise.
    const bool ok = m_qtPainter->begin( widget );

    if (m_qtPainter->device()->depth() > 1)
    {
        m_qtPainter->setRenderHints(QPainter::Antialiasing,
                                    true);
    }

    if ( ok )
    {
        bool handled;

        if ( !m_qtPicture )
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
                        erase.SetEventObject(this);
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
                    wxFALLTHROUGH;
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
            wxPaintEvent paint( this );
            handled = ProcessWindowEvent(paint);
            m_updateRegion.Clear();
        }
        else
        {
            // Data from wxClientDC, paint it
            m_qtPicture->play( m_qtPainter.get() );
            // Reset picture
            m_qtPicture.reset();
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

bool wxWindowQt::QtHandleResizeEvent ( QWidget *WXUNUSED( handler ), QResizeEvent *event )
{
    m_pendingClientSize = wxDefaultSize;

    wxSizeEvent e( wxQtConvertSize( event->size() ) );
    e.SetEventObject(this);

    return ProcessWindowEvent( e );
}

bool wxWindowQt::QtHandleWheelEvent ( QWidget *WXUNUSED( handler ), QWheelEvent *event )
{
    wxMouseEvent e( wxEVT_MOUSEWHEEL );
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QPoint qPt = event->position().toPoint();
    wxMouseWheelAxis wheelAxis = event->angleDelta().y() > 0
                               ? wxMOUSE_WHEEL_VERTICAL : wxMOUSE_WHEEL_HORIZONTAL;
    int wheelRotation = wheelAxis == wxMOUSE_WHEEL_VERTICAL
                      ? (event->angleDelta().y() / 8) : (event->angleDelta().x() / 8);
#else
    QPoint qPt = event->pos();
    wxMouseWheelAxis wheelAxis = event->orientation() == Qt::Vertical
                               ? wxMOUSE_WHEEL_VERTICAL : wxMOUSE_WHEEL_HORIZONTAL;
    int wheelRotation = event->delta();
#endif
    e.SetPosition( wxQtConvertPoint( qPt ) );
    e.SetEventObject(this);

    e.m_wheelAxis = wheelAxis;
    e.m_wheelRotation = wheelRotation;
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
    e.SetEventObject(this);
    // TODO: m_x, m_y
    e.m_keyCode = wxQtConvertKeyCode( event->key(), event->modifiers() );

    if ( event->text().isEmpty() )
        e.m_uniChar = 0;
    else
        e.m_uniChar = event->text().at( 0 ).unicode();

    e.m_rawCode = event->nativeVirtualKey();
    e.m_rawFlags = event->nativeModifiers();
    e.m_isRepeat = event->isAutoRepeat();

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

        // For compatibility with wxMSW, don't generate wxEVT_CHAR event for
        // the following keys: SHIFT, CONTROL, MENU, CAPITAL, NUMLOCK and SCROLL.
        switch ( event->key() )
        {
            case Qt::Key_Shift:
            case Qt::Key_Control:
            case Qt::Key_Meta:
            case Qt::Key_Alt:
            case Qt::Key_AltGr:
            case Qt::Key_CapsLock:
            case Qt::Key_NumLock:
            case Qt::Key_ScrollLock:
                // Skip event generation.
                return handled;
        }

        e.SetEventType( wxEVT_CHAR );
        e.SetEventObject(this);

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
                case Qt::MiddleButton:
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
                case Qt::MiddleButton:
                    wxType = wxEVT_MIDDLE_DOWN;
                    break;
                case Qt::XButton1:
                    wxType = wxEVT_AUX1_DOWN;
                    break;
                case Qt::XButton2:
                    wxType = wxEVT_AUX2_DOWN;
                    break;
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
                case Qt::MiddleButton:
                    wxType = wxEVT_MIDDLE_UP;
                    break;
                case Qt::XButton1:
                    wxType = wxEVT_AUX1_UP;
                    break;
                case Qt::XButton2:
                    wxType = wxEVT_AUX2_UP;
                    break;
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

    // Use screen position as the event might originate from a different
    // Qt window than this one.
    wxPoint mousePos = ScreenToClient(wxQtConvertPoint(event->globalPos()));

    wxMouseEvent e( wxType );
    e.SetEventObject(this);
    e.m_clickCount = -1;
    e.SetPosition(mousePos);

    // Mouse buttons
    wxQtFillMouseButtons( event->buttons(), &e );

    // Keyboard modifiers
    wxQtFillKeyboardModifiers( event->modifiers(), &e );

    bool handled = ProcessWindowEvent( e );

    // Determine if mouse is inside the widget
    bool mouseInside = true;
    if ( mousePos.x < 0 || mousePos.x > handler->width() ||
        mousePos.y < 0 || mousePos.y > handler->height() )
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
        }

        QtSendSetCursorEvent(this, mousePos);
    }

    m_mouseInside = mouseInside;

    return handled;
}

bool wxWindowQt::QtHandleEnterEvent ( QWidget *handler, QEvent *event )
{
    wxMouseEvent e( event->type() == QEvent::Enter ? wxEVT_ENTER_WINDOW : wxEVT_LEAVE_WINDOW );
    e.m_clickCount = 0;
    e.SetPosition( wxQtConvertPoint( handler->mapFromGlobal( QCursor::pos() ) ) );
    e.SetEventObject(this);

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

    wxMoveEvent e( wxQtConvertPoint( event->pos() ), GetId() );
    e.SetEventObject(this);

    return ProcessWindowEvent( e );
}

bool wxWindowQt::QtHandleShowEvent ( QWidget *handler, QEvent *event )
{
    if ( GetHandle() != handler )
        return false;

    if ( m_pendingSize )
    {
        const auto frameSize = GetHandle()->geometry().size();
        wxQtSetClientSize(GetHandle(), frameSize.width(), frameSize.height());

        m_pendingSize = false;
    }

    wxShowEvent e(GetId(), event->type() == QEvent::Show);
    e.SetEventObject(this);

    return ProcessWindowEvent( e );
}

bool wxWindowQt::QtHandleChangeEvent ( QWidget *handler, QEvent *event )
{
    if ( GetHandle() != handler )
        return false;

    if ( event->type() == QEvent::ActivationChange )
    {
        wxActivateEvent e( wxEVT_ACTIVATE, handler->isActiveWindow(), GetId() );
        e.SetEventObject(this);

        return ProcessWindowEvent( e );
    }
    else
        return false;
}

// Returns true if the closing should be vetoed and false if the window should be closed.
bool wxWindowQt::QtHandleCloseEvent ( QWidget *handler, QCloseEvent *WXUNUSED( event ) )
{
    if ( GetHandle() != handler )
        return false;

    // This is required as Qt will still send out close events when the window is disabled.
    if ( !IsEnabled() )
        return true;

    return !Close();
}

bool wxWindowQt::QtHandleContextMenuEvent ( QWidget *WXUNUSED( handler ), QContextMenuEvent *event )
{
    const wxPoint pos =
        event->reason() == QContextMenuEvent::Keyboard
            ? wxDefaultPosition
            : wxQtConvertPoint( event->globalPos() );
    return WXSendContextMenuEvent(pos);
}

bool wxWindowQt::QtHandleFocusEvent ( QWidget *WXUNUSED( handler ), QFocusEvent *event )
{
    wxFocusEvent e( event->gotFocus() ? wxEVT_SET_FOCUS : wxEVT_KILL_FOCUS, GetId() );
    e.SetEventObject(this);
    e.SetWindow(event->gotFocus() ? this : FindFocus());

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
            button_event.SetEventObject(this);
            ProcessWindowEvent( button_event );
        }
    }
}
#endif // wxUSE_ACCEL

QWidget *wxWindowQt::GetHandle() const
{
    return m_qtWindow;
}

void wxWindowQt::QtSetPicture( QPicture* pict )
{
    m_qtPicture.reset(pict);
}

QPainter *wxWindowQt::QtGetPainter()
{
    return m_qtPainter.get();
}

bool wxWindowQt::QtCanPaintWithoutActivePainter() const
{
    return false;
}

bool wxWindowQt::EnableTouchEvents(int eventsMask)
{
    wxCHECK_MSG( GetHandle(), false, "can't be called before creating the window" );

    if ( eventsMask == wxTOUCH_NONE )
    {
        m_qtWindow->setAttribute(Qt::WA_AcceptTouchEvents, false);
        return true;
    }

    if ( eventsMask & wxTOUCH_PRESS_GESTURES )
    {
        m_qtWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
        m_qtWindow->grabGesture(Qt::TapAndHoldGesture);
        QTapAndHoldGesture::setTimeout ( 1000 );
    }
    if ( eventsMask & wxTOUCH_PAN_GESTURES )
    {
        m_qtWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
        m_qtWindow->grabGesture(Qt::PanGesture);
    }
    if ( eventsMask & wxTOUCH_ZOOM_GESTURE )
    {
        m_qtWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
        m_qtWindow->grabGesture(Qt::PinchGesture);
    }

    return true;
}
