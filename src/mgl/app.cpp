/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Author:      Vaclav Slavik
//              based on GTK and MSW implementations
// Id:          $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "app.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/module.h"
    #include "wx/evtloop.h"
    #include "wx/frame.h"
    #include "wx/dialog.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif

#include "wx/app.h"
#include "wx/fontutil.h"
#include "wx/univ/theme.h"
#include "wx/univ/renderer.h"
#include "wx/univ/colschem.h"
#include "wx/sysopt.h"
#include "wx/mgl/private.h"

//-----------------------------------------------------------------------------
// wxApp::Exit()
//-----------------------------------------------------------------------------

void wxApp::Exit()
{
    MGL_exit();
    exit(0);
}

//-----------------------------------------------------------------------------
// wxYield
//-----------------------------------------------------------------------------

static bool gs_inYield = FALSE;

bool wxApp::Yield(bool onlyIfNeeded)
{
    if ( gs_inYield )
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( wxT("wxYield called recursively" ) );
        }

        return FALSE;
    }

#if wxUSE_THREADS
    if ( !wxThread::IsMain() )
    {
        // can't process events from other threads, MGL is thread-unsafe
        return TRUE;
    }
#endif // wxUSE_THREADS

    gs_inYield = TRUE;

    wxLog::Suspend();

    if ( wxEventLoop::GetActive() )
    {
        while (wxEventLoop::GetActive()->Pending())
            wxEventLoop::GetActive()->Dispatch();
    }

    /* it's necessary to call ProcessIdle() to update the frames sizes which
       might have been changed (it also will update other things set from
       OnUpdateUI() which is a nice (and desired) side effect) */
    while (wxTheApp->ProcessIdle()) { }

    wxLog::Resume();

    gs_inYield = FALSE;

    return TRUE;
}


//-----------------------------------------------------------------------------
// wxWakeUpIdle
//-----------------------------------------------------------------------------

void wxApp::WakeUpIdle()
{
#if wxUSE_THREADS
    if (!wxThread::IsMain())
        wxMutexGuiEnter();
#endif

    while (wxTheApp->ProcessIdle())
        ;

#if wxUSE_THREADS
    if (!wxThread::IsMain())
        wxMutexGuiLeave();
#endif
}

//-----------------------------------------------------------------------------
// Root window
//-----------------------------------------------------------------------------

class wxRootWindow : public wxWindow
{
    public:
        wxRootWindow() : wxWindow(NULL, -1)
        {
            SetMGLwindow_t(MGL_wmGetRootWindow(g_winMng));
            SetBackgroundColour(wxTHEME_COLOUR(DESKTOP));
        }
        ~wxRootWindow()
        {
            // we don't want to delete MGL_WM's rootWnd
            m_wnd = NULL;
        }

        virtual bool AcceptsFocus() const { return FALSE; }

        DECLARE_DYNAMIC_CLASS(wxRootWindow)
};

IMPLEMENT_DYNAMIC_CLASS(wxRootWindow, wxWindow)

static wxRootWindow *gs_rootWindow = NULL;

//-----------------------------------------------------------------------------
// MGL initialization
//-----------------------------------------------------------------------------

static bool wxCreateMGL_WM(const wxVideoMode& displayMode)
{
    int mode;
    int refresh = MGL_DEFAULT_REFRESH;

#if wxUSE_SYSTEM_OPTIONS
    if ( wxSystemOptions::HasOption(wxT("mgl.screen-refresh")) )
        refresh = wxSystemOptions::GetOptionInt(wxT("mgl.screen-refresh"));
#endif

    mode = MGL_findMode(displayMode.GetWidth(),
                        displayMode.GetHeight(),
                        displayMode.GetDepth());
    if ( mode == -1 )
    {
        wxLogError(_("Mode %ix%i-%i not available."),
                     displayMode.GetWidth(),
                     displayMode.GetHeight(),
                     displayMode.GetDepth());
        return FALSE;
    }
    g_displayDC = new MGLDisplayDC(mode, 1, refresh);
    if ( !g_displayDC->isValid() )
    {
        delete g_displayDC;
        g_displayDC = NULL;
        return FALSE;
    }

    g_winMng = MGL_wmCreate(g_displayDC->getDC());
    if (!g_winMng)
        return FALSE;

    return TRUE;
}

static void wxDestroyMGL_WM()
{
    if ( g_winMng )
    {
        MGL_wmDestroy(g_winMng);
        g_winMng = NULL;
    }
    if ( g_displayDC )
    {
        delete g_displayDC;
        g_displayDC = NULL;
    }
}

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp,wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxAppBase::OnIdle)
END_EVENT_TABLE()


wxApp::wxApp()
{
}

wxApp::~wxApp()
{
}

wxVideoMode wxGetDefaultDisplayMode()
{
    wxString mode;
    unsigned w, h, bpp;

    if ( !wxGetEnv(wxT("WXMODE"), &mode) ||
         (wxSscanf(mode.c_str(), _T("%ux%u-%u"), &w, &h, &bpp) != 3) )
    {
        w = 640, h = 480, bpp = 16;
    }

    return wxVideoMode(w, h, bpp);
}

bool wxApp::SetDisplayMode(const wxVideoMode& mode)
{
    if ( !mode.IsOk() )
    {
        return FALSE;
    }
    if ( g_displayDC != NULL )
    {
        // FIXME_MGL -- we currently don't allow to switch video mode
        // more than once. This can hopefully be changed...
        wxFAIL_MSG(wxT("Can't change display mode after intialization!"));
        return FALSE;
    }

    if ( !wxCreateMGL_WM(mode) )
        return FALSE;
    gs_rootWindow = new wxRootWindow;

    m_displayMode = mode;

    return TRUE;
}

bool wxApp::OnInitGui()
{
    if ( !wxAppBase::OnInitGui() )
        return FALSE;

#ifdef __WXDEBUG__
    // MGL redirects stdout and stderr to physical console, so lets redirect
    // it to file. Do it only when WXDEBUG environment variable is set
    wxString redirect;
    if ( wxGetEnv(wxT("WXSTDERR"), &redirect) )
        freopen(redirect.mb_str(), "wt", stderr);
#endif

    wxLog *oldLog = wxLog::SetActiveTarget(new wxLogGui);
    if ( oldLog ) delete oldLog;

    return TRUE;
}

bool wxApp::Initialize(int& argc, wxChar **argv)
{
#ifdef __DJGPP__
    // VS: disable long filenames under DJGPP as the very first thing,
    //     since SciTech MGL doesn't like them much...
    wxSetEnv(wxT("LFN"), wxT("N"));
#endif

    // must do it before calling wxAppBase::Initialize(), because fonts are
    // needed by stock lists which are created there
    wxTheFontsManager = new wxFontsManager;

    if ( !wxAppBase::Initialize(argc, argv) )
        return false;

    if ( MGL_init(".", NULL) == 0 )
    {
        wxLogError(_("Cannot initialize SciTech MGL!"));

        wxAppBase::CleanUp();

        return false;
    }

#if wxUSE_INTL
    wxFont::SetDefaultEncoding(wxLocale::GetSystemEncoding());
#endif

    return true;
}

void wxApp::CleanUp()
{
    delete gs_rootWindow;

    wxAppBase::CleanUp();

    // must do this after calling base class CleanUp()
    delete wxTheFontsManager;
    wxTheFontsManager = (wxFontsManager*) NULL;

    wxDestroyMGL_WM();
    MGL_exit();
}

