///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/metrics.h
// Purpose:     various helper functions to retrieve system metrics
// Author:      Vadim Zeitlin
// Created:     2008-09-05
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_METRICS_H_
#define _WX_MSW_PRIVATE_METRICS_H_

#include "wx/msw/private.h"

namespace wxMSWImpl
{

// return NONCLIENTMETRICS as retrieved by SystemParametersInfo()
//
// currently this is not cached as the values may change when system settings
// do and we don't react to this to invalidate the cache but it could be done
// in the future
//
// MT-safety: this function is only meant to be called from the main thread
inline const NONCLIENTMETRICS GetNonClientMetrics(const wxWindow* win)
{
    WinStruct<NONCLIENTMETRICS> nm;
    if ( !wxSystemParametersInfo(SPI_GETNONCLIENTMETRICS,
                                 sizeof(NONCLIENTMETRICS),
                                 &nm,
                                 0,
                                 win) )
    {
        wxLogLastError(wxT("SystemParametersInfo(SPI_GETNONCLIENTMETRICS)"));
    }

    return nm;
}

} // namespace wxMSWImpl

#endif // _WX_MSW_PRIVATE_METRICS_H_

