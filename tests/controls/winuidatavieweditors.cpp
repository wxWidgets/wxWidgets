/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuidatavieweditors.cpp
// Purpose:     Physical retirement of real transient DataView popup editors
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/frame.h"
#include "wx/scopeguard.h"
#include "wx/stopwatch.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "../../src/winui/datavieweditors.h"
#include "wx/winui/private/tlwhost.h"

#include <algorithm>

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;

template<typename Predicate>
bool WaitForEditor(Predicate predicate)
{
    wxEventLoopBase* const loop = wxEventLoopBase::GetActive();
    if ( !loop || !wxTheApp )
        return false;
    wxStopWatch elapsed;
    for ( ;; )
    {
        if ( predicate() )
            return true;
        if ( elapsed.Time() >= 2000 )
            return false;
        wxTheApp->ProcessPendingEvents();
        if ( wxEventLoopBase::GetActive() != loop )
            return false;
        if ( predicate() )
            return true;
        const long remaining = 2000 - elapsed.Time();
        if ( remaining <= 0 || loop->DispatchTimeout(
                static_cast<unsigned long>(std::min(5L, remaining))) == 0 )
        {
            return false;
        }
    }
}

class EditorRetirementFixture
{
public:
    ~EditorRetirementFixture()
    {
        if ( retirement )
            retirement->Retire();
        host.Close();
        if ( frame )
            frame->Destroy();
        wxTheApp->ProcessPendingEvents();
        wxYield();
        wxTheApp->ProcessIdle();
    }

    void Create(bool calendar)
    {
        // Only CTest's private desktop is a visible rendering target. Never
        // place this native-popup fixture over the user's input desktop.
        wxPoint position(-30000, -30000);
        wchar_t desktopName[256]{};
        DWORD needed = 0;
        const auto desktop = ::GetThreadDesktop(::GetCurrentThreadId());
        if ( ::GetUserObjectInformationW(desktop, UOI_NAME,
                desktopName, sizeof(desktopName), &needed) &&
             wxString(desktopName).StartsWith("wxWinUITest-") )
        {
            position = wxPoint(80, 80);
        }
        frame = new wxFrame(nullptr, wxID_ANY, "Native editor retirement",
                            position, wxSize(560, 420));
        owner = new wxWindow(frame.get(), wxID_ANY, wxPoint(10, 10),
                             wxSize(420, 300));
        REQUIRE(host.Initialize(owner));
        fanIn = wxWinUICreateCompositePhysicalDisconnectGate();
        REQUIRE(fanIn);
        host.SetPhysicalDisconnectGate(fanIn);
        REQUIRE(host.SetContent(mount));
        AddEditor(calendar);
        frame->ShowWithoutActivating();
        REQUIRE(WaitForEditor([&]()
        {
            if ( auto* top = wxWinUITopLevelHost::FindSlotOwner(owner) )
                top->FlushSync();
            return editor.IsLoaded() && editor.XamlRoot() &&
                   editor.ActualWidth() > 0 && editor.ActualHeight() > 0;
        }));
        REQUIRE_FALSE(retirement->IsDegraded());
    }

    void AddEditor(bool calendar)
    {
        if ( calendar )
        {
            editor = C::CalendarDatePicker();
        }
        else
        {
            C::ComboBox combo;
            combo.Items().Append(winrt::box_value(L"one"));
            combo.Items().Append(winrt::box_value(L"two"));
            combo.SelectedIndex(0);
            editor = combo;
        }
        editor.Width(240);
        editor.Height(36);
        retirement = wxWinUIDataViewEditorRetirement::Create(
            editor, mount, fanIn);
        REQUIRE(retirement);
        REQUIRE_FALSE(retirement->IsSatisfied());
        mount.Children().Append(editor);
    }

    void Open(bool waitForOpened = true)
    {
        owner->SetFocus();
        REQUIRE(editor.Focus(X::FocusState::Programmatic));
        const auto opened = std::make_shared<bool>(false);
        const auto onOpened = [opened](const auto&, const auto&) { *opened = true; };
        const auto combo = editor.try_as<C::ComboBox>();
        const auto date = editor.try_as<C::CalendarDatePicker>();
        const auto token = combo ? combo.DropDownOpened(onOpened) : date.Opened(onOpened);
        const auto revoke = wxMakeGuard([&]()
        {
            if ( combo ) combo.DropDownOpened(token);
            else date.Opened(token);
        });
        if ( combo )
            combo.IsDropDownOpen(true);
        else
            date.IsCalendarOpen(true);
        if ( !waitForOpened )
        {
            REQUIRE(retirement->IsPopupOpen());
            return;
        }
        REQUIRE(WaitForEditor([&]()
        {
            return *opened && retirement->IsPopupOpen() &&
                X::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(
                    editor.XamlRoot()).Size() != 0;
        }));
        REQUIRE_FALSE(retirement->IsDegraded());
    }

    wxWeakRef<wxFrame> frame;
    wxWindow* owner = nullptr;
    wxWinUIControlHost host;
    C::Canvas mount;
    C::Control editor{nullptr};
    std::shared_ptr<wxWinUIPhysicalDisconnectGate> fanIn;
    std::shared_ptr<wxWinUIDataViewEditorRetirement> retirement;
};
} // namespace

TEST_CASE("DataViewEditors::ClosedTransactionsRetireAndPruneNativePeers",
          "[winui-dataview][native-editor-retirement]")
{
    const bool calendar = GENERATE(false, true);
    CAPTURE(calendar);
    EditorRetirementFixture fixture;
    fixture.Create(calendar);
    for ( unsigned generation = 0; generation != 3; ++generation )
    {
        if ( generation )
            fixture.AddEditor(calendar);
        unsigned completions = 0;
        REQUIRE(fixture.mount.Children().Size() == 1);
        fixture.retirement->Retire([&]() { ++completions; });
        CHECK(completions == 1);
        CHECK(fixture.retirement->IsSatisfied());
        CHECK_FALSE(fixture.retirement->IsDegraded());
        CHECK(fixture.mount.Children().Size() == 0);
        CHECK(fixture.fanIn->IsSatisfied());
        // An idempotent logical cancel must not emit another completion.
        fixture.retirement->Retire([&]() { ++completions; });
        CHECK(completions == 1);
    }
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
}

TEST_CASE("DataViewEditors::PopupCloseRetainsExactMountUntilCausalBoundary",
          "[winui-dataview][native-editor-retirement]")
{
    const bool calendar = GENERATE(false, true);
    const bool closeHost = GENERATE(false, true);
    CAPTURE(calendar, closeHost);
    EditorRetirementFixture fixture;
    fixture.Create(calendar);
    fixture.Open();
    struct Observed
    {
        unsigned closed = 0;
        unsigned completed = 0;
        bool mountedDuringClosed = false;
        bool satisfiedDuringClosed = false;
    };
    const auto observed = std::make_shared<Observed>();
    const auto weakState = std::weak_ptr<wxWinUIDataViewEditorRetirement>(
        fixture.retirement);
    const auto closed = [observed, weakState,
                         mount = fixture.mount, editor = fixture.editor]
        (const auto&, const auto&)
    {
        ++observed->closed;
        uint32_t index = 0;
        observed->mountedDuringClosed = mount.Children().IndexOf(editor, index);
        if ( const auto state = weakState.lock() )
            observed->satisfiedDuringClosed = state->IsSatisfied();
    };
    winrt::event_token token{};
    const auto combo = fixture.editor.try_as<C::ComboBox>();
    const auto date = fixture.editor.try_as<C::CalendarDatePicker>();
    if ( combo )
        token = combo.DropDownClosed(closed);
    else
        token = date.Closed(closed);
    const auto revoke = wxMakeGuard([&]()
    {
        if ( combo ) combo.DropDownClosed(token);
        else date.Closed(token);
    });

    fixture.retirement->Retire([observed]() { ++observed->completed; });
    if ( closeHost )
        fixture.host.Close();
    const bool completed = WaitForEditor([&]() { return observed->completed == 1; });
    bool focusInPopup = false;
    if ( combo )
    {
        const auto popup = combo.as<C::IControlProtected>()
            .GetTemplateChild(L"Popup").try_as<P::Popup>();
        auto focused = X::Input::FocusManager::GetFocusedElement(combo.XamlRoot())
            .try_as<X::DependencyObject>();
        for ( unsigned depth = 0; popup && focused && depth != 512; ++depth )
        {
            if ( focused == popup.Child() ) { focusInPopup = true; break; }
            focused = X::Media::VisualTreeHelper::GetParent(focused);
        }
    }
    INFO("closed=" << observed->closed << " completed=" << observed->completed
         << " mounted=" << fixture.mount.Children().Size()
         << " satisfied=" << fixture.retirement->IsSatisfied()
         << " degraded=" << fixture.retirement->IsDegraded()
         << " sealed=" << fixture.retirement->IsSealedForHostShutdown()
         << " popup=" << fixture.retirement->IsPopupOpen()
         << " focusInPopup=" << focusInPopup);
    REQUIRE(completed);
    CHECK(observed->closed == 1);
    CHECK(observed->mountedDuringClosed);
    CHECK_FALSE(observed->satisfiedDuringClosed);
    CHECK(fixture.mount.Children().Size() == 0);
    CHECK(fixture.retirement->IsSatisfied());
    CHECK_FALSE(fixture.retirement->IsDegraded());
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
}

TEST_CASE("DataViewEditors::CalendarCancelBeforeOpenedRetiresPublishedOpening",
          "[winui-dataview][native-editor-retirement]")
{
    EditorRetirementFixture fixture;
    fixture.Create(true);
    fixture.Open(false);
    const auto completed = std::make_shared<unsigned>(0);
    fixture.retirement->Retire([completed]() { ++*completed; });
    const bool retired = WaitForEditor([&]() { return *completed == 1; });
    INFO("mounted=" << fixture.mount.Children().Size()
         << " degraded=" << fixture.retirement->IsDegraded()
         << " popup=" << fixture.retirement->IsPopupOpen());
    REQUIRE(retired);
    CHECK(fixture.mount.Children().Size() == 0);
    CHECK(fixture.retirement->IsSatisfied());
    CHECK_FALSE(fixture.retirement->IsDegraded());
    CHECK_FALSE(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
}

#endif // __WXWINUI__ && wxUSE_WINUI3
