/////////////////////////////////////////////////////////////////////////////
// Name:        webview.cpp
// Purpose:     wxWebViewChromium sample proxy
// Author:      Tobias Taschner
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
   The chromium sample is identical to the webview sample but needs different
   libraries to be linked and uses a different project/build files
*/

#include "wx/wx.h"

#if !wxUSE_WEBVIEW_CHROMIUM
#error "A wxWebView Chromium backend is required by this sample"
#endif

// Include the default webview sample
#define wxWEBVIEW_SAMPLE_CHROMIUM
#include "../webview/webview.cpp"
