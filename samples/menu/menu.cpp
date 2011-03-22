/////////////////////////////////////////////////////////////////////////////
// Name:        samples/menu.cpp
// Purpose:     wxMenu/wxMenuBar sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.11.99
// RCS-ID:      $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
    #include "wx/log.h"
    #include "wx/textctrl.h"
    #include "wx/textdlg.h"
#endif

#if !wxUSE_MENUS
    // nice try...
    #error "menu sample requires wxUSE_MENUS=1"
#endif // wxUSE_MENUS

// not all ports have support for EVT_CONTEXT_MENU yet, don't define
// USE_CONTEXT_MENU for those which don't
#if defined(__WXMOTIF__) || defined(__WXPM__) || defined(__WXX11__) || defined(__WXMGL__)
    #define USE_CONTEXT_MENU 0
#else
    #define USE_CONTEXT_MENU 1
#endif

// this sample is usefull when new port is developed
// and usually new port has majority of flags turned off
#if wxUSE_LOG && wxUSE_TEXTCTRL
    #define USE_LOG_WINDOW 1
#else
    #define USE_LOG_WINDOW 0
#endif

#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
#include "copy.xpm"
#endif

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit();
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
            LogMenuOpenOrClose(event, wxT("opened")); event.Skip();
#endif
        }
    void OnMenuClose(wxMenuEvent& event)
        {
#if USE_LOG_WINDOW
          LogMenuOpenOrClose(event, wxT("closed")); event.Skip();
#endif
       }

    void OnUpdateCheckMenuItemUI(wxUpdateUIEvent& event);

    void OnSize(wxSizeEvent& event);

private:
    void LogMenuOpenOrClose(const wxMenuEvent& event, const wxChar *what);
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

    // the previous log target
    wxLog *m_logOld;

    DECLARE_EVENT_TABLE()
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

    DECLARE_EVENT_TABLE()
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

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Menu_File_Quit,     MyFrame::OnQuit)
#if USE_LOG_WINDOW
    EVT_MENU(Menu_File_ClearLog, MyFrame::OnClearLog)
    EVT_UPDATE_UI(Menu_File_ClearLog, MyFrame::OnClearLogUpdateUI)
#endif

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

    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyEvtHandler, wxEvtHandler)
    EVT_MENU(wxID_ANY, MyEvtHandler::OnMenuEvent)
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
    MyFrame* frame = new MyFrame;

    frame->Show(true);

#if wxUSE_STATUSBAR
    frame->SetStatusText(wxT("Welcome to wxWidgets menu sample"));
#endif // wxUSE_STATUSBAR

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame()
       : wxFrame((wxFrame *)NULL, wxID_ANY, wxT("wxWidgets menu sample"))
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
    fileMenu->AppendSubMenu(stockSubMenu, wxT("&Standard items demo"));

#if USE_LOG_WINDOW
    wxMenuItem *item = new wxMenuItem(fileMenu, Menu_File_ClearLog,
                                      wxT("Clear &log\tCtrl-L"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    item->SetBitmap(copy_xpm);
#endif
    fileMenu->Append(item);
    fileMenu->AppendSeparator();
#endif // USE_LOG_WINDOW

    fileMenu->Append(Menu_File_Quit, wxT("E&xit\tAlt-X"), wxT("Quit menu sample"));

    wxMenu *menubarMenu = new wxMenu;
    menubarMenu->Append(Menu_MenuBar_Append, wxT("&Append menu\tCtrl-A"),
                        wxT("Append a menu to the menubar"));
    menubarMenu->Append(Menu_MenuBar_Insert, wxT("&Insert menu\tCtrl-I"),
                        wxT("Insert a menu into the menubar"));
    menubarMenu->Append(Menu_MenuBar_Delete, wxT("&Delete menu\tCtrl-D"),
                        wxT("Delete the last menu from the menubar"));
    menubarMenu->Append(Menu_MenuBar_Toggle, wxT("&Toggle menu\tCtrl-T"),
                        wxT("Toggle the first menu in the menubar"), true);
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_Enable, wxT("&Enable menu\tCtrl-E"),
                        wxT("Enable or disable the last menu"), true);
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_GetLabel, wxT("&Get menu label\tCtrl-G"),
                        wxT("Get the label of the last menu"));
#if wxUSE_TEXTDLG
    menubarMenu->Append(Menu_MenuBar_SetLabel, wxT("&Set menu label\tCtrl-S"),
                        wxT("Change the label of the last menu"));
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_FindMenu, wxT("&Find menu from label\tCtrl-F"),
                        wxT("Find a menu by searching for its label"));
#endif

    wxMenu* subMenu = new wxMenu;
    subMenu->Append(Menu_SubMenu_Normal, wxT("&Normal submenu item"), wxT("Disabled submenu item"));
    subMenu->AppendCheckItem(Menu_SubMenu_Check, wxT("&Check submenu item"), wxT("Check submenu item"));
    subMenu->AppendRadioItem(Menu_SubMenu_Radio1, wxT("Radio item &1"), wxT("Radio item"));
    subMenu->AppendRadioItem(Menu_SubMenu_Radio2, wxT("Radio item &2"), wxT("Radio item"));
    subMenu->AppendRadioItem(Menu_SubMenu_Radio3, wxT("Radio item &3"), wxT("Radio item"));

    menubarMenu->Append(Menu_SubMenu, wxT("Submenu"), subMenu);

    wxMenu *menuMenu = new wxMenu;
    menuMenu->Append(Menu_Menu_Append, wxT("&Append menu item\tAlt-A"),
                     wxT("Append a menu item to the 'Test' menu"));
    menuMenu->Append(Menu_Menu_AppendSub, wxT("&Append sub menu\tAlt-S"),
                     wxT("Append a sub menu to the 'Test' menu"));
    menuMenu->Append(Menu_Menu_Insert, wxT("&Insert menu item\tAlt-I"),
                     wxT("Insert a menu item in head of the 'Test' menu"));
    menuMenu->Append(Menu_Menu_Delete, wxT("&Delete menu item\tAlt-D"),
                     wxT("Delete the last menu item from the 'Test' menu"));
    menuMenu->Append(Menu_Menu_DeleteSub, wxT("Delete last &submenu\tAlt-K"),
                     wxT("Delete the last submenu from the 'Test' menu"));
    menuMenu->AppendSeparator();
    menuMenu->Append(Menu_Menu_Enable, wxT("&Enable menu item\tAlt-E"),
                     wxT("Enable or disable the last menu item"), true);
    menuMenu->Append(Menu_Menu_Check, wxT("&Check menu item\tAlt-C"),
                     wxT("Check or uncheck the last menu item"), true);
    menuMenu->AppendSeparator();
    menuMenu->Append(Menu_Menu_GetInfo, wxT("Get menu item in&fo\tAlt-F"),
                     wxT("Show the state of the last menu item"));
#if wxUSE_TEXTDLG
    menuMenu->Append(Menu_Menu_SetLabel, wxT("Set menu item label\tAlt-L"),
                     wxT("Set the label of a menu item"));
#endif
#if wxUSE_TEXTDLG
    menuMenu->AppendSeparator();
    menuMenu->Append(Menu_Menu_FindItem, wxT("Find menu item from label"),
                     wxT("Find a menu item by searching for its label"));
#endif

    wxMenu *testMenu = new wxMenu;
    testMenu->Append(Menu_Test_Normal, wxT("&Normal item"));
    testMenu->AppendSeparator();
    testMenu->AppendCheckItem(Menu_Test_Check, wxT("&Check item"));
    testMenu->AppendSeparator();
    testMenu->AppendRadioItem(Menu_Test_Radio1, wxT("Radio item &1"));
    testMenu->AppendRadioItem(Menu_Test_Radio2, wxT("Radio item &2"));
    testMenu->AppendRadioItem(Menu_Test_Radio3, wxT("Radio item &3"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Menu_Help_About, wxT("&About\tF1"), wxT("About menu sample"));

    wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(menubarMenu, wxT("Menu&bar"));
    menuBar->Append(menuMenu, wxT("&Menu"));
    menuBar->Append(testMenu, wxT("&Test"));
    menuBar->Append(helpMenu, wxT("&Help"));

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

    wxLogMessage(wxT("Brief explanations: the commands in the \"Menu\" menu ")
                 wxT("append/insert/delete items to/from the \"Test\" menu.\n")
                 wxT("The commands in the \"Menubar\" menu work with the ")
                 wxT("menubar itself.\n\n")
                 wxT("Right click the band below to test popup menus.\n"));
#endif
#ifdef __POCKETPC__
    EnableContextMenu();
#endif
}

MyFrame::~MyFrame()
{
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
    menu->Append(Menu_Dummy_First, wxT("&First item\tCtrl-F1"));
    menu->AppendSeparator();
    menu->AppendCheckItem(Menu_Dummy_Second, wxT("&Second item\tCtrl-F2"));

    if ( title )
    {
        title->Printf(wxT("Dummy menu &%u"), (unsigned)++m_countDummy);
    }

    return menu;
}

wxMenuItem *MyFrame::GetLastMenuItem() const
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_MSG( menu, NULL, wxT("no 'Test' menu?") );

    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetLast();
    if ( !node )
    {
        wxLogWarning(wxT("No last item in the 'Test' menu!"));

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

    wxString msg = wxString::Format(wxT("Menu command %d"), id);

    // catch all checkable menubar items and also the check item from the popup
    // menu
    wxMenuItem *item = GetMenuBar()->FindItem(id);
    if ( (item && item->IsCheckable()) || id == Menu_Popup_ToBeChecked )
    {
        msg += wxString::Format(wxT(" (the item is currently %schecked)"),
                                event.IsChecked() ? wxT("") : wxT("not "));
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

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox(wxT("wxWidgets menu sample\n(c) 1999-2001 Vadim Zeitlin"),
                       wxT("About wxWidgets menu sample"),
                       wxOK | wxICON_INFORMATION);
}

void MyFrame::OnDeleteMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();

    size_t count = mbar->GetMenuCount();
    if ( count == 4 )
    {
        // don't let delete the first 4 menus
        wxLogError(wxT("Can't delete any more menus"));
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
        mbar->Insert(0, m_menu, wxT("&File"));
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

    wxCHECK_RET( count, wxT("no last menu?") );

    wxLogMessage(wxT("The label of the last menu item is '%s'"),
                 mbar->GetMenuLabel(count - 1).c_str());
}

#if wxUSE_TEXTDLG
void MyFrame::OnSetLabelMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();
    size_t count = mbar->GetMenuCount();

    wxCHECK_RET( count, wxT("no last menu?") );

    wxString label = wxGetTextFromUser
                     (
                        wxT("Enter new label: "),
                        wxT("Change last menu text"),
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

    wxCHECK_RET( count, wxT("no last menu?") );

    wxString label = wxGetTextFromUser
                     (
                        wxT("Enter label to search for: "),
                        wxT("Find menu"),
                        wxEmptyString,
                        this
                     );

    if ( !label.empty() )
    {
        int index = mbar->FindMenu(label);

        if (index == wxNOT_FOUND)
        {
            wxLogWarning(wxT("No menu with label '%s'"), label.c_str());
        }
        else
        {
            wxLogMessage(wxT("Menu %d has label '%s'"), index, label.c_str());
        }
    }
}
#endif

void MyFrame::OnDummy(wxCommandEvent& event)
{
    wxLogMessage(wxT("Dummy item #%d"), event.GetId() - Menu_Dummy_First + 1);
}

void MyFrame::OnAppendMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, wxT("no 'Test' menu?") );

    menu->AppendSeparator();
    menu->Append(Menu_Dummy_Third, wxT("&Third dummy item\tCtrl-F3"),
                 wxT("Checkable item"), true);
}

void MyFrame::OnAppendSubMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, wxT("no 'Test' menu?") );

    menu->Append(Menu_Dummy_Last, wxT("&Dummy sub menu"),
                 CreateDummyMenu(NULL), wxT("Dummy sub menu help"));
}

void MyFrame::OnDeleteMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, wxT("no 'Test' menu?") );

    size_t count = menu->GetMenuItemCount();
    if ( !count )
    {
        wxLogWarning(wxT("No items to delete!"));
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
    wxCHECK_RET( menu, wxT("no 'Test' menu?") );

    for ( int n = menu->GetMenuItemCount() - 1; n >=0 ; --n )
    {
        wxMenuItem* item = menu->FindItemByPosition(n);
        if (item->IsSubMenu())
        {
            menu->Destroy(item);
            return;
        }
    }

    wxLogWarning(wxT("No submenu to delete!"));
}

void MyFrame::OnInsertMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->FindMenu("Test"));
    wxCHECK_RET( menu, wxT("no 'Test' menu?") );

    menu->Insert(0, wxMenuItem::New(menu, Menu_Dummy_Fourth,
                                    wxT("Fourth dummy item\tCtrl-F4")));
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
        wxLogMessage(wxT("The label of the last menu item is '%s'"),
                     label.c_str());
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
                            wxT("Enter new label: "),
                            wxT("Change last menu item text"),
                            item->GetItemLabel(),
                            this
                         );
        label.Replace( wxT("\\t"), wxT("\t") );

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
        msg << wxT("The item is ") << (item->IsEnabled() ? wxT("enabled")
                                                    : wxT("disabled"))
            << wxT('\n');

        if ( item->IsCheckable() )
        {
            msg << wxT("It is checkable and ") << (item->IsChecked() ? wxT("") : wxT("un"))
                << wxT("checked\n");
        }

#if wxUSE_ACCEL
        wxAcceleratorEntry *accel = item->GetAccel();
        if ( accel )
        {
            msg << wxT("Its accelerator is ");

            int flags = accel->GetFlags();
            if ( flags & wxACCEL_ALT )
                msg << wxT("Alt-");
            if ( flags & wxACCEL_CTRL )
                msg << wxT("Ctrl-");
            if ( flags & wxACCEL_SHIFT )
                msg << wxT("Shift-");

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
                    msg << wxT('F') << code - WXK_F1 + 1;
                    break;

                // if there are any other keys wxGetAccelFromString() may return,
                // we should process them here

                default:
                    if ( wxIsalnum(code) )
                    {
                        msg << (wxChar)code;

                        break;
                    }

                    wxFAIL_MSG( wxT("unknown keyboard accel") );
            }

            delete accel;
        }
        else
        {
            msg << wxT("It doesn't have an accelerator");
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

    wxCHECK_RET( count, wxT("no last menu?") );

    wxString label = wxGetTextFromUser
                     (
                        wxT("Enter label to search for: "),
                        wxT("Find menu item"),
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
            wxLogWarning(wxT("No menu item with label '%s'"), label.c_str());
        }
        else
        {
            wxLogMessage(wxT("Menu item %d in menu %lu has label '%s'"),
                         index, (unsigned long)menuindex, label.c_str());
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
        menu.Append(Menu_Help_About, wxT("&About"));
        menu.Append(Menu_Popup_Submenu, wxT("&Submenu"), CreateDummyMenu(NULL));
        menu.Append(Menu_Popup_ToBeDeleted, wxT("To be &deleted"));
        menu.AppendCheckItem(Menu_Popup_ToBeChecked, wxT("To be &checked"));
        menu.Append(Menu_Popup_ToBeGreyed, wxT("To be &greyed"),
                    wxT("This menu item should be initially greyed out"));
        menu.AppendSeparator();
        menu.Append(Menu_File_Quit, wxT("E&xit"));

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
    wxLogMessage(wxT("Normal item selected"));
}

void MyFrame::OnTestCheck(wxCommandEvent& event)
{
    wxLogMessage(wxT("Check item %schecked"),
                 event.IsChecked() ? wxT("") : wxT("un"));
}

void MyFrame::OnTestRadio(wxCommandEvent& event)
{
    wxLogMessage(wxT("Radio item %d selected"),
                 event.GetId() - Menu_Test_Radio1 + 1);
}

#if USE_LOG_WINDOW
void MyFrame::LogMenuOpenOrClose(const wxMenuEvent& event, const wxChar *what)
{
    wxString msg;
    msg << wxT("A ")
        << ( event.IsPopup() ? wxT("popup ") : wxT("") )
        << wxT("menu has been ")
        << what
        << wxT(".");

    wxLogStatus(this, msg.c_str());
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

