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
#endif

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
        context->bar->WinUISetNextContentWriteHookForTesting(
            &ContinueInfoBarProjectionStorm, context);
    }

    context->bar->AddButton(
        context->firstId + context->calls,
        wxString::Format("nested %d", context->calls));
}

} // namespace

#endif // __WXWINUI__

TEST_CASE("wxInfoBar::Buttons", "[wxInfoBar][winui-v0-supported]")
{
    const std::unique_ptr<wxInfoBar>
        info(new wxInfoBar(wxTheApp->GetTopWindow(), wxID_ANY, wxINFOBAR_CHECKBOX));

    CHECK(info->GetButtonCount() == 0);

    const int buttonId = wxID_HIGHEST + 1000;
    info->AddButton(buttonId, "test");

    CHECK(info->GetButtonCount() == 1);
    CHECK(info->GetButtonId(0) == buttonId);
    CHECK(info->HasButtonId(buttonId));

    info->RemoveButton(buttonId);
    CHECK(info->GetButtonCount() == 0);
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

    // and changing it after creation must work too
    const wxFont italicFont(wxFontInfo(10).Italic());
    CHECK(info->SetFont(italicFont));
    CHECK(info->GetFont().GetStyle() == wxFONTSTYLE_ITALIC);

    CHECK(info->SetForegroundColour(*wxBLUE));
    CHECK(info->GetForegroundColour() == *wxBLUE);

    // resetting the font with wxNullFont goes back to the default one (and
    // must reach the native peer too, not leave the old font behind)
    CHECK(info->SetFont(wxNullFont));
    CHECK(info->GetFont().IsOk());
    CHECK(info->GetFont().GetStyle() != wxFONTSTYLE_ITALIC);
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
    info.WinUISetNextContentWriteHookForTesting(
        &ContinueInfoBarProjectionStorm, &context);

    info.AddButton(outerId, "outer");

    CHECK(context.calls == 1);
    CHECK(info.GetButtonCount() == 2);
    CHECK_FALSE(info.WinUIHasDeferredContentProjectionForTesting());
    CHECK_FALSE(info.WinUIIsContentProjectionQuarantinedForTesting());

    int nestedEvents = 0;
    info.Bind(
        wxEVT_BUTTON,
        [&](wxCommandEvent&) { ++nestedEvents; },
        nestedId);
    REQUIRE(info.WinUIClickButtonForTesting(nestedId));
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
    info.WinUISetNextContentWriteHookForTesting(
        &ContinueInfoBarProjectionStorm, &context);

    // A log target is application code and may pump the loop. The deferred
    // replay must not be consumable until the active projection has ended.
    YieldingInfoBarLogTarget yieldingLog;
    info.AddButton(firstId, "begin");

    CHECK(yieldingLog.DidYield());
    CHECK(context.calls == 8);
    CHECK(info.WinUIHasDeferredContentProjectionForTesting());
    REQUIRE(WaitFor("bounded InfoBar content quarantine", [&]()
    {
        return context.calls == 16 &&
               !info.WinUIHasDeferredContentProjectionForTesting() &&
               info.WinUIIsContentProjectionQuarantinedForTesting();
    }));

    const int callsAtQuarantine = context.calls;
    wxYield();
    wxYield();
    CHECK(context.calls == callsAtQuarantine);

    info.WinUISetNextContentWriteHookForTesting(nullptr, nullptr);
    const wxWindowID recoveryId = firstId + 100;
    info.AddButton(recoveryId, "recovered");
    CHECK_FALSE(info.WinUIHasDeferredContentProjectionForTesting());
    CHECK_FALSE(info.WinUIIsContentProjectionQuarantinedForTesting());

    int recoveryEvents = 0;
    info.Bind(
        wxEVT_BUTTON,
        [&](wxCommandEvent&) { ++recoveryEvents; },
        recoveryId);
    REQUIRE(info.WinUIClickButtonForTesting(recoveryId));
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
        invoking->WinUISetNextContentWriteHookForTesting(
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
        bool hookCalled = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                if ( window != invoking )
                    return;
                hookCalled = true;
                info.reset();
            });

        invoking->ShowMessage("delete while flushing");
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
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
    CHECK(info->WinUIIsPeerOpen());
    info->Dismiss();
    CHECK(closeEvents == 0);
    CHECK_FALSE(info->WinUIIsPeerOpen());
    CHECK_FALSE(info->IsShown());

    // ShowMessage() must really re-open the XAML peer
    info->ShowMessage("re-opened");
    CHECK(info->WinUIIsPeerOpen());
    CHECK(info->IsShown());

    // wait until the InfoBar template is realized so that the close button
    // exists, then click it: unhandled (skipped) event -> default dismiss
    bool clicked = false;
    for ( int i = 0; i < 300 && !(clicked = info->WinUIClickCloseButton()); ++i )
    {
        wxYield();
        wxMilliSleep(10);
    }
    REQUIRE(clicked);
    wxYield();
    CHECK(closeEvents == 1);
    CHECK_FALSE(info->WinUIIsPeerOpen());
    CHECK_FALSE(info->IsShown());

    // handled without dismissing -> the closing is cancelled, both stay open
    info->ShowMessage("kept open");
    CHECK(info->WinUIIsPeerOpen());
    mode = 1;
    closeEvents = 0;
    REQUIRE(info->WinUIClickCloseButton());
    wxYield();
    CHECK(closeEvents == 1);
    CHECK(info->WinUIIsPeerOpen());
    CHECK(info->IsShown());

    // handled with Dismiss() in the handler -> both closed, single event
    mode = 2;
    closeEvents = 0;
    REQUIRE(info->WinUIClickCloseButton());
    wxYield();
    CHECK(closeEvents == 1);
    CHECK_FALSE(info->WinUIIsPeerOpen());
    CHECK_FALSE(info->IsShown());

    // Reentrant close intent is ordered: the last public operation wins.
    info->ShowMessage("reentrant open wins");
    mode = 3;
    closeEvents = 0;
    REQUIRE(info->WinUIClickCloseButton());
    wxYield();
    CHECK(closeEvents == 1);
    CHECK(info->WinUIIsPeerOpen());
    CHECK(info->IsShown());

    mode = 4;
    closeEvents = 0;
    REQUIRE(info->WinUIClickCloseButton());
    wxYield();
    CHECK(closeEvents == 1);
    CHECK_FALSE(info->WinUIIsPeerOpen());
    CHECK_FALSE(info->IsShown());

    // and the cycle still works: the bar can be shown again afterwards
    info->ShowMessage("alive again");
    CHECK(info->WinUIIsPeerOpen());
    CHECK(info->IsShown());

    // Leave neither a closing animation nor a Loaded/layout callback queued
    // for the next test sharing this TLW. The public cycle was proved above;
    // this final close is test isolation, not another behavioural assertion.
    info->Dismiss();
    wxYield();
    CHECK_FALSE(info->WinUIIsPeerOpen());
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
                  !(clicked = raw->WinUIClickCloseButton());
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

        REQUIRE(raw->WinUIClickButtonForTesting(buttonId));
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

#endif // wxUSE_INFOBAR
