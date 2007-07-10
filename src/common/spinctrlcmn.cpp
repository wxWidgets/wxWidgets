/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/spinctrlcmn.cpp
// Purpose:     wxSpinCtrl common code
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.07.99
// RCS-ID:      $Id: spinctrl.cpp 45699 2007-04-27 21:32:40Z VS $
// Copyright:   (c) 1999-2005 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxSpinCtrlStyle )

wxBEGIN_FLAGS( wxSpinCtrlStyle )
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

    wxFLAGS_MEMBER(wxSP_HORIZONTAL)
    wxFLAGS_MEMBER(wxSP_VERTICAL)
    wxFLAGS_MEMBER(wxSP_ARROW_KEYS)
    wxFLAGS_MEMBER(wxSP_WRAP)

wxEND_FLAGS( wxSpinCtrlStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxSpinCtrl, wxControl,"wx/spinbut.h")

wxBEGIN_PROPERTIES_TABLE(wxSpinCtrl)
    wxEVENT_RANGE_PROPERTY( Spin , wxEVT_SCROLL_TOP , wxEVT_SCROLL_CHANGED , wxSpinEvent )
    wxEVENT_PROPERTY( Updated , wxEVT_COMMAND_SPINCTRL_UPDATED , wxCommandEvent )
    wxEVENT_PROPERTY( TextUpdated , wxEVT_COMMAND_TEXT_UPDATED , wxCommandEvent )
    wxEVENT_PROPERTY( TextEnter , wxEVT_COMMAND_TEXT_ENTER , wxCommandEvent )

    wxPROPERTY( ValueString , wxString , SetValue , GetValue , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) ;
    wxPROPERTY( Value , int , SetValue, GetValue, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))

/*
    wxPROPERTY( Min , int , SetMin, GetMin, 0, 0 /*flags , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Max , int , SetMax, GetMax, 0 , 0 /*flags , wxT("Helpstring") , wxT("group"))
*/
    wxPROPERTY_FLAGS( WindowStyle , wxSpinCtrlStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
/*
    TODO PROPERTIES
        style wxSP_ARROW_KEYS
*/
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxSpinCtrl)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_6( wxSpinCtrl , wxWindow* , Parent , wxWindowID , Id , wxString , ValueString , wxPoint , Position , wxSize , Size , long , WindowStyle )
#else
IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrl, wxControl)
#endif

#endif // wxUSE_SPINCTRL
