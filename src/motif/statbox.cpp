/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbox.h"
#endif

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

#include "wx/defs.h"

#include "wx/statbox.h"
#include "wx/utils.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

BEGIN_EVENT_TABLE(wxStaticBox, wxControl)
//EVT_ERASE_BACKGROUND(wxStaticBox::OnEraseBackground)
END_EVENT_TABLE()


/*
 * Static box
 */

wxStaticBox::wxStaticBox()
{
    m_labelWidget = (WXWidget) 0;
}

bool wxStaticBox::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    m_font = parent->GetFont();

    SetName(name);

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_windowStyle = style;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    m_mainWidget = XtVaCreateManagedWidget ("staticboxframe",
            xmFrameWidgetClass, parentWidget,
            XmNshadowType, XmSHADOW_IN,
            //XmNmarginHeight, 0,
            //XmNmarginWidth, 0,
            NULL);

    bool hasLabel = (!label.IsNull() && !label.IsEmpty()) ;
    if (hasLabel)
    {
        XmFontList fontList = (XmFontList) m_font.GetFontList(1.0, XtDisplay(parentWidget));
        wxString label1(wxStripMenuCodes(label));
        wxXmString text(label1);
        m_labelWidget = (WXWidget) XtVaCreateManagedWidget (label1.c_str(),
                xmLabelWidgetClass, (Widget)m_mainWidget,
                XmNfontList, fontList,
                XmNlabelString, text(),
#if (XmVersion > 1200)
                XmNframeChildType, XmFRAME_TITLE_CHILD,
#else
                XmNchildType, XmFRAME_TITLE_CHILD,          
#endif
                NULL);
    }
    
    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, NULL, pos.x, pos.y, size.x, size.y);
    ChangeBackgroundColour();

    return TRUE;
}

wxStaticBox::~wxStaticBox()
{
   DetachWidget(m_mainWidget);
   XtDestroyWidget((Widget) m_mainWidget);

   m_mainWidget = (WXWidget) 0;
   m_labelWidget = (WXWidget) 0;
}

void wxStaticBox::SetLabel(const wxString& label)
{
    if (!m_labelWidget)
        return;

    if (!label.IsNull())
    {
        wxString label1(wxStripMenuCodes(label));

        wxXmString text(label1);
        XtVaSetValues ((Widget) m_labelWidget,
                XmNlabelString, text(),
                XmNlabelType, XmSTRING,
                NULL);
    }
}

wxString wxStaticBox::GetLabel() const
{
    if (!m_labelWidget)
        return wxEmptyString;

    XmString text = 0;
    char *s;
    XtVaGetValues ((Widget) m_labelWidget,
            XmNlabelString, &text,
            NULL);

    if (!text)
        return wxEmptyString;

    if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
    {
        wxString str(s);
        XtFree (s);
        return str;
    }
    else
    {
        return wxEmptyString;
    }
}

void wxStaticBox::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}


