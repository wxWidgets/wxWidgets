///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/infobar.cpp
// Purpose:     wxInfoBar  tests
// Author:      Blake Madden
// Created:     2025-6-02
// Copyright:   (c) 2025 Blake Madden
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"
#include "waitfor.h"

#if wxUSE_INFOBAR

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
    #include "wx/sizer.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/infobar.h"
#include "wx/log.h"

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

#ifdef __WXWINUI__
    #include "wx/winui/private/tlwhost.h"
    #include "host-geometry-test-access.h"
    #include "winui/test-support/infobar-test-access.h"
#endif

#ifdef __WXQT__
    #include <QtWidgets/QWidget>
#endif

#include "waitfor.h"

#include <memory>

#ifdef __WXWINUI__

namespace
{

struct InfoBarProjectionStormContext
{
    wxInfoBar *bar = nullptr;
    int calls = 0;
    int targetCalls = 0;
    wxWindowID firstId = wxID_NONE;
};

class YieldingInfoBarLogTarget final : public wxLog
{
public:
    YieldingInfoBarLogTarget()
        : m_previous(wxLog::SetActiveTarget(this))
    {
    }

    ~YieldingInfoBarLogTarget() override
    {
        wxLog::SetActiveTarget(m_previous);
    }

    bool DidYield() const { return m_didYield; }

protected:
    void DoLogText(const wxString&) override
    {
        if ( m_didYield )
            return;

        m_didYield = true;
        wxYield();
    }

private:
    wxLog* m_previous;
    bool m_didYield = false;
};

void ContinueInfoBarProjectionStorm(void *opaque)
{
    auto * const context =
        static_cast<InfoBarProjectionStormContext *>(opaque);
    ++context->calls;
    if ( context->calls < context->targetCalls )
    {
        wxWinUIInfoBarTestAccess::SetNextContentWriteHook(
            *context->bar, &ContinueInfoBarProjectionStorm, context);
    }

    context->bar->AddButton(
        context->firstId + context->calls,
        wxString::Format("nested %d", context->calls));
}

} // namespace

#endif // __WXWINUI__

TEST_CASE("wxInfoBar::Buttons", "[wxInfoBar][winui-v0-supported]")
{
    const auto info = make_unique<wxInfoBar>(wxTheApp->GetTopWindow(), wxID_ANY,
                                             wxINFOBAR_CHECKBOX);

    CHECK(info->GetButtonCount() == 0);

    const int buttonId = wxID_HIGHEST + 1000;
    info->AddButton(buttonId, "test");

    CHECK(info->GetButtonCount() == 1);
    CHECK(info->GetButtonId(0) == buttonId);
    CHECK(info->HasButtonId(buttonId));

    info->RemoveButton(buttonId);
    CHECK(info->GetButtonCount() == 0);

    info->AddButton(buttonId, "first");
    info->AddButton(buttonId + 1, "second");
    info->AddButton(buttonId + 2, "third");
    CHECK(info->GetButtonCount() == 3);

    info->RemoveAllButtons();
    CHECK(info->GetButtonCount() == 0);
    CHECK(!info->HasButtonId(buttonId));
    CHECK(!info->HasButtonId(buttonId + 1));
    CHECK(!info->HasButtonId(buttonId + 2));

    // Calling it when there are no buttons is harmless.
    info->RemoveAllButtons();
    CHECK(info->GetButtonCount() == 0);
}

TEST_CASE("wxInfoBarGeneric::CloseButton", "[wxInfoBar][wxInfoBarGeneric]")
{
    // We specifically use wxInfoBarGeneric because the native version wouldn't
    // allow us to find the close button using FindWindow() and we can't write
    // this test using it.
    const auto info = make_unique<wxInfoBarGeneric>(wxTheApp->GetTopWindow());

    wxWindow* const closeButton = info->FindWindow(wxID_CLOSE);
    REQUIRE(closeButton);
    CHECK(closeButton->IsShown());

    // Adding a button replaces the close button.
    const int buttonId = wxID_HIGHEST + 1000;
    info->AddButton(buttonId, "test");
    CHECK(!closeButton->IsShown());

    // And removing the last one must restore it.
    info->RemoveButton(buttonId);
    CHECK(closeButton->IsShown());
}

TEST_CASE("wxInfoBarGeneric::RemoveAllButtons", "[wxInfoBar][wxInfoBarGeneric]")
{
    const auto info = make_unique<wxInfoBarGeneric>(wxTheApp->GetTopWindow());

    wxWindow* const closeButton = info->FindWindow(wxID_CLOSE);
    REQUIRE(closeButton);

    const int buttonId = wxID_HIGHEST + 1000;
    info->AddButton(buttonId, "first");
    info->AddButton(buttonId + 1, "second");
    CHECK(!closeButton->IsShown());

    info->RemoveAllButtons();
    CHECK(!info->FindWindow(buttonId));
    CHECK(!info->FindWindow(buttonId + 1));
    CHECK(closeButton->IsShown());
}

TEST_CASE("wxInfoBar::ShowCloseButton", "[wxInfoBar]")
{
    // This test uses the native version under GTK, so we can only check that
    // the close button is never counted as one of the custom buttons.
    const auto info = make_unique<wxInfoBar>(wxTheApp->GetTopWindow());

    info->ShowCloseButton(wxInfoBar::ShowButton::Yes);
    CHECK(info->GetButtonCount() == 0);
    CHECK(!info->HasButtonId(wxID_CLOSE));

    const int buttonId = wxID_HIGHEST + 1000;
    info->AddButton(buttonId, "test");
    CHECK(info->GetButtonCount() == 1);
    CHECK(info->GetButtonId(0) == buttonId);
    CHECK(!info->HasButtonId(wxID_CLOSE));

    info->ShowCloseButton(wxInfoBar::ShowButton::No);
    info->RemoveAllButtons();
    CHECK(info->GetButtonCount() == 0);
    CHECK(!info->HasButtonId(wxID_CLOSE));
}

TEST_CASE("wxInfoBar::Checkbox", "[wxInfoBar][winui-v0-supported]")
{
    const std::unique_ptr<wxInfoBar>
        info(new wxInfoBar(wxTheApp->GetTopWindow(), wxID_ANY, wxINFOBAR_CHECKBOX));

    // no checkbox shown yet
    CHECK_FALSE(info->IsCheckBoxChecked());

    info->ShowCheckBox("Remember my choice", true);
    CHECK(info->IsCheckBoxChecked());

    // custom buttons and the checkbox must be able to coexist
    const int buttonId = wxID_HIGHEST + 1001;
    info->AddButton(buttonId, "test");
    CHECK(info->GetButtonCount() == 1);
    CHECK(info->HasButtonId(buttonId));
    CHECK(info->IsCheckBoxChecked());

    info->ShowCheckBox("Remember my choice", false);
    CHECK_FALSE(info->IsCheckBoxChecked());
    CHECK(info->GetButtonCount() == 1);

    // an empty label hides the checkbox again without disturbing the rest
    info->ShowCheckBox(wxString(), false);
    CHECK_FALSE(info->IsCheckBoxChecked());
    CHECK(info->GetButtonCount() == 1);
}

TEST_CASE("wxInfoBar::Effects", "[wxInfoBar][winui-v0-supported]")
{
    const std::unique_ptr<wxInfoBar>
        info(new wxInfoBar(wxTheApp->GetTopWindow()));

    // the default duration means "use the platform default"
    CHECK(info->GetEffectDuration() == 0);

    // without a containing sizer the placement is unknown, so the automatic
    // effects resolve to none
    CHECK(info->GetShowEffect() == wxSHOW_EFFECT_NONE);
    CHECK(info->GetHideEffect() == wxSHOW_EFFECT_NONE);

    info->SetShowHideEffects(wxSHOW_EFFECT_BLEND, wxSHOW_EFFECT_EXPAND);
    CHECK(info->GetShowEffect() == wxSHOW_EFFECT_BLEND);
    CHECK(info->GetHideEffect() == wxSHOW_EFFECT_EXPAND);

    info->SetEffectDuration(250);
    CHECK(info->GetEffectDuration() == 250);

#ifdef __WXWINUI__
    // this port clamps the duration: a negative value can never reach the
    // unsigned AnimateWindow() timeout, and absurd durations are capped
    info->SetEffectDuration(-5);
    CHECK(info->GetEffectDuration() == 0);
    info->SetEffectDuration(999999);
    CHECK(info->GetEffectDuration() == 10000);
#endif // __WXWINUI__

    info->SetShowHideEffects(wxSHOW_EFFECT_NONE, wxSHOW_EFFECT_NONE);
    CHECK(info->GetShowEffect() == wxSHOW_EFFECT_NONE);
    CHECK(info->GetHideEffect() == wxSHOW_EFFECT_NONE);
}

TEST_CASE("wxInfoBar::RemoveLastAdded", "[wxInfoBar][winui-v0-supported]")
{
    const std::unique_ptr<wxInfoBar>
        info(new wxInfoBar(wxTheApp->GetTopWindow()));

    // with a repeated id, RemoveButton() must remove the most recently
    // added button, not the first one
    const int idA = wxID_HIGHEST + 1010;
    const int idB = wxID_HIGHEST + 1011;
    info->AddButton(idA, "first A");
    info->AddButton(idB, "B");
    info->AddButton(idA, "second A");

    info->RemoveButton(idA);
    REQUIRE(info->GetButtonCount() == 2);
    CHECK(info->GetButtonId(0) == idA);
    CHECK(info->GetButtonId(1) == idB);
}

TEST_CASE("wxInfoBar::Appearance", "[wxInfoBar][winui-v0-supported]")
{
    // setting the appearance before creation must be preserved by Create()
    std::unique_ptr<wxInfoBar> info(new wxInfoBar());

    const wxFont boldFont(wxFontInfo(12).Bold());
    CHECK(info->SetFont(boldFont));
    CHECK(info->SetForegroundColour(*wxRED));

    REQUIRE(info->Create(wxTheApp->GetTopWindow()));
    CHECK(info->GetFont().GetWeight() == wxFONTWEIGHT_BOLD);
    CHECK(info->GetForegroundColour() == *wxRED);
#ifdef __WXQT__
    REQUIRE(info->GetHandle());
    CHECK(info->GetHandle()->font().bold());
    CHECK(info->GetHandle()->palette().color(
              info->GetHandle()->foregroundRole()) == wxRED->GetQColor());
#endif

    // and changing it after creation must work too
    const wxFont italicFont(wxFontInfo(10).Italic());
    CHECK(info->SetFont(italicFont));
    CHECK(info->GetFont().GetStyle() == wxFONTSTYLE_ITALIC);
#ifdef __WXQT__
    CHECK(info->GetHandle()->font().italic());
#endif

    CHECK(info->SetForegroundColour(*wxBLUE));
    CHECK(info->GetForegroundColour() == *wxBLUE);
#ifdef __WXQT__
    CHECK(info->GetHandle()->palette().color(
              info->GetHandle()->foregroundRole()) == wxBLUE->GetQColor());
#endif

    // resetting the font with wxNullFont goes back to the default one (and
    // must reach the native peer too, not leave the old font behind)
    CHECK(info->SetFont(wxNullFont));
    CHECK(info->GetFont().IsOk());
    CHECK(info->GetFont().GetStyle() != wxFONTSTYLE_ITALIC);
#ifdef __WXQT__
    CHECK_FALSE(info->GetHandle()->font().italic());
#endif
}

TEST_CASE("wxInfoBar::EffectShowHide", "[wxInfoBar][winui-v0-supported]")
{
    // exercise the real show/hide path with effects enabled: the bar lives
    // in a sizer (so the automatic placement resolves) inside a fixed-width
    // panel (so an overlong message cannot grow the window)
    wxPanel* const panel = new wxPanel(wxTheApp->GetTopWindow());
    panel->SetClientSize(panel->FromDIP(wxSize(400, 300)));

    wxInfoBar* const info = new wxInfoBar(panel);
    wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(info, wxSizerFlags().Expand());
    sizer->AddStretchSpacer();
    panel->SetSizer(sizer);
    panel->Show();
    panel->Layout();

    info->SetEffectDuration(1);

    wxString longMessage;
    for ( int i = 0; i < 20; ++i )
        longMessage += "wrapped or ellipsized under a constrained width ";
    info->ShowMessage(longMessage);

    CHECK(info->IsShown());
#ifdef __WXMSW__
    // checking the real window state instead of just the wx flag: note that
    // this only automates the normal (successful) animation path, the
    // AnimateWindow-failure fallback has no test seam and is covered by
    // code review only
    CHECK(::IsWindowVisible(GetHwndOf(info)));
#endif
    CHECK(info->GetSize().x <= panel->GetClientSize().x);

    info->Dismiss();
    CHECK_FALSE(info->IsShown());
#ifdef __WXMSW__
    CHECK_FALSE(::IsWindowVisible(GetHwndOf(info)));
#endif

    panel->Destroy();
}

#ifdef __WXWINUI__

TEST_CASE("wxInfoBar::WinUIContentProjectionIsLastWriterWins",
          "[wxInfoBar][reentrancy][winui-v0-supported]")
{
    wxInfoBar info(
        wxTheApp->GetTopWindow(), wxID_ANY, wxINFOBAR_CHECKBOX);
    const wxWindowID outerId = wxID_HIGHEST + 1100;
    const wxWindowID nestedId = outerId + 1;
    InfoBarProjectionStormContext context;
    context.bar = &info;
    context.targetCalls = 1;
    context.firstId = outerId;
    wxWinUIInfoBarTestAccess::SetNextContentWriteHook(
        info, &ContinueInfoBarProjectionStorm, &context);

    info.AddButton(outerId, "outer");

    CHECK(context.calls == 1);
    CHECK(info.GetButtonCount() == 2);
    CHECK_FALSE(wxWinUIInfoBarTestAccess::HasDeferredContentProjection(info));
    CHECK_FALSE(wxWinUIInfoBarTestAccess::IsContentProjectionQuarantined(info));

    int nestedEvents = 0;
    info.Bind(
        wxEVT_BUTTON,
        [&](wxCommandEvent&) { ++nestedEvents; },
        nestedId);
    REQUIRE(wxWinUIInfoBarTestAccess::ClickButton(info, nestedId));
    CHECK(nestedEvents == 1);
}

TEST_CASE("wxInfoBar::WinUIContentProjectionStormIsBoundedAndRearmable",
          "[wxInfoBar][reentrancy][quarantine][winui-v0-supported]")
{
    wxInfoBar info(
        wxTheApp->GetTopWindow(), wxID_ANY, wxINFOBAR_CHECKBOX);
    const wxWindowID firstId = wxID_HIGHEST + 1120;
    InfoBarProjectionStormContext context;
    context.bar = &info;
    context.targetCalls = 1000000;
    context.firstId = firstId;
    wxWinUIInfoBarTestAccess::SetNextContentWriteHook(
        info, &ContinueInfoBarProjectionStorm, &context);

    // A log target is application code and may pump the loop. The deferred
    // replay must not be consumable until the active projection has ended.
    YieldingInfoBarLogTarget yieldingLog;
    info.AddButton(firstId, "begin");

    // The overrun is reported as a debug diagnostic: whether it reaches this
    // log target depends on the build, so it is not required to yield.
    CHECK(context.calls == 8);
    CHECK(wxWinUIInfoBarTestAccess::HasDeferredContentProjection(info));
    REQUIRE(WaitFor("bounded InfoBar content quarantine", [&]()
    {
        return context.calls == 16 &&
               !wxWinUIInfoBarTestAccess::HasDeferredContentProjection(info) &&
               wxWinUIInfoBarTestAccess::IsContentProjectionQuarantined(info);
    }));

    const int callsAtQuarantine = context.calls;
    wxYield();
    wxYield();
    CHECK(context.calls == callsAtQuarantine);

    wxWinUIInfoBarTestAccess::SetNextContentWriteHook(info, nullptr, nullptr);
    const wxWindowID recoveryId = firstId + 100;
    info.AddButton(recoveryId, "recovered");
    CHECK_FALSE(wxWinUIInfoBarTestAccess::HasDeferredContentProjection(info));
    CHECK_FALSE(wxWinUIInfoBarTestAccess::IsContentProjectionQuarantined(info));

    int recoveryEvents = 0;
    info.Bind(
        wxEVT_BUTTON,
        [&](wxCommandEvent&) { ++recoveryEvents; },
        recoveryId);
    REQUIRE(wxWinUIInfoBarTestAccess::ClickButton(info, recoveryId));
    CHECK(recoveryEvents == 1);
}

TEST_CASE("wxInfoBar::WinUIContentWriteAndFlushMayDestroyOwner",
          "[wxInfoBar][reentrancy][lifetime][winui-v0-supported]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("content write")
    {
        std::unique_ptr<wxInfoBar> info(
            new wxInfoBar(parent, wxID_ANY, wxINFOBAR_CHECKBOX));
        wxInfoBar * const invoking = info.get();
        bool hookCalled = false;
        struct DeleteContext
        {
            std::unique_ptr<wxInfoBar> *owned;
            bool *called;
        } context{ &info, &hookCalled };
        wxWinUIInfoBarTestAccess::SetNextContentWriteHook(
            *invoking,
            [](void *opaque)
            {
                auto * const current =
                    static_cast<DeleteContext *>(opaque);
                *current->called = true;
                current->owned->reset();
            },
            &context);

        invoking->AddButton(wxID_HIGHEST + 1150, "delete");
        CHECK(hookCalled);
        CHECK_FALSE(info);
    }

    SECTION("host flush")
    {
        std::unique_ptr<wxInfoBar> info(new wxInfoBar(parent));
        wxInfoBar * const invoking = info.get();
        invoking->SetSize(parent->FromDIP(wxSize(287, 57)));
        // Resizing alone repositions the slot at once: ask for the full
        // sync this test is about.
        wxWinUIHostGeometryTestAccess::RequestSlotSync(invoking);
        bool hookCalled = false;
        wxWinUIHostGeometryTestAccess::SetNextSlotSyncedHook(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;
                hookCalled = true;
                info.reset();
            });

        invoking->ShowMessage("delete while flushing");
        wxWinUIHostGeometryTestAccess::SetNextSlotSyncedHook({});
        CHECK(hookCalled);
        CHECK_FALSE(info);
    }
}

// Drive the real native close-button path (XAML CloseButtonClick -> Closing
// -> wxEVT_BUTTON/wxID_CLOSE routing) deterministically, without the mouse,
// and check that the XAML peer state stays in sync with the wx one in every
// outcome of the event.
TEST_CASE("wxInfoBar::WinUICloseButton",
          "[wxInfoBar][winui-v0-supported]")
{
    const std::unique_ptr<wxInfoBar>
        info(new wxInfoBar(wxTheApp->GetTopWindow(), wxID_ANY, wxINFOBAR_CHECKBOX));

    // 0 = skip (unhandled), 1 = handled without dismissing, 2 = handled and
    // dismissing from the handler (the official sample's pattern), 3/4 =
    // opposite reentrant orders proving that the last public operation wins.
    int mode = 0;
    int closeEvents = 0;
    info->Bind(wxEVT_BUTTON,
        [&, bar = info.get()](wxCommandEvent& event)
        {
            ++closeEvents;
            switch ( mode )
            {
                case 0:
                    event.Skip();
                    break;
                case 1:
                    break;
                case 2:
                    bar->Dismiss();
                    break;
                case 3:
                    bar->Dismiss();
                    bar->ShowMessage("shown after dismiss");
                    break;
                case 4:
                    bar->ShowMessage("dismissed after show");
                    bar->Dismiss();
                    break;
            }
        },
        wxID_CLOSE);

    // programmatic Dismiss() must close the peer without emitting wxID_CLOSE
    info->ShowMessage("winui close-button test");
    CHECK(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    info->Dismiss();
    CHECK(closeEvents == 0);
    CHECK_FALSE(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    CHECK_FALSE(info->IsShown());

    // ShowMessage() must really re-open the XAML peer
    info->ShowMessage("re-opened");
    CHECK(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    CHECK(info->IsShown());

    // wait until the InfoBar template is realized so that the close button
    // exists, then click it: unhandled (skipped) event -> default dismiss
    bool clicked = false;
    for ( int i = 0;
          i < 300 && !(clicked = wxWinUIInfoBarTestAccess::ClickCloseButton(*info));
          ++i )
    {
        wxYield();
        wxMilliSleep(10);
    }
    REQUIRE(clicked);
    wxYield();
    CHECK(closeEvents == 1);
    CHECK_FALSE(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    CHECK_FALSE(info->IsShown());

    // handled without dismissing -> the closing is cancelled, both stay open
    info->ShowMessage("kept open");
    CHECK(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    mode = 1;
    closeEvents = 0;
    REQUIRE(wxWinUIInfoBarTestAccess::ClickCloseButton(*info));
    wxYield();
    CHECK(closeEvents == 1);
    CHECK(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    CHECK(info->IsShown());

    // handled with Dismiss() in the handler -> both closed, single event
    mode = 2;
    closeEvents = 0;
    REQUIRE(wxWinUIInfoBarTestAccess::ClickCloseButton(*info));
    wxYield();
    CHECK(closeEvents == 1);
    CHECK_FALSE(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    CHECK_FALSE(info->IsShown());

    // Reentrant close intent is ordered: the last public operation wins.
    info->ShowMessage("reentrant open wins");
    mode = 3;
    closeEvents = 0;
    REQUIRE(wxWinUIInfoBarTestAccess::ClickCloseButton(*info));
    wxYield();
    CHECK(closeEvents == 1);
    CHECK(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    CHECK(info->IsShown());

    mode = 4;
    closeEvents = 0;
    REQUIRE(wxWinUIInfoBarTestAccess::ClickCloseButton(*info));
    wxYield();
    CHECK(closeEvents == 1);
    CHECK_FALSE(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    CHECK_FALSE(info->IsShown());

    // and the cycle still works: the bar can be shown again afterwards
    info->ShowMessage("alive again");
    CHECK(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
    CHECK(info->IsShown());

    // Leave neither a closing animation nor a Loaded/layout callback queued
    // for the next test sharing this TLW. The public cycle was proved above;
    // this final close is test isolation, not another behavioural assertion.
    info->Dismiss();
    wxYield();
    CHECK_FALSE(wxWinUIInfoBarTestAccess::IsPeerOpen(*info));
}

TEST_CASE("wxInfoBar::WinUICallbacksMayDestroyOwner",
          "[wxInfoBar][lifetime][winui-v0-supported]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent != nullptr);

    SECTION("native close button")
    {
        std::unique_ptr<wxInfoBar> info(new wxInfoBar(parent));
        wxInfoBar * const raw = info.get();
        raw->Bind(
            wxEVT_BUTTON,
            [&info](wxCommandEvent&) { info.reset(); },
            wxID_CLOSE);
        raw->ShowMessage("destroy from native close");

        bool clicked = false;
        for ( int i = 0;
              i < 300 && info &&
                  !(clicked = wxWinUIInfoBarTestAccess::ClickCloseButton(*raw));
              ++i )
        {
            wxYield();
            wxMilliSleep(10);
        }
        REQUIRE(clicked);
        CHECK(info == nullptr);
        wxYield();
    }

    SECTION("custom button")
    {
        const wxWindowID buttonId = wxID_HIGHEST + 1012;
        std::unique_ptr<wxInfoBar> info(new wxInfoBar(parent));
        wxInfoBar * const raw = info.get();
        raw->AddButton(buttonId, "Destroy");
        raw->Bind(
            wxEVT_BUTTON,
            [&info](wxCommandEvent&) { info.reset(); },
            buttonId);
        raw->ShowMessage("destroy from custom button");

        REQUIRE(wxWinUIInfoBarTestAccess::ClickButton(*raw, buttonId));
        CHECK(info == nullptr);
        wxYield();
    }
}

#endif // __WXWINUI__

TEST_CASE("wxInfoBar::BestSizeInvalidation",
          "[wxInfoBar][winui-v0-supported]")
{
    const std::unique_ptr<wxInfoBar>
        info(new wxInfoBar(wxTheApp->GetTopWindow(), wxID_ANY, wxINFOBAR_CHECKBOX));

    info->ShowMessage("short");
    const wxSize bestShort = info->GetBestSize();
    CHECK(bestShort.x > 0);
    CHECK(bestShort.y > 0);

    // a much longer message must not shrink the best size and must not
    // break the layout machinery
    wxString longMessage;
    for ( int i = 0; i < 20; ++i )
        longMessage += "this is a rather long informational message ";
    info->ShowMessage(longMessage);

    const wxSize bestLong = info->GetBestSize();
    CHECK(bestLong.x > 0);
    CHECK(bestLong.y > 0);
    CHECK(bestLong.x >= bestShort.x);

    // adding buttons and the checkbox keeps the best size usable as well
    info->AddButton(wxID_HIGHEST + 1002, "button");
    info->ShowCheckBox("check", false);
    const wxSize bestFull = info->GetBestSize();
    CHECK(bestFull.x > 0);
    CHECK(bestFull.y > 0);

    info->Dismiss();
}
TEST_CASE("wxInfoBarGeneric::ShowCloseButton", "[wxInfoBar][wxInfoBarGeneric]")
{
    const auto info = make_unique<wxInfoBarGeneric>(wxTheApp->GetTopWindow());

    wxWindow* const closeButton = info->FindWindow(wxID_CLOSE);
    REQUIRE(closeButton);

    const auto isLastItem = [&info](wxWindow* win)
    {
        return info->GetSizer()->GetChildren().GetLast()->GetData()->GetWindow() == win;
    };

    const int buttonId = wxID_HIGHEST + 1000;

    SECTION("Always")
    {
        info->ShowCloseButton(wxInfoBar::ShowButton::Yes);
        CHECK(closeButton->IsShown());

        // The close button must remain shown and come after the new buttons.
        info->AddButton(buttonId, "first");
        CHECK(closeButton->IsShown());
        CHECK(isLastItem(closeButton));

        info->AddButton(buttonId + 1, "second");
        CHECK(closeButton->IsShown());
        CHECK(isLastItem(closeButton));

        CHECK(info->GetButtonCount() == 2);
        CHECK(info->GetButtonId(0) == buttonId);
        CHECK(info->GetButtonId(1) == buttonId + 1);
        CHECK(!info->HasButtonId(wxID_CLOSE));

        info->RemoveAllButtons();
        CHECK(closeButton->IsShown());

        // Restoring the default behaviour hides the close button if there are
        // any other buttons.
        info->AddButton(buttonId, "test");
        info->ShowCloseButton(wxInfoBar::ShowButton::Auto);
        CHECK(!closeButton->IsShown());
        CHECK(isLastItem(info->FindWindow(buttonId)));
    }

    SECTION("Never")
    {
        info->ShowCloseButton(wxInfoBar::ShowButton::No);
        CHECK(!closeButton->IsShown());

        info->AddButton(buttonId, "test");
        CHECK(!closeButton->IsShown());

        // Removing the last custom button must not restore the close one.
        info->RemoveButton(buttonId);
        CHECK(!closeButton->IsShown());
        CHECK(info->GetButtonCount() == 0);

        info->ShowCloseButton(wxInfoBar::ShowButton::Auto);
        CHECK(closeButton->IsShown());
    }
}

TEST_CASE("wxInfoBarGeneric::Colours", "[wxInfoBar][wxInfoBarGeneric]")
{
    wxWindow* const parent = wxTheApp->GetTopWindow();

    const auto ref = make_unique<wxInfoBarGeneric>(parent);
    const wxColour colBg = ref->GetBackgroundColour();

    // Setting the foreground colour before creating the bar must not affect
    // its special background colour.
    auto info = make_unique<wxInfoBarGeneric>();
    info->SetForegroundColour(*wxRED);
    REQUIRE(info->Create(parent));
    CHECK(info->GetBackgroundColour() == colBg);

    // And it must not be lost when the system colours change either.
    wxSysColourChangedEvent event;
    info->HandleWindowEvent(event);
    CHECK(info->GetBackgroundColour() == colBg);

    // Same thing for the colour set after creating the bar.
    info = make_unique<wxInfoBarGeneric>(parent);
    info->SetForegroundColour(*wxRED);
    info->HandleWindowEvent(event);
    CHECK(info->GetBackgroundColour() == colBg);
}

namespace
{

class TestInfoBar : public wxInfoBarGeneric
{
public:
    explicit TestInfoBar(wxWindow* parent) : wxInfoBarGeneric(parent) { }

    virtual void Dismiss() override
    {
        ++m_dismissCount;

        wxInfoBarGeneric::Dismiss();
    }

    int m_dismissCount = 0;
};

} // anonymous namespace

TEST_CASE("wxInfoBarGeneric::Dismiss", "[wxInfoBar][wxInfoBarGeneric]")
{
    const auto info = make_unique<TestInfoBar>(wxTheApp->GetTopWindow());

    // Clicking any button, including the default close one, must call the
    // virtual Dismiss(), which may be overridden as done here.
    wxWindow* const closeButton = info->FindWindow(wxID_CLOSE);
    REQUIRE(closeButton);

    wxCommandEvent event(wxEVT_BUTTON, wxID_CLOSE);
    event.SetEventObject(closeButton);
    closeButton->HandleWindowEvent(event);
    CHECK(info->m_dismissCount == 1);

    const int buttonId = wxID_HIGHEST + 1000;
    info->AddButton(buttonId, "test");

    wxWindow* const button = info->FindWindow(buttonId);
    REQUIRE(button);

    event.SetId(buttonId);
    event.SetEventObject(button);
    button->HandleWindowEvent(event);
    CHECK(info->m_dismissCount == 2);
}

#if wxUSE_TIMER

// This test is not run by default because it takes a few seconds.
TEST_CASE("wxInfoBar::ShowMessageFor", "[wxInfoBar][.]")
{
    const auto info = make_unique<wxInfoBar>(wxTheApp->GetTopWindow());

    SECTION("Timeout")
    {
        info->ShowMessageFor("Hidden soon", 1);
        CHECK(info->IsShown());

        CHECK(WaitFor("info bar to be hidden",
                      [&info]() { return !info->IsShown(); },
                      2000));
    }

    SECTION("Cancel")
    {
        // Showing another message without timeout must cancel the timeout of
        // the previous one.
        info->ShowMessageFor("Hidden soon", 1);
        info->ShowMessage("Not hidden");

        YieldForAWhile(1500);
        CHECK(info->IsShown());
    }
}

// This test is not run by default because it takes a few seconds.
TEST_CASE("wxInfoBarGeneric::ShowMessageFor", "[wxInfoBar][wxInfoBarGeneric][.]")
{
    const auto info = make_unique<TestInfoBar>(wxTheApp->GetTopWindow());

    // Closing the bar before the timeout expires must cancel it.
    info->ShowMessageFor("Closed before timeout", 1);
    info->Dismiss();
    CHECK(info->m_dismissCount == 1);

    YieldForAWhile(1500);
    CHECK(info->m_dismissCount == 1);

    // And when it does expire, the virtual Dismiss() must be called.
    info->ShowMessageFor("Hidden soon", 1);
    CHECK(WaitFor("Dismiss() to be called",
                  [&info]() { return info->m_dismissCount == 2; },
                  2000));
}

#endif // wxUSE_TIMER

#endif // wxUSE_INFOBAR
