/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbmp.h"
#endif

#include "wx/statbmp.h"

#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>

#include "wx/motif/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)
#endif

/*
 * wxStaticBitmap
 */

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
           const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_messageBitmap = bitmap;
    SetName(name);
    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_windowStyle = style;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    m_mainWidget = (WXWidget) XtVaCreateManagedWidget ("staticBitmap",
#if USE_GADGETS
                    xmLabelGadgetClass, parentWidget,
#else
                    xmLabelWidgetClass, parentWidget,
#endif
                    XmNalignment, XmALIGNMENT_BEGINNING,
                    NULL);

    XtVaSetValues ((Widget) m_mainWidget,
                    XmNlabelPixmap, (Pixmap) ((wxBitmap&)bitmap).GetLabelPixmap (m_mainWidget),
                    XmNlabelType, XmPIXMAP,
                    NULL);

    m_font = parent->GetFont();
    ChangeFont(FALSE);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour ();

    return TRUE;
}

wxStaticBitmap::~wxStaticBitmap()
{
    SetBitmap(wxNullBitmap);
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    m_messageBitmap = bitmap;

    Widget widget = (Widget) m_mainWidget;
    int x, y, w1, h1, w2, h2;

    GetPosition(&x, &y);

    if (bitmap.Ok())
    {
        w2 = bitmap.GetWidth();
        h2 = bitmap.GetHeight();
        XtVaSetValues (widget,
            XmNlabelPixmap, ((wxBitmap&)bitmap).GetLabelPixmap (widget),
            XmNlabelType, XmPIXMAP,
            NULL);
        GetSize(&w1, &h1);

        if (! (w1 == w2) && (h1 == h2))
            SetSize(x, y, w2, h2);
    }
    else
    {
        // Null bitmap: must not use current pixmap
        // since it is no longer valid.
        XtVaSetValues (widget,
            XmNlabelType, XmSTRING,
            XmNlabelPixmap, XmUNSPECIFIED_PIXMAP,
            NULL);
    }
}

void wxStaticBitmap::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxStaticBitmap::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();
}

void wxStaticBitmap::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

