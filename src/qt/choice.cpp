/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/choice.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/choice.h"

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
    m_qtComboBox = new wxQtComboBox( parent );
    m_qtComboBox->AddChoices( n, choices );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}




unsigned wxChoice::GetCount() const
{
    return 0;
}

wxString wxChoice::GetString(unsigned int n) const
{
    return wxString();
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
}


void wxChoice::SetSelection(int n)
{
}

int wxChoice::GetSelection() const
{
    return 0;
}


int wxChoice::DoInsertItems(const wxArrayStringsAdapter & items,
                  unsigned int pos,
                  void **clientData,
                  wxClientDataType type)
{
    return 0;
}


void wxChoice::DoSetItemClientData(unsigned int n, void *clientData)
{
}

void *wxChoice::DoGetItemClientData(unsigned int n) const
{
    return NULL;
}


void wxChoice::DoClear()
{
}

void wxChoice::DoDeleteOneItem(unsigned int pos)
{
}

QComboBox *wxChoice::GetHandle() const
{
    return m_qtComboBox;
}
