/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:     wxRadioButton
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "radiobut.h"
#endif

#include "wx/radiobut.h"
#include "wx/utils.h"

#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>

#include <wx/motif/private.h>

void wxRadioButtonCallback (Widget w, XtPointer clientData,
                            XmToggleButtonCallbackStruct * cbs);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)
#endif

wxRadioButton::wxRadioButton()
{
}

bool wxRadioButton::Create(wxWindow *parent, wxWindowID id,
                           const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size, long style,
                           const wxValidator& validator,
                           const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    m_windowFont = parent->GetFont();
    
    if (parent) parent->AddChild(this);
    
    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;
    
    m_windowStyle = style ;
    
    Widget parentWidget = (Widget) parent->GetClientWidget();
    
    wxString label1(wxStripMenuCodes(label));
    
    XmString text = XmStringCreateSimple ((char*) (const char*) label1);
    
    XmFontList fontList = (XmFontList) m_windowFont.GetFontList(1.0, XtDisplay(parentWidget));
    
    Widget radioButtonWidget = XtVaCreateManagedWidget ("toggle",
#if wxUSE_GADGETS
        xmToggleButtonGadgetClass, parentWidget,
#else
        xmToggleButtonWidgetClass, parentWidget,
#endif
        XmNfontList, fontList,
        XmNlabelString, text,
        XmNfillOnSelect, True,
        XmNindicatorType, XmONE_OF_MANY, // diamond-shape
        NULL);
    XmStringFree (text);
    
    XtAddCallback (radioButtonWidget, XmNvalueChangedCallback, (XtCallbackProc) wxRadioButtonCallback,
        (XtCallbackProc) this);
    
    m_mainWidget = (WXWidget) radioButtonWidget;
    
    XtManageChild (radioButtonWidget);
    
    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);
    
    ChangeBackgroundColour();
    
    return TRUE;
}

void wxRadioButton::SetValue(bool value)
{
    m_inSetValue = TRUE;
    XmToggleButtonSetState ((Widget) m_mainWidget, (Boolean) value, FALSE);
    m_inSetValue = FALSE;
}

// Get single selection, for single choice list items
bool wxRadioButton::GetValue() const
{
    return (XmToggleButtonGetState ((Widget) m_mainWidget) != 0);
}

void wxRadioButton::Command (wxCommandEvent & event)
{
    SetValue ( (event.m_commandInt != 0) );
    ProcessCommand (event);
}

void wxRadioButton::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxRadioButton::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();
}

void wxRadioButton::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

void wxRadioButtonCallback (Widget w, XtPointer clientData,
                            XmToggleButtonCallbackStruct * cbs)
{
    if (!cbs->set)
        return;
    
    wxRadioButton *item = (wxRadioButton *) clientData;
    if (item->InSetValue())
        return;
    
    wxCommandEvent event (wxEVT_COMMAND_RADIOBUTTON_SELECTED, item->GetId());
    event.SetEventObject(item);
    
    item->ProcessCommand (event);
}

