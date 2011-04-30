///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/pathlist.cpp
// Purpose:     Test wxPathList
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-02
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#include "wx/filefn.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class PathListTestCase : public CppUnit::TestCase
{
public:
    PathListTestCase() { }

private:
    CPPUNIT_TEST_SUITE( PathListTestCase );
        CPPUNIT_TEST( FindValidPath );
    CPPUNIT_TEST_SUITE_END();

    void FindValidPath();

    DECLARE_NO_COPY_CLASS(PathListTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( PathListTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( PathListTestCase, "PathListTestCase" );

void PathListTestCase::FindValidPath()
{
#ifdef __UNIX__
    #define CMD_IN_PATH wxT("ls")
#else
    #define CMD_IN_PATH wxT("cmd.exe")
#endif

    wxPathList pathlist;
    pathlist.AddEnvList(wxT("PATH"));
    
    wxString path = pathlist.FindValidPath(CMD_IN_PATH);
    CPPUNIT_ASSERT( !path.empty() );
}


