///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/environ.cpp
// Purpose:     Test wxGet/SetEnv
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-13
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#include "wx/utils.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class EnvTestCase : public CppUnit::TestCase
{
public:
    EnvTestCase() { }

private:
    CPPUNIT_TEST_SUITE( EnvTestCase );
        CPPUNIT_TEST( GetSet );
        CPPUNIT_TEST( Path );
    CPPUNIT_TEST_SUITE_END();

    void GetSet();
    void Path();

    wxDECLARE_NO_COPY_CLASS(EnvTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( EnvTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( EnvTestCase, "EnvTestCase" );

void EnvTestCase::GetSet()
{
    const wxChar *var = wxT("wxTestVar");
    wxString contents;

    CPPUNIT_ASSERT(!wxGetEnv(var, &contents));
    CPPUNIT_ASSERT(contents.empty());

    wxSetEnv(var, wxT("value for wxTestVar"));
    CPPUNIT_ASSERT(wxGetEnv(var, &contents));
    CPPUNIT_ASSERT(contents == wxT("value for wxTestVar"));

    wxSetEnv(var, wxT("another value"));
    CPPUNIT_ASSERT(wxGetEnv(var, &contents));
    CPPUNIT_ASSERT(contents == wxT("another value"));

    wxUnsetEnv(var);
    CPPUNIT_ASSERT(!wxGetEnv(var, &contents));
}

void EnvTestCase::Path()
{
    wxString contents;

    CPPUNIT_ASSERT(wxGetEnv(wxT("PATH"), &contents));
    CPPUNIT_ASSERT(!contents.empty());
}
