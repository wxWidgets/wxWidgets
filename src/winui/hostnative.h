/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostnative.h
// Purpose:     Intracore native boundaries shared by host modules
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTNATIVE_H_
#define _WX_WINUI_HOSTNATIVE_H_

#include "wx/msw/wrapwin.h"
#include <commctrl.h>
#include <cstdint>
#include <memory>

class wxWinUIHostLifetime;
enum class wxWinUIInputDevice;
struct wxWinUIPointerSample;

// One shared context protocol for the bridge and native resize observer.
// The host TU owns the counter and failed-removal retirement implementation.
// These declarations are intracore only: this header is never installed.
struct wxWinUISubclassContext
{
    explicit wxWinUISubclassContext(
        const std::shared_ptr<wxWinUIHostLifetime>& hostState);
    ~wxWinUISubclassContext();

    std::weak_ptr<wxWinUIHostLifetime> host;
    bool active = true;
};

void wxWinUIRetireSubclass(HWND hwnd,
                          SUBCLASSPROC proc,
                          UINT_PTR subclassId,
                          void *&opaqueContext,
                          const char *description);
void wxWinUIInputLog(const char *fmt, ...);
// Step 3: every window which is not shown by a XAML control has a canvas,
// which takes no input, and the window whose canvas XAML finds topmost under
// the pointer is where the native target is looked for, see
// wxWinUIResolveNativeHitFrom(). WX_WINUI_INPUT_NODES=0 brings back the walk
// through the whole tree of windows; =2 does both and writes each
// disagreement to the input log.
bool wxWinUIInputNodesEnabled();
bool wxWinUIInputNodesCompared();
// Changes with each message taken from the queue: what cannot change while
// one message is being processed can be remembered against it.
unsigned long long wxWinUIGetMessagePumpSerial();
bool wxWinUIInputLogEnabled();
bool wxWinUIIsPointerContactActive(wxWinUIInputDevice device,
                                  std::uint32_t pointerId,
                                  int virtualKey);

// Read-only GUI-thread query. The global operation counter and its scheduling
// tail remain owned by tlwhost.cpp; the resize module never copies the depth.
bool wxWinUIIsGlobalOperationActive();

// A synchronous USER32 sizing loop owns its tracking packets, not XAML's
// ContentPreTranslateMessage. The resize module admits only the exact live
// target/capture while its stack-scoped native handoff is entered.
bool wxWinUIMessageBelongsToNativeSizing(MSG& message);

// Transport an observed XAML move to the already-entered USER32 sizing loop.
// This does not initiate sizing, mutate geometry, or inject physical input.
void wxWinUIQueueNativeSizingMove(
    const std::shared_ptr<wxWinUIHostLifetime>& lifetime,
    const wxWinUIPointerSample& sample);

#endif // _WX_WINUI_HOSTNATIVE_H_
