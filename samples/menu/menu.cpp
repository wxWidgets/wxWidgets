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
#endif

    void OnAbout(wxCommandEvent& event);

    void OnDummy(wxCommandEvent& event);

    void OnAppendMenuItem(wxCommandEvent& event);
    void OnAppendSubMenu(wxCommandEvent& event);
    void OnDeleteMenuItem(wxCommandEvent& event);
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
            LogMenuOpenOrClose(event, _T("opened")); event.Skip();
#endif
        }
    void OnMenuClose(wxMenuEvent& event)
        {
#if USE_LOG_WINDOW
          LogMenuOpenOrClose(event, _T("closed")); event.Skip();
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
    Menu_File_ClearLog,
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

    Menu_Max
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Menu_File_Quit,     MyFrame::OnQuit)
#if USE_LOG_WINDOW
    EVT_MENU(Menu_File_ClearLog, MyFrame::OnClearLog)
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
    // Create the main frame window
    MyFrame* frame = new MyFrame;

    frame->Show(true);

#if wxUSE_STATUSBAR
    frame->SetStatusText(_T("Welcome to wxWidgets menu sample"));
#endif // wxUSE_STATUSBAR

    SetTopWindow(frame);

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame()
       : wxFrame((wxFrame *)NULL, wxID_ANY, _T("wxWidgets menu sample"))
{
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
    stockSubMenu->Append(wxID_BOLD);
    stockSubMenu->Append(wxID_CANCEL);
    stockSubMenu->Append(wxID_CLEAR);
    stockSubMenu->Append(wxID_CLOSE);
    stockSubMenu->Append(wxID_COPY);
    stockSubMenu->Append(wxID_CUT);
    stockSubMenu->Append(wxID_DELETE);
    stockSubMenu->Append(wxID_FIND);
    stockSubMenu->Append(wxID_REPLACE);
    stockSubMenu->Append(wxID_BACKWARD);
    stockSubMenu->Append(wxID_DOWN);
    stockSubMenu->Append(wxID_FORWARD);
    stockSubMenu->Append(wxID_UP);
    stockSubMenu->Append(wxID_HELP);
    stockSubMenu->Append(wxID_HOME);
    stockSubMenu->Append(wxID_INDENT);
    stockSubMenu->Append(wxID_INDEX);
    stockSubMenu->Append(wxID_ITALIC);
    stockSubMenu->Append(wxID_JUSTIFY_CENTER);
    stockSubMenu->Append(wxID_JUSTIFY_FILL);
    stockSubMenu->Append(wxID_JUSTIFY_LEFT);
    stockSubMenu->Append(wxID_JUSTIFY_RIGHT);
    stockSubMenu->Append(wxID_NEW);
    stockSubMenu->Append(wxID_NO);
    stockSubMenu->Append(wxID_OK);
    stockSubMenu->Append(wxID_OPEN);
    stockSubMenu->Append(wxID_PASTE);
    stockSubMenu->Append(wxID_PREFERENCES);
    stockSubMenu->Append(wxID_PRINT);
    stockSubMenu->Append(wxID_PREVIEW);
    stockSubMenu->Append(wxID_PROPERTIES);
    stockSubMenu->Append(wxID_EXIT);
    stockSubMenu->Append(wxID_REDO);
    stockSubMenu->Append(wxID_REFRESH);
    stockSubMenu->Append(wxID_REMOVE);
    stockSubMenu->Append(wxID_REVERT_TO_SAVED);
    stockSubMenu->Append(wxID_SAVE);
    stockSubMenu->Append(wxID_SAVEAS);
    stockSubMenu->Append(wxID_STOP);
    stockSubMenu->Append(wxID_UNDELETE);
    stockSubMenu->Append(wxID_UNDERLINE);
    stockSubMenu->Append(wxID_UNDO);
    stockSubMenu->Append(wxID_UNINDENT);
    stockSubMenu->Append(wxID_YES);
    stockSubMenu->Append(wxID_ZOOM_100);
    stockSubMenu->Append(wxID_ZOOM_FIT);
    stockSubMenu->Append(wxID_ZOOM_IN);
    stockSubMenu->Append(wxID_ZOOM_OUT);
    fileMenu->AppendSubMenu(stockSubMenu, _T("&Standard items demo"));

#if USE_LOG_WINDOW
    wxMenuItem *item = new wxMenuItem(fileMenu, Menu_File_ClearLog,
                                      _T("Clear &log\tCtrl-L"));
#if wxUSE_OWNER_DRAWN || defined(__WXGTK__)
    item->SetBitmap(copy_xpm);
#endif
    fileMenu->Append(item);
    fileMenu->AppendSeparator();
#endif // USE_LOG_WINDOW

    fileMenu->Append(Menu_File_Quit, _T("E&xit\tAlt-X"), _T("Quit menu sample"));

    wxMenu *menubarMenu = new wxMenu;
    menubarMenu->Append(Menu_MenuBar_Append, _T("&Append menu\tCtrl-A"),
                        _T("Append a menu to the menubar"));
    menubarMenu->Append(Menu_MenuBar_Insert, _T("&Insert menu\tCtrl-I"),
                        _T("Insert a menu into the menubar"));
    menubarMenu->Append(Menu_MenuBar_Delete, _T("&Delete menu\tCtrl-D"),
                        _T("Delete the last menu from the menubar"));
    menubarMenu->Append(Menu_MenuBar_Toggle, _T("&Toggle menu\tCtrl-T"),
                        _T("Toggle the first menu in the menubar"), true);
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_Enable, _T("&Enable menu\tCtrl-E"),
                        _T("Enable or disable the last menu"), true);
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_GetLabel, _T("&Get menu label\tCtrl-G"),
                        _T("Get the label of the last menu"));
#if wxUSE_TEXTDLG
    menubarMenu->Append(Menu_MenuBar_SetLabel, _T("&Set menu label\tCtrl-S"),
                        _T("Change the label of the last menu"));
    menubarMenu->AppendSeparator();
    menubarMenu->Append(Menu_MenuBar_FindMenu, _T("&Find menu from label\tCtrl-F"),
                        _T("Find a menu by searching for its label"));
#endif

    wxMenu* subMenu = new wxMenu;
    subMenu->Append(Menu_SubMenu_Normal, _T("&Normal submenu item"), _T("Disabled submenu item"));
    subMenu->AppendCheckItem(Menu_SubMenu_Check, _T("&Check submenu item"), _T("Check submenu item"));
    subMenu->AppendRadioItem(Menu_SubMenu_Radio1, _T("Radio item &1"), _T("Radio item"));
    subMenu->AppendRadioItem(Menu_SubMenu_Radio2, _T("Radio item &2"), _T("Radio item"));
    subMenu->AppendRadioItem(Menu_SubMenu_Radio3, _T("Radio item &3"), _T("Radio item"));

    menubarMenu->Append(Menu_SubMenu, _T("Submenu"), subMenu);

    wxMenu *menuMenu = new wxMenu;
    menuMenu->Append(Menu_Menu_Append, _T("&Append menu item\tAlt-A"),
                     _T("Append a menu item to the last menu"));
    menuMenu->Append(Menu_Menu_AppendSub, _T("&Append sub menu\tAlt-S"),
                     _T("Append a sub menu to the last menu"));
    menuMenu->Append(Menu_Menu_Insert, _T("&Insert menu item\tAlt-I"),
                     _T("Insert a menu item in head of the last menu"));
    menuMenu->Append(Menu_Menu_Delete, _T("&Delete menu item\tAlt-D"),
                     _T("Delete the last menu item from the last menu"));
    menuMenu->AppendSeparator();
    menuMenu->Append(Menu_Menu_Enable, _T("&Enable menu item\tAlt-E"),
                     _T("Enable or disable the last menu item"), true);
    menuMenu->Append(Menu_Menu_Check, _T("&Check menu item\tAlt-C"),
                     _T("Check or uncheck the last menu item"), true);
    menuMenu->AppendSeparator();
    menuMenu->Append(Menu_Menu_GetInfo, _T("Get menu item in&fo\tAlt-F"),
                     _T("Show the state of the last menu item"));
#if wxUSE_TEXTDLG
    menuMenu->Append(Menu_Menu_SetLabel, _T("Set menu item label\tAlt-L"),
                     _T("Set the label of a menu item"));
#endif
#if wxUSE_TEXTDLG
    menuMenu->AppendSeparator();
    menuMenu->Append(Menu_Menu_FindItem, _T("Find menu item from label"),
                     _T("Find a menu item by searching for its label"));
#endif

    wxMenu *testMenu = new wxMenu;
    testMenu->Append(Menu_Test_Normal, _T("&Normal item"));
    testMenu->AppendSeparator();
    testMenu->AppendCheckItem(Menu_Test_Check, _T("&Check item"));
    testMenu->AppendSeparator();
    testMenu->AppendRadioItem(Menu_Test_Radio1, _T("Radio item &1"));
    testMenu->AppendRadioItem(Menu_Test_Radio2, _T("Radio item &2"));
    testMenu->AppendRadioItem(Menu_Test_Radio3, _T("Radio item &3"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Menu_Help_About, _T("&About\tF1"), _T("About menu sample"));

    wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(menubarMenu, _T("Menu&bar"));
    menuBar->Append(menuMenu, _T("&Menu"));
    menuBar->Append(testMenu, _T("&Test"));
    menuBar->Append(helpMenu, _T("&Help"));

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

    wxLog::SetTimestamp(NULL);
    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_textctrl));

    wxLogMessage(_T("Brief explanations: the commands or the \"Menu\" menu ")
                 _T("append/insert/delete items to/from the last menu.\n")
                 _T("The commands from \"Menubar\" menu work with the ")
                 _T("menubar itself.\n\n")
                 _T("Right click the band below to test popup menus.\n"));
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
    menu->Append(Menu_Dummy_First, _T("&First item\tCtrl-F1"));
    menu->AppendSeparator();
    menu->AppendCheckItem(Menu_Dummy_Second, _T("&Second item\tCtrl-F2"));

    if ( title )
    {
        title->Printf(_T("Dummy menu &%u"), (unsigned)++m_countDummy);
    }

    return menu;
}

wxMenuItem *MyFrame::GetLastMenuItem() const
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->GetMenuCount() - 1);

    wxMenuItemList::compatibility_iterator node = menu->GetMenuItems().GetLast();
    if ( !node )
    {
        wxLogWarning(_T("No last item in the last menu!"));

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

    wxString msg = wxString::Format(_T("Menu command %d"), id);

    // catch all checkable menubar items and also the check item from the popup
    // menu
    wxMenuItem *item = GetMenuBar()->FindItem(id);
    if ( (item && item->IsCheckable()) || id == Menu_Popup_ToBeChecked )
    {
        msg += wxString::Format(_T(" (the item is currently %schecked)"),
                                event.IsChecked() ? _T("") : _T("not "));
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
#endif

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox(_T("wxWidgets menu sample\n(c) 1999-2001 Vadim Zeitlin"),
                       _T("About wxWidgets menu sample"),
                       wxICON_INFORMATION);
}

void MyFrame::OnDeleteMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();

    size_t count = mbar->GetMenuCount();
    if ( count == 2 )
    {
        // don't let delete the first 2 menus
        wxLogError(_T("Can't delete any more menus"));
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
    GetMenuBar()->Insert(0, menu, title);
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
        mbar->Insert(0, m_menu, _T("&File"));
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

    wxCHECK_RET( count, _T("no last menu?") );

    wxLogMessage(_T("The label of the last menu item is '%s'"),
                 mbar->GetLabelTop(count - 1).c_str());
}

#if wxUSE_TEXTDLG
void MyFrame::OnSetLabelMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();
    size_t count = mbar->GetMenuCount();

    wxCHECK_RET( count, _T("no last menu?") );

    wxString label = wxGetTextFromUser
                     (
                        _T("Enter new label: "),
                        _T("Change last menu text"),
                        mbar->GetLabelTop(count - 1),
                        this
                     );

    if ( !label.empty() )
    {
        mbar->SetLabelTop(count - 1, label);
    }
}

void MyFrame::OnFindMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *mbar = GetMenuBar();
    size_t count = mbar->GetMenuCount();

    wxCHECK_RET( count, _T("no last menu?") );

    wxString label = wxGetTextFromUser
                     (
                        _T("Enter label to search for: "),
                        _T("Find menu"),
                        wxEmptyString,
                        this
                     );

    if ( !label.empty() )
    {
        int index = mbar->FindMenu(label);

        if (index == wxNOT_FOUND)
        {
            wxLogWarning(_T("No menu with label '%s'"), label.c_str());
        }
        else
        {
            wxLogMessage(_T("Menu %d has label '%s'"), index, label.c_str());
        }
    }
}
#endif

void MyFrame::OnDummy(wxCommandEvent& event)
{
    wxLogMessage(_T("Dummy item #%d"), event.GetId() - Menu_Dummy_First + 1);
}

void MyFrame::OnAppendMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->GetMenuCount() - 1);

    menu->AppendSeparator();
    menu->Append(Menu_Dummy_Third, _T("&Third dummy item\tCtrl-F3"),
                 _T("Checkable item"), true);
}

void MyFrame::OnAppendSubMenu(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();

    wxMenu *menu = menubar->GetMenu(menubar->GetMenuCount() - 2);

    menu->Append(Menu_Dummy_Last, _T("&Dummy sub menu"),
                 CreateDummyMenu(NULL), _T("Dummy sub menu help"));
}

void MyFrame::OnDeleteMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->GetMenuCount() - 1);

    size_t count = menu->GetMenuItemCount();
    if ( !count )
    {
        wxLogWarning(_T("No items to delete!"));
    }
    else
    {
        menu->Destroy(menu->GetMenuItems().Item(count - 1)->GetData());
    }
}

void MyFrame::OnInsertMenuItem(wxCommandEvent& WXUNUSED(event))
{
    wxMenuBar *menubar = GetMenuBar();
    wxMenu *menu = menubar->GetMenu(menubar->GetMenuCount() - 1);

    menu->Insert(0, wxMenuItem::New(menu, Menu_Dummy_Fourth,
                                    _T("Fourth dummy item\tCtrl-F4")));
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

    item->Toggle();
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
        wxLogMessage(_T("The label of the last menu item is '%s'"),
                     item->GetLabel().c_str());
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
                            _T("Enter new label: "),
                            _T("Change last menu item text"),
                            item->GetLabel(),
                            this
                         );
        label.Replace( _T("\\t"), _T("\t") );

        if ( !label.empty() )
        {
            item->SetText(label);
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
        msg << _T("The item is ") << (item->IsEnabled() ? _T("enabled")
                                                    : _T("disabled"))
            << _T('\n');

        if ( item->IsCheckable() )
        {
            msg << _T("It is checkable and ") << (item->IsChecked() ? _T("") : _T("un"))
                << _T("checked\n");
        }

#if wxUSE_ACCEL
        wxAcceleratorEntry *accel = item->GetAccel();
        if ( accel )
        {
            msg << _T("Its accelerator is ");

            int flags = accel->GetFlags();
            if ( flags & wxACCEL_ALT )
                msg << _T("Alt-");
            if ( flags & wxACCEL_CTRL )
                msg << _T("Ctrl-");
            if ( flags & wxACCEL_SHIFT )
                msg << _T("Shift-");

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
                    msg << _T('F') << code - WXK_F1 + 1;
                    break;

                // if there are any other keys wxGetAccelFromString() may return,
                // we should process them here

                default:
                    if ( wxIsalnum(code) )
                    {
                        msg << (wxChar)code;

                        break;
                    }

                    wxFAIL_MSG( _T("unknown keyboard accel") );
            }

            delete accel;
        }
        else
        {
            msg << _T("It doesn't have an accelerator");
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

    wxCHECK_RET( count, _T("no last menu?") );

    wxString label = wxGetTextFromUser
                     (
                        _T("Enter label to search for: "),
                        _T("Find menu item"),
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
            wxLogWarning(_T("No menu item with label '%s'"), label.c_str());
        }
        else
        {
            wxLogMessage(_T("Menu item %d in menu %lu has label '%s'"),
                         index, (unsigned long)menuindex, label.c_str());
        }
    }
}
#endif

void MyFrame::ShowContextMenu(const wxPoint& pos)
{
    wxMenu menu;

    menu.Append(Menu_Help_About, _T("&About"));
    menu.Append(Menu_Popup_Submenu, _T("&Submenu"), CreateDummyMenu(NULL));
    menu.Append(Menu_Popup_ToBeDeleted, _T("To be &deleted"));
    menu.AppendCheckItem(Menu_Popup_ToBeChecked, _T("To be &checked"));
    menu.Append(Menu_Popup_ToBeGreyed, _T("To be &greyed"),
                _T("This menu item should be initially greyed out"));
    menu.AppendSeparator();
    menu.Append(Menu_File_Quit, _T("E&xit"));

    menu.Delete(Menu_Popup_ToBeDeleted);
    menu.Check(Menu_Popup_ToBeChecked, true);
    menu.Enable(Menu_Popup_ToBeGreyed, false);

    PopupMenu(&menu, pos.x, pos.y);

    // test for destroying items in popup menus
#if 0 // doesn't work in wxGTK!
    menu.Destroy(Menu_Popup_Submenu);

    PopupMenu( &menu, event.GetX(), event.GetY() );
#endif // 0
}

void MyFrame::OnTestNormal(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(_T("Normal item selected"));
}

void MyFrame::OnTestCheck(wxCommandEvent& event)
{
    wxLogMessage(_T("Check item %schecked"),
                 event.IsChecked() ? _T("") : _T("un"));
}

void MyFrame::OnTestRadio(wxCommandEvent& event)
{
    wxLogMessage(_T("Radio item %d selected"),
                 event.GetId() - Menu_Test_Radio1 + 1);
}

#if USE_LOG_WINDOW
void MyFrame::LogMenuOpenOrClose(const wxMenuEvent& event, const wxChar *what)
{
    wxString msg;
    msg << _T("A ")
        << ( event.IsPopup() ? _T("popup ") : _T("") )
        << _T("menu has been ")
        << what
        << _T(".");

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

