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
            qtParent = parent->GetContainer();
            parent->AddChild(this);
        }
        
        m_qtWindow = new wxQtWidget(this, qtParent);
        
    }

    // Create layout for built-in scrolling bars
    if ( GetScrollBarsContainer() )
    {
        QGridLayout *scrollLayout = new QGridLayout();
        scrollLayout->setContentsMargins( 0, 0, 0, 0 );
        GetScrollBarsContainer()->setLayout( scrollLayout );

        // Container at top-left
        // Scrollbars are lazily initialized
        m_qtContainer = new wxQtWidget( this, GetHandle() );
        scrollLayout->addWidget( m_qtContainer, 0, 0 );
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
QScrollBar *wxWindow::GetScrollBar( int orient, bool create ) const
{
    wxCHECK_MSG( GetScrollBarsContainer(), NULL, "This window can't have scrollbars" );
    
    QLayoutItem *qtLayoutItem = NULL;
    QScrollBar *qtScrollBar = NULL;
    
    QGridLayout *scrollLayout = qobject_cast< QGridLayout* >( GetScrollBarsContainer()->layout() );
    wxCHECK_MSG( scrollLayout, NULL, "Window without scrolling layout" );
    
    if ( orient == wxHORIZONTAL )
        qtLayoutItem = scrollLayout->itemAtPosition( HORZ_SCROLLBAR_POSITION );
    else
        qtLayoutItem = scrollLayout->itemAtPosition( VERT_SCROLLBAR_POSITION );
    
    if ( !qtLayoutItem )
    {
        // No item at that position, create scrollbar
        if (create )
        {
            qtScrollBar = new wxQtScrollBarEventForwarder( (wxWindow*)this,
                orient == wxHORIZONTAL ? Qt::Horizontal : Qt:: Vertical );

            if ( orient == wxHORIZONTAL )
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

void wxWindow::SetScrollbar( int orient, int pos, int thumbvisible, int range, bool refresh )
{
    //If range is zero, don't create the scrollbar
    QScrollBar *qtScrollBar = GetScrollBar( orient, range != 0 );
    
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

void wxWindow::SetScrollPos( int orient, int pos, bool refresh )
{
    QScrollBar *qtScrollBar = GetScrollBar( orient );
    wxCHECK_RET( qtScrollBar, "Invalid scrollbar" );

    qtScrollBar->setValue( pos );
}

int wxWindow::GetScrollPos( int orient ) const
{
    QScrollBar *qtScrollBar = GetScrollBar( orient );
    wxCHECK_MSG( qtScrollBar, 0, "Invalid scrollbar" );
    
    return qtScrollBar->value();
}

int wxWindow::GetScrollThumb( int orient ) const
{
    QScrollBar *qtScrollBar = GetScrollBar( orient );
    wxCHECK_MSG( qtScrollBar, 0, "Invalid scrollbar" );
    
    return qtScrollBar->pageStep();
}

int wxWindow::GetScrollRange( int orient ) const
{
    QScrollBar *qtScrollBar = GetScrollBar( orient );
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
    QSize size = GetContainer()->size();
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

bool wxWindow::HandleQtPaintEvent ( QWidget *receiver, QPaintEvent *event )
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

QWidget *wxWindow::GetHandle() const
{
    return m_qtWindow;
}

QWidget *wxWindow::GetContainer() const
{
    if ( m_qtContainer )
        return m_qtContainer;
    else
        return GetHandle();
}

QWidget *wxWindow::GetScrollBarsContainer() const
{
    return m_qtWindow;
}
