/////////////////////////////////////////////////////////////////////////////
// Name:        common/appcmn.cpp
// Purpose:     wxAppBase methods common to all platforms
// Author:      Vadim Zeitlin
// Modified by:
// Created:     18.10.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "appbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/list.h"
    #if wxUSE_LOG
        #include "wx/log.h"
    #endif // wxUSE_LOG
    #if wxUSE_GUI
        #include "wx/msgdlg.h"
    #endif // wxUSE_GUI
#endif

#include "wx/cmdline.h"
#include "wx/thread.h"
#include "wx/confbase.h"
#include "wx/tokenzr.h"
#include "wx/utils.h"
#include "wx/msgout.h"

#if wxUSE_GUI
    #include "wx/artprov.h"
#endif // wxUSE_GUI

#if !defined(__WXMSW__) || defined(__WXMICROWIN__)
  #include  <signal.h>      // for SIGTRAP used by wxTrap()
#endif  //Win/Unix

#if defined(__WXMSW__)
  #include  "wx/msw/private.h"  // includes windows.h for MessageBox()
#endif

#if defined(__WXMAC__)
  #include  "wx/mac/private.h"  // includes mac headers
#endif

// private functions prototypes
#ifdef __WXDEBUG__
    static void LINKAGEMODE SetTraceMasks();
#endif // __WXDEBUG__

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// initialization and termination
// ----------------------------------------------------------------------------

wxAppBase::wxAppBase()
{
    wxTheApp = (wxApp *)this;

#if WXWIN_COMPATIBILITY_2_2
    m_wantDebugOutput = FALSE;
#endif // WXWIN_COMPATIBILITY_2_2

#if wxUSE_GUI
    m_topWindow = (wxWindow *)NULL;
    m_useBestVisual = FALSE;
    m_isActive = TRUE;

    // We don't want to exit the app if the user code shows a dialog from its
    // OnInit() -- but this is what would happen if we set m_exitOnFrameDelete
    // to Yes initially as this dialog would be the last top level window.
    // OTOH, if we set it to No initially we'll have to overwrite it with Yes
    // when we enter our OnRun() because we do want the default behaviour from
    // then on. But this would be a problem if the user code calls
    // SetExitOnFrameDelete(FALSE) from OnInit().
    //
    // So we use the special "Later" value which is such that
    // GetExitOnFrameDelete() returns FALSE for it but which we know we can
    // safely (i.e. without losing the effect of the users SetExitOnFrameDelete
    // call) overwrite in OnRun()
    m_exitOnFrameDelete = Later;
#endif // wxUSE_GUI

#ifdef __WXDEBUG__
    SetTraceMasks();
#endif
}

wxAppBase::~wxAppBase()
{
    // this destructor is required for Darwin
}

#if wxUSE_GUI

bool wxAppBase::OnInitGui()
{
#ifdef __WXUNIVERSAL__
    if ( !wxTheme::Get() && !wxTheme::CreateDefault() )
        return FALSE;
#endif // __WXUNIVERSAL__

    return TRUE;
}

int wxAppBase::OnRun()
{
    // see the comment in ctor: if the initial value hasn't been changed, use
    // the default Yes from now on
    if ( m_exitOnFrameDelete == Later )
    {
        m_exitOnFrameDelete = Yes;
    }
    //else: it has been changed, assume the user knows what he is doing

    return MainLoop();
}

#endif // wxUSE_GUI

int wxAppBase::OnExit()
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

// ----------------------------------------------------------------------------
// customization hooks
// ----------------------------------------------------------------------------

#if wxUSE_LOG

wxLog *wxAppBase::CreateLogTarget()
{
#if wxUSE_GUI && wxUSE_LOGGUI && !defined(__WXMICROWIN__)
    return new wxLogGui;
#else // !GUI
    return new wxLogStderr;
#endif // wxUSE_GUI
}

#endif // wxUSE_LOG

wxMessageOutput *wxAppBase::CreateMessageOutput()
{
    // The standard way of printing help on command line arguments (app --help)
    // is (according to common practice):
    //     - console apps: to stderr (on any platform)
    //     - GUI apps: stderr on Unix platforms (!)
    //                 message box under Windows and others
#if wxUSE_GUI && !defined(__UNIX__)
    // wxMessageOutputMessageBox doesn't work under Motif
    #ifdef __WXMOTIF__
        return new wxMessageOutputLog;
    #else
        return new wxMessageOutputMessageBox;
    #endif
#else // !wxUSE_GUI || __UNIX__
    return new wxMessageOutputStderr;
#endif
}

// ---------------------------------------------------------------------------
// wxAppBase
// ----------------------------------------------------------------------------

void wxAppBase::ProcessPendingEvents()
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

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

#if wxUSE_GUI

void wxAppBase::SetActive(bool active, wxWindow * WXUNUSED(lastFocus))
{
    if ( active == m_isActive )
        return;

    m_isActive = active;

    wxActivateEvent event(wxEVT_ACTIVATE_APP, active);
    event.SetEventObject(this);

    (void)ProcessEvent(event);
}

#endif // wxUSE_GUI

int wxAppBase::FilterEvent(wxEvent& WXUNUSED(event))
{
    // process the events normally by default
    return -1;
}

// ----------------------------------------------------------------------------
// cmd line parsing
// ----------------------------------------------------------------------------

bool wxAppBase::OnInit()
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

#if wxUSE_CMDLINE_PARSER

#define OPTION_VERBOSE _T("verbose")
#define OPTION_THEME   _T("theme")
#define OPTION_MODE    _T("mode")

void wxAppBase::OnInitCmdLine(wxCmdLineParser& parser)
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

bool wxAppBase::OnCmdLineParsed(wxCmdLineParser& parser)
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

bool wxAppBase::OnCmdLineHelp(wxCmdLineParser& parser)
{
    parser.Usage();

    return FALSE;
}

bool wxAppBase::OnCmdLineError(wxCmdLineParser& parser)
{
    parser.Usage();

    return FALSE;
}

#endif // wxUSE_CMDLINE_PARSER

// ----------------------------------------------------------------------------
// debugging support
// ----------------------------------------------------------------------------

/* static */
bool wxAppBase::CheckBuildOptions(const wxBuildOptions& opts)
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

#ifdef  __WXDEBUG__

static void LINKAGEMODE SetTraceMasks()
{
#if wxUSE_LOG
    wxString mask;
    if ( wxGetEnv(wxT("WXTRACE"), &mask) )
    {
        wxStringTokenizer tkn(mask, wxT(","));
        while ( tkn.HasMoreTokens() )
            wxLog::AddTraceMask(tkn.GetNextToken());
    }
#endif // wxUSE_LOG
}

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
  
// show the assert modal dialog
static
void ShowAssertDialog(const wxChar *szFile,
                      int nLine,
                      const wxChar *szCond,
                      const wxChar *szMsg)
{
    // this variable can be set to true to suppress "assert failure" messages
    static bool s_bNoAsserts = FALSE;

    wxChar szBuf[4096];

    // make life easier for people using VC++ IDE by using this format: like
    // this, clicking on the message will take us immediately to the place of
    // the failed assert
    wxSnprintf(szBuf, WXSIZEOF(szBuf),
               wxT("%s(%d): assert \"%s\" failed"),
               szFile, nLine, szCond);

    if ( szMsg != NULL )
    {
        wxStrcat(szBuf, wxT(": "));
        wxStrcat(szBuf, szMsg);
    }
    else // no message given
    {
        wxStrcat(szBuf, wxT("."));
    }

#if wxUSE_THREADS
    // if we are not in the main thread,
    // output the assert directly and trap since dialogs cannot be displayed
    if (!wxThread::IsMain()) {
        wxStrcat(szBuf, wxT(" [in child thread]"));
#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
        wxStrcat(szBuf, wxT("\r\n"));
        OutputDebugString(szBuf);
#else
        // send to stderr
        wxFprintf(stderr, wxT("%s\n"), szBuf);
        fflush(stderr);
#endif
        // He-e-e-e-elp!! we're asserting in a child thread
        wxTrap();
    }
#endif // wxUSE_THREADS

    if ( !s_bNoAsserts )
    {
        // send it to the normal log destination
        wxLogDebug(szBuf);

#if (wxUSE_GUI && wxUSE_MSGDLG) || defined(__WXMSW__)
        // this message is intentionally not translated - it is for
        // developpers only
        wxStrcat(szBuf, wxT("\nDo you want to stop the program?\nYou can also choose [Cancel] to suppress further warnings."));

        // use the native message box if available: this is more robust than
        // using our own
#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
        switch ( ::MessageBox(NULL, szBuf, _T("Debug"),
                              MB_YESNOCANCEL | MB_ICONSTOP ) )
        {
            case IDYES:
                wxTrap();
                break;

            case IDCANCEL:
                s_bNoAsserts = TRUE;
                break;

            //case IDNO: nothing to do
        }
#else // !MSW
        switch ( wxMessageBox(szBuf, wxT("Debug"),
                              wxYES_NO | wxCANCEL | wxICON_STOP ) )
        {
            case wxYES:
                wxTrap();
                break;

            case wxCANCEL:
                s_bNoAsserts = TRUE;
                break;

            //case wxNO: nothing to do
        }
#endif // GUI or MSW

#else // !GUI
        wxTrap();
#endif // GUI/!GUI
    }
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
        // by default, show the assert dialog box - we can't customize this
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

void wxAppBase::OnAssert(const wxChar *file,
                         int line,
                         const wxChar *cond,
                         const wxChar *msg)
{
    ShowAssertDialog(file, line, cond, msg);
}

#endif  //WXDEBUG

