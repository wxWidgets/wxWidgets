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
    m_labelWidget = (WXWidget) 0;
    m_formWidget = (WXWidget) 0;
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

    if (parent) parent->AddChild(this);

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    m_windowStyle = style ;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    wxString label1(wxStripMenuCodes(label));

    XmString text = XmStringCreateSimple ((char*) (const char*) label1);

    Widget formWidget = XtVaCreateManagedWidget ((char*) (const char*) name,
					xmFormWidgetClass, parentWidget,
					XmNmarginHeight, 0,
					XmNmarginWidth, 0,
					NULL);

    m_formWidget = (WXWidget) formWidget;

    Widget labelWidget = XtVaCreateManagedWidget ((char*) (const char*) label1,
#if wxUSE_GADGETS
				        xmLabelGadgetClass,
					    formWidget,
#else
					    xmLabelWidgetClass, formWidget,
#endif
					    XmNlabelString, text,
					    NULL);
    m_labelWidget = (WXWidget) labelWidget;
/* TODO
      if (labelFont)
	XtVaSetValues (labelWidget,
		       XmNfontList, labelFont->GetInternalFont (XtDisplay(formWidget)),
		       NULL);
*/

    XmStringFree (text);

    Widget radioButtonWidget = XtVaCreateManagedWidget ("toggle",
#if wxUSE_GADGETS
                    xmToggleButtonGadgetClass, formWidget,
#else
                    xmToggleButtonWidgetClass, formWidget,
#endif
						 NULL);
    XtAddCallback (radioButtonWidget, XmNvalueChangedCallback, (XtCallbackProc) wxRadioButtonCallback,
		     (XtCallbackProc) this);

    m_mainWidget = (WXWidget) radioButtonWidget;

/* TODO
  if (labelFont)
   XtVaSetValues (radioButtonWidget,
                  XmNfontList, labelFont->GetInternalFont (XtDisplay(formWidget)),
                  NULL);
*/

    if (labelWidget)
	    XtVaSetValues (labelWidget,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNbottomAttachment, XmATTACH_FORM,
		       XmNalignment, XmALIGNMENT_BEGINNING,
		       NULL);
    XtVaSetValues (radioButtonWidget,
		     XmNleftOffset, 4,
		     XmNtopAttachment, XmATTACH_FORM,
		     XmNbottomAttachment, XmATTACH_FORM,
	   XmNleftAttachment, (Widget) m_labelWidget ? XmATTACH_WIDGET : XmATTACH_FORM,
		     XmNleftWidget, (Widget) m_labelWidget ? (Widget) m_labelWidget : formWidget,
		     NULL);

    XtManageChild (radioButtonWidget);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, m_formWidget, pos.x, pos.y, size.x, size.y);

    SetFont(* parent->GetFont());
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

void wxRadioButton::ChangeFont()
{
    // TODO
}

void wxRadioButton::ChangeBackgroundColour()
{
    // TODO
}

void wxRadioButton::ChangeForegroundColour()
{
    // TODO
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

