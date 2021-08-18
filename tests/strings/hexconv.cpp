///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/hexconv.cpp
// Purpose:     wxDecToHex, wxHexToDec unit test
// Author:      Artur Wieczorek
// Created:     2017-02-23
// Copyright:   (c) 2017 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/utils.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class HexConvTestCase : public CppUnit::TestCase
{
public:
    HexConvTestCase() {}

private:
    CPPUNIT_TEST_SUITE( HexConvTestCase );
        CPPUNIT_TEST( DecToHex1 ); // Conversion to wxString
        CPPUNIT_TEST( DecToHex2 ); // Conversion to wxChar string
        CPPUNIT_TEST( DecToHex3 ); // Conversion to 2 characters
        CPPUNIT_TEST( HexToDec1 ); // Conversion from char string
        CPPUNIT_TEST( HexToDec2 ); // Conversion from wxString
    CPPUNIT_TEST_SUITE_END();

    void DecToHex1();
    void DecToHex2();
    void DecToHex3();
    void HexToDec1();
    void HexToDec2();

    wxDECLARE_NO_COPY_CLASS(HexConvTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( HexConvTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( HexConvTestCase, "HexConvTestCase" );

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void HexConvTestCase::DecToHex1()
{
    // Conversion to wxString
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStrRef[16];
        sprintf(szHexStrRef, "%02X", i);
        wxString hexStrRef = wxString(szHexStrRef);

        wxString hexStr = wxDecToHex(i);

        CPPUNIT_ASSERT_EQUAL( hexStr, hexStrRef );
    }
}

void HexConvTestCase::DecToHex2()
{
    // Conversion to wxChar string
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStrRef[16];
        sprintf(szHexStrRef, "%02X", i);

        wxChar hexStr[4];
        memset(hexStr, 0xFF, sizeof(hexStr));
        wxChar c3 = hexStr[3]; // This character should remain untouched
        wxDecToHex(i, hexStr);

        CPPUNIT_ASSERT_EQUAL( hexStr[0], (wxChar)szHexStrRef[0] );
        CPPUNIT_ASSERT_EQUAL( hexStr[1], (wxChar)szHexStrRef[1] );
        CPPUNIT_ASSERT_EQUAL( hexStr[2], (wxChar)'\0' );
        CPPUNIT_ASSERT_EQUAL( hexStr[3], c3 );
    }
}

void HexConvTestCase::DecToHex3()
{
    // Conversion to 2 characters
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStrRef[16];
        sprintf(szHexStrRef, "%02X", i);

        char c1 = '\xFF';
        char c2 = '\xFF';
        wxDecToHex(i, &c1, &c2);

        CPPUNIT_ASSERT_EQUAL( c1, szHexStrRef[0] );
        CPPUNIT_ASSERT_EQUAL( c2, szHexStrRef[1] );
    }
}

void HexConvTestCase::HexToDec1()
{
    // Conversion from char string
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStr[16];
        sprintf(szHexStr, "%02X", i);

        int n = wxHexToDec(szHexStr);
        CPPUNIT_ASSERT_EQUAL( n, i );
    }
}

void HexConvTestCase::HexToDec2()
{
    // Conversion from wxString
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStr[16];
        sprintf(szHexStr, "%02X", i);
        wxString hexStr = wxString(szHexStr);

        int n = wxHexToDec(hexStr);
        CPPUNIT_ASSERT_EQUAL( n, i );
    }
}
