///////////////////////////////////////////////////////////////////////////////
// Name:        tests/filesys/filesys.cpp
// Purpose:     wxFileSystem unit test
// Author:      Vaclav Slavik
// Created:     2004-03-28
// Copyright:   (c) 2004 Vaclav Slavik
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/filesys.h"

#if wxUSE_FILESYSTEM

#include "wx/fs_mem.h"
#include "wx/scopedptr.h"

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

// a hack to let us use wxFileSystemHandler's protected methods:
class UrlTester : public wxFileSystemHandler
{
public:
    UrlTester() : wxFileSystemHandler() {}

    wxString Protocol(const wxString& p) { return GetProtocol(p); }
    wxString LeftLocation(const wxString& p) { return GetLeftLocation(p); }
    wxString RightLocation(const wxString& p) { return GetRightLocation(p); }
    wxString Anchor(const wxString& p) { return GetAnchor(p); }

    bool CanOpen(const wxString& WXUNUSED(url)) override { return false; }
    wxFSFile *OpenFile(wxFileSystem& WXUNUSED(fs),
                       const wxString& WXUNUSED(url)) override { return nullptr; }


};


// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("wxFileSystem::URLParsing", "[filesys][url][parse]")
{
    static const struct Data
    {
        const wxChar *url;
        const wxChar *protocol, *left, *right, *anchor;
    } data[] =
    {
        // simple case:
        { wxT("http://www.root.cz/index.html"),
                wxT("http"), wxT(""), wxT("//www.root.cz/index.html"), wxT("")},
        // anchors:
        { wxT("http://www.root.cz/index.html#lbl"),
                wxT("http"), wxT(""), wxT("//www.root.cz/index.html"), wxT("lbl")},
        // file is default protocol:
        { wxT("testfile.html"),
                wxT("file"), wxT(""), wxT("testfile.html"), wxT("")},
        // stacked protocols:
        { wxT("file:myzipfile.zip#zip:index.htm"),
                wxT("zip"), wxT("file:myzipfile.zip"), wxT("index.htm"), wxT("")},
        // changes to ':' parsing often break things:
        { wxT("file:a#b:foo"),
                wxT("b"), wxT("file:a"), wxT("foo"), wxT("")},
        { wxT("\\\\host\\C$\\path to\\foo.txt"),
                wxT("file"), wxT(""), wxT("\\\\host\\C$\\path to\\foo.txt"), wxT("")},
        { wxT("C:\\path to\\foo.txt"),
                wxT("file"), wxT(""), wxT("C:\\path to\\foo.txt"), wxT("")},
        { wxT("file:///etc/fsconf"),
                wxT("file"), wxT(""), wxT("/etc/fsconf"), wxT("")},
        { wxT("file://etc/fsconf"),
                wxT("file"), wxT(""), wxT("//etc/fsconf"), wxT("")},
        { wxT("file:/etc/fsconf"),
                wxT("file"), wxT(""), wxT("/etc/fsconf"), wxT("")},
        { wxT("file:///dir/archive.tar.gz#tar:/foo.txt#anchor"),
                wxT("tar"), wxT("file:///dir/archive.tar.gz"),
                wxT("/foo.txt"), wxT("anchor")},
        { wxT("file://dir/archive.tar.gz#tar:/foo.txt#anchor"),
                wxT("tar"), wxT("file://dir/archive.tar.gz"),
                wxT("/foo.txt"), wxT("anchor")},
        { wxT("file:/dir/archive.tar.gz#tar:/foo.txt#anchor"),
                wxT("tar"), wxT("file:/dir/archive.tar.gz"),
                wxT("/foo.txt"), wxT("anchor")},
        { wxT("file:dir/archive.tar.gz#tar:/foo.txt#anchor"),
                wxT("tar"), wxT("file:dir/archive.tar.gz"),
                wxT("/foo.txt"), wxT("anchor")},
        { wxT("file:///dir/archive.tar.gz#tar:/foo.txt"),
                wxT("tar"), wxT("file:///dir/archive.tar.gz"),
                wxT("/foo.txt"), wxT("")},
        { wxT("file://dir/archive.tar.gz#tar:/foo.txt"),
                wxT("tar"), wxT("file://dir/archive.tar.gz"),
                wxT("/foo.txt"), wxT("")},
        { wxT("file:/dir/archive.tar.gz#tar:/foo.txt"),
                wxT("tar"), wxT("file:/dir/archive.tar.gz"),
                wxT("/foo.txt"), wxT("")},
        { wxT("file:dir/archive.tar.gz#tar:/foo.txt"),
                wxT("tar"), wxT("file:dir/archive.tar.gz"),
                wxT("/foo.txt"), wxT("")},
        { wxT("file://host/C:/path%20to/file.txt"),
                wxT("file"), wxT(""), wxT("//host/C:/path%20to/file.txt"), wxT("")},
        { wxT("file:///C:/path%20to/file.txt"),
                wxT("file"), wxT(""), wxT("C:/path%20to/file.txt"), wxT("")},
        { wxT("file:///C"),
                wxT("file"), wxT(""), wxT("/C"), wxT("")},
        { wxT("file://C"),
                wxT("file"), wxT(""), wxT("//C"), wxT("")},
        { wxT("file:/C"),
                wxT("file"), wxT(""), wxT("/C"), wxT("")},
        { wxT("file:C"),
                wxT("file"), wxT(""), wxT("C"), wxT("")}
    };

    UrlTester tst;
    for ( size_t n = 0; n < WXSIZEOF(data); n++ )
    {
        const Data& d = data[n];
        INFO( "Testing URL " << wxString(d.url) );
        CHECK( tst.Protocol(d.url) == d.protocol );
        CHECK( tst.LeftLocation(d.url) == d.left );
        CHECK( tst.RightLocation(d.url) == d.right );
        CHECK( tst.Anchor(d.url) == d.anchor );
    }
}

TEST_CASE("wxFileSystem::FileNameToUrlConversion", "[filesys][url][filename]")
{
    const static struct Data {
        const wxChar *input, *expected;
    } data[] =
    {
#ifdef __WINDOWS__
        { wxT("\\\\host\\C$\\path to\\file.txt"),
                wxT("file://host/C$/path%20to/file.txt")},
        { wxT("C:\\path to\\file.txt"), wxT("file:///C:/path%20to/file.txt")}
#else
        { wxT("/path to/file.txt"), wxT("file:///path%20to/file.txt")}
#endif
    };

    for ( size_t n = 0; n < WXSIZEOF(data); n++ )
    {
        const Data& d = data[n];
        wxFileName fn1(d.input);
        wxString url1 = wxFileSystem::FileNameToURL(fn1);

        INFO( "Testing URL " << wxString(d.input) );
        CHECK( url1 == d.expected );
        CHECK( fn1.SameAs(wxFileName::URLToFileName(url1)) );
    }
}

TEST_CASE("wxFileSystem::UnicodeFileNameToUrlConversion", "[filesys][url][filename][unicode]")
{
    const unsigned char filename_utf8[] = {
              0x4b, 0x72, 0xc3, 0xa1, 0x73, 0x79, 0x50, 0xc5,
              0x99, 0xc3, 0xad, 0x72, 0x6f, 0x64, 0x79, 0x2e,
              0x6a, 0x70, 0x67, 0x00
              // KrásyPřírody.jpg
        };
    wxFileName filename(wxString::FromUTF8((const char*)filename_utf8));

    wxString url = wxFileSystem::FileNameToURL(filename);

    CHECK( filename.SameAs(wxFileName::URLToFileName(url)) );
}

// Test that using FindFirst() after removing a previously found URL works:
// this used to be broken, see https://github.com/wxWidgets/wxWidgets/issues/18744
TEST_CASE("wxFileSystem::MemoryFSHandler", "[filesys][memoryfshandler][find]")
{
    // Install wxMemoryFSHandler just for the duration of this test.
    class AutoMemoryFSHandler
    {
    public:
        AutoMemoryFSHandler()
            : m_handler(new wxMemoryFSHandler())
        {
            wxFileSystem::AddHandler(m_handler.get());
        }

        ~AutoMemoryFSHandler()
        {
            wxFileSystem::RemoveHandler(m_handler.get());
        }

    private:
        wxScopedPtr<wxMemoryFSHandler> const m_handler;
    } autoMemoryFSHandler;

    wxMemoryFSHandler::AddFile("foo.txt", "foo contents");
    wxMemoryFSHandler::AddFile("bar.txt", "bar contents");
    wxFileSystem fs;

    wxString const url = fs.FindFirst("memory:*.txt");
    INFO("Found URL was: " << url);

    wxString filename;
    REQUIRE( url.StartsWith("memory:", &filename) );

    // We don't know which of the two files will be found, this depends on the
    // details of the hash table implementation and varies across builds, so
    // handle both cases and remove the other file, which should certainly
    // invalidate the iterator pointing to it.
    if ( filename == "foo.txt" )
        wxMemoryFSHandler::RemoveFile("bar.txt");
    else if ( filename == "bar.txt" )
        wxMemoryFSHandler::RemoveFile("foo.txt");
    else
        FAIL("Unexpected filename: " << filename);

    CHECK( fs.FindFirst(url) == url );
    CHECK( fs.FindNext() == "" );
}

#endif // wxUSE_FILESYSTEM
