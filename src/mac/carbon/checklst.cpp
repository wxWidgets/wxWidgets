///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
//
// new DataBrowser-based version


#include "wx/wxprec.h"

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"
#include "wx/arrstr.h"

#include "wx/mac/uma.h"

#ifndef __DARWIN__
#include <Appearance.h>
#endif

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)

BEGIN_EVENT_TABLE(wxCheckListBox, wxListBox)
END_EVENT_TABLE()

void wxCheckListBox::Init()
{
}

bool wxCheckListBox::Create(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint &pos,
    const wxSize &size,
    const wxArrayString& choices,
    long style,
    const wxValidator& validator,
    const wxString &name )
{
    wxCArrayString chs( choices );

    return Create( parent, id, pos, size, chs.GetCount(), chs.GetStrings(), style, validator, name );
}

bool wxCheckListBox::Create(
   wxWindow *parent,
   wxWindowID id,
   const wxPoint& pos,
   const wxSize& size,
   int n,
   const wxString choices[],
   long style,
   const wxValidator& validator,
   const wxString& name )
{
    m_macIsUserPane = false;

    wxASSERT_MSG( !(style & wxLB_MULTIPLE) || !(style & wxLB_EXTENDED),
                  wxT("only one of listbox selection modes can be specified") );

    if ( !wxListBoxBase::Create( parent, id, pos, size, style & ~(wxHSCROLL | wxVSCROLL), validator, name ) )
        return false;

    // this will be increased by our Append command
    m_noItems = 0;

    m_peer = (wxMacControl*) CreateMacListControl(pos , size , style );

    MacPostControlCreate(pos,size);

	InsertItems( n , choices , 0 );

    // Needed because it is a wxControlWithItems
    SetBestSize( size );

    return true;
}

// ----------------------------------------------------------------------------
// wxCheckListBox functions
// ----------------------------------------------------------------------------

bool wxCheckListBox::IsChecked(unsigned int item) const
{
    wxCHECK_MSG( IsValid(item), false,
                 wxT("invalid index in wxCheckListBox::IsChecked") );

    return m_checks[item] != 0;
}

void wxCheckListBox::Check(unsigned int item, bool check)
{
    wxCHECK_RET( IsValid(item),
                 wxT("invalid index in wxCheckListBox::Check") );

    bool isChecked = m_checks[item] != 0;
    if ( check != isChecked )
    {
        m_checks[item] = check;
        MacUpdateLine( item );
    }
}

// ----------------------------------------------------------------------------
// methods forwarded to wxCheckListBox
// ----------------------------------------------------------------------------

void wxCheckListBox::Delete(unsigned int n)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxCheckListBox::Delete") );

    wxListBox::Delete( n );
    m_checks.RemoveAt( n );
}

int wxCheckListBox::DoAppend(const wxString& item)
{
    int pos = wxListBox::DoAppend( item );

    // the item is initially unchecked
    m_checks.Insert( false, pos );

    return pos;
}

void wxCheckListBox::DoInsertItems(const wxArrayString& items, unsigned int pos)
{
    wxListBox::DoInsertItems( items, pos );

    unsigned int count = items.GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
        m_checks.Insert( false, pos + n );
    }
}

void wxCheckListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    // call it first as it does DoClear()
    wxListBox::DoSetItems( items, clientData );

    unsigned int count = items.GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
        m_checks.Add( false );
    }
}

void wxCheckListBox::DoClear()
{
    m_checks.Empty();
}

#endif // wxUSE_CHECKLISTBOX
