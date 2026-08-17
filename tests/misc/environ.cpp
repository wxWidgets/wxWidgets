///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/environ.cpp
// Purpose:     Test wxGet/SetEnv
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-13
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "wx/utils.h"

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("Environ::GetSet", "[env]")
{
    const wxChar *var = wxT("wxTestVar");
    wxString contents;

    CHECK(!wxGetEnv(var, &contents));
    CHECK(contents.empty());

    wxSetEnv(var, wxT("value for wxTestVar"));
    CHECK(wxGetEnv(var, &contents));
    CHECK(contents == wxT("value for wxTestVar"));

    wxSetEnv(var, wxT("another value"));
    CHECK(wxGetEnv(var, &contents));
    CHECK(contents == wxT("another value"));

    wxUnsetEnv(var);
    CHECK(!wxGetEnv(var, &contents));
}

TEST_CASE("Environ::Path", "[env]")
{
    wxString contents;

    REQUIRE(wxGetEnv(wxT("PATH"), &contents));
    CHECK(!contents.empty());
}
