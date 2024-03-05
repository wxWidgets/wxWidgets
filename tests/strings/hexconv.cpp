///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/hexconv.cpp
// Purpose:     wxDecToHex, wxHexToDec unit test
// Author:      Artur Wieczorek
// Created:     2017-02-23
// Copyright:   (c) 2017 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#include "wx/utils.h"

static void DecToHex1();
static void DecToHex2();
static void DecToHex3();
static void HexToDec1();
static void HexToDec2();

TEST_CASE("HexConvTestCase", "[string][hexconv]")
{
    SECTION("DecToHex1") // Conversion to wxString
    {
        DecToHex1();
    }

    SECTION("DecToHex2") // Conversion to wxChar string
    {
        DecToHex2();
    }

    SECTION("DecToHex3") // Conversion to 2 characters
    {
        DecToHex3();
    }

    SECTION("HexToDec1") // Conversion from char string
    {
        HexToDec1();
    }

    SECTION("HexToDec2") // Conversion from wxString
    {
        HexToDec2();
    }
}

// =====  Implementation  =====

static void DecToHex1()
{
    // Conversion to wxString
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStrRef[16];
        sprintf(szHexStrRef, "%02X", i);
        wxString hexStrRef = wxString(szHexStrRef);

        wxString hexStr = wxDecToHex(i);

        CHECK(hexStr == hexStrRef);
    }
}

static void DecToHex2()
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

        CHECK(hexStr[0] == (wxChar)szHexStrRef[0]);
        CHECK(hexStr[1] == (wxChar)szHexStrRef[1]);
        CHECK(hexStr[2] == (wxChar)'\0');
        CHECK(hexStr[3] == c3);
    }
}

static void DecToHex3()
{
    // Conversion to 2 characters
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStrRef[16];
        sprintf(szHexStrRef, "%02X", i);

        char c1 = '\xFF';
        char c2 = '\xFF';
        wxDecToHex(i, &c1, &c2);

        CHECK(c1 == szHexStrRef[0]);
        CHECK(c2 == szHexStrRef[1]);
    }
}

static void HexToDec1()
{
    // Conversion from char string
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStr[16];
        sprintf(szHexStr, "%02X", i);

        int n = wxHexToDec(szHexStr);
        CHECK(n == i);
    }
}

static void HexToDec2()
{
    // Conversion from wxString
    for ( int i = 0; i < 256; i++ )
    {
        char szHexStr[16];
        sprintf(szHexStr, "%02X", i);
        wxString hexStr = wxString(szHexStr);

        int n = wxHexToDec(hexStr);
        CHECK(n == i);
    }
}
