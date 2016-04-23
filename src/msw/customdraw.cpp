///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/customdraw.cpp
// Purpose:     wxMSWCustomDraw implementation
// Author:      Vadim Zeitlin
// Created:     2016-04-16
// Copyright:   (c) 2016 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/msw/private.h"
#endif // !WX_PRECOMP

#include "wx/msw/private/customdraw.h"

// ============================================================================
// implementation
// ============================================================================

LPARAM wxMSWImpl::CustomDraw::HandleItemPrepaint(const wxItemAttr& attr, HDC hdc)
{
    if ( attr.HasTextColour() )
        ::SetTextColor(hdc, wxColourToRGB(attr.GetTextColour()));

    if ( attr.HasBackgroundColour() )
        ::SetBkColor(hdc, wxColourToRGB(attr.GetBackgroundColour()));

    if ( attr.HasFont() )
    {
        ::SelectObject(hdc, GetHfontOf(attr.GetFont()));

        return CDRF_NEWFONT;
    }

    return CDRF_DODEFAULT;
}

LPARAM wxMSWImpl::CustomDraw::HandleCustomDraw(LPARAM lParam)
{
    NMCUSTOMDRAW* nmcd = reinterpret_cast<NMCUSTOMDRAW*>(lParam);
    switch ( nmcd->dwDrawStage )
    {
        case CDDS_PREPAINT:
            if ( HasCustomDrawnItems() )
                return CDRF_NOTIFYITEMDRAW;
            break;

        case CDDS_ITEMPREPAINT:
            const wxItemAttr* const attr = GetItemAttr(nmcd->dwItemSpec);
            if ( attr )
                return HandleItemPrepaint(*attr, nmcd->hdc);
    }

    return CDRF_DODEFAULT;
}
