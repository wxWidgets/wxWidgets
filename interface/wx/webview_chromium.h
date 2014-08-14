/////////////////////////////////////////////////////////////////////////////
// Name:        webview_chromium.h
// Purpose:     interface of wxWebViewChromium
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxWebViewChromium

    wxWebViewChromium is a Chromium backend base on Chromium Embedded Framework(CEF3),
    which uses the same rendering engine with Google Chrome Browser. The current
    CEF3 required version is 3.1750.1738. This backend is available for
    MSW/Linux/Mac OS X port. Custom schemes and virtual file systems are also supported.

    @section differences API Differences

    wxWebViewChromium aims to support the full wxWebView API, but there are some features 
    unsupported by CEF3 API, see the following differences.

    - GetSelectedSource/GetSelectedText: return ""
    - HasSelection: Not supported by CEF3, return false
    - IsEditable: Not supported by CEF3, return false
    - CanUndo/CanRedo/CanCut/CanCopy/CanPaste: Not supported by CEF3, return true
    - Find: return -1

    @section requirements Requirements

    Chromium Embedded Framework: 3.1750.1738.

    On Windows platform, you will need to change the vistual stdio building
    properties of `libcef_dll_wrapper` to consistent with wxWidgets, see following steps:

    - C/C++ - General - Treat Warngings as Error - No
    - C/C++ - Code Generation - Runtime Library - Multithreaded [Debug] DLL
    - C/C++ - Code Generation - Enable C++ Exceptions - Yes
    - C/C++ - Language - Enable Run-Time Type Information - Yes

    1. Download cef3 binary and extract it to wxWidgets src directory.(<wx_root>/src/cef is default directory)
    2. Build libcef_dll_wrapper static library.

    @section instructions Builing Instructions
    
    __Windows Platform__
    
    wxWidgets provides Visual Stdio 2010/2012/2013 project file to build wxWebViewChromium.
    
    1. Open `build/msw/wc_vc10.sln` project file in Visual Stdio 2010, and build `wxWidgets` library.
    2. Open `samples/webview_chromium/webview_chromium_vc10.vcxproj` file in Visual Stdio 2010, and build it.
    3. Copy CEF3 related resources(libcef.dll, libffmpegsumo.so, locales/\*, cef.apk, devtools_resources.pak) to webview_chromium binary directory.
    
    __Linux Platform__
    
    wxWebviewChromium feature is disabled defaultly in makefile build, you should enable it through `enable-webviewchromium` option.
    
    1. Regenerate the building files by running `bakefile_gen -f autoconf` command under `build/bakefiles` directory.
    2. Run `./autogen.sh` under `<wxWidgets_src>`.
    3. Run `./configure --enable-webviewchromium` and `make` commands.
    4. Copy CEF3 related resources as above step5 in windows Platform.

    __Mac OS X Platform__

    Require OS X 10.8 or above. Support makefile builds the same as Linux platfrom mentioned above.

    Due to the application bundle structure on OS X, wxWebviewChromium is a little complicated than Windows/Linux platforms.
    We need an extra `helper` application for executing separate other chromim processes(renderer, plugin, etc), so they
    have separated app bundles and Info.plist.
    
    For application using webviewChromium, below are details steps, you can retrieve it in `webview_chromium.bkl`
    (samples/webview_chromium/webview_chromium.bkl) files:
    
    1. Build webviewchromium library.
       - Use system tool `install_name_tool -change` to correct `Chromium Embedded Framework.framework/Chromium Embedded Framework` location.
    2. Compiled/link/package the `helper` app:
       - Require `process_helper_mac.cpp`
       - Reuqire link frameworks: Chromium Embedded Framework.framework, AppKit.frameworks.
       - Reuqire app bundle configuration: Info.plist
       - Use system tool `install_name_tool -change` to correct `Chromium Embedded Framework.framework/Chromium Embedded Framework` location.
    3. Compiled/link/package the `webviewchromium` app:
       - Require `webview.cpp`
       - Reuqire link frameworks: Chromium Embedded Framework.framework, AppKit.frameworks.
       - Reuqire app bundle configuration: Info.plist
       - Use system tool `install_name_tool -change` to correct `Chromium Embedded Framework.framework/Chromium Embedded Framework` location.
    4. Create a `Contents/Frameworks` directory in `webviewchromium.app` bundle and copy `helper.app`, `webviewchromium.dylib` and `Chromium Embedded Framework` in it.
    5. Use `samples/webview_chromium/mac/tools/make_more_helper.sh` to make sub-process helper app bundles based on `helper` app.
    
    
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
            

**/
class wxWebViewChromium : public wxWebView
{
public:
    wxWebViewChromium(wxWindow* parent,
                      wxWindowID id,
                      const wxString& url = wxWebViewDefaultURLStr,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = 0,
                      const wxString& name = wxWebViewNameStr);

    void OnSize(wxSizeEvent &event);

    void SetPageSource(const wxString& pageSource);

    void SetPageText(const wxString& pageText);

    bool Create(wxWindow* parent,
                wxWindowID id,
                const wxString& url = wxWebViewDefaultURLStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxWebViewNameStr);

    virtual void LoadURL(const wxString& url);
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item);
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory();
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory();

    virtual bool CanGoForward() const;
    virtual bool CanGoBack() const;
    virtual void GoBack();
    virtual void GoForward();
    virtual void ClearHistory();
    virtual void EnableHistory(bool enable = true);
    virtual void Stop();
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT);

    virtual wxString GetPageSource() const;
    virtual wxString GetPageText() const;

    virtual bool IsBusy() const;
    virtual wxString GetCurrentURL() const;
    virtual wxString GetCurrentTitle() const;

    virtual void SetZoomType(wxWebViewZoomType type);
    virtual wxWebViewZoomType GetZoomType() const;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const;

    virtual void Print();

    virtual wxWebViewZoom GetZoom() const;
    virtual void SetZoom(wxWebViewZoom zoom);

    virtual void* GetNativeBackend() const;

    virtual long Find(const wxString& text, int flags = wxWEBVIEW_FIND_DEFAULT) { return wxNOT_FOUND; }

    //Clipboard functions
    virtual bool CanCut() const { return true; }
    virtual bool CanCopy() const { return true; }
    virtual bool CanPaste() const { return true; }
    virtual void Cut();
    virtual void Copy();
    virtual void Paste();

    //Undo / redo functionality
    virtual bool CanUndo() const { return true; }
    virtual bool CanRedo() const { return true; }
    virtual void Undo();
    virtual void Redo();

    //Editing functions
    virtual void SetEditable(bool enable = true);
    virtual bool IsEditable() const { return false; }

    //Selection
    virtual void SelectAll();
    virtual bool HasSelection() const { return false; }
    virtual void DeleteSelection();
    virtual wxString GetSelectedText() const { return ""; }
    virtual wxString GetSelectedSource() const { return ""; }
    virtual void ClearSelection();

    virtual void RunScript(const wxString& javascript);

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler);

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
