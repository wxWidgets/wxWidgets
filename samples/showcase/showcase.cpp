/////////////////////////////////////////////////////////////////////////////
// Name:        samples/showcase/showcase.cpp
// Purpose:     Broad wxWidgets component showcase and smoke-test sample
// Author:      wxWidgets development team
// Created:     2026-06-03
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/aboutdlg.h"
#include "wx/artprov.h"
#include "wx/bmpbuttn.h"
#include "wx/button.h"
#include "wx/dcmemory.h"
#include "wx/filedlg.h"
#include "wx/fontdlg.h"
#include "wx/gbsizer.h"
#include "wx/imaglist.h"
#include "wx/msgdlg.h"
#include "wx/notebook.h"
#include "wx/progdlg.h"
#include "wx/scrolwin.h"
#include "wx/splitter.h"
#include "wx/statline.h"
#include "wx/timer.h"
#include "wx/treectrl.h"
#include "wx/vector.h"

#if wxUSE_ACTIVITYINDICATOR
    #include "wx/activityindicator.h"
#endif
#if wxUSE_AUI
    #include "wx/aui/auibook.h"
#endif
#if wxUSE_BANNERWINDOW
    #include "wx/bannerwindow.h"
#endif
#if wxUSE_BMPBUTTON
    #include "wx/bmpbuttn.h"
#endif
#if wxUSE_BITMAPCOMBOBOX
    #include "wx/bmpcbox.h"
#endif
#if wxUSE_CALENDARCTRL
    #include "wx/calctrl.h"
#endif
#if wxUSE_CHECKLISTBOX
    #include "wx/checklst.h"
#endif
#if wxUSE_CHOICEBOOK
    #include "wx/choicebk.h"
#endif
#if wxUSE_CHOICEDLG
    #include "wx/choicdlg.h"
#endif
#if wxUSE_COLLPANE
    #include "wx/collpane.h"
#endif
#if wxUSE_COLOURDLG
    #include "wx/colordlg.h"
#endif
#if wxUSE_COLOURPICKERCTRL
    #include "wx/clrpicker.h"
#endif
#if wxUSE_COMBOCTRL
    #include "wx/combo.h"
#endif
#if wxUSE_COMMANDLINKBUTTON
    #include "wx/commandlinkbutton.h"
#endif
#if wxUSE_DATAVIEWCTRL
    #include "wx/dataview.h"
#endif
#if wxUSE_DATEPICKCTRL
    #include "wx/datectrl.h"
#endif
#if wxUSE_DIRDLG
    #include "wx/dirdlg.h"
#endif
#if wxUSE_DIRPICKERCTRL
    #include "wx/filepicker.h"
#endif
#if wxUSE_EDITABLELISTBOX
    #include "wx/editlbox.h"
#endif
#if wxUSE_FILECTRL
    #include "wx/filectrl.h"
#endif
#if wxUSE_FILEDLG || wxUSE_DIRDLG
    #include "wx/dirctrl.h"
#endif
#if wxUSE_FILEPICKERCTRL
    #include "wx/filepicker.h"
#endif
#if wxUSE_FINDREPLDLG
    #include "wx/fdrepdlg.h"
#endif
#if wxUSE_FONTPICKERCTRL
    #include "wx/fontpicker.h"
#endif
#if wxUSE_GRID
    #include "wx/grid.h"
#endif
#if wxUSE_HEADERCTRL
    #include "wx/headerctrl.h"
#endif
#if wxUSE_HTML
    #include "wx/html/htmlwin.h"
#endif
#if wxUSE_HYPERLINKCTRL
    #include "wx/hyperlink.h"
#endif
#if wxUSE_INFOBAR
    #include "wx/infobar.h"
#endif
#if wxUSE_LISTBOOK
    #include "wx/listbook.h"
#endif
#if wxUSE_LISTCTRL
    #include "wx/listctrl.h"
#endif
#if wxUSE_MEDIACTRL
    #include "wx/mediactrl.h"
#endif
#if wxUSE_NOTIFICATION_MESSAGE
    #include "wx/notifmsg.h"
#endif
#if wxUSE_NUMBERDLG
    #include "wx/numdlg.h"
#endif
#if wxUSE_ODCOMBOBOX
    #include "wx/odcombo.h"
#endif
#if wxUSE_PROPGRID
    #include "wx/propgrid/advprops.h"
    #include "wx/propgrid/propgrid.h"
#endif
#if wxUSE_REARRANGECTRL
    #include "wx/rearrangectrl.h"
#endif
#if wxUSE_RIBBON
    #include "wx/ribbon/bar.h"
    #include "wx/ribbon/buttonbar.h"
    #include "wx/ribbon/page.h"
    #include "wx/ribbon/panel.h"
#endif
#if wxUSE_RICHTEXT
    #include "wx/richtext/richtextctrl.h"
#endif
#if wxUSE_RICHTOOLTIP
    #include "wx/richtooltip.h"
#endif
#if wxUSE_SEARCHCTRL
    #include "wx/srchctrl.h"
#endif
#if wxUSE_BOOKCTRL
    #include "wx/simplebook.h"
#endif
#if wxUSE_SPINBTN
    #include "wx/spinbutt.h"
#endif
#if wxUSE_SPINCTRL
    #include "wx/spinctrl.h"
#endif
#if wxUSE_SPLASH
    #include "wx/splash.h"
#endif
#if wxUSE_STC
    #include "wx/stc/stc.h"
#endif
#if wxUSE_TASKBARICON
    #include "wx/taskbar.h"
#endif
#if wxUSE_TEXTDLG
    #include "wx/textdlg.h"
#endif
#if wxUSE_TOGGLEBTN
    #include "wx/tglbtn.h"
#endif
#if wxUSE_TIMEPICKCTRL
    #include "wx/timectrl.h"
#endif
#if wxUSE_TIPWINDOW
    #include "wx/tipwin.h"
#endif
#if wxUSE_TOOLBOOK
    #include "wx/toolbook.h"
#endif
#if wxUSE_TREEBOOK
    #include "wx/treebook.h"
#endif
#if wxUSE_TREELISTCTRL
    #include "wx/treelist.h"
#endif
#if wxUSE_WEBVIEW
    #include "wx/webview.h"
#endif
#if wxUSE_WIZARDDLG
    #include "wx/wizard.h"
#endif

#include <functional>

namespace
{

enum
{
    ID_ShowMessage = wxID_HIGHEST + 1,
    ID_ShowFileDialog,
    ID_ShowColourDialog,
    ID_ShowFontDialog,
    ID_ShowProgressDialog,
    ID_ShowFindDialog,
    ID_ShowWizard,
    ID_ToggleEnabled,
    ID_Timer
};

class PageItemData : public wxTreeItemData
{
public:
    explicit PageItemData(int page) : m_page(page) { }

    int GetPage() const { return m_page; }

private:
    int m_page;
};

wxBitmapBundle GetArt(wxWindow* win, const wxArtID& id, int sizeDIP = 16)
{
    return wxArtProvider::GetBitmapBundle
           (
               id,
               wxART_OTHER,
               win->FromDIP(wxSize(sizeDIP, sizeDIP))
           );
}

wxBitmap MakeSwatchBitmap(wxWindow* win, const wxColour& colour)
{
    const wxSize size = win->FromDIP(wxSize(48, 32));
    wxBitmap bmp(size);
    wxMemoryDC dc(bmp);

    dc.SetBackground(wxBrush(colour));
    dc.Clear();
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(0, 0, size.x, size.y);
    dc.SelectObject(wxNullBitmap);

    return bmp;
}

wxString BoolText(bool value)
{
    return value ? "on" : "off";
}

} // anonymous namespace

class ShowcaseFrame : public wxFrame
{
public:
    ShowcaseFrame();
    ~ShowcaseFrame() override;

private:
    wxScrolledWindow* CreateScrolledPage(const wxString& intro);
    wxStaticBoxSizer* AddGroup(wxWindow* parent, wxSizer* top, const wxString& label);
    wxButton* AddActionButton(wxWindow* parent,
                              wxSizer* sizer,
                              const wxString& label,
                              const std::function<void()>& action);
    wxStaticText* AddWrappedText(wxWindow* parent, wxSizer* sizer, const wxString& label);

    void AddPage(wxWindow* page, const wxString& title);
    void CreateMenus();
    void CreateTools();
    void CreateStatus();
    void BindFrameCommands();

    wxWindow* CreateOverviewPage();
    wxWindow* CreateButtonPage();
    wxWindow* CreateTextInputPage();
    wxWindow* CreateChoicesAndListsPage();
    wxWindow* CreateContainerPage();
    wxWindow* CreatePickerPage();
    wxWindow* CreateDataPage();
    wxWindow* CreateAdvancedPage();
    wxWindow* CreateDialogPage();
    wxWindow* CreateEventsPage();

    void FillNavigation();
    void Log(const wxString& message);
    void WatchCommand(wxWindow* win, const wxString& name);
    void SetChildrenEnabled(wxWindow* parent, bool enabled);

    void OnNavigation(wxTreeEvent& event);
    void OnAnyCommand(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);

    void ShowMessageDialog();
    void ShowFileDialog();
    void ShowColourDialog();
    void ShowFontDialog();
    void ShowProgressDialog();
    void ShowFindDialog();
    void ShowWizardDialog();
    void ShowChoiceDialogs();
    void ShowDirectoryDialog();
    void ShowTextDialogs();
    void ShowRearrangeDialog();
    void ShowNotification();
    void ShowTooltipWindows();
    void ShowSplash();

    wxNotebook* m_book = nullptr;
    wxTreeCtrl* m_nav = nullptr;
    wxTextCtrl* m_log = nullptr;
    wxGauge* m_gauge = nullptr;
    wxFindReplaceDialog* m_findDialog = nullptr;
    wxFindReplaceData m_findData;
#if wxUSE_TIPWINDOW
    wxTipWindow::Ref m_tipWindow;
#endif
    wxTimer m_timer;
    bool m_childrenEnabled = true;
};

ShowcaseFrame::ShowcaseFrame()
    : wxFrame(nullptr,
              wxID_ANY,
              "wxWidgets Component Showcase",
              wxDefaultPosition,
              wxSize(1160, 760)),
      m_timer(this, ID_Timer)
{
    SetSize(FromDIP(wxSize(1160, 760)));

    CreateMenus();
    CreateTools();
    CreateStatus();

    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_3D);

    m_nav = new wxTreeCtrl(splitter, wxID_ANY, wxDefaultPosition,
        FromDIP(wxSize(220, -1)), wxTR_HIDE_ROOT | wxTR_SINGLE | wxTR_HAS_BUTTONS);
    m_book = new wxNotebook(splitter, wxID_ANY);

    AddPage(CreateOverviewPage(), "Overview");
    AddPage(CreateButtonPage(), "Buttons");
    AddPage(CreateTextInputPage(), "Text and input");
    AddPage(CreateChoicesAndListsPage(), "Choices and lists");
    AddPage(CreateContainerPage(), "Containers");
    AddPage(CreatePickerPage(), "Pickers");
    AddPage(CreateDataPage(), "Data views");
    AddPage(CreateAdvancedPage(), "Advanced");
    AddPage(CreateDialogPage(), "Dialogs");
    AddPage(CreateEventsPage(), "Events");

    FillNavigation();
    BindFrameCommands();
    splitter->SplitVertically(m_nav, m_book, FromDIP(230));
    splitter->SetMinimumPaneSize(FromDIP(150));

    m_timer.Start(250);
    Log("Showcase ready.");
}

ShowcaseFrame::~ShowcaseFrame()
{
#if wxUSE_FINDREPLDLG
    if ( m_findDialog )
    {
        m_findDialog->Destroy();
        m_findDialog = nullptr;
    }
#endif
}

wxScrolledWindow* ShowcaseFrame::CreateScrolledPage(const wxString& intro)
{
    wxScrolledWindow* page = new wxScrolledWindow(m_book, wxID_ANY);
    page->SetScrollRate(0, FromDIP(12));

    wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);
    page->SetSizer(top);

    if ( !intro.empty() )
    {
        AddWrappedText(page, top, intro);
    }

    return page;
}

wxStaticBoxSizer* ShowcaseFrame::AddGroup(wxWindow* parent,
                                          wxSizer* top,
                                          const wxString& label)
{
    wxStaticBoxSizer* group = new wxStaticBoxSizer(wxVERTICAL, parent, label);
    top->Add(group, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(10));
    return group;
}

wxButton* ShowcaseFrame::AddActionButton(wxWindow* parent,
                                         wxSizer* sizer,
                                         const wxString& label,
                                         const std::function<void()>& action)
{
    wxButton* button = new wxButton(parent, wxID_ANY, label);
    button->SetToolTip("Runs " + label);
    button->Bind(wxEVT_BUTTON, [this, label, action](wxCommandEvent&)
    {
        Log(label);
        action();
    });
    sizer->Add(button, 0, wxALL, FromDIP(4));
    return button;
}

wxStaticText* ShowcaseFrame::AddWrappedText(wxWindow* parent,
                                            wxSizer* sizer,
                                            const wxString& label)
{
    wxStaticText* text = new wxStaticText(parent, wxID_ANY, label);
    text->Wrap(FromDIP(850));
    sizer->Add(text, 0, wxEXPAND | wxALL, FromDIP(10));
    return text;
}

void ShowcaseFrame::AddPage(wxWindow* page, const wxString& title)
{
    m_book->AddPage(page, title);
}

void ShowcaseFrame::CreateMenus()
{
#if wxUSE_MENUS
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_NEW, "&New\tCtrl+N");
    fileMenu->Append(wxID_OPEN, "&Open...\tCtrl+O");
    fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tCtrl+Q");

    wxMenu* viewMenu = new wxMenu;
    viewMenu->AppendCheckItem(ID_ToggleEnabled, "Enable sample controls\tCtrl+E")->Check(true);
    viewMenu->Append(wxID_REFRESH, "Refresh status");

    wxMenu* dialogMenu = new wxMenu;
    dialogMenu->Append(ID_ShowMessage, "Message box...");
    dialogMenu->Append(ID_ShowFileDialog, "File dialog...");
    dialogMenu->Append(ID_ShowColourDialog, "Colour dialog...");
    dialogMenu->Append(ID_ShowFontDialog, "Font dialog...");
    dialogMenu->Append(ID_ShowProgressDialog, "Progress dialog...");
    dialogMenu->Append(ID_ShowFindDialog, "Find/replace dialog...");
    dialogMenu->Append(ID_ShowWizard, "Wizard...");

    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(viewMenu, "&View");
    menuBar->Append(dialogMenu, "&Dialogs");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);
#endif
}

void ShowcaseFrame::CreateTools()
{
#if wxUSE_TOOLBAR
    wxToolBar* toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_TEXT | wxTB_FLAT);
    toolbar->AddTool(wxID_NEW, "New", GetArt(this, wxART_NEW, 16), "New tooltip");
    toolbar->AddTool(wxID_OPEN, "Open", GetArt(this, wxART_FILE_OPEN, 16), "Open tooltip");
    toolbar->AddTool(wxID_SAVE, "Save", GetArt(this, wxART_FILE_SAVE, 16), "Save tooltip");
    toolbar->AddSeparator();
    toolbar->AddTool(ID_ShowMessage, "Message", GetArt(this, wxART_INFORMATION, 16), "Show message dialog");
    toolbar->AddTool(ID_ShowProgressDialog, "Progress", GetArt(this, wxART_EXECUTABLE_FILE, 16), "Show progress dialog");
    toolbar->Realize();
#endif
}

void ShowcaseFrame::CreateStatus()
{
#if wxUSE_STATUSBAR
    CreateStatusBar(3);
    int widths[] = { -1, FromDIP(160), FromDIP(180) };
    SetStatusWidths(WXSIZEOF(widths), widths);
    SetStatusText("Ready", 0);
    SetStatusText("Controls: enabled", 1);
#ifdef __WXWINUI__
    SetStatusText("Toolkit: wxWinUI", 2);
#else
    SetStatusText("Toolkit: native/generic", 2);
#endif
#endif
}

void ShowcaseFrame::BindFrameCommands()
{
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(); }, wxID_EXIT);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Log("New menu command"); }, wxID_NEW);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ShowFileDialog(); }, wxID_OPEN);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Log("Save menu command"); }, wxID_SAVE);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Log("Refresh status"); }, wxID_REFRESH);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ShowMessageDialog(); }, ID_ShowMessage);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ShowFileDialog(); }, ID_ShowFileDialog);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ShowColourDialog(); }, ID_ShowColourDialog);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ShowFontDialog(); }, ID_ShowFontDialog);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ShowProgressDialog(); }, ID_ShowProgressDialog);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ShowFindDialog(); }, ID_ShowFindDialog);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ShowWizardDialog(); }, ID_ShowWizard);
    Bind(wxEVT_MENU, [this](wxCommandEvent& event)
    {
        m_childrenEnabled = event.IsChecked();
        SetChildrenEnabled(m_book, m_childrenEnabled);
#if wxUSE_STATUSBAR
        SetStatusText("Controls: " + wxString(m_childrenEnabled ? "enabled" : "disabled"), 1);
#endif
        Log("Controls enabled: " + BoolText(m_childrenEnabled));
    }, ID_ToggleEnabled);

    Bind(wxEVT_MENU, [this](wxCommandEvent&)
    {
        wxAboutDialogInfo info;
        info.SetName("wxWidgets Component Showcase");
        info.SetDescription("Broad smoke-test application for controls, frame surfaces and dialogs.");
        info.SetCopyright("(c) wxWidgets development team");
        wxAboutBox(info, this);
    }, wxID_ABOUT);

    m_nav->Bind(wxEVT_TREE_SEL_CHANGED, &ShowcaseFrame::OnNavigation, this);
    Bind(wxEVT_TIMER, &ShowcaseFrame::OnTimer, this, ID_Timer);
}

wxWindow* ShowcaseFrame::CreateOverviewPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "This sample intentionally creates many wxWidgets controls in one frame. "
        "It is meant as a broad smoke-test for native, generic and wxWinUI-backed implementations.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

    wxStaticBoxSizer* build = AddGroup(page, top, "Build and toolkit");
    wxFlexGridSizer* grid = new wxFlexGridSizer(0, 2, FromDIP(6), FromDIP(18));
    grid->AddGrowableCol(1, 1);
    build->Add(grid, 0, wxEXPAND | wxALL, FromDIP(8));

    auto addRow = [grid, build](const wxString& name, const wxString& value)
    {
        grid->Add(new wxStaticText(build->GetStaticBox(), wxID_ANY, name), 0, wxALIGN_CENTER_VERTICAL);
        grid->Add(new wxStaticText(build->GetStaticBox(), wxID_ANY, value), 0, wxEXPAND);
    };

    addRow("wxWidgets version", wxVERSION_STRING);
#ifdef __WXWINUI__
    addRow("Port macro", "__WXWINUI__");
#else
    addRow("Port macro", "not __WXWINUI__");
#endif
#if wxUSE_WINUI3
    addRow("wxUSE_WINUI3", "1");
#else
    addRow("wxUSE_WINUI3", "0");
#endif
#if wxUSE_UNICODE
    addRow("Unicode", "1");
#else
    addRow("Unicode", "0");
#endif

    wxStaticBoxSizer* surfaces = AddGroup(page, top, "Frame surfaces present in this sample");
    wxGridSizer* surfaceGrid = new wxGridSizer(0, 3, FromDIP(6), FromDIP(6));
    surfaces->Add(surfaceGrid, 0, wxEXPAND | wxALL, FromDIP(8));
    surfaceGrid->Add(new wxStaticText(surfaces->GetStaticBox(), wxID_ANY, "Menu bar"), 0, wxALL, FromDIP(3));
    surfaceGrid->Add(new wxStaticText(surfaces->GetStaticBox(), wxID_ANY, "Tool bar"), 0, wxALL, FromDIP(3));
    surfaceGrid->Add(new wxStaticText(surfaces->GetStaticBox(), wxID_ANY, "Status bar"), 0, wxALL, FromDIP(3));
    surfaceGrid->Add(new wxStaticText(surfaces->GetStaticBox(), wxID_ANY, "Splitter"), 0, wxALL, FromDIP(3));
    surfaceGrid->Add(new wxStaticText(surfaces->GetStaticBox(), wxID_ANY, "Tree navigation"), 0, wxALL, FromDIP(3));
    surfaceGrid->Add(new wxStaticText(surfaces->GetStaticBox(), wxID_ANY, "Notebook"), 0, wxALL, FromDIP(3));

#if wxUSE_BANNERWINDOW
    wxStaticBoxSizer* bannerGroup = AddGroup(page, top, "wxBannerWindow");
    wxBannerWindow* banner = new wxBannerWindow(bannerGroup->GetStaticBox(), wxTOP);
    banner->SetText("Showcase banner", "A banner window with title and message.");
    bannerGroup->Add(banner, 0, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_INFOBAR
    wxStaticBoxSizer* infoGroup = AddGroup(page, top, "wxInfoBar");
    wxInfoBar* info = new wxInfoBar(infoGroup->GetStaticBox());
    infoGroup->Add(info, 0, wxEXPAND | wxALL, FromDIP(8));
    info->ShowMessage("wxInfoBar message with an action button.", wxICON_INFORMATION);
    info->AddButton(wxID_OK, "Action");
#endif

    return page;
}

wxWindow* ShowcaseFrame::CreateButtonPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Button variants exercise labels, bitmaps, per-state bitmaps, auth shield, disabled/default state and command links.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

    wxStaticBoxSizer* basic = AddGroup(page, top, "wxButton and state");
    wxGridBagSizer* buttons = new wxGridBagSizer(FromDIP(6), FromDIP(6));
    basic->Add(buttons, 0, wxALL, FromDIP(8));

    wxButton* normal = new wxButton(basic->GetStaticBox(), wxID_ANY, "Normal button");
    normal->SetToolTip("Normal wxButton tooltip");
    WatchCommand(normal, "Normal button");
    buttons->Add(normal, wxGBPosition(0, 0), wxDefaultSpan, wxEXPAND);

    wxButton* def = new wxButton(basic->GetStaticBox(), wxID_ANY, "Default button");
    def->SetDefault();
    def->SetToolTip("Default button tooltip");
    WatchCommand(def, "Default button");
    buttons->Add(def, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND);

    wxButton* disabled = new wxButton(basic->GetStaticBox(), wxID_ANY, "Disabled button");
    disabled->Enable(false);
    disabled->SetToolTip("Disabled button tooltip");
    buttons->Add(disabled, wxGBPosition(0, 2), wxDefaultSpan, wxEXPAND);

    wxButton* styled = new wxButton(basic->GetStaticBox(), wxID_ANY, "Styled button");
    styled->SetForegroundColour(*wxBLUE);
    styled->SetBackgroundColour(wxColour(255, 245, 190));
    styled->SetFont(styled->GetFont().Bold());
    WatchCommand(styled, "Styled button");
    buttons->Add(styled, wxGBPosition(1, 0), wxDefaultSpan, wxEXPAND);

    wxButton* bitmapButton = new wxButton(basic->GetStaticBox(), wxID_ANY, "Bitmap states");
    bitmapButton->SetBitmap(GetArt(bitmapButton, wxART_INFORMATION, 16));
    bitmapButton->SetBitmapPressed(GetArt(bitmapButton, wxART_HELP, 16));
    bitmapButton->SetBitmapCurrent(GetArt(bitmapButton, wxART_WARNING, 16));
    bitmapButton->SetBitmapFocus(GetArt(bitmapButton, wxART_ERROR, 16));
    bitmapButton->SetBitmapDisabled(GetArt(bitmapButton, wxART_MISSING_IMAGE, 16));
    bitmapButton->SetBitmapMargins(FromDIP(4), FromDIP(2));
    bitmapButton->SetToolTip("Hover, press, focus and disable should use different images.");
    WatchCommand(bitmapButton, "Button bitmap states");
    buttons->Add(bitmapButton, wxGBPosition(1, 1), wxDefaultSpan, wxEXPAND);

    wxButton* auth = new wxButton(basic->GetStaticBox(), wxID_ANY, "Requires auth");
    auth->SetAuthNeeded();
    auth->SetToolTip("wxButton::SetAuthNeeded()");
    WatchCommand(auth, "Auth button");
    buttons->Add(auth, wxGBPosition(1, 2), wxDefaultSpan, wxEXPAND);

#if wxUSE_BMPBUTTON
    wxStaticBoxSizer* bitmapGroup = AddGroup(page, top, "wxBitmapButton");
    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    bitmapGroup->Add(row, 0, wxALL, FromDIP(8));
    wxBitmapButton* bmp = new wxBitmapButton(bitmapGroup->GetStaticBox(), wxID_ANY,
        GetArt(bitmapGroup->GetStaticBox(), wxART_INFORMATION, 32));
    bmp->SetBitmapPressed(GetArt(bmp, wxART_HELP, 32));
    bmp->SetBitmapCurrent(GetArt(bmp, wxART_WARNING, 32));
    bmp->SetBitmapFocus(GetArt(bmp, wxART_ERROR, 32));
    bmp->SetBitmapDisabled(GetArt(bmp, wxART_MISSING_IMAGE, 32));
    bmp->SetToolTip("wxBitmapButton with per-state images");
    WatchCommand(bmp, "Bitmap button");
    row->Add(bmp, 0, wxRIGHT, FromDIP(8));
    row->Add(wxBitmapButton::NewCloseButton(bitmapGroup->GetStaticBox(), wxID_ANY), 0);
#endif

    wxStaticBoxSizer* toggles = AddGroup(page, top, "Toggle and radio controls");
    wxGridSizer* toggleGrid = new wxGridSizer(0, 3, FromDIP(8), FromDIP(8));
    toggles->Add(toggleGrid, 0, wxALL, FromDIP(8));

#if wxUSE_TOGGLEBTN
    wxToggleButton* toggle = new wxToggleButton(toggles->GetStaticBox(), wxID_ANY, "wxToggleButton");
    toggle->SetValue(true);
    WatchCommand(toggle, "Toggle button");
    toggleGrid->Add(toggle, 0, wxEXPAND);
#endif
#if wxUSE_CHECKBOX
    wxCheckBox* check = new wxCheckBox(toggles->GetStaticBox(), wxID_ANY, "wxCheckBox",
        wxDefaultPosition, wxDefaultSize, wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);
    check->Set3StateValue(wxCHK_UNDETERMINED);
    WatchCommand(check, "Check box");
    toggleGrid->Add(check, 0, wxEXPAND);
#endif
#if wxUSE_RADIOBTN
    wxPanel* radioPanel = new wxPanel(toggles->GetStaticBox());
    wxBoxSizer* radioSizer = new wxBoxSizer(wxHORIZONTAL);
    radioPanel->SetSizer(radioSizer);
    radioSizer->Add(new wxRadioButton(radioPanel, wxID_ANY, "A", wxDefaultPosition,
        wxDefaultSize, wxRB_GROUP), 0, wxRIGHT, FromDIP(8));
    radioSizer->Add(new wxRadioButton(radioPanel, wxID_ANY, "B"), 0);
    toggleGrid->Add(radioPanel, 0, wxEXPAND);
#endif
#if wxUSE_RADIOBOX
    const wxString rbChoices[] = { "left", "centre", "right" };
    wxRadioBox* radioBox = new wxRadioBox(toggles->GetStaticBox(), wxID_ANY, "wxRadioBox",
        wxDefaultPosition, wxDefaultSize, WXSIZEOF(rbChoices), rbChoices, 1, wxRA_SPECIFY_ROWS);
    toggles->Add(radioBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(8));
#endif

#if wxUSE_COMMANDLINKBUTTON
    wxStaticBoxSizer* command = AddGroup(page, top, "wxCommandLinkButton");
    wxBoxSizer* commandRow = new wxBoxSizer(wxHORIZONTAL);
    command->Add(commandRow, 0, wxEXPAND | wxALL, FromDIP(8));

    wxCommandLinkButton* cmd = new wxCommandLinkButton(command->GetStaticBox(), wxID_ANY,
        "Command link", "The note should be visually distinct from the main label.");
    cmd->SetToolTip("Command link tooltip");
    WatchCommand(cmd, "Command link");
    commandRow->Add(cmd, 0, wxRIGHT, FromDIP(12));

    wxCommandLinkButton* cmdAuth = new wxCommandLinkButton(command->GetStaticBox(), wxID_ANY,
        "Admin command link", "This combines command-link layout with SetAuthNeeded().");
    cmdAuth->SetAuthNeeded();
    WatchCommand(cmdAuth, "Auth command link");
    commandRow->Add(cmdAuth, 0);
#endif

#if wxUSE_HYPERLINKCTRL
    wxStaticBoxSizer* links = AddGroup(page, top, "wxHyperlinkCtrl");
    wxHyperlinkCtrl* link = new wxHyperlinkCtrl(links->GetStaticBox(), wxID_ANY,
        "wxWidgets home page", "https://www.wxwidgets.org/");
    link->SetToolTip("Hyperlink tooltip");
    links->Add(link, 0, wxALL, FromDIP(8));
#endif

    return page;
}

wxWindow* ShowcaseFrame::CreateTextInputPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Text and numeric inputs cover editable, read-only, multiline, password, search, spin, slider, scrollbar and gauge controls.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

    wxStaticBoxSizer* text = AddGroup(page, top, "wxTextCtrl");
    wxFlexGridSizer* grid = new wxFlexGridSizer(0, 2, FromDIP(6), FromDIP(8));
    grid->AddGrowableCol(1, 1);
    text->Add(grid, 0, wxEXPAND | wxALL, FromDIP(8));

    grid->Add(new wxStaticText(text->GetStaticBox(), wxID_ANY, "Single line"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxTextCtrl(text->GetStaticBox(), wxID_ANY, "Editable text", wxDefaultPosition,
        wxDefaultSize, wxTE_PROCESS_ENTER), 0, wxEXPAND);

    grid->Add(new wxStaticText(text->GetStaticBox(), wxID_ANY, "Password"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxTextCtrl(text->GetStaticBox(), wxID_ANY, "secret", wxDefaultPosition,
        wxDefaultSize, wxTE_PASSWORD), 0, wxEXPAND);

    grid->Add(new wxStaticText(text->GetStaticBox(), wxID_ANY, "Read only"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxTextCtrl(text->GetStaticBox(), wxID_ANY, "Read-only value", wxDefaultPosition,
        wxDefaultSize, wxTE_READONLY), 0, wxEXPAND);

    wxTextCtrl* multi = new wxTextCtrl(text->GetStaticBox(), wxID_ANY,
        "Multiline text control\nSecond line\nThird line",
        wxDefaultPosition, FromDIP(wxSize(-1, 95)), wxTE_MULTILINE | wxTE_RICH2);
    text->Add(multi, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(8));

#if wxUSE_SEARCHCTRL
    wxStaticBoxSizer* searchGroup = AddGroup(page, top, "wxSearchCtrl");
    wxSearchCtrl* search = new wxSearchCtrl(searchGroup->GetStaticBox(), wxID_ANY);
    search->ShowSearchButton(true);
    search->ShowCancelButton(true);
    search->SetDescriptiveText("Search placeholder");
    wxArrayString suggestions;
    suggestions.Add("button");
    suggestions.Add("bitmap");
    suggestions.Add("command link");
    suggestions.Add("tooltip");
    search->AutoComplete(suggestions);
    searchGroup->Add(search, 0, wxEXPAND | wxALL, FromDIP(8));
#endif

    wxStaticBoxSizer* numeric = AddGroup(page, top, "Numeric inputs");
    wxGridSizer* ngrid = new wxGridSizer(0, 2, FromDIP(8), FromDIP(8));
    numeric->Add(ngrid, 0, wxEXPAND | wxALL, FromDIP(8));

#if wxUSE_SPINCTRL
    wxSpinCtrl* spin = new wxSpinCtrl(numeric->GetStaticBox(), wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 42);
    ngrid->Add(spin, 0, wxEXPAND);

    wxSpinCtrlDouble* spinDouble = new wxSpinCtrlDouble(numeric->GetStaticBox(), wxID_ANY,
        wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.0, 10.0, 2.5, 0.25);
    spinDouble->SetDigits(2);
    ngrid->Add(spinDouble, 0, wxEXPAND);
#endif
#if wxUSE_SPINBTN
    wxSpinButton* spinButton = new wxSpinButton(numeric->GetStaticBox(), wxID_ANY);
    spinButton->SetRange(0, 100);
    spinButton->SetValue(30);
    ngrid->Add(spinButton, 0, wxEXPAND);
#endif
#if wxUSE_SLIDER
    wxSlider* slider = new wxSlider(numeric->GetStaticBox(), wxID_ANY, 40, 0, 100,
        wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    numeric->Add(slider, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(8));
#endif
#if wxUSE_SCROLLBAR
    wxScrollBar* scrollbar = new wxScrollBar(numeric->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    scrollbar->SetScrollbar(20, 10, 100, 10);
    numeric->Add(scrollbar, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(8));
#endif
#if wxUSE_GAUGE
    m_gauge = new wxGauge(numeric->GetStaticBox(), wxID_ANY, 100);
    m_gauge->SetValue(35);
    numeric->Add(m_gauge, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(8));
#endif

    return page;
}

wxWindow* ShowcaseFrame::CreateChoicesAndListsPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Choice-like controls test selection, editable combo surfaces, owner drawn items and list mutations.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

    wxStaticBoxSizer* choices = AddGroup(page, top, "Choices and combo controls");
    wxGridSizer* choiceGrid = new wxGridSizer(0, 2, FromDIP(8), FromDIP(8));
    choices->Add(choiceGrid, 0, wxEXPAND | wxALL, FromDIP(8));

    const wxString items[] = { "First", "Second", "Third", "Fourth" };
#if wxUSE_CHOICE
    wxChoice* choice = new wxChoice(choices->GetStaticBox(), wxID_ANY, wxDefaultPosition,
        wxDefaultSize, WXSIZEOF(items), items);
    choice->SetSelection(1);
    WatchCommand(choice, "Choice");
    choiceGrid->Add(choice, 0, wxEXPAND);
#endif
#if wxUSE_COMBOBOX
    wxComboBox* combo = new wxComboBox(choices->GetStaticBox(), wxID_ANY, "Editable value",
        wxDefaultPosition, wxDefaultSize, WXSIZEOF(items), items);
    WatchCommand(combo, "ComboBox");
    choiceGrid->Add(combo, 0, wxEXPAND);
#endif
#if wxUSE_COMBOCTRL
    wxComboCtrl* comboCtrl = new wxComboCtrl(choices->GetStaticBox(), wxID_ANY, "wxComboCtrl");
    comboCtrl->SetToolTip("Base wxComboCtrl without a custom popup");
    choiceGrid->Add(comboCtrl, 0, wxEXPAND);
#endif
#if wxUSE_ODCOMBOBOX
    wxOwnerDrawnComboBox* od = new wxOwnerDrawnComboBox(choices->GetStaticBox(), wxID_ANY);
    od->Append("Owner drawn item A");
    od->Append("Owner drawn item B");
    od->Append("Owner drawn item C");
    od->SetSelection(0);
    choiceGrid->Add(od, 0, wxEXPAND);
#endif
#if wxUSE_BITMAPCOMBOBOX
    wxBitmapComboBox* bmpCombo = new wxBitmapComboBox(choices->GetStaticBox(), wxID_ANY);
    bmpCombo->Append("Information", GetArt(bmpCombo, wxART_INFORMATION, 16));
    bmpCombo->Append("Warning", GetArt(bmpCombo, wxART_WARNING, 16));
    bmpCombo->Append("Error", GetArt(bmpCombo, wxART_ERROR, 16));
    bmpCombo->SetSelection(0);
    choiceGrid->Add(bmpCombo, 0, wxEXPAND);
#endif

    wxStaticBoxSizer* lists = AddGroup(page, top, "List controls");
    wxGridSizer* listGrid = new wxGridSizer(0, 2, FromDIP(8), FromDIP(8));
    lists->Add(listGrid, 1, wxEXPAND | wxALL, FromDIP(8));

#if wxUSE_LISTBOX
    wxListBox* list = new wxListBox(lists->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 120)), WXSIZEOF(items), items);
    list->SetSelection(0);
    listGrid->Add(list, 1, wxEXPAND);
#endif
#if wxUSE_CHECKLISTBOX
    wxCheckListBox* checklist = new wxCheckListBox(lists->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 120)), WXSIZEOF(items), items);
    checklist->Check(0);
    checklist->Check(2);
    listGrid->Add(checklist, 1, wxEXPAND);
#endif
#if wxUSE_REARRANGECTRL
    wxArrayString rearrangeItems;
    rearrangeItems.Add("Alpha");
    rearrangeItems.Add("Beta");
    rearrangeItems.Add("Gamma");
    rearrangeItems.Add("Delta");
    wxArrayInt order;
    order.Add(0);
    order.Add(1);
    order.Add(-2);
    order.Add(3);
    wxRearrangeCtrl* rearrange = new wxRearrangeCtrl(lists->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 130)), order, rearrangeItems);
    listGrid->Add(rearrange, 1, wxEXPAND);
#endif
#if wxUSE_EDITABLELISTBOX && !defined(__WXWINUI__)
    wxEditableListBox* editable = new wxEditableListBox(lists->GetStaticBox(), wxID_ANY,
        "wxEditableListBox", wxDefaultPosition, FromDIP(wxSize(-1, 130)),
        wxEL_ALLOW_NEW | wxEL_ALLOW_EDIT | wxEL_ALLOW_DELETE);
    wxArrayString editItems;
    editItems.Add("*.cpp");
    editItems.Add("*.h");
    editItems.Add("CMakeLists.txt");
    editable->SetStrings(editItems);
    listGrid->Add(editable, 1, wxEXPAND);
#elif wxUSE_EDITABLELISTBOX
    wxStaticText* editableSkipped = new wxStaticText(lists->GetStaticBox(), wxID_ANY,
        "wxEditableListBox: skipped on wxWinUI for now. Its generic implementation "
        "currently asserts while casting the internal list control to wxGenericListCtrl.");
    editableSkipped->Wrap(FromDIP(360));
    listGrid->Add(editableSkipped, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

    return page;
}

wxWindow* ShowcaseFrame::CreateContainerPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Nested book controls and layout containers test sizing, scrolling, page switching and child reparenting.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

#if wxUSE_SPLITTER
    wxStaticBoxSizer* splitterGroup = AddGroup(page, top, "wxSplitterWindow");
    wxSplitterWindow* splitter = new wxSplitterWindow(splitterGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 120)), wxSP_LIVE_UPDATE | wxSP_3D);
    wxPanel* left = new wxPanel(splitter);
    wxPanel* right = new wxPanel(splitter);
    left->SetBackgroundColour(wxColour(235, 245, 255));
    right->SetBackgroundColour(wxColour(245, 235, 255));
    new wxStaticText(left, wxID_ANY, "Left pane", FromDIP(wxPoint(10, 10)));
    new wxStaticText(right, wxID_ANY, "Right pane", FromDIP(wxPoint(10, 10)));
    splitter->SplitVertically(left, right, FromDIP(220));
    splitter->SetMinimumPaneSize(FromDIP(80));
    splitterGroup->Add(splitter, 0, wxEXPAND | wxALL, FromDIP(8));
#endif

    wxStaticBoxSizer* books = AddGroup(page, top, "Book controls");
    wxGridSizer* bookGrid = new wxGridSizer(0, 2, FromDIP(8), FromDIP(8));
    books->Add(bookGrid, 0, wxEXPAND | wxALL, FromDIP(8));

#if wxUSE_NOTEBOOK
    wxNotebook* notebook = new wxNotebook(books->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 120)));
    notebook->AddPage(new wxPanel(notebook), "Notebook A");
    notebook->AddPage(new wxPanel(notebook), "Notebook B");
    bookGrid->Add(notebook, 1, wxEXPAND);
#endif
#if wxUSE_LISTBOOK
    wxListbook* listbook = new wxListbook(books->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 120)));
    listbook->AddPage(new wxPanel(listbook), "List A");
    listbook->AddPage(new wxPanel(listbook), "List B");
    bookGrid->Add(listbook, 1, wxEXPAND);
#endif
#if wxUSE_CHOICEBOOK
    wxChoicebook* choicebook = new wxChoicebook(books->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 120)));
    choicebook->AddPage(new wxPanel(choicebook), "Choice A");
    choicebook->AddPage(new wxPanel(choicebook), "Choice B");
    bookGrid->Add(choicebook, 1, wxEXPAND);
#endif
#if wxUSE_TREEBOOK
    wxTreebook* treebook = new wxTreebook(books->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 140)));
    treebook->AddPage(new wxPanel(treebook), "Root");
    treebook->AddSubPage(new wxPanel(treebook), "Child");
    bookGrid->Add(treebook, 1, wxEXPAND);
#endif
#if wxUSE_TOOLBOOK && !defined(__WXWINUI__)
    wxToolbook* toolbook = new wxToolbook(books->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 130)));
    wxImageList* imgs = new wxImageList(16, 16);
    imgs->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, FromDIP(wxSize(16, 16))));
    imgs->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, FromDIP(wxSize(16, 16))));
    toolbook->AssignImageList(imgs);
    toolbook->AddPage(new wxPanel(toolbook), "Tool A", true, 0);
    toolbook->AddPage(new wxPanel(toolbook), "Tool B", false, 1);
    bookGrid->Add(toolbook, 1, wxEXPAND);
#elif wxUSE_TOOLBOOK
    wxStaticText* toolbookSkipped = new wxStaticText(books->GetStaticBox(), wxID_ANY,
        "wxToolbook: skipped on wxWinUI for now. Its generic implementation "
        "uses an internal wxToolBar path that logs TB_GETITEMRECT failures during startup.");
    toolbookSkipped->Wrap(FromDIP(360));
    bookGrid->Add(toolbookSkipped, 1, wxEXPAND | wxALL, FromDIP(8));
#endif
#if wxUSE_BOOKCTRL
    wxSimplebook* simple = new wxSimplebook(books->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 120)));
    wxPanel* first = new wxPanel(simple);
    wxBoxSizer* firstSizer = new wxBoxSizer(wxVERTICAL);
    first->SetSizer(firstSizer);
    firstSizer->Add(new wxStaticText(first, wxID_ANY, "Simplebook page 1"), 0, wxALL, FromDIP(8));
    wxPanel* second = new wxPanel(simple);
    wxBoxSizer* secondSizer = new wxBoxSizer(wxVERTICAL);
    second->SetSizer(secondSizer);
    secondSizer->Add(new wxStaticText(second, wxID_ANY, "Simplebook page 2"), 0, wxALL, FromDIP(8));
    simple->AddPage(first, "Simple A");
    simple->AddPage(second, "Simple B");
    bookGrid->Add(simple, 1, wxEXPAND);
#endif

#if wxUSE_COLLPANE
    wxStaticBoxSizer* coll = AddGroup(page, top, "wxCollapsiblePane");
    wxCollapsiblePane* pane = new wxCollapsiblePane(coll->GetStaticBox(), wxID_ANY, "Expandable details");
    wxWindow* inner = pane->GetPane();
    wxBoxSizer* innerSizer = new wxBoxSizer(wxVERTICAL);
    inner->SetSizer(innerSizer);
    innerSizer->Add(new wxStaticText(inner, wxID_ANY, "Content inside the collapsible pane."),
        0, wxALL, FromDIP(8));
    coll->Add(pane, 0, wxEXPAND | wxALL, FromDIP(8));
#endif

    return page;
}

wxWindow* ShowcaseFrame::CreatePickerPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Pickers and calendar controls exercise composite controls plus native dialog integration.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

    wxStaticBoxSizer* pickers = AddGroup(page, top, "Picker controls");
    wxFlexGridSizer* grid = new wxFlexGridSizer(0, 2, FromDIP(8), FromDIP(8));
    grid->AddGrowableCol(1, 1);
    pickers->Add(grid, 0, wxEXPAND | wxALL, FromDIP(8));

#if wxUSE_COLOURPICKERCTRL
    grid->Add(new wxStaticText(pickers->GetStaticBox(), wxID_ANY, "Colour"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxColourPickerCtrl(pickers->GetStaticBox(), wxID_ANY, *wxBLUE), 0, wxEXPAND);
#endif
#if wxUSE_FILEPICKERCTRL
    grid->Add(new wxStaticText(pickers->GetStaticBox(), wxID_ANY, "File"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxFilePickerCtrl(pickers->GetStaticBox(), wxID_ANY), 0, wxEXPAND);
#endif
#if wxUSE_DIRPICKERCTRL
    grid->Add(new wxStaticText(pickers->GetStaticBox(), wxID_ANY, "Directory"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxDirPickerCtrl(pickers->GetStaticBox(), wxID_ANY), 0, wxEXPAND);
#endif
#if wxUSE_FONTPICKERCTRL
    grid->Add(new wxStaticText(pickers->GetStaticBox(), wxID_ANY, "Font"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxFontPickerCtrl(pickers->GetStaticBox(), wxID_ANY), 0, wxEXPAND);
#endif
#if wxUSE_DATEPICKCTRL
    grid->Add(new wxStaticText(pickers->GetStaticBox(), wxID_ANY, "Date"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxDatePickerCtrl(pickers->GetStaticBox(), wxID_ANY), 0, wxEXPAND);
#endif
#if wxUSE_TIMEPICKCTRL
    grid->Add(new wxStaticText(pickers->GetStaticBox(), wxID_ANY, "Time"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(new wxTimePickerCtrl(pickers->GetStaticBox(), wxID_ANY), 0, wxEXPAND);
#endif

#if wxUSE_CALENDARCTRL
    wxStaticBoxSizer* calendarGroup = AddGroup(page, top, "wxCalendarCtrl");
    calendarGroup->Add(new wxCalendarCtrl(calendarGroup->GetStaticBox(), wxID_ANY),
        0, wxALL, FromDIP(8));
#endif

    wxStaticBoxSizer* fileCtrls = AddGroup(page, top, "File and directory browser controls");
    wxGridSizer* browserGrid = new wxGridSizer(0, 2, FromDIP(8), FromDIP(8));
    fileCtrls->Add(browserGrid, 1, wxEXPAND | wxALL, FromDIP(8));
#if wxUSE_FILECTRL && !defined(__WXWINUI__)
    wxFileCtrl* fileCtrl = new wxFileCtrl(fileCtrls->GetStaticBox(), wxID_ANY,
        wxEmptyString, wxEmptyString, "*.*", wxFC_OPEN,
        wxDefaultPosition, FromDIP(wxSize(-1, 180)));
    browserGrid->Add(fileCtrl, 1, wxEXPAND);
#elif wxUSE_FILECTRL
    wxStaticText* fileCtrlSkipped = new wxStaticText(fileCtrls->GetStaticBox(), wxID_ANY,
        "wxFileCtrl: skipped on wxWinUI for now. The generic file browser uses "
        "wxGenericListCtrl internally and currently asserts on resize.");
    fileCtrlSkipped->Wrap(FromDIP(360));
    browserGrid->Add(fileCtrlSkipped, 1, wxEXPAND | wxALL, FromDIP(8));
#endif
#if (wxUSE_FILEDLG || wxUSE_DIRDLG) && !defined(__WXWINUI__)
    wxGenericDirCtrl* dirCtrl = new wxGenericDirCtrl(fileCtrls->GetStaticBox(), wxID_ANY,
        wxEmptyString, wxDefaultPosition, FromDIP(wxSize(-1, 180)));
    browserGrid->Add(dirCtrl, 1, wxEXPAND);
#elif wxUSE_FILEDLG || wxUSE_DIRDLG
    wxStaticText* dirCtrlSkipped = new wxStaticText(fileCtrls->GetStaticBox(), wxID_ANY,
        "wxGenericDirCtrl: skipped on wxWinUI for now to keep this startup "
        "showcase usable while generic browser controls are being ported.");
    dirCtrlSkipped->Wrap(FromDIP(360));
    browserGrid->Add(dirCtrlSkipped, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

    return page;
}

wxWindow* ShowcaseFrame::CreateDataPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Data controls include report/list/tree/header/grid/property views and the generic data view controls.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

    wxStaticBoxSizer* classic = AddGroup(page, top, "Classic native/generic data controls");
    wxGridSizer* classicGrid = new wxGridSizer(0, 2, FromDIP(8), FromDIP(8));
    classic->Add(classicGrid, 1, wxEXPAND | wxALL, FromDIP(8));

#if wxUSE_LISTCTRL && !defined(__WXWINUI__)
    wxListCtrl* list = new wxListCtrl(classic->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 150)), wxLC_REPORT | wxLC_SINGLE_SEL);
    list->AppendColumn("Control");
    list->AppendColumn("Status");
    list->InsertItem(0, "wxButton");
    list->SetItem(0, 1, "created");
    list->InsertItem(1, "wxTextCtrl");
    list->SetItem(1, 1, "created");
    list->InsertItem(2, "wxDataViewListCtrl");
    list->SetItem(2, 1, "optional");
    classicGrid->Add(list, 1, wxEXPAND);
#elif wxUSE_LISTCTRL
    wxStaticText* listSkipped = new wxStaticText(classic->GetStaticBox(), wxID_ANY,
        "wxListCtrl: skipped on wxWinUI for now. The current generic path asserts "
        "in wxGenericListCtrl::OnSize() while casting the list implementation.");
    listSkipped->Wrap(FromDIP(360));
    classicGrid->Add(listSkipped, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_TREECTRL
    wxTreeCtrl* tree = new wxTreeCtrl(classic->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 150)), wxTR_DEFAULT_STYLE | wxTR_HAS_BUTTONS);
    wxTreeItemId root = tree->AddRoot("wxWidgets");
    wxTreeItemId controls = tree->AppendItem(root, "Controls");
    tree->AppendItem(controls, "Buttons");
    tree->AppendItem(controls, "Text");
    tree->AppendItem(controls, "Data");
    tree->ExpandAll();
    classicGrid->Add(tree, 1, wxEXPAND);
#endif

#if wxUSE_HEADERCTRL
    wxHeaderCtrlSimple* header = new wxHeaderCtrlSimple(classic->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 40)));
    wxHeaderColumnSimple col1("Name", FromDIP(150), wxALIGN_LEFT,
        wxCOL_RESIZABLE | wxCOL_SORTABLE);
    col1.SetBitmap(GetArt(header, wxART_INFORMATION, 16));
    header->AppendColumn(col1);
    header->AppendColumn(wxHeaderColumnSimple("Value", FromDIP(120), wxALIGN_RIGHT,
        wxCOL_RESIZABLE | wxCOL_SORTABLE));
    classicGrid->Add(header, 0, wxEXPAND);
#endif

#if wxUSE_DATAVIEWCTRL && !defined(__WXWINUI__)
    wxStaticBoxSizer* dataViewGroup = AddGroup(page, top, "wxDataViewListCtrl");
    wxDataViewListCtrl* data = new wxDataViewListCtrl(dataViewGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 170)));
    data->AppendToggleColumn("Enabled");
    data->AppendTextColumn("Control");
    data->AppendProgressColumn("Progress");
    for ( int i = 0; i < 5; ++i )
    {
        wxVector<wxVariant> row;
        row.push_back((i % 2) == 0);
        row.push_back(wxString::Format("Row %d", i + 1));
        row.push_back(long(i * 20));
        data->AppendItem(row);
    }
    dataViewGroup->Add(data, 1, wxEXPAND | wxALL, FromDIP(8));
#elif wxUSE_DATAVIEWCTRL
    wxStaticBoxSizer* dataViewGroup = AddGroup(page, top, "wxDataViewListCtrl");
    wxStaticText* dataSkipped = new wxStaticText(dataViewGroup->GetStaticBox(), wxID_ANY,
        "wxDataViewListCtrl: skipped on wxWinUI startup for now. Re-enable it "
        "after the generic list control cast/assert path is fixed.");
    dataSkipped->Wrap(FromDIP(420));
    dataViewGroup->Add(dataSkipped, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_TREELISTCTRL && !defined(__WXWINUI__)
    wxStaticBoxSizer* treeListGroup = AddGroup(page, top, "wxTreeListCtrl");
    wxTreeListCtrl* treeList = new wxTreeListCtrl(treeListGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 170)),
        wxTL_DEFAULT_STYLE | wxTL_CHECKBOX);
    treeList->AppendColumn("Component", wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT,
        wxCOL_RESIZABLE | wxCOL_SORTABLE);
    treeList->AppendColumn("Files", treeList->WidthFor("1,000"), wxALIGN_RIGHT,
        wxCOL_RESIZABLE | wxCOL_SORTABLE);
    wxTreeListItem treeListRoot = treeList->GetRootItem();
    wxTreeListItem src = treeList->AppendItem(treeListRoot, "src");
    treeList->SetItemText(src, 1, "many");
    wxTreeListItem winui = treeList->AppendItem(src, "winui");
    treeList->SetItemText(winui, 1, "controls");
    treeList->CheckItem(winui, wxCHK_CHECKED);
    wxTreeListItem samples = treeList->AppendItem(treeListRoot, "samples");
    treeList->SetItemText(samples, 1, "showcase");
    treeList->Expand(src);
    treeList->Expand(treeListRoot);
    treeListGroup->Add(treeList, 1, wxEXPAND | wxALL, FromDIP(8));
#elif wxUSE_TREELISTCTRL
    wxStaticBoxSizer* treeListGroup = AddGroup(page, top, "wxTreeListCtrl");
    wxStaticText* treeListSkipped = new wxStaticText(treeListGroup->GetStaticBox(), wxID_ANY,
        "wxTreeListCtrl: skipped on wxWinUI startup for now. This keeps the "
        "rest of the showcase available while tree/list data controls are tested separately.");
    treeListSkipped->Wrap(FromDIP(420));
    treeListGroup->Add(treeListSkipped, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_GRID
    wxStaticBoxSizer* gridGroup = AddGroup(page, top, "wxGrid");
    wxGrid* grid = new wxGrid(gridGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 180)));
    grid->CreateGrid(5, 4);
    for ( int row = 0; row < 5; ++row )
    {
        for ( int col = 0; col < 4; ++col )
        {
            grid->SetCellValue(row, col, wxString::Format("R%d C%d", row + 1, col + 1));
        }
    }
    gridGroup->Add(grid, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_PROPGRID
    wxStaticBoxSizer* propGroup = AddGroup(page, top, "wxPropertyGrid");
    wxPropertyGrid* pg = new wxPropertyGrid(propGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 210)),
        wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED);
    pg->Append(new wxPropertyCategory("Appearance"));
    pg->Append(new wxStringProperty("Label", wxPG_LABEL, "Showcase"));
    pg->Append(new wxIntProperty("Margin", wxPG_LABEL, 8));
    pg->Append(new wxBoolProperty("Enabled", wxPG_LABEL, true));
    pg->Append(new wxColourProperty("Accent", wxPG_LABEL, *wxBLUE));
    propGroup->Add(pg, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

    return page;
}

wxWindow* ShowcaseFrame::CreateAdvancedPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Advanced optional libraries are shown when they are enabled in this build.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

#if wxUSE_AUI
    wxStaticBoxSizer* auiGroup = AddGroup(page, top, "wxAuiNotebook");
    wxAuiNotebook* aui = new wxAuiNotebook(auiGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 140)));
    aui->AddPage(new wxTextCtrl(aui, wxID_ANY, "AUI page A", wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE), "AUI A");
    aui->AddPage(new wxTextCtrl(aui, wxID_ANY, "AUI page B", wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE), "AUI B");
    auiGroup->Add(aui, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_RIBBON
    wxStaticBoxSizer* ribbonGroup = AddGroup(page, top, "wxRibbonBar");
    wxRibbonBar* ribbon = new wxRibbonBar(ribbonGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, wxDefaultSize,
        wxRIBBON_BAR_DEFAULT_STYLE | wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS);
    wxRibbonPage* home = new wxRibbonPage(ribbon, wxID_ANY, "Home", GetArt(ribbon, wxART_GO_HOME, 16));
    wxRibbonPanel* actions = new wxRibbonPanel(home, wxID_ANY, "Actions");
    wxRibbonButtonBar* bar = new wxRibbonButtonBar(actions);
    bar->AddButton(wxID_NEW, "New", GetArt(bar, wxART_NEW, 32));
    bar->AddButton(wxID_OPEN, "Open", GetArt(bar, wxART_FILE_OPEN, 32));
    bar->AddHybridButton(wxID_SAVE, "Save", GetArt(bar, wxART_FILE_SAVE, 32));
    ribbon->Realize();
    ribbonGroup->Add(ribbon, 0, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_RICHTEXT
    wxStaticBoxSizer* richGroup = AddGroup(page, top, "wxRichTextCtrl");
    wxRichTextCtrl* rich = new wxRichTextCtrl(richGroup->GetStaticBox(), wxID_ANY,
        wxEmptyString, wxDefaultPosition, FromDIP(wxSize(-1, 150)),
        wxTE_MULTILINE | wxVSCROLL);
    rich->BeginBold();
    rich->WriteText("Bold rich text");
    rich->EndBold();
    rich->Newline();
    rich->BeginItalic();
    rich->WriteText("Italic rich text");
    rich->EndItalic();
    richGroup->Add(rich, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_STC
    wxStaticBoxSizer* stcGroup = AddGroup(page, top, "wxStyledTextCtrl");
    wxStyledTextCtrl* stc = new wxStyledTextCtrl(stcGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 150)));
    stc->SetText("int main()\n{\n    return 0;\n}\n");
    stc->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(96, 96, 96));
    stc->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    stc->SetMarginWidth(0, FromDIP(36));
    stcGroup->Add(stc, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_HTML
    wxStaticBoxSizer* htmlGroup = AddGroup(page, top, "wxHtmlWindow");
    wxHtmlWindow* html = new wxHtmlWindow(htmlGroup->GetStaticBox(), wxID_ANY,
        wxDefaultPosition, FromDIP(wxSize(-1, 150)));
    html->SetPage("<html><body><h3>wxHtmlWindow</h3><p>HTML rendering smoke test.</p></body></html>");
    htmlGroup->Add(html, 1, wxEXPAND | wxALL, FromDIP(8));
#endif

#if wxUSE_WEBVIEW
    wxStaticBoxSizer* webGroup = AddGroup(page, top, "wxWebView");
    wxWebView* web = wxWebView::New(webGroup->GetStaticBox(), wxID_ANY,
        wxWebViewDefaultURLStr, wxDefaultPosition, FromDIP(wxSize(-1, 180)));
    if ( web )
    {
        web->SetPage("<html><body><h3>wxWebView</h3><p>WebView backend is available.</p></body></html>", "");
        webGroup->Add(web, 1, wxEXPAND | wxALL, FromDIP(8));
    }
    else
    {
        AddWrappedText(webGroup->GetStaticBox(), webGroup, "wxWebView is enabled but no backend was created.");
    }
#endif

#if wxUSE_MEDIACTRL
    wxStaticBoxSizer* mediaGroup = AddGroup(page, top, "wxMediaCtrl");
    wxMediaCtrl* media = new wxMediaCtrl();
    if ( media->Create(mediaGroup->GetStaticBox(), wxID_ANY, wxEmptyString,
                       wxDefaultPosition, FromDIP(wxSize(-1, 80))) )
    {
        mediaGroup->Add(media, 0, wxEXPAND | wxALL, FromDIP(8));
        AddWrappedText(mediaGroup->GetStaticBox(), mediaGroup,
            "No media file is loaded; this only verifies that the control/backend can be created.");
    }
    else
    {
        delete media;
        AddWrappedText(mediaGroup->GetStaticBox(), mediaGroup,
            "wxMediaCtrl is enabled but the backend refused to create a control.");
    }
#endif

#if wxUSE_ACTIVITYINDICATOR
    wxStaticBoxSizer* activityGroup = AddGroup(page, top, "wxActivityIndicator");
    wxActivityIndicator* activity = new wxActivityIndicator(activityGroup->GetStaticBox(), wxID_ANY);
    activity->Start();
    activityGroup->Add(activity, 0, wxALL, FromDIP(8));
#endif

    return page;
}

wxWindow* ShowcaseFrame::CreateDialogPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Dialog launchers keep modal and transient surfaces available from one app.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

    wxStaticBoxSizer* standard = AddGroup(page, top, "Common dialogs");
    wxGridSizer* buttons = new wxGridSizer(0, 3, FromDIP(6), FromDIP(6));
    standard->Add(buttons, 0, wxALL, FromDIP(8));

    AddActionButton(standard->GetStaticBox(), buttons, "wxMessageDialog", [this] { ShowMessageDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "wxFileDialog", [this] { ShowFileDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "wxDirDialog", [this] { ShowDirectoryDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "wxColourDialog", [this] { ShowColourDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "wxFontDialog", [this] { ShowFontDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "wxProgressDialog", [this] { ShowProgressDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "Text/number dialogs", [this] { ShowTextDialogs(); });
    AddActionButton(standard->GetStaticBox(), buttons, "Choice dialogs", [this] { ShowChoiceDialogs(); });
    AddActionButton(standard->GetStaticBox(), buttons, "Find/replace", [this] { ShowFindDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "Wizard", [this] { ShowWizardDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "Rearrange dialog", [this] { ShowRearrangeDialog(); });
    AddActionButton(standard->GetStaticBox(), buttons, "Notification", [this] { ShowNotification(); });
    AddActionButton(standard->GetStaticBox(), buttons, "Tooltip windows", [this] { ShowTooltipWindows(); });
    AddActionButton(standard->GetStaticBox(), buttons, "Splash screen", [this] { ShowSplash(); });

    return page;
}

wxWindow* ShowcaseFrame::CreateEventsPage()
{
    wxScrolledWindow* page = CreateScrolledPage(
        "Event log for menu, toolbar, button and dialog launcher activity.");
    wxBoxSizer* top = wxDynamicCast(page->GetSizer(), wxBoxSizer);

    m_log = new wxTextCtrl(page, wxID_ANY, wxEmptyString, wxDefaultPosition,
        FromDIP(wxSize(-1, 420)), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    top->Add(m_log, 1, wxEXPAND | wxALL, FromDIP(10));

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    top->Add(row, 0, wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(10));
    AddActionButton(page, row, "Clear log", [this]
    {
        if ( m_log )
            m_log->Clear();
    });
    AddActionButton(page, row, "Append test line", [this]
    {
        Log("Manual log line");
    });

    return page;
}

void ShowcaseFrame::FillNavigation()
{
    wxTreeItemId root = m_nav->AddRoot("Pages");
    const int count = m_book->GetPageCount();
    for ( int i = 0; i < count; ++i )
    {
        m_nav->AppendItem(root, m_book->GetPageText(i), -1, -1, new PageItemData(i));
    }
    if ( count > 0 )
    {
        wxTreeItemIdValue cookie;
        m_nav->SelectItem(m_nav->GetFirstChild(root, cookie));
    }
}

void ShowcaseFrame::Log(const wxString& message)
{
    const wxString line = wxDateTime::Now().FormatISOTime() + "  " + message + "\n";
    if ( m_log )
        m_log->AppendText(line);
#if wxUSE_STATUSBAR
    SetStatusText(message, 0);
#endif
}

void ShowcaseFrame::WatchCommand(wxWindow* win, const wxString& name)
{
    win->Bind(wxEVT_BUTTON, [this, name](wxCommandEvent&)
    {
        Log(name + " clicked");
    });
    win->Bind(wxEVT_CHECKBOX, [this, name](wxCommandEvent&)
    {
        Log(name + " toggled");
    });
    win->Bind(wxEVT_CHOICE, [this, name](wxCommandEvent&)
    {
        Log(name + " changed");
    });
}

void ShowcaseFrame::SetChildrenEnabled(wxWindow* parent, bool enabled)
{
    const wxWindowList& children = parent->GetChildren();
    for ( wxWindowList::const_iterator it = children.begin(); it != children.end(); ++it )
    {
        wxWindow* child = *it;
        if ( child == m_nav || child == m_book )
            continue;

        child->Enable(enabled);
        SetChildrenEnabled(child, enabled);
    }
}

void ShowcaseFrame::OnNavigation(wxTreeEvent& event)
{
    PageItemData* data = static_cast<PageItemData*>(m_nav->GetItemData(event.GetItem()));
    if ( data )
    {
        m_book->ChangeSelection(data->GetPage());
        Log("Selected page: " + m_book->GetPageText(data->GetPage()));
    }
}

void ShowcaseFrame::OnAnyCommand(wxCommandEvent& event)
{
    Log(wxString::Format("Command id %d", event.GetId()));
    event.Skip();
}

void ShowcaseFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
#if wxUSE_GAUGE
    if ( m_gauge )
    {
        int value = m_gauge->GetValue() + 3;
        if ( value > m_gauge->GetRange() )
            value = 0;
        m_gauge->SetValue(value);
    }
#endif
}

void ShowcaseFrame::ShowMessageDialog()
{
    wxMessageDialog dialog(this,
        "This is a wxMessageDialog with Yes/No/Cancel buttons.",
        "wxMessageDialog",
        wxYES_NO | wxCANCEL | wxICON_INFORMATION);
    dialog.ShowModal();
}

void ShowcaseFrame::ShowFileDialog()
{
#if wxUSE_FILEDLG
    wxFileDialog dialog(this, "Open file", wxEmptyString, wxEmptyString,
        "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    dialog.ShowModal();
#else
    wxMessageBox("wxFileDialog is disabled in this build.", "wxFileDialog", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowColourDialog()
{
#if wxUSE_COLOURDLG
    wxColourData data;
    data.SetColour(*wxBLUE);
    wxColourDialog dialog(this, &data);
    dialog.ShowModal();
#else
    wxMessageBox("wxColourDialog is disabled in this build.", "wxColourDialog", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowFontDialog()
{
#if wxUSE_FONTDLG
    wxFontData data;
    data.SetInitialFont(GetFont());
    wxFontDialog dialog(this, data);
    dialog.ShowModal();
#else
    wxMessageBox("wxFontDialog is disabled in this build.", "wxFontDialog", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowProgressDialog()
{
#if wxUSE_PROGRESSDLG
    wxProgressDialog dialog("wxProgressDialog", "Progress smoke test",
        100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME);
    for ( int i = 0; i <= 100; i += 5 )
    {
        dialog.Update(i);
        wxMilliSleep(8);
    }
#else
    wxMessageBox("wxProgressDialog is disabled in this build.", "wxProgressDialog", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowFindDialog()
{
#if wxUSE_FINDREPLDLG
    if ( m_findDialog )
    {
        m_findDialog->Raise();
        return;
    }

    m_findDialog = new wxFindReplaceDialog(this, &m_findData,
        "wxFindReplaceDialog", wxFR_REPLACEDIALOG);
    m_findDialog->Bind(wxEVT_FIND_CLOSE, [this](wxFindDialogEvent&)
    {
        if ( m_findDialog )
        {
            m_findDialog->Destroy();
            m_findDialog = nullptr;
        }
    });
    m_findDialog->Show();
#else
    wxMessageBox("wxFindReplaceDialog is disabled in this build.", "wxFindReplaceDialog", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowWizardDialog()
{
#if wxUSE_WIZARDDLG
    wxWizard wizard(this, wxID_ANY, "wxWizard");
    wxWizardPageSimple* first = new wxWizardPageSimple(&wizard);
    wxWizardPageSimple* second = new wxWizardPageSimple(&wizard);
    wxWizardPageSimple::Chain(first, second);

    wxBoxSizer* firstSizer = new wxBoxSizer(wxVERTICAL);
    first->SetSizer(firstSizer);
    firstSizer->Add(new wxStaticText(first, wxID_ANY, "First wizard page."), 0, wxALL, FromDIP(12));

    wxBoxSizer* secondSizer = new wxBoxSizer(wxVERTICAL);
    second->SetSizer(secondSizer);
    secondSizer->Add(new wxStaticText(second, wxID_ANY, "Second wizard page."), 0, wxALL, FromDIP(12));

    wizard.GetPageAreaSizer()->Add(first);
    wizard.RunWizard(first);
#else
    wxMessageBox("wxWizard is disabled in this build.", "wxWizard", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowChoiceDialogs()
{
#if wxUSE_CHOICEDLG
    const wxString choices[] = { "Alpha", "Beta", "Gamma" };
    wxSingleChoiceDialog single(this, "Pick one item", "wxSingleChoiceDialog",
        WXSIZEOF(choices), choices);
    single.ShowModal();

    wxMultiChoiceDialog multi(this, "Pick multiple items", "wxMultiChoiceDialog",
        WXSIZEOF(choices), choices);
    multi.ShowModal();
#else
    wxMessageBox("Choice dialogs are disabled in this build.", "Choice dialogs", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowDirectoryDialog()
{
#if wxUSE_DIRDLG
    wxDirDialog dialog(this, "Choose a directory");
    dialog.ShowModal();
#else
    wxMessageBox("wxDirDialog is disabled in this build.", "wxDirDialog", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowTextDialogs()
{
#if wxUSE_TEXTDLG
    wxTextEntryDialog text(this, "Enter text", "wxTextEntryDialog", "value");
    text.ShowModal();
#endif
#if wxUSE_NUMBERDLG
    wxGetNumberFromUser("Enter a number", "Value:", "wxNumberEntryDialog", 5, 0, 100, this);
#endif
#if !wxUSE_TEXTDLG && !wxUSE_NUMBERDLG
    wxMessageBox("Text and number dialogs are disabled in this build.", "Text dialogs", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowRearrangeDialog()
{
#if wxUSE_REARRANGECTRL
    wxArrayString items;
    items.Add("First");
    items.Add("Second");
    items.Add("Third");
    wxArrayInt order;
    order.Add(0);
    order.Add(-1);
    order.Add(2);
    wxRearrangeDialog dialog(this, "Reorder and check items", "wxRearrangeDialog", order, items);
    dialog.ShowModal();
#else
    wxMessageBox("wxRearrangeDialog is disabled in this build.", "wxRearrangeDialog", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowNotification()
{
#if wxUSE_NOTIFICATION_MESSAGE
    wxNotificationMessage notification("wxNotificationMessage",
        "Notification smoke test from the showcase sample.", this, wxICON_INFORMATION);
    notification.Show();
#else
    wxMessageBox("wxNotificationMessage is disabled in this build.", "Notification", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowTooltipWindows()
{
#if wxUSE_RICHTOOLTIP
    wxRichToolTip richTip("wxRichToolTip", "Rich tooltip smoke test.");
    richTip.ShowFor(this);
#endif
#if wxUSE_TIPWINDOW
    if ( m_tipWindow )
    {
        m_tipWindow->Close();
    }
    else
    {
        m_tipWindow = wxTipWindow::New(this, "wxTipWindow smoke test.", FromDIP(180));
    }
#endif
#if !wxUSE_RICHTOOLTIP && !wxUSE_TIPWINDOW
    wxMessageBox("Tooltip windows are disabled in this build.", "Tooltips", wxOK | wxICON_INFORMATION, this);
#endif
}

void ShowcaseFrame::ShowSplash()
{
#if wxUSE_SPLASH
    wxBitmap bmp = MakeSwatchBitmap(this, wxColour(80, 140, 220));
    new wxSplashScreen(bmp,
        wxSPLASH_CENTRE_ON_PARENT | wxSPLASH_TIMEOUT,
        1400, this, wxID_ANY);
#else
    wxMessageBox("wxSplashScreen is disabled in this build.", "Splash", wxOK | wxICON_INFORMATION, this);
#endif
}

class ShowcaseApp : public wxApp
{
public:
    bool OnInit() override
    {
        if ( !wxApp::OnInit() )
            return false;

        ShowcaseFrame* frame = new ShowcaseFrame;
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(ShowcaseApp);
