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
_T("subentry2=subvalue2\n")
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
        CPPUNIT_TEST( Path );
        CPPUNIT_TEST( GetEntries );
        CPPUNIT_TEST( GetGroups );
        CPPUNIT_TEST( HasEntry );
        CPPUNIT_TEST( HasGroup );
        CPPUNIT_TEST( Save );
        CPPUNIT_TEST( DeleteEntry );
        CPPUNIT_TEST( DeleteGroup );
        CPPUNIT_TEST( DeleteAll );
        CPPUNIT_TEST( RenameEntry );
        CPPUNIT_TEST( RenameGroup );
    CPPUNIT_TEST_SUITE_END();

    void Path();
    void GetEntries();
    void GetGroups();
    void HasEntry();
    void HasGroup();
    void Save();
    void DeleteEntry();
    void DeleteGroup();
    void DeleteAll();
    void RenameEntry();
    void RenameGroup();

    static wxString ChangePath(wxFileConfig& fc, const wxChar *path)
    {
        fc.SetPath(path);

        return fc.GetPath();
    }

    static wxString Dump(wxFileConfig& fc)
    {
        wxStringOutputStream sos;
        fc.Save(sos);
        return wxTextFile::Translate(sos.GetString(), wxTextFileType_Unix);
    }

    void CheckGroupEntries(const wxFileConfig& fc,
                           const wxChar *path,
                           size_t nEntries,
                           ...);
    void CheckGroupSubgroups(const wxFileConfig& fc,
                             const wxChar *path,
                             size_t nGroups,
                             ...);

    DECLARE_NO_COPY_CLASS(FileConfigTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FileConfigTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileConfigTestCase, "FileConfigTestCase" );

void FileConfigTestCase::Path()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( ChangePath(fc, _T("")) == _T("") );
    CPPUNIT_ASSERT( ChangePath(fc, _T("/")) == _T("") );
    CPPUNIT_ASSERT( ChangePath(fc, _T("root")) == _T("/root") );
    CPPUNIT_ASSERT( ChangePath(fc, _T("/root")) == _T("/root") );
    CPPUNIT_ASSERT( ChangePath(fc, _T("/root/group1/subgroup")) == _T("/root/group1/subgroup") );
    CPPUNIT_ASSERT( ChangePath(fc, _T("/root/group2")) == _T("/root/group2") );
}

void
FileConfigTestCase::CheckGroupEntries(const wxFileConfig& fc,
                                      const wxChar *path,
                                      size_t nEntries,
                                      ...)
{
    wxConfigPathChanger change(&fc, wxString(path) + _T("/"));

    CPPUNIT_ASSERT( fc.GetNumberOfEntries() == nEntries );

    va_list ap;
    va_start(ap, nEntries);

    long cookie;
    wxString name;
    for ( bool cont = fc.GetFirstEntry(name, cookie);
          cont;
          cont = fc.GetNextEntry(name, cookie), nEntries-- )
    {
        CPPUNIT_ASSERT( name == va_arg(ap, wxChar *) );
    }

    CPPUNIT_ASSERT( nEntries == 0 );

    va_end(ap);
}

void
FileConfigTestCase::CheckGroupSubgroups(const wxFileConfig& fc,
                                        const wxChar *path,
                                        size_t nGroups,
                                        ...)
{
    wxConfigPathChanger change(&fc, wxString(path) + _T("/"));

    CPPUNIT_ASSERT( fc.GetNumberOfGroups() == nGroups );

    va_list ap;
    va_start(ap, nGroups);

    long cookie;
    wxString name;
    for ( bool cont = fc.GetFirstGroup(name, cookie);
          cont;
          cont = fc.GetNextGroup(name, cookie), nGroups-- )
    {
        CPPUNIT_ASSERT( name == va_arg(ap, wxChar *) );
    }

    CPPUNIT_ASSERT( nGroups == 0 );

    va_end(ap);
}

void FileConfigTestCase::GetEntries()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CheckGroupEntries(fc, _T(""), 0);
    CheckGroupEntries(fc, _T("/root"), 1, _T("entry"));
    CheckGroupEntries(fc, _T("/root/group1"), 0);
    CheckGroupEntries(fc, _T("/root/group1/subgroup"),
                        2, _T("subentry"), _T("subentry2"));
}

void FileConfigTestCase::GetGroups()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CheckGroupSubgroups(fc, _T(""), 1, _T("root"));
    CheckGroupSubgroups(fc, _T("/root"), 2, _T("group1"), _T("group2"));
    CheckGroupSubgroups(fc, _T("/root/group1"), 1, _T("subgroup"));
    CheckGroupSubgroups(fc, _T("/root/group2"), 0);
}

void FileConfigTestCase::HasEntry()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( !fc.HasEntry(_T("root")) );
    CPPUNIT_ASSERT( fc.HasEntry(_T("root/entry")) );
    CPPUNIT_ASSERT( fc.HasEntry(_T("/root/entry")) );
    CPPUNIT_ASSERT( fc.HasEntry(_T("root/group1/subgroup/subentry")) );
    CPPUNIT_ASSERT( !fc.HasEntry(_T("")) );
    CPPUNIT_ASSERT( !fc.HasEntry(_T("root/group1")) );
    CPPUNIT_ASSERT( !fc.HasEntry(_T("subgroup/subentry")) );
}

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

void FileConfigTestCase::Save()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);
    CPPUNIT_ASSERT( Dump(fc) == testconfig );
}

void FileConfigTestCase::DeleteEntry()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( !fc.DeleteEntry(_T("foo")) );

    CPPUNIT_ASSERT( fc.DeleteEntry(_T("root/group1/subgroup/subentry")) );
    CPPUNIT_ASSERT( Dump(fc) == _T("[root]\n")
                                _T("entry=value\n")
                                _T("[root/group1]\n")
                                _T("[root/group1/subgroup]\n")
                                _T("subentry2=subvalue2\n")
                                _T("[root/group2]\n") );

    // group should be deleted now as well as it became empty
    wxConfigPathChanger change(&fc, _T("root/group1/subgroup/subentry2"));
    CPPUNIT_ASSERT( fc.DeleteEntry(_T("subentry2")) );
    CPPUNIT_ASSERT( Dump(fc) == _T("[root]\n")
                                _T("entry=value\n")
                                _T("[root/group1]\n")
                                _T("[root/group2]\n") );
}

void FileConfigTestCase::DeleteGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( !fc.DeleteGroup(_T("foo")) );

    CPPUNIT_ASSERT( fc.DeleteGroup(_T("root/group1")) );
    CPPUNIT_ASSERT( Dump(fc) == _T("[root]\n")
                                _T("entry=value\n")
                                _T("[root/group2]\n") );

    CPPUNIT_ASSERT( fc.DeleteGroup(_T("root/group2")) );
    CPPUNIT_ASSERT( Dump(fc) == _T("[root]\n")
                                _T("entry=value\n") );

    CPPUNIT_ASSERT( fc.DeleteGroup(_T("root")) );
    CPPUNIT_ASSERT( Dump(fc).empty() );
}

void FileConfigTestCase::DeleteAll()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( fc.DeleteAll() );
    CPPUNIT_ASSERT( Dump(fc).empty() );
}

void FileConfigTestCase::RenameEntry()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    fc.SetPath(_T("root"));
    CPPUNIT_ASSERT( fc.RenameEntry(_T("entry"), _T("newname")) );
    CPPUNIT_ASSERT( Dump(fc) == _T("[root]\n")
                                _T("newname=value\n")
                                _T("[root/group1]\n")
                                _T("[root/group1/subgroup]\n")
                                _T("subentry=subvalue\n")
                                _T("subentry2=subvalue2\n")
                                _T("[root/group2]\n") );

    fc.SetPath(_T("group1/subgroup"));
    CPPUNIT_ASSERT( !fc.RenameEntry(_T("entry"), _T("newname")) );
    CPPUNIT_ASSERT( !fc.RenameEntry(_T("subentry"), _T("subentry2")) );

    CPPUNIT_ASSERT( fc.RenameEntry(_T("subentry"), _T("subentry1")) );
    CPPUNIT_ASSERT( Dump(fc) == _T("[root]\n")
                                _T("newname=value\n")
                                _T("[root/group1]\n")
                                _T("[root/group1/subgroup]\n")
                                _T("subentry2=subvalue2\n")
                                _T("subentry1=subvalue\n")
                                _T("[root/group2]\n") );
}

void FileConfigTestCase::RenameGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( fc.RenameGroup(_T("root"), _T("foot")) );
    CPPUNIT_ASSERT( Dump(fc) == _T("[foot]\n")
                                _T("entry=value\n")
                                _T("[foot/group1]\n")
                                _T("[foot/group1/subgroup]\n")
                                _T("subentry=subvalue\n")
                                _T("subentry2=subvalue2\n")
                                _T("[foot/group2]\n") );
}

#endif // wxUSE_FILECONFIG

