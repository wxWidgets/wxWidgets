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


typedef std::map<wxWindow*, QButtonGroup*> WindowToButtonGroupMap;

static WindowToButtonGroupMap& GetWindowToButtonGroupMap()
{
    static WindowToButtonGroupMap s_map;
    return s_map;
}

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

    if ( style & wxRB_SINGLE )
    {
        // Ensure that other buttons cannot join the last existing group
        GetWindowToButtonGroupMap().erase(parent);
    }
    else if ( style & wxRB_GROUP )
    {
        m_qtButtonGroup = new QButtonGroup();
        m_qtButtonGroup->addButton( m_qtRadioButton );

        GetWindowToButtonGroupMap()[parent] = m_qtButtonGroup;
        m_qtRadioButton->setChecked(true);
    }
    else
    {
        // Add it to the previous group, if any
        WindowToButtonGroupMap::iterator it = GetWindowToButtonGroupMap().find(parent);
        if ( it != GetWindowToButtonGroupMap().end() )
        {
            it->second->addButton(m_qtRadioButton);
        }
    }

    m_qtRadioButton->setText( wxQtConvertString( label ));

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

wxRadioButton::~wxRadioButton()
{
    if ( m_qtRadioButton->group() && m_qtRadioButton->group()->buttons().size() == 1 )
    {
        // If this button is the only member of the last group, remove the map entry for the group
        WindowToButtonGroupMap::iterator it = GetWindowToButtonGroupMap().find( GetParent() );
        if ( it != GetWindowToButtonGroupMap().end() && m_qtRadioButton->group() == it->second )
        {
            GetWindowToButtonGroupMap().erase(it);

            if( m_qtButtonGroup )
            {
                delete m_qtButtonGroup;
            }
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
