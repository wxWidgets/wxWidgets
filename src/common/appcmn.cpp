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
#endif

#include "wx/cmdline.h"
#include "wx/thread.h"
#include "wx/confbase.h"

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
    m_isActive = active;
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
#endif wxUSE_LOG

#ifdef __WXUNIVERSAL__
        {
            wxCMD_LINE_OPTION,
            _T(""),
            OPTION_THEME,
            gettext_noop("specify the theme to use"),
            wxCMD_LINE_VAL_STRING
        },
#endif // __WXUNIVERSAL__

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

