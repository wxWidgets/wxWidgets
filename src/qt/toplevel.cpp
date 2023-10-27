/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/toplevel.cpp
// Author:      Peter Most, Javier Torres, Sean D'Epagnier, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/toplevel.h"
#include "wx/qt/private/converter.h"
#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

wxTopLevelWindowQt::wxTopLevelWindowQt()
{
}

wxTopLevelWindowQt::wxTopLevelWindowQt(wxWindow *parent,
           wxWindowID winId,
           const wxString &title,
           const wxPoint &pos,
           const wxSize &size,
           long style,
           const wxString &name )
{
    Create( parent, winId, title, pos, size, style, name );
}

bool wxTopLevelWindowQt::Create( wxWindow *parent, wxWindowID winId,
    const wxString &title, const wxPoint &pos, const wxSize &sizeOrig,
    long style, const wxString &name )
{
    wxSize size(sizeOrig);
    if ( !size.IsFullySpecified() )
        size.SetDefaults( GetDefaultSize() );

    wxTopLevelWindows.Append( this );

    if (!CreateBase( parent, winId, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxTopLevelWindowNative creation failed") );
        return false;
    }

    SetTitle( title );
    SetWindowStyleFlag( style );

    if (pos != wxDefaultPosition)
        m_qtWindow->move( pos.x, pos.y );

    m_qtWindow->resize( wxQtConvertSize( size ) );

    // Prevent automatic deletion of Qt main window on close
    // (this should be the default, but left just fo enforce it)
    GetHandle()->setAttribute(Qt::WA_DeleteOnClose, false);

    // not calling to wxWindow::Create, so do the rest of initialization:
    if (parent) parent->AddChild( this );

    return true;
}

bool wxTopLevelWindowQt::Show(bool show)
{
    if ( !wxTopLevelWindowBase::Show(show) )
        return false;

    if ( show && !m_qtWindow->isActiveWindow() )
        m_qtWindow->activateWindow();

    return true;
}

void wxTopLevelWindowQt::Maximize(bool maximize)
{
    QWidget *widget = GetHandle();

    if ( maximize )
    {
        widget->showMaximized();
    }
    else
    {
        widget->showNormal();
    }
}

void wxTopLevelWindowQt::Restore()
{
    GetHandle()->showNormal();
}

void wxTopLevelWindowQt::Iconize(bool iconize )
{
    QWidget *widget = GetHandle();

    if ( iconize )
    {
        widget->showMinimized();
    }
    else
    {
        widget->showNormal();
    }
}

bool wxTopLevelWindowQt::IsMaximized() const
{
    return GetHandle()->isMaximized();
}

bool wxTopLevelWindowQt::IsIconized() const
{
    return GetHandle()->isMinimized();
}


bool wxTopLevelWindowQt::ShowFullScreen(bool show, long WXUNUSED(style))
{
    QWidget *widget = GetHandle();

    if ( show )
    {
        widget->showFullScreen();
    }
    else
    {
        widget->showNormal();
    }

    return true;
}

bool wxTopLevelWindowQt::IsFullScreen() const
{
    return GetHandle()->isFullScreen();
}

void wxTopLevelWindowQt::SetTitle(const wxString& title)
{
    GetHandle()->setWindowTitle( wxQtConvertString( title ));
}

wxString wxTopLevelWindowQt::GetTitle() const
{
    return ( wxQtConvertString( GetHandle()->windowTitle() ));
}

void wxTopLevelWindowQt::SetIcons( const wxIconBundle& icons )
{
    wxTopLevelWindowBase::SetIcons( icons );

    QIcon qtIcons;
    for ( size_t i = 0; i < icons.GetIconCount(); i++ )
    {
        qtIcons.addPixmap( *icons.GetIconByIndex( i ).GetHandle() );
    }
    GetHandle()->setWindowIcon( qtIcons );
}

void wxTopLevelWindowQt::SetWindowStyleFlag( long style )
{
    wxWindow::SetWindowStyleFlag( style );

    if ( HasFlag( wxCENTRE ) )
    {
        Centre();
    }

    if ( !GetHandle() )
        return;

    Qt::WindowFlags qtFlags = GetHandle()->windowFlags();

    if ( HasFlag( wxSTAY_ON_TOP ) != qtFlags.testFlag( Qt::WindowStaysOnTopHint ) )
        qtFlags ^= Qt::WindowStaysOnTopHint;

    if ( HasFlag( wxCAPTION ) )
    {
        // Only show buttons if window has caption
        if ( HasFlag( wxSYSTEM_MENU ) )
        {
            qtFlags |= Qt::WindowSystemMenuHint;
            if ( HasFlag( wxMINIMIZE_BOX ) )
                qtFlags |= Qt::WindowMinimizeButtonHint;
            else
                qtFlags &= ~Qt::WindowMinimizeButtonHint;

            if ( HasFlag( wxMAXIMIZE_BOX ) )
                qtFlags |= Qt::WindowMaximizeButtonHint;
            else
                qtFlags &= ~Qt::WindowMaximizeButtonHint;

            if ( HasFlag( wxCLOSE_BOX ) )
                qtFlags |= Qt::WindowCloseButtonHint;
            else
                qtFlags &= ~Qt::WindowCloseButtonHint;
        }
        else
        {
            qtFlags &= ~Qt::WindowSystemMenuHint;
            qtFlags &= ~Qt::WindowMinMaxButtonsHint;
            qtFlags &= ~Qt::WindowCloseButtonHint;
        }
    }

    GetHandle()->setWindowFlags( qtFlags );

    wxCHECK_RET( !( HasFlag( wxMAXIMIZE ) && HasFlag( wxMAXIMIZE ) ), "Window cannot be both maximized and minimized" );
    if ( HasFlag( wxMAXIMIZE ) )
        GetHandle()->setWindowState( Qt::WindowMaximized );
    else if ( HasFlag( wxMINIMIZE ) )
        GetHandle()->setWindowState( Qt::WindowMinimized );

    if ( HasFlag( wxRESIZE_BORDER ) )
        GetHandle()->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    else
        GetHandle()->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    }

long wxTopLevelWindowQt::GetWindowStyleFlag() const
{
    // Update maximized/minimized state
    long winStyle = wxWindow::GetWindowStyleFlag();

    if (GetHandle())
    {
        switch ( GetHandle()->windowState() )
        {
        case Qt::WindowMaximized:
            winStyle &= ~wxMINIMIZE;
            winStyle |= wxMAXIMIZE;
            break;
        case Qt::WindowMinimized:
            winStyle &= ~wxMAXIMIZE;
            winStyle |= wxMINIMIZE;
            break;
        default:
            winStyle &= ~wxMINIMIZE;
            winStyle &= ~wxMAXIMIZE;
        }
    }

    return winStyle;
}

void wxTopLevelWindowQt::DoSetSizeHints( int minW, int minH,
                                         int maxW, int maxH,
                                         int incW, int incH )
{
    // The value -1 is special which means no constraints will be used.
    // In other words, use the Qt defaults if -1 was specified.

    if ( maxW == wxDefaultCoord )
        maxW = QWIDGETSIZE_MAX;
    if ( maxH == wxDefaultCoord )
        maxH = QWIDGETSIZE_MAX;

    minW = wxMax(0, minW);
    minH = wxMax(0, minH);

    incW = wxMax(0, incW);
    incH = wxMax(0, incH);

    GetHandle()->setMinimumSize(minW, minH);
    GetHandle()->setMaximumSize(maxW, maxH);
    GetHandle()->setSizeIncrement(incW, incH);

    wxTopLevelWindowBase::DoSetSizeHints(minW, minH, maxW, maxH, incW, incH);
}
