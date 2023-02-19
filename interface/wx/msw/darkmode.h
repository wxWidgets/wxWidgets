/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/darkmode.h
// Purpose:     Documentation for MSW-specific dark mode functionality.
// Author:      Vadim Zeitlin
// Created:     2023-02-19
// Copyright:   (c) 2023 Vadim Zeitlin
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/**
    Allows to customize some of the settings used in MSW dark mode.

    An object of this class may be passed to wxApp::MSWEnableDarkMode() to
    customize some aspects of the dark mode when it is used under MSW systems.

    For example, to customize the background colour to use a reddish black
    instead of normal black used by default, you could do the following:
    @code
    class MySettings : public wxDarkModeSettings
    {
    public:
        wxColour GetColour(wxSystemColour index) override
        {
            switch ( index )
            {
                case wxSYS_COLOUR_ACTIVECAPTION:
                case wxSYS_COLOUR_APPWORKSPACE:
                case wxSYS_COLOUR_INFOBK:
                case wxSYS_COLOUR_LISTBOX:
                case wxSYS_COLOUR_WINDOW:
                case wxSYS_COLOUR_BTNFACE:
                    // Default colour used here is 0x202020.
                    return wxColour(0x402020);
            }

            return wxDarkModeSettings::GetColour(index);
        }
    };

    wxTheApp->MSWEnableDarkMode(wxApp::DarkMode_Always, new MySettings());
    @endcode

    @since 3.3.0
 */
class wxDarkModeSettings
{
public:
    /**
        Default constructor does nothing.
     */
    wxDarkModeSettings() = default;

    /**
        Get the colour to use for the given system colour when dark mode is on.

        The base class version of this function returns the colours commonly
        used in dark mode. As the rest of dark mode support, their exact values
        are not documented and are subject to change in the future Windows or
        wxWidgets versions.
     */
    virtual wxColour GetColour(wxSystemColour index);
};
