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

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/cmdline.h"
#include "wx/msgout.h"
#include "wx/scopeguard.h"

// --------------------------------------------------------------------------
// test helpers
// --------------------------------------------------------------------------

// Use this macro to compare a wxArrayString with the pipe-separated elements
// of the given string
//
// NB: it's a macro and not a function to have the correct line numbers in the
//     test failure messages
#define WX_ASSERT_STRARRAY_EQUAL(s, a)                                        \
    {                                                                         \
        wxArrayString expected(wxSplit(s, '|', '\0'));                        \
                                                                              \
        CHECK( a.size() == expected.size() );                    \
                                                                              \
        for ( size_t n = 0; n < a.size(); n++ )                               \
        {                                                                     \
            CHECK( a[n] == expected[n] );                        \
        }                                                                     \
    }

// ============================================================================
// tests
// ============================================================================

TEST_CASE("CmdLine::ConvertString", "[cmdline]")
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

TEST_CASE("CmdLine::ParseSwitches", "[cmdline]")
{
    // install a dummy message output object just suppress error messages from
    // wxCmdLineParser::Parse()
    class NoMessageOutput : public wxMessageOutput
    {
    public:
        virtual void Output(const wxString& WXUNUSED(str)) override { }
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
    CHECK(p.Parse(false) == 0 );
    CHECK( !p.Found("a") );

    p.SetCmdLine("-z");
    CHECK( p.Parse(false) != 0 );

    p.SetCmdLine("-a");
    CHECK(p.Parse(false) == 0 );
    CHECK( p.Found("a") );
    CHECK( !p.Found("b") );

    p.SetCmdLine("-a -d");
    CHECK(p.Parse(false) == 0 );
    CHECK( p.Found("a") );
    CHECK( !p.Found("b") );
    CHECK( !p.Found("c") );
    CHECK( p.Found("d") );

    p.SetCmdLine("-abd");
    CHECK(p.Parse(false) == 0 );
    CHECK( p.Found("a") );
    CHECK( p.Found("b") );
    CHECK( !p.Found("c") );
    CHECK( p.Found("d") );

    p.SetCmdLine("-abdz");
    CHECK( p.Parse(false) != 0 );

    p.SetCmdLine("-ab -cd");
    CHECK(p.Parse(false) == 0 );
    CHECK( p.Found("a") );
    CHECK( p.Found("b") );
    CHECK( p.Found("c") );
    CHECK( p.Found("d") );

    p.SetCmdLine("-da");
    CHECK(p.Parse(false) == 0 );
    CHECK( p.Found("a") );
    CHECK( !p.Found("b") );
    CHECK( !p.Found("c") );
    CHECK( p.Found("d") );

    p.SetCmdLine("-n");
    CHECK(p.Parse(false) == 0 );
    CHECK(p.FoundSwitch("a") == wxCMD_SWITCH_NOT_FOUND );
    CHECK(p.FoundSwitch("n") == wxCMD_SWITCH_ON );

    p.SetCmdLine("-n-");
    CHECK(p.Parse(false) == 0 );
    CHECK(p.FoundSwitch("neg") == wxCMD_SWITCH_OFF );

    p.SetCmdLine("--neg");
    CHECK(p.Parse(false) == 0 );
    CHECK(p.FoundSwitch("neg") == wxCMD_SWITCH_ON );

    p.SetCmdLine("--neg-");
    CHECK(p.Parse(false) == 0 );
    CHECK(p.FoundSwitch("n") == wxCMD_SWITCH_OFF );
}

TEST_CASE("CmdLine::ArgumentsCollection", "[cmdline]")
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

    CHECK(p.Parse(false) == 0 );

    wxCmdLineArgs::const_iterator itargs = p.GetArguments().begin();

    // --verbose
    CHECK(itargs->GetKind() == wxCMD_LINE_SWITCH);
    CHECK(itargs->GetLongName() == "verbose");
    CHECK(itargs->IsNegated() == false);

    // param1
    ++itargs; // pre incrementation test
    CHECK(itargs->GetKind() == wxCMD_LINE_PARAM);
    CHECK(itargs->GetStrVal() == "param1");

    // -l 22
    itargs++; // post incrementation test
    CHECK(itargs->GetKind() == wxCMD_LINE_OPTION);
    CHECK(itargs->GetType() == wxCMD_LINE_VAL_NUMBER);
    CHECK(itargs->GetShortName() == "l");
    CHECK(itargs->GetLongVal() == 22);

    // -d (some date)
    ++itargs;
    CHECK(itargs->GetKind() == wxCMD_LINE_OPTION);
    CHECK(itargs->GetType() == wxCMD_LINE_VAL_DATE);
    CHECK(itargs->GetShortName() == "d");
    CHECK(itargs->GetDateVal() == wasNow);

    // -f 50.12e-1
    ++itargs;
    CHECK(itargs->GetKind() == wxCMD_LINE_OPTION);
    CHECK(itargs->GetType() == wxCMD_LINE_VAL_DOUBLE);
    CHECK(itargs->GetShortName() == "f");
    CHECK(itargs->GetDoubleVal() == Approx(50.12e-1).margin(0.000001));

    // param2
    ++itargs;
    CHECK(itargs->GetKind() == wxCMD_LINE_PARAM);
    CHECK(itargs->GetStrVal() == "param2");

    // --string "some string"
    ++itargs;
    CHECK(itargs->GetKind() == wxCMD_LINE_OPTION);
    CHECK(itargs->GetType() == wxCMD_LINE_VAL_STRING);
    CHECK(itargs->GetShortName() == "s");
    CHECK(itargs->GetLongName() == "string");
    CHECK(itargs->GetStrVal() == "some string");

    // testing pre and post-increment
    --itargs;
    itargs--;
    CHECK(itargs->GetType() == wxCMD_LINE_VAL_DOUBLE);

    ++itargs;++itargs;++itargs;
    CHECK(itargs == p.GetArguments().end());
}

TEST_CASE("CmdLine::Usage", "[cmdline]")
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    // check that Usage() returns roughly what we expect (don't check all the
    // details, its format can change in the future)
    static const wxCmdLineEntryDesc desc[] =
    {
        { wxCMD_LINE_USAGE_TEXT, nullptr, nullptr, "Verbosity options" },
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_SWITCH, "q", "quiet",   "be quiet" },

        { wxCMD_LINE_USAGE_TEXT, nullptr, nullptr, "Output options" },
        { wxCMD_LINE_OPTION, "o", "output",  "output file" },
        { wxCMD_LINE_OPTION, "s", "size",    "output block size", wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, "d", "date",    "output file date", wxCMD_LINE_VAL_DATE },
        { wxCMD_LINE_OPTION, "f", "double",  "output double", wxCMD_LINE_VAL_DOUBLE },

        { wxCMD_LINE_PARAM,  nullptr, nullptr, "input file", },

        { wxCMD_LINE_USAGE_TEXT, nullptr, nullptr, "\nEven more usage text" },
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

    CHECK(usageLines.size() == (size_t)Line_Max);
    CHECK(usageLines[Line_Text_Verbosity] == "Verbosity options");
    CHECK(usageLines[Line_Text_Dummy1] == "");
    CHECK(usageLines[Line_Text_Dummy2] == "Even more usage text");
    CHECK(usageLines[Line_Last] == "");
}

TEST_CASE("CmdLine::Found", "[cmdline]")
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const wxCmdLineEntryDesc desc[] =
    {
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_OPTION, "o", "output",  "output file" },
        { wxCMD_LINE_OPTION, "s", "size",    "output block size", wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, "d", "date",    "output file date", wxCMD_LINE_VAL_DATE },
        { wxCMD_LINE_OPTION, "f", "double",  "output double", wxCMD_LINE_VAL_DOUBLE },
        { wxCMD_LINE_PARAM,  nullptr, nullptr, "input file", },
        { wxCMD_LINE_NONE }
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    wxCmdLineParser p(desc);
    p.SetCmdLine ("-v --output hello -s 2 --date=2014-02-17 -f 0.2 input-file.txt");

    CHECK(p.Parse() == 0);

    wxString dummys;
    wxDateTime dummydate;
    long dummyl;
    double dummyd;
    // now verify that any option/switch badly queried actually generates an exception
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("v", &dummyd));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("v", &dummydate));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("v", &dummyl));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("v", &dummys));
    CHECK(p.FoundSwitch("v") != wxCMD_SWITCH_NOT_FOUND);
    CHECK(p.Found("v"));

    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("o", &dummyd));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("o", &dummydate));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("o", &dummyl));
    WX_ASSERT_FAILS_WITH_ASSERT(p.FoundSwitch("o"));
    CHECK(p.Found("o", &dummys));
    CHECK(p.Found("o"));

    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("s", &dummyd));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("s", &dummydate));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("s", &dummys));
    WX_ASSERT_FAILS_WITH_ASSERT(p.FoundSwitch("s"));
    CHECK(p.Found("s", &dummyl));
    CHECK(p.Found("s"));

    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("d", &dummyd));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("d", &dummyl));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("d", &dummys));
    WX_ASSERT_FAILS_WITH_ASSERT(p.FoundSwitch("d"));
    CHECK(p.Found("d", &dummydate));
    CHECK(p.Found("d"));

    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("f", &dummydate));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("f", &dummyl));
    WX_ASSERT_FAILS_WITH_ASSERT(p.Found("f", &dummys));
    WX_ASSERT_FAILS_WITH_ASSERT(p.FoundSwitch("f"));
    CHECK(p.Found("f", &dummyd));
    CHECK(p.Found("f"));
}
