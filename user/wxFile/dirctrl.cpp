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
#pragma implementation "dirctrl.h"
#endif

#include "dirctrl.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"

//-----------------------------------------------------------------------------
// wxDirInfo
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDirInfo,wxObject)
 
wxDirInfo::wxDirInfo( const wxString &path )
{
  m_showHidden = FALSE;
  m_path = path;
  if (m_path == "/") m_name ="The Computer";
  else 
  if (m_path == "/home")
  {
    m_name = "My Home";
    m_path += "/";
    char buf[300];
    wxGetHomeDir( buf );
    m_path = buf;
  }
  else
  if (m_path == "/proc") m_name = "Info Filesystem";
  else
  if (m_path == "/mnt") m_name = "Mounted Devices";
  else
  if (m_path == "/usr/X11R6") m_name = "User X11";
  else
  if (m_path == "/usr") m_name = "User";
  else
  if (m_path == "/var") m_name = "Variables";
  else
  if (m_path == "/usr/local") m_name = "User local";
  else
  if (m_path == "/mnt") m_name = "Mounted Devices";
  else 
    m_name = wxFileNameFromPath( m_path ); 
};

wxString wxDirInfo::GetName(void) const
{
  return m_name;
};

wxString wxDirInfo::GetPath(void) const
{
  return m_path;
};

//-----------------------------------------------------------------------------
// wxDirCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDirCtrl,wxTreeCtrl)

BEGIN_EVENT_TABLE(wxDirCtrl,wxTreeCtrl)
  EVT_TREE_ITEM_EXPANDED      (-1, wxDirCtrl::OnExpandItem)
  EVT_TREE_ITEM_COLLAPSED     (-1, wxDirCtrl::OnCollapseItem)
  EVT_TREE_DELETE_ITEM        (-1, wxDirCtrl::OnDeleteItem)
  EVT_MOUSE_EVENTS            (wxDirCtrl::OnMouse)
END_EVENT_TABLE()

wxDirCtrl::wxDirCtrl(void)
{
  m_showHidden = FALSE;
};

wxDirCtrl::wxDirCtrl(wxWindow *parent, const wxWindowID id, const wxString &WXUNUSED(dir),
            const wxPoint& pos, const wxSize& size,
            const long style, const wxString& name )
 :
  wxTreeCtrl( parent, id, pos, size, style, name )
{
  m_showHidden = FALSE;

  wxTreeItem item;
  item.m_mask = wxTREE_MASK_TEXT | wxTREE_MASK_CHILDREN | wxTREE_MASK_DATA;
  item.m_text = "Sections";
  item.m_children = 1;
/*
  wxDirInfo *info = new wxDirInfo( dir );
  item.m_data = (long)info;
*/  
  m_rootId = InsertItem( 0, item );
};

void wxDirCtrl::OnExpandItem( const wxTreeEvent &event )
{
  if (event.m_item.m_itemId == m_rootId)
  {
    
    wxTreeItem item;
    item.m_mask = wxTREE_MASK_TEXT | wxTREE_MASK_CHILDREN | wxTREE_MASK_DATA;
    item.m_children = 1;
    
    wxDirInfo *info = new wxDirInfo( "/" );
    item.m_text = info->GetName();
    item.m_data = (long)info;
    InsertItem( m_rootId, item );
  
    info = new wxDirInfo( "/home" );
    item.m_text = info->GetName();
    item.m_data = (long)info;
    InsertItem( m_rootId, item );
  
    info = new wxDirInfo( "/mnt" );
    item.m_text = info->GetName();
    item.m_data = (long)info;
    InsertItem( m_rootId, item );
  
    info = new wxDirInfo( "/usr" );
    item.m_text = info->GetName();
    item.m_data = (long)info;
    InsertItem( m_rootId, item );
  
    info = new wxDirInfo( "/usr/X11R6" );
    item.m_text = info->GetName();
    item.m_data = (long)info;
    InsertItem( m_rootId, item );
  
    info = new wxDirInfo( "/usr/local" );
    item.m_text = info->GetName();
    item.m_data = (long)info;
    InsertItem( m_rootId, item );
  
    info = new wxDirInfo( "/var" );
    item.m_text = info->GetName();
    item.m_data = (long)info;
    InsertItem( m_rootId, item );
  
    info = new wxDirInfo( "/proc" );
    item.m_text = info->GetName();
    item.m_data = (long)info;
    InsertItem( m_rootId, item );
    
    return;
  };

  wxDirInfo *info = (wxDirInfo *)event.m_item.m_data;
  if (!info) return;
  
  wxArrayString slist;
  wxString search,path,filename;
  
  search = info->GetPath();
  search += "/*";
  
  path = wxFindFirstFile( search, wxDIR );
  while (!path.IsNull())
  {
    filename = wxFileNameFromPath( path );
    if (m_showHidden || (filename[0] != '.'))
    {
      if ((filename != ".") && 
          (filename != "..") &&
	  (path != "/home") &&
	  (path != "/usr/X11R6") &&
	  (path != "/usr/local") &&
	  (path != "/usr") &&
	  (path != "/var") &&
	  (path != "/home") &&
	  (path != "/proc") &&
	  (path != "/mnt")
	 )
        slist.Add( path );  // ref counting in action !
    };
    path = wxFindNextFile();
  };

  for (uint i = 0; i < slist.Count(); i++)
  {
    search = slist[i];
    search += "/*";
    path = wxFindFirstFile( search, wxDIR );
    
    wxDirInfo *child = new wxDirInfo( slist[i] );
    wxTreeItem item;
    item.m_mask = wxTREE_MASK_TEXT | wxTREE_MASK_CHILDREN | wxTREE_MASK_DATA;
    item.m_text = child->GetName();
    item.m_children = 0;
    if (!path.IsNull()) item.m_children = 1;
    item.m_data = (long)child;
    InsertItem( event.m_item.m_itemId, item );
  };
};

void wxDirCtrl::OnCollapseItem( const wxTreeEvent &event )
{
  DeleteChildren( event.m_item.m_itemId );
};

void wxDirCtrl::OnDeleteItem( const wxTreeEvent &event )
{
  wxDirInfo *info = (wxDirInfo *)event.m_item.m_data;
  if (info) delete info;
};

void wxDirCtrl::OnMouse( wxMouseEvent &event )
{
  event.Skip(TRUE);
  
  if (event.LeftDown())
  {
    m_dragX = event.GetX();
    m_dragY = event.GetY();
    return;
  };
  
  if (event.Dragging())
  {
    if ((abs(m_dragX-event.GetX()) < 2) &&
        (abs(m_dragY-event.GetY()) < 2)) return;
	
    wxTextDragSource drag( this );
    drag.SetTextData( "Oh, what a drag." );
    drag.Start( event.GetX(), event.GetY() );
  };
};

