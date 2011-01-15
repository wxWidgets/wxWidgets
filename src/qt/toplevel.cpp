/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/toplevel.cpp
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/toplevel.h"
#include "wx/qt/converter.h"
#include <QtGui/QIcon>

wxTopLevelWindowNative::wxTopLevelWindowNative()
{
}

wxTopLevelWindowNative::wxTopLevelWindowNative(wxWindow *parent,
           wxWindowID winId,
           const wxString &title,
           const wxPoint &pos,
           const wxSize &size,
           long style,
           const wxString &name )
{
    Create( parent, winId, title, pos, size, style, name );
}

bool wxTopLevelWindowNative::Create( wxWindow *parent, wxWindowID winId,
    const wxString &title, const wxPoint &pos, const wxSize &size,
    long style, const wxString &name )
{
    wxSize defaultSize = size;
    if ( !defaultSize.IsFullySpecified() )
        defaultSize.SetDefaults( GetDefaultSize() );

    wxTopLevelWindows.Append( this );

    SetTitle( title );

    return CreateBase( parent, winId, pos, size, style, wxDefaultValidator, name );
}

void wxTopLevelWindowNative::Maximize(bool maximize) 
{
}

void wxTopLevelWindowNative::Restore()
{
}

void wxTopLevelWindowNative::Iconize(bool iconize )
{
}

bool wxTopLevelWindowNative::IsMaximized() const
{
    return false;
}

bool wxTopLevelWindowNative::IsIconized() const
{
    return false;
}


bool wxTopLevelWindowNative::ShowFullScreen(bool show, long style)
{
    return false;
}

bool wxTopLevelWindowNative::IsFullScreen() const
{
    return false;
}

void wxTopLevelWindowNative::SetTitle(const wxString& title)
{
    GetHandle()->setWindowTitle( wxQtConvertString( title ));
}

wxString wxTopLevelWindowNative::GetTitle() const
{
    return ( wxQtConvertString( GetHandle()->windowTitle() ));
}

void wxTopLevelWindowNative::SetIcons( const wxIconBundle& icons )
{
    wxTopLevelWindowBase::SetIcons( icons );
    
    QIcon qtIcons;
    for ( size_t i = 0; i < icons.GetIconCount(); i++ )
    {
        qtIcons.addPixmap( *icons.GetIconByIndex( i ).GetHandle() );
    }
    GetHandle()->setWindowIcon( qtIcons );
}

void wxTopLevelWindowNative::SetWindowStyleFlag( long style )
{
    wxWindow::SetWindowStyleFlag( style );
    
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
    
    if ( HasFlag( wxCENTRE ) )
    {
        Centre();
    }
}

long wxTopLevelWindowNative::GetWindowStyleFlag() const
{
    // Update maximized/minimized state
    long winStyle = wxWindow::GetWindowStyleFlag();
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

    return winStyle;
}
