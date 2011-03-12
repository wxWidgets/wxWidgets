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
    m_qtRadioButton = new QRadioButton( parent->GetHandle() );
    m_qtRadioButton->setText( wxQtConvertString( label ));

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

void wxRadioButton::SetValue(bool value)
{
    m_qtRadioButton->setChecked( value );
}

bool wxRadioButton::GetValue() const
{
    return m_qtRadioButton->isChecked();
}

QRadioButton *wxRadioButton::GetHandle() const
{
    return m_qtRadioButton;
}
