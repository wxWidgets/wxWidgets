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

#endif // wxUSE_INFOBAR
