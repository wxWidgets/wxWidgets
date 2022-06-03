/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.cpp
// Purpose:     Common dialogs demo
// Author:      Julian Smart, Vadim Zeitlin, ABX
// Created:     04/01/98
// Copyright:   (c) Julian Smart
//              (c) 2004 ABX
//              (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../sample.xpm"

#include "wx/apptrait.h"
#include "wx/datetime.h"
#include "wx/filename.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/minifram.h"
#include "wx/sysopt.h"
#include "wx/notifmsg.h"
#include "wx/generic/notifmsg.h"
#include "wx/modalhook.h"

#if defined(__WXMSW__) && wxUSE_TASKBARICON
#include "wx/taskbar.h"
#endif

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
#include "wx/addremovectrl.h"

#if wxUSE_STARTUP_TIPS
    #include "wx/tipdlg.h"
#endif // wxUSE_STARTUP_TIPS

#if wxUSE_TIPWINDOW
    #include "wx/tipwin.h"
#endif // wxUSE_TIPWINDOW

#if wxUSE_PROGRESSDLG
#if wxUSE_STOPWATCH && wxUSE_LONGLONG
    #include "wx/datetime.h"      // wxDateTime
#endif

    #include "wx/progdlg.h"
#endif // wxUSE_PROGRESSDLG

#include "wx/appprogress.h"

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
    #include "wx/filedlgcustomize.h"
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
#include "wx/valgen.h"

#include "dialogs.h"

#if wxUSE_CREDENTIALDLG
    #include "wx/creddlg.h"
#endif

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

wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
wxEND_EVENT_TABLE()



wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
#if wxUSE_MSGDLG
    EVT_MENU(DIALOGS_MESSAGE_BOX,                   MyFrame::MessageBox)
    EVT_MENU(DIALOGS_MESSAGE_BOX_WINDOW_MODAL,      MyFrame::MessageBoxWindowModal)
    EVT_MENU(DIALOGS_MESSAGE_DIALOG,                MyFrame::MessageBoxDialog)
    EVT_MENU(DIALOGS_MESSAGE_DIALOG_WINDOW_MODAL,   MyFrame::MessageBoxDialogWindowModal)
    EVT_MENU(DIALOGS_MESSAGE_BOX_WXINFO,            MyFrame::MessageBoxInfo)
#endif // wxUSE_MSGDLG
#if wxUSE_RICHMSGDLG
    EVT_MENU(DIALOGS_RICH_MESSAGE_DIALOG,           MyFrame::RichMessageDialog)
#endif // wxUSE_RICHMSGDLG
#if wxUSE_COLOURDLG
    EVT_MENU(DIALOGS_CHOOSE_COLOUR,                 MyFrame::ChooseColour)
    EVT_MENU(DIALOGS_CHOOSE_COLOUR_ALPHA,           MyFrame::ChooseColour)
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
    EVT_MENU(DIALOGS_INFOBAR_SIMPLE_WRAPPED,        MyFrame::InfoBarSimpleWrapped)
    EVT_MENU(DIALOGS_INFOBAR_ADVANCED,              MyFrame::InfoBarAdvanced)
#endif // wxUSE_INFOBAR

#if wxUSE_TEXTDLG
    EVT_MENU(DIALOGS_LINE_ENTRY,                    MyFrame::LineEntry)
    EVT_MENU(DIALOGS_TEXT_ENTRY,                    MyFrame::TextEntry)
    EVT_MENU(DIALOGS_PASSWORD_ENTRY,                MyFrame::PasswordEntry)
#endif // wxUSE_TEXTDLG

#if wxUSE_CREDENTIALDLG
    EVT_MENU(DIALOGS_CREDENTIAL_ENTRY,              MyFrame::CredentialEntry)
#endif // wxUSE_CREDENTIALDLG

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

#if wxUSE_ADDREMOVECTRL
    EVT_MENU(DIALOGS_ADDREMOVE,                     MyFrame::AddRemove)
#endif // wxUSE_ADDREMOVECTRL

#if wxUSE_FILEDLG
    EVT_MENU(DIALOGS_FILE_OPEN,                     MyFrame::FileOpen)
    EVT_MENU(DIALOGS_FILE_OPEN2,                    MyFrame::FileOpen2)
    EVT_MENU(DIALOGS_FILES_OPEN,                    MyFrame::FilesOpen)
    EVT_MENU(DIALOGS_FILES_OPEN_WINDOW_MODAL,       MyFrame::FilesOpenWindowModal)
    EVT_MENU(DIALOGS_FILE_SAVE,                     MyFrame::FileSave)
    EVT_MENU(DIALOGS_FILE_SAVE_WINDOW_MODAL,        MyFrame::FileSaveWindowModal)
    EVT_MENU(DIALOGS_MAC_TOGGLE_ALWAYS_SHOW_TYPES,  MyFrame::MacToggleAlwaysShowTypes)
#endif // wxUSE_FILEDLG

#if USE_FILEDLG_GENERIC
    EVT_MENU(DIALOGS_FILE_OPEN_GENERIC,             MyFrame::FileOpenGeneric)
    EVT_MENU(DIALOGS_FILES_OPEN_GENERIC,            MyFrame::FilesOpenGeneric)
    EVT_MENU(DIALOGS_FILE_SAVE_GENERIC,             MyFrame::FileSaveGeneric)
#endif // USE_FILEDLG_GENERIC

#if wxUSE_DIRDLG
    EVT_MENU(DIALOGS_DIR_CHOOSE,                    MyFrame::DirChoose)
    EVT_MENU(DIALOGS_DIR_CHOOSE_WINDOW_MODAL,       MyFrame::DirChooseWindowModal)
    EVT_MENU(DIALOGS_DIRNEW_CHOOSE,                 MyFrame::DirChooseNew)
    EVT_MENU(DIALOGS_DIRMULTIPLE_CHOOSE,            MyFrame::DirChooseMultiple)
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
    EVT_MENU(DIALOGS_CHOOSE_COLOUR_GENERIC_ALPHA,   MyFrame::ChooseColourGeneric)
#endif // USE_COLOURDLG_GENERIC

#if wxUSE_PROGRESSDLG
    EVT_MENU(DIALOGS_PROGRESS,                      MyFrame::ShowProgress)
#ifdef wxHAS_NATIVE_PROGRESSDIALOG
    EVT_MENU(DIALOGS_PROGRESS_GENERIC,              MyFrame::ShowProgressGeneric)
#endif // wxHAS_NATIVE_PROGRESSDIALOG
#endif // wxUSE_PROGRESSDLG

    EVT_MENU(DIALOGS_APP_PROGRESS,                  MyFrame::ShowAppProgress)

#if wxUSE_ABOUTDLG
    EVT_MENU(DIALOGS_ABOUTDLG_SIMPLE,               MyFrame::ShowSimpleAboutDialog)
    EVT_MENU(DIALOGS_ABOUTDLG_FANCY,                MyFrame::ShowFancyAboutDialog)
    EVT_MENU(DIALOGS_ABOUTDLG_FULL,                 MyFrame::ShowFullAboutDialog)
    EVT_MENU(DIALOGS_ABOUTDLG_CUSTOM,               MyFrame::ShowCustomAboutDialog)
#endif // wxUSE_ABOUTDLG

#if wxUSE_BUSYINFO
    EVT_MENU(DIALOGS_BUSYINFO,                      MyFrame::ShowBusyInfo)
    EVT_MENU(DIALOGS_BUSYINFO_RICH,                 MyFrame::ShowRichBusyInfo)
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
    EVT_MENU(DIALOGS_MODAL_HOOK,                    MyFrame::OnModalHook)
    EVT_MENU(DIALOGS_SIMULATE_UNSAVED,              MyFrame::OnSimulatedUnsaved)

    EVT_MENU(DIALOGS_REQUEST,                       MyFrame::OnRequestUserAttention)
#if wxUSE_NOTIFICATION_MESSAGE
    EVT_MENU(DIALOGS_NOTIFY_MSG,                    MyFrame::OnNotifMsg)
#endif // wxUSE_NOTIFICATION_MESSAGE

#if wxUSE_TIPWINDOW
    EVT_MENU(DIALOGS_SHOW_TIP,                      MyFrame::OnShowTip)
    EVT_UPDATE_UI(DIALOGS_SHOW_TIP,                 MyFrame::OnUpdateShowTipUI)
#endif // wxUSE_TIPWINDOW
#if wxUSE_RICHTOOLTIP
    EVT_MENU(DIALOGS_RICHTIP_DIALOG,                MyFrame::OnRichTipDialog)
#endif // wxUSE_RICHTOOLTIP

    EVT_MENU(wxID_EXIT,                             MyFrame::OnExit)
    EVT_CLOSE(                                      MyFrame::OnClose)
wxEND_EVENT_TABLE()

#if USE_MODAL_PRESENTATION

    wxBEGIN_EVENT_TABLE(MyModalDialog, wxDialog)
        EVT_BUTTON(wxID_ANY, MyModalDialog::OnButton)
   wxEND_EVENT_TABLE()

    wxBEGIN_EVENT_TABLE(MyModelessDialog, wxDialog)
        EVT_BUTTON(DIALOGS_MODELESS_BTN, MyModelessDialog::OnButton)
        EVT_CLOSE(MyModelessDialog::OnClose)
   wxEND_EVENT_TABLE()

#endif // USE_MODAL_PRESENTATION

wxBEGIN_EVENT_TABLE(StdButtonSizerDialog, wxDialog)
    EVT_CHECKBOX(wxID_ANY, StdButtonSizerDialog::OnEvent)
    EVT_RADIOBUTTON(wxID_ANY, StdButtonSizerDialog::OnEvent)
wxEND_EVENT_TABLE()

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
            wxString msg;
            switch ( i )
            {
                case 15:
                    msg = "And the same dialog but with a very, very, very long"
                          " message, just to test how it appears in this case.";
                    break;

                case 30:
                    msg = "Back to brevity";
                    break;

                case 80:
                    msg = "Back and adjusted";
                    dlg.Fit();
                    break;
            }

            if ( !dlg.Update(i, msg) )
                break;

            wxMilliSleep(50);
        }
    }
#endif // wxUSE_PROGRESSDLG

#if wxUSE_IMAGE
    wxInitAllImageHandlers();
#endif

    // Create the main frame window
    MyFrame *frame = new MyFrame("wxWidgets dialogs example");

    // Make a menubar
    wxMenu *menuDlg = new wxMenu;

    menuDlg->Append(DIALOGS_MESSAGE_BOX, "&Message box\tCtrl-M");
    menuDlg->Append(DIALOGS_MESSAGE_BOX_WINDOW_MODAL, "Window-Modal Message box ");
    menuDlg->Append(DIALOGS_MESSAGE_DIALOG, "Message dialog\tShift-Ctrl-M");
    menuDlg->Append(DIALOGS_MESSAGE_DIALOG_WINDOW_MODAL, "Window-Modal Message dialog");
#if wxUSE_RICHMSGDLG
    menuDlg->Append(DIALOGS_RICH_MESSAGE_DIALOG, "Rich message dialog");
#endif // wxUSE_RICHMSGDLG


#if wxUSE_COLOURDLG || wxUSE_FONTDLG || wxUSE_CHOICEDLG

    wxMenu *choices_menu = new wxMenu;

    #if wxUSE_COLOURDLG
        wxMenu *choices_bg_colour = new wxMenu;
        choices_bg_colour->Append(DIALOGS_CHOOSE_COLOUR, "&No opacity");
        choices_bg_colour->Append(DIALOGS_CHOOSE_COLOUR_ALPHA, "&With opacity");
        choices_menu->Append(wxID_ANY, "&Choose bg colour", choices_bg_colour);
        choices_menu->Append(DIALOGS_GET_COLOUR, "&Choose fg colour");
    #endif // wxUSE_COLOURDLG

    #if wxUSE_FONTDLG
        choices_menu->Append(DIALOGS_CHOOSE_FONT, "Choose &font\tShift-Ctrl-N");
    #endif // wxUSE_FONTDLG

    #if wxUSE_CHOICEDLG
        choices_menu->Append(DIALOGS_SINGLE_CHOICE,  "&Single choice\tCtrl-C");
        choices_menu->Append(DIALOGS_MULTI_CHOICE,  "M&ultiple choice\tCtrl-U");
    #endif // wxUSE_CHOICEDLG

    #if wxUSE_REARRANGECTRL
        choices_menu->Append(DIALOGS_REARRANGE,  "&Rearrange dialog\tCtrl-R");
    #endif // wxUSE_REARRANGECTRL

    #if wxUSE_ADDREMOVECTRL
        choices_menu->Append(DIALOGS_ADDREMOVE, "&Add/remove items control\tCtrl-A");
    #endif // wxUSE_ADDREMOVECTRL

    #if USE_COLOURDLG_GENERIC || USE_FONTDLG_GENERIC
        choices_menu->AppendSeparator();
    #endif // USE_COLOURDLG_GENERIC || USE_FONTDLG_GENERIC

    #if USE_COLOURDLG_GENERIC
        wxMenu *colourGeneric_menu = new wxMenu;
        colourGeneric_menu->Append(DIALOGS_CHOOSE_COLOUR_GENERIC, "&No opacity");
        colourGeneric_menu->Append(DIALOGS_CHOOSE_COLOUR_GENERIC_ALPHA, "&With opacity");
        choices_menu->Append(wxID_ANY, "&Choose colour (generic)", colourGeneric_menu);
    #endif // USE_COLOURDLG_GENERIC

    #if USE_FONTDLG_GENERIC
        choices_menu->Append(DIALOGS_CHOOSE_FONT_GENERIC, "Choose &font (generic)");
    #endif // USE_FONTDLG_GENERIC

    menuDlg->Append(wxID_ANY,"&Choices and selectors",choices_menu);
#endif // wxUSE_COLOURDLG || wxUSE_FONTDLG || wxUSE_CHOICEDLG


#if wxUSE_TEXTDLG || wxUSE_NUMBERDLG || wxUSE_CREDENTIALDLG

    wxMenu *entry_menu = new wxMenu;

    #if wxUSE_TEXTDLG
        entry_menu->Append(DIALOGS_LINE_ENTRY,  "Single line &entry\tCtrl-E");
        entry_menu->Append(DIALOGS_TEXT_ENTRY,  "Multi line text &entry\tShift-Ctrl-E");
        entry_menu->Append(DIALOGS_PASSWORD_ENTRY,  "&Password entry\tCtrl-P");
    #endif // wxUSE_TEXTDLG

    #if wxUSE_CREDENTIALDLG
        entry_menu->Append(DIALOGS_CREDENTIAL_ENTRY, "&Credential entry\tShift-Ctrl-C");
    #endif // wxUSE_CREDENTIALDLG

    #if wxUSE_NUMBERDLG
        entry_menu->Append(DIALOGS_NUM_ENTRY, "&Numeric entry\tCtrl-N");
    #endif // wxUSE_NUMBERDLG

    menuDlg->Append(wxID_ANY,"&Entry dialogs",entry_menu);

#endif // wxUSE_TEXTDLG || wxUSE_NUMBERDLG


#if wxUSE_FILEDLG

    wxMenu *filedlg_menu = new wxMenu;
    filedlg_menu->Append(DIALOGS_FILE_OPEN,  "&Open file\tCtrl-O");
    filedlg_menu->Append(DIALOGS_FILE_OPEN2,  "&Second open file\tCtrl-2");
    filedlg_menu->Append(DIALOGS_FILES_OPEN,  "Open &files\tShift-Ctrl-O");
    filedlg_menu->Append(DIALOGS_FILES_OPEN_WINDOW_MODAL, "Window-Modal Open files");
    filedlg_menu->Append(DIALOGS_FILE_SAVE,  "Sa&ve file\tCtrl-S");
    filedlg_menu->Append(DIALOGS_FILE_SAVE_WINDOW_MODAL,  "Window-Modal Save file");

#if USE_FILEDLG_GENERIC
    filedlg_menu->AppendSeparator();
    filedlg_menu->Append(DIALOGS_FILE_OPEN_GENERIC, "&Open file (generic)");
    filedlg_menu->Append(DIALOGS_FILES_OPEN_GENERIC, "Open &files (generic)");
    filedlg_menu->Append(DIALOGS_FILE_SAVE_GENERIC, "Sa&ve file (generic)");
#endif // USE_FILEDLG_GENERIC

    filedlg_menu->AppendSeparator();
    filedlg_menu->AppendRadioItem(
        DIALOGS_FILE_USE_CUSTOMIZER,
       "Use new customization API",
       "Use wxFileDialog::SetCustomizeHook() for file dialog customization"
    );
    filedlg_menu->AppendRadioItem(
        DIALOGS_FILE_USE_EXTRA_CONTROL_CREATOR,
       "Use old customization API",
       "Use wxFileDialog::SetExtraControlCreator() for file dialog customization"
    );
#ifdef __WXOSX_COCOA__
    filedlg_menu->AppendSeparator();
    filedlg_menu->AppendCheckItem(DIALOGS_MAC_TOGGLE_ALWAYS_SHOW_TYPES,
                                  "macOS only: Toggle open file "
                                    "\"Always show types\"\tRawCtrl+Ctrl+S");
#endif

    menuDlg->Append(wxID_ANY,"&File operations",filedlg_menu);

#endif // wxUSE_FILEDLG

#if wxUSE_DIRDLG
    wxMenu *dir_menu = new wxMenu;

    dir_menu->Append(DIALOGS_DIR_CHOOSE,  "&Choose a directory\tCtrl-D");
    dir_menu->Append(DIALOGS_DIR_CHOOSE_WINDOW_MODAL,  "Choose a directory window-modally");
    dir_menu->Append(DIALOGS_DIRNEW_CHOOSE,  "Choose a directory (with \"Ne&w\" button)\tShift-Ctrl-D");
    dir_menu->Append(DIALOGS_DIRMULTIPLE_CHOOSE,  "Choose multiple and hidden directories\tAlt-Ctrl-D");
    menuDlg->Append(wxID_ANY,"&Directory operations",dir_menu);

    #if USE_DIRDLG_GENERIC
        dir_menu->AppendSeparator();
        dir_menu->Append(DIALOGS_GENERIC_DIR_CHOOSE,  "&Choose a directory (generic)");
    #endif // USE_DIRDLG_GENERIC

#endif // wxUSE_DIRDLG


#if wxUSE_STARTUP_TIPS || \
    wxUSE_PROGRESSDLG || \
    wxUSE_BUSYINFO || \
    wxUSE_LOG_DIALOG || \
    wxUSE_MSGDLG

    wxMenu *info_menu = new wxMenu;

    #if wxUSE_STARTUP_TIPS
        info_menu->Append(DIALOGS_TIP,  "&Tip of the day\tCtrl-T");
    #endif // wxUSE_STARTUP_TIPS

    #if wxUSE_PROGRESSDLG
        info_menu->Append(DIALOGS_PROGRESS, "Pro&gress dialog\tCtrl-G");
        #ifdef wxHAS_NATIVE_PROGRESSDIALOG
            info_menu->Append(DIALOGS_PROGRESS_GENERIC,
                              "Generic progress dialog\tCtrl-Alt-G");
        #endif // wxHAS_NATIVE_PROGRESSDIALOG
    #endif // wxUSE_PROGRESSDLG

        info_menu->Append(DIALOGS_APP_PROGRESS, "&App progress\tShift-Ctrl-G");

    #if wxUSE_BUSYINFO
       info_menu->Append(DIALOGS_BUSYINFO, "&Busy info dialog\tCtrl-B");
       info_menu->Append(DIALOGS_BUSYINFO_RICH, "&Rich busy info dialog\tShift-Ctrl-B");
    #endif // wxUSE_BUSYINFO

    #if wxUSE_LOG_DIALOG
       info_menu->Append(DIALOGS_LOG_DIALOG, "&Log dialog\tCtrl-L");
    #endif // wxUSE_LOG_DIALOG

    #if wxUSE_INFOBAR
       info_menu->Append(DIALOGS_INFOBAR_SIMPLE, "Simple &info bar\tCtrl-I");
       info_menu->Append(DIALOGS_INFOBAR_SIMPLE_WRAPPED, "Simple info bar with wrapped text");
       info_menu->Append(DIALOGS_INFOBAR_ADVANCED, "&Advanced info bar\tShift-Ctrl-I");
    #endif // wxUSE_INFOBAR

    #if wxUSE_MSGDLG
        info_menu->Append(DIALOGS_MESSAGE_BOX_WXINFO,
                             "&wxWidgets information\tCtrl-W");
    #endif // wxUSE_MSGDLG

    menuDlg->Append(wxID_ANY,"&Informative dialogs",info_menu);

#endif // wxUSE_STARTUP_TIPS || wxUSE_PROGRESSDLG || wxUSE_BUSYINFO || wxUSE_LOG_DIALOG


#if wxUSE_FINDREPLDLG
    wxMenu *find_menu = new wxMenu;
    find_menu->AppendCheckItem(DIALOGS_FIND, "&Find dialog\tCtrl-F");
    find_menu->AppendCheckItem(DIALOGS_REPLACE, "Find and &replace dialog\tShift-Ctrl-F");
    menuDlg->Append(wxID_ANY,"&Searching",find_menu);
#endif // wxUSE_FINDREPLDLG

    wxMenu *dialogs_menu = new wxMenu;
#if USE_MODAL_PRESENTATION
    dialogs_menu->Append(DIALOGS_MODAL, "&Modal dialog\tShift-Ctrl-W");
#endif // USE_MODAL_PRESENTATION
    dialogs_menu->AppendCheckItem(DIALOGS_MODELESS, "Mode&less dialog\tShift-Ctrl-Z");
    dialogs_menu->Append(DIALOGS_CENTRE_SCREEN, "Centered on &screen\tShift-Ctrl-1");
    dialogs_menu->Append(DIALOGS_CENTRE_PARENT, "Centered on &parent\tShift-Ctrl-2");
#if wxUSE_MINIFRAME
    dialogs_menu->Append(DIALOGS_MINIFRAME, "&Mini frame");
#endif // wxUSE_MINIFRAME
    dialogs_menu->Append(DIALOGS_ONTOP, "Dialog staying on &top");
    menuDlg->Append(wxID_ANY, "&Generic dialogs", dialogs_menu);

#if USE_SETTINGS_DIALOG
    wxMenu *sheet_menu = new wxMenu;
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET, "&Standard property sheet\tShift-Ctrl-P");
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET_TOOLBOOK, "&Toolbook sheet\tShift-Ctrl-T");

    if (wxPlatformIs(wxPORT_MAC))
        sheet_menu->Append(DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK, "Button &Toolbook sheet\tShift-Ctrl-U");
/*
#ifdef __WXMAC__
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK, "Button &Toolbook sheet\tShift-Ctrl-U");
#endif
*/
    menuDlg->Append(wxID_ANY, "&Property sheets", sheet_menu);
#endif // USE_SETTINGS_DIALOG

    wxMenu *menuNotif = new wxMenu;
    menuNotif->Append(DIALOGS_REQUEST, "&Request user attention\tCtrl-Shift-R");
#if wxUSE_NOTIFICATION_MESSAGE
    menuNotif->AppendSeparator();
    menuNotif->Append(DIALOGS_NOTIFY_MSG, "User &Notification\tCtrl-Shift-N");
#endif // wxUSE_NOTIFICATION_MESSAGE
    menuDlg->AppendSubMenu(menuNotif, "&User notifications");

#if wxUSE_TIPWINDOW
    menuDlg->AppendCheckItem(DIALOGS_SHOW_TIP, "Show &tip window\tShift-Ctrl-H");
#endif // wxUSE_TIPWINDOW

#if wxUSE_RICHTOOLTIP
    menuDlg->Append(DIALOGS_RICHTIP_DIALOG, "Rich &tooltip dialog...\tCtrl-H");
    menuDlg->AppendSeparator();
#endif // wxUSE_RICHTOOLTIP

    menuDlg->Append(DIALOGS_STANDARD_BUTTON_SIZER_DIALOG, "&Standard Buttons Sizer Dialog");
    menuDlg->Append(DIALOGS_TEST_DEFAULT_ACTION, "&Test dialog default action");
    menuDlg->AppendCheckItem(DIALOGS_MODAL_HOOK, "Enable modal dialog hook");
    menuDlg->AppendCheckItem(DIALOGS_SIMULATE_UNSAVED, "Simulate an unsaved document at exit");

    menuDlg->AppendSeparator();
    menuDlg->Append(wxID_EXIT, "E&xit\tAlt-X");

#if wxUSE_ABOUTDLG
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(DIALOGS_ABOUTDLG_SIMPLE, "&About (simple)...\tF1");
    menuHelp->Append(DIALOGS_ABOUTDLG_FANCY, "About (&fancy)...\tShift-F1");
    menuHelp->Append(DIALOGS_ABOUTDLG_FULL, "About (f&ull)...\tCtrl-F1");
    menuHelp->Append(DIALOGS_ABOUTDLG_CUSTOM, "About (&custom)...\tCtrl-Shift-F1");
#endif // wxUSE_ABOUTDLG

    wxMenu* editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO, "&Undo\tCtrl+Z");
    editMenu->Append(wxID_REDO, "&Redo\tCtrl+Y");
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, "Cu&t\tCtrl+X");
    editMenu->Append(wxID_COPY, "&Copy\tCtrl+C");
    editMenu->Append(wxID_PASTE, "&Paste\tCtrl+V");
    editMenu->Append(wxID_CLEAR, "&Delete");

    editMenu->AppendSeparator();
    editMenu->Append(wxID_SELECTALL, "Select All\tCtrl+A");

    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append(menuDlg, "&Dialogs");

    menubar->Append(editMenu, "&Edit");

#if wxUSE_ABOUTDLG
    menubar->Append(menuHelp, "&Help");
#endif // wxUSE_ABOUTDLG

    frame->SetMenuBar(menubar);

    frame->Centre(wxBOTH);
    frame->Show(true);

    return true;
}

// My frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title), m_confirmExit(false)
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

    m_infoBarAdvanced->Bind(wxEVT_BUTTON, &MyFrame::OnInfoBarRedo, this,
                            wxID_REDO);

    // adding and removing a button immediately doesn't make sense here, of
    // course, it's done just to show that it is possible
    m_infoBarAdvanced->AddButton(wxID_EXIT);
    m_infoBarAdvanced->RemoveButton(wxID_EXIT);

    // ... changing the colours and/or fonts
    m_infoBarAdvanced->SetOwnBackgroundColour(0xc8ffff);
    m_infoBarAdvanced->SetForegroundColour(0x123312);
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

#if wxUSE_TIPWINDOW
    m_tipWindow = NULL;
#endif // wxUSE_TIPWINDOW

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

        menu->Append(DIALOGS_SYSTEM_ABOUT, "&About");
        Bind(wxEVT_MENU, &MyFrame::ShowSimpleAboutDialog, this,
             DIALOGS_SYSTEM_ABOUT);
    }
#endif // __WXMSW__
}

MyFrame::~MyFrame()
{
}

#if wxUSE_COLOURDLG

void MyFrame::DoApplyColour(const wxColour& colour)
{
    if ( colour == m_canvas->GetBackgroundColour() )
        return;

    m_canvas->SetBackgroundColour(colour);
    m_canvas->ClearBackground();
    m_canvas->Refresh();
}

void MyFrame::OnColourChanged(wxColourDialogEvent& event)
{
    DoApplyColour(event.GetColour());
}

void MyFrame::ChooseColour(wxCommandEvent& event)
{
    m_clrData.SetColour(m_canvas->GetBackgroundColour());
    m_clrData.SetChooseAlpha(event.GetId() == DIALOGS_CHOOSE_COLOUR_ALPHA);

    wxColourDialog dialog(this, &m_clrData);
    dialog.Bind(wxEVT_COLOUR_CHANGED, &MyFrame::OnColourChanged, this);
    dialog.SetTitle("Please choose the background colour");
    if ( dialog.ShowModal() == wxID_OK )
    {
        m_clrData = dialog.GetColourData();
    }

    DoApplyColour(m_clrData.GetColour());
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
void MyFrame::ChooseColourGeneric(wxCommandEvent& event)
{
    m_clrData.SetColour(m_canvas->GetBackgroundColour());

    //FIXME:TODO:This has no effect...
    m_clrData.SetChooseFull(true);
    m_clrData.SetChooseAlpha(event.GetId() == DIALOGS_CHOOSE_COLOUR_GENERIC_ALPHA);

    for (int i = 0; i < wxColourData::NUM_CUSTOM; i++)
    {
        unsigned char n = i*(256/wxColourData::NUM_CUSTOM);
        m_clrData.SetCustomColour(i, wxColour(n, n, n));
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
        wxLogMessage("This is some message - everything is ok so far.");
        wxLogMessage("Another message...\n... this one is on multiple lines");
        wxLogWarning("And then something went wrong!");

        // and if ~wxBusyCursor doesn't do it, then call it manually
        wxYield();
    }

    wxLogError("Intermediary error handler decided to abort.");
    wxLogError("The top level caller detected an unrecoverable error.");

    wxLog::FlushActive();

    wxLogMessage("And this is the same dialog but with only one message.");
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

void MyFrame::InfoBarSimpleWrapped(wxCommandEvent &WXUNUSED(event))
{
    m_infoBarSimple->ShowMessage( "This is very very long message to try the label wrapping on the info bar" );
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
            wxLogStatus("You pressed \"Yes\"");
            break;

        case wxID_NO:
            wxLogStatus("You pressed \"No\"");
            break;

        case wxID_CANCEL:
            wxLogStatus("You pressed \"Cancel\"");
            break;

        default:
            wxLogError("Unexpected wxMessageDialog return code!");
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
    dialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
                 &MyFrame::MessageBoxWindowModalClosed, this);
    dialog->ShowWindowModal();
}

void MyFrame::MessageBoxWindowModalClosed(wxWindowModalDialogEvent& event)
{
    wxDialog* dialog = event.GetDialog();
    switch ( dialog->GetReturnCode() )
    {
        case wxID_YES:
            wxLogStatus("You pressed \"Yes\"");
            break;

        case wxID_NO:
            wxLogStatus("You pressed \"No\"");
            break;

        case wxID_CANCEL:
            wxLogStatus("You pressed \"Cancel\"");
            break;

        default:
            wxLogError("Unexpected wxMessageDialog return code!");
    }
    delete dialog;
}

void MyFrame::MessageBoxDialog(wxCommandEvent& WXUNUSED(event))
{
    TestMessageBoxDialog dlg(this);
    dlg.Create();
    dlg.ShowModal();
}

void MyFrame::MessageBoxDialogWindowModal(wxCommandEvent& WXUNUSED(event))
{
    TestMessageBoxDialog* dlg = new TestMessageBoxDialog(this);
    dlg->Create();
    dlg->ShowWindowModal();
}

void MyFrame::MessageBoxDialogWindowModalClosed(wxWindowModalDialogEvent& event)
{
    TestMessageBoxDialog* dialog = dynamic_cast<TestMessageBoxDialog*>(event.GetDialog());
    delete dialog;
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
    long res = wxGetNumberFromUser( "This is some text, actually a lot of text.\n"
                                    "Even two rows of text.",
                                    "Enter a number:", "Numeric input test",
                                     50, 0, 100, this );

    wxString msg;
    int icon;
    if ( res == -1 )
    {
        msg = "Invalid number entered or dialog cancelled.";
        icon = wxICON_HAND;
    }
    else
    {
        msg.Printf("You've entered %lu", res );
        icon = wxICON_INFORMATION;
    }

    wxMessageBox(msg, "Numeric test result", wxOK | icon, this);
}
#endif // wxUSE_NUMBERDLG

#if wxUSE_TEXTDLG
void MyFrame::PasswordEntry(wxCommandEvent& WXUNUSED(event))
{
    wxString pwd = wxGetPasswordFromUser("Enter password:",
                                         "Password entry dialog",
                                         wxEmptyString,
                                         this);
    if ( !pwd.empty() )
    {
        wxMessageBox(wxString::Format("Your password is '%s'", pwd),
                     "Got password", wxOK | wxICON_INFORMATION, this);
    }
}

void MyFrame::LineEntry(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog(this,
                             "This is a small sample\n"
                             "A long, long string to test out the text entrybox",
                             "Please enter a string",
                             "Default value",
                             wxOK | wxCANCEL);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageBox(dialog.GetValue(), "Got string", wxOK | wxICON_INFORMATION, this);
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
        wxMessageBox(dialog.GetValue(), "Got text", wxOK | wxICON_INFORMATION, this);
    }
}
#endif // wxUSE_TEXTDLG

#if wxUSE_CREDENTIALDLG
void MyFrame::CredentialEntry(wxCommandEvent& WXUNUSED(event))
{
    wxCredentialEntryDialog dialog(this, "A login is required", "Credentials");
    if (dialog.ShowModal() == wxID_OK)
    {
        const wxWebCredentials credentials = dialog.GetCredentials();
        const wxString& password = wxSecretString(credentials.GetPassword());
        wxMessageBox
        (
            wxString::Format
            (
                "User: %s Password: %s",
                credentials.GetUser(),
                password
            ),
            "Credentials",
            wxOK | wxICON_INFORMATION,
            this
        );
    }
}
#endif // wxUSE_CREDENTIALDLG

#if wxUSE_CHOICEDLG
void MyFrame::SingleChoice(wxCommandEvent& WXUNUSED(event) )
{
    const wxString choices[] = { "One", "Two", "Three", "Four", "Five" } ;

    wxSingleChoiceDialog dialog(this,
                                "This is a small sample\n"
                                "A single-choice convenience dialog",
                                "Please select a value",
                                WXSIZEOF(choices), choices);

    dialog.SetSelection(2);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetStringSelection(), "Got string");
        dialog2.ShowModal();
    }
}

void MyFrame::MultiChoice(wxCommandEvent& WXUNUSED(event) )
{
    const wxString choices[] =
    {
        "One", "Two", "Three", "Four", "Five",
        "Six", "Seven", "Eight", "Nine", "Ten",
        "Eleven", "Twelve", "Seventeen",
    };

    wxArrayInt selections;
    const int count = wxGetSelectedChoices(selections,
                                        "This is a small sample\n"
                                        "A multi-choice convenience dialog",
                                        "Please select a value",
                                        WXSIZEOF(choices), choices,
                                        this);
    if ( count >= 0 )
    {
        wxString msg;
        if ( count == 0 )
        {
            msg = "You did not select any items";
        }
        else
        {
            msg.Printf("You selected %u items:\n", (unsigned)count);
            for ( int n = 0; n < count; n++ )
            {
                msg += wxString::Format("\t%u: %u (%s)\n",
                                        (unsigned)n, (unsigned)selections[n],
                                        choices[selections[n]]);
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

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(MyRearrangeDialog);
};

wxBEGIN_EVENT_TABLE(MyRearrangeDialog, wxRearrangeDialog)
    EVT_LISTBOX(wxID_ANY, MyRearrangeDialog::OnSelChange)

    EVT_UPDATE_UI(wxID_APPLY, MyRearrangeDialog::OnUpdateUIRename)

    EVT_TEXT_ENTER(wxID_ANY, MyRearrangeDialog::OnRename)
    EVT_BUTTON(wxID_APPLY, MyRearrangeDialog::OnRename)
    EVT_BUTTON(wxID_RESET, MyRearrangeDialog::OnReset)
wxEND_EVENT_TABLE()

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

#if wxUSE_ADDREMOVECTRL

void MyFrame::AddRemove(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg(this, wxID_ANY, "wxAddRemoveCtrl test",
                 wxDefaultPosition, wxDefaultSize,
                 wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    wxAddRemoveCtrl* const ctrl = new wxAddRemoveCtrl(&dlg);
    ctrl->SetInitialSize(wxSize(-1, 12*GetCharHeight()));

    const wxString items[] =
    {
        "some", "items", "for", "testing", "wxAddRemoveCtrl",
    };
    wxListBox* const lbox = new wxListBox(ctrl, wxID_ANY,
                                          wxDefaultPosition, wxDefaultSize,
                                          WXSIZEOF(items), items);

    // Test adaptor class connecting wxAddRemoveCtrl with wxListBox we use
    // inside it.
    class ListBoxAdaptor : public wxAddRemoveAdaptor
    {
    public:
        explicit ListBoxAdaptor(wxListBox* lbox)
            : m_lbox(lbox)
        {
        }

        wxWindow* GetItemsCtrl() const wxOVERRIDE
        {
            return m_lbox;
        }

        bool CanAdd() const wxOVERRIDE
        {
            // Restrict the maximal number of items to 10 just for testing.
            return m_lbox->GetCount() <= 10;
        }

        bool CanRemove() const wxOVERRIDE
        {
            // We must have a selected item in order to be able to delete it.
            return m_lbox->GetSelection() != wxNOT_FOUND;
        }

        void OnAdd() wxOVERRIDE
        {
            // A real program would use a wxDataViewCtrl or wxListCtrl and
            // allow editing the newly edited item in place, here we just use a
            // hardcoded item value instead.
            static int s_item = 0;
            m_lbox->Append(wxString::Format("new item #%d", ++s_item));
        }

        void OnRemove() wxOVERRIDE
        {
            // Notice that we don't need to check if we have a valid selection,
            // we can be only called if CanRemove(), which already checks for
            // this, had returned true.
            const unsigned pos = m_lbox->GetSelection();

            m_lbox->Delete(pos);
            m_lbox->SetSelection(pos == m_lbox->GetCount() ? pos - 1 : pos);
        }

    private:
        wxListBox* const m_lbox;
    };

    ctrl->SetAdaptor(new ListBoxAdaptor(lbox));

    ctrl->SetButtonsToolTips("Add up to 10 items", "Remove current item");

    wxSizer* const sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(ctrl, wxSizerFlags(1).Expand().Border());
    sizerTop->Add(dlg.CreateStdDialogButtonSizer(wxOK | wxCANCEL),
                  wxSizerFlags().Expand().Border());
    dlg.SetSizerAndFit(sizerTop);

    dlg.ShowModal();
}

#endif // wxUSE_ADDREMOVECTRL

#if wxUSE_FILEDLG

// Simple function showing the current wxFileDialog state.
wxString GetFileDialogStateDescription(wxFileDialogBase* dialog)
{
    const wxString fn = dialog->GetCurrentlySelectedFilename();

    wxString msg;
    if ( fn.empty() )
        msg = "Nothing";
    else if ( wxFileName::FileExists(fn) )
        msg = "File";
    else if ( wxFileName::DirExists(fn) )
        msg = "Directory";
    else
        msg = "Something else";

    msg += " selected";

    const int filter = dialog->GetCurrentlySelectedFilterIndex();
    if ( filter != wxNOT_FOUND )
        msg += wxString::Format(" (filter=%d)", filter);

    return msg;
}

// Another helper translating demo combobox selection.
wxString GetFileDialogPaperSize(int selection)
{
    switch ( selection )
    {
        case -1: return "<none>";
        case  0: return "A4";
        case  1: return "Letter";
        default: return "INVALID";
    }
}

// panel with custom controls for file dialog
class MyExtraPanel : public wxPanel
{
public:
    MyExtraPanel(wxWindow *parent);
    wxString GetInfo() const
    {
        return wxString::Format("paper=%s (%s), enabled=%d, text=\"%s\"",
                                m_paperSize, m_paperOrient, m_checked, m_str);
    }

private:
    void OnCheckBox(wxCommandEvent& event)
    {
        m_checked = event.IsChecked();
        m_btn->Enable(m_checked);
    }

    void OnRadioButton(wxCommandEvent& event)
    {
        if ( event.GetEventObject() == m_radioPortrait )
            m_paperOrient = "portrait";
        else if ( event.GetEventObject() == m_radioLandscape )
            m_paperOrient = "landscape";
        else
            m_paperOrient = "unknown";
    }

    void OnChoice(wxCommandEvent& event)
    {
        m_paperSize = GetFileDialogPaperSize(event.GetSelection());
    }

    void OnText(wxCommandEvent& event)
    {
        m_str = event.GetString();
    }

    void OnUpdateLabelUI(wxUpdateUIEvent& event)
    {
        // In this sample, the dialog may be either wxFileDialog itself, or
        // wxGenericFileDialog, so we need to cast to the base class. In a
        // typical application, we would cast to just wxFileDialog instead.
        wxFileDialogBase* const dialog = wxStaticCast(GetParent(), wxFileDialogBase);

        event.SetText(GetFileDialogStateDescription(dialog));
    }

    wxString m_str;
    bool m_checked;
    wxString m_paperSize;
    wxString m_paperOrient;

    wxButton *m_btn;
    wxCheckBox *m_cb;
    wxRadioButton *m_radioPortrait;
    wxRadioButton *m_radioLandscape;
    wxStaticText *m_label;
    wxTextCtrl *m_text;
};

MyExtraPanel::MyExtraPanel(wxWindow *parent)
            : wxPanel(parent),
              m_str("extra text"),
              m_checked(false)
{
    m_btn = new wxButton(this, -1, "Custom Button");
    m_btn->Enable(false);
    m_cb = new wxCheckBox(this, -1, "Enable Custom Button");
    m_cb->Bind(wxEVT_CHECKBOX, &MyExtraPanel::OnCheckBox, this);
    wxChoice* choiceSize = new wxChoice(this, wxID_ANY);
    choiceSize->Append("A4");
    choiceSize->Append("Letter");
    choiceSize->Bind(wxEVT_CHOICE, &MyExtraPanel::OnChoice, this);
    m_radioPortrait = new wxRadioButton(this, wxID_ANY, "&Portrait",
                                  wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_radioPortrait->Bind(wxEVT_RADIOBUTTON, &MyExtraPanel::OnRadioButton, this);
    m_radioLandscape = new wxRadioButton(this, wxID_ANY, "&Landscape");
    m_radioLandscape->Bind(wxEVT_RADIOBUTTON, &MyExtraPanel::OnRadioButton, this);
    m_label = new wxStaticText(this, wxID_ANY, "Nothing selected");
    m_label->Bind(wxEVT_UPDATE_UI, &MyExtraPanel::OnUpdateLabelUI, this);

    m_text = new wxTextCtrl(this, -1, m_str,
                            wxDefaultPosition, wxSize(40*GetCharWidth(), -1));
    m_text->Bind(wxEVT_TEXT, &MyExtraPanel::OnText, this);

    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(new wxStaticText(this, wxID_ANY, "Just some extra text:"),
                  wxSizerFlags().Centre().Border());
    sizerTop->Add(m_text, wxSizerFlags(1).Centre().Border());
    sizerTop->AddSpacer(10);
    sizerTop->Add(choiceSize, wxSizerFlags().Centre().Border(wxRIGHT));
    sizerTop->Add(m_radioPortrait, wxSizerFlags().Centre().Border());
    sizerTop->Add(m_radioLandscape, wxSizerFlags().Centre().Border());
    sizerTop->Add(m_cb, wxSizerFlags().Centre().Border());
    sizerTop->AddSpacer(5);
    sizerTop->Add(m_btn, wxSizerFlags().Centre().Border());
    sizerTop->AddSpacer(5);
    sizerTop->Add(m_label, wxSizerFlags(1).Centre().Border());

    SetSizerAndFit(sizerTop);
}

// a static method can be used instead of a function with most of compilers
static wxWindow* createMyExtraPanel(wxWindow *parent)
{
    return new MyExtraPanel(parent);
}

// This class does the same thing as MyExtraPanel above, but uses newer API for
// wxFileDialog customization.
class MyCustomizeHook : public wxFileDialogCustomizeHook
{
public:
    // Normally we would just use wxFileDialog, but this sample allows using
    // both the real wxFileDialog and wxGenericFileDialog, so allow passing
    // either of them here.
    explicit MyCustomizeHook(wxFileDialogBase& dialog)
        : m_dialog(&dialog)
    {
    }

    // Override pure virtual base class method to add our custom controls.
    virtual void AddCustomControls(wxFileDialogCustomize& customizer) wxOVERRIDE
    {
        // Note: all the pointers created here cease to be valid once
        // ShowModal() returns, TransferDataFromCustomControls() is the latest
        // moment when they can still be used.
        m_text = customizer.AddTextCtrl("Just some extra text:");
        const wxString sizes[] = { "A4", "Letter" };
        m_choiceSize = customizer.AddChoice(WXSIZEOF(sizes), sizes);
        m_radioPortrait = customizer.AddRadioButton("&Portrait");
        m_radioLandscape = customizer.AddRadioButton("&Landscape");
        m_cb = customizer.AddCheckBox("Enable Custom Button");
        m_cb->Bind(wxEVT_CHECKBOX, &MyCustomizeHook::OnCheckBox, this);
        m_btn = customizer.AddButton("Custom Button");
        m_btn->Bind(wxEVT_BUTTON, &MyCustomizeHook::OnButton, this);
        m_label = customizer.AddStaticText("Nothing selected");
    }

    // Override another method called whenever something changes in the dialog.
    virtual void UpdateCustomControls() wxOVERRIDE
    {
        // Enable the button if and only if the checkbox is checked.
        m_btn->Enable(m_cb->GetValue());

        // Enable radio buttons only if a file is selected.
        bool hasFile = wxFileName::FileExists(
                            m_dialog->GetCurrentlySelectedFilename()
                        );
        m_radioPortrait->Enable(hasFile);
        m_radioLandscape->Enable(hasFile);

        // Also show the current dialog state.
        m_label->SetLabelText(GetFileDialogStateDescription(m_dialog));
    }

    // And another one called when the dialog is accepted.
    virtual void TransferDataFromCustomControls() wxOVERRIDE
    {
        m_info.Printf("paper=%s (%s), enabled=%d, text=\"%s\"",
                      GetFileDialogPaperSize(m_choiceSize->GetSelection()),
                      m_radioPortrait->GetValue() ? "portrait" : "landscape",
                      m_cb->GetValue(), m_text->GetValue());
    }


    // This is just a helper function allowing to show the values of the custom
    // controls.
    wxString GetInfo() const { return m_info; }

private:
    void OnCheckBox(wxCommandEvent& event)
    {
        m_btn->Enable(event.IsChecked());
    }

    void OnButton(wxCommandEvent& WXUNUSED(event))
    {
        wxMessageBox("Custom button pressed", "wxWidgets dialogs sample",
                     wxOK | wxICON_INFORMATION, m_dialog);
    }

    wxFileDialogBase* const m_dialog;

    wxFileDialogButton* m_btn;
    wxFileDialogCheckBox* m_cb;
    wxFileDialogChoice* m_choiceSize;
    wxFileDialogRadioButton* m_radioPortrait;
    wxFileDialogRadioButton* m_radioLandscape;
    wxFileDialogTextCtrl* m_text;
    wxFileDialogStaticText* m_label;

    wxString m_info;

    wxDECLARE_NO_COPY_CLASS(MyCustomizeHook);
};

void MyFrame::FileOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog
                 (
                    this,
                    "Testing open file dialog",
                    wxEmptyString,
                    wxEmptyString,
                    wxString::Format
                    (
                        "All files (%s)|%s|C++ files (*.cpp;*.h)|*.cpp;*.h",
                        wxFileSelectorDefaultWildcardStr,
                        wxFileSelectorDefaultWildcardStr
                    )
                 );

    // Note: this object must remain alive until ShowModal() returns.
    MyCustomizeHook myCustomizer(dialog);

    // Normal programs would use either SetCustomizeHook() (preferred) or
    // SetExtraControlCreator() (if its extra flexibility is really required),
    // but, for demonstration purposes, this sample allows either one or the
    // other.
    const bool useExtra =
        GetMenuBar()->IsChecked(DIALOGS_FILE_USE_EXTRA_CONTROL_CREATOR);
    const bool hasExtra =
        useExtra ? dialog.SetExtraControlCreator(&createMyExtraPanel)
                 : dialog.SetCustomizeHook(myCustomizer);

    dialog.CentreOnParent();
    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString extraInfo;
        if ( hasExtra )
        {
            if ( useExtra )
            {
                wxWindow * const extra = dialog.GetExtraControl();
                extraInfo = static_cast<MyExtraPanel*>(extra)->GetInfo();
            }
            else
            {
                extraInfo = myCustomizer.GetInfo();
            }
        }
        else
        {
            extraInfo = "<not supported>";
        }

        wxString info;
        info.Printf("Full file name: %s\n"
                    "Path: %s\n"
                    "Name: %s\n"
                    "Custom window: %s",
                    dialog.GetPath(),
                    dialog.GetDirectory(),
                    dialog.GetFilename(),
                    extraInfo);
        wxMessageDialog dialog2(this, info, "Selected file");
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
                                    "Select the file to load",
                                    wxEmptyString, wxEmptyString,
                                    s_extDef,
                                    wxString::Format
                                    (
                                        "Waveform (*.wav)|*.wav|Plain text (*.txt)|*.txt|All files (%s)|%s",
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFileSelectorDefaultWildcardStr
                                    ),
                                    wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW|wxFD_NO_FOLLOW|wxFD_SHOW_HIDDEN,
                                    this
                                   );

    if ( !path )
        return;

    // it is just a sample, would use wxSplitPath in real program
    s_extDef = path.AfterLast('.');

    wxLogMessage("You selected the file '%s', remembered extension '%s'",
                 path, s_extDef);
}

void MyFrame::FilesOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxString wildcards =
#ifdef __WXMOTIF__
                    "C++ files (*.cpp)|*.cpp";
#else
                    wxString::Format
                    (
                        "All files (%s)|%s|C++ files (*.cpp;*.h)|*.cpp;*.h",
                        wxFileSelectorDefaultWildcardStr,
                        wxFileSelectorDefaultWildcardStr
                    );
#endif
    wxFileDialog dialog(this, "Testing open multiple file dialog",
                        wxEmptyString, wxEmptyString, wildcards,
                        wxFD_OPEN|wxFD_MULTIPLE);

    dialog.Centre(wxCENTER_ON_SCREEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxArrayString paths, filenames;

        dialog.GetPaths(paths);
        dialog.GetFilenames(filenames);

        wxString msg, s;
        size_t count = paths.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            s.Printf("File %d: %s (%s)\n",
                     (int)n, paths[n], filenames[n]);

            msg += s;
        }
        s.Printf("Filter index: %d", dialog.GetFilterIndex());
        msg += s;

        wxMessageDialog dialog2(this, msg, "Selected files");
        dialog2.ShowModal();
    }
}

void MyFrame::FilesOpenWindowModal(wxCommandEvent& WXUNUSED(event) )
{
    wxString wildcards =
#ifdef __WXMOTIF__
                    "C++ files (*.cpp)|*.cpp";
#else
                    wxString::Format
                    (
                        "All files (%s)|%s|C++ files (*.cpp;*.h)|*.cpp;*.h",
                        wxFileSelectorDefaultWildcardStr,
                        wxFileSelectorDefaultWildcardStr
                    );
#endif
    wxFileDialog* dialog = new wxFileDialog(this, "Testing open multiple file dialog",
                        wxEmptyString, wxEmptyString, wildcards,
                        wxFD_OPEN|wxFD_MULTIPLE);

    dialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
                 &MyFrame::FilesOpenWindowModalClosed, this);

    dialog->ShowWindowModal();
}

void MyFrame::FilesOpenWindowModalClosed(wxWindowModalDialogEvent& event)
{
    wxFileDialog* dialog = dynamic_cast<wxFileDialog*>(event.GetDialog());
    if ( dialog->GetReturnCode() == wxID_OK)
    {
        wxArrayString paths, filenames;

        dialog->GetPaths(paths);
        dialog->GetFilenames(filenames);

        wxString msg, s;
        size_t count = paths.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            s.Printf("File %d: %s (%s)\n",
                     (int)n, paths[n], filenames[n]);

            msg += s;
        }
        s.Printf("Filter index: %d", dialog->GetFilterIndex());
        msg += s;

        wxMessageDialog dialog2(this, msg, "Selected files");
        dialog2.ShowModal();
    }
    delete dialog;
}

void MyFrame::FileSave(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog(this,
                        "Testing save file dialog",
                        wxEmptyString,
                        "myletter.doc",
                        "Text files (*.txt)|*.txt|Document files (*.doc;*.ods)|*.doc;*.ods",
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage("%s, filter %d",
                     dialog.GetPath(), dialog.GetFilterIndex());
    }
}

void MyFrame::FileSaveWindowModal(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog* dialog = new wxFileDialog(this,
                        "Testing save file dialog",
                        wxEmptyString,
                        "myletter.doc",
                        "Text files (*.txt)|*.txt|Document files (*.doc;*.ods)|*.doc;*.ods",
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog->SetFilterIndex(1);

    dialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
                 &MyFrame::FileSaveWindowModalClosed, this);

    dialog->ShowWindowModal();
}

void MyFrame::FileSaveWindowModalClosed(wxWindowModalDialogEvent& event)
{
    wxFileDialog* dialog = dynamic_cast<wxFileDialog*>(event.GetDialog());
    if ( dialog->GetReturnCode() == wxID_OK)
    {
        wxLogMessage("%s, filter %d",
                     dialog->GetPath(), dialog->GetFilterIndex());
    }
    delete dialog;
}

#endif // wxUSE_FILEDLG

void MyFrame::MacToggleAlwaysShowTypes(wxCommandEvent& event)
{
#ifdef wxOSX_FILEDIALOG_ALWAYS_SHOW_TYPES
    wxSystemOptions::SetOption(wxOSX_FILEDIALOG_ALWAYS_SHOW_TYPES,
                               event.IsChecked());
#else
    wxUnusedVar(event);
#endif
}

#if USE_FILEDLG_GENERIC
void MyFrame::FileOpenGeneric(wxCommandEvent& WXUNUSED(event) )
{
    wxGenericFileDialog dialog
                 (
                    this,
                    "Testing open file dialog",
                    wxEmptyString,
                    wxEmptyString,
                    "C++ files (*.cpp;*.h)|*.cpp;*.h"
                 );

    MyCustomizeHook myCustomizer(dialog);
    dialog.SetCustomizeHook(myCustomizer);
    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString info;
        info.Printf("Full file name: %s\n"
                    "Path: %s\n"
                    "Name: %s\n"
                    "Custom window: %s",
                    dialog.GetPath(),
                    dialog.GetDirectory(),
                    dialog.GetFilename(),
                    myCustomizer.GetInfo());
        wxMessageDialog dialog2(this, info, "Selected file");
        dialog2.ShowModal();
    }
}

void MyFrame::FilesOpenGeneric(wxCommandEvent& WXUNUSED(event) )
{
    wxString wildcards = "All files (*.*)|*.*|C++ files (*.cpp;*.h)|*.cpp;*.h";
    wxGenericFileDialog dialog(this, "Testing open multiple file dialog",
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
            s.Printf("File %d: %s (%s)\n",
                     (int)n, paths[n], filenames[n]);

            msg += s;
        }
        s.Printf("Filter index: %d", dialog.GetFilterIndex());
        msg += s;

        wxMessageDialog dialog2(this, msg, "Selected files");
        dialog2.ShowModal();
    }
}

void MyFrame::FileSaveGeneric(wxCommandEvent& WXUNUSED(event) )
{
    wxGenericFileDialog dialog(this,
                        "Testing save file dialog",
                        wxEmptyString,
                        "myletter.doc",
                        "Text files (*.txt)|*.txt|Document files (*.doc;*.ods)|*.doc;*.ods",
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage("%s, filter %d",
                     dialog.GetPath(), dialog.GetFilterIndex());
    }
}
#endif // USE_FILEDLG_GENERIC

#if wxUSE_DIRDLG
void MyFrame::DoDirChoose(int style)
{
    // pass some initial dir to wxDirDialog
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxDirDialog dialog(this, "Testing directory picker", dirHome, style);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage("Selected path: %s", dialog.GetPath());
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

void MyFrame::DirChooseMultiple(wxCommandEvent& WXUNUSED(event))
{
    // pass some initial dir and the style to wxDirDialog
    int style = wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_MULTIPLE | wxDD_SHOW_HIDDEN;
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxDirDialog dialog(this, "Testing multiple directory picker", dirHome, style);

    if ( dialog.ShowModal() == wxID_OK )
    {
        wxArrayString paths;

        dialog.GetPaths(paths);

        wxString msg, s;
        size_t count = paths.GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            s.Printf("Directory %d: %s\n",
                     (int)n, paths[n]);

            msg += s;
        }

        wxMessageDialog dialog2(this, msg, "Selected directories");
        dialog2.ShowModal();
    }
}

void MyFrame::DirChooseWindowModal(wxCommandEvent& WXUNUSED(event) )
{
    // pass some initial dir to wxDirDialog
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxDirDialog* dialog = new wxDirDialog(this, "Testing directory picker", dirHome, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    dialog->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
                 &MyFrame::DirChooseWindowModalClosed, this);

    dialog->ShowWindowModal();
}

void MyFrame::DirChooseWindowModalClosed(wxWindowModalDialogEvent& event)
{
    wxDirDialog* dialog = dynamic_cast<wxDirDialog*>(event.GetDialog());
    if ( dialog->GetReturnCode() == wxID_OK)
    {
        wxLogMessage("Selected path: %s", dialog->GetPath());
    }
    delete dialog;
}
#endif // wxUSE_DIRDLG

#if USE_DIRDLG_GENERIC
void MyFrame::GenericDirChoose(wxCommandEvent& WXUNUSED(event) )
{
    // pass some initial dir to wxDirDialog
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxGenericDirDialog dialog(this, "Testing generic directory picker", dirHome);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetPath(), "Selected path");
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
    wxDialog dlg(this, wxID_ANY, "Dialog centered on screen",
                 wxDefaultPosition, wxSize(200, 100));
    (new wxButton(&dlg, wxID_OK, "Close"))->Centre();
    dlg.CentreOnScreen();
    dlg.ShowModal();
}

void MyFrame::DlgCenteredParent(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg(this, wxID_ANY, "Dialog centered on parent",
                 wxDefaultPosition, wxSize(200, 100));
    (new wxButton(&dlg, wxID_OK, "Close"))->Centre();
    dlg.CentreOnParent();
    dlg.ShowModal();
}

#if wxUSE_MINIFRAME
void MyFrame::MiniFrame(wxCommandEvent& WXUNUSED(event))
{
    wxFrame *frame = new wxMiniFrame(this, wxID_ANY, "Mini frame",
                                     wxDefaultPosition, wxSize(300, 100),
                                     wxCAPTION | wxCLOSE_BOX);
    new wxStaticText(frame,
                     wxID_ANY,
                     "Mini frames have slightly different appearance",
                     wxPoint(5, 5));
    new wxStaticText(frame,
                     wxID_ANY,
                     "from the normal frames but that's the only difference.",
                     wxPoint(5, 25));

    frame->CentreOnParent();
    frame->Show();
}
#endif // wxUSE_MINIFRAME

void MyFrame::DlgOnTop(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg(this, wxID_ANY, "Dialog staying on top of other windows",
                 wxDefaultPosition, wxSize(300, 100),
                 wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
    (new wxButton(&dlg, wxID_OK, "Close"))->Centre();
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

    wxTipProvider *tipProvider = wxCreateFileTipProvider("tips.txt", s_index);

    bool showAtStartup = wxShowTip(this, tipProvider);

    if ( showAtStartup )
    {
        wxMessageBox("Will show tips on startup", "Tips dialog",
                     wxOK | wxICON_INFORMATION, this);
    }

    s_index = tipProvider->GetCurrentTip();
    delete tipProvider;
}
#endif // wxUSE_STARTUP_TIPS

#if USE_SETTINGS_DIALOG
void MyFrame::OnPropertySheet(wxCommandEvent& event)
{
    SettingsDialog dialog(this, m_settingsData, event.GetId());
    dialog.ShowModal();
}
#endif // USE_SETTINGS_DIALOG

void MyFrame::OnRequestUserAttention(wxCommandEvent& WXUNUSED(event))
{
    wxLogStatus("Sleeping for 3 seconds to allow you to switch to another window");

    wxSleep(3);

    RequestUserAttention(wxUSER_ATTENTION_ERROR);
}

#if wxUSE_RICHTOOLTIP || wxUSE_NOTIFICATION_MESSAGE

#include "tip.xpm"

#endif

#if wxUSE_NOTIFICATION_MESSAGE

// ----------------------------------------------------------------------------
// TestNotificationMessageDialog
// ----------------------------------------------------------------------------

class TestNotificationMessageWindow : public wxFrame
{
public:
    TestNotificationMessageWindow(wxWindow *parent) :
        wxFrame(parent, wxID_ANY, "User Notification Test Dialog")
    {
#ifdef __WXMSW__
        SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
#endif
        wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

        wxSizer* sizerText = new wxStaticBoxSizer(wxVERTICAL, this, "Notification Texts");

        sizerText->Add(new wxStaticText(this, wxID_ANY, "&Title:"),
            wxSizerFlags());
        m_textTitle = new wxTextCtrl(this, wxID_ANY, "Notification Title");
        sizerText->Add(m_textTitle, wxSizerFlags().Expand());

        sizerText->Add(new wxStaticText(this, wxID_ANY, "&Message:"),
            wxSizerFlags());
        m_textMessage = new wxTextCtrl(this, wxID_ANY, "A message within the notification",
            wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
        m_textMessage->SetMinSize(wxSize(300, -1));
        sizerText->Add(m_textMessage, wxSizerFlags().Expand());

        sizerTop->Add(sizerText, wxSizerFlags().Expand().Border());

        const wxString icons[] =
        {
            "De&fault",
            "None",
            "&Information",
            "&Warning",
            "&Error",
            "&Custom"
        };
        wxCOMPILE_TIME_ASSERT(WXSIZEOF(icons) == Icon_Max, IconMismatch);
        m_icons = new wxRadioBox(this, wxID_ANY, "Ic&on in notification",
            wxDefaultPosition, wxDefaultSize,
            WXSIZEOF(icons), icons,
            1, wxRA_SPECIFY_ROWS);
        m_icons->SetSelection(Icon_Default);
        sizerTop->Add(m_icons, wxSizerFlags().Expand().Border());

        const wxString timeouts[] =
        {
            "&Automatic",
            "&Never",
            "&5 sec",
            "&15 sec"
        };
        m_showTimeout = new wxRadioBox(this, wxID_ANY, "&Timeout for notification",
            wxDefaultPosition, wxDefaultSize,
            WXSIZEOF(timeouts), timeouts,
            1, wxRA_SPECIFY_ROWS);
        m_showTimeout->SetSelection(0);
        sizerTop->Add(m_showTimeout, wxSizerFlags().Expand().Border());

        wxSizer* sizerActions = new wxStaticBoxSizer(wxVERTICAL, this, "Additional Actions");

        m_actionList = new wxListBox(this, wxID_ANY);
        sizerActions->Add(m_actionList, wxSizerFlags().Expand());

        wxSizer* sizerActionMod = new wxBoxSizer(wxHORIZONTAL);
        sizerActionMod->Add(new wxStaticText(this, wxID_ANY, "ID:"), wxSizerFlags().Center());
        const wxString actionIds[] =
        {
            "wxID_DELETE",
            "wxID_CLOSE",
            "wxID_OK",
            "wxID_CANCEL"
        };
        m_actionChoice = new wxChoice(this, wxID_ANY,
            wxDefaultPosition, wxDefaultSize,
            WXSIZEOF(actionIds), actionIds
            );
        m_actionChoice->SetSelection(0);
        sizerActionMod->Add(m_actionChoice);
        sizerActionMod->Add(new wxStaticText(this, wxID_ANY, "Custom label:"), wxSizerFlags().Center());
        m_actionCaption = new wxTextCtrl(this, wxID_ANY);
        sizerActionMod->Add(m_actionCaption);
        wxButton* actionAddBtn = new wxButton(this, wxID_ADD);
        actionAddBtn->Bind(wxEVT_BUTTON, &TestNotificationMessageWindow::OnActionAddClicked, this);
        sizerActionMod->Add(actionAddBtn);
        wxButton* actionRemoveBtn = new wxButton(this, wxID_REMOVE);
        actionRemoveBtn->Bind(wxEVT_BUTTON, &TestNotificationMessageWindow::OnActionRemoveClicked, this);
        sizerActionMod->Add(actionRemoveBtn);

        sizerActions->Add(sizerActionMod, wxSizerFlags().Border());

        sizerTop->Add(sizerActions, wxSizerFlags().Expand().Border());

        wxSizer* sizerSettings = new wxStaticBoxSizer(wxVERTICAL, this, "Notification Settings");

#ifdef wxHAS_NATIVE_NOTIFICATION_MESSAGE
        m_useGeneric = new wxCheckBox(this, wxID_ANY, "Use &generic notifications");
        sizerSettings->Add(m_useGeneric);
#endif

        m_delayShow = new wxCheckBox(this, wxID_ANY, "&Delay show");
#if defined(__WXOSX__)
        m_delayShow->SetValue(true);
#endif
        sizerSettings->Add(m_delayShow);

        m_handleEvents = new wxCheckBox(this, wxID_ANY, "&Handle events");
        m_handleEvents->SetValue(true);
        sizerSettings->Add(m_handleEvents);

#if defined(__WXMSW__) && wxUSE_TASKBARICON
        m_taskbarIcon = NULL;
        m_useTaskbar = new wxCheckBox(this, wxID_ANY, "Use persistent &taskbar icon");
        m_useTaskbar->SetValue(false);
        sizerSettings->Add(m_useTaskbar);
#endif

        sizerTop->Add(sizerSettings, wxSizerFlags().Expand().Border());

        m_textStatus = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
            wxST_NO_AUTORESIZE | wxALIGN_CENTRE_HORIZONTAL);
        m_textStatus->SetForegroundColour(*wxBLUE);
        sizerTop->Add(m_textStatus, wxSizerFlags().Expand().Border());

        wxSizer* sizerButtons = new wxBoxSizer(wxHORIZONTAL);
        sizerButtons->Add(new wxButton(this, wxID_NEW, "&Show"));
        m_closeButton = new wxButton(this, wxID_CLOSE, "&Close");
        m_closeButton->Disable();
        sizerButtons->Add(m_closeButton);
        sizerTop->Add(sizerButtons, wxSizerFlags().Center());

        SetSizerAndFit(sizerTop);

        Center();

        Bind(wxEVT_BUTTON, &TestNotificationMessageWindow::OnShowClicked, this, wxID_NEW);
        Bind(wxEVT_BUTTON, &TestNotificationMessageWindow::OnCloseClicked, this, wxID_CLOSE);
    }

private:
    enum
    {
        Icon_Default,
        Icon_None,
        Icon_Info,
        Icon_Warning,
        Icon_Error,
        Icon_Custom,
        Icon_Max
    };

    class ActionInfo : public wxClientData
    {
    public:
        ActionInfo(wxWindowID actionId, const wxString& actionCaption):
            id(actionId),
            customCaption(actionCaption)
        {

        }

        wxWindowID id;
        wxString customCaption;
    };

    wxTextCtrl* m_textTitle;
    wxTextCtrl* m_textMessage;
    wxRadioBox* m_icons;
    wxRadioBox* m_showTimeout;
    wxListBox* m_actionList;
    wxChoice* m_actionChoice;
    wxTextCtrl* m_actionCaption;
#ifdef wxHAS_NATIVE_NOTIFICATION_MESSAGE
    wxCheckBox* m_useGeneric;
#endif
    wxCheckBox* m_delayShow;
    wxCheckBox* m_handleEvents;
    wxStaticText* m_textStatus;
    wxButton* m_closeButton;

#if defined(__WXMSW__) && wxUSE_TASKBARICON
    wxCheckBox* m_useTaskbar;
    wxTaskBarIcon* m_taskbarIcon;
#endif

    wxSharedPtr< wxNotificationMessageBase> m_notif;

    void DoShowNotification()
    {
        if ( m_delayShow->GetValue() )
        {
            ShowStatus("Sleeping for 3 seconds to allow you to switch to another window");
            wxYield();
            wxSleep(3);
        }

        m_closeButton->Enable();
        ShowStatus("Showing notification...");
#ifdef wxHAS_NATIVE_NOTIFICATION_MESSAGE
        if ( m_useGeneric->GetValue() )
            m_notif = new wxGenericNotificationMessage(
                m_textTitle->GetValue(),
                m_textMessage->GetValue(),
                this);
        else
#endif
        {
            m_notif = new wxNotificationMessage(
                m_textTitle->GetValue(),
                m_textMessage->GetValue(),
                this);

#if defined(__WXMSW__) && wxUSE_TASKBARICON
            if ( m_useTaskbar->GetValue() )
            {
                if ( !m_taskbarIcon )
                {
                    m_taskbarIcon = new wxTaskBarIcon();
                    m_taskbarIcon->SetIcon(reinterpret_cast<wxTopLevelWindow*>(GetParent())->GetIcon(),
                        "Dialogs Sample (Persistent)");
                }
                wxNotificationMessage::UseTaskBarIcon(m_taskbarIcon);
            }
            else
            if ( m_taskbarIcon )
            {
                wxNotificationMessage::UseTaskBarIcon(NULL);
                delete m_taskbarIcon;
                m_taskbarIcon = NULL;
            }
#endif
        }

        switch (m_icons->GetSelection())
        {
            case Icon_Default:
                // Don't call SetFlags or SetIcon to see the implementations default
                break;
            case Icon_None:
                m_notif->SetFlags(0);
                break;
            case Icon_Info:
                m_notif->SetFlags(wxICON_INFORMATION);
                break;
            case Icon_Warning:
                m_notif->SetFlags(wxICON_WARNING);
                break;
            case Icon_Error:
                m_notif->SetFlags(wxICON_ERROR);
                break;
            case Icon_Custom:
                m_notif->SetIcon(tip_xpm);
                break;
        }

        int timeout;
        switch (m_showTimeout->GetSelection())
        {
            case 1:
                timeout = wxNotificationMessage::Timeout_Never;
                break;
            case 2:
                timeout = 5;
                break;
            case 3:
                timeout = 10;
                break;
            default:
                timeout = wxNotificationMessage::Timeout_Auto;
                break;
        }

        for (unsigned int i = 0; i < m_actionList->GetCount(); i++)
        {
            ActionInfo* ai = reinterpret_cast<ActionInfo*>(m_actionList->GetClientObject(i));
            if ( !m_notif->AddAction(ai->id, ai->customCaption) )
                wxLogWarning("Could not add action: %s", m_actionList->GetString(i));
        }

        if ( m_handleEvents->GetValue() )
        {
            m_notif->Bind(wxEVT_NOTIFICATION_MESSAGE_ACTION, &TestNotificationMessageWindow::OnNotificationAction, this);
            m_notif->Bind(wxEVT_NOTIFICATION_MESSAGE_CLICK, &TestNotificationMessageWindow::OnNotificationClicked, this);
            m_notif->Bind(wxEVT_NOTIFICATION_MESSAGE_DISMISSED, &TestNotificationMessageWindow::OnNotificationDismissed, this);
        }

        m_notif->Show(timeout);

        // Free the notification if we don't handle it's events
        if ( !m_handleEvents->GetValue() )
        {
            // Notice that the notification remains shown even after the
            // wxNotificationMessage object itself is destroyed so we can show simple
            // notifications using temporary objects.
            m_notif.reset();
            ShowStatus("Showing notification, deleted object");
        }
    }

    void OnShowClicked(wxCommandEvent& WXUNUSED(event))
    {
        DoShowNotification();
    }

    void OnCloseClicked(wxCommandEvent& WXUNUSED(event))
    {
        if ( m_notif )
            m_notif->Close();
    }

    void OnActionAddClicked(wxCommandEvent& WXUNUSED(event))
    {
        wxWindowID actionId;
        switch (m_actionChoice->GetSelection())
        {
            case 1:
                actionId = wxID_CLOSE;
                break;
            case 2:
                actionId = wxID_OK;
                break;
            case 3:
                actionId = wxID_CANCEL;
                break;
            default:
                actionId = wxID_DELETE;
                break;
        }

        wxString actionCaption = m_actionCaption->GetValue();
        wxString desc = m_actionChoice->GetStringSelection();
        if ( !actionCaption.empty() )
            desc += " (" + actionCaption + ")";
        m_actionList->SetSelection( m_actionList->Append( desc, new ActionInfo(actionId, actionCaption) ) );
    }

    void OnActionRemoveClicked(wxCommandEvent& WXUNUSED(event))
    {
        int pos = m_actionList->GetSelection();
        if ( pos != wxNOT_FOUND )
        {
            m_actionList->Delete(pos);
            if ( pos > 0 && m_actionList->GetCount() > 0 )
                m_actionList->SetSelection(pos - 1);
        }
        else
            wxLogError("No action selected");
    }

    void OnNotificationClicked(wxCommandEvent& event)
    {
        ShowStatus("Notification was clicked");

        Raise();

        event.Skip();
    }

    void OnNotificationDismissed(wxCommandEvent& event)
    {
        ShowStatus("Notification was dismissed");

        Raise();

        event.Skip();
    }

    void OnNotificationAction(wxCommandEvent& event)
    {
        ShowStatus(wxString::Format("Selected %s action in notification", wxGetStockLabel(event.GetId(), 0)) );

        event.Skip();
    }

    void ShowStatus(const wxString& text)
    {
        m_textStatus->SetLabelText(text);
    }

};

void MyFrame::OnNotifMsg(wxCommandEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    // Try to enable toast notifications (available since Win8)
    if ( !wxNotificationMessage::MSWUseToasts() )
    {
        wxLogDebug("Toast notifications not available.");
    }
#endif

    TestNotificationMessageWindow* dlg = new TestNotificationMessageWindow(this);
    dlg->Show();
}

#endif // wxUSE_NOTIFICATION_MESSAGE

#if wxUSE_TIPWINDOW

void MyFrame::OnShowTip(wxCommandEvent& WXUNUSED(event))
{
    if ( m_tipWindow )
    {
        m_tipWindow->Close();
    }
    else
    {
        m_tipWindow = new wxTipWindow
                          (
                            this,
                            "This is just some text to be shown in the tip "
                            "window, broken into multiple lines, each less "
                            "than 60 logical pixels wide.",
                            FromDIP(60),
                            &m_tipWindow
                          );
    }
}

void MyFrame::OnUpdateShowTipUI(wxUpdateUIEvent& event)
{
    event.Check(m_tipWindow != NULL);
}

#endif // wxUSE_TIPWINDOW

#if wxUSE_RICHTOOLTIP

#include "wx/richtooltip.h"

class RichTipDialog : public wxDialog
{
public:
    RichTipDialog(wxWindow* parent)
        : wxDialog(parent, wxID_ANY, "wxRichToolTip Test",
                   wxDefaultPosition, wxDefaultSize,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    {
        // Create the controls.
        m_textTitle = new wxTextCtrl(this, wxID_ANY, "Tooltip title");
        m_textBody = new wxTextCtrl(this, wxID_ANY, "Main tooltip text\n"
                                                    "possibly on several\n"
                                                    "lines.",
                                    wxDefaultPosition, wxDefaultSize,
                                    wxTE_MULTILINE);
        wxButton* btnShowText = new wxButton(this, wxID_ANY, "Show for &text");
        wxButton* btnShowBtn = new wxButton(this, wxID_ANY, "Show for &button");

        const wxString icons[] =
        {
            "&None",
            "&Information",
            "&Warning",
            "&Error",
            "&Custom"
        };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(icons) == Icon_Max, IconMismatch );
        m_icons = new wxRadioBox(this, wxID_ANY, "&Icon choice:",
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(icons), icons,
                                 1, wxRA_SPECIFY_ROWS);
        m_icons->SetSelection(Icon_Info);

        const wxString tipKinds[] =
        {
            "&None", "Top left", "Top", "Top right",
            "Bottom left", "Bottom", "Bottom right", "&Auto"
        };
        m_tipKinds = new wxRadioBox(this, wxID_ANY, "Tip &kind:",
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(tipKinds), tipKinds,
                                    4, wxRA_SPECIFY_COLS);
        m_tipKinds->SetSelection(wxTipKind_Auto);

        const wxString bgStyles[] =
        {
            "&Default", "&Solid", "&Gradient",
        };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(bgStyles) == Bg_Max, BgMismatch );
        m_bgStyles = new wxRadioBox(this, wxID_ANY, "Background style:",
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(bgStyles), bgStyles,
                                    1, wxRA_SPECIFY_ROWS);

        const wxString timeouts[] = { "&None", "&Default (no delay)", "&3 seconds" };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(timeouts) == Timeout_Max, TmMismatch );
        m_timeouts = new wxRadioBox(this, wxID_ANY, "Timeout:",
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(timeouts), timeouts,
                                    1, wxRA_SPECIFY_ROWS);
        m_timeouts->SetSelection(Timeout_Default);
        m_timeDelay = new wxCheckBox(this, wxID_ANY, "Delay show" );

        // Lay them out.
        m_textBody->SetMinSize(wxSize(300, 200));

        wxBoxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(m_textTitle, wxSizerFlags().Expand().Border());
        sizer->Add(m_textBody, wxSizerFlags(1).Expand().Border());
        sizer->Add(m_icons, wxSizerFlags().Expand().Border());
        sizer->Add(m_tipKinds, wxSizerFlags().Centre().Border());
        sizer->Add(m_bgStyles, wxSizerFlags().Centre().Border());
        sizer->Add(m_timeouts, wxSizerFlags().Centre().Border());
        sizer->Add(m_timeDelay, wxSizerFlags().Centre().Border());
        wxBoxSizer* const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
        sizerBtns->Add(btnShowText, wxSizerFlags().Border(wxRIGHT));
        sizerBtns->Add(btnShowBtn, wxSizerFlags().Border(wxLEFT));
        sizer->Add(sizerBtns, wxSizerFlags().Centre().Border());
        sizer->Add(CreateStdDialogButtonSizer(wxOK),
                   wxSizerFlags().Expand().Border());
        SetSizerAndFit(sizer);


        // And connect the event handlers.
        btnShowText->Bind(wxEVT_BUTTON, &RichTipDialog::OnShowTipForText, this);
        btnShowBtn->Bind(wxEVT_BUTTON, &RichTipDialog::OnShowTipForBtn, this);
    }

private:
    enum
    {
        Icon_None,
        Icon_Info,
        Icon_Warning,
        Icon_Error,
        Icon_Custom,
        Icon_Max
    };

    enum
    {
        Bg_Default,
        Bg_Solid,
        Bg_Gradient,
        Bg_Max
    };

    enum
    {
        Timeout_None,
        Timeout_Default,
        Timeout_3sec,
        Timeout_Max
    };


    void OnShowTipForText(wxCommandEvent& WXUNUSED(event))
    {
        DoShowTip(m_textTitle);
    }

    void OnShowTipForBtn(wxCommandEvent& WXUNUSED(event))
    {
        DoShowTip(FindWindow(wxID_OK));
    }

    void DoShowTip(wxWindow* win)
    {
        wxRichToolTip tip(m_textTitle->GetValue(), m_textBody->GetValue());
        const int iconSel = m_icons->GetSelection();
        if ( iconSel == Icon_Custom )
        {
            tip.SetIcon(tip_xpm);
        }
        else // Use a standard icon.
        {
            static const int stdIcons[] =
            {
                wxICON_NONE,
                wxICON_INFORMATION,
                wxICON_WARNING,
                wxICON_ERROR,
            };

            tip.SetIcon(stdIcons[iconSel]);
        }

        switch ( m_bgStyles->GetSelection() )
        {
            case Bg_Default:
                break;

            case Bg_Solid:
                tip.SetBackgroundColour(*wxLIGHT_GREY);
                break;

            case Bg_Gradient:
                tip.SetBackgroundColour(*wxWHITE, wxColour(0xe4, 0xe5, 0xf0));
                break;
        }

        int delay = m_timeDelay->IsChecked() ? 500 : 0;

        switch ( m_timeouts->GetSelection() )
        {
            case Timeout_None:
                // Don't call SetTimeout unnecessarily
                // or msw will show generic impl
                if ( delay )
                    tip.SetTimeout(0, delay);
                break;

            case Timeout_Default:
                break;

            case Timeout_3sec:
                tip.SetTimeout(3000, delay);
                break;
        }

        tip.SetTipKind(static_cast<wxTipKind>(m_tipKinds->GetSelection()));

        tip.ShowFor(win);
    }

    wxTextCtrl* m_textTitle;
    wxTextCtrl* m_textBody;
    wxRadioBox* m_icons;
    wxRadioBox* m_tipKinds;
    wxRadioBox* m_bgStyles;
    wxRadioBox* m_timeouts;
    wxCheckBox* m_timeDelay;
};

void MyFrame::OnRichTipDialog(wxCommandEvent& WXUNUSED(event))
{
    RichTipDialog dialog(this);
    dialog.ShowModal();
}

#endif // wxUSE_RICHTOOLTIP

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

wxBEGIN_EVENT_TABLE(TestDefaultActionDialog, wxDialog)
    EVT_CHECKBOX(ID_CATCH_LISTBOX_DCLICK,    TestDefaultActionDialog::OnCatchListBoxDClick)
    EVT_CHECKBOX(ID_DISABLE_OK,              TestDefaultActionDialog::OnDisableOK)
    EVT_CHECKBOX(ID_DISABLE_CANCEL,          TestDefaultActionDialog::OnDisableCancel)
    EVT_LISTBOX_DCLICK(ID_LISTBOX,           TestDefaultActionDialog::OnListBoxDClick)
    EVT_TEXT_ENTER(wxID_ANY,                 TestDefaultActionDialog::OnTextEnter)
wxEND_EVENT_TABLE()

// TODO-C++11: We can't declare this class inside TestDefaultActionDialog
//             itself when using C++98, so we have to do it here instead.
namespace
{

// We have to define a new class in order to actually handle pressing
// Enter, if we didn't do it, pressing it would still close the dialog.
class EnterHandlingTextCtrl : public wxTextCtrl
{
public:
    EnterHandlingTextCtrl(wxWindow* parent, int id, const wxString& value)
        : wxTextCtrl(parent, id, value,
                     wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER)
    {
        Bind(wxEVT_TEXT_ENTER, &EnterHandlingTextCtrl::OnEnter, this);

        SetInitialSize(GetSizeFromTextSize(GetTextExtent(value).x));
    }

private:
    void OnEnter(wxCommandEvent& WXUNUSED(event))
    {
        wxLogMessage("Enter pressed");
    }
};

} // anonymous namespace

TestDefaultActionDialog::TestDefaultActionDialog( wxWindow *parent ) :
  wxDialog( parent, -1, "Test default action" )
{
    m_catchListBoxDClick = false;

    wxBoxSizer *main_sizer = new wxBoxSizer( wxVERTICAL );

    const int border = wxSizerFlags::GetDefaultBorder();
    wxFlexGridSizer *grid_sizer = new wxFlexGridSizer(2, wxSize(border, border));

#if wxUSE_LISTBOX
    wxListBox *listbox = new wxListBox( this, ID_LISTBOX );
    listbox->Append( "String 1" );
    listbox->Append( "String 2" );
    listbox->Append( "String 3" );
    listbox->Append( "String 4" );
    grid_sizer->Add( listbox );
#endif // wxUSE_LISTBOX

    grid_sizer->Add(new wxCheckBox(this, ID_CATCH_LISTBOX_DCLICK, "Catch DoubleClick from wxListBox"),
                    wxSizerFlags().CentreVertical());

    grid_sizer->Add(new wxTextCtrl(this, wxID_ANY, "Enter here closes the dialog"),
                    wxSizerFlags().Expand().CentreVertical());
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, "wxTextCtrl without wxTE_PROCESS_ENTER"),
                    wxSizerFlags().CentreVertical());

    grid_sizer->Add(new EnterHandlingTextCtrl(this, wxID_ANY, "Enter here is handled by the application"),
                    wxSizerFlags().CentreVertical());
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, "wxTextCtrl with wxTE_PROCESS_ENTER"),
                    wxSizerFlags().CentreVertical());

    grid_sizer->Add(new wxTextCtrl(this, wxID_ANY,
                                   "Enter here adds another line,\n"
                                   "while Ctrl-Enter closes the dialog",
                                   wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE),
                    wxSizerFlags().Expand());
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, "wxTextCtrl without wxTE_PROCESS_ENTER"),
                    wxSizerFlags().CentreVertical());

    grid_sizer->Add(new wxCheckBox(this, ID_DISABLE_OK, "Disable \"OK\""),
                    wxSizerFlags().CentreVertical());
    grid_sizer->Add(new wxCheckBox(this, ID_DISABLE_CANCEL, "Disable \"Cancel\""),
                    wxSizerFlags().CentreVertical());

    main_sizer->Add(grid_sizer, wxSizerFlags().DoubleBorder());

    wxSizer *button_sizer = CreateSeparatedButtonSizer( wxOK|wxCANCEL );
    if ( button_sizer )
        main_sizer->Add(button_sizer, wxSizerFlags().Expand().Border());

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
    const wxString& text = event.GetString();
    if ( text.empty() )
    {
        event.Skip();
        return;
    }

    wxLogMessage("Text \"%s\" entered.", text);
}

void MyFrame::OnTestDefaultActionDialog(wxCommandEvent& WXUNUSED(event))
{
    TestDefaultActionDialog dialog( this );
    dialog.ShowModal();
}

void MyFrame::OnModalHook(wxCommandEvent& event)
{
    class TestModalHook : public wxModalDialogHook
    {
    protected:
        virtual int Enter(wxDialog* dialog) wxOVERRIDE
        {
            wxLogStatus("Showing %s modal dialog",
                        dialog->GetClassInfo()->GetClassName());
            return wxID_NONE;
        }

        virtual void Exit(wxDialog* dialog) wxOVERRIDE
        {
            wxLogStatus("Leaving %s modal dialog",
                        dialog->GetClassInfo()->GetClassName());
        }
    };

    static TestModalHook s_hook;
    if ( event.IsChecked() )
        s_hook.Register();
    else
        s_hook.Unregister();
}

void MyFrame::OnSimulatedUnsaved(wxCommandEvent& event)
{
    m_confirmExit = event.IsChecked();
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnClose(wxCloseEvent& event)
{
    if ( m_confirmExit && event.CanVeto() )
    {
        wxMessageDialog dialog(this,
            "You have an unsaved file; save before closing?",
            "OnClose",
            wxCENTER |
            wxYES_NO | wxCANCEL |
            wxICON_QUESTION);

        dialog.SetYesNoLabels(
            "&Save",
            "&Discard changes"
        );
        switch ( dialog.ShowModal() )
        {
        case wxID_CANCEL:
            event.Veto();
            wxLogStatus("You cancelled closing the application.");
            // Return without calling event.Skip() to prevent closing the frame.
            // The application should resume operation as if closing it had not
            // been attempted.
            return;
        case wxID_YES:
            wxMessageBox("You chose to save your file.", "OnClose", wxOK);
            // In a real application, do something to save the
            // file(s), possibly asking for a file name and location
            // using wxFileDialog.
            break;
        default:
            wxLogError("Unexpected wxMessageDialog return code!");
            wxFALLTHROUGH;
        case wxID_NO:
            // Don't save anything, and simply continue with closing the frame.
            break;
        }
    }

    // Continue with closing the frame.
    event.Skip();
}

#if wxUSE_PROGRESSDLG

static const int max_ = 100;

void MyFrame::ShowProgress( wxCommandEvent& WXUNUSED(event) )
{
    wxProgressDialog dialog("Progress dialog example",
                            // "Reserve" enough space for the multiline
                            // messages below, we'll change it anyhow
                            // immediately in the loop below
                            wxString(' ', 100) + "\n\n\n\n",
                            max_,    // range
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

    DoShowProgress(dialog);
}

#ifdef wxHAS_NATIVE_PROGRESSDIALOG
void MyFrame::ShowProgressGeneric( wxCommandEvent& WXUNUSED(event) )
{
    wxGenericProgressDialog dialog("Generic progress dialog example",
                                   wxString(' ', 100) + "\n\n\n\n",
                                   max_,
                                   this,
                                   wxPD_CAN_ABORT |
                                   wxPD_CAN_SKIP |
                                   wxPD_APP_MODAL |
                                   wxPD_ELAPSED_TIME |
                                   wxPD_ESTIMATED_TIME |
                                   wxPD_REMAINING_TIME |
                                   wxPD_SMOOTH);

    DoShowProgress(dialog);
}
#endif // wxHAS_NATIVE_PROGRESSDIALOG

void MyFrame::DoShowProgress(wxGenericProgressDialog& dialog)
{
    bool cont = true;
    for ( int i = 0; i <= max_; i++ )
    {
        wxString msg;

        // test both modes of wxProgressDialog behaviour: start in
        // indeterminate mode but switch to the determinate one later
        const bool determinate = i > max_/2;

        if ( i == max_ )
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
            i += max_/4;

            if ( i >= max_ )
                i = max_ - 1;
        }

        if ( !cont )
        {
            if ( wxMessageBox("Do you really want to cancel?",
                              "Progress dialog question",  // caption
                              wxYES_NO | wxICON_QUESTION) == wxYES )
                break;

            cont = true;
            dialog.Resume();
        }

        wxMilliSleep(100);
    }

    if ( !cont )
    {
        wxLogStatus("Progress dialog aborted!");
    }
    else
    {
        wxLogStatus("Countdown from %d finished", max_);
    }
}

#endif // wxUSE_PROGRESSDLG

void MyFrame::ShowAppProgress( wxCommandEvent& WXUNUSED(event) )
{
    wxAppProgressIndicator progress(this);
    if ( !progress.IsAvailable() )
    {
        wxLogStatus("Progress indicator not available under this platform.");
        return;
    }

    wxLogStatus("Using application progress indicator...");

    const int range = 10;
    progress.SetRange(range);
    for ( int i = 0; i < range; i++ )
    {
        progress.SetValue(i);

        wxMilliSleep(200);
    }

    wxLogStatus("Progress finished");
}

#if wxUSE_ABOUTDLG

static void InitAboutInfoMinimal(wxAboutDialogInfo& info)
{
    info.SetName("Dialogs Sample");
    info.SetVersion(wxVERSION_NUM_DOT_STRING,
                    wxString::Format
                    (
                        "%s version %s",
                        wxMINOR_VERSION % 2 ? "Development" : "Stable",
                        wxVERSION_NUM_DOT_STRING
                    ));
    info.SetDescription("This sample shows different wxWidgets dialogs");
    info.SetCopyright("(C) 1998-2006 wxWidgets dev team");
    info.AddDeveloper("Vadim Zeitlin");
}

static void InitAboutInfoWebsite(wxAboutDialogInfo& info)
{
    InitAboutInfoMinimal(info);

    info.SetWebSite("http://www.wxwidgets.org/", "wxWidgets web site");
}

static void InitAboutInfoAll(wxAboutDialogInfo& info)
{
    InitAboutInfoWebsite(info);

    // we can add a second developer
    info.AddDeveloper("A.N. Other");

    // or we can add several persons at once like this
    wxArrayString docwriters;
    docwriters.Add("First D. Writer");
    docwriters.Add("Second One");

    info.SetDocWriters(docwriters);
    info.SetLicence(wxString::FromAscii(
"                wxWindows Library Licence, Version 3.1\n"
"                ======================================\n"
"\n"
"  Copyright (c) 1998-2018 Julian Smart, Robert Roebling et al\n"
"\n"
"  Everyone is permitted to copy and distribute verbatim copies\n"
"  of this licence document, but changing it is not allowed.\n"
"\n"
"                       WXWINDOWS LIBRARY LICENCE\n"
"     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n"
"\n"
"                    ...and so on and so forth...\n"
    ));

    info.AddTranslator("Wun Ngo Wen (Martian)");
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
    virtual void DoAddCustomControls() wxOVERRIDE
    {
        AddControl(new wxStaticLine(this), wxSizerFlags().Expand());
        AddText("Some custom text");
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

    wxBusyInfo info("Working, please wait...", this);

    for ( int i = 0; i < 18; i++ )
    {
        wxMilliSleep(100);
        wxTheApp->Yield();
    }

    wxSleep(2);
}

void MyFrame::ShowRichBusyInfo(wxCommandEvent& WXUNUSED(event))
{
    wxWindowDisabler disableAll;

    // This is just an example and not an encouragement for printing
    // synchronously from the main thread.
    wxBusyInfo info
        (
            wxBusyInfoFlags()
                .Parent(this)
                .Icon(wxArtProvider::GetIcon(wxART_PRINT,
                                             wxART_OTHER, wxSize(128, 128)))
                .Title("<b>Printing your document</b>")
                .Text("Please wait...")
                .Foreground(*wxWHITE)
                .Background(*wxBLACK)
                .Transparency(4*wxALPHA_OPAQUE/5)
        );

    wxSleep(5);
}

#endif // wxUSE_BUSYINFO

#if wxUSE_FINDREPLDLG

void MyFrame::ShowReplaceDialog( wxCommandEvent& WXUNUSED(event) )
{
    if ( m_dlgReplace )
    {
        m_dlgReplace->Destroy();

        m_dlgReplace = NULL;
    }
    else
    {
        m_dlgReplace = new wxFindReplaceDialog
                           (
                            this,
                            &m_findData,
                            "Find and replace dialog",
                            wxFR_REPLACEDIALOG
                           );

        m_dlgReplace->Show(true);
    }
}

void MyFrame::ShowFindDialog( wxCommandEvent& WXUNUSED(event) )
{
    if ( m_dlgFind )
    {
        m_dlgFind->Destroy();

        m_dlgFind = NULL;
    }
    else
    {
        m_dlgFind = new wxFindReplaceDialog
                        (
                            this,
                            &m_findData,
                            "Find dialog",
                            // just for testing
                            wxFR_NOWHOLEWORD
                        );

        m_dlgFind->Show(true);
    }
}

static wxString DecodeFindDialogEventFlags(int flags)
{
    wxString str;
    str << (flags & wxFR_DOWN ? "down" : "up") << ", "
        << (flags & wxFR_WHOLEWORD ? "whole words only, " : "")
        << (flags & wxFR_MATCHCASE ? "" : "not ")
        << "case sensitive";

    return str;
}

void MyFrame::OnFindDialog(wxFindDialogEvent& event)
{
    wxEventType type = event.GetEventType();

    if ( type == wxEVT_FIND || type == wxEVT_FIND_NEXT )
    {
        wxLogMessage("Find %s'%s' (flags: %s)",
                     type == wxEVT_FIND_NEXT ? "next " : "",
                     event.GetFindString(),
                     DecodeFindDialogEventFlags(event.GetFlags()));
    }
    else if ( type == wxEVT_FIND_REPLACE ||
                type == wxEVT_FIND_REPLACE_ALL )
    {
        wxLogMessage("Replace %s'%s' with '%s' (flags: %s)",
                     type == wxEVT_FIND_REPLACE_ALL ? "all " : "",
                     event.GetFindString(),
                     event.GetReplaceString(),
                     DecodeFindDialogEventFlags(event.GetFlags()));
    }
    else if ( type == wxEVT_FIND_CLOSE )
    {
        wxFindReplaceDialog *dlg = event.GetDialog();

        int idMenu;
        wxString txt;
        if ( dlg == m_dlgFind )
        {
            txt = "Find";
            idMenu = DIALOGS_FIND;
            m_dlgFind = NULL;
        }
        else if ( dlg == m_dlgReplace )
        {
            txt = "Replace";
            idMenu = DIALOGS_REPLACE;
            m_dlgReplace = NULL;
        }
        else
        {
            txt = "Unknown";
            idMenu = wxID_ANY;

            wxFAIL_MSG( "unexpected event" );
        }

        wxLogMessage("%s dialog is being closed.", txt);

        if ( idMenu != wxID_ANY )
        {
            GetMenuBar()->Check(idMenu, false);
        }

        dlg->Destroy();
    }
    else
    {
        wxLogError("Unknown find dialog event!");
    }
}

#endif // wxUSE_FINDREPLDLG

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc.DrawText(
                "wxWidgets common dialogs"
                " test application"
                , 10, 10);
}

#if USE_MODAL_PRESENTATION

// ----------------------------------------------------------------------------
// MyModelessDialog
// ----------------------------------------------------------------------------

MyModelessDialog::MyModelessDialog(wxWindow *parent)
                : wxDialog(parent, wxID_ANY, wxString("Modeless dialog"))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    wxButton *btn = new wxButton(this, DIALOGS_MODELESS_BTN, "Press me");
    wxCheckBox *check = new wxCheckBox(this, wxID_ANY, "Should be disabled");
    check->Disable();

    sizerTop->Add(btn, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(check, 1, wxEXPAND | wxALL, 5);

    SetSizerAndFit(sizerTop);
}

void MyModelessDialog::OnButton(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Button pressed in modeless dialog", "Info",
                 wxOK | wxICON_INFORMATION, this);
}

void MyModelessDialog::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto() )
    {
        wxMessageBox("Use the menu item to close this dialog",
                     "Modeless dialog",
                     wxOK | wxICON_INFORMATION, this);

        event.Veto();
    }
}

// ----------------------------------------------------------------------------
// MyModalDialog
// ----------------------------------------------------------------------------

MyModalDialog::MyModalDialog(wxWindow *parent)
             : wxDialog(parent, wxID_ANY, wxString("Modal dialog"))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    m_btnModal = new wxButton(this, wxID_ANY, "&Modal dialog...");
    m_btnModeless = new wxButton(this, wxID_ANY, "Mode&less dialog");
    m_btnDelete = new wxButton(this, wxID_ANY, "&Delete button");

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
        wxGetTextFromUser("Dummy prompt",
                          "Modal dialog called from dialog",
                          wxEmptyString, this);
#else
        wxMessageBox("Modal dialog called from dialog");
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
    : wxDialog(parent, wxID_ANY, wxString("StdButtonSizer dialog"),
      wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
      m_buttonsSizer(NULL)
{
    wxBoxSizer *const sizerTop = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *const sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *const sizerInside1 = new wxBoxSizer(wxVERTICAL);

    m_chkboxAffirmativeButton = new wxCheckBox(this, wxID_ANY, "Enable Affirmative Button");

    wxStaticBoxSizer *const sizer1 = new wxStaticBoxSizer(wxVERTICAL, this, "Affirmative Button");

    m_radiobtnOk = new wxRadioButton(this, wxID_ANY, "Ok", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_radiobtnYes = new wxRadioButton(this, wxID_ANY, "Yes");

    wxBoxSizer *const sizerInside2 = new wxBoxSizer(wxVERTICAL);

    m_chkboxDismissButton = new wxCheckBox(this, wxID_ANY, "Enable Dismiss Button");

    wxStaticBoxSizer *const sizer2 = new wxStaticBoxSizer(wxVERTICAL, this, "Dismiss Button");

    m_radiobtnCancel = new wxRadioButton(this, wxID_ANY, "Cancel", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    m_radiobtnClose = new wxRadioButton(this, wxID_ANY, "Close");

    wxBoxSizer *const sizer3 = new wxBoxSizer(wxHORIZONTAL);

    m_chkboxNo = new wxCheckBox(this, wxID_ANY, "No");
    m_chkboxHelp = new wxCheckBox(this, wxID_ANY, "Help");
    m_chkboxApply = new wxCheckBox(this, wxID_ANY, "Apply");

    m_chkboxNoDefault = new wxCheckBox(this, wxID_ANY, "No Default");

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

wxIMPLEMENT_CLASS(SettingsDialog, wxPropertySheetDialog);

wxBEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
wxEND_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win, SettingsData& settingsData, int dialogType)
    : m_settingsData(settingsData)
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);

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

    Create(win, wxID_ANY, "Preferences", wxDefaultPosition, wxDefaultSize,
           wxDEFAULT_DIALOG_STYLE | resizeBorder);

    // If using a toolbook, also follow Mac style and don't create buttons
    if (!useToolBook)
        CreateButtons(wxOK | wxCANCEL | wxHELP);

    wxBookCtrlBase* notebook = GetBookCtrl();
    notebook->SetImageList(m_imageList);

    wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
    wxPanel* aestheticSettings = CreateAestheticSettingsPage(notebook);

    notebook->AddPage(generalSettings, "General", true, tabImage1);
    notebook->AddPage(aestheticSettings, "Aesthetics", false, tabImage2);

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
    wxCheckBox* checkBox3 = new wxCheckBox(panel, ID_LOAD_LAST_PROJECT, "&Load last project on startup", wxDefaultPosition, wxDefaultSize);
    checkBox3->SetValidator(wxGenericValidator(&m_settingsData.m_loadLastOnStartup));
    itemSizer3->Add(checkBox3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer3, 0, wxGROW|wxALL, 0);

    //// AUTOSAVE

    wxString autoSaveLabel = "&Auto-save every";
    wxString minsLabel = "mins";

    wxBoxSizer* itemSizer12 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* checkBox12 = new wxCheckBox(panel, ID_AUTO_SAVE, autoSaveLabel, wxDefaultPosition, wxDefaultSize);

#if wxUSE_SPINCTRL
    wxSpinCtrl* spinCtrl12 = new wxSpinCtrl(panel, ID_AUTO_SAVE_MINS, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 60, 1);
    spinCtrl12->SetValidator(wxGenericValidator(&m_settingsData.m_autoSaveInterval));
#endif

    itemSizer12->Add(checkBox12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
#if wxUSE_SPINCTRL
    itemSizer12->Add(spinCtrl12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
#endif
    itemSizer12->Add(new wxStaticText(panel, wxID_STATIC, minsLabel), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer12, 0, wxGROW|wxALL, 0);

    //// TOOLTIPS

    wxBoxSizer* itemSizer8 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* checkBox6 = new wxCheckBox(panel, ID_SHOW_TOOLTIPS, "Show &tooltips", wxDefaultPosition, wxDefaultSize);
    checkBox6->SetValidator(wxGenericValidator(&m_settingsData.m_showToolTips));
    itemSizer8->Add(checkBox6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer8, 0, wxGROW|wxALL, 0);

    topSizer->Add( item0, wxSizerFlags(1).Expand().Border(wxALL, 5) );

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
    globalOrProjectChoices[0] = "&New projects";
    globalOrProjectChoices[1] = "&This project";

    wxRadioBox* projectOrGlobal = new wxRadioBox(panel, ID_APPLY_SETTINGS_TO, "&Apply settings to:",
        wxDefaultPosition, wxDefaultSize, 2, globalOrProjectChoices);
    projectOrGlobal->SetValidator(wxGenericValidator(&m_settingsData.m_applyTo));
    item0->Add(projectOrGlobal, 0, wxGROW|wxALL, 5);

    projectOrGlobal->SetSelection(0);

    //// BACKGROUND STYLE
    wxArrayString backgroundStyleChoices;
    backgroundStyleChoices.Add("Colour");
    backgroundStyleChoices.Add("Image");
    wxStaticBox* staticBox3 = new wxStaticBox(panel, wxID_ANY, "Background style:");

    wxBoxSizer* styleSizer = new wxStaticBoxSizer( staticBox3, wxVERTICAL );
    item0->Add(styleSizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemSizer2 = new wxBoxSizer( wxHORIZONTAL );

    wxChoice* choice2 = new wxChoice(panel, ID_BACKGROUND_STYLE, wxDefaultPosition, wxDefaultSize, backgroundStyleChoices);
    choice2->SetValidator(wxGenericValidator(&m_settingsData.m_bgStyle));

    itemSizer2->Add(new wxStaticText(panel, wxID_ANY, "&Window:"), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    itemSizer2->Add(5, 5, 1, wxALL, 0);
    itemSizer2->Add(choice2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    styleSizer->Add(itemSizer2, 0, wxGROW|wxALL, 5);

#if wxUSE_SPINCTRL
    //// FONT SIZE SELECTION

    wxStaticBox* staticBox1 = new wxStaticBox(panel, wxID_ANY, "Tile font size:");
    wxBoxSizer* itemSizer5 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );

    wxSpinCtrl* spinCtrl = new wxSpinCtrl(panel, ID_FONT_SIZE, wxEmptyString);
    spinCtrl->SetValidator(wxGenericValidator(&m_settingsData.m_titleFontSize));
    itemSizer5->Add(spinCtrl, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    item0->Add(itemSizer5, 0, wxGROW|wxLEFT|wxRIGHT, 5);
#endif

    topSizer->Add( item0, wxSizerFlags(1).Expand().Border(wxALL, 5) );
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

wxBEGIN_EVENT_TABLE(TestMessageBoxDialog, wxDialog)
    EVT_BUTTON(wxID_APPLY, TestMessageBoxDialog::OnApply)
    EVT_BUTTON(wxID_CLOSE, TestMessageBoxDialog::OnClose)
wxEND_EVENT_TABLE()

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
    sizerMsgs->Add(new wxStaticText(this, wxID_ANY, "&Title:"));
    m_textTitle = new wxTextCtrl(this, wxID_ANY, "Test Message Box");
    sizerMsgs->Add(m_textTitle, wxSizerFlags().Expand().Border(wxBOTTOM));

    sizerMsgs->Add(new wxStaticText(this, wxID_ANY, "&Main message:"));
    m_textMsg = new wxTextCtrl(this, wxID_ANY, "Hello from a box!",
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE);
    sizerMsgs->Add(m_textMsg, wxSizerFlags(1).Expand().Border(wxBOTTOM));

    sizerMsgs->Add(new wxStaticText(this, wxID_ANY, "&Extended message:"));
    m_textExtMsg = new wxTextCtrl(this, wxID_ANY, "",
                                  wxDefaultPosition, wxDefaultSize,
                                  wxTE_MULTILINE);
    sizerMsgs->Add(m_textExtMsg, wxSizerFlags().Expand());

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
        sizerBtns->Add(m_labels[n], wxSizerFlags().Expand());

        m_labels[n]->Bind(wxEVT_UPDATE_UI,
                          &TestMessageBoxDialog::OnUpdateLabelUI, this);
    }

    sizerBtnsBox->Add(sizerBtns, wxSizerFlags().Expand());
    sizerTop->Add(sizerBtnsBox, wxSizerFlags().Expand().Border());


    // icon choice
    const wxString icons[] =
    {
        "&Not specified",
        "E&xplicitly none",
        "&Information icon",
        "&Question icon",
        "&Warning icon",
        "&Error icon",
        "A&uth needed icon"
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
    m_chkNoDefault->Bind(wxEVT_UPDATE_UI,
                         &TestMessageBoxDialog::OnUpdateNoDefaultUI, this);
    sizerFlags->Add(m_chkNoDefault, wxSizerFlags().Border());

    m_chkCentre = new wxCheckBox(this, wxID_ANY, "Centre on &parent");
    sizerFlags->Add(m_chkCentre, wxSizerFlags().Border());

    // add any additional flag from subclasses
    AddAdditionalFlags(sizerFlags);

    sizerTop->Add(sizerFlags, wxSizerFlags().Expand().Border());

    // add the currently unused zone for displaying the dialog result
    m_labelResult = new wxStaticText(this, wxID_ANY, "",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxST_NO_AUTORESIZE | wxALIGN_CENTRE);
    m_labelResult->SetForegroundColour(*wxBLUE);
    sizerTop->Add(m_labelResult, wxSizerFlags().Expand().DoubleBorder());

    // finally buttons to show the resulting message box and close this dialog
    sizerTop->Add(CreateStdDialogButtonSizer(wxAPPLY | wxCLOSE),
                  wxSizerFlags().Right().Border());

    SetSizerAndFit(sizerTop);

    m_buttons[Btn_Ok]->SetValue(true);

    CentreOnScreen();

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
            wxFALLTHROUGH;

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

        case MsgDlgIcon_AuthNeeded:
            style |= wxICON_AUTH_NEEDED;
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
    wxMessageDialog dlg(this, GetMessage(), GetBoxTitle(), GetStyle());
    PrepareMessageDialog(dlg);

    ShowResult(dlg.ShowModal());
}

void TestMessageBoxDialog::ShowResult(int res)
{
    wxString btnName;
    switch ( res )
    {
        case wxID_OK:
            btnName = "OK";
            break;

        case wxID_CANCEL:
            btnName = "Cancel";
            break;

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

    m_labelResult->SetLabel(
        wxString::Format("Dialog was closed with the \"%s\" button.", btnName)
    );
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

wxBEGIN_EVENT_TABLE(TestRichMessageDialog, TestMessageBoxDialog)
    EVT_BUTTON(wxID_APPLY, TestRichMessageDialog::OnApply)
wxEND_EVENT_TABLE()

TestRichMessageDialog::TestRichMessageDialog(wxWindow *parent)
                     : TestMessageBoxDialog(parent)
{
    SetTitle("Rich Message Dialog Test Dialog");
}

void TestRichMessageDialog::AddAdditionalTextOptions(wxSizer *sizer)
{
    wxSizer * const sizerMsgs = new wxStaticBoxSizer(wxVERTICAL, this,
                                                     "&Additional Elements");

    // add an option to show a check box.
    wxSizer * const sizerCheckBox = new wxBoxSizer(wxHORIZONTAL);
    sizerCheckBox->Add(new wxStaticText(this, wxID_ANY, "&Check box:"),
                       wxSizerFlags().Centre().Border(wxRIGHT));
    m_textCheckBox = new wxTextCtrl(this, wxID_ANY);
    sizerCheckBox->Add(m_textCheckBox, wxSizerFlags(1).Centre());
    sizerMsgs->Add(sizerCheckBox, wxSizerFlags().Expand().Border(wxBOTTOM));

    // add option to show a detailed text.
    sizerMsgs->Add(new wxStaticText(this, wxID_ANY, "&Detailed message:"));
    m_textDetailed = new wxTextCtrl(this, wxID_ANY, "",
                                    wxDefaultPosition, wxDefaultSize,
                                    wxTE_MULTILINE);
    sizerMsgs->Add(m_textDetailed, wxSizerFlags().Expand());

    // add option to show footer text
    wxSizer * const sizerFooter = new wxBoxSizer(wxHORIZONTAL);
    sizerFooter->Add(new wxStaticText(this, wxID_ANY, "&Footer Text:"),
        wxSizerFlags().Centre().Border(wxRIGHT));
    m_textFooter = new wxTextCtrl(this, wxID_ANY);
    sizerFooter->Add(m_textFooter, wxSizerFlags(1).Centre());

    // add option to select footer icon
    const wxString icons[] =
    {
        "None",
        "Info",
        "Warning",
        "Error",
        "Auth needed"
    };

    sizerFooter->Add(new wxStaticText(this, wxID_ANY, "Icon:"),
        wxSizerFlags().Centre().Border(wxLEFT));
    m_iconsFooter = new wxChoice(this, wxID_ANY,
        wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(icons), icons);
    // Make the None the default:
    m_iconsFooter->SetSelection(0);
    sizerFooter->Add(m_iconsFooter, wxSizerFlags().Expand().Border());

    sizerMsgs->Add(sizerFooter, wxSizerFlags().Expand().Border(wxTOP));

    sizer->Add(sizerMsgs, wxSizerFlags().Expand().Border());
}

void TestRichMessageDialog::AddAdditionalFlags(wxSizer *sizer)
{
    // add checkbox to set the initial state for the checkbox shown
    // in the dialog.
    m_initialValueCheckBox =
        new wxCheckBox(this, wxID_ANY, "Checkbox initially checked");
    sizer->Add(m_initialValueCheckBox, wxSizerFlags().Border());
}

void TestRichMessageDialog::OnApply(wxCommandEvent& WXUNUSED(event))
{
    wxRichMessageDialog dlg(this, GetMessage(), GetBoxTitle(), GetStyle());
    PrepareMessageDialog(dlg);

    dlg.ShowCheckBox(m_textCheckBox->GetValue(),
                     m_initialValueCheckBox->GetValue());
    dlg.ShowDetailedText(m_textDetailed->GetValue());
    dlg.SetFooterText(m_textFooter->GetValue());
    switch ( m_iconsFooter->GetSelection() )
    {
        case 1:
            dlg.SetFooterIcon(wxICON_INFORMATION);
            break;

        case 2:
            dlg.SetFooterIcon(wxICON_WARNING);
            break;

        case 3:
            dlg.SetFooterIcon(wxICON_ERROR);
            break;

        case 4:
            dlg.SetFooterIcon(wxICON_AUTH_NEEDED);
            break;
    }

    ShowResult(dlg.ShowModal());
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
                                        int style) wxOVERRIDE
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
