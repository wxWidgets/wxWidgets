/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/listbox.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/listbox.h"

wxListBox::wxListBox()
{
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name)
{
    Create( parent, id, pos, size, n, choices, style, validator, name );
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name)
{
    Create( parent, id, pos, size, choices, style, validator, name );
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    m_qtListWidget = new QListWidget( parent->GetHandle() );

    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    m_qtListWidget = new QListWidget( parent->GetHandle() );

    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

bool wxListBox::IsSelected(int n) const
{
    return false;
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    return 0;
}

unsigned wxListBox::GetCount() const
{
    return 0;
}

wxString wxListBox::GetString(unsigned int n) const
{
    return wxString();
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
}

void wxListBox::SetSelection(int n)
{
}

int wxListBox::GetSelection() const
{
    return 0;
}

void wxListBox::DoSetFirstItem(int n)
{
}

void wxListBox::DoSetSelection(int n, bool select)
{
}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter & items,
                          unsigned int pos,
                          void **clientData,
                          wxClientDataType type)
{
    return 0;
}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    return NULL;
}

void wxListBox::DoClear()
{
}

void wxListBox::DoDeleteOneItem(unsigned int pos)
{
}

QListWidget *wxListBox::GetHandle() const
{
    return m_qtListWidget;
}
