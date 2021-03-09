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
    wxString app = wxT("wxConfigTestCase");
    wxString vendor = wxT("wxWidgets");
    wxScopedPtr<wxConfig> config(new wxConfig(app, vendor, wxT(""), wxT(""),
                                              wxCONFIG_USE_LOCAL_FILE));
    config->DeleteAll();
    config->Write(wxT("string1"), wxT("abc"));
    config->Write(wxT("string2"), wxString(wxT("def")));
    config->Write(wxT("int1"), 123);
    config->Write(wxString(wxT("long1")), 234L);
    config->Write(wxT("double1"), 345.67);
    config->Write(wxT("bool1"), true);
#ifdef wxHAS_CONFIG_TEMPLATE_RW
    config->Write(wxT("color1"), wxColour(11,22,33,44));
#endif // wxHAS_CONFIG_TEMPLATE_RW
    config->Flush();

    config.reset(new wxConfig(app, vendor, wxT(""), wxT(""),
                              wxCONFIG_USE_LOCAL_FILE));
    wxString string1 = config->Read(wxT("string1"));
    CHECK( string1 == "abc" );
    string1 = config->Read(wxT("string1"), wxT("defaultvalue"));
    CHECK( string1 == "abc" );

    wxString string2;
    bool r = config->Read(wxT("string2"), &string2);
    CHECK( r );
    CHECK( string2 == "def" );

    r = config->Read(wxT("string2"), &string2, wxT("defaultvalue"));
    CHECK( r );
    CHECK( string2 == "def" );

    int int1 = config->Read(wxT("int1"), 5);
    CHECK( int1 == 123 );

    long long1;
    r = config->Read(wxT("long1"), &long1);
    CHECK( r );
    CHECK( long1 == 234L );

    CHECK( config->ReadLong(wxT("long1"), 0) == 234 );

    double double1;
    r = config->Read(wxT("double1"), &double1);
    CHECK( r );
    CHECK( double1 == 345.67 );

    CHECK( config->ReadDouble(wxT("double1"), 0) == double1 );

    bool bool1;
    r = config->Read(wxT("foo"), &bool1); // there is no "foo" key
    CHECK( !r );

    r = config->Read(wxT("bool1"), &bool1);
    CHECK( r );
    CHECK( bool1 == true );

    CHECK( config->ReadBool(wxT("bool1"), false) == bool1 );

#ifdef wxHAS_CONFIG_TEMPLATE_RW
    wxColour color1;
    r = config->Read(wxT("color1"), &color1);
    CHECK( r );
    CHECK( color1 == wxColour(11,22,33,44) );

    CHECK( config->ReadObject(wxT("color1"), wxNullColour) == color1 );
#endif // wxHAS_CONFIG_TEMPLATE_RW

    config->DeleteAll();
}

// Helper of RecordingDefaultsTest() test.
static
size_t ReadValues(const wxConfig& config, bool has_values)
{
    size_t read = 0;
    bool r;

    wxString string1 = config.Read(wxT("string1"), wxT("abc"));
    read++;

    wxString string2 = config.Read(wxT("string2"), wxString(wxT("def")));
    read++;

    wxString string3;
    r = config.Read(wxT("string3"), &string3, wxT("abc"));
    CHECK( r == has_values );
    read++;

    wxString string4;
    r = config.Read(wxT("string4"), &string4, wxString(wxT("def")));
    CHECK( r == has_values );
    read++;

    int int1;
    r = config.Read(wxT("int1"), &int1, 123);
    CHECK( r == has_values );
    read++;

    int int2 = config.Read(wxT("int2"), 1234);
    CHECK( 1234 == int2 );
    read++;

    long long1;
    r = config.Read(wxString(wxT("long1")), &long1, 234L);
    CHECK( r == has_values );
    read++;

    double double1;
    r = config.Read(wxT("double1"), &double1, 345.67);
    CHECK( r == has_values );
    read++;

    bool bool1;
    r = config.Read(wxT("bool1"), &bool1, true);
    CHECK( r == has_values );
    read++;

#ifdef wxHAS_CONFIG_TEMPLATE_RW
    wxColour color1;
    r = config.Read(wxT("color1"), &color1, wxColour(11,22,33,44));
    CHECK( r == has_values );
    read++;
#endif // wxHAS_CONFIG_TEMPLATE_RW

    return read;
}


TEST_CASE("wxConfig::RecordingDefaults", "[config]")
{
    wxString app = wxT("wxConfigTestCaseRD");
    wxString vendor = wxT("wxWidgets");
    wxScopedPtr<wxConfig> config(new wxConfig(app, vendor, wxT(""), wxT(""),
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
