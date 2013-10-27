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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_WEBVIEW_WEBKIT && !wxUSE_WEBVIEW_IE
#error "A wxWebView backend is required by this sample"
#endif

#include "wx/artprov.h"
#include "wx/cmdline.h"
#include "wx/notifmsg.h"
#include "wx/settings.h"
#include "wx/webview.h"
#include "wx/webviewarchivehandler.h"
#include "wx/webviewfshandler.h"
#include "wx/infobar.h"
#include "wx/filesys.h"
#include "wx/fs_arc.h"
#include "wx/fs_mem.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#if wxUSE_STC
#include "wx/stc/stc.h"
#else
#error "wxStyledTextControl is needed by this sample"
#endif

#if defined(__WXMSW__) || defined(__WXOSX__)
#include "stop.xpm"
#include "refresh.xpm"
#endif

#include "wxlogo.xpm"


//We map menu items to their history items
WX_DECLARE_HASH_MAP(int, wxSharedPtr<wxWebViewHistoryItem>,
                    wxIntegerHash, wxIntegerEqual, wxMenuHistoryMap);

class WebApp : public wxApp
{
public:
    WebApp() :
        m_url("http://www.wxwidgets.org")
    {
    }

    virtual bool OnInit();

#if wxUSE_CMDLINE_PARSER
    virtual void OnInitCmdLine(wxCmdLineParser& parser)
    {
        wxApp::OnInitCmdLine(parser);

        parser.AddParam("URL to open",
                        wxCMD_LINE_VAL_STRING,
                        wxCMD_LINE_PARAM_OPTIONAL);
    }

    virtual bool OnCmdLineParsed(wxCmdLineParser& parser)
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
    void OnViewSourceRequest(wxCommandEvent& evt);
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
    void OnHistory(wxCommandEvent& evt);
    void OnScrollLineUp(wxCommandEvent&) { m_browser->LineUp(); }
    void OnScrollLineDown(wxCommandEvent&) { m_browser->LineDown(); }
    void OnScrollPageUp(wxCommandEvent&) { m_browser->PageUp(); }
    void OnScrollPageDown(wxCommandEvent&) { m_browser->PageDown(); }
    void OnRunScript(wxCommandEvent& evt);
    void OnClearSelection(wxCommandEvent& evt);
    void OnDeleteSelection(wxCommandEvent& evt);
    void OnSelectAll(wxCommandEvent& evt);
    void OnLoadScheme(wxCommandEvent& evt);
    void OnUseMemoryFS(wxCommandEvent& evt);
    void OnFind(wxCommandEvent& evt);
    void OnFindDone(wxCommandEvent& evt);
    void OnFindText(wxCommandEvent& evt);
    void OnFindOptions(wxCommandEvent& evt);
    void OnEnableContextMenu(wxCommandEvent& evt);

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
    wxMenuItem* m_selection_clear;
    wxMenuItem* m_selection_delete;
    wxMenuItem* m_find;
    wxMenuItem* m_context_menu;

    wxInfoBar *m_info;
    wxStaticText* m_info_text;
    wxTextCtrl* m_find_ctrl;
    wxToolBar* m_find_toolbar;

    wxMenuHistoryMap m_histMenuItems;
    wxString m_findText;
    int m_findFlags, m_findCount;
};

class SourceViewDialog : public wxDialog
{
public:
    SourceViewDialog(wxWindow* parent, wxString source);
};

IMPLEMENT_APP(WebApp)

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
    wxFrame(NULL, wxID_ANY, "wxWebView Sample")
{
    // set the frame icon
    SetIcon(wxICON(sample));
    SetTitle("wxWebView Sample");

    wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

    // Create the toolbar
    m_toolbar = CreateToolBar(wxTB_TEXT);
    m_toolbar->SetToolBitmapSize(wxSize(32, 32));

    wxBitmap back = wxArtProvider::GetBitmap(wxART_GO_BACK , wxART_TOOLBAR);
    wxBitmap forward = wxArtProvider::GetBitmap(wxART_GO_FORWARD , wxART_TOOLBAR);
    #ifdef __WXGTK__
        wxBitmap stop = wxArtProvider::GetBitmap("gtk-stop", wxART_TOOLBAR);
    #else
        wxBitmap stop = wxBitmap(stop_xpm);
    #endif
    #ifdef __WXGTK__
        wxBitmap refresh = wxArtProvider::GetBitmap("gtk-refresh", wxART_TOOLBAR);
    #else
        wxBitmap refresh = wxBitmap(refresh_xpm);
    #endif

    m_toolbar_back = m_toolbar->AddTool(wxID_ANY, _("Back"), back);
    m_toolbar_forward = m_toolbar->AddTool(wxID_ANY, _("Forward"), forward);
    m_toolbar_stop = m_toolbar->AddTool(wxID_ANY, _("Stop"), stop);
    m_toolbar_reload = m_toolbar->AddTool(wxID_ANY, _("Reload"),  refresh);
    m_url = new wxTextCtrl(m_toolbar, wxID_ANY, wxT(""),  wxDefaultPosition, wxSize(400, -1), wxTE_PROCESS_ENTER );
    m_toolbar->AddControl(m_url, _("URL"));
    m_toolbar_tools = m_toolbar->AddTool(wxID_ANY, _("Menu"), wxBitmap(wxlogo_xpm));

    m_toolbar->Realize();

    // Set find values.
    m_findFlags = wxWEBVIEW_FIND_DEFAULT;
    m_findText = wxEmptyString;
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

    // Create the webview
    m_browser = wxWebView::New(this, wxID_ANY, url);
    topsizer->Add(m_browser, wxSizerFlags().Expand().Proportion(1));

    //We register the wxfs:// protocol for testing purposes
    m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewArchiveHandler("wxfs")));
    //And the memory: file system
    m_browser->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler("memory")));

    SetSizer(topsizer);

    //Set a more sensible size for web browsing
    SetSize(wxSize(800, 600));

    // Create a log window
    new wxLogWindow(this, _("Logging"), true, false);

    // Create the Tools menu
    m_tools_menu = new wxMenu();
    wxMenuItem* print = m_tools_menu->Append(wxID_ANY , _("Print"));
    wxMenuItem* viewSource = m_tools_menu->Append(wxID_ANY , _("View Source"));
    m_tools_menu->AppendSeparator();
    m_tools_layout = m_tools_menu->AppendCheckItem(wxID_ANY, _("Use Layout Zoom"));
    m_tools_tiny = m_tools_menu->AppendCheckItem(wxID_ANY, _("Tiny"));
    m_tools_small = m_tools_menu->AppendCheckItem(wxID_ANY, _("Small"));
    m_tools_medium = m_tools_menu->AppendCheckItem(wxID_ANY, _("Medium"));
    m_tools_large = m_tools_menu->AppendCheckItem(wxID_ANY, _("Large"));
    m_tools_largest = m_tools_menu->AppendCheckItem(wxID_ANY, _("Largest"));
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

    wxMenuItem* script =  m_tools_menu->Append(wxID_ANY, _("Run Script"));

    //Selection menu
    wxMenu* selection = new wxMenu();
    m_selection_clear = selection->Append(wxID_ANY, _("Clear Selection"));
    m_selection_delete = selection->Append(wxID_ANY, _("Delete Selection"));
    wxMenuItem* selectall = selection->Append(wxID_ANY, _("Select All"));

    editmenu->AppendSubMenu(selection, "Selection");

    wxMenuItem* loadscheme =  m_tools_menu->Append(wxID_ANY, _("Custom Scheme Example"));
    wxMenuItem* usememoryfs =  m_tools_menu->Append(wxID_ANY, _("Memory File System Example"));

    m_context_menu = m_tools_menu->AppendCheckItem(wxID_ANY, _("Enable Context Menu"));

    //By default we want to handle navigation and new windows
    m_tools_handle_navigation->Check();
    m_tools_handle_new_window->Check();
    m_tools_enable_history->Check();
    if(!m_browser->CanSetZoomType(wxWEBVIEW_ZOOM_TYPE_LAYOUT))
        m_tools_layout->Enable(false);


    // Connect the toolbar events
    Connect(m_toolbar_back->GetId(), wxEVT_TOOL,
            wxCommandEventHandler(WebFrame::OnBack), NULL, this );
    Connect(m_toolbar_forward->GetId(), wxEVT_TOOL,
            wxCommandEventHandler(WebFrame::OnForward), NULL, this );
    Connect(m_toolbar_stop->GetId(), wxEVT_TOOL,
            wxCommandEventHandler(WebFrame::OnStop), NULL, this );
    Connect(m_toolbar_reload->GetId(), wxEVT_TOOL,
            wxCommandEventHandler(WebFrame::OnReload),NULL, this );
    Connect(m_toolbar_tools->GetId(), wxEVT_TOOL,
            wxCommandEventHandler(WebFrame::OnToolsClicked), NULL, this );

    Connect(m_url->GetId(), wxEVT_TEXT_ENTER,
            wxCommandEventHandler(WebFrame::OnUrl), NULL, this );

    // Connect find toolbar events.
    Connect(m_find_toolbar_done->GetId(), wxEVT_TOOL,
            wxCommandEventHandler(WebFrame::OnFindDone), NULL, this );
    Connect(m_find_toolbar_next->GetId(), wxEVT_TOOL,
            wxCommandEventHandler(WebFrame::OnFindText), NULL, this );
    Connect(m_find_toolbar_previous->GetId(), wxEVT_TOOL,
            wxCommandEventHandler(WebFrame::OnFindText), NULL, this );

    // Connect find control events.
    Connect(m_find_ctrl->GetId(), wxEVT_TEXT,
            wxCommandEventHandler(WebFrame::OnFindText), NULL, this );
    Connect(m_find_ctrl->GetId(), wxEVT_TEXT_ENTER,
            wxCommandEventHandler(WebFrame::OnFindText), NULL, this );

    // Connect the webview events
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_NAVIGATING,
            wxWebViewEventHandler(WebFrame::OnNavigationRequest), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_NAVIGATED,
            wxWebViewEventHandler(WebFrame::OnNavigationComplete), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_LOADED,
            wxWebViewEventHandler(WebFrame::OnDocumentLoaded), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_ERROR,
            wxWebViewEventHandler(WebFrame::OnError), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_NEWWINDOW,
            wxWebViewEventHandler(WebFrame::OnNewWindow), NULL, this);
    Connect(m_browser->GetId(), wxEVT_WEBVIEW_TITLE_CHANGED,
            wxWebViewEventHandler(WebFrame::OnTitleChanged), NULL, this);

    // Connect the menu events
    Connect(viewSource->GetId(), wxEVT_MENU,
           wxCommandEventHandler(WebFrame::OnViewSourceRequest),  NULL, this );
    Connect(print->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnPrint),  NULL, this );
    Connect(m_tools_layout->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnZoomLayout),  NULL, this );
    Connect(m_tools_tiny->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_small->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_medium->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_large->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(m_tools_largest->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnSetZoom),  NULL, this );
    Connect(clearhist->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnClearHistory),  NULL, this );
    Connect(m_tools_enable_history->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnEnableHistory),  NULL, this );
    Connect(m_edit_cut->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnCut),  NULL, this );
    Connect(m_edit_copy->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnCopy),  NULL, this );
    Connect(m_edit_paste->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnPaste),  NULL, this );
    Connect(m_edit_undo->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnUndo),  NULL, this );
    Connect(m_edit_redo->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnRedo),  NULL, this );
    Connect(m_edit_mode->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnMode),  NULL, this );
    Connect(m_scroll_line_up->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnScrollLineUp),  NULL, this );
    Connect(m_scroll_line_down->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnScrollLineDown),  NULL, this );
    Connect(m_scroll_page_up->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnScrollPageUp),  NULL, this );
    Connect(m_scroll_page_down->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnScrollPageDown),  NULL, this );
    Connect(script->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnRunScript),  NULL, this );
    Connect(m_selection_clear->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnClearSelection),  NULL, this );
    Connect(m_selection_delete->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnDeleteSelection),  NULL, this );
    Connect(selectall->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnSelectAll),  NULL, this );
    Connect(loadscheme->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnLoadScheme),  NULL, this );
    Connect(usememoryfs->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnUseMemoryFS),  NULL, this );
    Connect(m_find->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnFind),  NULL, this );
    Connect(m_context_menu->GetId(), wxEVT_MENU,
            wxCommandEventHandler(WebFrame::OnEnableContextMenu), NULL, this );

    //Connect the idle events
    Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(WebFrame::OnIdle), NULL, this);
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
    wxFileName helpfile("../help/doc.zip");
    helpfile.MakeAbsolute();
    wxString path = helpfile.GetFullPath();
    //Under MSW we need to flip the slashes
    path.Replace("\\", "/");
    path = "wxfs:///" + path + ";protocol=zip/doc.htm";
    m_browser->LoadURL(path);
}

void WebFrame::OnUseMemoryFS(wxCommandEvent& WXUNUSED(evt))
{
    m_browser->LoadURL("memory:page1.htm");
}

void WebFrame::OnEnableContextMenu(wxCommandEvent& evt)
{
    m_browser->EnableContextMenu(evt.IsChecked());
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
    wxLogMessage("Searching for:%s  current match:%i/%i", m_findText.c_str(), count, m_findCount);
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

    wxASSERT(m_browser->IsBusy());

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
    wxLogMessage("%s", "New window; url='" + evt.GetURL() + "'");

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

/**
  * Invoked when user selects the "View Source" menu item
  */
void WebFrame::OnViewSourceRequest(wxCommandEvent& WXUNUSED(evt))
{
    SourceViewDialog dlg(this, m_browser->GetPageSource());
    dlg.ShowModal();
}

/**
  * Invoked when user selects the "Menu" item
  */
void WebFrame::OnToolsClicked(wxCommandEvent& WXUNUSED(evt))
{
    if(m_browser->GetCurrentURL() == "")
        return;

    m_tools_tiny->Check(false);
    m_tools_small->Check(false);
    m_tools_medium->Check(false);
    m_tools_large->Check(false);
    m_tools_largest->Check(false);

    wxWebViewZoom zoom = m_browser->GetZoom();
    switch (zoom)
    {
    case wxWEBVIEW_ZOOM_TINY:
        m_tools_tiny->Check();
        break;
    case wxWEBVIEW_ZOOM_SMALL:
        m_tools_small->Check();
        break;
    case wxWEBVIEW_ZOOM_MEDIUM:
        m_tools_medium->Check();
        break;
    case wxWEBVIEW_ZOOM_LARGE:
        m_tools_large->Check();
        break;
    case wxWEBVIEW_ZOOM_LARGEST:
        m_tools_largest->Check();
        break;
    }

    m_edit_cut->Enable(m_browser->CanCut());
    m_edit_copy->Enable(m_browser->CanCopy());
    m_edit_paste->Enable(m_browser->CanPaste());

    m_edit_undo->Enable(m_browser->CanUndo());
    m_edit_redo->Enable(m_browser->CanRedo());

    m_selection_clear->Enable(m_browser->HasSelection());
    m_selection_delete->Enable(m_browser->HasSelection());

    m_context_menu->Check(m_browser->IsContextMenuEnabled());

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
        Connect(item->GetId(), wxEVT_MENU,
                wxCommandEventHandler(WebFrame::OnHistory), NULL, this );
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
        Connect(item->GetId(), wxEVT_TOOL,
                wxCommandEventHandler(WebFrame::OnHistory), NULL, this );
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

void WebFrame::OnHistory(wxCommandEvent& evt)
{
    m_browser->LoadHistoryItem(m_histMenuItems[evt.GetId()]);
}

void WebFrame::OnRunScript(wxCommandEvent& WXUNUSED(evt))
{
    wxTextEntryDialog dialog(this, "Enter JavaScript to run.", wxGetTextFromUserPromptStr, "", wxOK|wxCANCEL|wxCENTRE|wxTE_MULTILINE);
    if(dialog.ShowModal() == wxID_OK)
    {
        m_browser->RunScript(dialog.GetValue());
    }
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

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(text, 1, wxEXPAND);
    SetSizer(sizer);
}
