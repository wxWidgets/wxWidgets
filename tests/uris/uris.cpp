///////////////////////////////////////////////////////////////////////////////
// Name:        tests/uris/uris.cpp
// Purpose:     wxURI unit test
// Author:      Ryan Norton
// Created:     2004-08-14
// Copyright:   (c) 2004 Ryan Norton
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/uri.h"
#include "wx/url.h"

// Test wxURL & wxURI compat?
#define TEST_URL wxUSE_URL

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
        CPPUNIT_TEST( Host );
        CPPUNIT_TEST( Paths );
        CPPUNIT_TEST( NormalResolving );
        CPPUNIT_TEST( ComplexResolving );
        CPPUNIT_TEST( ReallyComplexResolving );
        CPPUNIT_TEST( QueryFragmentResolving );
        CPPUNIT_TEST( BackwardsResolving );
        CPPUNIT_TEST( Assignment );
        CPPUNIT_TEST( Comparison );
        CPPUNIT_TEST( Unescaping );
        CPPUNIT_TEST( FileScheme );
        CPPUNIT_TEST( Normalizing );
#if TEST_URL
        CPPUNIT_TEST( URLCompat );
#if 0 && wxUSE_PROTOCOL_HTTP
        CPPUNIT_TEST( URLProxy  );
#endif
#endif
    CPPUNIT_TEST_SUITE_END();

    void IPv4();
    void IPv6();
    void Host();
    void Paths();
    void NormalResolving();
    void ComplexResolving();
    void ReallyComplexResolving();
    void QueryFragmentResolving();
    void BackwardsResolving();
    void Assignment();
    void Comparison();
    void Unescaping();
    void FileScheme();
    void Normalizing();

#if TEST_URL
    void URLCompat();
#if 0 && wxUSE_PROTOCOL_HTTP
    void URLProxy();
#endif
#endif

    wxDECLARE_NO_COPY_CLASS(URITestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( URITestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( URITestCase, "URITestCase" );

URITestCase::URITestCase()
{
}

// apply the given accessor to the URI, check that the result is as expected
#define URI_ASSERT_PART_EQUAL(uri, expected, accessor) \
    CHECK(wxURI(uri).accessor == expected)
#define URI_ASSERT_HOST_TEST(uri, expectedhost, expectedtype) \
    CPPUNIT_ASSERT(wxURI(uri).GetServer() == (expectedhost)); \
    CPPUNIT_ASSERT(wxURI(uri).GetHostType() == (expectedtype))
#define URI_ASSERT_HOST_TESTBAD(uri, ne) CPPUNIT_ASSERT(wxURI(uri).GetHostType() != (ne))
#define URI_ASSERT_HOST_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL((uri), (expected), GetServer())
#define URI_ASSERT_PATH_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL((uri), (expected), GetPath())
#define URI_ASSERT_HOSTTYPE_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL((uri), (expected), GetHostType())
#define URI_ASSERT_USER_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL((uri), (expected), GetUser())
#define URI_ASSERT_BADPATH(uri) CPPUNIT_ASSERT(!wxURI(uri).HasPath())
// IPv4
#define URI_ASSERT_IPV4_TEST(ip, expected) \
    URI_ASSERT_HOST_TEST("http://user:password@" ip ":5050/path", expected, wxURI_IPV4ADDRESS)
#define URI_ASSERT_IPV4_TESTBAD(ip) \
    URI_ASSERT_HOST_TESTBAD("http://user:password@" ip ":5050/path", wxURI_IPV4ADDRESS)
// IPv6
#define URI_ASSERT_IPV6_TEST(ip, expected) \
    URI_ASSERT_HOST_TEST("http://user:password@" ip ":5050/path", expected, wxURI_IPV6ADDRESS)
#define URI_ASSERT_IPV6_TESTBAD(ip) \
    URI_ASSERT_HOST_TESTBAD("http://user:password@" ip ":5050/path", wxURI_IPV6ADDRESS)
// Resolve
#define URI_TEST_RESOLVE_IMPL(string, eq, strictness) \
    {\
        wxURI uri(string); \
        uri.Resolve(masteruri, strictness); \
        CPPUNIT_ASSERT_EQUAL(eq, uri.BuildURI()); \
    }
#define URI_TEST_RESOLVE(string, eq) \
        URI_TEST_RESOLVE_IMPL(string, eq, wxURI_STRICT);
#define URI_TEST_RESOLVE_LAX(string, eq) \
        URI_TEST_RESOLVE_IMPL(string, eq, 0);

// Normalization
#define URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL(uri, expected) \
    { wxURI nuri(uri); nuri.Resolve(wxURI("http://a/"));\
      CPPUNIT_ASSERT_EQUAL(expected, nuri.GetPath()); }
#define URI_ASSERT_NORMALIZEDPATH_EQUAL(uri, expected) \
    { URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL(uri, expected); }

void URITestCase::IPv4()
{
    URI_ASSERT_IPV4_TEST("192.168.1.100", "192.168.1.100");
    URI_ASSERT_IPV4_TEST("192.255.1.100", "192.255.1.100");
    URI_ASSERT_IPV4_TEST("192.0.2.16", "192.0.2.16");
    URI_ASSERT_IPV4_TEST("255.0.0.0", "255.0.0.0");
    URI_ASSERT_IPV4_TEST("0.0.0.0", "0.0.0.0");
    URI_ASSERT_IPV4_TEST("1.0.0.0", "1.0.0.0");
    URI_ASSERT_IPV4_TEST("2.0.0.0", "2.0.0.0");
    URI_ASSERT_IPV4_TEST("3.0.0.0", "3.0.0.0");
    URI_ASSERT_IPV4_TEST("30.0.0.0", "30.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("192.256.1.100");
    URI_ASSERT_IPV4_TESTBAD("01.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("001.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("00.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("000.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("256.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("300.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("1111.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("-1.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("0.0.0");
    URI_ASSERT_IPV4_TESTBAD("0.0.0.");
    URI_ASSERT_IPV4_TESTBAD("0.0.0.0.");
    URI_ASSERT_IPV4_TESTBAD("0.0.0.0.0");
    URI_ASSERT_IPV4_TESTBAD("0.0..0");
    URI_ASSERT_IPV4_TESTBAD(".0.0.0");
    URI_ASSERT_HOSTTYPE_EQUAL("http://user:password@192.168.1.100:5050/path",
                            wxURI_IPV4ADDRESS);
    URI_ASSERT_HOSTTYPE_EQUAL("http://user:password@192.255.1.100:5050/path",
                            wxURI_IPV4ADDRESS);
    // bogus ipv4
    CPPUNIT_ASSERT( wxURI("http://user:password@192.256.1.100:5050/path").
                    GetHostType() != wxURI_IPV4ADDRESS);
}

void URITestCase::IPv6()
{
    URI_ASSERT_HOSTTYPE_EQUAL
    (
        "http://user:password@[aa:aa:aa:aa:aa:aa:192.168.1.100]:5050/path",
        wxURI_IPV6ADDRESS
    );
    URI_ASSERT_HOSTTYPE_EQUAL
    (
        "http://user:password@[aa:aa:aa:aa:aa:aa:aa:aa]:5050/path",
        wxURI_IPV6ADDRESS
    );
    URI_ASSERT_HOSTTYPE_EQUAL
    (
        "http://user:password@[aa:aa:aa:aa::192.168.1.100]:5050/path",
        wxURI_IPV6ADDRESS
    );
    URI_ASSERT_HOSTTYPE_EQUAL
    (
        "http://user:password@[aa:aa:aa:aa::aa:aa]:5050/path",
        wxURI_IPV6ADDRESS
    );
    URI_ASSERT_IPV6_TEST("[aa:aa:aa:aa:aa:aa:192.168.1.100]", "aa:aa:aa:aa:aa:aa:192.168.1.100");
    URI_ASSERT_IPV6_TEST("[aa:aa:aa:aa:aa:aa:aa:aa]", "aa:aa:aa:aa:aa:aa:aa:aa");
    URI_ASSERT_IPV6_TEST("[aa:aa:aa:aa::192.168.1.100]", "aa:aa:aa:aa::192.168.1.100");
    URI_ASSERT_IPV6_TEST("[aa:aa:aa:aa::aa:aa]", "aa:aa:aa:aa::aa:aa");
    URI_ASSERT_IPV6_TEST("[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]", "FEDC:BA98:7654:3210:FEDC:BA98:7654:3210");
    URI_ASSERT_IPV6_TEST("[1080:0:0:0:8:800:200C:417A]", "1080:0:0:0:8:800:200C:417A");
    URI_ASSERT_IPV6_TEST("[3ffe:2a00:100:7031::1]", "3ffe:2a00:100:7031::1");
    URI_ASSERT_IPV6_TEST("[1080::8:800:200C:417A]", "1080::8:800:200C:417A");
    URI_ASSERT_IPV6_TEST("[::192.9.5.5]", "::192.9.5.5");
    URI_ASSERT_IPV6_TEST("[::FFFF:129.144.52.38]", "::FFFF:129.144.52.38");
    URI_ASSERT_IPV6_TEST("[2010:836B:4179::836B:4179]", "2010:836B:4179::836B:4179");
    URI_ASSERT_IPV6_TEST("[abcd::]", "abcd::");
    URI_ASSERT_IPV6_TEST("[abcd::1]", "abcd::1");
    URI_ASSERT_IPV6_TEST("[abcd::12]", "abcd::12");
    URI_ASSERT_IPV6_TEST("[abcd::123]", "abcd::123");
    URI_ASSERT_IPV6_TEST("[abcd::1234]", "abcd::1234");
    URI_ASSERT_IPV6_TEST("[2001:0db8:0100:f101:0210:a4ff:fee3:9566]", "2001:0db8:0100:f101:0210:a4ff:fee3:9566");
    URI_ASSERT_IPV6_TEST("[2001:0DB8:0100:F101:0210:A4FF:FEE3:9566]", "2001:0DB8:0100:F101:0210:A4FF:FEE3:9566");
    URI_ASSERT_IPV6_TEST("[2001:db8:100:f101:210:a4ff:fee3:9566]", "2001:db8:100:f101:210:a4ff:fee3:9566");
    URI_ASSERT_IPV6_TEST("[2001:0db8:100:f101:0:0:0:1]","2001:0db8:100:f101:0:0:0:1");
    URI_ASSERT_IPV6_TEST("[1:2:3:4:5:6:255.255.255.255]","1:2:3:4:5:6:255.255.255.255");
    URI_ASSERT_IPV6_TEST("[::1.2.3.4]", "::1.2.3.4");
    URI_ASSERT_IPV6_TEST("[3:4::5:1.2.3.4]", "3:4::5:1.2.3.4");
    URI_ASSERT_IPV6_TEST("[::ffff:1.2.3.4]", "::ffff:1.2.3.4");
    URI_ASSERT_IPV6_TEST("[::0.0.0.0]", "::0.0.0.0");
    URI_ASSERT_IPV6_TEST("[::255.255.255.255]", "::255.255.255.255");
    URI_ASSERT_IPV6_TEST("[::1:2:3:4:5:6:7]", "::1:2:3:4:5:6:7");
    URI_ASSERT_IPV6_TEST("[1::1:2:3:4:5:6]", "1::1:2:3:4:5:6");
    URI_ASSERT_IPV6_TEST("[1:2::1:2:3:4:5]", "1:2::1:2:3:4:5");
    URI_ASSERT_IPV6_TEST("[1:2:3::1:2:3:4]", "1:2:3::1:2:3:4");
    URI_ASSERT_IPV6_TEST("[1:2:3:4::1:2:3]", "1:2:3:4::1:2:3");
    URI_ASSERT_IPV6_TEST("[1:2:3:4:5::1:2]", "1:2:3:4:5::1:2");
    URI_ASSERT_IPV6_TEST("[1:2:3:4:5:6::1]", "1:2:3:4:5:6::1");
    URI_ASSERT_IPV6_TEST("[1:2:3:4:5:6:7::]", "1:2:3:4:5:6:7::");
    URI_ASSERT_IPV6_TEST("[1:1:1::1:1:1:1]", "1:1:1::1:1:1:1");
    URI_ASSERT_IPV6_TEST("[1:1:1::1:1:1]", "1:1:1::1:1:1");
    URI_ASSERT_IPV6_TEST("[1:1:1::1:1]", "1:1:1::1:1");
    URI_ASSERT_IPV6_TEST("[1:1::1:1]", "1:1::1:1");
    URI_ASSERT_IPV6_TEST("[1:1::1]", "1:1::1");
    URI_ASSERT_IPV6_TEST("[1::1]", "1::1");
    URI_ASSERT_IPV6_TEST("[::1]", "::1");
    URI_ASSERT_IPV6_TEST("[::]", "::");
    URI_ASSERT_IPV6_TEST("[21ff:abcd::1]", "21ff:abcd::1");
    URI_ASSERT_IPV6_TEST("[2001:db8:100:f101::1]", "2001:db8:100:f101::1");
    URI_ASSERT_IPV6_TEST("[a:b:c::12:1]", "a:b:c::12:1");
    URI_ASSERT_IPV6_TEST("[a:b::0:1:2:3]", "a:b::0:1:2:3");
    URI_ASSERT_IPV6_TESTBAD("[::12345]");
    URI_ASSERT_IPV6_TESTBAD("[abcd::abcd::abcd]");
    URI_ASSERT_IPV6_TESTBAD("[:::1234]");
    URI_ASSERT_IPV6_TESTBAD("[1234:::1234:1234]");
    URI_ASSERT_IPV6_TESTBAD("[1234:1234:::1234]");
    URI_ASSERT_IPV6_TESTBAD("[1234:::]");
    URI_ASSERT_IPV6_TESTBAD("[1.2.3.4]");
    URI_ASSERT_IPV6_TESTBAD("[0001.0002.0003.0004]");
    URI_ASSERT_IPV6_TESTBAD("[0000:0000:0000:0000:0000:1.2.3.4]");
    URI_ASSERT_IPV6_TESTBAD("[0:0:0:0:0:0:0]");
    URI_ASSERT_IPV6_TESTBAD("[0:0:0:0:0:0:0:]");
    URI_ASSERT_IPV6_TESTBAD("[0:0:0:0:0:0:0:1.2.3.4]");
    URI_ASSERT_IPV6_TESTBAD("[0:0:0:0:0:0:0:0:0]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:001.02.03.004]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:1.2.3.1111]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:1.2.3.256]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:311.2.3.4]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:1.2.3:4]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:1.2.3]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:1.2.3.]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:1.2.3a.4]");
    URI_ASSERT_IPV6_TESTBAD("[::ffff:1.2.3.4:123]");
    URI_ASSERT_IPV6_TESTBAD("[g:0:0:0:0:0:0]");
}

void URITestCase::Host()
{
    URI_ASSERT_HOST_EQUAL("", "");
    URI_ASSERT_HOST_EQUAL("http://foo/", "foo");
    URI_ASSERT_HOST_EQUAL("http://foo-bar/", "foo-bar");
    URI_ASSERT_HOST_EQUAL("http://foo/bar/", "foo");
    URI_ASSERT_HOST_EQUAL("http://192.168.1.0/", "192.168.1.0");
    URI_ASSERT_HOST_EQUAL("http://192.168.1.17/", "192.168.1.17");
    URI_ASSERT_HOST_EQUAL("http://192.168.1.255/", "192.168.1.255");
    URI_ASSERT_HOST_EQUAL("http://192.168.1.1/index.html", "192.168.1.1");
    URI_ASSERT_HOST_EQUAL("http://[aa:aa:aa:aa::aa:aa]/foo", "aa:aa:aa:aa::aa:aa");
    URI_ASSERT_USER_EQUAL("http://user:pass@host/path/", "user");
    URI_ASSERT_USER_EQUAL("http://user@host/path/", "user");
    URI_ASSERT_USER_EQUAL("http://host/path/", "");
}

void URITestCase::Paths()
{
    try
    {
        wxURI test("http://user:password@192.256.1.100:5050/../path");
        test.BuildURI(); // This isn't a unit test, just a niche parsing crash test
    }
    catch (...)
    {
        CPPUNIT_ASSERT(false);
    }
    URI_ASSERT_PATH_EQUAL("http://user:password@192.256.1.100:5050/../path", "/../path");
    URI_ASSERT_PATH_EQUAL("http://user:password@192.256.1.100:5050/path/../", "/path/../");
    URI_ASSERT_PATH_EQUAL("http://user:password@192.256.1.100:5050/path/.", "/path/.");
    URI_ASSERT_PATH_EQUAL("http://user:password@192.256.1.100:5050/path/./", "/path/./");
    URI_ASSERT_PART_EQUAL("path/john/../../../joe", "path/john/../../../joe", BuildURI());
    //When authority is present, the path must either be empty or begin with a slash ("/") character.
    URI_ASSERT_BADPATH("http://good.com:8042BADPATH");
    URI_ASSERT_PATH_EQUAL("http://good.com:8042/GOODPATH", "/GOODPATH");
    //When authority is not present, the path cannot begin with two slash characters ("//").
    URI_ASSERT_BADPATH("http:////BADPATH");
}

//examples taken from RFC 2396.bis
void URITestCase::NormalResolving()
{
    wxURI masteruri("http://a/b/c/d;p?q");
    URI_TEST_RESOLVE("g:h"	,"g:h")
    URI_TEST_RESOLVE("g"	,"http://a/b/c/g")
    URI_TEST_RESOLVE("./g"	,"http://a/b/c/g")
    URI_TEST_RESOLVE("g/"	,"http://a/b/c/g/")
    URI_TEST_RESOLVE("/g"	,"http://a/g")
    URI_TEST_RESOLVE("//g"	,"http://g")
    URI_TEST_RESOLVE("?y"	,"http://a/b/c/d;p?y")
    URI_TEST_RESOLVE("g?y"	,"http://a/b/c/g?y")
    URI_TEST_RESOLVE("#s"	,"http://a/b/c/d;p?q#s")
    URI_TEST_RESOLVE("g#s"	,"http://a/b/c/g#s")
    URI_TEST_RESOLVE("g?y#s","http://a/b/c/g?y#s")
    URI_TEST_RESOLVE(";x"	,"http://a/b/c/;x")
    URI_TEST_RESOLVE("g;x"	,"http://a/b/c/g;x")
    URI_TEST_RESOLVE("g;x?y#s","http://a/b/c/g;x?y#s")
    URI_TEST_RESOLVE(""		,"http://a/b/c/d;p?q")
    URI_TEST_RESOLVE("."	,"http://a/b/c/")
    URI_TEST_RESOLVE("./"	,"http://a/b/c/")
    URI_TEST_RESOLVE(".."	,"http://a/b/")
    URI_TEST_RESOLVE("../"	,"http://a/b/")
    URI_TEST_RESOLVE("../g" ,"http://a/b/g")
    URI_TEST_RESOLVE("../..","http://a/")
    URI_TEST_RESOLVE("../../"		 ,	"http://a/")
    URI_TEST_RESOLVE("../../g"		 ,	"http://a/g")
}

void URITestCase::ComplexResolving()
{
    wxURI masteruri("http://a/b/c/d;p?q");
    URI_TEST_RESOLVE("../../../g"	, "http://a/g")
    URI_TEST_RESOLVE("../../../../g", "http://a/g")
    URI_TEST_RESOLVE("/./g"	  ,"http://a/g")
    URI_TEST_RESOLVE("/../g"  ,"http://a/g")
    URI_TEST_RESOLVE("g."	  ,"http://a/b/c/g.")
    URI_TEST_RESOLVE(".g"	  ,"http://a/b/c/.g")
    URI_TEST_RESOLVE("g.."	  ,"http://a/b/c/g..")
    URI_TEST_RESOLVE("..g"	  ,"http://a/b/c/..g")
    // github issue #3350
    masteruri = "file:doc.chm#xchm:/d/e";
    URI_TEST_RESOLVE("/a/b/c.jpg"	  ,"file://doc.chm/a/b/c.jpg")
}

void URITestCase::ReallyComplexResolving()
{
    wxURI masteruri("http://a/b/c/d;p?q");
    URI_TEST_RESOLVE("./../g"		,"http://a/b/g")
    URI_TEST_RESOLVE("./g/."		,"http://a/b/c/g/")
    URI_TEST_RESOLVE("g/./h"		,"http://a/b/c/g/h")
    URI_TEST_RESOLVE("g/../h"		,"http://a/b/c/h")
    URI_TEST_RESOLVE("g;x=1/./y"	,"http://a/b/c/g;x=1/y")
    URI_TEST_RESOLVE("g;x=1/../y"	,"http://a/b/c/y")
}

void URITestCase::QueryFragmentResolving()
{
    wxURI masteruri("http://a/b/c/d;p?q"); //query/fragment ambigiousness
    URI_TEST_RESOLVE("g?y/./x",		"http://a/b/c/g?y/./x")
    URI_TEST_RESOLVE("g?y/../x",	"http://a/b/c/g?y/../x")
    URI_TEST_RESOLVE("g#s/./x",		"http://a/b/c/g#s/./x")
    URI_TEST_RESOLVE("g#s/../x",	"http://a/b/c/g#s/../x")
}

void URITestCase::BackwardsResolving()
{
    wxURI masteruri("http://a/b/c/d;p?q");

    //"NEW"
    URI_TEST_RESOLVE("http:g" ,  "http:g")         //strict
    //bw compat
    URI_TEST_RESOLVE_LAX("http:g", "http://a/b/c/g");
}

void URITestCase::Assignment()
{
    wxURI uri1("http://mysite.com"),
          uri2("http://mysite2.com");

    uri2 = uri1;

    CPPUNIT_ASSERT_EQUAL(uri1.BuildURI(), uri2.BuildURI());
}

void URITestCase::Comparison()
{
    CPPUNIT_ASSERT(wxURI("http://mysite.com") == wxURI("http://mysite.com"));
}

void URITestCase::Unescaping()
{
    wxString escaped,
             unescaped;

    escaped = "http://test.com/of/file%3A%2F%2FC%3A%5Curi%5C"
              "escaping%5Cthat%5Cseems%5Cbroken%5Csadly%5B1%5D.rss";

    unescaped = wxURI(escaped).BuildUnescapedURI();

    CPPUNIT_ASSERT_EQUAL( "http://test.com/of/file://C:\\uri\\"
                          "escaping\\that\\seems\\broken\\sadly[1].rss",
                          unescaped );

    CPPUNIT_ASSERT_EQUAL( unescaped, wxURI::Unescape(escaped) );


    escaped = "http://ru.wikipedia.org/wiki/"
              "%D0%A6%D0%B5%D0%BB%D0%BE%D0%B5_%D1%87%D0%B8%D1%81%D0%BB%D0%BE";

    unescaped = wxURI::Unescape(escaped);

    CPPUNIT_ASSERT_EQUAL( wxString::FromUTF8(
                            "http://ru.wikipedia.org/wiki/"
                            "\xD0\xA6\xD0\xB5\xD0\xBB\xD0\xBE\xD0\xB5_"
                            "\xD1\x87\xD0\xB8\xD1\x81\xD0\xBB\xD0\xBE"
                          ),
                          unescaped );

    escaped = L"file://\u043C\u043E\u0439%5C%d1%84%d0%b0%d0%b9%d0%bb";
    unescaped = wxURI::Unescape(escaped);

    CPPUNIT_ASSERT_EQUAL
    (
        L"file://\u043C\u043E\u0439\\\u0444\u0430\u0439\u043B",
        unescaped
    );


    escaped = "%2FH%C3%A4ll%C3%B6%5C";
    unescaped = wxURI(escaped).BuildUnescapedURI();
    CPPUNIT_ASSERT_EQUAL(wxString::FromUTF8("\x2FH\xC3\xA4ll\xC3\xB6\x5C"),
                    unescaped);
}

void URITestCase::FileScheme()
{
    //file:// variety (NOT CONFORMING TO THE RFC)
    URI_ASSERT_PATH_EQUAL( "file://e:/wxcode/script1.xml",
                    "e:/wxcode/script1.xml" );

    //file:/// variety
    URI_ASSERT_PATH_EQUAL( "file:///e:/wxcode/script1.xml",
                    "/e:/wxcode/script1.xml" );

    //file:/ variety
    URI_ASSERT_PATH_EQUAL( "file:/e:/wxcode/script1.xml",
                    "/e:/wxcode/script1.xml" );

    //file: variety
    URI_ASSERT_PATH_EQUAL( "file:e:/wxcode/script1.xml",
                    "e:/wxcode/script1.xml" );
}

#if TEST_URL

#include "wx/url.h"
#include "wx/file.h"

void URITestCase::URLCompat()
{
    wxURL url("http://user:password@wxwidgets.org");

    CPPUNIT_ASSERT( url.GetError() == wxURL_NOERR );
    CPPUNIT_ASSERT( url == wxURL("http://user:password@wxwidgets.org") );

    wxURI uri("http://user:password@wxwidgets.org");

    CPPUNIT_ASSERT( url == uri );

    wxURL urlcopy(uri);

    CPPUNIT_ASSERT( urlcopy == url );
    CPPUNIT_ASSERT( urlcopy == uri );

    wxURI uricopy(url);

    CPPUNIT_ASSERT( uricopy == url );
    CPPUNIT_ASSERT( uricopy == urlcopy );
    CPPUNIT_ASSERT( uricopy == uri );
    CPPUNIT_ASSERT_EQUAL( " A ", wxURI::Unescape("%20%41%20") );

    wxURI test("file:\"myf\"ile.txt");

    CPPUNIT_ASSERT_EQUAL( "file:%22myf%22ile.txt" , test.BuildURI() );
    CPPUNIT_ASSERT_EQUAL( "file", test.GetScheme() );
    CPPUNIT_ASSERT_EQUAL( "%22myf%22ile.txt", test.GetPath() );

    // these could be put under a named registry since they take some
    // time to complete
#if 0
    // Test problem urls (reported not to work some time ago by a user...)
    const wxChar* pszProblemUrls[] = { "http://www.csdn.net",
                                       "http://www.163.com",
                                       "http://www.sina.com.cn" };

    for ( size_t i = 0; i < WXSIZEOF(pszProblemUrls); ++i )
    {
        wxURL urlProblem(pszProblemUrls[i]);
        CPPUNIT_ASSERT(urlProblem.GetError() == wxURL_NOERR);

        wxInputStream* is = urlProblem.GetInputStream();
        CPPUNIT_ASSERT(is != nullptr);

        wxFile fOut(wxT("test.html"), wxFile::write);
        wxASSERT(fOut.IsOpened());

        char buf[1001];
        for( ;; )
        {
            is->Read(buf, 1000);
            size_t n = is->LastRead();
            if ( n == 0 )
                break;
            buf[n] = 0;
            fOut.Write(buf, n);
        }

        delete is;
    }
#endif
}

void URITestCase::Normalizing()
{
#if 0 // NB: wxURI doesn't have dedicated normalization support yet
    //5.2.4 #2 remove dot segments
    URI_ASSERT_NORMALIZEDPATH_EQUAL("./", "");			//A
    wxURI ss("./");
    ss.Resolve(wxURI("http://a.com/"));
    URI_ASSERT_NORMALIZEDPATH_EQUAL("/./", "/");		//B
    URI_ASSERT_NORMALIZEDPATH_EQUAL("/.", "/");			//B2
    URI_ASSERT_NORMALIZEDPATH_EQUAL("/../", "/");		//C
    URI_ASSERT_NORMALIZEDPATH_EQUAL("/..", "/");		//C2
    URI_ASSERT_NORMALIZEDPATH_EQUAL(".", "");			//D
    URI_ASSERT_NORMALIZEDPATH_EQUAL("../", "");			//A2
    URI_ASSERT_NORMALIZEDPATH_EQUAL("..", "");			//D2
    URI_ASSERT_NORMALIZEDPATH_EQUAL("../../../", "");	//A2 complex
    URI_ASSERT_NORMALIZEDPATH_EQUAL("../..", "");		//A2+D2 complex
                                                        //5.2.4 in practice
    URI_ASSERT_NORMALIZEDPATH_EQUAL("path/john/../../../joe", "joe");
    URI_ASSERT_NORMALIZEDPATH_EQUAL("http://user:password@192.256.1.100:5050/../path", "/path");
    URI_ASSERT_NORMALIZEDPATH_EQUAL("http://user:password@192.256.1.100:5050/path/../", "/");
    URI_ASSERT_NORMALIZEDPATH_EQUAL("http://user:password@192.256.1.100:5050/path/.", "/path/");
    URI_ASSERT_NORMALIZEDPATH_EQUAL("http://user:password@192.256.1.100:5050/path/./", "/path/");
    // hexdigit normalizing
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%aA", "%AA");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%Aa", "%AA");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%aa", "%AA");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%AA", "%AA");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%Af", "%AF");
    //Alpha/Digit/'-'/'.'/'_'/'~'
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%42", "B");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%30", "0");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%2D", "-");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%2E", ".");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%5F", "_");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%7E", "~");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%42%30%2D%2E%5F%7E", "B0-._~");
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL("%F1%42%30%2D%Fa%2E%5F%7E%F1", "%F1B0-%FA._~%F1");
#endif
}

// This is for testing routing through a proxy with wxURL, it's a little niche
// and requires a specific setup.
#if 0 && wxUSE_PROTOCOL_HTTP
void URITestCase::URLProxy()
{
    wxURL url(wxT("http://www.asite.com/index.html"));
    url.SetProxy(wxT("pserv:3122"));

    wxURL::SetDefaultProxy(wxT("fol.singnet.com.sg:8080"));
    wxURL url2(wxT("http://server-name/path/to/file?query_data=value"));
    wxInputStream *data = url2.GetInputStream();
    CPPUNIT_ASSERT(data != nullptr);
}
#endif // wxUSE_PROTOCOL_HTTP

#endif // TEST_URL
