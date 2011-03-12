/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statbox.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statbox.h"
#include "wx/qt/groupbox_qt.h"

wxStaticBox::wxStaticBox()
{
}

wxStaticBox::wxStaticBox(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    m_qtGroupBox = new wxQtGroupBox( parent, label );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

QGroupBox *wxStaticBox::GetHandle() const
{
    return m_qtGroupBox;
}

