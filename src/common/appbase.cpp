///////////////////////////////////////////////////////////////////////////////
// Name:        common/base/appbase.cpp
// Purpose:     implements wxAppConsole class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.06.2003 (extracted from common/appcmn.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/list.h"
    #if wxUSE_LOG
        #include "wx/log.h"
    #endif // wxUSE_LOG
#endif //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/cmdline.h"
#include "wx/confbase.h"
#if wxUSE_FONTMAP
    #include "wx/fontmap.h"
#endif // wxUSE_FONTMAP
#include "wx/msgout.h"
#include "wx/tokenzr.h"

#if !defined(__WXMSW__) || defined(__WXMICROWIN__)
  #include  <signal.h>      // for SIGTRAP used by wxTrap()
#endif  //Win/Unix

#if defined(__WXMSW__)
  #include  "wx/msw/private.h"  // includes windows.h for MessageBox()
#endif

#if defined(__WXMAC__)
    // VZ: MacTypes.h is enough under Mac OS X (where I could test it) but
    //     I don't know which headers are needed under earlier systems so
    //     include everything when in doubt
    #ifdef __DARWIN__
        #include  "MacTypes.h"
    #else
        #include  "wx/mac/private.h"  // includes mac headers
    #endif
#endif // __WXMAC__

// ----------------------------------------------------------------------------
// private functions prototypes
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__
    // really just show the assert dialog
    static bool DoShowAssertDialog(const wxString& msg);

    // prepare for showing the assert dialog, use the given traits or
    // DoShowAssertDialog() as last fallback to really show it
    static
    void ShowAssertDialog(const wxChar *szFile,
                          int nLine,
                          const wxChar *szCond,
                          const wxChar *szMsg,
                          wxAppTraits *traits = NULL);

    // turn on the trace masks specified in the env variable WXTRACE
    static void LINKAGEMODE SetTraceMasks();
#endif // __WXDEBUG__

// ----------------------------------------------------------------------------
// global vars
// ----------------------------------------------------------------------------

wxApp *wxTheApp = NULL;

wxAppInitializerFunction wxAppConsole::ms_appInitFn = NULL;

// ============================================================================
// wxAppConsole implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxAppConsole::wxAppConsole()
{
    m_traits = NULL;

    wxTheApp = (wxApp *)this;

#ifdef __WXDEBUG__
    SetTraceMasks();
#endif
}

wxAppConsole::~wxAppConsole()
{
    delete m_traits;
}

// ----------------------------------------------------------------------------
// OnXXX() callbacks
// ----------------------------------------------------------------------------

bool wxAppConsole::OnInit()
{
#if wxUSE_CMDLINE_PARSER
    wxCmdLineParser parser(argc, argv);

    OnInitCmdLine(parser);

    bool cont;
    switch ( parser.Parse(FALSE /* don't show usage */) )
    {
        case -1:
            cont = OnCmdLineHelp(parser);
            break;

        case 0:
            cont = OnCmdLineParsed(parser);
            break;

        default:
            cont = OnCmdLineError(parser);
            break;
    }

    if ( !cont )
        return FALSE;
#endif // wxUSE_CMDLINE_PARSER

    return TRUE;
}

int wxAppConsole::OnExit()
{
#if wxUSE_CONFIG
    // delete the config object if any (don't use Get() here, but Set()
    // because Get() could create a new config object)
    delete wxConfigBase::Set((wxConfigBase *) NULL);
#endif // wxUSE_CONFIG

#ifdef __WXUNIVERSAL__
    delete wxTheme::Set(NULL);
#endif // __WXUNIVERSAL__

    // use Set(NULL) and not Get() to avoid creating a message output object on
    // demand when we just want to delete it
    delete wxMessageOutput::Set(NULL);

    return 0;
}

void wxAppConsole::Exit()
{
    exit(-1);
}

// ----------------------------------------------------------------------------
// traits stuff
// ----------------------------------------------------------------------------

wxAppTraits *wxAppConsole::CreateTraits()
{
    return wxAppTraits::CreateConsole();
}

wxAppTraits *wxAppConsole::GetTraits()
{
    // FIXME-MT: protect this with a CS?
    if ( !m_traits )
    {
        m_traits = CreateTraits();

        wxASSERT_MSG( m_traits, _T("wxApp::CreateTraits() failed?") );
    }

    return m_traits;
}

// we must implement CreateXXX() in wxApp itself for backwards compatibility
#if WXWIN_COMPATIBILITY_2_4

#if wxUSE_LOG

wxLog *wxAppConsole::CreateLogTarget()
{
    wxAppTraits *traits = GetTraits();
    return traits ? traits->CreateLogTarget() : NULL;
}

#endif // wxUSE_LOG

wxMessageOutput *wxAppConsole::CreateMessageOutput()
{
    wxAppTraits *traits = GetTraits();
    return traits ? traits->CreateMessageOutput() : NULL;
}

#endif // WXWIN_COMPATIBILITY_2_4

// ----------------------------------------------------------------------------
// event processing
// ----------------------------------------------------------------------------

void wxAppConsole::ProcessPendingEvents()
{
    // ensure that we're the only thread to modify the pending events list
    wxENTER_CRIT_SECT( *wxPendingEventsLocker );

    if ( !wxPendingEvents )
    {
        wxLEAVE_CRIT_SECT( *wxPendingEventsLocker );
        return;
    }

    // iterate until the list becomes empty
    wxNode *node = wxPendingEvents->GetFirst();
    while (node)
    {
        wxEvtHandler *handler = (wxEvtHandler *)node->GetData();
        delete node;

        // In ProcessPendingEvents(), new handlers might be add
        // and we can safely leave the critical section here.
        wxLEAVE_CRIT_SECT( *wxPendingEventsLocker );
        handler->ProcessPendingEvents();
        wxENTER_CRIT_SECT( *wxPendingEventsLocker );

        node = wxPendingEvents->GetFirst();
    }

    wxLEAVE_CRIT_SECT( *wxPendingEventsLocker );
}

int wxAppConsole::FilterEvent(wxEvent& WXUNUSED(event))
{
    // process the events normally by default
    return -1;
}

// ----------------------------------------------------------------------------
// cmd line parsing
// ----------------------------------------------------------------------------

#if wxUSE_CMDLINE_PARSER

#define OPTION_VERBOSE _T("verbose")
#define OPTION_THEME   _T("theme")
#define OPTION_MODE    _T("mode")

void wxAppConsole::OnInitCmdLine(wxCmdLineParser& parser)
{
    // the standard command line options
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        {
            wxCMD_LINE_SWITCH,
            _T("h"),
            _T("help"),
            gettext_noop("show this help message"),
            wxCMD_LINE_VAL_NONE,
            wxCMD_LINE_OPTION_HELP
        },

#if wxUSE_LOG
        {
            wxCMD_LINE_SWITCH,
            _T(""),
            OPTION_VERBOSE,
            gettext_noop("generate verbose log messages"),
            wxCMD_LINE_VAL_NONE,
            0x0
        },
#endif // wxUSE_LOG

#ifdef __WXUNIVERSAL__
        {
            wxCMD_LINE_OPTION,
            _T(""),
            OPTION_THEME,
            gettext_noop("specify the theme to use"),
            wxCMD_LINE_VAL_STRING,
            0x0
        },
#endif // __WXUNIVERSAL__

#if defined(__WXMGL__)
        // VS: this is not specific to wxMGL, all fullscreen (framebuffer) ports
        //     should provide this option. That's why it is in common/appcmn.cpp
        //     and not mgl/app.cpp
        {
            wxCMD_LINE_OPTION,
            _T(""),
            OPTION_MODE,
            gettext_noop("specify display mode to use (e.g. 640x480-16)"),
            wxCMD_LINE_VAL_STRING,
            0x0
        },
#endif // __WXMGL__

        // terminator
        {
            wxCMD_LINE_NONE,
            _T(""),
            _T(""),
            _T(""),
            wxCMD_LINE_VAL_NONE,
            0x0
        }
    };

    parser.SetDesc(cmdLineDesc);
}

bool wxAppConsole::OnCmdLineParsed(wxCmdLineParser& parser)
{
#if wxUSE_LOG
    if ( parser.Found(OPTION_VERBOSE) )
    {
        wxLog::SetVerbose(TRUE);
    }
#endif // wxUSE_LOG

#ifdef __WXUNIVERSAL__
    wxString themeName;
    if ( parser.Found(OPTION_THEME, &themeName) )
    {
        wxTheme *theme = wxTheme::Create(themeName);
        if ( !theme )
        {
            wxLogError(_("Unsupported theme '%s'."), themeName.c_str());
            return FALSE;
        }

        // Delete the defaultly created theme and set the new theme.
        delete wxTheme::Get();
        wxTheme::Set(theme);
    }
#endif // __WXUNIVERSAL__

#if defined(__WXMGL__)
    wxString modeDesc;
    if ( parser.Found(OPTION_MODE, &modeDesc) )
    {
        unsigned w, h, bpp;
        if ( wxSscanf(modeDesc.c_str(), _T("%ux%u-%u"), &w, &h, &bpp) != 3 )
        {
            wxLogError(_("Invalid display mode specification '%s'."), modeDesc.c_str());
            return FALSE;
        }

        if ( !SetDisplayMode(wxDisplayModeInfo(w, h, bpp)) )
            return FALSE;
    }
#endif // __WXMGL__

    return TRUE;
}

bool wxAppConsole::OnCmdLineHelp(wxCmdLineParser& parser)
{
    parser.Usage();

    return FALSE;
}

bool wxAppConsole::OnCmdLineError(wxCmdLineParser& parser)
{
    parser.Usage();

    return FALSE;
}

#endif // wxUSE_CMDLINE_PARSER

// ----------------------------------------------------------------------------
// debugging support
// ----------------------------------------------------------------------------

/* static */
bool wxAppConsole::CheckBuildOptions(const wxBuildOptions& opts)
{
#define wxCMP(what)   (what == opts.m_ ## what)

    bool
#ifdef __WXDEBUG__
    isDebug = TRUE;
#else
    isDebug = FALSE;
#endif

    int verMaj = wxMAJOR_VERSION,
        verMin = wxMINOR_VERSION;

    if ( !(wxCMP(isDebug) && wxCMP(verMaj) && wxCMP(verMin)) )
    {
        wxString msg;
        wxString libDebug, progDebug;

        if (isDebug)
            libDebug = wxT("debug");
        else
            libDebug = wxT("no debug");

        if (opts.m_isDebug)
            progDebug = wxT("debug");
        else
            progDebug = wxT("no debug");
        
        msg.Printf(_T("Mismatch between the program and library build versions detected.\nThe library used %d.%d (%s), and your program used %d.%d (%s)."),
                   verMaj, verMin, libDebug.c_str(), opts.m_verMaj, opts.m_verMin, progDebug.c_str());
        
        wxLogFatalError(msg);

        // normally wxLogFatalError doesn't return
        return FALSE;
    }
#undef wxCMP

    return TRUE;
}

#ifdef __WXDEBUG__

void wxAppConsole::OnAssert(const wxChar *file,
                            int line,
                            const wxChar *cond,
                            const wxChar *msg)
{
    ShowAssertDialog(file, line, cond, msg, m_traits);
}

#endif // __WXDEBUG__

// ============================================================================
// other classes implementations
// ============================================================================

// ----------------------------------------------------------------------------
// wxConsoleAppTraitsBase
// ----------------------------------------------------------------------------

#if wxUSE_LOG

wxLog *wxConsoleAppTraitsBase::CreateLogTarget()
{
    return new wxLogStderr;
}

#endif // wxUSE_LOG

wxMessageOutput *wxConsoleAppTraitsBase::CreateMessageOutput()
{
    return new wxMessageOutputStderr;
}

#if wxUSE_FONTMAP

wxFontMapper *wxConsoleAppTraitsBase::CreateFontMapper()
{
    return (wxFontMapper *)new wxFontMapperBase;
}

#endif // wxUSE_FONTMAP

#ifdef __WXDEBUG__
bool wxConsoleAppTraitsBase::ShowAssertDialog(const wxString& msg)
{
    return wxAppTraitsBase::ShowAssertDialog(msg);
}
#endif

bool wxConsoleAppTraitsBase::HasStderr()
{
    // console applications always have stderr, even under Mac/Windows
    return true;
}

void wxConsoleAppTraitsBase::ScheduleForDestroy(wxObject *object)
{
    delete object;
}

void wxConsoleAppTraitsBase::RemoveFromPendingDelete(wxObject * WXUNUSED(object))
{
    // nothing to do
}

// ----------------------------------------------------------------------------
// wxAppTraits
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__

bool wxAppTraitsBase::ShowAssertDialog(const wxString& msg)
{
    return DoShowAssertDialog(msg);
}

#endif // __WXDEBUG__

wxAppTraits *wxAppTraitsBase::CreateConsole()
{
    return new wxConsoleAppTraits;
}

// ============================================================================
// global functions implementation
// ============================================================================

void wxExit()
{
    if ( wxTheApp )
    {
        wxTheApp->Exit();
    }
    else
    {
        // what else can we do?
        exit(-1);
    }
}

void wxWakeUpIdle()
{
    if ( wxTheApp )
    {
        wxTheApp->WakeUpIdle();
    }
    //else: do nothing, what can we do?
}

#ifdef  __WXDEBUG__

// wxASSERT() helper
bool wxAssertIsEqual(int x, int y)
{
    return x == y;
}

// break into the debugger
void wxTrap()
{
#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
    DebugBreak();
#elif defined(__WXMAC__) && !defined(__DARWIN__)
    #if __powerc
        Debugger();
    #else
        SysBreak();
    #endif
#elif defined(__UNIX__)
    raise(SIGTRAP);
#else
    // TODO
#endif // Win/Unix
}

void wxAssert(int cond,
              const wxChar *szFile,
              int nLine,
              const wxChar *szCond,
              const wxChar *szMsg) 
{
    if ( !cond )
        wxOnAssert(szFile, nLine, szCond, szMsg);
}

// this function is called when an assert fails
void wxOnAssert(const wxChar *szFile,
                int nLine,
                const wxChar *szCond,
                const wxChar *szMsg)
{
    // FIXME MT-unsafe
    static bool s_bInAssert = FALSE;

    if ( s_bInAssert )
    {
        // He-e-e-e-elp!! we're trapped in endless loop
        wxTrap();

        s_bInAssert = FALSE;

        return;
    }

    s_bInAssert = TRUE;

    if ( !wxTheApp )
    {
        // by default, show the assert dialog box -- we can't customize this
        // behaviour
        ShowAssertDialog(szFile, nLine, szCond, szMsg);
    }
    else
    {
        // let the app process it as it wants
        wxTheApp->OnAssert(szFile, nLine, szCond, szMsg);
    }

    s_bInAssert = FALSE;
}

#endif // __WXDEBUG__

// ============================================================================
// private functions implementation
// ============================================================================

#ifdef __WXDEBUG__

static void LINKAGEMODE SetTraceMasks()
{
#if wxUSE_LOG
    wxString mask;
    if ( wxGetEnv(wxT("WXTRACE"), &mask) )
    {
        wxStringTokenizer tkn(mask, wxT(",;:"));
        while ( tkn.HasMoreTokens() )
            wxLog::AddTraceMask(tkn.GetNextToken());
    }
#endif // wxUSE_LOG
}

bool DoShowAssertDialog(const wxString& msg)
{
    // under MSW we can show the dialog even in the console mode
#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
    wxString msgDlg(msg);

    // this message is intentionally not translated -- it is for
    // developpers only
    msgDlg += wxT("\nDo you want to stop the program?\n")
              wxT("You can also choose [Cancel] to suppress ")
              wxT("further warnings.");

    switch ( ::MessageBox(NULL, msgDlg, _T("wxWindows Debug Alert"),
                          MB_YESNOCANCEL | MB_ICONSTOP ) )
    {
        case IDYES:
            wxTrap();
            break;

        case IDCANCEL:
            // stop the asserts
            return true;

        //case IDNO: nothing to do
    }
#else // !__WXMSW__
    wxFprintf(stderr, wxT("%s\n"), msg.c_str());
    fflush(stderr);

    // TODO: ask the user to enter "Y" or "N" on the console?
    wxTrap();
#endif // __WXMSW__/!__WXMSW__

    // continue with the asserts
    return false;
}

// show the assert modal dialog
static
void ShowAssertDialog(const wxChar *szFile,
                      int nLine,
                      const wxChar *szCond,
                      const wxChar *szMsg,
                      wxAppTraits *traits)
{
    // this variable can be set to true to suppress "assert failure" messages
    static bool s_bNoAsserts = FALSE;

    wxString msg;
    msg.reserve(2048);

    // make life easier for people using VC++ IDE by using this format: like
    // this, clicking on the message will take us immediately to the place of
    // the failed assert
    msg.Printf(wxT("%s(%d): assert \"%s\" failed"), szFile, nLine, szCond);

    if ( szMsg )
    {
        msg << _T(": ") << szMsg;
    }
    else // no message given
    {
        msg << _T('.');
    }

#if wxUSE_THREADS
    // if we are not in the main thread, output the assert directly and trap
    // since dialogs cannot be displayed
    if ( !wxThread::IsMain() )
    {
        msg += wxT(" [in child thread]");

#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
        msg << wxT("\r\n");
        OutputDebugString(msg );
#else
        // send to stderr
        wxFprintf(stderr, wxT("%s\n"), msg.c_str());
        fflush(stderr);
#endif
        // He-e-e-e-elp!! we're asserting in a child thread
        wxTrap();
    }
#endif // wxUSE_THREADS

    if ( !s_bNoAsserts )
    {
        // send it to the normal log destination
        wxLogDebug(_T("%s"), msg);

        if ( traits )
        {
            // delegate showing assert dialog (if possible) to that class
            s_bNoAsserts = traits->ShowAssertDialog(msg);
        }
        else // no traits object
        {
            // fall back to the function of last resort
            s_bNoAsserts = DoShowAssertDialog(msg);
        }
    }
}

#endif // __WXDEBUG__

