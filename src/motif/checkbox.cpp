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
    XmFontList fontList =
        (XmFontList) m_font.GetFontList(1.0, XtDisplay(parentWidget));

    m_mainWidget = (WXWidget) XtVaCreateManagedWidget ("toggle",
        xmToggleButtonWidgetClass, parentWidget,
        XmNfontList, fontList,
        XmNlabelString, text(),
        NULL);
    
    XtAddCallback( (Widget)m_mainWidget,
                   XmNvalueChangedCallback, (XtCallbackProc)wxCheckBoxCallback,
                   (XtPointer)this );

    XmToggleButtonSetState ((Widget) m_mainWidget, FALSE, TRUE);

    SetCanAddEventHandler(TRUE);
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

    wxCommandEvent event (wxEVT_COMMAND_CHECKBOX_CLICKED, item->GetId());
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

    int selectPixel = wxBLACK->AllocColour(wxGetDisplay());

    // Better to have the checkbox selection in black, or it's
    // hard to determine what state it is in.
    XtVaSetValues ((Widget) m_mainWidget,
           XmNselectColor, selectPixel,
        NULL);
}
