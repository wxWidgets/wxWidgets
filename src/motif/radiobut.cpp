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

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

#include "wx/defs.h"

#include "wx/radiobut.h"
#include "wx/utils.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

void wxRadioButtonCallback (Widget w, XtPointer clientData,
                            XmToggleButtonCallbackStruct * cbs);

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)

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
    m_font = parent->GetFont();

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_windowStyle = style ;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    wxString label1(wxStripMenuCodes(label));

    XmString text = XmStringCreateSimple ((char*) (const char*) label1);

    XmFontList fontList = (XmFontList) m_font.GetFontList(1.0, XtDisplay(parentWidget));

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
        (XtPointer) this);

    m_mainWidget = (WXWidget) radioButtonWidget;

    XtManageChild (radioButtonWidget);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour();

    //copied from mac/radiobut.cpp (from here till "return TRUE;")
    m_cycle = this ;
  
    if (HasFlag(wxRB_GROUP))
    {
        AddInCycle( NULL ) ;
    }
    else
    {
        /* search backward for last group start */
        wxRadioButton *chief = (wxRadioButton*) NULL;
        wxWindowList::Node *node = parent->GetChildren().GetLast();
        while (node)
        {
            wxWindow *child = node->GetData();
            if (child->IsKindOf( CLASSINFO( wxRadioButton ) ) )
            {
                chief = (wxRadioButton*) child;
                if (child->HasFlag(wxRB_GROUP)) break;
            }
            node = node->GetPrevious();
        }
        AddInCycle( chief ) ;
    }
    return TRUE;
}

void wxRadioButton::SetValue(bool value)
{
    if (GetValue() == value)
        return;

    m_inSetValue = TRUE;
    XmToggleButtonSetState ((Widget) m_mainWidget, (Boolean) value, FALSE);
    m_inSetValue = FALSE;

    ClearSelections();
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

    // What colour should this be?
    int selectPixel = wxBLACK->AllocColour(wxGetDisplay());

    XtVaSetValues ((Widget) GetMainWidget(),
          XmNselectColor, selectPixel,
          NULL);
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

    //based on mac/radiobut.cpp
    wxRadioButton* old = item->ClearSelections();
    item->SetValue(TRUE);

    if ( old )
    {
        wxCommandEvent event(wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                             old->GetId() );
        event.SetEventObject(old);
        event.SetInt( FALSE );
        old->ProcessCommand(event);
    }
    wxCommandEvent event2(wxEVT_COMMAND_RADIOBUTTON_SELECTED, item->GetId() );
    event2.SetEventObject(item);
    event2.SetInt( TRUE );
    item->ProcessCommand(event2);
}

wxRadioButton* wxRadioButton::AddInCycle(wxRadioButton *cycle)
{
    wxRadioButton* next;
    wxRadioButton* current;
	
    if (cycle == NULL)
    {
        m_cycle = this;
        return this;
    }
    else
    {
        current = cycle;
        while ((next = current->m_cycle) != cycle) 
            current = current->m_cycle;
        m_cycle = cycle;
        current->m_cycle = this;
        return cycle;
    }
}

wxRadioButton* wxRadioButton::ClearSelections()
{
    wxRadioButton* cycle = NextInCycle();
    wxRadioButton* old = 0;

    if (cycle)
    {
        while (cycle != this)
        {
            if ( cycle->GetValue() )
            {
                old = cycle;
                cycle->SetValue(FALSE);
            }
            cycle = cycle->NextInCycle();
        }
    }

    return old;
}

void wxRadioButton::RemoveFromCycle()
{
    wxRadioButton* curr = NextInCycle();

    while( curr )
    {
        if( curr->NextInCycle() == this )
        {
            curr->m_cycle = this->m_cycle;
            return;
        }

        curr = curr->NextInCycle();
    }
}
