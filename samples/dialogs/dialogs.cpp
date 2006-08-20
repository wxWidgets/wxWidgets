/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.cpp
// Purpose:     Common dialogs demo
// Author:      Julian Smart
// Modified by: ABX (2004) - adjustements for conditional building + new menu
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
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

#include "wx/datetime.h"
#include "wx/image.h"
#include "wx/bookctrl.h"
#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/sysopt.h"

#if wxUSE_COLOURDLG
    #include "wx/colordlg.h"
#endif // wxUSE_COLOURDLG

#if wxUSE_CHOICEDLG
    #include "wx/choicdlg.h"
#endif // wxUSE_CHOICEDLG

#if wxUSE_STARTUP_TIPS
    #include "wx/tipdlg.h"
#endif // wxUSE_STARTUP_TIPS

#if wxUSE_PROGRESSDLG
#if wxUSE_STOPWATCH && wxUSE_LONGLONG
    #include "wx/datetime.h"      // wxDateTime
#endif

    #include "wx/progdlg.h"
#endif // wxUSE_PROGRESSDLG

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

#if wxUSE_SPINCTRL
#include "wx/spinctrl.h"
#endif

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
    EVT_MENU(DIALOGS_MESSAGE_BOX,                   MyFrame::MessageBox)

#if wxUSE_COLOURDLG
    EVT_MENU(DIALOGS_CHOOSE_COLOUR,                 MyFrame::ChooseColour)
#endif // wxUSE_COLOURDLG

#if wxUSE_FONTDLG
    EVT_MENU(DIALOGS_CHOOSE_FONT,                   MyFrame::ChooseFont)
#endif // wxUSE_FONTDLG

#if wxUSE_LOG_DIALOG
    EVT_MENU(DIALOGS_LOG_DIALOG,                    MyFrame::LogDialog)
#endif // wxUSE_LOG_DIALOG

#if wxUSE_TEXTDLG
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
    EVT_MENU(DIALOGS_MODELESS,                      MyFrame::ModelessDlg)
    EVT_MENU(DIALOGS_CENTRE_SCREEN,                 MyFrame::DlgCenteredScreen)
    EVT_MENU(DIALOGS_CENTRE_PARENT,                 MyFrame::DlgCenteredParent)
#endif // USE_MODAL

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
#endif

    EVT_MENU(DIALOGS_REQUEST,                       MyFrame::OnRequestUserAttention)

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

MyCanvas *myCanvas = (MyCanvas *) NULL;

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
#if wxUSE_IMAGE
    wxInitAllImageHandlers();
#endif

    m_canvasTextColour = *wxBLACK;
    m_canvasFont = *wxNORMAL_FONT;

    // Create the main frame window
    MyFrame *frame = new MyFrame((wxFrame *) NULL, _T("wxWidgets dialogs example"));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(DIALOGS_MESSAGE_BOX, _T("&Message box\tCtrl-M"));


#if wxUSE_COLOURDLG || wxUSE_FONTDLG || wxUSE_CHOICEDLG

    wxMenu *choices_menu = new wxMenu;

    #if wxUSE_COLOURDLG
        choices_menu->Append(DIALOGS_CHOOSE_COLOUR, _T("&Choose colour"));
    #endif // wxUSE_COLOURDLG

    #if wxUSE_FONTDLG
        choices_menu->Append(DIALOGS_CHOOSE_FONT, _T("Choose &font"));
    #endif // wxUSE_FONTDLG

    #if wxUSE_CHOICEDLG
        choices_menu->Append(DIALOGS_SINGLE_CHOICE,  _T("&Single choice\tCtrl-C"));
        choices_menu->Append(DIALOGS_MULTI_CHOICE,  _T("M&ultiple choice\tCtrl-U"));
    #endif // wxUSE_CHOICEDLG

    #if USE_COLOURDLG_GENERIC || USE_FONTDLG_GENERIC
        choices_menu->AppendSeparator();
    #endif // USE_COLOURDLG_GENERIC || USE_FONTDLG_GENERIC

    #if USE_COLOURDLG_GENERIC
        choices_menu->Append(DIALOGS_CHOOSE_COLOUR_GENERIC, _T("&Choose colour (generic)"));
    #endif // USE_COLOURDLG_GENERIC

    #if USE_FONTDLG_GENERIC
        choices_menu->Append(DIALOGS_CHOOSE_FONT_GENERIC, _T("Choose &font (generic)"));
    #endif // USE_FONTDLG_GENERIC

    file_menu->Append(wxID_ANY,_T("&Choices and selectors"),choices_menu);
#endif // wxUSE_COLOURDLG || wxUSE_FONTDLG || wxUSE_CHOICEDLG


#if wxUSE_TEXTDLG || wxUSE_NUMBERDLG

    wxMenu *entry_menu = new wxMenu;

    #if wxUSE_TEXTDLG
        entry_menu->Append(DIALOGS_TEXT_ENTRY,  _T("Text &entry\tCtrl-E"));
        entry_menu->Append(DIALOGS_PASSWORD_ENTRY,  _T("&Password entry\tCtrl-P"));
    #endif // wxUSE_TEXTDLG

    #if wxUSE_NUMBERDLG
        entry_menu->Append(DIALOGS_NUM_ENTRY, _T("&Numeric entry\tCtrl-N"));
    #endif // wxUSE_NUMBERDLG

    file_menu->Append(wxID_ANY,_T("&Entry dialogs"),entry_menu);

#endif // wxUSE_TEXTDLG || wxUSE_NUMBERDLG


#if wxUSE_FILEDLG

    wxMenu *filedlg_menu = new wxMenu;
    filedlg_menu->Append(DIALOGS_FILE_OPEN,  _T("&Open file\tCtrl-O"));
    filedlg_menu->Append(DIALOGS_FILE_OPEN2,  _T("&Second open file\tCtrl-2"));
    filedlg_menu->Append(DIALOGS_FILES_OPEN,  _T("Open &files\tCtrl-Q"));
    filedlg_menu->Append(DIALOGS_FILE_SAVE,  _T("Sa&ve file\tCtrl-S"));

    #if USE_FILEDLG_GENERIC
        filedlg_menu->AppendSeparator();
        filedlg_menu->Append(DIALOGS_FILE_OPEN_GENERIC,  _T("&Open file (generic)"));
        filedlg_menu->Append(DIALOGS_FILES_OPEN_GENERIC,  _T("Open &files (generic)"));
        filedlg_menu->Append(DIALOGS_FILE_SAVE_GENERIC,  _T("Sa&ve file (generic)"));
    #endif // USE_FILEDLG_GENERIC

    file_menu->Append(wxID_ANY,_T("&File operations"),filedlg_menu);

#endif // wxUSE_FILEDLG

#if wxUSE_DIRDLG
    wxMenu *dir_menu = new wxMenu;

    dir_menu->Append(DIALOGS_DIR_CHOOSE,  _T("&Choose a directory\tCtrl-D"));
    dir_menu->Append(DIALOGS_DIRNEW_CHOOSE,  _T("Choose a directory (with \"Ne&w\" button)\tShift-Ctrl-D"));
    file_menu->Append(wxID_ANY,_T("&Directory operations"),dir_menu);

    #if USE_DIRDLG_GENERIC
        dir_menu->AppendSeparator();
        dir_menu->Append(DIALOGS_GENERIC_DIR_CHOOSE,  _T("&Choose a directory (generic)"));
    #endif // USE_DIRDLG_GENERIC

#endif // wxUSE_DIRDLG


#if wxUSE_STARTUP_TIPS || wxUSE_PROGRESSDLG || wxUSE_BUSYINFO || wxUSE_LOG_DIALOG

    wxMenu *info_menu = new wxMenu;

    #if wxUSE_STARTUP_TIPS
        info_menu->Append(DIALOGS_TIP,  _T("&Tip of the day\tCtrl-T"));
    #endif // wxUSE_STARTUP_TIPS

    #if wxUSE_PROGRESSDLG
        info_menu->Append(DIALOGS_PROGRESS, _T("Pro&gress dialog\tCtrl-G"));
    #endif // wxUSE_PROGRESSDLG

    #if wxUSE_BUSYINFO
       info_menu->Append(DIALOGS_BUSYINFO, _T("&Busy info dialog\tCtrl-B"));
    #endif // wxUSE_BUSYINFO

    #if wxUSE_LOG_DIALOG
       info_menu->Append(DIALOGS_LOG_DIALOG, _T("&Log dialog\tCtrl-L"));
    #endif // wxUSE_LOG_DIALOG

    file_menu->Append(wxID_ANY,_T("&Informative dialogs"),info_menu);

#endif // wxUSE_STARTUP_TIPS || wxUSE_PROGRESSDLG || wxUSE_BUSYINFO || wxUSE_LOG_DIALOG


#if wxUSE_FINDREPLDLG
    wxMenu *find_menu = new wxMenu;
    find_menu->AppendCheckItem(DIALOGS_FIND, _T("&Find dialog\tCtrl-F"));
    find_menu->AppendCheckItem(DIALOGS_REPLACE, _T("Find and &replace dialog\tShift-Ctrl-F"));
    file_menu->Append(wxID_ANY,_T("&Searching"),find_menu);
#endif // wxUSE_FINDREPLDLG

#if USE_MODAL_PRESENTATION
    wxMenu *dialogs_menu = new wxMenu;
    dialogs_menu->Append(DIALOGS_MODAL, _T("&Modal dialog\tCtrl-W"));
    dialogs_menu->AppendCheckItem(DIALOGS_MODELESS, _T("Mode&less dialog\tCtrl-Z"));
    dialogs_menu->Append(DIALOGS_CENTRE_SCREEN, _T("Centered on &screen\tShift-Ctrl-1"));
    dialogs_menu->Append(DIALOGS_CENTRE_PARENT, _T("Centered on &parent\tShift-Ctrl-2"));
    file_menu->Append(wxID_ANY, _T("&Generic dialogs"), dialogs_menu);
#endif // USE_MODAL_PRESENTATION

#if USE_SETTINGS_DIALOG
    wxMenu *sheet_menu = new wxMenu;
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET, _T("&Standard property sheet\tShift-Ctrl-P"));
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET_TOOLBOOK, _T("&Toolbook sheet\tShift-Ctrl-T"));

    if (wxPlatformIs(wxMac))
        sheet_menu->Append(DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK, _T("Button &Toolbook sheet\tShift-Ctrl-U"));
/*
#ifdef __WXMAC__
    sheet_menu->Append(DIALOGS_PROPERTY_SHEET_BUTTONTOOLBOOK, _T("Button &Toolbook sheet\tShift-Ctrl-U"));
#endif
*/
    file_menu->Append(wxID_ANY, _T("&Property sheets"), sheet_menu);
#endif // USE_SETTINGS_DIALOG

    file_menu->Append(DIALOGS_REQUEST, _T("&Request user attention\tCtrl-R"));

    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _T("E&xit\tAlt-X"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    frame->SetMenuBar(menu_bar);

    myCanvas = new MyCanvas(frame);
    myCanvas->SetBackgroundColour(*wxWHITE);

    frame->Centre(wxBOTH);

    // Show the frame
    frame->Show(true);

    SetTopWindow(frame);

    return true;
}

// My frame constructor
MyFrame::MyFrame(wxWindow *parent,
                 const wxString& title)
       : wxFrame(parent, wxID_ANY, title)
{
    SetIcon(sample_xpm);

#if USE_MODAL_PRESENTATION
    m_dialog = (MyModelessDialog *)NULL;
#endif // USE_MODAL_PRESENTATION

#if wxUSE_FINDREPLDLG
    m_dlgFind =
    m_dlgReplace = NULL;
#endif

#if wxUSE_COLOURDLG
    m_clrData.SetChooseFull(true);
    for (int i = 0; i < 16; i++)
    {
        m_clrData.SetCustomColour(
          i,
          wxColour(
            (unsigned char)(i*16),
            (unsigned char)(i*16),
            (unsigned char)(i*16)
          )
        );
    }
#endif // wxUSE_COLOURDLG

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR
}

#if wxUSE_COLOURDLG
void MyFrame::ChooseColour(wxCommandEvent& WXUNUSED(event) )
{
    m_clrData.SetColour(myCanvas->GetBackgroundColour());

    wxColourDialog dialog(this, &m_clrData);
    dialog.SetTitle(_T("Choose the background colour"));
    if (dialog.ShowModal() == wxID_OK)
    {
        m_clrData = dialog.GetColourData();
        myCanvas->SetBackgroundColour(m_clrData.GetColour());
        myCanvas->ClearBackground();
        myCanvas->Refresh();
    }
}
#endif // wxUSE_COLOURDLG

#if USE_COLOURDLG_GENERIC
void MyFrame::ChooseColourGeneric(wxCommandEvent& WXUNUSED(event))
{
    m_clrData.SetColour(myCanvas->GetBackgroundColour());

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
        myCanvas->SetBackgroundColour(m_clrData.GetColour());
        myCanvas->ClearBackground();
        myCanvas->Refresh();
    }
    dialog->Destroy();
}
#endif // USE_COLOURDLG_GENERIC

#if wxUSE_FONTDLG
void MyFrame::ChooseFont(wxCommandEvent& WXUNUSED(event) )
{
    wxFontData data;
    data.SetInitialFont(wxGetApp().m_canvasFont);
    data.SetColour(wxGetApp().m_canvasTextColour);

    // you might also do this:
    //
    //  wxFontDialog dialog;
    //  if ( !dialog.Create(this, data) { ... error ... }
    //
    wxFontDialog dialog(this, data);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog.GetFontData();
        wxGetApp().m_canvasFont = retData.GetChosenFont();
        wxGetApp().m_canvasTextColour = retData.GetColour();
        myCanvas->Refresh();
    }
    //else: cancelled by the user, don't change the font
}
#endif // wxUSE_FONTDLG

#if USE_FONTDLG_GENERIC
void MyFrame::ChooseFontGeneric(wxCommandEvent& WXUNUSED(event) )
{
    wxFontData data;
    data.SetInitialFont(wxGetApp().m_canvasFont);
    data.SetColour(wxGetApp().m_canvasTextColour);

    wxGenericFontDialog *dialog = new wxGenericFontDialog(this, data);
    if (dialog->ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog->GetFontData();
        wxGetApp().m_canvasFont = retData.GetChosenFont();
        wxGetApp().m_canvasTextColour = retData.GetColour();
        myCanvas->Refresh();
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

void MyFrame::MessageBox(wxCommandEvent& WXUNUSED(event) )
{
    wxMessageDialog dialog( NULL, _T("This is a message box\nA long, long string to test out the message box properly"),
        _T("Message box text"), wxNO_DEFAULT|wxYES_NO|wxCANCEL|wxICON_INFORMATION);

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

#if wxUSE_NUMBERDLG
void MyFrame::NumericEntry(wxCommandEvent& WXUNUSED(event) )
{
    long res = wxGetNumberFromUser( _T("This is some text, actually a lot of text.\n")
                                    _T("Even two rows of text."),
                                    _T("Enter a number:"), _T("Numeric input test"),
                                     50, 0, 100, this );

    wxString msg;
    int icon;
    if ( res == -1 )
    {
        msg = _T("Invalid number entered or dialog cancelled.");
        icon = wxICON_HAND;
    }
    else
    {
        msg.Printf(_T("You've entered %lu"), res );
        icon = wxICON_INFORMATION;
    }

    wxMessageBox(msg, _T("Numeric test result"), wxOK | icon, this);
}
#endif // wxUSE_NUMBERDLG

#if wxUSE_TEXTDLG
void MyFrame::PasswordEntry(wxCommandEvent& WXUNUSED(event))
{
    wxString pwd = wxGetPasswordFromUser(_T("Enter password:"),
                                         _T("Password entry dialog"),
                                         wxEmptyString,
                                         this);
    if ( !pwd.empty() )
    {
        wxMessageBox(wxString::Format(wxT("Your password is '%s'"), pwd.c_str()),
                     _T("Got password"), wxOK | wxICON_INFORMATION, this);
    }
}

void MyFrame::TextEntry(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog(this,
                             _T("This is a small sample\n")
                             _T("A long, long string to test out the text entrybox"),
                             _T("Please enter a string"),
                             _T("Default value"),
                             wxOK | wxCANCEL);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageBox(dialog.GetValue(), _T("Got string"), wxOK | wxICON_INFORMATION, this);
    }
}
#endif // wxUSE_TEXTDLG

#if wxUSE_CHOICEDLG
void MyFrame::SingleChoice(wxCommandEvent& WXUNUSED(event) )
{
    const wxString choices[] = { _T("One"), _T("Two"), _T("Three"), _T("Four"), _T("Five") } ;

    wxSingleChoiceDialog dialog(this,
                                _T("This is a small sample\n")
                                _T("A single-choice convenience dialog"),
                                _T("Please select a value"),
                                WXSIZEOF(choices), choices);

    dialog.SetSelection(2);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetStringSelection(), _T("Got string"));
        dialog2.ShowModal();
    }
}

void MyFrame::MultiChoice(wxCommandEvent& WXUNUSED(event) )
{
    const wxString choices[] =
    {
        _T("One"), _T("Two"), _T("Three"), _T("Four"), _T("Five"),
        _T("Six"), _T("Seven"), _T("Eight"), _T("Nine"), _T("Ten"),
        _T("Eleven"), _T("Twelve"), _T("Seventeen"),
    };

    wxArrayInt selections;
    size_t count = wxGetMultipleChoices(selections,
                                        _T("This is a small sample\n")
                                        _T("A multi-choice convenience dialog"),
                                        _T("Please select a value"),
                                        WXSIZEOF(choices), choices,
                                        this);
    if ( count )
    {
        wxString msg;
        msg.Printf(wxT("You selected %u items:\n"), (unsigned)count);
        for ( size_t n = 0; n < count; n++ )
        {
            msg += wxString::Format(wxT("\t%u: %u (%s)\n"),
                                    (unsigned)n, (unsigned)selections[n],
                                    choices[selections[n]].c_str());
        }
        wxLogMessage(msg);
    }
    //else: cancelled or nothing selected
}
#endif // wxUSE_CHOICEDLG

#if wxUSE_FILEDLG
void MyFrame::FileOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog
                 (
                    this,
                    _T("Testing open file dialog"),
                    wxEmptyString,
                    wxEmptyString,
#ifdef __WXMOTIF__
                    _T("C++ files (*.cpp)|*.cpp")
#else
                    _T("C++ files (*.cpp;*.h)|*.cpp;*.h")
#endif
                 );

    dialog.CentreOnParent();
    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString info;
        info.Printf(_T("Full file name: %s\n")
                    _T("Path: %s\n")
                    _T("Name: %s"),
                    dialog.GetPath().c_str(),
                    dialog.GetDirectory().c_str(),
                    dialog.GetFilename().c_str());
        wxMessageDialog dialog2(this, info, _T("Selected file"));
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
                                    _T("Select the file to load"),
                                    wxEmptyString, wxEmptyString,
                                    s_extDef,
                                    wxString::Format
                                    (
                                        _T("Waveform (*.wav)|*.wav|Plain text (*.txt)|*.txt|All files (%s)|%s"),
                                        wxFileSelectorDefaultWildcardStr,
                                        wxFileSelectorDefaultWildcardStr
                                    ),
                                    wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
                                    this
                                   );

    if ( !path )
        return;

    // it is just a sample, would use wxSplitPath in real program
    s_extDef = path.AfterLast(_T('.'));

    wxLogMessage(_T("You selected the file '%s', remembered extension '%s'"),
                 (const wxChar*) path, (const wxChar*) s_extDef);
}

void MyFrame::FilesOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxString wildcards =
#ifdef __WXMOTIF__
                    _T("C++ files (*.cpp)|*.cpp");
#else
                    wxString::Format
                    (
                        _T("All files (%s)|%s|C++ files (*.cpp;*.h)|*.cpp;*.h"),
                        wxFileSelectorDefaultWildcardStr,
                        wxFileSelectorDefaultWildcardStr
                    );
#endif
    wxFileDialog dialog(this, _T("Testing open multiple file dialog"),
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
            s.Printf(_T("File %d: %s (%s)\n"),
                     (int)n, paths[n].c_str(), filenames[n].c_str());

            msg += s;
        }
        s.Printf(_T("Filter index: %d"), dialog.GetFilterIndex());
        msg += s;

        wxMessageDialog dialog2(this, msg, _T("Selected files"));
        dialog2.ShowModal();
    }
}

void MyFrame::FileSave(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog(this,
                        _T("Testing save file dialog"),
                        wxEmptyString,
                        _T("myletter.doc"),
                        _T("Text files (*.txt)|*.txt|Document files (*.doc)|*.doc"),
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage(_T("%s, filter %d"),
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
                    _T("Testing open file dialog"),
                    wxEmptyString,
                    wxEmptyString,
                    _T("C++ files (*.cpp;*.h)|*.cpp;*.h")
                 );

    dialog.SetDirectory(wxGetHomeDir());

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString info;
        info.Printf(_T("Full file name: %s\n")
                    _T("Path: %s\n")
                    _T("Name: %s"),
                    dialog.GetPath().c_str(),
                    dialog.GetDirectory().c_str(),
                    dialog.GetFilename().c_str());
        wxMessageDialog dialog2(this, info, _T("Selected file"));
        dialog2.ShowModal();
    }
}

void MyFrame::FilesOpenGeneric(wxCommandEvent& WXUNUSED(event) )
{
    // On PocketPC you can disable OK-only dialogs policy using system option
    int buttons = wxSystemOptions::GetOptionInt(wxT("wince.dialog.real-ok-cancel"));
    wxSystemOptions::SetOption(wxT("wince.dialog.real-ok-cancel"), 1);

    wxString wildcards = _T("All files (*.*)|*.*|C++ files (*.cpp;*.h)|*.cpp;*.h");
    wxGenericFileDialog dialog(this, _T("Testing open multiple file dialog"),
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
            s.Printf(_T("File %d: %s (%s)\n"),
                     (int)n, paths[n].c_str(), filenames[n].c_str());

            msg += s;
        }
        s.Printf(_T("Filter index: %d"), dialog.GetFilterIndex());
        msg += s;

        wxMessageDialog dialog2(this, msg, _T("Selected files"));
        dialog2.ShowModal();
    }

    // restore system option
    wxSystemOptions::SetOption(wxT("wince.dialog.real-ok-cancel"), buttons);
}

void MyFrame::FileSaveGeneric(wxCommandEvent& WXUNUSED(event) )
{
    wxGenericFileDialog dialog(this,
                        _T("Testing save file dialog"),
                        wxEmptyString,
                        _T("myletter.doc"),
                        _T("Text files (*.txt)|*.txt|Document files (*.doc)|*.doc"),
                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage(_T("%s, filter %d"),
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

    wxDirDialog dialog(this, _T("Testing directory picker"), dirHome, style);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage(_T("Selected path: %s"), dialog.GetPath().c_str());
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

    wxGenericDirDialog dialog(this, _T("Testing generic directory picker"), dirHome);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetPath(), _T("Selected path"));
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
    wxDialog dlg(this, wxID_ANY, _T("Dialog centered on screen"),
                 wxDefaultPosition, wxSize(200, 100));
    (new wxButton(&dlg, wxID_OK, _T("Close")))->Centre();
    dlg.CentreOnScreen();
    dlg.ShowModal();
}

void MyFrame::DlgCenteredParent(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg(this, wxID_ANY, _T("Dialog centered on parent"),
                 wxDefaultPosition, wxSize(200, 100));
    (new wxButton(&dlg, wxID_OK, _T("Close")))->Centre();
    dlg.CentreOnParent();
    dlg.ShowModal();
}

#endif // USE_MODAL_PRESENTATION

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

    wxTipProvider *tipProvider = wxCreateFileTipProvider(_T("tips.txt"), s_index);

    bool showAtStartup = wxShowTip(this, tipProvider);

    if ( showAtStartup )
    {
        wxMessageBox(_T("Will show tips on startup"), _T("Tips dialog"),
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
    wxLogStatus(_T("Sleeping for 3 seconds to allow you to switch to another window"));

    wxSleep(3);

    RequestUserAttention(wxUSER_ATTENTION_ERROR);
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

#if wxUSE_PROGRESSDLG

void MyFrame::ShowProgress( wxCommandEvent& WXUNUSED(event) )
{
    #if wxUSE_STOPWATCH && wxUSE_LONGLONG
    // check the performance
    int countrandomnumbers = 0, count = 0;
    wxTimeSpan tsTest(0,0,0,250);
    wxDateTime DT2, DT1 = wxDateTime::UNow();
    srand(0);
    while(1)
    {
        rand();
        ++countrandomnumbers;
        if ( countrandomnumbers == 1000 )
        {
            srand(0);
            countrandomnumbers = 0;
            ++count;
            DT2 = wxDateTime::UNow();
            wxTimeSpan ts = DT2.Subtract( DT1 );
            if ( ts.IsLongerThan( tsTest ) )
            {
                break;
            }
        }
    }
    const int max = 40 * count;
    #else
    static const int max = 10;
    #endif // wxUSE_STOPWATCH && wxUSE_LONGLONG

    wxProgressDialog dialog(_T("Progress dialog example"),
                            _T("An informative message"),
                            max,    // range
                            this,   // parent
                            wxPD_CAN_ABORT |
                            wxPD_CAN_SKIP |
                            wxPD_APP_MODAL |
                            // wxPD_AUTO_HIDE | -- try this as well
                            wxPD_ELAPSED_TIME |
                            wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME |
                            wxPD_SMOOTH);

    bool cont = true;
    bool skip = false;
    // each skip will move progress about quarter forward
    for ( int i = 0; i <= max; i = wxMin(i+(skip?int(max/4):1), max+1), skip = false )
    {
        #if wxUSE_STOPWATCH && wxUSE_LONGLONG
        // do (almost) the same operations as we did for the performance test
        srand(0);
        for ( int j = 0; j < 1000; j++ )
        {
            rand();
            if ( j == 999 )
            {
                DT2 = wxDateTime::UNow();
                wxTimeSpan ts = DT2.Subtract( DT1 );
                if ( ts.IsLongerThan( tsTest ) )
                {
                    // nothing to do
                }
            }
        }
        #else
        wxSleep(1);
        #endif

        wxString msg;

        if ( i == max )
        {
            msg = _T("That's all, folks!");
        }
        else if ( i > max / 2 )
        {
            msg = _T("Only a half left (very long message)!");
        }

#if wxUSE_STOPWATCH && wxUSE_LONGLONG
        if ( (i % (max/100)) == 0 ) // // only 100 updates, this makes it much faster
        {
            cont = dialog.Update(i, msg, &skip);
        }
#else
        cont = dialog.Update(i, msg, &skip);
#endif

        if ( !cont )
        {
            if ( wxMessageBox(_T("Do you really want to cancel?"),
                              _T("Progress dialog question"),  // caption
                              wxYES_NO | wxICON_QUESTION) == wxYES )
                break;

            cont = true;
            dialog.Resume();
        }
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

#if wxUSE_BUSYINFO

void MyFrame::ShowBusyInfo(wxCommandEvent& WXUNUSED(event))
{
    wxWindowDisabler disableAll;

    wxBusyInfo info(_T("Working, please wait..."), this);

    for ( int i = 0; i < 18; i++ )
    {
        //wxUsleep(100);
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
        delete m_dlgReplace;
        m_dlgReplace = NULL;
    }
    else
    {
        m_dlgReplace = new wxFindReplaceDialog
                           (
                            this,
                            &m_findData,
                            _T("Find and replace dialog"),
                            wxFR_REPLACEDIALOG
                           );

        m_dlgReplace->Show(true);
    }
}

void MyFrame::ShowFindDialog( wxCommandEvent& WXUNUSED(event) )
{
    if ( m_dlgFind )
    {
        delete m_dlgFind;
        m_dlgFind = NULL;
    }
    else
    {
        m_dlgFind = new wxFindReplaceDialog
                        (
                            this,
                            &m_findData,
                            _T("Find dialog"),
                            // just for testing
                            wxFR_NOWHOLEWORD
                        );

        m_dlgFind->Show(true);
    }
}

static wxString DecodeFindDialogEventFlags(int flags)
{
    wxString str;
    str << (flags & wxFR_DOWN ? _T("down") : _T("up")) << _T(", ")
        << (flags & wxFR_WHOLEWORD ? _T("whole words only, ") : _T(""))
        << (flags & wxFR_MATCHCASE ? _T("") : _T("not "))
        << _T("case sensitive");

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
                     type == wxEVT_COMMAND_FIND_REPLACE_ALL ? _T("all ") : wxT(""),
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
            txt = _T("Find");
            idMenu = DIALOGS_FIND;
            m_dlgFind = NULL;
        }
        else if ( dlg == m_dlgReplace )
        {
            txt = _T("Replace");
            idMenu = DIALOGS_REPLACE;
            m_dlgReplace = NULL;
        }
        else
        {
            txt = _T("Unknown");
            idMenu = wxID_ANY;

            wxFAIL_MSG( _T("unexpected event") );
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
    dc.SetFont(wxGetApp().m_canvasFont);
    dc.SetTextForeground(wxGetApp().m_canvasTextColour);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.DrawText(
                _T("wxWidgets common dialogs")
#if !defined(__SMARTPHONE__)
                _T(" test application")
#endif
                , 10, 10);
}

#if USE_MODAL_PRESENTATION

// ----------------------------------------------------------------------------
// MyModelessDialog
// ----------------------------------------------------------------------------

MyModelessDialog::MyModelessDialog(wxWindow *parent)
                : wxDialog(parent, wxID_ANY, wxString(_T("Modeless dialog")))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    wxButton *btn = new wxButton(this, DIALOGS_MODELESS_BTN, _T("Press me"));
    wxCheckBox *check = new wxCheckBox(this, wxID_ANY, _T("Should be disabled"));
    check->Disable();

    sizerTop->Add(btn, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(check, 1, wxEXPAND | wxALL, 5);

    SetSizer(sizerTop);

    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);
}

void MyModelessDialog::OnButton(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Button pressed in modeless dialog"), _T("Info"),
                 wxOK | wxICON_INFORMATION, this);
}

void MyModelessDialog::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto() )
    {
        wxMessageBox(_T("Use the menu item to close this dialog"),
                     _T("Modeless dialog"),
                     wxOK | wxICON_INFORMATION, this);

        event.Veto();
    }
}

// ----------------------------------------------------------------------------
// MyModalDialog
// ----------------------------------------------------------------------------

MyModalDialog::MyModalDialog(wxWindow *parent)
             : wxDialog(parent, wxID_ANY, wxString(_T("Modal dialog")))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    m_btnModal = new wxButton(this, wxID_ANY, _T("&Modal dialog..."));
    m_btnModeless = new wxButton(this, wxID_ANY, _T("Mode&less dialog"));
    m_btnDelete = new wxButton(this, wxID_ANY, _T("&Delete button"));

    wxButton *btnOk = new wxButton(this, wxID_CANCEL, _T("&Close"));
    sizerTop->Add(m_btnModal, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(m_btnModeless, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(m_btnDelete, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(btnOk, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizer(sizerTop);

    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);

    m_btnModal->SetFocus();
    m_btnModal->SetDefault();
}

void MyModalDialog::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == m_btnDelete )
    {
        delete m_btnModal;
        m_btnModal = NULL;

        m_btnDelete->Disable();
    }
    else if ( event.GetEventObject() == m_btnModal )
    {
#if wxUSE_TEXTDLG
        wxGetTextFromUser(_T("Dummy prompt"),
                          _T("Modal dialog called from dialog"),
                          wxEmptyString, this);
#else
        wxMessageBox(_T("Modal dialog called from dialog"));
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

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

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

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

#endif // USE_SETTINGS_DIALOG
