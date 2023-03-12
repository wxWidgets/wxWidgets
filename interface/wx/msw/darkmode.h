/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/darkmode.h
// Purpose:     Documentation for MSW-specific dark mode functionality.
// Author:      Vadim Zeitlin
// Created:     2023-02-19
// Copyright:   (c) 2023 Vadim Zeitlin
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

/**
    Constants used with wxDarkModeSettings::GetMenuColour().

    @since 3.3.0
 */
enum class wxMenuColour
{
    /// Text colour used for the normal items in dark mode.
    StandardFg,

    /// Standard menu background colour.
    StandardBg,

    /// Foreground colour for the disabled items.
    DisabledFg,

    /// Background colour used for the item over which mouse is hovering.
    HotBg
};

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

                default:
                    return wxDarkModeSettings::GetColour(index);
            }
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

        @see GetMenuColour()
     */
    virtual wxColour GetColour(wxSystemColour index);

    /**
        Get the colour to use for the menu bar in the given state.

        Currently the colours used by the menus in the menu bar in dark mode
        don't correspond to any of wxSystemColour values and this separate
        function is used for customizing them instead of GetColour().

        Note that the colours returned by this function only affect the top
        level menus, the colours of the menu items inside them can be
        customized in the usual way using wxOwnerDrawn::SetTextColour().

        The returned colour must be valid.
     */
    virtual wxColour GetMenuColour(wxMenuColour which);

    /**
        Get the pen to use for drawing wxStaticBox border in dark mode.

        Returning an invalid pen indicates that the default border drawn by the
        system should be used, which doesn't look very well in dark mode but
        shouldn't result in any problems worse than cosmetic ones.

        The base class version returns a grey pen, which looks better than the
        default white one.
     */
    virtual wxPen GetBorderPen();
};
