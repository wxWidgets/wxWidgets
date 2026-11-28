/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/visualcoordinates.h
// Purpose:     Intracore visual-tree and coordinate predicates
// Author:      wxWidgets development team
// Created:     2026-08-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_VISUALCOORDINATES_H_
#define _WX_WINUI_VISUALCOORDINATES_H_

// This non-installed header shares only the predicates used by the host
// and coordinate mapper. It owns no state and exposes no DLL interface.
namespace winrt
{
namespace Microsoft
{
namespace UI
{
namespace Xaml
{
struct UIElement;
}
}
}
namespace Windows
{
namespace Foundation
{
struct Point;
}
}
}

bool wxWinUIElementIsInSubtree(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const winrt::Microsoft::UI::Xaml::UIElement& root);

bool wxWinUIIsFinitePoint(
    const winrt::Windows::Foundation::Point& point);

#endif // _WX_WINUI_VISUALCOORDINATES_H_
