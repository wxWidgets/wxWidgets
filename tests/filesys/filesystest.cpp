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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/filesys.h"

#if wxUSE_FILESYSTEM

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

    bool CanOpen(const wxString& WXUNUSED(url)) wxOVERRIDE { return false; }
    wxFSFile *OpenFile(wxFileSystem& WXUNUSED(fs),
                       const wxString& WXUNUSED(url)) wxOVERRIDE { return NULL; }


};


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FileSystemTestCase : public CppUnit::TestCase
{
public:
    FileSystemTestCase() { }

private:
    CPPUNIT_TEST_SUITE( FileSystemTestCase );
        CPPUNIT_TEST( UrlParsing );
        CPPUNIT_TEST( FileNameToUrlConversion );
        CPPUNIT_TEST( UnicodeFileNameToUrlConversion );
    CPPUNIT_TEST_SUITE_END();

    void UrlParsing();
    void FileNameToUrlConversion();
    void UnicodeFileNameToUrlConversion();

    wxDECLARE_NO_COPY_CLASS(FileSystemTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FileSystemTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileSystemTestCase, "FileSystemTestCase" );

void FileSystemTestCase::UrlParsing()
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
        CPPUNIT_ASSERT_EQUAL( d.protocol, tst.Protocol(d.url) );
        CPPUNIT_ASSERT_EQUAL( d.left, tst.LeftLocation(d.url) );
        CPPUNIT_ASSERT_EQUAL( d.right, tst.RightLocation(d.url) );
        CPPUNIT_ASSERT_EQUAL( d.anchor, tst.Anchor(d.url) );
    }
}

void FileSystemTestCase::FileNameToUrlConversion()
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

        CPPUNIT_ASSERT_EQUAL( d.expected, url1 );
        CPPUNIT_ASSERT( fn1.SameAs(wxFileSystem::URLToFileName(url1)) );
    }
}

void FileSystemTestCase::UnicodeFileNameToUrlConversion()
{
    const unsigned char filename_utf8[] = {
              0x4b, 0x72, 0xc3, 0xa1, 0x73, 0x79, 0x50, 0xc5,
              0x99, 0xc3, 0xad, 0x72, 0x6f, 0x64, 0x79, 0x2e,
              0x6a, 0x70, 0x67, 0x00
              // KrásyPřírody.jpg
        };
    wxFileName filename(wxString::FromUTF8((const char*)filename_utf8));

    wxString url = wxFileSystem::FileNameToURL(filename);

    CPPUNIT_ASSERT( filename.SameAs(wxFileSystem::URLToFileName(url)) );
}

#endif // wxUSE_FILESYSTEM
