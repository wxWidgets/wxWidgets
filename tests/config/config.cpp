///////////////////////////////////////////////////////////////////////////////
// Name:        tests/config/config.cpp
// Purpose:     wxConfig unit test
// Author:      Marcin Wojdyr
// Created:     2007-07-07
// Copyright:   (c) 2007 Marcin Wojdyr
///////////////////////////////////////////////////////////////////////////////

// NOTE: this test is compiled in test_gui because it uses wxColour for
//       its testing purpose.
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
#include "wx/colour.h"
#include "wx/scopedptr.h"

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
#ifdef wxHAS_CONFIG_TEMPLATE_RW
    config->Write("color1", wxColour(11,22,33,44));
#endif // wxHAS_CONFIG_TEMPLATE_RW
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

#ifdef wxHAS_CONFIG_TEMPLATE_RW
    wxColour color1;
    r = config->Read("color1", &color1);
    CHECK( r );
    CHECK( color1 == wxColour(11,22,33,44) );

    CHECK( config->ReadObject("color1", wxNullColour) == color1 );
#endif // wxHAS_CONFIG_TEMPLATE_RW

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

#ifdef wxHAS_CONFIG_TEMPLATE_RW
    wxColour color1;
    r = config.Read("color1", &color1, wxColour(11,22,33,44));
    CHECK( r == has_values );
    read++;
#endif // wxHAS_CONFIG_TEMPLATE_RW

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
