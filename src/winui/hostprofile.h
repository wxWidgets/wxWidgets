/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostprofile.h
// Purpose:     Private shared declarations for existing host profiling
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTPROFILE_H_
#define _WX_WINUI_HOSTPROFILE_H_

#include "wx/defs.h"
#include <cstdio>

// Intracore observations only, not a new service or public tracing interface.
// Every function and counter has exactly one definition in tlwhost.cpp.
// The phases of one pointer sample, in the order they run.
enum class wxWinUIProfilePhase
{
    Total,
    Point,        // args.GetCurrentPoint() and the properties it exposes
    XamlHit,      // VisualTreeHelper hit test against the island content
    NativeHit,    // resolving the child window under the pointer
    Dispatch,     // the state machine and the SendMessage it ends with
    Mirror,       // mirroring the native cursor for the hit target
    MirrorSend,   // the WM_SETCURSOR the target runs
    MirrorCheck,  // re-proving the target still owns the point
    MirrorApply,  // converting and applying the island cursor
    Refresh,      // re-proving the native hit before delivery
    Send,         // the WM_MOUSEMOVE the target finally receives
    Cursor,       // observing and re-applying the XAML pointer cursor
    Max
};

// Host-wide invalidations seen while routing pointer input: one of these
// per pointer move means every hosted control is re-synchronised for
// every movement of the mouse.
extern unsigned gs_profileCursorPolicy;
extern unsigned gs_profileLayoutNotify;
extern unsigned gs_profileLayoutSuppressed;
extern unsigned gs_profileMarkAllDirty;
extern unsigned gs_profileMarkDirty;
extern unsigned gs_profileFlushRuns;
extern unsigned gs_profileSamples;

FILE *wxWinUIInputProfileFile();
bool wxWinUIInputProfileEnabled();
long long wxWinUIProfileNow();
void wxWinUIProfileAdd(wxWinUIProfilePhase phase, long long started);
void wxWinUIProfileSample();

// Times one phase for as long as it is in scope.
class wxWinUIProfileScope
{
public:
    explicit wxWinUIProfileScope(wxWinUIProfilePhase phase)
        : m_phase(phase),
          m_started(wxWinUIInputProfileEnabled() ? wxWinUIProfileNow() : 0)
    {
    }

    ~wxWinUIProfileScope()
    {
        if ( m_started )
            wxWinUIProfileAdd(m_phase, m_started);
    }

private:
    const wxWinUIProfilePhase m_phase;
    const long long m_started;

    wxDECLARE_NO_COPY_CLASS(wxWinUIProfileScope);
};

#endif // _WX_WINUI_HOSTPROFILE_H_
