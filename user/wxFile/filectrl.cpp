/*
 * Author: Robert Roebling
 *
 * Copyright: (C) 1997,1998 Robert Roebling
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the wxWindows Licence, which
 * you have received with this library (see Licence.htm).
 *
 */


#ifdef __GNUG__
#pragma implementation "filectrl.h"
#endif

#include "filectrl.h"

#include "wx/dnd.h"

#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
#include "pwd.h"
#include "grp.h"
#include "time.h"

#include "folder.xpm"
#include "txt.xpm"
#include "list.xpm"
#include "find.xpm"

//-----------------------------------------------------------------------------
//  wxFileData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileData,wxObject);

wxFileData::wxFileData( const wxString &name, const wxString &fname )
{
  m_name = name;
  m_fileName = fname;

  struct stat buff;
  stat( m_fileName.GetData(), &buff );
  struct stat lbuff;
  lstat( m_fileName.GetData(), &lbuff );

  struct tm *t = localtime( &lbuff.st_mtime );
//  struct passwd *user = getpwuid( buff.st_uid );
//  struct group *grp = getgrgid( buff.st_gid );

  m_isDir = S_ISDIR( buff.st_mode );
  m_isLink = S_ISLNK( lbuff.st_mode );
  m_isExe = ((buff.st_mode & S_IXUSR ) == S_IXUSR );

  m_size = buff.st_size;

  m_hour = t->tm_hour;
  m_minute = t->tm_min;
  m_month = t->tm_mon+1;
  m_day = t->tm_mday;
  m_year = t->tm_year;

  m_permissions.sprintf( "%c%c%c", 
     ((( buff.st_mode & S_IRUSR ) == S_IRUSR ) ? 'r' : '-'),
     ((( buff.st_mode & S_IWUSR ) == S_IWUSR ) ? 'w' : '-'),
     ((( buff.st_mode & S_IXUSR ) == S_IXUSR ) ? 'x' : '-') );
};

wxString wxFileData::GetName(void) const
{
  return m_name;
};

wxString wxFileData::GetFullName(void) const
{
  return m_fileName;
};

wxString wxFileData::GetHint(void) const
{
  wxString s = m_fileName;
  s += "  ";
  if (m_isDir) s += "<DIR> ";
  else if (m_isLink) s += "<LINK> ";
  else
  {
    s += LongToString( m_size );
    s += " bytes ";
  };
  s += IntToString( m_day );
  s += ".";
  s += IntToString( m_month );
  s += ".";
  s += IntToString( m_year );
  s += "  ";
  s += IntToString( m_hour );
  s += ":";
  s += IntToString( m_minute );
  s += "  ";
  s += m_permissions;
  return s;
};

wxString wxFileData::GetEntry( const int num )
{
  wxString s;
  switch (num)
  {
    case 0:
      s = m_name;
      break;
    case 1:
      if (m_isDir) s = "<DIR>";
      else if (m_isLink) s = "<LINK>";
      else s = LongToString( m_size );
      break;
    case 2:
      if (m_day < 10) s = "0"; else s = "";
      s += IntToString( m_day );
      s += ".";
      if (m_month < 10) s += "0"; 
      s += IntToString( m_month );
      s += ".";
      if (m_year < 10) s += "0"; // this should happen real soon...
      s += IntToString( m_year );
      break;
    case 3:
      if (m_hour < 10) s = "0"; else s = "";
      s += IntToString( m_hour );
      s += ":";
      if (m_minute < 10) s += "0";
      s += IntToString( m_minute );
      break;
    case 4:
      s = m_permissions;
      break;
    default:
      s = "No entry";
      break;
  };
  return s;
};

bool wxFileData::IsDir( void )
{
  return m_isDir;
};

bool wxFileData::IsExe( void )
{
  return m_isExe;
};

bool wxFileData::IsLink( void )
{
  return m_isLink;
};

long wxFileData::GetSize( void )
{
  return m_size;
};

bool wxFileData::NewNameIsLegal( const wxString &s )
{
  wxString fileName = wxPathOnly( m_fileName );
  fileName += "/";
  fileName += s;
  return (!wxFileExists( fileName ));
};

bool wxFileData::Rename( const wxString &s )
{
  wxString fileName = wxPathOnly( m_fileName );
  fileName += "/";
  fileName += s;
  bool ret = wxRenameFile( m_fileName, fileName );
  if (ret)
  {
    m_fileName = fileName;
    m_name = s;
  };
  return ret;
};

void wxFileData::MakeItem( wxListItem &item )
{
  item.m_text = m_name;
  item.m_colour = wxBLACK;
  if (IsExe()) item.m_colour = wxRED;
  if (IsDir()) item.m_colour = wxBLUE;
  if (IsLink())
  {
    wxColour *dg = wxTheColourDatabase->FindColour( "MEDIUM GREY" );
    item.m_colour = dg;
  };
  item.m_data = (long)this;
};
  
//-----------------------------------------------------------------------------
//  wxFileCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileCtrl,wxListCtrl);

BEGIN_EVENT_TABLE(wxFileCtrl,wxListCtrl)
  EVT_SET_FOCUS   (wxFileCtrl::OnSetFocus)
END_EVENT_TABLE()

wxFileCtrl *wxFileCtrl::m_lastFocus = NULL;

wxFileCtrl::wxFileCtrl( void )
{
  m_dirName = "/";
  m_showHidden = FALSE;
};

wxFileCtrl::wxFileCtrl( wxWindow *win, const wxWindowID id, const wxString &dirName,
      const wxPoint &pos, const wxSize &size,
      const long style, const wxString &name ) :
  wxListCtrl( win, id, pos, size, style, name )
{
  wxImageList *imageList = new wxImageList();
  imageList->Add( wxBitmap( folder_xpm ) );
  imageList->Add( wxBitmap( txt_xpm ) );
  imageList->Add( wxBitmap( list_xpm ) );
  imageList->Add( wxBitmap( find_xpm ) );
  
  SetImageList( imageList, wxIMAGE_LIST_NORMAL );
  
  m_dirName = dirName;
  m_showHidden = FALSE;
  Update();
  
  m_lastFocus = this;
  
  SetDropTarget( new wxTextDropTarget() );
};

void wxFileCtrl::ChangeToListMode()
{
  SetSingleStyle( wxLC_LIST );
  Update();
};

void wxFileCtrl::ChangeToReportMode()
{
  SetSingleStyle( wxLC_REPORT );
  Update();
};

void wxFileCtrl::ChangeToIconMode()
{
  SetSingleStyle( wxLC_ICON );
  Update();
};

void wxFileCtrl::ShowHidden( bool show )
{
  m_showHidden = show;
  Update();
};

int ListCompare( const long data1, const long data2, const long WXUNUSED(data) )
{
   wxFileData *fd1 = (wxFileData*)data1 ;
   wxFileData *fd2 = (wxFileData*)data2 ;
   if (fd1->IsDir() && !fd2->IsDir()) return -1;
   if (fd2->IsDir() && !fd1->IsDir()) return 1;
   return strcmp( fd1->GetName(), fd2->GetName() );
};

void wxFileCtrl::Update( void )
{ 
  DeleteAllItems();
  for (int i = 0; i < 5; i++) DeleteColumn( 0 );
  long my_style = GetWindowStyleFlag();
  if (my_style & wxLC_REPORT)
  {
    InsertColumn( 0, "Name", wxLIST_FORMAT_LEFT, 110 );
    InsertColumn( 1, "Size", wxLIST_FORMAT_LEFT, 60 );
    InsertColumn( 2, "Date", wxLIST_FORMAT_LEFT, 55 );
    InsertColumn( 3, "Time", wxLIST_FORMAT_LEFT, 50 );
    InsertColumn( 4, "Permissions", wxLIST_FORMAT_LEFT, 120 );
  };
  wxFileData *fd = NULL;
  wxListItem item;
  item.m_mask = wxLIST_MASK_TEXT + wxLIST_MASK_DATA;
  if (my_style & wxLC_ICON) item.m_mask += wxLIST_MASK_IMAGE;
  item.m_itemId = 0;
  item.m_col = 0;
  wxString s;
  wxString res = m_dirName + "/*";
  char *f = wxFindFirstFile( res.GetData(), 0 );
  while (f)   
  {
    res = wxFileNameFromPath( f );
    fd = new wxFileData( res, f );
    s = fd->GetName();
    if (m_showHidden || (s[0] != '.'))
    {
      fd->MakeItem( item );
      if (my_style & wxLC_REPORT)
      {
        InsertItem( item );
        for (int i = 1; i < 5; i++) SetItem( item.m_itemId, i, fd->GetEntry( i) );
      }
      else if (my_style & wxLC_LIST)
      {
        InsertItem( item );
      } 
     else if (my_style & wxLC_ICON)
      {
        if (fd->IsDir()) item.m_image = 0; else item.m_image = 1;
        InsertItem( item );
      };
      item.m_itemId++;
    };
    f = wxFindNextFile();
  };
  SortItems( ListCompare, 0 );
  RealizeChanges();
};


int wxFileCtrl::FillList( wxStringList &list )
{
  long index = -1;
  int count = 0;
  wxString s;
  for (;;)
  {
    index = GetNextItem( index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    if (index == -1) break;
    wxListItem item;
    item.m_itemId = index;
    GetItem( item );
    wxFileData *fd = (wxFileData*)item.m_data;
    list.Add( fd->GetFullName() );
    index++;
    count++;
  };
  if (count == 0)
  {
    index = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED );
    if (index == -1) return 0;
    wxListItem item;
    item.m_itemId = index;
    GetItem( item );
    wxFileData *fd = (wxFileData*)item.m_data;
    list.Add( fd->GetFullName() );
    count = 1;
  };
  return count;
};

void wxFileCtrl::DeleteFiles(void)
{
/*
  wxStringList list;
  int count = FillList( list );
  if (count > 0)
  { 
    wxString s = "Delete ";
    s += wxIntToString( count );
    s += " selected file";
    if (count > 1) s += "s";
    s += " or director";
    if (count > 1) s += "ies?"; else s+= "y?";
    if (wxYES == wxMessageBox( s, "Delete", wxYES_NO ))
      wxDeleteStatusDia( NULL, &list );
  };
*/
};

void wxFileCtrl::CopyFiles( char *WXUNUSED(dest) )
{
/*
  wxStringList list;
  int count = FillList( list );
  wxString s = dest;
  int ret = 0; // 0 = nix, 1 = copy, 2 = move
  wxCopyMoveDia( (wxFrame*)GetParent(), count, &ret, &s );
  if (ret == 1) 
    wxCopyStatusDia( NULL, s, &list );
*/
};

void wxFileCtrl::MoveFiles( char *WXUNUSED(dest) )
{
};

void wxFileCtrl::RenameFile(void)
{
};

void wxFileCtrl::MakeDir(void)
{
/*
  wxString s = wxGetTextFromUser( "Enter new directory name:", "Make directory" );
  if (s.IsNull()) return;
  if (s == "") return;
  if ((s == ".") || (s == ".."))
  {
    wxMessageBox( "This was obviously an invalid directory name.", "Go away." );
    return;
  };
  wxString dir;
  GetDir( dir );
  dir += "/";
  dir += s;
  if (wxFileExists( dir ))
  {
    wxMessageBox( "Filename exists already. Cannot create directoy.", "Make directory" );
    return;
  };
  wxMkdir( dir );
  Update();
*/
};

void wxFileCtrl::GoToParentDir(void)
{
  wxString s = m_dirName;
  int pos = s.Last( '/' );  
  if ((pos >= 0) && (s != "/"))
  {
    s.Remove( pos, s.Length()-pos );
    if (s.Length() == 0) s = "/";
    m_dirName = s;
    Update();
  };
};

void wxFileCtrl::GoToHomeDir(void)
{
  wxString s = wxGetUserHome( wxString() );
  m_dirName = s;
  Update();
};

void wxFileCtrl::GoToDir( const wxString &dir )
{
  m_dirName = dir;
  Update();
};

void wxFileCtrl::GetDir( wxString &dir )
{
  dir = m_dirName;
};

/*
void wxFileCtrl::OnDropFiles( int WXUNUSED(n), char **WXUNUSED(data), int WXUNUSED(x), int WXUNUSED(y) )
{
  wxString destDir;
  wxPoint pt( x, y );
  int flag = wxLIST_HITTEST_ONITEM;
  long hit = HitTest( pt, flag );
  if (hit > -1)
  {
    wxListItem li;
    li.m_itemId = hit;
    GetItem( li );
    wxFileData *fd = (wxFileData*)li.m_data;
    if (fd->IsDir()) fd->GetFullName( destDir );
  };
  if (destDir.IsNull()) destDir = m_dirName;
  int ret = 0; // 0 = nix, 1 = copy, 2 = move
  wxCopyMoveDia( (wxFrame*)GetParent(), n, &ret, &destDir );
  if (ret == 1)
  {  
     wxStringList slist;
     for (int i = 0; i < n; i++) slist.Add( data[i] );
     wxCopyStatusDia( NULL, destDir.GetData(), &slist );
     Update();
  };
};
*/

void wxFileCtrl::OnListDeleteItem( wxListEvent &event )
{
  wxFileData *fd = (wxFileData*)event.m_item.m_data;
  delete fd;
};

void wxFileCtrl::OnListKeyDown( wxListEvent &event )
{
  wxFileData *fd = (wxFileData*)event.m_item.m_data;
  if (fd->IsDir())
  {
    m_dirName = fd->GetFullName();
    Update();
    Refresh();
    return;
  };
  if (fd->IsExe())
  {
    wxExecute( fd->GetFullName() );
    return;
  };
};

void wxFileCtrl::OnListEndLabelEdit( wxListEvent &event )
{
  wxFileData *fd = (wxFileData*)event.m_item.m_data;
  wxString newName = event.m_item.m_text;
  if (fd->NewNameIsLegal( newName ))
  {
    if (fd->Rename( newName ))
    {
      Update();
    }
    else
    {
      wxString s = "Could not rename file to ";
      s += newName;
      s += ".";
      wxMessageBox( s, "FileMaker", wxOK );
    };
  }
  else
  {
    wxString s = "File name ";
    s += newName;
    s += " exists already or is invalid.\n";
    s += "Could not rename file.";
    wxMessageBox( s, "FileMaker", wxOK );
  };
  return;
};

void wxFileCtrl::OnSetFocus( wxFocusEvent &event )
{
  m_lastFocus = this;
  event.Skip();
};


