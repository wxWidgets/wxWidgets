/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/bench.cpp
// Purpose:     Main file of the benchmarking suite
// Author:      Vadim Zeitlin
// Created:     2008-07-19
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <float.h>

#include "wx/app.h"
#include "wx/cmdline.h"
#include "wx/stopwatch.h"

#if wxUSE_GUI
    #include "wx/frame.h"
#endif

#include "bench.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const char OPTION_LIST = 'l';
static const char OPTION_SINGLE = '1';

static const char OPTION_RUN_TIME = 't';
static const char OPTION_NUM_RUNS = 'n';
static const char OPTION_NUMERIC_PARAM = 'p';
static const char OPTION_STRING_PARAM = 's';

// ----------------------------------------------------------------------------
// BenchApp declaration
// ----------------------------------------------------------------------------

#if wxUSE_GUI
    typedef wxApp BenchAppBase;
#else
    typedef wxAppConsole BenchAppBase;
#endif

class BenchApp : public BenchAppBase
{
public:
    BenchApp();

    // standard overrides
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
    virtual bool OnInit();
    virtual int  OnRun();
    virtual int  OnExit();

    // accessors
    int GetNumericParameter() const { return m_numParam; }
    const wxString& GetStringParameter() const { return m_strParam; }

private:
    // output the results of a single benchmark if successful or just return
    // false if anything went wrong
    bool RunSingleBenchmark(Bench::Function* func);

    // list all registered benchmarks
    void ListBenchmarks();

    // command lines options/parameters
    wxSortedArrayString m_toRun;
    long m_numRuns, // number of times to run a single benchmark or 0
         m_runTime, // minimum time to run a single benchmark if m_numRuns == 0
         m_numParam;
    wxString m_strParam;
};

wxIMPLEMENT_APP_CONSOLE(BenchApp);

// ============================================================================
// Bench namespace symbols implementation
// ============================================================================

Bench::Function *Bench::Function::ms_head = NULL;

long Bench::GetNumericParameter(long defVal)
{
    const long val = wxGetApp().GetNumericParameter();
    return val ? val : defVal;
}

wxString Bench::GetStringParameter(const wxString& defVal)
{
    const wxString& val = wxGetApp().GetStringParameter();
    return !val.empty() ? val : defVal;
}

// ============================================================================
// BenchApp implementation
// ============================================================================

BenchApp::BenchApp()
{
    m_numRuns = 0; // this means to use m_runTime
    m_runTime = 500; // default minimum
    m_numParam = 0;
}

bool BenchApp::OnInit()
{
    if ( !BenchAppBase::OnInit() )
        return false;

    wxPrintf("wxWidgets benchmarking program\n"
             "Build: %s\n", WX_BUILD_OPTIONS_SIGNATURE);

#if wxUSE_GUI
    // create a hidden parent window to be used as parent for the GUI controls
    new wxFrame(NULL, wxID_ANY, "Hidden wx benchmark frame");
#endif // wxUSE_GUI

    return true;
}

void BenchApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    BenchAppBase::OnInitCmdLine(parser);

    parser.AddSwitch(OPTION_LIST,
                     "list",
                     "list all the existing benchmarks");

    parser.AddSwitch(OPTION_SINGLE,
                     "single",
                     "run the benchmark once only");

    parser.AddOption(OPTION_RUN_TIME,
                     "run-time",
                     wxString::Format
                     (
                        "maximum time to run each benchmark in ms "
                        "(default: %ld, set to 0 to disable)",
                        m_runTime
                     ),
                     wxCMD_LINE_VAL_NUMBER);
    parser.AddOption(OPTION_NUM_RUNS,
                     "num-runs",
                     wxString::Format
                     (
                         "number of times to run each benchmark in a loop "
                         "(default: %ld, 0 means to run until max time passes)",
                         m_numRuns
                     ),
                     wxCMD_LINE_VAL_NUMBER);
    parser.AddOption(OPTION_NUMERIC_PARAM,
                     "num-param",
                     wxString::Format
                     (
                         "numeric parameter used by some benchmark functions "
                         "(default: %ld)",
                         m_numParam
                     ),
                     wxCMD_LINE_VAL_NUMBER);
    parser.AddOption(OPTION_STRING_PARAM,
                     "str-param",
                     "string parameter used by some benchmark functions "
                     "(default: empty)",
                     wxCMD_LINE_VAL_STRING);

    parser.AddParam("benchmark name",
                    wxCMD_LINE_VAL_STRING,
                    wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE);
}

bool BenchApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if ( parser.Found(OPTION_LIST) )
    {
        ListBenchmarks();

        return false;
    }

    const size_t count = parser.GetParamCount();
    if ( !count )
    {
        parser.Usage();

        ListBenchmarks();

        return false;
    }

    const bool runTimeSpecified = parser.Found(OPTION_RUN_TIME, &m_runTime);
    const bool numRunsSpecified = parser.Found(OPTION_NUM_RUNS, &m_numRuns);
    parser.Found(OPTION_NUMERIC_PARAM, &m_numParam);
    parser.Found(OPTION_STRING_PARAM, &m_strParam);
    if ( parser.Found(OPTION_SINGLE) )
    {
        if ( runTimeSpecified || numRunsSpecified )
        {
            wxFprintf(stderr, "Incompatible options specified.\n");

            return false;
        }

        m_numRuns = 1;
    }
    else if ( numRunsSpecified && !runTimeSpecified )
    {
        // If only the number of runs is specified, use it only.
        m_runTime = 0;
    }

    // construct sorted array for quick verification of benchmark names
    wxSortedArrayString benchmarks;
    for ( Bench::Function *func = Bench::Function::GetFirst();
          func;
          func = func->GetNext() )
    {
        benchmarks.push_back(func->GetName());
    }

    for ( size_t n = 0; n < count; n++ )
    {
        const wxString name = parser.GetParam(n);
        if ( benchmarks.Index(name) == wxNOT_FOUND )
        {
            wxFprintf(stderr, "No benchmark named \"%s\".\n", name);
            return false;
        }

        m_toRun.push_back(name);
    }

    return BenchAppBase::OnCmdLineParsed(parser);
}

int BenchApp::OnRun()
{
    int rc = EXIT_SUCCESS;

    wxString params;
    if ( m_numParam )
        params += wxString::Format("N=%ld", m_numParam);
    if ( !m_strParam.empty() )
    {
        if ( !params.empty() )
            params += " and ";
        params += wxString::Format("s=\"%s\"", m_strParam);
    }

    if ( !params.empty() )
        wxPrintf("Benchmarks are running with non-default %s\n", params);

    for ( Bench::Function *func = Bench::Function::GetFirst();
          func;
          func = func->GetNext() )
    {
        if ( m_toRun.Index(func->GetName()) == wxNOT_FOUND )
            continue;

        if ( !RunSingleBenchmark(func) )
        {
            wxFprintf(stderr, "ERROR running %s\n", func->GetName());
            rc = EXIT_FAILURE;
        }
    }

    return rc;
}

bool BenchApp::RunSingleBenchmark(Bench::Function* func)
{
    if ( !func->Init() )
        return false;

    wxPrintf("Benchmarking %s: ", func->GetName());
    fflush(stdout);

    // We use the algorithm for iteratively computing the mean and the
    // standard deviation of the sequence of values described in Knuth's
    // "The Art of Computer Programming, Volume 2: Seminumerical
    // Algorithms", section 4.2.2.
    //
    // The algorithm defines the sequences M(k) and S(k) as follows:
    //
    //  M(1) = x(1), M(k) = M(k-1) + (x(k) - M(k-1)) / k
    //  S(1) = 0,    S(k) = S(k-1) + (x(k) - M(k-1))*(x(k) - M(k))
    //
    // where x(k) is the k-th value. Then the mean is simply the last value
    // of the first sequence M(N) and the standard deviation is
    // sqrt(S(N)/(N-1)).

    wxStopWatch swTotal;
    if ( !func->Run() )
        return false;

    double timeMin = DBL_MAX,
           timeMax = 0;

    double m = swTotal.TimeInMicro().ToDouble();
    double s = 0;

    long n = 0;
    for ( ;; )
    {
        // One termination condition is reaching the maximum number of runs.
        if ( ++n == m_numRuns )
            break;

        double t;
        {
            wxStopWatch swThis;
            if ( !func->Run() )
                return false;

            t = swThis.TimeInMicro().ToDouble();
        }

        if ( t < timeMin )
            timeMin = t;
        if ( t > timeMax )
            timeMax = t;

        const double lastM = m;
        m += (t - lastM) / n;
        s += (t - lastM)*(t - m);

        // The other termination condition is that we are running for at least
        // m_runTime milliseconds.
        if ( m_runTime && swTotal.Time() >= m_runTime )
            break;
    }

    func->Done();

    // For a single run there is no standard deviation and min/max don't make
    // much sense.
    if ( n == 1 )
    {
        wxPrintf("single run took %.0fus\n", m);
    }
    else
    {
        s = sqrt(s / (n - 1));

        wxPrintf
        (
            "%ld runs, %.0fus avg, %.0f std dev (%.0f/%.0f min/max)\n",
            n, m, s, timeMin, timeMax
        );
    }

    fflush(stdout);

    return true;
}

int BenchApp::OnExit()
{
#if wxUSE_GUI
    delete GetTopWindow();
#endif // wxUSE_GUI

    return 0;
}

/* static */
void BenchApp::ListBenchmarks()
{
    wxPrintf("Available benchmarks:\n");
    for ( Bench::Function *func = Bench::Function::GetFirst();
          func;
          func = func->GetNext() )
    {
        wxPrintf("\t%s\n", func->GetName());
    }
}
