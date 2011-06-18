/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/panel.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/panel.h"

wxPanel::wxPanel()
{
}

wxPanel::wxPanel( wxWindow *parent, int x, int y, int width, int height,
    long style, const wxString& name)
{
    Create( parent, wxID_ANY, wxPoint( x, y ), wxSize( width, height ), style, name );
}

wxPanel::wxPanel(wxWindow *parent, wxWindowID winid, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name )
{
    Create( parent, winid, pos, size, style, name );
}

bool wxPanel::Create( wxWindow *parent, wxWindowID winid, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name)
{
    m_qtWidget = new wxQtWidget( parent, this );

    return wxWindow::Create( parent, winid, pos, size, style, name );
}

wxPanel::~wxPanel()
{
}

QWidget *wxPanel::GetHandle() const
{
    return m_qtWidget;
}
