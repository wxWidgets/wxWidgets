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

#include "wx/defs.h"

#if wxUSE_BMPBUTTON

#include "wx/button.h"

// FIXME: right now only wxMSW implements bitmap support in wxButton
//        itself, this shouldn't be used for the other platforms neither
//        when all of them do it
#ifdef __WXMSW__
    #define wxHAS_BUTTON_BITMAP
#endif

// ----------------------------------------------------------------------------
// wxBitmapButton: a button which shows bitmaps instead of the usual string.
// It has different bitmaps for different states (focused/disabled/pressed)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapButtonBase : public wxButton
{
public:
    wxBitmapButtonBase()
    {
#ifndef wxHAS_BUTTON_BITMAP
        m_marginX =
        m_marginY = 0;
#endif // wxHAS_BUTTON_BITMAP
    }

    // set/get the margins around the button
    virtual void SetMargins(int x, int y)
    {
        DoSetBitmapMargins(x, y);
    }

    int GetMarginX() const { return DoGetBitmapMargins().x; }
    int GetMarginY() const { return DoGetBitmapMargins().y; }

    // deprecated synonym for SetBitmapLabel()
#if WXWIN_COMPATIBILITY_2_6
    wxDEPRECATED_INLINE( void SetLabel(const wxBitmap& bitmap).
       SetBitmapLabel(bitmap); );

    // prevent virtual function hiding
    virtual void SetLabel(const wxString& label)
        { wxWindow::SetLabel(label); }
#endif // WXWIN_COMPATIBILITY_2_6

protected:
#ifndef wxHAS_BUTTON_BITMAP
    // function called when any of the bitmaps changes
    virtual void OnSetBitmap() { InvalidateBestSize(); Refresh(); }

    virtual wxBitmap DoGetBitmap(State which) const { return m_bitmaps[which]; }
    virtual void DoSetBitmap(const wxBitmap& bitmap, State which)
        { m_bitmaps[which] = bitmap; OnSetBitmap(); }

    virtual wxSize DoGetBitmapMargins() const
    {
        return wxSize(m_marginX, m_marginY);
    }

    virtual void DoSetBitmapMargins(int x, int y)
    {
        m_marginX = x;
        m_marginY = y;
    }

    // the bitmaps for various states
    wxBitmap m_bitmaps[State_Max];

    // the margins around the bitmap
    int m_marginX,
        m_marginY;
#endif // !wxHAS_BUTTON_BITMAP

    wxDECLARE_NO_COPY_CLASS(wxBitmapButtonBase);
};

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/bmpbuttn.h"
#elif defined(__WXMSW__)
    #include "wx/msw/bmpbuttn.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/bmpbuttn.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/bmpbuttn.h"
#elif defined(__WXGTK__)
    #include "wx/gtk1/bmpbuttn.h"
#elif defined(__WXMAC__)
    #include "wx/osx/bmpbuttn.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/bmpbuttn.h"
#elif defined(__WXPM__)
    #include "wx/os2/bmpbuttn.h"
#elif defined(__WXPALMOS__)
    #include "wx/palmos/bmpbuttn.h"
#endif

#endif // wxUSE_BMPBUTTON

#endif // _WX_BMPBUTTON_H_BASE_
