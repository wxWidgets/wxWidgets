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
bool wxWinUIIsPointerContactActive(wxWinUIInputDevice device,
                                  std::uint32_t pointerId,
                                  int virtualKey);

// Read-only GUI-thread query. The global operation counter and its scheduling
// tail remain owned by tlwhost.cpp; the resize module never copies the depth.
bool wxWinUIIsGlobalOperationActive();

#endif // _WX_WINUI_HOSTNATIVE_H_
