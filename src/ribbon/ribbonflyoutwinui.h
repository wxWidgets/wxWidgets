/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/ribbonflyoutwinui.h
// Purpose:     Exact native Toolkit collapsed-group template boundary
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_FLYOUT_WINUI_H_
#define _WX_RIBBON_FLYOUT_WINUI_H_

#include "ribbontoolkitruntime.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

enum class wxRibbonFlyoutResolveResult
{
    Ready,
    Pending,
    Invalid
};

// An explicit short-lived snapshot, not a cache of template parts. The popup
// session may retain it for retirement, but must call IsCurrent() before any
// new publication. Retemplating invalidates the old identities even when the
// same ControlTemplate resource is applied again.
struct wxRibbonFlyoutParts
{
    winrt::XamlToolkit::Labs::WinUI::RibbonCollapsibleGroup group{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ControlTemplate controlTemplate{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::Button button{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::Flyout flyout{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ContentControl inlineMount{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ContentControl popupMount{nullptr};
};

// Applies the current template but never opens/closes its Flyout or changes
// State. Failure clears result; Pending is only legitimate before Loaded.
WXDLLIMPEXP_RIBBON wxRibbonFlyoutResolveResult wxRibbonResolveFlyoutParts(
    const winrt::XamlToolkit::Labs::WinUI::RibbonCollapsibleGroup& group,
    wxRibbonFlyoutParts& result) noexcept;

// Read the existing namescope without materializing or replacing a template.
WXDLLIMPEXP_RIBBON wxRibbonFlyoutResolveResult wxRibbonReadCurrentFlyoutParts(
    const winrt::XamlToolkit::Labs::WinUI::RibbonCollapsibleGroup& group,
    wxRibbonFlyoutParts& result) noexcept;

// Does not ApplyTemplate, pump messages, or repair a stale snapshot.
WXDLLIMPEXP_RIBBON bool wxRibbonFlyoutPartsAreCurrent(
    const wxRibbonFlyoutParts& parts) noexcept;

// The native Ribbon layout owns State. Leave result unchanged on failure.
WXDLLIMPEXP_RIBBON bool wxRibbonReadFlyoutState(
    const winrt::XamlToolkit::Labs::WinUI::RibbonCollapsibleGroup& group,
    winrt::Microsoft::UI::Xaml::Visibility& result) noexcept;

#endif
#endif // _WX_RIBBON_FLYOUT_WINUI_H_
