/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      David Webster
// Modified by:
// Created:     11/27/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/window.h"
#include "wx/os2/private.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/statbmp.h"
#endif

#include <stdio.h>

// ---------------------------------------------------------------------------
// macors
// ---------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)
#endif

// ---------------------------------------------------------------------------
//  wxStaticBitmap
// ---------------------------------------------------------------------------

bool wxStaticBitmap::Create(
  wxWindow*                         pParent
, wxWindowID                        nId
, const wxGDIImage&                 rBitmap
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rName
)
{
    Init();

    SetName(rName);
    if (pParent)
        pParent->AddChild(this);

    m_backgroundColour = pParent->GetBackgroundColour() ;
    m_foregroundColour = pParent->GetForegroundColour() ;

    if (nId == -1)
        m_windowId = (int)NewControlId();
    else
        m_windowId = nId;

    m_windowStyle = lStyle;

    int                             nX= rPos.x;
    int                             nY = rPos.y;
    int                             nWidth = rSize.x;
    int                             nHeight = rSize.y;

    m_windowStyle = lStyle;

    m_bIsIcon = rBitmap.IsKindOf(CLASSINFO(wxIcon));

    // TODO: create static bitmap control
    const wxChar*                   zClassname = wxT("WX_STATIC");
    int                             nWinstyle = m_bIsIcon ? SS_ICON : SS_BITMAP;

    m_hWnd = (WXHWND)::WinCreateWindow( pParent->GetHWND()
                                       ,zClassname
                                       ,wxT("")
                                       ,nWinstyle | WS_VISIBLE
                                       ,0,0,0,0
                                       ,pParent->GetHWND()
                                       ,HWND_TOP
                                       ,m_windowId
                                       ,NULL
                                       ,NULL
                                      );

    wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create static bitmap") );

    SetImage(rBitmap);

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);
    SetFont(GetParent()->GetFont());
    SetSize(nX, nY, nWidth, nHeight);
    return(FALSE);
}

bool wxStaticBitmap::ImageIsOk() const
{
    return(m_pImage && m_pImage->Ok());
}

void wxStaticBitmap::Free()
{
    delete m_pImage;
    m_pImage = NULL;
}

wxSize wxStaticBitmap::DoGetBestSize() const
{
    // reuse the current size (as wxWindow does) instead of using some
    // arbitrary default size (as wxControl, our immediate base class, does)
    return wxWindow::DoGetBestSize();
}

void wxStaticBitmap::SetImage(
  const wxGDIImage&                 rBitmap
)
{
    Free();

    m_bIsIcon = rBitmap.IsKindOf(CLASSINFO(wxIcon));
    if (m_bIsIcon)
        m_pImage = new wxIcon((const wxIcon&)rBitmap);
    else
        m_pImage = new wxBitmap((const wxBitmap &)rBitmap);

    int                             nX;
    int                             nY;
    int                             nW;
    int                             nH;

    GetPosition(&nX, &nY);
    GetSize(&nW, &nH);

    ::WinSendMsg( GetHwnd()
                 ,SM_SETHANDLE
                 ,MPFROMHWND(m_pImage->GetHandle())
                 ,NULL
                );
    if (ImageIsOk())
    {
        int                         nWidth = rBitmap.GetWidth();
        int                         nHeight = rBitmap.GetHeight();

        if (nWidth && nHeight)
        {
            nW = nWidth;
            nW = nHeight;

            ::WinSetWindowPos( GetHwnd()
                              ,HWND_TOP
                              ,nX
                              ,nY
                              ,nWidth
                              ,nHeight
                              ,SWP_SIZE | SWP_MOVE | SWP_SHOW
                             );
        }
    }

    RECTL                           vRect;

    vRect.xLeft   = nW;
    vRect.yTop    = nY;
    vRect.xRight  = nX + nW;
    vRect.yBottom = nY + nH;

    ::WinInvalidateRect(GetHwndOf(GetParent()), &vRect, TRUE);
}

