/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostvisual.h
// Purpose:     Host-owned visual implementations shared by private adapters
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTVISUAL_H_
#define _WX_WINUI_HOSTVISUAL_H_

#include "private.h"
#include <winrt/Microsoft.UI.Input.h>

class wxWinUISlot;
class wxWinUITopLevelHost;

// The slot containers are Grids extended with cursor support: ProtectedCursor
// is only reachable from a derived element, and it is how a per-window
// wxWindow::SetCursor() (e.g. a wait cursor on one control) is reflected over
// the island, which otherwise insists on its own arrow.
struct wxWinUISlotGrid
    : winrt::Microsoft::UI::Xaml::Controls::GridT<wxWinUISlotGrid>
{
    bool SetWindowCursor(winrt::Microsoft::UI::Input::InputCursor const& cursor)
    {
        try
        {
            // A null cursor means "use the default arrow".
            ProtectedCursor(cursor);
            return true;
        }
        catch ( const winrt::hresult_error& )
        {
            // Not being able to set the cursor is never fatal.
            return false;
        }
    }

    winrt::Microsoft::UI::Input::InputCursor GetWindowCursor() const
    {
        try
        {
            return ProtectedCursor();
        }
        catch ( const winrt::hresult_error& )
        {
            return nullptr;
        }
    }
};

// Root canvas subclass with cursor access, so the island can mirror the
// native cursor of whatever wx window the pointer hovers.
struct wxWinUIRootCanvas
    : winrt::Microsoft::UI::Xaml::Controls::CanvasT<wxWinUIRootCanvas>
{
    bool SetPointerCursor(winrt::Microsoft::UI::Input::InputCursor const& cursor)
    {
        try
        {
            ProtectedCursor(cursor);
            return true;
        }
        catch ( const winrt::hresult_error& )
        {
            return false;
        }
    }
};

// The maps and their lifetime stay in tlwhost.cpp. These passive copies
// pin the same implementation across reentrant setters; no lookup creates
// a peer, mutates a registry, or calls WinRT.
winrt::com_ptr<wxWinUISlotGrid> wxWinUIGetSlotGrid(const wxWinUISlot *slot);
winrt::com_ptr<wxWinUIRootCanvas>
wxWinUIGetRootCanvas(const wxWinUITopLevelHost *host);

#endif // _WX_WINUI_HOSTVISUAL_H_
