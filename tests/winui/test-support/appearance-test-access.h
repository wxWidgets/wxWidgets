/////////////////////////////////////////////////////////////////////////////
// Private, non-installed WinUI appearance policy test adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_APPEARANCE_TEST_ACCESS_H
#define WX_WINUI_APPEARANCE_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/appearance.h"

#if wxUSE_WINUI3

// Compact deterministic observation of local DependencyProperty ownership and
// UIA projection. It is deliberately unavailable to installed consumers.
struct wxWinUIAppearanceSnapshot
{
    bool hasFontFamily = false;
    bool hasFontSize = false;
    bool hasFontWeight = false;
    bool hasFontStyle = false;
    bool hasForeground = false;
    bool hasBackground = false;
    wxString automationName;
    wxString accessKey;
    wxString localizedControlType;
    bool hasAutomationPeer = false;
    int automationControlType = -1;
    wxString automationClassName;
    wxString peerName;
    wxString peerLocalizedControlType;
};

class WXDLLIMPEXP_CORE wxWinUIAppearanceTestAccess final
{
public:
    enum class HighContrastOverride
    {
        System,
        ForceOff,
        ForceOn
    };

    static void SetHighContrastOverride(HighContrastOverride value);

    static wxWinUIAppearanceSnapshot Capture(
        const winrt::Microsoft::UI::Xaml::Controls::TextBlock& text,
        const winrt::Microsoft::UI::Xaml::Controls::Border& background,
        const winrt::Microsoft::UI::Xaml::UIElement& accessibleElement);

    static wxWinUIAppearanceSnapshot Capture(
        const winrt::Microsoft::UI::Xaml::Controls::Control& control,
        const winrt::Microsoft::UI::Xaml::UIElement& accessibleElement);
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_APPEARANCE_TEST_ACCESS_H
