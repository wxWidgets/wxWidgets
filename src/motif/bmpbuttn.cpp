/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpbuttn.h"
#endif

#include "wx/bmpbuttn.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

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
    m_buttonBitmapOriginal = bitmap;
    m_buttonBitmapSelected = bitmap;
    m_buttonBitmapSelectedOriginal = bitmap;

    SetName(name);
    SetValidator(validator);
    parent->AddChild(this);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;
    m_windowStyle = style;
    m_marginX = 0;
    m_marginY = 0;

    /*
    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;
    */

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

    m_font = parent->GetFont();
    ChangeFont(FALSE);

    ChangeBackgroundColour ();

    DoSetBitmap();

    XtAddCallback (buttonWidget, XmNactivateCallback, (XtCallbackProc) wxButtonCallback,
        (XtPointer) this);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

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
    m_buttonBitmapOriginal = bitmap;
    m_buttonBitmap = bitmap;

    DoSetBitmap();
}

void wxBitmapButton::SetBitmapSelected(const wxBitmap& sel)
{
    m_buttonBitmapSelected = sel;
    m_buttonBitmapSelectedOriginal = sel;

    DoSetBitmap();
};

void wxBitmapButton::SetBitmapFocus(const wxBitmap& focus)
{
    m_buttonBitmapFocus = focus;
    // Not used in Motif
};

void wxBitmapButton::SetBitmapDisabled(const wxBitmap& disabled)
{
    m_buttonBitmapDisabled = disabled;
    m_buttonBitmapDisabledOriginal = disabled;

    DoSetBitmap();
};

void wxBitmapButton::DoSetBitmap()
{
    if (m_buttonBitmapOriginal.Ok())
    {
        Pixmap pixmap = 0;
        Pixmap insensPixmap = 0;
        Pixmap armPixmap = 0;

        // Must re-make the bitmap to have its transparent areas drawn
        // in the current widget background colour.
        if (m_buttonBitmapOriginal.GetMask())
        {
            int backgroundPixel;
            XtVaGetValues((Widget) m_mainWidget, XmNbackground, &backgroundPixel,
                NULL);

            wxColour col;
            col.SetPixel(backgroundPixel);

            wxBitmap newBitmap = wxCreateMaskedBitmap(m_buttonBitmapOriginal, col);
            m_buttonBitmap = newBitmap;

            pixmap = (Pixmap) m_buttonBitmap.GetPixmap();
        }
        else
            pixmap = (Pixmap) m_buttonBitmap.GetLabelPixmap(m_mainWidget);

        if (m_buttonBitmapDisabledOriginal.Ok())
        {
            if (m_buttonBitmapDisabledOriginal.GetMask())
            {
                int backgroundPixel;
                XtVaGetValues((Widget) m_mainWidget, XmNbackground, &backgroundPixel,
                    NULL);

                wxColour col;
                col.SetPixel(backgroundPixel);

                wxBitmap newBitmap = wxCreateMaskedBitmap(m_buttonBitmapDisabledOriginal, col);
                m_buttonBitmapDisabled = newBitmap;

                insensPixmap = (Pixmap) m_buttonBitmapDisabled.GetPixmap();
            }
            else
                insensPixmap = (Pixmap) m_buttonBitmap.GetInsensPixmap(m_mainWidget);
        }
        else
            insensPixmap = (Pixmap) m_buttonBitmap.GetInsensPixmap(m_mainWidget);

        // Now make the bitmap representing the armed state
        if (m_buttonBitmapSelectedOriginal.Ok())
        {
            if (m_buttonBitmapSelectedOriginal.GetMask())
            {
                int backgroundPixel;
                XtVaGetValues((Widget) m_mainWidget, XmNarmColor, &backgroundPixel,
                    NULL);

                wxColour col;
                col.SetPixel(backgroundPixel);

                wxBitmap newBitmap = wxCreateMaskedBitmap(m_buttonBitmapSelectedOriginal, col);
                m_buttonBitmapSelected = newBitmap;

                armPixmap = (Pixmap) m_buttonBitmapSelected.GetPixmap();
            }
            else
                armPixmap = (Pixmap) m_buttonBitmap.GetArmPixmap(m_mainWidget);
        }
        else
            armPixmap = (Pixmap) m_buttonBitmap.GetArmPixmap(m_mainWidget);

        if (insensPixmap == pixmap) // <- the Get...Pixmap()-functions return the same pixmap!
        {
            insensPixmap =
                XCreateInsensitivePixmap(DisplayOfScreen(XtScreen((Widget) m_mainWidget)), pixmap);
            m_insensPixmap = (WXPixmap) insensPixmap;
        }

        XtVaSetValues ((Widget) m_mainWidget,
            XmNlabelPixmap, pixmap,
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
            XmNlabelInsensitivePixmap, XmUNSPECIFIED_PIXMAP,
            XmNarmPixmap, XmUNSPECIFIED_PIXMAP,
            NULL);
    }
}

void wxBitmapButton::ChangeBackgroundColour()
{
    DoChangeBackgroundColour(m_mainWidget, m_backgroundColour, TRUE);

    // Must reset the bitmaps since the colours have changed.
    DoSetBitmap();
}
