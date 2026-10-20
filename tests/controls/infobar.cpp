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
#endif // WX_PRECOMP

#include "wx/infobar.h"

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

#endif // wxUSE_INFOBAR
