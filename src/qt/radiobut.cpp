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
