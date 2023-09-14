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

    When wxWebViewChromium backend is enabled, it is compiled as part of the
    webview library which means that this library depends on CEF and all
    applications using it must be linked with it.

    Currently building wxWebViewChromium is possible only with configure-based
    build system under Unix (including macOS) and using the provided MSVS
    project files under MSW. Notably building it using CMake does not work.

    Before enabling wxWebViewChromium you need to ensure that CEF is available:

    1. Download the binaries for your platform from the CEF official site at
    https://cef-builds.spotifycdn.com/index.html ("Minimal Distribution" is
    sufficient)
    2. Unpack the archive into `3rdparty/cef` directory under wxWidgets source
    directory.
    3. Build `libcef_dll_wrapper` using the instructions provided in the CEF
    distribution, but, in short, just by using `cmake` to do it.
    4. Copy the static library binary in the platform-dependent location:
      - Under MSW, copy `libcefl_dll_wrapper.lib` file to either
      `3rdparty/cef/Release` or `3rdparty/cef/Debug` depending on the build
      configuration (it's recommended to build CEF wrapper in both
      configurations and so copy the two files to both locations).
      - Under Unix (including macOS), copy `libcef_dll_wrapper.a` file to
      `3rdparty/cef/libcef_dll_wrapper` directory.

    Then enable wxWebViewChromium support:

    - Under MSW, set `wxUSE_WEBVIEW_CHROMIUM` to 1 in `wx/msw/setup.h`.
    - Under Unix, add `--enable-webviewchromium` option to configure command
    line.

    Finally, build wxWidgets as usual: the resulting webview library will
    include wxWebViewChromium.

    It is recommended to build webview sample to check that everything is
    working as expected. Under Unix, the required CEF files will be copied to
    the sample directory as part of the build process, but under MSW you need
    to copy them manually before running the sample: please copy

    - All files from either `3rdparty/cef/Debug` or
      `3rdparty/cef/Release` folder, depending on your build type
    - The contents of the wx_root/3rdparty/cef/Resources folder.

    to the directory containing `webview` executable.

    Please see CEF `README.txt` for more details and also note that when
    redistributing CEF you must abide by the terms of its `LICENSE.txt`.

    Note that by default the webview sample uses the default platform-dependent
    wxWebView backend and to use Chromium backend in it you need to set
    `WX_WEBVIEW_BACKEND` environment variable to the value `wxWebViewChromium`.


    __Microsoft Windows Platform__

    Windows 10 or newer is required to run applications using wxWebViewChromium.
    Such applications should also have an application manifest declaring compatibility
    with supported Windows versions.

    Microsoft Visual C++ 2022 must be used to build wxWebViewChromium.

    When linking an application using it, both `libcef.lib` and
    `libcef_dll_wrapper.lib` should be included in the libraries list.

    __Linux with GTK__

    wxWebviewChromium currently only supports X11 and not Wayland, i.e. you
    need to either ensure that `GDK_BACKEND` environment variable is set to
    "x11" before running your program using it or call

    @code
    gdk_set_allowed_backends("x11")
    @endcode

    in your application code, otherwise wxWebView::IsBackendAvailable() will
    return @false when the application is running under Wayland and creating
    wxWebviewChromium will fail in this case.

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
    it work, based off the webview sample bakefile.

    1. Build the webview library with CEF support.
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

        webview.app
        |____Contents
          |____MacOS
          | |____webview
          |____Resources
          | |____wxmac.icns
          |____Frameworks
          | |____webview Helper.app
          | | |____Contents
          | |   |____MacOS
          | |   | |____webview Helper
          | |   |____Resources
          | |   | |____wxmac.icns
          | |   |____Info.plist
          | |____webview Helper (GPU).app
          | | |____Contents
          | |   |____MacOS
          | |   | |____webview Helper (GPU)
          | |   |____Resources
          | |   | |____wxmac.icns
          | |   |____Info.plist
          | |____webview Helper (Plugin).app
          | | |____Contents
          | |   |____MacOS
          | |   | |____webview Helper (Plugin)
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
          | |____webview Helper (Renderer).app
          | | |____Contents
          | |   |____MacOS
          | |   | |____webview Helper (Renderer)
          | |   |____Resources
          | |   | |____wxmac.icns
          | |   |____Info.plist
          | |____webview Helper (Alerts).app
          |   |____Contents
          |     |____MacOS
          |     | |____webview Helper (Alerts)
          |     |____Resources
          |     | |____wxmac.icns
          |     |____Info.plist
          |____Info.plist


    __Miscellaneous Notes__

    When running applications using wxWebViewChromium, the command line option
    `--type=xxx` is interpreted specially as it is used by CEF to launch helper
    applications, so your program must not use this option for anything else.



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
    @library{wxwebview}
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
