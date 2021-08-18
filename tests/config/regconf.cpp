///////////////////////////////////////////////////////////////////////////////
// Name:        tests/config/regconf.cpp
// Purpose:     wxRegConfig unit test
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-02
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_CONFIG && wxUSE_REGKEY

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/msw/regconf.h"

#include "wx/scopedptr.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE("wxRegConfig::ReadWrite", "[regconfig][config][registry]")
{
    wxString app = "wxRegConfigTestCase";
    wxString vendor = "wxWidgets";

    // NOTE: we use wxCONFIG_USE_LOCAL_FILE explicitly to test wxRegConfig
    //       with something different from the default value wxCONFIG_USE_GLOBAL_FILE
    wxScopedPtr<wxConfigBase> config(new wxRegConfig(app, vendor, "", "",
                                                     wxCONFIG_USE_LOCAL_FILE));

    // test writing
    config->SetPath("/group1");
    CHECK( config->Write("entry1", "foo") );
    config->SetPath("/group2");
    CHECK( config->Write("entry1", "bar") );

    CHECK( config->Write("int32", 1234567) );

    // Note that type of wxLL(0x8000000000000008) literal is somehow unsigned
    // long long with MinGW, not sure if it's a bug or not, but work around it
    // by specifying the type explicitly.
    const wxLongLong_t val64 = wxLL(0x8000000000000008);
    CHECK( config->Write("int64", val64) );

    // test reading
    wxString str;
    long dummy;

    config->SetPath("/");
    CHECK( config->GetFirstGroup(str, dummy) );
    CHECK( str == "group1" );
    CHECK( config->Read("group1/entry1", "INVALID DEFAULT") == "foo" );
    CHECK( config->GetNextGroup(str, dummy) );
    CHECK( str == "group2" );
    CHECK( config->Read("group2/entry1", "INVALID DEFAULT") == "bar" );

    CHECK( config->ReadLong("group2/int32", 0) == 1234567 );
    CHECK( config->ReadLongLong("group2/int64", 0) == val64 );

    config->DeleteAll();
}

TEST_CASE("wxRegKey::DeleteFromRedirectedView", "[registry][64bits]")
{
    if ( !wxIsPlatform64Bit() )
    {
        // Test needs WoW64.
        return;
    }

    // Test inside a key that's known to be redirected and is in HKCU so that
    // admin rights are not required (unlike with HKLM).
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\Classes\\CLSID\\wxWidgetsTestKey",
        sizeof(void *) == 4
            ? wxRegKey::WOW64ViewMode_64
            : wxRegKey::WOW64ViewMode_32);

    REQUIRE( key.Create() );
    CHECK( key.DeleteSelf() );
    CHECK( !key.Exists() );
}

#endif // wxUSE_CONFIG && wxUSE_REGKEY

