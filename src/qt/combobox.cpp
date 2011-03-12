/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/combobox.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/combobox.h"
#include "wx/qt/converter.h"

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
    m_qtComboBox = new wxQtComboBox( parent, value );
    m_qtComboBox->AddChoices( n, choices );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

void wxComboBox::SetSelection(int n)
{
}

void wxComboBox::SetSelection(long from, long to)
{
}

int wxComboBox::GetSelection() const
{
    return 0;
}

void wxComboBox::GetSelection(long *from, long *to) const
{
}

wxString wxComboBox::GetStringSelection() const
{
    return wxString();
}


unsigned wxComboBox::GetCount() const
{
    return 0;
}

wxString wxComboBox::GetString(unsigned int n) const
{
    return wxString();
}

void wxComboBox::SetString(unsigned int n, const wxString& s)
{
}


int wxComboBox::DoInsertItems(const wxArrayStringsAdapter & items,
                  unsigned int pos,
                  void **clientData,
                  wxClientDataType type)
{
    return 0;
}


void wxComboBox::DoSetItemClientData(unsigned int n, void *clientData)
{
}

void *wxComboBox::DoGetItemClientData(unsigned int n) const
{
    return NULL;
}


void wxComboBox::DoClear()
{
}

void wxComboBox::DoDeleteOneItem(unsigned int pos)
{
}

QComboBox *wxComboBox::GetHandle() const
{
    return m_qtComboBox;
}
