///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fileconf/fileconf.cpp
// Purpose:     wxFileConf unit test
// Author:      Vadim Zeitlin
// Created:     2004-09-19
// Copyright:   (c) 2004 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_FILECONFIG

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/fileconf.h"
#include "wx/sstream.h"
#include "wx/log.h"

static const char *testconfig =
"[root]\n"
"entry=value\n"
"[root/group1]\n"
"[root/group1/subgroup]\n"
"subentry=subvalue\n"
"subentry2=subvalue2\n"
"[root/group2]\n"
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
#define wxVERIFY_FILECONFIG(t, fc) CHECK(Dump(fc) == t)

static wxString ChangePath(wxFileConfig& fc, const char *path)
{
    fc.SetPath(path);

    return fc.GetPath();
}

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE("wxFileConfig::Path", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( ChangePath(fc, "") == "" );
    CHECK( ChangePath(fc, "/") == "" );
    CHECK( ChangePath(fc, "root") == "/root" );
    CHECK( ChangePath(fc, "/root") == "/root" );
    CHECK( ChangePath(fc, "/root/group1/subgroup") == "/root/group1/subgroup" );
    CHECK( ChangePath(fc, "/root/group2") == "/root/group2" );
}

TEST_CASE("wxFileConfig::AddEntries", "[fileconfig][config]")
{
    wxFileConfig fc;

    wxVERIFY_FILECONFIG( "", fc  );

    fc.Write("/Foo", "foo");
    wxVERIFY_FILECONFIG( "Foo=foo\n", fc  );

    fc.Write("/Bar/Baz", "baz");
    wxVERIFY_FILECONFIG( "Foo=foo\n[Bar]\nBaz=baz\n", fc  );

    fc.DeleteAll();
    wxVERIFY_FILECONFIG( "", fc  );

    fc.Write("/Bar/Baz", "baz");
    wxVERIFY_FILECONFIG( "[Bar]\nBaz=baz\n", fc  );

    fc.Write("/Foo", "foo");
    wxVERIFY_FILECONFIG( "Foo=foo\n[Bar]\nBaz=baz\n", fc  );
}

namespace
{

void
CheckGroupEntries(const wxFileConfig& fc,
                  const char *path,
                  size_t nEntries,
                  ...)
{
    wxConfigPathChanger change(&fc, wxString(path) + "/");

    CHECK( fc.GetNumberOfEntries() == nEntries );

    va_list ap;
    va_start(ap, nEntries);

    long cookie;
    wxString name;
    for ( bool cont = fc.GetFirstEntry(name, cookie);
          cont;
          cont = fc.GetNextEntry(name, cookie), nEntries-- )
    {
        CHECK( name == va_arg(ap, char *) );
    }

    CHECK( nEntries == 0 );

    va_end(ap);
}

void
CheckGroupSubgroups(const wxFileConfig& fc,
                    const char *path,
                    size_t nGroups,
                    ...)
{
    wxConfigPathChanger change(&fc, wxString(path) + "/");

    CHECK( fc.GetNumberOfGroups() == nGroups );

    va_list ap;
    va_start(ap, nGroups);

    long cookie;
    wxString name;
    for ( bool cont = fc.GetFirstGroup(name, cookie);
          cont;
          cont = fc.GetNextGroup(name, cookie), nGroups-- )
    {
        CHECK( name == va_arg(ap, char *) );
    }

    CHECK( nGroups == 0 );

    va_end(ap);
}

} // anonymous namespace

TEST_CASE("wxFileConfig::GetEntries", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CheckGroupEntries(fc, "", 0);
    CheckGroupEntries(fc, "/root", 1, "entry");
    CheckGroupEntries(fc, "/root/group1", 0);
    CheckGroupEntries(fc, "/root/group1/subgroup",
                        2, "subentry", "subentry2");
}

TEST_CASE("wxFileConfig::GetGroups", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CheckGroupSubgroups(fc, "", 1, "root");
    CheckGroupSubgroups(fc, "/root", 2, "group1", "group2");
    CheckGroupSubgroups(fc, "/root/group1", 1, "subgroup");
    CheckGroupSubgroups(fc, "/root/group2", 0);
}

TEST_CASE("wxFileConfig::HasEntry", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( !fc.HasEntry("root") );
    CHECK( fc.HasEntry("root/entry") );
    CHECK( fc.HasEntry("/root/entry") );
    CHECK( fc.HasEntry("root/group1/subgroup/subentry") );
    CHECK( !fc.HasEntry("") );
    CHECK( !fc.HasEntry("root/group1") );
    CHECK( !fc.HasEntry("subgroup/subentry") );
    CHECK( !fc.HasEntry("/root/no_such_group/entry") );
    CHECK( !fc.HasGroup("/root/no_such_group") );
}

TEST_CASE("wxFileConfig::HasGroup", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( fc.HasGroup("root") );
    CHECK( fc.HasGroup("root/group1") );
    CHECK( fc.HasGroup("root/group1/subgroup") );
    CHECK( fc.HasGroup("root/group2") );
    CHECK( !fc.HasGroup("") );
    CHECK( !fc.HasGroup("root/group") );
    CHECK( !fc.HasGroup("root//subgroup") );
    CHECK( !fc.HasGroup("foot/subgroup") );
    CHECK( !fc.HasGroup("foot") );
}

TEST_CASE("wxFileConfig::Binary", "[fileconfig][config]")
{
    wxStringInputStream sis(
        "[root]\n"
        "binary=Zm9vCg==\n"
    );
    wxFileConfig fc(sis);

    wxMemoryBuffer buf;
    fc.Read("/root/binary", &buf);

    CHECK( memcmp("foo\n", buf.GetData(), buf.GetDataLen()) == 0 );

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

TEST_CASE("wxFileConfig::Save", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);
    wxVERIFY_FILECONFIG( testconfig, fc );
}

TEST_CASE("wxFileConfig::DeleteEntry", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( !fc.DeleteEntry("foo") );

    CHECK( fc.DeleteEntry("root/group1/subgroup/subentry") );
    wxVERIFY_FILECONFIG( "[root]\n"
                         "entry=value\n"
                         "[root/group1]\n"
                         "[root/group1/subgroup]\n"
                         "subentry2=subvalue2\n"
                         "[root/group2]\n",
                         fc );

    // group should be deleted now as well as it became empty
    wxConfigPathChanger change(&fc, "root/group1/subgroup/subentry2");
    CHECK( fc.DeleteEntry("subentry2") );
    wxVERIFY_FILECONFIG( "[root]\n"
                         "entry=value\n"
                         "[root/group1]\n"
                         "[root/group2]\n",
                         fc );
}

TEST_CASE("wxFileConfig::DeleteAndWriteEntry", "[fileconfig][config]")
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

TEST_CASE("wxFileConfig::DeleteLastRootEntry", "[fileconfig][config]")
{
    // This tests for the bug which occurred when the last entry of the root
    // group was deleted: this corrupted internal state and resulted in a crash
    // after trying to write the just deleted entry again.
    wxStringInputStream sis("");
    wxFileConfig fc(sis);

    fc.Write("key", "value");
    wxVERIFY_FILECONFIG( "key=value\n", fc );

    fc.DeleteEntry("key");
    wxVERIFY_FILECONFIG( "", fc );

    fc.Write("key", "value");
    wxVERIFY_FILECONFIG( "key=value\n", fc );
}

TEST_CASE("wxFileConfig::DeleteGroup", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( !fc.DeleteGroup("foo") );

    CHECK( fc.DeleteGroup("root/group1") );
    wxVERIFY_FILECONFIG( "[root]\n"
                         "entry=value\n"
                         "[root/group2]\n",
                         fc );

    // notice trailing slash: it should be ignored
    CHECK( fc.DeleteGroup("root/group2/") );
    wxVERIFY_FILECONFIG( "[root]\n"
                         "entry=value\n",
                         fc );

    CHECK( fc.DeleteGroup("root") );
    CHECK( Dump(fc).empty() );
}

TEST_CASE("wxFileConfig::DeleteAll", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( fc.DeleteAll() );
    CHECK( Dump(fc).empty() );
}

TEST_CASE("wxFileConfig::RenameEntry", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    fc.SetPath("root");
    CHECK( fc.RenameEntry("entry", "newname") );
    wxVERIFY_FILECONFIG( "[root]\n"
                         "newname=value\n"
                         "[root/group1]\n"
                         "[root/group1/subgroup]\n"
                         "subentry=subvalue\n"
                         "subentry2=subvalue2\n"
                         "[root/group2]\n",
                         fc );

    fc.SetPath("group1/subgroup");
    CHECK( !fc.RenameEntry("entry", "newname") );
    CHECK( !fc.RenameEntry("subentry", "subentry2") );

    CHECK( fc.RenameEntry("subentry", "subentry1") );
    wxVERIFY_FILECONFIG( "[root]\n"
                         "newname=value\n"
                         "[root/group1]\n"
                         "[root/group1/subgroup]\n"
                         "subentry2=subvalue2\n"
                         "subentry1=subvalue\n"
                         "[root/group2]\n",
                         fc );
}

TEST_CASE("wxFileConfig::RenameGroup", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( fc.RenameGroup("root", "foot") );
    wxVERIFY_FILECONFIG( "[foot]\n"
                         "entry=value\n"
                         "[foot/group1]\n"
                         "[foot/group1/subgroup]\n"
                         "subentry=subvalue\n"
                         "subentry2=subvalue2\n"
                         "[foot/group2]\n",
                         fc );

    // renaming a path doesn't work, it must be the immediate group
    CHECK( !fc.RenameGroup("foot/group1", "group2") );


    fc.SetPath("foot");

    // renaming to a name of existing group doesn't work
    CHECK( !fc.RenameGroup("group1", "group2") );

    // try exchanging the groups names and then restore them back
    CHECK( fc.RenameGroup("group1", "groupTmp") );
    wxVERIFY_FILECONFIG( "[foot]\n"
                         "entry=value\n"
                         "[foot/groupTmp]\n"
                         "[foot/groupTmp/subgroup]\n"
                         "subentry=subvalue\n"
                         "subentry2=subvalue2\n"
                         "[foot/group2]\n",
                         fc );

    CHECK( fc.RenameGroup("group2", "group1") );
    wxVERIFY_FILECONFIG( "[foot]\n"
                         "entry=value\n"
                         "[foot/groupTmp]\n"
                         "[foot/groupTmp/subgroup]\n"
                         "subentry=subvalue\n"
                         "subentry2=subvalue2\n"
                         "[foot/group1]\n",
                         fc );

    CHECK( fc.RenameGroup("groupTmp", "group2") );
    wxVERIFY_FILECONFIG( "[foot]\n"
                         "entry=value\n"
                         "[foot/group2]\n"
                         "[foot/group2/subgroup]\n"
                         "subentry=subvalue\n"
                         "subentry2=subvalue2\n"
                         "[foot/group1]\n",
                         fc );

    CHECK( fc.RenameGroup("group1", "groupTmp") );
    wxVERIFY_FILECONFIG( "[foot]\n"
                         "entry=value\n"
                         "[foot/group2]\n"
                         "[foot/group2/subgroup]\n"
                         "subentry=subvalue\n"
                         "subentry2=subvalue2\n"
                         "[foot/groupTmp]\n",
                         fc );

    CHECK( fc.RenameGroup("group2", "group1") );
    wxVERIFY_FILECONFIG( "[foot]\n"
                         "entry=value\n"
                         "[foot/group1]\n"
                         "[foot/group1/subgroup]\n"
                         "subentry=subvalue\n"
                         "subentry2=subvalue2\n"
                         "[foot/groupTmp]\n",
                         fc );

    CHECK( fc.RenameGroup("groupTmp", "group2") );
    wxVERIFY_FILECONFIG( "[foot]\n"
                         "entry=value\n"
                         "[foot/group1]\n"
                         "[foot/group1/subgroup]\n"
                         "subentry=subvalue\n"
                         "subentry2=subvalue2\n"
                         "[foot/group2]\n",
                         fc );
}

TEST_CASE("wxFileConfig::CreateSubgroupAndEntries", "[fileconfig][config]")
{
    wxFileConfig fc;
    fc.Write("sub/sub_first", "sub_one");
    fc.Write("first", "one");

    wxVERIFY_FILECONFIG( "first=one\n"
                         "[sub]\n"
                         "sub_first=sub_one\n",
                         fc );
}

TEST_CASE("wxFileConfig::CreateEntriesAndSubgroup", "[fileconfig][config]")
{
    wxFileConfig fc;
    fc.Write("first", "one");
    fc.Write("second", "two");
    fc.Write("sub/sub_first", "sub_one");

    wxVERIFY_FILECONFIG( "first=one\n"
                         "second=two\n"
                         "[sub]\n"
                         "sub_first=sub_one\n",
                         fc );
}

static void EmptyConfigAndWriteKey()
{
    wxFileConfig fc("deleteconftest");

    const wxString groupPath = "/root";

    if ( fc.Exists(groupPath) )
    {
        // using DeleteGroup exposes the problem, using DeleteAll doesn't
        CHECK( fc.DeleteGroup(groupPath) );
    }

    // the config must be empty for the problem to arise
    CHECK( !fc.GetNumberOfEntries(true) );
    CHECK( !fc.GetNumberOfGroups(true) );


    // this crashes on second call of this function
    CHECK( fc.Write(groupPath + "/entry", "value") );
}

TEST_CASE("wxFileConfig::DeleteLastGroup", "[fileconfig][config]")
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
    (void) ::wxRemoveFile(wxFileConfig::GetLocalFileName("deleteconftest"));
}

TEST_CASE("wxFileConfig::DeleteAndRecreateGroup", "[fileconfig][config]")
{
    static const char *confInitial =
        "[First]\n"
        "Value1=Foo\n"
        "[Second]\n"
        "Value2=Bar\n";

    wxStringInputStream sis(confInitial);
    wxFileConfig fc(sis);

    fc.DeleteGroup("Second");
    wxVERIFY_FILECONFIG( "[First]\n"
                         "Value1=Foo\n",
                         fc );

    fc.Write("Second/Value2", "New");
    wxVERIFY_FILECONFIG( "[First]\n"
                         "Value1=Foo\n"
                         "[Second]\n"
                         "Value2=New\n",
                         fc );
}

TEST_CASE("wxFileConfig::AddToExistingRoot", "[fileconfig][config]")
{
    static const char *confInitial =
        "[Group]\n"
        "value1=foo\n";

    wxStringInputStream sis(confInitial);
    wxFileConfig fc(sis);

    fc.Write("/value1", "bar");
    wxVERIFY_FILECONFIG(
        "value1=bar\n"
        "[Group]\n"
        "value1=foo\n",
        fc
    );
}

TEST_CASE("wxFileConfig::ReadNonExistent", "[fileconfig][config]")
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
    CHECK( !fc.Read("URL", &url) );
}

TEST_CASE("wxFileConfig::ReadEmpty", "[fileconfig][config]")
{
    static const char *confTest = "";

    wxStringInputStream sis(confTest);
    wxFileConfig fc(sis);
}

TEST_CASE("wxFileConfig::ReadFloat", "[fileconfig][config]")
{
    static const char *confTest =
        "x=1.234\n"
        "y=-9876.5432\n"
        "z=2e+308\n"
    ;

    wxStringInputStream sis(confTest);
    wxFileConfig fc(sis);

    float f;
    CHECK( fc.Read("x", &f) );
    CHECK( f == 1.234f );

    CHECK( fc.Read("y", &f) );
    CHECK( f == -9876.5432f );
}

TEST_CASE("wxFileConfig::LongLong", "[fileconfig][config][longlong]")
{
    wxFileConfig fc("", "", "", "", 0); // Don't use any files.

    // See comment near val64 definition in regconf.cpp.
    const wxLongLong_t val = wxLL(0x8000000000000008);
    REQUIRE( fc.Write("ll", val) );

    wxLongLong_t ll;
    REQUIRE( fc.Read("ll", &ll) );
    CHECK( ll == val );
}

#endif // wxUSE_FILECONFIG

