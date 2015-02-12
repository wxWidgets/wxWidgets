/////////////////////////////////////////////////////////////////////////////
// Name:        samples/webview_chromium/mac/process_helper_mac.cpp
// Purpose:     webview_chromium launch helper
// Author:      Haojian Wu <hokein.wu@gmail.com>
// Created:     2014-06-22
// Copyright:   (c) 2014 - 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "include/cef_app.h"
#include "include/cef_version.h"

// Process entry point.
int main(int argc, char* argv[]) {
  CefMainArgs main_args(argc, argv);

  // Execute the secondary process.
#if CHROME_VERSION_BUILD >= 1750
  return CefExecuteProcess(main_args, NULL, NULL);
#else
  return CefExecuteProcess(main_args, NULL);
#endif
}
