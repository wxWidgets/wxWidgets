/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.cpp
// Purpose:     Common dialogs demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
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

#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/fontdlg.h>
#include <wx/choicdlg.h>

#if !defined(__WINDOWS__) || USE_GENERIC_DIALOGS_IN_MSW
#include <wx/generic/colrdlgg.h>
#include <wx/generic/fontdlgg.h>
#endif

#include "dialogs.h"

IMPLEMENT_APP(MyApp)

MyCanvas *myCanvas = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  m_canvasTextColour = wxColour("BLACK");
  m_canvasFont = *wxNORMAL_FONT;

  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "wxWindows dialogs example", wxPoint(50, 50), wxSize(400, 300));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(DIALOGS_CHOOSE_COLOUR, "&Choose colour");

#if !defined(__WINDOWS__) || USE_GENERIC_DIALOGS_IN_MSW
  file_menu->Append(DIALOGS_CHOOSE_COLOUR_GENERIC, "Choose colour (&generic)");
#endif

  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_CHOOSE_FONT, "Choose &font");

#if !defined(__WINDOWS__) || USE_GENERIC_DIALOGS_IN_MSW
  file_menu->Append(DIALOGS_CHOOSE_FONT_GENERIC, "Choose f&ont (generic)");

#endif
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_MESSAGE_BOX, "&Message box");
  file_menu->Append(DIALOGS_TEXT_ENTRY,  "Text &entry");
  file_menu->Append(DIALOGS_SINGLE_CHOICE,  "&Single choice");
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_FILE_OPEN,  "&Open file");
  file_menu->Append(DIALOGS_FILE_SAVE,  "Sa&ve file");
  file_menu->Append(DIALOGS_DIR_CHOOSE,  "&Choose a directory");
  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, "E&xit");
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
MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(parent, -1, title, pos, size)
{}

void MyFrame::ChooseColour(wxCommandEvent& event)
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
      dialog->Close();
}

void MyFrame::ChooseFont(wxCommandEvent& event)
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
      dialog->Close();
}

#if !defined(__WINDOWS__) || USE_GENERIC_DIALOGS_IN_MSW
void MyFrame::ChooseColourGeneric(wxCommandEvent& event)
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
      dialog->Close();
}

void MyFrame::ChooseFontGeneric(wxCommandEvent& event)
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
      dialog->Close();
}
#endif

void MyFrame::MessageBox(wxCommandEvent& event)
{
  wxMessageDialog dialog(this, "This is a message box\nA long, long string to test out the message box properly",
  	"Message box text", wxYES_NO|wxCANCEL);

  dialog.ShowModal();
}

void MyFrame::TextEntry(wxCommandEvent& event)
{
  wxTextEntryDialog dialog(this, "This is a small sample\nA long, long string to test out the text entrybox",
  	"Please enter a string", "Default value", wxOK|wxCANCEL);

  if (dialog.ShowModal() == wxID_OK)
  {
	wxMessageDialog dialog2(this, dialog.GetValue(), "Got string");
	dialog2.ShowModal();
  }
}

void MyFrame::SingleChoice(wxCommandEvent& event)
{
	const wxString choices[] = { "One", "Two", "Three", "Four", "Five" } ;
	int n = 5;

  wxSingleChoiceDialog dialog(this, "This is a small sample\nA single-choice convenience dialog",
  	"Please select a value", n, (const wxString *)choices);

  if (dialog.ShowModal() == wxID_OK)
  {
	wxMessageDialog dialog2(this, dialog.GetStringSelection(), "Got string");
	dialog2.ShowModal();
  }
}

void MyFrame::FileOpen(wxCommandEvent& event)
{
	wxFileDialog dialog(this, "Testing open file dialog", "", "", "*.txt", 0);

	if (dialog.ShowModal() == wxID_OK)
	{
		wxMessageDialog dialog2(this, dialog.GetPath(), "Selected path");
		dialog2.ShowModal();
	}
}

void MyFrame::FileSave(wxCommandEvent& event)
{
	wxFileDialog dialog(this, "Testing save file dialog", "", "",
		"Text files (*.txt)|*.txt|Document files (*.doc)|*.doc",
		wxSAVE|wxOVERWRITE_PROMPT);

	if (dialog.ShowModal() == wxID_OK)
	{
		char buf[400];
		sprintf(buf, "%s, filter %d", (const char *)dialog.GetPath(), dialog.GetFilterIndex());
		wxMessageDialog dialog2(this, wxString(buf), "Selected path");
		dialog2.ShowModal();
	}
}

void MyFrame::DirChoose(wxCommandEvent& event)
{
	wxDirDialog dialog(this, "Testing directory picker", "");

	if (dialog.ShowModal() == wxID_OK)
	{
		wxMessageDialog dialog2(this, dialog.GetPath(), "Selected path");
		dialog2.ShowModal();
	}
}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(TRUE);
}

void MyCanvas::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
    dc.SetFont(wxGetApp().m_canvasFont);
    dc.SetTextForeground(wxGetApp().m_canvasTextColour);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.DrawText("wxWindows common dialogs test application", 10, 10);
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
	EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(DIALOGS_CHOOSE_COLOUR,				MyFrame::ChooseColour)
	EVT_MENU(DIALOGS_CHOOSE_FONT,				MyFrame::ChooseFont)
	EVT_MENU(DIALOGS_MESSAGE_BOX,				MyFrame::MessageBox)
	EVT_MENU(DIALOGS_TEXT_ENTRY,				MyFrame::TextEntry)
	EVT_MENU(DIALOGS_SINGLE_CHOICE,				MyFrame::SingleChoice)
	EVT_MENU(DIALOGS_FILE_OPEN,					MyFrame::FileOpen)
	EVT_MENU(DIALOGS_FILE_SAVE,					MyFrame::FileSave)
	EVT_MENU(DIALOGS_DIR_CHOOSE,				MyFrame::DirChoose)
#if !defined(__WINDOWS__) || USE_GENERIC_DIALOGS_IN_MSW
	EVT_MENU(DIALOGS_CHOOSE_COLOUR_GENERIC,		MyFrame::ChooseColourGeneric)
	EVT_MENU(DIALOGS_CHOOSE_FONT_GENERIC,		MyFrame::ChooseFontGeneric)
#endif
	EVT_MENU(wxID_EXIT,							MyFrame::OnExit)
END_EVENT_TABLE()

