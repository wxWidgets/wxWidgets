/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpbuttn.h"
#endif

#include "wx/bmpbuttn.h"

#include <Xm/PushBG.h>
#include <Xm/PushB.h>

#include "wx/motif/private.h"

// Implemented in button.cpp
void wxButtonCallback (Widget w, XtPointer clientData, XtPointer ptr);

Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap );

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)
#endif

wxBitmapButton::wxBitmapButton()
{
    m_marginX = wxDEFAULT_BUTTON_MARGIN; m_marginY = wxDEFAULT_BUTTON_MARGIN;
    m_insensPixmap = (WXPixmap) 0;
}

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_buttonBitmap = bitmap;
    SetName(name);
    SetValidator(validator);
    parent->AddChild(this);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;
    m_windowStyle = style;
    m_marginX = 0;
    m_marginY = 0;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    Widget parentWidget = (Widget) parent->GetClientWidget();

  /*
   * Patch Note (important)
   * There is no major reason to put a defaultButtonThickness here.
   * Not requesting it give the ability to put wxButton with a spacing
   * as small as requested. However, if some button become a DefaultButton,
   * other buttons are no more aligned -- This is why we set
   * defaultButtonThickness of ALL buttons belonging to the same wxPanel,
   * in the ::SetDefaultButton method.
   */
    Widget buttonWidget = XtVaCreateManagedWidget ("button",

    // Gadget causes problems for default button operation.
#if wxUSE_GADGETS
			    xmPushButtonGadgetClass, parentWidget,
#else
                xmPushButtonWidgetClass, parentWidget,
#endif
//                  XmNdefaultButtonShadowThickness, 1, // See comment for wxButton::SetDefault
						 NULL);

    m_mainWidget = (WXWidget) buttonWidget;

    if (bitmap.Ok())
    {
        Pixmap p1, p2;

        p1 = (Pixmap) ((wxBitmap&)bitmap).GetLabelPixmap(m_mainWidget);
        p2 = (Pixmap) ((wxBitmap&)bitmap).GetInsensPixmap(m_mainWidget);

        if(p1 == p2) // <- the Get...Pixmap()-functions return the same pixmap!
        {
            p2 =
                XCreateInsensitivePixmap(DisplayOfScreen(XtScreen(buttonWidget)), p1);
            m_insensPixmap = (WXPixmap) p2;
        }

        XtVaSetValues (buttonWidget,
            XmNlabelPixmap, p1,
            XmNlabelInsensitivePixmap, p2,
            XmNarmPixmap, (Pixmap) ((wxBitmap&)bitmap).GetArmPixmap (m_mainWidget),
            XmNlabelType, XmPIXMAP,
            NULL);
    }

    XtAddCallback (buttonWidget, XmNactivateCallback, (XtCallbackProc) wxButtonCallback,
            (XtPointer) this);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    SetFont(* parent->GetFont());

    ChangeBackgroundColour ();


    return TRUE;
}

wxBitmapButton::~wxBitmapButton()
{
    SetBitmapLabel(wxNullBitmap);

    if (m_insensPixmap)
        XmDestroyPixmap (DefaultScreenOfDisplay ((Display*) GetXDisplay()), (Pixmap) m_insensPixmap);
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
    m_buttonBitmap = bitmap;

    if (bitmap.Ok())
    {
        Pixmap labelPixmap, insensPixmap, armPixmap;

        labelPixmap = (Pixmap) ((wxBitmap&)bitmap).GetLabelPixmap(m_mainWidget);

        if (m_buttonBitmapSelected.Ok())
            armPixmap = (Pixmap) m_buttonBitmapSelected.GetLabelPixmap(m_mainWidget);
        else
            armPixmap = (Pixmap) ((wxBitmap&)bitmap).GetArmPixmap(m_mainWidget);

        if (m_buttonBitmapDisabled.Ok())
            insensPixmap = (Pixmap) m_buttonBitmapDisabled.GetLabelPixmap(m_mainWidget);
        else
            insensPixmap = (Pixmap) ((wxBitmap&)bitmap).GetInsensPixmap(m_mainWidget);

        if (!insensPixmap || (insensPixmap == labelPixmap)) // <- the Get...Pixmap()-functions return the same pixmap!
        {
            insensPixmap = XCreateInsensitivePixmap(DisplayOfScreen(XtScreen((Widget) m_mainWidget)), labelPixmap);
            m_insensPixmap = (WXPixmap) insensPixmap;
        }

        XtVaSetValues ((Widget) m_mainWidget,
            XmNlabelPixmap, labelPixmap,
    	    XmNlabelInsensitivePixmap, insensPixmap,
            XmNarmPixmap, armPixmap,
            XmNlabelType, XmPIXMAP,
            NULL);
    }
    else
    {
        // Null bitmap: must not use current pixmap
        // since it is no longer valid.
        XtVaSetValues ((Widget) m_mainWidget,
            XmNlabelType, XmSTRING,
            XmNlabelPixmap, XmUNSPECIFIED_PIXMAP,
    	    XmNlabelInsensitivePixmap, NULL,
            XmNarmPixmap, NULL,
            NULL);
    }
}

void wxBitmapButton::SetBitmapSelected(const wxBitmap& sel)
{
    m_buttonBitmapSelected = sel;
};

void wxBitmapButton::SetBitmapFocus(const wxBitmap& focus)
{
    m_buttonBitmapFocus = focus;
    // Not used in Motif
};

void wxBitmapButton::SetBitmapDisabled(const wxBitmap& disabled)
{
    m_buttonBitmapDisabled = disabled;
};


