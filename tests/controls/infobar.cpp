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

#if wxUSE_INFOBAR

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/sizer.h"
#endif // WX_PRECOMP

#include "wx/infobar.h"

#include "waitfor.h"

#include <memory>

TEST_CASE("wxInfoBar::Buttons", "[wxInfoBar]")
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

    info->ShowCloseButton(wxInfoBar::ShowButton::Always);
    CHECK(info->GetButtonCount() == 0);
    CHECK(!info->HasButtonId(wxID_CLOSE));

    const int buttonId = wxID_HIGHEST + 1000;
    info->AddButton(buttonId, "test");
    CHECK(info->GetButtonCount() == 1);
    CHECK(info->GetButtonId(0) == buttonId);
    CHECK(!info->HasButtonId(wxID_CLOSE));

    info->ShowCloseButton(wxInfoBar::ShowButton::Never);
    info->RemoveAllButtons();
    CHECK(info->GetButtonCount() == 0);
    CHECK(!info->HasButtonId(wxID_CLOSE));
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
        info->ShowCloseButton(wxInfoBar::ShowButton::Always);
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
        info->ShowCloseButton(wxInfoBar::ShowButton::Never);
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
