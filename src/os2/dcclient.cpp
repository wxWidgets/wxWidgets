/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:     wxClientDC class
// Author:      David Webster
// Modified by:
// Created:     09/21/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/string.h"
#include "wx/log.h"
#include "wx/window.h"

#include "wx/os2/private.h"

#include "wx/dcclient.h"

// ----------------------------------------------------------------------------
// array/list types
// ----------------------------------------------------------------------------

struct WXDLLEXPORT wxPaintDCInfo
{
    wxPaintDCInfo( wxWindow* pWin
                  ,wxDC*     pDC
                 )
    {
        m_hWnd = pWin->GetHWND();
        m_hDC = pDC->GetHDC();
        m_nCount = 1;
    }

    WXHWND                          m_hWnd;   // window for this DC
    WXHDC                           m_hDC;    // the DC handle
    size_t                          m_nCount; // usage count
}; // end of wxPaintDCInfot

#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxArrayDCInfo);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
    IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)
    IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static RECT        g_paintStruct;

#ifdef __WXDEBUG__
    // a global variable which we check to verify that wxPaintDC are only
    // created in resopnse to WM_PAINT message - doing this from elsewhere is a
    // common programming error among wxWindows programmers and might lead to
    // very subtle and difficult to debug refresh/repaint bugs.
    int g_isPainting = 0;
#endif // __WXDEBUG__

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// wxWindowDC
// ----------------------------------------------------------------------------

wxWindowDC::wxWindowDC()
{
    m_pCanvas = NULL;
}

wxWindowDC::wxWindowDC(
  wxWindow*                         pTheCanvas
)
{
    ERRORID                         vError;
    wxString                        sError;

    m_pCanvas = pTheCanvas;
    m_hDC = (WXHDC) ::WinOpenWindowDC(GetWinHwnd(pTheCanvas) );
    m_nDCCount++;
    //
    // default under PM is that Window and Client DC's are the same
    // so we offer a separate Presentation Space to use for the
    // entire window.  Otherwise, calling BeginPaint will just create
    // chached-micro client presentation space
    //
     m_hPS = GpiCreatePS( m_hab
                         ,m_hDC
                         ,&m_PageSize
                         ,PU_PELS | GPIF_LONG | GPIA_ASSOC
                        );
    ::GpiAssociate(m_hPS, NULLHANDLE);
    ::GpiAssociate(m_hPS, m_hDC);
    // Set the wxWindows color table
    if (!::GpiCreateLogColorTable( m_hPS
                                  ,0L
                                  ,LCOLF_CONSECRGB
                                  ,0L
                                  ,(LONG)wxTheColourDatabase->m_nSize
                                  ,(PLONG)wxTheColourDatabase->m_palTable
                                 ))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Unable to set current color table. Error: %s\n", sError);
    }
    SetBackground(wxBrush(m_pCanvas->GetBackgroundColour(), wxSOLID));
}

wxWindowDC::~wxWindowDC()
{
    if (m_pCanvas && m_hDC)
    {
        SelectOldObjects(m_hDC);

        //
        // In PM one does not explicitly close or release an open WindowDC
        // They automatically close with the window, unless explicitly detached
        // but we need to destroy our PS
        //
        if(m_hPS)
        {
            ::GpiAssociate(m_hPS, NULLHANDLE);
            ::GpiDestroyPS(m_hPS);
        }
        m_hPS = NULLHANDLE;
        m_hDC = NULLHANDLE;
    }
    m_nDCCount--;
}

// ----------------------------------------------------------------------------
// wxClientDC
// ----------------------------------------------------------------------------

wxClientDC::wxClientDC()
{
    m_pCanvas = NULL;
}

wxClientDC::wxClientDC(
  wxWindow*                         pTheCanvas
)
{
    SIZEL                           vSizl = { 0,0};
    ERRORID                         vError;
    wxString                        sError;

    m_pCanvas = pTheCanvas;

    //
    // default under PM is that Window and Client DC's are the same
    //
    m_hDC = (WXHDC) ::WinOpenWindowDC(GetWinHwnd(pTheCanvas));
    m_hPS = ::GpiCreatePS( wxGetInstance()
                          ,m_hDC
                          ,&vSizl
                          ,PU_PELS | GPIF_LONG | GPIA_ASSOC
                         );

    // Set the wxWindows color table
    if (!::GpiCreateLogColorTable( m_hPS
                                  ,0L
                                  ,LCOLF_CONSECRGB
                                  ,0L
                                  ,(LONG)wxTheColourDatabase->m_nSize
                                  ,(PLONG)wxTheColourDatabase->m_palTable
                                 ))
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogError("Unable to set current color table. Error: %s\n", sError);
    }
    //
    // Default mode is BM_LEAVEALONE so we make no call Set the mix
    //
    SetBackground(wxBrush( m_pCanvas->GetBackgroundColour()
                          ,wxSOLID
                         )
                 );
} // end of wxClientDC::wxClientDC

wxClientDC::~wxClientDC()
{
    if ( m_pCanvas && GetHdc() )
    {
        SelectOldObjects(m_hDC);

        //
        // We don't explicitly release Device contexts in PM and
        // the cached micro PS is already gone
        //
        m_hDC = 0;
    }
} // end of wxClientDC::~wxClientDC

// ----------------------------------------------------------------------------
// wxPaintDC
// ----------------------------------------------------------------------------

// VZ: initial implementation (by JACS) only remembered the last wxPaintDC
//     created and tried to reuse - this was supposed to take care of a
//     situation when a derived class OnPaint() calls base class OnPaint()
//     because in this case ::BeginPaint() shouldn't be called second time.
//
//     I'm not sure how useful this is, however we must remember the HWND
//     associated with the last HDC as well - otherwise we may (and will!) try
//     to reuse the HDC for another HWND which is a nice recipe for disaster.
//
//     So we store a list of windows for which we already have the DC and not
//     just one single hDC. This seems to work, but I'm really not sure about
//     the usefullness of the whole idea - IMHO it's much better to not call
//     base class OnPaint() at all, or, if we really want to allow it, add a
//     "wxPaintDC *" parameter to wxPaintEvent which should be used if it's
//     !NULL instead of creating a new DC.

wxArrayDCInfo wxPaintDC::ms_cache;

wxPaintDC::wxPaintDC()
{
    m_pCanvas = NULL;
    m_hDC = 0;
}

wxPaintDC::wxPaintDC(
  wxWindow*                         pCanvas
)
{
    wxCHECK_RET(pCanvas, wxT("NULL canvas in wxPaintDC ctor"));
    RECTL                           vRect;

#ifdef __WXDEBUG__
    if (g_isPainting <= 0)
    {
        wxFAIL_MSG( wxT("wxPaintDC may be created only in EVT_PAINT handler!") );
        return;
    }
#endif // __WXDEBUG__

    m_pCanvas = pCanvas;

    //
    // Do we have a DC for this window in the cache?
    //
    wxPaintDCInfo*                  pInfo = FindInCache();

    if (pInfo)
    {
        m_hDC = pInfo->m_hDC;
        pInfo->m_nCount++;
    }
    else // not in cache, create a new one
    {
        SIZEL                       vSizl = { 0,0};
        HPS                         hPS;
        HRGN                        hRgn;

        hPS = ::WinBeginPaint( GetWinHwnd(m_pCanvas)
                              ,NULLHANDLE
                              ,&g_paintStruct
                             );
        if(hPS)
        {
            m_hOldPS = m_hPS;
            m_hPS = hPS;
            ::GpiCreateLogColorTable( m_hPS
                                     ,0L
                                     ,LCOLF_CONSECRGB
                                     ,0L
                                     ,(LONG)wxTheColourDatabase->m_nSize
                                     ,(PLONG)wxTheColourDatabase->m_palTable
                                    );
            ::GpiCreateLogColorTable( m_hPS
                                     ,0L
                                     ,LCOLF_RGB
                                     ,0L
                                     ,0L
                                     ,NULL
                                    );
        }

        m_bIsPaintTime   = TRUE;
        m_hDC = (WXHDC) -1; // to satisfy those anonizmous efforts
        m_vRclPaint = g_paintStruct;
        ms_cache.Add(new wxPaintDCInfo(m_pCanvas, this));
    }
    SetBackground(wxBrush(m_pCanvas->GetBackgroundColour(), wxSOLID));
}

wxPaintDC::~wxPaintDC()
{
    if ( m_hDC )
    {
        SelectOldObjects(m_hDC);

        size_t                      nIndex;
        wxPaintDCInfo*              pInfo = FindInCache(&nIndex);

        wxCHECK_RET( pInfo, wxT("existing DC should have a cache entry") );

        if ( !--pInfo->m_nCount )
        {
            ::WinEndPaint(m_hPS);
            m_hPS          = m_hOldPS;
            m_bIsPaintTime = FALSE;
            ms_cache.Remove(nIndex);
        }
        //else: cached DC entry is still in use

        // prevent the base class dtor from ReleaseDC()ing it again
        m_hDC = 0;
    }
}

wxPaintDCInfo* wxPaintDC::FindInCache(
  size_t*                           pIndex
) const
{
    wxPaintDCInfo*                  pInfo = NULL;
    size_t                          nCache = ms_cache.GetCount();

    for (size_t n = 0; n < nCache; n++)
    {
        pInfo = &ms_cache[n];
        if (pInfo->m_hWnd == m_pCanvas->GetHWND())
        {
            if (pIndex)
                *pIndex = n;
            break;
        }
    }
    return pInfo;
} // end of wxPaintDC::FindInCache

