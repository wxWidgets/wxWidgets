///////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     Test program for wxWidgets
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation
    #pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/cmdline.h"
#include "wx/cppunit.h"
#include <iostream>

using namespace std;
using namespace CppUnit;

// The application class
//
class TestApp : public wxAppConsole
{
public:
    TestApp();

    // standard overrides
    void OnInitCmdLine(wxCmdLineParser& parser);
    bool OnCmdLineParsed(wxCmdLineParser& parser);
    bool OnInit();
    int  OnRun();

private:
    void List(Test *test, int depth = 0) const;

    // command lines options/parameters
    bool m_list;
    string m_registry;
};

IMPLEMENT_APP_CONSOLE(TestApp)

TestApp::TestApp()
  : m_list(false)
{
}

// Init
//
bool TestApp::OnInit()
{
    cout << "Test program for wxWidgets\n"
         << "build: " << WX_BUILD_OPTIONS_SIGNATURE << endl;
    return wxAppConsole::OnInit();
};

// The table of command line options
//
void TestApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxAppConsole::OnInitCmdLine(parser);

    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        { wxCMD_LINE_SWITCH, _T("l"), _T("list"),
            _T("list the tests, do not run them"),
            wxCMD_LINE_VAL_NONE, 0 },
        { wxCMD_LINE_PARAM, 0, 0, _T("REGISTRY"), wxCMD_LINE_VAL_STRING,
            wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_NONE , 0, 0, 0, wxCMD_LINE_VAL_NONE, 0 }
    };

    parser.SetDesc(cmdLineDesc);
}

// Handle command line options
//
bool TestApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (parser.GetParamCount() > 0)
        m_registry = parser.GetParam(0).mb_str();
 
    m_list = parser.Found(_T("list"));

    return wxAppConsole::OnCmdLineParsed(parser);
}

// Run
//
int TestApp::OnRun()
{
    Test *test = m_registry.empty()?
        TestFactoryRegistry::getRegistry().makeTest() :
        TestFactoryRegistry::getRegistry(m_registry).makeTest();

    if (m_list) {
        List(test);
        return EXIT_SUCCESS;
    } else {
        TextUi::TestRunner runner;
        runner.addTest(test);
        return runner.run("", false, true, false) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
}

// List the tests
//
void TestApp::List(Test *test, int depth /*=0*/) const
{
    cout << string(depth * 2, ' ') << test->getName() << "\n";

    TestSuite *suite = dynamic_cast<TestSuite*>(test);

    if (suite) {
        typedef const std::vector<Test*> Tests;
        typedef Tests::const_iterator Iter;

        Tests& tests = suite->getTests();

        for (Iter it = tests.begin(); it != tests.end(); ++it)
            List(*it, depth + 1);
    }
}
