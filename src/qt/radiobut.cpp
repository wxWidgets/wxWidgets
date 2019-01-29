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

wxRadioButton::wxRadioButton() :
    m_qtRadioButton(NULL),
    m_qtButtonGroup(NULL)
{
}

wxRadioButton::wxRadioButton( wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name) :
        m_qtRadioButton(NULL),
        m_qtButtonGroup(NULL)
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

    if ( style & wxRB_GROUP )
    {
        createAndJoinNewGroup( parent );
    }
    else if ( !( style & wxRB_SINGLE) )
    {
        searchForPreviousGroupToJoin( parent );
    }

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

wxRadioButton::~wxRadioButton()
{
    delete m_qtButtonGroup;
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

void wxRadioButton::createAndJoinNewGroup( wxWindow *parent )
{
    m_qtButtonGroup = new QButtonGroup( parent->GetHandle() );
    m_qtButtonGroup->addButton( m_qtRadioButton );
}

void wxRadioButton::searchForPreviousGroupToJoin( wxWindow *parent )
{
    wxWindowList::compatibility_iterator node = parent->GetChildren().GetLast();

    for ( ; node; node = node->GetPrevious() )
    {
        wxWindow *previous = node->GetData();

        if ( wxIsKindOf( previous, wxRadioButton ) )
        {
            if ( !previous->HasFlag( wxRB_SINGLE ) )
            {
                QRadioButton *ptr = dynamic_cast<QRadioButton *>( previous->GetHandle() );

                QButtonGroup* btnGroup = ptr->group();
                btnGroup->addButton( m_qtRadioButton );
            }
            else
            {
                createAndJoinNewGroup(parent);
            }

            break;
        }
    }
}

void wxRadioButton::addButtonToGroup(QRadioButton* radioButton)
{
    if( !m_qtButtonGroup )
        return;

    m_qtButtonGroup->addButton(radioButton);
}
