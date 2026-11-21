/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/dataviewheader.h
// Purpose:     Retained XAML DataView column header and native resize authority
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_DATAVIEWHEADER_H_
#define _WX_WINUI_DATAVIEWHEADER_H_

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <UIAutomation.h>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace wxWinUIDataViewHeader
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation::Peers;
namespace P = X::Automation::Provider;

struct DropLocation
{
    unsigned position = 0;
    double edge = 0;
    bool changesOrder = false;
};

// A single midpoint rule supplies both live feedback and the committed order.
// Zero-width (hidden) columns remain in the logical permutation.
inline bool FindDropLocation(const std::vector<double>& widths, unsigned source,
                             double left, DropLocation& result)
{
    if ( source >= widths.size() || !std::isfinite(left) ) return false;
    for ( const auto width : widths )
        if ( !std::isfinite(width) || width < 0 ) return false;
    const double centre = left + widths[source] / 2;
    if ( !std::isfinite(centre) ) return false;
    double edge = 0;
    unsigned target = 0;
    for ( unsigned index = 0; index < widths.size(); ++index )
    {
        if ( index != source )
        {
            if ( centre < edge + widths[index] / 2 ) break;
            ++target;
        }
        edge += widths[index];
    }
    result = {target, edge, target != source};
    return true;
}

struct InsertionMarker
{
    InsertionMarker()
    {
        // Real themed XAML decoration; it must never intercept drag input.
        grid = X::Markup::XamlReader::Load(LR"(
          <Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                Width="9" Height="36" IsHitTestVisible="False" Visibility="Collapsed">
            <Border Width="3" CornerRadius="1.5" Background="{ThemeResource AccentFillColorDefaultBrush}"/>
            <Border Height="3" VerticalAlignment="Top" CornerRadius="1.5" Background="{ThemeResource AccentFillColorDefaultBrush}"/>
            <Border Height="3" VerticalAlignment="Bottom" CornerRadius="1.5" Background="{ThemeResource AccentFillColorDefaultBrush}"/>
          </Grid>)").as<C::Grid>();
        X::Automation::AutomationProperties::SetAccessibilityView(grid, A::AccessibilityView::Raw);
        C::Canvas::SetZIndex(grid, 2);
    }
    void Update(double edge, bool visible)
    {
        C::Canvas::SetLeft(grid, edge - 4.5);
        grid.Visibility(visible ? X::Visibility::Visible : X::Visibility::Collapsed);
    }
    void Clear() { grid.Visibility(X::Visibility::Collapsed); }
    C::Grid grid{nullptr};
};

struct Binding
{
    std::function<bool()> current;
    std::function<bool()> resizable;
    std::function<bool()> beginResize;
    std::function<bool()> movable;
    std::function<bool()> beginMove;
    std::function<double()> left;
    std::function<std::uint64_t()> version;
    std::function<bool(double)> moveToLeft;
    std::function<bool(double)> previewMove;
    std::function<void()> clearMove;
    std::function<bool(double, double)> moveToScreen;
    std::function<bool()> rightToLeft;
    std::function<double()> width;
    std::function<double()> scale;
    std::function<bool(double)> resize;
    std::function<void()> autoSize;
    std::function<void(bool)> click;
    std::function<winrt::Windows::Foundation::Rect()> bounds;
    double startWidth = 0;
    double requestedWidth = 0;
    bool dragging = false;
    bool moving = false;
    bool automationRequested = false;
    double startLeft = 0;
    double requestedLeft = 0;
    std::uint64_t moveVersion = 0;

    bool Current() const { return current && current(); }
    bool CanResize() const { return Current() && resizable && resizable(); }
    bool CanMove() const { return Current() && movable && movable(); }

    bool StartMoveGesture()
    {
        if ( !CanMove() || !beginMove() || !Current() ) return false;
        startLeft = requestedLeft = left();
        if ( !Current() || startLeft < 0 ) return false;
        moveVersion = version();
        moving = true;
        if ( previewMove && !previewMove(requestedLeft) )
        { EndMoveGesture(true); return false; }
        return true;
    }
    bool ContinueMoveGesture(double delta)
    {
        if ( !moving || !CanMove() || moveVersion != version() ) return false;
        if ( !std::isfinite(delta) ) return true;
        requestedLeft += rightToLeft() ? -delta : delta;
        if ( !std::isfinite(requestedLeft) ) return false;
        return !previewMove || previewMove(requestedLeft);
    }
    void EndMoveGesture(bool cancelled)
    {
        const bool wasMoving = moving;
        moving = false;
        if ( clearMove ) clearMove();
        if ( wasMoving && !cancelled && CanMove() && moveVersion == version() )
            moveToLeft(requestedLeft);
    }
};

// Expose the same width authority used by the native Thumb to accessibility.
// No test-only entry point or public wx API is needed to exercise it.
struct Peer : A::FrameworkElementAutomationPeerT<Peer, P::ITransformProvider>
{
    Peer(const X::FrameworkElement& owner, const std::shared_ptr<Binding>& binding)
        : FrameworkElementAutomationPeerT(owner), m_binding(binding)
        { binding->automationRequested = true; }

    A::AutomationControlType GetAutomationControlTypeCore() const
        { Check(); return A::AutomationControlType::HeaderItem; }
    winrt::hstring GetClassNameCore() const { Check(); return L"wxDataViewColumnHeader"; }
    winrt::hstring GetNameCore()
        { Check(); return FrameworkElementAutomationPeerT::GetNameCore(); }
    winrt::Windows::Foundation::Rect GetBoundingRectangleCore() const
        { return Check()->bounds(); }
    bool IsOffscreenCore() const
    {
        const auto rect = GetBoundingRectangleCore();
        return rect.Width <= 0 || rect.Height <= 0;
    }
    winrt::Windows::Foundation::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        Check();
        if ( pattern == A::PatternInterface::Transform ) return *this;
        return FrameworkElementAutomationPeerT::GetPatternCore(pattern);
    }
    bool CanMove() const
    {
        const auto binding = m_binding.lock();
        return binding && binding->CanMove();
    }
    bool CanRotate() const { return false; }
    bool CanResize() const
    {
        const auto binding = m_binding.lock();
        return binding && binding->CanResize();
    }
    void Move(double x, double y)
    {
        const auto binding = Check();
        if ( !std::isfinite(x) || !std::isfinite(y) ) throw winrt::hresult_invalid_argument();
        if ( !binding->CanMove() )
            throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
        const bool ready = binding->beginMove();
        Check();
        if ( !ready || !binding->CanMove() ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
        const bool moved = binding->moveToScreen(x, y);
        Check();
        if ( !moved ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    }
    void Rotate(double) { throw winrt::hresult_error(UIA_E_INVALIDOPERATION); }
    void Resize(double width, double height)
    {
        const auto binding = m_binding.lock();
        if ( !binding || !binding->Current() )
            throw winrt::hresult_error(UIA_E_ELEMENTNOTAVAILABLE);
        if ( !binding->CanResize() ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
        const double scale = binding->scale();
        // TransformProvider uses physical pixels; native Thumb deltas and
        // the retained XAML geometry are device-independent units.
        if ( !std::isfinite(width) || width <= 0 ||
             !std::isfinite(height) ||
             std::abs(height - Owner().as<X::FrameworkElement>().ActualHeight() * scale) > 0.5 )
            throw winrt::hresult_invalid_argument();
        if ( !binding->beginResize() || !binding->Current() || !binding->resize(width / binding->scale()) )
            throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    }
private:
    std::shared_ptr<Binding> Check() const
    {
        const auto binding = m_binding.lock();
        if ( !binding || !binding->Current() )
            throw winrt::hresult_error(UIA_E_ELEMENTNOTAVAILABLE);
        return binding;
    }
    std::weak_ptr<Binding> m_binding;
};

struct HeaderGrid : C::GridT<HeaderGrid>
{
    explicit HeaderGrid(const std::shared_ptr<Binding>& binding) : m_binding(binding) {}
    A::AutomationPeer OnCreateAutomationPeer()
    {
        return winrt::make<Peer>(this->try_as<X::FrameworkElement>(), m_binding);
    }
private:
    std::shared_ptr<Binding> m_binding;
};

struct GripGrid : C::GridT<GripGrid>
{
    GripGrid()
    {
        ProtectedCursor(winrt::Microsoft::UI::Input::InputSystemCursor::Create(
            winrt::Microsoft::UI::Input::InputSystemCursorShape::SizeWestEast));
    }
};

struct Header
{
    Header()
        : binding(std::make_shared<Binding>()),
          grid(winrt::make<HeaderGrid>(binding)),
          grip(winrt::make<GripGrid>())
    {
        grid.Height(36);
        button.Height(36);
        button.Padding({8, 0, 8, 0});
        button.HorizontalContentAlignment(X::HorizontalAlignment::Left);
        button.HorizontalAlignment(X::HorizontalAlignment::Stretch);
        button.VerticalAlignment(X::VerticalAlignment::Stretch);
        grid.Children().Append(button);
        moveGrip.Width(22);
        moveGrip.HorizontalAlignment(X::HorizontalAlignment::Left);
        moveThumb.HorizontalAlignment(X::HorizontalAlignment::Stretch);
        moveThumb.VerticalAlignment(X::VerticalAlignment::Stretch);
        moveThumb.IsTabStop(false);
        moveThumb.Template(X::Markup::XamlReader::Load(
            LR"(<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" TargetType="Thumb"><Grid Background="Transparent"><TextBlock Text="&#x22ee;&#x22ee;" HorizontalAlignment="Center" VerticalAlignment="Center"/></Grid></ControlTemplate>)")
                .as<C::ControlTemplate>());
        X::Automation::AutomationProperties::SetName(moveThumb, L"Reorder column");
        moveGrip.Children().Append(moveThumb);
        grid.RenderTransform(moveTransform);
        grip.Width(8);
        grip.HorizontalAlignment(X::HorizontalAlignment::Right);
        thumb.HorizontalAlignment(X::HorizontalAlignment::Stretch);
        thumb.VerticalAlignment(X::VerticalAlignment::Stretch);
        thumb.IsTabStop(false);
        thumb.Template(X::Markup::XamlReader::Load(
            LR"(<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" TargetType="Thumb"><Grid Background="Transparent"><Border Width="1" HorizontalAlignment="Center" Background="{ThemeResource ControlStrokeColorDefaultBrush}" Margin="0,8"/></Grid></ControlTemplate>)")
                .as<C::ControlTemplate>());
        grip.Children().Append(thumb);
        grid.Children().Append(grip);
        grid.Children().Append(moveGrip);

        const std::weak_ptr<Binding> weak = binding;
        button.Click([weak](const auto&, const auto&)
        {
            if ( auto state = weak.lock() )
                if ( state->Current() && state->click ) state->click(false);
        });
        button.RightTapped([weak](const auto&, const X::Input::RightTappedRoutedEventArgs& args)
        {
            if ( auto state = weak.lock() )
                if ( state->Current() && state->click )
                { args.Handled(true); state->click(true); }
        });
        thumb.DoubleTapped([weak](const auto&, const X::Input::DoubleTappedRoutedEventArgs& args)
        {
            if ( auto state = weak.lock() )
                if ( state->CanResize() && state->autoSize )
                { args.Handled(true); state->autoSize(); }
        });
        thumb.DragStarted([weak](const auto& sender, const auto&)
        {
            auto state = weak.lock();
            if ( !state || !state->CanResize() || !state->beginResize() || !state->Current() )
            { sender.template as<C::Primitives::Thumb>().CancelDrag(); return; }
            state->startWidth = state->requestedWidth = state->width();
            if ( !state->Current() )
            { sender.template as<C::Primitives::Thumb>().CancelDrag(); return; }
            state->dragging = true;
        });
        thumb.DragDelta([weak](const auto& sender, const C::Primitives::DragDeltaEventArgs& args)
        {
            auto state = weak.lock();
            if ( !state || !state->dragging || !state->CanResize() )
            { sender.template as<C::Primitives::Thumb>().CancelDrag(); return; }
            const double delta = args.HorizontalChange();
            if ( !std::isfinite(delta) ) return;
            state->requestedWidth += state->rightToLeft() ? -delta : delta;
            if ( !state->resize(state->requestedWidth) )
                sender.template as<C::Primitives::Thumb>().CancelDrag();
        });
        thumb.DragCompleted([weak](const auto&, const C::Primitives::DragCompletedEventArgs& args)
        {
            auto state = weak.lock();
            if ( !state || !state->dragging ) return;
            state->dragging = false;
            if ( args.Canceled() && state->CanResize() ) state->resize(state->startWidth);
        });
        const auto transform = moveTransform;
        const auto weakGrid = winrt::make_weak(grid);
        moveThumb.DragStarted([weak, weakGrid](const auto& sender, const auto&)
        {
            auto state = weak.lock();
            if ( !state || !state->StartMoveGesture() )
            { sender.template as<C::Primitives::Thumb>().CancelDrag(); return; }
            if ( const auto visual = weakGrid.get() )
            {
                C::Canvas::SetZIndex(visual, 1);
                visual.Opacity(0.72);
            }
        });
        moveThumb.DragDelta([weak, transform](const auto& sender, const C::Primitives::DragDeltaEventArgs& args)
        {
            auto state = weak.lock();
            if ( !state || !state->ContinueMoveGesture(args.HorizontalChange()) )
            { sender.template as<C::Primitives::Thumb>().CancelDrag(); return; }
            transform.X(state->requestedLeft - state->startLeft);
        });
        moveThumb.DragCompleted([weak, transform, weakGrid](const auto&, const C::Primitives::DragCompletedEventArgs& args)
        {
            transform.X(0);
            if ( const auto visual = weakGrid.get() )
            {
                C::Canvas::SetZIndex(visual, 0);
                visual.Opacity(1);
            }
            auto state = weak.lock();
            if ( state ) state->EndMoveGesture(args.Canceled());
        });
    }

    void Retire()
    {
        binding->EndMoveGesture(true);
        binding->current = nullptr;
        binding->dragging = false;
        binding->moving = false;
        moveTransform.X(0);
        C::Canvas::SetZIndex(grid, 0);
        grid.Opacity(1);
        try { thumb.CancelDrag(); }
        catch ( const winrt::hresult_error& )
        {
            // A disconnected native input site no longer owns a capture.
            // Retirement has already invalidated every callback/provider.
        }
        try { moveThumb.CancelDrag(); }
        catch ( const winrt::hresult_error& ) { }
    }

    std::shared_ptr<Binding> binding;
    C::Grid grid{nullptr};
    C::Grid grip{nullptr};
    C::Grid moveGrip;
    C::Button button;
    C::Primitives::Thumb thumb;
    C::Primitives::Thumb moveThumb;
    X::Media::TranslateTransform moveTransform;
    wxString title;
};
}

#endif // _WX_WINUI_DATAVIEWHEADER_H_
