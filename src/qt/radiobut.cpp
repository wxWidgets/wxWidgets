/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/radiobut.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/radiobut.h"
#include "wx/qt/converter.h"

wxRadioButton::wxRadioButton()
{
}

wxRadioButton::wxRadioButton( wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name)
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxRadioButton::Create( wxWindow *parent,
             wxWindowID id,
             const wxString& label,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxValidator& validator,
             const wxString& name)
{
    if ( !CreateControl( parent, id, pos, size, style, validator, name ))
        return false;

    m_qtRadioButton = new QRadioButton( wxQtConvertString( label ), parent->GetHandle() );

    return wxControl::Create( parent, id, pos, size, style, validator, name );
}

void wxRadioButton::SetValue(bool value)
{
}

bool wxRadioButton::GetValue() const
{
    return false;
}

QRadioButton *wxRadioButton::GetHandle() const
{
    return m_qtRadioButton;
}
