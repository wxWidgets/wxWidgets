///////////////////////////////////////////////////////////////////////////////
// Name:        tests/uris/uris.cpp
// Purpose:     wxURI unit test
// Author:      Ryan Norton
// Created:     2004-08-14
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Ryan Norton
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/uri.h"

#include "wx/cppunit.h"

// Test wxURL & wxURI compat?
#define TEST_URL 1

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class URITestCase : public CppUnit::TestCase
{
public:
    URITestCase();

private:
    CPPUNIT_TEST_SUITE( URITestCase );
        CPPUNIT_TEST( IPv4 );
        CPPUNIT_TEST( IPv6 );
        CPPUNIT_TEST( Paths );
        CPPUNIT_TEST( NormalResolving );
        CPPUNIT_TEST( ComplexResolving );
        CPPUNIT_TEST( ReallyComplexResolving );
        CPPUNIT_TEST( QueryFragmentResolving );
        CPPUNIT_TEST( BackwardsResolving );
        CPPUNIT_TEST( Assignment );
        CPPUNIT_TEST( Comparison );
#if TEST_URL
        CPPUNIT_TEST( URLCompat );
#endif
    CPPUNIT_TEST_SUITE_END();

    void IPv4();
    void IPv6();
    void Paths();
    void NormalResolving();
    void ComplexResolving();
    void ReallyComplexResolving();
    void QueryFragmentResolving();
    void BackwardsResolving();
    void Assignment();
    void Comparison();

#if TEST_URL
    void URLCompat();
#endif

    DECLARE_NO_COPY_CLASS(URITestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( URITestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( URITestCase, "URITestCase" );

URITestCase::URITestCase()
{
}


#define URI_TEST(uristring, cond) \
    uri = new wxURI(wxT(uristring));\
    CPPUNIT_ASSERT(cond);\
    delete uri;

#define URI_PRINT(uri)\
    wxPrintf(wxT("SCHEME:%s\n"), uri.GetScheme());\
    wxPrintf(wxT("USER:%s\n"), uri.GetUser());\
    wxPrintf(wxT("SERVER:%s\n"), uri.GetServer());\
    wxPrintf(wxT("PORT:%s\n"), uri.GetPort());\
    wxPrintf(wxT("PATH:%s\n"), uri.GetPath());\
    wxPrintf(wxT("QUERY:%s\n"), uri.GetQuery());\
    wxPrintf(wxT("FRAGMENT:%s\n"), uri.GetFragment());

void URITestCase::IPv4()
{
    wxURI* uri;


    URI_TEST("http://user:password@192.168.1.100:5050/path", 
            uri->GetHostType() == wxURI_IPV4ADDRESS);

    URI_TEST("http://user:password@192.255.1.100:5050/path",
            uri->GetHostType() == wxURI_IPV4ADDRESS);

    //bogus ipv4
    URI_TEST("http://user:password@192.256.1.100:5050/path",
            uri->GetHostType() != wxURI_IPV4ADDRESS);
}

void URITestCase::IPv6()
{
    wxURI* uri;

    // IPv6address   =                            6( h16 ":" ) ls32
    //               /                       "::" 5( h16 ":" ) ls32
    //               / [               h16 ] "::" 4( h16 ":" ) ls32
    //               / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
    //               / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
    //               / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
    //               / [ *4( h16 ":" ) h16 ] "::"              ls32
    //               / [ *5( h16 ":" ) h16 ] "::"              h16
    //               / [ *6( h16 ":" ) h16 ] "::"
    // ls32          = ( h16 ":" h16 ) / IPv4address
    
    URI_TEST("http://user:password@[aa:aa:aa:aa:aa:aa:192.168.1.100]:5050/path", 
                uri->GetHostType() == wxURI_IPV6ADDRESS);

    URI_TEST("http://user:password@[aa:aa:aa:aa:aa:aa:aa:aa]:5050/path", 
                uri->GetHostType() == wxURI_IPV6ADDRESS);

    URI_TEST("http://user:password@[aa:aa:aa:aa::192.168.1.100]:5050/path", 
                uri->GetHostType() == wxURI_IPV6ADDRESS);

    URI_TEST("http://user:password@[aa:aa:aa:aa::aa:aa]:5050/path", 
                uri->GetHostType() == wxURI_IPV6ADDRESS);
}

void URITestCase::Paths()
{
    wxURI* uri;

    //path tests
    URI_TEST("http://user:password@192.256.1.100:5050/../path", 
        uri->GetPath() == wxT("/path"));

    URI_TEST("http://user:password@192.256.1.100:5050/path/../", 
        uri->GetPath() == wxT("/"));

    URI_TEST("http://user:password@192.256.1.100:5050/path/.", 
        uri->GetPath() == wxT("/path/"));

    URI_TEST("http://user:password@192.256.1.100:5050/path/./", 
        uri->GetPath() == wxT("/path/"));
 
    URI_TEST("path/john/../../../joe", 
        uri->BuildURI() == wxT("../joe"));
}
#undef URI_TEST

#define URI_TEST_RESOLVE(string, eq, strict) \
        uri = new wxURI(wxT(string));\
        uri->Resolve(masteruri, strict);\
        CPPUNIT_ASSERT(uri->BuildURI() == wxT(eq));\
        delete uri;

#define URI_TEST(string, eq) \
        URI_TEST_RESOLVE(string, eq, true);


//examples taken from RFC 2396.bis

void URITestCase::NormalResolving()
{
    wxURI masteruri(wxT("http://a/b/c/d;p?q"));
    wxURI* uri;
    
    URI_TEST("g:h"  ,"g:h")
   URI_TEST("g"    ,"http://a/b/c/g")
   URI_TEST("./g"  ,"http://a/b/c/g")
   URI_TEST("g/"   ,"http://a/b/c/g/")
   URI_TEST("/g"   ,"http://a/g")
   URI_TEST("//g"  ,"http://g")
   URI_TEST("?y"   ,"http://a/b/c/d;p?y")
    URI_TEST("g?y"  ,"http://a/b/c/g?y")
  URI_TEST("#s"   ,"http://a/b/c/d;p?q#s")
       URI_TEST("g#s"  ,"http://a/b/c/g#s")
   URI_TEST("g?y#s","http://a/b/c/g?y#s")
   URI_TEST(";x"   ,"http://a/b/c/;x")
   URI_TEST("g;x"  ,"http://a/b/c/g;x")
  URI_TEST("g;x?y#s","http://a/b/c/g;x?y#s")

   URI_TEST(""     ,"http://a/b/c/d;p?q")
   URI_TEST("."    ,"http://a/b/c/")
   URI_TEST("./"   ,"http://a/b/c/")
   URI_TEST(".."   ,"http://a/b/")
   URI_TEST("../"  ,"http://a/b/")
   URI_TEST("../g" ,"http://a/b/g")
   URI_TEST("../..","http://a/")
   URI_TEST("../../"        ,  "http://a/")
   URI_TEST("../../g"       ,  "http://a/g")
}

void URITestCase::ComplexResolving()
{
    wxURI masteruri(wxT("http://a/b/c/d;p?q"));
    wxURI* uri;

    //odd path examples
   URI_TEST("/./g"   ,"http://a/g")
   URI_TEST("/../g"  ,"http://a/g")
   URI_TEST("g."     ,"http://a/b/c/g.")
   URI_TEST(".g"     ,"http://a/b/c/.g")
   URI_TEST("g.."    ,"http://a/b/c/g..")
   URI_TEST("..g"    ,"http://a/b/c/..g")
}
   //Should Fail
   //"../../../g"    =  "http://a/g"
   //"../../../../g" =  "http://a/g"

void URITestCase::ReallyComplexResolving()
{
    wxURI masteruri(wxT("http://a/b/c/d;p?q"));
    wxURI* uri;

    //even more odder path examples
   URI_TEST("./../g" ,"http://a/b/g")
   URI_TEST("./g/."  ,"http://a/b/c/g/")
   URI_TEST("g/./h"  ,"http://a/b/c/g/h")
   URI_TEST("g/../h" ,"http://a/b/c/h")
   URI_TEST("g;x=1/./y"     ,  "http://a/b/c/g;x=1/y")
   URI_TEST("g;x=1/../y"    ,  "http://a/b/c/y")
}

void URITestCase::QueryFragmentResolving()
{
    wxURI masteruri(wxT("http://a/b/c/d;p?q"));
    wxURI* uri;

   //query/fragment ambigiousness
   URI_TEST("g?y/./x","http://a/b/c/g?y/./x")
   URI_TEST("g?y/../x"      ,  "http://a/b/c/g?y/../x")
   URI_TEST("g#s/./x","http://a/b/c/g#s/./x")
   URI_TEST("g#s/../x"      ,  "http://a/b/c/g#s/../x")
}

void URITestCase::BackwardsResolving()
{
    wxURI masteruri(wxT("http://a/b/c/d;p?q"));
    wxURI* uri;

   //"NEW" 
   URI_TEST("http:g" ,  "http:g")         //strict
   //bw compat
   URI_TEST_RESOLVE("http:g", "http://a/b/c/g", false); 
}

void URITestCase::Assignment()
{
    wxURI uri1(wxT("http://mysite.com")),
          uri2(wxT("http://mysite2.com"));

    uri2 = uri1;

    CPPUNIT_ASSERT(uri1.BuildURI() == uri2.BuildURI());
}

void URITestCase::Comparison()
{
    CPPUNIT_ASSERT(wxURI(wxT("http://mysite.com")) == wxURI(wxT("http://mysite.com")));
}

#if TEST_URL

#include "wx/url.h"

void URITestCase::URLCompat()
{
    wxURL url(wxT("http://user:password@wxwidgets.org"));

    CPPUNIT_ASSERT(url.GetError() == wxURL_NOERR);
    
    wxInputStream* pInput = url.GetInputStream();

    CPPUNIT_ASSERT( pInput != NULL );

    CPPUNIT_ASSERT( url == wxURL(wxT("http://user:password@wxwidgets.org")) );

    wxURI uri(wxT("http://user:password@wxwidgets.org"));

    CPPUNIT_ASSERT( url == uri );

    wxURL urlcopy(uri);

    CPPUNIT_ASSERT( urlcopy == url );
    CPPUNIT_ASSERT( urlcopy == uri );

    wxURI uricopy(url);

    CPPUNIT_ASSERT( uricopy == url );
    CPPUNIT_ASSERT( uricopy == urlcopy );
    CPPUNIT_ASSERT( uricopy == uri );
#if WXWIN_COMPATIBILITY_2_4
    CPPUNIT_ASSERT( wxURL::ConvertFromURI(wxT("%20%41%20")) == wxT(" A ") );
#endif
}

#endif
