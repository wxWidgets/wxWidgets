/////////////////////////////////////////////////////////////////////////////
// Name:        control.cpp
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "control.h"
#endif

#include "wx/control.h"
#include "wx/panel.h"
#include "wx/utils.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
END_EVENT_TABLE()
#endif

// Item members
wxControl::wxControl()
{
    m_backgroundColour = *wxWHITE;
    m_foregroundColour = *wxBLACK;

#if WXWIN_COMPATIBILITY
    m_callback = 0;
#endif // WXWIN_COMPATIBILITY

    m_inSetValue = FALSE;
}

wxControl::~wxControl()
{
    // If we delete an item, we should initialize the parent panel,
    // because it could now be invalid.
    wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
    if (panel)
    {
        if (panel->GetDefaultItem() == this)
            panel->SetDefaultItem((wxButton*) NULL);
    }
}

void wxControl::SetLabel(const wxString& label)
{
    Widget widget = (Widget) GetLabelWidget() ;
    if (!widget)
        return;

    wxStripMenuCodes((char*) (const char*) label, wxBuffer);

    XmString text = XmStringCreateSimple (wxBuffer);
    XtVaSetValues (widget,
        XmNlabelString, text,
        XmNlabelType, XmSTRING,
        NULL);
    XmStringFree (text);
}

wxString wxControl::GetLabel() const
{
    Widget widget = (Widget) GetLabelWidget() ;
    if (!widget)
        return wxEmptyString;

    XmString text;
    char *s;
    XtVaGetValues (widget,
        XmNlabelString, &text,
        NULL);

    if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
    {
        wxString str(s);
        XtFree (s);
        XmStringFree(text);
        return str;
    }
    else
    {
      //        XmStringFree(text);
        return wxEmptyString;
    }
}

bool wxControl::ProcessCommand(wxCommandEvent & event)
{
#if WXWIN_COMPATIBILITY
    if ( m_callback )
    {
        (void)(*m_callback)(this, event);

        return TRUE;
    }
    else
#endif // WXWIN_COMPATIBILITY

    return GetEventHandler()->ProcessEvent(event);
}
