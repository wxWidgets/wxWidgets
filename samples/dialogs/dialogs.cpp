/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.cpp
// Purpose:     Common dialogs demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
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

#define wxTEST_GENERIC_DIALOGS_IN_MSW 0

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
#include <wx/generic/colrdlgg.h>
#include <wx/generic/fontdlgg.h>
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
    EVT_MENU(DIALOGS_FILE_OPEN,                     MyFrame::FileOpen)
    EVT_MENU(DIALOGS_FILES_OPEN,                    MyFrame::FilesOpen)
    EVT_MENU(DIALOGS_FILE_SAVE,                     MyFrame::FileSave)
    EVT_MENU(DIALOGS_DIR_CHOOSE,                    MyFrame::DirChoose)
    EVT_MENU(DIALOGS_MODAL,                         MyFrame::ModalDlg)
    EVT_MENU(DIALOGS_MODELESS,                      MyFrame::ModelessDlg)
    EVT_MENU(DIALOGS_TIP,                           MyFrame::ShowTip)
#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
    EVT_MENU(DIALOGS_CHOOSE_COLOUR_GENERIC,         MyFrame::ChooseColourGeneric)
    EVT_MENU(DIALOGS_CHOOSE_FONT_GENERIC,           MyFrame::ChooseFontGeneric)
#endif
#if wxUSE_PROGRESSDLG
    EVT_MENU(DIALOGS_PROGRESS,                      MyFrame::ShowProgress)
#endif
    EVT_MENU(wxID_EXIT,                             MyFrame::OnExit)

    EVT_BUTTON(DIALOGS_MODELESS_BTN,                MyFrame::OnButton)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyModalDialog, wxDialog)
    EVT_BUTTON(-1, MyModalDialog::OnButton)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyModelessDialog, wxDialog)
    EVT_CLOSE(MyModelessDialog::OnClose)
END_EVENT_TABLE()

MyCanvas *myCanvas = (MyCanvas *) NULL;

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
#if defined(__WXGTK__) && defined(wxUSE_UNICODE)
  wxConvCurrent = &wxConvLibc;
#endif

  m_canvasTextColour = wxColour("BLACK");
  m_canvasFont = *wxNORMAL_FONT;

  // Create the main frame window
  MyFrame *frame = new MyFrame((wxFrame *) NULL, (char *) "wxWindows dialogs example", wxPoint(20, 20), wxSize(400, 300));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(DIALOGS_CHOOSE_COLOUR, "&Choose colour");

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
  file_menu->Append(DIALOGS_CHOOSE_COLOUR_GENERIC, "Choose colour (&generic)");
#endif

  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_CHOOSE_FONT, "Choose &font");

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
  file_menu->Append(DIALOGS_CHOOSE_FONT_GENERIC, "Choose f&ont (generic)");

#endif
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_LOG_DIALOG, "&Log dialog\tCtrl-L");
  file_menu->Append(DIALOGS_MESSAGE_BOX, "&Message box\tCtrl-M");
  file_menu->Append(DIALOGS_TEXT_ENTRY,  "Text &entry\tCtrl-E");
  file_menu->Append(DIALOGS_PASSWORD_ENTRY,  "&Password entry\tCtrl-P");
  file_menu->Append(DIALOGS_NUM_ENTRY, "&Numeric entry\tCtrl-N");
  file_menu->Append(DIALOGS_SINGLE_CHOICE,  "&Single choice\tCtrl-C");
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_TIP,  "&Tip of the day\tCtrl-T");
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_FILE_OPEN,  "&Open file\tCtrl-O");
  file_menu->Append(DIALOGS_FILES_OPEN,  "Open &files\tCtrl-Q");
  file_menu->Append(DIALOGS_FILE_SAVE,  "Sa&ve file\tCtrl-S");
  file_menu->Append(DIALOGS_DIR_CHOOSE,  "&Choose a directory\tCtrl-D");
#if wxUSE_PROGRESSDLG
  file_menu->Append(DIALOGS_PROGRESS, "Pro&gress dialog\tCtrl-G");
#endif // wxUSE_PROGRESSDLG
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_MODAL, "Mo&dal dialog\tCtrl-F");
  file_menu->Append(DIALOGS_MODELESS, "Modeless &dialog\tCtrl-Z", "", TRUE);
  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, "E&xit\tAlt-X");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
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
}

void MyFrame::ChooseColour(wxCommandEvent& WXUNUSED(event) )
{
      wxColourData data;
      data.SetChooseFull(TRUE);
      for (int i = 0; i < 16; i++)
      {
        wxColour colour(i*16, i*16, i*16);
        data.SetCustomColour(i, colour);
      }

      wxColourDialog *dialog = new wxColourDialog(this, &data);
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

void MyFrame::ChooseFont(wxCommandEvent& WXUNUSED(event) )
{
      wxFontData data;
      data.SetInitialFont(wxGetApp().m_canvasFont);
      data.SetColour(wxGetApp().m_canvasTextColour);

      wxFontDialog *dialog = new wxFontDialog(this, &data);
      if (dialog->ShowModal() == wxID_OK)
      {
        wxFontData retData = dialog->GetFontData();
        wxGetApp().m_canvasFont = retData.GetChosenFont();
        wxGetApp().m_canvasTextColour = retData.GetColour();
        myCanvas->Refresh();
      }
      dialog->Destroy();
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
        wxLogMessage("This is some message - everything is ok so far.");
        wxLogMessage("Another message...\n... this one is on multiple lines");
        wxLogWarning("And then something went wrong!");
    }

    wxLogError("Intermediary error handler decided to abort.");
    wxLogError("The top level caller detected an unrecoverable error.");

    wxLog::FlushActive();

    wxLogMessage("And this is the same dialog but with only one message.");
}

void MyFrame::MessageBox(wxCommandEvent& WXUNUSED(event) )
{
  wxMessageDialog dialog( NULL, "This is a message box\nA long, long string to test out the message box properly",
      "Message box text", wxYES_NO|wxCANCEL|wxICON_INFORMATION);

  dialog.ShowModal();
}

void MyFrame::NumericEntry(wxCommandEvent& WXUNUSED(event) )
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
        msg.Printf(_T("You've entered %lu"), res );
        icon = wxICON_INFORMATION;
    }

    wxMessageBox(msg, "Numeric test result", wxOK | icon, this);
}

void MyFrame::PasswordEntry(wxCommandEvent& WXUNUSED(event))
{
    wxString pwd = wxGetPasswordFromUser("Enter password:",
                                         "Passowrd entry dialog",
                                         "",
                                         this);
    if ( !!pwd )
    {
        wxMessageBox(wxString::Format("Your password is '%s'", pwd.c_str()),
                     "Got password", wxOK | wxICON_INFORMATION, this);
    }
}

void MyFrame::TextEntry(wxCommandEvent& WXUNUSED(event))
{
  wxTextEntryDialog dialog(this,
                           "This is a small sample\n"
                           "A long, long string to test out the text entrybox",
                           "Please enter a string",
                           "Default value",
                           wxOK | wxCANCEL);

  if (dialog.ShowModal() == wxID_OK)
  {
    wxMessageDialog dialog2(this, dialog.GetValue(), "Got string");
    dialog2.ShowModal();
  }
}

void MyFrame::SingleChoice(wxCommandEvent& WXUNUSED(event) )
{
    const wxString choices[] = { "One", "Two", "Three", "Four", "Five" } ;
    int n = 5;

    wxSingleChoiceDialog dialog(this, "This is a small sample\nA single-choice convenience dialog",
        "Please select a value", n, (const wxString *)choices);

    dialog.SetSelection(2);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetStringSelection(), "Got string");
        dialog2.ShowModal();
    }
}

void MyFrame::FileOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog(this, "Testing open file dialog", "", "", "*.txt", 0);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString info;
        info.Printf(_T("Full file name: %s\n")
                    _T("Path: %s\n")
                    _T("Name: %s"),
                    dialog.GetPath().c_str(),
                    dialog.GetDirectory().c_str(),
                    dialog.GetFilename().c_str());
        wxMessageDialog dialog2(this, info, "Selected file");
        dialog2.ShowModal();
    }
}

void MyFrame::FilesOpen(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog(this, "Testing open multiple file dialog",
                        "", "", wxFileSelectorDefaultWildcardStr,
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
                     n, paths[n].c_str(), filenames[n].c_str());

            msg += s;
        }

        wxMessageDialog dialog2(this, msg, "Selected files");
        dialog2.ShowModal();
    }
}

void MyFrame::FileSave(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog dialog(this, "Testing save file dialog", "", "myletter.txt",
        "Text files (*.txt)|*.txt|Document files (*.doc)|*.doc",
        wxSAVE|wxOVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxChar buf[400];
        wxSprintf(buf, _T("%s, filter %d"), (const wxChar*)dialog.GetPath(), dialog.GetFilterIndex());
        wxMessageDialog dialog2(this, wxString(buf), "Selected path");
        dialog2.ShowModal();
    }
}

void MyFrame::DirChoose(wxCommandEvent& WXUNUSED(event) )
{
    // pass some initial dir to wxDirDialog
    wxString dirHome;
    wxGetHomeDir(&dirHome);

    wxDirDialog dialog(this, "Testing directory picker", dirHome);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetPath(), "Selected path");
        dialog2.ShowModal();
    }
}

void MyFrame::ModalDlg(wxCommandEvent& WXUNUSED(event))
{
    MyModalDialog dlg(this);
    dlg.ShowModal();
}

void MyFrame::ModelessDlg(wxCommandEvent& event)
{
    bool show = GetMenuBar()->IsChecked(event.GetInt());

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

void MyFrame::OnButton(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Button pressed in modeless dialog", "Info",
                 wxOK | wxICON_INFORMATION, this);
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

    wxTipProvider *tipProvider = wxCreateFileTipProvider("tips.txt", s_index);

    bool showAtStartup = wxShowTip(this, tipProvider);

    if ( showAtStartup )
    {
        wxMessageBox("Will show tips on startup", "Tips dialog",
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

    wxProgressDialog dialog("Progress dialog example",
                            "An informative message",
                            max,    // range
                            this,   // parent
                            wxPD_CAN_ABORT |
                            wxPD_APP_MODAL |
                            wxPD_ELAPSED_TIME |
                            wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME);

    bool cont = TRUE;
    for ( int i = 0; i <= max && cont; i++ )
    {
        wxSleep(1);
        if ( i == max )
        {
            cont = dialog.Update(i, "That's all, folks!");
        }
        else if ( i == max / 2 )
        {
            cont = dialog.Update(i, "Only a half left (very long message)!");
        }
        else
        {
            cont = dialog.Update(i);
        }
    }

    if ( !cont )
    {
        wxLogStatus("Progress dialog aborted!");
    }
    else
    {
        wxLogStatus("Countdown from %d finished", max);
    }
}

#endif // wxUSE_PROGRESSDLG

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    dc.SetFont(wxGetApp().m_canvasFont);
    dc.SetTextForeground(wxGetApp().m_canvasTextColour);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.DrawText("wxWindows common dialogs test application", 10, 10);
}

// ----------------------------------------------------------------------------
// MyModelessDialog
// ----------------------------------------------------------------------------

MyModelessDialog::MyModelessDialog(wxWindow *parent)
                : wxDialog(parent, -1, wxString("Modeless dialog"))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    wxButton *btn = new wxButton(this, DIALOGS_MODELESS_BTN, "Press me");
    wxCheckBox *check = new wxCheckBox(this, -1, "Should be disabled");
    check->Disable();

    sizerTop->Add(btn, 1, wxEXPAND | wxALL, 5);
    sizerTop->Add(check, 1, wxEXPAND | wxALL, 5);

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);
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
             : wxDialog(parent, -1, wxString("Modal dialog"))
{
    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    m_btnFocused = new wxButton(this, -1, "Default button");
    m_btnDelete = new wxButton(this, -1, "&Delete button");
    sizerTop->Add(m_btnFocused, 0, wxALIGN_CENTER | wxALL, 5);
    sizerTop->Add(m_btnDelete, 0, wxALIGN_CENTER | wxALL, 5);

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);

    m_btnFocused->SetFocus();
    m_btnFocused->SetDefault();
}

void MyModalDialog::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == m_btnDelete )
    {
        delete m_btnFocused;
        m_btnFocused = NULL;

        m_btnDelete->Disable();
    }
    else
    {
        event.Skip();
    }
}
