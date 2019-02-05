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
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>

namespace
{

// Tiny helper applying a safe cast to return the handle of wxRadioButton as
// its actual type.
inline
QRadioButton* QtGetRadioButton(wxRadioButton* radioBtn)
{
    return static_cast<QRadioButton*>(radioBtn->GetHandle());
}

void QtStartNewGroup(QRadioButton* qtRadioButton)
{
    // Note that the QButtonGroup created here will be deallocated when its
    // parent // QRadioButton is destroyed.
    QButtonGroup* qtButtonGroup = new QButtonGroup(qtRadioButton);
    qtButtonGroup->addButton(qtRadioButton);
}

bool QtTryJoiningExistingGroup(wxRadioButton* radioBtnThis)
{
    for ( wxWindow* previous = radioBtnThis->GetPrevSibling();
          previous;
          previous = previous->GetPrevSibling() )
    {
        if ( wxRadioButton* radioBtn = wxDynamicCast(previous, wxRadioButton) )
        {
            // We should never join the exclusive group of wxRB_SINGLE button.
            if ( !radioBtn->HasFlag(wxRB_SINGLE) )
            {
                if ( QButtonGroup *qtGroup = QtGetRadioButton(radioBtn)->group() )
                {
                    qtGroup->addButton(QtGetRadioButton(radioBtnThis));
                    return true;
                }
            }

            break;
        }
    }

    return false;
}

} // anonymous namespace

class wxQtRadioButton : public wxQtEventSignalHandler< QRadioButton, wxRadioButton >
{
public:
    wxQtRadioButton( wxWindow *parent, wxRadioButton *handler ) :
         wxQtEventSignalHandler< QRadioButton, wxRadioButton >(parent, handler)

    {
        connect(this, &QRadioButton::clicked, this, &wxQtRadioButton::OnClicked);
    }

    void OnClicked(bool checked)
    {
        wxRadioButton* handler = GetHandler();
        if ( handler == NULL )
            return;

        wxCommandEvent event(wxEVT_RADIOBUTTON, handler->GetId());
        event.SetInt(checked ? 1 : 0);
        EmitEvent(event);
    }
};

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
    m_qtRadioButton = new wxQtRadioButton( parent, this );
    m_qtRadioButton->setText( wxQtConvertString( label ));

    if ( !QtCreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // Check if we need to create a new button group: this must be done when
    // explicitly requested to do so (wxRB_GROUP) but also for wxRB_SINGLE
    // buttons to prevent them implicitly becoming part of an existing group.
    if ( (style & wxRB_GROUP) || (style & wxRB_SINGLE) )
    {
        QtStartNewGroup(m_qtRadioButton);
    }
    else
    {
        // Otherwise try to join an already existing group. Currently we don't
        // do anything if joining it fails, i.e. if there is no current group
        // yet, because the radio buttons not explicitly associated with some
        // group still work as if they were part of one, so we just ignore the
        // return value of this function.
        QtTryJoiningExistingGroup(this);
    }

    return true;
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
