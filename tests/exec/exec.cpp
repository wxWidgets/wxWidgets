///////////////////////////////////////////////////////////////////////////////
// Name:        tests/exec/exec.cpp
// Purpose:     test wxExecute()
// Author:      Francesco Montorsi
//              (based on console sample TestExecute() function)
// Created:     2009-01-10
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Francesco Montorsi
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/utils.h"
#include "wx/process.h"
#include "wx/sstream.h"

#ifdef __UNIX__
    #define COMMAND "echo hi"
    #define ASYNC_COMMAND "xclock"
    #define SHELL_COMMAND "echo hi from shell>/dev/null"
    #define COMMAND_NO_OUTPUT "echo -n"
#elif defined(__WXMSW__)
    #define COMMAND "cmd.exe /c \"echo hi\""
    #define ASYNC_COMMAND "notepad"
    #define SHELL_COMMAND "echo hi > nul:"
    #define COMMAND_NO_OUTPUT COMMAND " > nul:"
#else
    #error "no command to exec"
#endif // OS

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ExecTestCase : public CppUnit::TestCase
{
public:
    ExecTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ExecTestCase );
        CPPUNIT_TEST( TestShell );
        CPPUNIT_TEST( TestExecute );
        CPPUNIT_TEST( TestProcess );
    CPPUNIT_TEST_SUITE_END();

    void TestShell();
    void TestExecute();
    void TestProcess();

    DECLARE_NO_COPY_CLASS(ExecTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ExecTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ExecTestCase, "ExecTestCase" );


void ExecTestCase::TestShell()
{
    CPPUNIT_ASSERT( wxShell(SHELL_COMMAND) );
}

void ExecTestCase::TestExecute()
{
    // test sync exec (with a command not producing any output to avoid
    // interfering with the test):
    CPPUNIT_ASSERT( wxExecute(COMMAND_NO_OUTPUT, wxEXEC_SYNC) == 0 );

    // test asynch exec
    long pid = wxExecute(ASYNC_COMMAND, wxEXEC_ASYNC);
    CPPUNIT_ASSERT( pid != 0 );

    // NOTE: under Windows the first wxKill() invocation with wxSIGTERM
    //       may fail if the system is fast and the ASYNC_COMMAND app
    //       doesn't manage to create its HWND before our wxKill() is
    //       executed; in that case we "fall back" to the second invocation
    //       with wxSIGKILL (which should always succeed)
    CPPUNIT_ASSERT( wxKill(pid, wxSIGTERM) == 0 ||
                    wxKill(pid, wxSIGKILL) == 0 );

    // test running COMMAND again, but this time with redirection:
    wxArrayString stdout_arr;
    CPPUNIT_ASSERT_EQUAL( 0, wxExecute(COMMAND, stdout_arr, wxEXEC_SYNC) );
    CPPUNIT_ASSERT_EQUAL( "hi", stdout_arr[0] );
}

void ExecTestCase::TestProcess()
{
    // test wxExecute with wxProcess
    wxProcess *proc = new wxProcess;
    long pid = wxExecute(ASYNC_COMMAND, wxEXEC_ASYNC, proc);
    CPPUNIT_ASSERT( proc->GetPid() == pid && pid != 0 );

    // we're not going to process the wxEVT_END_PROCESS event,
    // so the proc instance will auto-delete itself after we kill
    // the asynch process:
    CPPUNIT_ASSERT( wxKill(pid, wxSIGTERM) == 0 ||
                    wxKill(pid, wxSIGKILL) == 0 );


    // test wxExecute with wxProcess and REDIRECTION
    wxProcess *proc2 = new wxProcess;
    proc2->Redirect();
    CPPUNIT_ASSERT( wxExecute(COMMAND, wxEXEC_SYNC, proc2) == 0 );

    wxStringOutputStream stdout_stream;
    CPPUNIT_ASSERT( proc2->GetInputStream() );
    CPPUNIT_ASSERT_EQUAL( wxSTREAM_EOF,
        proc2->GetInputStream()->Read(stdout_stream).GetLastError() );

    wxString str(stdout_stream.GetString());
    CPPUNIT_ASSERT_EQUAL( "hi", str.Trim() );
}

