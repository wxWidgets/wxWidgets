///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/listbox.cpp
// Purpose:     wxListBox
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "listbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTBOX

#ifndef WX_PRECOMP
#include "wx/listbox.h"
#include "wx/settings.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/dc.h"
#include "wx/utils.h"
#endif

#include "wx/window.h"
#include "wx/palmos/private.h"

#include "wx/dynarray.h"
#include "wx/log.h"

#if wxUSE_OWNER_DRAWN
    #include  "wx/ownerdrw.h"
#endif

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

IMPLEMENT_DYNAMIC_CLASS_XTI(wxListBox, wxControl,"wx/listbox.h")

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
IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)
#endif

// ============================================================================
// list box item declaration and implementation
// ============================================================================

#if wxUSE_OWNER_DRAWN

class wxListBoxItem : public wxOwnerDrawn
{
public:
    wxListBoxItem(const wxString& str = wxEmptyString);
};

wxListBoxItem::wxListBoxItem(const wxString& str) : wxOwnerDrawn(str, FALSE)
{
    // no bitmaps/checkmarks
    SetMarginWidth(0);
}

wxOwnerDrawn *wxListBox::CreateLboxItem(size_t WXUNUSED(n))
{
    return new wxListBoxItem();
}

#endif  //USE_OWNER_DRAWN

// ============================================================================
// list box control implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

// Listbox item
wxListBox::wxListBox()
{
}

bool wxListBox::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    return false;
}

bool wxListBox::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       const wxArrayString& choices,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    return false;
}

wxListBox::~wxListBox()
{
}

WXDWORD wxListBox::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    return 0;
}

// ----------------------------------------------------------------------------
// implementation of wxListBoxBase methods
// ----------------------------------------------------------------------------

void wxListBox::DoSetFirstItem(int N)
{
}

void wxListBox::Delete(int N)
{
}

int wxListBox::DoAppend(const wxString& item)
{
    return 0;
}

void wxListBox::DoSetItems(const wxArrayString& choices, void** clientData)
{
}

int wxListBox::FindString(const wxString& s) const
{
    return wxNOT_FOUND;
}

void wxListBox::Clear()
{
}

void wxListBox::Free()
{
}

void wxListBox::SetSelection(int N, bool select)
{
}

bool wxListBox::IsSelected(int N) const
{
    return false;
}

wxClientData* wxListBox::DoGetItemClientObject(int n) const
{
    return (wxClientData *)DoGetItemClientData(n);
}

void *wxListBox::DoGetItemClientData(int n) const
{
    return (void *)NULL;
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
}

void wxListBox::DoSetItemClientData(int n, void *clientData)
{
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    return 0;
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    return 0;
}

// Find string for position
wxString wxListBox::GetString(int N) const
{
    wxString result;

    return result;
}

void
wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
}

void wxListBox::SetString(int N, const wxString& s)
{
}

int wxListBox::GetCount() const
{
    return m_noItems;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

void wxListBox::SetHorizontalExtent(const wxString& s)
{
}

wxSize wxListBox::DoGetBestSize() const
{
    return wxSize(0,0);
}

// ----------------------------------------------------------------------------
// callbacks
// ----------------------------------------------------------------------------

bool wxListBox::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    return false;
}

// ----------------------------------------------------------------------------
// wxCheckListBox support
// ----------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN

// drawing
// -------

// space beneath/above each row in pixels
// "standard" checklistbox use 1 here, some might prefer 2. 0 is ugly.
#define OWNER_DRAWN_LISTBOX_EXTRA_SPACE    (1)

// the height is the same for all items
// TODO should be changed for LBS_OWNERDRAWVARIABLE style listboxes

// NB: can't forward this to wxListBoxItem because LB_SETITEMDATA
//     message is not yet sent when we get here!
bool wxListBox::MSWOnMeasure(WXMEASUREITEMSTRUCT *item)
{
    return true;
}

// forward the message to the appropriate item
bool wxListBox::MSWOnDraw(WXDRAWITEMSTRUCT *item)
{
    return true;
}

#endif // wxUSE_OWNER_DRAWN

#endif // wxUSE_LISTBOX
