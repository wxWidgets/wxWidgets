/////////////////////////////////////////////////////////////////////////////
// Name:        webview_chromium.h
// Purpose:     interface of wxWebViewChromium
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxWebViewChromium

    wxWebViewChromium is a Chromium-based backend for wxWebView using the
    Chromium Embedded Framework (CEF).

    This backend is only available for Windows, Linux and macOS and currently
    requires CEF version 116.

    Using CEF also requires a compiler with C++14 support.

    @section requirements Building CEF

    If you are compiling your own copy of CEF then your compile flags must
    match those you are using to compile wxWidgets. Specifically on Windows
    it is likely you will need to adjust the following properties in the CEF
    provided build files:

    - C/C++ - Code Generation - Runtime Library - Multithreaded [Debug] DLL
    - C/C++ - Code Generation - Enable C++ Exceptions - Yes
    - C/C++ - Language - Enable Run-Time Type Information - Yes

    @section instructions Build Instructions

    __General__

    The wxWebViewChromium backend is built into a separate webview_chromium
    library which depends on the webview library.

    When building with CMake enable wxUSE_WEBVIEW_CHROMIUM and CEF can be
    automatically downloaded during configuration based on your platform.
    You can also download it manually from the CEF official binaries site at
    https://cef-builds.spotifycdn.com/index.html and then build only
    `libcef_dll_wrapper` -- but not the `libcef` itself.

    For other build systems please follow the following instructions:

    Once you have a copy of CEF, either from compiling it yourself or using
    prebuilt binaries, you should copy it into `wx_root/3rdparty/cef`. To run the
    webview_chromium sample you need to copy the CEF resources into the
    sample directory. The following files need to be copied:

    - All files from either `wx_root/3rdparty/cef/Debug` or
      `wx_root/3rdparty/cef/Release` folder, depending on your build type
    - The contents of the wx_root/3rdparty/cef/Resources folder.

    Please see CEF `README.txt` for more details and also note that when
    redistributing CEF you must abide by the terms of its `LICENSE.txt`.

    When building wxWidgets you need to ensure wxWebViewChromium is enabled,
    either by passing --enable-webviewchromium for autoconf based builds or
    setting wxUSE_WEBVIEW_CHROMIUM equal to 1 in setup.h for Visual Studio
    based builds.

    When running applications using wxWebViewChromium, the command line option
    `--type=xxx` is interpreted specially as it is used by CEF to launch helper
    applications, so your program must not use this option for anything else.


    __Microsoft Windows Platform__

    Windows 10 or newer is required to run applications using wxWebViewChromium.
    Such applications should also have an application manifest declaring compatibility
    with supported Windows versions.

    Microsoft Visual C++ 2022 must be used to build wxWebViewChromium.

    __Linux with GTK__

    wxWebviewChromium currently only supports X11 and not Wayland, i.e. you
    need to either ensure that `GDK_BACKEND` environment variable is set to
    "x11" before running your program using it or call

    @code
    gdk_set_allowed_backends("x11")
    @endcode

    in your application code.

    Moreover, the actual browser is only created once the window is shown, and
    can't be used until then. You can bind an event handler for wxEVT_CREATE to
    know when it is usable.


    __Mac OS X Platform__

    OS X 10.13 or above is required.

    Due to the application bundle structure on OS X, wxWebviewChromium is a
    little more complicated than on Windows/Linux platforms as extra helper
    applications for executing separate Chromium processes(renderer, plugin,
    etc) are required.

    For applications using wxWebviewChromium, below are the steps to make
    it work, based off the webview_chromium sample bakefile.

    1. Build the webview_chromium library.
    2. Compile/link/package the `YourApp helper` app:
       - Require `cef_process_helper.cpp`
       - Require link frameworks: Chromium Embedded Framework.framework,
         AppKit.frameworks.
       - Require app bundle configuration: Info.plist
       - Use system tool `install_name_tool -change` to correct `Chromium
         Embedded Framework.framework/Chromium Embedded Framework` location.
    3. Compile/link/package `YourApp` itself as usual under macOS.
    4. Create a `Contents/Frameworks` directory in `YourApp.app` bundle
       and copy `Chromium Embedded Framework` and `YourApp helper.app` into it.
    5. Create clones of `YourApp helper.app`, in the same directory, with the
       names `YourApp helper (Alerts).app`, `YourApp helper (GUI).app` etc. It
       is possible to use symbolic links to `YourApp helper` binary for the
       executable files of these extra helpers.

    Below is the wxWebviewChromium sample app bundle directory structure,
    omitting some resource files

        webview_chromium.app
        |____Contents
          |____MacOS
          | |____webview_chromium
          |____Resources
          | |____wxmac.icns
          |____Frameworks
          | |____webview_chromium Helper.app
          | | |____Contents
          | |   |____MacOS
          | |   | |____webview_chromium Helper
          | |   |____Resources
          | |   | |____wxmac.icns
          | |   |____Info.plist
          | |____webview_chromium Helper (GPU).app
          | | |____Contents
          | |   |____MacOS
          | |   | |____webview_chromium Helper (GPU)
          | |   |____Resources
          | |   | |____wxmac.icns
          | |   |____Info.plist
          | |____webview_chromium Helper (Plugin).app
          | | |____Contents
          | |   |____MacOS
          | |   | |____webview_chromium Helper (Plugin)
          | |   |____Resources
          | |   | |____wxmac.icns
          | |   |____Info.plist
          | |____Chromium Embedded Framework.framework
          | | |____Resources
          | | | |____chrome_200_percent.pak
          | | | |____chrome_100_percent.pak
          | | | |____icudtl.dat
          | | | |____snapshot_blob.bin
          | | | |____v8_context_snapshot.arm64.bin
          | | | |____resources.pak
          | | | |____gpu_shader_cache.bin
          | | | |____Info.plist
          | | |____Libraries
          | | | |____libEGL.dylib
          | | | |____vk_swiftshader_icd.json
          | | | |____libvk_swiftshader.dylib
          | | | |____libGLESv2.dylib
          | | |____Chromium Embedded Framework
          | |____webview_chromium Helper (Renderer).app
          | | |____Contents
          | |   |____MacOS
          | |   | |____webview_chromium Helper (Renderer)
          | |   |____Resources
          | |   | |____wxmac.icns
          | |   |____Info.plist
          | |____webview_chromium Helper (Alerts).app
          |   |____Contents
          |     |____MacOS
          |     | |____webview_chromium Helper (Alerts)
          |     |____Resources
          |     | |____wxmac.icns
          |     |____Info.plist
          |____Info.plist


    @section differences API Differences

    wxWebViewChromium aims to support the full wxWebView API, but there are
    some features which are currently unsupported:

    - GetSelectedSource/GetSelectedText: Always returns "".
    - HasSelection: Always returns false.
    - IsEditable: Always returns false.
    - CanUndo/CanRedo/CanCut/CanCopy/CanPaste: Always returns true.
    - Find: Always returns -1.
    - RunScript: Retrieving the result of JavaScript execution is not supported and
      if the @a output parameter is non-null, the function will always return false.

    @since 3.3.0
    @library{wxwebview_chromium}
    @category{webview}

**/
class wxWebViewChromium : public wxWebView
{
public:
    /**
        wxWebViewChromium constructor, arguments as per wxWebView::New.
    */
    wxWebViewChromium(wxWindow* parent,
                      wxWindowID id,
                      const wxString& url = wxWebViewDefaultURLStr,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxString& name = wxWebViewNameStr);

    /**
        Creation function for two-step creation.
    */
    virtual bool Create(wxWindow* parent,
                        wxWindowID id,
                        const wxString& url = wxWebViewDefaultURLStr,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxWebViewNameStr);
};
