/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/radiobut.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/radiobut.h"
#include "wx/qt/private/converter.h"

#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>

std::map<wxWindow*, QButtonGroup*> wxRadioButton::m_lastGroup;

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

    if ( (style & wxRB_GROUP) || (style & wxRB_SINGLE) )
    {
        JoinNewGroup();

        if( style & wxRB_SINGLE )
        {
            // Ensure that other buttons cannot join this group
            m_lastGroup.erase(parent);
        }
        else
        {
            m_lastGroup[parent] = m_qtButtonGroup;
            m_qtRadioButton->setChecked(true); // The first button in a group should be selected
        }
    }
    else
    {
        // Add it to the previous group, if any
        std::map<wxWindow*, QButtonGroup*>::iterator it = m_lastGroup.find(parent);
        if ( it != m_lastGroup.end() )
        {
            it->second->addButton(m_qtRadioButton);
        }
    }

    m_qtRadioButton->setText( wxQtConvertString( label ));

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

wxRadioButton::~wxRadioButton()
{
    if ( m_qtRadioButton->group() &&
         m_qtRadioButton->group()->buttons().size() == 1 )
    {
        std::map<wxWindow*, QButtonGroup*>::iterator it = m_lastGroup.find( GetParent() );
        if ( it != m_lastGroup.end() && m_qtRadioButton->group() == it->second )
        {
            m_lastGroup.erase(it);
        }
    }
}

void wxRadioButton::SetValue(bool value)
{
    m_qtRadioButton->setChecked( value );
}

bool wxRadioButton::GetValue() const
{
    return m_qtRadioButton->isChecked();
}

QWidget *wxRadioButton::GetHandle() const
{
    return m_qtRadioButton;
}

void wxRadioButton::JoinNewGroup()
{
    m_qtButtonGroup = new QButtonGroup( );
    m_qtButtonGroup->addButton( m_qtRadioButton );
}
