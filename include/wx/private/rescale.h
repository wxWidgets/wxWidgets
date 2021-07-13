///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/rescale.h
// Purpose:     Helpers for rescaling coordinates
// Author:      Vadim Zeitlin
// Created:     2021-07-13
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_RESCALE_H_
#define _WX_PRIVATE_RESCALE_H_

#include "wx/gdicmn.h"
#include "wx/math.h"

#ifdef __WINDOWS__
    // Required in order to use wxMulDivInt32().
    #include "wx/msw/wrapwin.h"
#endif


// Scale the given value by the ratio between 2 other values, with rounding.
// Do not scale the value if it's -1, just return it unchanged in this case.
inline int wxRescaleCoord(int n, int newScale, int oldScale)
{
    return n == -1 ? -1 : wxMulDivInt32(n, newScale, oldScale);
}

inline wxPoint
wxRescaleCoord(wxPoint pt, wxSize newScale, wxSize oldScale)
{
    return wxPoint(wxRescaleCoord(pt.x, newScale.x, oldScale.x),
                   wxRescaleCoord(pt.y, newScale.y, oldScale.y));
}

inline wxSize
wxRescaleCoord(wxSize sz, wxSize newScale, wxSize oldScale)
{
    return wxSize(wxRescaleCoord(sz.x, newScale.x, oldScale.x),
                  wxRescaleCoord(sz.y, newScale.y, oldScale.y));
}

#endif // _WX_PRIVATE_RESCALE_H_
