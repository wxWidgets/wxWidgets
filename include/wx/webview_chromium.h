/////////////////////////////////////////////////////////////////////////////
// Author: Steven Lamerton
// Copyright: (c) 2013 Steven Lamerton
// Licence: wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBVIEWCHROMIUM_H_
#define _WX_WEBVIEWCHROMIUM_H_

#include <wx/wxprec.h>
#include <include/cef_version.h>

#if !defined(CEF_VERSION_MAJOR) || \
    (defined(CEF_VERSION_MAJOR) && CEF_VERSION_MAJOR == 1)
#define CEF_API 1
#elif defined(CEF_VERSION_MAJOR) && CEF_VERSION_MAJOR == 3
#define CEF_API 3
#else
#error "Could not find CEF"
#endif

#if CEF_API == 1
#include "webview_chromium1.h"
#elif CEF_API == 3
#include "webview_chromium3.h"
#endif

#endif // _WX_WEBVIEWCHROMIUM_H_
