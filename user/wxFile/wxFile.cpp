/*
 * Program: wxFile
 * 
 * Author: Robert Roebling
 *
 * Copyright: (C) 1997, GNU (Robert Roebling)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef __GNUG__
#pragma implementation "wxFile.h"
#endif

#include "wxFile.h"
#include "wx/dnd.h"

#include "delete.xpm"
#include "home.xpm"
#include "prev.xpm"
#include "fileopen.xpm"
#include "exit.xpm"
#include "listview.xpm"
#include "iconview.xpm"
#include "reportview.xpm"
#include "treeview.xpm"
#include "commanderview.xpm"
#include "singleview.xpm"
#include "save.xpm"
#include "search.xpm"
#include "help.xpm"

//-----------------------------------------------------------------------------
// main program
//-----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

const  ID_FILECTRL   = 1000;
const  ID_DIRCTRL    = 1001;
const  ID_TOOLBAR    = 1002;

const  ID_QUIT       = 100;
const  ID_ABOUT      = 101;

const  ID_LIST       = 200;
const  ID_REPORT     = 201;
const  ID_ICON       = 202;

const  ID_SINGLE     = 203;
const  ID_TREE       = 204;
const  ID_COMMANDER  = 205;

const  ID_HOME       = 400;
const  ID_PARENT     = 401;
const  ID_MOUNT      = 402;
const  ID_SEARCH     = 403;

const  ID_DELETE     = 501;
const  ID_MD         = 502;


IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_SIZE    (MyFrame::OnSize)
  EVT_MENU    (ID_ABOUT,       MyFrame::OnAbout)
  EVT_TOOL    (ID_ABOUT,       MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,        MyFrame::OnCommand)
  EVT_TOOL    (ID_QUIT,        MyFrame::OnCommand)
  EVT_MENU    (ID_HOME,        MyFrame::OnCommand)
  EVT_TOOL    (ID_HOME,        MyFrame::OnCommand)
  EVT_MENU    (ID_PARENT,      MyFrame::OnCommand)
  EVT_TOOL    (ID_PARENT,      MyFrame::OnCommand)
  EVT_MENU    (ID_LIST,        MyFrame::OnView)
  EVT_MENU    (ID_REPORT,      MyFrame::OnView)
  EVT_MENU    (ID_ICON,        MyFrame::OnView)
  EVT_TOOL    (ID_LIST,        MyFrame::OnView)
  EVT_TOOL    (ID_REPORT,      MyFrame::OnView)
  EVT_TOOL    (ID_ICON,        MyFrame::OnView)
  EVT_TOOL    (ID_TREE,        MyFrame::OnView)
  EVT_TOOL    (ID_SINGLE,      MyFrame::OnView)
  EVT_TOOL    (ID_COMMANDER,   MyFrame::OnView)
  EVT_LIST_KEY_DOWN        (ID_FILECTRL,  MyFrame::OnListKeyDown)
  EVT_LIST_DELETE_ITEM     (ID_FILECTRL,  MyFrame::OnListDeleteItem)
  EVT_LIST_END_LABEL_EDIT  (ID_FILECTRL,  MyFrame::OnListEndLabelEdit)
  EVT_LIST_BEGIN_DRAG      (ID_FILECTRL,  MyFrame::OnListDrag)
  EVT_TREE_KEY_DOWN        (ID_DIRCTRL,   MyFrame::OnTreeKeyDown)
  EVT_TREE_SEL_CHANGED     (ID_DIRCTRL,   MyFrame::OnTreeSelected)
END_EVENT_TABLE()

MyFrame::MyFrame(void) :
  wxFrame( NULL, -1, "wxFile", wxPoint(20,20), wxSize(470,360) )
{
  wxMenu *file_menu = new wxMenu( "Menu 1" );
  file_menu->Append( ID_ABOUT, "About..");
  file_menu->Append( ID_QUIT, "Exit");
  
  wxMenu *view_menu = new wxMenu( "Menu 2" );
  view_menu->Append( ID_LIST, "List mode");
  view_menu->Append( ID_REPORT, "Report mode");
  view_menu->Append( ID_ICON, "Icon mode");
  
  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, "File" );
  menu_bar->Append(view_menu, "View" );
  menu_bar->Show( TRUE );
  
  SetMenuBar( menu_bar );
  
  CreateStatusBar( 2 );
  
  SetStatusText( "Welcome", 0 );
  SetStatusText( "wxFile v0.2 by Robert Roebling.", 1 );
  
  m_tb = new wxToolBar( this, ID_TOOLBAR, wxPoint(2,60), wxSize(300-4,26) );
  m_tb->SetMargins( 2, 2 );
  
  wxBitmap *bm;
  m_tb->AddSeparator();
  
  bm = new wxBitmap( exit_xpm );
  m_tb->AddTool( ID_QUIT, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Exit wxFile" );
  m_tb->AddSeparator();
  
  bm = new wxBitmap( prev_xpm );
  m_tb->AddTool( ID_PARENT, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Go to parent directory" );
  bm = new wxBitmap( home_xpm );
  m_tb->AddTool( ID_HOME, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Go to home directory" );
  m_tb->AddSeparator();
  
  bm = new wxBitmap( delete_xpm );
  m_tb->AddTool( ID_DELETE, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Delete file" );
  m_tb->AddSeparator();
  
  bm = new wxBitmap( fileopen_xpm );
  m_tb->AddTool( ID_MD, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Create directory" );
  m_tb->AddSeparator();
  
  bm = new wxBitmap( listview_xpm );
  m_tb->AddTool( ID_LIST, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "List view" );
  bm = new wxBitmap( reportview_xpm );
  m_tb->AddTool( ID_REPORT, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Report view" );
  bm = new wxBitmap( iconview_xpm );
  m_tb->AddTool( ID_ICON, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Icon view" );
  m_tb->AddSeparator();
  
  bm = new wxBitmap( treeview_xpm );
  m_tb->AddTool( ID_TREE, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Tree view" );
  bm = new wxBitmap( commanderview_xpm );
  m_tb->AddTool( ID_COMMANDER, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Commander view" );
  bm = new wxBitmap( singleview_xpm );
  m_tb->AddTool( ID_SINGLE, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Single view" );
  m_tb->AddSeparator();
  
  bm = new wxBitmap( search_xpm );
  m_tb->AddTool( ID_MOUNT, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Mount devices" );
  m_tb->AddSeparator();

  bm = new wxBitmap( save_xpm );
  m_tb->AddTool( ID_SEARCH, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "Find file(s)" );
  m_tb->AddSeparator();

  bm = new wxBitmap( help_xpm );
  m_tb->AddTool( ID_ABOUT, *bm, wxNullBitmap, FALSE, -1, -1, NULL, "About wxFile" );
  
  m_tb->Layout();
  
  m_splitter = new wxSplitterWindow( this, -1, wxPoint(0,0), wxSize(400,300), wxSP_3D );
  
  m_leftFile = NULL;
  m_dir = new wxDirCtrl( m_splitter, ID_DIRCTRL, "/", wxPoint(10,45), wxSize(200,330) );
  
  wxString homepath( "/home" );
  char buf[300];
  wxGetHomeDir( buf );
  homepath = buf;
  m_rightFile = new wxFileCtrl( m_splitter, ID_FILECTRL, homepath, wxPoint(220,5), wxSize(200,330) );
   
  m_leftFile = new wxFileCtrl( m_splitter, ID_FILECTRL, homepath, wxPoint(0,5), wxSize(200,330) );
  m_leftFile->Show( FALSE );
  
  m_leftFile->m_lastFocus = m_rightFile;
    
  int x = 0;
  GetClientSize( &x, NULL );
  
  m_splitter->SplitVertically( m_dir, m_rightFile, x / 3 );
  m_splitter->SetMinimumPaneSize( 10 );
};

void MyFrame::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  int x = 0;
  int y = 0;
  GetClientSize( &x, &y );
  
  m_tb->SetSize( 1, 0, x-2, 30 );
  m_splitter->SetSize( 0, 31, x, y-31 );
};

void MyFrame::OnView( wxCommandEvent &event )
{
  int x = 0;
  GetClientSize( &x, NULL );
  switch (event.GetId())
  {
    case ID_LIST:
      m_rightFile->ChangeToListMode();
      if (m_splitter->IsSplit() && (m_splitter->GetWindow1() == m_leftFile))
        m_leftFile->ChangeToListMode();
      break;
    case ID_REPORT:
      m_rightFile->ChangeToReportMode();
      if (m_splitter->IsSplit() && (m_splitter->GetWindow1() == m_leftFile))
        m_leftFile->ChangeToReportMode();
      break;
    case ID_ICON:
      m_rightFile->ChangeToIconMode();
      if (m_splitter->IsSplit() && (m_splitter->GetWindow1() == m_leftFile))
        m_leftFile->ChangeToIconMode();
      break;
    case ID_TREE:
      if (m_splitter->IsSplit())
      {
        if (m_splitter->GetWindow1() != m_dir)
	{
	  m_splitter->Unsplit( m_leftFile );
          m_dir->Show(TRUE);
          m_splitter->SplitVertically( m_dir, m_rightFile, x/3 );
	};
      }
      else
      {
        m_dir->Show(TRUE);
        m_splitter->SplitVertically( m_dir, m_rightFile, x/3 );
      };
      break;
    case ID_SINGLE:
      if (m_splitter->IsSplit()) m_splitter->Unsplit( m_splitter->GetWindow1() );
      break;
    case ID_COMMANDER:
      if (m_splitter->IsSplit())
      {
        if (m_splitter->GetWindow1() != m_leftFile)
	{
	  m_splitter->Unsplit( m_dir );
	  m_leftFile->ChangeToListMode();
	  m_rightFile->ChangeToListMode();
          m_leftFile->Show(TRUE);
          m_splitter->SplitVertically( m_leftFile, m_rightFile, x/2 );
	};
      }
      else
      {
	m_leftFile->ChangeToListMode();
	m_rightFile->ChangeToListMode();
        m_leftFile->Show(TRUE);
        m_splitter->SplitVertically( m_leftFile, m_rightFile, x/2 );
      };
      break;
    default:
      break;
  };
};

void MyFrame::OnCommand( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_QUIT:
      Close( TRUE );
      break;
    case ID_HOME:
      m_leftFile->m_lastFocus->GoToHomeDir();
      break;
    case ID_PARENT:
      m_leftFile->m_lastFocus->GoToParentDir();
      break;
    default:
      break;
  };
};

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  wxDialog dialog( this, -1, "About wxFile", wxPoint(100,100), wxSize(540,350), wxDIALOG_MODAL );
  
  int w = 0;
  int h = 0;
  dialog.GetSize( &w, &h );

  int x = 30;
  int y = 20;
  int step = 20;

  (void)new wxStaticBox( &dialog, -1, (const char*)NULL, wxPoint(10,10), wxSize(w-20,h-80) );
  
  (void)new wxStaticText( &dialog, -1, "wxFile v0.1", wxPoint(240,y) );
  y += 2*step-10;
  
  (void)new wxStaticText( &dialog, -1, "Written by Robert Roebling, 1998.", wxPoint(x,y) );
  y += 2*step;
  
  (void)new wxStaticText( &dialog, -1, 
    "wxFile uses wxGTK, the GTK port of the wxWindows GUI-library.", wxPoint(x,y) );
  y += step;
  (void)new wxStaticText( &dialog, -1, "http://www.freiburg.linux.de/~wxxt", wxPoint(x+50,y) );
  y += step;
  (void)new wxStaticText( &dialog, -1, "http://web.ukonline.co.uk/julian.smart/wxwin", wxPoint(x+50,y) );
  y += 2*step;

  (void)new wxStaticText( &dialog, -1, "wxFile Copyright: GPL.", wxPoint(x,y) );
  y += 2*step;
  (void)new wxStaticText( &dialog, -1, "For questions concerning wxGTK, you may mail to:", wxPoint(x,y) );
  y += step;
  (void)new wxStaticText( &dialog, -1, "roebling@ruf.uni-freiburg.de", wxPoint(x+50,y) );
  
  (void) new wxButton( &dialog, wxID_OK, "Return", wxPoint(w/2-40,h-50), wxSize(80,30) );
  
  dialog.ShowModal();
};

void MyFrame::OnListKeyDown( wxListEvent &event )
{
  m_rightFile->m_lastFocus->OnListKeyDown( event );
};

void MyFrame::OnListDeleteItem( wxListEvent &event )
{
  m_rightFile->m_lastFocus->OnListDeleteItem( event );
};

void MyFrame::OnListEndLabelEdit( wxListEvent &event )
{
  m_rightFile->m_lastFocus->OnListEndLabelEdit( event );
};

void MyFrame::OnListDrag( wxListEvent &event )
{
  printf( "OnDrag.\n" );
  return;
};
    
void MyFrame::OnTreeSelected( wxTreeEvent &event )
{
  wxDirInfo *info = (wxDirInfo*) event.m_item.m_data;
  if (info) SetStatusText( info->GetPath() );
};

void MyFrame::OnTreeKeyDown( wxTreeEvent &event )
{
  wxDirInfo *info = (wxDirInfo*) event.m_item.m_data;
  if (info) m_rightFile->GoToDir( info->GetPath() );
};

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

MyApp::MyApp(void) : 
  wxApp( )
{
};

bool MyApp::OnInit(void)
{
  wxFrame *frame = new MyFrame();
  frame->Show( TRUE );
  
  return TRUE;
};





