/////////////////////////////////////////////////////////////////////////////
// Name:        src/motif/bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __VMS
#define XtScreen XTSCREEN
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

// Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap );

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

wxBitmapButton::wxBitmapButton()
{
    m_marginX = m_marginY = wxDEFAULT_BUTTON_MARGIN;
    m_insensPixmap = (WXPixmap) 0;
}

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id,
                            const wxBitmap& bitmap,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    if( !CreateControl( parent, id, pos, size, style, validator, name ) )
        return false;

    m_bmpNormal = m_bmpNormalOriginal = bitmap;
    m_bmpSelected = m_bmpSelectedOriginal = bitmap;

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
        // See comment for wxButton::SetDefault
        // XmNdefaultButtonShadowThickness, 1,
        XmNrecomputeSize, False,
        NULL);

    m_mainWidget = (WXWidget) buttonWidget;

    ChangeFont(false);

    ChangeBackgroundColour ();

    DoSetBitmap();

    XtAddCallback (buttonWidget,
                   XmNactivateCallback, (XtCallbackProc) wxButtonCallback,
                   (XtPointer) this);

    wxSize best = m_bmpNormal.Ok() ? GetBestSize() : wxSize(30, 30);
    if( size.x != -1 ) best.x = size.x;
    if( size.y != -1 ) best.y = size.y;

    AttachWidget (parent, m_mainWidget, (WXWidget) NULL,
                  pos.x, pos.y, best.x, best.y);

    return true;
}

wxBitmapButton::~wxBitmapButton()
{
    SetBitmapLabel(wxNullBitmap);

    if (m_insensPixmap)
        XmDestroyPixmap (DefaultScreenOfDisplay ((Display*) GetXDisplay()),
                         (Pixmap) m_insensPixmap);
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
    m_bmpNormalOriginal = bitmap;
    m_bmpNormal = bitmap;

    DoSetBitmap();
}

void wxBitmapButton::SetBitmapSelected(const wxBitmap& sel)
{
    m_bmpSelected = sel;
    m_bmpSelectedOriginal = sel;

    DoSetBitmap();
}

void wxBitmapButton::SetBitmapFocus(const wxBitmap& focus)
{
    m_bmpFocus = focus;
    // Not used in Motif
}

void wxBitmapButton::SetBitmapDisabled(const wxBitmap& disabled)
{
    m_bmpDisabled = disabled;
    m_bmpDisabledOriginal = disabled;

    DoSetBitmap();
}

void wxBitmapButton::DoSetBitmap()
{
    if (m_bmpNormalOriginal.Ok())
    {
        Pixmap pixmap = 0;
        Pixmap insensPixmap = 0;
        Pixmap armPixmap = 0;

        // Must re-make the bitmap to have its transparent areas drawn
        // in the current widget background colour.
        if (m_bmpNormalOriginal.GetMask())
        {
            WXPixel backgroundPixel;
            XtVaGetValues((Widget) m_mainWidget,
                          XmNbackground, &backgroundPixel,
                          NULL);

            wxColour col;
            col.SetPixel(backgroundPixel);

            wxBitmap newBitmap =
                wxCreateMaskedBitmap(m_bmpNormalOriginal, col);
            m_bmpNormal = newBitmap;
            m_bitmapCache.SetBitmap( m_bmpNormal );

            pixmap = (Pixmap) m_bmpNormal.GetDrawable();
        }
        else
        {
            m_bitmapCache.SetBitmap( m_bmpNormal );
            pixmap = (Pixmap) m_bitmapCache.GetLabelPixmap(m_mainWidget);
        }

        if (m_bmpDisabledOriginal.Ok())
        {
            if (m_bmpDisabledOriginal.GetMask())
            {
                WXPixel backgroundPixel;
                XtVaGetValues((Widget) m_mainWidget,
                              XmNbackground, &backgroundPixel,
                              NULL);

                wxColour col;
                col.SetPixel(backgroundPixel);

                wxBitmap newBitmap =
                    wxCreateMaskedBitmap(m_bmpDisabledOriginal, col);
                m_bmpDisabled = newBitmap;

                insensPixmap = (Pixmap) m_bmpDisabled.GetDrawable();
            }
            else
                insensPixmap = (Pixmap) m_bitmapCache.GetInsensPixmap(m_mainWidget);
        }
        else
            insensPixmap = (Pixmap) m_bitmapCache.GetInsensPixmap(m_mainWidget);

        // Now make the bitmap representing the armed state
        if (m_bmpSelectedOriginal.Ok())
        {
            if (m_bmpSelectedOriginal.GetMask())
            {
                WXPixel backgroundPixel;
                XtVaGetValues((Widget) m_mainWidget,
                              XmNarmColor, &backgroundPixel,
                              NULL);

                wxColour col;
                col.SetPixel(backgroundPixel);

                wxBitmap newBitmap =
                    wxCreateMaskedBitmap(m_bmpSelectedOriginal, col);
                m_bmpSelected = newBitmap;

                armPixmap = (Pixmap) m_bmpSelected.GetDrawable();
            }
            else
                armPixmap = (Pixmap) m_bitmapCache.GetArmPixmap(m_mainWidget);
        }
        else
            armPixmap = (Pixmap) m_bitmapCache.GetArmPixmap(m_mainWidget);

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
    wxDoChangeBackgroundColour(m_mainWidget, m_backgroundColour, true);

    // Must reset the bitmaps since the colours have changed.
    DoSetBitmap();
}

wxSize wxBitmapButton::DoGetBestSize() const
{
    wxSize ret( 30,30 );

    if (m_bmpNormal.Ok())
    {
        int border = (GetWindowStyle() & wxNO_BORDER) ? 4 : 10;
        ret.x = m_bmpNormal.GetWidth()+border;
        ret.y = m_bmpNormal.GetHeight()+border;
    }

    return ret;
}
