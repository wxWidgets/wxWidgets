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
    void List(Test *test, const string& parent = "") const;

    // command lines options/parameters
    bool m_list;
    bool m_longlist;
    vector<string> m_registries;
};

IMPLEMENT_APP_CONSOLE(TestApp)

TestApp::TestApp()
  : m_list(false),
    m_longlist(false)
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
            _T("list the test suites, do not run them"),
            wxCMD_LINE_VAL_NONE, 0 },
        { wxCMD_LINE_SWITCH, _T("L"), _T("longlist"),
            _T("list the test cases, do not run them"),
            wxCMD_LINE_VAL_NONE, 0 },
        { wxCMD_LINE_PARAM, 0, 0, _T("REGISTRY"), wxCMD_LINE_VAL_STRING,
            wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
        { wxCMD_LINE_NONE , 0, 0, 0, wxCMD_LINE_VAL_NONE, 0 }
    };

    parser.SetDesc(cmdLineDesc);
}

// Handle command line options
//
bool TestApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (parser.GetParamCount())
        for (size_t i = 0; i < parser.GetParamCount(); i++)
            m_registries.push_back(string(parser.GetParam(i).mb_str()));
    else
        m_registries.push_back("");
 
    m_longlist = parser.Found(_T("longlist"));
    m_list = m_longlist || parser.Found(_T("list"));

    return wxAppConsole::OnCmdLineParsed(parser);
}

// Run
//
int TestApp::OnRun()
{
    TextUi::TestRunner runner;

    for (size_t i = 0; i < m_registries.size(); i++) {
        auto_ptr<Test> test(m_registries[i].empty() ?
            TestFactoryRegistry::getRegistry().makeTest() :
            TestFactoryRegistry::getRegistry(m_registries[i]).makeTest());

        TestSuite *suite = dynamic_cast<TestSuite*>(test.get());

        if (suite && suite->countTestCases() == 0)
            wxLogError(_T("No such test suite: %s"),
                wxString(m_registries[i].c_str(), wxConvUTF8).c_str());
        else if (m_list)
            List(test.get());
        else
            runner.addTest(test.release());
    }

#if wxUSE_LOG
    // Switch off logging unless --verbose
    wxLog::EnableLogging(wxLog::GetVerbose());
#endif // wxUSE_LOG

    return m_list || runner.run("", false, true, 
#if wxUSE_LOG
        !wxLog::GetVerbose()
#else
        true
#endif // wxUSE_LOG
    ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// List the tests
//
void TestApp::List(Test *test, const string& parent /*=""*/) const
{
    TestSuite *suite = dynamic_cast<TestSuite*>(test);
    string name;

    if (suite) {
        // take the last component of the name and append to the parent
        name = test->getName();
        string::size_type i = name.find_last_of(".:");
        name = parent + "." + (i != string::npos ? name.substr(i + 1) : name);

        // drop the 1st component from the display and indent
        if (parent != "") {
            string::size_type j = i = name.find('.', 1);
            while ((j = name.find('.', j + 1)) != string::npos)
                cout << "  ";
            cout << "  " << name.substr(i + 1) << "\n";
        }

        typedef vector<Test*> Tests;
        typedef Tests::const_iterator Iter;

        const Tests& tests = suite->getTests();

        for (Iter it = tests.begin(); it != tests.end(); ++it)
            List(*it, name);
    }
    else if (m_longlist) {
        string::size_type i = 0;
        while ((i = parent.find('.', i + 1)) != string::npos)
            cout << "  ";
        cout << "  " << test->getName() << "\n";
    }
}
