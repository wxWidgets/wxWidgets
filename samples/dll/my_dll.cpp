/////////////////////////////////////////////////////////////////////////////
// Name:        my_dll.cpp
// Purpose:     Sample showing how to use wx from a DLL
// Author:      Vaclav Slavik
// Created:     2009-12-03
// Copyright:   (c) 2009 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#ifndef __WINDOWS__
    #error "This sample is Windows-only"
#endif

#include "wx/app.h"
#include "wx/dynlib.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "wx/msw/wrapwin.h"

#include <process.h> // for _beginthreadex()

#include "my_dll.h"

// ----------------------------------------------------------------------------
// GUI classes
// ----------------------------------------------------------------------------

class MyDllFrame : public wxFrame
{
public:
    MyDllFrame(wxWindow *parent, const wxString& label);

    void OnAbout(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};


static const int CMD_SHOW_WINDOW = wxNewId();
static const int CMD_TERMINATE = wxNewId();

class MyDllApp : public wxApp
{
public:
    MyDllApp();

private:
    void OnShowWindow(wxThreadEvent& event);
    void OnTerminate(wxThreadEvent& event);
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyDllFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyDllFrame, wxFrame)
    EVT_BUTTON(wxID_ABOUT, MyDllFrame::OnAbout)
wxEND_EVENT_TABLE()

MyDllFrame::MyDllFrame(wxWindow *parent, const wxString& label)
    : wxFrame(parent, wxID_ANY, label)
{
    wxPanel *p = new wxPanel(this, wxID_ANY);
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add
           (
               new wxStaticText
                   (
                       p, wxID_ANY,
                       wxString::Format
                       (
                           "Running using %s\n"
                           "wxApp instance is %p, thread ID %ld",
                           wxVERSION_STRING,
                           wxApp::GetInstance(),
                           wxThread::GetCurrentId()
                       )
                   ),
               wxSizerFlags(1).Expand().Border(wxALL, 10)
           );

    sizer->Add
           (
               new wxButton(p, wxID_ABOUT, "Show info"),
               wxSizerFlags(0).Right().Border(wxALL, 10)
           );

    p->SetSizerAndFit(sizer);

    wxSizer *fsizer = new wxBoxSizer(wxVERTICAL);
    fsizer->Add(p, wxSizerFlags(1).Expand());
    SetSizerAndFit(fsizer);
}

void MyDllFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("This window is running in its own thread,\n"
                 "using private wxWidgets instance compiled into the DLL.",
                 "About",
                 wxOK | wxICON_INFORMATION);
}


// ----------------------------------------------------------------------------
// MyDllApp
// ----------------------------------------------------------------------------

MyDllApp::MyDllApp()
{
    // Keep the wx "main" thread running even without windows. This greatly
    // simplifies threads handling, because we don't have to correctly
    // implement wx-thread restarting.
    //
    // Note that this only works if you don't explicitly call ExitMainLoop(),
    // except in reaction to wx_dll_cleanup()'s message. wx_dll_cleanup()
    // relies on the availability of wxApp instance and if the event loop
    // terminated, wxEntry() would return and wxApp instance would be
    // destroyed.
    //
    // Also note that this is efficient, because if there are no windows, the
    // thread will sleep waiting for a new event. We could safe some memory
    // by shutting the thread down when it's no longer needed, though.
    SetExitOnFrameDelete(false);

    Bind(wxEVT_THREAD, &MyDllApp::OnShowWindow, this, CMD_SHOW_WINDOW);
    Bind(wxEVT_THREAD, &MyDllApp::OnTerminate, this, CMD_TERMINATE);
}

void MyDllApp::OnShowWindow(wxThreadEvent& event)
{
    wxFrame *f = new MyDllFrame(nullptr, event.GetString());
    f->Show(true);
}

void MyDllApp::OnTerminate(wxThreadEvent& WXUNUSED(event))
{
    ExitMainLoop();
}


// ----------------------------------------------------------------------------
// application startup
// ----------------------------------------------------------------------------

// we can't have WinMain() in a DLL and want to start the app ourselves
wxIMPLEMENT_APP_NO_MAIN(MyDllApp);

namespace
{

// Critical section that guards everything related to wxWidgets "main" thread
// startup or shutdown.
wxCriticalSection gs_wxStartupCS;
// Handle of wx "main" thread if running, nullptr otherwise
HANDLE gs_wxMainThread = nullptr;


//  wx application startup code -- runs from its own thread
unsigned wxSTDCALL MyAppLauncher(void* event)
{
    // Note: The thread that called run_wx_gui_from_dll() holds gs_wxStartupCS
    //       at this point and won't release it until we signal it.

    // We need to pass correct HINSTANCE to wxEntry() and the right value is
    // HINSTANCE of this DLL, not of the main .exe, use this MSW-specific wx
    // function to get it. Notice that under Windows XP and later the name is
    // not needed/used as we retrieve the DLL handle from an address inside it
    // but you do need to use the correct name for this code to work with older
    // systems as well.
    const HINSTANCE
        hInstance = wxDynamicLibrary::MSWGetModuleHandle("my_dll",
                                                         &gs_wxMainThread);
    if ( !hInstance )
        return 0; // failed to get DLL's handle

    // wxIMPLEMENT_WXWIN_MAIN does this as the first thing
    wxDISABLE_DEBUG_SUPPORT();

    // We do this before wxEntry() explicitly, even though wxEntry() would
    // do it too, so that we know when wx is initialized and can signal
    // run_wx_gui_from_dll() about it *before* starting the event loop.
    wxInitializer wxinit;
    if ( !wxinit.IsOk() )
        return 0; // failed to init wx

    // Signal run_wx_gui_from_dll() that it can continue
    HANDLE hEvent = *(static_cast<HANDLE*>(event));
    if ( !SetEvent(hEvent) )
        return 0; // failed setting up the mutex

    // Run the app:
    wxEntry(hInstance);

    return 1;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// public DLL interface
// ----------------------------------------------------------------------------

extern "C"
{

void run_wx_gui_from_dll(const char *title)
{
    // In order to prevent conflicts with hosting app's event loop, we
    // launch wx app from the DLL in its own thread.
    //
    // We can't even use wxInitializer: it initializes wxModules and one of
    // the modules it handles is wxThread's private module that remembers
    // ID of the main thread. But we need to fool wxWidgets into thinking that
    // the thread we are about to create now is the main thread, not the one
    // from which this function is called.
    //
    // Note that we cannot use wxThread here, because the wx library wasn't
    // initialized yet. wxCriticalSection is safe to use, though.

    wxCriticalSectionLocker lock(gs_wxStartupCS);

    if ( !gs_wxMainThread )
    {
        HANDLE hEvent = CreateEvent
                        (
                            nullptr,  // default security attributes
                            FALSE, // auto-reset
                            FALSE, // initially non-signaled
                            nullptr   // anonymous
                        );
        if ( !hEvent )
            return; // error

        // NB: If your compiler doesn't have _beginthreadex(), use CreateThread()
        gs_wxMainThread = (HANDLE)_beginthreadex
                                  (
                                      nullptr,           // default security
                                      0,              // default stack size
                                      &MyAppLauncher,
                                      &hEvent,        // arguments
                                      0,              // create running
                                      nullptr
                                  );

        if ( !gs_wxMainThread )
        {
            CloseHandle(hEvent);
            return; // error
        }

        // Wait until MyAppLauncher signals us that wx was initialized. This
        // is because we use wxMessageQueue<> and wxString later and so must
        // be sure that they are in working state.
        WaitForSingleObject(hEvent, INFINITE);
        CloseHandle(hEvent);
    }

    // Send a message to wx thread to show a new frame:
    wxThreadEvent *event =
        new wxThreadEvent(wxEVT_THREAD, CMD_SHOW_WINDOW);
    event->SetString(title);
    wxQueueEvent(wxApp::GetInstance(), event);
}

void wx_dll_cleanup()
{
    wxCriticalSectionLocker lock(gs_wxStartupCS);

    if ( !gs_wxMainThread )
        return;

    // If wx main thread is running, we need to stop it. To accomplish this,
    // send a message telling it to terminate the app.
    wxThreadEvent *event =
        new wxThreadEvent(wxEVT_THREAD, CMD_TERMINATE);
    wxQueueEvent(wxApp::GetInstance(), event);

    // We must then wait for the thread to actually terminate.
    WaitForSingleObject(gs_wxMainThread, INFINITE);
    CloseHandle(gs_wxMainThread);
    gs_wxMainThread = nullptr;
}

} // extern "C"
