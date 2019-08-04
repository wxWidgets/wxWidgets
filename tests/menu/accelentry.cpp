///////////////////////////////////////////////////////////////////////////////
// Name:        tests/menu/accelentry.cpp
// Purpose:     wxAcceleratorEntry unit test
// Author:      Vadim Zeitlin
// Created:     2010-12-03
// Copyright:   (c) 2010 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/accel.h"

namespace
{

void CheckAccelEntry(const wxAcceleratorEntry& accel, int keycode, int flags)
{
    REQUIRE( keycode == accel.GetKeyCode() );
    REQUIRE( flags == accel.GetFlags() );
}

} // anonymous namespace


/*
 * Test the creation of accelerator keys using the Create function
 */
TEST_CASE( "wxAcceleratorEntry::Create", "[accelentry]" )
{
    wxAcceleratorEntry* pa;

    SECTION( "Correct behavior" )
    {
        pa = wxAcceleratorEntry::Create("Foo\tCtrl+Z");

        REQUIRE( pa );
        REQUIRE( pa->IsOk() );
        CheckAccelEntry(*pa, 'Z', wxACCEL_CTRL);
    }

    SECTION( "Tab missing" )
    {
        pa = wxAcceleratorEntry::Create("Shift-Q");

        REQUIRE( !pa );
    }

    SECTION( "No accelerator key specified" )
    {
        pa = wxAcceleratorEntry::Create("bloordyblop");

        REQUIRE( !pa );
    }

    SECTION( "Display name parsing" )
    {
        pa = wxAcceleratorEntry::Create("Test\tBackSpace");

        REQUIRE( pa );
        REQUIRE( pa->IsOk() );
        CheckAccelEntry(*pa, WXK_BACK, wxACCEL_NORMAL);
    }
}


/*
 * Test the creation of accelerator keys from strings and also the
 * creation of strings from an accelerator key
 */
TEST_CASE( "wxAcceleratorEntry::StringTests", "[accelentry]" )
{
    wxAcceleratorEntry a(wxACCEL_ALT, 'X');

    SECTION( "Create string from key" )
    {
        REQUIRE( "Alt+X" == a.ToString() );
    }

    SECTION( "Create from valid string" )
    {
        REQUIRE( a.FromString("Alt+Shift+F1") );
        CheckAccelEntry(a, WXK_F1, wxACCEL_ALT | wxACCEL_SHIFT);
    }

    SECTION( "Create from invalid string" )
    {
            REQUIRE( !a.FromString("bloordyblop") );
    }
}
