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
#include "wx/radiobut.h"
#include "wx/slider.h"

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

void wxControl::Init()
{
    m_palmControl = false;
    m_palmField = false;
}

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
                                  const wxString& label,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  int groupID)
{
    FormType* form = GetParentForm();
    if(form==NULL)
        return false;

    m_label = label;

    ControlType *control = CtlNewControl(
                               (void **)&form,
                               GetId(),
                               style,
                               m_label.c_str(),
                               ( pos.x == wxDefaultCoord ) ? winUndefConstraint : pos.x,
                               ( pos.y == wxDefaultCoord ) ? winUndefConstraint : pos.y,
                               ( size.x == wxDefaultCoord ) ? winUndefConstraint : size.x,
                               ( size.y == wxDefaultCoord ) ? winUndefConstraint : size.y,
                               stdFont,
                               groupID,
                               false
                           );

    if(control==NULL)
        return false;

    m_palmControl = true;

    Show();
    return true;
}

bool wxControl::PalmCreateField(const wxString& label,
                                const wxPoint& pos,
                                const wxSize& size,
                                bool editable,
                                bool underlined,
                                JustificationType justification)
{
    FormType* form = GetParentForm();
    if(form==NULL)
        return false;

    m_label = label;

    FieldType *field = FldNewField(
                           (void **)&form,
                           GetId(),
                           ( pos.x == wxDefaultCoord ) ? winUndefConstraint : pos.x,
                           ( pos.y == wxDefaultCoord ) ? winUndefConstraint : pos.y,
                           ( size.x == wxDefaultCoord ) ? winUndefConstraint : size.x,
                           ( size.y == wxDefaultCoord ) ? winUndefConstraint : size.y,
                           stdFont,
                           10,
                           editable,
                           underlined,
                           false,
                           false,
                           justification,
                           false,
                           false,
                           false
                       );

    if(field==NULL)
        return false;

    m_palmField = true;

    Show();
    SetLabel(label);
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

uint16_t wxControl::GetObjectIndex() const
{
    FormType* form = GetParentForm();
    if(form==NULL)return frmInvalidObjectId;
    return FrmGetObjectIndex(form, GetId());
}

void* wxControl::GetObjectPtr() const
{
    FormType* form = GetParentForm();
    if(form==NULL)return NULL;
    uint16_t index = FrmGetObjectIndex(form, GetId());
    if(index==frmInvalidObjectId)return NULL;
    return FrmGetObjectPtr(form,index);
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

void wxControl::DoGetBounds( RectangleType &rect ) const
{
    FormType* form = GetParentForm();
    if(form==NULL)
        return;
    uint16_t index = FrmGetObjectIndex(form,GetId());
    if(index==frmInvalidObjectId)
        return;
    FrmGetObjectBounds(form,index,&rect);
}

void wxControl::DoGetPosition( int *x, int *y ) const
{
    RectangleType rect;
    DoGetBounds(rect);
    if(x)
        *x = rect.topLeft.x;
    if(y)
        *y = rect.topLeft.y;
}

void wxControl::DoGetSize( int *width, int *height ) const
{
    RectangleType rect;
    DoGetBounds(rect);
    if(width)
        *width = rect.extent.x;
    if(height)
        *height = rect.extent.y;
}

bool wxControl::Enable(bool enable)
{
    ControlType *control = (ControlType *)GetObjectPtr();
    if( (IsPalmControl()) || (control == NULL))
        return false;
    if( CtlEnabled(control) == enable)
        return false;
    CtlSetEnabled( control, enable);
    return true;
}

bool wxControl::IsEnabled() const
{
    ControlType *control = (ControlType *)GetObjectPtr();
    if( (IsPalmControl()) || (control == NULL))
        return false;
    return CtlEnabled(control);
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

void wxControl::SetFieldLabel(const wxString& label)
{
    FieldType* field = (FieldType*)GetObjectPtr();
    if(field==NULL)
        return;

    uint16_t newSize = label.Length() + 1;
    MemHandle strHandle = FldGetTextHandle(field);
    FldSetTextHandle(field, NULL );
    if (strHandle)
    {
        if(MemHandleResize(strHandle, newSize)!=errNone)
            strHandle = 0;
    }
    else
    {
        strHandle = MemHandleNew( newSize );
    }
    if(!strHandle)
        return;

    char* str = (char*) MemHandleLock( strHandle );
    if(str==NULL)
        return;

    strcpy(str, label.c_str());
    MemHandleUnlock(strHandle);
    FldSetTextHandle(field, strHandle);
    FldRecalculateField(field, true);
}

void wxControl::SetControlLabel(const wxString& label)
{
}

void wxControl::SetLabel(const wxString& label)
{
    if(IsPalmField())
        SetFieldLabel(label);

    // unlike other native controls, slider has no label
    if(IsPalmControl() && !wxDynamicCast(this,wxSlider))
        SetControlLabel(label);
}

wxString wxControl::GetFieldLabel()
{
    FieldType* field = (FieldType*)GetObjectPtr();
    if(field==NULL)
        return wxEmptyString;
    return FldGetTextPtr(field);
}

wxString wxControl::GetControlLabel()
{
    ControlType* control = (ControlType*)GetObjectPtr();
    if(control==NULL)
        return wxEmptyString;
    return CtlGetLabel(control);
}

wxString wxControl::GetLabel()
{
    if(IsPalmField())
        return GetFieldLabel();

    // unlike other native controls, slider has no label
    if(IsPalmControl() && !wxDynamicCast(this,wxSlider))
        return GetControlLabel();

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
                               WXUINT WXUNUSED(message),
                               WXWPARAM WXUNUSED(wParam),
                               WXLPARAM WXUNUSED(lParam)
    )
{
    return (WXHBRUSH)0;
}

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

#endif // wxUSE_CONTROLS
