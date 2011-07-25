///////////////////////////////////////////////////////////////////////////////
// Name:        tests/cmdline/cmdlinetest.cpp
// Purpose:     wxCmdLineParser unit test
// Author:      Vadim Zeitlin
// Created:     2008-04-12
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/cmdline.h"
#include "wx/msgout.h"
#include "wx/scopeguard.h"

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class CmdLineTestCase : public CppUnit::TestCase
{
public:
    CmdLineTestCase() {}

private:
    CPPUNIT_TEST_SUITE( CmdLineTestCase );
        CPPUNIT_TEST( ConvertStringTestCase );
        CPPUNIT_TEST( ParseSwitches );
        CPPUNIT_TEST( Usage );
    CPPUNIT_TEST_SUITE_END();

    void ConvertStringTestCase();
    void ParseSwitches();
    void Usage();

    DECLARE_NO_COPY_CLASS(CmdLineTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( CmdLineTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CmdLineTestCase, "CmdLineTestCase" );

// ============================================================================
// implementation
// ============================================================================

void CmdLineTestCase::ConvertStringTestCase()
{
    #define WX_ASSERT_DOS_ARGS_EQUAL(s, args)                                 \
        {                                                                     \
            const wxArrayString                                               \
                argsDOS(wxCmdLineParser::ConvertStringToArgs(args,            \
                                            wxCMD_LINE_SPLIT_DOS));           \
            WX_ASSERT_STRARRAY_EQUAL(s, argsDOS);                             \
        }

    #define WX_ASSERT_UNIX_ARGS_EQUAL(s, args)                                \
        {                                                                     \
            const wxArrayString                                               \
                argsUnix(wxCmdLineParser::ConvertStringToArgs(args,           \
                                            wxCMD_LINE_SPLIT_UNIX));          \
            WX_ASSERT_STRARRAY_EQUAL(s, argsUnix);                            \
        }

    #define WX_ASSERT_ARGS_EQUAL(s, args)                                     \
        WX_ASSERT_DOS_ARGS_EQUAL(s, args)                                     \
        WX_ASSERT_UNIX_ARGS_EQUAL(s, args)

    // normal cases
    WX_ASSERT_ARGS_EQUAL( "foo", "foo" )
    WX_ASSERT_ARGS_EQUAL( "foo bar", "\"foo bar\"" )
    WX_ASSERT_ARGS_EQUAL( "foo|bar", "foo bar" )
    WX_ASSERT_ARGS_EQUAL( "foo|bar|baz", "foo bar baz" )
    WX_ASSERT_ARGS_EQUAL( "foo|bar baz", "foo \"bar baz\"" )

    // special cases
    WX_ASSERT_ARGS_EQUAL( "", "" )
    WX_ASSERT_ARGS_EQUAL( "foo", "foo " )
    WX_ASSERT_ARGS_EQUAL( "foo", "foo \t   " )
    WX_ASSERT_ARGS_EQUAL( "foo|bar", "foo bar " )
    WX_ASSERT_ARGS_EQUAL( "foo|bar|", "foo bar \"" )
    WX_ASSERT_DOS_ARGS_EQUAL( "foo|bar|\\", "foo bar \\" )
    WX_ASSERT_UNIX_ARGS_EQUAL( "foo|bar|", "foo bar \\" )

    WX_ASSERT_ARGS_EQUAL( "12 34", "1\"2 3\"4" );
    WX_ASSERT_ARGS_EQUAL( "1|2 34", "1 \"2 3\"4" );
    WX_ASSERT_ARGS_EQUAL( "1|2 3|4", "1 \"2 3\" 4" );

    // check for (broken) Windows semantics: backslash doesn't escape spaces
    WX_ASSERT_DOS_ARGS_EQUAL( "\\\\foo\\\\|/bar", "\"\\\\foo\\\\\" /bar" );
    WX_ASSERT_DOS_ARGS_EQUAL( "foo|bar\\|baz", "foo bar\\ baz" );
    WX_ASSERT_DOS_ARGS_EQUAL( "foo|bar\\\"baz", "foo \"bar\\\"baz\"" );

    // check for more sane Unix semantics: backslash does escape spaces and
    // quotes
    WX_ASSERT_UNIX_ARGS_EQUAL( "foo|bar baz", "foo bar\\ baz" );
    WX_ASSERT_UNIX_ARGS_EQUAL( "foo|bar\"baz", "foo \"bar\\\"baz\"" );

    // check that single quotes work too with Unix semantics
    WX_ASSERT_UNIX_ARGS_EQUAL( "foo bar", "'foo bar'" )
    WX_ASSERT_UNIX_ARGS_EQUAL( "foo|bar baz", "foo 'bar baz'" )
    WX_ASSERT_UNIX_ARGS_EQUAL( "foo|bar baz", "foo 'bar baz'" )
    WX_ASSERT_UNIX_ARGS_EQUAL( "O'Henry", "\"O'Henry\"" )
    WX_ASSERT_UNIX_ARGS_EQUAL( "O'Henry", "O\\'Henry" )

    #undef WX_ASSERT_DOS_ARGS_EQUAL
    #undef WX_ASSERT_UNIX_ARGS_EQUAL
    #undef WX_ASSERT_ARGS_EQUAL
}

void CmdLineTestCase::ParseSwitches()
{
    // install a dummy message output object just suppress error messages from
    // wxCmdLineParser::Parse()
    class NoMessageOutput : public wxMessageOutput
    {
    public:
        virtual void Output(const wxString& WXUNUSED(str)) { }
    } noMessages;

    wxMessageOutput * const old = wxMessageOutput::Set(&noMessages);
    wxON_BLOCK_EXIT1( wxMessageOutput::Set, old );

    wxCmdLineParser p;
    p.AddSwitch("a");
    p.AddSwitch("b");
    p.AddSwitch("c");
    p.AddSwitch("d");
    p.AddSwitch("n", "neg", "Switch that can be negated",
                wxCMD_LINE_SWITCH_NEGATABLE);

    p.SetCmdLine("");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT( !p.Found("a") );

    p.SetCmdLine("-z");
    CPPUNIT_ASSERT( p.Parse(false) != 0 );

    p.SetCmdLine("-a");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT( p.Found("a") );
    CPPUNIT_ASSERT( !p.Found("b") );

    p.SetCmdLine("-a -d");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT( p.Found("a") );
    CPPUNIT_ASSERT( !p.Found("b") );
    CPPUNIT_ASSERT( !p.Found("c") );
    CPPUNIT_ASSERT( p.Found("d") );

    p.SetCmdLine("-abd");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT( p.Found("a") );
    CPPUNIT_ASSERT( p.Found("b") );
    CPPUNIT_ASSERT( !p.Found("c") );
    CPPUNIT_ASSERT( p.Found("d") );

    p.SetCmdLine("-abdz");
    CPPUNIT_ASSERT( p.Parse(false) != 0 );

    p.SetCmdLine("-ab -cd");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT( p.Found("a") );
    CPPUNIT_ASSERT( p.Found("b") );
    CPPUNIT_ASSERT( p.Found("c") );
    CPPUNIT_ASSERT( p.Found("d") );

    p.SetCmdLine("-da");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT( p.Found("a") );
    CPPUNIT_ASSERT( !p.Found("b") );
    CPPUNIT_ASSERT( !p.Found("c") );
    CPPUNIT_ASSERT( p.Found("d") );

    p.SetCmdLine("-n");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT_EQUAL(wxCMD_SWITCH_NOT_FOUND, p.FoundSwitch("a") );
    CPPUNIT_ASSERT_EQUAL(wxCMD_SWITCH_ON, p.FoundSwitch("n") );

    p.SetCmdLine("-n-");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT_EQUAL(wxCMD_SWITCH_OFF, p.FoundSwitch("neg") );

    p.SetCmdLine("--neg");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT_EQUAL(wxCMD_SWITCH_ON, p.FoundSwitch("neg") );

    p.SetCmdLine("--neg-");
    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );
    CPPUNIT_ASSERT_EQUAL(wxCMD_SWITCH_OFF, p.FoundSwitch("n") );
}

void CmdLineTestCase::Usage()
{
    // check that Usage() returns roughly what we expect (don't check all the
    // details, its format can change in the future)
    static const wxCmdLineEntryDesc desc[] =
    {
        { wxCMD_LINE_USAGE_TEXT, NULL, NULL, "Verbosity options" },
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_SWITCH, "q", "quiet",   "be quiet" },

        { wxCMD_LINE_USAGE_TEXT, NULL, NULL, "Output options" },
        { wxCMD_LINE_OPTION, "o", "output",  "output file" },
        { wxCMD_LINE_OPTION, "s", "size",    "output block size", wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, "d", "date",    "output file date", wxCMD_LINE_VAL_DATE },
        { wxCMD_LINE_OPTION, "f", "double",  "output double", wxCMD_LINE_VAL_DOUBLE },

        { wxCMD_LINE_PARAM,  NULL, NULL, "input file", },

        { wxCMD_LINE_USAGE_TEXT, NULL, NULL, "\nEven more usage text" },
        { wxCMD_LINE_NONE }
    };

    wxCmdLineParser p(desc);
    const wxArrayString usageLines = wxSplit(p.GetUsageString(), '\n');

    enum
    {
        Line_Synopsis,
        Line_Text_Verbosity,
        Line_Verbose,
        Line_Quiet,
        Line_Text_Output,
        Line_Output_File,
        Line_Output_Size,
        Line_Output_Date,
        Line_Output_Double,
        Line_Text_Dummy1,
        Line_Text_Dummy2,
        Line_Last,
        Line_Max
    };

    CPPUNIT_ASSERT_EQUAL(Line_Max, usageLines.size());
    CPPUNIT_ASSERT_EQUAL("Verbosity options", usageLines[Line_Text_Verbosity]);
    CPPUNIT_ASSERT_EQUAL("", usageLines[Line_Text_Dummy1]);
    CPPUNIT_ASSERT_EQUAL("Even more usage text", usageLines[Line_Text_Dummy2]);
    CPPUNIT_ASSERT_EQUAL("", usageLines[Line_Last]);
}
