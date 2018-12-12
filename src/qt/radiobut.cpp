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

    if ( ( style & wxRB_GROUP ) || ( style && wxRB_SINGLE ) )
    {
        createAndJoinNewGroup();
    }
    else
    {
        wxWindowList::compatibility_iterator node = parent->GetChildren().GetLast();
        for ( ; node; node = node->GetPrevious() )
        {
            wxWindow *child = node->GetData();

            if ( wxIsKindOf( child, wxRadioButton ) )
            {
                if ( !child->HasFlag( wxRB_SINGLE ) )
                {
                    QRadioButton *ptr = dynamic_cast<QRadioButton *>( child->GetHandle() );

                    QButtonGroup* btnGroup = ptr->group();
                    btnGroup->addButton( m_qtRadioButton );
                }
                else
                {
                    createAndJoinNewGroup();
                }

                break;
            }
        }
    }

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

QWidget *wxRadioButton::GetHandle() const
{
    return m_qtRadioButton;
}

void wxRadioButton::createAndJoinNewGroup()
{
    m_qtButtonGroup = new QButtonGroup( );
    m_qtButtonGroup->addButton( m_qtRadioButton );
}
