///////////////////////////////////////////////////////////////////////////////
// Name:        tests/cmdline/cmdlinetest.cpp
// Purpose:     wxCmdLineParser unit test
// Author:      Vadim Zeitlin
// Created:     2008-04-12
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

#include "testdate.h"

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
        CPPUNIT_TEST( ArgumentsCollection );
        CPPUNIT_TEST( Usage );
        CPPUNIT_TEST( Found );
    CPPUNIT_TEST_SUITE_END();

    void ConvertStringTestCase();
    void ParseSwitches();
    void ArgumentsCollection();
    void Usage();
    void Found();

    wxDECLARE_NO_COPY_CLASS(CmdLineTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( CmdLineTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CmdLineTestCase, "CmdLineTestCase" );

// Use this macro to compare a wxArrayString with the pipe-separated elements
// of the given string
//
// NB: it's a macro and not a function to have the correct line numbers in the
//     test failure messages
#define WX_ASSERT_STRARRAY_EQUAL(s, a)                                        \
    {                                                                         \
        wxArrayString expected(wxSplit(s, '|', '\0'));                        \
                                                                              \
        CPPUNIT_ASSERT_EQUAL( expected.size(), a.size() );                    \
                                                                              \
        for ( size_t n = 0; n < a.size(); n++ )                               \
        {                                                                     \
            CPPUNIT_ASSERT_EQUAL( expected[n], a[n] );                        \
        }                                                                     \
    }

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
        virtual void Output(const wxString& WXUNUSED(str)) wxOVERRIDE { }
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

void CmdLineTestCase::ArgumentsCollection()
{
    wxCmdLineParser p;

    p.AddLongSwitch ("verbose");
    p.AddOption ("l", "long", wxEmptyString, wxCMD_LINE_VAL_NUMBER);
    p.AddOption ("d", "date", wxEmptyString, wxCMD_LINE_VAL_DATE);
    p.AddOption ("f", "double", wxEmptyString, wxCMD_LINE_VAL_DOUBLE);
    p.AddOption ("s", "string", wxEmptyString, wxCMD_LINE_VAL_STRING);
    p.AddParam (wxEmptyString, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE);

    wxDateTime wasNow = wxDateTime::Now().GetDateOnly();
    p.SetCmdLine (wxString::Format ("--verbose param1 -l 22 -d \"%s\" -f 50.12e-1 param2 --string \"some string\"",
        wasNow.FormatISODate()));

    CPPUNIT_ASSERT_EQUAL(0, p.Parse(false) );

    wxCmdLineArgs::const_iterator itargs = p.GetArguments().begin();

    // --verbose
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_SWITCH, itargs->GetKind());
    CPPUNIT_ASSERT_EQUAL("verbose", itargs->GetLongName());
    CPPUNIT_ASSERT_EQUAL(false, itargs->IsNegated());

    // param1
    ++itargs; // pre incrementation test
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_PARAM, itargs->GetKind());
    CPPUNIT_ASSERT_EQUAL("param1", itargs->GetStrVal());

    // -l 22
    itargs++; // post incrementation test
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_OPTION, itargs->GetKind());
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_VAL_NUMBER, itargs->GetType());
    CPPUNIT_ASSERT_EQUAL("l", itargs->GetShortName());
    CPPUNIT_ASSERT_EQUAL(22, itargs->GetLongVal());

    // -d (some date)
    ++itargs;
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_OPTION, itargs->GetKind());
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_VAL_DATE, itargs->GetType());
    CPPUNIT_ASSERT_EQUAL("d", itargs->GetShortName());
    CPPUNIT_ASSERT_EQUAL(wasNow, itargs->GetDateVal());

    // -f 50.12e-1
    ++itargs;
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_OPTION, itargs->GetKind());
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_VAL_DOUBLE, itargs->GetType());
    CPPUNIT_ASSERT_EQUAL("f", itargs->GetShortName());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(50.12e-1, itargs->GetDoubleVal(), 0.000001);

    // param2
    ++itargs;
    CPPUNIT_ASSERT_EQUAL (wxCMD_LINE_PARAM, itargs->GetKind());
    CPPUNIT_ASSERT_EQUAL ("param2", itargs->GetStrVal());

    // --string "some string"
    ++itargs;
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_OPTION, itargs->GetKind());
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_VAL_STRING, itargs->GetType());
    CPPUNIT_ASSERT_EQUAL("s", itargs->GetShortName());
    CPPUNIT_ASSERT_EQUAL("string", itargs->GetLongName());
    CPPUNIT_ASSERT_EQUAL("some string", itargs->GetStrVal());

    // testing pre and post-increment
    --itargs;
    itargs--;
    CPPUNIT_ASSERT_EQUAL(wxCMD_LINE_VAL_DOUBLE, itargs->GetType());

    ++itargs;++itargs;++itargs;
    CPPUNIT_ASSERT(itargs == p.GetArguments().end());
}

void CmdLineTestCase::Usage()
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

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

    wxGCC_WARNING_RESTORE(missing-field-initializers)

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

    CPPUNIT_ASSERT_EQUAL((size_t)Line_Max, usageLines.size());
    CPPUNIT_ASSERT_EQUAL("Verbosity options", usageLines[Line_Text_Verbosity]);
    CPPUNIT_ASSERT_EQUAL("", usageLines[Line_Text_Dummy1]);
    CPPUNIT_ASSERT_EQUAL("Even more usage text", usageLines[Line_Text_Dummy2]);
    CPPUNIT_ASSERT_EQUAL("", usageLines[Line_Last]);
}

void CmdLineTestCase::Found()
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const wxCmdLineEntryDesc desc[] =
    {
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_OPTION, "o", "output",  "output file" },
        { wxCMD_LINE_OPTION, "s", "size",    "output block size", wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, "d", "date",    "output file date", wxCMD_LINE_VAL_DATE },
        { wxCMD_LINE_OPTION, "f", "double",  "output double", wxCMD_LINE_VAL_DOUBLE },
        { wxCMD_LINE_PARAM,  NULL, NULL, "input file", },
        { wxCMD_LINE_NONE }
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    wxCmdLineParser p(desc);
    p.SetCmdLine ("-v --output hello -s 2 --date=2014-02-17 -f 0.2 input-file.txt");

    CPPUNIT_ASSERT(p.Parse() == 0);

    wxString dummys;
    wxDateTime dummydate;
    long dummyl;
    double dummyd;
    // now verify that any option/switch badly queried actually generates an exception
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("v", &dummyd));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("v", &dummydate));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("v", &dummyl));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("v", &dummys));
    CPPUNIT_ASSERT(p.FoundSwitch("v") != wxCMD_SWITCH_NOT_FOUND);
    CPPUNIT_ASSERT(p.Found("v"));

    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("o", &dummyd));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("o", &dummydate));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("o", &dummyl));
    WX_ASSERT_FAILS_WITH_ASSERT(p.FoundSwitch("o"));
    CPPUNIT_ASSERT(p.Found("o", &dummys));
    CPPUNIT_ASSERT(p.Found("o"));

    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("s", &dummyd));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("s", &dummydate));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("s", &dummys));
    WX_ASSERT_FAILS_WITH_ASSERT(p.FoundSwitch("s"));
    CPPUNIT_ASSERT(p.Found("s", &dummyl));
    CPPUNIT_ASSERT(p.Found("s"));

    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("d", &dummyd));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("d", &dummyl));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("d", &dummys));
    WX_ASSERT_FAILS_WITH_ASSERT(p.FoundSwitch("d"));
    CPPUNIT_ASSERT(p.Found("d", &dummydate));
    CPPUNIT_ASSERT(p.Found("d"));

    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("f", &dummydate));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("f", &dummyl));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("f", &dummys));
    WX_ASSERT_FAILS_WITH_ASSERT(p.FoundSwitch("f"));
    CPPUNIT_ASSERT(p.Found("f", &dummyd));
    CPPUNIT_ASSERT(p.Found("f"));
}
