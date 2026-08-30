/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/slider.cpp
// Purpose:     wxWinUI wxSlider implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SLIDER

#include "wx/slider.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "slider-test-access.h"
#endif
#include "wx/stopwatch.h"
#include "wx/winui/private/appearance.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/event.h"
#endif

#include "private.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

// Gap between an endpoint label and the track, in DIPs: the same value wxMSW
// leaves between them (VGAP there).
constexpr double wxWinUISliderLabelGap = 4.0;

void wxWinUIMarkSliderDecorationRaw(const MUX::UIElement& element)
{
    if ( element )
    {
        MUXA::AutomationProperties::SetAccessibilityView(
            element, MUXAP::AccessibilityView::Raw);
    }
}

// XAML owns every delegate. It may therefore outlive the wx object while a
// routed event or a queued unknown-input classification is unwinding. The
// delegates retain only this invalidatable state, never a raw wxSlider.
class wxWinUISliderCallbackState
{
public:
    explicit wxWinUISliderCallbackState(wxSlider *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxSlider *GetOwner(std::uint64_t generation) const
    {
        if ( generation != m_generation.load(std::memory_order_acquire) )
            return nullptr;

        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
    }

    void BeginPeerMutation()
    {
        m_peerMutationDepth.fetch_add(1, std::memory_order_acq_rel);
    }

    void EndPeerMutation()
    {
        m_peerMutationDepth.fetch_sub(1, std::memory_order_acq_rel);
    }

    bool IsPeerMutationInProgress() const
    {
        return m_peerMutationDepth.load(std::memory_order_acquire) != 0;
    }

private:
    std::atomic<wxSlider *> m_owner;
    std::atomic<std::uint64_t> m_generation{1};
    std::atomic<unsigned> m_peerMutationDepth{0};
};

class wxWinUISliderPeerMutationGuard
{
public:
    explicit wxWinUISliderPeerMutationGuard(
        const std::shared_ptr<wxWinUISliderCallbackState>& state)
        : m_state(state)
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUISliderPeerMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

private:
    std::shared_ptr<wxWinUISliderCallbackState> m_state;
};

struct wxWinUISliderPendingChange
{
    int oldValue;
    int newValue;
};

template<typename T>
winrt::Windows::Foundation::IUnknown
wxWinUISliderControllingIdentity(const T& object) noexcept
{
    // Projected interfaces for one WinRT object may expose different ABI
    // pointers. QI for IUnknown is the COM identity operation and therefore
    // remains stable across DependencyObject/UIElement/Control projections.
    return object
        ? object.template try_as<winrt::Windows::Foundation::IUnknown>()
        : nullptr;
}

template<typename First, typename Second>
bool wxWinUISameSliderObject(const First& first,
                             const Second& second) noexcept
{
    return first && second &&
           wxWinUISliderControllingIdentity(first) ==
               wxWinUISliderControllingIdentity(second);
}

void wxWinUICollectSliderThumbs(
    const MUX::DependencyObject& root,
    std::vector<MUXCP::Thumb> *thumbs)
{
    if ( !root || !thumbs )
        return;

    if ( const MUXCP::Thumb thumb = root.try_as<MUXCP::Thumb>() )
        thumbs->push_back(thumb);

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        wxWinUICollectSliderThumbs(
            MUXM::VisualTreeHelper::GetChild(root, i), thumbs);
    }
}

void wxWinUICollectNamedSliderElements(
    const MUX::DependencyObject& root,
    const winrt::hstring& expectedName,
    std::vector<MUX::FrameworkElement> *matches)
{
    if ( !root || !matches )
        return;

    if ( const MUX::FrameworkElement element =
             root.try_as<MUX::FrameworkElement>() )
    {
        if ( element.Name() == expectedName )
            matches->push_back(element);
    }

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        wxWinUICollectNamedSliderElements(
            MUXM::VisualTreeHelper::GetChild(root, i),
            expectedName,
            matches);
    }
}

bool wxWinUIIsRealizedSliderThumbFallback(
    const MUXCP::Thumb& thumb,
    const MUXC::Slider& slider,
    bool vertical)
{
    if ( !thumb || !slider )
        return false;

    MUX::DependencyObject node = thumb;
    while ( node )
    {
        if ( const MUX::UIElement element =
                 node.try_as<MUX::UIElement>() )
        {
            if ( element.Visibility() != MUX::Visibility::Visible )
                return false;
        }

        if ( wxWinUISameSliderObject(node, slider) )
        {
            const double extent = vertical
                ? thumb.ActualHeight()
                : thumb.ActualWidth();
            return extent > 0.0 && std::isfinite(extent);
        }

        node = MUXM::VisualTreeHelper::GetParent(node);
    }

    return false;
}

MUXCP::Thumb wxWinUIFindSliderThumb(const MUXC::Slider& slider)
{
    if ( !slider )
        return nullptr;

    const bool vertical =
        slider.Orientation() == MUXC::Orientation::Vertical;
    const winrt::hstring expectedName =
        vertical ? L"VerticalThumb" : L"HorizontalThumb";
    std::vector<MUXCP::Thumb> thumbs;
    wxWinUICollectSliderThumbs(slider, &thumbs);

    MUXCP::Thumb named{ nullptr };
    MUXCP::Thumb fallback{ nullptr };
    bool fallbackAmbiguous = false;
    for ( const MUXCP::Thumb& thumb : thumbs )
    {
        if ( thumb.Name() == expectedName )
        {
            // A XAML namescope cannot legitimately contain two elements with
            // the same name. Fail closed if a custom template violates this
            // instead of silently choosing an unstable identity.
            if ( named && !wxWinUISameSliderObject(named, thumb) )
                return nullptr;
            named = thumb;
            continue;
        }

        // Custom templates are supported, but an unnamed/mismatched Thumb is
        // eligible only when every ancestor up to this Slider is visible and
        // its orientation-specific arranged extent is strictly positive.
        if ( !wxWinUIIsRealizedSliderThumbFallback(
                 thumb, slider, vertical) )
        {
            continue;
        }

        if ( fallback && !wxWinUISameSliderObject(fallback, thumb) )
            fallbackAmbiguous = true;
        else
            fallback = thumb;
    }

    if ( named )
        return named;
    return fallbackAmbiguous ? MUXCP::Thumb{nullptr} : fallback;
}

MUX::FrameworkElement wxWinUIFindSliderTrack(
    const MUXC::Slider& slider,
    const MUXCP::Thumb& thumb)
{
    if ( !slider || !thumb )
        return nullptr;

    const bool vertical =
        slider.Orientation() == MUXC::Orientation::Vertical;
    const winrt::hstring expectedName =
        vertical ? L"VerticalTrackRect" : L"HorizontalTrackRect";
    std::vector<MUX::FrameworkElement> matches;
    wxWinUICollectNamedSliderElements(slider, expectedName, &matches);

    MUX::FrameworkElement named{ nullptr };
    for ( const MUX::FrameworkElement& candidate : matches )
    {
        if ( candidate.Visibility() != MUX::Visibility::Visible )
            continue;
        if ( named && !wxWinUISameSliderObject(named, candidate) )
            return nullptr;
        named = candidate;
    }
    if ( named )
        return named;

    // A custom template need not use the stock TrackRect name. Its nearest
    // realized Thumb ancestor with a strictly larger primary extent is the
    // layout track which constrains Thumb travel. Identity comparisons use
    // controlling IUnknown because the walk crosses projected interfaces.
    const double thumbExtent = vertical
        ? thumb.ActualHeight()
        : thumb.ActualWidth();
    MUX::DependencyObject node = MUXM::VisualTreeHelper::GetParent(thumb);
    while ( node && !wxWinUISameSliderObject(node, slider) )
    {
        if ( const MUX::FrameworkElement candidate =
                 node.try_as<MUX::FrameworkElement>() )
        {
            const double extent = vertical
                ? candidate.ActualHeight()
                : candidate.ActualWidth();
            if ( candidate.Visibility() == MUX::Visibility::Visible &&
                 std::isfinite(extent) && extent > thumbExtent + 0.25 )
            {
                return candidate;
            }
        }
        node = MUXM::VisualTreeHelper::GetParent(node);
    }
    return nullptr;
}

bool wxWinUIIsSliderThumbSource(
    const winrt::Windows::Foundation::IInspectable& source,
    const MUXC::Slider& slider)
{
    const MUXCP::Thumb resolved = wxWinUIFindSliderThumb(slider);
    if ( !resolved )
        return false;

    MUX::DependencyObject node =
        source.try_as<MUX::DependencyObject>();
    while ( node )
    {
        if ( const MUXCP::Thumb thumb = node.try_as<MUXCP::Thumb>() )
            return wxWinUISameSliderObject(thumb, resolved);
        if ( wxWinUISameSliderObject(node, slider) )
            break;
        node = MUXM::VisualTreeHelper::GetParent(node);
    }

    return false;
}

bool wxWinUIHasExactSliderParent(
    const MUX::DependencyObject& child,
    const MUX::DependencyObject& expectedParent)
{
    return child && expectedParent &&
           wxWinUISameSliderObject(
               MUXM::VisualTreeHelper::GetParent(child),
               expectedParent);
}

bool wxWinUIHasSliderXamlRoot(
    const MUX::FrameworkElement& element,
    const MUX::XamlRoot& expectedRoot)
{
    return element && expectedRoot &&
           wxWinUISameSliderObject(element.XamlRoot(), expectedRoot);
}

std::uintptr_t wxWinUISliderObjectIdentity(
    const winrt::Windows::Foundation::IInspectable& object)
{
    const auto identity = wxWinUISliderControllingIdentity(object);
    return reinterpret_cast<std::uintptr_t>(winrt::get_abi(identity));
}

#ifdef WXWINUI_TEST_SUPPORT
std::uint32_t wxWinUIPackSliderColor(
    const winrt::Windows::UI::Color& color)
{
    return (static_cast<std::uint32_t>(color.A) << 24) |
           (static_cast<std::uint32_t>(color.R) << 16) |
           (static_cast<std::uint32_t>(color.G) << 8) |
           static_cast<std::uint32_t>(color.B);
}

wxWinUISliderTestAccess::ActualTheme wxWinUIGetObservedSliderTheme(
    MUX::ElementTheme theme)
{
    switch ( theme )
    {
        case MUX::ElementTheme::Light:
            return wxWinUISliderTestAccess::ActualTheme::Light;

        case MUX::ElementTheme::Dark:
            return wxWinUISliderTestAccess::ActualTheme::Dark;

        case MUX::ElementTheme::Default:
            return wxWinUISliderTestAccess::ActualTheme::Default;
    }

    return wxWinUISliderTestAccess::ActualTheme::Default;
}
#endif

double wxWinUIGetSliderRasterScale(const MUXC::Slider& slider,
                                   double fallbackScale)
{
    if ( slider )
    {
        if ( const MUX::XamlRoot root = slider.XamlRoot() )
        {
            const double scale = root.RasterizationScale();
            if ( scale > 0.0 && std::isfinite(scale) )
                return scale;
        }
    }

    return fallbackScale > 0.0 && std::isfinite(fallbackScale)
        ? fallbackScale
        : 1.0;
}

MUXC::Border wxWinUICreateSliderSelectionBorder(
    const wxString& resourceKey,
    const winrt::Windows::UI::Color& fallback,
    bool *themeBound)
{
    MUXC::Border border = wxWinUICreateThemeBrushBorder(
        resourceKey, wxWinUIThemeBrushProperty::Background);
    *themeBound = !!border;
    if ( !border )
    {
        border = MUXC::Border();
        border.Background(MUXM::SolidColorBrush(fallback));
    }

    border.IsHitTestVisible(false);
    border.Visibility(MUX::Visibility::Collapsed);
    return border;
}

bool wxWinUIEmitSliderScroll(
    const std::shared_ptr<wxWinUISliderCallbackState>& state,
    std::uint64_t generation,
    wxEventType type,
    int value)
{
    wxSlider * const owner = state->GetOwner(generation);
    if ( !owner )
        return false;

    wxScrollEvent event(
        type,
        owner->GetId(),
        value,
        owner->HasFlag(wxSL_VERTICAL) ? wxVERTICAL : wxHORIZONTAL);
    event.SetEventObject(owner);
    owner->HandleWindowEvent(event);

    return state->GetOwner(generation) != nullptr;
}

bool wxWinUIEmitSliderCommand(
    const std::shared_ptr<wxWinUISliderCallbackState>& state,
    std::uint64_t generation,
    int value)
{
    wxSlider * const owner = state->GetOwner(generation);
    if ( !owner )
        return false;

    wxCommandEvent event(wxEVT_SLIDER, owner->GetId());
    event.SetEventObject(owner);
    event.SetInt(value);
    owner->ProcessCommand(event);

    return state->GetOwner(generation) != nullptr;
}

bool wxWinUIEmitSliderScrollAndCommand(
    const std::shared_ptr<wxWinUISliderCallbackState>& state,
    std::uint64_t generation,
    wxEventType type,
    int value)
{
    if ( !wxWinUIEmitSliderScroll(state, generation, type, value) )
        return false;

    return wxWinUIEmitSliderCommand(state, generation, value);
}

} // anonymous namespace

class wxWinUISliderImpl
{
public:
    static bool GetRealizedRect(
        const MUX::FrameworkElement& element,
        const MUX::UIElement& reference,
        wxSlider::WinUIVisualRect *rect)
    {
        if ( !element || !reference || !rect ||
             element.Visibility() != MUX::Visibility::Visible )
        {
            return false;
        }

        const double width = element.ActualWidth();
        const double height = element.ActualHeight();
        if ( width <= 0.0 || height <= 0.0 ||
             !std::isfinite(width) || !std::isfinite(height) )
        {
            return false;
        }

        const auto transform = element.TransformToVisual(reference);
        if ( !transform )
            return false;

        const winrt::Windows::Foundation::Rect bounds =
            transform.TransformBounds(
                winrt::Windows::Foundation::Rect{
                    0.0f,
                    0.0f,
                    static_cast<float>(width),
                    static_cast<float>(height)});
        if ( !std::isfinite(bounds.X) || !std::isfinite(bounds.Y) ||
             !std::isfinite(bounds.Width) ||
             !std::isfinite(bounds.Height) || bounds.Width <= 0.0f ||
             bounds.Height <= 0.0f )
        {
            return false;
        }

        rect->x = bounds.X;
        rect->y = bounds.Y;
        rect->width = bounds.Width;
        rect->height = bounds.Height;
        return true;
    }

    static bool GetKeyInput(
        winrt::Windows::System::VirtualKey key,
        wxSlider::WinUIInput *input)
    {
        using winrt::Windows::System::VirtualKey;

        switch ( key )
        {
            case VirtualKey::Left:
            case VirtualKey::Up:
                *input = wxSlider::WinUIInput::LineDecrement;
                return true;

            case VirtualKey::Right:
            case VirtualKey::Down:
                *input = wxSlider::WinUIInput::LineIncrement;
                return true;

            case VirtualKey::PageUp:
                *input = wxSlider::WinUIInput::PageDecrement;
                return true;

            case VirtualKey::PageDown:
                *input = wxSlider::WinUIInput::PageIncrement;
                return true;

            case VirtualKey::Home:
                *input = wxSlider::WinUIInput::Minimum;
                return true;

            case VirtualKey::End:
                *input = wxSlider::WinUIInput::Maximum;
                return true;

            default:
                return false;
        }
    }

    static wxEventType GetScrollEvent(
        wxSlider::WinUIInput input)
    {
        switch ( input )
        {
            case wxSlider::WinUIInput::LineDecrement:
                return wxEVT_SCROLL_LINEUP;

            case wxSlider::WinUIInput::LineIncrement:
                return wxEVT_SCROLL_LINEDOWN;

            case wxSlider::WinUIInput::PageDecrement:
                return wxEVT_SCROLL_PAGEUP;

            case wxSlider::WinUIInput::PageIncrement:
                return wxEVT_SCROLL_PAGEDOWN;

            case wxSlider::WinUIInput::Minimum:
                return wxEVT_SCROLL_TOP;

            case wxSlider::WinUIInput::Maximum:
                return wxEVT_SCROLL_BOTTOM;

            case wxSlider::WinUIInput::ThumbTrack:
            case wxSlider::WinUIInput::ThumbRelease:
                return wxEVT_SCROLL_THUMBTRACK;

            case wxSlider::WinUIInput::Wheel:
            case wxSlider::WinUIInput::Automation:
                return wxEVT_SCROLL_CHANGED;
        }

        wxFAIL_MSG("unhandled WinUI slider input source");
        return wxEVT_SCROLL_CHANGED;
    }

    enum class PointerInteraction
    {
        None,
        Thumb,
        Track
    };

    ~wxWinUISliderImpl()
    {
        Close();
    }

    void Close()
    {
        // Invalidate first: RemoveHandler(), host detachment and a focused
        // peer can all synchronously drain routed callbacks.
        if ( callbackState )
            callbackState->Invalidate();

        auto removeRouted =
            [this](MUX::RoutedEvent const& routedEvent,
                   auto const& handler,
                   bool& attached,
                   const char *context)
            {
                if ( !slider || !handler || !attached )
                    return;

                try
                {
                    slider.RemoveHandler(
                        routedEvent, winrt::box_value(handler));
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(context, e);
                }
                attached = false;
            };

        if ( slider && valueChangedToken.value )
        {
            try
            {
                slider.ValueChanged(valueChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Slider ValueChanged removal", e);
            }
        }
        valueChangedToken = {};

        removeRouted(
            MUX::UIElement::PointerPressedEvent(),
            pointerPressedHandler,
            pointerPressedAttached,
            "WinUI Slider PointerPressed removal");
        removeRouted(
            MUX::UIElement::PointerReleasedEvent(),
            pointerReleasedHandler,
            pointerReleasedAttached,
            "WinUI Slider PointerReleased removal");
        removeRouted(
            MUX::UIElement::PointerCaptureLostEvent(),
            pointerCaptureLostHandler,
            pointerCaptureLostAttached,
            "WinUI Slider PointerCaptureLost removal");
        removeRouted(
            MUX::UIElement::PointerWheelChangedEvent(),
            pointerWheelHandler,
            pointerWheelAttached,
            "WinUI Slider PointerWheelChanged removal");
        removeRouted(
            MUX::UIElement::PreviewKeyDownEvent(),
            previewKeyDownHandler,
            previewKeyDownAttached,
            "WinUI Slider PreviewKeyDown removal");
        removeRouted(
            MUX::UIElement::KeyUpEvent(),
            keyUpHandler,
            keyUpAttached,
            "WinUI Slider KeyUp removal");

        if ( root && sizeChangedToken.value )
        {
            try
            {
                root.SizeChanged(sizeChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Slider SizeChanged removal", e);
            }
        }
        sizeChangedToken = {};

        if ( slider && loadedToken.value )
        {
            try
            {
                slider.Loaded(loadedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Slider Loaded removal", e);
            }
        }
        loadedToken = {};

        if ( slider && layoutUpdatedToken.value )
        {
            try
            {
                slider.LayoutUpdated(layoutUpdatedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Slider LayoutUpdated removal", e);
            }
        }
        layoutUpdatedToken = {};

        if ( slider && actualThemeChangedToken.value )
        {
            try
            {
                slider.ActualThemeChanged(actualThemeChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Slider ActualThemeChanged removal", e);
            }
        }
        actualThemeChangedToken = {};

        if ( slider && isEnabledCallbackToken )
        {
            try
            {
                slider.UnregisterPropertyChangedCallback(
                    MUXC::Control::IsEnabledProperty(),
                    isEnabledCallbackToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Slider IsEnabled callback removal", e);
            }
        }
        isEnabledCallbackToken = 0;

        pendingChanges.clear();
        pendingFlushScheduled = false;
        ++deferredGeneration;
        inputActive = false;
        inputChanged = false;
        pointerInteraction = PointerInteraction::None;

        host.Close();
        root = nullptr;
        decorationCanvas = nullptr;
        tickCanvas = nullptr;
        selectionNormal = nullptr;
        selectionDisabled = nullptr;
        selectionHighContrast = nullptr;
        selectionHighContrastDisabled = nullptr;
        minimumLabel = nullptr;
        maximumLabel = nullptr;
        valueLabel = nullptr;
        appliedThumb = nullptr;
        fallbackForeground = nullptr;
        slider = nullptr;
        pointerPressedHandler = nullptr;
        pointerReleasedHandler = nullptr;
        pointerCaptureLostHandler = nullptr;
        pointerWheelHandler = nullptr;
        previewKeyDownHandler = nullptr;
        keyUpHandler = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUISliderCallbackState> callbackState;
    MUXC::Grid root{ nullptr };
    MUXC::Slider slider{ nullptr };
    MUXC::Canvas decorationCanvas{ nullptr };
    MUXC::Canvas tickCanvas{ nullptr };
    MUXC::Border selectionNormal{ nullptr };
    MUXC::Border selectionDisabled{ nullptr };
    MUXC::Border selectionHighContrast{ nullptr };
    MUXC::Border selectionHighContrastDisabled{ nullptr };
    MUXC::TextBlock minimumLabel{ nullptr };
    MUXC::TextBlock maximumLabel{ nullptr };
    MUXC::TextBlock valueLabel{ nullptr };
    MUXCP::Thumb appliedThumb{ nullptr };
    MUXM::Brush fallbackForeground{ nullptr };
    winrt::event_token valueChangedToken{};
    winrt::event_token sizeChangedToken{};
    winrt::event_token loadedToken{};
    winrt::event_token layoutUpdatedToken{};
    winrt::event_token actualThemeChangedToken{};
    std::int64_t isEnabledCallbackToken = 0;
    double appliedThumbLength = -1.0;
    bool selectionNormalThemeBound = false;
    bool selectionDisabledThemeBound = false;
    bool selectionHighContrastThemeBound = false;
    bool selectionHighContrastDisabledThemeBound = false;
    bool updatingVisualState = false;
    // A visual update made before the shared slot has a non-empty arranged
    // size is only a request, never a committed layout. Loaded/SizeChanged/
    // LayoutUpdated consume this latch once the exact peer is realizable.
    bool visualStateDirty = true;
    bool visualLayoutReady = false;
    bool visualCacheValid = false;
    double visualWidth = -1.0;
    double visualHeight = -1.0;
    double visualRasterScale = -1.0;
    double visualFontSize = -1.0;
    double visualMinimumLabelWidth = -1.0;
    double visualMinimumLabelHeight = -1.0;
    double visualMaximumLabelWidth = -1.0;
    double visualMaximumLabelHeight = -1.0;
    double visualValueLabelWidth = -1.0;
    double visualValueLabelHeight = -1.0;
    int visualValue = 0;
    int visualMinimum = 0;
    int visualMaximum = 0;
    int visualSelectionStart = 0;
    int visualSelectionEnd = 0;
    int visualTickFrequency = 0;
    double visualThumbLengthDIP = -1.0;
    double visualAxisCross = 0.0;
    double visualAxisStart = 0.0;
    double visualAxisEnd = 0.0;
    double visualThumbAxis = 0.0;
    std::uintptr_t visualThumbIdentity = 0;
    std::uintptr_t visualTrackIdentity = 0;
    bool visualAxisReversed = false;
    long visualStyle = 0;
    wxLayoutDirection visualDirection = wxLayout_Default;
    bool visualTicksCleared = false;
    bool visualEnabled = true;
    bool visualHighContrast = false;
    MUX::ElementTheme visualActualTheme = MUX::ElementTheme::Default;
    std::size_t visualTickHash = 0;
    MUXM::Brush visualBrush{ nullptr };
#ifdef WXWINUI_TEST_SUPPORT
    wxWinUISliderTestAccess::ConvergenceFailure lastConvergenceFailure =
        wxWinUISliderTestAccess::ConvergenceFailure::None;
#endif

    MUXI::PointerEventHandler pointerPressedHandler{ nullptr };
    MUXI::PointerEventHandler pointerReleasedHandler{ nullptr };
    MUXI::PointerEventHandler pointerCaptureLostHandler{ nullptr };
    MUXI::PointerEventHandler pointerWheelHandler{ nullptr };
    MUXI::KeyEventHandler previewKeyDownHandler{ nullptr };
    MUXI::KeyEventHandler keyUpHandler{ nullptr };
    bool pointerPressedAttached = false;
    bool pointerReleasedAttached = false;
    bool pointerCaptureLostAttached = false;
    bool pointerWheelAttached = false;
    bool previewKeyDownAttached = false;
    bool keyUpAttached = false;

    wxSlider::WinUIInput input = wxSlider::WinUIInput::Automation;
    bool inputSourceKnown = false;
    bool inputActive = false;
    bool inputChanged = false;
    bool pendingFlushScheduled = false;
    std::uint64_t deferredGeneration = 1;
    PointerInteraction pointerInteraction = PointerInteraction::None;
    std::vector<wxWinUISliderPendingChange> pendingChanges;
};

wxSlider::wxSlider() = default;

wxSlider::wxSlider(wxWindow *parent,
                   wxWindowID id,
                   int value,
                   int minValue,
                   int maxValue,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxSlider()
{
    Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
}

wxSlider::~wxSlider()
{
    Unbind(wxEVT_DPI_CHANGED, &wxSlider::OnDPIChanged, this);
    if ( m_winui )
    {
        m_winui->Close();
        m_winui.reset();
    }
}

bool wxSlider::Create(wxWindow *parent,
                      wxWindowID id,
                      int value,
                      int minValue,
                      int maxValue,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxCHECK_MSG( minValue < maxValue, false,
                 wxT("Slider minimum must be strictly less than the maximum.") );

    // LEFT/RIGHT and TOP/BOTTOM imply orientation in the public wx contract.
    switch ( style & (wxSL_LEFT | wxSL_RIGHT | wxSL_TOP | wxSL_BOTTOM) )
    {
        case wxSL_LEFT:
        case wxSL_RIGHT:
            style |= wxSL_VERTICAL;
            break;

        case wxSL_TOP:
        case wxSL_BOTTOM:
            style |= wxSL_HORIZONTAL;
            break;
    }

    if ( !(style & (wxSL_HORIZONTAL | wxSL_VERTICAL)) )
        style |= wxSL_HORIZONTAL;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    Bind(wxEVT_DPI_CHANGED, &wxSlider::OnDPIChanged, this);

    m_rangeMin = minValue;
    m_rangeMax = maxValue;
    m_value = ClampValue(value);
    m_selStart = minValue;
    m_selEnd = minValue;
    m_thumbLengthDIP = -1.0;
    // TBS_AUTOTICKS starts with one tick per logical increment. Mirror that
    // visible state explicitly instead of reporting frequency zero while only
    // drawing the two endpoint marks.
    m_tickFreq = HasFlag(wxSL_AUTOTICKS) ? 1 : 0;
    m_ticksCleared = false;
    m_manualTicks.clear();
    const std::int64_t rangeWidth =
        static_cast<std::int64_t>(maxValue) -
        static_cast<std::int64_t>(minValue);
    const std::int64_t defaultPageSize = rangeWidth / 10;
    m_pageSize = static_cast<int>(
        defaultPageSize < 1
            ? 1
            : wxMin(defaultPageSize,
                    static_cast<std::int64_t>(
                        std::numeric_limits<int>::max())));

    m_winui = std::make_unique<wxWinUISliderImpl>();
    m_winui->callbackState =
        std::make_shared<wxWinUISliderCallbackState>(this);
    wxWinUISliderImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUISliderCallbackState> createState =
        m_winui->callbackState;
    const std::uint64_t createGeneration =
        createState->Generation();
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        return false;
    }

    MUXC::Slider createPeer{ nullptr };
    try
    {
        m_winui->slider = MUXC::Slider();
        createPeer = m_winui->slider;
        m_winui->slider.Orientation(
            HasFlag(wxSL_VERTICAL) ? MUXC::Orientation::Vertical
                                   : MUXC::Orientation::Horizontal);
        // The stock Slider template anchors its track near the leading edge
        // when the control is stretched across a cross-axis slot much larger
        // than its desired size. wxSlider owns decorations on both sides of
        // that track, so keep the native peer stretched only on its primary
        // axis and centre its intrinsic cross-axis layout in the wx slot.
        // This is a one-way Grid arrange policy, not a Track-coordinate-to-
        // Margin feedback loop.
        if ( HasFlag(wxSL_VERTICAL) )
            m_winui->slider.HorizontalAlignment(
                MUX::HorizontalAlignment::Center);
        else
            m_winui->slider.VerticalAlignment(
                MUX::VerticalAlignment::Center);
        m_winui->slider.IsDirectionReversed(HasFlag(wxSL_INVERSE));
        m_winui->slider.StepFrequency(1);
        m_winui->slider.SnapsTo(MUXCP::SliderSnapsTo::StepValues);
        // Native Slider ticks are frequency-only. The non-interactive overlay
        // below owns all ticks so SetTick(), ClearTicks() and both-side
        // placement have the same semantics as the Win32 trackbar.
        m_winui->slider.TickPlacement(MUXCP::TickPlacement::None);

        m_winui->root = MUXC::Grid();
        m_winui->decorationCanvas = MUXC::Canvas();
        m_winui->tickCanvas = MUXC::Canvas();
        m_winui->selectionNormal =
            wxWinUICreateSliderSelectionBorder(
                "SliderTrackValueFill",
                winrt::Windows::UI::Color{0xff, 0x00, 0x78, 0xd4},
                &m_winui->selectionNormalThemeBound);
        m_winui->selectionDisabled =
            wxWinUICreateSliderSelectionBorder(
                "SliderTrackValueFillDisabled",
                winrt::Windows::UI::Color{0x66, 0x78, 0x78, 0x78},
                &m_winui->selectionDisabledThemeBound);
        m_winui->selectionHighContrast =
            wxWinUICreateSliderSelectionBorder(
                "SystemColorHighlightColorBrush",
                winrt::Windows::UI::Color{0xff, 0xff, 0xff, 0xff},
                &m_winui->selectionHighContrastThemeBound);
        m_winui->selectionHighContrastDisabled =
            wxWinUICreateSliderSelectionBorder(
                "SystemColorGrayTextColorBrush",
                winrt::Windows::UI::Color{0xff, 0x80, 0x80, 0x80},
                &m_winui->selectionHighContrastDisabledThemeBound);
        m_winui->minimumLabel = MUXC::TextBlock();
        m_winui->maximumLabel = MUXC::TextBlock();
        m_winui->valueLabel = MUXC::TextBlock();

        // Only the native Slider is part of the public accessibility model.
        // Grid/Canvas default to Content view, as do the visible label
        // TextBlocks, which would otherwise add layout and decoration peers
        // around the SliderAutomationPeer selected as the host semantic
        // target. Keep the visual wrapper and every overlay primitive in Raw
        // view; the Slider itself retains its native Content-view contract.
        wxWinUIMarkSliderDecorationRaw(m_winui->root);
        wxWinUIMarkSliderDecorationRaw(m_winui->decorationCanvas);
        wxWinUIMarkSliderDecorationRaw(m_winui->tickCanvas);
        wxWinUIMarkSliderDecorationRaw(m_winui->selectionNormal);
        wxWinUIMarkSliderDecorationRaw(m_winui->selectionDisabled);
        wxWinUIMarkSliderDecorationRaw(m_winui->selectionHighContrast);
        wxWinUIMarkSliderDecorationRaw(
            m_winui->selectionHighContrastDisabled);
        wxWinUIMarkSliderDecorationRaw(m_winui->minimumLabel);
        wxWinUIMarkSliderDecorationRaw(m_winui->maximumLabel);
        wxWinUIMarkSliderDecorationRaw(m_winui->valueLabel);

        m_winui->decorationCanvas.IsHitTestVisible(false);
        m_winui->tickCanvas.IsHitTestVisible(false);
        m_winui->minimumLabel.IsHitTestVisible(false);
        m_winui->maximumLabel.IsHitTestVisible(false);
        m_winui->valueLabel.IsHitTestVisible(false);
        m_winui->minimumLabel.Visibility(MUX::Visibility::Collapsed);
        m_winui->maximumLabel.Visibility(MUX::Visibility::Collapsed);
        m_winui->valueLabel.Visibility(MUX::Visibility::Collapsed);

        m_winui->decorationCanvas.Children().Append(
            m_winui->selectionNormal);
        m_winui->decorationCanvas.Children().Append(
            m_winui->selectionDisabled);
        m_winui->decorationCanvas.Children().Append(
            m_winui->selectionHighContrast);
        m_winui->decorationCanvas.Children().Append(
            m_winui->selectionHighContrastDisabled);
        m_winui->decorationCanvas.Children().Append(m_winui->tickCanvas);
        m_winui->decorationCanvas.Children().Append(m_winui->minimumLabel);
        m_winui->decorationCanvas.Children().Append(m_winui->maximumLabel);
        m_winui->decorationCanvas.Children().Append(m_winui->valueLabel);
        m_winui->root.Children().Append(m_winui->slider);
        m_winui->root.Children().Append(m_winui->decorationCanvas);

        const std::shared_ptr<wxWinUISliderCallbackState> callbackState =
            m_winui->callbackState;
        const std::uint64_t generation = callbackState->Generation();

        m_winui->valueChangedToken = m_winui->slider.ValueChanged(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXCP::RangeBaseValueChangedEventArgs const& event)
            {
                wxSlider * const owner =
                    callbackState->GetOwner(generation);
                if ( !owner ||
                     callbackState->IsPeerMutationInProgress() )
                {
                    return;
                }

                owner->OnPeerValueChanged(
                    static_cast<int>(std::lround(event.NewValue())));
            });

        m_winui->pointerPressedHandler = MUXI::PointerEventHandler(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::PointerRoutedEventArgs const& event)
            {
                if ( wxSlider * const owner =
                        callbackState->GetOwner(generation) )
                {
                    const bool thumb =
                        owner->m_winui &&
                        wxWinUIIsSliderThumbSource(
                            event.OriginalSource(),
                            owner->m_winui->slider);
                    owner->BeginPointerInput(thumb);
                }
            });
        m_winui->slider.AddHandler(
            MUX::UIElement::PointerPressedEvent(),
            winrt::box_value(m_winui->pointerPressedHandler),
            true);
        m_winui->pointerPressedAttached = true;

        m_winui->pointerReleasedHandler = MUXI::PointerEventHandler(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::PointerRoutedEventArgs const&)
            {
                if ( wxSlider * const owner =
                        callbackState->GetOwner(generation) )
                {
                    const bool thumb =
                        owner->m_winui &&
                        owner->m_winui->pointerInteraction ==
                            wxWinUISliderImpl::
                                PointerInteraction::Thumb;
                    if ( owner->m_winui )
                    {
                        owner->m_winui->pointerInteraction =
                            wxWinUISliderImpl::
                                PointerInteraction::None;
                    }
                    owner->EndInput(thumb);
                }
            });
        m_winui->slider.AddHandler(
            MUX::UIElement::PointerReleasedEvent(),
            winrt::box_value(m_winui->pointerReleasedHandler),
            true);
        m_winui->pointerReleasedAttached = true;

        m_winui->pointerCaptureLostHandler = MUXI::PointerEventHandler(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::PointerRoutedEventArgs const&)
            {
                if ( wxSlider * const owner =
                        callbackState->GetOwner(generation) )
                {
                    const bool thumb =
                        owner->m_winui &&
                        owner->m_winui->pointerInteraction ==
                            wxWinUISliderImpl::
                                PointerInteraction::Thumb;
                    if ( owner->m_winui )
                    {
                        owner->m_winui->pointerInteraction =
                            wxWinUISliderImpl::
                                PointerInteraction::None;
                    }
                    owner->EndInput(thumb);
                }
            });
        m_winui->slider.AddHandler(
            MUX::UIElement::PointerCaptureLostEvent(),
            winrt::box_value(m_winui->pointerCaptureLostHandler),
            true);
        m_winui->pointerCaptureLostAttached = true;

        m_winui->pointerWheelHandler = MUXI::PointerEventHandler(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::PointerRoutedEventArgs const&)
            {
                if ( wxSlider * const owner =
                        callbackState->GetOwner(generation) )
                {
                    // Depending on the template, ValueChanged can precede or
                    // follow this handled routed event. Mark future changes
                    // and flush already observed ones through the same path.
                    owner->BeginInput(WinUIInput::Wheel);
                    owner->FlushPendingInput(WinUIInput::Wheel);

                    wxSlider * const liveOwner =
                        callbackState->GetOwner(generation);
                    if ( wxTheApp && liveOwner &&
                         liveOwner->m_winui )
                    {
                        const std::uint64_t deferredGeneration =
                            ++liveOwner->m_winui->
                                deferredGeneration;
                        wxTheApp->CallAfter(
                            [callbackState, generation,
                             deferredGeneration]()
                            {
                                wxSlider * const liveOwner =
                                    callbackState->GetOwner(generation);
                                if ( liveOwner && liveOwner->m_winui &&
                                     liveOwner->m_winui->
                                         deferredGeneration ==
                                             deferredGeneration &&
                                     liveOwner->m_winui->inputSourceKnown &&
                                     liveOwner->m_winui->input ==
                                         WinUIInput::Wheel &&
                                     !liveOwner->m_winui->inputActive )
                                {
                                    liveOwner->m_winui->
                                        inputSourceKnown = false;
                                }
                            });
                    }
                }
            });
        m_winui->slider.AddHandler(
            MUX::UIElement::PointerWheelChangedEvent(),
            winrt::box_value(m_winui->pointerWheelHandler),
            true);
        m_winui->pointerWheelAttached = true;

        m_winui->previewKeyDownHandler = MUXI::KeyEventHandler(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::KeyRoutedEventArgs const& event)
            {
                wxSlider::WinUIInput input;
                if ( !wxWinUISliderImpl::GetKeyInput(event.Key(), &input) )
                    return;

                wxSlider *owner = callbackState->GetOwner(generation);
                if ( !owner || !owner->IsEnabled() || !owner->m_winui )
                    return;

                int target = owner->m_value;
                const bool reversed = owner->HasFlag(wxSL_INVERSE);
                switch ( input )
                {
                    case WinUIInput::LineDecrement:
                    case WinUIInput::LineIncrement:
                    case WinUIInput::PageDecrement:
                    case WinUIInput::PageIncrement:
                    {
                        const int magnitude =
                            input == WinUIInput::LineDecrement ||
                            input == WinUIInput::LineIncrement
                                ? owner->m_lineSize
                                : owner->m_pageSize;
                        bool increment =
                            input == WinUIInput::LineIncrement ||
                            input == WinUIInput::PageIncrement;
                        if ( reversed )
                            increment = !increment;

                        const long long candidate =
                            static_cast<long long>(owner->m_value) +
                            (increment ? magnitude : -magnitude);
                        if ( candidate < owner->m_rangeMin )
                            target = owner->m_rangeMin;
                        else if ( candidate > owner->m_rangeMax )
                            target = owner->m_rangeMax;
                        else
                            target = static_cast<int>(candidate);
                        break;
                    }

                    case WinUIInput::Minimum:
                        target = reversed ? owner->m_rangeMax
                                          : owner->m_rangeMin;
                        break;

                    case WinUIInput::Maximum:
                        target = reversed ? owner->m_rangeMin
                                          : owner->m_rangeMax;
                        break;

                    case WinUIInput::ThumbTrack:
                    case WinUIInput::ThumbRelease:
                    case WinUIInput::Wheel:
                    case WinUIInput::Automation:
                        return;
                }

                // Own the recognized key instead of relying on the XAML
                // Slider's direction conventions, which differ from the
                // wxMSW trackbar for Up/Down and wxSL_INVERSE.
                event.Handled(true);
                owner->BeginInput(input);
                const MUXC::Slider peer = owner->m_winui->slider;
                try
                {
                    peer.Value(target);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI Slider keyboard input", e);
                }
            });
        m_winui->slider.AddHandler(
            MUX::UIElement::PreviewKeyDownEvent(),
            winrt::box_value(m_winui->previewKeyDownHandler),
            true);
        m_winui->previewKeyDownAttached = true;

        m_winui->keyUpHandler = MUXI::KeyEventHandler(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::KeyRoutedEventArgs const& event)
            {
                wxSlider::WinUIInput input;
                if ( !wxWinUISliderImpl::GetKeyInput(event.Key(), &input) )
                    return;

                if ( wxSlider * const owner =
                        callbackState->GetOwner(generation) )
                {
                    owner->EndInput(false);
                }
            });
        m_winui->slider.AddHandler(
            MUX::UIElement::KeyUpEvent(),
            winrt::box_value(m_winui->keyUpHandler),
            true);
        m_winui->keyUpAttached = true;

        m_winui->sizeChangedToken = m_winui->root.SizeChanged(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::SizeChangedEventArgs const&)
            {
                if ( wxSlider * const owner =
                        callbackState->GetOwner(generation) )
                {
                    owner->m_winui->visualStateDirty = true;
                    owner->m_winui->visualLayoutReady = false;
                    owner->UpdateVisualState();
                }
            });
        m_winui->loadedToken = m_winui->slider.Loaded(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                if ( wxSlider * const owner =
                        callbackState->GetOwner(generation) )
                {
                    owner->m_winui->visualStateDirty = true;
                    owner->m_winui->visualLayoutReady = false;
                    owner->UpdateVisualState();
                }
            });
        // Themes and accessibility settings may replace Slider's private
        // template without another Loaded notification. Identity-based thumb
        // application makes this callback a cheap no-op in the steady state.
        m_winui->layoutUpdatedToken = m_winui->slider.LayoutUpdated(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                if ( wxSlider * const owner =
                        callbackState->GetOwner(generation) )
                {
                    owner->UpdateVisualState();
                }
            });

        m_winui->actualThemeChangedToken =
            m_winui->slider.ActualThemeChanged(
                [callbackState, generation](
                    MUX::FrameworkElement const&,
                    winrt::Windows::Foundation::IInspectable const&)
                {
                    if ( wxSlider * const owner =
                            callbackState->GetOwner(generation) )
                    {
                        if ( owner->m_winui )
                            owner->m_winui->visualCacheValid = false;
                        owner->UpdateVisualState();
                    }
                });

        m_winui->isEnabledCallbackToken =
            m_winui->slider.RegisterPropertyChangedCallback(
                MUXC::Control::IsEnabledProperty(),
                [callbackState, generation](
                    MUX::DependencyObject const&,
                    MUX::DependencyProperty const&)
                {
                    if ( wxSlider * const owner =
                            callbackState->GetOwner(generation) )
                    {
                        if ( owner->m_winui )
                            owner->m_winui->visualCacheValid = false;
                        owner->UpdateVisualState();
                    }
                });

        m_winui->slider.SmallChange(m_lineSize);
        m_winui->slider.LargeChange(m_pageSize);
        ApplyRangeToPeer();
        UpdateVisualState();

        const bool contentSet =
            createImpl->host.SetContent(createImpl->root, createPeer);

        // RegisterSlot() attaches the XAML subtree and can synchronously run
        // Loaded/application code. The callback is allowed to destroy this
        // wxSlider (or replace its implementation), so resume exclusively
        // through the invalidatable state captured before SetContent().
        wxSlider * const liveOwner =
            createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             !wxWinUISameSliderObject(
                 liveOwner->m_winui->slider, createPeer) )
        {
            return false;
        }

        if ( !contentSet )
        {
            liveOwner->m_winui->Close();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider creation", e);
        wxSlider * const liveOwner =
            createState->GetOwner(createGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == createImpl &&
             liveOwner->m_winui->callbackState == createState )
        {
            liveOwner->m_winui->Close();
        }
        return false;
    }

    wxSlider *liveOwner =
        createState->GetOwner(createGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != createState ||
         !wxWinUISameSliderObject(
             liveOwner->m_winui->slider, createPeer) )
    {
        return false;
    }

    // SetInitialSize() can synchronously dispatch wx size/layout handlers.
    // It is deliberately the final owner call; determine the result from the
    // callback state only after it returns.
    liveOwner->SetInitialSize(size);
    liveOwner = createState->GetOwner(createGeneration);
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == createImpl &&
           liveOwner->m_winui->callbackState == createState &&
           wxWinUISameSliderObject(
               liveOwner->m_winui->slider, createPeer);
}

int wxSlider::GetValue() const
{
    return m_value;
}

void wxSlider::SetValue(int value)
{
    CancelPendingInput();
    m_value = ClampValue(value);
    ApplyValueToPeer();
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    wxCHECK_RET( minValue < maxValue,
                 wxT("Slider minimum must be strictly less than the maximum.") );

    CancelPendingInput();
    m_rangeMin = minValue;
    m_rangeMax = maxValue;
    m_value = ClampValue(m_value);
    m_selStart = ClampValue(m_selStart);
    m_selEnd = ClampValue(m_selEnd);
    if ( m_selStart > m_selEnd )
        std::swap(m_selStart, m_selEnd);
    m_manualTicks.erase(
        std::remove_if(
            m_manualTicks.begin(), m_manualTicks.end(),
            [this](int tick)
            {
                return tick < m_rangeMin || tick > m_rangeMax;
            }),
        m_manualTicks.end());

    ApplyRangeToPeer();
}

void wxSlider::ClearTicks()
{
    m_manualTicks.clear();
    m_ticksCleared = true;
    UpdateVisualState();
}

void wxSlider::SetTick(int tickPos)
{
    if ( tickPos < m_rangeMin || tickPos > m_rangeMax )
        return;

    const auto insertion =
        std::lower_bound(m_manualTicks.begin(), m_manualTicks.end(), tickPos);
    if ( insertion == m_manualTicks.end() || *insertion != tickPos )
        m_manualTicks.insert(insertion, tickPos);

    UpdateVisualState();
}

void wxSlider::ClearSel()
{
    m_selStart = m_rangeMin;
    m_selEnd = m_rangeMin;
    UpdateVisualState();
}

void wxSlider::SetSelection(int startPos, int endPos)
{
    m_selStart = ClampValue(startPos);
    m_selEnd = ClampValue(endPos);
    if ( m_selStart > m_selEnd )
        std::swap(m_selStart, m_selEnd);
    UpdateVisualState();
}

void wxSlider::SetLineSize(int lineSize)
{
    m_lineSize = wxMax(1, lineSize);
    if ( m_winui && m_winui->slider )
    {
        try
        {
            m_winui->slider.SmallChange(m_lineSize);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI Slider SmallChange", e);
        }
    }
}

void wxSlider::SetPageSize(int pageSize)
{
    m_pageSize = wxMax(1, pageSize);
    if ( m_winui && m_winui->slider )
    {
        try
        {
            m_winui->slider.LargeChange(m_pageSize);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI Slider LargeChange", e);
        }
    }
}

void wxSlider::SetThumbLength(int lenPixels)
{
    const double scale = GetEffectiveRasterScale();
    m_thumbLengthDIP = static_cast<double>(wxMax(0, lenPixels)) / scale;
    if ( m_winui )
    {
        m_winui->appliedThumb = nullptr;
        m_winui->appliedThumbLength = -1.0;
        m_winui->visualStateDirty = true;
        m_winui->visualLayoutReady = false;
    }
    ApplyThumbLengthToPeer();
    InvalidateBestSize();
    UpdateVisualState();
}

int wxSlider::GetThumbLength() const
{
    const double scale = GetEffectiveRasterScale();
    double dips = m_thumbLengthDIP;

    try
    {
        if ( m_winui && m_winui->slider )
        {
            const MUXCP::Thumb thumb =
                wxWinUIFindSliderThumb(m_winui->slider);
            if ( thumb )
            {
                const bool vertical =
                    m_winui->slider.Orientation() ==
                    MUXC::Orientation::Vertical;
                const double actual = vertical
                    ? thumb.ActualHeight()
                    : thumb.ActualWidth();
                if ( actual > 0.0 && std::isfinite(actual) &&
                     (m_thumbLengthDIP < 0.0 ||
                      (wxWinUISameSliderObject(
                           thumb, m_winui->appliedThumb) &&
                       std::abs(actual - m_thumbLengthDIP) < 0.25)) )
                {
                    dips = actual;
                }
            }
        }

        if ( dips < 0.0 || !std::isfinite(dips) )
            return 0;
        const double pixels = dips * scale;
        if ( pixels <= 0.0 || !std::isfinite(pixels) )
            return 0;
        if ( pixels >= std::numeric_limits<int>::max() )
            return std::numeric_limits<int>::max();
        return static_cast<int>(std::lround(pixels));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider Thumb query", e);
        return 0;
    }
}

void wxSlider::Command(wxCommandEvent& event)
{
    SetValue(event.GetInt());
    ProcessCommand(event);
}

void wxSlider::DoSetTickFreq(int freq)
{
    m_tickFreq = wxMax(0, freq);
    m_ticksCleared = false;
    if ( m_winui )
    {
        m_winui->visualStateDirty = true;
        m_winui->visualLayoutReady = false;
    }
    if ( m_winui && m_winui->slider && m_tickFreq > 0 )
    {
        try
        {
            m_winui->slider.TickFrequency(m_tickFreq);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI Slider TickFrequency", e);
        }
    }
    UpdateVisualState();
}

wxSize wxSlider::DoGetBestSize() const
{
    int longSide = HasFlag(wxSL_VERTICAL) ? 180 : 220;
    int shortSide = 44;

    if ( HasFlag(wxSL_TICKS) )
        shortSide += HasFlag(wxSL_BOTH) ? 14 : 7;
    if ( HasFlag(wxSL_LABELS) )
        shortSide += 22;
    if ( m_thumbLengthDIP > 0.0 )
    {
        // longSide is a DIP value and FromDIP() below is the one and only DPI
        // conversion performed by best-size calculation.
        longSide = wxMax(
            longSide,
            static_cast<int>(std::ceil(m_thumbLengthDIP)) + 40);
    }

    if ( HasFlag(wxSL_VERTICAL) )
    {
        return wxWindow::FromDIP(
            wxSize(shortSide, longSide), const_cast<wxSlider *>(this));
    }

    return wxWindow::FromDIP(
        wxSize(longSide, shortSide), const_cast<wxSlider *>(this));
}

double wxSlider::GetEffectiveRasterScale() const
{
    double fallbackScale = 1.0;
    if ( GetHandle() )
    {
        fallbackScale = GetDPIScaleFactor();
        if ( fallbackScale <= 0.0 || !std::isfinite(fallbackScale) )
            fallbackScale = 1.0;
    }

    return wxWinUIGetSliderRasterScale(
        m_winui ? m_winui->slider : MUXC::Slider{nullptr},
        fallbackScale);
}

void wxSlider::OnDPIChanged(wxDPIChangedEvent& event)
{
    event.Skip();
    InvalidateBestSize();
    if ( !m_winui )
        return;

    // The shared island updates RasterizationScale itself. Refresh only the
    // pixel-snapped overlay geometry: the custom thumb remains unchanged in
    // DIPs and XAML performs the physical rescaling.
    m_winui->visualCacheValid = false;
    m_winui->visualStateDirty = true;
    m_winui->visualLayoutReady = false;
    UpdateVisualState();
}

bool wxSlider::MSWOnEffectiveLayoutDirectionChanged()
{
    if ( !m_winui || !m_winui->callbackState || !m_winui->root ||
         !m_winui->slider )
    {
        return true;
    }

    wxWinUISliderImpl * const implementation = m_winui.get();
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::Grid root = m_winui->root;
    const MUXC::Slider slider = m_winui->slider;
    const auto getCurrentOwner = [&]() -> wxSlider *
    {
        wxSlider * const owner = state->GetOwner(generation);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == state &&
                       wxWinUISameSliderObject(
                           owner->m_winui->root, root) &&
                       wxWinUISameSliderObject(
                           owner->m_winui->slider, slider)
                   ? owner
                   : nullptr;
    };

    // SetLayoutDirection() commits WS_EX_LAYOUTRTL before invoking this hook.
    // Reproject the Slider and its overlay now that GetLayoutDirection()
    // observes the committed value: the earlier request can be asynchronous.
    m_winui->visualCacheValid = false;
    m_winui->visualStateDirty = true;
    m_winui->visualLayoutReady = false;
    UpdateVisualState();

    wxSlider *owner = getCurrentOwner();
    if ( !owner )
        return false;

    owner->m_winui->host.ForceRender();
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    owner->UpdateVisualState();
    return getCurrentOwner() != nullptr;
}

int wxSlider::ClampValue(int value) const
{
    return wxClip(value, m_rangeMin, m_rangeMax);
}

void wxSlider::CancelPendingInput()
{
    if ( !m_winui )
        return;

    // A queued unclassified ValueChanged belongs to the model state preceding
    // this programmatic transaction. Invalidate both its data and every
    // CallAfter closure which could otherwise publish it later.
    m_winui->pendingChanges.clear();
    m_winui->pendingFlushScheduled = false;
    ++m_winui->deferredGeneration;
    m_winui->inputSourceKnown = false;
    m_winui->inputActive = false;
    m_winui->inputChanged = false;
    m_winui->pointerInteraction =
        wxWinUISliderImpl::PointerInteraction::None;
}

void wxSlider::ApplyRangeToPeer()
{
    if ( !m_winui || !m_winui->slider || !m_winui->callbackState )
        return;

    try
    {
        wxWinUISliderPeerMutationGuard guard(m_winui->callbackState);

        // RangeBase rejects a transient Minimum > Maximum. Choose the update
        // order from the old peer interval so disjoint range changes remain
        // transactional from XAML's point of view.
        if ( m_rangeMin > m_winui->slider.Maximum() )
        {
            m_winui->slider.Maximum(m_rangeMax);
            m_winui->slider.Minimum(m_rangeMin);
        }
        else
        {
            m_winui->slider.Minimum(m_rangeMin);
            m_winui->slider.Maximum(m_rangeMax);
        }
        m_winui->slider.Value(m_value);
        if ( m_tickFreq > 0 )
            m_winui->slider.TickFrequency(m_tickFreq);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider range update", e);
        return;
    }

    UpdateVisualState();
    m_winui->host.ForceRender();
}

void wxSlider::ApplyValueToPeer()
{
    if ( !m_winui || !m_winui->slider || !m_winui->callbackState )
        return;

    try
    {
        wxWinUISliderPeerMutationGuard guard(m_winui->callbackState);
        m_winui->slider.Value(m_value);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider value update", e);
        return;
    }

    UpdateVisualState();
    m_winui->host.ForceRender();
}

bool wxSlider::ApplyThumbLengthToPeer()
{
    if ( m_thumbLengthDIP < 0.0 )
        return true;
    if ( !m_winui || !m_winui->slider )
        return false;

    try
    {
        m_winui->slider.ApplyTemplate();
        const MUXCP::Thumb thumb =
            wxWinUIFindSliderThumb(m_winui->slider);
        if ( !thumb )
            return false;

        // XAML sizes are DIPs. Keeping this value independent of the current
        // RasterizationScale lets XAML rasterize it once on the destination
        // monitor and avoids the old physical-pixel divide/re-divide cycle.
        const double length = m_thumbLengthDIP;
        const bool vertical =
            m_winui->slider.Orientation() ==
            MUXC::Orientation::Vertical;
        const auto isRealizedAtRequestedLength =
            [vertical, length](const MUXCP::Thumb& candidate)
            {
                const double actual = vertical
                    ? candidate.ActualHeight()
                    : candidate.ActualWidth();
                return actual >= 0.0 && std::isfinite(actual) &&
                       std::abs(actual - length) < 0.25;
            };
        if ( wxWinUISameSliderObject(m_winui->appliedThumb, thumb) &&
             std::abs(m_winui->appliedThumbLength - length) < 0.01 )
        {
            return isRealizedAtRequestedLength(thumb);
        }

        if ( vertical )
        {
            thumb.MinHeight(length);
            thumb.MaxHeight(length);
            thumb.Height(length);
        }
        else
        {
            thumb.MinWidth(length);
            thumb.MaxWidth(length);
            thumb.Width(length);
        }

        m_winui->appliedThumb = thumb;
        m_winui->appliedThumbLength = length;
        // Setting Width/Height is synchronous, arranging that new extent is
        // not. Keep the realized latch closed until the following layout pass
        // reports the requested ActualWidth/ActualHeight.
        return isRealizedAtRequestedLength(thumb);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider Thumb update", e);
    }
    return false;
}

void wxSlider::UpdateVisualState()
{
    if ( !m_winui || !m_winui->root || !m_winui->slider ||
         !m_winui->decorationCanvas || m_winui->updatingVisualState )
    {
        return;
    }

    m_winui->updatingVisualState = true;
    try
    {
        const bool thumbReady = ApplyThumbLengthToPeer();

        const bool vertical = HasFlag(wxSL_VERTICAL);
        const bool rtl = GetLayoutDirection() == wxLayout_RightToLeft;
        const MUX::FlowDirection flow =
            rtl ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight;

        // Keep the non-geometric part of the Win32 decoration contract
        // synchronous. In particular, Create()/SetSelection() may run before
        // the first non-empty XAML arrange; visibility, text, theme and flow
        // must not remain stuck at their construction defaults while the
        // geometry waits for Loaded/LayoutUpdated.
        if ( m_winui->root.FlowDirection() != flow )
            m_winui->root.FlowDirection(flow);
        if ( m_winui->slider.FlowDirection() != flow )
            m_winui->slider.FlowDirection(flow);
        // Canvas positions remain physical-LTR when the Canvas explicitly
        // overrides an RTL parent. WinUI Slider also leaves its value axis
        // unchanged by FlowDirection, so fold horizontal RTL into the native
        // direction property below and let the arranged Thumb be the final
        // geometry oracle.
        if ( m_winui->decorationCanvas.FlowDirection() !=
                 MUX::FlowDirection::LeftToRight )
        {
            m_winui->decorationCanvas.FlowDirection(
                MUX::FlowDirection::LeftToRight);
        }
        if ( m_winui->tickCanvas.FlowDirection() !=
                 MUX::FlowDirection::LeftToRight )
        {
            m_winui->tickCanvas.FlowDirection(
                MUX::FlowDirection::LeftToRight);
        }
        const bool reversed =
            HasFlag(wxSL_INVERSE) != (rtl && !vertical);
        if ( m_winui->slider.IsDirectionReversed() != reversed )
            m_winui->slider.IsDirectionReversed(reversed);

        const bool showMinMax = HasFlag(wxSL_MIN_MAX_LABELS);
        const bool showValue = HasFlag(wxSL_VALUE_LABEL);
        const bool enabled = IsEnabled();
        const bool highContrast = wxWinUIIsHighContrastActive();
        double fontSize = m_winui->slider.FontSize();
        if ( fontSize <= 0.0 || !std::isfinite(fontSize) )
            fontSize = 14.0;

        MUXM::Brush foreground = m_winui->slider.Foreground();
        if ( !foreground )
        {
            if ( !m_winui->fallbackForeground )
            {
                m_winui->fallbackForeground = MUXM::SolidColorBrush(
                    winrt::Windows::UI::Color{0xff, 0x60, 0x60, 0x60});
            }
            foreground = m_winui->fallbackForeground;
        }

        const auto updateLabel =
            [fontSize, foreground, flow](const MUXC::TextBlock& label,
                                         int value,
                                         bool show)
            {
                bool measureNeeded = false;
                const winrt::hstring text = wxWinUIToHString(
                    wxString::Format(wxT("%d"), value));
                if ( label.Text() != text )
                {
                    label.Text(text);
                    measureNeeded = true;
                }
                if ( std::abs(label.FontSize() - fontSize) >= 0.01 )
                {
                    label.FontSize(fontSize);
                    measureNeeded = true;
                }
                if ( !wxWinUISameSliderObject(
                         label.Foreground(), foreground) )
                {
                    label.Foreground(foreground);
                }
                if ( label.FlowDirection() != flow )
                {
                    label.FlowDirection(flow);
                    measureNeeded = true;
                }
                const MUX::Visibility visibility = show
                    ? MUX::Visibility::Visible
                    : MUX::Visibility::Collapsed;
                if ( label.Visibility() != visibility )
                {
                    label.Visibility(visibility);
                    measureNeeded = show;
                }
                if ( show && measureNeeded )
                {
                    label.Measure(
                        winrt::Windows::Foundation::Size{
                            1000000.0f, 1000000.0f});
                }
            };
        updateLabel(m_winui->minimumLabel, m_rangeMin, showMinMax);
        updateLabel(m_winui->maximumLabel, m_rangeMax, showMinMax);
        updateLabel(m_winui->valueLabel, m_value, showValue);

        MUXC::Border activeSelection = enabled
            ? (highContrast ? m_winui->selectionHighContrast
                            : m_winui->selectionNormal)
            : (highContrast ? m_winui->selectionHighContrastDisabled
                            : m_winui->selectionDisabled);
        const MUXC::Border selectionVisuals[] =
        {
            m_winui->selectionNormal,
            m_winui->selectionDisabled,
            m_winui->selectionHighContrast,
            m_winui->selectionHighContrastDisabled
        };
        const bool showSelection =
            HasFlag(wxSL_SELRANGE) && m_selStart != m_selEnd;
        for ( const MUXC::Border& selection : selectionVisuals )
        {
            if ( std::abs(selection.Opacity() - 1.0) >= 0.001 )
                selection.Opacity(1.0);
            const MUX::Visibility visibility =
                showSelection &&
                        wxWinUISameSliderObject(selection, activeSelection)
                    ? MUX::Visibility::Visible
                    : MUX::Visibility::Collapsed;
            if ( selection.Visibility() != visibility )
                selection.Visibility(visibility);
        }

        const double width = wxMax(0.0, m_winui->root.ActualWidth());
        const double height = wxMax(0.0, m_winui->root.ActualHeight());
        if ( !m_winui->root.IsLoaded() ||
             !m_winui->slider.IsLoaded() ||
             width <= 0.0 || height <= 0.0 ||
             !std::isfinite(width) || !std::isfinite(height) )
        {
            // Do not turn the pre-Loaded 0x0 pass into a stable oracle. The
            // dirty latch is consumed by the first real layout callback.
            m_winui->visualCacheValid = false;
            m_winui->visualStateDirty = true;
            m_winui->visualLayoutReady = false;
            m_winui->updatingVisualState = false;
            return;
        }
        const double scale = GetEffectiveRasterScale();
        const MUX::ElementTheme actualTheme =
            m_winui->slider.ActualTheme();

        // Slider.Margin(), a template change and a theme transition all move
        // the realized native track without changing the root size. Resolve
        // both named template primitives and project their exact arrangement
        // into the overlay root; no root-size/label heuristic is admissible.
        const MUXCP::Thumb realizedThumb =
            wxWinUIFindSliderThumb(m_winui->slider);
        const MUX::FrameworkElement realizedTrack =
            wxWinUIFindSliderTrack(m_winui->slider, realizedThumb);
        WinUIVisualRect realizedThumbRect;
        WinUIVisualRect realizedTrackRect;
        if ( !realizedThumb || !realizedTrack ||
             !wxWinUISliderImpl::GetRealizedRect(
                 realizedThumb,
                 m_winui->decorationCanvas,
                 &realizedThumbRect) ||
             !wxWinUISliderImpl::GetRealizedRect(
                 realizedTrack,
                 m_winui->decorationCanvas,
                 &realizedTrackRect) )
        {
            m_winui->visualCacheValid = false;
            m_winui->visualStateDirty = true;
            m_winui->visualLayoutReady = false;
            m_winui->updatingVisualState = false;
            return;
        }
        const double trackPrimaryStart = vertical
            ? realizedTrackRect.y
            : realizedTrackRect.x;
        const double trackPrimaryExtent = vertical
            ? realizedTrackRect.height
            : realizedTrackRect.width;
        const double thumbPrimaryExtent = vertical
            ? realizedThumbRect.height
            : realizedThumbRect.width;
        const double axisStart = trackPrimaryStart + thumbPrimaryExtent / 2.0;
        const double axisEnd =
            trackPrimaryStart + trackPrimaryExtent - thumbPrimaryExtent / 2.0;
        const double realizedThumbAxis = vertical
            ? realizedThumbRect.y + realizedThumbRect.height / 2.0
            : realizedThumbRect.x + realizedThumbRect.width / 2.0;
        const double realizedAxisCross = vertical
            ? realizedTrackRect.x + realizedTrackRect.width / 2.0
            : realizedTrackRect.y + realizedTrackRect.height / 2.0;
        if ( !std::isfinite(axisStart) || !std::isfinite(axisEnd) ||
             !std::isfinite(realizedThumbAxis) || axisEnd <= axisStart )
        {
            m_winui->visualCacheValid = false;
            m_winui->visualStateDirty = true;
            m_winui->visualLayoutReady = false;
            m_winui->updatingVisualState = false;
            return;
        }

        const double rangeWidth =
            static_cast<double>(
                static_cast<std::int64_t>(m_rangeMax) -
                static_cast<std::int64_t>(m_rangeMin));
        const double currentRatio = wxClip(
            (static_cast<double>(m_value) -
             static_cast<double>(m_rangeMin)) / rangeWidth,
            0.0,
            1.0);
        const double normalThumbAxis =
            axisStart + currentRatio * (axisEnd - axisStart);
        const double reversedThumbAxis =
            axisEnd - currentRatio * (axisEnd - axisStart);
        // IsDirectionReversed describes the native Slider value direction in
        // root coordinates. The explicitly LTR decoration canvas introduces
        // one relative horizontal reflection under an RTL root, so convert
        // the property into this local coordinate space for the mid-range tie
        // where the arranged Thumb cannot distinguish the two candidates.
        bool axisReversed =
            vertical != m_winui->slider.IsDirectionReversed();
        if ( !vertical && rtl )
            axisReversed = !axisReversed;
        if ( std::abs(normalThumbAxis - reversedThumbAxis) >= 0.25 )
        {
            axisReversed =
                std::abs(realizedThumbAxis - reversedThumbAxis) <
                std::abs(realizedThumbAxis - normalThumbAxis);
        }
        const double expectedThumbAxis =
            axisReversed ? reversedThumbAxis : normalThumbAxis;
        if ( std::abs(expectedThumbAxis - realizedThumbAxis) >
                 wxMax(1.0, 2.0 / scale) )
        {
            m_winui->visualCacheValid = false;
            m_winui->visualStateDirty = true;
            m_winui->visualLayoutReady = false;
            m_winui->updatingVisualState = false;
            return;
        }
        const std::uintptr_t realizedThumbIdentity =
            wxWinUISliderObjectIdentity(realizedThumb);
        const std::uintptr_t realizedTrackIdentity =
            wxWinUISliderObjectIdentity(realizedTrack);

        // DesiredSize is part of the visual cache key: a same-point-size font
        // family/weight change can alter endpoint width without changing
        // FontSize, and therefore requires a fresh native track reservation.
        const auto minSize = m_winui->minimumLabel.DesiredSize();
        const auto maxSize = m_winui->maximumLabel.DesiredSize();
        const auto valueSize = m_winui->valueLabel.DesiredSize();

        std::size_t tickHash = m_manualTicks.size();
        for ( int tick : m_manualTicks )
        {
            tickHash ^= static_cast<std::size_t>(
                static_cast<unsigned>(tick)) +
                static_cast<std::size_t>(0x9e3779b9U) +
                (tickHash << 6) + (tickHash >> 2);
        }

        const long style = GetWindowStyleFlag();
        const bool cacheMatches =
            !m_winui->visualStateDirty &&
            m_winui->visualCacheValid &&
            std::abs(m_winui->visualWidth - width) < 0.01 &&
            std::abs(m_winui->visualHeight - height) < 0.01 &&
            std::abs(m_winui->visualRasterScale - scale) < 0.001 &&
            std::abs(m_winui->visualFontSize - fontSize) < 0.01 &&
            std::abs(m_winui->visualMinimumLabelWidth -
                     minSize.Width) < 0.01 &&
            std::abs(m_winui->visualMinimumLabelHeight -
                     minSize.Height) < 0.01 &&
            std::abs(m_winui->visualMaximumLabelWidth -
                     maxSize.Width) < 0.01 &&
            std::abs(m_winui->visualMaximumLabelHeight -
                     maxSize.Height) < 0.01 &&
            std::abs(m_winui->visualValueLabelWidth -
                     valueSize.Width) < 0.01 &&
            std::abs(m_winui->visualValueLabelHeight -
                     valueSize.Height) < 0.01 &&
            m_winui->visualValue == m_value &&
            m_winui->visualMinimum == m_rangeMin &&
            m_winui->visualMaximum == m_rangeMax &&
            m_winui->visualSelectionStart == m_selStart &&
            m_winui->visualSelectionEnd == m_selEnd &&
            m_winui->visualTickFrequency == m_tickFreq &&
            std::abs(m_winui->visualThumbLengthDIP -
                     m_thumbLengthDIP) < 0.01 &&
            m_winui->visualStyle == style &&
            m_winui->visualDirection == GetLayoutDirection() &&
            m_winui->visualTicksCleared == m_ticksCleared &&
            m_winui->visualEnabled == enabled &&
            m_winui->visualHighContrast == highContrast &&
            m_winui->visualActualTheme == actualTheme &&
            m_winui->visualTickHash == tickHash &&
            wxWinUISameSliderObject(
                m_winui->visualBrush, foreground) &&
            std::abs(m_winui->visualAxisCross - realizedAxisCross) < 0.25 &&
            std::abs(m_winui->visualAxisStart - axisStart) < 0.25 &&
            std::abs(m_winui->visualAxisEnd - axisEnd) < 0.25 &&
            std::abs(m_winui->visualThumbAxis - realizedThumbAxis) < 0.25 &&
            m_winui->visualThumbIdentity == realizedThumbIdentity &&
            m_winui->visualTrackIdentity == realizedTrackIdentity &&
            m_winui->visualAxisReversed == axisReversed;
        if ( cacheMatches )
        {
            m_winui->visualLayoutReady = thumbReady;
            m_winui->updatingVisualState = false;
            return;
        }

        const bool showTicks = HasFlag(wxSL_TICKS);
        const bool preferredTicksFirst =
            vertical ? HasFlag(wxSL_LEFT) : HasFlag(wxSL_TOP);
        const bool ticksFirst =
            showTicks &&
            (HasFlag(wxSL_BOTH) || preferredTicksFirst);
        const bool ticksSecond =
            showTicks && (HasFlag(wxSL_BOTH) || !preferredTicksFirst);
        // wxSL_LEFT/TOP/RIGHT/BOTTOM name the tick side. Composite labels are
        // on the opposite side; with wxSL_BOTH use the normal RIGHT/BOTTOM
        // preference and put the value label on the first (left/top) side.
        const bool labelsFirst = !preferredTicksFirst;
        const double tickBand = showTicks ? 8.0 : 0.0;

        const MUX::Thickness oldMargin = m_winui->slider.Margin();
        double primaryStartReserve = 0.0;
        double primaryEndReserve = 0.0;
        if ( showMinMax )
        {
            // wxMSW puts the endpoint labels beside the slider, at the very
            // start and end of the control, and shortens the slider itself by
            // their extent plus a small gap (VGAP there). Reserve exactly that
            // so the labels are never drawn over the track. The value is a
            // function of the observed label sizes only, so it stays
            // idempotent across LayoutUpdated and RTL coordinate passes.
            const double startLabelExtent = vertical
                ? (axisReversed ? maxSize.Height : minSize.Height)
                : (axisReversed ? maxSize.Width : minSize.Width);
            const double endLabelExtent = vertical
                ? (axisReversed ? minSize.Height : maxSize.Height)
                : (axisReversed ? minSize.Width : maxSize.Width);
            primaryStartReserve = startLabelExtent + wxWinUISliderLabelGap;
            primaryEndReserve = endLabelExtent + wxWinUISliderLabelGap;
        }

        // Side bands are overlay coordinates, not native Slider margins.
        // Applying them to the peer would keep a fixed-DIP template offset as
        // the physical client shrinks at high raster scales, eventually
        // arranging the Track at (or beyond) the root edge. Keep the peer's
        // intrinsic cross-axis layout centred and anchor every band to its
        // observed Track instead.
        const MUX::Thickness margin =
            vertical
                ? MUX::Thickness{0.0, primaryStartReserve,
                                 0.0, primaryEndReserve}
                : rtl
                    ? MUX::Thickness{primaryEndReserve, 0.0,
                                     primaryStartReserve, 0.0}
                    : MUX::Thickness{primaryStartReserve, 0.0,
                                     primaryEndReserve, 0.0};
        if ( std::abs(oldMargin.Left - margin.Left) > 0.01 ||
             std::abs(oldMargin.Top - margin.Top) > 0.01 ||
             std::abs(oldMargin.Right - margin.Right) > 0.01 ||
             std::abs(oldMargin.Bottom - margin.Bottom) > 0.01 )
        {
            m_winui->slider.Margin(margin);
            // Margin invalidates the native template arrangement. Never draw
            // or cache against the retired Track/Thumb coordinates: the next
            // LayoutUpdated pass re-observes their exact transformed geometry.
            m_winui->visualCacheValid = false;
            m_winui->visualStateDirty = true;
            m_winui->visualLayoutReady = false;
            m_winui->updatingVisualState = false;
            return;
        }

        const double axisCross = realizedAxisCross;
        const auto axisPosition =
            [this, axisStart, axisEnd, rangeWidth, axisReversed](int value)
            {
                double ratio =
                    (static_cast<double>(value) -
                     static_cast<double>(m_rangeMin)) / rangeWidth;
                ratio = wxClip(ratio, 0.0, 1.0);
                if ( axisReversed )
                    ratio = 1.0 - ratio;
                return axisStart + ratio * (axisEnd - axisStart);
            };

        if ( showSelection )
        {
            const double first = axisPosition(m_selStart);
            const double second = axisPosition(m_selEnd);
            const double low = wxMin(first, second);
            const double extent = wxMax(1.0 / scale, std::abs(second - first));
            for ( const MUXC::Border& selection : selectionVisuals )
            {
                if ( vertical )
                {
                    selection.Width(4.0);
                    selection.Height(extent);
                    MUXC::Canvas::SetLeft(selection, axisCross - 2.0);
                    MUXC::Canvas::SetTop(selection, low);
                }
                else
                {
                    selection.Width(extent);
                    selection.Height(4.0);
                    MUXC::Canvas::SetLeft(selection, low);
                    MUXC::Canvas::SetTop(selection, axisCross - 2.0);
                }
            }
        }

        std::vector<int> ticks = m_manualTicks;
        if ( showTicks )
        {
            ticks.push_back(m_rangeMin);
            ticks.push_back(m_rangeMax);

            if ( !m_ticksCleared && m_tickFreq > 0 )
            {
                const std::int64_t fullRange =
                    static_cast<std::int64_t>(m_rangeMax) -
                    static_cast<std::int64_t>(m_rangeMin);
                const std::int64_t count = fullRange / m_tickFreq;
                const double axisSpan = wxMax(1.0, axisEnd - axisStart);
                const std::size_t budget = wxMin<std::size_t>(
                    4096,
                    wxMax<std::size_t>(
                        2, static_cast<std::size_t>(
                               std::ceil(axisSpan * 2.0))));
                if ( count <= static_cast<std::int64_t>(budget) )
                {
                    for ( std::int64_t offset = m_tickFreq;
                          offset < fullRange;
                          offset += m_tickFreq )
                    {
                        ticks.push_back(static_cast<int>(
                            static_cast<std::int64_t>(m_rangeMin) + offset));
                    }
                }
                else
                {
                    for ( std::size_t n = 1; n < budget; ++n )
                    {
                        const std::int64_t approximate =
                            (fullRange * static_cast<std::int64_t>(n)) /
                            static_cast<std::int64_t>(budget);
                        const std::int64_t offset =
                            (approximate / m_tickFreq) * m_tickFreq;
                        if ( offset > 0 && offset < fullRange )
                        {
                            ticks.push_back(static_cast<int>(
                                static_cast<std::int64_t>(m_rangeMin) +
                                offset));
                        }
                    }
                }
            }
        }
        std::sort(ticks.begin(), ticks.end());
        ticks.erase(std::unique(ticks.begin(), ticks.end()), ticks.end());

        m_winui->tickCanvas.Children().Clear();
        const double crisp = 1.0 / scale;
        const auto appendTick =
            [this, foreground](double left, double top,
                               double markWidth, double markHeight)
            {
                MUXC::Border mark;
                mark.Width(markWidth);
                mark.Height(markHeight);
                mark.Background(foreground);
                mark.Opacity(0.72);
                mark.IsHitTestVisible(false);
                wxWinUIMarkSliderDecorationRaw(mark);
                MUXC::Canvas::SetLeft(mark, left);
                MUXC::Canvas::SetTop(mark, top);
                m_winui->tickCanvas.Children().Append(mark);
            };
        if ( showTicks )
        {
            for ( int tick : ticks )
            {
                const double position = axisPosition(tick);
                if ( vertical )
                {
                    if ( ticksFirst )
                    {
                        appendTick(axisCross - tickBand, position - crisp / 2.0,
                                   6.0, crisp);
                    }
                    if ( ticksSecond )
                    {
                        appendTick(axisCross + 2.0, position - crisp / 2.0,
                                   6.0, crisp);
                    }
                }
                else
                {
                    if ( ticksFirst )
                    {
                        appendTick(position - crisp / 2.0, axisCross - tickBand,
                                   crisp, 6.0);
                    }
                    if ( ticksSecond )
                    {
                        appendTick(position - crisp / 2.0, axisCross + 2.0,
                                   crisp, 6.0);
                    }
                }
            }
        }

        // Like wxMSW: each endpoint label goes to the extremity of the control
        // on its own side of the axis -- the track is inset by the reserve
        // computed above -- and is centred on the track band. Centring it on
        // the endpoint instead would draw it over the track, which is exactly
        // what that reserve exists to avoid.
        const auto placeEndpointLabel =
            [vertical, width, height, axisCross](
                const MUXC::TextBlock& label,
                bool atAxisStart,
                bool visible)
            {
                if ( !visible )
                    return;
                const auto desired = label.DesiredSize();
                if ( vertical )
                {
                    const double left = wxClip(
                        axisCross - desired.Width / 2.0,
                        0.0,
                        wxMax(0.0, width - desired.Width));
                    const double top = atAxisStart
                        ? 0.0
                        : wxMax(0.0, height - desired.Height);
                    MUXC::Canvas::SetLeft(label, left);
                    MUXC::Canvas::SetTop(label, top);
                }
                else
                {
                    const double left = atAxisStart
                        ? 0.0
                        : wxMax(0.0, width - desired.Width);
                    const double top = wxClip(
                        axisCross - desired.Height / 2.0,
                        0.0,
                        wxMax(0.0, height - desired.Height));
                    MUXC::Canvas::SetLeft(label, left);
                    MUXC::Canvas::SetTop(label, top);
                }
            };
        placeEndpointLabel(
            m_winui->minimumLabel, !axisReversed, showMinMax);
        placeEndpointLabel(
            m_winui->maximumLabel, axisReversed, showMinMax);

        if ( showValue )
        {
            const auto desired = m_winui->valueLabel.DesiredSize();
            constexpr double valueLabelGap = 2.0;
            if ( vertical )
            {
                // Side-band reserves describe decoration demand; they are not
                // coordinates. Anchor the realized label directly to the
                // observed native Track axis and clamp only to the root slot.
                const double left = wxClip(
                    labelsFirst
                        ? axisCross - valueLabelGap - desired.Width
                        : axisCross + valueLabelGap,
                    0.0,
                    wxMax(0.0, width - desired.Width));
                const double top = wxClip(
                    (axisStart + axisEnd - desired.Height) / 2.0,
                    0.0,
                    wxMax(0.0, height - desired.Height));
                MUXC::Canvas::SetLeft(m_winui->valueLabel, left);
                MUXC::Canvas::SetTop(m_winui->valueLabel, top);
            }
            else
            {
                const double left = wxClip(
                    (axisStart + axisEnd - desired.Width) / 2.0,
                    0.0,
                    wxMax(0.0, width - desired.Width));
                const double top = wxClip(
                    labelsFirst
                        ? axisCross - valueLabelGap - desired.Height
                        : axisCross + valueLabelGap,
                    0.0,
                    wxMax(0.0, height - desired.Height));
                MUXC::Canvas::SetLeft(m_winui->valueLabel, left);
                MUXC::Canvas::SetTop(m_winui->valueLabel, top);
            }
        }

        m_winui->visualCacheValid = true;
        m_winui->visualWidth = width;
        m_winui->visualHeight = height;
        m_winui->visualRasterScale = scale;
        m_winui->visualFontSize = fontSize;
        m_winui->visualMinimumLabelWidth = minSize.Width;
        m_winui->visualMinimumLabelHeight = minSize.Height;
        m_winui->visualMaximumLabelWidth = maxSize.Width;
        m_winui->visualMaximumLabelHeight = maxSize.Height;
        m_winui->visualValueLabelWidth = valueSize.Width;
        m_winui->visualValueLabelHeight = valueSize.Height;
        m_winui->visualValue = m_value;
        m_winui->visualMinimum = m_rangeMin;
        m_winui->visualMaximum = m_rangeMax;
        m_winui->visualSelectionStart = m_selStart;
        m_winui->visualSelectionEnd = m_selEnd;
        m_winui->visualTickFrequency = m_tickFreq;
        m_winui->visualThumbLengthDIP = m_thumbLengthDIP;
        m_winui->visualAxisCross = axisCross;
        m_winui->visualAxisStart = axisStart;
        m_winui->visualAxisEnd = axisEnd;
        m_winui->visualThumbAxis = realizedThumbAxis;
        m_winui->visualThumbIdentity = realizedThumbIdentity;
        m_winui->visualTrackIdentity = realizedTrackIdentity;
        m_winui->visualAxisReversed = axisReversed;
        m_winui->visualStyle = style;
        m_winui->visualDirection = GetLayoutDirection();
        m_winui->visualTicksCleared = m_ticksCleared;
        m_winui->visualEnabled = enabled;
        m_winui->visualHighContrast = highContrast;
        m_winui->visualActualTheme = actualTheme;
        m_winui->visualTickHash = tickHash;
        m_winui->visualBrush = foreground;
        m_winui->visualStateDirty = false;
        m_winui->visualLayoutReady = thumbReady;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider visual update", e);
        m_winui->visualStateDirty = true;
        m_winui->visualLayoutReady = false;
    }
    m_winui->updatingVisualState = false;
}

void wxSlider::BeginInput(WinUIInput input)
{
    if ( !m_winui )
        return;

    const bool immediate =
        input == WinUIInput::Wheel ||
        input == WinUIInput::Automation;

    if ( !immediate && !m_winui->inputActive )
    {
        m_winui->inputActive = true;
        m_winui->inputChanged = false;
    }

    m_winui->input = input;
    m_winui->inputSourceKnown = true;
}

wxSlider::WinUIInput
wxSlider::GetPageInput(int oldValue, int newValue) const
{
    bool decrement = newValue < oldValue;
    if ( HasFlag(wxSL_INVERSE) )
        decrement = !decrement;
    return decrement ? WinUIInput::PageDecrement
                     : WinUIInput::PageIncrement;
}

void wxSlider::BeginPointerInput(bool thumb)
{
    if ( !m_winui )
        return;

    m_winui->pointerInteraction =
        thumb ? wxWinUISliderImpl::PointerInteraction::Thumb
              : wxWinUISliderImpl::PointerInteraction::Track;

    if ( thumb )
    {
        BeginInput(WinUIInput::ThumbTrack);
        FlushPendingInput(WinUIInput::ThumbTrack);
        return;
    }

    // The Slider class handler can run before this handled-routed callback.
    // In that ordering ValueChanged is queued with both sides of the delta;
    // classify it now. Otherwise OnPeerValueChanged resolves the first delta.
    if ( !m_winui->pendingChanges.empty() )
    {
        const wxWinUISliderPendingChange& first =
            m_winui->pendingChanges.front();
        const WinUIInput input =
            GetPageInput(first.oldValue, first.newValue);
        BeginInput(input);
        FlushPendingInput(input);
    }
    else
    {
        m_winui->inputSourceKnown = false;
    }
}

void wxSlider::OnPeerValueChanged(int value)
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    const std::shared_ptr<wxWinUISliderCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    const int newValue = ClampValue(value);

    if ( !IsEnabled() )
    {
        ApplyValueToPeer();
        return;
    }

    if ( newValue == m_value )
        return;

    const int oldValue = m_value;

    if ( m_winui->pointerInteraction ==
             wxWinUISliderImpl::PointerInteraction::Track &&
         !m_winui->inputSourceKnown )
    {
        BeginInput(GetPageInput(oldValue, newValue));
    }

    m_value = newValue;
    UpdateVisualState();

    if ( !m_winui->inputSourceKnown )
    {
        m_winui->pendingChanges.push_back({oldValue, newValue});

        if ( !m_winui->pendingFlushScheduled )
        {
            m_winui->pendingFlushScheduled = true;
            if ( wxTheApp )
            {
                const std::uint64_t deferredGeneration =
                    ++m_winui->deferredGeneration;
                wxTheApp->CallAfter(
                    [callbackState, generation, deferredGeneration]()
                    {
                        wxSlider * const owner =
                            callbackState->GetOwner(generation);
                        if ( owner && owner->m_winui &&
                             owner->m_winui->deferredGeneration ==
                                 deferredGeneration &&
                             owner->m_winui->pendingFlushScheduled )
                        {
                            owner->FlushPendingInput(
                                WinUIInput::Automation);
                        }
                    });
            }
            else
            {
                FlushPendingInput(WinUIInput::Automation);
            }
        }
        return;
    }

    const WinUIInput input = m_winui->input;
    if ( input == WinUIInput::Wheel ||
         input == WinUIInput::Automation )
    {
        m_winui->inputSourceKnown = false;
        wxWinUIEmitSliderScrollAndCommand(
            callbackState,
            generation,
            wxEVT_SCROLL_CHANGED,
            newValue);
        return;
    }

    m_winui->inputChanged = true;
    wxWinUIEmitSliderScrollAndCommand(
        callbackState,
        generation,
        wxWinUISliderImpl::GetScrollEvent(input),
        newValue);
}

void wxSlider::FlushPendingInput(WinUIInput input)
{
    if ( m_winui )
        ++m_winui->deferredGeneration;

    if ( !m_winui || !m_winui->callbackState ||
         m_winui->pendingChanges.empty() )
    {
        if ( m_winui )
            m_winui->pendingFlushScheduled = false;
        return;
    }

    const std::shared_ptr<wxWinUISliderCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    std::vector<wxWinUISliderPendingChange> changes;
    changes.swap(m_winui->pendingChanges);
    m_winui->pendingFlushScheduled = false;

    const bool immediate =
        input == WinUIInput::Wheel ||
        input == WinUIInput::Automation;

    if ( immediate )
    {
        // ValueChanged preceded the routed wheel event (or this is the
        // deferred UIA fallback); don't let its source leak into the next
        // unrelated peer change.
        m_winui->inputSourceKnown = false;
    }
    else
    {
        if ( !m_winui->inputActive )
        {
            m_winui->inputActive = true;
            m_winui->inputChanged = false;
        }
        m_winui->inputChanged = true;
    }

    for ( const wxWinUISliderPendingChange& change : changes )
    {
        if ( immediate )
        {
            if ( !wxWinUIEmitSliderScrollAndCommand(
                    callbackState,
                    generation,
                    wxEVT_SCROLL_CHANGED,
                    change.newValue) )
            {
                return;
            }
        }
        else
        {
            if ( !wxWinUIEmitSliderScrollAndCommand(
                    callbackState,
                    generation,
                    wxWinUISliderImpl::GetScrollEvent(input),
                    change.newValue) )
            {
                return;
            }
        }
    }
}

void wxSlider::EndInput(bool pointerInput)
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    const std::shared_ptr<wxWinUISliderCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    const bool changed = m_winui->inputActive && m_winui->inputChanged;
    const int value = m_value;

    m_winui->inputActive = false;
    m_winui->inputChanged = false;
    m_winui->inputSourceKnown = false;

    if ( !changed )
        return;

    if ( pointerInput )
    {
        // wxMSW emits the release notification and its matching command
        // event at the final position, then ENDTRACK/CHANGED exactly once.
        if ( !wxWinUIEmitSliderScrollAndCommand(
                callbackState,
                generation,
                wxEVT_SCROLL_THUMBRELEASE,
                value) )
        {
            return;
        }
    }

    wxWinUIEmitSliderScroll(
        callbackState, generation, wxEVT_SCROLL_CHANGED, value);
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUISliderTestAccess::ApplyInput(
    wxSlider& control,
    Input input,
    int value,
    bool finishInteraction)
{
    if ( !control.m_winui || !control.m_winui->slider || !control.m_winui->callbackState )
        return false;

    const std::shared_ptr<wxWinUISliderCallbackState> callbackState =
        control.m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    const MUXC::Slider peer = control.m_winui->slider;

    const bool pointerInput =
        input == Input::ThumbTrack ||
        input == Input::ThumbRelease;
    control.BeginInput(pointerInput ? Input::ThumbTrack : input);

    try
    {
        peer.Value(control.ClampValue(value));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider test input", e);
        return false;
    }

    wxSlider * const owner = callbackState->GetOwner(generation);
    if ( !owner )
        return true;

    if ( input == Input::ThumbRelease ||
         (finishInteraction &&
          input != Input::Wheel &&
          input != Input::Automation) )
    {
        owner->EndInput(pointerInput);
    }

    return true;
}

bool wxWinUISliderTestAccess::ApplyPointerInput(
    wxSlider& control,
    bool thumb,
    int value,
    bool valueBeforePress)
{
    if ( !control.m_winui || !control.m_winui->slider ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    const std::shared_ptr<wxWinUISliderCallbackState> state =
        control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::Slider peer = control.m_winui->slider;

    try
    {
        if ( valueBeforePress )
            peer.Value(control.ClampValue(value));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI Slider pointer test pre-change", e);
        return false;
    }

    wxSlider *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui )
        return true;

    // Exercise the production OriginalSource classifier with the exact Thumb
    // realized by this peer. A detached synthetic Thumb necessarily classifies
    // as track input and cannot prove the release path used by real pointer
    // routing.
    MUXCP::Thumb peerThumb{ nullptr };
    if ( thumb )
    {
        try
        {
            peer.ApplyTemplate();
            peerThumb = wxWinUIFindSliderThumb(peer);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI Slider pointer test thumb resolve", e);
            return false;
        }
        if ( !peerThumb )
            return false;
    }
    const winrt::Windows::Foundation::IInspectable source =
        thumb
            ? peerThumb.as<
                  winrt::Windows::Foundation::IInspectable>()
            : peer.as<
                  winrt::Windows::Foundation::IInspectable>();
    owner->BeginPointerInput(
        wxWinUIIsSliderThumbSource(source, peer));

    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui )
        return true;

    if ( !valueBeforePress )
    {
        try
        {
            peer.Value(owner->ClampValue(value));
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI Slider pointer test post-change", e);
            return false;
        }
    }

    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui )
        return true;

    const bool wasThumb =
        owner->m_winui->pointerInteraction ==
        wxWinUISliderImpl::PointerInteraction::Thumb;
    owner->m_winui->pointerInteraction =
        wxWinUISliderImpl::PointerInteraction::None;
    owner->EndInput(wasThumb);
    return true;
}

bool wxWinUISliderTestAccess::SetUnclassifiedPeerValue(
    wxSlider& control,
    int value)
{
    if ( !control.m_winui || !control.m_winui->slider ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    const std::shared_ptr<wxWinUISliderCallbackState> state =
        control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    try
    {
        // Deliberately omit BeginInput(): this is the exact ValueChanged-first
        // ordering whose classification is deferred through CallAfter.
        control.m_winui->slider.Value(control.ClampValue(value));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI Slider unclassified test input", e);
        return false;
    }

    return state->GetOwner(generation) != nullptr;
}

bool wxWinUISliderTestAccess::GetPeerState(
    const wxSlider& control,
    double *minimum,
    double *maximum,
    double *value,
    double *smallChange,
    double *largeChange,
    bool *vertical,
    bool *reversed)
{
    if ( !control.m_winui || !control.m_winui->slider )
        return false;

    try
    {
        if ( minimum )
            *minimum = control.m_winui->slider.Minimum();
        if ( maximum )
            *maximum = control.m_winui->slider.Maximum();
        if ( value )
            *value = control.m_winui->slider.Value();
        if ( smallChange )
            *smallChange = control.m_winui->slider.SmallChange();
        if ( largeChange )
            *largeChange = control.m_winui->slider.LargeChange();
        if ( vertical )
        {
            *vertical =
                control.m_winui->slider.Orientation() ==
                MUXC::Orientation::Vertical;
        }
        if ( reversed )
            *reversed = control.m_winui->slider.IsDirectionReversed();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider test peer query", e);
        return false;
    }

    return true;
}

bool wxWinUISliderTestAccess::GetDecorations(
    const wxSlider& control,
    int *thumbLength,
    int *selectionStart,
    int *selectionEnd,
    unsigned *manualTickCount,
    bool *selectionVisible,
    bool *minMaxLabelsVisible,
    bool *valueLabelVisible,
    bool *rightToLeft,
    bool *rootRightToLeft,
    bool *sliderRightToLeft)
{
    if ( !control.m_winui || !control.m_winui->root || !control.m_winui->selectionNormal ||
         !control.m_winui->selectionDisabled ||
         !control.m_winui->selectionHighContrast ||
         !control.m_winui->selectionHighContrastDisabled ||
         !control.m_winui->minimumLabel || !control.m_winui->maximumLabel ||
         !control.m_winui->valueLabel )
    {
        return false;
    }

    try
    {
        if ( thumbLength )
            *thumbLength = control.GetThumbLength();
        if ( selectionStart )
            *selectionStart = control.m_selStart;
        if ( selectionEnd )
            *selectionEnd = control.m_selEnd;
        if ( manualTickCount )
        {
            *manualTickCount = static_cast<unsigned>(wxMin<std::size_t>(
                control.m_manualTicks.size(),
                std::numeric_limits<unsigned>::max()));
        }
        if ( selectionVisible )
        {
            *selectionVisible =
                control.m_winui->selectionNormal.Visibility() ==
                    MUX::Visibility::Visible ||
                control.m_winui->selectionDisabled.Visibility() ==
                    MUX::Visibility::Visible ||
                control.m_winui->selectionHighContrast.Visibility() ==
                    MUX::Visibility::Visible ||
                control.m_winui->selectionHighContrastDisabled.Visibility() ==
                    MUX::Visibility::Visible;
        }
        if ( minMaxLabelsVisible )
        {
            *minMaxLabelsVisible =
                control.m_winui->minimumLabel.Visibility() == MUX::Visibility::Visible &&
                control.m_winui->maximumLabel.Visibility() == MUX::Visibility::Visible;
        }
        if ( valueLabelVisible )
        {
            *valueLabelVisible =
                control.m_winui->valueLabel.Visibility() == MUX::Visibility::Visible;
        }
        const bool rootRTL =
            control.m_winui->root.FlowDirection() ==
                MUX::FlowDirection::RightToLeft;
        const bool sliderRTL =
            control.m_winui->slider.FlowDirection() ==
                MUX::FlowDirection::RightToLeft;
        if ( rightToLeft )
            *rightToLeft = rootRTL && sliderRTL;
        if ( rootRightToLeft )
            *rootRightToLeft = rootRTL;
        if ( sliderRightToLeft )
            *sliderRightToLeft = sliderRTL;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider decoration test query", e);
        return false;
    }

    return true;
}

bool wxWinUISliderTestAccess::HasTick(
    const wxSlider& control,
    int tickPos)
{
    return std::binary_search(
        control.m_manualTicks.begin(), control.m_manualTicks.end(), tickPos);
}

bool wxWinUISliderTestAccess::ConvergeVisualState(wxSlider& control)
{
    if ( !control.m_winui || !control.m_winui->root || !control.m_winui->slider ||
         !control.m_winui->callbackState )
    {
        if ( control.m_winui )
        {
            control.m_winui->lastConvergenceFailure =
                ConvergenceFailure::MissingPeer;
        }
        return false;
    }

    control.m_winui->lastConvergenceFailure = ConvergenceFailure::None;

    wxWinUISliderImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::Grid root = control.m_winui->root;
    const MUXC::Slider slider = control.m_winui->slider;
    const auto getCurrentOwner = [&]() -> wxSlider *
    {
        wxSlider * const owner = state->GetOwner(generation);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == state &&
                       wxWinUISameSliderObject(
                           owner->m_winui->root, root) &&
                       wxWinUISameSliderObject(
                           owner->m_winui->slider, slider)
                   ? owner
                   : nullptr;
    };

    // A newly registered shared slot can require a dispatcher turn before its
    // first non-empty XAML arrange. Use the same complete snapshot gate as the
    // visual tests and the same 500 ms bound as their WaitFor() helper instead
    // of assuming that an arbitrary number of immediate layout calls is a
    // completed frame. Every operation below is a re-entrant lifetime
    // boundary, so resolve the owner again before touching the implementation.
    wxStopWatch stopwatch;
    for ( ;; )
    {
        wxSlider *owner = getCurrentOwner();
        if ( !owner )
            return false;

        owner->m_winui->host.ForceRender();
        owner = getCurrentOwner();
        if ( !owner )
            return false;

        owner->UpdateVisualState();
        owner = getCurrentOwner();
        if ( !owner )
            return false;

        VisualState observed;
        if ( GetVisualState(*owner, &observed) )
        {
            owner = getCurrentOwner();
            if ( !owner )
                return false;
            owner->m_winui->lastConvergenceFailure =
                ConvergenceFailure::None;
            return true;
        }

        owner = getCurrentOwner();
        if ( !owner )
            return false;

        if ( stopwatch.Time() > 500 )
        {
            try
            {
                const double rootWidth = root.ActualWidth();
                const double rootHeight = root.ActualHeight();
                const double sliderWidth = slider.ActualWidth();
                const double sliderHeight = slider.ActualHeight();
                const MUXCP::Thumb thumb = wxWinUIFindSliderThumb(slider);
                const MUX::FrameworkElement track =
                    wxWinUIFindSliderTrack(slider, thumb);

                if ( !root.IsLoaded() || !slider.IsLoaded() )
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::NotLoaded;
                }
                else if ( rootWidth <= 0.0 || rootHeight <= 0.0 ||
                          sliderWidth <= 0.0 || sliderHeight <= 0.0 )
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::EmptyGeometry;
                }
                else if ( !thumb )
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::MissingThumb;
                }
                else if ( !track )
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::MissingTrack;
                }
                else if ( owner->m_thumbLengthDIP >= 0.0 &&
                          !(owner->m_winui->visualLayoutReady) )
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::ThumbNotArranged;
                }
                else if ( owner->m_winui->visualStateDirty )
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::VisualStateDirty;
                }
                else if ( !owner->m_winui->visualCacheValid )
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::CacheInvalid;
                }
                else if ( !owner->m_winui->visualLayoutReady )
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::LayoutNotReady;
                }
                else
                {
                    owner->m_winui->lastConvergenceFailure =
                        ConvergenceFailure::SnapshotRejected;
                }
            }
            catch ( const winrt::hresult_error& )
            {
                owner->m_winui->lastConvergenceFailure =
                    ConvergenceFailure::SnapshotRejected;
            }
            return false;
        }

        wxYield();
        if ( !getCurrentOwner() )
            return false;
    }
}

bool wxWinUISliderTestAccess::RefreshForScale(
    wxSlider& control,
    double scale)
{
    if ( !control.m_winui || !control.m_winui->slider || !control.m_winui->callbackState ||
         scale <= 0.0 || !std::isfinite(scale) )
    {
        return false;
    }

    // This seam no longer fabricates a raster scale. The caller must provide
    // the value observed from this control's current real XamlRoot; a host/slot
    // migration is therefore part of the tested production path.
    if ( std::abs(control.GetEffectiveRasterScale() - scale) >= 0.001 )
        return false;
    control.m_winui->visualCacheValid = false;
    control.m_winui->visualStateDirty = true;
    control.m_winui->visualLayoutReady = false;
    return ConvergeVisualState(control);
}

bool wxWinUISliderTestAccess::GetHostScaleState(
    const wxSlider& control,
    double *scale,
    std::uintptr_t *xamlRootIdentity,
    std::uintptr_t *contentRootIdentity)
{
    if ( !scale || !xamlRootIdentity || !control.m_winui || !control.m_winui->root ||
         !control.m_winui->slider || !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUISliderImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::Grid root = control.m_winui->root;
    const MUXC::Slider slider = control.m_winui->slider;
    implementation->host.ForceRender();

    wxSlider * const owner = state->GetOwner(generation);
    if ( owner != &control || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         !wxWinUISameSliderObject(owner->m_winui->root, root) ||
         !wxWinUISameSliderObject(owner->m_winui->slider, slider) )
    {
        return false;
    }

    try
    {
        const MUX::XamlRoot xamlRoot = root.XamlRoot();
        if ( !xamlRoot || !root.IsLoaded() || !slider.IsLoaded() )
            return false;

        const double observedScale = xamlRoot.RasterizationScale();
        const std::uintptr_t observedRootIdentity =
            wxWinUISliderObjectIdentity(xamlRoot);
        const std::uintptr_t observedContentIdentity =
            wxWinUISliderObjectIdentity(root);
        if ( !(observedScale > 0.0) || !std::isfinite(observedScale) ||
             !observedRootIdentity || !observedContentIdentity )
        {
            return false;
        }

        *scale = observedScale;
        *xamlRootIdentity = observedRootIdentity;
        if ( contentRootIdentity )
            *contentRootIdentity = observedContentIdentity;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider host scale query", e);
        return false;
    }
    return true;
}

bool wxWinUISliderTestAccess::RefreshVisualState(wxSlider& control)
{
    if ( !control.m_winui || !control.m_winui->slider || !control.m_winui->callbackState )
        return false;

    control.m_winui->visualCacheValid = false;
    control.m_winui->visualStateDirty = true;
    control.m_winui->visualLayoutReady = false;
    return ConvergeVisualState(control);
}

wxWinUISliderTestAccess::ConvergenceFailure wxWinUISliderTestAccess::GetLastConvergenceFailure(const wxSlider& control)
{
    return control.m_winui
        ? control.m_winui->lastConvergenceFailure
        : ConvergenceFailure::MissingPeer;
}

bool wxWinUISliderTestAccess::DeliverThemeChanged(wxSlider& control)
{
    if ( !control.m_winui || !control.m_winui->slider || !control.m_winui->callbackState ||
         !control.m_winui->actualThemeChangedToken.value )
    {
        return false;
    }

    wxWinUISliderImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::Slider slider = control.m_winui->slider;
    try
    {
        const MUX::ElementTheme actual = slider.ActualTheme();
        slider.RequestedTheme(
            actual == MUX::ElementTheme::Dark
                ? MUX::ElementTheme::Light
                : MUX::ElementTheme::Dark);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI Slider test theme transition", e);
        return false;
    }

    wxSlider * const owner = state->GetOwner(generation);
    return owner && owner->m_winui &&
           owner->m_winui.get() == implementation &&
           wxWinUISameSliderObject(owner->m_winui->slider, slider);
}

bool wxWinUISliderTestAccess::RetemplatePeer(
    wxSlider& control,
    RetemplateHook hook,
    void *context)
{
    if ( !control.m_winui || !control.m_winui->root || !control.m_winui->slider ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUISliderImpl * const implementation = control.m_winui.get();
    const auto callbackState = control.m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    const MUXC::Grid root = control.m_winui->root;
    const MUXC::Slider slider = control.m_winui->slider;
    const auto getCurrentOwner = [&]() -> wxSlider *
    {
        wxSlider * const owner = callbackState->GetOwner(generation);
        return owner && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState &&
                       wxWinUISameSliderObject(
                           owner->m_winui->root, root) &&
                       wxWinUISameSliderObject(
                           owner->m_winui->slider, slider)
                   ? owner
                   : nullptr;
    };

    MUXC::ControlTemplate peerTemplate{ nullptr };
    MUXCP::Thumb retiredThumb{ nullptr };
    try
    {
        slider.ApplyTemplate();
        peerTemplate = slider.Template();
        retiredThumb = wxWinUIFindSliderThumb(slider);
        if ( !peerTemplate || !retiredThumb || !getCurrentOwner() )
            return false;

        // Dropping and restoring the real ControlTemplate guarantees that
        // the old Thumb identity is detached and a new visual subtree is
        // instantiated. This is a test seam, but all re-application remains
        // in the production LayoutUpdated/UpdateVisualState path.
        implementation->appliedThumb = nullptr;
        implementation->appliedThumbLength = -1.0;
        implementation->visualCacheValid = false;
        implementation->visualStateDirty = true;
        implementation->visualLayoutReady = false;
        slider.Template(nullptr);
        slider.ApplyTemplate();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider test template detach", e);
        if ( getCurrentOwner() && peerTemplate )
        {
            try
            {
                slider.Template(peerTemplate);
                slider.ApplyTemplate();
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
        return false;
    }

    wxSlider *owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( hook )
        hook(owner, context);

    // The hook may synchronously destroy this wxSlider. Resolve through the
    // invalidatable state before touching either the peer or implementation.
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    try
    {
        slider.Template(peerTemplate);
        slider.ApplyTemplate();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider test template restore", e);
        return false;
    }

    owner = getCurrentOwner();
    if ( !owner )
        return false;
    owner->UpdateVisualState();
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    implementation->host.ForceRender();
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    owner->UpdateVisualState();
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    const MUXCP::Thumb replacementThumb =
        wxWinUIFindSliderThumb(slider);
    return replacementThumb &&
           !wxWinUISameSliderObject(
               replacementThumb, retiredThumb);
}

bool wxWinUISliderTestAccess::GetVisualState(
    const wxSlider& control,
    VisualState *state)
{
    if ( !state || !control.m_winui || !control.m_winui->root ||
         !control.m_winui->slider || !control.m_winui->tickCanvas ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUISliderImpl * const implementation = control.m_winui.get();
    const auto callbackState = control.m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    const MUXC::Grid root = control.m_winui->root;
    const MUXC::Slider slider = control.m_winui->slider;
    const MUXC::Canvas tickCanvas = control.m_winui->tickCanvas;

    // This seam observes production state: it does not invalidate or rebuild
    // the overlay itself. The synchronous host layout drives the same
    // Loaded/SizeChanged/LayoutUpdated callbacks used outside tests.
    implementation->host.ForceRender();
    wxSlider * const liveOwner = callbackState->GetOwner(generation);
    if ( !liveOwner || liveOwner != &control || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != implementation ||
         liveOwner->m_winui->callbackState != callbackState ||
         !wxWinUISameSliderObject(liveOwner->m_winui->root, root) ||
         !wxWinUISameSliderObject(liveOwner->m_winui->slider, slider) ||
         !wxWinUISameSliderObject(
             liveOwner->m_winui->tickCanvas, tickCanvas) ||
         !liveOwner->m_winui->visualCacheValid ||
         liveOwner->m_winui->visualStateDirty ||
         !liveOwner->m_winui->visualLayoutReady )
    {
        return false;
    }

    try
    {
        const double width = root.ActualWidth();
        const double height = root.ActualHeight();
        const double sliderWidth = slider.ActualWidth();
        const double sliderHeight = slider.ActualHeight();
        if ( !root.IsLoaded() || !slider.IsLoaded() ||
             width <= 0.0 || height <= 0.0 ||
             sliderWidth <= 0.0 || sliderHeight <= 0.0 ||
             !std::isfinite(width) || !std::isfinite(height) ||
             !std::isfinite(sliderWidth) ||
             !std::isfinite(sliderHeight) )
        {
            return false;
        }

        const MUXC::Canvas decorationCanvas =
            implementation->decorationCanvas;
        const MUXC::Border selectionVisuals[] =
        {
            implementation->selectionNormal,
            implementation->selectionDisabled,
            implementation->selectionHighContrast,
            implementation->selectionHighContrastDisabled
        };
        const MUXC::TextBlock labels[] =
        {
            implementation->minimumLabel,
            implementation->maximumLabel,
            implementation->valueLabel
        };
        if ( !decorationCanvas || !selectionVisuals[0] ||
             !selectionVisuals[1] || !selectionVisuals[2] ||
             !selectionVisuals[3] || !labels[0] || !labels[1] ||
             !labels[2] )
        {
            return false;
        }

        // The snapshot is admissible only for the exact subtree authored by
        // UpdateVisualState(): overlay -> root, tick canvas/labels/selections
        // -> overlay, and ticks -> tick canvas. A detached or foreign peer is
        // never interpreted using cached/model coordinates.
        if ( !wxWinUIHasExactSliderParent(slider, root) ||
             !wxWinUIHasExactSliderParent(decorationCanvas, root) ||
             !wxWinUIHasExactSliderParent(
                 tickCanvas, decorationCanvas) )
        {
            return false;
        }
        for ( const MUXC::Border& selection : selectionVisuals )
        {
            if ( !wxWinUIHasExactSliderParent(
                     selection, decorationCanvas) )
            {
                return false;
            }
        }
        for ( const MUXC::TextBlock& label : labels )
        {
            if ( !wxWinUIHasExactSliderParent(label, decorationCanvas) )
                return false;
        }

        const MUX::XamlRoot xamlRoot = root.XamlRoot();
        if ( !xamlRoot || !wxWinUIHasSliderXamlRoot(slider, xamlRoot) ||
             !wxWinUIHasSliderXamlRoot(
                 decorationCanvas, xamlRoot) ||
             !wxWinUIHasSliderXamlRoot(tickCanvas, xamlRoot) )
        {
            return false;
        }
        for ( const MUXC::Border& selection : selectionVisuals )
        {
            if ( !wxWinUIHasSliderXamlRoot(selection, xamlRoot) )
                return false;
        }
        for ( const MUXC::TextBlock& label : labels )
        {
            if ( !wxWinUIHasSliderXamlRoot(label, xamlRoot) )
                return false;
        }

        VisualRect sliderRect;
        VisualRect decorationRect;
        if ( !wxWinUISliderImpl::GetRealizedRect(
                 slider, root, &sliderRect) ||
             !wxWinUISliderImpl::GetRealizedRect(
                 decorationCanvas, root, &decorationRect) )
        {
            return false;
        }
        if ( std::abs(decorationRect.x) >= 0.25 ||
             std::abs(decorationRect.y) >= 0.25 ||
             std::abs(decorationRect.width - width) >= 0.25 ||
             std::abs(decorationRect.height - height) >= 0.25 )
        {
            return false;
        }

        VisualState snapshot;
        snapshot.rasterScale = liveOwner->GetEffectiveRasterScale();
        snapshot.rootWidth = width;
        snapshot.rootHeight = height;
        const MUX::Thickness sliderMargin = slider.Margin();
        snapshot.sliderMarginLeft = sliderMargin.Left;
        snapshot.sliderMarginTop = sliderMargin.Top;
        snapshot.sliderMarginRight = sliderMargin.Right;
        snapshot.sliderMarginBottom = sliderMargin.Bottom;
        if ( !std::isfinite(snapshot.sliderMarginLeft) ||
             !std::isfinite(snapshot.sliderMarginTop) ||
             !std::isfinite(snapshot.sliderMarginRight) ||
             !std::isfinite(snapshot.sliderMarginBottom) ||
             snapshot.sliderMarginLeft < 0.0 ||
             snapshot.sliderMarginTop < 0.0 ||
             snapshot.sliderMarginRight < 0.0 ||
             snapshot.sliderMarginBottom < 0.0 )
        {
            return false;
        }
        snapshot.vertical =
            slider.Orientation() == MUXC::Orientation::Vertical;
        snapshot.actualTheme =
            wxWinUIGetObservedSliderTheme(slider.ActualTheme());

        const MUXCP::Thumb thumb =
            wxWinUIFindSliderThumb(slider);
        const MUX::FrameworkElement track =
            wxWinUIFindSliderTrack(slider, thumb);
        if ( !thumb || !track ||
             !wxWinUIHasSliderXamlRoot(thumb, xamlRoot) ||
             !wxWinUIHasSliderXamlRoot(track, xamlRoot) )
        {
            return false;
        }

        VisualRect thumbRect;
        VisualRect trackRect;
        VisualRect thumbOverlayRect;
        VisualRect trackOverlayRect;
        if ( !wxWinUISliderImpl::GetRealizedRect(thumb, root, &thumbRect) ||
             !wxWinUISliderImpl::GetRealizedRect(track, root, &trackRect) ||
             !wxWinUISliderImpl::GetRealizedRect(
                 thumb, decorationCanvas, &thumbOverlayRect) ||
             !wxWinUISliderImpl::GetRealizedRect(
                 track, decorationCanvas, &trackOverlayRect) )
        {
            return false;
        }
        snapshot.thumbIdentity = wxWinUISliderObjectIdentity(thumb);
        snapshot.trackIdentity = wxWinUISliderObjectIdentity(track);
        snapshot.nativeThumb = thumbRect;
        snapshot.nativeTrack = trackRect;
        const winrt::hstring expectedThumbName = snapshot.vertical
            ? L"VerticalThumb"
            : L"HorizontalThumb";
        snapshot.thumbNamedForOrientation =
            thumb.Name() == expectedThumbName;
        snapshot.thumbLengthDIPs = snapshot.vertical
            ? thumb.ActualHeight()
            : thumb.ActualWidth();
        snapshot.axisCross = snapshot.vertical
            ? trackRect.x + trackRect.width / 2.0
            : trackRect.y + trackRect.height / 2.0;
        const double trackPrimaryStart = snapshot.vertical
            ? trackRect.y
            : trackRect.x;
        const double trackPrimaryExtent = snapshot.vertical
            ? trackRect.height
            : trackRect.width;
        const double thumbPrimaryExtent = snapshot.vertical
            ? thumbRect.height
            : thumbRect.width;
        snapshot.axisStart =
            trackPrimaryStart + thumbPrimaryExtent / 2.0;
        snapshot.axisEnd =
            trackPrimaryStart + trackPrimaryExtent - thumbPrimaryExtent / 2.0;
        snapshot.nativeThumbAxis = snapshot.vertical
            ? thumbRect.y + thumbRect.height / 2.0
            : thumbRect.x + thumbRect.width / 2.0;
        const double rangeWidth =
            static_cast<double>(
                static_cast<std::int64_t>(liveOwner->m_rangeMax) -
                static_cast<std::int64_t>(liveOwner->m_rangeMin));
        const double currentRatio = wxClip(
            (static_cast<double>(liveOwner->m_value) -
             static_cast<double>(liveOwner->m_rangeMin)) / rangeWidth,
            0.0,
            1.0);
        const double normalThumbAxis =
            snapshot.axisStart +
                currentRatio * (snapshot.axisEnd - snapshot.axisStart);
        const double reversedThumbAxis =
            snapshot.axisEnd -
                currentRatio * (snapshot.axisEnd - snapshot.axisStart);
        snapshot.axisReversed =
            snapshot.vertical != slider.IsDirectionReversed();
        if ( std::abs(normalThumbAxis - reversedThumbAxis) >= 0.25 )
        {
            snapshot.axisReversed =
                std::abs(snapshot.nativeThumbAxis - reversedThumbAxis) <
                std::abs(snapshot.nativeThumbAxis - normalThumbAxis);
        }
        const double overlayTrackStart = snapshot.vertical
            ? trackOverlayRect.y
            : trackOverlayRect.x;
        const double overlayTrackExtent = snapshot.vertical
            ? trackOverlayRect.height
            : trackOverlayRect.width;
        const double overlayThumbExtent = snapshot.vertical
            ? thumbOverlayRect.height
            : thumbOverlayRect.width;
        const double overlayAxisStart =
            overlayTrackStart + overlayThumbExtent / 2.0;
        const double overlayAxisEnd =
            overlayTrackStart + overlayTrackExtent -
                overlayThumbExtent / 2.0;
        const double overlayThumbAxis = snapshot.vertical
            ? thumbOverlayRect.y + thumbOverlayRect.height / 2.0
            : thumbOverlayRect.x + thumbOverlayRect.width / 2.0;
        const double overlayAxisCross = snapshot.vertical
            ? trackOverlayRect.x + trackOverlayRect.width / 2.0
            : trackOverlayRect.y + trackOverlayRect.height / 2.0;
        if ( !std::isfinite(snapshot.thumbLengthDIPs) ||
             snapshot.thumbLengthDIPs <= 0.0 ||
             !std::isfinite(snapshot.axisCross) ||
             !std::isfinite(snapshot.axisStart) ||
             !std::isfinite(snapshot.axisEnd) ||
             !std::isfinite(snapshot.nativeThumbAxis) ||
             snapshot.axisEnd <= snapshot.axisStart ||
             snapshot.thumbIdentity != implementation->visualThumbIdentity ||
             snapshot.trackIdentity != implementation->visualTrackIdentity ||
             std::abs(overlayAxisCross -
                      implementation->visualAxisCross) >= 0.25 ||
             std::abs(overlayAxisStart -
                      implementation->visualAxisStart) >= 0.25 ||
             std::abs(overlayAxisEnd -
                      implementation->visualAxisEnd) >= 0.25 ||
             std::abs(overlayThumbAxis -
                      implementation->visualThumbAxis) >= 0.25 )
        {
            return false;
        }
        if ( liveOwner->m_thumbLengthDIP >= 0.0 &&
             (!wxWinUISameSliderObject(
                  thumb, implementation->appliedThumb) ||
              std::abs(snapshot.thumbLengthDIPs -
                       liveOwner->m_thumbLengthDIP) >= 0.25) )
        {
            return false;
        }

        const auto tickChildren = tickCanvas.Children();
        snapshot.renderedTickCount = tickChildren.Size();
        double tickAxisMinimum = std::numeric_limits<double>::infinity();
        double tickAxisMaximum = -std::numeric_limits<double>::infinity();
        for ( const MUX::UIElement& child : tickChildren )
        {
            const MUXC::Border mark = child.try_as<MUXC::Border>();
            if ( !mark ||
                 !wxWinUIHasExactSliderParent(mark, tickCanvas) ||
                 !wxWinUIHasSliderXamlRoot(mark, xamlRoot) )
            {
                return false;
            }

            VisualRect markRect;
            if ( !wxWinUISliderImpl::GetRealizedRect(
                     mark, root, &markRect) )
            {
                return false;
            }
            const double cross = snapshot.vertical
                ? markRect.x + markRect.width / 2.0
                : markRect.y + markRect.height / 2.0;
            const double primary = snapshot.vertical
                ? markRect.y + markRect.height / 2.0
                : markRect.x + markRect.width / 2.0;
            tickAxisMinimum = wxMin(tickAxisMinimum, primary);
            tickAxisMaximum = wxMax(tickAxisMaximum, primary);
            if ( cross < snapshot.axisCross )
                ++snapshot.firstSideTickCount;
            else
                ++snapshot.secondSideTickCount;
        }
        if ( snapshot.renderedTickCount )
        {
            snapshot.renderedTickAxisMinimum = tickAxisMinimum;
            snapshot.renderedTickAxisMaximum = tickAxisMaximum;
        }

        const auto getTextRect =
            [&root](const MUXC::TextBlock& label,
                    VisualRect *rect)
        {
            if ( label.Visibility() != MUX::Visibility::Visible )
            {
                *rect = VisualRect{};
                return true;
            }
            return wxWinUISliderImpl::GetRealizedRect(label, root, rect);
        };
        if ( !getTextRect(labels[0], &snapshot.minimumLabel) ||
             !getTextRect(labels[1], &snapshot.maximumLabel) ||
             !getTextRect(labels[2], &snapshot.valueLabel) )
        {
            return false;
        }

        MUXC::Border visibleSelection{ nullptr };
        for ( const MUXC::Border& selection : selectionVisuals )
        {
            if ( selection.Visibility() == MUX::Visibility::Visible )
            {
                ++snapshot.visibleSelectionCount;
                visibleSelection = selection;
            }
        }
        if ( snapshot.visibleSelectionCount > 1 )
            return false;

        snapshot.selectionVisible =
            snapshot.visibleSelectionCount == 1;
        if ( visibleSelection )
        {
            if ( !wxWinUISliderImpl::GetRealizedRect(
                     visibleSelection, root, &snapshot.selection) )
            {
                return false;
            }

            if ( wxWinUISameSliderObject(
                     visibleSelection,
                     implementation->selectionNormal) )
            {
                snapshot.selectionVisual = SelectionVisual::Normal;
                snapshot.selectionThemeBound =
                    implementation->selectionNormalThemeBound;
            }
            else if ( wxWinUISameSliderObject(
                          visibleSelection,
                          implementation->selectionDisabled) )
            {
                snapshot.selectionVisual = SelectionVisual::Disabled;
                snapshot.selectionThemeBound =
                    implementation->selectionDisabledThemeBound;
            }
            else if ( wxWinUISameSliderObject(
                          visibleSelection,
                          implementation->selectionHighContrast) )
            {
                snapshot.selectionVisual =
                    SelectionVisual::HighContrast;
                snapshot.selectionThemeBound =
                    implementation->selectionHighContrastThemeBound;
            }
            else if ( wxWinUISameSliderObject(
                          visibleSelection,
                          implementation->selectionHighContrastDisabled) )
            {
                snapshot.selectionVisual =
                    SelectionVisual::HighContrastDisabled;
                snapshot.selectionThemeBound =
                    implementation->selectionHighContrastDisabledThemeBound;
            }
            else
            {
                return false;
            }

            const MUXM::Brush brush = visibleSelection.Background();
            if ( !brush )
                return false;
            snapshot.selectionBrushIdentity =
                wxWinUISliderObjectIdentity(brush);
            const MUXM::SolidColorBrush solid =
                brush.try_as<MUXM::SolidColorBrush>();
            if ( !solid )
                return false;
            snapshot.selectionHasSolidColor = true;
            snapshot.selectionColorARGB =
                wxWinUIPackSliderColor(solid.Color());
        }

        wxSlider * const currentOwner =
            callbackState->GetOwner(generation);
        if ( currentOwner != &control || !currentOwner->m_winui ||
             currentOwner->m_winui.get() != implementation ||
             !wxWinUISameSliderObject(
                 currentOwner->m_winui->root, root) ||
             !wxWinUISameSliderObject(
                 currentOwner->m_winui->slider, slider) ||
             !wxWinUISameSliderObject(
                 currentOwner->m_winui->decorationCanvas,
                 decorationCanvas) ||
             !wxWinUISameSliderObject(
                 currentOwner->m_winui->tickCanvas, tickCanvas) )
        {
            return false;
        }

        snapshot.visualTreeCoherent = true;
        snapshot.sameXamlRoot = true;
        *state = snapshot;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider visual test query", e);
        return false;
    }

    return true;
}
#endif

#endif // wxUSE_SLIDER
