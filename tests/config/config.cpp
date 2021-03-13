///////////////////////////////////////////////////////////////////////////////
// Name:        tests/config/config.cpp
// Purpose:     wxConfig unit test
// Author:      Marcin Wojdyr
// Created:     2007-07-07
// Copyright:   (c) 2007 Marcin Wojdyr
///////////////////////////////////////////////////////////////////////////////

// NOTE: this test is compiled both as part of the non-GUI test and test_gui
//       because it can only use wxColour in the latter.
//
// See also tests/config/fileconf.cpp for wxFileConfig specific tests and
// tests/config/regconf.cpp for wxRegConfig specific tests.

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_CONFIG


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/config.h"
#include "wx/scopedptr.h"

// Tests using wxColour can only be done when using GUI library and they
// require template functions that are not supported by some ancient compilers.
#if wxUSE_GUI && defined(wxHAS_CONFIG_TEMPLATE_RW)
    #define TEST_WXCOLOUR
#endif

#ifdef TEST_WXCOLOUR
    #include "wx/colour.h"
#endif

// --------------------------------------------------------------------------
// the tests
// --------------------------------------------------------------------------

TEST_CASE("wxConfig::ReadWriteLocal", "[config]")
{
    wxString app = "wxConfigTestCase";
    wxString vendor = "wxWidgets";
    wxScopedPtr<wxConfig> config(new wxConfig(app, vendor, "", "",
                                              wxCONFIG_USE_LOCAL_FILE));
    config->DeleteAll();
    config->Write("string1", "abc");
    config->Write("string2", wxString("def"));
    config->Write("int1", 123);
    config->Write(wxString("long1"), 234L);
    config->Write("double1", 345.67);
    config->Write("bool1", true);

    // See comment in regconf.cpp.
    const wxLongLong_t val64 = wxLL(0x8000000000000008);
    config->Write("ll", val64);

    const wxULongLong_t uval64 = wxULL(0x9000000000000009);
    config->Write("ull", uval64);

    config->Write("size", size_t(UINT_MAX));
#ifdef TEST_WXCOLOUR
    config->Write("color1", wxColour(11,22,33,44));
#endif // TEST_WXCOLOUR
    config->Flush();

    config.reset(new wxConfig(app, vendor, "", "",
                              wxCONFIG_USE_LOCAL_FILE));
    wxString string1 = config->Read("string1");
    CHECK( string1 == "abc" );
    string1 = config->Read("string1", "defaultvalue");
    CHECK( string1 == "abc" );

    wxString string2;
    bool r = config->Read("string2", &string2);
    CHECK( r );
    CHECK( string2 == "def" );

    r = config->Read("string2", &string2, "defaultvalue");
    CHECK( r );
    CHECK( string2 == "def" );

    int int1 = config->Read("int1", 5);
    CHECK( int1 == 123 );

    long long1;
    r = config->Read("long1", &long1);
    CHECK( r );
    CHECK( long1 == 234L );

    CHECK( config->ReadLong("long1", 0) == 234 );

    double double1;
    r = config->Read("double1", &double1);
    CHECK( r );
    CHECK( double1 == 345.67 );

    CHECK( config->ReadDouble("double1", 0) == double1 );

    bool bool1;
    r = config->Read("foo", &bool1); // there is no "foo" key
    CHECK( !r );

    r = config->Read("bool1", &bool1);
    CHECK( r );
    CHECK( bool1 == true );

    CHECK( config->ReadBool("bool1", false) == bool1 );

    wxLongLong_t ll;
    CHECK( config->Read("ll", &ll) );
    CHECK( ll == val64 );
    CHECK( config->ReadLongLong("ll", 0) == val64 );

    CHECK( config->Read("ull", &ll) );
    CHECK( ll == static_cast<wxLongLong_t>(uval64) );
    CHECK( config->ReadLongLong("ull", 0) == static_cast<wxLongLong_t>(uval64) );

    size_t size;
    CHECK( config->Read("size", &size) );
    CHECK( size == UINT_MAX );

#ifdef TEST_WXCOLOUR
    wxColour color1;
    r = config->Read("color1", &color1);
    CHECK( r );
    CHECK( color1 == wxColour(11,22,33,44) );

    CHECK( config->ReadObject("color1", wxNullColour) == color1 );
#endif // TEST_WXCOLOUR

    config->DeleteAll();
}

// Helper of RecordingDefaultsTest() test.
static
size_t ReadValues(const wxConfig& config, bool has_values)
{
    size_t read = 0;
    bool r;

    wxString string1 = config.Read("string1", "abc");
    read++;

    wxString string2 = config.Read("string2", wxString("def"));
    read++;

    wxString string3;
    r = config.Read("string3", &string3, "abc");
    CHECK( r == has_values );
    read++;

    wxString string4;
    r = config.Read("string4", &string4, wxString("def"));
    CHECK( r == has_values );
    read++;

    int int1;
    r = config.Read("int1", &int1, 123);
    CHECK( r == has_values );
    read++;

    int int2 = config.Read("int2", 1234);
    CHECK( 1234 == int2 );
    read++;

    long long1;
    r = config.Read(wxString("long1"), &long1, 234L);
    CHECK( r == has_values );
    read++;

    double double1;
    r = config.Read("double1", &double1, 345.67);
    CHECK( r == has_values );
    read++;

    bool bool1;
    r = config.Read("bool1", &bool1, true);
    CHECK( r == has_values );
    read++;

#ifdef TEST_WXCOLOUR
    wxColour color1;
    r = config.Read("color1", &color1, wxColour(11,22,33,44));
    CHECK( r == has_values );
    read++;
#endif // TEST_WXCOLOUR

    return read;
}


TEST_CASE("wxConfig::RecordingDefaults", "[config]")
{
    wxString app = "wxConfigTestCaseRD";
    wxString vendor = "wxWidgets";
    wxScopedPtr<wxConfig> config(new wxConfig(app, vendor, "", "",
                                              wxCONFIG_USE_LOCAL_FILE));
    config->DeleteAll();
    config->SetRecordDefaults(false); // by default it is false
    ReadValues(*config, false);
    CHECK( config->GetNumberOfEntries() == 0 );
    config->SetRecordDefaults(true);
    size_t read = ReadValues(*config, false);
    CHECK( config->GetNumberOfEntries() == read );
    ReadValues(*config, true);
    config->DeleteAll();
}

#endif //wxUSE_CONFIG
