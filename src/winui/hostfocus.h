/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hostfocus.h
// Purpose:     Private focus query shared with host reparent orchestration
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_HOSTFOCUS_H_
#define _WX_WINUI_HOSTFOCUS_H_

#include "private.h"

// No cached focus or owner state. The host's existing migration/intent records
// remain authoritative, and callers revalidate after this WinRT boundary.
bool wxWinUIFocusIsInside(
    const winrt::Microsoft::UI::Xaml::FrameworkElement& container,
    const winrt::Microsoft::UI::Xaml::XamlRoot& root);

#endif // _WX_WINUI_HOSTFOCUS_H_
