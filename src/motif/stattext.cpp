/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:     wxStaticText
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

#include "wx/app.h"
#include "wx/stattext.h"

#include <stdio.h>

#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)
#endif

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    SetName(name);
    if (parent) parent->AddChild(this);

    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();

    if ( id == -1 )
              m_windowId = (int)NewControlId();
    else
            m_windowId = id;

    m_windowStyle = style;
    m_font = parent->GetFont();

    char* label1 = (label.IsNull() ? "" : (char*) (const char*) label);

    Widget parentWidget = (Widget) parent->GetClientWidget();

    // Use XmStringCreateLtoR(), since XmStringCreateSimple
    // doesn't obey separators.
//    XmString text = XmStringCreateSimple (label1);
    XmString text = XmStringCreateLtoR (label1, XmSTRING_DEFAULT_CHARSET);

    XmFontList fontList = (XmFontList) m_font.GetFontList(1.0, XtDisplay(parentWidget));

    m_mainWidget = (WXWidget) XtVaCreateManagedWidget ((char*) (const char*) name,
                                         xmLabelWidgetClass,
                                         parentWidget,
                                         XmNfontList, fontList,
                                         XmNlabelString, text,
                                         XmNalignment,
                     ((style & wxALIGN_RIGHT) ? XmALIGNMENT_END :
                     ((style & wxALIGN_CENTRE) ? XmALIGNMENT_CENTER :
                     XmALIGNMENT_BEGINNING)),
                                         NULL);

    XmStringFree (text);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour ();

    return TRUE;
}

void wxStaticText::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxStaticText::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();
}

void wxStaticText::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

