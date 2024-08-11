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
    requires CEF version 116 (later, or even slightly earlier, versions may
    work too, but compatibility with them is not guaranteed).

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
    distribution, which basically means using `cmake` to do it. Please note:
        * Under Windows you must use `-DCEF_RUNTIME_LIBRARY_FLAG=/MD` option
          to match the default wxWidgets build configuration.
        * Under Unix remember that if you use `-DCMAKE_BUILD_TYPE=Debug` when
          building it, you need to pass `--enable-cef_debug` option to
          wxWidgets configure to avoid mismatches between various definitions
          in the wrapper itself and in wxWidgets.
        * Under all platforms you may want to use `-DUSE_SANDBOX=OFF`
          option to avoid linking with the sandbox library which is not
          provided in the "Minimal Distribution" if you don't use this feature.
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


    __CEF Helper Process__

    CEF uses multiple processes. The main process, using wxWidgets and creating
    wxWebViewChromium, is known as the "browser process" but CEF also creates
    additional helper processes for other tasks it needs to perform. Under
    non-Mac platforms by default the main process itself is also used as the
    helper process: if the main process detects the presence of the special
    `--type=` command line option, it considers that it is executed as "CEF
    helper" and just passes the command line to CEF instead of executing as
    usual. This happens before executing any application-specific code and so
    makes it impossible to use `--type` option for the application itself.

    Under macOS main process cannot be used as the helper process and a
    separate executable with the fixed name must be built to be used instead.
    wxWidgets provides a trivial source of such helper process in
    `samples/webview/cef_process_helper.cpp` file, which has to be compiled and
    linked with CEF at least under macOS.

    It may also be desirable to use a separate helper process under the other
    platforms too and if an executable called `yourapp_cef_helper` (Unix) or
    `yourapp_cef_helper.exe` (MSW), where `yourapp` is the name of the
    application such as returned by wxApp::GetAppName(), is found in the same
    directory as the main application executable itself, it will be used as the
    CEF helper process.


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


    __macOS Platform__

    macOS 10.13 or above is required.

    Due to the application bundle structure on macOS, wxWebviewChromium is a
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

    wxWebView also provides some functions not available in the base class:

    - SetRoot() allows to use the given directory for all application files.


    @section events Events

    wxWebViewChromiumEvent is a specific event generated when a message from
    the renderer process (in which any user-defined JavaScript functions are
    executing) is received in the browser process (containing UI). Such
    messages are sent using `CefFrame::SendProcessMessage(PID_BROWSER)` from
    the renderer process, but describing Chromium IPC in details is out of
    scope of this document, please see Chromium documentation for more
    information.

    @since 3.3.0
    @library{wxwebview}
    @category{webview}

**/
class wxWebViewChromium : public wxWebView
{
public:
    /**
        Default constructor.

        Use Create() to actually create the web view object later.
     */
    wxWebViewChromium();

    /**
        Constructor allowing to specify extra configuration parameters.

        You must use Create() to really initialize the object created with this
        constructor.

        Chromium-specific configuration parameters can be specified by setting
        wxWebViewConfigurationChromium fields before passing @a config to this
        function, e.g.

        @code
        wxWebViewConfiguration config =
            wxWebView::NewConfiguration(wxWebViewBackendChromium);

        auto configChrome =
            static_cast<wxWebViewConfigurationChromium*>(config.GetNativeConfiguration());
        configChrome->m_logFile = "/my/custom/CEF/log/file/path.txt";

        auto webview = new wxWebViewChromium(config);
        webview->Create(this, wxID_ANY, url);
        @endcode
    */
    explicit wxWebViewChromium(const wxWebViewConfiguration& config);

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

    /**
        Use the specified directory as root for the file:// URLs.

        After calling this function, all file paths will be resolved relatively
        to the given @a rootDir rather than to the actual file system root.
     */
    void SetRoot(const wxFileName& rootDir);
};


/**
    Chromium-specific configuration parameters.

    This simple class contains parameters that can be passed to the function
    initializing CEF when wxWebView is used for the first time.

    To use this struct you need to cast the value returned from
    wxWebViewConfiguration::GetNativeConfiguration() to this type:
    @code
    wxWebViewConfiguration config =
        wxWebView::NewConfiguration(wxWebViewBackendChromium);

    auto configChrome =
        static_cast<wxWebViewConfigurationChromium*>(config.GetNativeConfiguration());
    configChrome->m_logLevel = 99; // Disable all logging.
    @endcode

    @since 3.3.0
 */
class wxWebViewConfigurationChromium
{
public:
    /**
        Default constructor initializes the object to default parameters.
     */
    wxWebViewConfigurationChromium();

    /**
        CEF log file location.

        Should be an absolute path if specified.

        If this variable is empty, `debug.log` under the data directory is
        used.
     */
    wxString m_logFile;

    /**
        Logging level to use for CEF.

        This must be one of `cef_log_severity_t` values.

        Default value 0 means to use default "INFO" log level.
     */
    int m_logLevel = 0;

    /**
        Port for remote debugging if non-zero.

        Settings this to non-zero value, e.g. 9223 which is commonly used,
        enables Chrome remote debugging on the given port.
     */
    int m_remoteDebuggingPort = 0;

    /**
        Function to create the custom CefClient to use if non-null.

        CEF uses an object of CefClient class to customize handling of many
        operations, by allowing to return custom objects from its callbacks,
        and for processing IPC messages received from the other processes used
        by CEF. By defining this function pointer, the application can use its
        own CefClient subclass to customize many aspects of CEF behaviour
        beyond what is possible using the standard wxWebView API.

        Please note that the returned object must delegate all not otherwise
        implemented functions to the provided @a client (and should always
        delegate the lifetime-related callbacks). You can ensure that this is
        the case by deriving your custom CefClient subclass from
        wxDelegatingCefClient, but you still need to do it manually if not
        using this class.
     */
    CefClient* (*m_clientCreate)(CefClient* client, void* data) = nullptr;

    /**
        Data to pass to m_clientCreate if it is used.

        This is just an arbitrary pointer, which is passed as argument to
        m_clientCreate function if it is non-null.

        This pointer itself may be null if it is not necessary to pass any
        extra data to the client creation function.
     */
    void* m_clientCreateData = nullptr;
};

/**
    Event class for events generated exclusively by wxWebViewChromium.

    See wxWebViewEvent for other events also generated by wxWebViewEvent.

    Currently there is a single event type corresponding to this event:
    `wxEVT_WEBVIEW_CHROMIUM_MESSAGE_RECEIVED`. To use it (this example assumes
    that the required CEF headers are included):

    @code
    webview->Bind(
        wxEVT_WEBVIEW_CHROMIUM_MESSAGE_RECEIVED,
        [this](wxWebViewChromiumEvent& event) {
            if ( event.GetMessageName() == "MyCustomRequest" ) {
                CefRefPtr<CefListValue> arguments = event.GetMessage().GetArgumentList();
                CefRefPtr<CefProcessMessage> response = CefProcessMessage::Create("MyCustomResponse");

                // ... do whatever is needed to construct the response ...

                event.GetFrame().SendProcessMessage(PID_RENDERER, response);
            } else {
                // Not our message.
                event.Skip();
            }
        }
    );
    @endcode

    @note Using wxEvent dispatching adds a significant overhead to handling of
    CEF IPC messages, so if performance is important (i.e. many such messages
    are expected), it is recommended to configure wxWebViewChromium to use a
    custom `CefClient` as described in wxWebViewConfigurationChromium
    documentation and handle the messages directly in the overridden
    `OnProcessMessageReceived()` of the custom client class.

    @since 3.3.0
    @library{wxwebview}
    @category{webview}
**/
class wxWebViewChromiumEvent : public wxCommandEvent
{
public:
    /**
        Get the associated browser frame.

        This object can be used to call `SendProcessMessage(PID_RENDERER)` on
        it to send the reply back to the renderer process.
    */
    CefFrame& GetFrame() const;

    /**
        Get the actual message.

        Message arguments can be used in the event handler to get information
        associated with this message.
    */
    CefProcessMessage& GetMessage() const;

    /**
        Get the message name.

        This is just a convenient wrapper for `GetMessage().GetName()`.
    */
    wxString GetMessageName() const;
};
