/////////////////////////////////////////////////////////////////////////////
// Name:        treetest.cpp
// Purpose:     wxTreeCtrl sample
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

#include "wx/treectrl.h"

#include "treetest.h"

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(TREE_QUIT, MyFrame::OnQuit)
	EVT_MENU(TREE_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyTreeCtrl, wxTreeCtrl)
	EVT_TREE_BEGIN_DRAG(TREE_CTRL, MyTreeCtrl::OnBeginDrag)
	EVT_TREE_BEGIN_RDRAG(TREE_CTRL, MyTreeCtrl::OnBeginRDrag)
	EVT_TREE_BEGIN_LABEL_EDIT(TREE_CTRL, MyTreeCtrl::OnBeginLabelEdit)
	EVT_TREE_END_LABEL_EDIT(TREE_CTRL, MyTreeCtrl::OnEndLabelEdit)
	EVT_TREE_DELETE_ITEM(TREE_CTRL, MyTreeCtrl::OnDeleteItem)
	EVT_TREE_GET_INFO(TREE_CTRL, MyTreeCtrl::OnGetInfo)
	EVT_TREE_SET_INFO(TREE_CTRL, MyTreeCtrl::OnSetInfo)
	EVT_TREE_ITEM_EXPANDED(TREE_CTRL, MyTreeCtrl::OnItemExpanded)
	EVT_TREE_ITEM_EXPANDING(TREE_CTRL, MyTreeCtrl::OnItemExpanding)
	EVT_TREE_SEL_CHANGED(TREE_CTRL, MyTreeCtrl::OnSelChanged)
	EVT_TREE_SEL_CHANGING(TREE_CTRL, MyTreeCtrl::OnSelChanging)
	EVT_TREE_KEY_DOWN(TREE_CTRL, MyTreeCtrl::OnKeyDown)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "wxTreeCtrl Test", 50, 50, 450, 340);

  // This reduces flicker effects - even better would be to define OnEraseBackground
  // to do nothing. When the tree control's scrollbars are show or hidden, the
  // frame is sent a background erase event.
  frame->SetBackgroundColour(wxColour(255, 255, 255));

  // Give it an icon
#ifdef __WINDOWS__
  frame->SetIcon(wxIcon("mondrian"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("aiai.xbm"));
#endif

  // Make an image list containing large icons
  m_imageListNormal = new wxImageList(16, 16, TRUE);

  wxIcon *icon = new wxIcon("icon1", wxBITMAP_TYPE_ICO_RESOURCE);
  m_imageListNormal->Add(*icon);
  delete icon;
  icon = new wxIcon("icon2", wxBITMAP_TYPE_ICO_RESOURCE);
  m_imageListNormal->Add(*icon);
  delete icon;

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(TREE_ABOUT, "&About");
  file_menu->Append(TREE_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  frame->m_treeCtrl = new MyTreeCtrl(frame, TREE_CTRL, wxPoint(0, 0), wxSize(400, 200),
  	wxTR_HAS_BUTTONS|wxSUNKEN_BORDER);
  frame->m_logWindow = new wxTextCtrl(frame, -1, "", wxPoint(0, 0), wxSize(400, 200),
  	wxTE_MULTILINE|wxSUNKEN_BORDER);

  wxLayoutConstraints *c = new wxLayoutConstraints;
  c->top.SameAs			(frame, wxTop);
  c->left.SameAs		(frame, wxLeft);
  c->right.SameAs		(frame, wxRight);
  c->height.PercentOf	(frame, wxHeight, 66);
  frame->m_treeCtrl->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->top.Below			(frame->m_treeCtrl);
  c->left.SameAs		(frame, wxLeft);
  c->right.SameAs		(frame, wxRight);
  c->bottom.SameAs		(frame, wxBottom);
  frame->m_logWindow->SetConstraints(c);
  frame->SetAutoLayout(TRUE);

  frame->m_treeCtrl->SetImageList(wxGetApp().m_imageListNormal, wxIMAGE_LIST_NORMAL);

  long rootId = frame->m_treeCtrl->InsertItem(0, "Root", 0);

  char buf[20];
  int i;
  wxString str;

  for ( i = 0; i < 10; i++)
  {
	sprintf(buf, "Folder child %d", i);
    str = buf;
    long id = frame->m_treeCtrl->InsertItem(rootId, str, 0);
	int j;
    for ( j = 0; j < 5; j++)
    {
	  sprintf(buf, "File child %d", j);
      str = buf;
      frame->m_treeCtrl->InsertItem(id, str, 1);
    }
  }
  for ( i = 0; i < 10; i++)
  {
	sprintf(buf, "File child %d", i);
    str = buf;
    frame->m_treeCtrl->InsertItem(rootId, str, 1);
  }

  frame->CreateStatusBar(3);
  frame->SetStatusText("", 0);

  // Show the frame
  frame->Show(TRUE);
  
  SetTopWindow(frame);

  return TRUE;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
	m_treeCtrl = NULL;
	m_logWindow = NULL;
}

MyFrame::~MyFrame(void)
{
	delete wxGetApp().m_imageListNormal;
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageDialog dialog(this, "Tree test sample\nJulian Smart (c) 1997",
  	"About tree test", wxOK|wxCANCEL);

  dialog.ShowModal();
}

// MyTreeCtrl

void MyTreeCtrl::OnBeginDrag(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnBeginDrag\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnBeginRDrag(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnBeginRDrag\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnBeginLabelEdit(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnBeginLabelEdit\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnEndLabelEdit(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnEndLabelEdit\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnDeleteItem(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnDeleteItem\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnGetInfo(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnGetInfo\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnSetInfo(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnSetInfo\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnItemExpanded(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnItemExpanded\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnItemExpanding(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnItemExpanding\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnSelChanged(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnSelChanged\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnSelChanging(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnSelChanging\n";
	str.flush();
#endif
}

void MyTreeCtrl::OnKeyDown(wxTreeEvent& event)
{
	if ( !wxGetApp().GetTopWindow() )
		return;

	wxTextCtrl *text = ((MyFrame *)wxGetApp().GetTopWindow())->m_logWindow;
	if ( !text )
		return;

#ifndef __GNUWIN32__
	ostream str(text);

	str << "OnKeyDown\n";
	str.flush();
#endif
}

