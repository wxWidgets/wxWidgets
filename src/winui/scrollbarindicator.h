/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/scrollbarindicator.h
// Purpose:     Conscious indicators for standalone native ScrollBar peers
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WINUI_SCROLLBARINDICATOR_H_
#define _WX_WINUI_SCROLLBARINDICATOR_H_

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <chrono>
#include <memory>
#include <utility>
#include <vector>

// ScrollViewer owns this policy for its template children. Standalone bars
// have no ScrollViewer and default to None, even when their range is nonzero.
// Only IndicatorMode is controlled here: native template states, hit testing,
// conscious expansion and animation/accessibility preferences stay native.
class wxWinUIScrollBarIndicator final
    : public std::enable_shared_from_this<wxWinUIScrollBarIndicator>
{
    using Bar = winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollBar;
    using Thumb = winrt::Microsoft::UI::Xaml::Controls::Primitives::Thumb;
    using Mode = winrt::Microsoft::UI::Xaml::Controls::Primitives::ScrollingIndicatorMode;
    using Element = winrt::Microsoft::UI::Xaml::UIElement;
    using Handler = winrt::Microsoft::UI::Xaml::Input::PointerEventHandler;

public:
    static std::shared_ptr<wxWinUIScrollBarIndicator> Create(const Bar& bar)
    {
        auto self = std::shared_ptr<wxWinUIScrollBarIndicator>(new wxWinUIScrollBarIndicator(bar));
        self->Connect();
        return self;
    }

    ~wxWinUIScrollBarIndicator() { Retire(); }

    void Activity(bool touch = false)
    {
        if ( !m_live || !m_bar.IsLoaded() ) return;
        // Like ScrollViewer, mouse indication wins while it is already shown.
        m_bar.IndicatorMode(!touch || !m_autoHide || m_over || m_dragging ||
            m_bar.IndicatorMode() == Mode::MouseIndicator
                ? Mode::MouseIndicator : Mode::TouchIndicator);
        if ( m_live ) Arm();
    }

    void PointerOver(bool over)
    {
        if ( !m_live ) return;
        m_over = over;
        Activity();
    }

    void Dragging(bool dragging)
    {
        if ( !m_live ) return;
        m_dragging = dragging;
        Activity();
    }

    void Retire() noexcept
    {
        if ( !std::exchange(m_live, false) ) return;
        try
        {
            if ( m_timer ) { m_timer.Stop(); m_timer.Tick(m_tick); }
            if ( m_settings && m_settingsHooked )
                m_settings.AutoHideScrollBarsChanged(m_settingsChanged);
            if ( m_bar )
            {
                if ( m_enter ) m_bar.RemoveHandler(Element::PointerEnteredEvent(), m_enter);
                if ( m_exit ) m_bar.RemoveHandler(Element::PointerExitedEvent(), m_exit);
                if ( m_loadedHooked ) m_bar.Loaded(m_loaded);
                if ( m_unloadedHooked ) m_bar.Unloaded(m_unloaded);
            }
            ClearThumbs();
        }
        catch ( const winrt::hresult_error& ) { }
        m_bar = nullptr; m_timer = nullptr; m_settings = nullptr;
    }

private:
    explicit wxWinUIScrollBarIndicator(const Bar& bar) : m_bar(bar) {}

    struct ThumbHooks
    {
        Thumb peer{nullptr};
        winrt::event_token started{}, completed{};
    };

    void ClearThumbs()
    {
        for ( const auto& hooks : m_thumbs )
        {
            hooks.peer.DragStarted(hooks.started);
            hooks.peer.DragCompleted(hooks.completed);
        }
        m_thumbs.clear();
    }

    void FindThumbs(const winrt::Microsoft::UI::Xaml::DependencyObject& node)
    {
        namespace X = winrt::Microsoft::UI::Xaml;
        if ( auto thumb = node.try_as<Thumb>() )
        {
            const std::weak_ptr<wxWinUIScrollBarIndicator> weak = shared_from_this();
            ThumbHooks hooks;
            hooks.peer = thumb;
            hooks.started = thumb.DragStarted([weak](const auto&, const auto&)
                { if ( const auto self = weak.lock() ) self->Dragging(true); });
            hooks.completed = thumb.DragCompleted([weak](const auto&, const auto&)
                { if ( const auto self = weak.lock() ) self->Dragging(false); });
            m_thumbs.push_back(hooks);
            return;
        }
        const int count = X::Media::VisualTreeHelper::GetChildrenCount(node);
        for ( int i = 0; i < count; ++i )
            FindThumbs(X::Media::VisualTreeHelper::GetChild(node, i));
    }

    void OnLoaded()
    {
        if ( !m_live ) return;
        ClearThumbs();
        m_bar.ApplyTemplate();
        if ( !m_live ) return;
        FindThumbs(m_bar);
        if ( m_live ) PreferenceChanged();
    }

    void PreferenceChanged()
    {
        if ( !m_live ) return;
        m_autoHide = m_settings.AutoHideScrollBars();
        if ( !m_autoHide )
        {
            m_timer.Stop();
            m_bar.IndicatorMode(Mode::MouseIndicator);
        }
        else if ( m_bar.IndicatorMode() != Mode::None ) Arm();
    }

    void Arm()
    {
        if ( !m_live || !m_timer ) return;
        m_timer.Stop();
        if ( m_autoHide && !m_over && !m_dragging )
        {
            m_timer.Interval(std::chrono::seconds(2));
            m_timer.Start();
        }
    }

    void Connect()
    {
        namespace X = winrt::Microsoft::UI::Xaml;
        const std::weak_ptr<wxWinUIScrollBarIndicator> weak = shared_from_this();
        const auto queue = m_bar.DispatcherQueue();
        m_timer = queue.CreateTimer();
        m_timer.IsRepeating(false);
        m_tick = m_timer.Tick([weak](const auto&, const auto&)
        {
            if ( const auto self = weak.lock(); self && self->m_live &&
                 self->m_autoHide && !self->m_over && !self->m_dragging )
                self->m_bar.IndicatorMode(Mode::None);
        });
        m_settings = winrt::Windows::UI::ViewManagement::UISettings();
        m_settingsChanged = m_settings.AutoHideScrollBarsChanged([weak, queue](const auto&, const auto&)
        {
            // UISettings notifications are not guaranteed to run on the XAML thread.
            (void)queue.TryEnqueue([weak]()
            { if ( const auto self = weak.lock() ) self->PreferenceChanged(); });
        });
        m_settingsHooked = true;
        m_enter = winrt::box_value(Handler([weak](const auto&, const X::Input::PointerRoutedEventArgs& args)
        {
            if ( args.Pointer().PointerDeviceType() != winrt::Microsoft::UI::Input::PointerDeviceType::Touch )
                if ( const auto self = weak.lock() ) self->PointerOver(true);
        }));
        m_exit = winrt::box_value(Handler([weak](const auto&, const auto&)
            { if ( const auto self = weak.lock() ) self->PointerOver(false); }));
        m_bar.AddHandler(Element::PointerEnteredEvent(), m_enter, true);
        m_bar.AddHandler(Element::PointerExitedEvent(), m_exit, true);
        m_loaded = m_bar.Loaded([weak](const auto&, const auto&)
            { if ( const auto self = weak.lock() ) self->OnLoaded(); });
        m_loadedHooked = true;
        m_unloaded = m_bar.Unloaded([weak](const auto&, const auto&)
        {
            if ( const auto self = weak.lock(); self && self->m_live )
            {
                self->m_timer.Stop(); self->m_over = false; self->m_dragging = false;
                self->ClearThumbs();
            }
        });
        m_unloadedHooked = true;
        PreferenceChanged();
        if ( m_bar.IsLoaded() ) OnLoaded();
    }

    Bar m_bar{nullptr};
    winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer m_timer{nullptr};
    winrt::Windows::UI::ViewManagement::UISettings m_settings{nullptr};
    winrt::Windows::Foundation::IInspectable m_enter{nullptr}, m_exit{nullptr};
    winrt::event_token m_tick{}, m_settingsChanged{}, m_loaded{}, m_unloaded{};
    std::vector<ThumbHooks> m_thumbs;
    bool m_live = true, m_autoHide = true, m_over = false, m_dragging = false;
    bool m_settingsHooked = false, m_loadedHooked = false, m_unloadedHooked = false;
};

#endif
