///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.cpp
// Purpose:     wxCheckListBox sample
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     13.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include  "wx/ownerdrw.h"
#include  "wx/menuitem.h"
#include  "wx/msw/checklst.h"

// Define a new application type
class CheckListBoxApp: public wxApp
{
public:
  bool OnInit();
};

// Define a new frame type
class CheckListBoxFrame : public wxFrame
{
public:
  // ctor & dtor
  CheckListBoxFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
  ~CheckListBoxFrame();

  // notifications
  void OnQuit             (wxCommandEvent& event);
  void OnAbout            (wxCommandEvent& event);
  void OnListboxSelect    (wxCommandEvent& event);
  void OnCheckboxToggle   (wxCommandEvent& event);
  void OnListboxDblClick  (wxCommandEvent& event);
  bool OnClose            ()                        { return TRUE; }

  DECLARE_EVENT_TABLE()

private:
  wxCheckListBox *m_pListBox;
};

enum 
{ 
  Menu_Quit = 1,
  Control_First = 1000,
  Control_Listbox, Control_Listbox2,
};

BEGIN_EVENT_TABLE(CheckListBoxFrame, wxFrame)
  EVT_MENU(Menu_Quit, CheckListBoxFrame::OnQuit)
  EVT_LISTBOX(Control_Listbox, CheckListBoxFrame::OnListboxSelect)
  EVT_CHECKLISTBOX(Control_Listbox, CheckListBoxFrame::OnCheckboxToggle)
  EVT_COMMAND(Control_Listbox, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,
              CheckListBoxFrame::OnListboxDblClick)
END_EVENT_TABLE()

IMPLEMENT_APP(CheckListBoxApp);

// init our app: create windows
bool CheckListBoxApp::OnInit(void)
{
  CheckListBoxFrame *pFrame = new CheckListBoxFrame(NULL, "wxWindows Ownerdraw Sample",
                                                50, 50, 450, 320);
  SetTopWindow(pFrame);

  return TRUE;
}

// main frame constructor
CheckListBoxFrame::CheckListBoxFrame(wxFrame *frame, char *title, int x, int y, int w, int h)
         : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
  // set the icon
  SetIcon(wxIcon("mondrian"));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  // construct submenu
  file_menu->Append(Menu_Quit, "E&xit");

  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  SetMenuBar(menu_bar);

  // make a panel with some controls
  wxPanel *pPanel = new wxPanel(this, -1, wxPoint(0, 0), 
                                wxSize(400, 200), wxTAB_TRAVERSAL);

  // check list box
  static const char* aszChoices[] = { "Hello", "world", "and", 
                                      "goodbye", "cruel", "world",
                                      "-------", "owner-drawn", "listbox" };

  wxString *astrChoices = new wxString[WXSIZEOF(aszChoices)];
  unsigned int ui;
  for ( ui = 0; ui < WXSIZEOF(aszChoices); ui++ )
    astrChoices[ui] = aszChoices[ui];

  m_pListBox = new wxCheckListBox
                   (                                             
                     pPanel,             // parent               
                     Control_Listbox,    // control id           
                     wxPoint(10, 10),    // listbox poistion     
                     wxSize(400, 200),   // listbox size
                     WXSIZEOF(aszChoices), // number of strings    
                     astrChoices         // array of strings
                   );                                            
                                                                            
  delete [] astrChoices;

  for ( ui = 0; ui < WXSIZEOF(aszChoices); ui += 2 ) {
    m_pListBox->GetItem(ui)->SetBackgroundColour(wxColor(200, 200, 200));
  }

  m_pListBox->Check(2);

  // create the status line
  const int widths[] = { -1, 60 };
  CreateStatusBar(2);
  SetStatusWidths(2, widths);
  SetStatusText("no selection", 0);

  Show(TRUE);
}

CheckListBoxFrame::~CheckListBoxFrame()
{
}

void CheckListBoxFrame::OnQuit(wxCommandEvent& event)
{
  Close(TRUE);
}

void CheckListBoxFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageDialog dialog(this, "Demo of wxCheckListBox control\n"
                         "About wxCheckListBox", wxYES_NO | wxCANCEL);
  dialog.ShowModal();
}

void CheckListBoxFrame::OnListboxSelect(wxCommandEvent& event)
{
  wxString strSelection;
  unsigned int nSel = event.GetSelection();
  strSelection.sprintf("item %d selected (%schecked)", nSel,
                       m_pListBox->IsChecked(nSel) ? "" : "not ");
  SetStatusText(strSelection);
}

void CheckListBoxFrame::OnListboxDblClick(wxCommandEvent& event)
{
  wxString strSelection;
  strSelection.sprintf("item %d double clicked", m_pListBox->GetSelection());
  wxMessageDialog dialog(this, strSelection);
  dialog.ShowModal();
}

void CheckListBoxFrame::OnCheckboxToggle(wxCommandEvent& event)
{
  wxString strSelection;
  unsigned int nItem = event.GetInt();
  if(event.GetInt()==-1)
	  return;
  strSelection.sprintf("item %d was %schecked", nItem,
                       m_pListBox->IsChecked(nItem) ? "" : "un");
  SetStatusText(strSelection);
}