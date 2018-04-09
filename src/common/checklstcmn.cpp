///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/checklstcmn.cpp
// Purpose:     wxCheckListBox common code
// Author:      Vadim Zeitlin
// Modified by:
// Created:     16.11.97
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
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

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/colour.h"
    #include "wx/font.h"
    #include "wx/bitmap.h"
    #include "wx/window.h"
    #include "wx/listbox.h"
    #include "wx/dcmemory.h"
    #include "wx/settings.h"
    #include "wx/log.h"
#endif

#include "wx/valgen.h"

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxCheckListBoxStyle )
wxBEGIN_FLAGS( wxCheckListBoxStyle )
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
    wxFLAGS_MEMBER(wxLB_OWNERDRAW)

wxEND_FLAGS( wxCheckListBoxStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxCheckListBox, wxListBox, "wx/checklst.h");

wxBEGIN_PROPERTIES_TABLE(wxCheckListBox)
    wxEVENT_PROPERTY( Toggle, wxEVT_CHECKLISTBOX, wxCommandEvent )
    wxPROPERTY_FLAGS( WindowStyle, wxCheckListBoxStyle, long, SetWindowStyleFlag, \
                      GetWindowStyleFlag, wxEMPTY_PARAMETER_VALUE, wxLB_OWNERDRAW /*flags*/, \
                      wxT("Helpstring"), wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxEMPTY_HANDLERS_TABLE(wxCheckListBox)

wxCONSTRUCTOR_4( wxCheckListBox, wxWindow*, Parent, wxWindowID, Id, \
                 wxPoint, Position, wxSize, Size )


// ============================================================================
// implementation
// ============================================================================

unsigned int wxCheckListBoxBase::GetCheckedItems(wxArrayInt& checkedItems) const
{
    unsigned int const numberOfItems = GetCount();

    checkedItems.clear();
    for ( unsigned int i = 0; i < numberOfItems; ++i )
    {
        if ( IsChecked(i) )
            checkedItems.push_back(i);
    }

    return checkedItems.size();
}

#if wxUSE_VALIDATORS

bool wxCheckListBoxBase::DoTransferDataToWindow(const wxValidator::DataPtr& ptr)
{
    wxASSERT_MSG(ptr->IsOfType<wxArrayInt>(), "Expected type: 'wxArrayInt'");

    // clear all selections
    size_t i, count = GetCount();

    for ( i = 0 ; i < count; ++i )
        Check(i, false);

    const wxArrayInt& arr = ptr->GetValue<wxArrayInt>();

    // select each item in our array
    count = arr.GetCount();
    for ( i = 0 ; i < count; ++i )
        Check(arr.Item(i));

    return true;
}

bool wxCheckListBoxBase::DoTransferDataFromWindow(wxValidator::DataPtr& ptr)
{
    wxASSERT_MSG(ptr->IsOfType<wxArrayInt>(), "Expected type: 'wxArrayInt'");

    wxArrayInt& arr = ptr->GetValue<wxArrayInt>();
        
    arr.Clear();

    for ( size_t i = 0, count = GetCount(); i < count; ++i )
    {
        if ( IsChecked(i) )
            arr.Add(i);
    }

    return true;
}

#endif // wxUSE_VALIDATORS

#endif // wxUSE_CHECKLISTBOX
