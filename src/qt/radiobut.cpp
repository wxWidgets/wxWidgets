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
    m_qtRadioButton(NULL)
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
        m_qtRadioButton(NULL)
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

    if( ( style & wxRB_GROUP ) || ( style & wxRB_SINGLE ) || ( ! ))
    {
        CreateAndJoinNewGroup();
    }

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

void wxRadioButton::CreateAndJoinNewGroup()
{
    // Note that the QButtonGroup created below will be deallocated by the QRadioButton owing
    // to using that as its parent.
    QButtonGroup* qtButtonGroup = new QButtonGroup( GetHandle() );
    qtButtonGroup->addButton( m_qtRadioButton );
}

bool wxRadioButton::SearchForPreviousGroupToJoin( wxWindow *parent )
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

                QButtonGroup *btnGroup = ptr->group();

                if ( btnGroup )
                {
                    btnGroup->addButton(m_qtRadioButton);
                    return true;
                }
            }

            break;
        }
    }

    return false;
}
