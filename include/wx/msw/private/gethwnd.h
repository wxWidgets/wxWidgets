///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/gethwnd.h
// Purpose:     Private header for getting HWND from wxWindow
// Author:      Vadim Zeitlin
// Created:     2025-03-30
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_GETHWND_H_
#define _WX_MSW_PRIVATE_GETHWND_H_

#ifdef __WXQT__
    // This is needed to call winId()
    #include <QtWidgets/QWidget>
#endif // __WXQT__

// Just return the associated HWND if the window is not null.
//
// This is not completely trivial because we want this function to also work
// when compiling MSW-specific code as part of wxQt.
inline HWND wxGetHWND(const wxWindow* parent)
{
#ifdef __WXMSW__
    return parent ? (HWND)parent->GetHandle() : nullptr;
#elif defined(__WXQT__)
    return parent ? (HWND)parent->GetHandle()->winId() : nullptr;
#endif
}

#endif // _WX_MSW_PRIVATE_GETHWND_H_
