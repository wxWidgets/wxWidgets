/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbox.h"
#endif

#include "wx/statbox.h"
#include "wx/utils.h"

#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>

#include <wx/motif/private.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

BEGIN_EVENT_TABLE(wxStaticBox, wxControl)
	EVT_ERASE_BACKGROUND(wxStaticBox::OnEraseBackground)
END_EVENT_TABLE()

#endif

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
        wxString label1(wxStripMenuCodes(label));
        XmString text = XmStringCreateSimple ((char*) (const char*) label1);
        m_labelWidget = (WXWidget) XtVaCreateManagedWidget ((char*) (const char*) label1,
					     xmLabelWidgetClass, formWidget,
					     XmNlabelString, text,
					     NULL);
        XmStringFree (text);
    }

    Widget frameWidget = XtVaCreateManagedWidget ("frame",
					xmFrameWidgetClass, formWidget,
                                        XmNshadowType, XmSHADOW_IN,
//					XmNmarginHeight, 0,
//					XmNmarginWidth, 0,
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

    m_mainWidget = (Widget) formWidget;

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) frameWidget, pos.x, pos.y, size.x, size.y);
    ChangeColour(m_mainWidget);

    return TRUE;
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

void wxStaticBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::SetSize (x, y, width, height, sizeFlags);

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

