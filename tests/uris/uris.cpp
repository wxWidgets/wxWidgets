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

// Test wxURL & wxURI compat?
#define TEST_URL wxUSE_URL

// ----------------------------------------------------------------------------
// helper macros
// ----------------------------------------------------------------------------

// apply the given accessor to the URI, check that the result is as expected
#define URI_ASSERT_PART_EQUAL(uri, expected, accessor) \
    wxSTATEMENT_MACRO_BEGIN \
        const wxURI u(uri); \
        INFO(DumpURI(u)); \
        CHECK(u.accessor == expected); \
    wxSTATEMENT_MACRO_END

#define URI_ASSERT_HOST_TEST(uri, expectedhost, expectedtype) \
    wxSTATEMENT_MACRO_BEGIN \
        const wxURI u(uri); \
        INFO(DumpURI(u)); \
        CHECK(u.GetServer() == expectedhost); \
        CHECK(u.GetHostType() == expectedtype); \
    wxSTATEMENT_MACRO_END

#define URI_ASSERT_HOST_TESTBAD(uri, ne) \
    wxSTATEMENT_MACRO_BEGIN \
        const wxURI u(uri); \
        INFO(DumpURI(u)); \
        CHECK(u.GetHostType() != ne); \
    wxSTATEMENT_MACRO_END

#define URI_ASSERT_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL(uri, expected, BuildURI())
#define URI_ASSERT_HOST_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL(uri, expected, GetServer())
#define URI_ASSERT_PATH_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL((uri), (expected), GetPath())
#define URI_ASSERT_HOSTTYPE_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL(uri, expected, GetHostType())
#define URI_ASSERT_USER_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL(uri, expected, GetUser())
#define URI_ASSERT_USERINFO_EQUAL(uri, expected) \
    URI_ASSERT_PART_EQUAL(uri, expected, GetUserInfo())

#define URI_ASSERT_BADPATH(uri) \
    wxSTATEMENT_MACRO_BEGIN \
        const wxURI u(uri); \
        INFO(DumpURI(u)); \
        CHECK(!u.HasPath()); \
    wxSTATEMENT_MACRO_END

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
    wxSTATEMENT_MACRO_BEGIN \
        wxURI uri(string); \
        uri.Resolve(masteruri, strictness); \
        INFO(DumpURI(uri)); \
        CHECK(uri.BuildURI() == eq); \
    wxSTATEMENT_MACRO_END
#define URI_TEST_RESOLVE(string, eq) \
        URI_TEST_RESOLVE_IMPL(string, eq, wxURI_STRICT);
#define URI_TEST_RESOLVE_LAX(string, eq) \
        URI_TEST_RESOLVE_IMPL(string, eq, 0);

// Normalization
#define URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL(uri, expected) \
    wxSTATEMENT_MACRO_BEGIN \
      wxURI nuri(uri); \
      nuri.Resolve(wxURI("http://a/")); \
      INFO(DumpURI(nuri)); \
      CHECK(nuri.GetPath() == expected); \
    wxSTATEMENT_MACRO_END
#define URI_ASSERT_NORMALIZEDPATH_EQUAL(uri, expected) \
    URI_ASSERT_NORMALIZEDENCODEDPATH_EQUAL(uri, expected);

// Helper function used to show components of wxURI.
static wxString DumpURI(const wxURI& uri)
{
    return wxString::Format
           (
             "URI{[%s]://[%s]:[%s]@[%s]:[%s]/[%s]?[%s]#[%s]}",
             uri.GetScheme(),
             uri.GetUser(),
             uri.GetPassword(),
             uri.GetServer(),
             uri.GetPort(),
             uri.GetPath(),
             uri.GetQuery(),
             uri.GetFragment()
           );
}

// Allow CATCH macros output wxURI objects unambiguously.
namespace Catch
{
    template <>
    struct StringMaker<wxURI>
    {
        static std::string convert(const wxURI& uri)
        {
            return DumpURI(uri).utf8_string();
        }
    };
}

// Can be used to check that parsing the given URI fails and log the URI that
// we created if it succeeded.
static inline bool CheckBadURI(const wxString& uri)
{
    wxURI test;
    if ( !test.Create(uri) )
        return true;

    WARN("Parsed \"" << uri << "\" as \"" << DumpURI(test) << "\"");

    return false;
}

TEST_CASE("URI::IPv4", "[uri]")
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
    CHECK( wxURI("http://user:password@192.256.1.100:5050/path").
                    GetHostType() != wxURI_IPV4ADDRESS);
}

TEST_CASE("URI::IPv6", "[uri][ipv6]")
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

TEST_CASE("URI::Host", "[uri]")
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

TEST_CASE("URI::Paths", "[uri]")
{
    try
    {
        wxURI test("http://user:password@192.256.1.100:5050/../path");
        test.BuildURI(); // This isn't a unit test, just a niche parsing crash test
    }
    catch (...)
    {
        FAIL_CHECK(false);
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

    // 8-bit characters in the path should be percent-encoded.
    URI_ASSERT_PATH_EQUAL( wxString::FromUTF8("http://host/é"), "/%c3%a9" );
}

TEST_CASE("URI::UserInfo", "[uri]")
{
    wxURI uri;

    // Simple cases.
    URI_ASSERT_USER_EQUAL( "https://host/", "" );
    URI_ASSERT_USER_EQUAL( "https://user@host/", "user" );

    CHECK( uri.Create("https://user:password@host/") );
    CHECK( uri.GetUser() == "user" );
    CHECK( uri.GetPassword() == "password" );

    // Percent-encoded characters should be accepted.
    CHECK( uri.Create("https://me%40some%3awhere@host/") );
    CHECK( wxURI::Unescape(uri.GetUser()) == "me@some:where" );

    CHECK( uri.Create("https://u:p%3as@h/") );
    CHECK( uri.GetUser() == "u" );
    CHECK( wxURI::Unescape(uri.GetPassword()) == "p:s" );

    // Non-percent encoded characters from gen-delims (RFC 3986 2.2) must be
    // rejected. Note that in some cases the URI will still be parsed, but the
    // user part will not be interpreted as the user name.
    CHECK( wxURI::Unescape(wxURI("https://me@foo@host/").GetServer())
                == "foo@host" );
    CHECK( wxURI("https://me/foo@host/").GetServer() == "me" );
    CHECK( wxURI("https://me#foo@host/").GetFragment() == "foo@host/" );
    CHECK( CheckBadURI("https://me:pass?@host/") );

    // But sub-delims (defined in the same section of the RFC) may be used
    // either in the encoded or raw form.
    URI_ASSERT_USER_EQUAL( "https://me!@host/", "me!" );
    URI_ASSERT_USER_EQUAL( "https://me%21@host/", "me%21" );

    URI_ASSERT_USERINFO_EQUAL( "https://u:pass=word@h/", "u:pass=word" );
    URI_ASSERT_USERINFO_EQUAL( "https://u:pass%3Dword@h/", "u:pass%3Dword" );

    // Also test that using SetUserAndPassword() works.
    uri = "https://host/";
    uri.SetUserAndPassword("me@here!");
    URI_ASSERT_EQUAL( uri, "https://me%40here!@host/" );

    uri.SetUserAndPassword("you:", "?me");
    URI_ASSERT_EQUAL( uri, "https://you%3a:%3fme@host/" );

    uri.SetUserAndPassword(wxString::FromUTF8("ç"));
    URI_ASSERT_USER_EQUAL( uri, "%c3%a7");
}

//examples taken from RFC 2396.bis
TEST_CASE("URI::NormalResolving", "[uri]")
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

TEST_CASE("URI::ComplexResolving", "[uri]")
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

TEST_CASE("URI::ReallyComplexResolving", "[uri]")
{
    wxURI masteruri("http://a/b/c/d;p?q");
    URI_TEST_RESOLVE("./../g"		,"http://a/b/g")
    URI_TEST_RESOLVE("./g/."		,"http://a/b/c/g/")
    URI_TEST_RESOLVE("g/./h"		,"http://a/b/c/g/h")
    URI_TEST_RESOLVE("g/../h"		,"http://a/b/c/h")
    URI_TEST_RESOLVE("g;x=1/./y"	,"http://a/b/c/g;x=1/y")
    URI_TEST_RESOLVE("g;x=1/../y"	,"http://a/b/c/y")
}

TEST_CASE("URI::QueryFragmentResolving", "[uri]")
{
    wxURI masteruri("http://a/b/c/d;p?q"); //query/fragment ambigiousness
    URI_TEST_RESOLVE("g?y/./x",		"http://a/b/c/g?y/./x")
    URI_TEST_RESOLVE("g?y/../x",	"http://a/b/c/g?y/../x")
    URI_TEST_RESOLVE("g#s/./x",		"http://a/b/c/g#s/./x")
    URI_TEST_RESOLVE("g#s/../x",	"http://a/b/c/g#s/../x")
}

TEST_CASE("URI::BackwardsResolving", "[uri]")
{
    wxURI masteruri("http://a/b/c/d;p?q");

    //"NEW"
    URI_TEST_RESOLVE("http:g" ,  "http:g")         //strict
    //bw compat
    URI_TEST_RESOLVE_LAX("http:g", "http://a/b/c/g");
}

TEST_CASE("URI::Ctors", "[uri]")
{
    wxURI uri;
    CHECK( uri.IsEmpty() );

    wxURI uri2("http://foo.bar");
    uri = uri2;
    CHECK( !uri.IsEmpty() );
    CHECK( uri == uri2 );

    uri = wxURI();
    CHECK( uri.IsEmpty() );

    uri = std::move(uri2);
    CHECK( !uri.IsEmpty() );
}

TEST_CASE("URI::Assignment", "[uri]")
{
    wxURI uri1("http://mysite.com"),
          uri2("http://mysite2.com");

    uri2 = uri1;

    CHECK( uri2.BuildURI() == uri1.BuildURI() );
}

TEST_CASE("URI::Comparison", "[uri]")
{
    CHECK(wxURI("http://mysite.com") == wxURI("http://mysite.com"));
}

TEST_CASE("URI::Unescape", "[uri]")
{
    wxString escaped,
             unescaped;

    escaped = "http://test.com/of/file%3A%2F%2FC%3A%5Curi%5C"
              "escaping%5Cthat%5Cseems%5Cbroken%5Csadly%5B1%5D.rss";

    unescaped = wxURI(escaped).BuildUnescapedURI();

    CHECK( unescaped ==
           "http://test.com/of/file://C:\\uri\\"
           "escaping\\that\\seems\\broken\\sadly[1].rss" );

    CHECK( wxURI::Unescape(escaped) == unescaped );


    escaped = "http://ru.wikipedia.org/wiki/"
              "%D0%A6%D0%B5%D0%BB%D0%BE%D0%B5_%D1%87%D0%B8%D1%81%D0%BB%D0%BE";

    unescaped = wxURI::Unescape(escaped);

    CHECK( unescaped == wxString::FromUTF8(
                            "http://ru.wikipedia.org/wiki/Целое_число"
                          ) );

#ifdef wxMUST_USE_U_ESCAPE
    escaped = L"file://\u043C\u043E\u0439%5C%d1%84%d0%b0%d0%b9%d0%bb";
#else
    escaped = L"file://мой%5C%d1%84%d0%b0%d0%b9%d0%bb";
#endif
    unescaped = wxURI::Unescape(escaped);

#ifdef wxMUST_USE_U_ESCAPE
    CHECK
    (
        unescaped == L"file://\u043C\u043E\u0439\\\u0444\u0430\u0439\u043B"
    );
#else
    CHECK( unescaped == L"file://мой\\файл" );
#endif


    escaped = "%2FH%C3%A4ll%C3%B6%5C";
    unescaped = wxURI(escaped).BuildUnescapedURI();
    CHECK( unescaped == wxString::FromUTF8("/Hällö\\") );
}

TEST_CASE("URI::FileScheme", "[uri]")
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

TEST_CASE("URI::Compatibility", "[uri]")
{
    wxURL url("http://user:password@wxwidgets.org");

    CHECK( url.GetError() == wxURL_NOERR );
    CHECK( url == wxURL("http://user:password@wxwidgets.org") );

    wxURI uri("http://user:password@wxwidgets.org");

    CHECK( url == uri );

    wxURL urlcopy(uri);

    CHECK( urlcopy == url );
    CHECK( urlcopy == uri );

    wxURI uricopy(url);

    CHECK( uricopy == url );
    CHECK( uricopy == urlcopy );
    CHECK( uricopy == uri );
    CHECK( wxURI::Unescape("%20%41%20") == " A " );

    wxURI test("file:\"myf\"ile.txt");

    CHECK( test.BuildURI() == "file:%22myf%22ile.txt"  );
    CHECK( test.GetScheme() == "file" );
    CHECK( test.GetPath() == "%22myf%22ile.txt" );

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
        REQUIRE(urlProblem.GetError() == wxURL_NOERR);

        wxInputStream* is = urlProblem.GetInputStream();
        REQUIRE(is != nullptr);

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

TEST_CASE("URI::Normalize", "[uri]")
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
TEST_CASE("URI::Proxy", "[uri]")
{
    wxURL url(wxT("http://www.asite.com/index.html"));
    url.SetProxy(wxT("pserv:3122"));

    wxURL::SetDefaultProxy(wxT("fol.singnet.com.sg:8080"));
    wxURL url2(wxT("http://server-name/path/to/file?query_data=value"));
    wxInputStream *data = url2.GetInputStream();
    REQUIRE(data != nullptr);
}
#endif // wxUSE_PROTOCOL_HTTP

#endif // TEST_URL
