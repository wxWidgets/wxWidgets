///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/private/uilocale.h
// Purpose:     Helper for making pointer to current NSLocale available
//              for locale-dependent controls under macOS.
// Author:      Ulrich Telle
// Created:     2023-10-13
// Copyright:   (c) 2023 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_PRIVATE_UILOCALE_H_
#define _WX_OSX_PRIVATE_UILOCALE_H_

#if wxUSE_INTL

// Function returning a pointer to the current NSLocale
WXDLLIMPEXP_BASE NSLocale* wxGetCurrentNSLocale();

#endif

#endif // _WX_OSX_PRIVATE_UILOCALE_H_
