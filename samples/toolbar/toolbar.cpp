/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar sample
// Author:      Julian Smart
// Created:     04/01/98
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


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/toolbar.h"
#include "wx/bmpbndl.h"
#include "wx/log.h"
#include "wx/image.h"
#include "wx/filedlg.h"
#include "wx/colordlg.h"
#include "wx/srchctrl.h"

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

// We don't embed this item in the resources under Mac, so we need to still use
// the XPM there.
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif // !wxHAS_IMAGES_IN_RESOURCES

// If PNG files are not available in resources, we need to embed them in the
// program itself. We could also load them during run-time.
#ifndef wxHAS_IMAGE_RESOURCES
    #include "bitmaps/new_png.c"
    #include "bitmaps/open_png.c"
    #include "bitmaps/save_png.c"
    #include "bitmaps/copy_png.c"
    #include "bitmaps/cut_png.c"
    #include "bitmaps/paste_png.c"
    #include "bitmaps/print_png.c"
    #include "bitmaps/help_png.c"

    #include "bitmaps/new_2x_png.c"
    #include "bitmaps/open_2x_png.c"
    #include "bitmaps/save_2x_png.c"
    #include "bitmaps/copy_2x_png.c"
    #include "bitmaps/cut_2x_png.c"
    #include "bitmaps/paste_2x_png.c"
    #include "bitmaps/print_2x_png.c"
    #include "bitmaps/help_2x_png.c"
#endif // !wxHAS_IMAGE_RESOURCES

// Real SVGs would typically be loaded from files, but to keep things as simple
// as possible here, we embed this one directly in the program text.
static const char svg_data[] =
"<svg version=\"1.1\" viewBox=\"0.0 0.0 360.0 360.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\">"
"<g>"
"<path stroke=\"#000000\" fill=\"#ff0000\" d=\"m 10 170 c0 -12 10 -24 24 -24 l100 0 c6 0 12 2 17 7 c4 4 7 10 7 17 l0 100 c0 12 -10 24 -24 24 l-100 0c-12 0 -24 -10 -24 -24 z\"/>"
"<path stroke=\"#000000\" fill=\"#0000ff\" d=\"m100  90 c0 -12 10 -24 24 -24 l100 0 c6 0 12 2 17 7 c4 4 7 10 7 17 l0 100 c0 12 -10 24 -24 24 l-100 0c-12 0 -24 -10 -24 -24 z\"/>"
"<path stroke=\"#000000\" fill=\"#ffff00\" d=\"m210 140 c0 -12 10 -24 24 -24 l100 0 c6 0 12 2 17 7 c4 4 7 10 7 17 l0 100 c0 12 -10 24 -24 24 l-100 0c-12 0 -24 -10 -24 -24 z\"/>"
"</g>"
"</svg>"
;

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
    bool OnInit() override;
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame();
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
    void OnIncToolSpacing(wxCommandEvent& event);
    void OnDecToolSpacing(wxCommandEvent& event);
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
    void OnUpdateNeedsToolbar(wxUpdateUIEvent& event)
        { event.Enable( GetToolBar() != nullptr ); }
    void OnUpdateToggleRadioBtn(wxUpdateUIEvent& event)
        { event.Enable( m_tbar != nullptr ); }

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

    // the search tool, initially nullptr
    wxToolBarToolBase *m_searchTool;

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const int ID_TOOLBAR = wxID_HIGHEST;

static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;

enum
{
    // toolbar menu items
    IDM_TOOLBAR_TOGGLE_TOOLBAR = 200,
    IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
    IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
    IDM_TOOLBAR_TOGGLETOOLBARSIZE,
    IDM_TOOLBAR_TOGGLETOOLTIPS,
    IDM_TOOLBAR_TOGGLECUSTOMDISABLED,
    IDM_TOOLBAR_CUSTOM_PATH,
    IDM_TOOLBAR_TOP_ORIENTATION,
    IDM_TOOLBAR_LEFT_ORIENTATION,
    IDM_TOOLBAR_BOTTOM_ORIENTATION,
    IDM_TOOLBAR_RIGHT_ORIENTATION,
    IDM_TOOLBAR_OTHER_1,
    IDM_TOOLBAR_OTHER_2,
    IDM_TOOLBAR_OTHER_3,
    IDM_TOOLBAR_OTHER_4,

    // items starting from this one must be disabled when there is no toolbar
    IDM_FIRST_NEEDS_TOOLBAR,

    IDM_TOOLBAR_TOGGLETOOLBARROWS,
    IDM_TOOLBAR_SHOW_TEXT,
    IDM_TOOLBAR_SHOW_ICONS,
    IDM_TOOLBAR_SHOW_BOTH,
    IDM_TOOLBAR_BG_COL,

    // tools menu items
    IDM_TOOLBAR_ENABLEPRINT,
    IDM_TOOLBAR_DELETEPRINT,
    IDM_TOOLBAR_INSERTPRINT,
    IDM_TOOLBAR_TOGGLEHELP,
    IDM_TOOLBAR_TOGGLESEARCH,
    IDM_TOOLBAR_CHANGE_TOOLTIP,
    IDM_TOOLBAR_INC_TOOL_SPACING,
    IDM_TOOLBAR_DEC_TOOL_SPACING,

    IDM_LAST_NEEDS_TOOLBAR,

    IDM_TOOLBAR_TOGGLERADIOBTN1,
    IDM_TOOLBAR_TOGGLERADIOBTN2,
    IDM_TOOLBAR_TOGGLERADIOBTN3,

    ID_COMBO = 1000
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

// Notice that wxID_ABOUT will be processed for the 'About' menu and the toolbar
// help button.

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_SIZE(MyFrame::OnSize)

    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)

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
    EVT_MENU(IDM_TOOLBAR_INC_TOOL_SPACING, MyFrame::OnIncToolSpacing)
    EVT_MENU(IDM_TOOLBAR_DEC_TOOL_SPACING, MyFrame::OnDecToolSpacing)

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

    EVT_UPDATE_UI_RANGE(IDM_FIRST_NEEDS_TOOLBAR,
                        IDM_LAST_NEEDS_TOOLBAR,
                        MyFrame::OnUpdateNeedsToolbar)
    EVT_UPDATE_UI_RANGE(IDM_TOOLBAR_TOGGLERADIOBTN1,
                        IDM_TOOLBAR_TOGGLERADIOBTN3,
                        MyFrame::OnUpdateToggleRadioBtn)
    EVT_UPDATE_UI(IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
                  MyFrame::OnUpdateToggleHorzText)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Because we use PNG icons in the frame ctor, we need to register this
    // image handler before creating the frame.
    wxImage::AddHandler(new wxPNGHandler);

    // Create the main frame window
    MyFrame* frame = new MyFrame();

    frame->Show(true);

#if wxUSE_STATUSBAR
    frame->SetStatusText("Hello, wxWidgets");
#endif

    return true;
}

void MyFrame::RecreateToolbar()
{
    // delete and recreate the toolbar
    wxToolBarBase *toolBar = GetToolBar();
    long style = toolBar ? toolBar->GetWindowStyle() : TOOLBAR_STYLE;

    if (toolBar && m_searchTool && m_searchTool->GetToolBar() == nullptr)
    {
        // see ~MyFrame()
        toolBar->AddTool(m_searchTool);
    }
    m_searchTool = nullptr;

    delete toolBar;

    SetToolBar(nullptr);

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
        Tool_about,
        Tool_Max
    };

    wxBitmapBundle toolBarBitmaps[Tool_Max];

    // Note that to use wxBITMAP_BUNDLE_2() macro it is necessary to
    //
    //  1. Have resources (either RT_RCDATA under MSW or files in the app
    //     bundle resources subdirectory under Mac) with this name.
    //
    //  2. Have name_png and name_2x_png arrays defined under the other
    //     platforms (as is done in this sample by including the corresponding
    //     *_png.c files above).
    toolBarBitmaps[Tool_new  ] = wxBITMAP_BUNDLE_2(new  );
    toolBarBitmaps[Tool_open ] = wxBITMAP_BUNDLE_2(open );
    toolBarBitmaps[Tool_save ] = wxBITMAP_BUNDLE_2(save );
    toolBarBitmaps[Tool_copy ] = wxBITMAP_BUNDLE_2(copy );
    toolBarBitmaps[Tool_cut  ] = wxBITMAP_BUNDLE_2(cut  );
    toolBarBitmaps[Tool_paste] = wxBITMAP_BUNDLE_2(paste);
    toolBarBitmaps[Tool_print] = wxBITMAP_BUNDLE_2(print);
    toolBarBitmaps[Tool_help ] = wxBITMAP_BUNDLE_2(help );

    // Size of the bitmaps we use by default.
    //
    // Note that scaling it up by 2 is not something we would do in real
    // application code, it is only done in the sample for testing of bigger
    // bitmap sizes.
    const wxSize sizeBitmap = toolBarBitmaps[Tool_new].GetDefaultSize() *
                                (m_smallToolbar ? 1 : 2);

#ifdef wxHAS_SVG
    // Use vector SVG image for this button for demonstration purposes.
    toolBarBitmaps[Tool_about] = wxBitmapBundle::FromSVG(svg_data, sizeBitmap);
#endif // wxHAS_SVG

    // We don't have to call this function at all when using the default bitmap
    // size (i.e. when m_smallToolbar == true), but it's harmless to do it.
    //
    // Note that sizeBitmap is in DIPs, so we need to use FromDIP() to scale it
    // up for the current DPI, if necessary.
    toolBar->SetToolBitmapSize(FromDIP(sizeBitmap));

    toolBar->AddTool(wxID_NEW, "New",
                     toolBarBitmaps[Tool_new], wxNullBitmap, wxITEM_DROPDOWN,
                     "New file", "This is help for new file tool");

    wxMenu* menu = new wxMenu;
    menu->Append(wxID_ANY, "&First dummy item");
    menu->Append(wxID_ANY, "&Second dummy item");
    menu->AppendSeparator();
    menu->Append(wxID_EXIT, "Exit");
    toolBar->SetDropdownMenu(wxID_NEW, menu);

    toolBar->AddTool(wxID_OPEN, "Open",
                     toolBarBitmaps[Tool_open], wxNullBitmap, wxITEM_NORMAL,
                     "Open file", "This is help for open file tool");

#if USE_CONTROLS_IN_TOOLBAR
    // adding a combo to a vertical toolbar is not very smart
    if ( !toolBar->IsVertical() )
    {
        wxComboBox *combo = new wxComboBox(toolBar, ID_COMBO, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(100,-1)) );
        combo->Append("This");
        combo->Append("is a");
        combo->Append("combobox with extremely, extremely, extremely, extremely long label");
        combo->Append("in a");
        combo->Append("toolbar");
        toolBar->AddControl(combo, "Combo Label");
    }
#endif // USE_CONTROLS_IN_TOOLBAR

    toolBar->AddTool(wxID_SAVE, "Save", toolBarBitmaps[Tool_save], "Toggle button 1", wxITEM_CHECK);

    toolBar->AddSeparator();
    toolBar->AddTool(wxID_COPY, "Copy", toolBarBitmaps[Tool_copy], "Toggle button 2", wxITEM_CHECK);
    toolBar->AddTool(wxID_CUT, "Cut", toolBarBitmaps[Tool_cut], "Toggle/Untoggle help button");
    toolBar->AddTool(wxID_PASTE, "Paste", toolBarBitmaps[Tool_paste], "Paste");
    toolBar->AddSeparator();

    if ( m_useCustomDisabled )
    {
        wxBitmap bmpDisabled(sizeBitmap);
        {
            wxMemoryDC dc;
            dc.SelectObject(bmpDisabled);
            dc.DrawBitmap(toolBarBitmaps[Tool_print].GetBitmap(sizeBitmap), 0, 0);

            wxPen pen(*wxRED, 5);
            dc.SetPen(pen);
            dc.DrawLine(0, 0, sizeBitmap.x, sizeBitmap.y);
        }

        toolBar->AddTool(wxID_PRINT, "Print", toolBarBitmaps[Tool_print],
                         bmpDisabled);
    }
    else
    {
        toolBar->AddTool(wxID_PRINT, "Print", toolBarBitmaps[Tool_print],
                         "Delete this tool. This is a very long tooltip to test whether it does the right thing when the tooltip is more than Windows can cope with.");
    }

    m_nPrint = 1;

    // add a stretchable space before the "Help" button to make it
    // right-aligned
    toolBar->AddStretchableSpace();
    toolBar->AddTool(wxID_HELP, "Help", toolBarBitmaps[Tool_help], "Help button", wxITEM_CHECK);

    toolBar->AddTool(wxID_ABOUT, "About", toolBarBitmaps[Tool_about], "About");

    if ( !m_pathBmp.empty() )
    {
        wxImage image(m_pathBmp);
        if ( image.IsOk() )
        {
            // create a custom bitmap bundle for testing
            class MyCustomBitmapBundleImpl : public wxBitmapBundleImpl
            {
            public:
                MyCustomBitmapBundleImpl(const wxImage& image,
                                         const wxSize& sizeDef)
                    : m_image(image),
                      m_sizeDef(sizeDef)
                {
                }

                wxSize GetDefaultSize() const override
                {
                    return m_sizeDef;
                }

                wxSize GetPreferredBitmapSizeAtScale(double scale) const override
                {
                    // We just scale the bitmap to fit the requested size, so
                    // we don't really have any preferences.
                    return m_sizeDef*scale;
                }

                wxBitmap GetBitmap(const wxSize& size) override
                {
                    // In this simple implementation we don't bother caching
                    // anything.
                    wxImage image = m_image;
                    if ( image.GetSize() != size )
                        image.Rescale(size.x, size.y, wxIMAGE_QUALITY_HIGH);

                    // This is required under MSW in order to be able to draw
                    // over the bitmap using wxDC. For full alpha support,
                    // wxGraphicsContext should be used.
                    if ( image.HasAlpha() )
                        image.ClearAlpha();

                    wxBitmap bitmap(image);

                    // This is the custom part: we show the size of the bitmap
                    // being used in the bitmap itself.
                    wxMemoryDC dc(bitmap);
                    dc.SetTextForeground(*wxRED);
                    dc.SetFont(wxFontInfo(wxSize(size.x/4, size.y/2)).Bold());
                    dc.DrawText(wxString::Format("%d", size.y), size.x/4, size.y/4);

                    return bitmap;
                }

            private:
                const wxImage m_image;
                const wxSize m_sizeDef;
            };

            wxBitmapBundleImpl* const
                impl = new MyCustomBitmapBundleImpl(image, sizeBitmap);

            toolBar->AddSeparator();
            toolBar->AddTool(wxID_ANY, "Custom", wxBitmapBundle::FromImpl(impl));
        }
    }

    // after adding the buttons to the toolbar, must call Realize() to reflect
    // the changes
    toolBar->Realize();

    toolBar->SetRows(toolBar->IsVertical() ? toolBar->GetToolsCount() / m_rows
                                           : m_rows);
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame()
       : wxFrame(nullptr, wxID_ANY, "wxToolBar Sample")
{
    m_tbar = nullptr;

    m_smallToolbar = true;
    m_horzText = false;
    m_useCustomDisabled = false;
    m_showTooltips = true;
    m_searchTool = nullptr;

    m_rows = 1;
    m_nPrint = 0; // set to 1 in PopulateToolbar()

#if wxUSE_STATUSBAR
    // Give it a status line
    CreateStatusBar();
#endif

    // Give it an icon
    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *tbarMenu = new wxMenu;
    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_TOOLBAR,
                              "Toggle &toolbar\tCtrl-Z",
                              "Show or hide the toolbar");

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
                              "Toggle &another toolbar\tCtrl-A",
                              "Show/hide another test toolbar");

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
                              "Toggle hori&zontal text\tCtrl-H",
                              "Show text under/alongside the icon");

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLBARSIZE,
                              "&Toggle toolbar size\tCtrl-S",
                              "Toggle between big/small toolbar");

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLBARROWS,
                              "Toggle number of &rows\tCtrl-R",
                              "Toggle number of toolbar rows between 1 and 2");

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLTIPS,
                              "Show &tooltips\tCtrl-L",
                              "Show tooltips for the toolbar tools");

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLECUSTOMDISABLED,
                              "Use c&ustom disabled images\tCtrl-U",
                              "Switch between using system-generated and custom disabled images");


    tbarMenu->AppendSeparator();
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_TOP_ORIENTATION,
                              "Set toolbar at the top of the window\tCtrl-Up",
                              "Set toolbar at the top of the window");
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_LEFT_ORIENTATION,
                              "Set toolbar at the left of the window\tCtrl-Left",
                              "Set toolbar at the left of the window");
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_BOTTOM_ORIENTATION,
                              "Set toolbar at the bottom of the window\tCtrl-Down",
                              "Set toolbar at the bottom of the window");
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_RIGHT_ORIENTATION,
                              "Set toolbar at the right edge of the window\tCtrl-Right",
                              "Set toolbar at the right edge of the window");
    tbarMenu->AppendSeparator();

    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_TEXT, "Show &text\tCtrl-Alt-T");
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_ICONS, "Show &icons\tCtrl-Alt-I");
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_BOTH, "Show &both\tCtrl-Alt-B");
    tbarMenu->AppendSeparator();
    tbarMenu->Append(IDM_TOOLBAR_BG_COL, "Choose bac&kground colour...");
    tbarMenu->Append(IDM_TOOLBAR_CUSTOM_PATH, "Custom &bitmap...\tCtrl-B");

    wxMenu *toolMenu = new wxMenu;
    toolMenu->Append(IDM_TOOLBAR_ENABLEPRINT, "&Enable print button\tCtrl-E");
    toolMenu->Append(IDM_TOOLBAR_DELETEPRINT, "&Delete print button\tCtrl-D");
    toolMenu->Append(IDM_TOOLBAR_INSERTPRINT, "&Insert print button\tCtrl-I");
    toolMenu->Append(IDM_TOOLBAR_TOGGLEHELP, "Toggle &help button\tCtrl-T");
    toolMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLESEARCH, "Toggle &search field\tCtrl-F");
    toolMenu->AppendSeparator();
    toolMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN1, "Toggle &1st radio button\tCtrl-1");
    toolMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN2, "Toggle &2nd radio button\tCtrl-2");
    toolMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN3, "Toggle &3rd radio button\tCtrl-3");
    toolMenu->AppendSeparator();
    toolMenu->Append(IDM_TOOLBAR_CHANGE_TOOLTIP, "Change tooltip of \"New\"");
    toolMenu->AppendSeparator();
    toolMenu->Append(IDM_TOOLBAR_INC_TOOL_SPACING, "Increase spacing\tCtrl-+");
    toolMenu->Append(IDM_TOOLBAR_DEC_TOOL_SPACING, "Decrease spacing\tCtrl--");

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit toolbar sample" );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About", "About toolbar sample");

    wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

    menuBar->Append(fileMenu, "&File");
    menuBar->Append(tbarMenu, "&Toolbar");
    menuBar->Append(toolMenu, "Tool&s");
    menuBar->Append(helpMenu, "&Help");

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

    // Use a read-only text control; Cut tool will not cut selected text anyway.
    m_textWindow = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    m_panel->SetSizer(sizer);
#if USE_UNMANAGED_TOOLBAR
    if (m_extraToolBar)
        sizer->Add(m_extraToolBar, 0, wxEXPAND, 0);
#endif
    sizer->Add(m_textWindow, 1, wxEXPAND, 0);

    SetInitialSize(FromDIP(wxSize(650, 350)));
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
        // notice that there is no need to call SetToolBar(nullptr) here (although
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

        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_1, "First", wxBITMAP_PNG(new));
        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_2, "Second", wxBITMAP_PNG(open));
        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_3, "Third", wxBITMAP_PNG(save));
        m_tbar->AddSeparator();
        m_tbar->AddTool(wxID_HELP, "Help", wxBITMAP_PNG(help));
        m_tbar->AddTool(IDM_TOOLBAR_OTHER_4, "Disabled", wxBITMAP_PNG(cut), wxBITMAP_PNG(paste));
        m_tbar->EnableTool(IDM_TOOLBAR_OTHER_4, false);

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

    wxToolBar* const toolBar = GetToolBar();
    toolBar->SetRows(toolBar->IsVertical() ? toolBar->GetToolsCount() / m_rows
                                           : m_rows);

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
        m_textWindow->AppendText( "Help button down now.\n" );
    else
        m_textWindow->AppendText( "Help button up now.\n" );

    (void)wxMessageBox("wxWidgets toolbar sample", "About wxToolBar");
}

void MyFrame::OnToolLeftClick(wxCommandEvent& event)
{
    wxString str;
    str.Printf( "Clicked on tool %d\n", event.GetId());
    m_textWindow->AppendText( str );

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
            wxString::Format("Tool %d right clicked.\n",
                             (int) event.GetInt()));
}

void MyFrame::OnCombo(wxCommandEvent& event)
{
    wxLogStatus("Combobox string '%s' selected", event.GetString());
}

void MyFrame::DoEnablePrint()
{
    if ( !m_nPrint )
        return;

    wxToolBarBase *tb = GetToolBar();
    tb->EnableTool(wxID_PRINT, !tb->GetToolEnabled(wxID_PRINT));
    m_textWindow->AppendText("Print tool state changed.\n");
}

void MyFrame::DoDeletePrint()
{
    if ( !m_nPrint )
        return;

    wxToolBarBase *tb = GetToolBar();
    tb->DeleteTool( wxID_PRINT );
    m_textWindow->AppendText("Print tool was deleted.\n");

    m_nPrint--;
}

void MyFrame::DoToggleHelp()
{
    wxToolBarBase *tb = GetToolBar();
    tb->ToggleTool( wxID_HELP, !tb->GetToolState( wxID_HELP ) );
    m_textWindow->AppendText("Help tool was toggled.\n");
}

void MyFrame::OnToggleSearch(wxCommandEvent& WXUNUSED(event))
{
    wxToolBarBase * const tb = GetToolBar();
    if ( !m_searchTool )
    {
        wxSearchCtrl * const srch = new wxSearchCtrl(tb, wxID_ANY, "needle");
        srch->SetMinSize(FromDIP(wxSize(80, -1)));
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
    GetToolBar()->SetToolShortHelp(wxID_NEW, "New toolbar button");
}

void MyFrame::OnIncToolSpacing(wxCommandEvent& WXUNUSED(event))
{
    wxToolBar *tb = GetToolBar();
    tb->SetToolPacking(tb->GetToolPacking()+1);
}

void MyFrame::OnDecToolSpacing(wxCommandEvent& WXUNUSED(event))
{
    wxToolBar *tb = GetToolBar();
    tb->SetToolPacking(tb->GetToolPacking()-1);
}

void MyFrame::OnToolbarStyle(wxCommandEvent& event)
{
    long style = GetToolBar()->GetWindowStyle();
    style &= ~(wxTB_NOICONS | wxTB_HORZ_TEXT);

    switch ( event.GetId() )
    {
        case IDM_TOOLBAR_SHOW_TEXT:
            style |= wxTB_NOICONS | (m_horzText ? wxTB_HORZ_TEXT : wxTB_TEXT);
            break;

        case IDM_TOOLBAR_SHOW_ICONS:
            // nothing to do
            break;

        case IDM_TOOLBAR_SHOW_BOTH:
            style |= (m_horzText ? wxTB_HORZ_TEXT : wxTB_TEXT);
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
    tb->InsertTool(0, wxID_PRINT, "New print",
                   wxBITMAP_PNG(print), wxNullBitmap,
                   wxITEM_NORMAL,
                   "Delete this tool",
                   "This button was inserted into the toolbar");

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
    str.Printf( "Dropdown on tool %d\n", event.GetId());
    m_textWindow->AppendText( str );

    event.Skip();
}
