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
    #define SHELL_COMMAND "echo hi from shell"
    #define REDIRECT_COMMAND "cat -n Makefile"
#elif defined(__WXMSW__)
    #define COMMAND "cmd.exe /c \"echo hi\""
    #define ASYNC_COMMAND "notepad"
    #define SHELL_COMMAND "echo hi"
    #define REDIRECT_COMMAND COMMAND
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

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ExecTestCase, "ExecTestCase" );


void ExecTestCase::TestShell()
{
    CPPUNIT_ASSERT( wxShell(SHELL_COMMAND) );
}

void ExecTestCase::TestExecute()
{
    // test sync exec:
    CPPUNIT_ASSERT( wxExecute(COMMAND, wxEXEC_SYNC) == 0 );

    // test asynch exec
    long pid = wxExecute(ASYNC_COMMAND, wxEXEC_ASYNC);
    CPPUNIT_ASSERT( pid != 0 );
    CPPUNIT_ASSERT( wxKill(pid) == 0 );

    // test running COMMAND again, but this time with redirection:
    wxArrayString stdout;
    CPPUNIT_ASSERT( wxExecute(COMMAND, stdout, wxEXEC_SYNC) == 0 );
    CPPUNIT_ASSERT( stdout[0] == "hi" );
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
    CPPUNIT_ASSERT( wxKill(pid) == 0 );

    
    // test wxExecute with wxProcess and REDIRECTION
    wxProcess *proc2 = new wxProcess;
    proc2->Redirect();
    CPPUNIT_ASSERT( wxExecute(COMMAND, wxEXEC_SYNC, proc2) == 0 );
    
    wxStringOutputStream stdout;
    CPPUNIT_ASSERT( proc2->GetInputStream() );
    CPPUNIT_ASSERT( proc2->GetInputStream()->Read(stdout).GetLastError() == wxSTREAM_EOF );
    
    wxString str(stdout.GetString());
    CPPUNIT_ASSERT_EQUAL( "hi", str.Trim() );
}

