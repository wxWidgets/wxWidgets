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


#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif // WX_PRECOMP

#include "wx/accel.h"

#include <memory>

namespace
{

void CheckAccelEntry(const wxAcceleratorEntry& accel, int keycode, int flags)
{
    CHECK( keycode == accel.GetKeyCode() );
    CHECK( flags == accel.GetFlags() );
}

} // anonymous namespace


/*
 * Test the creation of accelerator keys using the Create function
 */
TEST_CASE( "wxAcceleratorEntry::Create", "[accelentry]" )
{
    std::unique_ptr<wxAcceleratorEntry> pa;

    SECTION( "Correct behavior" )
    {
        pa.reset( wxAcceleratorEntry::Create("Foo\tCtrl+Z") );

        CHECK( pa );
        CHECK( pa->IsOk() );
        CheckAccelEntry(*pa, 'Z', wxACCEL_CTRL);
    }

    SECTION( "Tab missing" )
    {
        pa.reset( wxAcceleratorEntry::Create("Shift-Q") );

        CHECK( !pa );
    }

    SECTION( "No accelerator key specified" )
    {
        pa.reset( wxAcceleratorEntry::Create("bloordyblop") );

        CHECK( !pa );
    }

    SECTION( "Display name parsing" )
    {
        pa.reset( wxAcceleratorEntry::Create("Test\tBackSpace") );

        CHECK( pa );
        CHECK( pa->IsOk() );
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
        CHECK( "Alt+X" == a.ToString() );
    }

    SECTION( "Create from valid string" )
    {
        CHECK( a.FromString("Alt+Shift+F1") );
        CheckAccelEntry(a, WXK_F1, wxACCEL_ALT | wxACCEL_SHIFT);

        // Note that this is just "+" and not WXK_ADD.
        CHECK( a.FromString("Ctrl-+") );
        CheckAccelEntry(a, '+', wxACCEL_CTRL);

        // But this is WXK_NUMPAD_ADD, to distinguish it from the main "+" key.
        CHECK( a.FromString("Ctrl-Num +") );
        CheckAccelEntry(a, WXK_NUMPAD_ADD, wxACCEL_CTRL);
    }

    SECTION( "Create from invalid string" )
    {
        CHECK( !a.FromString("bloordyblop") );
    }
}

TEST_CASE( "wxAcceleratorTable::Create", "[accelentry]" )
{
    CHECK( !wxAcceleratorTable(0, nullptr).IsOk() );

    const wxAcceleratorEntry entries[] =
    {
        wxAcceleratorEntry(wxACCEL_CTRL, 'A'),
        wxAcceleratorEntry(wxACCEL_ALT, 'B'),
        wxAcceleratorEntry(wxACCEL_SHIFT, 'C')
    };

    CHECK( wxAcceleratorTable(WXSIZEOF(entries), entries).IsOk() );
}

TEST_CASE( "wxAcceleratorEntry::MatchesEvent", "[accelentry]" )
{
    wxKeyEvent event(wxEVT_KEY_DOWN);
    event.m_keyCode = 'A';
    event.SetControlDown(true);

    CHECK( wxAcceleratorEntry(wxACCEL_CTRL, 'A').MatchesEvent(event) );

    // The case of the letters doesn't matter.
    CHECK( wxAcceleratorEntry(wxACCEL_CTRL, 'a').MatchesEvent(event) );

    // But the key and all the modifiers must match exactly.
    CHECK( !wxAcceleratorEntry(wxACCEL_CTRL, 'B').MatchesEvent(event) );
    CHECK( !wxAcceleratorEntry(wxACCEL_NORMAL, 'A').MatchesEvent(event) );
    CHECK( !wxAcceleratorEntry(wxACCEL_CTRL | wxACCEL_SHIFT, 'A')
                .MatchesEvent(event) );

    wxKeyEvent eventDel(wxEVT_KEY_DOWN);
    eventDel.m_keyCode = WXK_DELETE;

    CHECK( wxAcceleratorEntry(wxACCEL_NORMAL, WXK_DELETE)
                .MatchesEvent(eventDel) );

    // The keypad keys are different from the normal ones.
    CHECK( !wxAcceleratorEntry(wxACCEL_NORMAL, WXK_NUMPAD_DELETE)
                .MatchesEvent(eventDel) );
}
