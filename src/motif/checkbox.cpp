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

#include "wx/checkbox.h"

#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include "wx/motif/private.h"

void wxCheckBoxCallback (Widget w, XtPointer clientData,
		    XtPointer ptr);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)
#endif

// Single check box item
bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_windowStyle = style;
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    m_windowFont = parent->GetFont();

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = NewControlId();
    else
        m_windowId = id;

    char* label1 = (label.IsNull() ? "" : (char*) (const char*) label);

    XmString text = XmStringCreateSimple (label1);
    Widget parentWidget = (Widget) parent->GetClientWidget();
    XmFontList fontList = (XmFontList) m_windowFont.GetFontList(1.0, XtDisplay(parentWidget));

    m_mainWidget = (WXWidget) XtVaCreateManagedWidget ("toggle",
                                xmToggleButtonWidgetClass, parentWidget,
                                XmNfontList, fontList,
                                XmNlabelString, text,
                                NULL);
    XmStringFree (text);

    XtAddCallback ((Widget) m_mainWidget, XmNvalueChangedCallback, (XtCallbackProc) wxCheckBoxCallback,
		 (XtPointer) this);

    XmToggleButtonSetState ((Widget) m_mainWidget, FALSE, TRUE);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

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

// Bitmap checkbox
bool wxBitmapCheckBox::Create(wxWindow *parent, wxWindowID id, const wxBitmap *label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_windowStyle = style;

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = NewControlId();
    else
        m_windowId = id;

    // TODO: Create the bitmap checkbox

    return FALSE;
}

void wxBitmapCheckBox::SetLabel(const wxBitmap *bitmap)
{
    // TODO
}

void wxBitmapCheckBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxBitmapCheckBox::SetValue(bool val)
{
    // TODO
}

bool wxBitmapCheckBox::GetValue() const
{
    // TODOD
    return FALSE;
}

void wxCheckBoxCallback (Widget w, XtPointer clientData,
		    XtPointer ptr)
{
  wxCheckBox *item = (wxCheckBox *) clientData;

  if (item->InSetValue())
    return;
    
  wxCommandEvent event (wxEVT_COMMAND_CHECKBOX_CLICKED, item->GetId());
  event.SetInt((int) item->GetValue ());
  event.SetEventObject(item);
  item->ProcessCommand (event);
}

void wxCheckBox::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
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

  XtVaSetValues ((Widget) m_mainWidget,
    		   XmNselectColor, g_itemColors[wxSELE_INDEX].pixel,
               NULL);
}

void wxCheckBox::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}
