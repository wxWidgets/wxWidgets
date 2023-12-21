/////////////////////////////////////////////////////////////////////////////
// Name:        samples/webview/cef_process_helper.cpp
// Purpose:     Simplest possible CEF helper.
// Author:      Haojian Wu <hokein.wu@gmail.com>
// Created:     2014-06-22
// Copyright:   (c) 2014 - 2023 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// This application doesn't use wxWidgets at all but does require CEF and its
// wrapper DLL.
//
// To build it using MSVC under MSW use
//
//      > cl /Fewebview_cef_helper /MD /DNDEBUG /EHsc /I../../3rdparty/cef cef_process_helper.cpp ../../3rdparty/cef/Release/libcef.lib ../../3rdparty/cef/Release/libcef_dll_wrapper.lib
//
// in release mode and
//
//      > cl /Fewebview_cef_helper /MDd /D_DEBUG /EHsc /I../../3rdparty/cef cef_process_helper.cpp ../../3rdparty/cef/Debug/libcef.lib ../../3rdparty/cef/Debug/libcef_dll_wrapper.lib
//
// in debug.
//
// Under Unix use
//
//      $ c++ -o webview_cef_helper -I../../3rdparty/cef cef_process_helper.cpp -L../../3rdparty/cef/Release -lcef -L../../3rdparty/cef/libcef_dll_wrapper -lcef_dll_wrapper
//
// and under macOS see the commands executed by the sample makefile.

#include "include/cef_app.h"
#if defined(__APPLE__)
    #include "include/wrapper/cef_library_loader.h"
#endif

// Define simplest possible process entry point just forwarding to CEF.
#if defined(_WIN32)

extern "C" int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE, char*, int)
{
    CefMainArgs cefArgs(hInstance);

    return CefExecuteProcess(cefArgs, nullptr, nullptr);
}

#else // !Windows

int main(int argc, char* argv[])
{
#if defined(__APPLE__)
    // Load the CEF framework library at runtime instead of linking directly
    // as required by the macOS sandbox implementation.
    CefScopedLibraryLoader library_loader;
    if (!library_loader.LoadInHelper()) {
        return 1;
    }
#endif

    CefMainArgs cefArgs(argc, argv);

    return CefExecuteProcess(cefArgs, nullptr, nullptr);
}

#endif // Windows/!Windows
