/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statbox.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statbox.h"
#include "wx/window.h"
#include "wx/qt/converter.h"
#include <QtWidgets/QGroupBox>


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
    m_qtGroupBox = new QGroupBox( wxQtConvertString( label ), parent->GetHandle() );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

QGroupBox *wxStaticBox::GetHandle() const
{
    return m_qtGroupBox;
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    wxStaticBoxBase::GetBordersForSizer(borderTop, borderOther);

    // need extra space for the label:
    *borderTop += GetCharHeight();
}
