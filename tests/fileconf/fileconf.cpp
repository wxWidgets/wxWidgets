///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fileconf/fileconf.cpp
// Purpose:     wxFileConf unit test
// Author:      Vadim Zeitlin
// Created:     2004-09-19
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FILECONFIG

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/fileconf.h"
#include "wx/sstream.h"

#include "wx/cppunit.h"

static const wxChar *testconfig =
_T("[root]\n")
_T("entry=value\n")
_T("[root/group1]\n")
_T("[root/group1/subgroup]\n")
_T("subentry=subvalue\n")
_T("[root/group2]\n")
;

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FileConfigTestCase : public CppUnit::TestCase
{
public:
    FileConfigTestCase() { }

private:
    CPPUNIT_TEST_SUITE( FileConfigTestCase );
        CPPUNIT_TEST( HasGroup );
    CPPUNIT_TEST_SUITE_END();

    void HasGroup();

    DECLARE_NO_COPY_CLASS(FileConfigTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FileConfigTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileConfigTestCase, "FileConfigTestCase" );

void FileConfigTestCase::HasGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( fc.HasGroup(_T("root")) );
    CPPUNIT_ASSERT( fc.HasGroup(_T("root/group1")) );
    CPPUNIT_ASSERT( fc.HasGroup(_T("root/group1/subgroup")) );
    CPPUNIT_ASSERT( fc.HasGroup(_T("root/group2")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("foot")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("root/group")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("root//subgroup")) );
}

#endif // wxUSE_FILECONFIG

