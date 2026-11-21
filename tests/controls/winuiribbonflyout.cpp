/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiribbonflyout.cpp
// Purpose:     Exact pinned Ribbon collapsed-group template contracts
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_RIBBON && \
    defined(WXWINUI_RIBBON_TOOLKIT)

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "../../src/ribbon/ribbonflyoutwinui.h"

#include <algorithm>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace Labs = winrt::XamlToolkit::Labs::WinUI;
using Status = wxRibbonFlyoutResolveResult;

template <typename Predicate>
bool WaitForRibbonTemplate(Predicate ready)
{
    auto* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp ) return false;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( ready() ) return true;
        if ( elapsed.Time() >= 500 ) return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop ) return false;
        if ( ready() ) return true;
        const long remaining = 500 - elapsed.Time();
        if ( remaining <= 0 || loop->DispatchTimeout(
                 static_cast<unsigned long>(std::min(5L, remaining))) == 0 )
            return false;
    }
}

struct RibbonTemplateFixture
{
    RibbonTemplateFixture()
    {
        wxRibbonToolkitFailure failure;
        const bool acquired = wxRibbonToolkitRuntime::TryAcquire(runtime, &failure);
        INFO("Toolkit stage=" << failure.stage << " detail="
             << failure.detail.ToStdString(wxConvUTF8));
        REQUIRE(acquired);
        REQUIRE(runtime->TryCreateCollapsibleGroup(group, &failure));
        group.Label(L"Exact template group");
        content = C::Border();
        content.Width(100);
        content.Height(40);
        group.Content(content);
        frame = new wxFrame(nullptr, wxID_ANY, "Ribbon template contract",
                            wxPoint(-32000, -32000), wxSize(420, 220));
        auto* const window = new wxWindow(frame.get(), wxID_ANY,
                                         wxPoint(0, 0), wxSize(380, 180));
        REQUIRE(host.Initialize(window));
        // No native RibbonPanel surrounds this group: State is changed by the
        // test itself, not overwritten by adaptive page layout during measure.
        REQUIRE(host.SetContent(group, group));
        frame->ShowWithoutActivating();
        host.ForceRender();
        REQUIRE(WaitForRibbonTemplate([&]()
        {
            return group.IsLoaded() && content.IsLoaded();
        }));
    }

    ~RibbonTemplateFixture()
    {
        host.Close();
        if ( frame ) frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    std::shared_ptr<wxRibbonToolkitRuntime> runtime;
    Labs::RibbonCollapsibleGroup group{nullptr};
    C::Border content{nullptr};
    wxWeakRef<wxFrame> frame;
    wxWinUIControlHost host;
};

C::ControlTemplate ReplacementTemplate()
{
    // A distinct real template exercises re-application of the documented
    // namescope contract, without opening a popup or faking native events.
    return X::Markup::XamlReader::Load(LR"xaml(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
 xmlns:labs="using:XamlToolkit.Labs.WinUI"
 TargetType="labs:RibbonCollapsibleGroup">
 <Grid>
  <ContentControl x:Name="VisibleContentContainer" />
  <Button x:Name="CollapsedButton">
   <Button.Flyout>
    <Flyout x:Name="CollapsedFlyout">
     <StackPanel><ContentControl x:Name="CollapsedContentPresenter" /></StackPanel>
    </Flyout>
   </Button.Flyout>
  </Button>
 </Grid>
</ControlTemplate>)xaml").as<C::ControlTemplate>();
}
} // namespace

TEST_CASE("RibbonToolkit::FlyoutPartsResolveExactNativeIdentities",
          "[winui-ribbon-toolkit][winui-ribbon-flyout]")
{
    RibbonTemplateFixture fixture;
    wxRibbonFlyoutParts parts;
    REQUIRE(wxRibbonResolveFlyoutParts(fixture.group, parts) == Status::Ready);
    REQUIRE(wxRibbonFlyoutPartsAreCurrent(parts));
    const auto control = fixture.group.as<C::IControlProtected>();
    CHECK(parts.group == fixture.group);
    CHECK(parts.controlTemplate == fixture.group.Template());
    CHECK(parts.button == control.GetTemplateChild(L"CollapsedButton"));
    CHECK(parts.inlineMount == control.GetTemplateChild(L"VisibleContentContainer"));
    CHECK(parts.popupMount == control.GetTemplateChild(L"CollapsedContentPresenter"));
    CHECK(parts.flyout == control.GetTemplateChild(L"CollapsedFlyout"));
    CHECK(parts.button.Flyout() == parts.flyout);
    CHECK_FALSE(parts.flyout.IsOpen());
    wxRibbonFlyoutParts again;
    REQUIRE(wxRibbonResolveFlyoutParts(fixture.group, again) == Status::Ready);
    CHECK(again.button == parts.button);
    CHECK(again.flyout == parts.flyout);
    CHECK(again.popupMount == parts.popupMount);

    X::Visibility state = X::Visibility::Collapsed;
    REQUIRE(wxRibbonReadFlyoutState(fixture.group, state));
    CHECK(state == X::Visibility::Visible);
    CHECK_FALSE(wxRibbonReadFlyoutState(nullptr, state));
    CHECK(state == X::Visibility::Visible);
    REQUIRE(wxRibbonResolveFlyoutParts(nullptr, again) == Status::Invalid);
    CHECK_FALSE(again.group);
    CHECK_FALSE(again.button);
    CHECK_FALSE(wxRibbonFlyoutPartsAreCurrent(again));
}

TEST_CASE("RibbonToolkit::FlyoutStateMovesSameContentWithoutOpeningPopup",
          "[winui-ribbon-toolkit][winui-ribbon-flyout]")
{
    RibbonTemplateFixture fixture;
    wxRibbonFlyoutParts parts;
    REQUIRE(wxRibbonResolveFlyoutParts(fixture.group, parts) == Status::Ready);
    REQUIRE(parts.inlineMount.Content() == fixture.content);
    CHECK_FALSE(parts.popupMount.Content());

    fixture.group.State(X::Visibility::Collapsed);
    X::Visibility state = X::Visibility::Visible;
    REQUIRE(wxRibbonReadFlyoutState(fixture.group, state));
    CHECK(state == X::Visibility::Collapsed);
    CHECK(parts.inlineMount.Visibility() == X::Visibility::Collapsed);
    CHECK(parts.button.Visibility() == X::Visibility::Visible);
    CHECK_FALSE(parts.inlineMount.Content());
    CHECK(parts.popupMount.Content() == fixture.content);
    CHECK(fixture.group.Content() == fixture.content);
    CHECK_FALSE(parts.flyout.IsOpen());
    REQUIRE(wxRibbonFlyoutPartsAreCurrent(parts));

    fixture.group.State(X::Visibility::Visible);
    CHECK(parts.inlineMount.Visibility() == X::Visibility::Visible);
    CHECK(parts.button.Visibility() == X::Visibility::Collapsed);
    CHECK(parts.inlineMount.Content() == fixture.content);
    CHECK_FALSE(parts.popupMount.Content());
    CHECK_FALSE(parts.flyout.IsOpen());
    REQUIRE(wxRibbonFlyoutPartsAreCurrent(parts));
}

TEST_CASE("RibbonToolkit::FlyoutRetemplateRejectsStaleAndMalformedParts",
          "[winui-ribbon-toolkit][winui-ribbon-flyout]")
{
    RibbonTemplateFixture fixture;
    wxRibbonFlyoutParts old;
    REQUIRE(wxRibbonResolveFlyoutParts(fixture.group, old) == Status::Ready);

    SECTION("Replacement publishes fresh parts, never the old flyout")
    {
        const auto replacement = ReplacementTemplate();
        REQUIRE(replacement != old.controlTemplate);
        // The Toolkit transfers Content on State changes, but does not clear
        // the previous template's ContentControl when retemplating. Detach it
        // explicitly here so this test isolates part identity validation.
        old.inlineMount.Content(nullptr);
        fixture.group.Template(replacement);
        CHECK_FALSE(wxRibbonFlyoutPartsAreCurrent(old));
        wxRibbonFlyoutParts current;
        REQUIRE(wxRibbonResolveFlyoutParts(fixture.group, current) == Status::Ready);
        CHECK(current.controlTemplate == replacement);
        CHECK(current.button != old.button);
        CHECK(current.flyout != old.flyout);
        CHECK(current.inlineMount != old.inlineMount);
        CHECK(current.popupMount != old.popupMount);
        CHECK(current.inlineMount.Content() == fixture.content);
        CHECK_FALSE(wxRibbonFlyoutPartsAreCurrent(old));
        CHECK(wxRibbonFlyoutPartsAreCurrent(current));
    }

    SECTION("A detached namescope flyout is not the button's flyout")
    {
        const C::Flyout foreign;
        old.button.Flyout(foreign);
        CHECK_FALSE(wxRibbonFlyoutPartsAreCurrent(old));
        wxRibbonFlyoutParts rejected;
        CHECK(wxRibbonResolveFlyoutParts(fixture.group, rejected) == Status::Invalid);
        CHECK_FALSE(rejected.flyout);
    }

    SECTION("A loaded template with absent or wrong parts is Invalid, not Pending")
    {
        old.inlineMount.Content(nullptr);
        const auto malformed = X::Markup::XamlReader::Load(LR"xaml(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
 xmlns:labs="using:XamlToolkit.Labs.WinUI"
 TargetType="labs:RibbonCollapsibleGroup">
 <Grid><TextBlock x:Name="CollapsedButton" Text="not a Button" /></Grid>
</ControlTemplate>)xaml").as<C::ControlTemplate>();
        fixture.group.Template(malformed);
        wxRibbonFlyoutParts rejected;
        CHECK(wxRibbonResolveFlyoutParts(fixture.group, rejected) == Status::Invalid);
        CHECK_FALSE(rejected.group);
        CHECK_FALSE(wxRibbonFlyoutPartsAreCurrent(old));
    }
}

#endif
