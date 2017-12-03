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
    explicit wxMFCWnd(wxWindow* w)
    {
        Attach(w->GetHWND());
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

class wxMFCWinApp : public CWinApp
{
public:
    BOOL InitInstance() wxOVERRIDE
    {
        if ( !CWinApp::InitInstance() )
            return FALSE;

        if ( !wxEntryStart(m_hInstance) )
            return FALSE;

        if ( !wxTheApp || !wxTheApp->CallOnInit() )
            return FALSE;

        if ( !InitMainWnd() )
            return FALSE;

        return TRUE;
    }

    int ExitInstance() wxOVERRIDE
    {
        delete m_pMainWnd;

        if ( wxTheApp )
            wxTheApp->OnExit();

        wxEntryCleanup();

        return CWinApp::ExitInstance();
    }

    // Override this to provide wxWidgets message loop compatibility
    BOOL PreTranslateMessage(MSG *msg) wxOVERRIDE
    {
        wxEventLoop * const
            evtLoop = static_cast<wxEventLoop *>(wxEventLoop::GetActive());
        if ( evtLoop && evtLoop->PreProcessMessage(msg) )
            return TRUE;

        return CWinApp::PreTranslateMessage(msg);
    }

    BOOL OnIdle(LONG lCount) wxOVERRIDE
    {
        BOOL moreIdle = CWinApp::OnIdle(lCount);

        if ( wxTheApp && wxTheApp->ProcessIdle() )
            moreIdle = TRUE;

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
        m_pMainWnd = new wxMFCWnd(w);

        // And we need to let wxWidgets know that it should exit the
        // application when this window is closed, as OnRun(), which does this
        // by default, won't be called when using MFC main message loop.
        wxTheApp->SetExitOnFrameDelete(true);

        return TRUE;
    }
};

// ----------------------------------------------------------------------------
// wxWidgets application class to be used in MFC applications
// ----------------------------------------------------------------------------

class wxAppWithMFC : public wxApp
{
public:
    void ExitMainLoop() wxOVERRIDE
    {
        // There is no wxEventLoop to exit, tell MFC to stop pumping messages
        // instead.
        ::PostQuitMessage(0);
    }
};

#endif // _WX_MSW_MFC_H_
