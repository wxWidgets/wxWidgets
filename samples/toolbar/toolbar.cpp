/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

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

#include "wx/toolbar.h"
#include "wx/log.h"
#include "wx/image.h"
#include "wx/filedlg.h"
#include "wx/colordlg.h"
#include "wx/srchctrl.h"

// define this to use XPMs everywhere (by default, BMPs are used under Win)
// BMPs use less space, but aren't compiled into the executable on other platforms
#ifdef __WXMSW__
    #define USE_XPM_BITMAPS 0
#else
    #define USE_XPM_BITMAPS 1
#endif

#if USE_XPM_BITMAPS && defined(__WXMSW__) && !wxUSE_XPM_IN_MSW
    #error You need to enable XPM support to use XPM bitmaps with toolbar!
#endif // USE_XPM_BITMAPS

// If this is 1, the sample will test an extra toolbar identical to the
// main one, but not managed by the frame. This can test subtle differences
// in the way toolbars are handled, especially on Mac where there is one
// native, 'installed' toolbar.
#define USE_UNMANAGED_TOOLBAR 0

// Define this as 0 for the platforms not supporting controls in toolbars
#define USE_CONTROLS_IN_TOOLBAR 1

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

#if USE_XPM_BITMAPS
    #include "bitmaps/new.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/save.xpm"
    #include "bitmaps/copy.xpm"
    #include "bitmaps/cut.xpm"
    #include "bitmaps/preview.xpm"  // paste XPM
    #include "bitmaps/print.xpm"
    #include "bitmaps/help.xpm"
#endif // USE_XPM_BITMAPS

enum Positions
{
    TOOLBAR_LEFT,
    TOOLBAR_TOP,
    TOOLBAR_RIGHT,
    TOOLBAR_BOTTOM
};

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *parent,
            wxWindowID id = wxID_ANY,
            const wxString& title = wxT("wxToolBar Sample"),
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
    virtual ~MyFrame();

    void PopulateToolbar(wxToolBarBase* toolBar);
    void RecreateToolbar();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event);

    void OnToggleToolbar(wxCommandEvent& event);
    void OnToggleAnotherToolbar(wxCommandEvent& event);
    void OnToggleHorizontalText(wxCommandEvent& WXUNUSED(event));

    void OnToggleToolbarSize(wxCommandEvent& event);
    void OnChangeOrientation(wxCommandEvent& event);
    void OnToggleToolbarRows(wxCommandEvent& event);
    void OnToggleTooltips(wxCommandEvent& event);
    void OnToggleCustomDisabled(wxCommandEvent& event);

    void OnEnablePrint(wxCommandEvent& WXUNUSED(event)) { DoEnablePrint(); }
    void OnDeletePrint(wxCommandEvent& WXUNUSED(event)) { DoDeletePrint(); }
    void OnInsertPrint(wxCommandEvent& event);
    void OnChangeToolTip(wxCommandEvent& event);
    void OnToggleHelp(wxCommandEvent& WXUNUSED(event)) { DoToggleHelp(); }
    void OnToggleSearch(wxCommandEvent& event);
    void OnToggleRadioBtn(wxCommandEvent& event);

    void OnToolbarStyle(wxCommandEvent& event);
    void OnToolbarBgCol(wxCommandEvent& event);
    void OnToolbarCustomBitmap(wxCommandEvent& event);

    void OnToolLeftClick(wxCommandEvent& event);
    void OnToolRightClick(wxCommandEvent& event);
    void OnToolDropdown(wxCommandEvent& event);

    void OnCombo(wxCommandEvent& event);

    void OnUpdateCopyAndCut(wxUpdateUIEvent& event);
    void OnUpdateToggleHorzText(wxUpdateUIEvent& event);
    void OnUpdateToggleRadioBtn(wxUpdateUIEvent& event)
        { event.Enable( m_tbar != NULL ); }

private:
    void DoEnablePrint();
    void DoDeletePrint();
    void DoToggleHelp();

    void LayoutChildren();

    bool                m_smallToolbar,
                        m_horzText,
                        m_useCustomDisabled,
                        m_showTooltips;
    size_t              m_rows;             // 1 or 2 only

    // the number of print buttons we have (they're added/removed dynamically)
    size_t              m_nPrint;

    // store toolbar position for future use
    Positions           m_toolbarPosition;

    wxTextCtrl         *m_textWindow;

    wxPanel            *m_panel;
#if USE_UNMANAGED_TOOLBAR
    wxToolBar          *m_extraToolBar;
#endif

    wxToolBar          *m_tbar;

    // the path to the custom bitmap for the test toolbar tool
    wxString            m_pathBmp;

    // the search tool, initially NULL
    wxToolBarToolBase *m_searchTool;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const int ID_TOOLBAR = 500;

static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;

enum
{
    // toolbar menu items
    IDM_TOOLBAR_TOGGLE_TOOLBAR = 200,
    IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
    IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
    IDM_TOOLBAR_TOGGLETOOLBARSIZE,
    IDM_TOOLBAR_TOGGLETOOLBARROWS,
    IDM_TOOLBAR_TOGGLETOOLTIPS,
    IDM_TOOLBAR_TOGGLECUSTOMDISABLED,
    IDM_TOOLBAR_SHOW_TEXT,
    IDM_TOOLBAR_SHOW_ICONS,
    IDM_TOOLBAR_SHOW_BOTH,
    IDM_TOOLBAR_BG_COL,
    IDM_TOOLBAR_CUSTOM_PATH,
    IDM_TOOLBAR_TOP_ORIENTATION,
    IDM_TOOLBAR_LEFT_ORIENTATION,
    IDM_TOOLBAR_BOTTOM_ORIENTATION,
    IDM_TOOLBAR_RIGHT_ORIENTATION,
    IDM_TOOLBAR_OTHER_1,
    IDM_TOOLBAR_OTHER_2,
    IDM_TOOLBAR_OTHER_3,

    // tools menu items
    IDM_TOOLBAR_ENABLEPRINT,
    IDM_TOOLBAR_DELETEPRINT,
    IDM_TOOLBAR_INSERTPRINT,
    IDM_TOOLBAR_TOGGLEHELP,
    IDM_TOOLBAR_TOGGLESEARCH,
    IDM_TOOLBAR_TOGGLERADIOBTN1,
    IDM_TOOLBAR_TOGGLERADIOBTN2,
    IDM_TOOLBAR_TOGGLERADIOBTN3,
    IDM_TOOLBAR_CHANGE_TOOLTIP,

    ID_COMBO = 1000
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

// Notice that wxID_HELP will be processed for the 'About' menu and the toolbar
// help button.

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_SIZE(MyFrame::OnSize)

    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(wxID_HELP, MyFrame::OnAbout)

    EVT_MENU(IDM_TOOLBAR_TOGGLE_TOOLBAR, MyFrame::OnToggleToolbar)
    EVT_MENU(IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR, MyFrame::OnToggleAnotherToolbar)
    EVT_MENU(IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT, MyFrame::OnToggleHorizontalText)

    EVT_MENU_RANGE(IDM_TOOLBAR_TOP_ORIENTATION, IDM_TOOLBAR_RIGHT_ORIENTATION, MyFrame::OnChangeOrientation)
    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLBARSIZE, MyFrame::OnToggleToolbarSize)
    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLBARROWS, MyFrame::OnToggleToolbarRows)
    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLTIPS, MyFrame::OnToggleTooltips)
    EVT_MENU(IDM_TOOLBAR_TOGGLECUSTOMDISABLED, MyFrame::OnToggleCustomDisabled)

    EVT_MENU(IDM_TOOLBAR_ENABLEPRINT, MyFrame::OnEnablePrint)
    EVT_MENU(IDM_TOOLBAR_DELETEPRINT, MyFrame::OnDeletePrint)
    EVT_MENU(IDM_TOOLBAR_INSERTPRINT, MyFrame::OnInsertPrint)
    EVT_MENU(IDM_TOOLBAR_TOGGLEHELP, MyFrame::OnToggleHelp)
    EVT_MENU(IDM_TOOLBAR_TOGGLESEARCH, MyFrame::OnToggleSearch)
    EVT_MENU_RANGE(IDM_TOOLBAR_TOGGLERADIOBTN1, IDM_TOOLBAR_TOGGLERADIOBTN3,
                   MyFrame::OnToggleRadioBtn)
    EVT_MENU(IDM_TOOLBAR_CHANGE_TOOLTIP, MyFrame::OnChangeToolTip)

    EVT_MENU_RANGE(IDM_TOOLBAR_SHOW_TEXT, IDM_TOOLBAR_SHOW_BOTH,
                   MyFrame::OnToolbarStyle)
    EVT_MENU(IDM_TOOLBAR_BG_COL, MyFrame::OnToolbarBgCol)

    EVT_MENU(IDM_TOOLBAR_CUSTOM_PATH, MyFrame::OnToolbarCustomBitmap)

    EVT_MENU(wxID_ANY, MyFrame::OnToolLeftClick)

    EVT_COMBOBOX(ID_COMBO, MyFrame::OnCombo)

    EVT_TOOL_RCLICKED(wxID_ANY, MyFrame::OnToolRightClick)

    EVT_TOOL_DROPDOWN(wxID_ANY, MyFrame::OnToolDropdown)

    EVT_UPDATE_UI(wxID_COPY, MyFrame::OnUpdateCopyAndCut)
    EVT_UPDATE_UI(wxID_CUT, MyFrame::OnUpdateCopyAndCut)

    EVT_UPDATE_UI_RANGE(IDM_TOOLBAR_TOGGLERADIOBTN1,
                        IDM_TOOLBAR_TOGGLERADIOBTN3,
                        MyFrame::OnUpdateToggleRadioBtn)
    EVT_UPDATE_UI(IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
                  MyFrame::OnUpdateToggleHorzText)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    MyFrame* frame = new MyFrame((wxFrame *) NULL, wxID_ANY,
                                 wxT("wxToolBar Sample"),
                                  wxPoint(100, 100), wxSize(650, 300));

    frame->Show(true);

#if wxUSE_STATUSBAR
    frame->SetStatusText(wxT("Hello, wxWidgets"));
#endif

    wxInitAllImageHandlers();

    return true;
}

void MyFrame::RecreateToolbar()
{
#ifdef __WXWINCE__
    // On Windows CE, we should not delete the
    // previous toolbar in case it contains the menubar.
    // We'll try to accommodate this usage in due course.
    wxToolBar* toolBar = CreateToolBar();
#else
    // delete and recreate the toolbar
    wxToolBarBase *toolBar = GetToolBar();
    long style = toolBar ? toolBar->GetWindowStyle() : TOOLBAR_STYLE;

    if (toolBar && m_searchTool && m_searchTool->GetToolBar() == NULL)
    {
        // see ~MyFrame()
        toolBar->AddTool(m_searchTool);
    }
    m_searchTool = NULL;

    delete toolBar;

    SetToolBar(NULL);

    style &= ~(wxTB_HORIZONTAL | wxTB_VERTICAL | wxTB_BOTTOM | wxTB_RIGHT | wxTB_HORZ_LAYOUT);
    switch( m_toolbarPosition )
    {
        case TOOLBAR_LEFT:
            style |= wxTB_LEFT;
            break;
        case TOOLBAR_TOP:
            style |= wxTB_TOP;
            break;
        case TOOLBAR_RIGHT:
            style |= wxTB_RIGHT;
            break;
        case TOOLBAR_BOTTOM:
        style |= wxTB_BOTTOM;
        break;
    }

    if ( m_showTooltips )
        style &= ~wxTB_NO_TOOLTIPS;
    else
        style |= wxTB_NO_TOOLTIPS;

    if ( style & wxTB_TEXT && !(style & wxTB_NOICONS) && m_horzText )
        style |= wxTB_HORZ_LAYOUT;

    toolBar = CreateToolBar(style, ID_TOOLBAR);
#endif

    PopulateToolbar(toolBar);
}

void MyFrame::PopulateToolbar(wxToolBarBase* toolBar)
{
    // Set up toolbar
    enum
    {
        Tool_new,
        Tool_open,
        Tool_save,
        Tool_copy,
        Tool_cut,
        Tool_paste,
        Tool_print,
        Tool_help,
        Tool_Max
    };

    wxBitmap toolBarBitmaps[Tool_Max];

#if USE_XPM_BITMAPS
    #define INIT_TOOL_BMP(bmp) \
        toolBarBitmaps[Tool_##bmp] = wxBitmap(bmp##_xpm)
#else // !USE_XPM_BITMAPS
    #define INIT_TOOL_BMP(bmp) \
        toolBarBitmaps[Tool_##bmp] = wxBITMAP(bmp)
#endif // USE_XPM_BITMAPS/!USE_XPM_BITMAPS

    INIT_TOOL_BMP(new);
    INIT_TOOL_BMP(open);
    INIT_TOOL_BMP(save);
    INIT_TOOL_BMP(copy);
    INIT_TOOL_BMP(cut);
    INIT_TOOL_BMP(paste);
    INIT_TOOL_BMP(print);
    INIT_TOOL_BMP(help);

    int w = toolBarBitmaps[Tool_new].GetWidth(),
        h = toolBarBitmaps[Tool_new].GetHeight();

    if ( !m_smallToolbar )
    {
        w *= 2;
        h *= 2;

        for ( size_t n = Tool_new; n < WXSIZEOF(toolBarBitmaps); n++ )
        {
            toolBarBitmaps[n] =
                wxBitmap(toolBarBitmaps[n].ConvertToImage().Scale(w, h));
        }
    }

    // this call is actually unnecessary as the toolbar will adjust its tools
    // size to fit the biggest icon used anyhow but it doesn't hurt neither
    toolBar->SetToolBitmapSize(wxSize(w, h));

    toolBar->AddTool(wxID_NEW, wxT("New"),
                     toolBarBitmaps[Tool_new], wxNullBitmap, wxITEM_DROPDOWN,
                     wxT("New file"), wxT("This is help for new file tool"));

    wxMenu* menu = new wxMenu;
    menu->Append(wxID_ANY, wxT("&First dummy item"));
    menu->Append(wxID_ANY, wxT("&Second dummy item"));
    menu->AppendSeparator();
    menu->Append(wxID_EXIT, wxT("Exit"));
    toolBar->SetDropdownMenu(wxID_NEW, menu);

    toolBar->AddTool(wxID_OPEN, wxT("Open"),
                     toolBarBitmaps[Tool_open], wxNullBitmap, wxITEM_NORMAL,
                     wxT("Open file"), wxT("This is help for open file tool"));

#if USE_CONTROLS_IN_TOOLBAR
    // adding a combo to a vertical toolbar is not very smart
    if ( !toolBar->IsVertical() )
    {
        wxComboBox *combo = new wxComboBox(toolBar, ID_COMBO, wxEmptyString, wxDefaultPosition, wxSize(100,-1) );
        combo->Append(wxT("This"));
        combo->Append(wxT("is a"));
        combo->Append(wxT("combobox"));
        combo->Append(wxT("in a"));
        combo->Append(wxT("toolbar"));
        toolBar->AddControl(combo, wxT("Combo Label"));
    }
#endif // USE_CONTROLS_IN_TOOLBAR

    toolBar->AddTool(wxID_SAVE, wxT("Save"), toolBarBitmaps[Tool_save], wxT("Toggle button 1"), wxITEM_CHECK);

    toolBar->AddSeparator();
    toolBar->AddTool(wxID_COPY, wxT("Copy"), toolBarBitmaps[Tool_copy], wxT("Toggle button 2"), wxITEM_CHECK);
    toolBar->AddTool(wxID_CUT, wxT("Cut"), toolBarBitmaps[Tool_cut], wxT("Toggle/Untoggle help button"));
    toolBar->AddTool(wxID_PASTE, wxT("Paste"), toolBarBitmaps[Tool_paste], wxT("Paste"));
    toolBar->AddSeparator();

    if ( m_useCustomDisabled )
    {
        wxBitmap bmpDisabled(w, h);
        {
            wxMemoryDC dc;
            dc.SelectObject(bmpDisabled);
            dc.DrawBitmap(toolBarBitmaps[Tool_print], 0, 0);

            wxPen pen(*wxRED, 5);
            dc.SetPen(pen);
            dc.DrawLine(0, 0, w, h);
        }

        toolBar->AddTool(wxID_PRINT, wxT("Print"), toolBarBitmaps[Tool_print],
                         bmpDisabled);
    }
    else
    {
        toolBar->AddTool(wxID_PRINT, wxT("Print"), toolBarBitmaps[Tool_print],
                         wxT("Delete this tool. This is a very long tooltip to test whether it does the right thing when the tooltip is more than Windows can cope with."));
    }

    // add a stretchable space before the "Help" button to make it
    // right-aligned
    toolBar->AddStretchableSpace();
    toolBar->AddTool(wxID_HELP, wxT("Help"), toolBarBitmaps[Tool_help], wxT("Help button"), wxITEM_CHECK);

    if ( !m_pathBmp.empty() )
    {
        // create a tool with a custom bitmap for testing
        wxImage img(m_pathBmp);
        if ( img.IsOk() )
        {
            if ( img.GetWidth() > w && img.GetHeight() > h )
                img = img.GetSubImage(wxRect(0, 0, w, h));

            toolBar->AddSeparator();
            toolBar->AddTool(wxID_ANY, wxT("Custom"), img);
        }
    }

    // after adding the buttons to the toolbar, must call Realize() to reflect
    // the changes
    toolBar->Realize();

    toolBar->SetRows(!(toolBar->IsVertical()) ? m_rows : 10 / m_rows);
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxFrame* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
       : wxFrame(parent, id, title, pos, size, style)
{
    m_tbar = NULL;

    m_smallToolbar = true;
    m_horzText = false;
    m_useCustomDisabled = false;
    m_showTooltips = true;
    m_searchTool = NULL;

    m_rows = 1;
    m_nPrint = 1;

#if wxUSE_STATUSBAR
    // Give it a status line
    CreateStatusBar();
#endif

    // Give it an icon
    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *tbarMenu = new wxMenu;
    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_TOOLBAR,
                              wxT("Toggle &toolbar\tCtrl-Z"),
                              wxT("Show or hide the toolbar"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
                              wxT("Toggle &another toolbar\tCtrl-A"),
                              wxT("Show/hide another test toolbar"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
                              wxT("Toggle hori&zontal text\tCtrl-H"),
                              wxT("Show text under/alongside the icon"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLBARSIZE,
                              wxT("&Toggle toolbar size\tCtrl-S"),
                              wxT("Toggle between big/small toolbar"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLBARROWS,
                              wxT("Toggle number of &rows\tCtrl-R"),
                              wxT("Toggle number of toolbar rows between 1 and 2"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLTIPS,
                              wxT("Show &tooltips\tCtrl-L"),
                              wxT("Show tooltips for the toolbar tools"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLECUSTOMDISABLED,
                              wxT("Use c&ustom disabled images\tCtrl-U"),
                              wxT("Switch between using system-generated and custom disabled images"));


    tbarMenu->AppendSeparator();
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_TOP_ORIENTATION,
                              wxT("Set toolbar at the top of the window"),
                              wxT("Set toolbar at the top of the window"));
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_LEFT_ORIENTATION,
                              wxT("Set toolbar at the left of the window"),
                              wxT("Set toolbar at the left of the window"));
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_BOTTOM_ORIENTATION,
                              wxT("Set toolbar at the bottom of the window"),
                              wxT("Set toolbar at the bottom of the window"));
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_RIGHT_ORIENTATION,
                              wxT("Set toolbar at the right edge of the window"),
                              wxT("Set toolbar at the right edge of the window"));
    tbarMenu->AppendSeparator();

    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_TEXT, wxT("Show &text\tCtrl-Alt-T"));
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_ICONS, wxT("Show &icons\tCtrl-Alt-I"));
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_BOTH, wxT("Show &both\tCtrl-Alt-B"));
    tbarMenu->AppendSeparator();
    tbarMenu->Append(IDM_TOOLBAR_BG_COL, wxT("Choose bac&kground colour..."));
    tbarMenu->Append(IDM_TOOLBAR_CUSTOM_PATH, wxT("Custom &bitmap...\tCtrl-B"));

    wxMenu *toolMenu = new wxMenu;
    toolMenu->Append(IDM_TOOLBAR_ENABLEPRINT, wxT("&Enable print button\tCtrl-E"));
    toolMenu->Append(IDM_TOOLBAR_DELETEPRINT, wxT("&Delete print button\tCtrl-D"));
    toolMenu->Append(IDM_TOOLBAR_INSERTPRINT, wxT("&Insert print button\tCtrl-I"));
    toolMenu->Append(IDM_TOOLBAR_TOGGLEHELP, wxT("Toggle &help button\tCtrl-T"));
    toolMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLESEARCH, wxT("Toggle &search field\tCtrl-F"));
    toolMenu->AppendSeparator();
    toolMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN1, wxT("Toggle &1st radio button\tCtrl-1"));
    toolMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN2, wxT("Toggle &2nd radio button\tCtrl-2"));
    toolMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN3, wxT("Toggle &3rd radio button\tCtrl-3"));
    toolMenu->AppendSeparator();
    toolMenu->Append(IDM_TOOLBAR_CHANGE_TOOLTIP, wxT("Change tooltip of \"New\""));

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Quit toolbar sample") );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_HELP, wxT("&About"), wxT("About toolbar sample"));

    wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(tbarMenu, wxT("&Toolbar"));
    menuBar->Append(toolMenu, wxT("Tool&s"));
    menuBar->Append(helpMenu, wxT("&Help"));

    // Associate the menu bar with the frame
    SetMenuBar(menuBar);

    menuBar->Check(IDM_TOOLBAR_TOGGLE_TOOLBAR, true);
    menuBar->Check(IDM_TOOLBAR_SHOW_BOTH, true);
    menuBar->Check(IDM_TOOLBAR_TOGGLETOOLTIPS, true);

    menuBar->Check(IDM_TOOLBAR_TOP_ORIENTATION, true );
    m_toolbarPosition = TOOLBAR_TOP;

    // Create the toolbar
    RecreateToolbar();

    m_panel = new wxPanel(this, wxID_ANY);
#if USE_UNMANAGED_TOOLBAR
    m_extraToolBar = new wxToolBar(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_TEXT|wxTB_FLAT|wxTB_TOP);
    PopulateToolbar(m_extraToolBar);
#endif

    m_textWindow = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    m_panel->SetSizer(sizer);
#if USE_UNMANAGED_TOOLBAR
    if (m_extraToolBar)
        sizer->Add(m_extraToolBar, 0, wxEXPAND, 0);
#endif
    sizer->Add(m_textWindow, 1, wxEXPAND, 0);
}

MyFrame::~MyFrame()
{
    if ( m_searchTool && !m_searchTool->GetToolBar() )
    {
        // we currently can't delete a toolbar tool ourselves, so we have to
        // attach it to the toolbar just for it to be deleted, this is pretty
        // ugly and will need to be changed
        GetToolBar()->AddTool(m_searchTool);
    }
}

void MyFrame::LayoutChildren()
{
    wxSize size = GetClientSize();

    int offset;
    if ( m_tbar )
    {
        m_tbar->SetSize(0, 0, wxDefaultCoord, size.y);

        offset = m_tbar->GetSize().x;
    }
    else
    {
        offset = 0;
    }

    m_panel->SetSize(offset, 0, size.x - offset, size.y);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    if ( m_tbar )
    {
        LayoutChildren();
    }
    else
    {
        event.Skip();
    }
}

void MyFrame::OnToggleToolbar(wxCommandEvent& WXUNUSED(event))
{
    wxToolBar *tbar = GetToolBar();

    if ( !tbar )
    {
        RecreateToolbar();
    }
    else
    {
        // notice that there is no need to call SetToolBar(NULL) here (although
        // this it is harmless to do and it must be called if you do not delete
        // the toolbar but keep it for later reuse), just delete the toolbar
        // directly and it will reset the associated frame toolbar pointer
        delete tbar;
    }
}

void MyFrame::OnToggleHorizontalText(wxCommandEvent& WXUNUSED(event))
{
    m_horzText = !m_horzText;

    RecreateToolbar();
}

void MyFrame::OnToggleAnotherToolbar(wxCommandEvent& WXUNUSED(event))
{
    if ( m_tbar )
    {
        wxDELETE(m_tbar);
    }
    else
    {
        long style = GetToolBar() ? GetToolBar()->GetWindowStyle()
                                  : TOOLBAR_STYLE;
        style &= ~wxTB_HORIZONTAL;
        style |= wxTB_VERTICAL;

        m_tbar = new wxToolBar(this, wxID_ANY,
                               wxDefaultPosition, wxDefaultSize,
                               style);

        m_tbar->SetMargins(4, 4);

        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_1, wxT("First"), wxBITMAP(new));
        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_2, wxT("Second"), wxBITMAP(open));
        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_3, wxT("Third"), wxBITMAP(save));
        m_tbar->AddSeparator();
        m_tbar->AddTool(wxID_HELP, wxT("Help"), wxBITMAP(help));

        m_tbar->Realize();
    }

    LayoutChildren();
}

void MyFrame::OnToggleToolbarSize(wxCommandEvent& WXUNUSED(event))
{
    m_smallToolbar = !m_smallToolbar;

    RecreateToolbar();
}

void MyFrame::OnToggleToolbarRows(wxCommandEvent& WXUNUSED(event))
{
    // m_rows may be only 1 or 2
    m_rows = 3 - m_rows;

    GetToolBar()->SetRows(!(GetToolBar()->IsVertical()) ? m_rows : 10 / m_rows);

    //RecreateToolbar(); -- this is unneeded
}

void MyFrame::OnToggleTooltips(wxCommandEvent& WXUNUSED(event))
{
    m_showTooltips = !m_showTooltips;

    RecreateToolbar();
}

void MyFrame::OnToggleCustomDisabled(wxCommandEvent& WXUNUSED(event))
{
    m_useCustomDisabled = !m_useCustomDisabled;

    RecreateToolbar();
}

void MyFrame::OnChangeOrientation(wxCommandEvent& event)
{
    switch( event.GetId() )
    {
        case IDM_TOOLBAR_LEFT_ORIENTATION:
            m_toolbarPosition = TOOLBAR_LEFT;
            break;
        case IDM_TOOLBAR_TOP_ORIENTATION:
            m_toolbarPosition = TOOLBAR_TOP;
            break;
        case IDM_TOOLBAR_RIGHT_ORIENTATION:
            m_toolbarPosition = TOOLBAR_RIGHT;
            break;
        case IDM_TOOLBAR_BOTTOM_ORIENTATION:
            m_toolbarPosition = TOOLBAR_BOTTOM;
            break;
    }
    RecreateToolbar();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    if ( event.IsChecked() )
        m_textWindow->WriteText( wxT("Help button down now.\n") );
    else
        m_textWindow->WriteText( wxT("Help button up now.\n") );

    (void)wxMessageBox(wxT("wxWidgets toolbar sample"), wxT("About wxToolBar"));
}

void MyFrame::OnToolLeftClick(wxCommandEvent& event)
{
    wxString str;
    str.Printf( wxT("Clicked on tool %d\n"), event.GetId());
    m_textWindow->WriteText( str );

    if (event.GetId() == wxID_COPY)
    {
        DoEnablePrint();
    }

    if (event.GetId() == wxID_CUT)
    {
        DoToggleHelp();
    }

    if (event.GetId() == wxID_PRINT)
    {
        DoDeletePrint();
    }
}

void MyFrame::OnToolRightClick(wxCommandEvent& event)
{
    m_textWindow->AppendText(
            wxString::Format(wxT("Tool %d right clicked.\n"),
                             (int) event.GetInt()));
}

void MyFrame::OnCombo(wxCommandEvent& event)
{
    wxLogStatus(wxT("Combobox string '%s' selected"), event.GetString().c_str());
}

void MyFrame::DoEnablePrint()
{
    if ( !m_nPrint )
        return;

    wxToolBarBase *tb = GetToolBar();
    tb->EnableTool(wxID_PRINT, !tb->GetToolEnabled(wxID_PRINT));
}

void MyFrame::DoDeletePrint()
{
    if ( !m_nPrint )
        return;

    wxToolBarBase *tb = GetToolBar();
    tb->DeleteTool( wxID_PRINT );

    m_nPrint--;
}

void MyFrame::DoToggleHelp()
{
    wxToolBarBase *tb = GetToolBar();
    tb->ToggleTool( wxID_HELP, !tb->GetToolState( wxID_HELP ) );
}

void MyFrame::OnToggleSearch(wxCommandEvent& WXUNUSED(event))
{
    wxToolBarBase * const tb = GetToolBar();
    if ( !m_searchTool )
    {
        wxSearchCtrl * const srch = new wxSearchCtrl(tb, wxID_ANY, "needle");
        srch->SetMinSize(wxSize(80, -1));
        m_searchTool = tb->AddControl(srch);
    }
    else // tool already exists
    {
        wxControl * const win = m_searchTool->GetControl();
        if ( m_searchTool->GetToolBar() )
        {
            // attached now, remove it
            win->Hide();
            tb->RemoveTool(m_searchTool->GetId());
        }
        else // tool exists in detached state, attach it back
        {
            tb->AddTool(m_searchTool);
            win->Show();
        }
    }

    tb->Realize();
}

void MyFrame::OnUpdateCopyAndCut(wxUpdateUIEvent& event)
{
    event.Enable( m_textWindow->CanCopy() );
}

void MyFrame::OnUpdateToggleHorzText(wxUpdateUIEvent& event)
{
    wxToolBar *tbar = GetToolBar();
    event.Enable( tbar &&
                    tbar->HasFlag(wxTB_TEXT) &&
                        !tbar->HasFlag(wxTB_NOICONS) );
}

void MyFrame::OnChangeToolTip(wxCommandEvent& WXUNUSED(event))
{
    GetToolBar()->SetToolShortHelp(wxID_NEW, wxT("New toolbar button"));
}

void MyFrame::OnToolbarStyle(wxCommandEvent& event)
{
    long style = GetToolBar()->GetWindowStyle();
    style &= ~(wxTB_NOICONS | wxTB_TEXT);

    switch ( event.GetId() )
    {
        case IDM_TOOLBAR_SHOW_TEXT:
            style |= wxTB_NOICONS | wxTB_TEXT;
            break;

        case IDM_TOOLBAR_SHOW_ICONS:
            // nothing to do
            break;

        case IDM_TOOLBAR_SHOW_BOTH:
            style |= wxTB_TEXT;
    }

    GetToolBar()->SetWindowStyle(style);
}

void MyFrame::OnToolbarBgCol(wxCommandEvent& WXUNUSED(event))
{
    wxColour col = wxGetColourFromUser
                   (
                    this,
                    GetToolBar()->GetBackgroundColour(),
                    "Toolbar background colour"
                   );
    if ( col.IsOk() )
    {
        GetToolBar()->SetBackgroundColour(col);
        GetToolBar()->Refresh();
    }
}

void MyFrame::OnToolbarCustomBitmap(wxCommandEvent& WXUNUSED(event))
{
    m_pathBmp = wxLoadFileSelector("custom bitmap", "");

    RecreateToolbar();
}

void MyFrame::OnInsertPrint(wxCommandEvent& WXUNUSED(event))
{
    m_nPrint++;

    wxToolBarBase *tb = GetToolBar();
    tb->InsertTool(0, wxID_PRINT, wxT("New print"),
                   wxBITMAP(print), wxNullBitmap,
                   wxITEM_NORMAL,
                   wxT("Delete this tool"),
                   wxT("This button was inserted into the toolbar"));

    // must call Realize() after adding a new button
    tb->Realize();
}

void MyFrame::OnToggleRadioBtn(wxCommandEvent& event)
{
    if ( m_tbar )
    {
        m_tbar->ToggleTool(IDM_TOOLBAR_OTHER_1 +
                            event.GetId() - IDM_TOOLBAR_TOGGLERADIOBTN1, true);
    }
}

void MyFrame::OnToolDropdown(wxCommandEvent& event)
{
    wxString str;
    str.Printf( wxT("Dropdown on tool %d\n"), event.GetId());
    m_textWindow->WriteText( str );

    event.Skip();
}
