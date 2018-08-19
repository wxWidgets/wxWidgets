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
#include <QtWidgets/QLineEdit>

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
    static_cast<QComboBox *>(GetHandle())->showPopup();
}

void wxComboBox::Dismiss()
{
    static_cast<QComboBox *>(GetHandle())->hidePopup();
}

void wxComboBox::SetSelection( long from, long to )
{
    // SelectAll uses -1 to -1, adjust for qt:
    if (from == -1 && to == -1)
    {
        from = 0;
        to = GetValue().length();
    }
    // use the inner text entry widget (note that can be null if not editable)
    if ( m_qtComboBox->lineEdit() != NULL )
        m_qtComboBox->lineEdit()->setSelection(from, to);
}

void wxComboBox::GetSelection(long* from, long* to) const
{
    // use the inner text entry widget (note that can be null if not editable)
    if ( m_qtComboBox->lineEdit() != NULL )
    {
        *from = m_qtComboBox->lineEdit()->selectionStart();
        if ( *from >= 0 )
        {
            *to = *from + m_qtComboBox->lineEdit()->selectedText().length();
            return;
        }
    }
    // No selection or text control, call base for default behaviour:
    wxTextEntry::GetSelection(from, to);
}
