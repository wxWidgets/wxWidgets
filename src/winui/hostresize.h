/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostresize.h
// Purpose:     Private state of the host-owned native resize transaction
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTRESIZE_H_
#define _WX_WINUI_HOSTRESIZE_H_

#include "wx/winui/private/tlwhost.h"

// Records only the real native entry/exit notifications for one request.
// Calling Enter/Exit does not initiate USER32 tracking or publish host state.
struct wxWinUINativeResizeProgress
{
    bool Enter(bool dispatching)
    {
        if ( !dispatching || entered )
            return false;
        entered = true;
        return true;
    }

    bool Exit()
    {
        if ( !entered || exited )
            return false;
        exited = true;
        return true;
    }

    bool entered = false;
    bool exited = false;
};

// The host keeps the single shared request. Root routing and slot retirement
// inspect this same state; this header introduces no second owner or model.
struct wxWinUITopLevelHost::NativeResizeRequest
    : wxWinUINativeResizeProgress
{
    wxWeakRef<wxWindow> source;
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::UIElement> grip;
    wxWinUINativeTarget target;
    wxWinUIPointerSample sample;
    std::function<bool ()> isCurrent;
    std::uint64_t ticket = 0;
    std::uint32_t inputSiteGeneration = 0;
    int hitTest = HTNOWHERE;
    void *subclassContext = nullptr;
    bool preparing = false;
    bool accepted = false;
    bool dispatching = false;
    bool cancelled = false;
};

// The bridge receives the private wake; dispatch stays in the resize module.
UINT wxWinUINativeResizeMessage();

#endif // _WX_WINUI_HOSTRESIZE_H_
