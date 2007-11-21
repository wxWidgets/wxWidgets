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
    CPPUNIT_TEST_SUITE_END();

    void UrlParsing();
    void FileNameToUrlConversion();

    DECLARE_NO_COPY_CLASS(FileSystemTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FileSystemTestCase );

// also include in it's own registry so that these tests can be run alone
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
        { _T("http://www.root.cz/index.html"),
                _T("http"), _T(""), _T("//www.root.cz/index.html"), _T("")},
        // anchors:
        { _T("http://www.root.cz/index.html#lbl"),
                _T("http"), _T(""), _T("//www.root.cz/index.html"), _T("lbl")},
        // file is default protocol:
        { _T("testfile.html"),
                _T("file"), _T(""), _T("testfile.html"), _T("")},
        // stacked protocols:
        { _T("file:myzipfile.zip#zip:index.htm"),
                _T("zip"), _T("file:myzipfile.zip"), _T("index.htm"), _T("")},
        // changes to ':' parsing often break things:
        { _T("file:a#b:foo"),
                _T("b"), _T("file:a"), _T("foo"), _T("")}
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
    wxFileName fn1(_T("\\\\server\\share\\path\\to\\file"));
    wxString url1 = wxFileSystem::FileNameToURL(fn1);
   
    CPPUNIT_ASSERT( fn1.SameAs(wxFileSystem::URLToFileName(url1)) );
#endif
}

#endif // wxUSE_FILESYSTEM
