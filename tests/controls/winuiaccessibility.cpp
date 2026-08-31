///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiaccessibility.cpp
// Purpose:     Deterministic WinUI UI Automation contract tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
#include "radiobox-test-access.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/event.h"
    #include "wx/panel.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#if wxUSE_BUTTON
    #include "wx/button.h"
#endif
#if wxUSE_CHECKBOX
    #include "wx/checkbox.h"
#endif
#if wxUSE_RADIOBOX
    #include "wx/radiobox.h"
#endif
#if wxUSE_RADIOBTN
    #include "wx/radiobut.h"
#endif
#if wxUSE_TOGGLEBTN
    #include "wx/tglbtn.h"
#endif
#if wxUSE_LISTBOX
    #include "wx/listbox.h"
#endif
#if wxUSE_SLIDER
    #include "wx/slider.h"
#endif
#if wxUSE_TEXTCTRL
    #include "wx/textctrl.h"
#endif

#include "wx/winui/private/tlwhost.h"

#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXAPR = winrt::Microsoft::UI::Xaml::Automation::Provider;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace WUT = winrt::Microsoft::UI::Text;

// Plan-009 deterministic coverage matrix.
//
// Family       Real peer/pattern             Mutation/action       Disabled
// ------------ ----------------------------- --------------------- --------
// Button       Button / Invoke               label, reparent, call throws
// CheckBox     CheckBox / Toggle             RTL, label, toggle     throws
// ToggleButton Button / Toggle                toggle, teardown       throws
// RadioButton  RadioButton / SelectionItem    RTL, select, teardown  throws
// RadioBox     RadioButton / SelectionItem    rebuild, stale provider throws
// TextCtrl     Edit/native model             external Text/Value gate   n/a
// Slider       Slider / RangeValue           range/steps/value       throws
// ListBox      List+ListItem / Selection(*)  label/select            throws
//
// (*) The container's ISelectionProvider is queried and the action is driven
// through its realized item's ISelectionItemProvider, exactly as a UIA client
// selects an item. All cases use the already visible test runner TLW. They
// create no TLW, call no Show(), synthesize no input and never steal focus.

void DrainDispatch(int rounds = 8)
{
    for ( int i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(1);
    }
}

wxPoint OffscreenPosition()
{
    return wxPoint(-30000, -30000);
}

MUX::UIElement GetHostedElement(wxWindow *window)
{
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( !host )
        return nullptr;

    host->FlushSync();
    wxWinUISlot * const slot = host->FindSlot(window);
    return slot ? slot->GetContent() : nullptr;
}

MUX::UIElement GetHostedSemanticElement(wxWindow *window)
{
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( !host )
        return nullptr;

    host->FlushSync();
    wxWinUISlot * const slot = host->FindSlot(window);
    return slot ? slot->GetSemanticTarget() : nullptr;
}

MUXAP::AutomationPeer GetPeer(const MUX::UIElement& element)
{
    if ( !element )
        return nullptr;

    const MUX::FrameworkElement frameworkElement =
        element.try_as<MUX::FrameworkElement>();
    if ( !frameworkElement )
        return nullptr;

    return MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
        frameworkElement);
}

#if wxUSE_TEXTCTRL
wxString GetRichEditDocumentText(const MUXC::RichEditBox& richEditBox)
{
    if ( !richEditBox )
        return wxString();

    winrt::hstring text;
    const WUT::TextGetOptions options = static_cast<WUT::TextGetOptions>(
        static_cast<std::uint32_t>(WUT::TextGetOptions::UseLf) |
        static_cast<std::uint32_t>(WUT::TextGetOptions::AllowFinalEop));
    richEditBox.Document().GetText(options, text);

    wxString value(text.c_str());
    // TOM owns one final end-of-paragraph marker which is not part of the wx
    // value. An actual trailing newline creates a second marker, so remove
    // exactly one, matching the production RichEdit model projection.
    if ( !value.empty() &&
         (value.Last() == '\r' || value.Last() == '\n') )
    {
        value.RemoveLast();
    }
    return value;
}
#endif // wxUSE_TEXTCTRL

#if wxUSE_SLIDER
bool IsAutomationSubtreeRaw(const MUX::DependencyObject& object)
{
    if ( !object )
        return false;

    if ( const MUX::UIElement element = object.try_as<MUX::UIElement>() )
    {
        if ( MUXA::AutomationProperties::GetAccessibilityView(element) !=
                 MUXAP::AccessibilityView::Raw )
        {
            return false;
        }
    }

    const int count = MUX::Media::VisualTreeHelper::GetChildrenCount(object);
    for ( int i = 0; i < count; ++i )
    {
        if ( !IsAutomationSubtreeRaw(
                 MUX::Media::VisualTreeHelper::GetChild(object, i)) )
        {
            return false;
        }
    }
    return true;
}
#endif // wxUSE_SLIDER

#if wxUSE_RADIOBOX
void CollectRadioButtons(
    const MUX::DependencyObject& object,
    std::vector<MUXC::RadioButton>& buttons)
{
    if ( !object )
        return;

    if ( const MUXC::RadioButton button =
            object.try_as<MUXC::RadioButton>() )
    {
        buttons.push_back(button);
    }

    const int count =
        MUX::Media::VisualTreeHelper::GetChildrenCount(object);
    for ( int i = 0; i < count; ++i )
    {
        CollectRadioButtons(
            MUX::Media::VisualTreeHelper::GetChild(object, i), buttons);
    }
}

std::vector<MUXC::RadioButton>
GetHostedRadioButtons(wxRadioBox *radio)
{
    std::vector<MUXC::RadioButton> buttons;
    CollectRadioButtons(GetHostedElement(radio), buttons);
    return buttons;
}
#endif // wxUSE_RADIOBOX

template <typename Provider>
Provider GetPattern(const MUXAP::AutomationPeer& peer,
                    MUXAP::PatternInterface pattern)
{
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

template <typename F>
bool IsRejectedByUIA(F&& function)
{
    try
    {
        std::forward<F>(function)();
    }
    catch ( const winrt::hresult_error& )
    {
        return true;
    }

    return false;
}

void CheckPeerIdentity(const MUXAP::AutomationPeer& peer,
                       MUXAP::AutomationControlType role,
                       const wxString& name)
{
    REQUIRE(peer != nullptr);
    CHECK(peer.GetAutomationControlType() == role);
    CHECK(wxString(peer.GetName().c_str()) == name);
    CHECK(peer.IsEnabled());
}

class CommandCounter final : public wxEvtHandler
{
public:
    void OnCommand(wxCommandEvent&)
    {
        ++m_count;
    }

    int GetCount() const { return m_count; }

private:
    int m_count = 0;
};

struct HostBalance
{
    unsigned slots;
    unsigned lifetimes;
    unsigned loadedHooks;
    unsigned handlerAdds;
    unsigned handlerRevokes;
};

HostBalance CaptureHostBalance()
{
    DrainDispatch();
    return
    {
        wxWinUITopLevelHost::GetLiveSlotCount(),
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount(),
        wxWinUITopLevelHost::GetLiveLoadedHookCount(),
        wxWinUITopLevelHost::GetSlotHandlerAddCount(),
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount()
    };
}

void CheckHostBalanceRestored(const HostBalance& before)
{
    DrainDispatch(16);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
          before.lifetimes);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() ==
          before.loadedHooks);

    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() -
        before.handlerAdds;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
        before.handlerRevokes;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

#if wxUSE_LISTBOX
MUXAP::AutomationPeer GetListItemDataPeer(
    const MUXAP::AutomationPeer& listPeer,
    const MUXC::ListView& list,
    int index)
{
    if ( !listPeer || !list || index < 0 ||
         static_cast<uint32_t>(index) >= list.Items().Size() )
    {
        return nullptr;
    }

    // A ListViewItem used directly as an Items entry has two peers:
    // its FrameworkElement peer (visual/container details only) and the
    // parent-created data peer that appears in the UIA item tree. Selection
    // belongs to the latter; querying CreatePeerForElement(container) tests
    // the wrong peer and correctly returns no SelectionItem pattern.
    const MUXAP::ItemsControlAutomationPeer itemsPeer =
        listPeer.try_as<MUXAP::ItemsControlAutomationPeer>();
    if ( !itemsPeer )
        return nullptr;

    return itemsPeer.CreateItemAutomationPeer(
        list.Items().GetAt(index));
}
#endif // wxUSE_LISTBOX

} // anonymous namespace

#if wxUSE_BUTTON
TEST_CASE("wxWinUI UIA Invoke preserves identity across dynamic state",
          "[winui-accessibility][winui-009][uia-invoke]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE(top != nullptr);

    std::unique_ptr<wxPanel> left(new wxPanel(top, wxID_ANY));
    std::unique_ptr<wxPanel> right(new wxPanel(top, wxID_ANY));
    const HostBalance balance = CaptureHostBalance();
    CommandCounter counter;
    std::unique_ptr<wxButton> button(new wxButton);
    REQUIRE(button->Create(left.get(), wxID_ANY, "Run now"));
    button->Bind(wxEVT_BUTTON, &CommandCounter::OnCommand, &counter);

    DrainDispatch();
    const MUX::UIElement element = GetHostedElement(button.get());
    REQUIRE(element != nullptr);
    const MUX::FrameworkElement frameworkElement =
        element.try_as<MUX::FrameworkElement>();
    REQUIRE(frameworkElement != nullptr);
    button->SetLayoutDirection(wxLayout_RightToLeft);
    DrainDispatch();
    CHECK(frameworkElement.FlowDirection() ==
          MUX::FlowDirection::RightToLeft);
    button->SetLayoutDirection(wxLayout_LeftToRight);
    DrainDispatch();
    CHECK(frameworkElement.FlowDirection() ==
          MUX::FlowDirection::LeftToRight);

    const MUXAP::AutomationPeer peer = GetPeer(element);
    CheckPeerIdentity(
        peer, MUXAP::AutomationControlType::Button, "Run now");
#if wxUSE_TOOLTIPS
    button->SetToolTip("Executes the current action");
    DrainDispatch();
    CHECK(wxString(peer.GetHelpText().c_str()) ==
          "Executes the current action");
#endif

    const MUXAPR::IInvokeProvider invoke =
        GetPattern<MUXAPR::IInvokeProvider>(
            peer, MUXAP::PatternInterface::Invoke);
    REQUIRE(invoke != nullptr);

    button->SetLabel("Run later");
    DrainDispatch();
    CHECK(wxString(peer.GetName().c_str()) == "Run later");

    invoke.Invoke();
    DrainDispatch();
    CHECK(counter.GetCount() == 1);

    // Same-TLW reparenting must preserve the logical peer/action and must not
    // require another visible top-level window.
    REQUIRE(button->Reparent(right.get()));
    DrainDispatch();
    REQUIRE(GetHostedElement(button.get()) == element);
    invoke.Invoke();
    DrainDispatch();
    CHECK(counter.GetCount() == 2);

    button->Disable();
    DrainDispatch();
    CHECK_FALSE(peer.IsEnabled());
    CHECK(IsRejectedByUIA([&] { invoke.Invoke(); }));
    DrainDispatch();
    CHECK(counter.GetCount() == 2);
    button->Enable();
    DrainDispatch();
    REQUIRE(peer.IsEnabled());

    // Keep the real provider alive beyond wx teardown. Button destruction
    // must revoke its XAML handler, so even a callable retained provider can
    // never reach the dead wx object.
    button.reset();
    DrainDispatch();
    try
    {
        invoke.Invoke();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    DrainDispatch();
    CHECK(counter.GetCount() == 2);

    std::unique_ptr<wxButton> doomed(
        new wxButton(top, wxID_ANY, "Delete button", OffscreenPosition()));
    const MUXAPR::IInvokeProvider doomedInvoke =
        GetPattern<MUXAPR::IInvokeProvider>(
            GetPeer(GetHostedElement(doomed.get())),
            MUXAP::PatternInterface::Invoke);
    REQUIRE(doomedInvoke != nullptr);
    doomed->Bind(wxEVT_BUTTON,
                 [&doomed](wxCommandEvent&) { doomed.reset(); });
    doomedInvoke.Invoke();
    DrainDispatch();
    CHECK(doomed == nullptr);

    CheckHostBalanceRestored(balance);
}
#endif // wxUSE_BUTTON

#if wxUSE_CHECKBOX
TEST_CASE("wxWinUI UIA Toggle follows label, RTL and teardown",
          "[winui-accessibility][winui-009][uia-toggle][rtl]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE(top != nullptr);

    const HostBalance balance = CaptureHostBalance();
    CommandCounter counter;
    std::unique_ptr<wxCheckBox> natural(new wxCheckBox);
    REQUIRE(natural->Create(top, wxID_ANY, "Natural"));
    std::unique_ptr<wxCheckBox> check(new wxCheckBox);
    REQUIRE(check->Create(top, wxID_ANY, "Remember", wxDefaultPosition,
                          wxDefaultSize, wxALIGN_RIGHT));
    check->Bind(wxEVT_CHECKBOX, &CommandCounter::OnCommand, &counter);

    natural->SetLayoutDirection(wxLayout_LeftToRight);
    DrainDispatch();
    const MUX::UIElement naturalElement =
        GetHostedElement(natural.get());
    REQUIRE(naturalElement != nullptr);
    const MUXC::CheckBox naturalPeer =
        naturalElement.try_as<MUXC::CheckBox>();
    REQUIRE(naturalPeer != nullptr);
    REQUIRE(naturalPeer.Content() != nullptr);
    MUXC::TextBlock naturalText =
        naturalPeer.Content().try_as<MUXC::TextBlock>();
    REQUIRE(naturalText != nullptr);
    CHECK(naturalPeer.FlowDirection() == MUX::FlowDirection::LeftToRight);
    CHECK(naturalText.FlowDirection() == MUX::FlowDirection::LeftToRight);

    natural->SetLayoutDirection(wxLayout_RightToLeft);
    DrainDispatch();
    naturalText =
        naturalPeer.Content().try_as<MUXC::TextBlock>();
    REQUIRE(naturalText != nullptr);
    CHECK(naturalPeer.FlowDirection() == MUX::FlowDirection::RightToLeft);
    CHECK(naturalText.FlowDirection() == MUX::FlowDirection::RightToLeft);

    check->SetLayoutDirection(wxLayout_LeftToRight);
    DrainDispatch();
    const MUX::UIElement element = GetHostedElement(check.get());
    REQUIRE(element != nullptr);
    const MUXC::CheckBox xamlCheck = element.try_as<MUXC::CheckBox>();
    REQUIRE(xamlCheck != nullptr);
    const MUXAP::AutomationPeer peer = GetPeer(element);
    CheckPeerIdentity(
        peer, MUXAP::AutomationControlType::CheckBox, "Remember");

    // wxALIGN_RIGHT keeps the label left of the glyph while the label itself
    // follows the hot application direction.
    REQUIRE(xamlCheck.Content() != nullptr);
    MUXC::TextBlock text =
        xamlCheck.Content().try_as<MUXC::TextBlock>();
    REQUIRE(text != nullptr);
    CHECK(xamlCheck.FlowDirection() == MUX::FlowDirection::RightToLeft);
    CHECK(text.FlowDirection() == MUX::FlowDirection::LeftToRight);

    check->SetLayoutDirection(wxLayout_RightToLeft);
    DrainDispatch();
    text = xamlCheck.Content().try_as<MUXC::TextBlock>();
    REQUIRE(text != nullptr);
    CHECK(xamlCheck.FlowDirection() == MUX::FlowDirection::RightToLeft);
    CHECK(text.FlowDirection() == MUX::FlowDirection::RightToLeft);

    check->SetLayoutDirection(wxLayout_LeftToRight);
    check->SetLabel("Remember choice");
    DrainDispatch();
    CHECK(wxString(peer.GetName().c_str()) == "Remember choice");

    const MUXAPR::IToggleProvider toggle =
        GetPattern<MUXAPR::IToggleProvider>(
            peer, MUXAP::PatternInterface::Toggle);
    REQUIRE(toggle != nullptr);
    CHECK(toggle.ToggleState() == MUXA::ToggleState::Off);

    toggle.Toggle();
    DrainDispatch();
    CHECK(check->GetValue());
    CHECK(toggle.ToggleState() == MUXA::ToggleState::On);
    CHECK(counter.GetCount() == 1);

    check->Disable();
    DrainDispatch();
    CHECK_FALSE(peer.IsEnabled());
    CHECK(IsRejectedByUIA([&] { toggle.Toggle(); }));
    DrainDispatch();
    CHECK(check->GetValue());
    CHECK(toggle.ToggleState() == MUXA::ToggleState::On);
    CHECK(counter.GetCount() == 1);
    check->Enable();
    DrainDispatch();
    REQUIRE(peer.IsEnabled());

    // This is the regression guard for a provider retaining CheckBox after
    // wxCheckBox teardown: callback state is invalidated and every token is
    // revoked before host/content detachment.
    check.reset();
    DrainDispatch();
    try
    {
        toggle.Toggle();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    DrainDispatch();
    CHECK(counter.GetCount() == 1);
    natural.reset();

    std::unique_ptr<wxCheckBox> doomed(
        new wxCheckBox(top, wxID_ANY, "Delete check", OffscreenPosition()));
    const MUXAPR::IToggleProvider doomedToggle =
        GetPattern<MUXAPR::IToggleProvider>(
            GetPeer(GetHostedElement(doomed.get())),
            MUXAP::PatternInterface::Toggle);
    REQUIRE(doomedToggle != nullptr);
    doomed->Bind(wxEVT_CHECKBOX,
                 [&doomed](wxCommandEvent&) { doomed.reset(); });
    doomedToggle.Toggle();
    DrainDispatch();
    CHECK(doomed == nullptr);

    CheckHostBalanceRestored(balance);
}
#endif // wxUSE_CHECKBOX

#if wxUSE_TOGGLEBTN
TEST_CASE("wxWinUI UIA ToggleButton provider is inert after teardown",
          "[winui-accessibility][winui-008d][uia-toggle-button][lifetime]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE(top != nullptr);

    const HostBalance balance = CaptureHostBalance();
    CommandCounter counter;
    std::unique_ptr<wxToggleButton> toggleButton(new wxToggleButton);
    REQUIRE(toggleButton->Create(
        top, wxID_ANY, "Pin", OffscreenPosition()));
    toggleButton->Bind(
        wxEVT_TOGGLEBUTTON, &CommandCounter::OnCommand, &counter);

    DrainDispatch();
    const MUX::UIElement element =
        GetHostedElement(toggleButton.get());
    REQUIRE(element != nullptr);
    const MUX::FrameworkElement frameworkElement =
        element.try_as<MUX::FrameworkElement>();
    REQUIRE(frameworkElement != nullptr);
    toggleButton->SetLayoutDirection(wxLayout_RightToLeft);
    DrainDispatch();
    CHECK(frameworkElement.FlowDirection() ==
          MUX::FlowDirection::RightToLeft);
    toggleButton->SetLayoutDirection(wxLayout_LeftToRight);
    DrainDispatch();
    CHECK(frameworkElement.FlowDirection() ==
          MUX::FlowDirection::LeftToRight);

    const MUXAP::AutomationPeer peer = GetPeer(element);
    CheckPeerIdentity(
        peer, MUXAP::AutomationControlType::Button, "Pin");

    const MUXAPR::IToggleProvider toggle =
        GetPattern<MUXAPR::IToggleProvider>(
            peer, MUXAP::PatternInterface::Toggle);
    REQUIRE(toggle != nullptr);
    CHECK(toggle.ToggleState() == MUXA::ToggleState::Off);

    toggle.Toggle();
    DrainDispatch();
    CHECK(toggleButton->GetValue());
    CHECK(counter.GetCount() == 1);

    toggleButton->Disable();
    DrainDispatch();
    CHECK_FALSE(peer.IsEnabled());
    CHECK(IsRejectedByUIA([&] { toggle.Toggle(); }));
    CHECK(counter.GetCount() == 1);
    toggleButton->Enable();

    // Leave the retained peer in an actionable unchecked state. If its state
    // delegate survives wx teardown, the following UIA call would dispatch a
    // second wx event through a dead owner.
    toggleButton->SetValue(false);
    DrainDispatch();
    toggleButton.reset();
    DrainDispatch();
    try
    {
        toggle.Toggle();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    DrainDispatch();
    CHECK(counter.GetCount() == 1);

    std::unique_ptr<wxToggleButton> doomed(
        new wxToggleButton(
            top, wxID_ANY, "Delete toggle", OffscreenPosition()));
    const MUXAPR::IToggleProvider doomedToggle =
        GetPattern<MUXAPR::IToggleProvider>(
            GetPeer(GetHostedElement(doomed.get())),
            MUXAP::PatternInterface::Toggle);
    REQUIRE(doomedToggle != nullptr);
    doomed->Bind(wxEVT_TOGGLEBUTTON,
                 [&doomed](wxCommandEvent&) { doomed.reset(); });
    doomedToggle.Toggle();
    DrainDispatch();
    CHECK(doomed == nullptr);

    CheckHostBalanceRestored(balance);
}
#endif // wxUSE_TOGGLEBTN

#if wxUSE_RADIOBTN
TEST_CASE("wxWinUI UIA RadioButton follows hot RTL and retained-provider teardown",
          "[winui-accessibility][winui-008d][uia-radio][rtl][lifetime]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE(top != nullptr);

    const HostBalance balance = CaptureHostBalance();
    CommandCounter counter;
    std::unique_ptr<wxRadioButton> radio(new wxRadioButton);
    REQUIRE(radio->Create(
        top, wxID_ANY, "Choice", OffscreenPosition(),
        wxDefaultSize, wxRB_SINGLE));
    radio->Bind(wxEVT_RADIOBUTTON, &CommandCounter::OnCommand, &counter);

    radio->SetLayoutDirection(wxLayout_LeftToRight);
    DrainDispatch();
    const MUX::UIElement element = GetHostedElement(radio.get());
    REQUIRE(element != nullptr);
    const MUXC::RadioButton xamlRadio =
        element.try_as<MUXC::RadioButton>();
    REQUIRE(xamlRadio != nullptr);
    REQUIRE(xamlRadio.Content() != nullptr);
    MUXC::TextBlock text =
        xamlRadio.Content().try_as<MUXC::TextBlock>();
    REQUIRE(text != nullptr);
    CHECK(xamlRadio.FlowDirection() == MUX::FlowDirection::LeftToRight);
    CHECK(text.FlowDirection() == MUX::FlowDirection::LeftToRight);

    radio->SetLayoutDirection(wxLayout_RightToLeft);
    DrainDispatch();
    text = xamlRadio.Content().try_as<MUXC::TextBlock>();
    REQUIRE(text != nullptr);
    CHECK(xamlRadio.FlowDirection() == MUX::FlowDirection::RightToLeft);
    CHECK(text.FlowDirection() == MUX::FlowDirection::RightToLeft);

    radio->SetLayoutDirection(wxLayout_LeftToRight);
    DrainDispatch();
    const MUXAP::AutomationPeer peer = GetPeer(element);
    CheckPeerIdentity(
        peer, MUXAP::AutomationControlType::RadioButton, "Choice");
    const MUXAPR::ISelectionItemProvider select =
        GetPattern<MUXAPR::ISelectionItemProvider>(
            peer, MUXAP::PatternInterface::SelectionItem);
    REQUIRE(select != nullptr);

    select.Select();
    DrainDispatch();
    CHECK(radio->GetValue());
    CHECK(counter.GetCount() == 1);

    radio->SetValue(false);
    DrainDispatch();
    radio->Disable();
    DrainDispatch();
    CHECK_FALSE(peer.IsEnabled());
    CHECK(IsRejectedByUIA([&] { select.Select(); }));
    CHECK(counter.GetCount() == 1);
    radio->Enable();

    radio.reset();
    DrainDispatch();
    try
    {
        select.Select();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    DrainDispatch();
    CHECK(counter.GetCount() == 1);

    std::unique_ptr<wxRadioButton> doomed(
        new wxRadioButton(top, wxID_ANY, "Delete radio", OffscreenPosition(),
                          wxDefaultSize, wxRB_SINGLE));
    const MUXAPR::ISelectionItemProvider doomedSelect =
        GetPattern<MUXAPR::ISelectionItemProvider>(
            GetPeer(GetHostedElement(doomed.get())),
            MUXAP::PatternInterface::SelectionItem);
    REQUIRE(doomedSelect != nullptr);
    doomed->Bind(wxEVT_RADIOBUTTON,
                 [&doomed](wxCommandEvent&) { doomed.reset(); });
    doomedSelect.Select();
    DrainDispatch();
    CHECK(doomed == nullptr);

    CheckHostBalanceRestored(balance);
}
#endif // wxUSE_RADIOBTN

#if wxUSE_RADIOBOX
TEST_CASE("wxWinUI UIA RadioBox rejects stale content generations",
          "[winui-accessibility][winui-008d][uia-radiobox][generation][lifetime]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE(top != nullptr);

    const HostBalance balance = CaptureHostBalance();
    CommandCounter counter;
    const wxString choices[] = { "zero", "one", "two" };
    std::unique_ptr<wxRadioBox> radio(new wxRadioBox);
    REQUIRE(radio->Create(
        top, wxID_ANY, "Numbers", OffscreenPosition(),
        wxDefaultSize, WXSIZEOF(choices), choices));
    radio->Bind(wxEVT_RADIOBOX, &CommandCounter::OnCommand, &counter);

    DrainDispatch();
    std::vector<MUXC::RadioButton> oldButtons =
        GetHostedRadioButtons(radio.get());
    REQUIRE(oldButtons.size() == WXSIZEOF(choices));

    radio->SetLayoutDirection(wxLayout_RightToLeft);
    DrainDispatch();
    for ( const MUXC::RadioButton& button : oldButtons )
        CHECK(button.FlowDirection() == MUX::FlowDirection::RightToLeft);
    radio->SetLayoutDirection(wxLayout_LeftToRight);
    DrainDispatch();
    for ( const MUXC::RadioButton& button : oldButtons )
        CHECK(button.FlowDirection() == MUX::FlowDirection::LeftToRight);

    const MUXAP::AutomationPeer stalePeer = GetPeer(oldButtons[2]);
    REQUIRE(stalePeer != nullptr);
    const MUXAPR::ISelectionItemProvider staleSelect =
        GetPattern<MUXAPR::ISelectionItemProvider>(
            stalePeer, MUXAP::PatternInterface::SelectionItem);
    REQUIRE(staleSelect != nullptr);

    // Rebuild replaces every XAML RadioButton. The old provider is retained
    // deliberately and must be inert even if WinUI still lets it Select().
    radio->SetString(0, "zero renamed");
    DrainDispatch();
    try
    {
        staleSelect.Select();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    DrainDispatch();
    CHECK(radio->GetSelection() == 0);
    CHECK(counter.GetCount() == 0);

    std::vector<MUXC::RadioButton> currentButtons =
        GetHostedRadioButtons(radio.get());
    REQUIRE(currentButtons.size() == WXSIZEOF(choices));
    CHECK(currentButtons[2] != oldButtons[2]);
    unsigned long long stateGeneration = 0;
    REQUIRE(wxWinUIRadioBoxTestAccess::GetPeerState(*radio,
        nullptr, nullptr, &stateGeneration));
    const MUXAP::AutomationPeer currentPeer = GetPeer(currentButtons[2]);
    REQUIRE(currentPeer != nullptr);
    const MUXAPR::ISelectionItemProvider currentSelect =
        GetPattern<MUXAPR::ISelectionItemProvider>(
            currentPeer, MUXAP::PatternInterface::SelectionItem);
    REQUIRE(currentSelect != nullptr);

    currentSelect.Select();
    DrainDispatch();
    CHECK(radio->GetSelection() == 2);
    CHECK(counter.GetCount() == 1);

    radio->SetSelection(0);
    DrainDispatch();
    std::vector<MUXC::RadioButton> stateButtons =
        GetHostedRadioButtons(radio.get());
    REQUIRE(stateButtons.size() == currentButtons.size());
    for ( size_t i = 0; i < stateButtons.size(); ++i )
        CHECK(stateButtons[i] == currentButtons[i]);
    unsigned long long generationAfterSelection = 0;
    REQUIRE(wxWinUIRadioBoxTestAccess::GetPeerState(*radio,
        nullptr, nullptr, &generationAfterSelection));
    CHECK(generationAfterSelection == stateGeneration);

    REQUIRE(radio->Show(2, false));
    DrainDispatch();
    CHECK(currentButtons[2].Visibility() == MUX::Visibility::Collapsed);
    REQUIRE(radio->Show(2, true));
    DrainDispatch();
    CHECK(currentButtons[2].Visibility() == MUX::Visibility::Visible);
    CHECK_FALSE(radio->Show(2, true));
    CHECK_FALSE(radio->Enable(2, true));

    radio->Disable();
    // An item-level Enable() must not punch through the disabled composite.
    REQUIRE(radio->Enable(2, true));
    DrainDispatch();
    CHECK_FALSE(currentPeer.IsEnabled());
    CHECK(IsRejectedByUIA([&] { currentSelect.Select(); }));
    CHECK(counter.GetCount() == 1);
    unsigned long long generationAfterDisable = 0;
    REQUIRE(wxWinUIRadioBoxTestAccess::GetPeerState(*radio,
        nullptr, nullptr, &generationAfterDisable));
    CHECK(generationAfterDisable == stateGeneration);
    radio->Enable();

    // Item 2 is still actionable: retain its provider across destruction.
    // Callback-state invalidation must suppress late calls.
    radio.reset();
    DrainDispatch();
    try
    {
        currentSelect.Select();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    DrainDispatch();
    CHECK(counter.GetCount() == 1);

    std::unique_ptr<wxRadioBox> doomed(
        new wxRadioBox(top, wxID_ANY, "Delete group", OffscreenPosition(),
                       wxDefaultSize, WXSIZEOF(choices), choices));
    std::vector<MUXC::RadioButton> doomedButtons =
        GetHostedRadioButtons(doomed.get());
    REQUIRE(doomedButtons.size() == WXSIZEOF(choices));
    const MUXAPR::ISelectionItemProvider doomedSelect =
        GetPattern<MUXAPR::ISelectionItemProvider>(
            GetPeer(doomedButtons[1]),
            MUXAP::PatternInterface::SelectionItem);
    REQUIRE(doomedSelect != nullptr);
    doomed->Bind(wxEVT_RADIOBOX,
                 [&doomed](wxCommandEvent&) { doomed.reset(); });
    doomedSelect.Select();
    DrainDispatch();
    CHECK(doomed == nullptr);

    CheckHostBalanceRestored(balance);
}
#endif // wxUSE_RADIOBOX

#if wxUSE_TEXTCTRL
TEST_CASE("wxWinUI UIA Text identity tracks edits and protected state",
          "[winui-accessibility][winui-009][uia-text]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE(top != nullptr);

    const HostBalance balance = CaptureHostBalance();
    std::unique_ptr<wxTextCtrl> text(
        new wxTextCtrl(top, wxID_ANY, "before"));
    DrainDispatch();

    const MUX::UIElement element = GetHostedSemanticElement(text.get());
    REQUIRE(element != nullptr);
    CHECK(GetHostedElement(text.get()) == element);
    MUXA::AutomationProperties::SetName(element, L"Editor");
    const MUXAP::AutomationPeer peer = GetPeer(element);
    CheckPeerIdentity(
        peer, MUXAP::AutomationControlType::Edit, "Editor");

    // The external UIA Text/Value projection used by screen readers is a
    // separate plan-009 gate. WinAppSDK 1.8's genuine RichEditBox peer can
    // return no in-process Value pattern object in an unpackaged island.
    // Exercise IValueProvider when the runtime projects it; either way, the
    // peer identity and the real TOM document remain mandatory assertions.
    const winrt::Windows::Foundation::IInspectable valuePatternObject =
        peer.GetPattern(MUXAP::PatternInterface::Value);
    const MUXAPR::IValueProvider valuePattern =
        valuePatternObject.try_as<MUXAPR::IValueProvider>();
    const MUXC::RichEditBox richEditBox =
        element.try_as<MUXC::RichEditBox>();
    REQUIRE(richEditBox != nullptr);
    REQUIRE(peer.try_as<MUXAP::RichEditBoxAutomationPeer>() != nullptr);
    CHECK(GetRichEditDocumentText(richEditBox) == "before");

    if ( valuePattern )
    {
        CHECK_FALSE(valuePattern.IsReadOnly());
        CHECK(wxString(valuePattern.Value().c_str()) == "before");
        valuePattern.SetValue(L"through UIA");
    }
    else
    {
        CHECK(valuePatternObject == nullptr);
        text->SetValue("through wx");
    }
    DrainDispatch();
    const wxString expectedValue =
        valuePattern ? "through UIA" : "through wx";
    CHECK(text->GetValue() == expectedValue);
    CHECK(GetRichEditDocumentText(richEditBox) == expectedValue);

    text->ChangeValue("after");
    DrainDispatch();
    CHECK(text->GetValue() == "after");
    CHECK(GetRichEditDocumentText(richEditBox) == "after");
    if ( valuePattern )
        CHECK(wxString(valuePattern.Value().c_str()) == "after");

    MUXA::AutomationProperties::SetName(element, L"Renamed editor");
    CHECK(wxString(peer.GetName().c_str()) == "Renamed editor");

    text->SetEditable(false);
    DrainDispatch();
    CHECK(richEditBox.IsReadOnly());
    if ( valuePattern )
    {
        CHECK(valuePattern.IsReadOnly());
        CHECK(IsRejectedByUIA(
            [&]() { valuePattern.SetValue(L"must be rejected"); }));
    }
    CHECK(text->GetValue() == "after");

    text->SetEditable(true);
    text->Disable();
    DrainDispatch();
    CHECK_FALSE(peer.IsEnabled());
    if ( valuePattern )
    {
        CHECK(wxString(valuePattern.Value().c_str()) == "after");
        CHECK(IsRejectedByUIA(
            [&]() { valuePattern.SetValue(L"disabled write"); }));
    }
    CHECK(text->GetValue() == "after");

    text.reset();
    CheckHostBalanceRestored(balance);
}
#endif // wxUSE_TEXTCTRL

#if wxUSE_SLIDER
TEST_CASE("wxWinUI UIA RangeValue tracks the public numeric contract",
          "[winui-accessibility][winui-009][uia-range-value]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE(top != nullptr);

    const HostBalance balance = CaptureHostBalance();
    std::unique_ptr<wxSlider> slider(
        new wxSlider(top, wxID_ANY, 40, 0, 100));
    slider->SetLineSize(2);
    slider->SetPageSize(10);
    DrainDispatch();

    const MUX::UIElement visualRoot = GetHostedElement(slider.get());
    REQUIRE(visualRoot != nullptr);
    const MUXC::Grid root = visualRoot.try_as<MUXC::Grid>();
    REQUIRE(root != nullptr);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(root) ==
          MUXAP::AccessibilityView::Raw);

    const MUX::UIElement element =
        GetHostedSemanticElement(slider.get());
    REQUIRE(element != nullptr);
    REQUIRE(element != visualRoot);
    const MUXC::Slider xamlSlider = element.try_as<MUXC::Slider>();
    REQUIRE(xamlSlider != nullptr);
    REQUIRE(root.Children().Size() == 2);
    CHECK(root.Children().GetAt(0) == element);
    CHECK(IsAutomationSubtreeRaw(root.Children().GetAt(1)));

    MUXA::AutomationProperties::SetName(element, L"Zoom");
    const MUXAP::AutomationPeer peer = GetPeer(element);
    REQUIRE(peer.try_as<MUXAP::SliderAutomationPeer>() != nullptr);
    CheckPeerIdentity(
        peer, MUXAP::AutomationControlType::Slider, "Zoom");

    const MUXAPR::IRangeValueProvider range =
        GetPattern<MUXAPR::IRangeValueProvider>(
            peer, MUXAP::PatternInterface::RangeValue);
    REQUIRE(range != nullptr);
    CHECK_FALSE(range.IsReadOnly());
    CHECK(range.Minimum() == 0.0);
    CHECK(range.Maximum() == 100.0);
    CHECK(range.SmallChange() == 2.0);
    CHECK(range.LargeChange() == 10.0);
    CHECK(range.Value() == 40.0);

    range.SetValue(75.0);
    DrainDispatch(16);
    CHECK(slider->GetValue() == 75);
    CHECK(range.Value() == 75.0);

    slider->SetRange(-10, 10);
    slider->SetLineSize(1);
    slider->SetPageSize(5);
    slider->SetValue(0);
    DrainDispatch();
    CHECK(range.Minimum() == -10.0);
    CHECK(range.Maximum() == 10.0);
    CHECK(range.SmallChange() == 1.0);
    CHECK(range.LargeChange() == 5.0);
    CHECK(range.Value() == 0.0);

    slider->Disable();
    DrainDispatch();
    CHECK_FALSE(peer.IsEnabled());
    CHECK(IsRejectedByUIA([&] { range.SetValue(5.0); }));
    DrainDispatch();
    CHECK(slider->GetValue() == 0);
    CHECK(range.Value() == 0.0);

    slider.reset();
    CheckHostBalanceRestored(balance);
}
#endif // wxUSE_SLIDER

#if wxUSE_LISTBOX
TEST_CASE("wxWinUI UIA Selection acts through the item data provider",
          "[winui-accessibility][winui-009][uia-selection]")
{
    wxWindow * const top = wxTheApp->GetTopWindow();
    REQUIRE(top != nullptr);

    const HostBalance balance = CaptureHostBalance();
    CommandCounter counter;
    const wxString choices[] = { "alpha", "beta", "gamma" };
    std::unique_ptr<wxListBox> list(
        new wxListBox(top, wxID_ANY, wxDefaultPosition, wxSize(180, 100),
                      WXSIZEOF(choices), choices));
    list->Bind(wxEVT_LISTBOX, &CommandCounter::OnCommand, &counter);
    list->SetSelection(0);
    DrainDispatch(12);

    const MUX::UIElement element = GetHostedElement(list.get());
    REQUIRE(element != nullptr);
    const MUXC::ListView xamlList = element.try_as<MUXC::ListView>();
    REQUIRE(xamlList != nullptr);
    MUXA::AutomationProperties::SetName(element, L"Options");
    const MUXAP::AutomationPeer peer = GetPeer(element);
    CheckPeerIdentity(
        peer, MUXAP::AutomationControlType::List, "Options");

    const MUXAPR::ISelectionProvider selection =
        GetPattern<MUXAPR::ISelectionProvider>(
            peer, MUXAP::PatternInterface::Selection);
    REQUIRE(selection != nullptr);
    CHECK_FALSE(selection.CanSelectMultiple());
    CHECK_FALSE(selection.IsSelectionRequired());
    CHECK(selection.GetSelection().size() == 1);

    const MUXAP::AutomationPeer itemTwoPeer =
        GetListItemDataPeer(peer, xamlList, 2);
    CheckPeerIdentity(
        itemTwoPeer, MUXAP::AutomationControlType::ListItem, "gamma");
    const MUXAPR::ISelectionItemProvider selectTwo =
        GetPattern<MUXAPR::ISelectionItemProvider>(
            itemTwoPeer, MUXAP::PatternInterface::SelectionItem);
    REQUIRE(selectTwo != nullptr);
    REQUIRE(selectTwo.SelectionContainer() != nullptr);

    selectTwo.Select();
    DrainDispatch();
    CHECK(list->GetSelection() == 2);
    CHECK(selectTwo.IsSelected());
    CHECK(selection.GetSelection().size() == 1);
    CHECK(counter.GetCount() == 1);

    list->SetString(2, "delta");
    DrainDispatch();
    CHECK(wxString(itemTwoPeer.GetName().c_str()) == "delta");

    const MUXAP::AutomationPeer itemOnePeer =
        GetListItemDataPeer(peer, xamlList, 1);
    REQUIRE(itemOnePeer != nullptr);
    const MUXAPR::ISelectionItemProvider selectOne =
        GetPattern<MUXAPR::ISelectionItemProvider>(
            itemOnePeer, MUXAP::PatternInterface::SelectionItem);
    REQUIRE(selectOne != nullptr);

    list->Disable();
    DrainDispatch();
    CHECK_FALSE(peer.IsEnabled());
    CHECK_FALSE(itemOnePeer.IsEnabled());
    CHECK(IsRejectedByUIA([&] { selectOne.Select(); }));
    DrainDispatch();
    CHECK(list->GetSelection() == 2);
    CHECK(counter.GetCount() == 1);
    list->Enable();
    DrainDispatch();
    REQUIRE(itemOnePeer.IsEnabled());

    // wxListBox already uses invalidated shared callback state. Retaining
    // both container and item providers after teardown must not dispatch a
    // late wx selection event or retain a host slot/lifetime state.
    list.reset();
    DrainDispatch();
    try
    {
        selectOne.Select();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    DrainDispatch();
    CHECK(counter.GetCount() == 1);
    CheckHostBalanceRestored(balance);
}
#endif // wxUSE_LISTBOX

#endif // __WXWINUI__ && wxUSE_WINUI3
