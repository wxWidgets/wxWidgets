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
#define wxVERIFY_FILECONFIG(t, fc) CHECK(Dump(fc) == t)

static wxString ChangePath(wxFileConfig& fc, const wxChar *path)
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

    CHECK( ChangePath(fc, wxT("")) == wxT("") );
    CHECK( ChangePath(fc, wxT("/")) == wxT("") );
    CHECK( ChangePath(fc, wxT("root")) == wxT("/root") );
    CHECK( ChangePath(fc, wxT("/root")) == wxT("/root") );
    CHECK( ChangePath(fc, wxT("/root/group1/subgroup")) == wxT("/root/group1/subgroup") );
    CHECK( ChangePath(fc, wxT("/root/group2")) == wxT("/root/group2") );
}

TEST_CASE("wxFileConfig::AddEntries", "[fileconfig][config]")
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

namespace
{

void
CheckGroupEntries(const wxFileConfig& fc,
                  const wxChar *path,
                  size_t nEntries,
                  ...)
{
    wxConfigPathChanger change(&fc, wxString(path) + wxT("/"));

    CHECK( fc.GetNumberOfEntries() == nEntries );

    va_list ap;
    va_start(ap, nEntries);

    long cookie;
    wxString name;
    for ( bool cont = fc.GetFirstEntry(name, cookie);
          cont;
          cont = fc.GetNextEntry(name, cookie), nEntries-- )
    {
        CHECK( name == va_arg(ap, wxChar *) );
    }

    CHECK( nEntries == 0 );

    va_end(ap);
}

void
CheckGroupSubgroups(const wxFileConfig& fc,
                    const wxChar *path,
                    size_t nGroups,
                    ...)
{
    wxConfigPathChanger change(&fc, wxString(path) + wxT("/"));

    CHECK( fc.GetNumberOfGroups() == nGroups );

    va_list ap;
    va_start(ap, nGroups);

    long cookie;
    wxString name;
    for ( bool cont = fc.GetFirstGroup(name, cookie);
          cont;
          cont = fc.GetNextGroup(name, cookie), nGroups-- )
    {
        CHECK( name == va_arg(ap, wxChar *) );
    }

    CHECK( nGroups == 0 );

    va_end(ap);
}

} // anonymous namespace

TEST_CASE("wxFileConfig::GetEntries", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CheckGroupEntries(fc, wxT(""), 0);
    CheckGroupEntries(fc, wxT("/root"), 1, wxT("entry"));
    CheckGroupEntries(fc, wxT("/root/group1"), 0);
    CheckGroupEntries(fc, wxT("/root/group1/subgroup"),
                        2, wxT("subentry"), wxT("subentry2"));
}

TEST_CASE("wxFileConfig::GetGroups", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CheckGroupSubgroups(fc, wxT(""), 1, wxT("root"));
    CheckGroupSubgroups(fc, wxT("/root"), 2, wxT("group1"), wxT("group2"));
    CheckGroupSubgroups(fc, wxT("/root/group1"), 1, wxT("subgroup"));
    CheckGroupSubgroups(fc, wxT("/root/group2"), 0);
}

TEST_CASE("wxFileConfig::HasEntry", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( !fc.HasEntry(wxT("root")) );
    CHECK( fc.HasEntry(wxT("root/entry")) );
    CHECK( fc.HasEntry(wxT("/root/entry")) );
    CHECK( fc.HasEntry(wxT("root/group1/subgroup/subentry")) );
    CHECK( !fc.HasEntry(wxT("")) );
    CHECK( !fc.HasEntry(wxT("root/group1")) );
    CHECK( !fc.HasEntry(wxT("subgroup/subentry")) );
    CHECK( !fc.HasEntry(wxT("/root/no_such_group/entry")) );
    CHECK( !fc.HasGroup(wxT("/root/no_such_group")) );
}

TEST_CASE("wxFileConfig::HasGroup", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( fc.HasGroup(wxT("root")) );
    CHECK( fc.HasGroup(wxT("root/group1")) );
    CHECK( fc.HasGroup(wxT("root/group1/subgroup")) );
    CHECK( fc.HasGroup(wxT("root/group2")) );
    CHECK( !fc.HasGroup(wxT("")) );
    CHECK( !fc.HasGroup(wxT("root/group")) );
    CHECK( !fc.HasGroup(wxT("root//subgroup")) );
    CHECK( !fc.HasGroup(wxT("foot/subgroup")) );
    CHECK( !fc.HasGroup(wxT("foot")) );
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

    CHECK( !fc.DeleteEntry(wxT("foo")) );

    CHECK( fc.DeleteEntry(wxT("root/group1/subgroup/subentry")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("entry=value\n")
                         wxT("[root/group1]\n")
                         wxT("[root/group1/subgroup]\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[root/group2]\n"),
                         fc );

    // group should be deleted now as well as it became empty
    wxConfigPathChanger change(&fc, wxT("root/group1/subgroup/subentry2"));
    CHECK( fc.DeleteEntry(wxT("subentry2")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("entry=value\n")
                         wxT("[root/group1]\n")
                         wxT("[root/group2]\n"),
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

    CHECK( !fc.DeleteGroup(wxT("foo")) );

    CHECK( fc.DeleteGroup(wxT("root/group1")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("entry=value\n")
                         wxT("[root/group2]\n"),
                         fc );

    // notice trailing slash: it should be ignored
    CHECK( fc.DeleteGroup(wxT("root/group2/")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("entry=value\n"),
                         fc );

    CHECK( fc.DeleteGroup(wxT("root")) );
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

    fc.SetPath(wxT("root"));
    CHECK( fc.RenameEntry(wxT("entry"), wxT("newname")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("newname=value\n")
                         wxT("[root/group1]\n")
                         wxT("[root/group1/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[root/group2]\n"),
                         fc );

    fc.SetPath(wxT("group1/subgroup"));
    CHECK( !fc.RenameEntry(wxT("entry"), wxT("newname")) );
    CHECK( !fc.RenameEntry(wxT("subentry"), wxT("subentry2")) );

    CHECK( fc.RenameEntry(wxT("subentry"), wxT("subentry1")) );
    wxVERIFY_FILECONFIG( wxT("[root]\n")
                         wxT("newname=value\n")
                         wxT("[root/group1]\n")
                         wxT("[root/group1/subgroup]\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("subentry1=subvalue\n")
                         wxT("[root/group2]\n"),
                         fc );
}

TEST_CASE("wxFileConfig::RenameGroup", "[fileconfig][config]")
{
    wxStringInputStream sis(testconfig);
    wxFileConfig fc(sis);

    CHECK( fc.RenameGroup(wxT("root"), wxT("foot")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group1]\n")
                         wxT("[foot/group1/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group2]\n"),
                         fc );

    // renaming a path doesn't work, it must be the immediate group
    CHECK( !fc.RenameGroup(wxT("foot/group1"), wxT("group2")) );


    fc.SetPath(wxT("foot"));

    // renaming to a name of existing group doesn't work
    CHECK( !fc.RenameGroup(wxT("group1"), wxT("group2")) );

    // try exchanging the groups names and then restore them back
    CHECK( fc.RenameGroup(wxT("group1"), wxT("groupTmp")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/groupTmp]\n")
                         wxT("[foot/groupTmp/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group2]\n"),
                         fc );

    CHECK( fc.RenameGroup(wxT("group2"), wxT("group1")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/groupTmp]\n")
                         wxT("[foot/groupTmp/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group1]\n"),
                         fc );

    CHECK( fc.RenameGroup(wxT("groupTmp"), wxT("group2")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group2]\n")
                         wxT("[foot/group2/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group1]\n"),
                         fc );

    CHECK( fc.RenameGroup(wxT("group1"), wxT("groupTmp")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group2]\n")
                         wxT("[foot/group2/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/groupTmp]\n"),
                         fc );

    CHECK( fc.RenameGroup(wxT("group2"), wxT("group1")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group1]\n")
                         wxT("[foot/group1/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/groupTmp]\n"),
                         fc );

    CHECK( fc.RenameGroup(wxT("groupTmp"), wxT("group2")) );
    wxVERIFY_FILECONFIG( wxT("[foot]\n")
                         wxT("entry=value\n")
                         wxT("[foot/group1]\n")
                         wxT("[foot/group1/subgroup]\n")
                         wxT("subentry=subvalue\n")
                         wxT("subentry2=subvalue2\n")
                         wxT("[foot/group2]\n"),
                         fc );
}

TEST_CASE("wxFileConfig::CreateSubgroupAndEntries", "[fileconfig][config]")
{
    wxFileConfig fc;
    fc.Write(wxT("sub/sub_first"), wxT("sub_one"));
    fc.Write(wxT("first"), wxT("one"));

    wxVERIFY_FILECONFIG( wxT("first=one\n")
                         wxT("[sub]\n")
                         wxT("sub_first=sub_one\n"),
                         fc );
}

TEST_CASE("wxFileConfig::CreateEntriesAndSubgroup", "[fileconfig][config]")
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
        CHECK( fc.DeleteGroup(groupPath) );
    }

    // the config must be empty for the problem to arise
    CHECK( !fc.GetNumberOfEntries(true) );
    CHECK( !fc.GetNumberOfGroups(true) );


    // this crashes on second call of this function
    CHECK( fc.Write(groupPath + wxT("/entry"), wxT("value")) );
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
    (void) ::wxRemoveFile(wxFileConfig::GetLocalFileName(wxT("deleteconftest")));
}

TEST_CASE("wxFileConfig::DeleteAndRecreateGroup", "[fileconfig][config]")
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

TEST_CASE("wxFileConfig::AddToExistingRoot", "[fileconfig][config]")
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

#endif // wxUSE_FILECONFIG

