/////////////////////////////////////////////////////////////////////////////
// Name:        msw/statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "statbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATBOX

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
#endif

#include "wx/statbox.h"
#include "wx/notebook.h"

#include "wx/msw/private.h"

// this is for Win CE
#ifndef WS_EX_TRANSPARENT
    #define WS_EX_TRANSPARENT 0
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxStaticBoxStyle )

wxBEGIN_FLAGS( wxStaticBoxStyle )
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

wxEND_FLAGS( wxStaticBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticBox, wxControl,"wx/statbox.h")

wxBEGIN_PROPERTIES_TABLE(wxStaticBox)
    wxPROPERTY( Label,wxString, SetLabel, GetLabel, wxString() , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxStaticBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE, 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
/*
    TODO PROPERTIES :
        label
*/
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxStaticBox)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_6( wxStaticBox , wxWindow* , Parent , wxWindowID , Id , wxString , Label , wxPoint , Position , wxSize , Size , long , WindowStyle )
#else
IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStaticBox
// ----------------------------------------------------------------------------

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(wxT("BUTTON"), BS_GROUPBOX, pos, size, label,
                           WS_EX_TRANSPARENT) )
        return false;

    return true;
}

wxSize wxStaticBox::DoGetBestSize() const
{
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, GetFont());

    int wBox;
    GetTextExtent(wxGetWindowText(m_hWnd), &wBox, &cy);

    wBox += 3*cx;
    int hBox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    return wxSize(wBox, hBox);
}

WXHBRUSH
wxStaticBox::MSWGetBgBrushForSelf(wxWindow *parent, WXHDC WXUNUSED(pDC))
{
    // we can't use pattern brushes because, apparently, the window proc of our
    // class does something which invalidates the brush origin before drawing
    // with it and so any patetrn brush is used incorrectly as can be seen by
    // putting a static box inside a (themed) notebook
    //
    // so always use solid brush for painting the background (note that this
    // only applies to the background of the box label, not the insides of the
    // box itself)
    return parent->MSWGetSolidBgBrushForChild(this);
}

#endif // wxUSE_STATBOX
