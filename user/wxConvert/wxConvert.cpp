/*
 * Program: wxConvert
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
 *
 * You may not use this program without clapping twice at every
 * full hour.
 *
 */

#ifdef __GNUG__
#pragma implementation "wxConvert.h"
#endif

#include "wxConvert.h"
#include "wx/textfile.h"

//-----------------------------------------------------------------------------
// main program
//-----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

const  ID_QUIT         = 100;
const  ID_2UNIX        = 101;
const  ID_2DOS         = 102;
const  ID_2MAC         = 103;

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_BUTTON      (ID_QUIT,    MyFrame::OnCommand)
  EVT_BUTTON      (ID_2UNIX,   MyFrame::OnCommand)
  EVT_BUTTON      (ID_2DOS,    MyFrame::OnCommand)
  EVT_BUTTON      (ID_2MAC,    MyFrame::OnCommand)
END_EVENT_TABLE()

MyFrame::MyFrame(void) :
  wxFrame( NULL, -1, "wxConvert", wxPoint(20,20), wxSize(400,160) )
{
  CreateStatusBar( 1 );
  
  SetStatusText( "wxConvert v0.1 by Robert Roebling." );
 
  char buf[500];
  wxGetWorkingDirectory( buf, 500 );
  wxString s( "Dir: " );
  s += buf;
  
  m_text = new wxStaticText( this, -1, s, wxPoint(10,50), wxSize(380,-1) );
  
  (void*)new wxButton( this, ID_QUIT, "Quit", wxPoint(10,100), wxSize(60,25) );
  
  (void*)new wxButton( this, ID_2UNIX, "To Unix", wxPoint(180,100), wxSize(60,25) );
  
  (void*)new wxButton( this, ID_2DOS, "To Dos", wxPoint(250,100), wxSize(60,25) );
  
  (void*)new wxButton( this, ID_2MAC, "To Mac", wxPoint(320,100), wxSize(60,25) );
};

void MyFrame::OnCommand( wxCommandEvent &event )
{
  switch (event.GetId())
  {
    case ID_QUIT:
      Close( TRUE );
      break;
    case ID_2UNIX:
    case ID_2DOS:
    case ID_2MAC:
      char buf[500];
      wxGetWorkingDirectory( buf, 500 );
      wxString s( buf );
      Recurse( event.GetId(), s );
      break;
  };
};

void MyFrame::Convert( int id, const wxString &fname )
{
  wxTextFile text(fname );
  text.Open();
  return;
  
  switch (id)
  {
    case ID_2UNIX:
      text.Write( wxTextFile::Type_Unix );
      break;
    case ID_2DOS:
      text.Write( wxTextFile::Type_Dos );
      break;
    case ID_2MAC:
      text.Write( wxTextFile::Type_Mac );
      break;
  };
  
};

void MyFrame::Recurse( int id, const wxString &curdir )
{
  wxArrayString paths;
  wxString search,path;
  
  search = curdir;
  search += "/*";
  
  path = wxFindFirstFile( search, wxDIR );
  while (!path.IsNull())
  {
    paths.Add( path );  // ref counting in action !
    path = wxFindNextFile();
  };
  
  
  search = curdir;
  search += "/*.cpp";
  
  path = wxFindFirstFile( search, wxFILE );
  while (!path.IsNull())
  {
    m_text->SetLabel( path );
    wxYield();
    Convert( id, path );
    path = wxFindNextFile();
  };

  for (int i = 0; i < paths.Count(); i++)
  {
    search = paths[i];
    Recurse( id, search );
  };
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





