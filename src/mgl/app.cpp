/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Author:      Vaclav Slavik
//              based on GTK and MSW implementations
// Id:          $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
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
    #include "wx/resource.h"
#endif

#include "wx/app.h"
#include "wx/fontutil.h"
#include "wx/univ/theme.h"
#include "wx/univ/renderer.h"
#include "wx/univ/colschem.h"
#include "wx/sysopt.h"
#include "wx/mgl/private.h"

//-----------------------------------------------------------------------------
// Global data
//-----------------------------------------------------------------------------

wxApp *wxTheApp = NULL;
wxAppInitializerFunction wxAppBase::m_appInitFn = (wxAppInitializerFunction) NULL;


//-----------------------------------------------------------------------------
// wxExit
//-----------------------------------------------------------------------------

void wxExit()
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

void wxWakeUpIdle()
{
#if wxUSE_THREADS
    if (!wxThread::IsMain())
        wxMutexGuiEnter();
#endif

    while (wxTheApp->ProcessIdle()) {} 

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

static bool wxCreateMGL_WM(const wxDisplayModeInfo& displayMode)
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
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()


wxApp::wxApp() : m_mainLoop(NULL)
{
}

wxApp::~wxApp()
{
}

wxDisplayModeInfo wxGetDefaultDisplayMode()
{
    wxString mode;
    unsigned w, h, bpp;

    if ( !wxGetEnv(wxT("WXMODE"), &mode) || 
         (wxSscanf(mode.c_str(), _T("%ux%u-%u"), &w, &h, &bpp) != 3) )
    {
        w = 640, h = 480, bpp = 16;
    }

    return wxDisplayModeInfo(w, h, bpp);
}

bool wxApp::SetDisplayMode(const wxDisplayModeInfo& mode)
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

bool wxApp::ProcessIdle()
{
    wxIdleEvent event;
    event.SetEventObject(this);
    ProcessEvent(event);

    return event.MoreRequested();
}

void wxApp::OnIdle(wxIdleEvent &event)
{
    static bool s_inOnIdle = FALSE;

    /* Avoid recursion (via ProcessEvent default case) */
    if (s_inOnIdle)
        return;

    s_inOnIdle = TRUE;

    /* Resend in the main thread events which have been prepared in other
       threads */
    ProcessPendingEvents();

    // 'Garbage' collection of windows deleted with Close().
    DeletePendingObjects();

#if wxUSE_LOG
    // flush the logged messages if any
    wxLog::FlushActive();
#endif // wxUSE_LOG

    // Send OnIdle events to all windows
    if ( SendIdleEvents() )
        event.RequestMore(TRUE);

    s_inOnIdle = FALSE;
}

bool wxApp::SendIdleEvents()
{
    bool needMore = FALSE;

    wxWindowList::Node* node = wxTopLevelWindows.GetFirst();
    while (node)
    {
        wxWindow* win = node->GetData();
        if ( SendIdleEvents(win) )
            needMore = TRUE;
        node = node->GetNext();
    }

    return needMore;
}

bool wxApp::SendIdleEvents(wxWindow* win)
{
    bool needMore = FALSE;

    wxIdleEvent event;
    event.SetEventObject(win);

    win->GetEventHandler()->ProcessEvent(event);

    if ( event.MoreRequested() )
        needMore = TRUE;

    wxNode* node = win->GetChildren().First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();
        if ( SendIdleEvents(win) )
            needMore = TRUE;

        node = node->Next();
    }
    return needMore;
}

int wxApp::MainLoop()
{
    int rt;
    m_mainLoop = new wxEventLoop;

    rt = m_mainLoop->Run();

    delete m_mainLoop;
    m_mainLoop = NULL;
    return rt;
}

void wxApp::ExitMainLoop()
{
    if ( m_mainLoop )
        m_mainLoop->Exit(0);
}

bool wxApp::Initialized()
{
    return (wxTopLevelWindows.GetCount() != 0);
}

bool wxApp::Pending()
{
    return wxEventLoop::GetActive()->Pending();
}

void wxApp::Dispatch()
{
    wxEventLoop::GetActive()->Dispatch();
}

void wxApp::DeletePendingObjects()
{
    wxNode *node = wxPendingDelete.First();
    while (node)
    {
        wxObject *obj = (wxObject *)node->Data();

        delete obj;

        if ( wxPendingDelete.Find(obj) )
            delete node;

        node = wxPendingDelete.First();
    }
}

bool wxApp::Initialize()
{
    if ( MGL_init(".", NULL) == 0 )
    {
        wxLogError(_("Cannot initialize SciTech MGL!"));
        return FALSE;
    }

    wxClassInfo::InitializeClasses();

#if wxUSE_INTL
    wxFont::SetDefaultEncoding(wxLocale::GetSystemEncoding());
#endif

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    wxPendingEvents = new wxList;
    wxPendingEventsLocker = new wxCriticalSection;
#endif

    wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
    wxTheColourDatabase->Initialize();
    
    // Can't do this in wxModule, because fonts are needed by stock lists
    wxTheFontsManager = new wxFontsManager;

    wxInitializeStockLists();
    wxInitializeStockObjects();

#if wxUSE_WX_RESOURCES
    wxInitializeResourceSystem();
#endif

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules()) return FALSE;

    return TRUE;
}

void wxApp::CleanUp()
{
#if wxUSE_LOG
    // continuing to use user defined log target is unsafe from now on because
    // some resources may be already unavailable, so replace it by something
    // more safe
    wxLog *oldlog = wxLog::SetActiveTarget(new wxLogStderr);
    if ( oldlog )
        delete oldlog;
#endif // wxUSE_LOG

    delete gs_rootWindow;

    wxModule::CleanUpModules();

#if wxUSE_WX_RESOURCES
    wxCleanUpResourceSystem();
#endif

    if (wxTheColourDatabase)
        delete wxTheColourDatabase;

    wxTheColourDatabase = (wxColourDatabase*) NULL;

    wxDeleteStockObjects();
    wxDeleteStockLists();

    delete wxTheApp;
    wxTheApp = (wxApp*) NULL;


    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    delete wxPendingEvents;
    delete wxPendingEventsLocker;
#endif

    wxClassInfo::CleanUpClasses();

    // Can't do this in wxModule, because fonts are needed by stock lists
    // (do it after deleting wxTheApp and cleaning modules up, since somebody
    // may be deleting fonts that lately)
    delete wxTheFontsManager;
    wxTheFontsManager = (wxFontsManager*) NULL;

    // check for memory leaks
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    if (wxDebugContext::CountObjectsLeft(TRUE) > 0)
    {
        wxLogDebug(wxT("There were memory leaks.\n"));
        wxDebugContext::Dump();
        wxDebugContext::PrintStatistics();
    }
#endif // Debug

#if wxUSE_LOG
    // do this as the very last thing because everything else can log messages
    wxLog::DontCreateOnDemand();

    wxLog *oldLog = wxLog::SetActiveTarget( (wxLog*) NULL );
    if (oldLog)
        delete oldLog;
#endif // wxUSE_LOG

    wxDestroyMGL_WM();
    MGL_exit();
}


int wxEntryStart(int argc, char *argv[])
{
    return wxApp::Initialize() ? 0 : -1;
}


int wxEntryInitGui()
{
    return wxTheApp->OnInitGui() ? 0 : -1;
}


void wxEntryCleanup()
{
    wxApp::CleanUp();
}



int wxEntry(int argc, char *argv[])
{
#ifdef __DJGPP__
    // VS: disable long filenames under DJGPP as the very first thing,
    //     since SciTech MGL doesn't like them much...
    wxSetEnv(wxT("LFN"), wxT("N"));
#endif

#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    // This seems to be necessary since there are 'rogue'
    // objects present at this point (perhaps global objects?)
    // Setting a checkpoint will ignore them as far as the
    // memory checking facility is concerned.
    // Of course you may argue that memory allocated in globals should be
    // checked, but this is a reasonable compromise.
    wxDebugContext::SetCheckpoint();
#endif
    int err = wxEntryStart(argc, argv);
    if ( err )
        return err;

    if ( !wxTheApp )
    {
        wxCHECK_MSG( wxApp::GetInitializerFunction(), -1,
                     wxT("wxWindows error: No initializer - use IMPLEMENT_APP macro.\n") );

        wxAppInitializerFunction app_ini = wxApp::GetInitializerFunction();

        wxObject *test_app = app_ini();

        wxTheApp = (wxApp*) test_app;
    }

    wxCHECK_MSG( wxTheApp, -1, wxT("wxWindows error: no application object") );

    wxTheApp->argc = argc;
#if wxUSE_UNICODE
    wxTheApp->argv = new wxChar*[argc+1];
    int mb_argc = 0;
    while (mb_argc < argc)
    {
        wxTheApp->argv[mb_argc] = wxStrdup(wxConvLibc.cMB2WX(argv[mb_argc]));
        mb_argc++;
    }
    wxTheApp->argv[mb_argc] = (wxChar *)NULL;
#else
    wxTheApp->argv = argv;
#endif

    wxString name(wxFileNameFromPath(argv[0]));
    wxStripExtension(name);
    wxTheApp->SetAppName(name);

    int retValue;
    retValue = wxEntryInitGui();

    // Here frames insert themselves automatically into wxTopLevelWindows by
    // getting created in OnInit().
    if ( retValue == 0 )
    {
        if ( !wxTheApp->OnInit() )
            retValue = -1;
    }

    if ( retValue == 0 )
    {
        /* delete pending toplevel windows (typically a single
           dialog) so that, if there isn't any left, we don't
           call OnRun() */
        wxTheApp->DeletePendingObjects();

        if ( wxTheApp->Initialized() )
        {
            wxTheApp->OnRun();

            wxWindow *topWindow = wxTheApp->GetTopWindow();
            if ( topWindow )
            {
                /* Forcibly delete the window. */
                if (topWindow->IsKindOf(CLASSINFO(wxFrame)) ||
                    topWindow->IsKindOf(CLASSINFO(wxDialog)) )
                {
                    topWindow->Close(TRUE);
                    wxTheApp->DeletePendingObjects();
                }
                else
                {
                    delete topWindow;
                    wxTheApp->SetTopWindow((wxWindow*) NULL);
                }
            }

#if wxUSE_LOG
            // flush the logged messages if any
            wxLog *log = wxLog::GetActiveTarget();
            if (log != NULL && log->HasPendingMessages())
                log->Flush();
#endif // wxUSE_LOG
            retValue = wxTheApp->OnExit();
        }
    }

    wxEntryCleanup();

    return retValue;
}
