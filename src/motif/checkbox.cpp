/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "checkbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#include "wx/checkbox.h"
#include "wx/tglbtn.h"
#include "wx/utils.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

// define symbols that are missing in old versions of Motif.
#if defined(__VMS) || (XmVersion < 2000)
#define XtDisplay XTDISPLAY
#define XmNtoggleMode 0
#define XmTOGGLE_INDETERMINATE 1
#define XmTOGGLE_BOOLEAN 2
#define XmUNSET 3
#define XmSET 4
#define XmINDETERMINATE 5
#endif


#include "wx/motif/private.h"

void wxCheckBoxCallback (Widget w, XtPointer clientData,
                         XtPointer ptr);

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)

// Single check box item
bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size, long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    if( !wxControl::CreateControl( parent, id, pos, size, style, validator,
                                   name ) )
        return FALSE;

    wxString label1(wxStripMenuCodes(label));
    wxXmString text( label1 );
    
    Widget parentWidget = (Widget) parent->GetClientWidget();

    m_mainWidget = (WXWidget) XtVaCreateManagedWidget ("toggle",
        xmToggleButtonWidgetClass, parentWidget,
        wxFont::GetFontTag(), m_font.GetFontType(XtDisplay(parentWidget)),
        XmNlabelString, text(),
        XmNrecomputeSize, False,
        // XmNindicatorOn, XmINDICATOR_CHECK_BOX,
        // XmNfillOnSelect, False,
        XmNtoggleMode, Is3State() ? XmTOGGLE_INDETERMINATE : XmTOGGLE_BOOLEAN,
        NULL);
    
    XtAddCallback( (Widget)m_mainWidget,
                   XmNvalueChangedCallback, (XtCallbackProc)wxCheckBoxCallback,
                   (XtPointer)this );

    XmToggleButtonSetState ((Widget) m_mainWidget, FALSE, TRUE);

    AttachWidget( parent, m_mainWidget, (WXWidget)NULL,
                  pos.x, pos.y, size.x, size.y );

    ChangeBackgroundColour();
    return TRUE;
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

void wxCheckBox::Command (wxCommandEvent & event)
{
    int state = event.GetInt();
    wxCHECK_RET( (state == wxCHK_UNCHECKED) || (state == wxCHK_CHECKED)
        || (state == wxCHK_UNDETERMINED),
        wxT("event.GetInt() returned an invalid checkbox state") );

    Set3StateValue((wxCheckBoxState) state);
    ProcessCommand(event);
}

void wxCheckBoxCallback (Widget WXUNUSED(w), XtPointer clientData,
                         XtPointer WXUNUSED(ptr))
{
    wxCheckBox *item = (wxCheckBox *) clientData;

    if (item->InSetValue())
        return;

    wxCheckBoxState state = item->Get3StateValue();

    if( !item->Is3rdStateAllowedForUser() && state == wxCHK_UNDETERMINED )
    {
        state = wxCHK_UNCHECKED;
        item->Set3StateValue( state );
    }

    wxCommandEvent event( item->m_evtType, item->GetId() );
    event.SetInt( (int)state );
    event.SetEventObject( item );
    item->ProcessCommand( event );
}

void wxCheckBox::ChangeBackgroundColour()
{
    wxComputeColours (XtDisplay((Widget) m_mainWidget), & m_backgroundColour,
        (wxColour*) NULL);

    XtVaSetValues ((Widget) m_mainWidget,
        XmNbackground, g_itemColors[wxBACK_INDEX].pixel,
        XmNtopShadowColor, g_itemColors[wxTOPS_INDEX].pixel,
        XmNbottomShadowColor, g_itemColors[wxBOTS_INDEX].pixel,
        XmNforeground, g_itemColors[wxFORE_INDEX].pixel,
        NULL);

    int selectPixel = wxBLACK->AllocColour(XtDisplay((Widget)m_mainWidget));

    // Better to have the checkbox selection in black, or it's
    // hard to determine what state it is in.
    XtVaSetValues ((Widget) m_mainWidget,
           XmNselectColor, selectPixel,
        NULL);
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState state)
{
    m_inSetValue = true;

    unsigned char value;

    switch (state)
    {
    case wxCHK_UNCHECKED: value = XmUNSET; break;
    case wxCHK_CHECKED: value = XmSET; break;
    case wxCHK_UNDETERMINED: value = XmINDETERMINATE; break;
    default: wxASSERT(0); return;
    }

    XtVaSetValues( (Widget) m_mainWidget,
                   XmNset, value,
                   NULL );

    m_inSetValue = false;
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    unsigned char value = 0;

    XtVaGetValues( (Widget) m_mainWidget,
                   XmNset, &value,
                   NULL );

    switch (value)
    {
    case XmUNSET: return wxCHK_UNCHECKED;
    case XmSET: return wxCHK_CHECKED;
    case XmINDETERMINATE: return wxCHK_UNDETERMINED;
    }

    // impossible...
    return wxCHK_UNDETERMINED;
}

///////////////////////////////////////////////////////////////////////////////
// wxToggleButton
///////////////////////////////////////////////////////////////////////////////

#if wxUSE_TOGGLEBTN

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED)
IMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl)

bool wxToggleButton::Create( wxWindow* parent, wxWindowID id,
                             const wxString& label,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxValidator& val,
                             const wxString &name )
{
    if( !wxCheckBox::Create( parent, id, label, pos, size, style, val, name ) )
        return false;

    XtVaSetValues( (Widget)m_mainWidget,
                   XmNindicatorSize, 0,
#if XmVersion >= 2000
                   XmNindicatorOn, XmINDICATOR_NONE,
#else
                   XmNindicatorOn, False,
#endif
                   XmNfillOnSelect, False,
                   XmNshadowThickness, 2,
                   XmNalignment, XmALIGNMENT_CENTER,
                   XmNmarginLeft, 0,
                   XmNmarginRight, 0,
                   NULL );

    // set it again, because the XtVaSetValue above resets it
    if( size.x != -1 || size.y != -1 )
        SetSize( size );

    return true;
}

#endif // wxUSE_TOGGLEBUTTON
