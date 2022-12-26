/////////////////////////////////////////////////////////////////////////////
// Name:        webview.cpp
// Purpose:     wxWebView sample
// Author:      Marianne Gagnon
// Copyright:   (c) 2010 Marianne Gagnon, Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_WEBVIEW_WEBKIT && !wxUSE_WEBVIEW_WEBKIT2 && !wxUSE_WEBVIEW_IE && !wxUSE_WEBVIEW_EDGE
#error "A wxWebView backend is required by this sample"
#endif

#include "wx/artprov.h"
#include "wx/cmdline.h"
#include "wx/notifmsg.h"
#include "wx/settings.h"
#include "wx/webview.h"
#if wxUSE_WEBVIEW_IE
#include "wx/msw/webview_ie.h"
#endif
#if wxUSE_WEBVIEW_EDGE
#include "wx/msw/webview_edge.h"
#endif
#include "wx/webviewarchivehandler.h"
#include "wx/webviewfshandler.h"
#include "wx/numdlg.h"
#include "wx/infobar.h"
#include "wx/filesys.h"
#include "wx/fs_arc.h"
#include "wx/fs_mem.h"
#include "wx/stdpaths.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#if wxUSE_STC
#include "wx/stc/stc.h"
#else
#include "wx/textctrl.h"
#endif

#include "wxlogo.xpm"


//We map menu items to their history items
WX_DECLARE_HASH_MAP(int, wxSharedPtr<wxWebViewHistoryItem>,
                    wxIntegerHash, wxIntegerEqual, wxMenuHistoryMap);

class WebApp : public wxApp
{
public:
    WebApp() :
        m_url("https://www.wxwidgets.org")
    {
    }

    virtual bool OnInit() override;

#if wxUSE_CMDLINE_PARSER
    virtual void OnInitCmdLine(wxCmdLineParser& parser) override
    {
        wxApp::OnInitCmdLine(parser);

        parser.AddParam("URL to open",
                        wxCMD_LINE_VAL_STRING,
                        wxCMD_LINE_PARAM_OPTIONAL);
    }

    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override
    {
        if ( !wxApp::OnCmdLineParsed(parser) )
            return false;

        if ( parser.GetParamCount() )
            m_url = parser.GetParam(0);

        return true;
    }
#endif // wxUSE_CMDLINE_PARSER

private:
    wxString m_url;
};

class WebFrame : public wxFrame
{
public:
    WebFrame(const wxString& url);
    virtual ~WebFrame();

    void UpdateState();
    void OnIdle(wxIdleEvent& evt);
    void OnUrl(wxCommandEvent& evt);
    void OnBack(wxCommandEvent& evt);
    void OnForward(wxCommandEvent& evt);
    void OnStop(wxCommandEvent& evt);
    void OnReload(wxCommandEvent& evt);
    void OnClearHistory(wxCommandEvent& evt);
    void OnEnableHistory(wxCommandEvent& evt);
    void OnNavigationRequest(wxWebViewEvent& evt);
    void OnNavigationComplete(wxWebViewEvent& evt);
    void OnDocumentLoaded(wxWebViewEvent& evt);
    void OnNewWindow(wxWebViewEvent& evt);
    void OnTitleChanged(wxWebViewEvent& evt);
    void OnFullScreenChanged(wxWebViewEvent& evt);
    void OnScriptMessage(wxWebViewEvent& evt);
    void OnScriptResult(wxWebViewEvent& evt);
    void OnSetPage(wxCommandEvent& evt);
    void OnViewSourceRequest(wxCommandEvent& evt);
    void OnViewTextRequest(wxCommandEvent& evt);
    void OnToolsClicked(wxCommandEvent& evt);
    void OnSetZoom(wxCommandEvent& evt);
    void OnError(wxWebViewEvent& evt);
    void OnPrint(wxCommandEvent& evt);
    void OnCut(wxCommandEvent& evt);
    void OnCopy(wxCommandEvent& evt);
    void OnPaste(wxCommandEvent& evt);
    void OnUndo(wxCommandEvent& evt);
    void OnRedo(wxCommandEvent& evt);
    void OnMode(wxCommandEvent& evt);
    void OnZoomLayout(wxCommandEvent& evt);
    void OnZoomCustom(wxCommandEvent& evt);
    void OnHistory(wxCommandEvent& evt);
    void OnScrollLineUp(wxCommandEvent&) { m_browser->LineUp(); }
    void OnScrollLineDown(wxCommandEvent&) { m_browser->LineDown(); }
    void OnScrollPageUp(wxCommandEvent&) { m_browser->PageUp(); }
    void OnScrollPageDown(wxCommandEvent&) { m_browser->PageDown(); }
    void RunScript(const wxString& javascript);
    void OnRunScriptString(wxCommandEvent& evt);
    void OnRunScriptInteger(wxCommandEvent& evt);
    void OnRunScriptDouble(wxCommandEvent& evt);
    void OnRunScriptBool(wxCommandEvent& evt);
    void OnRunScriptObject(wxCommandEvent& evt);
    void OnRunScriptArray(wxCommandEvent& evt);
    void OnRunScriptDOM(wxCommandEvent& evt);
    void OnRunScriptUndefined(wxCommandEvent& evt);
    void OnRunScriptNull(wxCommandEvent& evt);
    void OnRunScriptDate(wxCommandEvent& evt);
#if wxUSE_WEBVIEW_IE
    void OnRunScriptObjectWithEmulationLevel(wxCommandEvent& evt);
    void OnRunScriptDateWithEmulationLevel(wxCommandEvent& evt);
    void OnRunScriptArrayWithEmulationLevel(wxCommandEvent& evt);
#endif
    void OnRunScriptMessage(wxCommandEvent& evt);
    void OnRunScriptAsync(wxCommandEvent& evt);
    void OnRunScriptCustom(wxCommandEvent& evt);
    void OnAddUserScript(wxCommandEvent& evt);
    void OnSetCustomUserAgent(wxCommandEvent& evt);
    void OnClearSelection(wxCommandEvent& evt);
    void OnDeleteSelection(wxCommandEvent& evt);
    void OnSelectAll(wxCommandEvent& evt);
    void OnLoadScheme(wxCommandEvent& evt);
    void OnUseMemoryFS(wxCommandEvent& evt);
    void OnLoadAdvancedHandler(wxCommandEvent& evt);
    void OnFind(wxCommandEvent& evt);
    void OnFindDone(wxCommandEvent& evt);
    void OnFindText(wxCommandEvent& evt);
    void OnFindOptions(wxCommandEvent& evt);
    void OnEnableContextMenu(wxCommandEvent& evt);
    void OnEnableDevTools(wxCommandEvent& evt);
    void OnEnableBrowserAcceleratorKeys(wxCommandEvent& evt);

private:
    wxTextCtrl* m_url;
    wxWebView* m_browser;

    wxToolBar* m_toolbar;
    wxToolBarToolBase* m_toolbar_back;
    wxToolBarToolBase* m_toolbar_forward;
    wxToolBarToolBase* m_toolbar_stop;
    wxToolBarToolBase* m_toolbar_reload;
    wxToolBarToolBase* m_toolbar_tools;

    wxToolBarToolBase* m_find_toolbar_done;
    wxToolBarToolBase* m_find_toolbar_next;
    wxToolBarToolBase* m_find_toolbar_previous;
    wxToolBarToolBase* m_find_toolbar_options;
    wxMenuItem* m_find_toolbar_wrap;
    wxMenuItem* m_find_toolbar_highlight;
    wxMenuItem* m_find_toolbar_matchcase;
    wxMenuItem* m_find_toolbar_wholeword;

    wxMenu* m_tools_menu;
    wxMenu* m_tools_history_menu;
    wxMenuItem* m_tools_layout;
    wxMenuItem* m_tools_tiny;
    wxMenuItem* m_tools_small;
    wxMenuItem* m_tools_medium;
    wxMenuItem* m_tools_large;
    wxMenuItem* m_tools_largest;
    wxMenuItem* m_tools_custom;
    wxMenuItem* m_tools_handle_navigation;
    wxMenuItem* m_tools_handle_new_window;
    wxMenuItem* m_tools_enable_history;
    wxMenuItem* m_edit_cut;
    wxMenuItem* m_edit_copy;
    wxMenuItem* m_edit_paste;
    wxMenuItem* m_edit_undo;
    wxMenuItem* m_edit_redo;
    wxMenuItem* m_edit_mode;
    wxMenuItem* m_scroll_line_up;
    wxMenuItem* m_scroll_line_down;
    wxMenuItem* m_scroll_page_up;
    wxMenuItem* m_scroll_page_down;
    wxMenuItem* m_script_string;
    wxMenuItem* m_script_integer;
    wxMenuItem* m_script_double;
    wxMenuItem* m_script_bool;
    wxMenuItem* m_script_object;
    wxMenuItem* m_script_array;
    wxMenuItem* m_script_dom;
    wxMenuItem* m_script_undefined;
    wxMenuItem* m_script_null;
    wxMenuItem* m_script_date;
#if wxUSE_WEBVIEW_IE
    wxMenuItem* m_script_object_el;
    wxMenuItem* m_script_date_el;
    wxMenuItem* m_script_array_el;
#endif
    wxMenuItem* m_script_message;
    wxMenuItem* m_script_custom;
    wxMenuItem* m_script_async;
    wxMenuItem* m_selection_clear;
    wxMenuItem* m_selection_delete;
    wxMenuItem* m_find;
    wxMenuItem* m_context_menu;
    wxMenuItem* m_dev_tools;
    wxMenuItem* m_browser_accelerator_keys;

    wxInfoBar *m_info;
    wxStaticText* m_info_text;
    wxTextCtrl* m_find_ctrl;
    wxToolBar* m_find_toolbar;

    wxMenuHistoryMap m_histMenuItems;
    wxString m_findText;
    int m_findFlags, m_findCount;
    long m_zoomFactor;

    // Last executed JavaScript snippet, for convenience.
    wxString m_javascript;
};

class SourceViewDialog : public wxDialog
{
public:
    SourceViewDialog(wxWindow* parent, wxString source);
};

// AdvancedWebViewHandler is a sample handler used by handler_advanced.html
// to show a sample implementation of wxWebViewHandler::StartRequest().
// see the documentation for additional details.
class AdvancedWebViewHandler: public wxWebViewHandler
{
public:
    AdvancedWebViewHandler():
        wxWebViewHandler("wxpost")
    { }

    virtual void StartRequest(const wxWebViewHandlerRequest& request,
                              wxSharedPtr<wxWebViewHandlerResponse> response) override
    {
        response->SetHeader("Access-Control-Allow-Origin", "*");
        response->SetHeader("Access-Control-Allow-Headers", "*");

        // Handle options request
        if (request.GetMethod().IsSameAs("options", false))
        {
            response->Finish("");
            return;
        }

        response->SetContentType("application/json");
        response->Finish(
            wxString::Format(
                "{\n  contentType: \"%s\",\n  method: \"%s\",\n  data: \"%s\"\n}",
                request.GetHeader("Content-Type"),
                request.GetMethod(),
                request.GetDataString()
        ));
    }
};

wxIMPLEMENT_APP(WebApp);

// ============================================================================
// implementation
// ============================================================================

bool WebApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    //Required for virtual file system archive and memory support
    wxFileSystem::AddHandler(new wxArchiveFSHandler);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);

    // Create the memory files
    wxImage::AddHandler(new wxPNGHandler);
    wxMemoryFSHandler::AddFile("logo.png",
        wxBitmap(wxlogo_xpm), wxBITMAP_TYPE_PNG);
    wxMemoryFSHandler::AddFile("page1.htm",
        "<html><head><title>File System Example</title>"
        "<link rel='stylesheet' type='text/css' href='memory:test.css'>"
        "</head><body><h1>Page 1</h1>"
        "<p><img src='memory:logo.png'></p>"
        "<p>Some text about <a href='memory:page2.htm'>Page 2</a>.</p></body>");
    wxMemoryFSHandler::AddFile("page2.htm",
        "<html><head><title>File System Example</title>"
        "<link rel='stylesheet' type='text/css' href='memory:test.css'>"
        "</head><body><h1>Page 2</h1>"
        "<p><a href='memory:page1.htm'>Page 1</a> was better.</p></body>");
    wxMemoryFSHandler::AddFile("test.css", "h1 {color: red;}");

    WebFrame *frame = new WebFrame(m_url);
    frame->Show();

    return true;
}

WebFrame::WebFrame(const wxString& url) :
    wxFrame(nullptr, wxID_ANY, "wxWebView Sample")
{
    // set the frame icon
    SetIcon(wxICON(sample));
    SetTitle("wxWebView Sample");
    EnableFullScreenView(); // Enable native fullscreen API on macOS

    wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

    // Create the toolbar
    m_toolbar = CreateToolBar(wxTB_TEXT);

    m_toolbar_back = m_toolbar->AddTool(wxID_ANY, _("Back"), wxArtProvider::GetBitmapBundle(wxART_GO_BACK, wxART_TOOLBAR));
    m_toolbar_forward = m_toolbar->AddTool(wxID_ANY, _("Forward"), wxArtProvider::GetBitmapBundle(wxART_GO_FORWARD, wxART_TOOLBAR));
    m_toolbar_stop = m_toolbar->AddTool(wxID_ANY, _("Stop"), wxArtProvider::GetBitmapBundle(wxART_STOP, wxART_TOOLBAR));
    m_toolbar_reload = m_toolbar->AddTool(wxID_ANY, _("Reload"), wxArtProvider::GetBitmapBundle(wxART_REFRESH, wxART_TOOLBAR));
    m_url = new wxTextCtrl(m_toolbar, wxID_ANY, "",  wxDefaultPosition, FromDIP(wxSize(400, -1)), wxTE_PROCESS_ENTER );
    m_toolbar->AddControl(m_url, _("URL"));
    m_toolbar_tools = m_toolbar->AddTool(wxID_ANY, _("Menu"), wxArtProvider::GetBitmapBundle(wxART_WX_LOGO, wxART_TOOLBAR));

    m_toolbar->Realize();

    // Set find values.
    m_findFlags = wxWEBVIEW_FIND_DEFAULT;
    m_findCount = 0;

    // Create panel for find toolbar.
    wxPanel* panel = new wxPanel(this);
    topsizer->Add(panel, wxSizerFlags().Expand());

    // Create sizer for panel.
    wxBoxSizer* panel_sizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(panel_sizer);

    // Create the find toolbar.
    m_find_toolbar = new wxToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxTB_TEXT|wxTB_HORZ_LAYOUT);
    m_find_toolbar->Hide();
    panel_sizer->Add(m_find_toolbar, wxSizerFlags().Expand());

    // Create find control.
    m_find_ctrl = new wxTextCtrl(m_find_toolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(140,-1), wxTE_PROCESS_ENTER);


    //Find options menu
    wxMenu* findmenu = new wxMenu;
    m_find_toolbar_wrap = findmenu->AppendCheckItem(wxID_ANY,"Wrap");
    m_find_toolbar_matchcase = findmenu->AppendCheckItem(wxID_ANY,"Match Case");
    m_find_toolbar_wholeword = findmenu->AppendCheckItem(wxID_ANY,"Entire Word");
    m_find_toolbar_highlight = findmenu->AppendCheckItem(wxID_ANY,"Highlight");
    // Add find toolbar tools.
    m_find_toolbar->SetToolSeparation(7);
    m_find_toolbar_done = m_find_toolbar->AddTool(wxID_ANY, "Close", wxArtProvider::GetBitmap(wxART_CROSS_MARK));
    m_find_toolbar->AddSeparator();
    m_find_toolbar->AddControl(m_find_ctrl, "Find");
    m_find_toolbar->AddSeparator();
    m_find_toolbar_next = m_find_toolbar->AddTool(wxID_ANY, "Next", wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR, wxSize(16,16)));
    m_find_toolbar_previous = m_find_toolbar->AddTool(wxID_ANY, "Previous", wxArtProvider::GetBitmap(wxART_GO_UP, wxART_TOOLBAR, wxSize(16,16)));
    m_find_toolbar->AddSeparator();
    m_find_toolbar_options = m_find_toolbar->AddTool(wxID_ANY, "Options", wxArtProvider::GetBitmap(wxART_PLUS, wxART_TOOLBAR, wxSize(16,16)), "", wxITEM_DROPDOWN);
    m_find_toolbar_options->SetDropdownMenu(findmenu);
    m_find_toolbar->Realize();

    // Create the info panel
    m_info = new wxInfoBar(this);
    topsizer->Add(m_info, wxSizerFlags().Expand());

    // Create a log window
    new wxLogWindow(this, _("Logging"), true, false);

#if wxUSE_WEBVIEW_EDGE
    // Check if a fixed version of edge is present in
    // $executable_path/edge_fixed and use it
    wxFileName edgeFixedDir(wxStandardPaths::Get().GetExecutablePath());
    edgeFixedDir.SetFullName("");
    edgeFixedDir.AppendDir("edge_fixed");
    if (edgeFixedDir.DirExists())
    {
        wxWebViewEdge::MSWSetBrowserExecutableDir(edgeFixedDir.GetFullPath());
        wxLogMessage("Using fixed edge version");
    }
#endif
    // Create the webview
    m_browser = wxWebView::New();
#ifdef __WXMAC__
    // With WKWebView handlers need to be registered before creation
    m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewArchiveHandler("wxfs")));
    m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
    m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new AdvancedWebViewHandler()));
#endif
    m_browser->Create(this, wxID_ANY, url, wxDefaultPosition, wxDefaultSize);
    topsizer->Add(m_browser, wxSizerFlags().Expand().Proportion(1));

    // Log backend information
    wxLogMessage("Backend: %s Version: %s", m_browser->GetClassInfo()->GetClassName(),
        wxWebView::GetBackendVersionInfo().ToString());
    wxLogMessage("User Agent: %s", m_browser->GetUserAgent());

#ifndef __WXMAC__
    //We register the wxfs:// protocol for testing purposes
    m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewArchiveHandler("wxfs")));
    //And the memory: file system
    m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));
    m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new AdvancedWebViewHandler()));
#endif
    if (!m_browser->AddScriptMessageHandler("wx"))
        wxLogError("Could not add script message handler");

    SetSizer(topsizer);

    //Set a more sensible size for web browsing
    SetSize(FromDIP(wxSize(800, 600)));

    // Create the Tools menu
    m_tools_menu = new wxMenu();
    wxMenuItem* print = m_tools_menu->Append(wxID_ANY , _("Print"));
    wxMenuItem* setPage = m_tools_menu->Append(wxID_ANY , _("Set page text"));
    wxMenuItem* viewSource = m_tools_menu->Append(wxID_ANY , _("View Source"));
    wxMenuItem* viewText = m_tools_menu->Append(wxID_ANY, _("View Text"));
    m_tools_menu->AppendSeparator();
    m_tools_layout = m_tools_menu->AppendRadioItem(wxID_ANY, _("Use Layout Zoom"));
    m_tools_tiny = m_tools_menu->AppendRadioItem(wxID_ANY, _("Tiny"));
    m_tools_small = m_tools_menu->AppendRadioItem(wxID_ANY, _("Small"));
    m_tools_medium = m_tools_menu->AppendRadioItem(wxID_ANY, _("Medium"));
    m_tools_large = m_tools_menu->AppendRadioItem(wxID_ANY, _("Large"));
    m_tools_largest = m_tools_menu->AppendRadioItem(wxID_ANY, _("Largest"));
    m_tools_custom = m_tools_menu->AppendRadioItem(wxID_ANY, _("Custom Size"));
    m_tools_menu->AppendSeparator();
    m_tools_handle_navigation = m_tools_menu->AppendCheckItem(wxID_ANY, _("Handle Navigation"));
    m_tools_handle_new_window = m_tools_menu->AppendCheckItem(wxID_ANY, _("Handle New Windows"));
    m_tools_menu->AppendSeparator();

    //Find
    m_find = m_tools_menu->Append(wxID_ANY, _("Find"));
    m_tools_menu->AppendSeparator();

    //History menu
    m_tools_history_menu = new wxMenu();
    wxMenuItem* clearhist =  m_tools_history_menu->Append(wxID_ANY, _("Clear History"));
    m_tools_enable_history = m_tools_history_menu->AppendCheckItem(wxID_ANY, _("Enable History"));
    m_tools_history_menu->AppendSeparator();

    m_tools_menu->AppendSubMenu(m_tools_history_menu, "History");

    //Create an editing menu
    wxMenu* editmenu = new wxMenu();
    m_edit_cut = editmenu->Append(wxID_ANY, _("Cut"));
    m_edit_copy = editmenu->Append(wxID_ANY, _("Copy"));
    m_edit_paste = editmenu->Append(wxID_ANY, _("Paste"));
    editmenu->AppendSeparator();
    m_edit_undo = editmenu->Append(wxID_ANY, _("Undo"));
    m_edit_redo = editmenu->Append(wxID_ANY, _("Redo"));
    editmenu->AppendSeparator();
    m_edit_mode = editmenu->AppendCheckItem(wxID_ANY, _("Edit Mode"));

    m_tools_menu->AppendSeparator();
    m_tools_menu->AppendSubMenu(editmenu, "Edit");

    wxMenu* scroll_menu = new wxMenu;
    m_scroll_line_up = scroll_menu->Append(wxID_ANY, "Line &up");
    m_scroll_line_down = scroll_menu->Append(wxID_ANY, "Line &down");
    m_scroll_page_up = scroll_menu->Append(wxID_ANY, "Page u&p");
    m_scroll_page_down = scroll_menu->Append(wxID_ANY, "Page d&own");
    m_tools_menu->AppendSubMenu(scroll_menu, "Scroll");

    wxMenu* script_menu = new wxMenu;
    m_script_string = script_menu->Append(wxID_ANY, "Return String");
    m_script_integer = script_menu->Append(wxID_ANY, "Return integer");
    m_script_double = script_menu->Append(wxID_ANY, "Return double");
    m_script_bool = script_menu->Append(wxID_ANY, "Return bool");
    m_script_object = script_menu->Append(wxID_ANY, "Return JSON object");
    m_script_array = script_menu->Append(wxID_ANY, "Return array");
    m_script_dom = script_menu->Append(wxID_ANY, "Modify DOM");
    m_script_undefined = script_menu->Append(wxID_ANY, "Return undefined");
    m_script_null = script_menu->Append(wxID_ANY, "Return null");
    m_script_date = script_menu->Append(wxID_ANY, "Return Date");
#if wxUSE_WEBVIEW_IE
    if (!wxWebView::IsBackendAvailable(wxWebViewBackendEdge))
    {
        m_script_object_el = script_menu->Append(wxID_ANY, "Return JSON object changing emulation level");
        m_script_date_el = script_menu->Append(wxID_ANY, "Return Date changing emulation level");
        m_script_array_el = script_menu->Append(wxID_ANY, "Return array changing emulation level");
    }
#endif
    m_script_async = script_menu->Append(wxID_ANY, "Return String async");
    m_script_message = script_menu->Append(wxID_ANY, "Send script message");
    m_script_custom = script_menu->Append(wxID_ANY, "Custom script");
    m_tools_menu->AppendSubMenu(script_menu, _("Run Script"));
    wxMenuItem* addUserScript = m_tools_menu->Append(wxID_ANY, _("Add user script"));
    wxMenuItem* setCustomUserAgent = m_tools_menu->Append(wxID_ANY, _("Set custom user agent"));

    //Selection menu
    wxMenu* selection = new wxMenu();
    m_selection_clear = selection->Append(wxID_ANY, _("Clear Selection"));
    m_selection_delete = selection->Append(wxID_ANY, _("Delete Selection"));
    wxMenuItem* selectall = selection->Append(wxID_ANY, _("Select All"));

    editmenu->AppendSubMenu(selection, "Selection");

    wxMenu* handlers = new wxMenu();
    wxMenuItem* loadscheme =  handlers->Append(wxID_ANY, _("Custom Scheme"));
    wxMenuItem* usememoryfs =  handlers->Append(wxID_ANY, _("Memory File System"));
    wxMenuItem* advancedHandler =  handlers->Append(wxID_ANY, _("Advanced Handler"));
    m_tools_menu->AppendSubMenu(handlers, _("Handler Examples"));

    m_context_menu = m_tools_menu->AppendCheckItem(wxID_ANY, _("Enable Context Menu"));
    m_dev_tools = m_tools_menu->AppendCheckItem(wxID_ANY, _("Enable Dev Tools"));
    m_browser_accelerator_keys = m_tools_menu->AppendCheckItem(wxID_ANY, _("Enable Browser Accelerator Keys"));

    //By default we want to handle navigation and new windows
    m_tools_handle_navigation->Check();
    m_tools_handle_new_window->Check();
    m_tools_enable_history->Check();

    //Zoom
    m_zoomFactor = 100;
    m_tools_medium->Check();

    if(!m_browser->CanSetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT))
        m_tools_layout->Enable(false);

    // Connect the toolbar events
    Bind(wxEVT_TOOL, &WebFrame::OnBack, this, m_toolbar_back->GetId());
    Bind(wxEVT_TOOL, &WebFrame::OnForward, this, m_toolbar_forward->GetId());
    Bind(wxEVT_TOOL, &WebFrame::OnStop, this, m_toolbar_stop->GetId());
    Bind(wxEVT_TOOL, &WebFrame::OnReload, this, m_toolbar_reload->GetId());
    Bind(wxEVT_TOOL, &WebFrame::OnToolsClicked, this, m_toolbar_tools->GetId());

    Bind(wxEVT_TEXT_ENTER, &WebFrame::OnUrl, this, m_url->GetId());

    // Connect find toolbar events.
    Bind(wxEVT_TOOL, &WebFrame::OnFindDone, this, m_find_toolbar_done->GetId());
    Bind(wxEVT_TOOL, &WebFrame::OnFindText, this, m_find_toolbar_next->GetId());
    Bind(wxEVT_TOOL, &WebFrame::OnFindText, this, m_find_toolbar_previous->GetId());

    // Connect find control events.
    Bind(wxEVT_TEXT, &WebFrame::OnFindText, this, m_find_ctrl->GetId());
    Bind(wxEVT_TEXT_ENTER, &WebFrame::OnFindText, this, m_find_ctrl->GetId());

    // Connect the webview events
    Bind(wxEVT_WEBVIEW_NAVIGATING, &WebFrame::OnNavigationRequest, this, m_browser->GetId());
    Bind(wxEVT_WEBVIEW_NAVIGATED, &WebFrame::OnNavigationComplete, this, m_browser->GetId());
    Bind(wxEVT_WEBVIEW_LOADED, &WebFrame::OnDocumentLoaded, this, m_browser->GetId());
    Bind(wxEVT_WEBVIEW_ERROR, &WebFrame::OnError, this, m_browser->GetId());
    Bind(wxEVT_WEBVIEW_NEWWINDOW, &WebFrame::OnNewWindow, this, m_browser->GetId());
    Bind(wxEVT_WEBVIEW_TITLE_CHANGED, &WebFrame::OnTitleChanged, this, m_browser->GetId());
    Bind(wxEVT_WEBVIEW_FULLSCREEN_CHANGED, &WebFrame::OnFullScreenChanged, this, m_browser->GetId());
    Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &WebFrame::OnScriptMessage, this, m_browser->GetId());
    Bind(wxEVT_WEBVIEW_SCRIPT_RESULT, &WebFrame::OnScriptResult, this, m_browser->GetId());

    // Connect the menu events
    Bind(wxEVT_MENU, &WebFrame::OnSetPage, this, setPage->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnViewSourceRequest, this, viewSource->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnViewTextRequest, this, viewText->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnPrint, this, print->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnZoomLayout, this, m_tools_layout->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnSetZoom, this, m_tools_tiny->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnSetZoom, this, m_tools_small->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnSetZoom, this, m_tools_medium->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnSetZoom, this, m_tools_large->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnSetZoom, this, m_tools_largest->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnSetZoom, this, m_tools_custom->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnClearHistory, this, clearhist->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnEnableHistory, this, m_tools_enable_history->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnCut, this, m_edit_cut->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnCopy, this, m_edit_copy->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnPaste, this, m_edit_paste->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnUndo, this, m_edit_undo->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRedo, this, m_edit_redo->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnMode, this, m_edit_mode->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnScrollLineUp, this, m_scroll_line_up->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnScrollLineDown, this, m_scroll_line_down->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnScrollPageUp, this, m_scroll_page_up->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnScrollPageDown, this, m_scroll_page_down->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptString, this, m_script_string->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptInteger, this, m_script_integer->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptDouble, this, m_script_double->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptBool, this, m_script_bool->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptObject, this, m_script_object->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptArray, this, m_script_array->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptDOM, this, m_script_dom->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptUndefined, this, m_script_undefined->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptNull, this, m_script_null->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptDate, this, m_script_date->GetId());
#if wxUSE_WEBVIEW_IE
    if (!wxWebView::IsBackendAvailable(wxWebViewBackendEdge))
    {
        Bind(wxEVT_MENU, &WebFrame::OnRunScriptObjectWithEmulationLevel, this, m_script_object_el->GetId());
        Bind(wxEVT_MENU, &WebFrame::OnRunScriptDateWithEmulationLevel, this, m_script_date_el->GetId());
        Bind(wxEVT_MENU, &WebFrame::OnRunScriptArrayWithEmulationLevel, this, m_script_array_el->GetId());
    }
#endif
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptMessage, this, m_script_message->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptCustom, this, m_script_custom->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnRunScriptAsync, this, m_script_async->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnAddUserScript, this, addUserScript->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnSetCustomUserAgent, this, setCustomUserAgent->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnClearSelection, this, m_selection_clear->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnDeleteSelection, this, m_selection_delete->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnSelectAll, this, selectall->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnLoadScheme, this, loadscheme->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnUseMemoryFS, this, usememoryfs->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnLoadAdvancedHandler, this, advancedHandler->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnFind, this, m_find->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnEnableContextMenu, this, m_context_menu->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnEnableDevTools, this, m_dev_tools->GetId());
    Bind(wxEVT_MENU, &WebFrame::OnEnableBrowserAcceleratorKeys, this, m_browser_accelerator_keys->GetId());

    //Connect the idle events
    Bind(wxEVT_IDLE, &WebFrame::OnIdle, this);
}

WebFrame::~WebFrame()
{
    delete m_tools_menu;
}

/**
  * Method that retrieves the current state from the web control and updates the GUI
  * the reflect this current state.
  */
void WebFrame::UpdateState()
{
    m_toolbar->EnableTool( m_toolbar_back->GetId(), m_browser->CanGoBack() );
    m_toolbar->EnableTool( m_toolbar_forward->GetId(), m_browser->CanGoForward() );

    if (m_browser->IsBusy())
    {
        m_toolbar->EnableTool( m_toolbar_stop->GetId(), true );
    }
    else
    {
        m_toolbar->EnableTool( m_toolbar_stop->GetId(), false );
    }

    SetTitle( m_browser->GetCurrentTitle() );
    m_url->SetValue( m_browser->GetCurrentURL() );
}

void WebFrame::OnIdle(wxIdleEvent& WXUNUSED(evt))
{
    if(m_browser->IsBusy())
    {
        wxSetCursor(wxCURSOR_ARROWWAIT);
        m_toolbar->EnableTool(m_toolbar_stop->GetId(), true);
    }
    else
    {
        wxSetCursor(wxNullCursor);
        m_toolbar->EnableTool(m_toolbar_stop->GetId(), false);
    }
}

/**
  * Callback invoked when user entered an URL and pressed enter
  */
void WebFrame::OnUrl(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->LoadURL( m_url->GetValue() );
    m_browser->SetFocus();
    UpdateState();
}

/**
    * Callback invoked when user pressed the "back" button
    */
void WebFrame::OnBack(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->GoBack();
    UpdateState();
}

/**
  * Callback invoked when user pressed the "forward" button
  */
void WebFrame::OnForward(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->GoForward();
    UpdateState();
}

/**
  * Callback invoked when user pressed the "stop" button
  */
void WebFrame::OnStop(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Stop();
    UpdateState();
}

/**
  * Callback invoked when user pressed the "reload" button
  */
void WebFrame::OnReload(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Reload();
    UpdateState();
}

void WebFrame::OnClearHistory(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->ClearHistory();
    UpdateState();
}

void WebFrame::OnEnableHistory(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->EnableHistory(m_tools_enable_history->IsChecked());
    UpdateState();
}

void WebFrame::OnCut(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Cut();
}

void WebFrame::OnCopy(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Copy();
}

void WebFrame::OnPaste(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Paste();
}

void WebFrame::OnUndo(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Undo();
}

void WebFrame::OnRedo(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Redo();
}

void WebFrame::OnMode(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->SetEditable(m_edit_mode->IsChecked());
}

void WebFrame::OnLoadScheme(wxCommandEvent& WXUNUSED(evt))
{
    wxPathList pathlist;
    pathlist.Add(".");
    pathlist.Add("..");
    pathlist.Add("../help");
    pathlist.Add("../../../samples/help");

    wxString path = wxFileName(pathlist.FindValidPath("doc.zip")).GetAbsolutePath();
    //Under MSW we need to flip the slashes
    path.Replace("\\", "/");
    path = "wxfs:///" + path + ";protocol=zip/doc.htm";
    m_browser->LoadURL(path);
}

void WebFrame::OnUseMemoryFS(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->LoadURL("memory:page1.htm");
}

void WebFrame::OnLoadAdvancedHandler(wxCommandEvent& WXUNUSED(evt))
{
    wxPathList pathlist;
    pathlist.Add(".");
    pathlist.Add("..");

    wxString path = wxFileName(pathlist.FindValidPath("handler_advanced.html")).GetAbsolutePath();
    path = "file://" + path;
    m_browser->LoadURL(path);
}

void WebFrame::OnEnableContextMenu(wxCommandEvent& evt)
{
    m_browser->EnableContextMenu(evt.IsChecked());
}

void WebFrame::OnEnableDevTools(wxCommandEvent& evt)
{
    m_browser->EnableAccessToDevTools(evt.IsChecked());
}

void WebFrame::OnEnableBrowserAcceleratorKeys(wxCommandEvent& evt)
{
    m_browser->EnableBrowserAcceleratorKeys(evt.IsChecked());
}

void WebFrame::OnFind(wxCommandEvent& WXUNUSED(evt))
{
    wxString value = m_browser->GetSelectedText();
    if(value.Len() > 150)
    {
        value.Truncate(150);
    }
    m_find_ctrl->SetValue(value);
    if(!m_find_toolbar->IsShown()){
        m_find_toolbar->Show(true);
        SendSizeEvent();
    }
    m_find_ctrl->SelectAll();
}

void WebFrame::OnFindDone(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Find("");
    m_find_toolbar->Show(false);
    SendSizeEvent();
}

void WebFrame::OnFindText(wxCommandEvent& evt)
{
    int flags = 0;

    if(m_find_toolbar_wrap->IsChecked())
        flags |= wxWEBVIEW_FIND_WRAP;
    if(m_find_toolbar_wholeword->IsChecked())
        flags |= wxWEBVIEW_FIND_ENTIRE_WORD;
    if(m_find_toolbar_matchcase->IsChecked())
        flags |= wxWEBVIEW_FIND_MATCH_CASE;
    if(m_find_toolbar_highlight->IsChecked())
        flags |= wxWEBVIEW_FIND_HIGHLIGHT_RESULT;

    if(m_find_toolbar_previous->GetId() == evt.GetId())
        flags |= wxWEBVIEW_FIND_BACKWARDS;

    wxString find_text = m_find_ctrl->GetValue();
    long count = m_browser->Find(find_text, flags);

    if(m_findText != find_text)
    {
        m_findCount = count;
        m_findText = find_text;
    }

    if(count != wxNOT_FOUND || find_text.IsEmpty())
    {
        m_find_ctrl->SetBackgroundColour(*wxWHITE);
    }
    else
    {
        m_find_ctrl->SetBackgroundColour(wxColour(255, 101, 101));
    }

    m_find_ctrl->Refresh();

    //Log the result, note that count is zero indexed.
    if(count != m_findCount)
    {
        count++;
    }
    wxLogMessage("Searching for:%s  current match:%li/%i", m_findText, count, m_findCount);
}

/**
  * Callback invoked when there is a request to load a new page (for instance
  * when the user clicks a link)
  */
void WebFrame::OnNavigationRequest(wxWebViewEvent& evt)
{
    if(m_info->IsShown())
    {
        m_info->Dismiss();
    }

    wxLogMessage("%s", "Navigation request to '" + evt.GetURL() + "' (target='" +
    evt.GetTarget() + "')");

    //If we don't want to handle navigation then veto the event and navigation
    //will not take place, we also need to stop the loading animation
    if(!m_tools_handle_navigation->IsChecked())
    {
        evt.Veto();
        m_toolbar->EnableTool( m_toolbar_stop->GetId(), false );
    }
    else
    {
        UpdateState();
    }
}

/**
  * Callback invoked when a navigation request was accepted
  */
void WebFrame::OnNavigationComplete(wxWebViewEvent& evt)
{
    wxLogMessage("%s", "Navigation complete; url='" + evt.GetURL() + "'");
    UpdateState();
}

/**
  * Callback invoked when a page is finished loading
  */
void WebFrame::OnDocumentLoaded(wxWebViewEvent& evt)
{
    //Only notify if the document is the main frame, not a subframe
    if(evt.GetURL() == m_browser->GetCurrentURL())
    {
        wxLogMessage("%s", "Document loaded; url='" + evt.GetURL() + "'");
    }
    UpdateState();
}

/**
  * On new window, we veto to stop extra windows appearing
  */
void WebFrame::OnNewWindow(wxWebViewEvent& evt)
{
    wxString flag = " (other)";

    if(evt.GetNavigationAction() == wxWEBVIEW_NAV_ACTION_USER)
    {
        flag = " (user)";
    }

    wxLogMessage("%s", "New window; url='" + evt.GetURL() + "'" + flag);

    //If we handle new window events then just load them in this window as we
    //are a single window browser
    if(m_tools_handle_new_window->IsChecked())
        m_browser->LoadURL(evt.GetURL());

    UpdateState();
}

void WebFrame::OnTitleChanged(wxWebViewEvent& evt)
{
    SetTitle(evt.GetString());
    wxLogMessage("%s", "Title changed; title='" + evt.GetString() + "'");
}

void WebFrame::OnFullScreenChanged(wxWebViewEvent & evt)
{
    wxLogMessage("Full screen changed; status = %d", evt.GetInt());
    ShowFullScreen(evt.GetInt() != 0);
}

void WebFrame::OnScriptMessage(wxWebViewEvent& evt)
{
    wxLogMessage("Script message received; value = %s, handler = %s", evt.GetString(), evt.GetMessageHandler());
}

void WebFrame::OnScriptResult(wxWebViewEvent& evt)
{
    if (evt.IsError())
        wxLogError("Async script execution failed: %s", evt.GetString());
    else
        wxLogMessage("Async script result received; value = %s", evt.GetString());
}

void WebFrame::OnSetPage(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->SetPage
               (
                "<html><title>New Page</title>"
                "<body>Created using <tt>SetPage()</tt> method.</body></html>",
                wxString()
               );
}

/**
  * Invoked when user selects the "View Source" menu item
  */
void WebFrame::OnViewSourceRequest(wxCommandEvent& WXUNUSED(evt))
{
    SourceViewDialog dlg(this, m_browser->GetPageSource());
    dlg.ShowModal();
}

/**
 * Invoked when user selects the "View Text" menu item
 */
void WebFrame::OnViewTextRequest(wxCommandEvent& WXUNUSED(evt))
{
    wxDialog textViewDialog(this, wxID_ANY, "Page Text",
                            wxDefaultPosition, wxSize(700,500),
                            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
#if wxUSE_STC
    wxStyledTextCtrl* text = new wxStyledTextCtrl(&textViewDialog, wxID_ANY);
    text->SetText(m_browser->GetPageText());
#else // !wxUSE_STC
    wxTextCtrl* text = new wxTextCtrl(this, wxID_ANY, m_browser->GetPageText(),
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_MULTILINE |
                                      wxTE_RICH |
                                      wxTE_READONLY);
#endif // wxUSE_STC/!wxUSE_STC
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(text, 1, wxEXPAND);
    SetSizer(sizer);
    textViewDialog.ShowModal();
}

/**
  * Invoked when user selects the "Menu" item
  */
void WebFrame::OnToolsClicked(wxCommandEvent& WXUNUSED(evt))
{
    if(m_browser->GetCurrentURL() == "")
        return;

    m_edit_cut->Enable(m_browser->CanCut());
    m_edit_copy->Enable(m_browser->CanCopy());
    m_edit_paste->Enable(m_browser->CanPaste());

    m_edit_undo->Enable(m_browser->CanUndo());
    m_edit_redo->Enable(m_browser->CanRedo());

    m_selection_clear->Enable(m_browser->HasSelection());
    m_selection_delete->Enable(m_browser->HasSelection());

    m_context_menu->Check(m_browser->IsContextMenuEnabled());
    m_dev_tools->Check(m_browser->IsAccessToDevToolsEnabled());
    m_browser_accelerator_keys->Check(m_browser->AreBrowserAcceleratorKeysEnabled());

    //Firstly we clear the existing menu items, then we add the current ones
    wxMenuHistoryMap::const_iterator it;
    for( it = m_histMenuItems.begin(); it != m_histMenuItems.end(); ++it )
    {
        m_tools_history_menu->Destroy(it->first);
    }
    m_histMenuItems.clear();

    wxVector<wxSharedPtr<wxWebViewHistoryItem> > back = m_browser->GetBackwardHistory();
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > forward = m_browser->GetForwardHistory();

    wxMenuItem* item;

    unsigned int i;
    for(i = 0; i < back.size(); i++)
    {
        item = m_tools_history_menu->AppendRadioItem(wxID_ANY, back[i]->GetTitle());
        m_histMenuItems[item->GetId()] = back[i];
        Bind(wxEVT_MENU, &WebFrame::OnHistory, this, item->GetId());
    }

    wxString title = m_browser->GetCurrentTitle();
    if ( title.empty() )
        title = "(untitled)";
    item = m_tools_history_menu->AppendRadioItem(wxID_ANY, title);
    item->Check();

    //No need to connect the current item
    m_histMenuItems[item->GetId()] = wxSharedPtr<wxWebViewHistoryItem>(new wxWebViewHistoryItem(m_browser->GetCurrentURL(), m_browser->GetCurrentTitle()));

    for(i = 0; i < forward.size(); i++)
    {
        item = m_tools_history_menu->AppendRadioItem(wxID_ANY, forward[i]->GetTitle());
        m_histMenuItems[item->GetId()] = forward[i];
        Bind(wxEVT_TOOL, &WebFrame::OnHistory, this, item->GetId());
    }

    wxPoint position = ScreenToClient( wxGetMousePosition() );
    PopupMenu(m_tools_menu, position.x, position.y);
}

/**
  * Invoked when user selects the zoom size in the menu
  */
void WebFrame::OnSetZoom(wxCommandEvent& evt)
{
    if (evt.GetId() == m_tools_tiny->GetId())
    {
        m_browser->SetZoom(wxWEBVIEW_ZOOM_TINY);
    }
    else if (evt.GetId() == m_tools_small->GetId())
    {
        m_browser->SetZoom(wxWEBVIEW_ZOOM_SMALL);
    }
    else if (evt.GetId() == m_tools_medium->GetId())
    {
        m_browser->SetZoom(wxWEBVIEW_ZOOM_MEDIUM);
    }
    else if (evt.GetId() == m_tools_large->GetId())
    {
        m_browser->SetZoom(wxWEBVIEW_ZOOM_LARGE);
    }
    else if (evt.GetId() == m_tools_largest->GetId())
    {
        m_browser->SetZoom(wxWEBVIEW_ZOOM_LARGEST);
    }
    else if (evt.GetId() == m_tools_custom->GetId())
    {
        m_zoomFactor = m_browser->GetZoomFactor() * 100;
        OnZoomCustom(evt);
    }
    else
    {
        wxFAIL;
    }
}

void WebFrame::OnZoomLayout(wxCommandEvent& WXUNUSED(evt))
{
    if(m_tools_layout->IsChecked())
        m_browser->SetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT);
    else
        m_browser->SetZoomType(wxWEBVIEW_ZOOM_TYPE_TEXT);
}

void WebFrame::OnZoomCustom(wxCommandEvent& WXUNUSED(evt))
{
    wxNumberEntryDialog dialog
                      (
                        this,
                        "Enter zoom factor as a percentage (10-10000)%",
                        "Zoom Factor:",
                        "Change Zoom Factor",
                        m_zoomFactor,
                        10, 10000
                      );
    if( dialog.ShowModal() != wxID_OK )
        return;

    m_zoomFactor = dialog.GetValue();
    m_browser->SetZoomFactor((float)m_zoomFactor/100);
}

void WebFrame::OnHistory(wxCommandEvent& evt)
{
    m_browser->LoadHistoryItem(m_histMenuItems[evt.GetId()]);
}

void WebFrame::RunScript(const wxString& javascript)
{
    // Remember the script we run in any case, so the next time the user opens
    // the "Run Script" dialog box, it is shown there for convenient updating.
    m_javascript = javascript;

    wxLogMessage("Running JavaScript:\n%s\n", javascript);

    wxString result;
    if ( m_browser->RunScript(javascript, &result) )
    {
        wxLogMessage("RunScript() returned \"%s\"", result);
    }
    else
    {
        wxLogWarning("RunScript() failed");
    }
}

void WebFrame::OnRunScriptString(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(a){return a;}f('Hello World!');");
}

void WebFrame::OnRunScriptInteger(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(a){return a;}f(123);");
}

void WebFrame::OnRunScriptDouble(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(a){return a;}f(2.34);");
}

void WebFrame::OnRunScriptBool(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(a){return a;}f(false);");
}

void WebFrame::OnRunScriptObject(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(){var person = new Object();person.name = 'Foo'; \
        person.lastName = 'Bar';return person;}f();");
}

void WebFrame::OnRunScriptArray(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(){ return [\"foo\", \"bar\"]; }f();");
}

void WebFrame::OnRunScriptDOM(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("document.write(\"Hello World!\");");
}

void WebFrame::OnRunScriptUndefined(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(){var person = new Object();}f();");
}

void WebFrame::OnRunScriptNull(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(){return null;}f();");
}

void WebFrame::OnRunScriptDate(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("function f(){var d = new Date('10/08/2017 21:30:40'); \
        var tzoffset = d.getTimezoneOffset() * 60000; \
        return new Date(d.getTime() - tzoffset);}f();");
}

#if wxUSE_WEBVIEW_IE
void WebFrame::OnRunScriptObjectWithEmulationLevel(wxCommandEvent& WXUNUSED(evt))
{
    wxWebViewIE::MSWSetModernEmulationLevel();
    RunScript("function f(){var person = new Object();person.name = 'Foo'; \
        person.lastName = 'Bar';return person;}f();");
    wxWebViewIE::MSWSetModernEmulationLevel(false);
}

void WebFrame::OnRunScriptDateWithEmulationLevel(wxCommandEvent& WXUNUSED(evt))
{
    wxWebViewIE::MSWSetModernEmulationLevel();
    RunScript("function f(){var d = new Date('10/08/2017 21:30:40'); \
        var tzoffset = d.getTimezoneOffset() * 60000; return \
        new Date(d.getTime() - tzoffset);}f();");
    wxWebViewIE::MSWSetModernEmulationLevel(false);
}

void WebFrame::OnRunScriptArrayWithEmulationLevel(wxCommandEvent& WXUNUSED(evt))
{
    wxWebViewIE::MSWSetModernEmulationLevel();
    RunScript("function f(){ return [\"foo\", \"bar\"]; }f();");
    wxWebViewIE::MSWSetModernEmulationLevel(false);
}
#endif

void WebFrame::OnRunScriptMessage(wxCommandEvent& WXUNUSED(evt))
{
    RunScript("window.wx.postMessage('This is a web message');");
}

void WebFrame::OnRunScriptAsync(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->RunScriptAsync("function f(a){return a;}f('Hello World!');");
}

void WebFrame::OnRunScriptCustom(wxCommandEvent& WXUNUSED(evt))
{
    wxTextEntryDialog dialog
                      (
                        this,
                        "Please enter JavaScript code to execute",
                        wxGetTextFromUserPromptStr,
                        m_javascript,
                        wxOK | wxCANCEL | wxCENTRE | wxTE_MULTILINE
                      );
    if( dialog.ShowModal() != wxID_OK )
        return;

    RunScript(dialog.GetValue());
}

void WebFrame::OnAddUserScript(wxCommandEvent & WXUNUSED(evt))
{
    wxString userScript = "window.wx_test_var = 'wxWidgets webview sample';";
    wxTextEntryDialog dialog
    (
        this,
        "Enter the JavaScript code to run as the initialization script that runs before any script in the HTML document.",
        wxGetTextFromUserPromptStr,
        userScript,
        wxOK | wxCANCEL | wxCENTRE | wxTE_MULTILINE
    );
    if (dialog.ShowModal() != wxID_OK)
        return;

    if (!m_browser->AddUserScript(dialog.GetValue()))
        wxLogError("Could not add user script");
}

void WebFrame::OnSetCustomUserAgent(wxCommandEvent& WXUNUSED(evt))
{
    wxString customUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.1 Mobile/15E148 Safari/604.1";
    wxTextEntryDialog dialog
    (
        this,
        "Enter the custom user agent string you would like to use.",
        wxGetTextFromUserPromptStr,
        customUserAgent,
        wxOK | wxCANCEL | wxCENTRE
    );
    if (dialog.ShowModal() != wxID_OK)
        return;

    if (!m_browser->SetUserAgent(customUserAgent))
        wxLogError("Could not set custom user agent");
}

void WebFrame::OnClearSelection(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->ClearSelection();
}

void WebFrame::OnDeleteSelection(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->DeleteSelection();
}

void WebFrame::OnSelectAll(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->SelectAll();
}

/**
  * Callback invoked when a loading error occurs
  */
void WebFrame::OnError(wxWebViewEvent& evt)
{
#define WX_ERROR_CASE(type) \
    case type: \
        category = #type; \
        break;

    wxString category;
    switch (evt.GetInt())
    {
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_CONNECTION);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_CERTIFICATE);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_AUTH);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_SECURITY);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_NOT_FOUND);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_REQUEST);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_USER_CANCELLED);
        WX_ERROR_CASE(wxWEBVIEW_NAV_ERR_OTHER);
    }

    wxLogMessage("%s", "Error; url='" + evt.GetURL() + "', error='" + category + " (" + evt.GetString() + ")'");

    //Show the info bar with an error
    m_info->ShowMessage(_("An error occurred loading ") + evt.GetURL() + "\n" +
    "'" + category + "'", wxICON_ERROR);

    UpdateState();
}

/**
  * Invoked when user selects "Print" from the menu
  */
void WebFrame::OnPrint(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->Print();
}

SourceViewDialog::SourceViewDialog(wxWindow* parent, wxString source) :
                  wxDialog(parent, wxID_ANY, "Source Code",
                           wxDefaultPosition, wxSize(700,500),
                           wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
#if wxUSE_STC
    wxStyledTextCtrl* text = new wxStyledTextCtrl(this, wxID_ANY);
    text->SetMarginWidth(1, 30);
    text->SetMarginType(1, wxSTC_MARGIN_NUMBER);
    text->SetText(source);

    text->StyleClearAll();
    text->SetLexer(wxSTC_LEX_HTML);
    text->StyleSetForeground(wxSTC_H_DOUBLESTRING, wxColour(255,0,0));
    text->StyleSetForeground(wxSTC_H_SINGLESTRING, wxColour(255,0,0));
    text->StyleSetForeground(wxSTC_H_ENTITY, wxColour(255,0,0));
    text->StyleSetForeground(wxSTC_H_TAG, wxColour(0,150,0));
    text->StyleSetForeground(wxSTC_H_TAGUNKNOWN, wxColour(0,150,0));
    text->StyleSetForeground(wxSTC_H_ATTRIBUTE, wxColour(0,0,150));
    text->StyleSetForeground(wxSTC_H_ATTRIBUTEUNKNOWN, wxColour(0,0,150));
    text->StyleSetForeground(wxSTC_H_COMMENT, wxColour(150,150,150));
#else // !wxUSE_STC
    wxTextCtrl* text = new wxTextCtrl(this, wxID_ANY, source,
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_MULTILINE |
                                      wxTE_RICH |
                                      wxTE_READONLY);
#endif // wxUSE_STC/!wxUSE_STC

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(text, 1, wxEXPAND);
    SetSizer(sizer);
}
