///////////////////////////////////////////////////////////////////////////////
// Name:        tests/filesys/filesys.cpp
// Purpose:     wxFileSystem unit test
// Author:      Vaclav Slavik
// Created:     2004-03-28
// RCS-ID:      $Id$
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

    bool CanOpen(const wxString& WXUNUSED(url)) { return false; }
    wxFSFile *OpenFile(wxFileSystem& WXUNUSED(fs),
                       const wxString& WXUNUSED(url)) { return NULL; }


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

    DECLARE_NO_COPY_CLASS(FileSystemTestCase)
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
                wxT("b"), wxT("file:a"), wxT("foo"), wxT("")}
    };

    UrlTester tst;
    for ( size_t n = 0; n < WXSIZEOF(data); n++ )
    {
        const Data& d = data[n];
        CPPUNIT_ASSERT( tst.Protocol(d.url) == d.protocol );
        CPPUNIT_ASSERT( tst.LeftLocation(d.url) == d.left );
        CPPUNIT_ASSERT( tst.RightLocation(d.url) == d.right );
        CPPUNIT_ASSERT( tst.Anchor(d.url) == d.anchor );
    }
}

void FileSystemTestCase::FileNameToUrlConversion()
{
#ifdef __WINDOWS__
    wxFileName fn1(wxT("\\\\server\\share\\path\\to\\file"));
    wxString url1 = wxFileSystem::FileNameToURL(fn1);

    CPPUNIT_ASSERT( fn1.SameAs(wxFileSystem::URLToFileName(url1)) );
#endif
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
