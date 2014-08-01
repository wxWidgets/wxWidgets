/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/combobox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/combobox.h"
#include "wx/window.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"
#include <QtWidgets/QComboBox>

class wxQtComboBox : public wxQtEventSignalHandler< QComboBox, wxComboBox >
{
public:
    wxQtComboBox( wxWindow *parent, wxComboBox *handler );

private:
    void activated(int index);
    void editTextChanged(const QString &text);
};

wxQtComboBox::wxQtComboBox( wxWindow *parent, wxComboBox *handler )
    : wxQtEventSignalHandler< QComboBox, wxComboBox >( parent, handler )
{
    setEditable( true );
    connect(this, static_cast<void (QComboBox::*)(int index)>(&QComboBox::activated),
            this, &wxQtComboBox::activated);
    connect(this, &QComboBox::editTextChanged,
            this, &wxQtComboBox::editTextChanged);
}

void wxQtComboBox::activated(int WXUNUSED(index))
{
    wxComboBox *handler = GetHandler();
    if ( handler )
        handler->SendSelectionChangedEvent(wxEVT_COMBOBOX);
}

void wxQtComboBox::editTextChanged(const QString &text)
{
    wxComboBox *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_TEXT, handler->GetId() );
        event.SetString( wxQtConvertString( text ) );
        EmitEvent( event );
    }
}


wxComboBox::wxComboBox()
{
}


wxComboBox::wxComboBox(wxWindow *parent,
           wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name )
{
    Create( parent, id, value, pos, size, n, choices, style, validator, name );
}


wxComboBox::wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name )
{
    Create( parent, id, value, pos, size, choices, style, validator, name );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    return Create( parent, id, value, pos, size, choices.size(), &choices[ 0 ],
        style, validator, name );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    m_qtComboBox = new wxQtComboBox( parent, this );
    while ( n-- > 0 )
        m_qtComboBox->addItem( wxQtConvertString( *choices++ ));
    m_qtComboBox->setEditText( wxQtConvertString( value ));

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

wxString wxComboBox::DoGetValue() const
{
    return wxQtConvertString( m_qtComboBox->currentText() );
}

void wxComboBox::Popup()
{
     GetHandle()->showPopup();
}

void wxComboBox::Dismiss()
{
    GetHandle()->hidePopup();
}
