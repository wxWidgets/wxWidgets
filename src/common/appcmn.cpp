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
    #if wxUSE_GUI
        #include "wx/msgdlg.h"
    #endif // wxUSE_GUI
#endif

#include "wx/cmdline.h"
#include "wx/thread.h"
#include "wx/confbase.h"

#if !defined(__WXMSW__) || defined(__WXMICROWIN__)
  #include  <signal.h>      // for SIGTRAP used by wxTrap()
#endif  //Win/Unix

#if defined(__WXMSW__)
  #include  "wx/msw/private.h"  // includes windows.h for MessageBox()
#endif

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// initialization and termination
// ----------------------------------------------------------------------------

wxAppBase::wxAppBase()
{
    wxTheApp = (wxApp *)this;

    // VZ: what's this? is it obsolete?
    m_wantDebugOutput = FALSE;

#if wxUSE_GUI
    m_topWindow = (wxWindow *)NULL;
    m_useBestVisual = FALSE;
    m_exitOnFrameDelete = TRUE;
    m_isActive = TRUE;
#endif // wxUSE_GUI
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

    return 0;
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
    wxNode *node = wxPendingEvents->First();
    while (node)
    {
        wxEvtHandler *handler = (wxEvtHandler *)node->Data();
        delete node;

        // In ProcessPendingEvents(), new handlers might be add
        // and we can safely leave the critical section here.
        wxLEAVE_CRIT_SECT( *wxPendingEventsLocker );
        handler->ProcessPendingEvents();
        wxENTER_CRIT_SECT( *wxPendingEventsLocker );

        node = wxPendingEvents->First();
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

// ----------------------------------------------------------------------------
// cmd line parsing
// ----------------------------------------------------------------------------

bool wxAppBase::OnInit()
{
#if wxUSE_CMDLINE_PARSER
    wxCmdLineParser parser(argc, argv);

    OnInitCmdLine(parser);

    bool cont;
    switch ( parser.Parse() )
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
            gettext_noop("generate verbose log messages")
        },
#endif // wxUSE_LOG

#ifdef __WXUNIVERSAL__
        {
            wxCMD_LINE_OPTION,
            _T(""),
            OPTION_THEME,
            gettext_noop("specify the theme to use"),
            wxCMD_LINE_VAL_STRING
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
            wxCMD_LINE_VAL_STRING
        },
#endif // __WXMGL__

        // terminator
        { wxCMD_LINE_NONE }
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

        if ( !SetDisplayMode(wxDisplayModeInfo(wxSize(w, h), bpp)) )
            return FALSE;
    }
#endif

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

// show the assert modal dialog
static
void ShowAssertDialog(const wxChar *szFile, int nLine, const wxChar *szMsg)
{
    // this variable can be set to true to suppress "assert failure" messages
    static bool s_bNoAsserts = FALSE;

    wxChar szBuf[4096];

    // make life easier for people using VC++ IDE: clicking on the message
    // will take us immediately to the place of the failed assert
    wxSnprintf(szBuf, WXSIZEOF(szBuf),
#ifdef __VISUALC__
               wxT("%s(%d): assert failed"),
#else  // !VC++
    // make the error message more clear for all the others
               wxT("Assert failed in file %s at line %d"),
#endif // VC/!VC
               szFile, nLine);

    if ( szMsg != NULL )
    {
        wxStrcat(szBuf, wxT(": "));
        wxStrcat(szBuf, szMsg);
    }
    else // no message given
    {
        wxStrcat(szBuf, wxT("."));
    }

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
void wxOnAssert(const wxChar *szFile, int nLine, const wxChar *szMsg)
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
        ShowAssertDialog(szFile, nLine, szMsg);
    }
    else
    {
        // let the app process it as it wants
        wxTheApp->OnAssert(szFile, nLine, szMsg);
    }

    s_bInAssert = FALSE;
}

void wxAppBase::OnAssert(const wxChar *file, int line, const wxChar *msg)
{
    ShowAssertDialog(file, line, msg);
}

#endif  //WXDEBUG

