/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/radiobox.cpp
// Purpose:     wxRadioBox implementation
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native wxRadioBox implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "radiobox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RADIOBOX

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/brush.h"
    #include "wx/radiobox.h"
    #include "wx/settings.h"
    #include "wx/log.h"
#endif

#include "wx/palmos/private.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

// TODO: wxCONSTRUCTOR
#if 0 // wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxRadioBoxStyle )

wxBEGIN_FLAGS( wxRadioBoxStyle )
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

    wxFLAGS_MEMBER(wxRA_SPECIFY_COLS)
    wxFLAGS_MEMBER(wxRA_HORIZONTAL)
    wxFLAGS_MEMBER(wxRA_SPECIFY_ROWS)
    wxFLAGS_MEMBER(wxRA_VERTICAL)

wxEND_FLAGS( wxRadioBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxRadioBox, wxControl,"wx/radiobox.h")

wxBEGIN_PROPERTIES_TABLE(wxRadioBox)
    wxEVENT_PROPERTY( Select , wxEVT_COMMAND_RADIOBOX_SELECTED , wxCommandEvent )
    wxPROPERTY_FLAGS( WindowStyle , wxRadioBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

#else
IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)
#endif

/*
    selection
    content
        label
        dimension
        item
*/

#define RADIOBTN_PARENT_IS_RADIOBOX 0

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// global vars
// ---------------------------------------------------------------------------

// the pointer to standard radio button wnd proc
static WXFARPROC s_wndprocRadioBtn = (WXFARPROC)NULL;

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxRadioBox
// ---------------------------------------------------------------------------

int wxRadioBox::GetCount() const
{
    return 0;
}

int wxRadioBox::GetColumnCount() const
{
    return 0;
}

int wxRadioBox::GetRowCount() const
{
    return 0;
}

// returns the number of rows
int wxRadioBox::GetNumVer() const
{
    return 0;
}

// returns the number of columns
int wxRadioBox::GetNumHor() const
{
    return 0;
}

// Radio box item
wxRadioBox::wxRadioBox()
{
}

bool wxRadioBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& title,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n,
                        const wxString choices[],
                        int majorDim,
                        long style,
                        const wxValidator& val,
                        const wxString& name)
{
    return false;
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
    return false;
}

wxRadioBox::~wxRadioBox()
{
}

void wxRadioBox::SetString(int item, const wxString& label)
{
}

void wxRadioBox::SetSelection(int N)
{
}

// Get single selection, for single choice list items
int wxRadioBox::GetSelection() const
{
    return 0;
}

// Find string for position
wxString wxRadioBox::GetString(int item) const
{
    wxString ret;

    return ret;
}

// ----------------------------------------------------------------------------
// size calculations
// ----------------------------------------------------------------------------

wxSize wxRadioBox::GetMaxButtonSize() const
{
    return wxSize(0,0);
}

wxSize wxRadioBox::GetTotalButtonSize(const wxSize& sizeBtn) const
{
    return wxSize(0,0);
}

wxSize wxRadioBox::DoGetBestSize() const
{
    return wxSize(0,0);
}

// Restored old code.
void wxRadioBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
}

void wxRadioBox::GetSize(int *width, int *height) const
{
}

void wxRadioBox::GetPosition(int *x, int *y) const
{
}

void wxRadioBox::SetFocus()
{
}

bool wxRadioBox::Show(bool show)
{
    return false;
}

// Enable a specific button
void wxRadioBox::Enable(int item, bool enable)
{
}

// Enable all controls
bool wxRadioBox::Enable(bool enable)
{
    return false;
}

// Show a specific button
void wxRadioBox::Show(int item, bool show)
{
}

bool wxRadioBox::ContainsHWND(WXHWND hWnd) const
{
    return false;
}

void wxRadioBox::Command(wxCommandEvent & event)
{
}

void wxRadioBox::SubclassRadioButton(WXHWND hWndBtn)
{
}

void wxRadioBox::SendNotificationEvent()
{
}

bool wxRadioBox::SetFont(const wxFont& font)
{
    return false;
}

// ----------------------------------------------------------------------------
// our window proc
// ----------------------------------------------------------------------------

WXHBRUSH wxRadioBox::OnCtlColor(WXHDC pDC, WXHWND WXUNUSED(pWnd), WXUINT WXUNUSED(nCtlColor),
                               WXUINT WXUNUSED(message),
                               WXWPARAM WXUNUSED(wParam),
                               WXLPARAM WXUNUSED(lParam)
    )
{
    return (WXHBRUSH)0;
}

#endif // wxUSE_RADIOBOX
