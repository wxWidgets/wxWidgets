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

#ifdef __GNUG__
#pragma implementation "checkbox.h"
#endif

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

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
    m_inSetValue = TRUE;
    XmToggleButtonSetState ((Widget) m_mainWidget, (Boolean) val, TRUE);
    m_inSetValue = FALSE;
}

bool wxCheckBox::GetValue() const
{
    return (XmToggleButtonGetState ((Widget) m_mainWidget) != 0);
}

void wxCheckBox::Command (wxCommandEvent & event)
{
    SetValue ((event.GetInt() != 0));
    ProcessCommand (event);
}

void wxCheckBoxCallback (Widget WXUNUSED(w), XtPointer clientData,
                         XtPointer WXUNUSED(ptr))
{
    wxCheckBox *item = (wxCheckBox *) clientData;

    if (item->InSetValue())
        return;

    wxCommandEvent event (item->m_evtType, item->GetId());
    event.SetInt((int) item->GetValue ());
    event.SetEventObject(item);
    item->ProcessCommand (event);
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
