/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/checkbox.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/checkbox.h"
#include "wx/qt/converter.h"

wxCheckBox::wxCheckBox()
{
}

wxCheckBox::wxCheckBox( wxWindow *parent, wxWindowID id, const wxString& label,
        const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
            const wxString& name )
{
    m_qtCheckBox = new QCheckBox( wxQtConvertString( label ), parent->GetHandle() );

    return wxCheckBoxBase::Create( parent, id, pos, size, style, validator, name );
}


void wxCheckBox::SetValue(bool value)
{
}

bool wxCheckBox::GetValue() const
{
    return false;
}

QCheckBox *wxCheckBox::GetHandle() const
{
    return m_qtCheckBox;
}
