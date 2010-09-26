/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/radiobox.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/radiobox.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

IMPLEMENT_DYNAMIC_CLASS( wxRadioBox, wxControl )

wxRadioBox::wxRadioBox()
{
}

wxRadioBox::wxRadioBox(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           int majorDim,
           long style,
           const wxValidator& val,
           const wxString& name)
{
    Create( parent, id, title, pos, size, n, choices, majorDim, style, val, name );
}

wxRadioBox::wxRadioBox(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           int majorDim,
           long style,
           const wxValidator& val,
           const wxString& name)
{
    Create( parent, id, title, pos, size, choices, majorDim, style, val, name );
}



bool wxRadioBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            int majorDim,
            long style,
            const wxValidator& val,
            const wxString& name)
{
    return Create( parent, id, title, pos, size, choices.size(), &choices[ 0 ],
        majorDim, style, val, name );
}


template < typename QtButton >
static void AddChoices( QButtonGroup *qtButtonGroup, int count, const wxString choices[] )
{
    while ( count-- > 0 )
        qtButtonGroup->addButton( new QtButton( wxQtConvertString( *choices++ )));
}


bool wxRadioBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            int majorDim,
            long style,
            const wxValidator& val,
            const wxString& name)
{
    if ( !CreateControl( parent, id, pos, size, style, val, name ))
        return false;

    m_qtGroupBox = new QGroupBox( wxQtConvertString( title ), parent->GetHandle() );
    m_qtButtonGroup = new QButtonGroup( m_qtGroupBox );

    if ( style & wxRA_SPECIFY_ROWS )
        wxMISSING_IMPLEMENTATION( wxSTRINGIZE( wxRA_SPECIFY_ROWS ));
    else if ( style & wxRA_SPECIFY_COLS )
        wxMISSING_IMPLEMENTATION( wxSTRINGIZE( wxRA_SPECIFY_COLS ));

    if ( style & wxRA_USE_CHECKBOX )
        AddChoices< QCheckBox >( m_qtButtonGroup, n, choices );
    else
        AddChoices< QRadioButton >( m_qtButtonGroup, n, choices );

    return true;
}



bool wxRadioBox::Enable(unsigned int n, bool enable)
{
    return false;
}

bool wxRadioBox::Show(unsigned int n, bool show)
{
    return false;
}

bool wxRadioBox::IsItemEnabled(unsigned int n) const
{
    return false;
}

bool wxRadioBox::IsItemShown(unsigned int n) const
{
    return false;
}

unsigned wxRadioBox::GetCount() const
{
    return 0;
}

wxString wxRadioBox::GetString(unsigned int n) const
{
    return wxString();
}

void wxRadioBox::SetString(unsigned int n, const wxString& s)
{
}

void wxRadioBox::SetSelection(int n)
{
}

int wxRadioBox::GetSelection() const
{
    return 0;
}

QGroupBox *wxRadioBox::GetHandle() const
{
    return m_qtGroupBox;
}

