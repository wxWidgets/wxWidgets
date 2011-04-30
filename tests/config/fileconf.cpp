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
wxT("[root]\n")
wxT("entry=value\n")
wxT("[root/group1]\n")
wxT("[root/group1/subgroup]\n")
wxT("subentry=subvalue\n")
wxT("subentry2=subvalue2\n")
wxT("[root/group2]\n")
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
        CPPUNIT_TEST( Binary );
        CPPUNIT_TEST( Save );
        CPPUNIT_TEST( DeleteEntry );
        CPPUNIT_TEST( DeleteAndWriteEntry );
        CPPUNIT_TEST( DeleteGroup );
        CPPUNIT_TEST( DeleteAll );
        CPPUNIT_TEST( RenameEntry );
        CPPUNIT_TEST( RenameGroup );
        CPPUNIT_TEST( CreateEntriesAndSubgroup );
        CPPUNIT_TEST( CreateSubgroupAndEntries );
        CPPUNIT_TEST( DeleteLastGroup );
        CPPUNIT_TEST( DeleteAndRecreateGroup );
        CPPUNIT_TEST( AddToExistingRoot );
        CPPUNIT_TEST( ReadNonExistent );
        CPPUNIT_TEST( ReadEmpty );
        CPPUNIT_TEST( ReadFloat );
    CPPUNIT_TEST_SUITE_END();

    void Path();
    void AddEntries();
    void GetEntries();
    void GetGroups();
    void HasEntry();
    void HasGroup();
    void Binary();
    void Save();
    void DeleteEntry();
    void DeleteAndWriteEntry();
    void DeleteGroup();
    void DeleteAll();
    void RenameEntry();
    void RenameGroup();
    void CreateEntriesAndSubgroup();
    void CreateSubgroupAndEntries();
    void DeleteLastGroup();
    void DeleteAndRecreateGroup();
    void AddToExistingRoot();
    void ReadNonExistent();
    void ReadEmpty();
    void ReadFloat();


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

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileConfigTestCase, "FileConfigTestCase" );

void FileConfigTestCase::Path()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( ChangePath(fc, wxT("")) == wxT("") );
    CPPUNIT_ASSERT( ChangePath(fc, wxT("/")) == wxT("") );
    CPPUNIT_ASSERT( ChangePath(fc, wxT("root")) == wxT("/root") );
    CPPUNIT_ASSERT( ChangePath(fc, wxT("/root")) == wxT("/root") );
    CPPUNIT_ASSERT( ChangePath(fc, wxT("/root/group1/subgroup")) == wxT("/root/group1/subgroup") );
    CPPUNIT_ASSERT( ChangePath(fc, wxT("/root/group2")) == wxT("/root/group2") );
}

void FileConfigTestCase::AddEntries()
{
    wxFileConfig fc;

    wxVERIFY_FILECONFIG( wxT(""), fc  );

    fc.Write(wxT("/Foo"), wxT("foo"));
    wxVERIFY_FILECONFIG( wxT("Foo=foo\n"), fc  );

    fc.Write(wxT("/Bar/Baz"), wxT("baz"));
    wxVERIFY_FILECONFIG( wxT("Foo=foo\n[Bar]\nBaz=baz\n"), fc  );

    fc.DeleteAll();
    wxVERIFY_FILECONFIG( wxT(""), fc  );

    fc.Write(wxT("/Bar/Baz"), wxT("baz"));
    wxVERIFY_FILECONFIG( wxT("[Bar]\nBaz=baz\n"), fc  );

    fc.Write(wxT("/Foo"), wxT("foo"));
    wxVERIFY_FILECONFIG( wxT("Foo=foo\n[Bar]\nBaz=baz\n"), fc  );
}

void
FileConfigTestCase::CheckGroupEntries(const wxFileConfig& fc,
                                      const wxChar *path,
                                      size_t nEntries,
                                      ...)
{
    wxConfigPathChanger change(&fc, wxString(path) + wxT("/"));

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
    wxConfigPathChanger change(&fc, wxString(path) + wxT("/"));

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

    CheckGroupEntries(fc, wxT(""), 0);
    CheckGroupEntries(fc, wxT("/root"), 1, wxT("entry"));
    CheckGroupEntries(fc, wxT("/root/group1"), 0);
    CheckGroupEntries(fc, wxT("/root/group1/subgroup"),
                        2, wxT("subentry"), wxT("subentry2"));
}

void FileConfigTestCase::GetGroups()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CheckGroupSubgroups(fc, wxT(""), 1, wxT("root"));
    CheckGroupSubgroups(fc, wxT("/root"), 2, wxT("group1"), wxT("group2"));
    CheckGroupSubgroups(fc, wxT("/root/group1"), 1, wxT("subgroup"));
    CheckGroupSubgroups(fc, wxT("/root/group2"), 0);
}

void FileConfigTestCase::HasEntry()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( !fc.HasEntry(wxT("root")) );
    CPPUNIT_ASSERT( fc.HasEntry(wxT("root/entry")) );
    CPPUNIT_ASSERT( fc.HasEntry(wxT("/root/entry")) );
    CPPUNIT_ASSERT( fc.HasEntry(wxT("root/group1/subgroup/subentry")) );
    CPPUNIT_ASSERT( !fc.HasEntry(wxT("")) );
    CPPUNIT_ASSERT( !fc.HasEntry(wxT("root/group1")) );
    CPPUNIT_ASSERT( !fc.HasEntry(wxT("subgroup/subentry")) );
    CPPUNIT_ASSERT( !fc.HasEntry(wxT("/root/no_such_group/entry")) );
    CPPUNIT_ASSERT( !fc.HasGroup(wxT("/root/no_such_group")) );
}

void FileConfigTestCase::HasGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( fc.HasGroup(wxT("root")) );
    CPPUNIT_ASSERT( fc.HasGroup(wxT("root/group1")) );
    CPPUNIT_ASSERT( fc.HasGroup(wxT("root/group1/subgroup")) );
    CPPUNIT_ASSERT( fc.HasGroup(wxT("root/group2")) );
    CPPUNIT_ASSERT( !fc.HasGroup(wxT("")) );
    CPPUNIT_ASSERT( !fc.HasGroup(wxT("root/group")) );
    CPPUNIT_ASSERT( !fc.HasGroup(wxT("root//subgroup")) );
    CPPUNIT_ASSERT( !fc.HasGroup(wxT("foot/subgroup")) );
    CPPUNIT_ASSERT( !fc.HasGroup(wxT("foot")) );
}

void FileConfigTestCase::Binary()
{
    wxStringInputStream sis(
        "[root]\n"
        "binary=Zm9vCg==\n"
    );
    wxFileConfig fc(sis);

    wxMemoryBuffer buf;
    fc.Read("/root/binary", &buf);

    CPPUNIT_ASSERT( memcmp("foo\n", buf.GetData(), buf.GetDataLen()) == 0 );

    buf.SetDataLen(0);
    buf.AppendData("\0\1\2", 3);
    fc.Write("/root/012", buf);
    wxVERIFY_FILECONFIG(
        "[root]\n"
        "binary=Zm9vCg==\n"
        "012=AAEC\n",
        fc
    );
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

    CPPUNIT_ASSERT( !fc.DeleteEntry(wxT("foo")) );

    CPPUNIT_ASSERT( fc.DeleteEntry(wxT("root/group1/subgroup/subentry")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("entry=value\n")
                         wxT("[root/group1]\n")
                         wxT("[root/group1/subgroup]\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[root/group2]\n"),
                         fc );

    // group should be deleted now as well as it became empty
    wxConfigPathChanger change(&fc, wxT("root/group1/subgroup/subentry2"));
    CPPUNIT_ASSERT( fc.DeleteEntry(wxT("subentry2")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("entry=value\n")
                         wxT("[root/group1]\n")
                         wxT("[root/group2]\n"),
                         fc );
}

void FileConfigTestCase::DeleteAndWriteEntry()
{
    wxStringInputStream sis(
            "[root/group1]\n"
            "subentry=subvalue\n"
            "subentry2=subvalue2\n"
            "subentry3=subvalue3\n"
    );

    wxFileConfig fc(sis);

    fc.DeleteEntry("/root/group1/subentry2");
    fc.Write("/root/group1/subentry2", "testvalue");
    fc.DeleteEntry("/root/group2/subentry2");
    fc.Write("/root/group2/subentry2", "testvalue2");
    fc.DeleteEntry("/root/group1/subentry2");
    fc.Write("/root/group1/subentry2", "testvalue");
    fc.DeleteEntry("/root/group2/subentry2");
    fc.Write("/root/group2/subentry2", "testvalue2");

    wxVERIFY_FILECONFIG( "[root/group1]\n"
                         "subentry=subvalue\n"
                         "subentry3=subvalue3\n"
                         "subentry2=testvalue\n"
                         "[root/group2]\n"
                         "subentry2=testvalue2\n",
                         fc );

    fc.DeleteEntry("/root/group2/subentry2");
    wxVERIFY_FILECONFIG( "[root/group1]\n"
                         "subentry=subvalue\n"
                         "subentry3=subvalue3\n"
                         "subentry2=testvalue\n",
                         fc );

    fc.DeleteEntry("/root/group1/subentry2");
    fc.DeleteEntry("/root/group1/subentry");
    fc.DeleteEntry("/root/group1/subentry3");
    wxVERIFY_FILECONFIG( "", fc );
}

void FileConfigTestCase::DeleteGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( !fc.DeleteGroup(wxT("foo")) );

    CPPUNIT_ASSERT( fc.DeleteGroup(wxT("root/group1")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("entry=value\n")
                         wxT("[root/group2]\n"),
                         fc );

    // notice trailing slash: it should be ignored
    CPPUNIT_ASSERT( fc.DeleteGroup(wxT("root/group2/")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("entry=value\n"),
                         fc );

    CPPUNIT_ASSERT( fc.DeleteGroup(wxT("root")) );
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

    fc.SetPath(wxT("root"));
    CPPUNIT_ASSERT( fc.RenameEntry(wxT("entry"), wxT("newname")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("newname=value\n")
                         wxT("[root/group1]\n")
                         wxT("[root/group1/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[root/group2]\n"),
                         fc );

    fc.SetPath(wxT("group1/subgroup"));
    CPPUNIT_ASSERT( !fc.RenameEntry(wxT("entry"), wxT("newname")) );
    CPPUNIT_ASSERT( !fc.RenameEntry(wxT("subentry"), wxT("subentry2")) );

    CPPUNIT_ASSERT( fc.RenameEntry(wxT("subentry"), wxT("subentry1")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("newname=value\n")
                         wxT("[root/group1]\n")
                         wxT("[root/group1/subgroup]\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("subentry1=subvalue\n")
                         wxT("[root/group2]\n"),
                         fc );
}

void FileConfigTestCase::RenameGroup()
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CPPUNIT_ASSERT( fc.RenameGroup(wxT("root"), wxT("foot")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group1]\n")
                         wxT("[foot/group1/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group2]\n"),
                         fc );

    // renaming a path doesn't work, it must be the immediate group
    CPPUNIT_ASSERT( !fc.RenameGroup(wxT("foot/group1"), wxT("group2")) );


    fc.SetPath(wxT("foot"));

    // renaming to a name of existing group doesn't work
    CPPUNIT_ASSERT( !fc.RenameGroup(wxT("group1"), wxT("group2")) );

    // try exchanging the groups names and then restore them back
    CPPUNIT_ASSERT( fc.RenameGroup(wxT("group1"), wxT("groupTmp")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/groupTmp]\n")
                         wxT("[foot/groupTmp/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group2]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(wxT("group2"), wxT("group1")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/groupTmp]\n")
                         wxT("[foot/groupTmp/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group1]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(wxT("groupTmp"), wxT("group2")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group2]\n")
                         wxT("[foot/group2/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group1]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(wxT("group1"), wxT("groupTmp")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group2]\n")
                         wxT("[foot/group2/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/groupTmp]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(wxT("group2"), wxT("group1")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group1]\n")
                         wxT("[foot/group1/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/groupTmp]\n"),
                         fc );

    CPPUNIT_ASSERT( fc.RenameGroup(wxT("groupTmp"), wxT("group2")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group1]\n")
                         wxT("[foot/group1/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group2]\n"),
                         fc );
}

void FileConfigTestCase::CreateSubgroupAndEntries()
{
    wxFileConfig fc;
    fc.Write(wxT("sub/sub_first"), wxT("sub_one"));
    fc.Write(wxT("first"), wxT("one"));

    wxVERIFY_FILECONFIG( wxT("first=one\n")
                         wxT("[sub]\n")
                         wxT("sub_first=sub_one\n"),
                         fc );
}

void FileConfigTestCase::CreateEntriesAndSubgroup()
{
    wxFileConfig fc;
    fc.Write(wxT("first"), wxT("one"));
    fc.Write(wxT("second"), wxT("two"));
    fc.Write(wxT("sub/sub_first"), wxT("sub_one"));

    wxVERIFY_FILECONFIG( wxT("first=one\n")
                         wxT("second=two\n")
                         wxT("[sub]\n")
                         wxT("sub_first=sub_one\n"),
                         fc );
}

static void EmptyConfigAndWriteKey()
{
    wxFileConfig fc(wxT("deleteconftest"));

    const wxString groupPath = wxT("/root");

    if ( fc.Exists(groupPath) )
    {
        // using DeleteGroup exposes the problem, using DeleteAll doesn't
        CPPUNIT_ASSERT( fc.DeleteGroup(groupPath) );
    }

    // the config must be empty for the problem to arise
    CPPUNIT_ASSERT( !fc.GetNumberOfEntries(true) );
    CPPUNIT_ASSERT( !fc.GetNumberOfGroups(true) );


    // this crashes on second call of this function
    CPPUNIT_ASSERT( fc.Write(groupPath + wxT("/entry"), wxT("value")) );
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
    (void) ::wxRemoveFile(wxFileConfig::GetLocalFileName(wxT("deleteconftest")));
}

void FileConfigTestCase::DeleteAndRecreateGroup()
{
    static const wxChar *confInitial =
        wxT("[First]\n")
        wxT("Value1=Foo\n")
        wxT("[Second]\n")
        wxT("Value2=Bar\n");

    wxStringInputStream sis(confInitial);
    wxFileConfig fc(sis);

    fc.DeleteGroup(wxT("Second"));
    wxVERIFY_FILECONFIG( wxT("[First]\n")
                         wxT("Value1=Foo\n"),
                         fc );

    fc.Write(wxT("Second/Value2"), wxT("New"));
    wxVERIFY_FILECONFIG( wxT("[First]\n")
                         wxT("Value1=Foo\n")
                         wxT("[Second]\n")
                         wxT("Value2=New\n"),
                         fc );
}

void FileConfigTestCase::AddToExistingRoot()
{
    static const wxChar *confInitial =
        wxT("[Group]\n")
        wxT("value1=foo\n");

    wxStringInputStream sis(confInitial);
    wxFileConfig fc(sis);

    fc.Write(wxT("/value1"), wxT("bar"));
    wxVERIFY_FILECONFIG(
        wxT("value1=bar\n")
        wxT("[Group]\n")
        wxT("value1=foo\n"),
        fc
    );
}

void FileConfigTestCase::ReadNonExistent()
{
    static const char *confTest =
        "community=censored\n"
        "[City1]\n"
        "URL=www.fake1.na\n"
        "[City1/A1]\n"
        "[City1/A1/1]\n"
        "IP=192.168.1.66\n"
        "URL=www.fake2.na\n"
    ;

    wxStringInputStream sis(confTest);
    wxFileConfig fc(sis);

    wxString url;
    CPPUNIT_ASSERT( !fc.Read("URL", &url) );
}

void FileConfigTestCase::ReadEmpty()
{
    static const char *confTest = "";

    wxStringInputStream sis(confTest);
    wxFileConfig fc(sis);
}

void FileConfigTestCase::ReadFloat()
{
    static const char *confTest =
        "x=1.234\n"
        "y=-9876.5432\n"
        "z=2e+308\n"
    ;

    wxStringInputStream sis(confTest);
    wxFileConfig fc(sis);

    float f;
    CPPUNIT_ASSERT( fc.Read("x", &f) );
    CPPUNIT_ASSERT_EQUAL( 1.234f, f );

    CPPUNIT_ASSERT( fc.Read("y", &f) );
    CPPUNIT_ASSERT_EQUAL( -9876.5432f, f );
}

#endif // wxUSE_FILECONFIG

