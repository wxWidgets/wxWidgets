///////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/crt.cpp
// Purpose:     Test for wx C runtime functions wrappers
// Author:      Vaclav Slavik
// Created:     2004-06-03
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vaclav Slavik 
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/textfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class CrtTestCase : public CppUnit::TestCase
{
public:
    CrtTestCase() {}

private:
    CPPUNIT_TEST_SUITE( CrtTestCase );
        CPPUNIT_TEST( SetGetEnv );
    CPPUNIT_TEST_SUITE_END();

    void SetGetEnv();

    DECLARE_NO_COPY_CLASS(CrtTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( CrtTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CrtTestCase, "CrtTestCase" );

void CrtTestCase::SetGetEnv()
{
    wxString val;
    wxSetEnv(_T("TESTVAR"), _T("value"));
    CPPUNIT_ASSERT( wxGetEnv(_T("TESTVAR"), &val) == true );
    CPPUNIT_ASSERT( val == _T("value") );
    wxSetEnv(_T("TESTVAR"), _T("something else"));
    CPPUNIT_ASSERT( wxGetEnv(_T("TESTVAR"), &val) );
    CPPUNIT_ASSERT( val == _T("something else") );

    // this test doesn't work under Unix systems without setenv(): putenv() can
    // only be used to add or modify environment variables but not to unset
    // them
#if !defined(__UNIX__) || defined(HAVE_SETENV)
    CPPUNIT_ASSERT( wxUnsetEnv(_T("TESTVAR")) );
    CPPUNIT_ASSERT( wxGetEnv(_T("TESTVAR"), NULL) == false );
#endif
}
