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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "stattext.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

#include "wx/defs.h"
#include "wx/stattext.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Label.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl);

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    if( !CreateControl( parent, id, pos, size, style,
                        wxDefaultValidator, name ) )
        return false;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    Widget borderWidget =
        (Widget) wxCreateBorderWidget( (WXWidget)parentWidget, style );
    wxXmString text( wxStripMenuCodes( label ) );
    WXFontType fontType = m_font.GetFontType(XtDisplay(parentWidget));

    m_labelWidget = XtVaCreateManagedWidget (wxConstCast(name.c_str(), char),
                                         xmLabelWidgetClass,
                                         borderWidget ? borderWidget : parentWidget,
                                         wxFont::GetFontTag(), fontType,
                                         XmNlabelString, text(),
                                         XmNalignment,
                     ((style & wxALIGN_RIGHT) ? XmALIGNMENT_END :
                     ((style & wxALIGN_CENTRE) ? XmALIGNMENT_CENTER :
                     XmALIGNMENT_BEGINNING)),
                                         NULL);

    m_mainWidget = borderWidget ? borderWidget : m_labelWidget;

    AttachWidget (parent, m_mainWidget, (WXWidget) NULL,
                  pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour ();

    return TRUE;
}

void wxStaticText::SetLabel(const wxString& label)
{
    wxXmString label_str(wxStripMenuCodes(label));

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
