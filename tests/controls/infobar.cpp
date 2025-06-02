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


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/infobar.h"

#include <memory>

namespace
{

TEST_CASE("wxInfoBar", "[wxInfoBar]")
{
    SECTION("Buttons")
    {
        const std::unique_ptr<wxInfoBar>
            info(new wxInfoBar(wxTheApp->GetTopWindow(), wxID_ANY, wxINFOBAR_CHECKBOX));

        ASSERT_EQUAL(1, info->GetButtonCount());
        ASSERT_EQUAL(wxID_HIGHEST + 1000, info->GetButtonId(0));
        ASSERT(info->HasButtonId(wxID_HIGHEST + 1000));
        info->RemoveButton(wxID_HIGHEST + 1000);
        ASSERT_EQUAL(0, info->GetButtonCount());
    }
}
