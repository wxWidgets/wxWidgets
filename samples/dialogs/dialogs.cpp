/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.cpp
// Purpose:     Common dialogs demo
// Author:      Julian Smart, Vadim Zeitlin, ABX
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
//              (c) 2004 ABX
//              (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../sample.xpm"

#include "wx/apptrait.h"
#include "wx/datetime.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/minifram.h"
#include "wx/sysopt.h"
#include "wx/notifmsg.h"

#if wxUSE_RICHMSGDLG
    #include "wx/richmsgdlg.h"
#endif // wxUSE_RICHMSGDLG

#if wxUSE_COLOURDLG
    #include "wx/colordlg.h"
#endif // wxUSE_COLOURDLG

#if wxUSE_CHOICEDLG
    #include "wx/choicdlg.h"
#endif // wxUSE_CHOICEDLG

#include "wx/rearrangectrl.h"

#if wxUSE_STARTUP_TIPS
    #include "wx/tipdlg.h"
#endif // wxUSE_STARTUP_TIPS

#if wxUSE_PROGRESSDLG
#if wxUSE_STOPWATCH && wxUSE_LONGLONG
    #include "wx/datetime.h"      // wxDateTime
#endif

    #include "wx/progdlg.h"
#endif // wxUSE_PROGRESSDLG

#if wxUSE_ABOUTDLG
    #include "wx/aboutdlg.h"

    // these headers are only needed for custom about dialog
    #include "wx/statline.h"
    #include "wx/generic/aboutdlgg.h"
#endif // wxUSE_ABOUTDLG

#if wxUSE_BUSYINFO
    #include "wx/busyinfo.h"
#endif // wxUSE_BUSYINFO

#if wxUSE_NUMBERDLG
    #include "wx/numdlg.h"
#endif // wxUSE_NUMBERDLG

#if wxUSE_FILEDLG
    #include "wx/filedlg.h"
#endif // wxUSE_FILEDLG

#if wxUSE_DIRDLG
    #include "wx/dirdlg.h"
#endif // wxUSE_DIRDLG

#if wxUSE_FONTDLG
    #include "wx/fontdlg.h"
#endif // wxUSE_FONTDLG

#if wxUSE_FINDREPLDLG
    #include "wx/fdrepdlg.h"
#endif // wxUSE_FINDREPLDLG

#if wxUSE_INFOBAR
    #include "wx/infobar.h"
#endif // wxUSE_INFOBAR

#include "wx/spinctrl.h"
#include "wx/propdlg.h"

#include "dialogs.h"

#if USE_COLOURDLG_GENERIC
    #include "wx/generic/colrdlgg.h"
#endif // USE_COLOURDLG_GENERIC

#if USE_DIRDLG_GENERIC
    #include "wx/generic/dirdlgg.h"
#endif // USE_DIRDLG_GENERIC

#if USE_FILEDLG_GENERIC
    #include "wx/generic/filedlgg.h"
#endif // USE_FILEDLG_GENERIC

#if USE_FONTDLG_GENERIC
    #include "wx/generic/fontdlgg.h"
#endif // USE_FONTDLG_GENERIC

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()



BEGIN_EVENT_TABLE(MyFrame, wxFrame)
#if wxUSE_MSGDLG
    EVT_MENU(DIALOGS_MESSAGE_BOX,                   MyFrame::MessageBox)
    EVT_MENU(DIALOGS_MESSAGE_BOX_WINDOW_MODAL,      MyFrame::MessageBoxWindowModal)
    EVT_MENU(DIALOGS_MESSAGE_DIALOG,                MyFrame::MessageBoxDialog)
    EVT_MENU(DIALOGS_MESSAGE_BOX_WXINFO,            MyFrame::MessageBoxInfo)
#endif // wxUSE_MSGDLG
#if wxUSE_RICHMSGDLG
    EVT_MENU(DIALOGS_RICH_MESSAGE_DIALOG,           MyFrame::RichMessageDialog)
#endif // wxUSE_RICHMSGDLG
#if wxUSE_COLOURDLG
    EVT_MENU(DIALOGS_CHOOSE_COLOUR,                 MyFrame::ChooseColour)
    EVT_MENU(DIALOGS_GET_COLOUR,                    MyFrame::GetColour)
#endif // wxUSE_COLOURDLG

#if wxUSE_FONTDLG
    EVT_MENU(DIALOGS_CHOOSE_FONT,                   MyFrame::ChooseFont)
#endif // wxUSE_FONTDLG

#if wxUSE_LOG_DIALOG
    EVT_MENU(DIALOGS_LOG_DIALOG,                    MyFrame::LogDialog)
#endif // wxUSE_LOG_DIALOG
#if wxUSE_INFOBAR
    EVT_MENU(DIALOGS_INFOBAR_SIMPLE,                MyFrame::InfoBarSimple)
    EVT_MENU(DIALOGS_INFOBAR_ADVANCED,              MyFrame::InfoBarAdvanced)
#endif // wxUSE_INFOBAR

#if wxUSE_TEXTDLG
    EVT_MENU(DIALOGS_LINE_ENTRY,                    MyFrame::LineEntry)
    EVT_MENU(DIALOGS_TEXT_ENTRY,                    MyFrame::TextEntry)
    EVT_MENU(DIALOGS_PASSWORD_ENTRY,                MyFrame::PasswordEntry)
#endif // wxUSE_TEXTDLG

#if wxUSE_NUMBERDLG
    EVT_MENU(DIALOGS_NUM_ENTRY,                     MyFrame::NumericEntry)
#endif // wxUSE_NUMBERDLG

#if wxUSE_CHOICEDLG
    EVT_MENU(DIALOGS_SINGLE_CHOICE,                 MyFrame::SingleChoice)
    EVT_MENU(DIALOGS_MULTI_CHOICE,                  MyFrame::MultiChoice)
#endif // wxUSE_CHOICEDLG

#if wxUSE_REARRANGECTRL
    EVT_MENU(DIALOGS_REARRANGE,                     MyFrame::Rearrange)
#endif // wxUSE_REARRANGECTRL

#if wxUSE_FILEDLG
    EVT_MENU(DIALOGS_FILE_OPEN,                     MyFrame::FileOpen)
    EVT_MENU(DIALOGS_FILE_OPEN2,                    MyFrame::FileOpen2)
    EVT_MENU(DIALOGS_FILES_OPEN,                    MyFrame::FilesOpen)
    EVT_MENU(DIALOGS_FILE_SAVE,                     MyFrame::FileSave)
#endif // wxUSE_FILEDLG

#if USE_FILEDLG_GENERIC
    EVT_MENU(DIALOGS_FILE_OPEN_GENERIC,             MyFrame::FileOpenGeneric)
    EVT_MENU(DIALOGS_FILES_OPEN_GENERIC,            MyFrame::FilesOpenGeneric)
    EVT_MENU(DIALOGS_FILE_SAVE_GENERIC,             MyFrame::FileSaveGeneric)
#endif // USE_FILEDLG_GENERIC

#if wxUSE_DIRDLG
    EVT_MENU(DIALOGS_DIR_CHOOSE,                    MyFrame::DirChoose)
    EVT_MENU(DIALOGS_DIRNEW_CHOOSE,                 MyFrame::DirChooseNew)
#endif // wxUSE_DIRDLG

#if USE_MODAL_PRESENTATION
    EVT_MENU(DIALOGS_MODAL,                         MyFrame::ModalDlg)
#endif // USE_MODAL_PRESENTATION
    EVT_MENU(DIALOGS_MODELESS,                      MyFrame::ModelessDlg)
    EVT_MENU(DIALOGS_CENTRE_SCREEN,                 MyFrame::DlgCenteredScreen)
    EVT_MENU(DIALOGS_CENTRE_PARENT,                 MyFrame::DlgCenteredParent)
#if wxUSE_MINIFRAME
    EVT_MENU(DIALOGS_MINIFRAME,                     MyFrame::MiniFrame)
#endif // wxUSE_MINIFRAME
    EVT_MENU(DIALOGS_ONTOP,                         MyFrame::DlgOnTop)

#if wxUSE_STARTUP_TIPS
    EVT_MENU(DIALOGS_TIP,                           MyFrame::ShowTip)
#endif // wxUSE_STARTUP_TIPS

#if USE_FONTDLG_GENERIC
    EVT_MENU(DIALOGS_CHOOSE_FONT_GENERIC,           MyFrame::ChooseFontGeneric)
#endif // USE_FONTDLG_GENERIC

#if USE_DIRDLG_GENERIC
    EVT_MENU(DIALOGS_GENERIC_DIR_CHOOSE,            MyFrame::GenericDirChoose)
#endif // wxMSW || wxMAC

#if USE_COLOURDLG_GENERIC
    EVT_MENU(DIALOGS_CHOOSE_COLOUR_GENERIC,         MyFrame::ChooseColourGeneric)
#endif // USE_COLOURDLG_GENERIC

#if wxUSE_PROGRESSDLG
    EVT_MENU(DIALOGS_PROGRESS,                      MyFrame::ShowProgress)
#endif // wxUSE_PROGRESSDLG

#if wxUSE_ABOUTDLG
    EVT_MENU(DIALOGS_ABOUTDLG_SIMPLE,               MyFrame::ShowSimpleAboutDialog)
    EVT_MENU(DIALOGS_ABOUTDLG_FANCY,                MyFrame::ShowFancyAboutDialog)
    EVT_MENU(DIALOGS_ABOUTDLG_FULL,                 MyFrame::ShowFullAboutDialog)
    EVT_MENU(DIALOGS_ABOUTDLG_CUSTOM,               MyFrame::ShowCustomAboutDialog)
#endif // wxUSE_ABOUTDLG

#if wxUSE_BUSYINFO
    EVT_MENU(DIALOGS_BUSYINFO,                      MyFrame::ShowBusyInfo)
#endif // wxUSE_BUSYINFO

#if wxUSE_FINDREPLDLG
    EVT_MENU(DIALOGS_FIND,                          MyFrame::ShowFindDialog)
    EVT_MENU(DIALOGS_REPLACE,                       MyFrame::ShowReplaceDialog)

    EVT_FIND(wxID_ANY, MyFrame::OnFindDialog)
    EVT_FIND_NEXT(wxID_ANY, MyFrame::OnFindDialog)
    EVT_FIND_REPLACE(wxID_ANY, MyFrame::OnFindDialog)
    EVT_FIND_REPLACE_ALL(wxID_ANY, MyFrame::OnFindDialog)
    EVT_FIND_CLOSE(wxID_ANY, MyFrame::OnFindDialog)
#endif // wxUSE_FINDREPLDLG

#if USE_SETTINGS_DIALOG
    EVT_MENU(DIALOGS_PROPERTY_SHEET,                MyFrame::OnPropertySheet)
    EVT_MENU(DIALOGS_PROPERTY_SHEET_TOOLBOOK,       MyFrame::OnPropertySheet)
    EVT_MENU(DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK, MyFrame::OnPropertySheet)
#endif // USE_SETTINGS_DIALOG

    EVT_MENU(DIALOGS_STANDARD_BUTTON_SIZER_DIALOG,  MyFrame::OnStandardButtonsSizerDialog)
    EVT_MENU(DIALOGS_TEST_DEFAULT_ACTION,           MyFrame::OnTestDefaultActionDialog)

    EVT_MENU(DIALOGS_REQUEST,                       MyFrame::OnRequestUserAttention)
#if wxUSE_NOTIFICATION_MESSAGE
    EVT_MENU(DIALOGS_NOTIFY_AUTO,                   MyFrame::OnNotifMsgAuto)
    EVT_MENU(DIALOGS_NOTIFY_SHOW,                   MyFrame::OnNotifMsgShow)
    EVT_MENU(DIALOGS_NOTIFY_HIDE,                   MyFrame::OnNotifMsgHide)
#endif // wxUSE_NOTIFICATION_MESSAGE

    EVT_MENU(wxID_EXIT,                             MyFrame::OnExit)
END_EVENT_TABLE()

#if USE_MODAL_PRESENTATION

    BEGIN_EVENT_TABLE(MyModalDialog, wxDialog)
        EVT_BUTTON(wxID_ANY, MyModalDialog::OnButton)
    END_EVENT_TABLE()

    BEGIN_EVENT_TABLE(MyModelessDialog, wxDialog)
        EVT_BUTTON(DIALOGS_MODELESS_BTN, MyModelessDialog::OnButton)
        EVT_CLOSE(MyModelessDialog::OnClose)
    END_EVENT_TABLE()

#endif // USE_MODAL_PRESENTATION

BEGIN_EVENT_TABLE(StdButtonSizerDialog, wxDialog)
    EVT_CHECKBOX(wxID_ANY, StdButtonSizerDialog::OnEvent)
    EVT_RADIOBUTTON(wxID_ANY, StdButtonSizerDialog::OnEvent)
END_EVENT_TABLE()

#if wxUSE_CMDLINE_PARSER

#include "wx/cmdline.h"

static const char *PROGRESS_SWITCH = "progress";

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

    parser.AddOption("", PROGRESS_SWITCH,
                     "Style for the startup progress dialog (wxPD_XXX)",
                     wxCMD_LINE_VAL_NUMBER);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if ( !wxApp::OnCmdLineParsed(parser) )
        return false;

    parser.Found(PROGRESS_SWITCH, &m_startupProgressStyle);

    return true;
}

#endif // wxUSE_CMDLINE_PARSER

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_PROGRESSDLG
    if ( m_startupProgressStyle != -1 )
    {
        // Show a test progress dialog before the main event loop is started:
        // it should still work.
        const int PROGRESS_COUNT = 100;
        wxProgressDialog dlg
                         (
                            "Progress in progress",
                            "Please wait, starting...",
                            PROGRESS_COUNT,
                            NULL,
                            m_startupProgressStyle
                         );
        for ( int i = 0; i <= PROGRESS_COUNT; i++ )
        {
            if ( !dlg.Update(i) )
                break;

            wxMilliSleep(50);
        }
    }
#endif // wxUSE_PROGRESSDLG

#if wxUSE_IMAGE
    wxInitAllImageHandlers();
#endif

    // Create the main frame window
    MyFrame *frame = new MyFrame(wxT("wxWidgets dialogs example"));

    // Make a menubar
    wxMenu *menuDlg = new wxMenu;

    menuDlg->Append(DIALOGS_MESSAGE_BOX, wxT("&Message box\tCtrl-M"));
    menuDlg->Append(DIALOGS_MESSAGE_BOX_WINDOW_MODAL, wxT("Window-Modal Message box "));
    menuDlg->Append(DIALOGS_MESSAGE_DIALOG, wxT("Message dialog\tShift-Ctrl-M"));
#if wxUSE_RICHMSGDLG
    menuDlg->Append(DIALOGS_RICH_MESSAGE_DIALOG, wxT("Rich message dialog"));
#endif // wxUSE_RICHMSGDLG


#if wxUSE_COLOURDLG || wxUSE_FONTDLG || wxUSE_CHOICEDLG

    wxMenu *choices_menu = new wxMenu;

    #if wxUSE_COLOURDLG
        choices_menu->Append(DIALOGS_CHOOSE_COLOUR, wxT("&Choose bg colour"));
        choices_menu->Append(DIALOGS_GET_COLOUR, wxT("&Choose fg colour"));
    #endif // wxUSE_COLOURDLG

    #if wxUSE_FONTDLG
        choices_menu->Append(DIALOGS_CHOOSE_FONT, wxT("Choose &font"));
    #endif // wxUSE_FONTDLG

    #if wxUSE_CHOICEDLG
        choices_menu->Append(DIALOGS_SINGLE_CHOICE,  wxT("&Single choice\tCtrl-C"));
        choices_menu->Append(DIALOGS_MULTI_CHOICE,  wxT("M&ultiple choice\tCtrl-U"));
    #endif // wxUSE_CHOICEDLG

    #if wxUSE_REARRANGECTRL
        choices_menu->Append(DIALOGS_REARRANGE,  wxT("&Rearrange dialog\tCtrl-R"));
    #endif // wxUSE_REARRANGECTRL

    #if USE_COLOURDLG_GENERIC || USE_FONTDLG_GENERIC
        choices_menu->AppendSeparator();
    #endif // USE_COLOURDLG_GENERIC || USE_FONTDLG_GENERIC

    #if USE_COLOURDLG_GENERIC
        choices_menu->Append(DIALOGS_CHOOSE_COLOUR_GENERIC, wxT("&Choose colour (generic)"));
    #endif // USE_COLOURDLG_GENERIC

    #if USE_FONTDLG_GENERIC
        choices_menu->Append(DIALOGS_CHOOSE_FONT_GENERIC, wxT("Choose &font (generic)"));
    #endif // USE_FONTDLG_GENERIC

    menuDlg->Append(wxID_ANY,wxT("&Choices and selectors"),choices_menu);
#endif // wxUSE_COLOURDLG || wxUSE_FONTDLG || wxUSE_CHOICEDLG


#if wxUSE_TEXTDLG || wxUSE_NUMBERDLG

    wxMenu *entry_menu = new wxMenu;

    #if wxUSE_TEXTDLG
        entry_menu->Append(DIALOGS_LINE_ENTRY,  wxT("Single line &entry\tCtrl-E"));
        entry_menu->Append(DIALOGS_TEXT_ENTRY,  wxT("Multi line text &entry\tShift-Ctrl-E"));
        entry_menu->Append(DIALOGS_PASSWORD_ENTRY,  wxT("&Password entry\tCtrl-P"));
    #endif // wxUSE_TEXTDLG

    #if wxUSE_NUMBERDLG
        entry_menu->Append(DIALOGS_NUM_ENTRY, wxT("&Numeric entry\tCtrl-N"));
    #endif // wxUSE_NUMBERDLG

    menuDlg->Append(wxID_ANY,wxT("&Entry dialogs"),entry_menu);

#endif // wxUSE_TEXTDLG || wxUSE_NUMBERDLG


#if wxUSE_FILEDLG

    wxMenu *filedlg_menu = new wxMenu;
    filedlg_menu->Append(DIALOGS_FILE_OPEN,  wxT("&Open file\tCtrl-O"));
    filedlg_menu->Append(DIALOGS_FILE_OPEN2,  wxT("&Second open file\tCtrl-2"));
    filedlg_menu->Append(DIALOGS_FILES_OPEN,  wxT("Open &files\tCtrl-Q"));
    filedlg_menu->Append(DIALOGS_FILE_SAVE,  wxT("Sa&ve file\tCtrl-S"));

#if USE_FILEDLG_GENERIC
    filedlg_menu->AppendSeparator();
    filedlg_menu->Append(DIALOGS_FILE_OPEN_GENERIC, wxT("&Open file (generic)"));
    filedlg_menu->Append(DIALOGS_FILES_OPEN_GENERIC, wxT("Open &files (generic)"));
    filedlg_menu->Append(DIALOGS_FILE_SAVE_GENERIC, wxT("Sa&ve file (generic)"));
#endif // USE_FILEDLG_GENERIC

    menuDlg->Append(wxID_ANY,wxT("&File operations"),filedlg_menu);

#endif // wxUSE_FILEDLG

#if wxUSE_DIRDLG
    wxMenu *dir_menu = new wxMenu;

    dir_menu->Append(DIALOGS_DIR_CHOOSE,  wxT("&Choose a directory\tCtrl-D"));
    dir_menu->Append(DIALOGS_DIRNEW_CHOOSE,  wxT("Choose a directory (with \"Ne&w\" button)\tShift-Ctrl-D"));
    menuDlg->Append(wxID_ANY,wxT("&Directory operations"),dir_menu);

    #if USE_DIRDLG_GENERIC
        dir_menu->AppendSeparator();
        dir_menu->Append(DIALOGS_GENERIC_DIR_CHOOSE,  wxT("&Choose a directory (generic)"));
    #endif // USE_DIRDLG_GENERIC

#endif // wxUSE_DIRDLG


#if wxUSE_STARTUP_TIPS || \
    wxUSE_PROGRESSDLG || \
    wxUSE_BUSYINFO || \
    wxUSE_LOG_DIALOG || \
    wxUSE_MSGDLG

    wxMenu *info_menu = new wxMenu;

    #if wxUSE_STARTUP_TIPS
        info_menu->Append(DIALOGS_TIP,  wxT("&Tip of the day\tCtrl-T"));
    #endif // wxUSE_STARTUP_TIPS

    #if wxUSE_PROGRESSDLG
        info_menu->Append(DIALOGS_PROGRESS, wxT("Pro&gress dialog\tCtrl-G"));
    #endif // wxUSE_PROGRESSDLG

    #if wxUSE_BUSYINFO
       info_menu->Append(DIALOGS_BUSYINFO, wxT("&Busy info dialog\tCtrl-B"));
    #endif // wxUSE_BUSYINFO

    #if wxUSE_LOG_DIALOG
       info_menu->Append(DIALOGS_LOG_DIALOG, wxT("&Log dialog\tCtrl-L"));
    #endif // wxUSE_LOG_DIALOG

    #if wxUSE_INFOBAR
       info_menu->Append(DIALOGS_INFOBAR_SIMPLE, "Simple &info bar\tCtrl-I");
       info_menu->Append(DIALOGS_INFOBAR_ADVANCED, "&Advanced info bar\tShift-Ctrl-I");
    #endif // wxUSE_INFOBAR

    #if wxUSE_MSGDLG
        info_menu->Append(DIALOGS_MESSAGE_BOX_WXINFO,
                             wxT("&wxWidgets information\tCtrl-W"));
    #endif // wxUSE_MSGDLG

    menuDlg->Append(wxID_ANY,wxT("&Informative dialogs"),info_menu);

#endif // wxUSE_STARTUP_TIPS || wxUSE_PROGRESSDLG || wxUSE_BUSYINFO || wxUSE_LOG_DIALOG


#if wxUSE_FINDREPLDLG
    wxMenu *find_menu = new wxMenu;
    find_menu->AppendCheckItem(DIALOGS_FIND, wxT("&Find dialog\tCtrl-F"));
    find_menu->AppendCheckItem(DIALOGS_REPLACE, wxT("Find and &replace dialog\tShift-Ctrl-F"));
    menuDlg->Append(wxID_ANY,wxT("&Searching"),find_menu);
#endif // wxUSE_FINDREPLDLG

    wxMenu *dialogs_menu = new wxMenu;
#if USE_MODAL_PRESENTATION
    dialogs_menu->Append(DIALOGS_MODAL, wxT("&Modal dialog\tShift-Ctrl-W"));
#endif // USE_MODAL_PRESENTATION
    dialogs_menu->AppendCheckItem(DIALOGS_MODELESS, wxT("Mode&less dialog\tShift-Ctrl-Z"));
    dialogs_menu->Append(DIALOGS_CENTRE_SCREEN, wxT("Centered on &screen\tShift-Ctrl-1"));
    dialogs_menu->Append(DIALOGS_CENTRE_PARENT, wxT("Centered on &parent\tShift-Ctrl-2"));
#if wxUSE_MINIFRAME
    dialogs_menu->Append(DIALOGS_MINIFRAME, wxT("&Mini frame"));
#endif // wxUSE_MINIFRAME
    dialogs_menu->Append(DIALOGS_ONTOP, wxT("Dialog staying on &top"));
    menuDlg->Append(wxID_ANY, wxT("&Generic dialogs"), dialogs_menu);

#if USE_SETTINGS_DIALOG
    wxMenu *sheet_menu = new wxMenu;
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET, wxT("&Standard property sheet\tShift-Ctrl-P"));
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET_TOOLBOOK, wxT("&Toolbook sheet\tShift-Ctrl-T"));

    if (wxPlatformIs(wxPORT_MAC))
        sheet_menu->Append(DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK, wxT("Button &Toolbook sheet\tShift-Ctrl-U"));
/*
#ifdef __WXMAC__
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK, wxT("Button &Toolbook sheet\tShift-Ctrl-U"));
#endif
*/
    menuDlg->Append(wxID_ANY, wxT("&Property sheets"), sheet_menu);
#endif // USE_SETTINGS_DIALOG

    wxMenu *menuNotif = new wxMenu;
    menuNotif->Append(DIALOGS_REQUEST, wxT("&Request user attention\tCtrl-Shift-R"));
#if wxUSE_NOTIFICATION_MESSAGE
    menuNotif->Append(DIALOGS_NOTIFY_AUTO, "&Automatically hidden notification");
    menuNotif->Append(DIALOGS_NOTIFY_SHOW, "&Show manual notification");
    menuNotif->Append(DIALOGS_NOTIFY_HIDE, "&Hide manual notification");
#endif // wxUSE_NOTIFICATION_MESSAGE
    menuDlg->AppendSubMenu(menuNotif, "&User notifications");

    menuDlg->Append(DIALOGS_STANDARD_BUTTON_SIZER_DIALOG, wxT("&Standard Buttons Sizer Dialog"));
    menuDlg->Append(DIALOGS_TEST_DEFAULT_ACTION, wxT("&Test dialog default action"));

    menuDlg->AppendSeparator();
    menuDlg->Append(wxID_EXIT, wxT("E&xit\tAlt-X"));

#if wxUSE_ABOUTDLG
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(DIALOGS_ABOUTDLG_SIMPLE, wxT("&About (simple)...\tF1"));
    menuHelp->Append(DIALOGS_ABOUTDLG_FANCY, wxT("About (&fancy)...\tShift-F1"));
    menuHelp->Append(DIALOGS_ABOUTDLG_FULL, wxT("About (f&ull)...\tCtrl-F1"));
    menuHelp->Append(DIALOGS_ABOUTDLG_CUSTOM, wxT("About (&custom)...\tCtrl-Shift-F1"));
#endif // wxUSE_ABOUTDLG

    wxMenu* editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO, _("&Undo\tCtrl+Z"));
    editMenu->Append(wxID_REDO, _("&Redo\tCtrl+Y"));
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, _("Cu&t\tCtrl+X"));
    editMenu->Append(wxID_COPY, _("&Copy\tCtrl+C"));
    editMenu->Append(wxID_PASTE, _("&Paste\tCtrl+V"));
    editMenu->Append(wxID_CLEAR, _("&Delete"));
    
    editMenu->AppendSeparator();
    editMenu->Append(wxID_SELECTALL, _("Select All\tCtrl+A"));

    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append(menuDlg, wxT("&Dialogs"));
    
    menubar->Append(editMenu, wxT("&Edit"));
    
#if wxUSE_ABOUTDLG
    menubar->Append(menuHelp, wxT("&Help"));
#endif // wxUSE_ABOUTDLG

    frame->SetMenuBar(menubar);

    frame->Centre(wxBOTH);
    frame->Show(true);

    return true;
}

// My frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    SetIcon(wxICON(sample));

#if USE_MODAL_PRESENTATION
    m_dialog = (MyModelessDialog *)NULL;
#endif // USE_MODAL_PRESENTATION

#if wxUSE_FINDREPLDLG
    m_dlgFind =
    m_dlgReplace = NULL;
#endif

#if wxUSE_COLOURDLG
    m_clrData.SetChooseFull(true);
    for (int i = 0; i < wxColourData::NUM_CUSTOM; i++)
    {
        unsigned char n = i*16;
        m_clrData.SetCustomColour(i, wxColour(n, n, n));
    }
#endif // wxUSE_COLOURDLG

#if wxUSE_NOTIFICATION_MESSAGE
    m_notifMsg = NULL;
#endif // wxUSE_NOTIFICATION_MESSAGE

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    m_canvas = new MyCanvas(this);

#if wxUSE_INFOBAR
    // an info bar can be created very simply and used without any extra effort
    m_infoBarSimple = new wxInfoBar(this);

    // or it can also be customized by
    m_infoBarAdvanced = new wxInfoBar(this);

    // ... adding extra buttons (but more than two will usually be too many)
    m_infoBarAdvanced->AddButton(wxID_UNDO);
    m_infoBarAdvanced->AddButton(wxID_REDO);

    m_infoBarAdvanced->Connect(wxID_REDO, wxEVT_COMMAND_BUTTON_CLICKED,
                                wxCommandEventHandler(MyFrame::OnInfoBarRedo),
                                NULL,
                                this);

    // adding and removing a button immediately doesn't make sense here, of
    // course, it's done just to show that it is possible
    m_infoBarAdvanced->AddButton(wxID_EXIT);
    m_infoBarAdvanced->RemoveButton(wxID_EXIT);

    // ... changing the colours and/or fonts
    m_infoBarAdvanced->SetOwnBackgroundColour(0xc8ffff);
    m_infoBarAdvanced->SetFont(GetFont().Bold().Larger());

    // ... and changing the effect (only does anything under MSW currently)
    m_infoBarAdvanced->SetShowHideEffects(wxSHOW_EFFECT_EXPAND,
                                          wxSHOW_EFFECT_EXPAND);
    m_infoBarAdvanced->SetEffectDuration(1500);


    // to use the info bars we need to use sizer for the window layout
    wxBoxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_infoBarSimple, wxSizerFlags().Expand());
    sizer->Add(m_canvas, wxSizerFlags(1).Expand());
    sizer->Add(m_infoBarAdvanced, wxSizerFlags().Expand());
    SetSizer(sizer);

    // final touch: under MSW the info bars are shown progressively and parts
    // of the parent window can be seen during the process, so use the same
    // background colour for our background as for the canvas window which
    // covers our entire client area to avoid jarring colour jumps
    SetOwnBackgroundColour(m_canvas->GetBackgroundColour());
#endif // wxUSE_INFOBAR

#ifdef __WXMSW__
    // Test MSW-specific function allowing to access the "system" menu.
    wxMenu * const menu = MSWGetSystemMenu();
    if ( menu )
    {
        menu->AppendSeparator();

        // The ids of the menu commands in MSW system menu must be multiple of
        // 16 so we can't use DIALOGS_ABOUTDLG_SIMPLE here because it might not
        // satisfy this condition and need to define and connect a separate id.
        static const int DIALOGS_SYSTEM_ABOUT = 0x4010;

        menu->Append(DIALOGS_SYSTEM_ABOUT, "&About...");
        Connect(DIALOGS_SYSTEM_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
                wxCommandEventHandler(MyFrame::ShowSimpleAboutDialog));
    }
#endif // __WXMSW__
}

MyFrame::~MyFrame()
{
#if wxUSE_NOTIFICATION_MESSAGE
    delete m_notifMsg;
#endif // wxUSE_NOTIFICATION_MESSAGE
}

#if wxUSE_COLOURDLG

void MyFrame::ChooseColour(wxCommandEvent& WXUNUSED(event))
{
    m_clrData.SetColour(m_canvas->GetBackgroundColour());

    wxColourDialog dialog(this, &m_clrData);
    dialog.SetTitle(_("Please choose the background colour"));
    if ( dialog.ShowModal() == wxID_OK )
    {
        m_clrData = dialog.GetColourData();
        m_canvas->SetBackgroundColour(m_clrData.GetColour());
        m_canvas->ClearBackground();
        m_canvas->Refresh();
    }
}

void MyFrame::GetColour(wxCommandEvent& WXUNUSED(event))
{
    wxColour clr = wxGetColourFromUser
                   (
                    this,
                    m_canvas->GetForegroundColour(),
                    "Please choose the foreground colour"
                   );
    if ( clr.IsOk() )
    {
        m_canvas->SetForegroundColour(clr);
        m_canvas->Refresh();
    }
    //else: dialog cancelled by user
}

#endif // wxUSE_COLOURDLG


#if USE_COLOURDLG_GENERIC
void MyFrame::ChooseColourGeneric(wxCommandEvent& WXUNUSED(event))
{
    m_clrData.SetColour(m_canvas->GetBackgroundColour());

    //FIXME:TODO:This has no effect...
    m_clrData.SetChooseFull(true);

    for (int i = 0; i < 16; i++)
    {
        wxColour colour(
            (unsigned char)(i*16),
            (unsigned char)(i*16),
            (unsigned char)(i*16)
        );
        m_clrData.SetCustomColour(i, colour);
    }

    wxGenericColourDialog *dialog = new wxGenericColourDialog(this, &m_clrData);
    if (dialog->ShowModal() == wxID_OK)
    {
        m_clrData = dialog->GetColourData();
        m_canvas->SetBackgroundColour(m_clrData.GetColour());
        m_canvas->ClearBackground();
        m_canvas->Refresh();
    }
    dialog->Destroy();
}
#endif // USE_COLOURDLG_GENERIC

#if wxUSE_FONTDLG
void MyFrame::ChooseFont(wxCommandEvent& WXUNUSED(event) )
{
    wxFontData data;
    data.SetInitialFont(m_canvas->GetFont());
    data.SetColour(m_canvas->GetForegroundColour());

    // you might also do this:
    //
    //  wxFontDialog dialog;
    //  if ( !dialog.Create(this, data) { ... error ... }
    //
    wxFontDialog dialog(this, data);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog.GetFontData();
        m_canvas->SetFont(retData.GetChosenFont());
        m_canvas->SetForegroundColour(retData.GetColour());
        m_canvas->Refresh();
    }
    //else: cancelled by the user, don't change the font
}
#endif // wxUSE_FONTDLG

#if USE_FONTDLG_GENERIC
void MyFrame::ChooseFontGeneric(wxCommandEvent& WXUNUSED(event) )
{
    wxFontData data;
    data.SetInitialFont(m_canvas->GetFont());
    data.SetColour(m_canvas->GetForegroundColour());

    wxGenericFontDialog *dialog = new wxGenericFontDialog(this, data);
    if (dialog->ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog->GetFontData();
        m_canvas->SetFont(retData.GetChosenFont());
        m_canvas->SetForegroundColour(retData.GetColour());
        m_canvas->Refresh();
    }
    dialog->Destroy();
}
#endif // USE_FONTDLG_GENERIC

#if wxUSE_LOG_DIALOG
void MyFrame::LogDialog(wxCommandEvent& WXUNUSED(event))
{
    // calling wxYield() (as ~wxBusyCursor does) shouldn't result in messages
    // being flushed -- test it
    {
        wxBusyCursor bc;
        wxLogMessage(wxT("This is some message - everything is ok so far."));
        wxLogMessage(wxT("Another message...\n... this one is on multiple lines"));
        wxLogWarning(wxT("And then something went wrong!"));

        // and if ~wxBusyCursor doesn't do it, then call it manually
        wxYield();
    }

    wxLogError(wxT("Intermediary error handler decided to abort."));
    wxLogError(wxT("The top level caller detected an unrecoverable error."));

    wxLog::FlushActive();

    wxLogMessage(wxT("And this is the same dialog but with only one message."));
}
#endif // wxUSE_LOG_DIALOG

#if wxUSE_INFOBAR

void MyFrame::InfoBarSimple(wxCommandEvent& WXUNUSED(event))
{
    static int s_count = 0;
    m_infoBarSimple->ShowMessage
                     (
                      wxString::Format("Message #%d in the info bar.", ++s_count)
                     );
}

void MyFrame::InfoBarAdvanced(wxCommandEvent& WXUNUSED(event))
{
    m_infoBarAdvanced->ShowMessage("Sorry, it didn't work out.", wxICON_WARNING);
}

void MyFrame::OnInfoBarRedo(wxCommandEvent& WXUNUSED(event))
{
    m_infoBarAdvanced->ShowMessage("Still no, sorry again.", wxICON_ERROR);
}

#endif // wxUSE_INFOBAR


#if wxUSE_MSGDLG
void MyFrame::MessageBox(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog dialog(this,
                           "This is a message box\n"
                           "This is a long, long string to test out if the message box "
                           "is laid out properly.",
                           "Message box text",
                           wxCENTER |
                           wxNO_DEFAULT | wxYES_NO | wxCANCEL |
                           wxICON_INFORMATION);

    wxString extmsg;
    if ( dialog.SetYesNoCancelLabels
        (
         "Answer &Yes",
         "Answer &No",
         "Refuse to answer"
         ) )
    {
        extmsg = "This platform supports custom button labels,\n"
        "so you should see the descriptive labels below.";
    }
    else
    {
        extmsg = "Custom button labels are not supported on this platform,\n"
        "so the default \"Yes\"/\"No\"/\"Cancel\" buttons are used.";
    }
    dialog.SetExtendedMessage(extmsg);

    switch ( dialog.ShowModal() )
    {
        case wxID_YES:
            wxLogStatus(wxT("You pressed \"Yes\""));
            break;

        case wxID_NO:
            wxLogStatus(wxT("You pressed \"No\""));
            break;

        case wxID_CANCEL:
            wxLogStatus(wxT("You pressed \"Cancel\""));
            break;

        default:
            wxLogError(wxT("Unexpected wxMessageDialog return code!"));
    }
}

void MyFrame::MessageBoxWindowModal(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog* dialog = new wxMessageDialog(this,
                           "This is a message box\n"
                           "This is a long, long string to test out if the message box "
                           "is laid out properly.",
                           "Message box text",
                           wxCENTER |
                           wxNO_DEFAULT | wxYES_NO | wxCANCEL |
                           wxICON_INFORMATION);

    wxString extmsg;
    if ( dialog->SetYesNoCancelLabels
                (
                    "Answer &Yes",
                    "Answer &No",
                    "Refuse to answer"
                ) )
    {
        extmsg = "This platform supports custom button labels,\n"
                 "so you should see the descriptive labels below.";
    }
    else
    {
        extmsg = "Custom button labels are not supported on this platform,\n"
                 "so the default \"Yes\"/\"No\"/\"Cancel\" buttons are used.";
    }
    dialog->SetExtendedMessage(extmsg);
    dialog->Connect( wxEVT_WINDOW_MODAL_DIALOG_CLOSED, wxWindowModalDialogEventHandler(MyFrame::MessageBoxWindowModalClosed), NULL, this );
    dialog->ShowWindowModal();
}

void MyFrame::MessageBoxWindowModalClosed(wxWindowModalDialogEvent& event)
{
    wxDialog* dialog = event.GetDialog();
    switch ( dialog->GetReturnCode() )
    {
        case wxID_YES:
            wxLogStatus(wxT("You pressed \"Yes\""));
            break;

        case wxID_NO:
            wxLogStatus(wxT("You pressed \"No\""));
            break;

        case wxID_CANCEL:
            wxLogStatus(wxT("You pressed \"Cancel\""));
            break;

        default:
            wxLogError(wxT("Unexpected wxMessageDialog return code!"));
    }
    delete dialog;
}

void MyFrame::MessageBoxDialog(wxCommandEvent& WXUNUSED(event))
{
    TestMessageBoxDialog dlg(this);
    dlg.Create();
    dlg.ShowModal();
}

void MyFrame::MessageBoxInfo(wxCommandEvent& WXUNUSED(event))
{
    ::wxInfoMessageBox(this);
}
#endif // wxUSE_MSGDLG

#if wxUSE_RICHMSGDLG
void MyFrame::RichMessageDialog(wxCommandEvent& WXUNUSED(event))
{
    TestRichMessageDialog dlg(this);
    dlg.Create();
    dlg.ShowModal();
}
#endif // wxUSE_RICHMSGDLG

#if wxUSE_NUMBERDLG
void MyFrame::NumericEntry(wxCommandEvent& WXUNUSED(event))
{
    long res = wxGetNumberFromUser( wxT("This is some text, actually a lot of text.\n")
                                    wxT("Even two rows of text."),
                                    wxT("Enter a number:"), wxT("Numeric input test"),
                                     50, 0, 100, this );

    wxString msg;
    int icon;
    if ( res == -1 )
    {
        msg = wxT("Invalid number entered or dialog cancelled.");
        icon = wxICON_HAND;
    }
    else
    {
        msg.Printf(wxT("You've entered %lu"), res );
        icon = wxICON_INFORMATION;
    }

    wxMessageBox(msg, wxT("Numeric test result"), wxOK | icon, this);
}
#endif // wxUSE_NUMBERDLG

#if wxUSE_TEXTDLG
void MyFrame::PasswordEntry(wxCommandEvent& WXUNUSED(event))
{
    wxString pwd = wxGetPasswordFromUser(wxT("Enter password:"),
                                         wxT("Password entry dialog"),
                                         wxEmptyString,
                                         this);
    if ( !pwd.empty() )
    {
        wxMessageBox(wxString::Format(wxT("Your password is '%s'"), pwd.c_str()),
                     wxT("Got password"), wxOK | wxICON_INFORMATION, this);
    }
}

void MyFrame::LineEntry(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog(this,
                             wxT("This is a small sample\n")
                             wxT("A long, long string to test out the text entrybox"),
                             wxT("Please enter a string"),
                             wxT("Default value"),
                             wxOK | wxCANCEL);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageBox(dialog.GetValue(), wxT("Got string"), wxOK | wxICON_INFORMATION, this);
    }
}

void MyFrame::TextEntry(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog(this, "You can enter a multiline string here.",
                             "Please enter some text",
                             "First line\nSecond one\nAnd another one too",
                             wxOK | wxCANCEL | wxTE_MULTILINE);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageBox(dialog.GetValue(), wxT("Got text"), wxOK | wxICON_INFORMATION, this);
    }
}
#endif // wxUSE_TEXTDLG

#if wxUSE_CHOICEDLG
void MyFrame::SingleChoice(wxCommandEvent& WXUNUSED(event) )
{
    const wxString choices[] = { wxT("One"), wxT("Two"), wxT("Three"), wxT("Four"), wxT("Five") } ;

    wxSingleChoiceDialog dialog(this,
                                wxT("This is a small sample\n")
                                wxT("A single-choice convenience dialog"),
                                wxT("Please select a value"),
                                WXSIZEOF(choices), choices);

    dialog.SetSelection(2);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetStringSelection(), wxT("Got string"));
        dialog2.ShowModal();
    }
}

void MyFrame::MultiChoice(wxCommandEvent& WXUNUSED(event) )
{
    const wxString choices[] =
    {
        wxT("One"), wxT("Two"), wxT("Three"), wxT("Four"), wxT("Five"),
        wxT("Six"), wxT("Seven"), wxT("Eight"), wxT("Nine"), wxT("Ten"),
        wxT("Eleven"), wxT("Twelve"), wxT("Seventeen"),
    };

    wxArrayInt selections;
    const int count = wxGetSelectedChoices(selections,
                                        wxT("This is a small sample\n")
                                        wxT("A multi-choice convenience dialog"),
                                        wxT("Please select a value"),
                                        WXSIZEOF(choices), choices,
                                        this);
    if ( count >= 0 )
    {
        wxString msg;
        if ( count == 0 )
        {
            msg = wxT("You did not select any items");
        }
        else
        {
            msg.Printf(wxT("You selected %u items:\n"), (unsigned)count);
            for ( int n = 0; n < count; n++ )
            {
                msg += wxString::Format(wxT("\t%u: %u (%s)\n"),
                                        (unsigned)n, (unsigned)selections[n],
                                        choices[selections[n]].c_str());
            }
        }
        wxLogMessage(msg);
    }
    //else: cancelled
}
#endif // wxUSE_CHOICEDLG

#if wxUSE_REARRANGECTRL
// custom rearrange dialog: it adds the possibility to rename an item to the
// base class functionality
class MyRearrangeDialog : public wxRearrangeDialog
{
public:
    MyRearrangeDialog(wxWindow *parent,
                      wxArrayInt& order,
                      wxArrayString& labels,
                      wxArrayString& labelsOrig)
        : wxRearrangeDialog
          (
           parent,
           "Configure the columns shown:",
           "wxRearrangeDialog example",
           order,
           labels
          ),
          m_order(order),
          m_labels(labels),
          m_labelsOrig(labelsOrig)
    {
        m_sel = wxNOT_FOUND;

        wxPanel * const panel = new wxPanel(this);
        wxSizer * const sizer = new wxBoxSizer(wxHORIZONTAL);

        m_labelOrig = new wxStaticText(panel, wxID_ANY, "");
        sizer->Add(m_labelOrig, wxSizerFlags().Centre().Border(wxRIGHT));

        m_text = new wxTextCtrl(panel, wxID_ANY, "",
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_PROCESS_ENTER);
        sizer->Add(m_text, wxSizerFlags().Centre().Border(wxRIGHT));

        sizer->Add(new wxButton(panel, wxID_APPLY, "&Rename"),
                   wxSizerFlags().Centre());

        panel->SetSizer(sizer);

        // call this first to ensure that the controls have a reasonable best
        // size before they're added
        DoUpdateExtraControls(GetList()->GetSelection());

        AddExtraControls(panel);


        // another customization not directly supported by the dialog: add a
        // custom button
        wxWindow * const btnOk = FindWindow(wxID_OK);
        wxCHECK_RET( btnOk, "no Ok button?" );

        wxSizer * const sizerBtns = btnOk->GetContainingSizer();
        wxCHECK_RET( sizerBtns, "no buttons sizer?" );

        sizerBtns->Add(new wxButton(this, wxID_RESET, "&Reset all"),
                       wxSizerFlags().Border(wxLEFT));
    }

    // call this instead of ShowModal() to update order and labels array in
    // case the dialog was not cancelled
    bool Rearrange()
    {
        switch ( ShowModal() )
        {
            case wxID_CANCEL:
                return false;

            case wxID_OK:
                m_order = GetOrder();
                break;

            case wxID_RESET:
                // order already reset
                break;
        }

        return true;
    }

private:
    void OnSelChange(wxCommandEvent& event)
    {
        DoUpdateExtraControls(event.GetInt());
    }

    void OnUpdateUIRename(wxUpdateUIEvent& event)
    {
        event.Enable( CanRename() );
    }

    void OnRename(wxCommandEvent& WXUNUSED(event))
    {
        if ( !CanRename() )
            return;

        m_labels[m_sel] = m_text->GetValue();
        GetList()->SetString(m_sel, m_labels[m_sel]);
    }

    void OnReset(wxCommandEvent& WXUNUSED(event))
    {
        // in a real program we should probably ask if the user really wants to
        // do this but here we just go ahead and reset all columns labels and
        // their order without confirmation
        const unsigned count = m_order.size();
        for ( unsigned n = 0; n < count; n++ )
        {
            m_order[n] = n;
            m_labels[n] = m_labelsOrig[n];
        }

        EndModal(wxID_RESET);
    }

    bool CanRename() const
    {
        // only allow renaming if the user modified the currently selected item
        // text (which presupposes that we do have a current item)
        return m_sel != wxNOT_FOUND && m_text->GetValue() != m_labels[m_sel];
    }

    void DoUpdateExtraControls(int sel)
    {
        m_sel = sel;

        if ( m_sel == wxNOT_FOUND )
        {
            m_labelOrig->SetLabel("<no selection>");
            m_text->Clear();
            m_text->Disable();
        }
        else // have valid item
        {
            m_labelOrig->SetLabelText(m_labelsOrig[m_sel]);
            m_text->Enable();
            m_text->SetValue(m_labels[m_sel]);
        }
    }

    wxArrayInt& m_order;
    wxArrayString& m_labels,
                   m_labelsOrig;

    int m_sel;
    wxStaticText *m_labelOrig;
    wxTextCtrl *m_text;

    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyRearrangeDialog);
};

BEGIN_EVENT_TABLE(MyRearrangeDialog, wxRearrangeDialog)
    EVT_LISTBOX(wxID_ANY, MyRearrangeDialog::OnSelChange)

    EVT_UPDATE_UI(wxID_APPLY, MyRearrangeDialog::OnUpdateUIRename)

    EVT_TEXT_ENTER(wxID_ANY, MyRearrangeDialog::OnRename)
    EVT_BUTTON(wxID_APPLY, MyRearrangeDialog::OnRename)
    EVT_BUTTON(wxID_RESET, MyRearrangeDialog::OnReset)
END_EVENT_TABLE()

void MyFrame::Rearrange(wxCommandEvent& WXUNUSED(event))
{
    // the arrays are static so that we preserve the items order between calls
    // to this function
    static wxArrayInt s_order;
    static wxArrayString s_labels,
                         s_labelsOrig;

    // initialize them on the first call
    if ( s_labelsOrig.empty() )
    {
        static const struct ItemInfo
        {
            const char *label;
            const char *labelOrig;
            int order;
        } items[] =
        {
            { "File name",      "Name",   0 },
            { "File type",      "Ext",    1 },
            { "Size",           "Size",   2 },
            { "Creation time",  "Ctime", ~3 },  // negated so hidden
            { "Last accessed",  "Atime", ~4 },
            { "Last modified",  "Mtime",  5 },
        };

        s_order.reserve(WXSIZEOF(items));
        s_labels.reserve(WXSIZEOF(items));
        s_labelsOrig.reserve(WXSIZEOF(items));
        for ( unsigned n = 0; n < WXSIZEOF(items); n++ )
        {
            const ItemInfo& item = items[n];
            s_order.push_back(item.order);
            s_labels.push_back(item.label);
            s_labelsOrig.push_back(item.labelOrig);
        }
    }

    MyRearrangeDialog dlg(this, s_order, s_labels, s_labelsOrig);
    if ( !dlg.Rearrange() )
        return;

    wxString columns;
    for ( unsigned n = 0; n < s_order.size(); n++ )
    {
        columns += wxString::Format("\n    %u: ", n);
        int idx = s_order[n];
        if ( idx < 0 )
        {
            columns += "[hidden] ";
            idx = ~idx;
        }

        columns += s_labels[idx];
        if ( s_labels[idx] != s_labelsOrig[idx] )
        {
            columns += wxString::Format(" (original label: \"%s\")",
                                        s_labelsOrig[idx]);
        }
    }

    wxLogMessage("The columns order now is:%s", columns);
}
#endif // wxUSE_REARRANGECTRL

#if wxUSE_FILEDLG

// panel with custom controls for file dialog
class MyExtraPanel : public wxPanel
{
public:
    MyExtraPanel(wxWindow *parent);
    void OnCheckBox(wxCommandEvent& event) { m_btn->Enable(event.IsChecked()); }
    wxString GetInfo() const
    {
        return wxString::Format("checkbox value = %d", (int) m_cb->GetValue());
    }
private:
    wxButton *m_btn;
    wxCheckBox *m_cb;
};

MyExtraPanel::MyExtraPanel(wxWindow *parent)
            : wxPanel(parent)
{
    m_btn = new wxButton(this, -1, wxT("Custom Button"));
    m_btn->Enable(false);
    m_cb = new wxCheckBox(this, -1, wxT("Enable Custom Button"));
    m_cb->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED,
                  wxCommandEventHandler(MyExtraPanel::OnCheckBox), NULL, this);
    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(m_cb, wxSizerFlags().Centre().Border());
    sizerTop->AddStretchSpacer();
    sizerTop->Add(m_btn, wxSizerFlags().Right().Border());
    SetSizerAndFit(sizerTop);
}

// a static method can be used instead of a function with most of compilers
static wxWindow* createMyExtraPanel(wxWindow *parent)
{
    return new MyExtraPanel(parent);
}

void MyFrame::FileOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog
                 (
                    this,
                    wxT("Testing open file dialog"),
                    wxEmptyString,
                    wxEmptyString,
#ifdef __WXMOTIF__
                    wxT("C++ files (*.cpp)|*.cpp")
#else
                    wxT("C++ files (*.cpp;*.h)|*.cpp;*.h")
#endif
                 );

    dialog.SetExtraControlCreator(&createMyExtraPanel);
    dialog.CentreOnParent();
    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString info;
        wxWindow * const extra = dialog.GetExtraControl();
        info.Printf(wxT("Full file name: %s\n")
                    wxT("Path: %s\n")
                    wxT("Name: %s\n")
                    wxT("Custom window: %s"),
                    dialog.GetPath().c_str(),
                    dialog.GetDirectory().c_str(),
                    dialog.GetFilename().c_str(),
                    extra ? static_cast<MyExtraPanel*>(extra)->GetInfo()
                          : wxString("None"));
        wxMessageDialog dialog2(this, info, wxT("Selected file"));
        dialog2.ShowModal();
    }
}

// this shows how to take advantage of specifying a default extension in the
// call to wxFileSelector: it is remembered after each new call and the next
// one will use it by default
void MyFrame::FileOpen2(wxCommandEvent& WXUNUSED(event) )
{
    static wxString s_extDef;
    wxString path = wxFileSelector(
                                    wxT("Select the file to load"),
                                    wxEmptyString, wxEmptyString,
                                    s_extDef,
                                    wxString::Format
                                    (
                                        wxT("Waveform (*.wav)|*.wav|Plain text (*.txt)|*.txt|All files (%s)|%s"),
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFileSelectorDefaultWildcardStr
                                    ),
                                    wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                                    this
                                   );

    if ( !path )
        return;

    // it is just a sample, would use wxSplitPath in real program
    s_extDef = path.AfterLast(wxT('.'));

    wxLogMessage(wxT("You selected the file '%s', remembered extension '%s'"),
                 path, s_extDef);
}

void MyFrame::FilesOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxString wildcards =
#ifdef __WXMOTIF__
                    wxT("C++ files (*.cpp)|*.cpp");
#else
                    wxString::Format
                    (
                        wxT("All files (%s)|%s|C++ files (*.cpp;*.h)|*.cpp;*.h"),
                        wxFileSelectorDefaultWildcardStr,
                        wxFileSelectorDefaultWildcardStr
                    );
#endif
    wxFileDialog dialog(this, wxT("Testing open multiple file dialog"),
                        wxEmptyString, wxEmptyString, wildcards,
                        wxFD_OPEN|wxFD_MULTIPLE);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxArrayString paths, filenames;

        dialog.GetPaths(paths);
        dialog.GetFilenames(filenames);

        wxString msg, s;
        size_t count = paths.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            s.Printf(wxT("File %d: %s (%s)\n"),
                     (int)n, paths[n].c_str(), filenames[n].c_str());

            msg += s;
        }
        s.Printf(wxT("Filter index: %d"), dialog.GetFilterIndex());
        msg += s;

        wxMessageDialog dialog2(this, msg, wxT("Selected files"));
        dialog2.ShowModal();
    }
}

void MyFrame::FileSave(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog(this,
                        wxT("Testing save file dialog"),
                        wxEmptyString,
                        wxT("myletter.doc"),
                        wxT("Text files (*.txt)|*.txt|Document files (*.doc;*.ods)|*.doc;*.ods"),
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage(wxT("%s, filter %d"),
                     dialog.GetPath().c_str(), dialog.GetFilterIndex());
    }
}
#endif // wxUSE_FILEDLG

#if USE_FILEDLG_GENERIC
void MyFrame::FileOpenGeneric(wxCommandEvent& WXUNUSED(event) )
{
    wxGenericFileDialog dialog
                 (
                    this,
                    wxT("Testing open file dialog"),
                    wxEmptyString,
                    wxEmptyString,
                    wxT("C++ files (*.cpp;*.h)|*.cpp;*.h")
                 );

    dialog.SetExtraControlCreator(&createMyExtraPanel);
    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString info;
        info.Printf(wxT("Full file name: %s\n")
                    wxT("Path: %s\n")
                    wxT("Name: %s"),
                    dialog.GetPath().c_str(),
                    dialog.GetDirectory().c_str(),
                    dialog.GetFilename().c_str());
        wxMessageDialog dialog2(this, info, wxT("Selected file"));
        dialog2.ShowModal();
    }
}

void MyFrame::FilesOpenGeneric(wxCommandEvent& WXUNUSED(event) )
{
    // On PocketPC you can disable OK-only dialogs policy using system option
    int buttons = wxSystemOptions::GetOptionInt(wxT("wince.dialog.real-ok-cancel"));
    wxSystemOptions::SetOption(wxT("wince.dialog.real-ok-cancel"), 1);

    wxString wildcards = wxT("All files (*.*)|*.*|C++ files (*.cpp;*.h)|*.cpp;*.h");
    wxGenericFileDialog dialog(this, wxT("Testing open multiple file dialog"),
                        wxEmptyString, wxEmptyString, wildcards,
                        wxFD_MULTIPLE);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxArrayString paths, filenames;

        dialog.GetPaths(paths);
        dialog.GetFilenames(filenames);

        wxString msg, s;
        size_t count = paths.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            s.Printf(wxT("File %d: %s (%s)\n"),
                     (int)n, paths[n].c_str(), filenames[n].c_str());

            msg += s;
        }
        s.Printf(wxT("Filter index: %d"), dialog.GetFilterIndex());
        msg += s;

        wxMessageDialog dialog2(this, msg, wxT("Selected files"));
        dialog2.ShowModal();
    }

    // restore system option
    wxSystemOptions::SetOption(wxT("wince.dialog.real-ok-cancel"), buttons);
}

void MyFrame::FileSaveGeneric(wxCommandEvent& WXUNUSED(event) )
{
    wxGenericFileDialog dialog(this,
                        wxT("Testing save file dialog"),
                        wxEmptyString,
                        wxT("myletter.doc"),
                        wxT("Text files (*.txt)|*.txt|Document files (*.doc;*.ods)|*.doc;*.ods"),
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage(wxT("%s, filter %d"),
                     dialog.GetPath().c_str(), dialog.GetFilterIndex());
    }
}
#endif // USE_FILEDLG_GENERIC

#if wxUSE_DIRDLG
void MyFrame::DoDirChoose(int style)
{
    // pass some initial dir to wxDirDialog
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxDirDialog dialog(this, wxT("Testing directory picker"), dirHome, style);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage(wxT("Selected path: %s"), dialog.GetPath().c_str());
    }
}

void MyFrame::DirChoose(wxCommandEvent& WXUNUSED(event) )
{
    DoDirChoose(wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
}

void MyFrame::DirChooseNew(wxCommandEvent& WXUNUSED(event) )
{
    DoDirChoose(wxDD_DEFAULT_STYLE & ~wxDD_DIR_MUST_EXIST);
}
#endif // wxUSE_DIRDLG

#if USE_DIRDLG_GENERIC
void MyFrame::GenericDirChoose(wxCommandEvent& WXUNUSED(event) )
{
    // pass some initial dir to wxDirDialog
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxGenericDirDialog dialog(this, wxT("Testing generic directory picker"), dirHome);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetPath(), wxT("Selected path"));
        dialog2.ShowModal();
    }
}
#endif // USE_DIRDLG_GENERIC

#if USE_MODAL_PRESENTATION
void MyFrame::ModalDlg(wxCommandEvent& WXUNUSED(event))
{
    MyModalDialog dlg(this);
    dlg.ShowModal();
}
#endif // USE_MODAL_PRESENTATION

void MyFrame::ModelessDlg(wxCommandEvent& event)
{
    bool show = GetMenuBar()->IsChecked(event.GetId());

    if ( show )
    {
        if ( !m_dialog )
        {
            m_dialog = new MyModelessDialog(this);
        }

        m_dialog->Show(true);
    }
    else // hide
    {
        // If m_dialog is NULL, then possibly the system
        // didn't report the checked menu item status correctly.
        // It should be true just after the menu item was selected,
        // if there was no modeless dialog yet.

        wxASSERT( m_dialog != NULL );
        if (m_dialog)
            m_dialog->Hide();
    }
}

void MyFrame::DlgCenteredScreen(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg(this, wxID_ANY, wxT("Dialog centered on screen"),
                 wxDefaultPosition, wxSize(200, 100));
    (new wxButton(&dlg, wxID_OK, wxT("Close")))->Centre();
    dlg.CentreOnScreen();
    dlg.ShowModal();
}

void MyFrame::DlgCenteredParent(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg(this, wxID_ANY, wxT("Dialog centered on parent"),
                 wxDefaultPosition, wxSize(200, 100));
    (new wxButton(&dlg, wxID_OK, wxT("Close")))->Centre();
    dlg.CentreOnParent();
    dlg.ShowModal();
}

#if wxUSE_MINIFRAME
void MyFrame::MiniFrame(wxCommandEvent& WXUNUSED(event))
{
    wxFrame *frame = new wxMiniFrame(this, wxID_ANY, wxT("Mini frame"),
                                     wxDefaultPosition, wxSize(300, 100),
                                     wxCAPTION | wxCLOSE_BOX);
    new wxStaticText(frame,
                     wxID_ANY,
                     wxT("Mini frames have slightly different appearance"),
                     wxPoint(5, 5));
    new wxStaticText(frame,
                     wxID_ANY,
                     wxT("from the normal frames but that's the only difference."),
                     wxPoint(5, 25));

    frame->CentreOnParent();
    frame->Show();
}
#endif // wxUSE_MINIFRAME

void MyFrame::DlgOnTop(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg(this, wxID_ANY, wxT("Dialog staying on top of other windows"),
                 wxDefaultPosition, wxSize(300, 100),
                 wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
    (new wxButton(&dlg, wxID_OK, wxT("Close")))->Centre();
    dlg.ShowModal();
}

#if wxUSE_STARTUP_TIPS
void MyFrame::ShowTip(wxCommandEvent& WXUNUSED(event))
{
    static size_t s_index = (size_t)-1;

    if ( s_index == (size_t)-1 )
    {
        srand(time(NULL));

        // this is completely bogus, we don't know how many lines are there
        // in the file, but who cares, it's a demo only...
        s_index = rand() % 5;
    }

    wxTipProvider *tipProvider = wxCreateFileTipProvider(wxT("tips.txt"), s_index);

    bool showAtStartup = wxShowTip(this, tipProvider);

    if ( showAtStartup )
    {
        wxMessageBox(wxT("Will show tips on startup"), wxT("Tips dialog"),
                     wxOK | wxICON_INFORMATION, this);
    }

    s_index = tipProvider->GetCurrentTip();
    delete tipProvider;
}
#endif // wxUSE_STARTUP_TIPS

#if USE_SETTINGS_DIALOG
void MyFrame::OnPropertySheet(wxCommandEvent& event)
{
    SettingsDialog dialog(this, event.GetId());
    dialog.ShowModal();
}
#endif // USE_SETTINGS_DIALOG

void MyFrame::OnRequestUserAttention(wxCommandEvent& WXUNUSED(event))
{
    wxLogStatus(wxT("Sleeping for 3 seconds to allow you to switch to another window"));

    wxSleep(3);

    RequestUserAttention(wxUSER_ATTENTION_ERROR);
}

#if wxUSE_NOTIFICATION_MESSAGE

void MyFrame::OnNotifMsgAuto(wxCommandEvent& WXUNUSED(event))
{
    if ( !wxNotificationMessage
          (
            "Automatic Notification",
            "Nothing important has happened\n"
            "this notification will disappear soon."
          ).Show() )
    {
        wxLogStatus("Failed to show notification message");
    }
}

void MyFrame::OnNotifMsgShow(wxCommandEvent& WXUNUSED(event))
{
    if ( !m_notifMsg )
    {
        m_notifMsg = new wxNotificationMessage
                         (
                            "wxWidgets Manual Notification",
                            "You can hide this notification from the menu",
                            this
                         );
    }

    if ( !m_notifMsg->Show(wxNotificationMessage::Timeout_Never) )
    {
        wxLogStatus("Failed to show manual notification message");
    }
}

void MyFrame::OnNotifMsgHide(wxCommandEvent& WXUNUSED(event))
{
    if ( m_notifMsg && !m_notifMsg->Close() )
    {
        wxLogStatus("Failed to hide manual notification message");
    }
}

#endif // wxUSE_NOTIFICATION_MESSAGE

void MyFrame::OnStandardButtonsSizerDialog(wxCommandEvent& WXUNUSED(event))
{
    StdButtonSizerDialog  dialog(this);
    dialog.ShowModal();
}

// TestDefaultAction

#define ID_CATCH_LISTBOX_DCLICK 100
#define ID_LISTBOX              101
#define ID_DISABLE_OK           102
#define ID_DISABLE_CANCEL       103

BEGIN_EVENT_TABLE(TestDefaultActionDialog, wxDialog)
    EVT_CHECKBOX(ID_CATCH_LISTBOX_DCLICK,    TestDefaultActionDialog::OnCatchListBoxDClick)
    EVT_CHECKBOX(ID_DISABLE_OK,              TestDefaultActionDialog::OnDisableOK)
    EVT_CHECKBOX(ID_DISABLE_CANCEL,          TestDefaultActionDialog::OnDisableCancel)
    EVT_LISTBOX_DCLICK(ID_LISTBOX,           TestDefaultActionDialog::OnListBoxDClick)
    EVT_TEXT_ENTER(wxID_ANY,                 TestDefaultActionDialog::OnTextEnter)
END_EVENT_TABLE()

TestDefaultActionDialog::TestDefaultActionDialog( wxWindow *parent ) :
  wxDialog( parent, -1, "Test default action" )
{
    m_catchListBoxDClick = false;

    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *grid_sizer = new wxFlexGridSizer( 2, 5, 5 );

#if wxUSE_LISTBOX
    wxListBox *listbox = new wxListBox( this, ID_LISTBOX );
    listbox->Append( "String 1" );
    listbox->Append( "String 2" );
    listbox->Append( "String 3" );
    listbox->Append( "String 4" );
    grid_sizer->Add( listbox );
#endif // wxUSE_LISTBOX

    grid_sizer->Add( new wxCheckBox( this, ID_CATCH_LISTBOX_DCLICK, "Catch DoubleClick from wxListBox" ), 0, wxALIGN_CENTRE_VERTICAL );

    grid_sizer->Add( new wxTextCtrl( this, -1, "", wxDefaultPosition, wxSize(80,-1), 0 ), 0, wxALIGN_CENTRE_VERTICAL );
    grid_sizer->Add( new wxStaticText( this, -1, "wxTextCtrl without wxTE_PROCESS_ENTER" ), 0, wxALIGN_CENTRE_VERTICAL );

    grid_sizer->Add( new wxTextCtrl( this, -1, "", wxDefaultPosition, wxSize(80,-1), wxTE_PROCESS_ENTER ), 0, wxALIGN_CENTRE_VERTICAL );
    grid_sizer->Add( new wxStaticText( this, -1, "wxTextCtrl with wxTE_PROCESS_ENTER" ), 0, wxALIGN_CENTRE_VERTICAL );

    grid_sizer->Add( new wxCheckBox(this, ID_DISABLE_OK, "Disable \"OK\""), 0, wxALIGN_CENTRE_VERTICAL );
    grid_sizer->Add( new wxCheckBox(this, ID_DISABLE_CANCEL, "Disable \"Cancel\""), 0, wxALIGN_CENTRE_VERTICAL );

    main_sizer->Add( grid_sizer, 0, wxALL, 10 );

    wxSizer *button_sizer = CreateSeparatedButtonSizer( wxOK|wxCANCEL );
    if (button_sizer)
        main_sizer->Add( button_sizer, 0, wxALL|wxGROW, 5 );

    SetSizerAndFit( main_sizer );
}

void TestDefaultActionDialog::OnDisableOK(wxCommandEvent& event)
{
    FindWindow(wxID_OK)->Enable(!event.IsChecked());
}

void TestDefaultActionDialog::OnDisableCancel(wxCommandEvent& event)
{
    FindWindow(wxID_CANCEL)->Enable(!event.IsChecked());
}

void TestDefaultActionDialog::OnListBoxDClick(wxCommandEvent& event)
{
    event.Skip( !m_catchListBoxDClick );
}

void TestDefaultActionDialog::OnCatchListBoxDClick(wxCommandEvent& WXUNUSED(event))
{
    m_catchListBoxDClick = !m_catchListBoxDClick;
}

void TestDefaultActionDialog::OnTextEnter(wxCommandEvent& event)
{
    wxLogMessage("Text \"%s\" entered.", event.GetString());
}

void MyFrame::OnTestDefaultActionDialog(wxCommandEvent& WXUNUSED(event))
{
    TestDefaultActionDialog dialog( this );
    dialog.ShowModal();
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

#if wxUSE_PROGRESSDLG

void MyFrame::ShowProgress( wxCommandEvent& WXUNUSED(event) )
{
    static const int max = 100;

    wxProgressDialog dialog("Progress dialog example",
                            // "Reserve" enough space for the multiline
                            // messages below, we'll change it anyhow
                            // immediately in the loop below
                            wxString(' ', 100) + "\n\n\n\n",
                            max,    // range
                            this,   // parent
                            wxPD_CAN_ABORT |
                            wxPD_CAN_SKIP |
                            wxPD_APP_MODAL |
                            //wxPD_AUTO_HIDE | // -- try this as well
                            wxPD_ELAPSED_TIME |
                            wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME |
                            wxPD_SMOOTH // - makes indeterminate mode bar on WinXP very small
                            );

    bool cont = true;
    for ( int i = 0; i <= max; i++ )
    {
        wxString msg;

        // test both modes of wxProgressDialog behaviour: start in
        // indeterminate mode but switch to the determinate one later
        const bool determinate = i > max/2;

        if ( i == max )
        {
            msg = "That's all, folks!\n"
                  "\n"
                  "Nothing to see here any more.";
        }
        else if ( !determinate )
        {
            msg = "Testing indeterminate mode\n"
                  "\n"
                  "This mode allows you to show to the user\n"
                  "that something is going on even if you don't know\n"
                  "when exactly will you finish.";
        }
        else if ( determinate )
        {
            msg = "Now in standard determinate mode\n"
                  "\n"
                  "This is the standard usage mode in which you\n"
                  "update the dialog after performing each new step of work.\n"
                  "It requires knowing the total number of steps in advance.";
        }

        // will be set to true if "Skip" button was pressed
        bool skip = false;
        if ( determinate )
        {
            cont = dialog.Update(i, msg, &skip);
        }
        else
        {
            cont = dialog.Pulse(msg, &skip);
        }

        // each skip will move progress about quarter forward
        if ( skip )
        {
            i += max/4;

            if ( i >= 100 )
                i = 99;
        }

        if ( !cont )
        {
            if ( wxMessageBox(wxT("Do you really want to cancel?"),
                              wxT("Progress dialog question"),  // caption
                              wxYES_NO | wxICON_QUESTION) == wxYES )
                break;

            cont = true;
            dialog.Resume();
        }

        wxMilliSleep(200);
    }

    if ( !cont )
    {
        wxLogStatus(wxT("Progress dialog aborted!"));
    }
    else
    {
        wxLogStatus(wxT("Countdown from %d finished"), max);
    }
}

#endif // wxUSE_PROGRESSDLG

#if wxUSE_ABOUTDLG

static void InitAboutInfoMinimal(wxAboutDialogInfo& info)
{
    info.SetName(wxT("Dialogs Sample"));
    info.SetVersion(wxVERSION_NUM_DOT_STRING,
                    wxString::Format
                    (
                        "%s version %s",
                        wxMINOR_VERSION % 2 ? "Development" : "Stable",
                        wxVERSION_NUM_DOT_STRING
                    ));
    info.SetDescription(wxT("This sample shows different wxWidgets dialogs"));
    info.SetCopyright(wxT("(C) 1998-2006 wxWidgets dev team"));
    info.AddDeveloper(wxT("Vadim Zeitlin"));
}

static void InitAboutInfoWebsite(wxAboutDialogInfo& info)
{
    InitAboutInfoMinimal(info);

    info.SetWebSite(wxT("http://www.wxwidgets.org/"), wxT("wxWidgets web site"));
}

static void InitAboutInfoAll(wxAboutDialogInfo& info)
{
    InitAboutInfoWebsite(info);

    // we can add a second developer
    info.AddDeveloper(wxT("A.N. Other"));

    // or we can add several persons at once like this
    static const wxChar *docwriters[] =
    {
        wxT("First D. Writer"),
        wxT("Second One"),
    };

    info.SetDocWriters(wxArrayString(WXSIZEOF(docwriters), docwriters));
    info.SetLicence(wxString::FromAscii(
"                wxWindows Library Licence, Version 3.1\n"
"                ======================================\n"
"\n"
"  Copyright (c) 1998-2005 Julian Smart, Robert Roebling et al\n"
"\n"
"  Everyone is permitted to copy and distribute verbatim copies\n"
"  of this licence document, but changing it is not allowed.\n"
"\n"
"                       WXWINDOWS LIBRARY LICENCE\n"
"     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n"
"\n"
"                    ...and so on and so forth...\n"
    ));

    info.AddTranslator(wxT("Wun Ngo Wen (Martian)"));
}

void MyFrame::ShowSimpleAboutDialog(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo info;
    InitAboutInfoMinimal(info);

    wxAboutBox(info, this);
}

void MyFrame::ShowFancyAboutDialog(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo info;
    InitAboutInfoWebsite(info);

    wxAboutBox(info, this);
}

void MyFrame::ShowFullAboutDialog(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo info;
    InitAboutInfoAll(info);

    wxAboutBox(info, this);
}

// a trivial example of a custom dialog class
class MyAboutDialog : public wxGenericAboutDialog
{
public:
    MyAboutDialog(const wxAboutDialogInfo& info, wxWindow* parent)
    {
        Create(info, parent);
    }

    // add some custom controls
    virtual void DoAddCustomControls()
    {
        AddControl(new wxStaticLine(this), wxSizerFlags().Expand());
        AddText(wxT("Some custom text"));
        AddControl(new wxStaticLine(this), wxSizerFlags().Expand());
    }
};

void MyFrame::ShowCustomAboutDialog(wxCommandEvent& WXUNUSED(event))
{
    wxAboutDialogInfo info;
    InitAboutInfoAll(info);

    MyAboutDialog dlg(info, this);
    dlg.ShowModal();
}

#endif // wxUSE_ABOUTDLG

#if wxUSE_BUSYINFO

void MyFrame::ShowBusyInfo(wxCommandEvent& WXUNUSED(event))
{
    wxWindowDisabler disableAll;

    wxBusyInfo info(wxT("Working, please wait..."), this);

    for ( int i = 0; i < 18; i++ )
    {
        wxMilliSleep(100);
        wxTheApp->Yield();
    }

    wxSleep(2);
    //wxWakeUpIdle();
}

#endif // wxUSE_BUSYINFO

#if wxUSE_FINDREPLDLG

void MyFrame::ShowReplaceDialog( wxCommandEvent& WXUNUSED(event) )
{
    if ( m_dlgReplace )
    {
        wxDELETE(m_dlgReplace);
    }
    else
    {
        m_dlgReplace = new wxFindReplaceDialog
                           (
                            this,
                            &m_findData,
                            wxT("Find and replace dialog"),
                            wxFR_REPLACEDIALOG
                           );

        m_dlgReplace->Show(true);
    }
}

void MyFrame::ShowFindDialog( wxCommandEvent& WXUNUSED(event) )
{
    if ( m_dlgFind )
    {
        wxDELETE(m_dlgFind);
    }
    else
    {
        m_dlgFind = new wxFindReplaceDialog
                        (
                            this,
                            &m_findData,
                            wxT("Find dialog"),
                            // just for testing
                            wxFR_NOWHOLEWORD
                        );

        m_dlgFind->Show(true);
    }
}

static wxString DecodeFindDialogEventFlags(int flags)
{
    wxString str;
    str << (flags & wxFR_DOWN ? wxT("down") : wxT("up")) << wxT(", ")
        << (flags & wxFR_WHOLEWORD ? wxT("whole words only, ") : wxT(""))
        << (flags & wxFR_MATCHCASE ? wxT("") : wxT("not "))
        << wxT("case sensitive");

    return str;
}

void MyFrame::OnFindDialog(wxFindDialogEvent& event)
{
    wxEventType type = event.GetEventType();

    if ( type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT )
    {
        wxLogMessage(wxT("Find %s'%s' (flags: %s)"),
                     type == wxEVT_COMMAND_FIND_NEXT ? wxT("next ") : wxT(""),
                     event.GetFindString().c_str(),
                     DecodeFindDialogEventFlags(event.GetFlags()).c_str());
    }
    else if ( type == wxEVT_COMMAND_FIND_REPLACE ||
                type == wxEVT_COMMAND_FIND_REPLACE_ALL )
    {
        wxLogMessage(wxT("Replace %s'%s' with '%s' (flags: %s)"),
                     type == wxEVT_COMMAND_FIND_REPLACE_ALL ? wxT("all ") : wxT(""),
                     event.GetFindString().c_str(),
                     event.GetReplaceString().c_str(),
                     DecodeFindDialogEventFlags(event.GetFlags()).c_str());
    }
    else if ( type == wxEVT_COMMAND_FIND_CLOSE )
    {
        wxFindReplaceDialog *dlg = event.GetDialog();

        int idMenu;
        const wxChar *txt;
        if ( dlg == m_dlgFind )
        {
            txt = wxT("Find");
            idMenu = DIALOGS_FIND;
            m_dlgFind = NULL;
        }
        else if ( dlg == m_dlgReplace )
        {
            txt = wxT("Replace");
            idMenu = DIALOGS_REPLACE;
            m_dlgReplace = NULL;
        }
        else
        {
            txt = wxT("Unknown");
            idMenu = wxID_ANY;

            wxFAIL_MSG( wxT("unexpected event") );
        }

        wxLogMessage(wxT("%s dialog is being closed."), txt);

        if ( idMenu != wxID_ANY )
        {
            GetMenuBar()->Check(idMenu, false);
        }

        dlg->Destroy();
    }
    else
    {
        wxLogError(wxT("Unknown find dialog event!"));
    }
}

#endif // wxUSE_FINDREPLDLG

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.DrawText(
                wxT("wxWidgets common dialogs")
#if !defined(__SMARTPHONE__)
                wxT(" test application")
#endif
                , 10, 10);
}

#if USE_MODAL_PRESENTATION

// ----------------------------------------------------------------------------
// MyModelessDialog
// ----------------------------------------------------------------------------

MyModelessDialog::MyModelessDialog(wxWindow *parent)
                : wxDialog(parent, wxID_ANY, wxString(wxT("Modeless dialog")))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    wxButton *btn = new wxButton(this, DIALOGS_MODELESS_BTN, wxT("Press me"));
    wxCheckBox *check = new wxCheckBox(this, wxID_ANY, wxT("Should be disabled"));
    check->Disable();

    sizerTop->Add(btn, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(check, 1, wxEXPAND | wxALL, 5);

    SetSizerAndFit(sizerTop);
}

void MyModelessDialog::OnButton(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("Button pressed in modeless dialog"), wxT("Info"),
                 wxOK | wxICON_INFORMATION, this);
}

void MyModelessDialog::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto() )
    {
        wxMessageBox(wxT("Use the menu item to close this dialog"),
                     wxT("Modeless dialog"),
                     wxOK | wxICON_INFORMATION, this);

        event.Veto();
    }
}

// ----------------------------------------------------------------------------
// MyModalDialog
// ----------------------------------------------------------------------------

MyModalDialog::MyModalDialog(wxWindow *parent)
             : wxDialog(parent, wxID_ANY, wxString(wxT("Modal dialog")))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    m_btnModal = new wxButton(this, wxID_ANY, wxT("&Modal dialog..."));
    m_btnModeless = new wxButton(this, wxID_ANY, wxT("Mode&less dialog"));
    m_btnDelete = new wxButton(this, wxID_ANY, wxT("&Delete button"));

    sizerTop->Add(m_btnModal, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(m_btnModeless, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(m_btnDelete, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(new wxButton(this, wxID_CLOSE), 0, wxALIGN_CENTER | wxALL, 5);

    SetSizerAndFit(sizerTop);

    SetEscapeId(wxID_CLOSE);

    m_btnModal->SetFocus();
    m_btnModal->SetDefault();
}

void MyModalDialog::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == m_btnDelete )
    {
        wxDELETE(m_btnModal);
        m_btnDelete->Disable();
    }
    else if ( event.GetEventObject() == m_btnModal )
    {
#if wxUSE_TEXTDLG
        wxGetTextFromUser(wxT("Dummy prompt"),
                          wxT("Modal dialog called from dialog"),
                          wxEmptyString, this);
#else
        wxMessageBox(wxT("Modal dialog called from dialog"));
#endif // wxUSE_TEXTDLG
    }
    else if ( event.GetEventObject() == m_btnModeless )
    {
        (new MyModelessDialog(this))->Show();
    }
    else
    {
        event.Skip();
    }
}

#endif // USE_MODAL_PRESENTATION

// ----------------------------------------------------------------------------
// StdButtonSizerDialog
// ----------------------------------------------------------------------------

StdButtonSizerDialog::StdButtonSizerDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, wxString(wxT("StdButtonSizer dialog")),
      wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
      m_buttonsSizer(NULL)
{
    wxBoxSizer *const sizerTop = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *const sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *const sizerInside1 = new wxBoxSizer(wxVERTICAL);

    m_chkboxAffirmativeButton = new wxCheckBox(this, wxID_ANY, _("Enable Affirmative Button"));

    wxStaticBoxSizer *const sizer1 = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Affirmative Button"));

    m_radiobtnOk = new wxRadioButton(this, wxID_ANY, _("Ok"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_radiobtnYes = new wxRadioButton(this, wxID_ANY, _("Yes"));

    wxBoxSizer *const sizerInside2 = new wxBoxSizer(wxVERTICAL);

    m_chkboxDismissButton = new wxCheckBox(this, wxID_ANY, _("Enable Dismiss Button"));

    wxStaticBoxSizer *const sizer2 = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Dismiss Button"));

    m_radiobtnCancel = new wxRadioButton(this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_radiobtnClose = new wxRadioButton(this, wxID_ANY, _("Close"));

    wxBoxSizer *const sizer3 = new wxBoxSizer(wxHORIZONTAL);

    m_chkboxNo = new wxCheckBox(this, wxID_ANY, _("No"));
    m_chkboxHelp = new wxCheckBox(this, wxID_ANY, _("Help"));
    m_chkboxApply = new wxCheckBox(this, wxID_ANY, _("Apply"));

    m_chkboxNoDefault = new wxCheckBox(this, wxID_ANY, wxT("No Default"));

    sizer1->Add(m_radiobtnOk, 0, wxALL, 5);
    sizer1->Add(m_radiobtnYes, 0, wxALL, 5);

    sizer->Add(sizerInside1, 0, 0, 0);
    sizerInside1->Add(m_chkboxAffirmativeButton, 0, wxALL, 5);
    sizerInside1->Add(sizer1, 0, wxALL, 5);
    sizerInside1->SetItemMinSize(sizer1, sizer1->GetStaticBox()->GetBestSize());    // to prevent wrapping of static box label

    sizer2->Add(m_radiobtnCancel, 0, wxALL, 5);
    sizer2->Add(m_radiobtnClose, 0, wxALL, 5);

    sizer->Add(sizerInside2, 0, 0, 0);
    sizerInside2->Add(m_chkboxDismissButton, 0, wxALL, 5);
    sizerInside2->Add(sizer2, 0, wxALL, 5);
    sizerInside2->SetItemMinSize(sizer2, sizer2->GetStaticBox()->GetBestSize());    // to prevent wrapping of static box label

    sizerTop->Add(sizer, 0, wxALL, 5);

    sizer3->Add(m_chkboxNo, 0, wxALL, 5);
    sizer3->Add(m_chkboxHelp, 0, wxALL, 5);
    sizer3->Add(m_chkboxApply, 0, wxALL, 5);

    sizerTop->Add(sizer3, 0, wxALL, 5);

    sizerTop->Add(m_chkboxNoDefault, 0, wxLEFT|wxRIGHT, 10);

    EnableDisableControls();

    SetSizerAndFit(sizerTop);

    wxCommandEvent ev;
    OnEvent(ev);
}

void StdButtonSizerDialog::OnEvent(wxCommandEvent& WXUNUSED(event))
{
    if (m_buttonsSizer)
    {
        m_buttonsSizer->DeleteWindows();
        GetSizer()->Remove(m_buttonsSizer);
    }

    EnableDisableControls();

    long flags = 0;
    unsigned long numButtons = 0;

    if (m_chkboxAffirmativeButton->IsChecked())
    {
        if (m_radiobtnOk->GetValue())
        {
            flags |= wxOK;
            numButtons ++;
        }
        else if (m_radiobtnYes->GetValue())
        {
            flags |= wxYES;
            numButtons ++;
        }
    }

    if (m_chkboxDismissButton->IsChecked())
    {
        if (m_radiobtnCancel->GetValue())
        {
            flags |= wxCANCEL;
            numButtons ++;
        }

        else if (m_radiobtnClose->GetValue())
        {
            flags |= wxCLOSE;
            numButtons ++;
        }

    }

    if (m_chkboxApply->IsChecked())
    {
        flags |= wxAPPLY;
        numButtons ++;
    }

    if (m_chkboxNo->IsChecked())
    {
        flags |= wxNO;
        numButtons ++;
    }

    if (m_chkboxHelp->IsChecked())
    {
        flags |= wxHELP;
        numButtons ++;
    }

    if (m_chkboxNoDefault->IsChecked())
    {
        flags |= wxNO_DEFAULT;
    }

    m_buttonsSizer = CreateStdDialogButtonSizer(flags);
    GetSizer()->Add(m_buttonsSizer, 0, wxGROW|wxALL, 5);

    Layout();
    GetSizer()->SetSizeHints(this);
}

void StdButtonSizerDialog::EnableDisableControls()
{
    const bool affButtonEnabled = m_chkboxAffirmativeButton->IsChecked();

    m_radiobtnOk->Enable(affButtonEnabled);
    m_radiobtnYes->Enable(affButtonEnabled);

    const bool dismissButtonEnabled = m_chkboxDismissButton->IsChecked();

    m_radiobtnCancel->Enable(dismissButtonEnabled);
    m_radiobtnClose->Enable(dismissButtonEnabled);
}

#if USE_SETTINGS_DIALOG
// ----------------------------------------------------------------------------
// SettingsDialog
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(SettingsDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win, int dialogType)
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);

    int tabImage1 = -1;
    int tabImage2 = -1;

    bool useToolBook = (dialogType == DIALOGS_PROPERTY_SHEET_TOOLBOOK || dialogType == DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK);
    int resizeBorder = wxRESIZE_BORDER;

    if (useToolBook)
    {
        resizeBorder = 0;
        tabImage1 = 0;
        tabImage2 = 1;

        int sheetStyle = wxPROPSHEET_SHRINKTOFIT;
        if (dialogType == DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK)
            sheetStyle |= wxPROPSHEET_BUTTONTOOLBOOK;
        else
            sheetStyle |= wxPROPSHEET_TOOLBOOK;

        SetSheetStyle(sheetStyle);
        SetSheetInnerBorder(0);
        SetSheetOuterBorder(0);

        // create a dummy image list with a few icons
        const wxSize imageSize(32, 32);

        m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
        m_imageList->
            Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, imageSize));
        m_imageList->
            Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
        m_imageList->
            Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, imageSize));
        m_imageList->
            Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, imageSize));
    }
    else
        m_imageList = NULL;

    Create(win, wxID_ANY, _("Preferences"), wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE| (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, resizeBorder)
    );

    // If using a toolbook, also follow Mac style and don't create buttons
    if (!useToolBook)
        CreateButtons(wxOK | wxCANCEL |
                        (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxHELP)
    );

    wxBookCtrlBase* notebook = GetBookCtrl();
    notebook->SetImageList(m_imageList);

    wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
    wxPanel* aestheticSettings = CreateAestheticSettingsPage(notebook);

    notebook->AddPage(generalSettings, _("General"), true, tabImage1);
    notebook->AddPage(aestheticSettings, _("Aesthetics"), false, tabImage2);

    LayoutDialog();
}

SettingsDialog::~SettingsDialog()
{
    delete m_imageList;
}

wxPanel* SettingsDialog::CreateGeneralSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //// LOAD LAST FILE

    wxBoxSizer* itemSizer3 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* checkBox3 = new wxCheckBox(panel, ID_LOAD_LAST_PROJECT, _("&Load last project on startup"), wxDefaultPosition, wxDefaultSize);
    itemSizer3->Add(checkBox3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer3, 0, wxGROW|wxALL, 0);

    //// AUTOSAVE

    wxString autoSaveLabel = _("&Auto-save every");
    wxString minsLabel = _("mins");

    wxBoxSizer* itemSizer12 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* checkBox12 = new wxCheckBox(panel, ID_AUTO_SAVE, autoSaveLabel, wxDefaultPosition, wxDefaultSize);

#if wxUSE_SPINCTRL
    wxSpinCtrl* spinCtrl12 = new wxSpinCtrl(panel, ID_AUTO_SAVE_MINS, wxEmptyString,
        wxDefaultPosition, wxSize(40, wxDefaultCoord), wxSP_ARROW_KEYS, 1, 60, 1);
#endif

    itemSizer12->Add(checkBox12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
#if wxUSE_SPINCTRL
    itemSizer12->Add(spinCtrl12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
#endif
    itemSizer12->Add(new wxStaticText(panel, wxID_STATIC, minsLabel), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer12, 0, wxGROW|wxALL, 0);

    //// TOOLTIPS

    wxBoxSizer* itemSizer8 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* checkBox6 = new wxCheckBox(panel, ID_SHOW_TOOLTIPS, _("Show &tooltips"), wxDefaultPosition, wxDefaultSize);
    itemSizer8->Add(checkBox6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer8, 0, wxGROW|wxALL, 0);

    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

    panel->SetSizerAndFit(topSizer);

    return panel;
}

wxPanel* SettingsDialog::CreateAestheticSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //// PROJECT OR GLOBAL
    wxString globalOrProjectChoices[2];
    globalOrProjectChoices[0] = _("&New projects");
    globalOrProjectChoices[1] = _("&This project");

    wxRadioBox* projectOrGlobal = new wxRadioBox(panel, ID_APPLY_SETTINGS_TO, _("&Apply settings to:"),
        wxDefaultPosition, wxDefaultSize, 2, globalOrProjectChoices);
    item0->Add(projectOrGlobal, 0, wxGROW|wxALL, 5);

    projectOrGlobal->SetSelection(0);

    //// BACKGROUND STYLE
    wxArrayString backgroundStyleChoices;
    backgroundStyleChoices.Add(wxT("Colour"));
    backgroundStyleChoices.Add(wxT("Image"));
    wxStaticBox* staticBox3 = new wxStaticBox(panel, wxID_ANY, _("Background style:"));

    wxBoxSizer* styleSizer = new wxStaticBoxSizer( staticBox3, wxVERTICAL );
    item0->Add(styleSizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemSizer2 = new wxBoxSizer( wxHORIZONTAL );

    wxChoice* choice2 = new wxChoice(panel, ID_BACKGROUND_STYLE, wxDefaultPosition, wxDefaultSize, backgroundStyleChoices);

    itemSizer2->Add(new wxStaticText(panel, wxID_ANY, _("&Window:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    itemSizer2->Add(5, 5, 1, wxALL, 0);
    itemSizer2->Add(choice2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    styleSizer->Add(itemSizer2, 0, wxGROW|wxALL, 5);

#if wxUSE_SPINCTRL
    //// FONT SIZE SELECTION

    wxStaticBox* staticBox1 = new wxStaticBox(panel, wxID_ANY, _("Tile font size:"));
    wxBoxSizer* itemSizer5 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );

    wxSpinCtrl* spinCtrl = new wxSpinCtrl(panel, ID_FONT_SIZE, wxEmptyString, wxDefaultPosition,
        wxSize(80, wxDefaultCoord));
    itemSizer5->Add(spinCtrl, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    item0->Add(itemSizer5, 0, wxGROW|wxLEFT|wxRIGHT, 5);
#endif

    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizerAndFit(topSizer);

    return panel;
}

#endif // USE_SETTINGS_DIALOG

#if wxUSE_MSGDLG
// ----------------------------------------------------------------------------
// TestMessageBoxDialog
// ----------------------------------------------------------------------------

/* static */
const TestMessageBoxDialog::BtnInfo TestMessageBoxDialog::ms_btnInfo[] =
{
    { wxYES,    "&Yes"    },
    { wxNO,     "&No"     },
    { wxOK,     "&Ok"     },
    { wxCANCEL, "&Cancel" },
    { wxHELP,   "&Help"   },
};

BEGIN_EVENT_TABLE(TestMessageBoxDialog, wxDialog)
    EVT_BUTTON(wxID_APPLY, TestMessageBoxDialog::OnApply)
    EVT_BUTTON(wxID_CLOSE, TestMessageBoxDialog::OnClose)
END_EVENT_TABLE()

TestMessageBoxDialog::TestMessageBoxDialog(wxWindow *parent)
                    : wxDialog(parent, wxID_ANY, "Message Box Test Dialog",
                               wxDefaultPosition, wxDefaultSize,
                               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
}

bool TestMessageBoxDialog::Create()
{
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

    // this sizer allows to configure the messages shown in the message box
    wxSizer * const
        sizerMsgs = new wxStaticBoxSizer(wxVERTICAL, this, "&Messages");
    sizerMsgs->Add(new wxStaticText(this, wxID_ANY, "&Main message:"));
    m_textMsg = new wxTextCtrl(this, wxID_ANY, "Hello from a box!",
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE);
    sizerMsgs->Add(m_textMsg, wxSizerFlags(1).Expand().Border(wxBOTTOM));

    sizerMsgs->Add(new wxStaticText(this, wxID_ANY, "&Extended message:"));
    m_textExtMsg = new wxTextCtrl(this, wxID_ANY, "",
                                  wxDefaultPosition, wxDefaultSize,
                                  wxTE_MULTILINE);
    sizerMsgs->Add(m_textExtMsg, wxSizerFlags(1).Expand());

    sizerTop->Add(sizerMsgs, wxSizerFlags(1).Expand().Border());

    // if a derived class provides more message configurations, add these.
    AddAdditionalTextOptions(sizerTop);

    // this one is for configuring the buttons
    wxSizer * const
    sizerBtnsBox = new wxStaticBoxSizer(wxVERTICAL, this, "&Buttons");

    wxFlexGridSizer * const sizerBtns = new wxFlexGridSizer(2, 5, 5);
    sizerBtns->AddGrowableCol(1);

    sizerBtns->Add(new wxStaticText(this, wxID_ANY, "Button(s)"));
    sizerBtns->Add(new wxStaticText(this, wxID_ANY, "Custom label"));

    for ( int n = 0; n < Btn_Max; n++ )
    {
        m_buttons[n] = new wxCheckBox(this, wxID_ANY, ms_btnInfo[n].name);
        sizerBtns->Add(m_buttons[n], wxSizerFlags().Centre().Left());

        m_labels[n] = new wxTextCtrl(this, wxID_ANY);
        sizerBtns->Add(m_labels[n], wxSizerFlags(1).Centre().Expand());

        m_labels[n]->Connect(wxEVT_UPDATE_UI,
                             wxUpdateUIEventHandler(
                                 TestMessageBoxDialog::OnUpdateLabelUI),
                             NULL,
                             this);
    }

    sizerBtnsBox->Add(sizerBtns, wxSizerFlags(1).Expand());
    sizerTop->Add(sizerBtnsBox, wxSizerFlags().Expand().Border());


    // icon choice
    const wxString icons[] =
    {
        "&Not specified",
        "E&xplicitly none",
        "&Information icon",
        "&Question icon",
        "&Warning icon",
        "&Error icon"
    };

   wxCOMPILE_TIME_ASSERT( WXSIZEOF(icons) == MsgDlgIcon_Max, IconMismatch );

    m_icons = new wxRadioBox(this, wxID_ANY, "&Icon style",
                             wxDefaultPosition, wxDefaultSize,
                             WXSIZEOF(icons), icons,
                             2, wxRA_SPECIFY_ROWS);
    // Make the 'Information' icon the default one:
    m_icons->SetSelection(MsgDlgIcon_Info);
    sizerTop->Add(m_icons, wxSizerFlags().Expand().Border());


    // miscellaneous other stuff
    wxSizer * const
        sizerFlags = new wxStaticBoxSizer(wxHORIZONTAL, this, "&Other flags");

    m_chkNoDefault = new wxCheckBox(this, wxID_ANY, "Make \"No\" &default");
    m_chkNoDefault->Connect(wxEVT_UPDATE_UI,
                            wxUpdateUIEventHandler(
                                TestMessageBoxDialog::OnUpdateNoDefaultUI),
                            NULL,
                            this);
    sizerFlags->Add(m_chkNoDefault, wxSizerFlags(1).Border());

    m_chkCentre = new wxCheckBox(this, wxID_ANY, "Centre on &parent");
    sizerFlags->Add(m_chkCentre, wxSizerFlags(1).Border());

    // add any additional flag from subclasses
    AddAdditionalFlags(sizerFlags);

    sizerTop->Add(sizerFlags, wxSizerFlags().Expand().Border());

    // finally buttons to show the resulting message box and close this dialog
    sizerTop->Add(CreateStdDialogButtonSizer(wxAPPLY | wxCLOSE),
                  wxSizerFlags().Right().Border());

    SetSizerAndFit(sizerTop);

    m_buttons[Btn_Ok]->SetValue(true);

    return true;
}

void TestMessageBoxDialog::OnUpdateLabelUI(wxUpdateUIEvent& event)
{
    for ( int n = 0; n < Btn_Max; n++ )
    {
        if ( event.GetEventObject() == m_labels[n] )
        {
            event.Enable( m_buttons[n]->IsChecked() );
            return;
        }
    }

    wxFAIL_MSG( "called for unknown label" );
}

void TestMessageBoxDialog::OnUpdateNoDefaultUI(wxUpdateUIEvent& event)
{
    event.Enable( m_buttons[Btn_No]->IsChecked() );
}

long TestMessageBoxDialog::GetStyle()
{
    long style = 0;

    for ( int n = 0; n < Btn_Max; n++ )
    {
        if ( m_buttons[n]->IsChecked() )
            style |= ms_btnInfo[n].flag;
    }

    switch ( m_icons->GetSelection() )
    {
        case MsgDlgIcon_Max:
            wxFAIL_MSG( "unexpected selection" );

        case MsgDlgIcon_No:
            break;

        case MsgDlgIcon_None:
            style |= wxICON_NONE;
            break;

        case MsgDlgIcon_Info:
            style |= wxICON_INFORMATION;
            break;

        case MsgDlgIcon_Question:
            style |= wxICON_QUESTION;
            break;

        case MsgDlgIcon_Warning:
            style |= wxICON_WARNING;
            break;

        case MsgDlgIcon_Error:
            style |= wxICON_ERROR;
            break;
    }

    if ( m_chkCentre->IsChecked() )
        style |= wxCENTRE;

    if ( m_chkNoDefault->IsEnabled() && m_chkNoDefault->IsChecked() )
        style |= wxNO_DEFAULT;

    return style;
}

void TestMessageBoxDialog::PrepareMessageDialog(wxMessageDialogBase &dlg)
{
    long style = dlg.GetMessageDialogStyle();

    if ( !m_textExtMsg->IsEmpty() )
        dlg.SetExtendedMessage(m_textExtMsg->GetValue());

    if ( style & wxYES_NO )
    {
        if ( style & wxCANCEL )
        {
            dlg.SetYesNoCancelLabels(m_labels[Btn_Yes]->GetValue(),
                                     m_labels[Btn_No]->GetValue(),
                                     m_labels[Btn_Cancel]->GetValue());
        }
        else
        {
            dlg.SetYesNoLabels(m_labels[Btn_Yes]->GetValue(),
                               m_labels[Btn_No]->GetValue());
        }
    }
    else
    {
        if ( style & wxCANCEL )
        {
            dlg.SetOKCancelLabels(m_labels[Btn_Ok]->GetValue(),
                                  m_labels[Btn_Cancel]->GetValue());
        }
        else
        {
            dlg.SetOKLabel(m_labels[Btn_Ok]->GetValue());
        }
    }

    if ( style & wxHELP )
    {
        dlg.SetHelpLabel(m_labels[Btn_Help]->GetValue());
    }
}

void TestMessageBoxDialog::OnApply(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog dlg(this, GetMessage(), "Test Message Box", GetStyle());
    PrepareMessageDialog(dlg);

    wxString btnName;
    switch ( dlg.ShowModal() )
    {
        case wxID_OK:
            btnName = "OK";
            break;

        case wxID_CANCEL:
            // Avoid the extra message box if the dialog was cancelled.
            return;

        case wxID_YES:
            btnName = "Yes";
            break;

        case wxID_NO:
            btnName = "No";
            break;

        case wxID_HELP:
            btnName = "Help";
            break;

        default:
            btnName = "Unknown";
    }

    wxLogMessage("Dialog was closed with the \"%s\" button.", btnName);
}

void TestMessageBoxDialog::OnClose(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}
#endif // wxUSE_MSGDLG

#if wxUSE_RICHMSGDLG
// ----------------------------------------------------------------------------
// TestRichMessageDialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TestRichMessageDialog, TestMessageBoxDialog)
    EVT_BUTTON(wxID_APPLY, TestRichMessageDialog::OnApply)
END_EVENT_TABLE()

TestRichMessageDialog::TestRichMessageDialog(wxWindow *parent)
                     : TestMessageBoxDialog(parent)
{
    SetTitle("Rich Message Dialog Test Dialog");
}

void TestRichMessageDialog::AddAdditionalTextOptions(wxSizer *sizer)
{
    wxSizer * const sizerMsgs = new wxStaticBoxSizer(wxVERTICAL, this,
                                                     "&Additional Elements");

    // add a option to show a check box.
    wxFlexGridSizer * const sizerCheckBox = new wxFlexGridSizer(2, 5, 5);
    sizerCheckBox->AddGrowableCol(1);
    sizerCheckBox->Add(new wxStaticText(this, wxID_ANY, "&Check box:"));
    m_textCheckBox = new wxTextCtrl(this, wxID_ANY);
    sizerCheckBox->Add(m_textCheckBox, wxSizerFlags(1).Expand().Border(wxBOTTOM));
    sizerMsgs->Add(sizerCheckBox, wxSizerFlags(1).Expand());

    // add option to show a detailed text.
    sizerMsgs->Add(new wxStaticText(this, wxID_ANY, "&Detailed message:"));
    m_textDetailed = new wxTextCtrl(this, wxID_ANY, "",
                                    wxDefaultPosition, wxDefaultSize,
                                    wxTE_MULTILINE);
    sizerMsgs->Add(m_textDetailed, wxSizerFlags(1).Expand());

    sizer->Add(sizerMsgs, wxSizerFlags(1).Expand().Border());
}

void TestRichMessageDialog::AddAdditionalFlags(wxSizer *sizer)
{
    // add checkbox to set the initial state for the checkbox shown
    // in the dialog.
    m_initialValueCheckBox =
        new wxCheckBox(this, wxID_ANY, "Checkbox initially checked");
    sizer->Add(m_initialValueCheckBox, wxSizerFlags(1).Border());
}

void TestRichMessageDialog::OnApply(wxCommandEvent& WXUNUSED(event))
{
    wxRichMessageDialog dlg(this, GetMessage(), "Test Rich Message Dialog",
                            GetStyle());
    PrepareMessageDialog(dlg);

    dlg.ShowCheckBox(m_textCheckBox->GetValue(),
                     m_initialValueCheckBox->GetValue());
    dlg.ShowDetailedText(m_textDetailed->GetValue());

    dlg.ShowModal();
}

#endif // wxUSE_RICHMSGDLG

#if wxUSE_LOG

// ----------------------------------------------------------------------------
// custom log target
// ----------------------------------------------------------------------------

class MyLogGui : public wxLogGui
{
private:
    virtual void DoShowSingleLogMessage(const wxString& message,
                                        const wxString& title,
                                        int style)
    {
        wxMessageDialog dlg(NULL, message, title,
                            wxOK | wxCANCEL | wxCANCEL_DEFAULT | style);
        dlg.SetOKCancelLabels(wxID_COPY, wxID_OK);
        dlg.SetExtendedMessage("Note that this is a custom log dialog.");
        dlg.ShowModal();
    }
};

wxLog *MyAppTraits::CreateLogTarget()
{
    return new MyLogGui;
}

#endif // wxUSE_LOG
