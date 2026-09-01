/////////////////////////////////////////////////////////////////////////////
// Intracore OLE broker native operations. Not installed.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_DROPBROKER_STATE_H
#define WX_WINUI_DROPBROKER_STATE_H

#include "wx/winui/private/dropbroker.h"

#if wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP

// Production uses the real OLE operations. Only the non-installed test factory
// can replace this table; its storage has the same layout in ON/OFF builds.
struct wxWinUIDropBrokerNativeOperations
{
    void* context = nullptr;
    HRESULT (*lock)(void*, IUnknown*, BOOL, BOOL) noexcept = nullptr;
    HRESULT (*registerTarget)(void*, HWND, IDropTarget*) noexcept = nullptr;
    HRESULT (*revokeTarget)(void*, HWND) noexcept = nullptr;
};

#endif
#endif
