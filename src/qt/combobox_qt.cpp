/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/combobox_qt.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/combobox_qt.h"
#include "wx/qt/converter.h"

// Constructor for wxChoice:
wxQtComboBox::wxQtComboBox( wxWindow *parent )
    : QComboBox( parent->GetHandle() )
{
    setEditable( false );
}

// Constructor for wxComboBox:
wxQtComboBox::wxQtComboBox( wxWindow *parent, const wxString &value )
    : QComboBox( parent->GetHandle() )
{
    setEditable( true );
    setEditText( wxQtConvertString( value ));
}


template < typename StringIterator >
static void QtAddChoices( wxQtComboBox *qtComboBox,
        StringIterator begin, StringIterator end )
{
    for ( ; begin != end; ++begin )
        qtComboBox->addItem( wxQtConvertString( *begin ));
}


void wxQtComboBox::AddChoices( int count, const wxString choices[] )
{
   QtAddChoices( this, &choices[ 0 ], &choices[ count ] );
}

void wxQtComboBox::AddChoices( const wxArrayString &choices )
{
    QtAddChoices( this, choices.begin(), choices.end() );
}
