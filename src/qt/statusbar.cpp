/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statusbar.cpp
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statusbr.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

wxStatusBar::wxStatusBar()
{
}

wxStatusBar::wxStatusBar(wxWindow *parent, wxWindowID winid,
            long style,
            const wxString& name)
{
    Create( parent, winid, style, name );
}

void wxStatusBar::Init()
{
}

bool wxStatusBar::Create(wxWindow *parent, wxWindowID winid,
            long style,
            const wxString& name)
{
    wxMISSING_IMPLEMENTATION( "wxStatusBar::Create parameters" );

    m_qtStatusBar = new wxQtStatusBar( parent, this );

    return true;
}

bool wxStatusBar::GetFieldRect(int i, wxRect& rect) const
{
    return false;
}

void wxStatusBar::SetMinHeight(int height)
{
}

int wxStatusBar::GetBorderX() const
{
    return 0;
}

int wxStatusBar::GetBorderY() const
{
    return 0;
}

void wxStatusBar::DoUpdateStatusText(int number)
{
    m_qtPanes[number]->setText( wxQtConvertString( m_panes[number].GetText() ) );
}

// Called each time number/size of panes changes
void wxStatusBar::Refresh( bool eraseBackground, const wxRect *rect )
{
    UpdateFields();

    wxWindow::Refresh( eraseBackground, rect );
}

void wxStatusBar::UpdateFields()
{
    while ( !m_qtPanes.isEmpty() )
    {
        //Remove all panes
        delete m_qtPanes.takeLast();
    }

    for (size_t i = 0; i < m_panes.GetCount(); i++)
    {
        //Set sizes
        int width = m_panes[i].GetWidth();

        QLabel *pane = new QLabel( m_qtStatusBar );
        m_qtPanes.append( pane );

        if ( width >= 0 )
        {
            //Fixed width field
            pane->setMinimumSize( QSize(width, 0) );
            m_qtStatusBar->addWidget( pane );
        }
        else
        {
            m_qtStatusBar->addWidget( pane, -width );
        }
    }
}

QStatusBar *wxStatusBar::GetHandle() const
{
    return m_qtStatusBar;
}

//==============================================================================

wxQtStatusBar::wxQtStatusBar( wxWindow *parent, wxStatusBar *handler )
    : wxQtEventSignalHandler< QStatusBar, wxStatusBar >( parent, handler )
{
}

