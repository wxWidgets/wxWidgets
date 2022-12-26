/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statusbar.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart, Sean D'Epagnier
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statusbr.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLabel>

class wxQtStatusBar : public wxQtEventSignalHandler< QStatusBar, wxStatusBar >
{

public:
    wxQtStatusBar( wxWindow *parent, wxStatusBar *handler );
};

wxQtStatusBar::wxQtStatusBar( wxWindow *parent, wxStatusBar *handler )
    : wxQtEventSignalHandler< QStatusBar, wxStatusBar >( parent, handler )
{
}

//==============================================================================


wxStatusBar::wxStatusBar()
{
    Init();
}

wxStatusBar::wxStatusBar(wxWindow *parent, wxWindowID winid,
            long style,
            const wxString& name)
{
    Init();
    Create( parent, winid, style, name );
}

bool wxStatusBar::Create(wxWindow *parent, wxWindowID WXUNUSED(winid),
                         long style, const wxString& WXUNUSED(name))
{
    m_qtStatusBar = new wxQtStatusBar( parent, this );

    if ( style & wxSTB_SIZEGRIP )
        m_qtStatusBar->setSizeGripEnabled(true);

    PostCreation();

    SetFieldsCount(1);

    return true;
}

bool wxStatusBar::GetFieldRect(int i, wxRect& rect) const
{
    wxCHECK_MSG( (i >= 0) && ((size_t)i < m_panes.GetCount()), false,
                 "invalid statusbar field index" );

    if ( m_qtPanes.size() != m_panes.GetCount() )
        const_cast<wxStatusBar*>(this)->UpdateFields();

    rect = wxQtConvertRect(m_qtPanes[i]->geometry());
    return true;
}

void wxStatusBar::SetMinHeight(int height)
{
    m_qtStatusBar->setMinimumHeight(height);
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
    if ( m_qtPanes.size() != m_panes.GetCount() )
        UpdateFields();

    m_qtPanes[number]->setText( wxQtConvertString( m_panes[number].GetText() ) );
}

// Called each time number/size of panes changes
void wxStatusBar::Refresh( bool eraseBackground, const wxRect *rect )
{
    UpdateFields();

    wxWindow::Refresh( eraseBackground, rect );
}

void wxStatusBar::Init()
{
    m_qtStatusBar = nullptr;
}

void wxStatusBar::UpdateFields()
{
    // is it a good idea to recreate all the panes every update?

    for ( wxVector<QLabel*>::const_iterator it = m_qtPanes.begin();
          it != m_qtPanes.end(); ++it )
    {
        //Remove all panes
        delete *it;
    }
    m_qtPanes.clear();

    for (size_t i = 0; i < m_panes.GetCount(); i++)
    {
        //Set sizes
        int width = m_panes[i].GetWidth();

        QLabel *pane = new QLabel( m_qtStatusBar );
        m_qtPanes.push_back(pane);

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

QWidget *wxStatusBar::GetHandle() const
{
    return m_qtStatusBar;
}
