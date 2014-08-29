/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/choice.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/choice.h"
#include "wx/qt/private/winevent.h"
#include <QtWidgets/QComboBox>

class wxQtChoice : public wxQtEventSignalHandler< QComboBox, wxChoice >
{
public:
    wxQtChoice( wxWindow *parent, wxChoice *handler );

private:
    void activated(int index);
};

wxQtChoice::wxQtChoice( wxWindow *parent, wxChoice *handler )
    : wxQtEventSignalHandler< QComboBox, wxChoice >( parent, handler )
{
    // the activated signal is overloaded, the following explicit cast is needed:
    connect(this, static_cast<void (QComboBox::*)(int index)>(&QComboBox::activated),
            this, &wxQtChoice::activated);
}

void wxQtChoice::activated(int WXUNUSED(index))
{
    wxChoice *handler = GetHandler();
    if ( handler )
        handler->SendSelectionChangedEvent(wxEVT_CHOICE);
}


wxChoice::wxChoice()
{
}


wxChoice::wxChoice( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, n, choices, style, validator, name );
}


wxChoice::wxChoice( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, choices, style, validator, name );
}


bool wxChoice::Create( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    return Create( parent, id, pos, size, choices.size(), &choices[ 0 ], style,
        validator, name );
}


bool wxChoice::Create( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    m_qtComboBox = new wxQtChoice( parent, this );

    while ( n-- > 0 )
        m_qtComboBox->addItem( wxQtConvertString( *choices++ ));

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

wxSize wxChoice::DoGetBestSize() const
{
    wxSize basesize = wxChoiceBase::DoGetBestSize();
    wxSize size = wxControl::DoGetBestSize();
    // mix calculated size by wx base prioritizing qt hint (max):
    if (size.GetWidth() < basesize.GetWidth())
        size.SetWidth(basesize.GetWidth());
    if (size.GetHeight() < basesize.GetHeight())
        size.SetHeight(basesize.GetHeight());
    return size;
}


unsigned wxChoice::GetCount() const
{
    return m_qtComboBox->count();
}

wxString wxChoice::GetString(unsigned int n) const
{
    return wxQtConvertString( m_qtComboBox->itemText(n) );
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
    m_qtComboBox->setItemText(n, wxQtConvertString(s));
}


void wxChoice::SetSelection(int n)
{
    m_qtComboBox->setCurrentIndex(n);
}

int wxChoice::GetSelection() const
{
    return m_qtComboBox->currentIndex();
}


int wxChoice::DoInsertItems(const wxArrayStringsAdapter & items,
                  unsigned int pos,
                  void **clientData,
                  wxClientDataType type)
{
    InvalidateBestSize();
    int n = DoInsertItemsInLoop(items, pos, clientData, type);
    return n;
}

int wxChoice::DoInsertOneItem(const wxString& item, unsigned int pos)
{
    m_qtComboBox->insertItem(pos, wxQtConvertString(item));
    return pos;
}

void wxChoice::DoSetItemClientData(unsigned int n, void *clientData)
{
    QVariant variant = qVariantFromValue(clientData);
    m_qtComboBox->setItemData(n, variant);
}

void *wxChoice::DoGetItemClientData(unsigned int n) const
{
    QVariant variant = m_qtComboBox->itemData(n);
    return variant.value<void *>();
}


void wxChoice::DoClear()
{
    m_qtComboBox->clear();
}

void wxChoice::DoDeleteOneItem(unsigned int pos)
{
    m_qtComboBox->removeItem(pos);
}

QComboBox *wxChoice::GetHandle() const
{
    return m_qtComboBox;
}
