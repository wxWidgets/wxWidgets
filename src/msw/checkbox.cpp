/////////////////////////////////////////////////////////////////////////////
// Name:        msw/checkbox.cpp
// Purpose:     wxCheckBox
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

#include "wx/msw/private.h"

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
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, m_windowId);
    wxCheckBoxState state = Get3StateValue();

    // If the style flag to allow the user setting the undetermined state
    // is not set, then skip the undetermined state and set it to unchecked.
    if ( state == wxCHK_UNDETERMINED && !Is3rdStateAllowedForUser() )
    {
        state = wxCHK_UNCHECKED;
        Set3StateValue(state);
    }

    event.SetInt(state);
    event.SetEventObject(this);
    ProcessCommand(event);

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
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    long msStyle = WS_TABSTOP;

    if ( style & wxCHK_3STATE )
    {
        msStyle |= BS_AUTO3STATE;
    }
    else
    {
        wxASSERT_MSG( !Is3rdStateAllowedForUser(),
            wxT("Using wxCH_ALLOW_3RD_STATE_FOR_USER")
            wxT(" style flag for a 2-state checkbox is useless") );
        msStyle |= BS_AUTOCHECKBOX;
    }

    if ( style & wxALIGN_RIGHT )
    {
        msStyle |= BS_LEFTTEXT | BS_RIGHT;
    }

    return MSWCreateControl(wxT("BUTTON"), msStyle, pos, size, label, 0);
}


wxSize wxCheckBox::DoGetBestSize() const
{
    static int s_checkSize = 0;

    if ( !s_checkSize )
    {
        wxScreenDC dc;
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

        s_checkSize = dc.GetCharHeight();
    }

    wxString str = wxGetWindowText(GetHWND());

    int wCheckbox, hCheckbox;
    if ( !str.IsEmpty() )
    {
        GetTextExtent(str, &wCheckbox, &hCheckbox);
        wCheckbox += s_checkSize + GetCharWidth();

        if ( hCheckbox < s_checkSize )
            hCheckbox = s_checkSize;
    }
    else
    {
        wCheckbox = s_checkSize;
        hCheckbox = s_checkSize;
    }
#ifdef __WXWINCE__
    hCheckbox += 1;
#endif

    return wxSize(wCheckbox, hCheckbox);
}

WXHBRUSH wxCheckBox::MSWGetDefaultBgBrush()
{
    return ::GetStockObject(NULL_BRUSH);
}

void wxCheckBox::SetValue(bool val)
{
    if (val)
    {
        Set3StateValue(wxCHK_CHECKED);
    }
    else
    {
        Set3StateValue(wxCHK_UNCHECKED);
    }
}

bool wxCheckBox::GetValue() const
{
    return (Get3StateValue() != 0);
}

void wxCheckBox::Command(wxCommandEvent& event)
{
    int state = event.GetInt();
    wxCHECK_RET( (state == wxCHK_UNCHECKED) || (state == wxCHK_CHECKED)
        || (state == wxCHK_UNDETERMINED),
        wxT("event.GetInt() returned an invalid checkbox state") );

    Set3StateValue((wxCheckBoxState) state);
    ProcessCommand(event);
}

wxCOMPILE_TIME_ASSERT(wxCHK_UNCHECKED == BST_UNCHECKED
    && wxCHK_CHECKED == BST_CHECKED
    && wxCHK_UNDETERMINED == BST_INDETERMINATE, EnumValuesIncorrect);

void wxCheckBox::DoSet3StateValue(wxCheckBoxState state)
{
    ::SendMessage(GetHwnd(), BM_SETCHECK, (WPARAM) state, 0);
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
#ifdef __WIN32__
    return (wxCheckBoxState) ::SendMessage(GetHwnd(), BM_GETCHECK, 0, 0);
#else
    return (wxCheckBoxState) ((::SendMessage(GetHwnd(), BM_GETCHECK, 0, 0)
        & 0x001) == 0x001);
#endif

}

#endif // wxUSE_CHECKBOX
