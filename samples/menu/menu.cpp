/////////////////////////////////////////////////////////////////////////////
// Name:        samples/menu.cpp
// Purpose:     wxMenu/wxMenuBar sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.11.99
// Copyright:   (c) 1999 Vadim Zeitlin
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
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/frame.h"
    #include "wx/image.h"
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
    #include "wx/log.h"
    #include "wx/textctrl.h"
    #include "wx/textdlg.h"
#endif

#include "wx/artprov.h"
#include "wx/filehistory.h"
#include "wx/filename.h"

#if !wxUSE_MENUS
    // nice try...
    #error "menu sample requires wxUSE_MENUS=1"
#endif // wxUSE_MENUS

// not all ports have support for EVT_CONTEXT_MENU yet, don't define
// USE_CONTEXT_MENU for those which don't
#if defined(__WXMOTIF__) || defined(__WXX11__)
    #define USE_CONTEXT_MENU 0
#else
    #define USE_CONTEXT_MENU 1
#endif

// this sample is useful when a new port is developed
// and usually a new port has majority of flags turned off
#if wxUSE_LOG && wxUSE_TEXTCTRL
    #define USE_LOG_WINDOW 1
#else
    #define USE_LOG_WINDOW 0
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame();

    virtual ~MyFrame();

    void LogMenuEvent(const wxCommandEvent& event);

protected:
    void OnQuit(wxCommandEvent& event);
#if USE_LOG_WINDOW
    void OnClearLog(wxCommandEvent& event);
    void OnClearLogUpdateUI(wxUpdateUIEvent& event);
#endif // USE_LOG_WINDOW
    void OnShowDialog(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

    void OnDummy(wxCommandEvent& event);

    void OnAppendMenuItem(wxCommandEvent& event);
    void OnAppendSubMenu(wxCommandEvent& event);
    void OnDeleteMenuItem(wxCommandEvent& event);
    void OnDeleteSubMenu(wxCommandEvent& event);
    void OnInsertMenuItem(wxCommandEvent& event);
    void OnCheckMenuItem(wxCommandEvent& event);
    void OnEnableMenuItem(wxCommandEvent& event);
    void OnGetLabelMenuItem(wxCommandEvent& event);
#if wxUSE_TEXTDLG
    void OnSetLabelMenuItem(wxCommandEvent& event);
#endif
    void OnGetMenuItemInfo(wxCommandEvent& event);
#if wxUSE_TEXTDLG
    void OnFindMenuItem(wxCommandEvent& event);
#endif

    void OnAppendMenu(wxCommandEvent& event);
    void OnInsertMenu(wxCommandEvent& event);
    void OnDeleteMenu(wxCommandEvent& event);
    void OnToggleMenu(wxCommandEvent& event);
    void OnEnableMenu(wxCommandEvent& event);
    void OnGetLabelMenu(wxCommandEvent& event);
    void OnSetLabelMenu(wxCommandEvent& event);
#if wxUSE_TEXTDLG
    void OnFindMenu(wxCommandEvent& event);
#endif

    void OnTestNormal(wxCommandEvent& event);
    void OnTestCheck(wxCommandEvent& event);
    void OnTestRadio(wxCommandEvent& event);

    void OnUpdateSubMenuNormal(wxUpdateUIEvent& event);
    void OnUpdateSubMenuCheck(wxUpdateUIEvent& event);
    void OnUpdateSubMenuRadio(wxUpdateUIEvent& event);

#if USE_CONTEXT_MENU
    void OnContextMenu(wxContextMenuEvent& event);
#else
    void OnRightUp(wxMouseEvent& event)
        { ShowContextMenu(event.GetPosition()); }
#endif

    void OnMenuOpen(wxMenuEvent& event)
        {
#if USE_LOG_WINDOW
            LogMenuOpenCloseOrHighlight(event, "opened"); event.Skip();
#endif
        }
    void OnMenuClose(wxMenuEvent& event)
        {
#if USE_LOG_WINDOW
          LogMenuOpenCloseOrHighlight(event, "closed"); event.Skip();
#endif
       }
    void OnMenuHighlight(wxMenuEvent& event)
        {
#if USE_LOG_WINDOW
          LogMenuOpenCloseOrHighlight(event, "highlighted"); event.Skip();
#endif
       }

    void OnUpdateCheckMenuItemUI(wxUpdateUIEvent& event);

    void OnSize(wxSizeEvent& event);

#if wxUSE_FILE_HISTORY
    void OnFileHistoryMenuItem(wxCommandEvent& event);

    void OnFileHistoryStyleItem(wxCommandEvent& event);
#endif

private:
#if USE_LOG_WINDOW
    void LogMenuOpenCloseOrHighlight(const wxMenuEvent& event, const wxString& what);
#endif
    void ShowContextMenu(const wxPoint& pos);

    wxMenu *CreateDummyMenu(wxString *title);

    wxMenuItem *GetLastMenuItem() const;

    // the menu previously detached from the menubar (may be NULL)
    wxMenu *m_menu;

    // the count of dummy menus already created
    size_t m_countDummy;

#if USE_LOG_WINDOW
    // the control used for logging
    wxTextCtrl *m_textctrl;
#endif

#if wxUSE_FILE_HISTORY
    wxMenu*        m_fileHistoryMenu;
    wxFileHistory* m_fileHistory;
#endif

    // the previous log target
    wxLog *m_logOld;

    wxDECLARE_EVENT_TABLE();
};

class MyDialog : public wxDialog
{
public:
    MyDialog(wxWindow* parent);

#if USE_CONTEXT_MENU
    void OnContextMenu(wxContextMenuEvent& event);
#else
    void OnRightUp(wxMouseEvent& event)
        { ShowContextMenu(event.GetPosition()); }
#endif

    void OnMenuOpen(wxMenuEvent& event)
        {
#if USE_LOG_WINDOW
            LogMenuOpenCloseOrHighlight(event, "opened"); event.Skip();
#endif
        }
    void OnMenuClose(wxMenuEvent& event)
        {
#if USE_LOG_WINDOW
          LogMenuOpenCloseOrHighlight(event, "closed"); event.Skip();
#endif
       }
    void OnMenuHighlight(wxMenuEvent& event)
        {
#if USE_LOG_WINDOW
          LogMenuOpenCloseOrHighlight(event, "highlighted"); event.Skip();
#endif
       }

private:
#if USE_LOG_WINDOW
    void LogMenuOpenCloseOrHighlight(const wxMenuEvent& event, const wxString& what);
#endif
    void ShowContextMenu(const wxPoint& pos);

#if USE_LOG_WINDOW
    // the control used for logging
    wxTextCtrl *m_textctrl;
#endif

    wxDECLARE_EVENT_TABLE();
};

// A small helper class which intercepts all menu events and logs them
class MyEvtHandler : public wxEvtHandler
{
public:
    MyEvtHandler(MyFrame *frame) { m_frame = frame; }

    void OnMenuEvent(wxCommandEvent& event)
    {
        m_frame->LogMenuEvent(event);

        event.Skip();
    }

private:
    MyFrame *m_frame;

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    Menu_File_Quit = wxID_EXIT,
#if USE_LOG_WINDOW
    Menu_File_ClearLog = 100,
#endif
    Menu_File_ShowDialog,

    Menu_MenuBar_Toggle = 200,
    Menu_MenuBar_Append,
    Menu_MenuBar_Insert,
    Menu_MenuBar_Delete,
    Menu_MenuBar_Enable,
    Menu_MenuBar_GetLabel,
#if wxUSE_TEXTDLG
    Menu_MenuBar_SetLabel,
    Menu_MenuBar_FindMenu,
#endif

    Menu_Menu_Append = 300,
    Menu_Menu_AppendSub,
    Menu_Menu_Insert,
    Menu_Menu_Delete,
    Menu_Menu_DeleteSub,
    Menu_Menu_Enable,
    Menu_Menu_Check,
    Menu_Menu_GetLabel,
#if wxUSE_TEXTDLG
    Menu_Menu_SetLabel,
#endif
    Menu_Menu_GetInfo,
#if wxUSE_TEXTDLG
    Menu_Menu_FindItem,
#endif
#if wxUSE_FILE_HISTORY
    Menu_Menu_FileHistory1,
    Menu_Menu_FileHistory2,
    Menu_Menu_FileHistory3,
#endif

    Menu_Test_Normal = 400,
    Menu_Test_Check,
    Menu_Test_Radio1,
    Menu_Test_Radio2,
    Menu_Test_Radio3,

    Menu_SubMenu = 450,
    Menu_SubMenu_Normal,
    Menu_SubMenu_Check,
    Menu_SubMenu_Radio1,
    Menu_SubMenu_Radio2,
    Menu_SubMenu_Radio3,

    Menu_Dummy_First = 500,
    Menu_Dummy_Second,
    Menu_Dummy_Third,
    Menu_Dummy_Fourth,
    Menu_Dummy_Last,

    Menu_Help_About = wxID_ABOUT,

    Menu_Popup_ToBeDeleted = 2000,
    Menu_Popup_ToBeGreyed,
    Menu_Popup_ToBeChecked,
    Menu_Popup_Submenu,

    Menu_PopupChoice,

    Menu_Max
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Menu_File_Quit,     MyFrame::OnQuit)
#if USE_LOG_WINDOW
    EVT_MENU(Menu_File_ClearLog, MyFrame::OnClearLog)
    EVT_UPDATE_UI(Menu_File_ClearLog, MyFrame::OnClearLogUpdateUI)
#endif
    EVT_MENU(Menu_File_ShowDialog, MyFrame::OnShowDialog)

    EVT_MENU(Menu_Help_About, MyFrame::OnAbout)

    EVT_MENU(Menu_MenuBar_Toggle,   MyFrame::OnToggleMenu)
    EVT_MENU(Menu_MenuBar_Append,   MyFrame::OnAppendMenu)
    EVT_MENU(Menu_MenuBar_Insert,   MyFrame::OnInsertMenu)
    EVT_MENU(Menu_MenuBar_Delete,   MyFrame::OnDeleteMenu)
    EVT_MENU(Menu_MenuBar_Enable,   MyFrame::OnEnableMenu)
    EVT_MENU(Menu_MenuBar_GetLabel, MyFrame::OnGetLabelMenu)
#if wxUSE_TEXTDLG
    EVT_MENU(Menu_MenuBar_SetLabel, MyFrame::OnSetLabelMenu)
    EVT_MENU(Menu_MenuBar_FindMenu, MyFrame::OnFindMenu)
#endif

    EVT_MENU(Menu_Menu_Append,    MyFrame::OnAppendMenuItem)
    EVT_MENU(Menu_Menu_AppendSub, MyFrame::OnAppendSubMenu)
    EVT_MENU(Menu_Menu_Insert,    MyFrame::OnInsertMenuItem)
    EVT_MENU(Menu_Menu_Delete,    MyFrame::OnDeleteMenuItem)
    EVT_MENU(Menu_Menu_DeleteSub, MyFrame::OnDeleteSubMenu)
    EVT_MENU(Menu_Menu_Enable,    MyFrame::OnEnableMenuItem)
    EVT_MENU(Menu_Menu_Check,     MyFrame::OnCheckMenuItem)
    EVT_MENU(Menu_Menu_GetLabel,  MyFrame::OnGetLabelMenuItem)
#if wxUSE_TEXTDLG
    EVT_MENU(Menu_Menu_SetLabel,  MyFrame::OnSetLabelMenuItem)
#endif
    EVT_MENU(Menu_Menu_GetInfo,   MyFrame::OnGetMenuItemInfo)
#if wxUSE_TEXTDLG
    EVT_MENU(Menu_Menu_FindItem,  MyFrame::OnFindMenuItem)
#endif

    EVT_MENU(Menu_Test_Normal,    MyFrame::OnTestNormal)
    EVT_MENU(Menu_Test_Check,     MyFrame::OnTestCheck)
    EVT_MENU(Menu_Test_Radio1,    MyFrame::OnTestRadio)
    EVT_MENU(Menu_Test_Radio2,    MyFrame::OnTestRadio)
    EVT_MENU(Menu_Test_Radio3,    MyFrame::OnTestRadio)

#if wxUSE_FILE_HISTORY
    EVT_MENU(wxID_FILE1,          MyFrame::OnFileHistoryMenuItem)
    EVT_MENU(wxID_FILE2,          MyFrame::OnFileHistoryMenuItem)
    EVT_MENU(wxID_FILE3,          MyFrame::OnFileHistoryMenuItem)

    EVT_MENU(Menu_Menu_FileHistory1,     MyFrame::OnFileHistoryStyleItem)
    EVT_MENU(Menu_Menu_FileHistory2,     MyFrame::OnFileHistoryStyleItem)
    EVT_MENU(Menu_Menu_FileHistory3,     MyFrame::OnFileHistoryStyleItem)
#endif

    EVT_UPDATE_UI(Menu_SubMenu_Normal,    MyFrame::OnUpdateSubMenuNormal)
    EVT_UPDATE_UI(Menu_SubMenu_Check,     MyFrame::OnUpdateSubMenuCheck)
    EVT_UPDATE_UI(Menu_SubMenu_Radio1,    MyFrame::OnUpdateSubMenuRadio)
    EVT_UPDATE_UI(Menu_SubMenu_Radio2,    MyFrame::OnUpdateSubMenuRadio)
    EVT_UPDATE_UI(Menu_SubMenu_Radio3,    MyFrame::OnUpdateSubMenuRadio)

    EVT_MENU_RANGE(Menu_Dummy_First, Menu_Dummy_Last, MyFrame::OnDummy)

    EVT_UPDATE_UI(Menu_Menu_Check, MyFrame::OnUpdateCheckMenuItemUI)

#if USE_CONTEXT_MENU
    EVT_CONTEXT_MENU(MyFrame::OnContextMenu)
#else
    EVT_RIGHT_UP(MyFrame::OnRightUp)
#endif

    EVT_MENU_OPEN(MyFrame::OnMenuOpen)
    EVT_MENU_CLOSE(MyFrame::OnMenuClose)
    EVT_MENU_HIGHLIGHT_ALL(MyFrame::OnMenuHighlight)

    EVT_SIZE(MyFrame::OnSize)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
#if USE_CONTEXT_MENU
    EVT_CONTEXT_MENU(MyDialog::OnContextMenu)
#else
    EVT_RIGHT_UP(MyDialog::OnRightUp)
#endif
    EVT_MENU_OPEN(MyDialog::OnMenuOpen)
    EVT_MENU_CLOSE(MyDialog::OnMenuClose)
    EVT_MENU_HIGHLIGHT_ALL(MyDialog::OnMenuHighlight)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(MyEvtHandler, wxEvtHandler)
    EVT_MENU(wxID_ANY, MyEvtHandler::OnMenuEvent)
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

    // Create the main frame window
    MyFrame* frame = new MyFrame;

    frame->Show(true);

#if wxUSE_STATUSBAR
    frame->SetStatusText("Welcome to wxWidgets menu sample");
#endif // wxUSE_STATUSBAR

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame()
       : wxFrame((wxFrame *)NULL, wxID_ANY, "wxWidgets menu sample")
{
    SetIcon(wxICON(sample));

#if USE_LOG_WINDOW
    m_textctrl = NULL;
#endif
    m_menu = NULL;
    m_countDummy = 0;
    m_logOld = NULL;

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    // create the menubar
    wxMenu *fileMenu = new wxMenu;

    wxMenu *stockSubMenu = new wxMenu;
    stockSubMenu->Append(wxID_ADD);
    stockSubMenu->Append(wxID_APPLY);
    stockSubMenu->Append(wxID_BACKWARD);
    stockSubMenu->Append(wxID_BOLD);
    stockSubMenu->Append(wxID_BOTTOM);
    stockSubMenu->Append(wxID_CANCEL);
    stockSubMenu->Append(wxID_CDROM);
    stockSubMenu->Append(wxID_CLEAR);
    stockSubMenu->Append(wxID_CLOSE);
    stockSubMenu->Append(wxID_CONVERT);
    stockSubMenu->Append(wxID_COPY);
    stockSubMenu->Append(wxID_CUT);
    stockSubMenu->Append(wxID_DELETE);
    stockSubMenu->Append(wxID_DOWN);
    stockSubMenu->Append(wxID_EXECUTE);
    stockSubMenu->Append(wxID_EXIT);
    stockSubMenu->Append(wxID_FIND);
    stockSubMenu->Append(wxID_FIRST);
    stockSubMenu->Append(wxID_FLOPPY);
    stockSubMenu->Append(wxID_FORWARD);
    stockSubMenu->Append(wxID_HARDDISK);
    stockSubMenu->Append(wxID_HELP);
    stockSubMenu->Append(wxID_HOME);
    stockSubMenu->Append(wxID_INDENT);
    stockSubMenu->Append(wxID_INDEX);
    stockSubMenu->Append(wxID_INFO);
    stockSubMenu->Append(wxID_ITALIC);
    stockSubMenu->Append(wxID_JUMP_TO);
    stockSubMenu->Append(wxID_JUSTIFY_CENTER);
    stockSubMenu->Append(wxID_JUSTIFY_FILL);
    stockSubMenu->Append(wxID_JUSTIFY_LEFT);
    stockSubMenu->Append(wxID_JUSTIFY_RIGHT);
    stockSubMenu->Append(wxID_LAST);
    stockSubMenu->Append(wxID_NETWORK);
    stockSubMenu->Append(wxID_NEW);
    stockSubMenu->Append(wxID_NO);
    stockSubMenu->Append(wxID_OK);
    stockSubMenu->Append(wxID_OPEN);
    stockSubMenu->Append(wxID_PASTE);
    stockSubMenu->Append(wxID_PREFERENCES);
    stockSubMenu->Append(wxID_PREVIEW);
    stockSubMenu->Append(wxID_PRINT);
    stockSubMenu->Append(wxID_PROPERTIES);
    stockSubMenu->Append(wxID_REDO);
    stockSubMenu->Append(wxID_REFRESH);
    stockSubMenu->Append(wxID_REMOVE);
    stockSubMenu->Append(wxID_REPLACE);
    stockSubMenu->Append(wxID_REVERT_TO_SAVED);
    stockSubMenu->Append(wxID_SAVE);
    stockSubMenu->Append(wxID_SAVEAS);
    stockSubMenu->Append(wxID_SELECT_COLOR);
    stockSubMenu->Append(wxID_SELECT_FONT);
    stockSubMenu->Append(wxID_SORT_ASCENDING);
    stockSubMenu->Append(wxID_SORT_DESCENDING);
    stockSubMenu->Append(wxID_SPELL_CHECK);
    stockSubMenu->Append(wxID_STOP);
    stockSubMenu->Append(wxID_STRIKETHROUGH);
    stockSubMenu->Append(wxID_TOP);
    stockSubMenu->Append(wxID_UNDELETE);
    stockSubMenu->Append(wxID_UNDERLINE);
    stockSubMenu->Append(wxID_UNDO);
    stockSubMenu->Append(wxID_UNINDENT);
    stockSubMenu->Append(wxID_UP);
    stockSubMenu->Append(wxID_YES);
    stockSubMenu->Append(wxID_ZOOM_100);
    stockSubMenu->Append(wxID_ZOOM_FIT);
    stockSubMenu->Append(wxID_ZOOM_IN);
    stockSubMenu->Append(wxID_ZOOM_OUT);
    fileMenu->AppendSubMenu(stockSubMenu, "&Standard items demo");

#if USE_LOG_WINDOW
    wxMenuItem *item = new wxMenuItem(fileMenu, Menu_File_ClearLog,
                                      "Clear &log\tCtrl-L");
    item->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_DELETE, wxART_MENU));
    fileMenu->Append(item);
    fileMenu->AppendSeparator();
#endif // USE_LOG_WINDOW

    fileMenu->Append(Menu_File_ShowDialog, "Show &Dialog\tCtrl-D",
                        "Show a dialog");
    fileMenu->AppendSeparator();

#if wxUSE_FILE_HISTORY
    m_fileHistoryMenu = new wxMenu();

    m_fileHistory = new wxFileHistory();
    m_fileHistory->UseMenu(m_fileHistoryMenu);

    m_fileHistory->AddFileToHistory( wxFileName("menu.cpp").GetAbsolutePath() );
    m_fileHistory->AddFileToHistory( wxFileName("Makefile.in").GetAbsolutePath() );
    m_fileHistory->AddFileToHistory( wxFileName("minimal", "minimal", "cpp").GetAbsolutePath() );

    fileMenu->AppendSubMenu(m_fileHistoryMenu, "Sample file history");
#endif

    fileMenu->Append(Menu_File_Quit, "E&xit\tAlt-X", "Quit menu sample");

    wxMenu *menubarMenu = new wxMenu;
    menubarMenu->Append(Menu_MenuBar_Append, "&Append menu\tCtrl-A",
                        "Append a menu to the menubar");
    menubarMenu->Append(Menu_MenuBar_Insert, "&Insert menu\tCtrl-I",
                        "Insert a menu into the menubar");
    menubarMenu->Append(Menu_MenuBar_Delete, "&Delete menu\tCtrl-D",
                        "Delete the last menu from the menubar");
    menubarMenu->Append(Menu_MenuBar_Toggle, "&Toggle menu\tCtrl-T",
                        "Toggle the first menu in the menubar", true);
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_Enable, "&Enable menu\tCtrl-E",
                        "Enable or disable the last menu", true);
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_GetLabel, "&Get menu label\tCtrl-G",
                        "Get the label of the last menu");
#if wxUSE_TEXTDLG
    menubarMenu->Append(Menu_MenuBar_SetLabel, "&Set menu label\tCtrl-S",
                        "Change the label of the last menu");
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_FindMenu, "&Find menu from label\tCtrl-F",
                        "Find a menu by searching for its label");
#endif

    wxMenu* subMenu = new wxMenu;
    subMenu->Append(Menu_SubMenu_Normal, "&Normal submenu item", "Disabled submenu item");
    subMenu->AppendCheckItem(Menu_SubMenu_Check, "&Check submenu item", "Check submenu item");
    subMenu->AppendRadioItem(Menu_SubMenu_Radio1, "Radio item &1", "Radio item");
    subMenu->AppendRadioItem(Menu_SubMenu_Radio2, "Radio item &2", "Radio item");
    subMenu->AppendRadioItem(Menu_SubMenu_Radio3, "Radio item &3", "Radio item");

    menubarMenu->Append(Menu_SubMenu, "Submenu", subMenu);

    wxMenu *menuMenu = new wxMenu;
    menuMenu->Append(Menu_Menu_Append, "&Append menu item\tAlt-A",
                     "Append a menu item to the 'Test' menu");
    menuMenu->Append(Menu_Menu_AppendSub, "&Append sub menu\tAlt-S",
                     "Append a sub menu to the 'Test' menu");
    menuMenu->Append(Menu_Menu_Insert, "&Insert menu item\tAlt-I",
                     "Insert a menu item in head of the 'Test' menu");
    menuMenu->Append(Menu_Menu_Delete, "&Delete menu item\tAlt-D",
                     "Delete the last menu item from the 'Test' menu");
    menuMenu->Append(Menu_Menu_DeleteSub, "Delete last &submenu\tAlt-K",
                     "Delete the last submenu from the 'Test' menu");
    menuMenu->AppendSeparator();
    menuMenu->Append(Menu_Menu_Enable, "&Enable menu item\tAlt-E",
                     "Enable or disable the last menu item", true);
    menuMenu->Append(Menu_Menu_Check, "&Check menu item\tAlt-C",
                     "Check or uncheck the last menu item", true);

    // Show the effect of Break(). As wxMSW is the only port in which calling
    // it actually does something, insert a separator under the other platforms.
#ifdef __WXMSW__
    menuMenu->Break();
#else
    menuMenu->AppendSeparator();
#endif

    menuMenu->Append(Menu_Menu_GetInfo, "Get menu item in&fo\tAlt-F",
                     "Show the state of the last menu item");
#if wxUSE_TEXTDLG
    menuMenu->Append(Menu_Menu_SetLabel, "Set menu item label\tAlt-L",
                     "Set the label of a menu item");
#endif
#if wxUSE_TEXTDLG
    menuMenu->AppendSeparator();
    menuMenu->Append(Menu_Menu_FindItem, "Find menu item from label",
                     "Find a menu item by searching for its label");
#endif
#if wxUSE_FILE_HISTORY
    wxMenu* menuFileHistoryStyle = new wxMenu();

    menuFileHistoryStyle->AppendRadioItem(Menu_Menu_FileHistory1, "Hide current path");
    menuFileHistoryStyle->AppendRadioItem(Menu_Menu_FileHistory2, "Hide all paths");
    menuFileHistoryStyle->AppendRadioItem(Menu_Menu_FileHistory3, "Show all paths");

    menuMenu->AppendSeparator();
    menuMenu->AppendSubMenu(menuFileHistoryStyle, "Select file history menu style");
#endif

    wxMenu *testMenu = new wxMenu;
    testMenu->Append(Menu_Test_Normal, "&Normal item");
    testMenu->AppendSeparator();
    testMenu->AppendCheckItem(Menu_Test_Check, "&Check item");

#ifdef __WXMSW__
#if wxUSE_IMAGE
    wxBitmap bmpUnchecked(8, 8);

    wxImage imageChecked(8, 8);
    imageChecked.Clear(0xff);
    wxBitmap bmpChecked(imageChecked);

    wxMenuItem *checkedBitmapItem = new wxMenuItem(testMenu, wxID_ANY,
        "Check item with bitmaps", "", wxITEM_CHECK);
    checkedBitmapItem->SetBitmaps(bmpChecked, bmpUnchecked);
    testMenu->Append(checkedBitmapItem);

    checkedBitmapItem = new wxMenuItem(testMenu, wxID_ANY,
        "Check item with bitmaps set afterwards", "", wxITEM_CHECK);
    testMenu->Append(checkedBitmapItem);
    checkedBitmapItem->SetBitmaps(bmpChecked, bmpUnchecked);

    checkedBitmapItem = new wxMenuItem(testMenu, wxID_ANY,
        "Check item with bitmaps set afterwards (initially checked)", "", wxITEM_CHECK);
    testMenu->Append(checkedBitmapItem);
    checkedBitmapItem->Check();
    checkedBitmapItem->SetBitmaps(bmpChecked, bmpUnchecked);
#endif
#endif

    testMenu->AppendSeparator();
    testMenu->AppendRadioItem(Menu_Test_Radio1, "Radio item &1");
    testMenu->AppendRadioItem(Menu_Test_Radio2, "Radio item &2");
    testMenu->AppendRadioItem(Menu_Test_Radio3, "Radio item &3");

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Menu_Help_About, "&About\tF1", "About menu sample");

    wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

    menuBar->Append(fileMenu, "&File");
    menuBar->Append(menubarMenu, "Menu&bar");
    menuBar->Append(menuMenu, "&Menu");
    menuBar->Append(testMenu, "&Test");
    menuBar->Append(helpMenu, "&Help");

    // these items should be initially checked
    menuBar->Check(Menu_MenuBar_Toggle, true);
    menuBar->Check(Menu_MenuBar_Enable, true);
    menuBar->Check(Menu_Menu_Enable, true);
    menuBar->Check(Menu_Menu_Check, false);

    // associate the menu bar with the frame
    SetMenuBar(menuBar);

    // intercept all menu events and log them in this custom event handler
    PushEventHandler(new MyEvtHandler(this));

#if USE_LOG_WINDOW
    // create the log text window
    m_textctrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE);
    m_textctrl->SetEditable(false);

    wxLog::DisableTimestamp();
    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_textctrl));

    wxLogMessage("Brief explanations: the commands in the \"Menu\" menu "
                 "append/insert/delete items to/from the \"Test\" menu.\n"
                 "The commands in the \"Menubar\" menu work with the "
                 "menubar itself.\n\n"
                 "Right click the band below to test popup menus.\n");
#endif
}

MyFrame::~MyFrame()
{
    delete m_fileHistory;
    delete m_menu;

    // delete the event handler installed in ctor
    PopEventHandler(true);

#if USE_LOG_WINDOW
    // restore old logger
    delete wxLog::SetActiveTarget(m_logOld);
#endif
}

wxMenu *MyFrame::CreateDummyMenu(wxString *title)
{
    wxMenu *menu = new wxMenu;
    menu->Append(Menu_Dummy_First, "&First item\tCtrl-F1");
    menu->AppendSeparator();
    menu->AppendCheckItem(Menu_Dummy_Second, "&Second item\tCtrl-F2");

    if ( title )
    {
        title->Printf("Dummy menu &%u", (unsigned)++m_countDummy);
    }

    return menu;
}

wxMenuItem *MyFrame::GetLastMenuItem() const
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_MSG( menu, NULL, "no 'Test' menu?" );

    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetLast();
    if ( !node )
    {
        wxLogWarning("No last item in the 'Test' menu!");

        return NULL;
    }
    else
    {
        return node->GetData();
    }
}

void MyFrame::LogMenuEvent(const wxCommandEvent& event)
{
    int id = event.GetId();

    wxString msg = wxString::Format("Menu command %d", id);

    // catch all checkable menubar items and also the check item from the popup
    // menu
    wxMenuItem *item = GetMenuBar()->FindItem(id);
    if ( (item && item->IsCheckable()) || id == Menu_Popup_ToBeChecked )
    {
        msg += wxString::Format(" (the item is currently %schecked)",
                                event.IsChecked() ? "" : "not ");
    }

    wxLogMessage(msg);
}

// ----------------------------------------------------------------------------
// menu callbacks
// ----------------------------------------------------------------------------

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

#if USE_LOG_WINDOW

void MyFrame::OnClearLog(wxCommandEvent& WXUNUSED(event))
{
    m_textctrl->Clear();
}

void MyFrame::OnClearLogUpdateUI(wxUpdateUIEvent& event)
{
    // if we only enable this item when the log window is empty, we never see
    // it in the disable state as a message is logged whenever the menu is
    // opened, so we disable it if there is not "much" text in the window
    event.Enable( m_textctrl->GetNumberOfLines() > 5 );
}

#endif // USE_LOG_WINDOW

void MyFrame::OnShowDialog(wxCommandEvent& WXUNUSED(event))
{
    MyDialog dlg(this);
    dlg.ShowModal();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox("wxWidgets menu sample\n(c) 1999-2001 Vadim Zeitlin",
                       "About wxWidgets menu sample",
                       wxOK | wxICON_INFORMATION);
}

void MyFrame::OnDeleteMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();

    size_t count = mbar->GetMenuCount();
    if ( count == 4 )
    {
        // don't let delete the first 4 menus
        wxLogError("Can't delete any more menus");
    }
    else
    {
        delete mbar->Remove(count - 1);
    }
}

void MyFrame::OnInsertMenu(wxCommandEvent& WXUNUSED(event))
{
    wxString title;
    wxMenu *menu = CreateDummyMenu(&title);
    // Insert before the 'Help' menu
    // Otherwise repeated Deletes will remove the 'Test' menu
    GetMenuBar()->Insert(4, menu, title);
}

void MyFrame::OnAppendMenu(wxCommandEvent& WXUNUSED(event))
{
    wxString title;
    wxMenu *menu = CreateDummyMenu(&title);
    GetMenuBar()->Append(menu, title);
}

void MyFrame::OnToggleMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();
    if ( !m_menu )
    {
        // hide the menu
        m_menu = mbar->Remove(0);
    }
    else
    {
        // restore it
        mbar->Insert(0, m_menu, "&File");
        m_menu = NULL;
    }
}

void MyFrame::OnEnableMenu(wxCommandEvent& event)
{
    wxMenuBar *mbar = GetMenuBar();
    size_t count = mbar->GetMenuCount();

    mbar->EnableTop(count - 1, event.IsChecked());
}

void MyFrame::OnGetLabelMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();
    size_t count = mbar->GetMenuCount();

    wxCHECK_RET( count, "no last menu?" );

    wxLogMessage("The label of the last menu item is '%s'",
                 mbar->GetMenuLabel(count - 1));
}

#if wxUSE_TEXTDLG
void MyFrame::OnSetLabelMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();
    size_t count = mbar->GetMenuCount();

    wxCHECK_RET( count, "no last menu?" );

    wxString label = wxGetTextFromUser
                     (
                        "Enter new label: ",
                        "Change last menu text",
                        mbar->GetMenuLabel(count - 1),
                        this
                     );

    if ( !label.empty() )
    {
        mbar->SetMenuLabel(count - 1, label);
    }
}

void MyFrame::OnFindMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();
    size_t count = mbar->GetMenuCount();

    wxCHECK_RET( count, "no last menu?" );

    wxString label = wxGetTextFromUser
                     (
                        "Enter label to search for: ",
                        "Find menu",
                        wxEmptyString,
                        this
                     );

    if ( !label.empty() )
    {
        int index = mbar->FindMenu(label);

        if (index == wxNOT_FOUND)
        {
            wxLogWarning("No menu with label '%s'", label);
        }
        else
        {
            wxLogMessage("Menu %d has label '%s'", index, label);
        }
    }
}
#endif

void MyFrame::OnDummy(wxCommandEvent& event)
{
    wxLogMessage("Dummy item #%d", event.GetId() - Menu_Dummy_First + 1);
}

void MyFrame::OnAppendMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, "no 'Test' menu?" );

    menu->AppendSeparator();
    menu->Append(Menu_Dummy_Third, "&Third dummy item\tCtrl-F3",
                 "Checkable item", true);
}

void MyFrame::OnAppendSubMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, "no 'Test' menu?" );

    menu->Append(Menu_Dummy_Last, "&Dummy sub menu",
                 CreateDummyMenu(NULL), "Dummy sub menu help");
}

void MyFrame::OnDeleteMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, "no 'Test' menu?" );

    size_t count = menu->GetMenuItemCount();
    if ( !count )
    {
        wxLogWarning("No items to delete!");
    }
    else
    {
        menu->Destroy(menu->GetMenuItems().Item(count - 1)->GetData());
    }
}

void MyFrame::OnDeleteSubMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, "no 'Test' menu?" );

    for ( int n = menu->GetMenuItemCount() - 1; n >=0 ; --n )
    {
        wxMenuItem* item = menu->FindItemByPosition(n);
        if (item->IsSubMenu())
        {
            menu->Destroy(item);
            return;
        }
    }

    wxLogWarning("No submenu to delete!");
}

void MyFrame::OnInsertMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, "no 'Test' menu?" );

    menu->Insert(0, wxMenuItem::New(menu, Menu_Dummy_Fourth,
                                    "Fourth dummy item\tCtrl-F4"));
    menu->Insert(1, wxMenuItem::New(menu, wxID_SEPARATOR));
}

void MyFrame::OnEnableMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuItem *item = GetLastMenuItem();

    if ( item )
    {
        item->Enable(!item->IsEnabled());
    }
}

void MyFrame::OnCheckMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuItem *item = GetLastMenuItem();

    if (item && item->IsCheckable())
    {
    item->Toggle();
}
}

void MyFrame::OnUpdateCheckMenuItemUI(wxUpdateUIEvent& event)
{
    wxLogNull nolog;

    wxMenuItem *item = GetLastMenuItem();

    event.Enable(item && item->IsCheckable());
}

void MyFrame::OnGetLabelMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuItem *item = GetLastMenuItem();

    if ( item )
    {
        wxString label = item->GetItemLabel();
        wxLogMessage("The label of the last menu item is '%s'",
                     label);
    }
}

#if wxUSE_TEXTDLG
void MyFrame::OnSetLabelMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuItem *item = GetLastMenuItem();

    if ( item )
    {
        wxString label = wxGetTextFromUser
                         (
                            "Enter new label: ",
                            "Change last menu item text",
                            item->GetItemLabel(),
                            this
                         );
        label.Replace( "\\t", "\t" );

        if ( !label.empty() )
        {
            item->SetItemLabel(label);
        }
    }
}
#endif

void MyFrame::OnGetMenuItemInfo(wxCommandEvent& WXUNUSED(event))
{
    wxMenuItem *item = GetLastMenuItem();

    if ( item )
    {
        wxString msg;
        msg << "The item is " << (item->IsEnabled() ? "enabled"
                                                    : "disabled")
            << '\n';

        if ( item->IsCheckable() )
        {
            msg << "It is checkable and " << (item->IsChecked() ? "" : "un")
                << "checked\n";
        }

#if wxUSE_ACCEL
        wxAcceleratorEntry *accel = item->GetAccel();
        if ( accel )
        {
            msg << "Its accelerator is ";

            int flags = accel->GetFlags();
            if ( flags & wxACCEL_ALT )
                msg << "Alt-";
            if ( flags & wxACCEL_CTRL )
                msg << "Ctrl-";
            if ( flags & wxACCEL_SHIFT )
                msg << "Shift-";

            int code = accel->GetKeyCode();
            switch ( code )
            {
                case WXK_F1:
                case WXK_F2:
                case WXK_F3:
                case WXK_F4:
                case WXK_F5:
                case WXK_F6:
                case WXK_F7:
                case WXK_F8:
                case WXK_F9:
                case WXK_F10:
                case WXK_F11:
                case WXK_F12:
                    msg << 'F' << code - WXK_F1 + 1;
                    break;

                // if there are any other keys wxGetAccelFromString() may return,
                // we should process them here

                default:
                    if ( wxIsalnum(code) )
                    {
                        msg << (wxChar)code;

                        break;
                    }

                    wxFAIL_MSG( "unknown keyboard accel" );
            }

            delete accel;
        }
        else
        {
            msg << "It doesn't have an accelerator";
        }
#endif // wxUSE_ACCEL

        wxLogMessage(msg);
    }
}

#if wxUSE_TEXTDLG
void MyFrame::OnFindMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();
    size_t count = mbar->GetMenuCount();

    wxCHECK_RET( count, "no last menu?" );

    wxString label = wxGetTextFromUser
                     (
                        "Enter label to search for: ",
                        "Find menu item",
                        wxEmptyString,
                        this
                     );

    if ( !label.empty() )
    {
        size_t menuindex;
        int index = wxNOT_FOUND;

        for (menuindex = 0; (menuindex < count) && (index == wxNOT_FOUND); ++menuindex)
        {
            index = mbar->FindMenuItem(mbar->GetMenu(menuindex)->GetTitle(), label);
        }
        if (index == wxNOT_FOUND)
        {
            wxLogWarning("No menu item with label '%s'", label);
        }
        else
        {
            wxLogMessage("Menu item %d in menu %lu has label '%s'",
                         index, (unsigned long)menuindex, label);
        }
    }
}
#endif

void MyFrame::ShowContextMenu(const wxPoint& pos)
{
    wxMenu menu;

    if ( wxGetKeyState(WXK_SHIFT) )
    {
        // when Shift is pressed, demonstrate the use of a simple function
        // returning the id of the item selected in the popup menu
        menu.SetTitle("Choose one of:");
        static const char *choices[] = { "Apple", "Banana", "Cherry" };
        for ( size_t n = 0; n < WXSIZEOF(choices); n++ )
            menu.Append(Menu_PopupChoice + n, choices[n]);

        const int rc = GetPopupMenuSelectionFromUser(menu, pos);
        if ( rc == wxID_NONE )
        {
            wxLogMessage("No selection");
        }
        else
        {
            wxLogMessage("You have selected \"%s\"",
                         choices[rc - Menu_PopupChoice]);
        }
    }
    else // normal case, shift not pressed
    {
        menu.Append(Menu_Help_About, "&About");
        menu.Append(Menu_Popup_Submenu, "&Submenu", CreateDummyMenu(NULL));
        menu.Append(Menu_Popup_ToBeDeleted, "To be &deleted");
        menu.AppendCheckItem(Menu_Popup_ToBeChecked, "To be &checked");
        menu.Append(Menu_Popup_ToBeGreyed, "To be &greyed",
                    "This menu item should be initially greyed out");
        menu.AppendSeparator();
        menu.Append(Menu_File_Quit, "E&xit")
            ->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_QUIT, wxART_MENU));

        menu.Delete(Menu_Popup_ToBeDeleted);
        menu.Check(Menu_Popup_ToBeChecked, true);
        menu.Enable(Menu_Popup_ToBeGreyed, false);

        PopupMenu(&menu, pos);

        // test for destroying items in popup menus
#if 0 // doesn't work in wxGTK!
        menu.Destroy(Menu_Popup_Submenu);

        PopupMenu( &menu, event.GetX(), event.GetY() );
#endif // 0
    }
}

void MyFrame::OnTestNormal(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Normal item selected");
}

void MyFrame::OnTestCheck(wxCommandEvent& event)
{
    wxLogMessage("Check item %schecked",
                 event.IsChecked() ? "" : "un");
}

void MyFrame::OnTestRadio(wxCommandEvent& event)
{
    wxLogMessage("Radio item %d selected",
                 event.GetId() - Menu_Test_Radio1 + 1);
}

#if USE_LOG_WINDOW
void MyFrame::LogMenuOpenCloseOrHighlight(const wxMenuEvent& event, const wxString& what)
{
    wxString msg;
    msg << "A "
        << ( event.IsPopup() ? "popup " : "" )
        << "menu has been "
        << what;

    if ( event.GetEventType() == wxEVT_MENU_HIGHLIGHT )
    {
        msg << " (id=" << event.GetId() << " in ";
    }
    else
    {
        msg << " (";
    }

    wxMenu* const menu = event.GetMenu();
    if ( menu )
    {
        msg << "menu with title \"" << menu->GetTitle() << "\")";
    }
    else
    {
        msg << "no menu provided)";
    }

    msg << ".";

    wxLogStatus(this, msg);
}
#endif

void MyFrame::OnUpdateSubMenuNormal(wxUpdateUIEvent& event)
{
    event.Enable(false);
}

void MyFrame::OnUpdateSubMenuCheck(wxUpdateUIEvent& event)
{
    event.Enable(true);
}

void MyFrame::OnUpdateSubMenuRadio(wxUpdateUIEvent& event)
{
    int which = (event.GetId() - Menu_SubMenu_Radio1 + 1);
    if (which == 2)
        event.Check(true);
    else
        event.Check(false);
}

#if USE_CONTEXT_MENU
void MyFrame::OnContextMenu(wxContextMenuEvent& event)
{
    wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1) {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    } else {
        point = ScreenToClient(point);
    }
    ShowContextMenu(point);
}
#endif

void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
#if USE_LOG_WINDOW
    if ( !m_textctrl )
        return;

    // leave a band below for popup menu testing
    wxSize size = GetClientSize();
    m_textctrl->SetSize(0, 0, size.x, (3*size.y)/4);
#endif

    // this is really ugly but we have to do it as we can't just call
    // event.Skip() because wxFrameBase would make the text control fill the
    // entire frame then
#ifdef __WXUNIVERSAL__
    PositionMenuBar();
#endif // __WXUNIVERSAL__
}

#if wxUSE_FILE_HISTORY
void MyFrame::OnFileHistoryMenuItem(wxCommandEvent& event)
{
    int eventID = event.GetId();

    wxString fname = m_fileHistory->GetHistoryFile(eventID - wxID_FILE1);

    wxMessageBox(wxString::Format("Selected file %s", fname), "File history activated",
                 wxOK | wxICON_INFORMATION);

    m_fileHistory->AddFileToHistory(fname);
}

void MyFrame::OnFileHistoryStyleItem(wxCommandEvent& event)
{
    switch( event.GetId() )
    {
    case Menu_Menu_FileHistory1:
        m_fileHistory->SetMenuPathStyle(wxFH_PATH_SHOW_IF_DIFFERENT);
        break;
    case Menu_Menu_FileHistory2:
        m_fileHistory->SetMenuPathStyle(wxFH_PATH_SHOW_NEVER);
        break;
    case Menu_Menu_FileHistory3:
        m_fileHistory->SetMenuPathStyle(wxFH_PATH_SHOW_ALWAYS);
        break;
    }
}
#endif

// ----------------------------------------------------------------------------
// MyDialog
// ----------------------------------------------------------------------------

MyDialog::MyDialog(wxWindow* parent)
    :    wxDialog(parent, wxID_ANY, "Test Dialog")
{
#if USE_LOG_WINDOW
    // create the log text window
    m_textctrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE);
    m_textctrl->SetEditable(false);

    m_textctrl->AppendText("Dialogs do not have menus, but popup menus should function the same\n\n"
                 "Right click this text ctrl to test popup menus.\n");
#endif
}

#if USE_LOG_WINDOW
void MyDialog::LogMenuOpenCloseOrHighlight(const wxMenuEvent& event, const wxString& what)
{
    wxString msg;
    msg << "A "
        << ( event.IsPopup() ? "popup " : "" )
        << "menu has been "
        << what;
    if ( event.GetEventType() == wxEVT_MENU_HIGHLIGHT )
    {
        msg << " (id=" << event.GetId() << ")";
    }
    msg << ".\n";

    m_textctrl->AppendText(msg);
}
#endif // USE_LOG_WINDOW
#if USE_CONTEXT_MENU
void MyDialog::OnContextMenu(wxContextMenuEvent& event)
{
    wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1) {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    } else {
        point = ScreenToClient(point);
    }
    ShowContextMenu(point);
}
#endif

void MyDialog::ShowContextMenu(const wxPoint& pos)
{
    wxMenu menu;

    menu.Append(Menu_Help_About, "&About");
    menu.Append(Menu_Popup_ToBeDeleted, "To be &deleted");
    menu.AppendCheckItem(Menu_Popup_ToBeChecked, "To be &checked");
    menu.Append(Menu_Popup_ToBeGreyed, "To be &greyed",
                "This menu item should be initially greyed out");
    menu.AppendSeparator();
    menu.Append(Menu_File_Quit, "E&xit");

    menu.Delete(Menu_Popup_ToBeDeleted);
    menu.Check(Menu_Popup_ToBeChecked, true);
    menu.Enable(Menu_Popup_ToBeGreyed, false);

    PopupMenu(&menu, pos);
}
