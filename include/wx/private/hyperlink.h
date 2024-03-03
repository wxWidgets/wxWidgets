///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/link.h
// Purpose:     Private functions related to hyperlinks.
// Author:      Vadim Zeitlin
// Created:     2023-06-01
// Copyright:   (c) 2023 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_HYPERLINK_H_
#define _WX_PRIVATE_HYPERLINK_H_

#include "wx/settings.h"

namespace wxPrivate
{

inline wxColour GetLinkColour()
{
    // There is a standard link colour appropriate for the default "light" mode,
    // see https://html.spec.whatwg.org/multipage/rendering.html#phrasing-content-3
    // it doesn't stand out enough in dark mode, so choose "light sky blue"
    // colour for the links in dark mode instead (this is a rather arbitrary
    // choice, but there doesn't seem to be any standard one).
    return wxSystemSettings::SelectLightDark(wxColour(0x00, 0x00, 0xee),
                                             wxColour(0x87, 0xce, 0xfa));
}

} // namespace wxPrivate

#endif // _WX_PRIVATE_HYPERLINK_H_
