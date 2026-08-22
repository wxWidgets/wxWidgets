///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuispinmodel.cpp
// Purpose:     deterministic WinUI spin control contract tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "testableframe.h"

#include "wx/app.h"
#include "wx/spinbutt.h"
#include "wx/spinctrl.h"
#include "wx/winui/private/tlwhost.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>

#include <memory>
#include <vector>
#include <climits>

namespace
{

namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;

struct ObservedSpinEvent
{
    wxEventType type;
    double value;
    wxString text;
};

} // anonymous namespace

#if wxUSE_SPINBTN

TEST_CASE("wxWinUI SpinButton preserves orientation and action transaction",
          "[winui-spin-model][spinbutton][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinButton vertical;
    REQUIRE(vertical.Create(
        parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxSP_VERTICAL | wxSP_ARROW_KEYS | wxSP_WRAP));

    bool isVertical = false;
    unsigned rows = 0;
    unsigned columns = 0;
    REQUIRE(vertical.WinUIGetPeerLayoutForTesting(
        &isVertical, &rows, &columns));
    CHECK(isVertical);
    CHECK(rows == 2);
    CHECK(columns == 0);

    vertical.SetRange(0, 10);
    vertical.SetIncrement(3);
    vertical.SetValue(9);

    std::vector<ObservedSpinEvent> events;
    vertical.Bind(wxEVT_SPIN_UP, [&events](wxSpinEvent& event)
    {
        events.push_back(
            {event.GetEventType(),
             static_cast<double>(event.GetPosition()), wxString()});
    });
    vertical.Bind(wxEVT_SPIN, [&events](wxSpinEvent& event)
    {
        events.push_back(
            {event.GetEventType(),
             static_cast<double>(event.GetPosition()), wxString()});
    });

    REQUIRE(vertical.WinUIStepForTesting(+1));
    CHECK(vertical.GetValue() == 0);
    REQUIRE(events.size() == 2);
    CHECK(events[0].type == wxEVT_SPIN_UP);
    CHECK(events[0].value == 0);
    CHECK(events[1].type == wxEVT_SPIN);
    CHECK(events[1].value == 0);

    events.clear();
    vertical.SetValue(6);
    CHECK(events.empty());

    vertical.Enable(false);
    CHECK_FALSE(vertical.WinUIStepForTesting(+1));
    CHECK(vertical.GetValue() == 6);
    CHECK(events.empty());

    wxSpinButton horizontal(
        parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxSP_HORIZONTAL | wxSP_ARROW_KEYS);
    REQUIRE(horizontal.WinUIGetPeerLayoutForTesting(
        &isVertical, &rows, &columns));
    CHECK_FALSE(isVertical);
    CHECK(rows == 0);
    CHECK(columns == 2);
}

TEST_CASE("wxWinUI SpinButton veto and reentrant range stay canonical",
          "[winui-spin-model][spinbutton][reentrant]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinButton vetoed(parent);
    vetoed.SetRange(0, 10);
    vetoed.SetValue(5);
    vetoed.Bind(wxEVT_SPIN_UP, [](wxSpinEvent& event)
    {
        event.Veto();
    });
    CHECK_FALSE(vetoed.WinUIStepForTesting(+1));
    CHECK(vetoed.GetValue() == 5);

    wxSpinButton reentrant(parent);
    reentrant.SetRange(0, 10);
    reentrant.SetValue(9);
    reentrant.Bind(wxEVT_SPIN_UP, [&reentrant](wxSpinEvent&)
    {
        reentrant.SetRange(0, 3);
    });
    CHECK_FALSE(reentrant.WinUIStepForTesting(+1));
    CHECK(reentrant.GetValue() == 3);
}

TEST_CASE("wxWinUI SpinButton callbacks are destruction-safe",
          "[winui-spin-model][spinbutton][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinButton *spin = new wxSpinButton(parent);
    unsigned changedEvents = 0;
    spin->Bind(wxEVT_SPIN_UP, [&spin](wxSpinEvent&)
    {
        wxSpinButton * const doomed = spin;
        spin = nullptr;
        delete doomed;
    });
    spin->Bind(wxEVT_SPIN, [&changedEvents](wxSpinEvent&)
    {
        ++changedEvents;
    });

    wxSpinButton * const invoking = spin;
    CHECK_FALSE(invoking->WinUIStepForTesting(+1));
    CHECK(spin == nullptr);
    CHECK(changedEvents == 0);

    for ( int i = 0; i < 100; ++i )
    {
        wxSpinButton * const transient = new wxSpinButton(parent);
        delete transient;
    }
    wxYield();
}

TEST_CASE("wxWinUI SpinButton exposes the native UIA spinner topology",
          "[winui-spin-model][spinbutton][uia]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinButton spin(parent);
    spin.SetRange(0, 10);
    spin.SetValue(4);

    int rootType = -1;
    wxString rootClass;
    wxString incrementId;
    wxString decrementId;
    wxString incrementName;
    wxString decrementName;
    REQUIRE(spin.WinUIGetAutomationForTesting(
        &rootType, &rootClass,
        &incrementId, &decrementId,
        &incrementName, &decrementName));
    CHECK(rootType ==
          static_cast<int>(MUXAP::AutomationControlType::Spinner));
    CHECK(rootClass == "wxSpinButton");
    CHECK(incrementId == "SmallIncrement");
    CHECK(decrementId == "SmallDecrement");
    CHECK_FALSE(incrementName.empty());
    CHECK_FALSE(decrementName.empty());

    REQUIRE(spin.WinUIInvokeArrowForTesting(+1));
    CHECK(spin.GetValue() == 5);
    REQUIRE(spin.WinUIInvokeArrowForTesting(-1));
    CHECK(spin.GetValue() == 4);

    spin.Enable(false);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(&spin);
    REQUIRE(host != nullptr);
    host->FlushSync();
    CHECK_FALSE(spin.WinUIInvokeArrowForTesting(+1));
    CHECK(spin.GetValue() == 4);
}

#endif // wxUSE_SPINBTN

#if wxUSE_SPINCTRL

TEST_CASE("wxWinUI integer NumberBox has a silent canonical model",
          "[winui-spin-model][spinctrl][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinCtrl spin;
    REQUIRE(spin.Create(
        parent, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS | wxSP_WRAP | wxTE_PROCESS_ENTER,
        0, 100, 17));
    CHECK(spin.GetValue() == 17);
    CHECK(spin.GetTextValue() == "17");

    EventCounter textEvents(&spin, wxEVT_TEXT);
    EventCounter spinEvents(&spin, wxEVT_SPINCTRL);
    EventCounter enterEvents(&spin, wxEVT_TEXT_ENTER);

    spin.SetRange(10, 20);
    spin.SetIncrement(3);
    spin.SetValue(19);
    spin.SetSnapToTicks(true);
    CHECK(spin.GetValue() == 18);
    CHECK(textEvents.GetCount() == 0);
    CHECK(spinEvents.GetCount() == 0);

    double minimum = 0;
    double maximum = 0;
    double increment = 0;
    bool wrap = false;
    wxString peerText;
    REQUIRE(spin.WinUIGetPeerStateForTesting(
        &minimum, &maximum, &increment, &wrap, &peerText));
    CHECK(minimum == 10);
    CHECK(maximum == 20);
    CHECK(increment == 3);
    CHECK(wrap);
    CHECK(peerText == spin.GetTextValue());

    spin.SetSnapToTicks(false);
    spin.SetRange(200, 300);
    spin.SetValue(250);
    REQUIRE(spin.WinUIGetPeerStateForTesting(
        &minimum, &maximum, nullptr, nullptr, nullptr));
    CHECK(minimum == 200);
    CHECK(maximum == 300);
    CHECK(spin.GetValue() == 250);

    spin.SetRange(0, 255);
    spin.SetValue(42);
    REQUIRE(spin.SetBase(16));
    CHECK(spin.GetBase() == 16);
    CHECK(spin.GetTextValue().Lower().Contains("2a"));
    CHECK_FALSE(spin.SetBase(8));

    textEvents.Clear();
    spinEvents.Clear();
    enterEvents.Clear();
    REQUIRE(spin.WinUISetPeerTextForTesting("0x2b"));
    REQUIRE(spin.WinUIEnterForTesting());
    CHECK(spin.GetValue() == 43);
    CHECK(spin.GetTextValue().Lower().Contains("2b"));
    CHECK(textEvents.GetCount() >= 1);
    CHECK(spinEvents.GetCount() == 1);
    CHECK(enterEvents.GetCount() == 1);

    textEvents.Clear();
    spinEvents.Clear();
    enterEvents.Clear();
    spin.SetRange(-1, 255);
    CHECK(spin.GetMin() == 0);
    CHECK(spin.GetMax() == 255);

    REQUIRE(spin.SetBase(10));
    spin.SetValue("");
    CHECK(spin.GetValue() == 0);
    CHECK(spin.GetTextValue().empty());
    CHECK(textEvents.GetCount() == 0);
    CHECK(spinEvents.GetCount() == 0);

    spin.SetValue(123);
    spin.SetSelection(-1, -1);
    wxYield();
    long from = -1;
    long to = -1;
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(from == 0);
    CHECK(to == 3);

    REQUIRE(spin.WinUIEnterForTesting());
    CHECK(enterEvents.GetCount() == 1);

    wxSpinCtrl negative(
        parent, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, -10, 10, 0);
    negative.SetIncrement(2);
    negative.SetSnapToTicks(true);
    negative.SetValue(-1);
    CHECK(negative.GetValue() == 0);

    wxSpinCtrl overflow(
        parent, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, INT_MIN, INT_MAX, INT_MAX);
    overflow.SetIncrement(INT_MAX);
    CHECK_FALSE(overflow.WinUIStepForTesting(+1));
    CHECK(overflow.GetValue() == INT_MAX);
}

TEST_CASE("wxWinUI integer NumberBox selects changed programmatic text",
          "[winui-spin-model][spinctrl][selection]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinCtrl spin(
        parent, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 0, 100, 7);
    EventCounter textEvents(&spin, wxEVT_TEXT);
    EventCounter spinEvents(&spin, wxEVT_SPINCTRL);

    long from = -1;
    long to = -1;
    spin.SetSelection(1, 1);
    spin.SetValue(42);
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(from == 0);
    CHECK(to == 2);

    double peerValue = -1;
    wxString peerText;
    REQUIRE(spin.WinUIGetPeerStateForTesting(
        nullptr, nullptr, nullptr, nullptr, &peerText, &peerValue));
    CHECK(spin.GetValue() == 42);
    CHECK(spin.GetTextValue() == "42");
    CHECK(peerValue == 42.0);
    CHECK(peerText == "42");

    // Like the generic control, an unchanged numeric setter preserves the
    // current caret instead of selecting the text again.
    spin.SetSelection(1, 1);
    spin.SetValue(42);
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(from == 1);
    CHECK(to == 1);

    spin.SetRange(50, 100);
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(spin.GetValue() == 50);
    CHECK(spin.GetTextValue() == "50");
    CHECK(from == 0);
    CHECK(to == 2);

    const wxString invalid = "not-a-number";
    spin.SetValue(invalid);
    REQUIRE(spin.WinUIGetPeerStateForTesting(
        nullptr, nullptr, nullptr, nullptr, &peerText, &peerValue));
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(spin.GetValue() == 50);
    CHECK(spin.GetTextValue() == invalid);
    CHECK(peerValue == 50.0);
    CHECK(peerText == invalid);
    CHECK(from == 0);
    CHECK(to == static_cast<long>(invalid.length()));

    REQUIRE(spin.WinUIRetemplateForTesting());
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(from == 0);
    CHECK(to == static_cast<long>(invalid.length()));
    CHECK(textEvents.GetCount() == 0);
    CHECK(spinEvents.GetCount() == 0);
}

TEST_CASE("wxWinUI integer NumberBox emits text then spin once",
          "[winui-spin-model][spinctrl][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinCtrl spin(
        parent, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS | wxSP_WRAP, 0, 10, 9);

    std::vector<ObservedSpinEvent> events;
    spin.Bind(wxEVT_TEXT, [&events](wxCommandEvent& event)
    {
        events.push_back(
            {event.GetEventType(),
             static_cast<double>(event.GetInt()),
             event.GetString()});
    });
    spin.Bind(wxEVT_SPINCTRL, [&events](wxSpinEvent& event)
    {
        events.push_back(
            {event.GetEventType(),
             static_cast<double>(event.GetPosition()),
             event.GetString()});
    });

    REQUIRE(spin.WinUIStepForTesting(+1));
    CHECK(spin.GetValue() == 10);
    REQUIRE(events.size() == 2);
    CHECK(events[0].type == wxEVT_TEXT);
    CHECK(events[0].text == "10");
    CHECK(events[1].type == wxEVT_SPINCTRL);
    CHECK(events[1].value == 10);

    events.clear();
    REQUIRE(spin.WinUIStepForTesting(+1));
    CHECK(spin.GetValue() == 0);
    REQUIRE(events.size() == 2);
    CHECK(events[0].type == wxEVT_TEXT);
    CHECK(events[1].type == wxEVT_SPINCTRL);

    events.clear();
    spin.SetValue(5);
    CHECK(events.empty());

    spin.Enable(false);
    CHECK_FALSE(spin.WinUIStepForTesting(+1));
    REQUIRE(spin.WinUISetPeerValueForTesting(7));
    CHECK(spin.GetValue() == 5);
    CHECK(events.empty());
}

TEST_CASE("wxWinUI NumberBox survives destruction from text callback",
          "[winui-spin-model][spinctrl][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinCtrl *spin = new wxSpinCtrl(parent);
    unsigned spinEvents = 0;
    spin->Bind(wxEVT_TEXT, [&spin](wxCommandEvent&)
    {
        delete spin;
        spin = nullptr;
    });
    spin->Bind(wxEVT_SPINCTRL, [&spinEvents](wxSpinEvent&)
    {
        ++spinEvents;
    });

    wxSpinCtrl * const original = spin;
    CHECK_FALSE(original->WinUIStepForTesting(+1));
    CHECK(spin == nullptr);
    CHECK(spinEvents == 0);
}

TEST_CASE("wxWinUI double NumberBox derives precision and snaps",
          "[winui-spin-model][spinctrldouble][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinCtrlDouble spin(
        parent, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS | wxSP_WRAP,
        0.0, 10.0, 0.0, 0.1);
    CHECK(spin.GetDigits() == 1);
    CHECK(spin.GetTextValue() == "0.0");

    EventCounter textEvents(&spin, wxEVT_TEXT);
    EventCounter spinEvents(&spin, wxEVT_SPINCTRLDOUBLE);

    spin.SetValue(1.234);
    CHECK(spin.GetTextValue() == "1.2");
    spin.SetIncrement(0.01);
    CHECK(spin.GetDigits() == 2);
    CHECK(spin.GetTextValue() == "1.20");
    spin.SetDigits(5);
    spin.SetIncrement(2.5);
    spin.SetValue(7.5);
    CHECK(spin.GetTextValue() == "7.50000");
    CHECK(textEvents.GetCount() == 0);
    CHECK(spinEvents.GetCount() == 0);

    spin.SetRange(0.0, 10.0);
    spin.SetIncrement(0.25);
    spin.SetDigits(2);
    spin.SetSnapToTicks(true);
    spin.SetValue(1.13);
    CHECK(spin.GetValue() == 1.25);
    CHECK(spin.GetTextValue() == "1.25");

    textEvents.Clear();
    spinEvents.Clear();
    REQUIRE(spin.WinUIStepForTesting(+1));
    CHECK(spin.GetValue() == 1.5);
    CHECK(textEvents.GetCount() == 1);
    CHECK(spinEvents.GetCount() == 1);

    spin.SetValue("");
    CHECK(spin.GetValue() == 0.0);
    CHECK(spin.GetTextValue().empty());
    CHECK(textEvents.GetCount() == 1);
    CHECK(spinEvents.GetCount() == 1);

    wxSpinCtrlDouble negative(
        parent, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, -10.0, 10.0, 0.0, 2.0);
    negative.SetSnapToTicks(true);
    negative.SetValue(-1.0);
    CHECK(negative.GetValue() == 0.0);

    textEvents.Clear();
    spinEvents.Clear();
    spin.Enable(false);
    CHECK_FALSE(spin.WinUIStepForTesting(+1));
    REQUIRE(spin.WinUISetPeerValueForTesting(4.0));
    CHECK(spin.GetValue() == 0.0);
    CHECK(textEvents.GetCount() == 0);
    CHECK(spinEvents.GetCount() == 0);
}

TEST_CASE("wxWinUI double NumberBox recanonicalizes and selects setters",
          "[winui-spin-model][spinctrldouble][selection]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinCtrlDouble spin(
        parent, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS, 0.0, 10.0, 1.2345, 0.0001);
    EventCounter textEvents(&spin, wxEVT_TEXT);
    EventCounter spinEvents(&spin, wxEVT_SPINCTRLDOUBLE);

    long from = -1;
    long to = -1;
    // NumberBox creates its editable template part on Loaded on some WinUI
    // versions. Resolve that initial template before testing synchronous
    // setter updates; later retemplate coverage remains explicitly deferred.
    wxYield();
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    spin.SetSelection(2, 2);
    spin.SetValue(7.6543);
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(spin.GetValue() == 7.6543);
    CHECK(spin.GetTextValue() == "7.6543");
    CHECK(from == 0);
    CHECK(to == 6);

    spin.SetSelection(2, 2);
    spin.SetDigits(2);
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(spin.GetDigits() == 2);
    CHECK(spin.GetValue() == 7.65);
    CHECK(spin.GetTextValue() == "7.65");
    CHECK(from == 0);
    CHECK(to == 4);

    double peerValue = -1;
    wxString peerText;
    REQUIRE(spin.WinUIGetPeerStateForTesting(
        nullptr, nullptr, nullptr, nullptr, &peerText, &peerValue));
    CHECK(peerValue == spin.GetValue());
    CHECK(peerText == spin.GetTextValue());

    spin.SetSelection(1, 1);
    spin.SetRange(8.0, 9.0);
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(spin.GetValue() == 8.0);
    CHECK(spin.GetTextValue() == "8.00");
    CHECK(from == 0);
    CHECK(to == 4);

    const wxString invalid = "bad.double";
    spin.SetValue(invalid);
    REQUIRE(spin.WinUIGetPeerStateForTesting(
        nullptr, nullptr, nullptr, nullptr, &peerText, &peerValue));
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(spin.GetValue() == 8.0);
    CHECK(spin.GetTextValue() == invalid);
    CHECK(peerValue == 8.0);
    CHECK(peerText == invalid);
    CHECK(from == 0);
    CHECK(to == static_cast<long>(invalid.length()));

    REQUIRE(spin.WinUIRetemplateForTesting());
    REQUIRE(spin.WinUIGetPeerSelectionForTesting(&from, &to));
    CHECK(from == 0);
    CHECK(to == static_cast<long>(invalid.length()));
    CHECK(textEvents.GetCount() == 0);
    CHECK(spinEvents.GetCount() == 0);
}

TEST_CASE("wxWinUI double NumberBox survives destruction from text callback",
          "[winui-spin-model][spinctrldouble][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxSpinCtrlDouble *spin = new wxSpinCtrlDouble(parent);
    unsigned spinEvents = 0;
    spin->Bind(wxEVT_TEXT, [&spin](wxCommandEvent&)
    {
        wxSpinCtrlDouble * const doomed = spin;
        spin = nullptr;
        delete doomed;
    });
    spin->Bind(
        wxEVT_SPINCTRLDOUBLE,
        [&spinEvents](wxSpinDoubleEvent&)
        {
            ++spinEvents;
        });

    wxSpinCtrlDouble * const invoking = spin;
    CHECK_FALSE(invoking->WinUIStepForTesting(+1));
    CHECK(spin == nullptr);
    CHECK(spinEvents == 0);

    for ( int i = 0; i < 100; ++i )
    {
        wxSpinCtrlDouble * const transient =
            new wxSpinCtrlDouble(parent);
        delete transient;
    }
    wxYield();
}

TEST_CASE("wxWinUI NumberBox Configure survives synchronous Loaded destruction",
          "[winui-spin-model][spinctrl][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("integer")
    {
        wxSpinCtrl *spin = new wxSpinCtrl;
        wxSpinCtrl * const invoking = spin;
        bool attached = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                attached = true;
                CHECK(window == invoking);
                wxSpinCtrl * const doomed = spin;
                spin = nullptr;
                delete doomed;
            });

        const bool created = invoking->Create(
            parent, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize,
            wxSP_ARROW_KEYS, 0, 100, 17);
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(spin == nullptr);
        delete spin;
    }

    SECTION("double")
    {
        wxSpinCtrlDouble *spin = new wxSpinCtrlDouble;
        wxSpinCtrlDouble * const invoking = spin;
        bool attached = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                attached = true;
                CHECK(window == invoking);
                wxSpinCtrlDouble * const doomed = spin;
                spin = nullptr;
                delete doomed;
            });

        const bool created = invoking->Create(
            parent, wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize,
            wxSP_ARROW_KEYS, 0.0, 100.0, 17.5, 0.5);
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(spin == nullptr);
        delete spin;
    }
}

TEST_CASE("wxWinUI NumberBox ForceRender never resumes through a dead owner",
          "[winui-spin-model][spinctrl][lifetime][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("integer")
    {
        wxSpinCtrl *spin = new wxSpinCtrl(parent);
        wxSpinCtrl * const invoking = spin;
        if ( wxWinUITopLevelHost * const host =
                 wxWinUITopLevelHost::ForWindow(parent, false) )
        {
            host->FlushSync();
        }
        invoking->SetSize(wxSize(173, 41));
        bool slotSynced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                slotSynced = true;
                CHECK(window == invoking);
                wxSpinCtrl * const doomed = spin;
                spin = nullptr;
                delete doomed;
            });

        invoking->SetValue(23);
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(slotSynced);
        CHECK(spin == nullptr);
        delete spin;
    }

    SECTION("double")
    {
        wxSpinCtrlDouble *spin =
            new wxSpinCtrlDouble(parent);
        wxSpinCtrlDouble * const invoking = spin;
        if ( wxWinUITopLevelHost * const host =
                 wxWinUITopLevelHost::ForWindow(parent, false) )
        {
            host->FlushSync();
        }
        invoking->SetSize(wxSize(181, 43));
        bool slotSynced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                slotSynced = true;
                CHECK(window == invoking);
                wxSpinCtrlDouble * const doomed = spin;
                spin = nullptr;
                delete doomed;
            });

        invoking->SetValue(23.5);
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(slotSynced);
        CHECK(spin == nullptr);
        delete spin;
    }
}

#endif // wxUSE_SPINCTRL

#endif // __WXWINUI__ && wxUSE_WINUI3
