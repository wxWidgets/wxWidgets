/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiribbontoolkit.cpp
// Purpose:     Real Toolkit Ribbon activation and shared-island composition
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

#include "wx/app.h"
#include "wx/button.h"
#include "wx/frame.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/window.h"
#include "wx/artprov.h"
#include "wx/intl.h"
#include "wx/sizer.h"
#include "wx/statbox.h"
#include "wx/scrolwin.h"
#include "wx/msw/private.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/page.h"
#include "wx/ribbon/panel.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/art.h"
#include "../../src/ribbon/ribboncontainerswinui.h"
#include "../../src/ribbon/ribbonflyoutwinui.h"
#include "../../src/ribbon/ribbontoolkitruntime.h"
#include "wx/winui/private/tlwhost.h"
#include "host-focus-test-access.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace
{
namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = MUX::Controls;
namespace Labs = winrt::XamlToolkit::Labs::WinUI;


std::string DescribeToolkitFailure(const wxRibbonToolkitFailure& failure)
{
    std::ostringstream text;
    text << "Toolkit stage=" << failure.stage
         << " HRESULT=0x" << std::hex
         << static_cast<unsigned long>(failure.code.value) << std::dec
         << " loaded=" << failure.loadedModules
         << " pinned=" << failure.pinnedModules
         << " detail=" << failure.detail.ToStdString(wxConvUTF8);
    return text.str();
}

std::shared_ptr<wxRibbonToolkitRuntime> AcquireToolkit()
{
    std::shared_ptr<wxRibbonToolkitRuntime> runtime;
    wxRibbonToolkitFailure failure;
    const bool acquired = wxRibbonToolkitRuntime::TryAcquire(runtime, &failure);
    INFO(DescribeToolkitFailure(failure));
    REQUIRE(acquired);
    REQUIRE(runtime);
    CHECK(failure.code.value == S_OK);
    CHECK(failure.loadedModules == 8);
    CHECK(failure.pinnedModules == 8);
    return runtime;
}

template <typename Predicate>
bool WaitForToolkitDispatch(Predicate predicate)
{
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp )
        return false;

    // Template realization and Loaded require actual native dispatcher
    // progress. Do not replace this with wxYield plus a sleeping poll.
    constexpr long timeoutMs = 500;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( predicate() )
            return true;
        if ( elapsed.Time() >= timeoutMs )
            return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop )
            return false;
        if ( predicate() )
            return true;
        const long remaining = timeoutMs - elapsed.Time();
        if ( remaining <= 0 ||
             loop->DispatchTimeout(
                 static_cast<unsigned long>(std::min(5L, remaining))) == 0 )
        {
            return false;
        }
    }
}

bool IsInToolkitSubtree(MUX::DependencyObject node,
                       const MUX::DependencyObject& root)
{
    while ( node )
    {
        if ( node == root )
            return true;
        node = MUX::Media::VisualTreeHelper::GetParent(node);
    }
    return false;
}

MUXC::Control FindToolkitCommand(const MUX::DependencyObject& node,
                                 const wchar_t* name)
{
    if ( !node )
        return nullptr;
    const auto control = node.try_as<MUXC::Control>();
    if ( control && MUX::Automation::AutomationProperties::GetName(control) == name )
        return control;
    const int count = MUX::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i < count; ++i )
    {
        const auto found = FindToolkitCommand(
            MUX::Media::VisualTreeHelper::GetChild(node, i), name);
        if ( found )
            return found;
    }
    return nullptr;
}

template <typename Provider>
Provider GetToolkitPattern(const MUX::FrameworkElement& element,
                           MUX::Automation::Peers::PatternInterface pattern)
{
    const auto peer = MUX::Automation::Peers::FrameworkElementAutomationPeer::
        CreatePeerForElement(element);
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

MUXC::Button FindRibbonChromeButton(const MUX::DependencyObject& node,
                                   const wchar_t* id)
{
    if ( !node ) return nullptr;
    if ( const auto button = node.try_as<MUXC::Button>(); button &&
         MUX::Automation::AutomationProperties::GetAutomationId(button) == id )
        return button;
    // Footer controls exist before they are Loaded and while both style bits
    // are off. Inspect the real native panel collection, not only its current
    // realized visual children, so hidden controls retain their identities.
    if ( const auto panel = node.try_as<MUXC::Panel>() )
    {
        for ( const auto& child : panel.Children() )
            if ( const auto found = FindRibbonChromeButton(child, id) ) return found;
        // Panel.Children is already the complete child collection. Walking
        // the same children again through VisualTreeHelper duplicates every
        // nested panel subtree and can starve native dispatcher progress.
        return nullptr;
    }
    else if ( const auto content = node.try_as<MUXC::ContentControl>() )
    {
        if ( const auto found = FindRibbonChromeButton(
                content.Content().try_as<MUX::DependencyObject>(), id) ) return found;
    }
    for ( int i = 0, count = MUX::Media::VisualTreeHelper::GetChildrenCount(node);
          i != count; ++i )
        if ( const auto found = FindRibbonChromeButton(
                MUX::Media::VisualTreeHelper::GetChild(node, i), id) ) return found;
    return nullptr;
}

struct RibbonChromeEvent
{
    wxEventType type;
    int id;
    wxObject* object;
    wxRibbonPage* page;
    wxRibbonDisplayMode mode;
    bool shown;
};

struct RibbonChromeFixture
{
    static constexpr int BarId = wxID_HIGHEST + 5470;
    static constexpr long ChromeFlags =
        wxRIBBON_BAR_SHOW_HELP_BUTTON | wxRIBBON_BAR_SHOW_TOGGLE_BUTTON;

    ~RibbonChromeFixture()
    {
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(long chrome = ChromeFlags)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "Ribbon native chrome",
                            wxDefaultPosition, wxSize(900, 300));
        bar = new wxRibbonBar(frame.get(), BarId, wxPoint(0, 0), wxSize(860, 240),
            (wxRIBBON_BAR_DEFAULT_STYLE & ~ChromeFlags) | chrome);
        auto* const page = new wxRibbonPage(bar.get(), wxID_ANY, "Home");
        new wxRibbonPanel(page, wxID_ANY, "Commands");
        const auto record = [live = bar, log = events](wxRibbonBarEvent& event)
        {
            if ( live ) log->push_back({event.GetEventType(), event.GetId(),
                event.GetEventObject(), event.GetPage(), live->GetDisplayMode(),
                live->ArePanelsShown()});
        };
        bar->Bind(wxEVT_RIBBONBAR_HELP_CLICK, record);
        bar->Bind(wxEVT_RIBBONBAR_TOGGLED, record);
        REQUIRE(bar->Realize());
        CHECK(events->empty());
        frame->Show();
        Ready();
    }

    MUXC::Grid Root() const
    {
        const auto host = bar ? wxWinUITopLevelHost::FindSlotOwner(bar.get()) : nullptr;
        const auto slot = host ? host->FindSlot(bar.get()) : nullptr;
        return slot ? slot->GetContent().try_as<MUXC::Grid>() : nullptr;
    }

    MUXC::TabView Tabs() const
    {
        const auto root = Root();
        return root && root.Children().Size() > 0
            ? root.Children().GetAt(0).try_as<MUXC::TabView>() : nullptr;
    }

    MUXC::Button Button(bool help) const
    {
        const auto tabs = Tabs();
        return tabs ? FindRibbonChromeButton(
            tabs.TabStripFooter().try_as<MUX::DependencyObject>(),
            help ? L"wxRibbonHelpButton" : L"wxRibbonToggleButton") : nullptr;
    }

    void Ready()
    {
        REQUIRE(WaitForToolkitDispatch([&]()
        {
            const auto host = bar ? wxWinUITopLevelHost::FindSlotOwner(bar.get()) : nullptr;
            if ( host ) host->FlushSync();
            const auto tabs = Tabs();
            if ( !tabs || !tabs.IsLoaded() ) return false;
            for ( bool help : {false, true} )
            {
                const auto button = Button(help);
                if ( !button ) return false;
                if ( button.Visibility() == MUX::Visibility::Visible &&
                     (!button.IsLoaded() || button.ActualWidth() <= 0 ||
                      button.ActualHeight() <= 0) ) return false;
            }
            return true;
        }));
    }

    void SetChrome(long chrome)
    {
        bar->SetWindowStyleFlag((bar->GetWindowStyleFlag() & ~ChromeFlags) | chrome);
        Ready();
    }

    void CheckEvent(size_t count, wxEventType type, wxRibbonDisplayMode mode)
    {
        REQUIRE(WaitForToolkitDispatch([&]() { return events->size() >= count; }));
        REQUIRE(events->size() == count);
        const auto& event = events->back();
        CHECK(event.type == type);
        CHECK(event.id == BarId);
        CHECK(event.object == bar.get());
        CHECK(event.page == nullptr);
        CHECK(event.mode == mode);
        CHECK(event.shown == (mode != wxRIBBON_BAR_MINIMIZED));
        CHECK(bar->GetDisplayMode() == mode);
        CHECK(bar->GetActivePage() == 0);
        const auto pages = Root().Children().GetAt(1);
        CHECK(pages.Visibility() == (mode == wxRIBBON_BAR_MINIMIZED
              ? MUX::Visibility::Collapsed : MUX::Visibility::Visible));
    }

    wxWeakRef<wxFrame> frame;
    wxWeakRef<wxRibbonBar> bar;
    std::shared_ptr<std::vector<RibbonChromeEvent>> events =
        std::make_shared<std::vector<RibbonChromeEvent>>();
};

void InvokeUnavailableRibbonChrome(
    const MUX::Automation::Provider::IInvokeProvider& provider)
{
    try { provider.Invoke(); }
    catch ( const winrt::hresult_error& ) { }
    auto* const loop = wxEventLoopBase::GetActive();
    REQUIRE(loop);
    for ( unsigned turn = 0; turn != 3; ++turn )
    {
        wxTheApp->ProcessPendingEvents();
        REQUIRE(wxEventLoopBase::GetActive() == loop);
        REQUIRE(loop->DispatchTimeout(5) != 0);
    }
    wxTheApp->ProcessPendingEvents();
}

MUX::FrameworkElement FindRibbonImage(const MUX::DependencyObject& node,
                                     const MUXC::ImageIconSource& source)
{
    if ( !node || !source ) return nullptr;
    if ( const auto icon = node.try_as<MUXC::ImageIcon>();
         icon && icon.Source() == source.ImageSource() ) return icon;
    for ( int i = 0, count = MUX::Media::VisualTreeHelper::GetChildrenCount(node);
          i != count; ++i )
        if ( const auto found = FindRibbonImage(
                MUX::Media::VisualTreeHelper::GetChild(node, i), source) ) return found;
    return nullptr;
}

void DescribeRibbonImages(std::ostream& text, const MUX::DependencyObject& node,
                          const MUXC::ImageIconSource& source, unsigned depth = 0)
{
    if ( !node || depth > 12 ) return;
    text << std::string(depth, ' ') << winrt::to_string(winrt::get_class_name(node));
    if ( const auto element = node.try_as<MUXC::IconSourceElement>() )
        text << " source=" << bool(element.IconSource()) << " same="
             << (element.IconSource() == source) << " type="
             << (element.IconSource() ? winrt::to_string(winrt::get_class_name(element.IconSource())) : "null");
    if ( const auto icon = node.try_as<MUXC::ImageIcon>() )
        text << " source=" << bool(icon.Source()) << " same="
             << (icon.Source() == source.ImageSource());
    if ( const auto image = node.try_as<MUXC::Image>() )
        text << " source=" << bool(image.Source()) << " same="
             << (image.Source() == source.ImageSource());
    text << '\n';
    for ( int i = 0, count = MUX::Media::VisualTreeHelper::GetChildrenCount(node);
          i != count; ++i )
        DescribeRibbonImages(text, MUX::Media::VisualTreeHelper::GetChild(node, i),
                             source, depth + 1);
}

void CheckRibbonImagePixels(const MUXC::ImageIconSource& source,
                           const wxBitmapBundle& bundle, wxWindow* owner)
{
    REQUIRE(source);
    const auto bitmap = bundle.GetBitmapFor(owner);
    REQUIRE(bitmap.IsOk());
    const auto image = source.ImageSource().try_as<MUX::Media::Imaging::BitmapSource>();
    REQUIRE(image);
    CHECK(image.PixelWidth() == bitmap.GetWidth());
    CHECK(image.PixelHeight() == bitmap.GetHeight());
}

struct RibbonIconFixture
{
    static constexpr long PageFlags =
        wxRIBBON_BAR_SHOW_PAGE_LABELS | wxRIBBON_BAR_SHOW_PAGE_ICONS;

    ~RibbonIconFixture()
    {
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create()
    {
        const auto bitmap16 = wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(16, 16));
        const auto bitmap32 = wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(32, 32));
        REQUIRE(bitmap16.IsOk());
        REQUIRE(bitmap32.IsOk());
        const auto bundle = wxBitmapBundle::FromBitmaps(bitmap16, bitmap32);
        REQUIRE(bundle.IsOk());
        frame = new wxFrame(nullptr, wxID_ANY, "Ribbon native images",
                            wxDefaultPosition, wxSize(1000, 320));
        bar = new wxRibbonBar(frame.get(), wxID_ANY, wxPoint(0, 0), wxSize(960, 240),
                             wxRIBBON_BAR_DEFAULT_STYLE | PageFlags);
        page = new wxRibbonPage(bar.get(), wxID_ANY, "Image page", bundle);
        panel = new wxRibbonPanel(page.get(), wxID_ANY, "Image commands", bundle);
        auto* const commands = new wxRibbonButtonBar(panel.get());
        for ( int i = 0; i != 4; ++i )
            REQUIRE(commands->AddButton(wxID_HIGHEST + 5520 + i,
                wxString::Format("Image command %d", i), bitmap32));
        other = new wxRibbonPage(bar.get(), wxID_ANY, "No image");
        plainPanel = new wxRibbonPanel(page.get(), wxID_ANY, "No image commands");
        REQUIRE(bar->Realize());
        frame->Show();
        Ready();
    }

    MUX::UIElement Presentation(wxWindow* owner) const
    {
        const auto host = owner ? wxWinUITopLevelHost::FindSlotOwner(owner) : nullptr;
        const auto slot = host ? host->FindSlot(owner) : nullptr;
        return slot ? slot->GetPresentationRoot() : nullptr;
    }

    MUXC::TabView Tabs() const
    {
        const auto host = bar ? wxWinUITopLevelHost::FindSlotOwner(bar.get()) : nullptr;
        const auto slot = host ? host->FindSlot(bar.get()) : nullptr;
        const auto root = slot ? slot->GetContent().try_as<MUXC::Grid>() : nullptr;
        return root && root.Children().Size() > 0
            ? root.Children().GetAt(0).try_as<MUXC::TabView>() : nullptr;
    }

    void Ready()
    {
        REQUIRE(WaitForToolkitDispatch([&]()
        {
            const auto host = bar ? wxWinUITopLevelHost::FindSlotOwner(bar.get()) : nullptr;
            if ( host ) host->FlushSync();
            const auto tabs = Tabs();
            const auto group = Presentation(panel.get()).try_as<Labs::RibbonCollapsibleGroup>();
            return tabs && tabs.IsLoaded() && tabs.TabItems().Size() == 2 &&
                   tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>().IsLoaded() &&
                   group && group.IsLoaded() && group.ActualWidth() > 0;
        }));
    }

    wxWeakRef<wxFrame> frame;
    wxWeakRef<wxRibbonBar> bar;
    wxWeakRef<wxRibbonPage> page, other;
    wxWeakRef<wxRibbonPanel> panel, plainPanel;
};

MUXC::TextBlock FindRibbonTabLabel(const MUX::DependencyObject& node,
                                  const winrt::hstring& label)
{
    if ( !node ) return nullptr;
    if ( const auto text = node.try_as<MUXC::TextBlock>(); text && text.Text() == label )
        return text;
    const int count = MUX::Media::VisualTreeHelper::GetChildrenCount(node);
    for ( int i = 0; i != count; ++i )
        if ( const auto text = FindRibbonTabLabel(
                MUX::Media::VisualTreeHelper::GetChild(node, i), label) )
            return text;
    return nullptr;
}

template <typename NativeText>
void CheckRibbonTabFont(const NativeText& text, const wxFont& font)
{
    CHECK(text.FontFamily().Source() == winrt::hstring(font.GetFaceName().wc_str()));
    CHECK(text.FontSize() == Approx(font.GetFractionalPointSize() * 96.0 / 72.0));
    CHECK(text.FontWeight().Weight == font.GetNumericWeight());
    CHECK(text.FontStyle() == (font.GetStyle() == wxFONTSTYLE_ITALIC
        ? winrt::Windows::UI::Text::FontStyle::Italic
        : winrt::Windows::UI::Text::FontStyle::Normal));
}

class ReentrantRibbonTabFontArt final : public wxRibbonMSWArtProvider
{
public:
    wxFont GetFont(int id) const override
    {
        const wxFont font = wxRibbonMSWArtProvider::GetFont(id);
        if ( id == wxRIBBON_ART_TAB_LABEL_FONT && onRead )
        {
            auto callback = std::move(onRead);
            callback();
        }
        return font;
    }
    mutable std::function<void ()> onRead;
};

struct RibbonExtensionEvent
{
    int id;
    wxObject* object;
    wxRibbonPanel* panel;
};

struct RibbonPageTransition
{
    wxEventType type;
    int id;
    wxObject* object;
    wxRibbonPage* page;
    wxRibbonDisplayMode mode;
    int activePage;
};

bool RibbonNativeElementIsVisible(const MUX::FrameworkElement& element)
{
    if ( !element || !element.IsLoaded() ||
         element.ActualWidth() <= 0 || element.ActualHeight() <= 0 ) return false;
    // Visibility is not inherited as a property value. Inspect the actual
    // native ancestors so a collapsed carrier counts even if its leaf is
    // still locally Visible and retains its previous arranged size.
    for ( MUX::DependencyObject node = element; node;
          node = MUX::Media::VisualTreeHelper::GetParent(node) )
        if ( const auto visual = node.try_as<MUX::UIElement>(); visual &&
             visual.Visibility() != MUX::Visibility::Visible ) return false;
    return true;
}

struct RibbonExtensionFixture
{
    static constexpr int PanelId = wxID_HIGHEST + 5590;

    ~RibbonExtensionFixture()
    {
        if ( panel && panel->GetExpandedPanel() ) panel->HideExpanded();
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(bool noAutoMinimise = false, bool barAllowsExtension = true)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "Ribbon panel extensions",
                            wxDefaultPosition, wxSize(1100, 340));
        bar = new wxRibbonBar(frame.get(), wxID_ANY, wxPoint(0, 0), wxSize(1060, 260),
            (wxRIBBON_BAR_DEFAULT_STYLE & ~wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS) |
            (barAllowsExtension ? wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS : 0));
        page = new wxRibbonPage(bar.get(), wxID_ANY, "Extensions");
        panel = new wxRibbonPanel(page.get(), PanelId, "Extension commands",
            wxBitmapBundle(), wxDefaultPosition, wxDefaultSize,
            wxRIBBON_PANEL_EXT_BUTTON |
                (noAutoMinimise ? wxRIBBON_PANEL_NO_AUTO_MINIMISE : 0));
        commands = new wxRibbonButtonBar(panel.get());
        const auto bitmap = wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(32, 32));
        REQUIRE(bitmap.IsOk());
        for ( int i = 0; i != 4; ++i )
            REQUIRE(commands->AddButton(PanelId + 1 + i,
                wxString::Format("Extension command %d", i), bitmap));
        auto* const sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(commands.get());
        panel->SetSizer(sizer);
        plainPanel = new wxRibbonPanel(page.get(), PanelId + 10, "No extension",
            wxBitmapBundle(), wxDefaultPosition, wxDefaultSize,
            wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED,
            [log = events](wxRibbonPanelEvent& event)
            {
                log->push_back({event.GetId(), event.GetEventObject(), event.GetPanel()});
            });
        REQUIRE(bar->Realize());
        frame->Show();
        Ready();
    }

    MUX::UIElement Presentation(wxWindow* owner) const
    {
        const auto host = owner ? wxWinUITopLevelHost::FindSlotOwner(owner) : nullptr;
        const auto slot = host ? host->FindSlot(owner) : nullptr;
        return slot ? slot->GetPresentationRoot() : nullptr;
    }

    MUX::UIElement CommandRoot() const
    {
        const auto host = commands ? wxWinUITopLevelHost::FindSlotOwner(commands.get()) : nullptr;
        const auto slot = host ? host->FindSlot(commands.get()) : nullptr;
        return slot ? slot->GetContent() : nullptr;
    }

    MUXC::Button InlineButton() const
    {
        return FindRibbonChromeButton(Presentation(panel.get()), L"wxRibbonPanelExtension");
    }

    void Ready()
    {
        const bool ready = WaitForToolkitDispatch([&]()
        {
            const auto host = bar ? wxWinUITopLevelHost::FindSlotOwner(bar.get()) : nullptr;
            if ( host ) host->FlushSync();
            const auto command = FindToolkitCommand(CommandRoot(), L"Extension command 0");
            return command && command.IsLoaded() && command.ActualWidth() > 0 &&
                   command.ActualHeight() > 0;
        });
        const auto root = CommandRoot();
        const auto group = Presentation(panel.get()).try_as<Labs::RibbonCollapsibleGroup>();
        if ( !ready && group )
        {
            const auto control = group.as<MUXC::IControlProtected>();
            const auto label = control.GetTemplateChild(L"Label").try_as<MUX::FrameworkElement>();
            const auto popupLabel = control.GetTemplateChild(L"FlyoutLabel").try_as<MUX::FrameworkElement>();
            INFO("group loaded=" << group.IsLoaded() << " child count="
                 << MUX::Media::VisualTreeHelper::GetChildrenCount(group)
                 << " inline label=" << bool(label) << " parent=" << (label && bool(label.Parent()))
                 << " popup label=" << bool(popupLabel) << " parent=" << (popupLabel && bool(popupLabel.Parent()))
                 << " command root=" << bool(root));
            REQUIRE(ready);
        }
        REQUIRE(ready);
    }

    void ReadyExtension()
    {
        REQUIRE(WaitForToolkitDispatch([&]()
        {
            const auto button = InlineButton();
            return button && button.Visibility() == MUX::Visibility::Visible &&
                   button.IsLoaded() && button.ActualWidth() > 0 && button.ActualHeight() > 0;
        }));
    }

    void SetBarFlag(bool enabled)
    {
        bar->SetWindowStyleFlag(
            (bar->GetWindowStyleFlag() & ~wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS) |
            (enabled ? wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS : 0));
    }

    wxWeakRef<wxFrame> frame;
    wxWeakRef<wxRibbonBar> bar;
    wxWeakRef<wxRibbonPage> page;
    wxWeakRef<wxRibbonPanel> panel, plainPanel;
    wxWeakRef<wxRibbonButtonBar> commands;
    std::shared_ptr<std::vector<RibbonExtensionEvent>> events =
        std::make_shared<std::vector<RibbonExtensionEvent>>();
};
} // namespace

TEST_CASE("RibbonToolkit::PublicReparentPreservesCompositeTreeAndEvents",
          "[winui-ribbon-toolkit][winui-composite-migration]")
{
    namespace Patterns = MUX::Automation::Peers;
    namespace Providers = MUX::Automation::Provider;
    constexpr int NormalId = wxID_HIGHEST + 5170;
    constexpr int ToggleId = NormalId + 1;
    struct CommandEvent
    {
        int id;
        wxObject* object;
        wxRibbonButtonBar* bar;
        wxRibbonButtonBarButtonBase* item;
        bool checked;
    };
    std::vector<CommandEvent> events;
    int changing = 0, changed = 0;
    wxWeakRef<wxFrame> source(new wxFrame(nullptr, wxID_ANY, "Ribbon source",
        wxPoint(-32000, -32000), wxSize(1440, 360)));
    wxWeakRef<wxFrame> destination(new wxFrame(nullptr, wxID_ANY, "Ribbon destination",
        wxPoint(-31000, -32000), wxSize(1440, 360)));
    const auto cleanup = wxMakeGuard([&]()
    {
        if ( source ) source->Destroy();
        if ( destination ) destination->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    });
    auto* const firstParent = new wxWindow(source.get(), wxID_ANY,
        wxPoint(5, 5), wxSize(680, 280));
    auto* const secondParent = new wxWindow(source.get(), wxID_ANY,
        wxPoint(710, 5), wxSize(680, 280));
    auto* const destinationParent = new wxWindow(destination.get(), wxID_ANY,
        wxPoint(5, 5), wxSize(680, 280));
    auto* const bar = new wxRibbonBar(firstParent, wxID_ANY,
        wxPoint(0, 0), wxSize(660, 240));
    auto* const home = new wxRibbonPage(bar, wxID_ANY, "Home");
    auto* const panel = new wxRibbonPanel(home, wxID_ANY, "Migration commands");
    auto* const commands = new wxRibbonButtonBar(panel);
    const auto bitmap = wxArtProvider::GetBitmap(
        wxART_NEW, wxART_OTHER, wxSize(32, 32));
    auto* const normalItem = commands->AddButton(NormalId, "Migrate normal", bitmap);
    auto* const toggleItem = commands->AddToggleButton(ToggleId, "Migrate toggle", bitmap);
    REQUIRE(normalItem);
    REQUIRE(toggleItem);
    auto* const other = new wxRibbonPage(bar, wxID_ANY, "Other");
    const wxWeakRef<wxRibbonBar> liveBar(bar);
    const wxWeakRef<wxRibbonButtonBar> liveCommands(commands);
    commands->Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, [&](wxRibbonButtonBarEvent& event)
    {
        events.push_back({event.GetId(), event.GetEventObject(), event.GetBar(),
                          event.GetButton(), event.IsChecked()});
    });
    bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGING, [&, bar](wxRibbonBarEvent& event)
    {
        ++changing;
        CHECK(event.GetEventObject() == bar);
    });
    bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGED, [&, bar](wxRibbonBarEvent& event)
    {
        ++changed;
        CHECK(event.GetEventObject() == bar);
    });
    commands->ToggleButton(ToggleId, true);
    REQUIRE(bar->SetActivePage(size_t(0)));
    REQUIRE(bar->Realize());
    source->ShowWithoutActivating();
    destination->ShowWithoutActivating();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(commands);
        if ( !host ) return false;
        host->FlushSync();
        const auto* slot = host->FindSlot(commands);
        const auto content = slot ? slot->GetContent() : nullptr;
        const auto normal = FindToolkitCommand(content, L"Migrate normal");
        return normal && normal.IsLoaded() && normal.ActualWidth() > 0 &&
               normal.ActualHeight() > 0;
    }));
    auto* const originalHost = wxWinUITopLevelHost::FindSlotOwner(bar);
    REQUIRE(originalHost);
    const std::array<wxWindow*, 5> owners = {bar, home, other, panel, commands};
    std::array<MUX::UIElement, 5> roots = {nullptr, nullptr, nullptr, nullptr, nullptr};
    for ( size_t i = 0; i < owners.size(); ++i )
    {
        auto* const slot = originalHost->FindSlot(owners[i]);
        REQUIRE(slot);
        roots[i] = slot->GetContent();
        REQUIRE(roots[i]);
    }
    const auto nativePage = roots[1].as<Labs::Ribbon>();
    const auto nativeGroup = roots[3].as<Labs::RibbonCollapsibleGroup>();
    const auto tabs = roots[0].as<MUXC::Grid>().Children().GetAt(0).as<MUXC::TabView>();
    REQUIRE(tabs.TabItems().Size() == 2);
    const auto firstTab = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto secondTab = tabs.TabItems().GetAt(1).as<MUXC::TabViewItem>();
    const auto normal = FindToolkitCommand(roots[4], L"Migrate normal");
    const auto toggle = FindToolkitCommand(roots[4], L"Migrate toggle");
    REQUIRE(normal);
    REQUIRE(toggle);
    const auto invoke = GetToolkitPattern<Providers::IInvokeProvider>(
        normal, Patterns::PatternInterface::Invoke);
    const auto toggleProvider = GetToolkitPattern<Providers::IToggleProvider>(
        toggle, Patterns::PatternInterface::Toggle);
    const auto selectHome = GetToolkitPattern<Providers::ISelectionItemProvider>(
        firstTab, Patterns::PatternInterface::SelectionItem);
    const auto selectOther = GetToolkitPattern<Providers::ISelectionItemProvider>(
        secondTab, Patterns::PatternInterface::SelectionItem);
    REQUIRE(invoke);
    REQUIRE(toggleProvider);
    REQUIRE(selectHome);
    REQUIRE(selectOther);
    REQUIRE(events.empty());
    REQUIRE(changing == 0);
    REQUIRE(changed == 0);

    const auto checkTree = [&](wxFrame* frame, wxWindow* parent)
    {
        auto* const host = wxWinUITopLevelHost::FindForTLW(frame);
        REQUIRE(host);
        CHECK(bar->GetParent() == parent);
        CHECK(bar->GetPageCount() == 2);
        CHECK(bar->GetPage(0) == home);
        CHECK(bar->GetPage(1) == other);
        CHECK(bar->GetActivePage() == 0);
        CHECK(commands->GetButtonCount() == 2);
        CHECK(commands->GetItemById(NormalId) == normalItem);
        CHECK(commands->GetItemById(ToggleId) == toggleItem);
        for ( size_t i = 0; i < owners.size(); ++i )
        {
            CHECK(wxWinUITopLevelHost::FindSlotOwner(owners[i]) == host);
            auto* const slot = host->FindSlot(owners[i]);
            REQUIRE(slot);
            CHECK(slot->GetContent() == roots[i]);
            CHECK(roots[i].XamlRoot() == host->GetXamlRoot());
            CHECK(IsInToolkitSubtree(roots[i], host->Root()));
        }
        CHECK(nativePage.Items().Size() == 1);
        CHECK(nativePage.Items().GetAt(0) == nativeGroup);
        CHECK(host->FindSlot(panel)->GetPresentationRoot() == nativeGroup);
        CHECK(IsInToolkitSubtree(nativeGroup, nativePage));
        CHECK(IsInToolkitSubtree(roots[4], nativeGroup.Content()));
        CHECK(FindToolkitCommand(roots[4], L"Migrate normal") == normal);
        CHECK(FindToolkitCommand(roots[4], L"Migrate toggle") == toggle);
        CHECK(tabs.TabItems().GetAt(0) == firstTab);
        CHECK(tabs.TabItems().GetAt(1) == secondTab);
        CHECK(tabs.SelectedItem() == firstTab);
        CHECK(toggleProvider.ToggleState() == MUX::Automation::ToggleState::On);
        for ( wxFrame* candidate : {source.get(), destination.get()} )
        {
            if ( !candidate || candidate == frame ) continue;
            if ( auto* otherHost = wxWinUITopLevelHost::FindForTLW(candidate) )
                for ( auto* owner : owners ) CHECK_FALSE(otherHost->FindSlot(owner));
        }
    };
    const auto exerciseEvents = [&]()
    {
        const size_t start = events.size();
        invoke.Invoke();
        REQUIRE(WaitForToolkitDispatch([&]() { return events.size() >= start + 1; }));
        REQUIRE(events.size() == start + 1);
        CHECK(events.back().id == NormalId);
        CHECK(events.back().item == normalItem);
        toggleProvider.Toggle();
        REQUIRE(WaitForToolkitDispatch([&]() { return events.size() >= start + 2; }));
        REQUIRE(events.size() == start + 2);
        CHECK(events.back().id == ToggleId);
        CHECK(events.back().item == toggleItem);
        CHECK_FALSE(events.back().checked);
        toggleProvider.Toggle();
        REQUIRE(WaitForToolkitDispatch([&]() { return events.size() >= start + 3; }));
        REQUIRE(events.size() == start + 3);
        CHECK(events.back().checked);
        for ( size_t i = start; i < events.size(); ++i )
        {
            CHECK(events[i].object == commands);
            CHECK(events[i].bar == commands);
        }
        const int beforeChanging = changing, beforeChanged = changed;
        selectOther.Select();
        REQUIRE(WaitForToolkitDispatch([&]() { return bar->GetActivePage() == 1; }));
        CHECK(changing == beforeChanging + 1);
        CHECK(changed == beforeChanged + 1);
        selectHome.Select();
        REQUIRE(WaitForToolkitDispatch([&]() { return bar->GetActivePage() == 0; }));
        CHECK(changing == beforeChanging + 2);
        CHECK(changed == beforeChanged + 2);
        CHECK(events.size() == start + 3);
    };
    const auto moveTo = [&](wxWindow* parent, wxFrame* frame)
    {
        const size_t beforeCommands = events.size();
        const int beforeChanging = changing, beforeChanged = changed;
        REQUIRE(bar->Reparent(parent));
        // No recreation or Realize() workaround: public Reparent must migrate
        // the already-live cohort, including the originally acquired providers.
        REQUIRE(WaitForToolkitDispatch([&]()
        {
            auto* const host = wxWinUITopLevelHost::FindSlotOwner(bar);
            if ( !host || host->GetTLW() != frame ) return false;
            host->FlushSync();
            for ( size_t i = 0; i < owners.size(); ++i )
            {
                const auto* slot = host->FindSlot(owners[i]);
                if ( !slot || slot->GetContent() != roots[i] ||
                     roots[i].XamlRoot() != host->GetXamlRoot() ) return false;
            }
            return normal.IsLoaded() && normal.ActualWidth() > 0;
        }));
        checkTree(frame, parent);
        CHECK(events.size() == beforeCommands);
        CHECK(changing == beforeChanging);
        CHECK(changed == beforeChanged);
        exerciseEvents();
    };
    checkTree(source.get(), firstParent);
    exerciseEvents();

    SECTION("between subparents in the same top-level window")
    {
        const auto island = roots[0].XamlRoot();
        moveTo(secondParent, source.get());
        CHECK(wxWinUITopLevelHost::FindSlotOwner(bar) == originalHost);
        CHECK(roots[0].XamlRoot() == island);
        moveTo(firstParent, source.get());
        CHECK(roots[0].XamlRoot() == island);
    }
    SECTION("between top-level windows and back")
    {
        const auto initialIsland = roots[0].XamlRoot();
        moveTo(destinationParent, destination.get());
        CHECK(roots[0].XamlRoot() != initialIsland);
        const auto movedIsland = roots[0].XamlRoot();
        moveTo(firstParent, source.get());
        CHECK(roots[0].XamlRoot() != movedIsland);
    }
    SECTION("destroying the source window after a committed move")
    {
        moveTo(destinationParent, destination.get());
        const auto destinationIsland = roots[0].XamlRoot();
        REQUIRE(source->Destroy());
        REQUIRE(WaitForToolkitDispatch([&]()
        {
            // DispatchTimeout delivers native/XAML work, but deferred wx
            // object deletion is an idle duty of the normal main loop.
            wxTheApp->ProcessIdle();
            return !source;
        }));
        REQUIRE(liveBar.get() == bar);
        REQUIRE(liveCommands.get() == commands);
        checkTree(destination.get(), destinationParent);
        CHECK(roots[0].XamlRoot() == destinationIsland);
        exerciseEvents();
    }
}

TEST_CASE("RibbonToolkit::PublicControlsUseNativeContainers",
          "[winui-ribbon-toolkit]")
{
    int changing = 0, changed = 0;
    bool veto = false;
    wxFrame* const frame = new wxFrame(nullptr, wxID_ANY, "native wxRibbon",
        wxPoint(-32000, -32000), wxSize(720, 300));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
    });
    auto* const bar = new wxRibbonBar(frame, wxID_ANY);
    REQUIRE(wxRibbonContainerWinUI::Native(bar));
    auto* const page = new wxRibbonPage(bar, wxID_ANY, "Home");
    REQUIRE(wxRibbonContainerWinUI::Native(page));
    auto* const panel = new wxRibbonPanel(page, wxID_ANY, "Commands");
    REQUIRE(wxRibbonContainerWinUI::Native(panel));
    auto* const commands = new wxRibbonButtonBar(panel);
    REQUIRE(commands->AddButton(101, "New",
        wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(32, 32))));
    auto* const second = new wxRibbonPage(bar, wxID_ANY, "Other");
    REQUIRE(wxRibbonContainerWinUI::Native(second));
    // Public realization succeeds before Show(): the command model is built,
    // while its first native slot waits for the real Group.Content template.
    REQUIRE(bar->Realize());
    frame->ShowWithoutActivating();
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(bar);
    REQUIRE(host);
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        host->FlushSync();
        const auto* slot = host->FindSlot(commands);
        return slot && slot->GetContent() &&
               slot->GetContent().as<MUX::FrameworkElement>().IsLoaded() &&
               slot->GetRectInTLW().width > 0;
    }));
    auto* const pageSlot = host->FindSlot(page);
    auto* const panelSlot = host->FindSlot(panel);
    auto* const commandSlot = host->FindSlot(commands);
    REQUIRE(pageSlot);
    REQUIRE(panelSlot);
    REQUIRE(commandSlot);
    const auto nativePage = pageSlot->GetContent().try_as<Labs::Ribbon>();
    const auto nativePanel = panelSlot->GetContent().try_as<Labs::RibbonCollapsibleGroup>();
    REQUIRE(nativePage);
    REQUIRE(nativePanel);
    REQUIRE(nativePage.Items().Size() == 1);
    CHECK(nativePage.Items().GetAt(0) == nativePanel);
    CHECK(panelSlot->GetPresentationRoot() == nativePanel);
    CHECK(nativePanel.Label() == L"Commands");
    CHECK(IsInToolkitSubtree(commandSlot->GetContent(), nativePanel.Content()));
    CHECK(nativePanel.XamlRoot() == nativePage.XamlRoot());
    CHECK(commandSlot->GetContent().XamlRoot() == nativePage.XamlRoot());
    const auto commandRootBeforeRealize = commandSlot->GetContent();
    const auto commandBeforeRealize =
        FindToolkitCommand(commandRootBeforeRealize, L"New");
    REQUIRE(commandBeforeRealize);
    REQUIRE(bar->Realize());
    REQUIRE(host->FindSlot(commands));
    CHECK(host->FindSlot(commands)->GetContent() == commandRootBeforeRealize);
    CHECK(FindToolkitCommand(host->FindSlot(commands)->GetContent(), L"New") ==
          commandBeforeRealize);

    const auto barRoot = host->FindSlot(bar)->GetContent().as<MUXC::Grid>();
    const auto tabs = barRoot.Children().GetAt(0).as<MUXC::TabView>();
    REQUIRE(tabs.TabItems().Size() == 2);
    bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGING, [&](wxRibbonBarEvent& event)
    {
        ++changing;
        if ( veto ) event.Veto();
    });
    bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGED, [&](wxRibbonBarEvent&) { ++changed; });
    REQUIRE(bar->SetActivePage(1));
    CHECK(changing == 0);
    CHECK(changed == 0);
    CHECK(tabs.SelectedItem() == tabs.TabItems().GetAt(1));
    veto = true;
    tabs.SelectedItem(tabs.TabItems().GetAt(0));
    CHECK(changing == 1);
    CHECK(changed == 0);
    CHECK(bar->GetActivePage() == 1);
    CHECK(tabs.SelectedItem() == tabs.TabItems().GetAt(1));
    veto = false;
    tabs.SelectedItem(tabs.TabItems().GetAt(0));
    CHECK(changing == 2);
    CHECK(changed == 1);
    CHECK(bar->GetActivePage() == 0);
    CHECK(host->FindSlot(panel)->GetPresentationRoot() == nativePanel);
}

#if wxUSE_STATBOX
TEST_CASE("RibbonToolkit::DefaultSizeInApplicationSizerShowsNativeCommands",
          "[winui-ribbon-toolkit][winui-ribbon-autosize]")
{
    const int layout = GENERATE(0, 1, 2);
    const bool nested = layout != 0;
    CAPTURE(layout);
    wxPoint position(-30000, -30000);
    wchar_t desktop[256]{};
    DWORD needed = 0;
    if ( ::GetUserObjectInformationW(::GetThreadDesktop(::GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) &&
         wxString(desktop).StartsWith("wxWinUITest-") ) position = wxPoint(80, 80);
    auto* const frame = new wxFrame(nullptr, wxID_ANY, "Ribbon application sizing",
                                    position, wxSize(800, 500));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
    });
    auto* const outer = new wxBoxSizer(wxVERTICAL);
    wxWindow* parent = frame;
    wxSizer* ribbonSizer = outer;
    wxScrolledWindow* scroller = nullptr;
    if ( nested )
    {
        scroller = new wxScrolledWindow(frame, wxID_ANY);
        scroller->SetScrollRate(0, 10);
        auto* const contents = new wxBoxSizer(wxVERTICAL);
        auto* const group = new wxStaticBoxSizer(wxVERTICAL, scroller, "wxRibbonBar");
        parent = group->GetStaticBox();
        ribbonSizer = group;
        contents->Add(group, 0, wxEXPAND | wxALL, 8);
        contents->AddStretchSpacer();
        scroller->SetSizer(contents);
        outer->Add(scroller, 1, wxEXPAND);
    }
    auto* const bar = new wxRibbonBar(parent, wxID_ANY);
    auto* const page = new wxRibbonPage(bar, wxID_ANY, "Home");
    auto* const panel = new wxRibbonPanel(page, wxID_ANY, "Actions");
    auto* const commands = new wxRibbonButtonBar(panel);
    REQUIRE(commands->AddButton(171, "Visible command",
        wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(32, 32))));
    REQUIRE(bar->Realize());
    const wxSize initialBest = bar->GetBestSize();
    const double nativeHeaderMinimum = winrt::unbox_value<double>(
        MUX::Application::Current().Resources().Lookup(
            winrt::box_value(winrt::hstring(L"TabViewItemMinHeight"))));
    REQUIRE(nativeHeaderMinimum > 0);
    CHECK(initialBest.y >= bar->FromDIP(
        static_cast<int>(std::ceil(nativeHeaderMinimum))));
    ribbonSizer->Add(bar, 0, wxEXPAND | wxALL, nested ? 8 : 0);
    if ( !nested ) outer->AddStretchSpacer();
    frame->SetSizer(outer);
    if ( layout == 2 ) scroller->Hide();
    frame->ShowWithoutActivating();
    if ( layout == 2 )
    {
        // Like a gallery page revealed after startup: native descendants
        // were first attached while their wx ancestor was hidden.
        wxTheApp->ProcessPendingEvents();
        wxYield();
        scroller->Show();
        frame->Layout();
    }
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(bar);
    REQUIRE(host);
    MUXC::Control command{nullptr};
    MUXC::Grid root{nullptr};
    MUXC::TabView tabs{nullptr};
    winrt::Windows::Foundation::Rect commandBounds{};
    const bool visible = WaitForToolkitDispatch([&]()
    {
        host->FlushSync();
        const auto* const barSlot = host->FindSlot(bar);
        const auto* const commandSlot = host->FindSlot(commands);
        root = barSlot ? barSlot->GetContent().try_as<MUXC::Grid>() : nullptr;
        tabs = root ? root.Children().GetAt(0).try_as<MUXC::TabView>() : nullptr;
        command = commandSlot ? FindToolkitCommand(commandSlot->GetContent(),
                                                    L"Visible command") : nullptr;
        if ( !RibbonNativeElementIsVisible(command) ||
             !RibbonNativeElementIsVisible(tabs) || !root ) return false;
        commandBounds = command.TransformToVisual(root).TransformBounds(
            {0, 0, static_cast<float>(command.ActualWidth()),
                   static_cast<float>(command.ActualHeight())});
        // Loaded alone is insufficient: an element can keep its old arranged
        // size below a zero-height sizer allocation or a collapsed carrier.
        return commandBounds.Width > 0 && commandBounds.Height > 0 &&
            commandBounds.X >= -0.5 && commandBounds.Y >= -0.5 &&
            commandBounds.X + commandBounds.Width <= root.ActualWidth() + 0.5 &&
            commandBounds.Y + commandBounds.Height <= root.ActualHeight() + 0.5 &&
            bar->GetSize().y > bar->FromDIP(static_cast<int>(tabs.ActualHeight()));
    });
    INFO("initial best=" << initialBest.x << 'x' << initialBest.y
         << " current best=" << bar->GetBestSize().x << 'x' << bar->GetBestSize().y
         << " wx=" << bar->GetSize().x << 'x' << bar->GetSize().y
         << " root=" << (root ? root.ActualWidth() : -1) << 'x'
         << (root ? root.ActualHeight() : -1)
         << " desired=" << (root ? root.DesiredSize().Width : -1) << 'x'
         << (root ? root.DesiredSize().Height : -1)
         << " tabs=" << (tabs ? tabs.ActualHeight() : -1)
         << " root loaded=" << (root && root.IsLoaded())
         << " tabs loaded=" << (tabs && tabs.IsLoaded())
         << " tab template=" << (tabs && bool(tabs.Template()))
         << " command=" << bool(command)
         << " command visible=" << RibbonNativeElementIsVisible(command));
    REQUIRE(visible);
    const auto originalRoot = root;
    const auto originalCommand = command;
    frame->SetClientSize(940, 560);
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return RibbonNativeElementIsVisible(originalCommand) &&
               bar->GetSize().x > 800 && root.ActualHeight() > tabs.ActualHeight();
    }));
    CHECK(host->FindSlot(bar)->GetContent() == originalRoot);
    CHECK(FindToolkitCommand(host->FindSlot(commands)->GetContent(),
                             L"Visible command") == originalCommand);
}
#endif // wxUSE_STATBOX

TEST_CASE("RibbonToolkit::RealApplicationRegistrationAndActivation",
          "[winui-ribbon-toolkit]")
{
    const auto application = MUX::Application::Current();
    REQUIRE(application);
    const auto runtime = AcquireToolkit();
    const auto metadata = application.as<MUX::Markup::IXamlMetadataProvider>();

    // These are the providers published by TryAcquire into wx's actual
    // Application, not a test Application, alias provider or metadata fallback.
    for ( const auto name : {L"XamlToolkit.Labs.WinUI.Ribbon",
                            L"XamlToolkit.Labs.WinUI.RibbonGroup",
                            L"XamlToolkit.Labs.WinUI.RibbonCollapsibleGroup"} )
    {
        const winrt::hstring expected(name);
        INFO(winrt::to_string(expected));
        const auto type = metadata.GetXamlType(expected);
        REQUIRE(type);
        CHECK(type.UnderlyingType().Name == expected);
        const auto byType = metadata.GetXamlType(type.UnderlyingType());
        REQUIRE(byType);
        CHECK(byType.UnderlyingType().Name == expected);
    }

    wxRibbonToolkitFailure failure;
    Labs::Ribbon ribbon{nullptr};
    Labs::RibbonGroup group{nullptr};
    Labs::RibbonCollapsibleGroup collapsible{nullptr};
    const bool ribbonCreated = runtime->TryCreateRibbon(ribbon, &failure);
    INFO(DescribeToolkitFailure(failure));
    REQUIRE(ribbonCreated);
    const bool groupCreated = runtime->TryCreateGroup(group, &failure);
    INFO(DescribeToolkitFailure(failure));
    REQUIRE(groupCreated);
    const bool collapsibleCreated = runtime->TryCreateCollapsibleGroup(
        collapsible, &failure);
    INFO(DescribeToolkitFailure(failure));
    REQUIRE(collapsibleCreated);
    CHECK(winrt::get_class_name(ribbon) == L"XamlToolkit.Labs.WinUI.Ribbon");
    CHECK(winrt::get_class_name(group) == L"XamlToolkit.Labs.WinUI.RibbonGroup");
    CHECK(winrt::get_class_name(collapsible) ==
          L"XamlToolkit.Labs.WinUI.RibbonCollapsibleGroup");
    CHECK(MUX::Application::Current() == application);

    std::shared_ptr<wxRibbonToolkitRuntime> reused;
    REQUIRE(wxRibbonToolkitRuntime::TryAcquire(reused, &failure));
    CHECK(std::string(failure.stage) == "reused");
    REQUIRE(reused);

    const auto previous = reused;
    CHECK_FALSE(wxRibbonToolkitRuntime::TryAcquire(
        reused, &failure, "relative-toolkit-payload"));
    CHECK(reused == previous);
    CHECK(failure.code.value == HRESULT_FROM_WIN32(ERROR_BAD_PATHNAME));
    CHECK(std::string(failure.stage) == "resolve-payload-directory");

    // Rejection must precede COM access and leave existing outputs intact.
    // No COM initialization or Toolkit factory fallback is installed here.
    wxRibbonToolkitFailure acquireFailure;
    wxRibbonToolkitFailure createFailure;
    bool acquiredOffThread = true;
    bool createdOffThread = true;
    const auto ribbonIdentity = winrt::get_abi(ribbon);
    std::thread worker([&]
    {
        acquiredOffThread = wxRibbonToolkitRuntime::TryAcquire(
            reused, &acquireFailure);
        createdOffThread = runtime->TryCreateRibbon(ribbon, &createFailure);
    });
    worker.join();
    CHECK_FALSE(acquiredOffThread);
    CHECK_FALSE(createdOffThread);
    CHECK(acquireFailure.code.value == RPC_E_WRONG_THREAD);
    CHECK(createFailure.code.value == RPC_E_WRONG_THREAD);
    CHECK(reused == previous);
    CHECK(winrt::get_abi(ribbon) == ribbonIdentity);
}

TEST_CASE("RibbonToolkit::RealGroupMountUsesSharedIslandSlot",
          "[winui-ribbon-toolkit][winui-composite]")
{
    const auto runtime = AcquireToolkit();
    wxRibbonToolkitFailure failure;
    Labs::Ribbon ribbon{nullptr};
    Labs::RibbonGroup group{nullptr};
    const bool ribbonCreated = runtime->TryCreateRibbon(ribbon, &failure);
    INFO(DescribeToolkitFailure(failure));
    REQUIRE(ribbonCreated);
    const bool groupCreated = runtime->TryCreateGroup(group, &failure);
    INFO(DescribeToolkitFailure(failure));
    REQUIRE(groupCreated);

    wxFrame * const frame = new wxFrame(
        nullptr, wxID_ANY, "toolkit-composite-ribbon",
        wxPoint(-32000, -32000), wxSize(640, 260));
    const auto cleanup = wxMakeGuard([frame]()
    {
        frame->Destroy();
        for ( int i = 0; i < 20; ++i )
        {
            wxYield();
            wxMilliSleep(5);
        }
    });
    wxWindow * const ribbonWindow = new wxWindow(frame, wxID_ANY);
    wxWindow * const commandsWindow = new wxWindow(ribbonWindow, wxID_ANY);
    ribbonWindow->SetSize(5, 5, 600, 200);
    commandsWindow->SetSize(0, 0, 160, 40);

    // Toolkit RibbonGroup derives from Control, not ContentControl. Publish
    // the mount through its real Content property and realize its template
    // before validating that the mount belongs to the parent's visual subtree.
    const MUXC::ContentControl mount;
    mount.MinWidth(160);
    mount.MinHeight(40);
    group.Label(L"Commands");
    group.Content(mount);
    ribbon.Items().Append(group);

    wxWinUIControlHost ribbonHost;
    wxWinUIControlHost commandsHost;
    REQUIRE(ribbonHost.Initialize(ribbonWindow));
    REQUIRE(commandsHost.Initialize(commandsWindow));
    REQUIRE(ribbonHost.SetContent(ribbon, ribbon));
    frame->Show();
    ribbonHost.ForceRender();
    const bool templateLoaded = WaitForToolkitDispatch([&]
    {
        return ribbon.IsLoaded() && group.IsLoaded() && mount.IsLoaded();
    });
    INFO("ribbon loaded=" << ribbon.IsLoaded()
         << " group loaded=" << group.IsLoaded()
         << " mount loaded=" << mount.IsLoaded());
    REQUIRE(templateLoaded);
    REQUIRE(IsInToolkitSubtree(mount, ribbon));

    const MUXC::StackPanel commands;
    const MUXC::Button command;
    command.Content(winrt::box_value(L"Toolkit command"));
    command.Width(150);
    command.Height(32);
    commands.Children().Append(command);
    REQUIRE(commandsHost.SetContentInParent(
        commands, command, ribbonWindow, mount));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(ribbonWindow);
    REQUIRE(host != nullptr);
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(commandsWindow) == host);
    wxWinUISlot * const parentSlot = host->FindSlot(ribbonWindow);
    wxWinUISlot * const childSlot = host->FindSlot(commandsWindow);
    REQUIRE(parentSlot != nullptr);
    REQUIRE(childSlot != nullptr);
    CHECK(parentSlot->GetVisualRoot() == ribbon);
    CHECK(childSlot->GetVisualRoot() == commands);
    CHECK(childSlot->GetSemanticTarget() == command);
    CHECK(group.Content() == mount);
    CHECK(mount.Content().try_as<MUXC::Grid>() == childSlot->GetContainer());

    commandsHost.ForceRender();
    REQUIRE(WaitForToolkitDispatch([&] { return command.IsLoaded(); }));
    // ContentControl realizes its content asynchronously. Verify the actual
    // native ancestry after that content's Loaded boundary, not just after
    // the transactional Content property has accepted the slot carrier.
    CHECK(IsInToolkitSubtree(childSlot->GetContainer(), ribbon));
    CHECK(command.XamlRoot() == ribbon.XamlRoot());
    CHECK(command.XamlRoot() == host->GetXamlRoot());
    REQUIRE(command.Focus(MUX::FocusState::Programmatic));
    REQUIRE(WaitForToolkitDispatch([&]
    {
        return wxWinUIHostFocusTestAccess::FindFocusedSlot(*host) == commandsWindow;
    }));

    commandsHost.Close();
    CHECK(host->FindSlot(commandsWindow) == nullptr);
    CHECK_FALSE(mount.Content());
    CHECK(group.Content() == mount);
    CHECK(host->FindSlot(ribbonWindow) == parentSlot);
    CHECK(ribbon.Items().GetAt(0) == group);
}

TEST_CASE("RibbonToolkit::NativeChromeDispatchesPublicEventsAfterStateChange",
          "[winui-ribbon-toolkit][winui-ribbon-chrome]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    RibbonChromeFixture fixture;
    fixture.Create();
    const auto help = fixture.Button(true);
    const auto toggle = fixture.Button(false);
    REQUIRE(help);
    REQUIRE(toggle);
    CHECK(IsInToolkitSubtree(help, fixture.Tabs()));
    CHECK(IsInToolkitSubtree(toggle, fixture.Tabs()));
    const auto invokeHelp = GetToolkitPattern<Providers::IInvokeProvider>(
        help, Patterns::PatternInterface::Invoke);
    const auto invokeToggle = GetToolkitPattern<Providers::IInvokeProvider>(
        toggle, Patterns::PatternInterface::Invoke);
    REQUIRE(invokeHelp);
    REQUIRE(invokeToggle);
    CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_PINNED);
    const auto expandedName = MUX::Automation::AutomationProperties::GetName(toggle);
    CHECK_FALSE(expandedName.empty());
    CHECK_FALSE(MUX::Automation::AutomationProperties::GetName(help).empty());
    invokeHelp.Invoke();
    fixture.CheckEvent(1, wxEVT_RIBBONBAR_HELP_CLICK, wxRIBBON_BAR_PINNED);
    invokeToggle.Invoke();
    fixture.CheckEvent(2, wxEVT_RIBBONBAR_TOGGLED, wxRIBBON_BAR_MINIMIZED);
    CHECK(MUX::Automation::AutomationProperties::GetName(toggle) != expandedName);
    invokeHelp.Invoke();
    fixture.CheckEvent(3, wxEVT_RIBBONBAR_HELP_CLICK, wxRIBBON_BAR_MINIMIZED);
    invokeToggle.Invoke();
    fixture.CheckEvent(4, wxEVT_RIBBONBAR_TOGGLED, wxRIBBON_BAR_PINNED);
    fixture.bar->ShowPanels(wxRIBBON_BAR_EXPANDED);
    CHECK(fixture.events->size() == 4);
    invokeToggle.Invoke();
    fixture.CheckEvent(5, wxEVT_RIBBONBAR_TOGGLED, wxRIBBON_BAR_MINIMIZED);
    fixture.bar->ShowPanels(true);
    CHECK(fixture.events->size() == 5);
    REQUIRE(fixture.bar->Realize());
    CHECK(fixture.Button(true) == help);
    CHECK(fixture.Button(false) == toggle);
}

TEST_CASE("RibbonToolkit::NativeChromeFlagsDisableRetainedProvidersWithoutRecreation",
          "[winui-ribbon-toolkit][winui-ribbon-chrome]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    RibbonChromeFixture fixture;
    fixture.Create(0);
    const auto help = fixture.Button(true);
    const auto toggle = fixture.Button(false);
    REQUIRE(help);
    REQUIRE(toggle);
    CHECK(help.Visibility() == MUX::Visibility::Collapsed);
    CHECK(toggle.Visibility() == MUX::Visibility::Collapsed);
    fixture.SetChrome(RibbonChromeFixture::ChromeFlags);
    CHECK(fixture.Button(true) == help);
    CHECK(fixture.Button(false) == toggle);
    const auto invokeHelp = GetToolkitPattern<Providers::IInvokeProvider>(
        help, Patterns::PatternInterface::Invoke);
    const auto invokeToggle = GetToolkitPattern<Providers::IInvokeProvider>(
        toggle, Patterns::PatternInterface::Invoke);
    REQUIRE(invokeHelp);
    REQUIRE(invokeToggle);
    fixture.SetChrome(wxRIBBON_BAR_SHOW_HELP_BUTTON);
    CHECK(help.Visibility() == MUX::Visibility::Visible);
    CHECK(toggle.Visibility() == MUX::Visibility::Collapsed);
    InvokeUnavailableRibbonChrome(invokeToggle);
    CHECK(fixture.events->empty());
    CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_PINNED);
    invokeHelp.Invoke();
    fixture.CheckEvent(1, wxEVT_RIBBONBAR_HELP_CLICK, wxRIBBON_BAR_PINNED);
    fixture.SetChrome(wxRIBBON_BAR_SHOW_TOGGLE_BUTTON);
    CHECK(help.Visibility() == MUX::Visibility::Collapsed);
    CHECK(toggle.Visibility() == MUX::Visibility::Visible);
    InvokeUnavailableRibbonChrome(invokeHelp);
    CHECK(fixture.events->size() == 1);
    invokeToggle.Invoke();
    fixture.CheckEvent(2, wxEVT_RIBBONBAR_TOGGLED, wxRIBBON_BAR_MINIMIZED);
    fixture.SetChrome(RibbonChromeFixture::ChromeFlags);
    fixture.bar->Hide();
    InvokeUnavailableRibbonChrome(invokeHelp);
    InvokeUnavailableRibbonChrome(invokeToggle);
    CHECK(fixture.events->size() == 2);
    CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_MINIMIZED);
    fixture.bar->Show();
    fixture.Ready();
    fixture.bar->Disable();
    InvokeUnavailableRibbonChrome(invokeHelp);
    InvokeUnavailableRibbonChrome(invokeToggle);
    CHECK(fixture.events->size() == 2);
    CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_MINIMIZED);
    REQUIRE(WaitForToolkitDispatch([&]()
        { return !help.IsEnabled() && !toggle.IsEnabled(); }));
    fixture.bar->Enable();
    REQUIRE(WaitForToolkitDispatch([&]()
        { return help.IsEnabled() && toggle.IsEnabled(); }));
    REQUIRE(fixture.bar->Realize());
    CHECK(fixture.Button(true) == help);
    CHECK(fixture.Button(false) == toggle);
    invokeToggle.Invoke();
    fixture.CheckEvent(3, wxEVT_RIBBONBAR_TOGGLED, wxRIBBON_BAR_PINNED);
}

TEST_CASE("RibbonToolkit::NativeChromeEventCanDeleteRibbonBar",
          "[winui-ribbon-toolkit][winui-ribbon-chrome]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    const bool help = GENERATE(false, true);
    CAPTURE(help);
    RibbonChromeFixture fixture;
    fixture.Create();
    const auto provider = GetToolkitPattern<Providers::IInvokeProvider>(
        fixture.Button(help), Patterns::PatternInterface::Invoke);
    REQUIRE(provider);
    auto calls = std::make_shared<unsigned>(0);
    fixture.bar->Bind(help ? wxEVT_RIBBONBAR_HELP_CLICK : wxEVT_RIBBONBAR_TOGGLED,
        [live = fixture.bar, calls, help](wxRibbonBarEvent& event)
        {
            ++*calls;
            REQUIRE(live);
            CHECK(event.GetId() == RibbonChromeFixture::BarId);
            CHECK(event.GetEventObject() == live.get());
            CHECK(event.GetPage() == nullptr);
            CHECK(live->GetDisplayMode() == (help ? wxRIBBON_BAR_PINNED
                                                 : wxRIBBON_BAR_MINIMIZED));
            delete live.get();
        });
    provider.Invoke();
    REQUIRE(WaitForToolkitDispatch([&]() { return !fixture.bar; }));
    CHECK(*calls == 1);
    REQUIRE(fixture.frame);
    CHECK_FALSE(fixture.frame->IsBeingDeleted());
    InvokeUnavailableRibbonChrome(provider);
    CHECK(*calls == 1);
}

TEST_CASE("RibbonToolkit::PageFlagsPreserveNativeImagesAndAccessibleLabels",
          "[winui-ribbon-toolkit][winui-ribbon-images]")
{
    RibbonIconFixture fixture;
    fixture.Create();
    const auto tabs = fixture.Tabs();
    const auto item = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto otherItem = tabs.TabItems().GetAt(1).as<MUXC::TabViewItem>();
    const auto source = item.IconSource().try_as<MUXC::ImageIconSource>();
    CheckRibbonImagePixels(source, fixture.page->GetIconBundle(), fixture.page.get());
    const auto imageSource = source.ImageSource();
    CHECK_FALSE(otherItem.IconSource());
    MUX::FrameworkElement image{nullptr};
    const bool imageReady = WaitForToolkitDispatch([&]()
    {
        if ( !image ) image = FindRibbonImage(item, source);
        return image && image.IsLoaded() && image.ActualWidth() > 0 && image.ActualHeight() > 0;
    });
    if ( !imageReady )
    {
        const auto nativeIcon = item.TabViewTemplateSettings().IconElement();
        std::ostringstream tree;
        DescribeRibbonImages(tree, item, source);
        INFO("image=" << bool(image) << " loaded=" << (image && image.IsLoaded())
             << " size=" << (image ? image.ActualWidth() : 0) << "x"
             << (image ? image.ActualHeight() : 0) << " nativeIcon="
             << (nativeIcon ? winrt::to_string(winrt::get_class_name(nativeIcon)) : "null")
             << " tree=" << tree.str());
        REQUIRE(imageReady);
    }
    REQUIRE(imageReady);
    for ( const long flags : {long(wxRIBBON_BAR_SHOW_PAGE_ICONS),
                             long(wxRIBBON_BAR_SHOW_PAGE_LABELS), 0L,
                             RibbonIconFixture::PageFlags} )
    {
        CAPTURE(flags);
        fixture.bar->SetWindowStyleFlag(
            (fixture.bar->GetWindowStyleFlag() & ~RibbonIconFixture::PageFlags) | flags);
        REQUIRE(fixture.bar->Realize());
        fixture.Ready();
        CHECK(fixture.Tabs() == tabs);
        CHECK(tabs.TabItems().GetAt(0) == item);
        CHECK(tabs.TabItems().GetAt(1) == otherItem);
        CHECK(winrt::unbox_value_or<winrt::hstring>(item.Header(), L"") ==
              ((flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) ? L"Image page" : L""));
        CHECK(MUX::Automation::AutomationProperties::GetName(item) == L"Image page");
        const auto peer = MUX::Automation::Peers::FrameworkElementAutomationPeer::
            CreatePeerForElement(item);
        REQUIRE(peer);
        CHECK(peer.GetName() == L"Image page");
        CHECK_FALSE(otherItem.IconSource());
        if ( flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
        {
            CHECK(item.IconSource() == source);
            CHECK(source.ImageSource() == imageSource);
        }
        else CHECK_FALSE(item.IconSource());
    }
    fixture.bar->SetWindowStyleFlag(
        fixture.bar->GetWindowStyleFlag() & ~wxRIBBON_BAR_SHOW_PAGE_LABELS);
    fixture.page->SetLabel("Renamed image page");
    REQUIRE(fixture.bar->Realize());
    CHECK(winrt::unbox_value_or<winrt::hstring>(item.Header(), L"").empty());
    CHECK(MUX::Automation::AutomationProperties::GetName(item) == L"Renamed image page");
    CHECK(item.IconSource() == source);
}

TEST_CASE("RibbonToolkit::MinimisedPanelUsesBundleImageInRealNativeButton",
          "[winui-ribbon-toolkit][winui-ribbon-images]")
{
    RibbonIconFixture fixture;
    fixture.Create();
    const auto group = fixture.Presentation(fixture.panel.get()).as<Labs::RibbonCollapsibleGroup>();
    const auto plainGroup = fixture.Presentation(fixture.plainPanel.get()).as<Labs::RibbonCollapsibleGroup>();
    const auto ribbon = fixture.Presentation(fixture.page.get()).as<Labs::Ribbon>();
    const auto source = group.IconSource().try_as<MUXC::ImageIconSource>();
    CheckRibbonImagePixels(source, fixture.panel->GetMinimisedIconBundle(), fixture.panel.get());
    CHECK_FALSE(plainGroup.IconSource());
    const auto imageSource = source.ImageSource();
    wxRibbonFlyoutParts parts;
    REQUIRE(wxRibbonResolveFlyoutParts(group, parts) == wxRibbonFlyoutResolveResult::Ready);
    ribbon.MaxWidth(80);
    ribbon.InvalidateMeasure();
    ribbon.UpdateLayout();
    const bool minimised = WaitForToolkitDispatch([&]()
    {
        ribbon.UpdateLayout();
        const auto image = FindRibbonImage(parts.button, source);
        return fixture.panel->IsMinimised() &&
               parts.button.Visibility() == MUX::Visibility::Visible &&
               image && image.IsLoaded() && image.ActualWidth() > 0 && image.ActualHeight() > 0;
    });
    const auto image = FindRibbonImage(parts.button, source);
    std::ostringstream tree;
    if ( !minimised ) DescribeRibbonImages(tree, parts.button, source);
    INFO(tree.str());
    INFO("panel=" << fixture.panel->IsMinimised()
         << ", group state=" << static_cast<int>(group.State())
         << ", ribbon width=" << ribbon.ActualWidth()
         << ", button visibility=" << static_cast<int>(parts.button.Visibility())
         << ", image=" << bool(image)
         << ", image loaded=" << (image && image.IsLoaded())
         << ", image size=" << (image ? image.ActualWidth() : 0)
         << "x" << (image ? image.ActualHeight() : 0));
    REQUIRE(minimised);
    const auto preferred = fixture.panel->GetMinimisedIconBundle()
        .GetPreferredBitmapSizeFor(fixture.panel.get());
    const double scale = fixture.panel->GetDPIScaleFactor();
    CHECK(std::abs(image.Width() - preferred.x / scale) < 0.01);
    CHECK(std::abs(image.Height() - preferred.y / scale) < 0.01);
    CHECK(std::abs(image.ActualWidth() - preferred.x / scale) <= 0.5);
    CHECK(std::abs(image.ActualHeight() - preferred.y / scale) <= 0.5);
    CHECK_FALSE(parts.flyout.IsOpen());
    CHECK(wxRibbonFlyoutPartsAreCurrent(parts));
    REQUIRE(fixture.bar->Realize());
    CHECK(fixture.Presentation(fixture.panel.get()) == group);
    CHECK(FindRibbonImage(parts.button, source) == image);
    CHECK(wxRibbonFlyoutPartsAreCurrent(parts));
    CHECK(group.IconSource() == source);
    CHECK(source.ImageSource() == imageSource);
    CHECK_FALSE(plainGroup.IconSource());
}

TEST_CASE("RibbonToolkit::HeaderChangeCanDeletePublicPageDuringSynchronization",
          "[winui-ribbon-toolkit][winui-ribbon-images]")
{
    RibbonIconFixture fixture;
    fixture.Create();
    const auto tabs = fixture.Tabs();
    const auto item = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto survivor = tabs.TabItems().GetAt(1).as<MUXC::TabViewItem>();
    const auto calls = std::make_shared<unsigned>(0);
    const auto token = item.RegisterPropertyChangedCallback(MUXC::TabViewItem::HeaderProperty(),
        [bar = fixture.bar, page = fixture.page, calls](const auto&, const auto&)
        {
            // DeletePage retires the public model synchronously and destroys
            // the wxWindow at idle. Never leave a raw deleted pointer in m_pages.
            if ( ++*calls != 1 || !bar || !page ) return;
            const int index = bar->GetPageNumber(page.get());
            REQUIRE(index != wxNOT_FOUND);
            bar->DeletePage(static_cast<size_t>(index));
        });
    const auto unhook = wxMakeGuard([&]()
        { item.UnregisterPropertyChangedCallback(MUXC::TabViewItem::HeaderProperty(), token); });
    fixture.page->SetLabel("Delete from native header callback");
    // A mutation can abort this synchronization; the coalesced replay must
    // converge without requiring a second explicit application Realize().
    fixture.bar->Realize();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return fixture.bar && fixture.bar->GetPageCount() == 1 &&
               tabs.TabItems().Size() == 1 && tabs.TabItems().GetAt(0) == survivor;
    }));
    CHECK(*calls == 1);
    REQUIRE(fixture.other);
    CHECK(fixture.bar->GetPage(0) == fixture.other.get());
    CHECK(fixture.bar->GetActivePage() == 0);
    CHECK(tabs.SelectedItem() == survivor);
    CHECK(MUX::Automation::AutomationProperties::GetName(survivor) == L"No image");
    wxTheApp->ProcessIdle();
    CHECK_FALSE(fixture.page);
    REQUIRE(fixture.bar);
    REQUIRE(fixture.bar->Realize());
    CHECK(tabs.TabItems().GetAt(0) == survivor);
    CHECK(*calls == 1);
}

TEST_CASE("RibbonToolkit::ArtTabFontUsesNativeLabelsWithoutRemount",
          "[winui-ribbon-toolkit][winui-ribbon-art-font]")
{
    RibbonIconFixture fixture;
    fixture.Create();
    const auto tabs = fixture.Tabs();
    const auto first = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto second = tabs.TabItems().GetAt(1).as<MUXC::TabViewItem>();
    const auto icon = first.IconSource();
    MUXC::TextBlock firstLabel{nullptr}, secondLabel{nullptr};
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        firstLabel = FindRibbonTabLabel(first, L"Image page");
        secondLabel = FindRibbonTabLabel(second, L"No image");
        return firstLabel && secondLabel && firstLabel.IsLoaded() && secondLabel.IsLoaded() &&
               firstLabel.ActualHeight() > 0 && secondLabel.ActualHeight() > 0;
    }));
    const double previousHeight = firstLabel.ActualHeight();
    const wxFont largeFont(wxFontInfo(18).FaceName("Consolas").Bold().Italic());
    fixture.bar->GetArtProvider()->SetFont(wxRIBBON_ART_TAB_LABEL_FONT, largeFont);
    REQUIRE(fixture.bar->Realize());
    const bool fontReady = WaitForToolkitDispatch([&]()
    {
        return firstLabel.FontSize() == 24 && secondLabel.FontSize() == 24 &&
               firstLabel.ActualHeight() > previousHeight;
    });
    INFO("tab font=" << first.FontSize() << ", labels="
         << firstLabel.FontSize() << "/" << secondLabel.FontSize()
         << ", label height=" << previousHeight << " -> "
         << firstLabel.ActualHeight());
    REQUIRE(fontReady);
    CheckRibbonTabFont(first, largeFont);
    CheckRibbonTabFont(second, largeFont);
    CheckRibbonTabFont(firstLabel, largeFont);
    CheckRibbonTabFont(secondLabel, largeFont);
    CHECK(fixture.Tabs() == tabs);
    CHECK(tabs.TabItems().GetAt(0) == first);
    CHECK(tabs.TabItems().GetAt(1) == second);
    CHECK(first.IconSource() == icon);
    CHECK(FindRibbonTabLabel(first, L"Image page") == firstLabel);
    CHECK(FindRibbonTabLabel(second, L"No image") == secondLabel);

    // Realize with an unchanged provider must not create a fresh FontFamily
    // resource and trigger a new native property/layout transaction.
    const auto family = first.FontFamily();
    unsigned fontChanges = 0;
    const auto token = first.RegisterPropertyChangedCallback(MUXC::Control::FontFamilyProperty(),
        [&](const auto&, const auto&) { ++fontChanges; });
    const auto unhook = wxMakeGuard([&]()
        { first.UnregisterPropertyChangedCallback(MUXC::Control::FontFamilyProperty(), token); });
    REQUIRE(fixture.bar->Realize());
    CHECK(first.FontFamily() == family);
    CHECK(fontChanges == 0);

    const wxFont replacementFont(wxFontInfo(12).FaceName("Segoe UI"));
    auto* const replacement = new wxRibbonMSWArtProvider;
    replacement->SetFont(wxRIBBON_ART_TAB_LABEL_FONT, replacementFont);
    fixture.bar->SetArtProvider(replacement);
    REQUIRE(fixture.bar->Realize());
    REQUIRE(WaitForToolkitDispatch([&]()
        { return firstLabel.FontSize() == 16 && secondLabel.FontSize() == 16; }));
    CheckRibbonTabFont(firstLabel, replacementFont);
    CheckRibbonTabFont(secondLabel, replacementFont);
    CHECK(tabs.TabItems().GetAt(0) == first);
    CHECK(tabs.TabItems().GetAt(1) == second);
    CHECK(first.IconSource() == icon);

    // Reapplying the actual native template must bind its new presenter,
    // rather than continuing to update the retired label. No SDK/global
    // resource replacement is involved and the public tab identity survives.
    // Use the text-only tab here. Retaining an old icon-bearing template
    // also retains its binding to TabViewTemplateSettings.IconElement; that
    // shared native UIElement cannot be parented to both template instances.
    // Icon retention is checked above without replacing its native template.
    const auto nativeTemplate = second.Template();
    REQUIRE(nativeTemplate);
    const char* templateStage = "invalidate native template";
    try
    {
        // Replace the template before materializing it. Applying a null
        // template would ask TabViewItem to initialize without its required
        // native parts, rather than exercising a complete retemplate.
        second.Template(nullptr);
        templateStage = "restore native template";
        second.Template(nativeTemplate);
        templateStage = "apply restored native template";
        second.ApplyTemplate();
    }
    catch ( const winrt::hresult_error& error )
    {
        FAIL("TabViewItem retemplate stage=" << templateStage
             << ", HRESULT=" << static_cast<int32_t>(error.code())
             << ", message=" << winrt::to_string(error.message()));
    }
    MUXC::TextBlock replacementLabel{nullptr};
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        replacementLabel = FindRibbonTabLabel(second, L"No image");
        return replacementLabel && replacementLabel != secondLabel &&
               replacementLabel.IsLoaded() && replacementLabel.FontSize() == 16;
    }));
    CheckRibbonTabFont(replacementLabel, replacementFont);
    CHECK(tabs.TabItems().GetAt(0) == first);
    CHECK(tabs.TabItems().GetAt(1) == second);
    CHECK(first.IconSource() == icon);
    CHECK_FALSE(second.IconSource());
    CHECK(second.Template() == nativeTemplate);
}

TEST_CASE("RibbonToolkit::NativeArtFontCallbackCanRetirePage",
          "[winui-ribbon-toolkit][winui-ribbon-art-font]")
{
    RibbonIconFixture fixture;
    fixture.Create();
    const auto tabs = fixture.Tabs();
    const auto first = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto survivor = tabs.TabItems().GetAt(1).as<MUXC::TabViewItem>();
    unsigned calls = 0;
    const auto token = first.RegisterPropertyChangedCallback(MUXC::Control::FontSizeProperty(),
        [bar = fixture.bar, page = fixture.page, &calls](const auto&, const auto&)
        {
            if ( ++calls != 1 || !bar || !page ) return;
            const int index = bar->GetPageNumber(page.get());
            if ( index != wxNOT_FOUND ) bar->DeletePage(static_cast<size_t>(index));
        });
    const auto unhook = wxMakeGuard([&]()
        { first.UnregisterPropertyChangedCallback(MUXC::Control::FontSizeProperty(), token); });
    const wxFont font(wxFontInfo(19).FaceName("Consolas").Bold());
    fixture.bar->GetArtProvider()->SetFont(wxRIBBON_ART_TAB_LABEL_FONT, font);
    fixture.bar->Realize();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return fixture.bar && fixture.bar->GetPageCount() == 1 &&
               tabs.TabItems().Size() == 1 && tabs.TabItems().GetAt(0) == survivor &&
               std::abs(survivor.FontSize() - font.GetFractionalPointSize() * 96.0 / 72.0) < 0.01;
    }));
    CHECK(calls == 1);
    CHECK(fixture.bar->GetPage(0) == fixture.other.get());
    CheckRibbonTabFont(survivor, font);
    wxTheApp->ProcessIdle();
    CHECK_FALSE(fixture.page);
    REQUIRE(fixture.bar->Realize());
    CHECK(tabs.TabItems().GetAt(0) == survivor);
    CHECK(calls == 1);
}

TEST_CASE("RibbonToolkit::ArtFontQueryCanDestroyItsRibbonOwner",
          "[winui-ribbon-toolkit][winui-ribbon-art-font]")
{
    RibbonIconFixture fixture;
    fixture.Create();
    auto* const art = new ReentrantRibbonTabFontArt;
    fixture.bar->SetArtProvider(art);
    unsigned calls = 0;
    art->onRead = [bar = fixture.bar, &calls]()
    {
        ++calls;
        delete bar.get();
    };
    CHECK_FALSE(fixture.bar->Realize());
    CHECK(calls == 1);
    CHECK_FALSE(fixture.bar);
    REQUIRE(fixture.frame);
    CHECK_FALSE(fixture.frame->IsBeingDeleted());
}

TEST_CASE("RibbonToolkit::PanelExtensionFlagsAndNativeInvocation",
          "[winui-ribbon-toolkit][winui-ribbon-extension]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    const bool noAutoMinimise = GENERATE(false, true);
    CAPTURE(noAutoMinimise);
    RibbonExtensionFixture fixture;
    fixture.Create(noAutoMinimise, false);
    CHECK_FALSE(fixture.panel->HasExtButton());
    const auto initiallyHidden = fixture.InlineButton();
    CHECK((!initiallyHidden || initiallyHidden.Visibility() == MUX::Visibility::Collapsed));
    fixture.SetBarFlag(true);
    fixture.ReadyExtension();
    REQUIRE(fixture.panel->HasExtButton());
    CHECK_FALSE(fixture.plainPanel->HasExtButton());
    const auto plainButton = FindRibbonChromeButton(
        fixture.Presentation(fixture.plainPanel.get()), L"wxRibbonPanelExtension");
    CHECK((!plainButton || plainButton.Visibility() == MUX::Visibility::Collapsed));
    const auto group = fixture.Presentation(fixture.panel.get());
    if ( noAutoMinimise )
    {
        CHECK(group.try_as<Labs::RibbonGroup>());
        CHECK_FALSE(group.try_as<Labs::RibbonCollapsibleGroup>());
    }
    else CHECK(group.try_as<Labs::RibbonCollapsibleGroup>());
    const auto button = fixture.InlineButton();
    const auto commandRoot = fixture.CommandRoot();
    CHECK(IsInToolkitSubtree(button, group));
    const auto glyph = button.Content().try_as<MUXC::FontIcon>();
    REQUIRE(glyph);
    CHECK_FALSE(glyph.Glyph().empty());
    const auto expectedName = wxString::Format(_("More options for %s"), fixture.panel->GetLabel());
    CHECK(MUX::Automation::AutomationProperties::GetName(button) ==
          winrt::hstring(expectedName.wc_str()));
    const auto invoke = GetToolkitPattern<Providers::IInvokeProvider>(
        button, Patterns::PatternInterface::Invoke);
    REQUIRE(invoke);
    invoke.Invoke();
    REQUIRE(fixture.events->size() == 1);
    CHECK(fixture.events->back().id == fixture.panel->GetId());
    CHECK(fixture.events->back().object == fixture.panel.get());
    CHECK(fixture.events->back().panel == fixture.panel.get());
    fixture.SetBarFlag(false);
    CHECK_FALSE(fixture.panel->HasExtButton());
    CHECK(button.Visibility() == MUX::Visibility::Collapsed);
    InvokeUnavailableRibbonChrome(invoke);
    CHECK(fixture.events->size() == 1);
    fixture.SetBarFlag(true);
    fixture.ReadyExtension();
    CHECK(fixture.InlineButton() == button);
    fixture.panel->Disable();
    InvokeUnavailableRibbonChrome(invoke);
    CHECK(fixture.events->size() == 1);
    REQUIRE(WaitForToolkitDispatch([&]() { return !button.IsEnabled(); }));
    fixture.panel->Enable();
    REQUIRE(WaitForToolkitDispatch([&]() { return button.IsEnabled(); }));
    fixture.panel->Hide();
    InvokeUnavailableRibbonChrome(invoke);
    CHECK(fixture.events->size() == 1);
    fixture.panel->Show();
    fixture.ReadyExtension();
    CHECK(fixture.InlineButton() == button);
    CHECK(fixture.Presentation(fixture.panel.get()) == group);
    CHECK(fixture.CommandRoot() == commandRoot);
    invoke.Invoke();
    REQUIRE(fixture.events->size() == 2);
    CHECK(fixture.events->back().id == fixture.panel->GetId());
    CHECK(fixture.events->back().object == fixture.panel.get());
    CHECK(fixture.events->back().panel == fixture.panel.get());
}

TEST_CASE("RibbonToolkit::ExpandedPanelExtensionUsesRealProxyAndRetiresProvider",
          "[winui-ribbon-toolkit][winui-ribbon-extension]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    RibbonExtensionFixture fixture;
    fixture.Create();
    fixture.ReadyExtension();
    const auto group = fixture.Presentation(fixture.panel.get()).as<Labs::RibbonCollapsibleGroup>();
    const auto ribbon = fixture.Presentation(fixture.page.get()).as<Labs::Ribbon>();
    const auto commandRoot = fixture.CommandRoot();
    const auto content = group.Content();
    const auto inlineButton = fixture.InlineButton();
    const auto inlineInvoke = GetToolkitPattern<Providers::IInvokeProvider>(
        inlineButton, Patterns::PatternInterface::Invoke);
    REQUIRE(inlineInvoke);
    wxRibbonFlyoutParts parts;
    REQUIRE(wxRibbonResolveFlyoutParts(group, parts) == wxRibbonFlyoutResolveResult::Ready);
    ribbon.MaxWidth(80);
    ribbon.InvalidateMeasure();
    ribbon.UpdateLayout();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        ribbon.UpdateLayout();
        return fixture.panel->IsMinimised() &&
               inlineButton.Visibility() == MUX::Visibility::Collapsed;
    }));
    CHECK(fixture.panel->HasExtButton());
    InvokeUnavailableRibbonChrome(inlineInvoke);
    CHECK(fixture.events->empty());
    REQUIRE(fixture.panel->ShowExpanded());
    MUXC::Button popupButton{nullptr};
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        popupButton = FindRibbonChromeButton(parts.flyout.Content(), L"wxRibbonPanelExtension");
        return fixture.panel->GetExpandedPanel() && parts.flyout.IsOpen() &&
               popupButton && popupButton.IsLoaded() && popupButton.IsEnabled() &&
               popupButton.Visibility() == MUX::Visibility::Visible &&
               popupButton.ActualWidth() > 0 && popupButton.ActualHeight() > 0;
    }));
    wxWeakRef<wxRibbonPanel> expanded(fixture.panel->GetExpandedPanel());
    REQUIRE(expanded);
    REQUIRE(expanded->HasExtButton());
    CHECK_FALSE(expanded->IsMinimised());
    CHECK(expanded->GetExpandedDummy() == fixture.panel.get());
    CHECK(popupButton != inlineButton);
    CHECK(IsInToolkitSubtree(popupButton, parts.flyout.Content()));
    CHECK(IsInToolkitSubtree(commandRoot, parts.popupMount));
    CHECK(fixture.CommandRoot() == commandRoot);
    CHECK(group.Content() == content);
    CHECK(wxRibbonFlyoutPartsAreCurrent(parts));
    const auto proxyEvents = std::make_shared<unsigned>(0);
    expanded->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED,
        [expanded, proxyEvents](wxRibbonPanelEvent& event)
        {
            ++*proxyEvents;
            REQUIRE(expanded);
            CHECK(event.GetId() == expanded->GetId());
            CHECK(event.GetEventObject() == expanded.get());
            CHECK(event.GetPanel() == expanded.get());
            event.Skip(); // The original dummy must receive this same payload.
        });
    const auto popupInvoke = GetToolkitPattern<Providers::IInvokeProvider>(
        popupButton, Patterns::PatternInterface::Invoke);
    REQUIRE(popupInvoke);
    const int proxyId = expanded->GetId();
    auto* const proxyIdentity = expanded.get();
    popupInvoke.Invoke();
    CHECK(*proxyEvents == 1);
    REQUIRE(fixture.events->size() == 1);
    CHECK(fixture.events->back().id == proxyId);
    CHECK(fixture.events->back().object == proxyIdentity);
    CHECK(fixture.events->back().panel == proxyIdentity);
    fixture.bar->Disable();
    InvokeUnavailableRibbonChrome(popupInvoke);
    CHECK(*proxyEvents == 1);
    CHECK(fixture.events->size() == 1);
    REQUIRE(WaitForToolkitDispatch([&]() { return !popupButton.IsEnabled(); }));
    fixture.bar->Enable();
    REQUIRE(WaitForToolkitDispatch([&]() { return popupButton.IsEnabled(); }));
    REQUIRE(fixture.panel->HideExpanded());
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return !fixture.panel->GetExpandedPanel() && !parts.flyout.IsOpen() &&
               wxWinUITopLevelHost::CanReparentSubtreeNow(fixture.bar.get());
    }));
    InvokeUnavailableRibbonChrome(popupInvoke);
    CHECK(*proxyEvents == 1);
    CHECK(fixture.events->size() == 1);
    CHECK(fixture.CommandRoot() == commandRoot);
    CHECK(group.Content() == content);
    CHECK(wxRibbonFlyoutPartsAreCurrent(parts));
    ribbon.MaxWidth(1060);
    ribbon.InvalidateMeasure();
    ribbon.UpdateLayout();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        ribbon.UpdateLayout();
        return !fixture.panel->IsMinimised() &&
               inlineButton.Visibility() == MUX::Visibility::Visible;
    }));
    CHECK(fixture.InlineButton() == inlineButton);
    CHECK(fixture.CommandRoot() == commandRoot);
}

TEST_CASE("RibbonToolkit::PanelExtensionCallbackCanDeleteRibbonBar",
          "[winui-ribbon-toolkit][winui-ribbon-extension]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    RibbonExtensionFixture fixture;
    fixture.Create();
    fixture.ReadyExtension();
    const auto invoke = GetToolkitPattern<Providers::IInvokeProvider>(
        fixture.InlineButton(), Patterns::PatternInterface::Invoke);
    REQUIRE(invoke);
    const auto calls = std::make_shared<unsigned>(0);
    fixture.panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED,
        [bar = fixture.bar, panel = fixture.panel, calls](wxRibbonPanelEvent& event)
        {
            ++*calls;
            REQUIRE(bar);
            REQUIRE(panel);
            CHECK(event.GetId() == panel->GetId());
            CHECK(event.GetEventObject() == panel.get());
            CHECK(event.GetPanel() == panel.get());
            delete bar.get();
        });
    invoke.Invoke();
    REQUIRE(WaitForToolkitDispatch([&]() { return !fixture.bar; }));
    CHECK_FALSE(fixture.panel);
    CHECK(*calls == 1);
    REQUIRE(fixture.frame);
    CHECK_FALSE(fixture.frame->IsBeingDeleted());
    InvokeUnavailableRibbonChrome(invoke);
    CHECK(*calls == 1);
}

TEST_CASE("RibbonToolkit::ExpandedProxyProjectsEnabledAndShownStateWithoutRealize",
          "[winui-ribbon-toolkit][winui-ribbon-extension][winui-ribbon-proxy-state]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    RibbonExtensionFixture fixture;
    fixture.Create();
    fixture.ReadyExtension();
    const auto group = fixture.Presentation(fixture.panel.get()).as<Labs::RibbonCollapsibleGroup>();
    const auto ribbon = fixture.Presentation(fixture.page.get()).as<Labs::Ribbon>();
    const auto content = group.Content();
    const auto commandRoot = fixture.CommandRoot();
    const auto command = FindToolkitCommand(commandRoot, L"Extension command 0");
    const auto inlineButton = fixture.InlineButton();
    REQUIRE(command);
    const auto commandEvents = std::make_shared<unsigned>(0);
    fixture.commands->Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
        [live = fixture.commands, commandEvents](wxRibbonButtonBarEvent& event)
        {
            ++*commandEvents;
            REQUIRE(live);
            CHECK(event.GetId() == RibbonExtensionFixture::PanelId + 1);
            CHECK(event.GetEventObject() == live.get());
            CHECK(event.GetBar() == live.get());
        });
    wxRibbonFlyoutParts parts;
    REQUIRE(wxRibbonResolveFlyoutParts(group, parts) == wxRibbonFlyoutResolveResult::Ready);
    ribbon.MaxWidth(80);
    ribbon.InvalidateMeasure();
    ribbon.UpdateLayout();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        ribbon.UpdateLayout();
        return fixture.panel->IsMinimised();
    }));
    REQUIRE(fixture.panel->ShowExpanded());
    MUXC::Button extension{nullptr};
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        extension = FindRibbonChromeButton(parts.flyout.Content(), L"wxRibbonPanelExtension");
        return fixture.panel->GetExpandedPanel() && parts.flyout.IsOpen() &&
               RibbonNativeElementIsVisible(extension) &&
               RibbonNativeElementIsVisible(command);
    }));
    wxWeakRef<wxRibbonPanel> expanded(fixture.panel->GetExpandedPanel());
    REQUIRE(expanded);
    auto* const proxyIdentity = expanded.get();
    const auto extensionPeer = Patterns::FrameworkElementAutomationPeer::CreatePeerForElement(extension);
    const auto commandPeer = Patterns::FrameworkElementAutomationPeer::CreatePeerForElement(command);
    REQUIRE(extensionPeer);
    REQUIRE(commandPeer);
    const auto extensionInvoke = extensionPeer.GetPattern(Patterns::PatternInterface::Invoke)
        .try_as<Providers::IInvokeProvider>();
    const auto commandInvoke = commandPeer.GetPattern(Patterns::PatternInterface::Invoke)
        .try_as<Providers::IInvokeProvider>();
    REQUIRE(extensionInvoke);
    REQUIRE(commandInvoke);
    const auto checkIdentities = [&]()
    {
        REQUIRE(expanded);
        CHECK(fixture.panel->GetExpandedPanel() == proxyIdentity);
        CHECK(expanded->GetExpandedDummy() == fixture.panel.get());
        CHECK(parts.flyout.IsOpen());
        CHECK(wxRibbonFlyoutPartsAreCurrent(parts));
        CHECK(fixture.Presentation(fixture.panel.get()) == group);
        CHECK(group.Content() == content);
        CHECK(fixture.CommandRoot() == commandRoot);
        CHECK(FindToolkitCommand(commandRoot, L"Extension command 0") == command);
        CHECK(FindRibbonChromeButton(parts.flyout.Content(), L"wxRibbonPanelExtension") == extension);
        CHECK(fixture.commands->GetParent() == expanded.get());
        CHECK(fixture.panel->IsEnabled());
        CHECK(fixture.panel->IsShown());
    };
    extensionInvoke.Invoke();
    REQUIRE(fixture.events->size() == 1);
    REQUIRE(*commandEvents == 0);

    // Exercise only the real proxy's public methods below, without Realize,
    // host FlushSync, or directly setting native IsEnabled/Visibility.
    REQUIRE(expanded->Disable());
    CHECK_FALSE(expanded->IsEnabled());
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return !extension.IsEnabled() && !command.IsEnabled() &&
               !extensionPeer.IsEnabled() && !commandPeer.IsEnabled();
    }));
    CHECK(RibbonNativeElementIsVisible(extension));
    CHECK(RibbonNativeElementIsVisible(command));
    checkIdentities();
    InvokeUnavailableRibbonChrome(extensionInvoke);
    InvokeUnavailableRibbonChrome(commandInvoke);
    CHECK(fixture.events->size() == 1);
    CHECK(*commandEvents == 0);
    REQUIRE(expanded->Enable());
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return extension.IsEnabled() && command.IsEnabled() &&
               extensionPeer.IsEnabled() && commandPeer.IsEnabled();
    }));
    checkIdentities();

    REQUIRE(expanded->Hide());
    CHECK_FALSE(expanded->IsShown());
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return !RibbonNativeElementIsVisible(extension) &&
               !RibbonNativeElementIsVisible(command);
    }));
    checkIdentities();
    InvokeUnavailableRibbonChrome(extensionInvoke);
    InvokeUnavailableRibbonChrome(commandInvoke);
    CHECK(fixture.events->size() == 1);
    CHECK(*commandEvents == 0);
    REQUIRE(expanded->Show());
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return RibbonNativeElementIsVisible(extension) && extension.IsEnabled() &&
               RibbonNativeElementIsVisible(command) && command.IsEnabled();
    }));
    checkIdentities();
    extensionInvoke.Invoke();
    REQUIRE(fixture.events->size() == 2);
    REQUIRE(*commandEvents == 0);

    // Closing an intentionally disabled/hidden proxy must not leak its state
    // onto the same command tree when it returns to the enabled, shown dummy.
    REQUIRE(expanded->Disable());
    REQUIRE(expanded->Hide());
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return !extension.IsEnabled() && !command.IsEnabled() &&
               !RibbonNativeElementIsVisible(extension) &&
               !RibbonNativeElementIsVisible(command);
    }));
    REQUIRE(fixture.panel->HideExpanded());
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return !fixture.panel->GetExpandedPanel() && !parts.flyout.IsOpen() &&
               wxWinUITopLevelHost::CanReparentSubtreeNow(fixture.bar.get());
    }));
    CHECK(fixture.panel->IsEnabled());
    CHECK(fixture.panel->IsShown());
    CHECK(fixture.commands->GetParent() == fixture.panel.get());
    CHECK(fixture.commands->IsEnabled());
    CHECK(fixture.commands->IsShown());
    InvokeUnavailableRibbonChrome(extensionInvoke);
    CHECK(fixture.events->size() == 2);
    ribbon.MaxWidth(1060);
    ribbon.InvalidateMeasure();
    ribbon.UpdateLayout();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        ribbon.UpdateLayout();
        return !fixture.panel->IsMinimised() && command.IsEnabled() &&
               RibbonNativeElementIsVisible(command) &&
               RibbonNativeElementIsVisible(inlineButton) && inlineButton.IsEnabled();
    }));
    CHECK(fixture.CommandRoot() == commandRoot);
    CHECK(FindToolkitCommand(commandRoot, L"Extension command 0") == command);
    CHECK(fixture.InlineButton() == inlineButton);
    CHECK(group.Content() == content);
    CHECK(wxRibbonFlyoutPartsAreCurrent(parts));
    commandInvoke.Invoke();
    CHECK(*commandEvents == 1);
    const auto inlineInvoke = GetToolkitPattern<Providers::IInvokeProvider>(
        inlineButton, Patterns::PatternInterface::Invoke);
    REQUIRE(inlineInvoke);
    inlineInvoke.Invoke();
    REQUIRE(fixture.events->size() == 3);
    CHECK(fixture.events->back().object == fixture.panel.get());
    CHECK(fixture.events->back().panel == fixture.panel.get());
}

TEST_CASE("RibbonToolkit::NativeTabSelectionExpandsTemporarilyAndCommandsRespectPinning",
          "[winui-ribbon-toolkit][winui-ribbon-display-mode]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    RibbonIconFixture fixture;
    fixture.Create();
    const auto tabs = fixture.Tabs();
    const auto firstTab = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto otherTab = tabs.TabItems().GetAt(1).as<MUXC::TabViewItem>();
    const auto selectFirst = GetToolkitPattern<Providers::ISelectionItemProvider>(
        firstTab, Patterns::PatternInterface::SelectionItem);
    REQUIRE(selectFirst);
    REQUIRE_FALSE(fixture.panel->GetChildren().IsEmpty());
    auto* const commands = wxDynamicCast(
        fixture.panel->GetChildren().GetFirst()->GetData(), wxRibbonButtonBar);
    REQUIRE(commands);
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(fixture.bar.get());
    REQUIRE(host);
    REQUIRE(host->FindSlot(commands));
    const auto barRoot = host->FindSlot(fixture.bar.get())->GetContent().as<MUXC::Grid>();
    const auto firstRoot = fixture.Presentation(fixture.page.get());
    const auto otherRoot = fixture.Presentation(fixture.other.get());
    const auto panelRoot = fixture.Presentation(fixture.panel.get());
    const auto commandRoot = host->FindSlot(commands)->GetContent();
    const auto command = FindToolkitCommand(commandRoot, L"Image command 0");
    REQUIRE(command);
    const auto invokeCommand = GetToolkitPattern<Providers::IInvokeProvider>(
        command, Patterns::PatternInterface::Invoke);
    REQUIRE(invokeCommand);
    const auto transitions = std::make_shared<std::vector<RibbonPageTransition>>();
    const auto commandModes = std::make_shared<std::vector<wxRibbonDisplayMode>>();
    const auto record = [bar = fixture.bar, transitions](wxRibbonBarEvent& event)
    {
        REQUIRE(bar);
        transitions->push_back({event.GetEventType(), event.GetId(), event.GetEventObject(),
            event.GetPage(), bar->GetDisplayMode(), bar->GetActivePage()});
    };
    fixture.bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGING, record);
    fixture.bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGED, record);
    commands->Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
        [bar = fixture.bar, commandModes](wxRibbonButtonBarEvent& event)
        {
            REQUIRE(bar);
            CHECK(event.GetId() == wxID_HIGHEST + 5520);
            commandModes->push_back(bar->GetDisplayMode());
        });
    const auto checkIdentities = [&]()
    {
        CHECK(fixture.Tabs() == tabs);
        CHECK(tabs.TabItems().GetAt(0) == firstTab);
        CHECK(tabs.TabItems().GetAt(1) == otherTab);
        CHECK(host->FindSlot(fixture.bar.get())->GetContent() == barRoot);
        CHECK(fixture.Presentation(fixture.page.get()) == firstRoot);
        CHECK(fixture.Presentation(fixture.other.get()) == otherRoot);
        CHECK(fixture.Presentation(fixture.panel.get()) == panelRoot);
        REQUIRE(host->FindSlot(commands));
        CHECK(host->FindSlot(commands)->GetContent() == commandRoot);
        CHECK(FindToolkitCommand(commandRoot, L"Image command 0") == command);
    };
    REQUIRE(fixture.bar->SetActivePage(size_t(1)));
    fixture.bar->ShowPanels(wxRIBBON_BAR_MINIMIZED);
    CHECK(transitions->empty());
    CHECK_FALSE(fixture.bar->ArePanelsShown());
    CHECK(barRoot.Children().GetAt(1).Visibility() == MUX::Visibility::Collapsed);

    // This is the real TabViewItem selection provider, not a synthetic wx
    // mouse event. Selecting an already selected tab cannot test a re-click.
    selectFirst.Select();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return fixture.bar->GetDisplayMode() == wxRIBBON_BAR_EXPANDED &&
               fixture.bar->GetActivePage() == 0 && RibbonNativeElementIsVisible(command);
    }));
    CHECK(fixture.bar->ArePanelsShown());
    CHECK(tabs.SelectedItem() == firstTab);
    CHECK(barRoot.Children().GetAt(1).Visibility() == MUX::Visibility::Visible);
    REQUIRE(transitions->size() == 2);
    CHECK(transitions->at(0).type == wxEVT_RIBBONBAR_PAGE_CHANGING);
    CHECK(transitions->at(0).activePage == 1);
    CHECK(transitions->at(1).type == wxEVT_RIBBONBAR_PAGE_CHANGED);
    CHECK(transitions->at(1).activePage == 0);
    for ( const auto& event : *transitions )
    {
        CHECK(event.id == fixture.bar->GetId());
        CHECK(event.object == fixture.bar.get());
        CHECK(event.page == fixture.page.get());
        CHECK(event.mode == wxRIBBON_BAR_EXPANDED);
    }
    checkIdentities();
    invokeCommand.Invoke();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return fixture.bar->GetDisplayMode() == wxRIBBON_BAR_MINIMIZED &&
               !RibbonNativeElementIsVisible(command);
    }));
    REQUIRE(commandModes->size() == 1);
    CHECK(commandModes->at(0) == wxRIBBON_BAR_EXPANDED);
    CHECK_FALSE(fixture.bar->ArePanelsShown());
    CHECK(barRoot.Children().GetAt(1).Visibility() == MUX::Visibility::Collapsed);
    CHECK(transitions->size() == 2);
    checkIdentities();

    fixture.bar->ShowPanels(wxRIBBON_BAR_PINNED);
    REQUIRE(WaitForToolkitDispatch([&]() { return RibbonNativeElementIsVisible(command); }));
    invokeCommand.Invoke();
    REQUIRE(commandModes->size() == 2);
    CHECK(commandModes->at(1) == wxRIBBON_BAR_PINNED);
    CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_PINNED);
    CHECK(fixture.bar->ArePanelsShown());
    CHECK(RibbonNativeElementIsVisible(command));
    CHECK(barRoot.Children().GetAt(1).Visibility() == MUX::Visibility::Visible);
    CHECK(transitions->size() == 2);
    checkIdentities();
}

TEST_CASE("RibbonToolkit::VetoedNativeTabSelectionKeepsExpandedModeAndOriginalPage",
          "[winui-ribbon-toolkit][winui-ribbon-display-mode]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    RibbonIconFixture fixture;
    fixture.Create();
    const auto tabs = fixture.Tabs();
    const auto firstTab = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto otherTab = tabs.TabItems().GetAt(1).as<MUXC::TabViewItem>();
    const auto firstRoot = fixture.Presentation(fixture.page.get());
    const auto otherRoot = fixture.Presentation(fixture.other.get());
    const auto panelRoot = fixture.Presentation(fixture.panel.get());
    const auto selectOther = GetToolkitPattern<Providers::ISelectionItemProvider>(
        otherTab, Patterns::PatternInterface::SelectionItem);
    REQUIRE(selectOther);
    const auto transitions = std::make_shared<std::vector<RibbonPageTransition>>();
    const auto veto = std::make_shared<bool>(true);
    fixture.bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGING,
        [bar = fixture.bar, transitions, veto](wxRibbonBarEvent& event)
        {
            REQUIRE(bar);
            transitions->push_back({event.GetEventType(), event.GetId(), event.GetEventObject(),
                event.GetPage(), bar->GetDisplayMode(), bar->GetActivePage()});
            if ( *veto ) event.Veto();
        });
    fixture.bar->Bind(wxEVT_RIBBONBAR_PAGE_CHANGED,
        [bar = fixture.bar, transitions](wxRibbonBarEvent& event)
        {
            REQUIRE(bar);
            transitions->push_back({event.GetEventType(), event.GetId(), event.GetEventObject(),
                event.GetPage(), bar->GetDisplayMode(), bar->GetActivePage()});
        });
    fixture.bar->ShowPanels(wxRIBBON_BAR_MINIMIZED);
    CHECK(transitions->empty());
    selectOther.Select();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return fixture.bar->GetDisplayMode() == wxRIBBON_BAR_EXPANDED &&
               fixture.bar->GetActivePage() == 0 && tabs.SelectedItem() == firstTab;
    }));
    // Generic OnMouseLeftDown expands before asking PAGE_CHANGING. A veto
    // restores the previous tab, not the previous minimized display mode.
    REQUIRE(transitions->size() == 1);
    CHECK(transitions->front().type == wxEVT_RIBBONBAR_PAGE_CHANGING);
    CHECK(transitions->front().activePage == 0);
    CHECK(fixture.bar->ArePanelsShown());
    CHECK(fixture.Presentation(fixture.page.get()) == firstRoot);
    CHECK(fixture.Presentation(fixture.other.get()) == otherRoot);
    CHECK(fixture.Presentation(fixture.panel.get()) == panelRoot);
    *veto = false;
    selectOther.Select();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return fixture.bar->GetActivePage() == 1 && tabs.SelectedItem() == otherTab;
    }));
    REQUIRE(transitions->size() == 3);
    CHECK(transitions->at(1).type == wxEVT_RIBBONBAR_PAGE_CHANGING);
    CHECK(transitions->at(1).activePage == 0);
    CHECK(transitions->at(2).type == wxEVT_RIBBONBAR_PAGE_CHANGED);
    CHECK(transitions->at(2).activePage == 1);
    for ( const auto& event : *transitions )
    {
        CHECK(event.id == fixture.bar->GetId());
        CHECK(event.object == fixture.bar.get());
        CHECK(event.page == fixture.other.get());
        CHECK(event.mode == wxRIBBON_BAR_EXPANDED);
    }
    CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_EXPANDED);
    CHECK(fixture.bar->ArePanelsShown());
    CHECK(fixture.Tabs() == tabs);
    CHECK(tabs.TabItems().GetAt(0) == firstTab);
    CHECK(tabs.TabItems().GetAt(1) == otherTab);
    CHECK(fixture.Presentation(fixture.page.get()) == firstRoot);
    CHECK(fixture.Presentation(fixture.other.get()) == otherRoot);
    CHECK(fixture.Presentation(fixture.panel.get()) == panelRoot);
}

TEST_CASE("RibbonToolkit::NativeFocusBoundaryClosesOnlyTemporaryDisplay",
          "[winui-ribbon-toolkit][winui-ribbon-display-mode][winui-ribbon-focus]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    const bool leaveFromCommand = GENERATE(false, true);
    CAPTURE(leaveFromCommand);
    RibbonExtensionFixture fixture;
    fixture.Create();
    // The private-desktop runner supplies an isolated, non-input desktop. These
    // are real programmatic XAML focus moves, never SendInput or wxFocusEvent.
    auto* const outside = new wxButton(fixture.frame.get(), wxID_ANY,
        "Outside Ribbon", wxPoint(10, 285), wxSize(160, 36));
    fixture.frame->SetClientSize(1100, 380);
    fixture.bar->SetSize(0, 0, 1060, 260);
    fixture.frame->Raise();
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(fixture.bar.get());
    REQUIRE(host);
    MUXC::Button outsidePeer{nullptr};
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        if ( wxWinUITopLevelHost::FindSlotOwner(outside) != host ) return false;
        const auto slot = host->FindSlot(outside);
        outsidePeer = slot ? slot->GetContent().try_as<MUXC::Button>() : nullptr;
        return RibbonNativeElementIsVisible(outsidePeer);
    }));
    const auto barRoot = host->FindSlot(fixture.bar.get())->GetContent().as<MUXC::Grid>();
    const auto tabs = barRoot.Children().GetAt(0).as<MUXC::TabView>();
    const auto tab = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto commandRoot = fixture.CommandRoot();
    const auto command = FindToolkitCommand(commandRoot, L"Extension command 0");
    REQUIRE(command);
    const auto group = fixture.Presentation(fixture.panel.get());
    const auto page = fixture.Presentation(fixture.page.get());
    const auto root = tabs.XamlRoot();
    REQUIRE(root);
    CHECK(root == host->GetXamlRoot());
    CHECK(root == command.XamlRoot());
    CHECK(root == outsidePeer.XamlRoot());
    const auto selection = GetToolkitPattern<Providers::ISelectionItemProvider>(
        tab, Patterns::PatternInterface::SelectionItem);
    const auto invoke = GetToolkitPattern<Providers::IInvokeProvider>(
        command, Patterns::PatternInterface::Invoke);
    REQUIRE(selection);
    REQUIRE(invoke);
    const auto commands = std::make_shared<unsigned>(0);
    fixture.commands->Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
        [commands](wxRibbonButtonBarEvent& event)
        {
            CHECK(event.GetId() == RibbonExtensionFixture::PanelId + 1);
            ++*commands;
        });
    const auto hasFocus = [&](const MUXC::Control& control, wxWindow* owner)
    {
        return MUX::Input::FocusManager::GetFocusedElement(root)
                   .try_as<MUX::DependencyObject>() == control &&
               host->GetFocusOwner() == owner;
    };
    const auto settleInternalFocus = [&](const MUXC::Control& control,
                                         wxWindow* owner, wxRibbonDisplayMode mode)
    {
        REQUIRE(control.Focus(MUX::FocusState::Programmatic));
        REQUIRE(WaitForToolkitDispatch([&]() { return hasFocus(control, owner); }));
        // A focus-boundary adapter may defer its decision until the new
        // native owner is known. Exercise those turns before declaring that
        // moving within the ribbon preserved the temporary display.
        auto* const loop = wxEventLoopBase::GetActive();
        REQUIRE(loop);
        for ( unsigned turn = 0; turn != 3; ++turn )
        {
            wxTheApp->ProcessPendingEvents();
            REQUIRE(wxEventLoopBase::GetActive() == loop);
            REQUIRE(loop->DispatchTimeout(5) != 0);
            CHECK(hasFocus(control, owner));
            CHECK(fixture.bar->GetDisplayMode() == mode);
            CHECK(fixture.bar->ArePanelsShown());
        }
    };
    for ( const auto mode : {wxRIBBON_BAR_EXPANDED, wxRIBBON_BAR_PINNED} )
    {
        CAPTURE(mode);
        fixture.bar->ShowPanels(wxRIBBON_BAR_PINNED);
        REQUIRE(WaitForToolkitDispatch([&]() { return RibbonNativeElementIsVisible(command); }));
        settleInternalFocus(tab, fixture.bar.get(), wxRIBBON_BAR_PINNED);
        fixture.bar->ShowPanels(mode);
        settleInternalFocus(tab, fixture.bar.get(), mode);
        if ( leaveFromCommand )
            settleInternalFocus(command, fixture.commands.get(), mode);
        CHECK(RibbonNativeElementIsVisible(command));
        REQUIRE(outsidePeer.Focus(MUX::FocusState::Programmatic));
        const auto expected = mode == wxRIBBON_BAR_EXPANDED
                                ? wxRIBBON_BAR_MINIMIZED : wxRIBBON_BAR_PINNED;
        REQUIRE(WaitForToolkitDispatch([&]()
        {
            return hasFocus(outsidePeer, outside) &&
                   fixture.bar->GetDisplayMode() == expected;
        }));
        CHECK(fixture.bar->ArePanelsShown() == (expected == wxRIBBON_BAR_PINNED));
        CHECK(RibbonNativeElementIsVisible(command) == (expected == wxRIBBON_BAR_PINNED));
        CHECK(barRoot.Children().GetAt(1).Visibility() ==
              (expected == wxRIBBON_BAR_PINNED ? MUX::Visibility::Visible
                                              : MUX::Visibility::Collapsed));
        CHECK(host->FindSlot(fixture.bar.get())->GetContent() == barRoot);
        CHECK(barRoot.Children().GetAt(0) == tabs);
        CHECK(tabs.TabItems().GetAt(0) == tab);
        CHECK(fixture.Presentation(fixture.page.get()) == page);
        CHECK(fixture.Presentation(fixture.panel.get()) == group);
        CHECK(fixture.CommandRoot() == commandRoot);
        CHECK(FindToolkitCommand(commandRoot, L"Extension command 0") == command);
        CHECK(host->FindSlot(outside)->GetContent() == outsidePeer);
        CHECK(selection.IsSelected());
        CHECK(*commands == 0);
    }
    // The retained command provider remains usable after both focus exits;
    // this final pinned invocation must neither recreate nor collapse it.
    invoke.Invoke();
    CHECK(*commands == 1);
    CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_PINNED);
    CHECK(fixture.CommandRoot() == commandRoot);
}

TEST_CASE("RibbonToolkit::NativePopupFocusStaysInsideTemporaryRibbonBoundary",
          "[winui-ribbon-toolkit][winui-ribbon-display-mode][winui-ribbon-focus][winui-ribbon-popup-focus]")
{
    RibbonExtensionFixture fixture;
    fixture.Create();
    auto* const outside = new wxButton(fixture.frame.get(), wxID_ANY,
        "Outside popup Ribbon", wxPoint(10, 285), wxSize(180, 36));
    fixture.frame->SetClientSize(1100, 380);
    fixture.bar->SetSize(0, 0, 1060, 260);
    fixture.frame->Raise();
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(fixture.bar.get());
    REQUIRE(host);
    MUXC::Button outsidePeer{nullptr};
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        if ( wxWinUITopLevelHost::FindSlotOwner(outside) != host ) return false;
        const auto slot = host->FindSlot(outside);
        outsidePeer = slot ? slot->GetContent().try_as<MUXC::Button>() : nullptr;
        return RibbonNativeElementIsVisible(outsidePeer);
    }));
    const auto barRoot = host->FindSlot(fixture.bar.get())->GetContent().as<MUXC::Grid>();
    const auto tabs = barRoot.Children().GetAt(0).as<MUXC::TabView>();
    const auto tab = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto ribbon = fixture.Presentation(fixture.page.get()).as<Labs::Ribbon>();
    const auto group = fixture.Presentation(fixture.panel.get()).as<Labs::RibbonCollapsibleGroup>();
    const auto content = group.Content();
    const auto commandRoot = fixture.CommandRoot();
    const auto command = FindToolkitCommand(commandRoot, L"Extension command 0");
    REQUIRE(command);
    const auto root = host->GetXamlRoot();
    REQUIRE(root);
    REQUIRE(tab.Focus(MUX::FocusState::Programmatic));
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return MUX::Input::FocusManager::GetFocusedElement(root)
                   .try_as<MUXC::TabViewItem>() == tab &&
               host->GetFocusOwner() == fixture.bar.get();
    }));
    fixture.bar->ShowPanels(wxRIBBON_BAR_EXPANDED);
    ribbon.MaxWidth(80);
    ribbon.InvalidateMeasure();
    ribbon.UpdateLayout();
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        ribbon.UpdateLayout();
        return fixture.panel->IsMinimised();
    }));
    wxRibbonFlyoutParts parts;
    REQUIRE(wxRibbonResolveFlyoutParts(group, parts) == wxRibbonFlyoutResolveResult::Ready);
    REQUIRE(fixture.panel->ShowExpanded());
    MUXC::Button extension{nullptr};
    wxStopWatch popupElapsed;
    const bool popupReady = WaitForToolkitDispatch([&]()
    {
        extension = FindRibbonChromeButton(parts.flyout.Content(), L"wxRibbonPanelExtension");
        return fixture.panel->GetExpandedPanel() && parts.flyout.IsOpen() &&
               RibbonNativeElementIsVisible(command) &&
               RibbonNativeElementIsVisible(extension);
    });
    INFO("mode=" << fixture.bar->GetDisplayMode() << " expanded="
         << bool(fixture.panel->GetExpandedPanel()) << " open=" << parts.flyout.IsOpen()
         << " commandLoaded=" << command.IsLoaded() << " commandVisible="
         << RibbonNativeElementIsVisible(command) << " extension=" << bool(extension)
         << " extensionVisible=" << RibbonNativeElementIsVisible(extension)
         << " hostVisible=" << root.IsHostVisible()
         << " frame=" << fixture.frame->GetScreenPosition().x << ","
         << fixture.frame->GetScreenPosition().y << " "
         << fixture.frame->GetSize().x << "x" << fixture.frame->GetSize().y
         << " shown=" << fixture.frame->IsShownOnScreen()
         << " iconized=" << fixture.frame->IsIconized()
         << " elapsed=" << popupElapsed.Time());
    REQUIRE(popupReady);
    wxWeakRef<wxRibbonPanel> expanded(fixture.panel->GetExpandedPanel());
    REQUIRE(expanded);
    auto* const proxyIdentity = expanded.get();
    CHECK(command.XamlRoot() == root);
    CHECK(extension.XamlRoot() == root);
    CHECK(outsidePeer.XamlRoot() == root);
    CHECK(IsInToolkitSubtree(command, parts.popupMount));
    CHECK(IsInToolkitSubtree(extension, parts.flyout.Content()));
    CHECK_FALSE(wxWinUITopLevelHost::CanReparentSubtreeNow(fixture.bar.get()));
    const auto checkPopupFocus = [&](const MUXC::Control& control, wxWindow* slotOwner)
    {
        REQUIRE(control.Focus(MUX::FocusState::Programmatic));
        REQUIRE(WaitForToolkitDispatch([&]()
        {
            return MUX::Input::FocusManager::GetFocusedElement(root)
                       .try_as<MUX::DependencyObject>() == control &&
                   (!slotOwner || host->GetFocusOwner() == slotOwner);
        }));
        auto* const loop = wxEventLoopBase::GetActive();
        REQUIRE(loop);
        for ( unsigned turn = 0; turn != 3; ++turn )
        {
            wxTheApp->ProcessPendingEvents();
            REQUIRE(wxEventLoopBase::GetActive() == loop);
            REQUIRE(loop->DispatchTimeout(5) != 0);
            CHECK(MUX::Input::FocusManager::GetFocusedElement(root)
                      .try_as<MUX::DependencyObject>() == control);
            // The footer is native popup chrome, not a wx slot. For it the
            // exact native element and popup ownership are the authority.
            if ( slotOwner ) CHECK(host->GetFocusOwner() == slotOwner);
            CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_EXPANDED);
            CHECK(fixture.bar->ArePanelsShown());
            CHECK(fixture.panel->GetExpandedPanel() == proxyIdentity);
            CHECK(parts.flyout.IsOpen());
            CHECK(wxRibbonFlyoutPartsAreCurrent(parts));
            CHECK(fixture.CommandRoot() == commandRoot);
            CHECK(group.Content() == content);
        }
    };
    checkPopupFocus(command, fixture.commands.get());
    checkPopupFocus(extension, nullptr);
    CHECK(fixture.events->empty());
    CHECK_FALSE(wxWinUITopLevelHost::CanReparentSubtreeNow(fixture.bar.get()));

    // Programmatic focus leaves the actual popup. Do not synthesize focus
    // events or explicitly HideExpanded: the real boundary must close both
    // temporary presentations and retire the popup's physical gate.
    REQUIRE(outsidePeer.Focus(MUX::FocusState::Programmatic));
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return MUX::Input::FocusManager::GetFocusedElement(root)
                   .try_as<MUXC::Button>() == outsidePeer &&
               host->GetFocusOwner() == outside &&
               fixture.bar->GetDisplayMode() == wxRIBBON_BAR_MINIMIZED &&
               !fixture.panel->GetExpandedPanel() && !parts.flyout.IsOpen() &&
               wxWinUITopLevelHost::CanReparentSubtreeNow(fixture.bar.get());
    }));
    CHECK_FALSE(fixture.bar->ArePanelsShown());
    CHECK_FALSE(RibbonNativeElementIsVisible(command));
    CHECK_FALSE(RibbonNativeElementIsVisible(extension));
    CHECK(fixture.commands->GetParent() == fixture.panel.get());
    CHECK(fixture.events->empty());
    CHECK(host->FindSlot(fixture.bar.get())->GetContent() == barRoot);
    CHECK(barRoot.Children().GetAt(0) == tabs);
    CHECK(tabs.TabItems().GetAt(0) == tab);
    CHECK(fixture.Presentation(fixture.page.get()) == ribbon);
    CHECK(fixture.Presentation(fixture.panel.get()) == group);
    CHECK(fixture.CommandRoot() == commandRoot);
    CHECK(FindToolkitCommand(commandRoot, L"Extension command 0") == command);
    CHECK(group.Content() == content);
    CHECK(wxRibbonFlyoutPartsAreCurrent(parts));
}

TEST_CASE("RibbonToolkit::ClassicHwndFocusExitClosesOnlyTemporaryDisplay",
          "[winui-ribbon-toolkit][winui-ribbon-display-mode][winui-ribbon-focus][winui-ribbon-hwnd-focus]")
{
    namespace Providers = MUX::Automation::Provider;
    namespace Patterns = MUX::Automation::Peers;
    const bool leaveFromCommand = GENERATE(false, true);
    CAPTURE(leaveFromCommand);
    RibbonExtensionFixture fixture;
    fixture.Create();
    auto* const outside = new wxWindow(fixture.frame.get(), wxID_ANY,
        wxPoint(10, 285), wxSize(180, 36), wxBORDER_SIMPLE);
    fixture.frame->SetClientSize(1100, 380);
    fixture.bar->SetSize(0, 0, 1060, 260);
    fixture.frame->Raise();
    auto* const host = wxWinUITopLevelHost::FindSlotOwner(fixture.bar.get());
    REQUIRE(host);
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(outside) == nullptr);
    REQUIRE(host->FindSlot(outside) == nullptr);
    const auto outsideHwnd = static_cast<HWND>(outside->GetHWND());
    REQUIRE(outsideHwnd);
    REQUIRE(::IsWindow(outsideHwnd));
    REQUIRE(outside->AcceptsFocus());
    REQUIRE(outside->IsShownOnScreen());
    const auto barRoot = host->FindSlot(fixture.bar.get())->GetContent().as<MUXC::Grid>();
    const auto tabs = barRoot.Children().GetAt(0).as<MUXC::TabView>();
    const auto tab = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto commandRoot = fixture.CommandRoot();
    const auto command = FindToolkitCommand(commandRoot, L"Extension command 0");
    REQUIRE(command);
    const auto group = fixture.Presentation(fixture.panel.get());
    const auto root = host->GetXamlRoot();
    REQUIRE(root);
    const auto selection = GetToolkitPattern<Providers::ISelectionItemProvider>(
        tab, Patterns::PatternInterface::SelectionItem);
    const auto invoke = GetToolkitPattern<Providers::IInvokeProvider>(
        command, Patterns::PatternInterface::Invoke);
    REQUIRE(selection);
    REQUIRE(invoke);
    const auto commandCalls = std::make_shared<unsigned>(0);
    fixture.commands->Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
        [commandCalls](wxRibbonButtonBarEvent& event)
        {
            CHECK(event.GetId() == RibbonExtensionFixture::PanelId + 1);
            ++*commandCalls;
        });
    const auto focusedInside = [&](const MUXC::Control& control, wxWindow* owner)
    {
        return MUX::Input::FocusManager::GetFocusedElement(root)
                   .try_as<MUX::DependencyObject>() == control &&
               host->GetFocusOwner() == owner && ::GetFocus() != outsideHwnd;
    };
    const auto focusedOutside = [&]()
    {
        // FocusManager may retain the former leaf after the keyboard leaves
        // its island. The actual HWND and wx focus owner are authoritative.
        return wxWindow::FindFocus() == outside && ::GetFocus() == outsideHwnd;
    };
    for ( const auto mode : {wxRIBBON_BAR_EXPANDED, wxRIBBON_BAR_PINNED} )
    {
        CAPTURE(mode);
        fixture.bar->ShowPanels(wxRIBBON_BAR_PINNED);
        REQUIRE(WaitForToolkitDispatch([&]() { return RibbonNativeElementIsVisible(command); }));
        // Return the real Win32 keyboard focus to the island before moving
        // to a particular native tab, including after the first HWND exit.
        fixture.bar->SetFocus();
        REQUIRE(tab.Focus(MUX::FocusState::Programmatic));
        REQUIRE(WaitForToolkitDispatch([&]()
            { return focusedInside(tab, fixture.bar.get()); }));
        fixture.bar->ShowPanels(mode);
        if ( leaveFromCommand )
        {
            REQUIRE(command.Focus(MUX::FocusState::Programmatic));
            REQUIRE(WaitForToolkitDispatch([&]()
                { return focusedInside(command, fixture.commands.get()); }));
        }
        auto* const loop = wxEventLoopBase::GetActive();
        REQUIRE(loop);
        for ( unsigned turn = 0; turn != 3; ++turn )
        {
            wxTheApp->ProcessPendingEvents();
            REQUIRE(wxEventLoopBase::GetActive() == loop);
            REQUIRE(loop->DispatchTimeout(5) != 0);
            CHECK(fixture.bar->GetDisplayMode() == mode);
            CHECK(fixture.bar->ArePanelsShown());
            CHECK((leaveFromCommand ? focusedInside(command, fixture.commands.get())
                                    : focusedInside(tab, fixture.bar.get())));
        }
        const auto beforeExit = MUX::Input::FocusManager::GetFocusedElement(root);
        outside->SetFocus();
        const auto expected = mode == wxRIBBON_BAR_EXPANDED
                                ? wxRIBBON_BAR_MINIMIZED : wxRIBBON_BAR_PINNED;
        const bool exited = WaitForToolkitDispatch([&]()
        {
            return focusedOutside() && fixture.bar->GetDisplayMode() == expected;
        });
        INFO("retained XAML leaf=" <<
             (MUX::Input::FocusManager::GetFocusedElement(root) == beforeExit)
             << ", wx outside=" << (wxWindow::FindFocus() == outside)
             << ", HWND outside=" << (::GetFocus() == outsideHwnd));
        REQUIRE(exited);
        CHECK(fixture.bar->ArePanelsShown() == (expected == wxRIBBON_BAR_PINNED));
        CHECK(RibbonNativeElementIsVisible(command) == (expected == wxRIBBON_BAR_PINNED));
        CHECK(barRoot.Children().GetAt(1).Visibility() ==
              (expected == wxRIBBON_BAR_PINNED ? MUX::Visibility::Visible
                                              : MUX::Visibility::Collapsed));
        CHECK(host->FindSlot(fixture.bar.get())->GetContent() == barRoot);
        CHECK(barRoot.Children().GetAt(0) == tabs);
        CHECK(tabs.TabItems().GetAt(0) == tab);
        CHECK(fixture.Presentation(fixture.panel.get()) == group);
        CHECK(fixture.CommandRoot() == commandRoot);
        CHECK(FindToolkitCommand(commandRoot, L"Extension command 0") == command);
        CHECK(selection.IsSelected());
        CHECK(*commandCalls == 0);
    }
    invoke.Invoke();
    CHECK(*commandCalls == 1);
    CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_PINNED);
    CHECK(fixture.CommandRoot() == commandRoot);
}

namespace
{
struct RibbonVerticalFixture
{
    ~RibbonVerticalFixture()
    {
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(bool rtl, bool adaptive)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "Native vertical ribbon",
                            wxDefaultPosition, wxSize(560, 420));
        bar = new wxRibbonBar(frame.get(), wxID_ANY, wxPoint(0, 0), wxSize(540, 360),
                             wxRIBBON_BAR_DEFAULT_STYLE | wxRIBBON_BAR_FLOW_VERTICAL);
        if ( rtl ) bar->SetLayoutDirection(wxLayout_RightToLeft);
        page = new wxRibbonPage(bar.get(), wxID_HIGHEST + 5810, "Vertical page");
        const auto bitmap = wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(32, 32));
        REQUIRE(bitmap.IsOk());
        for ( int i = 0; i != (adaptive ? 3 : 5); ++i )
        {
            auto* const ribbonPanel = new wxRibbonPanel(page.get(), wxID_ANY,
                wxString::Format("Vertical group %d", i), bitmap,
                wxDefaultPosition, wxDefaultSize,
                adaptive ? wxRIBBON_PANEL_DEFAULT_STYLE : wxRIBBON_PANEL_NO_AUTO_MINIMISE);
            auto* const command = new wxRibbonButtonBar(ribbonPanel);
            REQUIRE(command->AddButton(wxID_HIGHEST + 5820 + i,
                wxString::Format("Vertical action %d", i), bitmap));
            command->Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
                [calls = calls](wxRibbonButtonBarEvent&) { ++*calls; });
            auto* const sizer = new wxBoxSizer(wxHORIZONTAL);
            sizer->Add(command, 1, wxEXPAND);
            sizer->SetMinSize(ribbonPanel->FromDIP(wxSize(230, adaptive ? 240 : 160)));
            ribbonPanel->SetSizer(sizer);
            panels.emplace_back(ribbonPanel);
            commands.emplace_back(command);
        }
        REQUIRE(bar->Realize());
        frame->Show();
        Ready(true);
        for ( size_t i = 0; i != panels.size(); ++i )
        {
            groups.push_back(Presentation(panels[i].get()));
            commandRoots.push_back(Presentation(commands[i].get()));
            REQUIRE(groups.back());
            REQUIRE(commandRoots.back());
        }
    }

    MUX::UIElement Presentation(wxWindow* owner) const
    {
        const auto host = owner ? wxWinUITopLevelHost::FindSlotOwner(owner) : nullptr;
        const auto slot = host ? host->FindSlot(owner) : nullptr;
        return slot ? slot->GetPresentationRoot() : nullptr;
    }

    bool IsReady(bool vertical)
    {
        if ( !bar || !page ) return false;
        const auto host = wxWinUITopLevelHost::FindSlotOwner(page.get());
        if ( !host ) return false;
        host->FlushSync();
        ribbon = Presentation(page.get()).try_as<Labs::Ribbon>();
        if ( !ribbon ) return false;
        ribbon.UpdateLayout();
        const auto control = ribbon.as<MUXC::IControlProtected>();
        scroll = control.GetTemplateChild(L"ScrollViewer").try_as<MUXC::ScrollViewer>();
        panel = control.GetTemplateChild(L"Panel").try_as<MUXC::Panel>();
        if ( !scroll || !panel || !scroll.IsLoaded() || scroll.Content() != panel ||
             scroll.ViewportWidth() <= 0 || scroll.ViewportHeight() <= 0 ) return false;
        const auto stack = panel.try_as<MUXC::StackPanel>();
        if ( vertical != bool(stack) ||
             (stack && stack.Orientation() != MUXC::Orientation::Vertical) ) return false;
        for ( size_t i = 0; i != panels.size(); ++i )
            if ( !panels[i] || !commands[i] || !Presentation(commands[i].get()) ||
                 !Presentation(panels[i].get()).as<MUX::FrameworkElement>().IsLoaded() )
                return false;
        return true;
    }

    void Ready(bool vertical) { REQUIRE(WaitForToolkitDispatch([&]() { return IsReady(vertical); })); }

    void CheckIdentities()
    {
        for ( size_t i = 0; i != panels.size(); ++i )
        {
            CHECK(Presentation(panels[i].get()) == groups[i]);
            CHECK(Presentation(commands[i].get()) == commandRoots[i]);
            CHECK(ribbon.Items().GetAt(static_cast<uint32_t>(i)) == groups[i]);
            CHECK(commands[i]->GetParent() == panels[i].get());
        }
    }

    wxWeakRef<wxFrame> frame;
    wxWeakRef<wxRibbonBar> bar;
    wxWeakRef<wxRibbonPage> page;
    std::vector<wxWeakRef<wxRibbonPanel>> panels;
    std::vector<wxWeakRef<wxRibbonButtonBar>> commands;
    std::vector<MUX::UIElement> groups;
    std::vector<MUX::UIElement> commandRoots;
    Labs::Ribbon ribbon{nullptr};
    MUXC::ScrollViewer scroll{nullptr};
    MUXC::Panel panel{nullptr};
    std::shared_ptr<unsigned> calls = std::make_shared<unsigned>(0);
};
}

TEST_CASE("RibbonToolkit::VerticalPageLayoutScrollAndStyleSwitchPreservePeers",
          "[winui-ribbon-toolkit][winui-ribbon-vertical]")
{
    const bool rtl = GENERATE(false, true);
    CAPTURE(rtl);
    RibbonVerticalFixture fixture;
    fixture.Create(rtl, false);
    const auto originalRibbon = fixture.ribbon;
    const auto oldScroll = fixture.scroll;
    const auto oldScrollProvider = GetToolkitPattern<MUX::Automation::Provider::IScrollProvider>(
        oldScroll, MUX::Automation::Peers::PatternInterface::Scroll);
    REQUIRE(oldScrollProvider);
    CHECK(oldScrollProvider.VerticallyScrollable());
    CHECK_FALSE(oldScrollProvider.HorizontallyScrollable());
    const auto command = FindToolkitCommand(fixture.commandRoots[0], L"Vertical action 0");
    REQUIRE(command);
    const auto invoke = GetToolkitPattern<MUX::Automation::Provider::IInvokeProvider>(
        command, MUX::Automation::Peers::PatternInterface::Invoke);
    REQUIRE(invoke);
    CHECK(fixture.page->GetMajorAxis() == wxVERTICAL);
    const double scale = fixture.page->GetDPIScaleFactor();
    const auto at = [&](double value)
        { return std::abs(fixture.scroll.VerticalOffset() - value) < 0.25; };
    REQUIRE(fixture.scroll.ScrollableHeight() > 80);
    CHECK(fixture.scroll.ScrollableWidth() == 0);
    float previousEnd = 0;
    for ( const auto& group : fixture.groups )
    {
        const auto rect = MUXC::Primitives::LayoutInformation::GetLayoutSlot(
            group.as<MUX::FrameworkElement>());
        CHECK(std::abs(rect.X) < 0.5);
        CHECK(rect.Y >= previousEnd - 0.5);
        REQUIRE(rect.Height > 100);
        previousEnd = rect.Y + rect.Height;
    }
    REQUIRE(fixture.page->ScrollLines(1));
    REQUIRE(fixture.page->ScrollPixels(13));
    REQUIRE(WaitForToolkitDispatch([&]() { return at(21.0 / scale); }));
    REQUIRE(fixture.page->ScrollSections(-1));
    REQUIRE(WaitForToolkitDispatch([&]() { return at(0); }));
    CHECK_FALSE(fixture.page->ScrollSections(-1));
    unsigned steps = 0;
    while ( steps < 16 && fixture.page->ScrollSections(1) ) ++steps;
    REQUIRE(steps > 0);
    REQUIRE(steps < 16);
    REQUIRE(WaitForToolkitDispatch([&]() { return at(fixture.scroll.ScrollableHeight()); }));
    CHECK_FALSE(fixture.page->ScrollSections(1));
    REQUIRE(fixture.page->ScrollLines((std::numeric_limits<int>::min)()));
    REQUIRE(WaitForToolkitDispatch([&]() { return at(0); }));
    oldScrollProvider.SetScrollPercent(-1, 50);
    REQUIRE(WaitForToolkitDispatch([&]() { return at(fixture.scroll.ScrollableHeight() / 2); }));
    const double middle = fixture.scroll.VerticalOffset();
    REQUIRE(fixture.page->ScrollPixels(-17));
    REQUIRE(WaitForToolkitDispatch([&]() { return at(middle - 17.0 / scale); }));

    fixture.bar->SetWindowStyleFlag(fixture.bar->GetWindowStyleFlag() & ~wxRIBBON_BAR_FLOW_VERTICAL);
    fixture.Ready(false);
    CHECK(fixture.page->GetMajorAxis() == wxHORIZONTAL);
    CHECK(fixture.ribbon == originalRibbon);
    CHECK(fixture.scroll != oldScroll);
    fixture.CheckIdentities();
    REQUIRE(fixture.scroll.ScrollableWidth() > 80);
    REQUIRE(fixture.page->ScrollPixels(19));
    REQUIRE(WaitForToolkitDispatch([&]()
        { return std::abs(fixture.scroll.HorizontalOffset() - 19.0 / scale) < 0.25; }));
    fixture.bar->SetWindowStyleFlag(fixture.bar->GetWindowStyleFlag() | wxRIBBON_BAR_FLOW_VERTICAL);
    fixture.Ready(true);
    fixture.CheckIdentities();
    CHECK(fixture.ribbon == originalRibbon);
    CHECK(fixture.scroll != oldScroll);
    CHECK(at(0));
    try { oldScrollProvider.SetScrollPercent(-1, 75); }
    catch ( const winrt::hresult_error& ) { /* detached provider may reject */ }
    wxYield();
    CHECK(at(0));
    fixture.bar->SetSize(wxSize(540, 1600));
    REQUIRE(WaitForToolkitDispatch([&]()
        { return fixture.IsReady(true) && fixture.scroll.ScrollableHeight() < 0.01; }));
    CHECK_FALSE(fixture.page->ScrollLines(1));
    CHECK_FALSE(fixture.page->ScrollSections(1));
    fixture.bar->SetSize(wxSize(540, 360));
    REQUIRE(WaitForToolkitDispatch([&]()
        { return fixture.IsReady(true) && fixture.scroll.ScrollableHeight() > 80; }));
    fixture.CheckIdentities();
    invoke.Invoke();
    CHECK(*fixture.calls == 1);
}

TEST_CASE("RibbonToolkit::VerticalAdaptationAndPopupRetirementBeforeAxisSwitch",
          "[winui-ribbon-toolkit][winui-ribbon-vertical]")
{
    RibbonVerticalFixture fixture;
    fixture.Create(false, true);
    const auto anyCollapsed = [&]()
    {
        return std::any_of(fixture.panels.begin(), fixture.panels.end(),
            [](const auto& panel) { return panel->IsMinimised(); });
    };
    const bool initiallyAdapted = WaitForToolkitDispatch([&]()
        { return fixture.IsReady(true) && anyCollapsed(); });
    std::ostringstream adaptation;
    adaptation << "viewport=" << fixture.scroll.ViewportWidth() << 'x'
               << fixture.scroll.ViewportHeight() << ", extent="
               << fixture.scroll.ExtentWidth() << 'x' << fixture.scroll.ExtentHeight();
    for ( const auto& element : fixture.groups )
    {
        const auto group = element.as<Labs::RibbonCollapsibleGroup>();
        const auto size = group.DesiredSize();
        const auto content = group.Content().as<MUX::FrameworkElement>();
        const auto contentSize = content.DesiredSize();
        adaptation << "; state=" << static_cast<int>(group.State())
                   << ", desired=" << size.Width << 'x' << size.Height
                   << ", actual=" << group.ActualWidth() << 'x' << group.ActualHeight()
                   << ", height/min/max=" << group.Height() << '/'
                   << group.MinHeight() << '/' << group.MaxHeight()
                   << ", content=" << contentSize.Width << 'x' << contentSize.Height;
    }
    INFO(adaptation.str());
    REQUIRE(initiallyAdapted);
    fixture.bar->SetSize(wxSize(540, 1400));
    REQUIRE(WaitForToolkitDispatch([&]() { return fixture.IsReady(true) && !anyCollapsed(); }));
    fixture.CheckIdentities();
    fixture.bar->SetSize(wxSize(540, 360));
    REQUIRE(WaitForToolkitDispatch([&]() { return fixture.IsReady(true) && anyCollapsed(); }));
    const auto panel = *std::find_if(fixture.panels.begin(), fixture.panels.end(),
        [](const auto& candidate) { return candidate->IsMinimised(); });
    REQUIRE(panel);
    const auto group = fixture.Presentation(panel.get()).as<Labs::RibbonCollapsibleGroup>();
    wxRibbonFlyoutParts parts;
    REQUIRE(wxRibbonReadCurrentFlyoutParts(group, parts) == wxRibbonFlyoutResolveResult::Ready);
    REQUIRE(panel->ShowExpanded());
    REQUIRE(WaitForToolkitDispatch([&]()
        { return panel && panel->GetExpandedPanel() && parts.flyout.IsOpen(); }));
    CHECK_FALSE(wxWinUITopLevelHost::CanReparentSubtreeNow(fixture.bar.get()));
    const auto oldTemplate = fixture.ribbon.Template();
    fixture.bar->SetWindowStyleFlag(fixture.bar->GetWindowStyleFlag() & ~wxRIBBON_BAR_FLOW_VERTICAL);
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        return fixture.IsReady(false) && !panel->GetExpandedPanel() && !parts.flyout.IsOpen() &&
               wxWinUITopLevelHost::CanReparentSubtreeNow(fixture.bar.get());
    }));
    CHECK(fixture.ribbon.Template() != oldTemplate);
    fixture.CheckIdentities();
}

TEST_CASE("RibbonToolkit::AxisTemplateCallbackCanDeleteTheBar",
          "[winui-ribbon-toolkit][winui-ribbon-vertical]")
{
    RibbonVerticalFixture fixture;
    fixture.Create(false, false);
    const auto ribbon = fixture.ribbon;
    unsigned calls = 0;
    const auto token = ribbon.RegisterPropertyChangedCallback(MUXC::Control::TemplateProperty(),
        [&](const auto&, const auto&)
        {
            if ( ++calls == 1 && fixture.bar ) delete fixture.bar.get();
        });
    const auto unhook = wxMakeGuard([&]()
        { ribbon.UnregisterPropertyChangedCallback(MUXC::Control::TemplateProperty(), token); });
    fixture.bar->SetWindowStyleFlag(fixture.bar->GetWindowStyleFlag() & ~wxRIBBON_BAR_FLOW_VERTICAL);
    REQUIRE(WaitForToolkitDispatch([&]() { return !fixture.bar; }));
    CHECK(calls == 1);
    CHECK_FALSE(fixture.page);
}

TEST_CASE("RibbonToolkit::NativeTabMarginsReserveOnlyTheHeader",
          "[winui-ribbon-toolkit][winui-ribbon-tab-margins]")
{
    const bool rightToLeft = GENERATE(false, true);
    CAPTURE(rightToLeft);
    RibbonChromeFixture fixture;
    fixture.Create(RibbonChromeFixture::ChromeFlags);
    fixture.bar->SetLayoutDirection(rightToLeft ? wxLayout_RightToLeft : wxLayout_LeftToRight);
    const auto host = wxWinUITopLevelHost::FindSlotOwner(fixture.bar.get());
    REQUIRE(host);
    const auto root = fixture.Root();
    const auto tabs = fixture.Tabs();
    const auto item = tabs.TabItems().GetAt(0).as<MUXC::TabViewItem>();
    const auto pages = root.Children().GetAt(1).as<MUX::FrameworkElement>();
    const auto help = fixture.Button(true);
    const auto helpProvider = GetToolkitPattern<MUX::Automation::Provider::IInvokeProvider>(
        help, MUX::Automation::Peers::PatternInterface::Invoke);
    REQUIRE(helpProvider);
    const auto bounds = [&](const MUX::FrameworkElement& element)
    {
        return element.TransformToVisual(host->Root()).TransformBounds(
            {0, 0, static_cast<float>(element.ActualWidth()),
             static_cast<float>(element.ActualHeight())});
    };
    const auto checkMargins = [&](int left, int right)
    {
        const double scale = fixture.bar->GetDPIScaleFactor();
        REQUIRE(scale > 0);
        REQUIRE(WaitForToolkitDispatch([&]()
        {
            host->FlushSync();
            root.UpdateLayout();
            return tabs.IsLoaded() && tabs.ActualWidth() > 0 &&
                   std::abs(tabs.ActualWidth() - root.ActualWidth() +
                            (left + right) / scale) <= 1.0 / scale;
        }));
        const auto rootBounds = bounds(root);
        const auto tabsBounds = bounds(tabs);
        const auto bodyBounds = bounds(pages);
        const double physicalLeft = rightToLeft ? right : left;
        const double physicalRight = rightToLeft ? left : right;
        CHECK(std::abs(tabsBounds.X - rootBounds.X - physicalLeft / scale) <= 1.0 / scale);
        CHECK(std::abs(rootBounds.X + rootBounds.Width - tabsBounds.X - tabsBounds.Width -
                       physicalRight / scale) <= 1.0 / scale);
        // The page/command presentation is a sibling, not inset along with
        // the tab strip. These are real arranged rectangles, not DP values.
        CHECK(std::abs(bodyBounds.X - rootBounds.X) <= 1.0 / scale);
        CHECK(std::abs(bodyBounds.Width - rootBounds.Width) <= 1.0 / scale);
        CHECK(fixture.Root() == root);
        CHECK(fixture.Tabs() == tabs);
        CHECK(tabs.TabItems().GetAt(0) == item);
        CHECK(fixture.Button(true) == help);
        CHECK(fixture.bar->GetActivePage() == 0);
        CHECK(fixture.bar->GetDisplayMode() == wxRIBBON_BAR_PINNED);
    };
    // Default wx margins are already meaningful before the first explicit
    // setter; both enabled native chrome buttons add to the right reserve.
    checkMargins(50, 60);
    for ( const auto& margin : {wxSize(37, 91), wxSize(89, 13), wxSize(0, 0)} )
    {
        fixture.bar->SetTabCtrlMargins(margin.x, margin.y);
        checkMargins(margin.x, margin.y); // no Realize is necessary
    }
    fixture.bar->SetTabCtrlMargins(37, 91);
    fixture.bar->SetSize(wxSize(640, 240));
    checkMargins(37, 91);
    fixture.bar->SetSize(wxSize(840, 240));
    checkMargins(37, 91);
    REQUIRE(fixture.bar->Realize());
    checkMargins(37, 91);
    helpProvider.Invoke();
    fixture.CheckEvent(1, wxEVT_RIBBONBAR_HELP_CLICK, wxRIBBON_BAR_PINNED);
}

TEST_CASE("RibbonToolkit::NativePageScrollingUsesTheTemplateViewport",
          "[winui-ribbon-toolkit][winui-ribbon-page-scroll]")
{
    const bool rightToLeft = GENERATE(false, true);
    const bool keepOldPage = GENERATE(false, true);
    CAPTURE(keepOldPage);
    wxWeakRef<wxFrame> frame(new wxFrame(nullptr, wxID_ANY, "Native ribbon page scroll",
                                        wxDefaultPosition, wxSize(600, 330)));
    const auto cleanup = wxMakeGuard([&]()
    {
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    });
    auto* const bar = new wxRibbonBar(frame.get(), wxID_ANY, wxPoint(0, 0), wxSize(400, 240));
    if ( rightToLeft ) bar->SetLayoutDirection(wxLayout_RightToLeft);
    // This ID is intentionally reused after retiring the page. A released
    // wxID_ANY-generated ID belongs to the auto-ID allocator and is not a
    // legal application ID to reuse without reserving it again.
    constexpr wxWindowID pageId = wxID_HIGHEST + 5690;
    auto* const page = new wxRibbonPage(bar, pageId, "Scrollable page");
    std::vector<wxRibbonPanel*> panels;
    const auto bitmap = wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(32, 32));
    REQUIRE(bitmap.IsOk());
    for ( int i = 0; i != 5; ++i )
    {
        auto* const panel = new wxRibbonPanel(page, wxID_ANY,
            wxString::Format("Wide native group %d", i), wxBitmapBundle(),
            wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* const commands = new wxRibbonButtonBar(panel);
        REQUIRE(commands->AddButton(wxID_HIGHEST + 5700 + i, "Native command", bitmap));
        auto* const sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(commands, 1, wxEXPAND | wxALL, 4);
        // RibbonButtonBar::GetMinSize() reports its adaptive command layout,
        // not wxWindow's explicit minimum. A fixed group-content minimum is
        // the sizer's public contract and is projected by LayoutCommands().
        sizer->SetMinSize(panel->FromDIP(wxSize(230, 80)));
        panel->SetSizer(sizer);
        panels.push_back(panel);
    }
    REQUIRE(bar->Realize());
    CHECK_FALSE(page->ScrollPixels(0));
    CHECK_FALSE(page->ScrollLines(-1));
    frame->Show();
    const auto host = wxWinUITopLevelHost::FindSlotOwner(page);
    REQUIRE(host);
    const auto slot = host->FindSlot(page);
    REQUIRE(slot);
    const auto ribbon = slot->GetPresentationRoot().as<Labs::Ribbon>();
    REQUIRE(ribbon);
    const auto control = ribbon.as<MUXC::IControlProtected>();
    MUXC::ScrollViewer scroll{nullptr};
    const bool overflowReady = WaitForToolkitDispatch([&]()
    {
        host->FlushSync();
        ribbon.UpdateLayout();
        scroll = control.GetTemplateChild(L"ScrollViewer").try_as<MUXC::ScrollViewer>();
        return scroll && scroll.IsLoaded() && scroll.ViewportWidth() > 0 &&
               scroll.ScrollableWidth() > 80;
    });
    CAPTURE(rightToLeft, bar->GetSize(), page->GetSize(), page->GetDPIScaleFactor());
    INFO("ribbon=" << ribbon.IsLoaded() << " " << ribbon.ActualWidth() << "x"
         << ribbon.ActualHeight() << ", scroll=" << bool(scroll)
         << ", loaded=" << (scroll && scroll.IsLoaded())
         << ", viewport=" << (scroll ? scroll.ViewportWidth() : -1)
         << ", extent=" << (scroll ? scroll.ExtentWidth() : -1)
         << ", limit=" << (scroll ? scroll.ScrollableWidth() : -1));
    std::ostringstream groups;
    for ( unsigned i = 0; i != ribbon.Items().Size(); ++i )
    {
        const auto group = ribbon.Items().GetAt(i).as<Labs::RibbonGroup>();
        const auto content = group.Content().try_as<MUX::FrameworkElement>();
        groups << "group" << i << " collapsible=" << bool(group.try_as<Labs::RibbonCollapsibleGroup>())
             << " loaded=" << group.IsLoaded() << " desired=" << group.DesiredSize().Width
             << " actual=" << group.ActualWidth() << ", content desired="
             << (content ? content.DesiredSize().Width : -1)
             << ", wx min=" << panels[i]->GetSizer()->GetMinSize().x << "; ";
    }
    INFO(groups.str());
    REQUIRE(overflowReady);
    const auto nativePanel = control.GetTemplateChild(L"Panel").as<MUXC::Panel>();
    REQUIRE(scroll.Content() == nativePanel);
    const auto measureState = [](const Labs::Ribbon& measuredRibbon)
    {
        std::ostringstream measured;
        const auto measuredControl = measuredRibbon.as<MUXC::IControlProtected>();
        const auto measuredScroll = measuredControl.GetTemplateChild(L"ScrollViewer")
            .as<MUXC::ScrollViewer>();
        const auto measuredPanel = measuredControl.GetTemplateChild(L"Panel")
            .as<MUXC::Panel>();
        const auto measuredPresenter = measuredScroll.as<MUXC::IControlProtected>()
            .GetTemplateChild(L"ScrollContentPresenter").as<MUXC::ScrollContentPresenter>();
        const auto writeElement = [&](const char* name, const MUX::FrameworkElement& element)
        {
            const auto allocated = MUXC::Primitives::LayoutInformation::GetLayoutSlot(element);
            const auto desired = element.DesiredSize();
            measured << ' ' << name << " desired=" << desired.Width << ',' << desired.Height
                << " actual=" << element.ActualWidth() << ',' << element.ActualHeight()
                << " allocation=" << allocated.X << ',' << allocated.Y << ','
                << allocated.Width << ',' << allocated.Height
                << " width=" << element.Width() << " min=" << element.MinWidth()
                << " max=" << element.MaxWidth()
                << " alignment=" << static_cast<int>(element.HorizontalAlignment());
        };
        writeElement("ribbon", measuredRibbon);
        writeElement("scroll", measuredScroll);
        writeElement("presenter", measuredPresenter);
        writeElement("panel", measuredPanel);
        measured << " sizesToParent=" << measuredPresenter.SizesContentToTemplatedParent()
            << " ownerExact=" << (measuredPresenter.ScrollOwner() == measuredScroll)
            << " contentExact=" << (measuredPresenter.Content() == measuredPanel)
            << " offsets=" << measuredScroll.HorizontalOffset() << ','
            << measuredPresenter.HorizontalOffset()
            << " extents=" << measuredScroll.ExtentWidth() << ',' << measuredPresenter.ExtentWidth()
            << " viewports=" << measuredScroll.ViewportWidth() << ',' << measuredPresenter.ViewportWidth();
        return measured.str();
    };
    const auto initialMeasureState = measureState(ribbon);
    const auto first = ribbon.Items().GetAt(0).as<Labs::RibbonGroup>();
    const auto last = ribbon.Items().GetAt(4).as<Labs::RibbonGroup>();
    const auto firstMount = first.Content();
    const double scale = page->GetDPIScaleFactor();
    const auto nearOffset = [&](double expected)
        { return std::abs(scroll.HorizontalOffset() - expected) < 0.25; };
    REQUIRE(nearOffset(0));
    const auto origin = first.TransformToVisual(scroll).TransformPoint({0, 0});
    std::vector<wxPoint> wxPositions;
    for ( auto* const panel : panels ) wxPositions.push_back(panel->GetPosition());

    REQUIRE(page->ScrollLines(1));
    REQUIRE(page->ScrollPixels(13)); // no message pump between relative calls
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(21.0 / scale); }));
    const auto shifted = first.TransformToVisual(scroll).TransformPoint({0, 0});
    CHECK(std::abs(std::abs(shifted.X - origin.X) - 21.0 / scale) < 0.5);
    CHECK_FALSE(page->ScrollPixels(0));

    unsigned count = 0;
    while ( count < 512 && page->ScrollLines(1) ) ++count;
    CHECK(count > 0);
    REQUIRE(count < 512);
    CHECK_FALSE(page->ScrollLines(1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(scroll.ScrollableWidth()); }));
    REQUIRE(page->ScrollLines((std::numeric_limits<int>::min)()));
    CHECK_FALSE(page->ScrollPixels(-1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(0); }));
    REQUIRE(page->ScrollLines((std::numeric_limits<int>::max)()));
    CHECK_FALSE(page->ScrollPixels(1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(scroll.ScrollableWidth()); }));

    // A real ScrollViewer automation provider can change the viewport too;
    // subsequent wx calls must start from its native offset, not a stale one.
    const auto provider = GetToolkitPattern<MUX::Automation::Provider::IScrollProvider>(
        scroll, MUX::Automation::Peers::PatternInterface::Scroll);
    REQUIRE(provider);
    REQUIRE(provider.HorizontallyScrollable());
    provider.SetScrollPercent(50, -1);
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(scroll.ScrollableWidth() / 2); }));
    const double middle = scroll.HorizontalOffset();
    REQUIRE(page->ScrollPixels(-17));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(middle - 17.0 / scale); }));
    count = 0;
    while ( count < 512 && page->ScrollLines(-1) ) ++count;
    REQUIRE(count < 512);
    CHECK_FALSE(page->ScrollLines(-1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(0); }));

    // A section is the actual allocated group, not an arbitrary pixel step.
    // Five non-collapsible groups leave both a partially visible section and
    // a complete next section beyond the initial viewport in LTR and RTL.
    std::vector<winrt::Windows::Foundation::Rect> sections;
    unsigned partial = 0;
    for ( const auto& item : ribbon.Items() )
    {
        const auto group = item.as<Labs::RibbonGroup>();
        REQUIRE(MUX::Media::VisualTreeHelper::GetParent(group) == nativePanel);
        sections.push_back(MUXC::Primitives::LayoutInformation::GetLayoutSlot(group));
    }
    const double viewport = scroll.ViewportWidth();
    while ( partial < sections.size() &&
            sections[partial].X + sections[partial].Width <= viewport ) ++partial;
    REQUIRE(partial > 0);
    REQUIRE(partial + 1 < sections.size());
    REQUIRE(sections[partial].X < viewport);
    const double partialTarget = sections[partial].X + sections[partial].Width - viewport;
    const double nextTarget = sections[partial + 1].X + sections[partial + 1].Width - viewport;
    REQUIRE(partialTarget > 8.0 / scale);
    REQUIRE(nextTarget < scroll.ScrollableWidth());
    REQUIRE(page->ScrollSections(1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(partialTarget); }));
    const auto revealed = ribbon.Items().GetAt(partial).as<Labs::RibbonGroup>();
    const auto revealedBounds = revealed.TransformToVisual(scroll).TransformBounds(
        {0, 0, static_cast<float>(revealed.ActualWidth()),
         static_cast<float>(revealed.ActualHeight())});
    CHECK(revealedBounds.X >= -0.5);
    CHECK(revealedBounds.X + revealedBounds.Width <= viewport + 0.5);
    REQUIRE(page->ScrollSections(1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(nextTarget); }));

    // Backward uncovers the section intersecting the leading edge. At its
    // exact boundary, the following request uncovers the previous section.
    unsigned leading = 0;
    while ( leading + 1 < sections.size() && sections[leading + 1].X <= nextTarget )
        ++leading;
    REQUIRE(nextTarget > sections[leading].X + 8.0 / scale);
    REQUIRE(page->ScrollSections(-1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(sections[leading].X); }));
    if ( leading > 0 )
    {
        REQUIRE(page->ScrollSections(-1));
        REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(sections[leading - 1].X); }));
    }
    count = 0;
    while ( count < 16 && page->ScrollSections(-1) ) ++count;
    REQUIRE(count < 16);
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(0); }));
    CHECK_FALSE(page->ScrollSections(-1));
    REQUIRE(page->ScrollSections(1));
    REQUIRE(page->ScrollSections(1)); // logical pending offset, no dispatcher gap
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(nextTarget); }));
    count = 0;
    while ( count < 16 && page->ScrollSections(1) ) ++count;
    REQUIRE(count < 16);
    CHECK_FALSE(page->ScrollSections(1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(scroll.ScrollableWidth()); }));
    count = 0;
    while ( count < 16 && page->ScrollSections(-1) ) ++count;
    REQUIRE(count < 16);
    CHECK_FALSE(page->ScrollSections(-1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(0); }));

    // Scrolling moves actual XAML visuals, not the invisible wx HWND shells.
    for ( size_t i = 0; i != panels.size(); ++i )
        CHECK(panels[i]->GetPosition() == wxPositions[i]);
    CHECK(host->FindSlot(page)->GetPresentationRoot() == ribbon);
    CHECK(ribbon.Items().GetAt(0) == first);
    CHECK(ribbon.Items().GetAt(4) == last);
    CHECK(first.Content() == firstMount);
    CHECK(control.GetTemplateChild(L"ScrollViewer") == scroll);
    CHECK(bar->GetActivePage() == 0);
    CHECK(bar->GetDisplayMode() == wxRIBBON_BAR_PINNED);

    // Resize removes overflow in the real viewport; the public endpoints
    // follow the new native extent without reconstructing the containers.
    bar->SetSize(wxSize(1500, 240));
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        host->FlushSync();
        ribbon.UpdateLayout();
        return scroll.ScrollableWidth() < 0.01;
    }));
    CHECK_FALSE(page->ScrollPixels(1));
    CHECK_FALSE(page->ScrollPixels(-1));
    CHECK_FALSE(page->ScrollSections(1));
    CHECK_FALSE(page->ScrollSections(-1));
    CHECK(ribbon.Items().GetAt(0) == first);
    CHECK(control.GetTemplateChild(L"ScrollViewer") == scroll);

    // Retire a page while its native section offset request is still pending.
    // A retained old ScrollProvider must not affect a new page with the same
    // public ID; neither native observers nor requested offsets may leak.
    bar->SetSize(wxSize(400, 240));
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        host->FlushSync();
        ribbon.UpdateLayout();
        return scroll.ScrollableWidth() > 80;
    }));
    REQUIRE(page->ScrollSections(1));
    REQUIRE(page->GetId() == pageId);
    const wxWeakRef<wxRibbonPage> retiredPage(page);
    if ( !keepOldPage )
    {
        bar->DeletePage(0);
        wxTheApp->ProcessIdle();
        REQUIRE_FALSE(retiredPage);
    }
    // The diagnostic control case adds a second page after Show(), keeping
    // the original page and native viewport alive but inactive. It does not
    // pass through DeletePage(), an empty TabItems collection or ID reuse.
    auto* const replacement = new wxRibbonPage(bar,
        keepOldPage ? pageId + 1 : pageId, "Replacement page");
    auto* const replacementPanel = new wxRibbonPanel(replacement, wxID_ANY, "Replacement group",
        wxBitmapBundle(), wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
    auto* const replacementCommands = new wxRibbonButtonBar(replacementPanel);
    REQUIRE(replacementCommands->AddButton(wxID_HIGHEST + 5800, "Replacement command", bitmap));
    auto* const replacementSizer = new wxBoxSizer(wxHORIZONTAL);
    replacementSizer->Add(replacementCommands, 1, wxEXPAND);
    replacementSizer->SetMinSize(replacementPanel->FromDIP(wxSize(900, 80)));
    replacementPanel->SetSizer(replacementSizer);
    if ( keepOldPage )
    {
        REQUIRE(bar->SetActivePage(1));
        REQUIRE(retiredPage);
        REQUIRE_FALSE(retiredPage->IsShown());
    }
    REQUIRE(bar->Realize());
    MUXC::ScrollViewer replacementScroll{nullptr};
    Labs::Ribbon replacementRibbon{nullptr};
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        host->FlushSync();
        const auto replacementSlot = host->FindSlot(replacement);
        if ( !replacementSlot ) return false;
        replacementRibbon = replacementSlot->GetPresentationRoot().try_as<Labs::Ribbon>();
        if ( !replacementRibbon ) return false;
        replacementRibbon.UpdateLayout();
        replacementScroll = replacementRibbon.as<MUXC::IControlProtected>()
            .GetTemplateChild(L"ScrollViewer").try_as<MUXC::ScrollViewer>();
        return replacementScroll && replacementScroll.IsLoaded() &&
               replacementScroll.ScrollableWidth() > 80;
    }));
    REQUIRE(replacementRibbon != ribbon);
    REQUIRE(replacementScroll != scroll);
    CHECK(replacementScroll.HorizontalOffset() == 0);
    try { provider.SetScrollPercent(50, -1); }
    catch ( const winrt::hresult_error& ) { /* retired UIA providers may reject */ }
    wxYield();
    host->FlushSync();
    CHECK(replacementScroll.HorizontalOffset() == 0);
    const auto replacementGroup = replacementRibbon.Items().GetAt(0).as<Labs::RibbonGroup>();
    const auto replacementOrigin = replacementGroup.TransformToVisual(replacementScroll)
        .TransformPoint({0, 0});
    const auto replacementProvider = GetToolkitPattern<MUX::Automation::Provider::IScrollProvider>(
        replacementScroll, MUX::Automation::Peers::PatternInterface::Scroll);
    REQUIRE(replacementProvider);
    REQUIRE(replacementProvider.HorizontallyScrollable());
    unsigned replacementFinalViews = 0;
    const auto replacementViewToken = replacementScroll.ViewChanged(
        [&](const auto&, const auto& args)
        {
            if ( !args.IsIntermediate() ) ++replacementFinalViews;
        });
    const auto removeReplacementObserver = wxMakeGuard([&]()
        { replacementScroll.ViewChanged(replacementViewToken); });
    CHECK_FALSE(replacement->ScrollSections(-1));
    REQUIRE(replacement->ScrollSections(1));
    const bool replacementScrolled = WaitForToolkitDispatch([&]()
        { return std::abs(replacementScroll.HorizontalOffset() -
                          replacementScroll.ScrollableWidth()) < 0.25; });
    std::ostringstream replacementLayoutDiagnostic;
    if ( !replacementScrolled )
    {
        // Diagnostic only, AFTER recording the unchanged strict verdict.
        // RibbonPanel adapts to a finite measure but arranges its overflowing
        // children wider. Isolate the presenter's extent bookkeeping without
        // replacing the native viewport or publishing a fabricated offset.
        replacementLayoutDiagnostic << " initial:" << initialMeasureState
            << " dynamic:" << measureState(replacementRibbon);
        const auto presenter = replacementScroll.as<MUXC::IControlProtected>()
            .GetTemplateChild(L"ScrollContentPresenter").try_as<MUXC::ScrollContentPresenter>();
        if ( presenter )
        {
            const bool previousSizesToParent = presenter.SizesContentToTemplatedParent();
            const auto restoreConstraint = wxMakeGuard([&]()
                { presenter.SizesContentToTemplatedParent(previousSizesToParent); });
            presenter.SizesContentToTemplatedParent(false);
            presenter.InvalidateMeasure();
            replacementRibbon.UpdateLayout();
            const auto diagnosticTarget = winrt::box_value(replacementScroll.ScrollableWidth())
                .as<winrt::Windows::Foundation::IReference<double>>();
            const bool accepted = replacementScroll.ChangeView(diagnosticTarget, nullptr, nullptr, true);
            const bool progressed = WaitForToolkitDispatch([&]()
                { return std::abs(replacementScroll.HorizontalOffset() -
                                  replacementScroll.ScrollableWidth()) < 0.25; });
            replacementLayoutDiagnostic << " unconstrained accepted=" << accepted
                << " progressed=" << progressed << " finalViews=" << replacementFinalViews
                << measureState(replacementRibbon);
        }
    }
    INFO(replacementLayoutDiagnostic.str());
    REQUIRE(replacementScrolled);
    CHECK_FALSE(replacement->ScrollSections(1));
    REQUIRE(WaitForToolkitDispatch([&]() { return replacementFinalViews > 0; }));
    CHECK(std::abs(replacementProvider.HorizontalScrollPercent() - 100) < 0.01);
    const auto replacementShifted = replacementGroup.TransformToVisual(replacementScroll)
        .TransformPoint({0, 0});
    CHECK(std::abs(std::abs(replacementShifted.X - replacementOrigin.X) -
                   replacementScroll.ScrollableWidth()) < 0.5);
    replacementProvider.SetScrollPercent(50, -1);
    REQUIRE(WaitForToolkitDispatch([&]()
        { return std::abs(replacementScroll.HorizontalOffset() -
                          replacementScroll.ScrollableWidth() / 2) < 0.25; }));
    const double replacementMiddle = replacementScroll.HorizontalOffset();
    REQUIRE(replacement->ScrollPixels(-17));
    REQUIRE(WaitForToolkitDispatch([&]()
        { return std::abs(replacementScroll.HorizontalOffset() -
                          (replacementMiddle - 17.0 / scale)) < 0.25; }));
}

TEST_CASE("RibbonToolkit::NativePageScrollsInsideOneOversizedGroup",
          "[winui-ribbon-toolkit][winui-ribbon-page-scroll]")
{
    const bool rightToLeft = GENERATE(false, true);
    const bool firstRequestAtEnd = GENERATE(false, true);
    wxWeakRef<wxFrame> frame(new wxFrame(nullptr, wxID_ANY, "Oversized ribbon group",
                                        wxDefaultPosition, wxSize(600, 330)));
    const auto cleanup = wxMakeGuard([&]()
    {
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    });
    auto* const bar = new wxRibbonBar(frame.get(), wxID_ANY);
    if ( rightToLeft ) bar->SetLayoutDirection(wxLayout_RightToLeft);
    auto* const page = new wxRibbonPage(bar, wxID_ANY, "One group");
    auto* const panel = new wxRibbonPanel(page, wxID_ANY, "Oversized group",
        wxBitmapBundle(), wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
    auto* const commands = new wxRibbonButtonBar(panel);
    const auto bitmap = wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(32, 32));
    REQUIRE(bitmap.IsOk());
    REQUIRE(commands->AddButton(wxID_HIGHEST + 5801, "Native command", bitmap));
    auto* const sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(commands, 1, wxEXPAND);
    sizer->SetMinSize(panel->FromDIP(wxSize(900, 80)));
    panel->SetSizer(sizer);
    REQUIRE(bar->Realize());
    frame->Show();
    const auto host = wxWinUITopLevelHost::FindSlotOwner(page);
    REQUIRE(host);
    Labs::Ribbon ribbon{nullptr};
    MUXC::ScrollViewer scroll{nullptr};
    REQUIRE(WaitForToolkitDispatch([&]()
    {
        host->FlushSync();
        const auto slot = host->FindSlot(page);
        if ( !slot ) return false;
        ribbon = slot->GetPresentationRoot().try_as<Labs::Ribbon>();
        if ( !ribbon ) return false;
        ribbon.UpdateLayout();
        scroll = ribbon.as<MUXC::IControlProtected>()
            .GetTemplateChild(L"ScrollViewer").try_as<MUXC::ScrollViewer>();
        return scroll && scroll.IsLoaded() && scroll.ViewportWidth() > 0 &&
               scroll.ScrollableWidth() > 80;
    }));
    REQUIRE(ribbon.Items().Size() == 1);
    const auto group = ribbon.Items().GetAt(0).as<Labs::RibbonGroup>();
    const auto mount = group.Content();
    const double scale = page->GetDPIScaleFactor();
    const auto nearOffset = [&](double expected)
        { return std::abs(scroll.HorizontalOffset() - expected) < 0.25; };
    CAPTURE(rightToLeft, firstRequestAtEnd, static_cast<int>(scroll.HorizontalScrollMode()),
            static_cast<int>(scroll.HorizontalSnapPointsType()));
    REQUIRE(nearOffset(0));
    if ( !firstRequestAtEnd )
    {
        REQUIRE(page->ScrollPixels(13));
        REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(13.0 / scale); }));
    }
    REQUIRE(page->ScrollSections(1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(scroll.ScrollableWidth()); }));
    CHECK_FALSE(page->ScrollSections(1));
    REQUIRE(page->ScrollSections(-1));
    REQUIRE(WaitForToolkitDispatch([&]() { return nearOffset(0); }));
    CHECK_FALSE(page->ScrollSections(-1));
    CHECK(host->FindSlot(page)->GetPresentationRoot() == ribbon);
    CHECK(ribbon.Items().GetAt(0) == group);
    CHECK(group.Content() == mount);
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_RIBBON && WXWINUI_RIBBON_TOOLKIT
