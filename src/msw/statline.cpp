/////////////////////////////////////////////////////////////////////////////
// Name:        msw/statline.cpp
// Purpose:     MSW version of wxStaticLine class
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Version:     $Id$
// Copyright:   (c) 1998 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "statline.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/statline.h"

#if wxUSE_STATLINE

#include "wx/msw/private.h"
#include "wx/log.h"

#ifndef SS_SUNKEN
    #define SS_SUNKEN 0x00001000L
#endif

#ifndef SS_NOTIFY
    #define SS_NOTIFY 0x00000100L
#endif

// ============================================================================
// implementation
// ============================================================================

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxStaticLineStyle )

WX_BEGIN_FLAGS( wxStaticLineStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    WX_FLAGS_MEMBER(wxBORDER_SIMPLE)
    WX_FLAGS_MEMBER(wxBORDER_SUNKEN)
    WX_FLAGS_MEMBER(wxBORDER_DOUBLE)
    WX_FLAGS_MEMBER(wxBORDER_RAISED)
    WX_FLAGS_MEMBER(wxBORDER_STATIC)
    WX_FLAGS_MEMBER(wxBORDER_NONE)
    
    // old style border flags
    WX_FLAGS_MEMBER(wxSIMPLE_BORDER)
    WX_FLAGS_MEMBER(wxSUNKEN_BORDER)
    WX_FLAGS_MEMBER(wxDOUBLE_BORDER)
    WX_FLAGS_MEMBER(wxRAISED_BORDER)
    WX_FLAGS_MEMBER(wxSTATIC_BORDER)
    WX_FLAGS_MEMBER(wxNO_BORDER)

    // standard window styles
    WX_FLAGS_MEMBER(wxTAB_TRAVERSAL)
    WX_FLAGS_MEMBER(wxCLIP_CHILDREN)
    WX_FLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    WX_FLAGS_MEMBER(wxWANTS_CHARS)
    WX_FLAGS_MEMBER(wxNO_FULL_REPAINT_ON_RESIZE)
    WX_FLAGS_MEMBER(wxALWAYS_SHOW_SB )
    WX_FLAGS_MEMBER(wxVSCROLL)
    WX_FLAGS_MEMBER(wxHSCROLL)

    WX_FLAGS_MEMBER(wxLI_HORIZONTAL)
    WX_FLAGS_MEMBER(wxLI_VERTICAL)

WX_END_FLAGS( wxStaticLineStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticLine, wxControl,"wx/statline.h")

WX_BEGIN_PROPERTIES_TABLE(wxStaticLine)
    WX_PROPERTY_FLAGS( WindowStyle , wxStaticLineStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
WX_END_PROPERTIES_TABLE()

WX_BEGIN_HANDLERS_TABLE(wxStaticLine)
WX_END_HANDLERS_TABLE()

WX_CONSTRUCTOR_4( wxStaticLine, wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size )

#else
IMPLEMENT_DYNAMIC_CLASS(wxStaticLine, wxControl)
#endif

/*
	TODO PROPERTIES :
		style (wxLI_HORIZONTAL)
*/

// ----------------------------------------------------------------------------
// wxStaticLine
// ----------------------------------------------------------------------------

bool wxStaticLine::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& sizeOrig,
                          long style,
                          const wxString &name)
{
    wxSize size = AdjustSize(sizeOrig);

    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    return MSWCreateControl(_T("STATIC"), wxEmptyString, pos, size);
}

WXDWORD wxStaticLine::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    // we never have border
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // add our default styles
    msStyle |= SS_SUNKEN | SS_NOTIFY | WS_CLIPSIBLINGS;
#ifndef __WXWINCE__
    msStyle |= SS_GRAYRECT ;
#endif

    return msStyle ;
}

#endif // wxUSE_STATLINE

