/////////////////////////////////////////////////////////////////////////////
// Name:        wx/bmpbutton.h
// Purpose:     wxBitmapButton class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BMPBUTTON_H_BASE_
#define _WX_BMPBUTTON_H_BASE_

#if wxUSE_BMPBUTTON

#include "wx/bitmap.h"
#include "wx/button.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxButtonNameStr;

// ----------------------------------------------------------------------------
// wxBitmapButton: a button which shows bitmaps instead of the usual string.
// It has different bitmaps for different states (focused/disabled/pressed)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxBitmapButtonBase : public wxButton
{
public:
    wxBitmapButtonBase() { m_marginX = m_marginY = 0; }

    // set the bitmaps
    void SetBitmapLabel(const wxBitmap& bitmap)
        { m_bmpNormal = bitmap; OnSetBitmap(); }
    void SetBitmapSelected(const wxBitmap& sel)
        { m_bmpSelected = sel; OnSetBitmap(); };
    void SetBitmapFocus(const wxBitmap& focus)
        { m_bmpFocus = focus; OnSetBitmap(); };
    void SetBitmapDisabled(const wxBitmap& disabled)
        { m_bmpDisabled = disabled; OnSetBitmap(); };
    void SetLabel(const wxBitmap& bitmap)
        { SetBitmapLabel(bitmap); }

    // retrieve the bitmaps
    const wxBitmap& GetBitmapLabel() const { return m_bmpNormal; }
    const wxBitmap& GetBitmapSelected() const { return m_bmpSelected; }
    const wxBitmap& GetBitmapFocus() const { return m_bmpFocus; }
    const wxBitmap& GetBitmapDisabled() const { return m_bmpDisabled; }
    wxBitmap& GetBitmapLabel() { return m_bmpNormal; }
    wxBitmap& GetBitmapSelected() { return m_bmpSelected; }
    wxBitmap& GetBitmapFocus() { return m_bmpFocus; }
    wxBitmap& GetBitmapDisabled() { return m_bmpDisabled; }

    // set/get the margins around the button
    virtual void SetMargins(int x, int y) { m_marginX = x; m_marginY = y; }
    int GetMarginX() const { return m_marginX; }
    int GetMarginY() const { return m_marginY; }

protected:
    // function called when any of the bitmaps changes
    virtual void OnSetBitmap() { }

    // the bitmaps for various states
    wxBitmap m_bmpNormal,
             m_bmpSelected,
             m_bmpFocus,
             m_bmpDisabled;

    // the margins around the bitmap
    int m_marginX,
        m_marginY;
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/bmpbuttn.h"
#elif defined(__WXMSW__)
    #include "wx/msw/bmpbuttn.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/bmpbuttn.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/bmpbuttn.h"
#elif defined(__WXQT__)
    #include "wx/qt/bmpbuttn.h"
#elif defined(__WXMAC__)
    #include "wx/mac/bmpbuttn.h"
#elif defined(__WXPM__)
    #include "wx/os2/bmpbuttn.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/bmpbuttn.h"
#endif

#endif // wxUSE_BMPBUTTON

#endif // _WX_BMPBUTTON_H_BASE_
