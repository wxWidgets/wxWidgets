///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/lboxcmn.cpp
// Purpose:     wxListBox class methods common to all platforms
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTBOX

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/arrstr.h"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxListBoxStyle )

wxBEGIN_FLAGS( wxListBoxStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxLB_SINGLE)
    wxFLAGS_MEMBER(wxLB_MULTIPLE)
    wxFLAGS_MEMBER(wxLB_EXTENDED)
    wxFLAGS_MEMBER(wxLB_HSCROLL)
    wxFLAGS_MEMBER(wxLB_ALWAYS_SB)
    wxFLAGS_MEMBER(wxLB_NEEDED_SB)
    wxFLAGS_MEMBER(wxLB_SORT)

wxEND_FLAGS( wxListBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxListBox, wxControlWithItems,"wx/listbox.h")

wxBEGIN_PROPERTIES_TABLE(wxListBox)
    wxEVENT_PROPERTY( Select , wxEVT_COMMAND_LISTBOX_SELECTED , wxCommandEvent )
    wxEVENT_PROPERTY( DoubleClick , wxEVT_COMMAND_LISTBOX_DOUBLECLICKED , wxCommandEvent )

    wxPROPERTY( Font , wxFont , SetFont , GetFont  , EMPTY_MACROVALUE, 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_COLLECTION( Choices , wxArrayString , wxString , AppendString , GetStrings, 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Selection ,int, SetSelection, GetSelection, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY_FLAGS( WindowStyle , wxListBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxListBox)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_4( wxListBox , wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size )
#else
IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControlWithItems)
#endif

/*
TODO PROPERTIES
    selection
    content
        item
*/


wxListBoxBase::~wxListBoxBase()
{
    // this destructor is required for Darwin
}

// ----------------------------------------------------------------------------
// adding items
// ----------------------------------------------------------------------------

void wxListBoxBase::InsertItems(unsigned int nItems, const wxString *items, unsigned int pos)
{
    wxArrayString aItems;
    for ( unsigned int n = 0; n < nItems; n++ )
    {
        aItems.Add(items[n]);
    }

    DoInsertItems(aItems, pos);
}


void wxListBoxBase::Set(int nItems, const wxString* items, void **clientData)
{
    wxArrayString aItems;
    for ( int n = 0; n < nItems; n++ )
    {
        aItems.Add(items[n]);
    }

    DoSetItems(aItems, clientData);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

bool wxListBoxBase::SetStringSelection(const wxString& s, bool select)
{
    const int sel = FindString(s);
    if ( sel == wxNOT_FOUND )
        return false;

    SetSelection(sel, select);

    return true;
}

void wxListBoxBase::DeselectAll(int itemToLeaveSelected)
{
    if ( HasMultipleSelection() )
    {
        wxArrayInt selections;
        GetSelections(selections);

        size_t count = selections.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            int item = selections[n];
            if ( item != itemToLeaveSelected )
                Deselect(item);
        }
    }
    else // single selection
    {
        int sel = GetSelection();
        if ( sel != wxNOT_FOUND && sel != itemToLeaveSelected )
        {
            Deselect(sel);
        }
    }
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void wxListBoxBase::Command(wxCommandEvent& event)
{
    SetSelection(event.GetInt(), event.GetExtraLong() != 0);
    (void)ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// SetFirstItem() and such
// ----------------------------------------------------------------------------

void wxListBoxBase::SetFirstItem(const wxString& s)
{
    int n = FindString(s);

    wxCHECK_RET( n != wxNOT_FOUND, wxT("invalid string in wxListBox::SetFirstItem") );

    DoSetFirstItem(n);
}

void wxListBoxBase::AppendAndEnsureVisible(const wxString& s)
{
    Append(s);
    EnsureVisible(GetCount() - 1);
}

void wxListBoxBase::EnsureVisible(int WXUNUSED(n))
{
    // the base class version does nothing (the only alternative would be to
    // call SetFirstItem() but this is probably even more stupid)
}

#endif // wxUSE_LISTBOX
