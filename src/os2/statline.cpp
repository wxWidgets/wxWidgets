/////////////////////////////////////////////////////////////////////////////
// Name:        msw/statline.cpp
// Purpose:     OS2 version of wxStaticLine class
// Author:      David Webster
// Created:     10/23/99
// Version:     $Id$
// Copyright:   (c) 1999 David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "statline.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statline.h"

#if wxUSE_STATLINE

#include "wx/os2/private.h"
#include "wx/log.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxStaticLine, wxControl)

// ----------------------------------------------------------------------------
// wxStaticLine
// ----------------------------------------------------------------------------

bool wxStaticLine::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rsName
)
{
    if (!CreateBase( pParent
                    ,vId
                    ,rPos
                    ,rSize
                    ,lStyle
                    ,wxDefaultValidator
                    ,rsName
                   ))
        return FALSE;

    pParent->AddChild(this);

    wxSize                          vSizeReal = AdjustSize(rSize);

    m_hWnd = (WXHWND)::WinCreateWindow( GetWinHwnd(pParent)
                                       ,WC_STATIC
                                       ,""
                                       ,WS_VISIBLE | SS_TEXT | DT_VCENTER | DT_CENTER
                                       ,0
                                       ,0
                                       ,0
                                       ,0
                                       ,GetWinHwnd(pParent)
                                       ,HWND_TOP
                                       ,(ULONG)m_windowId
                                       ,NULL
                                       ,NULL
                                      );
    if ( !m_hWnd )
    {
        wxLogDebug(wxT("Failed to create static control"));
        return FALSE;
    }
    SubclassWin(m_hWnd);
    SetSize( rPos.x
            ,rPos.y
            ,rSize.x
            ,rSize.y
           );
    return TRUE;
} // end of wxStaticLine::Create

#endif // wxUSE_STATLINE
