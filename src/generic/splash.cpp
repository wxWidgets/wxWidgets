/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/splash.cpp
// Purpose:     wxSplashScreen class
// Author:      Julian Smart
// Created:     28/6/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_SPLASH

#ifdef __WXGTK__
    #include "wx/gtk/private/wrapgtk.h"
#endif

#include "wx/splash.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif

#include "wx/except.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/auxwindows.h"
#endif

#include <new>
#include <unordered_map>


// ----------------------------------------------------------------------------
// wxSplashScreen
// ----------------------------------------------------------------------------

#define wxSPLASH_TIMER_ID       9999

namespace
{

struct SplashRuntimeState
{
    wxWeakRef<wxWindow> splashLifetime;
    wxWindow* contentAddress = nullptr;
    wxWeakRef<wxWindow> contentLifetime;
    bool filterRegistered = false;
    bool closing = false;
};

using SplashRuntimeStates =
    std::unordered_map<const wxSplashScreen*, SplashRuntimeState>;

SplashRuntimeStates& GetSplashRuntimeStates()
{
    static SplashRuntimeStates* const states = new SplashRuntimeStates;
    return *states;
}

SplashRuntimeState* FindSplashRuntimeState(
    const wxSplashScreen* const splash)
{
    SplashRuntimeStates& states = GetSplashRuntimeStates();
    const SplashRuntimeStates::iterator it = states.find(splash);
    return it != states.end() && it->second.splashLifetime.get() == splash
        ? &it->second
        : nullptr;
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

wxWinUISplashHookForTesting gs_splashHookForTesting = nullptr;
bool gs_failNextSplashRuntimeAllocationForTesting = false;

void MaybeFailSplashRuntimeAllocationForTesting()
{
    if ( gs_failNextSplashRuntimeAllocationForTesting )
    {
        gs_failNextSplashRuntimeAllocationForTesting = false;
#if wxUSE_EXCEPTIONS
        throw std::bad_alloc();
#endif
    }
}

void InvokeSplashHookForTesting(
    wxSplashScreen* const splash,
    const wxWinUISplashHookPointForTesting point)
{
    if ( gs_splashHookForTesting )
        gs_splashHookForTesting(splash, point);
}

#endif // __WXWINUI__ && wxUSE_WINUI3

SplashRuntimeState& ResetSplashRuntimeState(wxSplashScreen* const splash)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    MaybeFailSplashRuntimeAllocationForTesting();
#endif
    SplashRuntimeState& state = GetSplashRuntimeStates()[splash];
    state.splashLifetime = wxWeakRef<wxWindow>(splash);
    state.contentAddress = nullptr;
    state.contentLifetime.Release();
    state.filterRegistered = false;
    state.closing = false;
    return state;
}

SplashRuntimeState& GetSplashRuntimeState(wxSplashScreen* const splash)
{
    SplashRuntimeState& state = GetSplashRuntimeStates()[splash];
    if ( state.splashLifetime.get() != splash )
        return ResetSplashRuntimeState(splash);
    return state;
}

void SetSplashContent(wxSplashScreen* const splash, wxWindow* const content)
{
    SplashRuntimeState& state = GetSplashRuntimeState(splash);
    state.contentAddress = content;
    state.contentLifetime = wxWeakRef<wxWindow>(content);
}

wxWindow* GetSplashContent(wxSplashScreen* const splash,
                           wxWindow* const expected)
{
    SplashRuntimeState& state = GetSplashRuntimeState(splash);
    return state.contentAddress == expected &&
           state.contentLifetime.get() == expected
                ? expected
                : nullptr;
}

} // anonymous namespace

#if defined(__WXWINUI__) && wxUSE_WINUI3

void wxWinUISetSplashHookForTesting(wxWinUISplashHookForTesting hook)
{
    gs_splashHookForTesting = hook;
}

void wxWinUIResetSplashHookForTesting()
{
    gs_splashHookForTesting = nullptr;
}

void wxWinUIFailNextSplashRuntimeAllocationForTesting()
{
    gs_failNextSplashRuntimeAllocationForTesting = true;
}

std::size_t wxWinUIGetSplashRuntimeStateCountForTesting()
{
    return GetSplashRuntimeStates().size();
}

#endif // __WXWINUI__ && wxUSE_WINUI3

wxIMPLEMENT_DYNAMIC_CLASS(wxSplashScreen, wxFrame);
wxBEGIN_EVENT_TABLE(wxSplashScreen, wxFrame)
    EVT_TIMER(wxSPLASH_TIMER_ID, wxSplashScreen::OnNotify)
    EVT_CLOSE(wxSplashScreen::OnCloseWindow)
wxEND_EVENT_TABLE()

void wxSplashScreen::Init()
{
    ResetSplashRuntimeState(this);
    m_window = nullptr;
    m_splashStyle = wxSPLASH_NO_CENTRE | wxSPLASH_NO_TIMEOUT;
    m_milliseconds = 0;
}

/* Note that unless we pass a non-default size to the frame, SetClientSize
 * won't work properly under Windows, and the splash screen frame is sized
 * slightly too small.
 */

wxSplashScreen::wxSplashScreen(const wxBitmap& bitmap, long splashStyle, int milliseconds,
                               wxWindow* parent, wxWindowID id, const wxPoint& pos,
                               const wxSize& size, long style)
    : wxFrame(parent, id, wxEmptyString, wxPoint(0,0), wxSize(100, 100),
              style | wxFRAME_TOOL_WINDOW | wxFRAME_NO_TASKBAR)
{
    Init();

    const wxWeakRef<wxWindow> weakThis(this);
    const wxWeakRef<wxWindow> weakParent(parent);
    const auto ownerContextIsValid =
        [this, parent, weakThis, weakParent]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   (!parent ||
                    wxWeakWindowIsAvailableForCallbacks(weakParent, parent)) &&
                   GetParent() == parent;
        };

    // Construction crosses several callback-bearing native boundaries. Keep
    // every provisional resource private until the final Update() has been
    // revalidated, and make both an ordinary early return and exception
    // unwinding remove the global filter before this object's vtables vanish.
    const auto rollbackConstruction =
        [this, weakThis](const bool unwinding)
        {
            if ( weakThis.get() != this )
                return;

            m_timer.Stop();

            SplashRuntimeState* const runtime =
                FindSplashRuntimeState(this);
            wxWindow* content = nullptr;
            wxWeakRef<wxWindow> contentLifetime;
            if ( runtime )
            {
                if ( runtime->filterRegistered )
                {
                    // Set the ownership bit first: RemoveFilter() itself is
                    // not a callback, and a nested destructor must never try
                    // to unlink the same intrusive node twice.
                    runtime->filterRegistered = false;
                    wxEvtHandler::RemoveFilter(this);
                }

                if ( runtime->contentAddress &&
                     runtime->contentLifetime.get() ==
                         runtime->contentAddress )
                {
                    content = runtime->contentAddress;
                    contentLifetime = runtime->contentLifetime;
                }
                runtime->contentAddress = nullptr;
                runtime->contentLifetime.Release();
            }
            m_window = nullptr;

            // During exception unwinding wxFrame's base destructor owns the
            // native shell and its children. Avoid introducing another
            // callback there. For an ordinary failed transaction, leave no
            // visible or independently live provisional content behind.
            if ( !unwinding &&
                 !wxWindowItselfIsUnavailableForCallbacks(this) )
            {
                if ( IsShown() )
                    (void)wxFrame::Show(false);

                if ( weakThis.get() == this && content &&
                     contentLifetime.get() == content &&
                     !wxWindowIsUnavailableForCallbacks(content) )
                {
                    (void)content->Destroy();
                }
            }

            // A throwing constructor never runs ~wxSplashScreen(), so this
            // erase is part of the transaction rather than destructor polish.
            GetSplashRuntimeStates().erase(this);
        };

    const auto initialize = [&]() -> bool
    {
        // splash screen must not be used as parent by the other windows
        // because it is going to disappear soon, indicate it by giving it
        // this special style.
        SetExtraStyle(GetExtraStyle() | wxWS_EX_TRANSIENT);
        if ( !ownerContextIsValid() )
            return false;

#if defined(__WXGTK__)
        gtk_window_set_type_hint(GTK_WINDOW(m_widget),
                                 GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
#endif

        m_splashStyle = splashStyle;
        // Preserve the public value exactly. Only the native one-shot timer
        // needs a positive delay; GetTimeout() historically reports what the
        // caller supplied, including zero.
        m_milliseconds = milliseconds;

        wxSplashScreenWindow* const content =
            new wxSplashScreenWindow(bitmap, this, wxID_ANY,
                                     pos, size, wxNO_BORDER);
        const wxWeakRef<wxWindow> weakContent(content);
        m_window = content;
        SetSplashContent(this, content);
        const auto splashContextIsValid =
            [&, content, weakContent]()
            {
                return ownerContextIsValid() &&
                       wxWeakWindowIsAvailableForCallbacks(weakContent,
                                                           content) &&
                       content->GetParent() == this &&
                       GetSplashContent(this, m_window) == content;
            };

#if defined(__WXWINUI__) && wxUSE_WINUI3
        InvokeSplashHookForTesting(
            this, wxWinUISplashHookPointForTesting::AfterContent);
#endif
        if ( !splashContextIsValid() )
            return false;

        const wxSize bitmapSize = bitmap.GetLogicalSize();
        SetClientSize(bitmapSize);
        if ( !splashContextIsValid() )
            return false;

        if (m_splashStyle & wxSPLASH_CENTRE_ON_PARENT)
            CentreOnParent();
        else if (m_splashStyle & wxSPLASH_CENTRE_ON_SCREEN)
            CentreOnScreen();
        if ( !splashContextIsValid() )
            return false;

        if (m_splashStyle & wxSPLASH_TIMEOUT)
        {
            m_timer.SetOwner(this, wxSPLASH_TIMER_ID);
            if ( !splashContextIsValid() )
                return false;
            m_timer.Start(wxMax(1, m_milliseconds), true);
#if defined(__WXWINUI__) && wxUSE_WINUI3
            InvokeSplashHookForTesting(
                this, wxWinUISplashHookPointForTesting::AfterTimerStart);
#endif
            if ( !splashContextIsValid() )
                return false;
        }

        // Publish before insertion. AddFilter() is currently non-throwing,
        // but this ordering also covers a future allocation/callback between
        // linking the intrusive node and returning to us.
        SplashRuntimeState* const runtime =
            FindSplashRuntimeState(this);
        if ( !runtime )
            return false;
        runtime->filterRegistered = true;
        wxEvtHandler::AddFilter(this);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        InvokeSplashHookForTesting(
            this,
            wxWinUISplashHookPointForTesting::AfterFilterRegistration);
#endif
        if ( !splashContextIsValid() )
            return false;

        if ( !Show(true) )
            return false;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        InvokeSplashHookForTesting(
            this, wxWinUISplashHookPointForTesting::AfterShow);
#endif
        if ( !splashContextIsValid() )
            return false;

        content->SetFocus();
        if ( !splashContextIsValid() )
            return false;
#if defined( __WXMSW__ ) || defined(__WXMAC__)
        Update(); // Without this, you see a blank screen for an instant
#elif defined(__WXGTK__)
        // we don't need to do anything at least on wxGTK with GTK+ 2.12.9
#else
        wxYieldIfNeeded(); // Should eliminate this
#endif
#if defined(__WXWINUI__) && wxUSE_WINUI3
        // Realizing the WinUI non-client frame can change the client extent
        // by one physical pixel after the pre-show SetClientSize() above.
        // Re-fit only once the native/XAML shell has crossed that boundary,
        // and treat the resize as another callback-bearing transaction step.
        if ( GetClientSize() != bitmapSize )
        {
            SetClientSize(bitmapSize);
            if ( !splashContextIsValid() ||
                 GetClientSize() != bitmapSize )
            {
                return false;
            }
        }
#endif
#if defined(__WXWINUI__) && wxUSE_WINUI3
        InvokeSplashHookForTesting(
            this, wxWinUISplashHookPointForTesting::AfterUpdate);
#endif
        // Update()/wxYieldIfNeeded() can synchronously run arbitrary user code.
        // It is the final boundary, not a fire-and-forget paint request.
        return splashContextIsValid();
    };

    wxTRY
    {
        if ( !initialize() )
            rollbackConstruction(false);
    }
    wxCATCH_ALL(
    {
        // This catch exists solely to repair intrusive/global ownership before
        // preserving the original exception. It does not translate or mask it.
        rollbackConstruction(true);
        throw;
    })
}

wxSplashScreen::~wxSplashScreen()
{
    m_timer.Stop();

    SplashRuntimeStates::iterator it = GetSplashRuntimeStates().find(this);
    if ( it != GetSplashRuntimeStates().end() &&
         it->second.splashLifetime.get() == this &&
         it->second.filterRegistered )
    {
        it->second.filterRegistered = false;
        wxEvtHandler::RemoveFilter(this);
    }
    GetSplashRuntimeStates().erase(this);
}

int wxSplashScreen::FilterEvent(wxEvent& event)
{
    if ( wxWindowIsUnavailableForCallbacks(this) )
        return -1;

    const wxEventType t = event.GetEventType();
    if ( t == wxEVT_KEY_DOWN ||
            t == wxEVT_LEFT_DOWN ||
                t == wxEVT_RIGHT_DOWN ||
                    t == wxEVT_MIDDLE_DOWN )
        Close(true);

    return -1;
}

void wxSplashScreen::OnNotify(wxTimerEvent& WXUNUSED(event))
{
    if ( !wxWindowIsUnavailableForCallbacks(this) )
        Close(true);
}

void wxSplashScreen::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    if ( wxWindowItselfIsUnavailableForCallbacks(this) )
        return;

    SplashRuntimeState& runtime = GetSplashRuntimeState(this);
    if ( runtime.closing )
        return;

    const wxWeakRef<wxWindow> weakThis(this);
    runtime.closing = true;
    const wxScopeGuard leaveClose = wxMakeGuard(
        [this, weakThis]()
        {
            if ( weakThis.get() == this )
                GetSplashRuntimeState(this).closing = false;
        });
    wxUnusedVar(leaveClose);

    m_timer.Stop();
    if ( weakThis.get() == this &&
         !wxWindowItselfIsUnavailableForCallbacks(this) )
    {
        Destroy();
    }
}

// ----------------------------------------------------------------------------
// wxSplashScreenWindow
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxSplashScreenWindow, wxWindow)
    EVT_PAINT(wxSplashScreenWindow::OnPaint)
wxEND_EVENT_TABLE()

wxSplashScreenWindow::wxSplashScreenWindow(const wxBitmap& bitmap, wxWindow* parent,
                                           wxWindowID id, const wxPoint& pos,
                                           const wxSize& size, long style)
    : wxWindow(parent, id, pos, size, style)
    , m_bitmap(bitmap)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

#if !defined(__WXGTK__) && wxUSE_PALETTE
    bool hiColour = (wxDisplayDepth() >= 16) ;

    if (bitmap.GetPalette() && !hiColour)
    {
        SetPalette(* bitmap.GetPalette());
    }
#endif
}

void wxSplashScreenWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    if (m_bitmap.IsOk())
        dc.DrawBitmap(m_bitmap, 0, 0, true);
}

#endif // wxUSE_SPLASH
