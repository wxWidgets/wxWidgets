/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/control.cpp
// Purpose:     wxControl class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "control.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CONTROLS

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/control.h"
#include "wx/toplevel.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/tglbtn.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
    EVT_ERASE_BACKGROUND(wxControl::OnEraseBackground)
END_EVENT_TABLE()

// ============================================================================
// wxControl implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxControl ctor/dtor
// ----------------------------------------------------------------------------

wxControl::~wxControl()
{
    m_isBeingDeleted = true;
}

// ----------------------------------------------------------------------------
// control window creation
// ----------------------------------------------------------------------------

bool wxControl::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& wxVALIDATOR_PARAM(validator),
                       const wxString& name)
{
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    return true;
}

bool wxControl::PalmCreateControl(ControlStyleType style,
                                  wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& label,
                                  const wxPoint& pos,
                                  const wxSize& size)
{
    SetParent(parent);
    SetId( id == wxID_ANY ? NewControlId() : id );
    FormType* form = GetParentForm();
    if(form==NULL)
        return false;

    m_label = label;

    m_control = CtlNewControl(
                    (void **)&form,
                    GetId(),
                    style,
                    m_label.c_str(),
                    ( pos.x == wxDefaultCoord ) ? winUndefConstraint : pos.x,
                    ( pos.y == wxDefaultCoord ) ? winUndefConstraint : pos.y,
                    ( size.x == wxDefaultCoord ) ? winUndefConstraint : size.x,
                    ( size.y == wxDefaultCoord ) ? winUndefConstraint : size.y,
                    boldFont,
                    0,
                    false
                );

    if(m_control==NULL)
        return false;

    Show();
    return true;
}

// ----------------------------------------------------------------------------
// various accessors
// ----------------------------------------------------------------------------

FormType* wxControl::GetParentForm() const
{
    wxWindow* parentTLW = GetParent();
    while ( parentTLW && !parentTLW->IsTopLevel() )
    {
        parentTLW = parentTLW->GetParent();
    }
    wxTopLevelWindowPalm* tlw = wxDynamicCast(parentTLW, wxTopLevelWindowPalm);
    if(!tlw)
        return NULL;
    return tlw->GetForm();
}

wxBorder wxControl::GetDefaultBorder() const
{
    // we want to automatically give controls a sunken style (confusingly,
    // it may not really mean sunken at all as we map it to WS_EX_CLIENTEDGE
    // which is not sunken at all under Windows XP -- rather, just the default)
    return wxBORDER_SUNKEN;
}

void wxControl::SetIntValue(int val)
{
    FormType* form = GetParentForm();
    if(form==NULL)
        return;
    uint16_t index = FrmGetObjectIndex(form, GetId());
    if(index==frmInvalidObjectId)
        return;
    FrmSetControlValue(form, index, val);
}

void wxControl::SetBoolValue(bool val)
{
    SetIntValue(val?1:0);
}

bool wxControl::GetBoolValue() const
{
    FormType* form = GetParentForm();
    if(form==NULL)
        return false;
    uint16_t index = FrmGetObjectIndex(form, GetId());
    if(index==frmInvalidObjectId)
        return false;
    return ( FrmGetControlValue(form, index) == 1 );
}

wxSize wxControl::DoGetBestSize() const
{
    return wxSize(16, 16);
}

bool wxControl::Enable(bool enable)
{
    if( m_control == NULL )
        return false;
    if( IsEnabled() == enable)
        return false;
    CtlSetEnabled( m_control, enable);
    return true;
}

bool wxControl::IsEnabled() const
{
    if( m_control == NULL )
        return false;
    return CtlEnabled(m_control);
}

bool wxControl::IsShown() const
{
    return StatGetAttribute ( statAttrBarVisible , NULL );
}

bool wxControl::Show( bool show )
{
    FormType* form = GetParentForm();
    if(form==NULL)
        return false;
    uint16_t index = FrmGetObjectIndex(form,GetId());
    if(index==frmInvalidObjectId)
        return false;
    if(show)
        FrmShowObject(form,index);
    else
        FrmHideObject(form,index);
    return true;
}

void wxControl::SetLabel(const wxString& label)
{
    // setting in wrong control causes crash
    if ( ( wxDynamicCast(this,wxButton) != NULL ) ||
         ( wxDynamicCast(this,wxCheckBox) != NULL ) ||
         ( wxDynamicCast(this,wxToggleButton) != NULL ) )
    {
        m_label = label;
        // TODO: as manual states, it crashes here
        // needs own manipulation on used string pointers
        // CtlSetLabel(m_control,m_label);
    }
}

wxString wxControl::GetLabel()
{
    // setting in wrong control causes crash
    if ( wxDynamicCast(this,wxButton) ||
         wxDynamicCast(this,wxCheckBox) ||
         wxDynamicCast(this,wxToggleButton) )
    {
        return m_label;
    }

    return wxEmptyString;
}

/* static */ wxVisualAttributes
wxControl::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attrs;

    // old school (i.e. not "common") controls use the standard dialog font
    // by default
    attrs.font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    // most, or at least many, of the controls use the same colours as the
    // buttons -- others will have to override this (and possibly simply call
    // GetCompositeControlsDefaultAttributes() from their versions)
    attrs.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
    attrs.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

    return attrs;
}

// another version for the "composite", i.e. non simple controls
/* static */ wxVisualAttributes
wxControl::GetCompositeControlsDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attrs;
    attrs.font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    attrs.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    attrs.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

    return attrs;
}

// ----------------------------------------------------------------------------
// message handling
// ----------------------------------------------------------------------------

bool wxControl::ProcessCommand(wxCommandEvent& event)
{
    return GetEventHandler()->ProcessEvent(event);
}

void wxControl::OnEraseBackground(wxEraseEvent& event)
{
}

WXHBRUSH wxControl::OnCtlColor(WXHDC pDC, WXHWND WXUNUSED(pWnd), WXUINT WXUNUSED(nCtlColor),
#if wxUSE_CTL3D
                               WXUINT message,
                               WXWPARAM wParam,
                               WXLPARAM lParam
#else
                               WXUINT WXUNUSED(message),
                               WXWPARAM WXUNUSED(wParam),
                               WXLPARAM WXUNUSED(lParam)
#endif
    )
{
    return (WXHBRUSH)0;
}

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

#endif // wxUSE_CONTROLS
