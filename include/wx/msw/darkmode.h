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

private:
    wxDECLARE_NO_COPY_CLASS(wxDarkModeSettings);
};

#endif // _WX_MSW_DARKMODE_H_
