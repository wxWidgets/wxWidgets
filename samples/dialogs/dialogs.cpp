/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.cpp
// Purpose:     Common dialogs demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/colordlg.h"
#include "wx/filedlg.h"
#include "wx/dirdlg.h"
#include "wx/fontdlg.h"
#include "wx/choicdlg.h"
#include "wx/tipdlg.h"
#include "wx/progdlg.h"
#include "wx/fdrepdlg.h"
#include "wx/busyinfo.h"

#define wxTEST_GENERIC_DIALOGS_IN_MSW 0

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
#include "wx/generic/colrdlgg.h"
#include "wx/generic/fontdlgg.h"
#endif

#define wxUSE_DIRDLGG 0

#if !defined(__WXMSW__) || defined(wxUSE_DIRDLGG) && wxUSE_DIRDLGG
#include "wx/generic/dirdlgg.h"
#endif

#include "dialogs.h"

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(DIALOGS_CHOOSE_COLOUR,                 MyFrame::ChooseColour)
    EVT_MENU(DIALOGS_CHOOSE_FONT,                   MyFrame::ChooseFont)
    EVT_MENU(DIALOGS_LOG_DIALOG,                    MyFrame::LogDialog)
    EVT_MENU(DIALOGS_MESSAGE_BOX,                   MyFrame::MessageBox)
    EVT_MENU(DIALOGS_TEXT_ENTRY,                    MyFrame::TextEntry)
    EVT_MENU(DIALOGS_PASSWORD_ENTRY,                MyFrame::PasswordEntry)
    EVT_MENU(DIALOGS_NUM_ENTRY,                     MyFrame::NumericEntry)
    EVT_MENU(DIALOGS_SINGLE_CHOICE,                 MyFrame::SingleChoice)
    EVT_MENU(DIALOGS_MULTI_CHOICE,                  MyFrame::MultiChoice)
    EVT_MENU(DIALOGS_FILE_OPEN,                     MyFrame::FileOpen)
    EVT_MENU(DIALOGS_FILE_OPEN2,                    MyFrame::FileOpen2)
    EVT_MENU(DIALOGS_FILES_OPEN,                    MyFrame::FilesOpen)
    EVT_MENU(DIALOGS_FILE_SAVE,                     MyFrame::FileSave)
    EVT_MENU(DIALOGS_DIR_CHOOSE,                    MyFrame::DirChoose)
    EVT_MENU(DIALOGS_DIRNEW_CHOOSE,                 MyFrame::DirChooseNew)
#if defined(__WXMSW__) || defined(__WXMAC__)
    EVT_MENU(DIALOGS_GENERIC_DIR_CHOOSE,            MyFrame::GenericDirChoose)
#endif // wxMSW || wxMAC
    EVT_MENU(DIALOGS_MODAL,                         MyFrame::ModalDlg)
    EVT_MENU(DIALOGS_MODELESS,                      MyFrame::ModelessDlg)
    EVT_MENU(DIALOGS_TIP,                           MyFrame::ShowTip)
#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
    EVT_MENU(DIALOGS_CHOOSE_COLOUR_GENERIC,         MyFrame::ChooseColourGeneric)
    EVT_MENU(DIALOGS_CHOOSE_FONT_GENERIC,           MyFrame::ChooseFontGeneric)
#endif

#if wxUSE_PROGRESSDLG
    EVT_MENU(DIALOGS_PROGRESS,                      MyFrame::ShowProgress)
#endif // wxUSE_PROGRESSDLG

#if wxUSE_BUSYINFO
    EVT_MENU(DIALOGS_BUSYINFO,                      MyFrame::ShowBusyInfo)
#endif // wxUSE_BUSYINFO

#if wxUSE_FINDREPLDLG
    EVT_MENU(DIALOGS_FIND,                          MyFrame::ShowFindDialog)
    EVT_MENU(DIALOGS_REPLACE,                       MyFrame::ShowReplaceDialog)

    EVT_FIND(-1, MyFrame::OnFindDialog)
    EVT_FIND_NEXT(-1, MyFrame::OnFindDialog)
    EVT_FIND_REPLACE(-1, MyFrame::OnFindDialog)
    EVT_FIND_REPLACE_ALL(-1, MyFrame::OnFindDialog)
    EVT_FIND_CLOSE(-1, MyFrame::OnFindDialog)
#endif // wxUSE_FINDREPLDLG
    EVT_MENU(wxID_EXIT,                             MyFrame::OnExit)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyModalDialog, wxDialog)
    EVT_BUTTON(-1, MyModalDialog::OnButton)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyModelessDialog, wxDialog)
    EVT_BUTTON(DIALOGS_MODELESS_BTN, MyModelessDialog::OnButton)

    EVT_CLOSE(MyModelessDialog::OnClose)
END_EVENT_TABLE()

MyCanvas *myCanvas = (MyCanvas *) NULL;

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
#if defined(__WXGTK__) && defined(wxUSE_UNICODE)
  wxConvCurrent = &wxConvLibc;
#endif

  m_canvasTextColour = wxColour(_T("BLACK"));
  m_canvasFont = *wxNORMAL_FONT;

  // Create the main frame window
  MyFrame *frame = new MyFrame((wxFrame *) NULL, _T("wxWindows dialogs example"), wxPoint(20, 20), wxSize(400, 300));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(DIALOGS_CHOOSE_COLOUR, _T("&Choose colour"));

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
  file_menu->Append(DIALOGS_CHOOSE_COLOUR_GENERIC, _T("Choose colour (&generic)"));
#endif

  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_CHOOSE_FONT, _T("Choose &font"));

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
  file_menu->Append(DIALOGS_CHOOSE_FONT_GENERIC, _T("Choose f&ont (generic)"));
#endif

  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_LOG_DIALOG, _T("&Log dialog\tCtrl-L"));
  file_menu->Append(DIALOGS_MESSAGE_BOX, _T("&Message box\tCtrl-M"));
  file_menu->Append(DIALOGS_TEXT_ENTRY,  _T("Text &entry\tCtrl-E"));
  file_menu->Append(DIALOGS_PASSWORD_ENTRY,  _T("&Password entry\tCtrl-P"));
  file_menu->Append(DIALOGS_NUM_ENTRY, _T("&Numeric entry\tCtrl-N"));
  file_menu->Append(DIALOGS_SINGLE_CHOICE,  _T("&Single choice\tCtrl-C"));
  file_menu->Append(DIALOGS_MULTI_CHOICE,  _T("M&ultiple choice\tCtrl-U"));
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_TIP,  _T("&Tip of the day\tCtrl-T"));
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_FILE_OPEN,  _T("&Open file\tCtrl-O"));
  file_menu->Append(DIALOGS_FILE_OPEN2,  _T("&Second open file\tCtrl-2"));
  file_menu->Append(DIALOGS_FILES_OPEN,  _T("Open &files\tCtrl-Q"));
  file_menu->Append(DIALOGS_FILE_SAVE,  _T("Sa&ve file\tCtrl-S"));
  file_menu->Append(DIALOGS_DIR_CHOOSE,  _T("&Choose a directory\tCtrl-D"));
  file_menu->Append(DIALOGS_DIRNEW_CHOOSE,  _T("Choose a directory (with \"Ne&w\" button)\tShift-Ctrl-D"));
#if defined(__WXMSW__) || defined(__WXMAC__)
  file_menu->Append(DIALOGS_GENERIC_DIR_CHOOSE,  _T("&Choose a directory (generic implementation)"));
#endif // wxMSW || wxMAC
  file_menu->AppendSeparator();

#if wxUSE_PROGRESSDLG
  file_menu->Append(DIALOGS_PROGRESS, _T("Pro&gress dialog\tCtrl-G"));
#endif // wxUSE_PROGRESSDLG
#if wxUSE_BUSYINFO
  file_menu->Append(DIALOGS_BUSYINFO, _T("&Busy info dialog\tCtrl-B"));
#endif // wxUSE_BUSYINFO
#if wxUSE_FINDREPLDLG
  file_menu->Append(DIALOGS_FIND, _T("&Find dialog\tCtrl-F"), _T(""), TRUE);
  file_menu->Append(DIALOGS_REPLACE, _T("Find and &replace dialog\tShift-Ctrl-F"), _T(""), TRUE);
#endif // wxUSE_FINDREPLDLG
  file_menu->AppendSeparator();

  file_menu->Append(DIALOGS_MODAL, _T("Mo&dal dialog\tCtrl-W"));
  file_menu->Append(DIALOGS_MODELESS, _T("Modeless &dialog\tCtrl-Z"), _T(""), TRUE);
  file_menu->AppendSeparator();

  file_menu->Append(wxID_EXIT, _T("E&xit\tAlt-X"));
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, _T("&File"));
  frame->SetMenuBar(menu_bar);

  myCanvas = new MyCanvas(frame);
  myCanvas->SetBackgroundColour(*wxWHITE);

  frame->Centre(wxBOTH);

  // Show the frame
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

// My frame constructor
MyFrame::MyFrame(wxWindow *parent,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size)
       : wxFrame(parent, -1, title, pos, size)
{
    m_dialog = (MyModelessDialog *)NULL;

#if wxUSE_FINDREPLDLG
    m_dlgFind =
    m_dlgReplace = NULL;
#endif
   
    CreateStatusBar();
}

void MyFrame::ChooseColour(wxCommandEvent& WXUNUSED(event) )
{
    wxColour col = myCanvas->GetBackgroundColour();

    wxColourData data;
    data.SetColour(col);
    data.SetChooseFull(TRUE);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour(i*16, i*16, i*16);
        data.SetCustomColour(i, colour);
    }

    wxColourDialog dialog(this, &data);
    dialog.SetTitle(_T("Choose the background colour"));
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        col = retData.GetColour();
        myCanvas->SetBackgroundColour(col);
        myCanvas->Clear();
        myCanvas->Refresh();
    }
}

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

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
void MyFrame::ChooseColourGeneric(wxCommandEvent& WXUNUSED(event))
{
      wxColourData data;
      data.SetChooseFull(TRUE);
      for (int i = 0; i < 16; i++)
      {
        wxColour colour(i*16, i*16, i*16);
        data.SetCustomColour(i, colour);
      }

      wxGenericColourDialog *dialog = new wxGenericColourDialog(this, &data);
      if (dialog->ShowModal() == wxID_OK)
      {
        wxColourData retData = dialog->GetColourData();
        wxColour col = retData.GetColour();
//        wxBrush *brush = wxTheBrushList->FindOrCreateBrush(&col, wxSOLID);
        myCanvas->SetBackgroundColour(col);
        myCanvas->Clear();
        myCanvas->Refresh();
      }
      dialog->Destroy();
}

void MyFrame::ChooseFontGeneric(wxCommandEvent& WXUNUSED(event) )
{
      wxFontData data;
      data.SetInitialFont(wxGetApp().m_canvasFont);
      data.SetColour(wxGetApp().m_canvasTextColour);

      wxGenericFontDialog *dialog = new wxGenericFontDialog(this, &data);
      if (dialog->ShowModal() == wxID_OK)
      {
        wxFontData retData = dialog->GetFontData();
        wxGetApp().m_canvasFont = retData.GetChosenFont();
        wxGetApp().m_canvasTextColour = retData.GetColour();
        myCanvas->Refresh();
      }
      dialog->Destroy();
}
#endif // wxTEST_GENERIC_DIALOGS_IN_MSW

void MyFrame::LogDialog(wxCommandEvent& event)
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

void MyFrame::PasswordEntry(wxCommandEvent& WXUNUSED(event))
{
    wxString pwd = wxGetPasswordFromUser(_T("Enter password:"),
                                         _T("Password entry dialog"),
                                         _T(""),
                                         this);
    if ( !!pwd )
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
    wxMessageDialog dialog2(this, dialog.GetValue(), _T("Got string"));
    dialog2.ShowModal();
  }
}

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

void MyFrame::FileOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog
                 (
                    this,
                    _T("Testing open file dialog"),
                    _T(""),
                    _T(""),
#ifdef __WXMOTIF__
                    _T("C++ files (*.cpp)|*.cpp")
#else
                    _T("C++ files (*.h;*.cpp)|*.h;*.cpp")
#endif
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

// this shows how to take advantage of specifying a default extension in the
// call to wxFileSelector: it is remembered after each new call and the next
// one will use it by default
void MyFrame::FileOpen2(wxCommandEvent& WXUNUSED(event) )
{
    static wxString s_extDef;
    wxString path = wxFileSelector(
                                    _T("Select the file to load"),
                                    _T(""), _T(""),
                                    s_extDef,
                                    _T("Waveform (*.wav)|*.wav|Plain text (*.txt)|*.txt|All files (*.*)|*.*"),
                                    wxCHANGE_DIR,
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
    wxFileDialog dialog(this, _T("Testing open multiple file dialog"),
                        _T(""), _T(""), wxFileSelectorDefaultWildcardStr,
                        wxMULTIPLE);

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

        wxMessageDialog dialog2(this, msg, _T("Selected files"));
        dialog2.ShowModal();
    }
}

void MyFrame::FileSave(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog(this,
                        _T("Testing save file dialog"),
                        _T(""),
                        _T("myletter.doc"),
                        _T("Text files (*.txt)|*.txt|Document files (*.doc)|*.doc"),
                        wxSAVE|wxOVERWRITE_PROMPT);

    dialog.SetFilterIndex(1);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxLogMessage(_T("%s, filter %d"),
                     dialog.GetPath().c_str(), dialog.GetFilterIndex());
    }
}

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
    DoDirChoose(wxDD_DEFAULT_STYLE & ~wxDD_NEW_DIR_BUTTON);
}

void MyFrame::DirChooseNew(wxCommandEvent& WXUNUSED(event) )
{
    DoDirChoose(wxDD_DEFAULT_STYLE | wxDD_NEW_DIR_BUTTON);
}

#if defined(__WXMSW__) || defined(__WXMAC__)

void MyFrame::GenericDirChoose(wxCommandEvent& WXUNUSED(event) )
{
#if !defined(__WXMSW__) || defined(wxUSE_DIRDLGG) && wxUSE_DIRDLGG
    // pass some initial dir to wxDirDialog
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxGenericDirDialog dialog(this, _T("Testing generic directory picker"), dirHome);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetPath(), _T("Selected path"));
        dialog2.ShowModal();
    }
#else
    wxLogError(wxT("Sorry, generic dir dialog not available:\n")
               wxT("set wxUSE_DIRDLGG to 1 and recompile"));
#endif
}

#endif // wxMSW || wxMAC

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

        m_dialog->Show(TRUE);
    }
    else // hide
    {
        m_dialog->Hide();
    }
}

void MyFrame::ShowTip(wxCommandEvent& event)
{
#if wxUSE_STARTUP_TIPS
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
#endif
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}

#if wxUSE_PROGRESSDLG

void MyFrame::ShowProgress( wxCommandEvent& WXUNUSED(event) )
{
    static const int max = 10;

    wxProgressDialog dialog(_T("Progress dialog example"),
                            _T("An informative message"),
                            max,    // range
                            this,   // parent
                            wxPD_CAN_ABORT |
                            wxPD_APP_MODAL |
                            // wxPD_AUTO_HIDE | -- try this as well
                            wxPD_ELAPSED_TIME |
                            wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME);

    bool cont = TRUE;
    for ( int i = 0; i <= max; i++ )
    {
        wxSleep(1);
        if ( i == max )
        {
            cont = dialog.Update(i, _T("That's all, folks!"));
        }
        else if ( i == max / 2 )
        {
            cont = dialog.Update(i, _T("Only a half left (very long message)!"));
        }
        else
        {
            cont = dialog.Update(i);
        }

        if ( !cont )
        {
            if ( wxMessageBox(_T("Do you really want to cancel?"),
                              _T("Progress dialog question"),  // caption
                              wxYES_NO | wxICON_QUESTION) == wxYES )
                break;

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

        m_dlgReplace->Show(TRUE);
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

        m_dlgFind->Show(TRUE);
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
            idMenu = -1;

            wxFAIL_MSG( _T("unexpected event") );
        }

        wxLogMessage(wxT("%s dialog is being closed."), txt);

        if ( idMenu != -1 )
        {
            GetMenuBar()->Check(idMenu, FALSE);
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
    dc.DrawText(_T("wxWindows common dialogs test application"), 10, 10);
}

// ----------------------------------------------------------------------------
// MyModelessDialog
// ----------------------------------------------------------------------------

MyModelessDialog::MyModelessDialog(wxWindow *parent)
                : wxDialog(parent, -1, wxString(_T("Modeless dialog")))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    wxButton *btn = new wxButton(this, DIALOGS_MODELESS_BTN, _T("Press me"));
    wxCheckBox *check = new wxCheckBox(this, -1, _T("Should be disabled"));
    check->Disable();

    sizerTop->Add(btn, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(check, 1, wxEXPAND | wxALL, 5);

    SetAutoLayout(TRUE);
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
             : wxDialog(parent, -1, wxString(_T("Modal dialog")))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    m_btnModal = new wxButton(this, -1, _T("&Modal dialog..."));
    m_btnModeless = new wxButton(this, -1, _T("Mode&less dialog"));
    m_btnDelete = new wxButton(this, -1, _T("&Delete button"));

    wxButton *btnOk = new wxButton(this, wxID_CANCEL, _T("&Close"));
    sizerTop->Add(m_btnModal, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(m_btnModeless, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(m_btnDelete, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(btnOk, 0, wxALIGN_CENTER | wxALL, 5);

    SetAutoLayout(TRUE);
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
        wxGetTextFromUser(_T("Dummy prompt"),
                          _T("Modal dialog called from dialog"),
                          _T(""), this);
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
