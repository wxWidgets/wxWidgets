/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/app.cpp
// Purpose:     wxApp
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/msw/wrapcctl.h"
    #include "wx/dynarray.h"
    #include "wx/frame.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/cursor.h"
    #include "wx/icon.h"
    #include "wx/palette.h"
    #include "wx/dc.h"
    #include "wx/dialog.h"
    #include "wx/msgdlg.h"
    #include "wx/intl.h"
    #include "wx/crt.h"
    #include "wx/log.h"
    #include "wx/module.h"
#endif

#include "wx/apptrait.h"
#include "wx/filename.h"
#include "wx/dynlib.h"
#include "wx/evtloop.h"
#include "wx/msgdlg.h"
#include "wx/thread.h"
#include "wx/platinfo.h"
#include "wx/scopeguard.h"
#include "wx/sysopt.h"
#include "wx/vector.h"
#include "wx/weakref.h"

#include "wx/msw/private.h"
#include "wx/msw/dc.h"
#include "wx/msw/ole/oleutils.h"
#include "wx/msw/private/darkmode.h"
#include "wx/msw/private/timer.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

#if wxUSE_OLE
    #include <ole2.h>
#endif

#include <string.h>
#include <ctype.h>

#include "wx/msw/missing.h"

// instead of including <shlwapi.h> which is not part of the core SDK and not
// shipped at all with other compilers, we always define the parts of it we
// need here ourselves
//
// NB: DLLVER_PLATFORM_WINDOWS will be defined if shlwapi.h had been somehow
//     included already
#ifndef DLLVER_PLATFORM_WINDOWS
    // hopefully we don't need to change packing as DWORDs should be already
    // correctly aligned
    struct DLLVERSIONINFO
    {
        DWORD cbSize;
        DWORD dwMajorVersion;                   // Major version
        DWORD dwMinorVersion;                   // Minor version
        DWORD dwBuildNumber;                    // Build number
        DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
    };

    typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO *);
#endif // defined(DLLVERSIONINFO)

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

extern void wxSetKeyboardHook(bool doIt);

// because of mingw32 4.3 bug this struct can't be inside the namespace below:
// see http://article.gmane.org/gmane.comp.lib.wxwidgets.devel/110282
struct ClassRegInfo
{
    ClassRegInfo(const wxChar *name, int flags)
    {
        if ( (flags & wxApp::RegClass_OnlyNR) == wxApp::RegClass_OnlyNR )
        {
            // We don't register the "normal" variant, so leave its name empty
            // to indicate that it's not used and use the given name for the
            // class that we do register: we don't need the "NR" suffix to
            // distinguish it in this case as there is only a single variant.
            regnameNR = name;
        }
        else // Register both normal and NR variants.
        {
            // Here we use a special suffix to make the class names unique.
            regname = name;
            regnameNR = regname + wxApp::GetNoRedrawClassSuffix();
        }
    }

    // Return the appropriate string depending on the presence of
    // RegClass_ReturnNR bit in the flags.
    const wxChar* GetRequestedName(int flags) const
    {
        return (flags & wxApp::RegClass_ReturnNR ? regnameNR : regname).t_str();
    }

    // the name of the registered class with and without CS_[HV]REDRAW styles
    wxString regname;
    wxString regnameNR;
};

namespace
{

wxVector<ClassRegInfo> gs_regClassesInfo;

} // anonymous namespace

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

LRESULT WXDLLEXPORT APIENTRY wxWndProc(HWND, UINT, WPARAM, LPARAM);

// ----------------------------------------------------------------------------
// Module for OLE initialization and cleanup
// ----------------------------------------------------------------------------

class wxOleInitModule : public wxModule
{
public:
    wxOleInitModule()
    {
    }

    virtual bool OnInit() override
    {
        return wxOleInitialize();
    }

    virtual void OnExit() override
    {
        wxOleUninitialize();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxOleInitModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxOleInitModule, wxModule);

// ===========================================================================
// wxGUIAppTraits implementation
// ===========================================================================

// private class which we use to pass parameters from BeforeChildWaitLoop() to
// AfterChildWaitLoop()
struct ChildWaitLoopData
{
    ChildWaitLoopData(wxWindowDisabler *wd_, wxWindow *focused_, wxWindow *winActive_)
    {
        wd = wd_;
        focused = focused_;
        winActive = winActive_;
    }

    wxWindowDisabler *wd;
    wxWeakRef<wxWindow> focused;
    wxWindow *winActive;
};

void *wxGUIAppTraits::BeforeChildWaitLoop()
{
    /*
       We use a dirty hack here to disable all application windows (which we
       must do because otherwise the calls to wxYield() could lead to some very
       unexpected reentrancies in the users code) but to avoid losing
       focus/activation entirely when the child process terminates which would
       happen if we simply disabled everything using wxWindowDisabler. Indeed,
       remember that Windows will never activate a disabled window and when the
       last child's window is closed and Windows looks for a window to activate
       all our windows are still disabled. There is no way to enable them in
       time because we don't know when the child's windows are going to be
       closed, so the solution we use here is to keep one special tiny dialog
       enabled all the time. Then when the child terminates it will get
       activated and when we close it below -- after re-enabling all the other
       windows! -- the previously active window becomes activated again and
       everything is ok.
     */
    wxBeginBusyCursor();

    wxWindow* const focus = wxWindow::FindFocus();

    // first disable all existing windows
    wxWindowDisabler *wd = new wxWindowDisabler;

    // then create an "invisible" dialog: it has minimal size, is positioned
    // (hopefully) outside the screen and doesn't appear in the Alt-TAB list
    // (unlike the frames, which is why we use a dialog here)
    wxWindow *winActive = new wxDialog
                    (
                        wxTheApp->GetTopWindow(),
                        wxID_ANY,
                        wxEmptyString,
                        wxPoint(32600, 32600),
                        wxSize(1, 1)
                    );
    winActive->Show();

    return new ChildWaitLoopData(wd, focus, winActive);
}

void wxGUIAppTraits::AfterChildWaitLoop(void *dataOrig)
{
    wxEndBusyCursor();

    ChildWaitLoopData * const data = (ChildWaitLoopData *)dataOrig;

    delete data->wd;

    if ( data->focused )
        data->focused->SetFocus();

    // finally delete the dummy dialog and, as wd has been already destroyed
    // and the other windows reenabled, the activation is going to return to
    // the window which had had it before
    data->winActive->Destroy();

    // also delete the temporary data object itself
    delete data;
}

#if wxUSE_THREADS
bool wxGUIAppTraits::DoMessageFromThreadWait()
{
    // we should return false only if the app should exit, i.e. only if
    // Dispatch() determines that the main event loop should terminate
    wxEventLoopBase * const evtLoop = wxEventLoop::GetActive();
    if ( !evtLoop || !evtLoop->Pending() )
    {
        // no events means no quit event
        return true;
    }

    return evtLoop->Dispatch();
}

WXDWORD wxGUIAppTraits::WaitForThread(WXHANDLE hThread, int flags)
{
    // We only ever dispatch messages from the main thread and, additionally,
    // even from the main thread we shouldn't wait for the message if we don't
    // have a running event loop as we would never remove them from the message
    // queue then and so we would enter an infinite loop as
    // MsgWaitForMultipleObjects() keeps returning WAIT_OBJECT_0 + 1.
    if ( flags == wxTHREAD_WAIT_YIELD && wxIsMainThread() )
    {
        wxMSWEventLoopBase* const
            evtLoop = static_cast<wxMSWEventLoopBase *>(wxEventLoop::GetActive());
        if ( evtLoop )
            return evtLoop->MSWWaitForThread(hThread);
    }

    // Simple blocking wait.
    return DoSimpleWaitForThread(hThread);
}
#endif // wxUSE_THREADS

wxPortId wxGUIAppTraits::GetToolkitVersion(int *majVer,
                                           int *minVer,
                                           int *microVer) const
{
    // on Windows, the toolkit version is the same of the OS version
    // as Windows integrates the OS kernel with the GUI toolkit.
    wxGetOsVersion(majVer, minVer, microVer);

    return wxPORT_MSW;
}

#if wxUSE_TIMER

wxTimerImpl *wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxMSWTimerImpl(timer);
}

#endif // wxUSE_TIMER

wxEventLoopBase* wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop;
}

// ---------------------------------------------------------------------------
// Stuff for using console from the GUI applications
// ---------------------------------------------------------------------------

namespace
{

/*
    Helper class to manipulate console from a GUI app.

    Notice that console output is available in the GUI app only if:
    - AttachConsole() returns TRUE
    - we have a valid STD_ERROR_HANDLE
    - command history hasn't been changed since our startup

    To check if all these conditions are verified, you need to simple call
    IsOkToUse(). It will check the first two conditions above the first time it
    is called (and if this fails, the subsequent calls will return immediately)
    and also recheck the last one every time it is called.
 */
class wxConsoleStderr
{
public:
    // default ctor does nothing, call Init() before using this class
    wxConsoleStderr()
    {
        m_hStderr = INVALID_HANDLE_VALUE;
        m_historyLen =
        m_dataLen =
        m_dataLine = 0;

        m_ok = -1;
    }

    ~wxConsoleStderr()
    {
        if ( m_hStderr != INVALID_HANDLE_VALUE )
        {
            if ( !::FreeConsole() )
            {
                wxLogLastError(wxT("FreeConsole"));
            }
        }
    }

    // return true if we were successfully initialized and there had been no
    // console activity which would interfere with our output since then
    bool IsOkToUse() const
    {
        if ( m_ok == -1 )
        {
            wxConsoleStderr * const self = const_cast<wxConsoleStderr *>(this);
            self->m_ok = self->DoInit();

            // no need to call IsHistoryUnchanged() as we just initialized
            // m_history anyhow
            return m_ok == 1;
        }

        return m_ok && IsHistoryUnchanged();
    }


    // output the provided text on the console, return true if ok
    bool Write(const wxString& text);

private:
    // called by Init() once only to do the real initialization
    bool DoInit();

    // retrieve the command line history into the provided buffer and return
    // its length
    int GetCommandHistory(wxWCharBuffer& buf) const;

    // check if the console history has changed
    bool IsHistoryUnchanged() const;

    int m_ok;                   // initially -1, set to true or false by Init()

    wxDynamicLibrary m_dllKernel32;

    HANDLE m_hStderr;           // console handle, if it's valid we must call
                                // FreeConsole() (even if m_ok != 1)

    wxWCharBuffer m_history;    // command history on startup
    int m_historyLen;           // length command history buffer

    wxCharBuffer m_data;        // data between empty line and cursor position
    int m_dataLen;              // length data buffer
    int m_dataLine;             // line offset

    typedef DWORD (WINAPI *GetConsoleCommandHistory_t)(LPTSTR sCommands,
                                                       DWORD nBufferLength,
                                                       LPCTSTR sExeName);
    typedef DWORD (WINAPI *GetConsoleCommandHistoryLength_t)(LPCTSTR sExeName);

    GetConsoleCommandHistory_t m_pfnGetConsoleCommandHistory;
    GetConsoleCommandHistoryLength_t m_pfnGetConsoleCommandHistoryLength;

    wxDECLARE_NO_COPY_CLASS(wxConsoleStderr);
};

bool wxConsoleStderr::DoInit()
{
    HANDLE hStderr = ::GetStdHandle(STD_ERROR_HANDLE);

    if ( hStderr == INVALID_HANDLE_VALUE || !hStderr )
        return false;

    if ( !m_dllKernel32.Load(wxT("kernel32.dll")) )
        return false;

    if ( !::AttachConsole(ATTACH_PARENT_PROCESS) )
        return false;

    // console attached, set m_hStderr now to ensure that we free it in the
    // dtor
    m_hStderr = hStderr;

    wxDL_INIT_FUNC_AW(m_pfn, GetConsoleCommandHistory, m_dllKernel32);
    if ( !m_pfnGetConsoleCommandHistory )
        return false;

    wxDL_INIT_FUNC_AW(m_pfn, GetConsoleCommandHistoryLength, m_dllKernel32);
    if ( !m_pfnGetConsoleCommandHistoryLength )
        return false;

    // remember the current command history to be able to compare with it later
    // in IsHistoryUnchanged()
    m_historyLen = GetCommandHistory(m_history);
    if ( !m_history )
        return false;


    // now find the first blank line above the current position
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if ( !::GetConsoleScreenBufferInfo(m_hStderr, &csbi) )
    {
        wxLogLastError(wxT("GetConsoleScreenBufferInfo"));
        return false;
    }

    COORD pos;
    pos.X = 0;
    pos.Y = csbi.dwCursorPosition.Y + 1;

    // we decide that a line is empty if first 4 characters are spaces
    DWORD ret;
    char buf[4];
    do
    {
        pos.Y--;
        if ( !::ReadConsoleOutputCharacterA(m_hStderr, buf, WXSIZEOF(buf),
                                            pos, &ret) )
        {
            wxLogLastError(wxT("ReadConsoleOutputCharacterA"));
            return false;
        }
    } while ( wxStrncmp("    ", buf, WXSIZEOF(buf)) != 0 );

    // calculate line offset and length of data
    m_dataLine = csbi.dwCursorPosition.Y - pos.Y;
    m_dataLen = m_dataLine*csbi.dwMaximumWindowSize.X + csbi.dwCursorPosition.X;

    if ( m_dataLen > 0 )
    {
        m_data.extend(m_dataLen);
        if ( !::ReadConsoleOutputCharacterA(m_hStderr, m_data.data(), m_dataLen,
                                            pos, &ret) )
        {
            wxLogLastError(wxT("ReadConsoleOutputCharacterA"));
            return false;
        }
    }

    return true;
}

int wxConsoleStderr::GetCommandHistory(wxWCharBuffer& buf) const
{
    // these functions are internal and may only be called by cmd.exe
    static const wxChar *CMD_EXE = wxT("cmd.exe");

    const int len = m_pfnGetConsoleCommandHistoryLength(CMD_EXE);
    if ( len )
    {
        buf.extend(len);

        int len2 = m_pfnGetConsoleCommandHistory(buf.data(), len, CMD_EXE);

        if ( len2 != len )
        {
            wxFAIL_MSG( wxT("failed getting history?") );
        }
    }

    return len;
}

bool wxConsoleStderr::IsHistoryUnchanged() const
{
    wxASSERT_MSG( m_ok == 1, wxT("shouldn't be called if not initialized") );

    // get (possibly changed) command history
    wxWCharBuffer history;
    const int historyLen = GetCommandHistory(history);

    // and compare it with the original one
    return historyLen == m_historyLen && history &&
                memcmp(m_history, history, historyLen) == 0;
}

bool wxConsoleStderr::Write(const wxString& text)
{
    wxASSERT_MSG( m_hStderr != INVALID_HANDLE_VALUE,
                    wxT("should only be called if Init() returned true") );

    // get current position
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if ( !::GetConsoleScreenBufferInfo(m_hStderr, &csbi) )
    {
        wxLogLastError(wxT("GetConsoleScreenBufferInfo"));
        return false;
    }

    // and calculate new position (where is empty line)
    csbi.dwCursorPosition.X = 0;
    csbi.dwCursorPosition.Y -= m_dataLine;

    if ( !::SetConsoleCursorPosition(m_hStderr, csbi.dwCursorPosition) )
    {
        wxLogLastError(wxT("SetConsoleCursorPosition"));
        return false;
    }

    DWORD ret;
    if ( !::FillConsoleOutputCharacter(m_hStderr, wxT(' '), m_dataLen,
                                       csbi.dwCursorPosition, &ret) )
    {
        wxLogLastError(wxT("FillConsoleOutputCharacter"));
        return false;
    }

    if ( !::WriteConsole(m_hStderr, text.t_str(), text.length(), &ret, nullptr) )
    {
        wxLogLastError(wxT("WriteConsole"));
        return false;
    }

    WriteConsoleA(m_hStderr, m_data, m_dataLen, &ret, 0);

    return true;
}

wxConsoleStderr s_consoleStderr;

} // anonymous namespace

bool wxGUIAppTraits::CanUseStderr()
{
    return s_consoleStderr.IsOkToUse();
}

bool wxGUIAppTraits::WriteToStderr(const wxString& text)
{
    return s_consoleStderr.IsOkToUse() && s_consoleStderr.Write(text);
}

WXHWND wxGUIAppTraits::GetMainHWND() const
{
    const wxWindow* const w = wxApp::GetMainTopWindow();
    return w ? w->GetHWND() : nullptr;
}

// ===========================================================================
// wxApp implementation
// ===========================================================================

int wxApp::m_nCmdShow = SW_SHOWNORMAL;

// ---------------------------------------------------------------------------
// wxWin macros
// ---------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler);

wxBEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
    EVT_END_SESSION(wxApp::OnEndSession)
    EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
wxEND_EVENT_TABLE()

// class to ensure that wxAppBase::CleanUp() is called if our Initialize()
// fails
class wxCallBaseCleanup
{
public:
    wxCallBaseCleanup(wxApp *app) : m_app(app) { }
    ~wxCallBaseCleanup() { if ( m_app ) m_app->wxAppBase::CleanUp(); }

    void Dismiss() { m_app = nullptr; }

private:
    wxApp *m_app;
};

//// Initialize
bool wxApp::Initialize(int& argc_, wxChar **argv_)
{
    if ( !wxAppBase::Initialize(argc_, argv_) )
        return false;

    // ensure that base cleanup is done if we return too early
    wxCallBaseCleanup callBaseCleanup(this);

    InitCommonControls();

    wxSetKeyboardHook(true);

    // this is useful to allow users to enable dark mode for the applications
    // not enabling it themselves by setting the corresponding environment
    // variable
    if ( const int darkMode = wxSystemOptions::GetOptionInt("msw.dark-mode") )
    {
        MSWEnableDarkMode(darkMode > 1 ? DarkMode_Always : DarkMode_Auto);
    }

    callBaseCleanup.Dismiss();

    return true;
}

// ---------------------------------------------------------------------------
// Win32 window class registration
// ---------------------------------------------------------------------------

/* static */
const wxChar *wxApp::GetRegisteredClassName(const wxChar *name,
                                            int bgBrushCol,
                                            int extraStyles,
                                            int flags)
{
    const size_t count = gs_regClassesInfo.size();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( gs_regClassesInfo[n].regname == name ||
                gs_regClassesInfo[n].regnameNR == name )
            return gs_regClassesInfo[n].GetRequestedName(flags);
    }

    // In dark mode, use the dark background brush instead of specified colour
    // which would result in light background.
    HBRUSH hbrBackground;
    if ( wxMSWDarkMode::IsActive() )
        hbrBackground = wxMSWDarkMode::GetBackgroundBrush();
    else
        hbrBackground = (HBRUSH)wxUIntToPtr(bgBrushCol + 1);


    // we need to register this class
    WNDCLASS wndclass;
    wxZeroMemory(wndclass);

    wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
    wndclass.hInstance     = wxGetInstance();
    wndclass.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
    wndclass.hbrBackground = hbrBackground;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | extraStyles;


    ClassRegInfo regClass(name, flags);
    if ( !regClass.regname.empty() )
    {
        wndclass.lpszClassName = regClass.regname.t_str();
        if ( !::RegisterClass(&wndclass) )
        {
            wxLogLastError(wxString::Format(wxT("RegisterClass(%s)"),
                           regClass.regname));
            return nullptr;
        }
    }

    wndclass.style &= ~(CS_HREDRAW | CS_VREDRAW);
    wndclass.lpszClassName = regClass.regnameNR.t_str();
    if ( !::RegisterClass(&wndclass) )
    {
        wxLogLastError(wxString::Format(wxT("RegisterClass(%s)"),
                       regClass.regname));
        ::UnregisterClass(regClass.regname.c_str(), wxGetInstance());
        return nullptr;
    }

    gs_regClassesInfo.push_back(regClass);

    // take care to return the pointer which will remain valid after the
    // function returns (it could be invalidated later if new elements are
    // added to the vector and it's reallocated but this shouldn't matter as
    // this pointer should be used right now, not stored)
    return gs_regClassesInfo.back().GetRequestedName(flags);
}

bool wxApp::IsRegisteredClassName(const wxString& name)
{
    const size_t count = gs_regClassesInfo.size();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( gs_regClassesInfo[n].regname == name ||
                gs_regClassesInfo[n].regnameNR == name )
            return true;
    }

    return false;
}

void wxApp::UnregisterWindowClasses()
{
    const size_t count = gs_regClassesInfo.size();
    for ( size_t n = 0; n < count; n++ )
    {
        const ClassRegInfo& regClass = gs_regClassesInfo[n];
        if ( !regClass.regname.empty() )
        {
            if ( !::UnregisterClass(regClass.regname.c_str(), wxGetInstance()) )
            {
                wxLogLastError(wxString::Format(wxT("UnregisterClass(%s)"),
                               regClass.regname));
            }
        }

        if ( !::UnregisterClass(regClass.regnameNR.c_str(), wxGetInstance()) )
        {
            wxLogLastError(wxString::Format(wxT("UnregisterClass(%s)"),
                           regClass.regnameNR));
        }
    }

    gs_regClassesInfo.clear();
}

void wxApp::CleanUp()
{
    // all objects pending for deletion must be deleted first, otherwise
    // UnregisterWindowClasses() call wouldn't succeed (because windows
    // using the classes being unregistered still exist), so call the base
    // class method first and only then do our clean up
    wxAppBase::CleanUp();

    wxSetKeyboardHook(false);

    // for an EXE the classes are unregistered when it terminates but DLL may
    // be loaded several times (load/unload/load) into the same process in
    // which case the registration will fail after the first time if we don't
    // unregister the classes now
    UnregisterWindowClasses();
}

// ----------------------------------------------------------------------------
// wxApp ctor/dtor
// ----------------------------------------------------------------------------

wxApp::wxApp()
{
    m_printMode = wxPRINT_WINDOWS;

    if ( !wxSystemOptions::GetOptionInt("msw.no-manifest-check") )
    {
        if ( GetComCtl32Version() < 610 )
        {
            // Check if we have wx resources in this program: this is not
            // mandatory, but recommended and could be the simplest way to
            // resolve the problem when not using MSVC.
            wxString maybeNoResources;
            if ( !::LoadIcon(wxGetInstance(), wxT("wxICON_AAA")) )
            {
                maybeNoResources = " (unless you don't include wx/msw/wx.rc "
                    "from your resource file intentionally, you should do it "
                    "and use the manifest defined in it)";
            }

            wxMessageBox
            (
                wxString::Format(R"(WARNING!

This application doesn't use a correct manifest specifying
the use of Common Controls Library v6%s.

This is deprecated and won't be supported in the future
wxWidgets versions, however for now you can still set
"msw.no-manifest-check" system option to 1 (see
https://docs.wxwidgets.org/latest/classwx_system_options.html
for how to do it) to skip this check.

Please use the appropriate manifest when building the
application as described at
https://docs.wxwidgets.org/latest/plat_msw_install.html#msw_manifest
or contact us by posting to wx-dev@googlegroups.com
if you believe not using the manifest should remain supported.
)", maybeNoResources),
                "wxWidgets Warning"
            );
        }
    }
}

wxApp::~wxApp()
{
}

// ----------------------------------------------------------------------------
// wxApp idle handling
// ----------------------------------------------------------------------------

void wxApp::OnIdle(wxIdleEvent& WXUNUSED(event))
{
#if wxUSE_DC_CACHEING
    // automated DC cache management: clear the cached DCs and bitmap
    // if it's likely that the app has finished with them, that is, we
    // get an idle event and we're not dragging anything.
    if (!::GetKeyState(MK_LBUTTON) && !::GetKeyState(MK_MBUTTON) && !::GetKeyState(MK_RBUTTON))
        wxMSWDCImpl::ClearCache();
#endif // wxUSE_DC_CACHEING
}

void wxApp::WakeUpIdle()
{
    wxEventLoopBase * const evtLoop = wxEventLoop::GetActive();
    if ( !evtLoop )
    {
        // We can't wake up the event loop if there is none and there is just
        // no need to do anything in this case, any pending events will be
        // handled when the event loop starts.
        return;
    }

    evtLoop->WakeUp();
}

void wxApp::MSWProcessPendingEventsIfNeeded()
{
    // The cast below is safe as wxEventLoop derives from wxMSWEventLoopBase in
    // both console and GUI applications.
    wxMSWEventLoopBase * const evtLoop =
        static_cast<wxMSWEventLoopBase *>(wxEventLoop::GetActive());
    if ( evtLoop && evtLoop->MSWIsWakeUpRequested() )
        ProcessPendingEvents();
}

// ----------------------------------------------------------------------------
// other wxApp event handlers
// ----------------------------------------------------------------------------

void wxApp::OnEndSession(wxCloseEvent& WXUNUSED(event))
{
    // Windows will terminate the process soon after we return from
    // WM_ENDSESSION handler or when we delete our last window, so make sure we
    // at least execute our cleanup code before

    // prevent the window from being destroyed when the corresponding wxTLW is
    // destroyed: this will result in a leak of a HWND, of course, but who
    // cares when the process is being killed anyhow
    if ( !wxTopLevelWindows.empty() )
        wxTopLevelWindows[0]->SetHWND(0);

    // Destroy all the remaining TLWs before calling OnExit() to have the same
    // sequence of events in this case as in case of the normal shutdown,
    // otherwise we could have many problems due to wxApp being already
    // destroyed when window cleanup code (in close event handlers or dtor) is
    // executed.
    DeleteAllTLWs();

    const int rc = OnExit();

    wxEntryCleanup();

    // calling exit() instead of ExitProcess() or not doing anything at all and
    // being killed by Windows has the advantage of executing the dtors of
    // global objects
    exit(rc);
}

// Default behaviour: close the application with prompts. The
// user can veto the close, and therefore the end session.
void wxApp::OnQueryEndSession(wxCloseEvent& event)
{
    if (GetTopWindow())
    {
        if (!GetTopWindow()->Close(!event.CanVeto()))
            event.Veto(true);
    }
}

// ----------------------------------------------------------------------------
// system DLL versions
// ----------------------------------------------------------------------------

namespace
{

// helper function: retrieve the DLL version by using DllGetVersion(), returns
// 0 if the DLL doesn't export such function
int CallDllGetVersion(wxDynamicLibrary& dll)
{
    // now check if the function is available during run-time
    wxDYNLIB_FUNCTION( DLLGETVERSIONPROC, DllGetVersion, dll );
    if ( !pfnDllGetVersion )
        return 0;

    DLLVERSIONINFO dvi;
    dvi.cbSize = sizeof(dvi);

    HRESULT hr = (*pfnDllGetVersion)(&dvi);
    if ( FAILED(hr) )
    {
        wxLogApiError(wxT("DllGetVersion"), hr);

        return 0;
    }

    return 100*dvi.dwMajorVersion + dvi.dwMinorVersion;
}

} // anonymous namespace

/* static */
int wxApp::GetComCtl32Version()
{
    // cache the result
    //
    // NB: this is MT-ok as in the worst case we'd compute s_verComCtl32 twice,
    //     but as its value should be the same both times it doesn't matter
    static int s_verComCtl32 = -1;

    if ( s_verComCtl32 == -1 )
    {
        // Test for Wine first because its comctl32.dll always returns 581 from
        // its DllGetVersion() even though it supports the functionality of
        // much later versions too.
        wxVersionInfo verWine;
        if ( wxIsRunningUnderWine(&verWine) )
        {
            // Not sure which version of Wine implements comctl32.dll v6
            // functionality, but 5 seems to have it already.
            if ( verWine.GetMajor() >= 5 )
                s_verComCtl32 = 610;
        }
    }

    if ( s_verComCtl32 == -1 )
    {
        // we're prepared to handle the errors
        wxLogNull noLog;

        // we don't want to load comctl32.dll, it should be already loaded but,
        // depending on the OS version and the presence of the manifest, it can
        // be either v5 or v6 and instead of trying to guess it just get the
        // handle of the already loaded version
        wxLoadedDLL dllComCtl32(wxT("comctl32.dll"));
        if ( !dllComCtl32.IsLoaded() )
        {
            s_verComCtl32 = 0;
            return 0;
        }

        // try DllGetVersion() for recent DLLs
        s_verComCtl32 = CallDllGetVersion(dllComCtl32);

        // if DllGetVersion() is unavailable either during compile or
        // run-time, try to guess the version otherwise
        if ( !s_verComCtl32 )
        {
            // InitCommonControlsEx is unique to 4.70 and later
            void *pfn = dllComCtl32.GetSymbol(wxT("InitCommonControlsEx"));
            if ( !pfn )
            {
                // not found, must be 4.00
                s_verComCtl32 = 400;
            }
            else // 4.70+
            {
                // many symbols appeared in comctl32 4.71, could use any of
                // them except may be DllInstall()
                pfn = dllComCtl32.GetSymbol(wxT("InitializeFlatSB"));
                if ( !pfn )
                {
                    // not found, must be 4.70
                    s_verComCtl32 = 470;
                }
                else
                {
                    // found, must be 4.71 or later
                    s_verComCtl32 = 471;
                }
            }
        }
    }

    return s_verComCtl32;
}

#if wxUSE_EXCEPTIONS

// ----------------------------------------------------------------------------
// exception handling
// ----------------------------------------------------------------------------

bool wxApp::OnExceptionInMainLoop()
{
    // ask the user about what to do: use the Win32 API function here as it
    // could be dangerous to use any wxWidgets code in this state
    switch (
            ::MessageBox
              (
                nullptr,
                wxT("An unhandled exception occurred. Press \"Abort\" to \
terminate the program,\r\n\
\"Retry\" to exit the program normally and \"Ignore\" to try to continue."),
                wxT("Unhandled exception"),
                MB_ABORTRETRYIGNORE |
                MB_ICONERROR|
                MB_TASKMODAL
              )
           )
    {
        case IDABORT:
            throw;

        default:
            wxFAIL_MSG( wxT("unexpected MessageBox() return code") );
            wxFALLTHROUGH;

        case IDRETRY:
            return false;

        case IDIGNORE:
            return true;
    }
}

#endif // wxUSE_EXCEPTIONS

// ----------------------------------------------------------------------------
// Layout direction
// ----------------------------------------------------------------------------

/* static */
wxLayoutDirection wxApp::MSWGetDefaultLayout(wxWindow* parent)
{
    wxLayoutDirection dir = wxLayout_Default;

    if ( parent )
        dir = parent->GetLayoutDirection();

    if ( dir == wxLayout_Default )
    {
        if ( wxTheApp )
            dir = wxTheApp->GetLayoutDirection();
    }

    if ( dir == wxLayout_Default )
    {
        DWORD dwLayout;
        if ( ::GetProcessDefaultLayout(&dwLayout) )
        {
            dir = dwLayout == LAYOUT_RTL ? wxLayout_RightToLeft
                                         : wxLayout_LeftToRight;
        }
    }

    return dir;
}
