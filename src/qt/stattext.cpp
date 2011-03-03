/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/stattext.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/stattext.h"
#include "wx/qt/converter.h"

wxStaticText::wxStaticText()
{
}

wxStaticText::wxStaticText(wxWindow *parent,
             wxWindowID id,
             const wxString &label,
             const wxPoint &pos,
             const wxSize &size,
             long style,
             const wxString &name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticText::Create(wxWindow *parent,
            wxWindowID id,
            const wxString &label,
            const wxPoint &pos,
            const wxSize &size,
            long style,
            const wxString &name)
{
    m_qtLabel = new QLabel( wxQtConvertString( label ), parent->GetHandle() );

    return CreateControl( parent, id, pos, size, style, wxDefaultValidator, name )
        && wxStaticTextBase::Create( parent, id, pos, size, style, wxDefaultValidator, name );
}


QLabel *wxStaticText::GetHandle() const
{
    return m_qtLabel;
}
