/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "treectrl.h"
#endif

#include "wx/treectrl.h"
#include "wx/settings.h"
#include "wx/log.h"

//-----------------------------------------------------------------------------
// wxTreeItem
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeItem, wxObject)

wxTreeItem::wxTreeItem()
{
  m_mask = 0;
  m_itemId = 0;
  m_state = 0;
  m_stateMask = 0;
  m_image = -1;
  m_selectedImage = -1;
  m_children = 0;
  m_data = 0;
};

//-----------------------------------------------------------------------------
// wxTreeEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent,wxCommandEvent)

wxTreeEvent::wxTreeEvent( wxEventType commandType, int id ) :
  wxCommandEvent( commandType, id )
{
  m_code = 0;
  m_oldItem = 0;
};

//-----------------------------------------------------------------------------
// wxGenericTreeItem
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGenericTreeItem,wxObject)

wxGenericTreeItem::wxGenericTreeItem( wxGenericTreeItem *parent )
{
  Reset();
  m_parent = parent;
  m_hasHilight = FALSE;
};

wxGenericTreeItem::wxGenericTreeItem( wxGenericTreeItem *parent, const wxTreeItem& item, wxDC *dc )
{
  Reset();
  SetItem( item, dc );
  m_parent = parent;
  m_hasHilight = FALSE;
};

void wxGenericTreeItem::SetItem( const wxTreeItem &item, wxDC *dc )
{
  if ((item.m_mask & wxTREE_MASK_HANDLE) == wxTREE_MASK_HANDLE)
    m_itemId = item.m_itemId;
  if ((item.m_mask & wxTREE_MASK_STATE) == wxTREE_MASK_STATE)
    m_state = item.m_state;
  if ((item.m_mask & wxTREE_MASK_TEXT) == wxTREE_MASK_TEXT)
    m_text = item.m_text;
  if ((item.m_mask & wxTREE_MASK_IMAGE) == wxTREE_MASK_IMAGE)
    m_image = item.m_image;
  if ((item.m_mask & wxTREE_MASK_SELECTED_IMAGE) == wxTREE_MASK_SELECTED_IMAGE)
    m_selectedImage = item.m_selectedImage;
  if ((item.m_mask & wxTREE_MASK_CHILDREN) == wxTREE_MASK_CHILDREN)
    m_hasChildren = (item.m_children > 0);
  if ((item.m_mask & wxTREE_MASK_DATA) == wxTREE_MASK_DATA)
    m_data = item.m_data;
  long lw = 0;
  long lh = 0;
  dc->GetTextExtent( m_text, &lw, &lh );
  m_width = lw;
  m_height = lh;
};

void wxGenericTreeItem::SetText( const wxString &text, wxDC *dc )
{
  m_text = text;
  long lw = 0;
  long lh = 0;
  dc->GetTextExtent( m_text, &lw, &lh );
  m_width = lw;
  m_height = lh;
};

void wxGenericTreeItem::Reset()
{
  m_itemId = -1;
  m_state = 0;
  m_text = "";
  m_image = -1;
  m_selectedImage = -1;
//  m_children = 0;
  m_hasChildren = FALSE;
  m_data = 0;
  m_x = 0;
  m_y = 0;
  m_height = 0;
  m_width = 0;
  m_xCross = 0;
  m_yCross = 0;
  m_level = 0;
  m_children.DeleteContents( TRUE );
  m_isCollapsed = TRUE;
  m_parent = NULL;
};

void wxGenericTreeItem::GetItem( wxTreeItem &item ) const
{
  if ((item.m_mask & wxTREE_MASK_STATE) == wxTREE_MASK_STATE)
    item.m_state = m_state;
  if ((item.m_mask & wxTREE_MASK_TEXT) == wxTREE_MASK_TEXT)
    item.m_text = m_text;
  if ((item.m_mask & wxTREE_MASK_IMAGE) == wxTREE_MASK_IMAGE)
    item.m_image = m_image;
  if ((item.m_mask & wxTREE_MASK_SELECTED_IMAGE) == wxTREE_MASK_SELECTED_IMAGE)
    item.m_selectedImage = m_selectedImage;
  if ((item.m_mask & wxTREE_MASK_CHILDREN) == wxTREE_MASK_CHILDREN)
    item.m_children = (int)m_hasChildren;
  if ((item.m_mask & wxTREE_MASK_DATA) == wxTREE_MASK_DATA)
    item.m_data = m_data;
};

bool wxGenericTreeItem::HasChildren()
{
  return m_hasChildren;
};

bool wxGenericTreeItem::HasPlus()
{
  if ( !HasChildren() )
    return FALSE;

  return !IsExpanded();
};

int wxGenericTreeItem::NumberOfVisibleDescendents()
{
  int ret = m_children.Number();
  wxNode *node = m_children.First();
  while (node)
  {
    wxGenericTreeItem *item = (wxGenericTreeItem*)node->Data();
    ret += item->NumberOfVisibleDescendents();
    node = node->Next();
  };
  return ret;
};

int wxGenericTreeItem::NumberOfVisibleChildren()
{
  return m_isCollapsed ? 0 : m_children.Number();
};

wxGenericTreeItem *wxGenericTreeItem::FindItem( long itemId ) const
{
  if (m_itemId == itemId) return (wxGenericTreeItem*)(this);
  wxNode *node = m_children.First();
  while (node)
  {
    wxGenericTreeItem *item = (wxGenericTreeItem*)node->Data();
    wxGenericTreeItem *res = item->FindItem( itemId );
    if (res) return (wxGenericTreeItem*)(res);
    node = node->Next();
  };
  return NULL;
};

void wxGenericTreeItem::AddChild( wxGenericTreeItem *child )
{
  m_children.Append( child );
};

void wxGenericTreeItem::SetCross( int x, int y )
{
  m_xCross = x;
  m_yCross = y;
};

void wxGenericTreeItem::GetSize( int &x, int &y )
{
  if (y < m_y + 10) y = m_y +10;
  int width = m_x +  m_width;
  if (width > x) x = width;
  wxNode *node = m_children.First();
  while (node)
  {
    wxGenericTreeItem *item = (wxGenericTreeItem*)node->Data();
    item->GetSize( x, y );
    node = node->Next();
  };
};

long wxGenericTreeItem::HitTest( const wxPoint& point, int &flags )
{
  if ((point.y > m_y) && (point.y < m_y+m_height))
  {
    if ((point.x > m_xCross-5) &&
        (point.x < m_xCross+5) &&
        (point.y > m_yCross-5) &&
        (point.y < m_yCross+5) &&
        (m_hasChildren))
    {
      flags = wxTREE_HITTEST_ONITEMBUTTON;
      return m_itemId;
    };
    if ((point.x > m_x) && (point.x < m_x+m_width))
    {
      flags = wxTREE_HITTEST_ONITEMLABEL;
      return m_itemId;
    };
    if (point.x > m_x)
    {
      flags = wxTREE_HITTEST_ONITEMRIGHT;
      return m_itemId;
    };
    flags = wxTREE_HITTEST_ONITEMINDENT;
    return m_itemId;
  }
  else
  {
    if (!m_isCollapsed)
    {
      wxNode *node = m_children.First();
      while (node)
      {
        wxGenericTreeItem *child = (wxGenericTreeItem*)node->Data();
        long res = child->HitTest( point, flags );
        if (res != -1) return res;
        node = node->Next();
      };
    };
  };
  return -1;
};

void wxGenericTreeItem::PrepareEvent( wxTreeEvent &event )
{
  event.m_item.m_itemId = m_itemId;
  event.m_item.m_state = m_state;
  event.m_item.m_text = m_text;
  event.m_item.m_image = m_image;
  event.m_item.m_selectedImage = m_selectedImage;
  event.m_item.m_children = (int)m_hasChildren;
  event.m_item.m_data = m_data;
  event.m_oldItem = 0;
  event.m_code = 0;
  event.m_pointDrag.x = 0;
  event.m_pointDrag.y = 0;
};

void wxGenericTreeItem::SendKeyDown( wxWindow *target )
{
  wxTreeEvent event( wxEVT_COMMAND_TREE_KEY_DOWN, target->GetId() );
  PrepareEvent( event );
  event.SetEventObject( target );
  target->ProcessEvent( event );
};

void wxGenericTreeItem::SendSelected( wxWindow *target )
{
  wxTreeEvent event( wxEVT_COMMAND_TREE_SEL_CHANGED, target->GetId() );
  PrepareEvent( event );
  event.SetEventObject( target );
  target->ProcessEvent( event );
};

void wxGenericTreeItem::SendDelete( wxWindow *target )
{
  wxTreeEvent event( wxEVT_COMMAND_TREE_DELETE_ITEM, target->GetId() );
  PrepareEvent( event );
  event.SetEventObject( target );
  target->ProcessEvent( event );
};

void wxGenericTreeItem::SendExpand( wxWindow *target )
{
  m_isCollapsed = FALSE;

  wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_EXPANDING, target->GetId() );
  event.SetEventObject( target );
  PrepareEvent( event );
  target->ProcessEvent( event );

  event.SetEventType(wxEVT_COMMAND_TREE_ITEM_EXPANDED);
  PrepareEvent( event );
  target->ProcessEvent( event );
};

void wxGenericTreeItem::SendCollapse( wxWindow *target )
{
  m_isCollapsed = TRUE;

  wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_COLLAPSING, target->GetId() );
  event.SetEventObject( target );
  PrepareEvent( event );
  target->ProcessEvent( event );

  event.SetEventType(wxEVT_COMMAND_TREE_ITEM_COLLAPSED);
  PrepareEvent( event );
  target->ProcessEvent( event );
};

void wxGenericTreeItem::SetHilight( bool set )
{
  m_hasHilight = set;
};

bool wxGenericTreeItem::HasHilight()
{
  return m_hasHilight;
};

//-----------------------------------------------------------------------------
// wxTreeCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl,wxScrolledWindow)

BEGIN_EVENT_TABLE(wxTreeCtrl,wxScrolledWindow)
  EVT_PAINT          (wxTreeCtrl::OnPaint)
  EVT_MOUSE_EVENTS   (wxTreeCtrl::OnMouse)
  EVT_CHAR           (wxTreeCtrl::OnChar)
  EVT_SET_FOCUS      (wxTreeCtrl::OnSetFocus)
  EVT_KILL_FOCUS     (wxTreeCtrl::OnKillFocus)
END_EVENT_TABLE()

wxTreeCtrl::wxTreeCtrl()
{
  m_current = NULL;
  m_anchor = NULL;
  m_hasFocus = FALSE;
  m_xScroll = 0;
  m_yScroll = 0;
  m_lastId = 0;
  m_lineHeight = 10;
  m_indent = 15;
  m_isCreated = FALSE;
  m_dc = NULL;
  m_hilightBrush = new wxBrush( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT), wxSOLID );
  m_imageList = NULL;
  m_smallImageList = NULL;
};

wxTreeCtrl::wxTreeCtrl(wxWindow *parent, wxWindowID id,
            const wxPoint& pos, const wxSize& size,
            long style, const wxString& name )
{
  m_current = NULL;
  m_anchor = NULL;
  m_hasFocus = FALSE;
  m_xScroll = 0;
  m_yScroll = 0;
  m_lastId = 0;
  m_lineHeight = 10;
  m_indent = 15;
  m_isCreated = FALSE;
  m_dc = NULL;
  m_hilightBrush = new wxBrush( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT), wxSOLID );
  m_imageList = NULL;
  m_smallImageList = NULL;
  Create( parent, id, pos, size, style, name );
};

wxTreeCtrl::~wxTreeCtrl()
{
  if (m_dc) delete m_dc;
};

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos, const wxSize& size,
            long style, const wxString& name )
{
  wxScrolledWindow::Create( parent, id, pos, size, style, name );
  SetBackgroundColour( *wxWHITE );
  m_dottedPen = wxPen( *wxBLACK, 0, 0 );
  return TRUE;
};

int wxTreeCtrl::GetCount() const
{
  if (!m_anchor) return 0;
  return m_anchor->NumberOfVisibleDescendents();
};

long wxTreeCtrl::InsertItem( long parent, const wxString& label, int image,
      int selImage, long WXUNUSED(insertAfter) )
{
  wxGenericTreeItem *p = NULL;
  if (parent == 0)
  {
    if (m_anchor) return -1;
  }
  else
  {
    p = FindItem( parent );
    if (!p) return -1;
  };
  wxTreeItem item;
  m_lastId++;
  item.m_mask = wxTREE_MASK_HANDLE;
  item.m_itemId = m_lastId;
  if (!label.IsNull() || (label == ""))
  {
    item.m_text = label;
    item.m_mask |= wxTREE_MASK_TEXT;
  };
  if (image >= 0)
  {
    item.m_image = image;
    item.m_mask |= wxTREE_MASK_IMAGE;
  };
  if (selImage >= 0)
  {
    item.m_selectedImage = selImage;
    item.m_mask |= wxTREE_MASK_SELECTED_IMAGE;
  };

  wxClientDC dc(this);
  wxGenericTreeItem *new_child = new wxGenericTreeItem( p, item, &dc );
  if (p)
    p->AddChild( new_child );
  else
    m_anchor = new_child;

  if (p)
  {
    CalculatePositions();

    if (!p->HasChildren()) p->m_hasChildren = TRUE;

    int ch = 0;
    GetClientSize( NULL, &ch );

    wxRectangle rect;
    rect.x = 0; rect.y = 0;
    rect.width = 10000; rect.height = ch;

    PrepareDC( dc );
    if (p->m_children.Number() == 1)
    {
      rect.y = dc.LogicalToDeviceY( p->m_y );
    }
    else
    {
      wxNode *node = p->m_children.Member( new_child )->Previous();
      wxGenericTreeItem* last_child = (wxGenericTreeItem*)node->Data();
      rect.y = dc.LogicalToDeviceY( last_child->m_y );
    };

    long doX = 0;
    long doY = 0;
    dc.GetDeviceOrigin( &doX, &doY );
    rect.height = ch-rect.y-doY;

    AdjustMyScrollbars();

    if (rect.height > 0) Refresh( FALSE, &rect);
  }
  else
  {
    AdjustMyScrollbars();

    Refresh();
  };

  return m_lastId;
};

long wxTreeCtrl::InsertItem( long parent, wxTreeItem &info, long WXUNUSED(insertAfter) )
{
  int oldMask = info.m_mask;
  wxGenericTreeItem *p = NULL;
  if (parent == 0)
  {
    if (m_anchor) return -1;
  }
  else
  {
    p = FindItem( parent );
    if (!p)
    {
      printf( "TreeItem not found.\n" );
      return -1;
    };
  };
  long ret = 0;
  if ((info.m_mask & wxTREE_MASK_HANDLE) == 0)
  {
    m_lastId++;
    info.m_itemId = m_lastId;
    info.m_mask |= wxTREE_MASK_HANDLE;
    ret = m_lastId;
  }
  else
  {
    ret = info.m_itemId;
  };

  wxClientDC dc(this);
  wxGenericTreeItem *new_child = new wxGenericTreeItem( p, info, &dc );
  if (p)
    p->AddChild( new_child );
  else
    m_anchor = new_child;

  if (p)
  {
    CalculatePositions();

    if (!p->HasChildren()) p->m_hasChildren = TRUE;

    int ch = 0;
    GetClientSize( NULL, &ch );

    wxRectangle rect;
    rect.x = 0; rect.y = 0;
    rect.width = 10000; rect.height = ch;

    PrepareDC( dc );
    if (p->m_children.Number() == 1)
    {
      rect.y = dc.LogicalToDeviceY( p->m_y );
    }
    else
    {
      wxNode *node = p->m_children.Member( new_child )->Previous();
      wxGenericTreeItem* last_child = (wxGenericTreeItem*)node->Data();
      rect.y = dc.LogicalToDeviceY( last_child->m_y );
    };

    long doX = 0;
    long doY = 0;
    dc.GetDeviceOrigin( &doX, &doY );
    rect.height = ch-rect.y-doY;

    AdjustMyScrollbars();

    if (rect.height > 0) Refresh( FALSE, &rect);
  }
  else
  {
    AdjustMyScrollbars();

    Refresh();
  };

  info.m_mask = oldMask;
  return ret;
};

bool wxTreeCtrl::ExpandItem( long item, int action )
{
  wxGenericTreeItem *i = FindItem( item );
  if (!i)
    return FALSE;

  switch (action)
  {
    case wxTREE_EXPAND_EXPAND:
    {
      i->SendExpand( this );
      break;
    }

    case wxTREE_EXPAND_COLLAPSE_RESET:
    case wxTREE_EXPAND_COLLAPSE:
    {
      wxNode *node = i->m_children.First();
      while (node)
      {
        wxGenericTreeItem *child = (wxGenericTreeItem*)node->Data();
        if ( child->IsExpanded() )
          ExpandItem( child->m_itemId, wxTREE_EXPAND_COLLAPSE );
        node = node->Next();
      };

      CalculatePositions();

      i->SendCollapse( this );
      break;
    }

    case wxTREE_EXPAND_TOGGLE:
    {
      if ( i->IsExpanded() )
        ExpandItem( item, wxTREE_EXPAND_COLLAPSE );
      else
        ExpandItem( item, wxTREE_EXPAND_EXPAND );
      return TRUE;
    }
  };

  int cw = 0;
  int ch = 0;
  GetClientSize( &cw, &ch );
  wxRect rect;
  rect.x = 0;
  rect.width = cw;
  wxClientDC dc(this);
  PrepareDC(dc);
  rect.y = dc.LogicalToDeviceY( i->m_y );

  long doY = 0;
  dc.GetDeviceOrigin( NULL, &doY );
  rect.height = ch-rect.y-doY;
  Refresh( TRUE, &rect );

  AdjustMyScrollbars();

  return TRUE;
};

void wxTreeCtrl::DeleteItem( long item )
{
  wxGenericTreeItem *pItem = FindItem( item );
  wxCHECK_RET( pItem != NULL, "wxTreeCtrl::DeleteItem: no such pItem." );

  pItem->m_parent->m_children.DeleteObject(pItem);

  Refresh();
}

void wxTreeCtrl::DeleteChildren( long item )
{
  wxGenericTreeItem *pItem = FindItem( item );
  wxCHECK_RET( pItem != NULL, "wxTreeCtrl::DeleteChildren: no such pItem." );

  pItem->m_children.Clear();

  Refresh();
}

bool wxTreeCtrl::DeleteAllItems()
{
  delete m_anchor;
  m_anchor = NULL;
  Refresh();
  return TRUE;
};

bool wxTreeCtrl::GetItem( wxTreeItem &info ) const
{
  wxGenericTreeItem *i = FindItem( info.m_itemId );
  if (!i) return FALSE;
  i->GetItem( info );
  return TRUE;
};

long wxTreeCtrl::GetItemData( long item ) const
{
  wxGenericTreeItem *i = FindItem( item );
  if (!i) return 0;
  return i->m_data;
};

wxString wxTreeCtrl::GetItemText( long item ) const
{
  wxGenericTreeItem *i = FindItem( item );
  if (!i) return "";
  return i->m_text;
};

int wxTreeCtrl::GetItemImage(long item) const
{
  wxGenericTreeItem *i = FindItem( item );
  return i == 0 ? -1 : i->GetImage();
}

long wxTreeCtrl::GetParent( long item ) const
{
  wxGenericTreeItem *i = FindItem( item );
  if (!i) return -1;
  i = i->m_parent;
  if (!i) return -1;
  return i->m_parent->m_itemId;
};

long wxTreeCtrl::GetRootItem() const
{
  if (m_anchor) return m_anchor->m_itemId;
  return -1;
};

long wxTreeCtrl::GetSelection() const
{
  return m_current ? m_current->GetItemId() : -1;
};

bool wxTreeCtrl::SelectItem(long itemId)
{
  wxGenericTreeItem *pItem = FindItem(itemId);
  if ( !pItem ) {
    wxLogDebug("Can't select an item %d which doesn't exist.", itemId);

    return FALSE;
  }

  SelectItem(pItem, FALSE /* no events */);

  return TRUE;
};

void wxTreeCtrl::SelectItem(wxGenericTreeItem *item, bool bDoEvents)
{
  if (m_current != item)
  {
    if (m_current)
    {
      m_current->SetHilight( FALSE );
      RefreshLine( m_current );
    };
    m_current = item;
    m_current->SetHilight( TRUE );
    RefreshLine( m_current );

    if ( bDoEvents )
      m_current->SendSelected( this );
  }
}

bool wxTreeCtrl::ItemHasChildren( long item ) const
{
  wxGenericTreeItem *i = FindItem( item );
  if (!i) return FALSE;
  return i->m_hasChildren;
};

void wxTreeCtrl::SetIndent( int indent )
{
  m_indent = indent;
  Refresh();
};

int wxTreeCtrl::GetIndent() const
{
  return m_indent;
};

bool wxTreeCtrl::SetItem( wxTreeItem &info )
{
  wxGenericTreeItem *i = FindItem( info.m_itemId );
  if (!i) return FALSE;
  wxClientDC dc(this);
  i->SetItem( info, &dc );
  Refresh();
  return TRUE;
};

bool wxTreeCtrl::SetItemData( long item, long data )
{
  wxGenericTreeItem *i = FindItem( item );
  if (!i) return FALSE;
  i->m_data = data;
  return TRUE;
};

bool wxTreeCtrl::SetItemText( long item, const wxString &text )
{
  wxGenericTreeItem *i = FindItem( item );
  if (!i) return FALSE;
  wxClientDC dc(this);
  i->SetText( text, &dc );
  return TRUE;
};

void wxTreeCtrl::SetItemImage(long item, int image, int imageSel) const
{
  wxGenericTreeItem *i = FindItem( item );
  if ( i != 0 ) {
    i->SetImage(image);
    i->SetSelectedImage(imageSel);
  }
}

long wxTreeCtrl::HitTest( const wxPoint& point, int &flags )
{
  flags = 0;
  if (!m_anchor) return -1;
  return m_anchor->HitTest( point, flags );
};

wxImageList *wxTreeCtrl::GetImageList( int which ) const
{
  if (which == wxIMAGE_LIST_NORMAL) return m_imageList;
  return m_smallImageList;
};

void wxTreeCtrl::SetImageList( wxImageList *imageList, int which )
{
  if (which == wxIMAGE_LIST_NORMAL)
  {
    if (m_imageList) delete m_imageList;
    m_imageList = imageList;
  }
  else
  {
    if (m_smallImageList) delete m_smallImageList;
    m_smallImageList = imageList;
  };
};

void wxTreeCtrl::AdjustMyScrollbars()
{
  if (m_anchor)
  {
    int x = 0;
    int y = 0;
    m_anchor->GetSize( x, y );
    y += 2*m_lineHeight;
    int x_pos = GetScrollPos( wxHORIZONTAL );
    int y_pos = GetScrollPos( wxVERTICAL );
    SetScrollbars( 10, 10, x/10, y/10, x_pos, y_pos );
  }
  else
  {
    SetScrollbars( 0, 0, 0, 0 );
  };
};

void wxTreeCtrl::PaintLevel( wxGenericTreeItem *item, wxPaintDC &dc, int level, int &y )
{
  int horizX = level*m_indent;

  item->m_x = horizX+33;
  item->m_y = y-m_lineHeight/3;
  item->m_height = m_lineHeight;

  item->SetCross( horizX+15, y );

  int oldY = y;

  if (IsExposed( 0, item->m_y-2, 10000, m_lineHeight+4 ))
  {
    int startX = horizX;
    int endX = horizX + 10;

    if (!item->HasChildren()) endX += 20;
    dc.DrawLine( startX, y, endX, y );

      if (item->HasChildren())
      {
        dc.DrawLine( horizX+20, y, horizX+30, y );
        dc.SetPen( *wxGREY_PEN );
        dc.DrawRectangle( horizX+10, y-4, 11, 9 );
        dc.SetPen( *wxBLACK_PEN );
        dc.DrawLine( horizX+13, y, horizX+18, y );
        if (item->HasPlus())
          dc.DrawLine( horizX+15, y-2, horizX+15, y+3 );
      };

      if (item->HasHilight())
      {
        dc.SetTextForeground( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
        dc.SetBrush( *m_hilightBrush );
        long tw, th;
        dc.GetTextExtent( item->m_text, &tw, &th );
        if (m_hasFocus)
        {
          dc.SetPen( *wxBLACK_PEN );
          dc.DrawRectangle( item->m_x-2, item->m_y-2, tw+4, th+4 );
        }
        else
        {
          dc.SetPen( *wxTRANSPARENT_PEN );
          dc.DrawRectangle( item->m_x-2, item->m_y-2, tw+4, th+4 );
        }
        dc.DrawText( item->m_text, item->m_x, item->m_y );

        dc.SetPen( *wxBLACK_PEN );
        dc.SetTextForeground( *wxBLACK );
        dc.SetBrush( *wxWHITE_BRUSH );
      }
      else
      {
        dc.SetPen( *wxTRANSPARENT_PEN );
        long tw, th;
        dc.GetTextExtent( item->m_text, &tw, &th );
        dc.DrawRectangle( item->m_x-2, item->m_y-2, tw+4, th+4 );
        dc.DrawText( item->m_text, item->m_x, item->m_y );
        dc.SetPen( *wxBLACK_PEN );
      };
  };

  if (item->NumberOfVisibleChildren() == 0) return;

  int semiOldY = y;

  wxNode *node = item->m_children.First();
  while (node)
  {
    wxGenericTreeItem *child = (wxGenericTreeItem *)node->Data();

    y += m_lineHeight;
    semiOldY = y;

    PaintLevel( child, dc, level+1, y );

    node = node->Next();
  };

  dc.DrawLine( horizX+15, oldY+5, horizX+15, semiOldY );
}

void wxTreeCtrl::OnPaint( const wxPaintEvent &WXUNUSED(event) )
{
  if (!m_anchor) return;

  if (!m_dc)
  {
    m_dc = new wxPaintDC(this);
    PrepareDC( *m_dc );
  };

  m_dc->SetFont( wxSystemSettings::GetSystemFont( wxSYS_SYSTEM_FONT ) );

  m_dc->SetPen( m_dottedPen );
  m_lineHeight = (int)(m_dc->GetCharHeight() + 4);

  int y = m_lineHeight / 2 + 2;
  PaintLevel( m_anchor, *m_dc, 0, y );
};

void wxTreeCtrl::OnSetFocus( const wxFocusEvent &WXUNUSED(event) )
{
  m_hasFocus = TRUE;
  if (m_current) RefreshLine( m_current );
};

void wxTreeCtrl::OnKillFocus( const wxFocusEvent &WXUNUSED(event) )
{
  m_hasFocus = FALSE;
  if (m_current) RefreshLine( m_current );
};

void wxTreeCtrl::OnChar( wxKeyEvent &event )
{
  event.Skip();
};

void wxTreeCtrl::OnMouse( const wxMouseEvent &event )
{
  if (!event.LeftDown() &&
      !event.LeftDClick()) return;

  wxClientDC dc(this);
  PrepareDC(dc);
  long x = dc.DeviceToLogicalX( (long)event.GetX() );
  long y = dc.DeviceToLogicalY( (long)event.GetY() );

  int flag = 0;
  long id = HitTest( wxPoint(x,y), flag );
  if (id == -1)
    return;
  wxGenericTreeItem *item = FindItem( id );

  if (!item) return;
  if ((flag != wxTREE_HITTEST_ONITEMBUTTON) &&
      (flag != wxTREE_HITTEST_ONITEMLABEL)) return;

  SelectItem(item);

  if (event.LeftDClick())
    m_current->SendKeyDown( this );

  if (flag == wxTREE_HITTEST_ONITEMBUTTON)
  {
    ExpandItem( item->m_itemId, wxTREE_EXPAND_TOGGLE );
    return;
  };
};

void wxTreeCtrl::CalculateLevel( wxGenericTreeItem *item, wxPaintDC &dc, int level, int &y )
{
  int horizX = level*m_indent;

  item->m_x = horizX+33;
  item->m_y = y-m_lineHeight/3-2;
  item->m_height = m_lineHeight;

  if (item->NumberOfVisibleChildren() == 0) return;

  wxNode *node = item->m_children.First();
  while (node)
  {
    wxGenericTreeItem *child = (wxGenericTreeItem *)node->Data();

    y += m_lineHeight;
    CalculateLevel( child, dc, level+1, y );

    node = node->Next();
  };
};

void wxTreeCtrl::CalculatePositions()
{
  if (!m_anchor)
    return;

  wxClientDC dc(this);
  PrepareDC( dc );

  dc.SetFont( wxSystemSettings::GetSystemFont( wxSYS_SYSTEM_FONT ) );

  dc.SetPen( m_dottedPen );
  m_lineHeight = (int)(dc.GetCharHeight() + 4);

  int y = m_lineHeight / 2 + 2;
  CalculateLevel( m_anchor, dc, 0, y );
};

wxGenericTreeItem *wxTreeCtrl::FindItem( long itemId ) const
{
  if (!m_anchor) return NULL;
  return m_anchor->FindItem( itemId );
};

void wxTreeCtrl::RefreshLine( wxGenericTreeItem *item )
{
  if (!item) return;
  wxClientDC dc(this);
  PrepareDC( dc );
  wxRect rect;
  rect.x = dc.LogicalToDeviceX( item->m_x-2 );
  rect.y = dc.LogicalToDeviceY( item->m_y-2 );
  rect.width = 1000;
  rect.height = dc.GetCharHeight()+4;
  Refresh( TRUE, &rect );
};



