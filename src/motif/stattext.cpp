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

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

#include "wx/app.h"
#include "wx/stattext.h"

#include <stdio.h>

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)

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

#if 0  // gcc 2.95 doesn't like this apparently    
    char* label1 = (label.IsNull() ? "" : (char*) (const char*) label);
#endif
    
    Widget parentWidget = (Widget) parent->GetClientWidget();

    Widget borderWidget = NULL;
    
    // Decorate the label widget if a border style is specified.
    if (style & wxSIMPLE_BORDER)
    {
        borderWidget = XtVaCreateManagedWidget
                                   (
                                    "simpleBorder",
                                    xmFrameWidgetClass, parentWidget,
                                    XmNshadowType, XmSHADOW_ETCHED_IN,
                                    XmNshadowThickness, 1,
                                    NULL
                                   );
    } else if (style & wxSUNKEN_BORDER)
    {
        borderWidget = XtVaCreateManagedWidget
                                   (
                                    "sunkenBorder",
                                    xmFrameWidgetClass, parentWidget,
                                    XmNshadowType, XmSHADOW_IN,
                                    NULL
                                   );
    } else if (style & wxRAISED_BORDER)
    {
        borderWidget = XtVaCreateManagedWidget
                                   (
                                    "raisedBorder",
                                    xmFrameWidgetClass, parentWidget,
                                    XmNshadowType, XmSHADOW_OUT,
                                    NULL
                                   );
    }

#if 0 // gcc 2.95 doesn't like this apparently    
    // Use XmStringCreateLtoR(), since XmStringCreateSimple
    // doesn't obey separators.
//    XmString text = XmStringCreateSimple (label1);
    XmString text = XmStringCreateLtoR (label1, XmSTRING_DEFAULT_CHARSET);
#endif // 0
    
    XmString text = XmStringCreateLtoR ((char *)(const char*)label, XmSTRING_DEFAULT_CHARSET);
    
    XmFontList fontList = (XmFontList) m_font.GetFontList(1.0, XtDisplay(parentWidget));

    m_labelWidget = XtVaCreateManagedWidget ((char*) (const char*) name,
                                         xmLabelWidgetClass,
                                         borderWidget ? borderWidget : parentWidget,
                                         XmNfontList, fontList,
                                         XmNlabelString, text,
                                         XmNalignment,
                     ((style & wxALIGN_RIGHT) ? XmALIGNMENT_END :
                     ((style & wxALIGN_CENTRE) ? XmALIGNMENT_CENTER :
                     XmALIGNMENT_BEGINNING)),
                                         NULL);

    XmStringFree (text);

    m_mainWidget = borderWidget ? borderWidget : m_labelWidget;

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

void wxStaticText::SetLabel(const wxString& label)
{
    wxString buf(wxStripMenuCodes(label));
    wxXmString label_str(buf);

    // This variable means we don't need so many casts later.
    Widget widget = (Widget) m_labelWidget;

    if (GetWindowStyle() & wxST_NO_AUTORESIZE)
    {
        XtUnmanageChild(widget);
        Dimension width, height;
        XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);

        XtVaSetValues(widget,
            XmNlabelString, label_str(),
            XmNlabelType, XmSTRING,
            NULL);
        XtVaSetValues(widget,
            XmNwidth, width,
            XmNheight, height,
            NULL);        
        XtManageChild(widget);
    }
    else
    {
        XtVaSetValues(widget,
            XmNlabelString, label_str(),
            XmNlabelType, XmSTRING,
            NULL);
    }
}

