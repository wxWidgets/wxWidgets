/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/ribbonflyoutwinui.cpp
// Purpose:     Exact native Toolkit collapsed-group template boundary
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "ribbonflyoutwinui.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

#include <vector>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace Labs = winrt::XamlToolkit::Labs::WinUI;

// These names are from the pinned 0.8.0-alpha resource package, corroborated
// by upstream RibbonCollapsibleGroup and RibbonStyle.xaml. They are resolved
// in this exact control's template namescope, never by a global FindName or
// by choosing an arbitrary descendant button.
constexpr auto InlineName = L"VisibleContentContainer";
constexpr auto ButtonName = L"CollapsedButton";
constexpr auto FlyoutName = L"CollapsedFlyout";
constexpr auto PopupName = L"CollapsedContentPresenter";

bool InVisualTree(X::DependencyObject node, const X::DependencyObject& root)
{
    // A malformed custom template must not turn validation into an unbounded
    // walk. Real templates are shallow; exceeding the bound fails closed.
    for ( unsigned depth = 0; node && depth != 256; ++depth )
    {
        if ( node == root ) return true;
        node = X::Media::VisualTreeHelper::GetParent(node);
    }
    return false;
}

bool InPopupContent(const X::DependencyObject& node,
                    const X::DependencyObject& root)
{
    if ( !node || !root ) return false;
    if ( InVisualTree(node, root) ) return true;

    // A closed Flyout has no loaded presenter yet. Its declared Panel children
    // and ContentControl.Content still prove ownership without opening it or
    // guessing a PopupRoot. Once it opens, the host resolves the exact Popup
    // independently and owns its physical-disconnect receipt.
    std::vector<X::DependencyObject> pending{root};
    std::vector<X::DependencyObject> visited;
    for ( size_t next = 0; next != pending.size() && next != 256; ++next )
    {
        const auto current = pending[next];
        if ( current == node ) return true;
        bool seen = false;
        for ( const auto& previous : visited )
            if ( previous == current ) { seen = true; break; }
        if ( seen ) continue;
        visited.push_back(current);
        if ( const auto panel = current.try_as<C::Panel>() )
        {
            for ( const auto& child : panel.Children() )
                if ( pending.size() < 256 ) pending.push_back(child);
        }
        if ( const auto content = current.try_as<C::ContentControl>() )
        {
            const auto child = content.Content().try_as<X::DependencyObject>();
            if ( child && pending.size() < 256 ) pending.push_back(child);
        }
        const int count = X::Media::VisualTreeHelper::GetChildrenCount(current);
        for ( int i = 0; i < count && pending.size() < 256; ++i )
            pending.push_back(X::Media::VisualTreeHelper::GetChild(current, i));
    }
    return false;
}

bool SameParts(const wxRibbonFlyoutParts& left,
               const wxRibbonFlyoutParts& right)
{
    return left.group == right.group &&
           left.controlTemplate == right.controlTemplate &&
           left.button == right.button && left.flyout == right.flyout &&
           left.inlineMount == right.inlineMount &&
           left.popupMount == right.popupMount;
}

wxRibbonFlyoutResolveResult ReadParts(
    const Labs::RibbonCollapsibleGroup& group, wxRibbonFlyoutParts& result)
{
    using Status = wxRibbonFlyoutResolveResult;
    if ( !group ) return Status::Invalid;
    const auto control = group.try_as<C::IControlProtected>();
    if ( !control ) return Status::Invalid;
    const auto controlTemplate = group.Template();
    if ( !controlTemplate )
        return group.IsLoaded() ? Status::Invalid : Status::Pending;

    const auto inlinePart = control.GetTemplateChild(InlineName);
    const auto buttonPart = control.GetTemplateChild(ButtonName);
    const auto flyoutPart = control.GetTemplateChild(FlyoutName);
    const auto popupPart = control.GetTemplateChild(PopupName);
    const auto inlineMount = inlinePart.try_as<C::ContentControl>();
    const auto button = buttonPart.try_as<C::Button>();
    const auto flyout = flyoutPart.try_as<C::Flyout>();
    const auto popupMount = popupPart.try_as<C::ContentControl>();
    if ( (inlinePart && !inlineMount) || (buttonPart && !button) ||
         (flyoutPart && !flyout) || (popupPart && !popupMount) )
        return Status::Invalid;
    if ( !inlineMount || !button || !flyout || !popupMount )
    {
        // A materialized tree with missing names is an incompatible template,
        // even if the control has not yet raised Loaded.
        return group.IsLoaded() ||
               X::Media::VisualTreeHelper::GetChildrenCount(group) != 0
                   ? Status::Invalid : Status::Pending;
    }

    // WinUI's projected FrameworkElement has no TemplatedParent getter.
    // IControlProtected::GetTemplateChild supplies the exact namescope
    // authority instead, followed by concrete ownership/type verification.
    if ( inlineMount == popupMount || button.Flyout() != flyout ||
         !InVisualTree(button, group) || !InVisualTree(inlineMount, group) ||
         !InPopupContent(popupMount,
                         flyout.Content().try_as<X::DependencyObject>()) )
        return Status::Invalid;

    // Native property/template callbacks are a reentrancy boundary. Reject a
    // mixed snapshot if any identity changed while checking relationships.
    if ( group.Template() != controlTemplate ||
         control.GetTemplateChild(InlineName) != inlineMount ||
         control.GetTemplateChild(ButtonName) != button ||
         control.GetTemplateChild(FlyoutName) != flyout ||
         control.GetTemplateChild(PopupName) != popupMount ||
         button.Flyout() != flyout )
        return Status::Invalid;

    result = {group, controlTemplate, button, flyout, inlineMount, popupMount};
    return Status::Ready;
}
} // namespace

wxRibbonFlyoutResolveResult wxRibbonResolveFlyoutParts(
    const Labs::RibbonCollapsibleGroup& group,
    wxRibbonFlyoutParts& result) noexcept
{
    result = {};
    try
    {
        if ( !group ) return wxRibbonFlyoutResolveResult::Invalid;
        group.ApplyTemplate();
        return ReadParts(group, result);
    }
    catch ( ... )
    {
        result = {};
        return wxRibbonFlyoutResolveResult::Invalid;
    }
}

bool wxRibbonFlyoutPartsAreCurrent(const wxRibbonFlyoutParts& parts) noexcept
{
    try
    {
        wxRibbonFlyoutParts current;
        return ReadParts(parts.group, current) ==
                   wxRibbonFlyoutResolveResult::Ready &&
               SameParts(parts, current);
    }
    catch ( ... )
    {
        return false;
    }
}

wxRibbonFlyoutResolveResult wxRibbonReadCurrentFlyoutParts(
    const Labs::RibbonCollapsibleGroup& group, wxRibbonFlyoutParts& result) noexcept
{
    result = {};
    try { return ReadParts(group, result); }
    catch ( ... ) { return wxRibbonFlyoutResolveResult::Invalid; }
}

bool wxRibbonReadFlyoutState(const Labs::RibbonCollapsibleGroup& group,
                           X::Visibility& result) noexcept
{
    try
    {
        if ( !group ) return false;
        const auto state = group.State();
        if ( state != X::Visibility::Visible &&
             state != X::Visibility::Collapsed ) return false;
        result = state;
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

#endif
