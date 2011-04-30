///////////////////////////////////////////////////////////////////////////////
// Name:        tests/menu/accelentry.cpp
// Purpose:     wxAcceleratorEntry unit test
// Author:      Vadim Zeitlin
// Created:     2010-12-03
// RCS-ID:      $Id$
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
#include "wx/scopedptr.h"

class AccelEntryTestCase : public CppUnit::TestCase
{
public:
    AccelEntryTestCase() {}

private:
    CPPUNIT_TEST_SUITE( AccelEntryTestCase );
        CPPUNIT_TEST( Create );
        CPPUNIT_TEST( ToFromString );
    CPPUNIT_TEST_SUITE_END();

    void Create();
    void ToFromString();

    wxDECLARE_NO_COPY_CLASS(AccelEntryTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( AccelEntryTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( AccelEntryTestCase, "AccelEntryTestCase" );

namespace
{

void CheckAccelEntry(const wxAcceleratorEntry& accel, int keycode, int flags)
{
    CPPUNIT_ASSERT_EQUAL( keycode, accel.GetKeyCode() );
    CPPUNIT_ASSERT_EQUAL( flags, accel.GetFlags() );
}

} // anonymous namespace

void AccelEntryTestCase::Create()
{
    wxScopedPtr<wxAcceleratorEntry>
        pa(wxAcceleratorEntry::Create("Foo\tCtrl+Z"));
    CPPUNIT_ASSERT( pa );
    CPPUNIT_ASSERT( pa->IsOk() );

    CheckAccelEntry(*pa, 'Z', wxACCEL_CTRL);


    // There must be a TAB in the string passed to Create()
    pa.reset(wxAcceleratorEntry::Create("Shift-Q"));
    CPPUNIT_ASSERT( !pa );

    pa.reset(wxAcceleratorEntry::Create("Bar\tShift-Q"));
    CPPUNIT_ASSERT( pa );
    CPPUNIT_ASSERT( pa->IsOk() );
    CheckAccelEntry(*pa, 'Q', wxACCEL_SHIFT);


    pa.reset(wxAcceleratorEntry::Create("bloordyblop"));
    CPPUNIT_ASSERT( !pa );
}

void AccelEntryTestCase::ToFromString()
{
    wxAcceleratorEntry a(wxACCEL_ALT, 'X');
    CPPUNIT_ASSERT_EQUAL( "Alt+X", a.ToString() );

    CPPUNIT_ASSERT( a.FromString("Alt+Shift+F1") );
    CheckAccelEntry(a, WXK_F1, wxACCEL_ALT | wxACCEL_SHIFT);

    CPPUNIT_ASSERT( !a.FromString("bloordyblop") );
}
