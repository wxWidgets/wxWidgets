/*
 * Program: FMJobs.cpp
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
#pragma implementation "FMJobs.h"
#endif

#include "FMJobs.h"
#include "wx/utils.h"
#include "wx/filefn.h"
#include "wx/msgdlg.h"

//-----------------------------------------------------------------------------
// wxCopyStatusDia
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCopyStatusDia,wxDialog);

const  ID_CANCEL_COPY = 1000;

BEGIN_EVENT_TABLE(wxCopyStatusDia,wxDialog)
  EVT_BUTTON  (ID_CANCEL_COPY,   wxCopyStatusDia::OnCommand)
END_EVENT_TABLE()

wxCopyStatusDia::wxCopyStatusDia( wxFrame *parent, const wxString &dest, wxArrayString *files ) :
  wxDialog( parent, -1, "FileMaker copy job control", wxPoint(180,180), wxSize(500,200) )
{
  int w = 0;
  int h = 0;
  GetSize( &w, &h );

  m_dest = dest;
  m_files = files;
  m_stop = FALSE;

  (void)new wxStaticText( this, -1, "Copying files", wxPoint(10,10) );
  (void)new wxStaticText( this, -1, "from:", wxPoint(30,40) );
  m_sourceMsg = new wxStaticText( this, -1, "", wxPoint(80,40), wxSize(200,-1) );
  (void)new wxStaticText( this, -1, "    to:", wxPoint(30,70) );
  m_destMsg = new wxStaticText( this, -1, "", wxPoint(80,70), wxSize(200,-1) );
  (void)new wxStaticText( this, -1, "    Kb copied:", wxPoint(30,100) );
  m_statusMsg = new wxStaticText( this, -1, "0", wxPoint(120,100), wxSize(100,-1) );

  m_cancelButton = new wxButton( this, ID_CANCEL_COPY, "Return", wxPoint(w-130,h-50), wxSize(85,30) );
  
  Centre( wxVERTICAL | wxHORIZONTAL );

  m_timer = new wxCopyTimer( this );
  m_timer->Start( 300, TRUE );
  
  Show( TRUE );
};

wxCopyStatusDia::~wxCopyStatusDia()
{
  delete m_timer;
};

void wxCopyStatusDia::OnCommand( wxCommandEvent &WXUNUSED(event) )
{
  if (m_stop) EndModal(wxID_CANCEL);
  m_stop = TRUE;
};

void wxCopyStatusDia::DoCopy(void)
{
  wxYield();
  
  if (!wxDirExists(m_dest))
  {
    wxMessageBox( "Target is not a directory or it doesn`t exist. Can`t copy.", "FileMaker" );
    return;
  };
  
  for (uint i = 0; i < m_files->Count(); i++)
  {
    wxString src = (*m_files)[i];
    if (wxDirExists( src ))
      CopyDir( src, m_dest );
    else
      CopyFile( src, m_dest );
    if (m_stop) return;
  };
  m_stop = TRUE;
};

void wxCopyStatusDia::CopyDir( wxString &srcDir, wxString &destDir )
{
  wxString src = srcDir;
  wxString dest = destDir;
  dest += "/";
  dest += wxFileNameFromPath( src );
  if (!wxMkdir( dest ))
  {
    wxMessageBox( "Could not create target directory.", "FileMaker" );
    return;
  };
  
  wxArrayString list;
  src += "/*";
  char *f = wxFindFirstFile( src, wxDIR );
  while (f)
  {
    list.Add( f );
    f = wxFindNextFile();
  };
  
  for (uint i = 0; i < list.Count(); i++)
  {
    wxString filename = list[i];
    if (wxDirExists( filename ))
      CopyDir( filename, dest );
    else
      CopyFile( filename, dest );
    if (m_stop) return;
  };
};

void wxCopyStatusDia::CopyFile( wxString &src, wxString &destDir )
{
  m_sourceMsg->SetLabel( src );
  wxString dest = destDir;
  dest += "/";
  dest += wxFileNameFromPath( src );
  m_destMsg->SetLabel( dest );
  
  wxYield();
  
  if (wxFileExists(dest))
  {
    wxString s = "Target file ";
    s += dest;
    s += " exists already. Overwrite?";
    int ret = wxMessageBox( s, "FileMaker", wxYES_NO );
    if (ret == wxNO) return;
  };
  
  FILE *fs = NULL, *fd = NULL;
  if (!(fs = fopen(src, "rb"))) 
  {
    wxString s = "Cannot open source file ";
    s += src;
    s += ".";
    wxMessageBox( s, "FileMaker" );
    return;
  } 
  else 
  if (!(fd = fopen(dest, "wb"))) 
  {
    fclose(fs);
    wxString s = "Cannot open target file ";
    s += dest;
    s += ".";
    wxMessageBox( s, "FileMaker" );
    return;
  };
  int ch;
  long kcounter = 0;
  while (!m_stop)
  {
    int counter = 0;
    while ((ch = getc( fs )) != EOF)
    {
      putc( ch, fd );
      counter++;
      if (counter == 1000) break;
    };
    kcounter++;
    m_statusMsg->SetLabel( IntToString( kcounter) );
    wxYield();
    if (ch == EOF) break;
  };
  fclose( fs ); 
  fclose( fd );
};


//-----------------------------------------------------------------------------
// wxDeleteStatusDia
//-----------------------------------------------------------------------------

/*

IMPLEMENT_DYNAMIC_CLASS(wxDeleteStatusDia,wxDialogBox);

wxDeleteStatusDia::wxDeleteStatusDia( wxFrame *parent, wxStringList *files ) :
  wxDialogBox( parent, "FileMaker delete job control", TRUE, 
    180, 180, 500, 200, wxCAPTION | wxTRANSIENT )
{
  int w = 0;
  int h = 0;
  GetSize( &w, &h );

  m_files = files;
  m_stop = FALSE;
  m_countFiles = 0;
  m_countDirs = 0;

  wxFont *myFont = wxTheFontList->FindOrCreateFont( 12, wxROMAN, wxNORMAL, wxNORMAL );
  SetLabelFont( myFont );
  SetButtonFont( myFont );
  
  wxStaticText *msg = new wxStaticText( this, "Deleting file or directory:", 10, 10 );
  m_targetMsg = new wxStaticText( this, "", 80, 40, 300 );
  msg = new wxStaticText( this, " Directories deleted:", 10, 80 );
  m_dirsMsg = new wxStaticText( this, "0", 120, 80, 80 );
  msg = new wxStaticText( this, "      Files deleted:", 10, 110 );
  m_filesMsg = new wxStaticText( this, "0", 120, 110, 100 );

  m_cancelButton = new wxButton( this, NULL, "Return", w-130, h-50, 85, 30 );
  
  Centre( wxVERTICAL | wxHORIZONTAL );

  m_timer = new wxDeleteTimer( this );
  m_timer->Start( 300, TRUE );
  
  Show( TRUE );
};

wxDeleteStatusDia::~wxDeleteStatusDia()
{
  delete m_timer;
};

void wxDeleteStatusDia::OnCommand( wxWindow &win, wxCommandEvent &WXUNUSED(event) )
{
  if (&win == m_cancelButton)
  {
    if (m_stop) Show( FALSE );
    m_stop = TRUE;
    return;
  };
};

void wxDeleteStatusDia::DoDelete(void)
{
  while (wxTheApp->Pending()) wxTheApp->Dispatch();
  wxNode *node = m_files->First();
  while (node)
  {
    char *target = (char*)node->Data();
    if (wxDirExists( target ))
      DeleteDir( target );
    else
      DeleteFile( target );
    if (m_stop) return;
    node = node->Next();
  };
  m_stop = TRUE;
};

void wxDeleteStatusDia::DeleteDir( char *target )
{
  wxString s = target;
  s += "// *";
  wxStringList list;
  char *f = wxFindFirstFile( s );
  while (f)
  {
    list.Add( f );
    f = wxFindNextFile();
  };
  wxNode *node = list.First();
  while (node)
  {
    f = (char*)node->Data();
    if (wxDirExists( f ))
      DeleteDir( f );
    else
      DeleteFile( f );
    if (m_stop) return;
    node = node->Next();
  };
  if (!wxRmdir( target ))
  {
    s = "Could not remove directory ";
    s += target;
    s += ".";
    wxMessageBox( s, "FileMaker" );
    return;
  }
  else
  {
    m_countDirs++;
    m_dirsMsg->SetLabel( wxIntToString( m_countDirs) );
  };
};

void wxDeleteStatusDia::DeleteFile( char *target )
{
  m_targetMsg->SetLabel( target );
  while (wxTheApp->Pending()) wxTheApp->Dispatch();
  if (!wxRemoveFile( target ))
  {
    wxString s = "Could not delete file ";
    s += target;
    s += ".";
    wxMessageBox( s, "FileMaker" );
  }
  else
  {
    m_countFiles++;
    m_filesMsg->SetLabel( wxIntToString( m_countFiles) );
  };
};

*/
