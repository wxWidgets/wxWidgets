/////////////////////////////////////////////////////////////////////////////
// Name:        palmos/checkbox.cpp
// Purpose:     wxCheckBox
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "checkbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CHECKBOX

#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
    #include "wx/brush.h"
    #include "wx/dcscreen.h"
    #include "wx/settings.h"
#endif

#include "wx/palmos/private.h"

#ifndef BST_UNCHECKED
    #define BST_UNCHECKED 0x0000
#endif

#ifndef BST_CHECKED
    #define BST_CHECKED 0x0001
#endif

#ifndef BST_INDETERMINATE
    #define BST_INDETERMINATE 0x0002
#endif

// ============================================================================
// implementation
// ============================================================================

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxCheckBoxStyle )

wxBEGIN_FLAGS( wxCheckBoxStyle )
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
    wxFLAGS_MEMBER(wxNO_BORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxNO_FULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

wxEND_FLAGS( wxCheckBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxCheckBox, wxControl,"wx/checkbox.h")

wxBEGIN_PROPERTIES_TABLE(wxCheckBox)
    wxEVENT_PROPERTY( Click , wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEvent )

    wxPROPERTY( Font , wxFont , SetFont , GetFont , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Label,wxString, SetLabel, GetLabel, wxString() , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Value ,bool, SetValue, GetValue, EMPTY_MACROVALUE, 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxCheckBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE, 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxCheckBox)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_6( wxCheckBox , wxWindow* , Parent , wxWindowID , Id , wxString , Label , wxPoint , Position , wxSize , Size , long , WindowStyle ) 
#else
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
#endif


// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

bool wxCheckBox::MSWCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
    return true;
}

bool wxCheckBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size, long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    return false;
}

void wxCheckBox::SetLabel(const wxString& label)
{
}

wxSize wxCheckBox::DoGetBestSize() const
{
    return wxSize(0,0);
}

void wxCheckBox::SetValue(bool val)
{
}

bool wxCheckBox::GetValue() const
{
    return false;
}

void wxCheckBox::Command(wxCommandEvent& event)
{
}

wxCOMPILE_TIME_ASSERT(wxCHK_UNCHECKED == BST_UNCHECKED
    && wxCHK_CHECKED == BST_CHECKED
    && wxCHK_UNDETERMINED == BST_INDETERMINATE, EnumValuesIncorrect);

void wxCheckBox::DoSet3StateValue(wxCheckBoxState state)
{
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    return (wxCheckBoxState) 0;
}

#endif // wxUSE_CHECKBOX
