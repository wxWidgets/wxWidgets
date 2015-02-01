/////////////////////////////////////////////////////////////////////////////
// Name:        webview_chromium.h
// Purpose:     interface of wxWebViewChromium
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxWebViewChromium

    wxWebViewChromium is a Chromium backend for wxWebView based on the
    Chromium Embedded Framework (CEF3). The current CEF3 required version is
    3.1750.1738. This backend is available for Windows, Linux and OS X only.

    @section requirements CEF3 Requirements

    If you are compiling your own copy of CEF3 then your compile flags must
    match those you are using to compile wxWidgets. Specifically on Windows
    it is likely you will need to adjust the following properties in the CEF3
    provided build files:

    - C/C++ - Code Generation - Runtime Library - Multithreaded [Debug] DLL
    - C/C++ - Code Generation - Enable C++ Exceptions - Yes
    - C/C++ - Language - Enable Run-Time Type Information - Yes

    The following release branches of CEF have been tested: 1650, 1750, 1916,
    2062 and 2171.

    @section instructions Build Instructions

    __General__

    The wxWebViewChromium backend is built into a seperate webview_chromium
    library which depends on the webview library.

    Once you have a copy of CEF3, either from compiling it yourself or using
    prebuild binaries you should copy it into `wx_root/src/cef`. To run the
    webview_chromium sample you need to copy the CEF3 resources into the
    sample directory. The following files need to be copied:

    - The shared objects in the wx_root/src/cef/Debug or
      wx_root/src/cef/Release folder, depending on your build type.
    - The contents of the wx_root/src/cef/Resources folder.

    When builing wxWidgets you need to enusre wxWebViewChromium is enabled,
    either by passing --enable-webviewchromium for autoconf based builds or
    setting wxUSE_WEBVIEW_CHROMIUM equal to 1 in setup.h for Visual Studio
    based builds.

    __Mac OS X Platform__

    OS X 10.8 or above is required.

    Due to the application bundle structure on OS X, wxWebviewChromium is a
    little complicated than on Windows/Linux platforms. An extra helper
    application for executing separate Chromium  processes(renderer, plugin,
    etc) is required.

    For applications using wxWebviewChromium below are details steps to make
    is worked, based off the webview_chromium sample bakefile.

    1. Build the webview_chromium library.
       - Use system tool `install_name_tool -change` to correct `Chromium
         Embedded Framework.framework/Chromium Embedded Framework` location.
    2. Compiled/link/package the `helper` app:
       - Require `process_helper_mac.cpp`
       - Reuqire link frameworks: Chromium Embedded Framework.framework,
         AppKit.frameworks.
       - Reuqire app bundle configuration: Info.plist
       - Use system tool `install_name_tool -change` to correct `Chromium
         Embedded Framework.framework/Chromium Embedded Framework` location.
    3. Compiled/link/package the `webviewchromium` app:
       - Require `webview.cpp`
       - Reuqire link frameworks: Chromium Embedded Framework.framework,
         AppKit.frameworks.
       - Reuqire app bundle configuration: Info.plist
       - Use system tool `install_name_tool -change` to correct `Chromium
         Embedded Framework.framework/Chromium Embedded Framework` location.
    4. Create a `Contents/Frameworks` directory in `webviewchromium.app` bundle
       and copy `helper.app`, `webviewchromium.dylib`, 'webview.dylib'
       and `Chromium Embedded Framework` into it.
    5. Use `samples/webview_chromium/mac/tools/make_more_helper.sh` to make
       sub-process helper app bundles based on `helper` app.

    Below is the wxWebviewChromium sample app bundle directory structure:

        webview_chromium.app
        `-- Contents
            |-- Frameworks
            |   |-- Chromium Embedded Framework.framework       <= CEF framework
            |   |   |-- Chromium Embedded Framework
            |   |   |-- Libraries
            |   |   |   `-- ffmpegsumo.so
            |   |   `-- Resources\
            |   |-- libwx_osx_cocoau_webviewchromium-3.1.dylib  <= wxWebviewChromium library supports webviewChromium backend.
            |   |-- libwx_osx_cocoau_webview-3.1.dylib          <= wxWebview library supports other backends.
            |   |-- webview_chromium Helper EH.app              <= helper app
            |   |   `-- Contents
            |   |       |-- Info.plist
            |   |       |-- MacOS
            |   |       |   `-- webview_chromium Helper EH
            |   |       |-- PkgInfo
            |   |       `-- Resources
            |   |           `-- wxmac.icns
            |   |-- webview_chromium Helper NP.app              <= helper app
            |   |   `-- Contents
            |   |       |-- Info.plist
            |   |       |-- MacOS
            |   |       |   `-- webview_chromium Helper NP
            |   |       |-- PkgInfo
            |   |       `-- Resources
            |   |           `-- wxmac.icns
            |   `-- webview_chromium Helper.app                 <= helper app
            |       `-- Contents
            |           |-- Info.plist
            |           |-- MacOS
            |           |   `-- webview_chromium Helper
            |           |-- PkgInfo
            |           `-- Resources
            |               `-- wxmac.icns
            |-- Info.plist
            |-- MacOS
            |   `-- webview_chromium                            <= webviewchromium sample executable
            |-- PkgInfo
            `-- Resources
                `-- wxmac.icns                                  <= resources.

    @section differences API Differences

    wxWebViewChromium aims to support the full wxWebView API, but there are
    some features which are currently unsupported by the CEF3 API, these are:

    - GetSelectedSource/GetSelectedText: returns ""
    - HasSelection: returns false
    - IsEditable: returns false
    - CanUndo/CanRedo/CanCut/CanCopy/CanPaste: returns true
    - Find: returns -1

    @since 3.1.0
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
    /**
        StartUp function for Windows platform.

        Setup CEF3 environment, creating and initializing browser/renderer processes.
    */
    static bool StartUp(int &code, const wxString &path = "");

    /**
        StartUp function for Linux/Mac OS X platforms.
        Setup CEF3 environment, creating and initializing browser/renderer processes.
    */
    static bool StartUp(int &code, const wxString &path,
                        int argc, char* argv[]);

    /** 
        Shutdown CEF

        The function should be callde before client application exits.
        
        Since there is a bug of CEF3, the behavior is different between
        Mac OS X with Linux/Windows platforms.  On Mac OS X, it should
        be called in wxFrame::OnClose.  On Linux/Windows, it shuold be
        called in wxApp::OnExit.
    */
    static void Shutdown();

    /**
        Run CEF3 messageloop once.

        The client app using wxWebviewChromium should integrate CEF messageloop in
        its own messageloop to give CEF3 a chance to run.

        Developer can integrate the messageloop through the following two ways:
        1. Implement wxApp::ProcessIdle
           @code
               class WebApp
               {
                   bool ProcessIdle()
                   {
                       wxWebViewChromium::DoCEFWork();
                       return wxApp::ProcessIdle();
                   }
               };
           @endcode
        2. Set a timer to run CEF messageloop each time interval:
           @code
               class WebFrame: public wxFrame
               {
                    void OnTimer(wxTimerEvent& evt)
                    {
                        wxWebViewChromium::DoCEFWork();
                    }
                    WebFrame()
                    {
                        Connect(wxID_ANY, wxEVT_TIMER, wxTimerEventHandler(WebFrame::OnTimer), NULL, this);
                        m_timer = new wxTimer(this);
                    }
                    wxTimer* m_timer;
               };
           @endcode
    */
    static void DoCEFWork();
};


/**
    @class wxWebViewFactoryChromium

    A factory class for creating wxWebViewChromium backend.
    By default, wxWebViewFactoryChromium is not registered in wxWebView, developer should register the
    factory class in wxWebView before using.

    @code
    wxWebView::RegisterFactory(wxWebViewBackendChromium,
                               wxSharedPtr<wxWebViewFactory>(new wxWebViewFactoryChromium));
    @endcode
*/
class wxWebViewFactoryChromium : public wxWebViewFactory
{
public:
    virtual wxWebView* Create();
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxWebViewNameStr);
};
