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

/*
   Qt provides the QButtonGroup class to provide grouping behaviour for button widgets. As there is no direct
   counterpart to this class in Wx, we need wxRadioButton to both use and store it, in order to provide this
   functionality (some external class having to manage something which only this class is concerned with makes no
   sense).
   Having a static map (with wxWindow* as the key and the value being the last QButtonGroup*) allows the wxRadioButton
   to access the required QButtonGroup or add to the map as necessary.
*/
typedef std::map<wxWindow*, QButtonGroup*> WindowToLastButtonGroupMap;

static WindowToLastButtonGroupMap& GetWindowToLastButtonGroupMap()
{
    static WindowToLastButtonGroupMap s_map;
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
        GetWindowToLastButtonGroupMap().erase(parent);
    }
    else if ( style & wxRB_GROUP )
    {
        QButtonGroup *qtButtonGroup = new QButtonGroup();
        qtButtonGroup->addButton(m_qtRadioButton);

        GetWindowToLastButtonGroupMap()[parent] = qtButtonGroup;
        m_qtRadioButton->setChecked(true);
    }
    else
    {
        // Add it to the previous group, if any
        WindowToLastButtonGroupMap::iterator it = GetWindowToLastButtonGroupMap().find(parent);
        if ( it != GetWindowToLastButtonGroupMap().end() )
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
        WindowToLastButtonGroupMap::iterator it = GetWindowToLastButtonGroupMap().find(GetParent());
        if ( it != GetWindowToLastButtonGroupMap().end() && m_qtRadioButton->group() == it->second )
        {
            delete it->second;
            GetWindowToLastButtonGroupMap().erase(it);
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
