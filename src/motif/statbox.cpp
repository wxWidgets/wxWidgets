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
    m_formWidget = (WXWidget) 0;
    m_labelWidget = (WXWidget) 0;
}

bool wxStaticBox::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_formWidget = (WXWidget) 0;
    m_labelWidget = (WXWidget) 0;
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

    bool hasLabel = (!label.IsNull() && !label.IsEmpty()) ;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    Widget formWidget = XtVaCreateManagedWidget ((char*) (const char*) name,
            xmFormWidgetClass, parentWidget,
            XmNmarginHeight, 0,
            XmNmarginWidth, 0,
            NULL);


    if (hasLabel)
    {
        XmFontList fontList = (XmFontList) m_font.GetFontList(1.0, XtDisplay(parentWidget));

        wxString label1(wxStripMenuCodes(label));
        XmString text = XmStringCreateSimple ((char*) (const char*) label1);
        m_labelWidget = (WXWidget) XtVaCreateManagedWidget ((char*) (const char*) label1,
                xmLabelWidgetClass, formWidget,
                XmNfontList, fontList,
                XmNlabelString, text,
                NULL);
        XmStringFree (text);
    }

    Widget frameWidget = XtVaCreateManagedWidget ("frame",
            xmFrameWidgetClass, formWidget,
            XmNshadowType, XmSHADOW_IN,
            //XmNmarginHeight, 0,
            //XmNmarginWidth, 0,
            NULL);

    if (hasLabel)
        XtVaSetValues ((Widget) m_labelWidget,
                XmNtopAttachment, XmATTACH_FORM,
                XmNleftAttachment, XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNalignment, XmALIGNMENT_BEGINNING,
                NULL);

    XtVaSetValues (frameWidget,
            XmNtopAttachment, hasLabel ? XmATTACH_WIDGET : XmATTACH_FORM,
            XmNtopWidget, hasLabel ? (Widget) m_labelWidget : formWidget,
            XmNbottomAttachment, XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            NULL);

    m_mainWidget = (WXWidget) frameWidget;
    m_formWidget = (WXWidget) formWidget;

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, (WXWidget) frameWidget, (WXWidget) formWidget, pos.x, pos.y, size.x, size.y);
    ChangeBackgroundColour();

    return TRUE;
}

wxStaticBox::~wxStaticBox()
{
   DetachWidget(m_formWidget);
   DetachWidget(m_mainWidget);
   XtDestroyWidget((Widget) m_mainWidget);
   if (m_labelWidget)
     XtDestroyWidget((Widget) m_labelWidget);
   XtDestroyWidget((Widget) m_formWidget);

   m_mainWidget = (WXWidget) 0;
   m_labelWidget = (WXWidget) 0;
   m_formWidget = (WXWidget) 0;
}

void wxStaticBox::SetLabel(const wxString& label)
{
    if (!m_labelWidget)
        return;

    if (!label.IsNull())
    {
        wxString label1(wxStripMenuCodes(label));

        XmString text = XmStringCreateSimple ((char*) (const char*) label1);
        XtVaSetValues ((Widget) m_labelWidget,
                XmNlabelString, text,
                XmNlabelType, XmSTRING,
                NULL);
        XmStringFree (text);
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

void wxStaticBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::DoSetSize (x, y, width, height, sizeFlags);

    if (m_labelWidget)
    {
        Dimension xx, yy;
        XtVaGetValues ((Widget) m_labelWidget, XmNwidth, &xx, XmNheight, &yy, NULL);

        if (width > -1)
            XtVaSetValues ((Widget) m_mainWidget, XmNwidth, width,
                    NULL);
        if (height > -1)
            XtVaSetValues ((Widget) m_mainWidget, XmNheight, height - yy,
                    NULL);
    }
}

void wxStaticBox::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxStaticBox::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();
    if (m_labelWidget)
        DoChangeBackgroundColour(m_labelWidget, m_backgroundColour);
}

void wxStaticBox::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
    if (m_labelWidget)
        DoChangeForegroundColour(m_labelWidget, m_foregroundColour);
}

