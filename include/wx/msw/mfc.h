///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/mfc.h
// Purpose:     Helpers for applications using both wxWidgets and MFC
// Author:      Julian Smart, Vadim Zeitlin
// Created:     2017-12-01 (mostly extracted from samples/mfc)
// Copyright:   (c) 2017 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_MFC_H_
#define _WX_MSW_MFC_H_

#ifndef __AFXWIN_H__
    #error "MFC headers must be included before including this file."
#endif

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/window.h"
#include "wx/msw/winundef.h"

// ----------------------------------------------------------------------------
// MFC window class wrapping a window created by wxWidgets
// ----------------------------------------------------------------------------

class wxMFCWnd : public CWnd
{
public:
    // If default ctor is used, Attach() must be called later.
    wxMFCWnd()
    {
    }

    // Combines default ctor and Attach().
    explicit wxMFCWnd(wxWindow* w)
    {
        Attach(w);
    }

    void Attach(wxWindow* w)
    {
        CWnd::Attach(w->GetHWND());
    }

    ~wxMFCWnd()
    {
        // Prevent MFC from destroying the wxWindow.
        Detach();
    }
};

// ----------------------------------------------------------------------------
// MFC application class forwarding everything to wxApp
// ----------------------------------------------------------------------------

// The template parameter here is an existing class deriving from CWinApp or,
// if there is no such class, just CWinApp itself.
template <typename T>
class wxMFCApp : public T
{
public:
    typedef T BaseApp;

    BOOL InitInstance() override
    {
        if ( !BaseApp::InitInstance() )
            return FALSE;

        if ( !wxEntryStart(BaseApp::m_hInstance) )
            return FALSE;

        if ( !wxTheApp || !wxTheApp->CallOnInit() )
            return FALSE;

        if ( !InitMainWnd() )
            return FALSE;

        return TRUE;
    }

    int ExitInstance() override
    {
        delete BaseApp::m_pMainWnd;
        BaseApp::m_pMainWnd = nullptr;

        if ( wxTheApp )
            wxTheApp->OnExit();

        wxEntryCleanup();

        return BaseApp::ExitInstance();
    }

    // Override this to provide messages pre-processing for wxWidgets windows.
    BOOL PreTranslateMessage(MSG *msg) override
    {
        // As reported in issue #23574, wxGUIEventLoop::PreProcessMessage()
        // is always returning true, so try BaseApp::PreTranslateMessage()
        // and hope it doesn't always report true
        if (BaseApp::PreTranslateMessage(msg))
            return TRUE;

        // Use the current event loop if there is one, or just fall back to the
        // standard one otherwise, but make sure we pre-process messages in any
        // case as otherwise many things would break (e.g. keyboard
        // accelerators).
        wxGUIEventLoop*
            evtLoop = static_cast<wxGUIEventLoop *>(wxEventLoop::GetActive());
        wxGUIEventLoop evtLoopStd;
        if ( !evtLoop )
            evtLoop = &evtLoopStd;
        return evtLoop->PreProcessMessage(msg);
    }

    BOOL OnIdle(LONG lCount) override
    {
        BOOL moreIdle = BaseApp::OnIdle(lCount);

        if ( wxTheApp )
        {
            wxTheApp->ProcessPendingEvents();

            if ( wxTheApp->ProcessIdle() )
                moreIdle = TRUE;
        }

        return moreIdle;
    }

protected:
    // This virtual method can be overridden to create the main window using
    // MFC code. The default implementation relies on wxApp::OnInit() creating
    // a top level window which is then wrapped in an MFC window and used as
    // the main window.
    virtual BOOL InitMainWnd()
    {
        wxWindow* const w = wxTheApp->GetTopWindow();
        if ( !w )
            return FALSE;

        // We need to initialize the main window to let the program continue
        // running.
        BaseApp::m_pMainWnd = new wxMFCWnd(w);

        // We also need to reset m_pMainWnd when this window will be destroyed
        // to prevent MFC from using an invalid HWND, which is probably not
        // fatal but can result in at least asserts failures.
        w->Bind(wxEVT_DESTROY, &wxMFCApp::OnMainWindowDestroyed, this);

        // And we need to let wxWidgets know that it should exit the
        // application when this window is closed, as OnRun(), which does this
        // by default, won't be called when using MFC main message loop.
        wxTheApp->SetExitOnFrameDelete(true);

        return TRUE;
    }

private:
    void OnMainWindowDestroyed(wxWindowDestroyEvent& event)
    {
        event.Skip();

        delete BaseApp::m_pMainWnd;
        BaseApp::m_pMainWnd = nullptr;
    }
};

typedef wxMFCApp<CWinApp> wxMFCWinApp;

// ----------------------------------------------------------------------------
// wxWidgets application class to be used in MFC applications
// ----------------------------------------------------------------------------

class wxAppWithMFC : public wxApp
{
public:
    void ExitMainLoop() override
    {
        // There is no wxEventLoop to exit, tell MFC to stop pumping messages
        // instead.
        ::PostQuitMessage(0);
    }

    void WakeUpIdle() override
    {
        // As above, we can't wake up any wx event loop, so try to wake up the
        // MFC one instead.
        CWinApp* const mfcApp = AfxGetApp();
        if ( mfcApp && mfcApp->m_pMainWnd )
        {
            ::PostMessage(mfcApp->m_pMainWnd->m_hWnd, WM_NULL, 0, 0);
        }
    }
};

#endif // _WX_MSW_MFC_H_
