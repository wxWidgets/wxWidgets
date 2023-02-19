///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/darkmode.h
// Purpose:     MSW-specific header with dark mode related declarations.
// Author:      Vadim Zeitlin
// Created:     2023-02-19
// Copyright:   (c) 2023 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DARKMODE_H_
#define _WX_MSW_DARKMODE_H_

#include "wx/settings.h"

// Constants used with wxDarkModeSettings::GetMenuColour().
enum class wxMenuColour
{
    StandardFg,
    StandardBg,
    DisabledFg,
    HotBg
};

// ----------------------------------------------------------------------------
// wxDarkModeSettings: allows to customize some of dark mode settings
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDarkModeSettings
{
public:
    wxDarkModeSettings() = default;
    virtual ~wxDarkModeSettings();

    // Get the colour to use for the given system colour when dark mode is on.
    virtual wxColour GetColour(wxSystemColour index);

    // Menu items don't use any of the standard colours, but are defined by
    // this function.
    virtual wxColour GetMenuColour(wxMenuColour which);

    // Get the pen to use for drawing wxStaticBox border in dark mode.
    //
    // Returning an invalid pen indicates that the default border drawn by the
    // system should be used, which doesn't look very well in dark mode but
    // shouldn't result in any problems worse than cosmetic ones.
    virtual wxPen GetBorderPen();

private:
    wxDECLARE_NO_COPY_CLASS(wxDarkModeSettings);
};

#endif // _WX_MSW_DARKMODE_H_
