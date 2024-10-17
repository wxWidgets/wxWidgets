///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/appearance.h
// Purpose:     Private wxGTK functions for working with appearance settings
// Copyright:   (c) 2024 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_APPEARANCE_H_
#define _WX_GTK_PRIVATE_APPEARANCE_H_

namespace wxGTKImpl
{

// This enum uses the same values as org.freedesktop.appearance.color-scheme
enum class ColorScheme
{
    NoPreference = 0,
    PreferDark   = 1,
    PreferLight  = 2
};

// Update GTK color scheme if possible.
//
// Return false if the preference wasn't changed because it is overridden by
// the theme anyhow.
bool UpdateColorScheme(ColorScheme colorScheme);

} // namespace wxGTKImpl

#endif // _WX_GTK_PRIVATE_APPEARANCE_H_
