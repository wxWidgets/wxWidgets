///////////////////////////////////////////////////////////////////////////////
// Name:        tests/tartest.cpp
// Purpose:     Test the tar classes
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

#if wxUSE_STREAMS

#include "archivetest.h"
#include "wx/tarstrm.h"

using std::string;


///////////////////////////////////////////////////////////////////////////////
// Tar suite 

class tartest : public ArchiveTestSuite
{
public:
    tartest();
    static CppUnit::Test *suite() { return (new tartest)->makeSuite(); }

protected:
    CppUnit::Test *makeTest(string descr, int options,
                            bool genericInterface,
                            const wxString& archiver,
                            const wxString& unarchiver);
};

tartest::tartest()
  : ArchiveTestSuite("tar")
{
    AddArchiver(_T("tar cf %s *"));
    AddUnArchiver(_T("tar xf %s"));
}

CppUnit::Test *tartest::makeTest(
    string descr,
    int   options,
    bool  genericInterface,
    const wxString& archiver,
    const wxString& unarchiver)
{
    if ((options & Stub) && (options & PipeIn) == 0)
        return NULL;

    if (genericInterface)
        return new ArchiveTestCase<wxArchiveClassFactory>(
                            descr, new wxTarClassFactory,
                            options, archiver, unarchiver);
    else
        return new ArchiveTestCase<wxTarClassFactory>(
                            descr, new wxTarClassFactory,
                            options, archiver, unarchiver);
}

CPPUNIT_TEST_SUITE_REGISTRATION(tartest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(tartest, "archive");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(tartest, "archive/tar");

#endif // wxUSE_STREAMS
