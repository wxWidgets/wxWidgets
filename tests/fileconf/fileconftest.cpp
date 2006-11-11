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

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FILECONFIG

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/fileconf.h"
#include "wx/sstream.h"
#include "wx/log.h"

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
// local functions
// ----------------------------------------------------------------------------

static wxString Dump(wxFileConfig& fc)
{
    wxStringOutputStream sos;
    fc.Save(sos);
    return wxTextFile::Translate(sos.GetString(), wxTextFileType_Unix);
}

// helper macro to test wxFileConfig contents
#define wxVERIFY_FILECONFIG(t, fc) CPPUNIT_ASSERT_EQUAL(wxString(t), Dump(fc))

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
        CPPUNIT_TEST( AddEntries );
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
        CPPUNIT_TEST( CreateEntriesAndSubgroup );
        CPPUNIT_TEST( CreateSubgroupAndEntries );
        CPPUNIT_TEST( DeleteLastGroup );
    CPPUNIT_TEST_SUITE_END();

    void Path();
    void AddEntries();
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
    void CreateEntriesAndSubgroup();
    void CreateSubgroupAndEntries();
    void DeleteLastGroup();

    static wxString ChangePath(wxFileConfig& fc, const wxChar *path)
    {
        fc.SetPath(path);

        return fc.GetPath();
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

void FileConfigTestCase::AddEntries()
{
    wxFileConfig fc;

    wxVERIFY_FILECONFIG( _T(""), fc  );

    fc.Write(_T("/Foo"), _T("foo"));
    wxVERIFY_FILECONFIG( _T("Foo=foo\n"), fc  );

    fc.Write(_T("/Bar/Baz"), _T("baz"));
    wxVERIFY_FILECONFIG( _T("Foo=foo\n[Bar]\nBaz=baz\n"), fc  );

    fc.DeleteAll();
    wxVERIFY_FILECONFIG( _T(""), fc  );

    fc.Write(_T("/Bar/Baz"), _T("baz"));
    wxVERIFY_FILECONFIG( _T("[Bar]\nBaz=baz\n"), fc  );

    fc.Write(_T("/Foo"), _T("foo"));
    wxVERIFY_FILECONFIG( _T("Foo=foo\n[Bar]\nBaz=baz\n"), fc  );
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
    CPPUNIT_ASSERT( !fc.HasEntry(_T("/root/no_such_group/entry")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("/root/no_such_group")) );
}

void FileConfigTestCase::HasGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( fc.HasGroup(_T("root")) );
    CPPUNIT_ASSERT( fc.HasGroup(_T("root/group1")) );
    CPPUNIT_ASSERT( fc.HasGroup(_T("root/group1/subgroup")) );
    CPPUNIT_ASSERT( fc.HasGroup(_T("root/group2")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("root/group")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("root//subgroup")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("foot/subgroup")) );
    CPPUNIT_ASSERT( !fc.HasGroup(_T("foot")) );
}

void FileConfigTestCase::Save()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);
    wxVERIFY_FILECONFIG( testconfig, fc );
}

void FileConfigTestCase::DeleteEntry()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( !fc.DeleteEntry(_T("foo")) );

    CPPUNIT_ASSERT( fc.DeleteEntry(_T("root/group1/subgroup/subentry")) );
    wxVERIFY_FILECONFIG( _T("[root]\n")
                         _T("entry=value\n")
                         _T("[root/group1]\n")
                         _T("[root/group1/subgroup]\n")
                         _T("subentry2=subvalue2\n")
                         _T("[root/group2]\n"),
                         fc );

    // group should be deleted now as well as it became empty
    wxConfigPathChanger change(&fc, _T("root/group1/subgroup/subentry2"));
    CPPUNIT_ASSERT( fc.DeleteEntry(_T("subentry2")) );
    wxVERIFY_FILECONFIG( _T("[root]\n")
                         _T("entry=value\n")
                         _T("[root/group1]\n")
                         _T("[root/group2]\n"),
                         fc );
}

void FileConfigTestCase::DeleteGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( !fc.DeleteGroup(_T("foo")) );

    CPPUNIT_ASSERT( fc.DeleteGroup(_T("root/group1")) );
    wxVERIFY_FILECONFIG( _T("[root]\n")
                         _T("entry=value\n")
                         _T("[root/group2]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.DeleteGroup(_T("root/group2")) );
    wxVERIFY_FILECONFIG( _T("[root]\n")
                         _T("entry=value\n"),
                         fc );

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
    wxVERIFY_FILECONFIG( _T("[root]\n")
                         _T("newname=value\n")
                         _T("[root/group1]\n")
                         _T("[root/group1/subgroup]\n")
                         _T("subentry=subvalue\n")
                         _T("subentry2=subvalue2\n")
                         _T("[root/group2]\n"),
                         fc );

    fc.SetPath(_T("group1/subgroup"));
    CPPUNIT_ASSERT( !fc.RenameEntry(_T("entry"), _T("newname")) );
    CPPUNIT_ASSERT( !fc.RenameEntry(_T("subentry"), _T("subentry2")) );

    CPPUNIT_ASSERT( fc.RenameEntry(_T("subentry"), _T("subentry1")) );
    wxVERIFY_FILECONFIG( _T("[root]\n")
                         _T("newname=value\n")
                         _T("[root/group1]\n")
                         _T("[root/group1/subgroup]\n")
                         _T("subentry2=subvalue2\n")
                         _T("subentry1=subvalue\n")
                         _T("[root/group2]\n"),
                         fc );
}

void FileConfigTestCase::RenameGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( fc.RenameGroup(_T("root"), _T("foot")) );
    wxVERIFY_FILECONFIG( _T("[foot]\n")
                         _T("entry=value\n")
                         _T("[foot/group1]\n")
                         _T("[foot/group1/subgroup]\n")
                         _T("subentry=subvalue\n")
                         _T("subentry2=subvalue2\n")
                         _T("[foot/group2]\n"),
                         fc );

    // renaming a path doesn't work, it must be the immediate group
    CPPUNIT_ASSERT( !fc.RenameGroup(_T("foot/group1"), _T("group2")) );


    fc.SetPath(_T("foot"));

    // renaming to a name of existing group doesn't work
    CPPUNIT_ASSERT( !fc.RenameGroup(_T("group1"), _T("group2")) );

    // try exchanging the groups names and then restore them back
    CPPUNIT_ASSERT( fc.RenameGroup(_T("group1"), _T("groupTmp")) );
    wxVERIFY_FILECONFIG( _T("[foot]\n")
                         _T("entry=value\n")
                         _T("[foot/groupTmp]\n")
                         _T("[foot/groupTmp/subgroup]\n")
                         _T("subentry=subvalue\n")
                         _T("subentry2=subvalue2\n")
                         _T("[foot/group2]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(_T("group2"), _T("group1")) );
    wxVERIFY_FILECONFIG( _T("[foot]\n")
                         _T("entry=value\n")
                         _T("[foot/groupTmp]\n")
                         _T("[foot/groupTmp/subgroup]\n")
                         _T("subentry=subvalue\n")
                         _T("subentry2=subvalue2\n")
                         _T("[foot/group1]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(_T("groupTmp"), _T("group2")) );
    wxVERIFY_FILECONFIG( _T("[foot]\n")
                         _T("entry=value\n")
                         _T("[foot/group2]\n")
                         _T("[foot/group2/subgroup]\n")
                         _T("subentry=subvalue\n")
                         _T("subentry2=subvalue2\n")
                         _T("[foot/group1]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(_T("group1"), _T("groupTmp")) );
    wxVERIFY_FILECONFIG( _T("[foot]\n")
                         _T("entry=value\n")
                         _T("[foot/group2]\n")
                         _T("[foot/group2/subgroup]\n")
                         _T("subentry=subvalue\n")
                         _T("subentry2=subvalue2\n")
                         _T("[foot/groupTmp]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(_T("group2"), _T("group1")) );
    wxVERIFY_FILECONFIG( _T("[foot]\n")
                         _T("entry=value\n")
                         _T("[foot/group1]\n")
                         _T("[foot/group1/subgroup]\n")
                         _T("subentry=subvalue\n")
                         _T("subentry2=subvalue2\n")
                         _T("[foot/groupTmp]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(_T("groupTmp"), _T("group2")) );
    wxVERIFY_FILECONFIG( _T("[foot]\n")
                         _T("entry=value\n")
                         _T("[foot/group1]\n")
                         _T("[foot/group1/subgroup]\n")
                         _T("subentry=subvalue\n")
                         _T("subentry2=subvalue2\n")
                         _T("[foot/group2]\n"),
                         fc );
}

void FileConfigTestCase::CreateSubgroupAndEntries()
{
    wxFileConfig fc;
    fc.Write(_T("sub/sub_first"), _T("sub_one"));
    fc.Write(_T("first"), _T("one"));

    wxVERIFY_FILECONFIG( _T("first=one\n")
                         _T("[sub]\n")
                         _T("sub_first=sub_one\n"),
                         fc );
}

void FileConfigTestCase::CreateEntriesAndSubgroup()
{
    wxFileConfig fc;
    fc.Write(_T("first"), _T("one"));
    fc.Write(_T("second"), _T("two"));
    fc.Write(_T("sub/sub_first"), _T("sub_one"));

    wxVERIFY_FILECONFIG( _T("first=one\n")
                         _T("second=two\n")
                         _T("[sub]\n")
                         _T("sub_first=sub_one\n"),
                         fc );
}

static void EmptyConfigAndWriteKey()
{
    wxFileConfig fc(_T("deleteconftest"));

    const wxString groupPath = _T("/root");

    if ( fc.Exists(groupPath) )
    {
        // using DeleteGroup exposes the problem, using DeleteAll doesn't
        CPPUNIT_ASSERT( fc.DeleteGroup(groupPath) );
    }

    // the config must be empty for the problem to arise
    CPPUNIT_ASSERT( !fc.GetNumberOfEntries(true) );
    CPPUNIT_ASSERT( !fc.GetNumberOfGroups(true) );


    // this crashes on second call of this function
    CPPUNIT_ASSERT( fc.Write(groupPath + _T("/entry"), _T("value")) );
}

void FileConfigTestCase::DeleteLastGroup()
{
    /*
    We make 2 of the same calls, first to create a file config with a single
    group and key...
    */
    ::EmptyConfigAndWriteKey();

    /*
    ... then the same but this time the key's group is deleted before the
    key is written again. This causes a crash.
    */
    ::EmptyConfigAndWriteKey();


    // clean up
    wxLogNull noLogging;
    (void) ::wxRemoveFile(wxFileConfig::GetLocalFileName(_T("deleteconftest")));
}

#endif // wxUSE_FILECONFIG

