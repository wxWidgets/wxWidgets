///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/subwin.h
// Purpose:     helper for implementing the controls with subwindows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-12-11
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_SUBWIN_H_
#define _WX_MSW_SUBWIN_H_

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// wxSubwindows contains all HWNDs making part of a single wx control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSubwindows
{
public:
    // the number of subwindows can be specified either as parameter to ctor or
    // later in Create()
    wxSubwindows(size_t n = 0) { Init(); if ( n ) Create(n); }

    // allocate enough space for the given number of windows
    void Create(size_t n)
    {
        wxASSERT_MSG( !m_hwnds, _T("Create() called twice?") );

        m_count = n;
        m_hwnds = (HWND *)calloc(n, sizeof(HWND));
    }

    // non-virtual dtor, this class is not supposed to be used polymorphically
    ~wxSubwindows()
    {
        for ( size_t n = 0; n < m_count; n++ )
        {
            ::DestroyWindow(m_hwnds[n]);
        }

        free(m_hwnds);
    }

    // get the number of subwindows
    size_t GetCount() const { return m_count; }

    // access a given window
    HWND& Get(size_t n)
    {
        wxASSERT_MSG( n < m_count, _T("subwindow index out of range") );

        return m_hwnds[n];
    }

    HWND& operator[](size_t n) { return Get(n); }
    HWND operator[](size_t n) const
    {
        return wx_const_cast(wxSubwindows *, this)->Get(n);
    }

    // check if we have this window
    bool HasWindow(HWND hwnd)
    {
        for ( size_t n = 0; n < m_count; n++ )
        {
            if ( m_hwnds[n] == hwnd )
                return true;
        }

        return false;
    }


    // methods which are forwarded to all subwindows
    // ---------------------------------------------

    // show/hide everything
    void Show(bool show)
    {
        int sw = show ? SW_SHOW : SW_HIDE;
        for ( size_t n = 0; n < m_count; n++ )
        {
            ::ShowWindow(m_hwnds[n], sw);
        }
    }

    // enable/disable everything
    void Enable(bool enable)
    {
        for ( size_t n = 0; n < m_count; n++ )
        {
            ::EnableWindow(m_hwnds[n], enable);
        }
    }

    // set font for all windows
    void SetFont(const wxFont& font)
    {
        HFONT hfont = GetHfontOf(font);
        wxCHECK_RET( hfont, _T("invalid font") );

        for ( size_t n = 0; n < m_count; n++ )
        {
            ::SendMessage(m_hwnds[n], WM_SETFONT, (WPARAM)hfont, 0);
        }
    }

    // find the bounding box for all windows
    wxRect GetBoundingBox() const
    {
        wxRect r;
        for ( size_t n = 0; n < m_count; n++ )
        {
            RECT rc;
            ::GetWindowRect(m_hwnds[n], &rc);

            r.Union(wxRectFromRECT(rc));
        }

        return r;
    }

private:
    void Init()
    {
        m_count = 0;
        m_hwnds = NULL;
    }

    // number of elements in m_hwnds array
    size_t m_count;

    // the HWNDs we contain
    HWND *m_hwnds;


    DECLARE_NO_COPY_CLASS(wxSubwindows)
};

#endif // _WX_MSW_SUBWIN_H_

