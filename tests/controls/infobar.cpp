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

#endif // wxUSE_INFOBAR
