/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/combobox.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/combobox.h"
#include "wx/window.h"
#include "wx/qt/converter.h"
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

void wxQtComboBox::activated(int index)
{
    // TODO: use SendSelectionChangedEvent(wxEVT_COMBOBOX);
    wxCommandEvent event( wxEVT_COMBOBOX, GetHandler()->GetId() );
    event.SetInt(index);
    event.SetString(GetHandler()->GetStringSelection());
    EmitEvent( event );
}

void wxQtComboBox::editTextChanged(const QString &text)
{
    wxCommandEvent event( wxEVT_TEXT, GetHandler()->GetId() );
    event.SetString( wxQtConvertString( text ) );
    EmitEvent( event );
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

void wxComboBox::SetSelection(int n)
{
    m_qtComboBox->setCurrentIndex(n);
}

void wxComboBox::SetSelection(long from, long to)
{
}

int wxComboBox::GetSelection() const
{
    return m_qtComboBox->currentIndex();
}

void wxComboBox::GetSelection(long *from, long *to) const
{
}

wxString wxComboBox::GetStringSelection() const
{
    return wxQtConvertString( m_qtComboBox->currentText() );
}


unsigned wxComboBox::GetCount() const
{
    return m_qtComboBox->count();
}

wxString wxComboBox::GetString(unsigned int n) const
{
    return wxQtConvertString( m_qtComboBox->itemText(n) );
}

void wxComboBox::SetString(unsigned int n, const wxString& s)
{
    m_qtComboBox->setItemText(n, wxQtConvertString(s));
}


int wxComboBox::DoInsertItems(const wxArrayStringsAdapter & items,
                  unsigned int pos,
                  void **clientData,
                  wxClientDataType type)
{
    InvalidateBestSize();
    int n = DoInsertItemsInLoop(items, pos, clientData, type);
    return n;
}

int wxComboBox::DoInsertOneItem(const wxString& item, unsigned int pos)
{
    m_qtComboBox->insertItem(pos, wxQtConvertString(item));
}


void wxComboBox::DoSetItemClientData(unsigned int n, void *clientData)
{
    QVariant variant = qVariantFromValue(clientData);
    m_qtComboBox->setItemData(n, variant);
}

void *wxComboBox::DoGetItemClientData(unsigned int n) const
{
    QVariant variant = m_qtComboBox->itemData(n);
    return variant.value<void *>();
}


void wxComboBox::DoClear()
{
    m_qtComboBox->clear();
}

void wxComboBox::DoDeleteOneItem(unsigned int pos)
{
    m_qtComboBox->removeItem(pos);
}

QComboBox *wxComboBox::GetHandle() const
{
    return m_qtComboBox;
}
