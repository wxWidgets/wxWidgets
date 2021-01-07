///////////////////////////////////////////////////////////////////////////////
// Name:        tests/interactive/input.cpp
// Purpose:     Miscellaneous tests requiring user input
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-21
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "wx/app.h"
#include "wx/wxcrt.h"           // for wxPuts
#include "wx/wxcrtvararg.h"     // for wxPrintf

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#define TEST_SNGLINST
#define TEST_FTP
#define TEST_INFO_FUNCTIONS
#if wxUSE_REGEX
    #define TEST_REGEX
#endif
#define TEST_DATETIME

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class InteractiveInputTestCase : public CppUnit::TestCase
{
public:
    InteractiveInputTestCase() { }

private:
    CPPUNIT_TEST_SUITE( InteractiveInputTestCase );
        CPPUNIT_TEST( TestSingleIstance );
        CPPUNIT_TEST( TestFtpInteractive );
        CPPUNIT_TEST( TestDiskInfo );
        CPPUNIT_TEST( TestRegExInteractive );
        CPPUNIT_TEST( TestDateTimeInteractive );
    CPPUNIT_TEST_SUITE_END();

    void TestSingleIstance();
    void TestFtpInteractive();
    void TestDiskInfo();
    void TestRegExInteractive();
    void TestDateTimeInteractive();

    wxDECLARE_NO_COPY_CLASS(InteractiveInputTestCase);
};

// ----------------------------------------------------------------------------
// CppUnit macros
// ----------------------------------------------------------------------------

// Register this test case as hidden, it shouldn't be run by default.
wxREGISTER_UNIT_TEST_WITH_TAGS(InteractiveInputTestCase,
                               "[!hide][interactive][input]");

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// misc information functions
// ----------------------------------------------------------------------------

#include "wx/utils.h"

void InteractiveInputTestCase::TestDiskInfo()
{
#ifdef TEST_INFO_FUNCTIONS
    wxPuts(wxT("*** Testing wxGetDiskSpace() ***"));

    for ( ;; )
    {
        wxChar pathname[128];
        wxPrintf(wxT("Enter a directory name (press ENTER or type 'quit' to escape): "));
        if ( !wxFgets(pathname, WXSIZEOF(pathname), stdin) )
            break;

        // kill the last '\n'
        pathname[wxStrlen(pathname) - 1] = 0;

        if (pathname[0] == '\0' || wxStrcmp(pathname, "quit") == 0)
            break;

        wxLongLong total, free;
        if ( !wxGetDiskSpace(pathname, &total, &free) )
        {
            wxPuts(wxT("ERROR: wxGetDiskSpace failed."));
        }
        else
        {
            wxPrintf(wxT("%sKb total, %sKb free on '%s'.\n"),
                    (total / 1024).ToString(),
                    (free / 1024).ToString(),
                    pathname);
        }

        wxPuts("\n");
    }

    wxPuts("\n");
#endif // TEST_INFO_FUNCTIONS
}


// ----------------------------------------------------------------------------
// regular expressions
// ----------------------------------------------------------------------------

#include "wx/regex.h"

void InteractiveInputTestCase::TestRegExInteractive()
{
#ifdef TEST_REGEX
    wxPuts(wxT("*** Testing RE interactively ***"));

    for ( ;; )
    {
        wxChar pattern[128];
        wxPrintf(wxT("Enter a pattern (press ENTER or type 'quit' to escape): "));
        if ( !wxFgets(pattern, WXSIZEOF(pattern), stdin) )
            break;

        // kill the last '\n'
        pattern[wxStrlen(pattern) - 1] = 0;

        if (pattern[0] == '\0' || wxStrcmp(pattern, "quit") == 0)
            break;

        wxRegEx re;
        if ( !re.Compile(pattern) )
        {
            continue;
        }

        wxChar text[128];
        for ( ;; )
        {
            wxPrintf(wxT("Enter text to match: "));
            if ( !wxFgets(text, WXSIZEOF(text), stdin) )
                break;

            // kill the last '\n'
            text[wxStrlen(text) - 1] = 0;

            if ( !re.Matches(text) )
            {
                wxPrintf(wxT("No match.\n"));
            }
            else
            {
                wxPrintf(wxT("Pattern matches at '%s'\n"), re.GetMatch(text));

                size_t start, len;
                for ( size_t n = 1; ; n++ )
                {
                    if ( !re.GetMatch(&start, &len, n) )
                    {
                        break;
                    }

                    wxPrintf(wxT("Subexpr %zu matched '%s'\n"),
                             n, wxString(text + start, len));
                }
            }
        }

        wxPuts("\n");
    }
#endif // TEST_REGEX
}


// ----------------------------------------------------------------------------
// FTP
// ----------------------------------------------------------------------------

#include "wx/protocol/ftp.h"
#include "wx/protocol/log.h"

#define FTP_ANONYMOUS
#ifdef FTP_ANONYMOUS
    static const wxChar *hostname = wxT("ftp.wxwidgets.org");
#else
    static const wxChar *hostname = "localhost";
#endif

void InteractiveInputTestCase::TestFtpInteractive()
{
#ifdef TEST_FTP
    wxFTP ftp;

    wxPuts(wxT("\n*** Interactive wxFTP test ***"));

#ifdef FTP_ANONYMOUS
    wxPrintf(wxT("--- Attempting to connect to %s:21 anonymously...\n"), hostname);
#else // !FTP_ANONYMOUS
    wxChar user[256];
    wxFgets(user, WXSIZEOF(user), stdin);
    user[wxStrlen(user) - 1] = '\0'; // chop off '\n'
    ftp.SetUser(user);

    wxChar password[256];
    wxPrintf(wxT("Password for %s: "), password);
    wxFgets(password, WXSIZEOF(password), stdin);
    password[wxStrlen(password) - 1] = '\0'; // chop off '\n'
    ftp.SetPassword(password);

    wxPrintf(wxT("--- Attempting to connect to %s:21 as %s...\n"), hostname, user);
#endif // FTP_ANONYMOUS/!FTP_ANONYMOUS

    if ( !ftp.Connect(hostname) )
    {
        wxPrintf(wxT("ERROR: failed to connect to %s\n"), hostname);

        return;
    }
    else
    {
        wxPrintf(wxT("--- Connected to %s, current directory is '%s'\n"),
                 hostname, ftp.Pwd());
    }

    wxChar buf[128];
    for ( ;; )
    {
        wxPrintf(wxT("Enter FTP command (press ENTER or type 'quit' to escape): "));
        if ( !wxFgets(buf, WXSIZEOF(buf), stdin) )
            break;

        // kill the last '\n'
        buf[wxStrlen(buf) - 1] = 0;

        if (buf[0] == '\0' || wxStrcmp(buf, "quit") == 0)
            break;

        // special handling of LIST and NLST as they require data connection
        wxString start(buf, 4);
        start.MakeUpper();
        if ( start == wxT("LIST") || start == wxT("NLST") )
        {
            wxString wildcard;
            if ( wxStrlen(buf) > 4 )
                wildcard = buf + 5;

            wxArrayString files;
            if ( !ftp.GetList(files, wildcard, start == wxT("LIST")) )
            {
                wxPrintf(wxT("ERROR: failed to get %s of files\n"), start);
            }
            else
            {
                wxPrintf(wxT("--- %s of '%s' under '%s':\n"),
                         start, wildcard, ftp.Pwd());
                size_t count = files.GetCount();
                for ( size_t n = 0; n < count; n++ )
                {
                    wxPrintf(wxT("\t%s\n"), files[n]);
                }
                wxPuts(wxT("--- End of the file list"));
            }
        }
        else // !list
        {
            wxChar ch = ftp.SendCommand(buf);
            wxPrintf(wxT("Command %s"), ch ? wxT("succeeded") : wxT("failed"));
            if ( ch )
            {
                wxPrintf(wxT(" (return code %c)"), ch);
            }

            wxPrintf(wxT(", server reply:\n%s\n\n"), ftp.GetLastResult());
        }
    }

    wxPuts(wxT("\n"));
#endif // TEST_FTP
}

// ----------------------------------------------------------------------------
// date time
// ----------------------------------------------------------------------------

#include "wx/math.h"
#include "wx/datetime.h"

void InteractiveInputTestCase::TestDateTimeInteractive()
{
#ifdef TEST_DATETIME
    wxPuts(wxT("\n*** interactive wxDateTime tests ***"));

    wxChar buf[128];

    for ( ;; )
    {
        wxPrintf(wxT("Enter a date (press ENTER or type 'quit' to escape): "));
        if ( !wxFgets(buf, WXSIZEOF(buf), stdin) )
            break;

        // kill the last '\n'
        buf[wxStrlen(buf) - 1] = 0;

        if ( buf[0] == '\0' || wxStrcmp(buf, "quit") == 0 )
            break;

        wxDateTime dt;
        const wxChar *p = dt.ParseDate(buf);
        if ( !p )
        {
            wxPrintf(wxT("ERROR: failed to parse the date '%s'.\n"), buf);

            continue;
        }
        else if ( *p )
        {
            wxPrintf(wxT("WARNING: parsed only first %u characters.\n"), p - buf);
        }

        wxPrintf(wxT("%s: day %u, week of month %u/%u, week of year %u\n"),
                 dt.Format(wxT("%b %d, %Y")),
                 dt.GetDayOfYear(),
                 dt.GetWeekOfMonth(wxDateTime::Monday_First),
                 dt.GetWeekOfMonth(wxDateTime::Sunday_First),
                 dt.GetWeekOfYear(wxDateTime::Monday_First));
    }

    wxPuts("\n");
#endif // TEST_DATETIME
}


// ----------------------------------------------------------------------------
// single instance
// ----------------------------------------------------------------------------

#include "wx/snglinst.h"

void InteractiveInputTestCase::TestSingleIstance()
{
#ifdef TEST_SNGLINST
    wxPuts(wxT("\n*** Testing wxSingleInstanceChecker ***"));

    wxSingleInstanceChecker checker;
    if ( checker.Create(wxT(".wxconsole.lock")) )
    {
        if ( checker.IsAnotherRunning() )
        {
            wxPrintf(wxT("Another instance of the program is running, exiting.\n"));
            return;
        }

        // wait some time to give time to launch another instance
        wxPuts(wxT("If you try to run another instance of this program now, it won't start."));
        wxPrintf(wxT("Press \"Enter\" to exit wxSingleInstanceChecker test and proceed..."));
        wxFgetc(stdin);
    }
    else // failed to create
    {
        wxPrintf(wxT("Failed to init wxSingleInstanceChecker.\n"));
    }

    wxPuts("\n");
#endif // defined(TEST_SNGLINST)
}

