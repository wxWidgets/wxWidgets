/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.cpp
// Purpose:     generic tree control implementation
// Author:      Robert Roebling
// Created:     01/02/97
// Modified:    22/10/98 - almost total rewrite, simpler interface (VZ)
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// =============================================================================
// declarations
// =============================================================================

// -----------------------------------------------------------------------------
// headers
// -----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "treectrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/generic/treectrl.h"
#include "wx/generic/imaglist.h"
#include "wx/settings.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/arrimpl.cpp"
#include "wx/dcclient.h"
#include "wx/msgdlg.h"

// -----------------------------------------------------------------------------
// array types
// -----------------------------------------------------------------------------

class WXDLLEXPORT wxGenericTreeItem;

WX_DEFINE_ARRAY(wxGenericTreeItem *, wxArrayGenericTreeItems);
WX_DEFINE_OBJARRAY(wxArrayTreeItemIds);

// -----------------------------------------------------------------------------
// private classes
// -----------------------------------------------------------------------------

// a tree item
class WXDLLEXPORT wxGenericTreeItem
{
public:
  // ctors & dtor
  wxGenericTreeItem() { m_data = NULL; }
  wxGenericTreeItem( wxGenericTreeItem *parent,
                     const wxString& text,
                     wxDC& dc,
                     int image, int selImage,
                     wxTreeItemData *data );

  ~wxGenericTreeItem();

  // trivial accessors
  wxArrayGenericTreeItems& GetChildren() { return m_children; }

  const wxString& GetText() const { return m_text; }
  int GetImage() const { return m_image; }
  int GetSelectedImage() const { return m_selImage; }
  wxTreeItemData *GetData() const { return m_data; }

  void SetText( const wxString &text );
  void SetImage(int image) { m_image = image; }
  void SetSelectedImage(int image) { m_selImage = image; }
  void SetData(wxTreeItemData *data) { m_data = data; }

  void SetHasPlus(bool has = TRUE) { m_hasPlus = has; }

  void SetBold(bool bold) { m_isBold = bold; }

  int GetX() const { return m_x; }
  int GetY() const { return m_y; }

  void SetX(int x) { m_x = x; }
  void SetY(int y) { m_y = y; }

  int  GetHeight() const { return m_height; }
  int  GetWidth()  const { return m_width; }

  void SetHeight(int h) { m_height = h; }
  void SetWidth(int w) { m_width = w; }


  wxGenericTreeItem *GetParent() const { return m_parent; }

  // operations
    // deletes all children notifying the treectrl about it if !NULL pointer
    // given
  void DeleteChildren(wxTreeCtrl *tree = NULL);
    // FIXME don't know what is it for
  void Reset();

  // get count of all children (and grand children if 'recursively')
  size_t GetChildrenCount(bool recursively = TRUE) const;

  void Insert(wxGenericTreeItem *child, size_t index)
    { m_children.Insert(child, index); }

  void SetCross( int x, int y );
  void GetSize( int &x, int &y, const wxTreeCtrl* );

  // return the item at given position (or NULL if no item), onButton is TRUE
  // if the point belongs to the item's button, otherwise it lies on the
  // button's label
  wxGenericTreeItem *HitTest( const wxPoint& point, const wxTreeCtrl *, int &flags);

  void Expand() { m_isCollapsed = FALSE; }
  void Collapse() { m_isCollapsed = TRUE; }

  void SetHilight( bool set = TRUE ) { m_hasHilight = set; }

  // status inquiries
  bool HasChildren() const { return !m_children.IsEmpty(); }
  bool HasHilight()  const { return m_hasHilight; }
  bool IsExpanded()  const { return !m_isCollapsed; }
  bool HasPlus()     const { return m_hasPlus || HasChildren(); }
  bool IsBold()      const { return m_isBold; }

private:
  wxString            m_text;

  int                 m_image,
                      m_selImage;

  wxTreeItemData     *m_data;

  // use bitfields to save size
  int                 m_isCollapsed :1;
  int                 m_hasHilight  :1; // same as focused
  int                 m_hasPlus     :1; // used for item which doesn't have
                                        // children but still has a [+] button
  int                 m_isBold      :1; // render the label in bold font

  int                 m_x, m_y;
  long                m_height, m_width;
  int                 m_xCross, m_yCross;
  int                 m_level;
  wxArrayGenericTreeItems m_children;
  wxGenericTreeItem  *m_parent;
};

// =============================================================================
// implementation
// =============================================================================

#define PIXELS_PER_UNIT 10
// -----------------------------------------------------------------------------
// wxTreeEvent
// -----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxNotifyEvent)

wxTreeEvent::wxTreeEvent( wxEventType commandType, int id )
           : wxNotifyEvent( commandType, id )
{
  m_code = 0;
  m_itemOld = (wxGenericTreeItem *)NULL;
}

// -----------------------------------------------------------------------------
// wxGenericTreeItem
// -----------------------------------------------------------------------------

wxGenericTreeItem::wxGenericTreeItem(wxGenericTreeItem *parent,
                                     const wxString& text,
                                     wxDC& dc,
                                     int image, int selImage,
                                     wxTreeItemData *data)
                 : m_text(text)
{
  m_image = image;
  m_selImage = selImage;
  m_data = data;
  m_x = m_y = 0;
  m_xCross = m_yCross = 0;

  m_level = 0;

  m_isCollapsed = TRUE;
  m_hasHilight = FALSE;
  m_hasPlus = FALSE;
  m_isBold = FALSE;

  m_parent = parent;

  dc.GetTextExtent( m_text, &m_width, &m_height );
  // TODO : Add the width of the image
  // PB   : We don't know which image is shown (image, selImage)
  //        We don't even know imageList from the treectrl this item belongs to !!!
  // At this point m_width doesn't mean much, this can be remove !
}

wxGenericTreeItem::~wxGenericTreeItem()
{
  delete m_data;

  wxASSERT_MSG( m_children.IsEmpty(),
                _T("please call DeleteChildren() before deleting the item") );
}

void wxGenericTreeItem::DeleteChildren(wxTreeCtrl *tree)
{
  size_t count = m_children.Count();
  for ( size_t n = 0; n < count; n++ )
  {
    wxGenericTreeItem *child = m_children[n];
    if ( tree )
    {
      tree->SendDeleteEvent(child);
    }

    child->DeleteChildren(tree);
    delete child;
  }

  m_children.Empty();
}

void wxGenericTreeItem::SetText( const wxString &text )
{
  m_text = text;
}

void wxGenericTreeItem::Reset()
{
  m_text.Empty();
  m_image =
  m_selImage = -1;
  m_data = NULL;
  m_x = m_y =
  m_height = m_width = 0;
  m_xCross =
  m_yCross = 0;

  m_level = 0;

  DeleteChildren();
  m_isCollapsed = TRUE;

  m_parent = (wxGenericTreeItem *)NULL;
}

size_t wxGenericTreeItem::GetChildrenCount(bool recursively) const
{
  size_t count = m_children.Count();
  if ( !recursively )
    return count;

  size_t total = count;
  for ( size_t n = 0; n < count; ++n )
  {
    total += m_children[n]->GetChildrenCount();
  }

  return total;
}

void wxGenericTreeItem::SetCross( int x, int y )
{
  m_xCross = x;
  m_yCross = y;
}

void wxGenericTreeItem::GetSize( int &x, int &y, const wxTreeCtrl *theTree )
{
  int bottomY=m_y+theTree->GetLineHeight(this);
  if ( y < bottomY ) y = bottomY;
  int width = m_x +  m_width;
  if ( x < width ) x = width;

  if (IsExpanded())
  {
    size_t count = m_children.Count();
    for ( size_t n = 0; n < count; ++n )
    {
      m_children[n]->GetSize( x, y, theTree );
    }
  }
}

wxGenericTreeItem *wxGenericTreeItem::HitTest( const wxPoint& point,
					       const wxTreeCtrl *theTree,
					       int &flags)
{
  if ((point.y > m_y) && (point.y < m_y + theTree->GetLineHeight(this)))
  {
    if (point.y<m_y+theTree->GetLineHeight(this)/2) flags|=wxTREE_HITTEST_ONITEMUPPERPART;
    else flags|=wxTREE_HITTEST_ONITEMLOWERPART;

    // FIXME why +5?
    //  Because that is the size of the plus sign, RR
    if ((point.x > m_xCross-5) && (point.x < m_xCross+5) &&
        (point.y > m_yCross-5) && (point.y < m_yCross+5) &&
        (IsExpanded() || HasPlus()))
    {
      flags|=wxTREE_HITTEST_ONITEMBUTTON;
      return this;
    }

    if ((point.x >= m_x) && (point.x <= m_x+m_width))
    {
      int image_w,image_h;
      
      // assuming every image (normal and selected ) has the same size !
      theTree->m_imageListNormal->GetSize(m_image, image_w, image_h);
      if (point.x<=m_x+image_w+1)
	flags|=wxTREE_HITTEST_ONITEMICON;
      else 
	flags|=wxTREE_HITTEST_ONITEMLABEL;

      return this;
    }

    if (point.x < m_x)         flags|=wxTREE_HITTEST_ONITEMIDENT;
    if (point.x > m_x+m_width) flags|=wxTREE_HITTEST_ONITEMRIGHT;

    return this;
  }
  else
  {
    if (!m_isCollapsed)
    {
      size_t count = m_children.Count();
      for ( size_t n = 0; n < count; n++ )
      {
        wxGenericTreeItem *res = m_children[n]->HitTest( point, theTree, flags );
        if ( res != NULL )
          return res;
      }
    }
  }

  flags|=wxTREE_HITTEST_NOWHERE;
  return NULL;
}

// -----------------------------------------------------------------------------
// wxTreeCtrl implementation
// -----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxTreeCtrl,wxScrolledWindow)
  EVT_PAINT          (wxTreeCtrl::OnPaint)
  EVT_MOUSE_EVENTS   (wxTreeCtrl::OnMouse)
  EVT_CHAR           (wxTreeCtrl::OnChar)
  EVT_SET_FOCUS      (wxTreeCtrl::OnSetFocus)
  EVT_KILL_FOCUS     (wxTreeCtrl::OnKillFocus)
  EVT_IDLE           (wxTreeCtrl::OnIdle)
END_EVENT_TABLE()

// -----------------------------------------------------------------------------
// construction/destruction
// -----------------------------------------------------------------------------

void wxTreeCtrl::Init()
{
  m_current = 
  m_key_current =
  m_anchor = (wxGenericTreeItem *) NULL;
  m_hasFocus = FALSE;
  m_dirty = FALSE;

  m_xScroll = 0;
  m_yScroll = 0;
  m_lineHeight = 10;
  m_indent = 15;
  m_spacing = 18;

  m_hilightBrush = new wxBrush
    (
      wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT),
      wxSOLID
    );

  m_imageListNormal =
  m_imageListState = (wxImageList *) NULL;

  m_dragCount = 0;
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style,
            const wxValidator &validator,
            const wxString& name )
{
  Init();

  wxScrolledWindow::Create( parent, id, pos, size, style|wxHSCROLL|wxVSCROLL, name );

  SetValidator( validator );

  SetBackgroundColour( *wxWHITE );
  m_dottedPen = wxPen( *wxBLACK, 0, 0 );

  return TRUE;
}

wxTreeCtrl::~wxTreeCtrl()
{
  wxDELETE( m_hilightBrush );

  DeleteAllItems();
}

// -----------------------------------------------------------------------------
// accessors
// -----------------------------------------------------------------------------

size_t wxTreeCtrl::GetCount() const
{
  return m_anchor == NULL ? 0u : m_anchor->GetChildrenCount();
}

void wxTreeCtrl::SetIndent(unsigned int indent)
{
  m_indent = indent;
  m_dirty = TRUE;
  Refresh();
}

void wxTreeCtrl::SetSpacing(unsigned int spacing)
{
  m_spacing = spacing;
  m_dirty = TRUE;
  Refresh();
}

size_t wxTreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively)
{
  wxCHECK_MSG( item.IsOk(), 0u, _T("invalid tree item") );

  return item.m_pItem->GetChildrenCount(recursively);
}

// -----------------------------------------------------------------------------
// functions to work with tree items
// -----------------------------------------------------------------------------

wxString wxTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), _T(""), _T("invalid tree item") );

  return item.m_pItem->GetText();
}

int wxTreeCtrl::GetItemImage(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), -1, _T("invalid tree item") );

  return item.m_pItem->GetImage();
}

int wxTreeCtrl::GetItemSelectedImage(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), -1, _T("invalid tree item") );

  return item.m_pItem->GetSelectedImage();
}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), NULL, _T("invalid tree item") );

  return item.m_pItem->GetData();
}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
  wxCHECK_RET( item.IsOk(), _T("invalid tree item") );

  wxClientDC dc(this);
  wxGenericTreeItem *pItem = item.m_pItem;
  pItem->SetText(text);
  CalculateSize(pItem, dc);
  RefreshLine(pItem);
}

void wxTreeCtrl::SetItemImage(const wxTreeItemId& item, int image)
{
  wxCHECK_RET( item.IsOk(), _T("invalid tree item") );

  wxClientDC dc(this);
  wxGenericTreeItem *pItem = item.m_pItem;
  pItem->SetImage(image);
  CalculateSize(pItem, dc);
  RefreshLine(pItem);
}

void wxTreeCtrl::SetItemSelectedImage(const wxTreeItemId& item, int image)
{
  wxCHECK_RET( item.IsOk(), _T("invalid tree item") );

  wxClientDC dc(this);
  wxGenericTreeItem *pItem = item.m_pItem;
  pItem->SetSelectedImage(image);
  CalculateSize(pItem, dc);
  RefreshLine(pItem);
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
  wxCHECK_RET( item.IsOk(), _T("invalid tree item") );

  item.m_pItem->SetData(data);
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
  wxCHECK_RET( item.IsOk(), _T("invalid tree item") );

  wxGenericTreeItem *pItem = item.m_pItem;
  pItem->SetHasPlus(has);
  RefreshLine(pItem);
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
  wxCHECK_RET( item.IsOk(), _T("invalid tree item") );

  // avoid redrawing the tree if no real change
  wxGenericTreeItem *pItem = item.m_pItem;
  if ( pItem->IsBold() != bold )
  {
    pItem->SetBold(bold);
    RefreshLine(pItem);
  }
}

// -----------------------------------------------------------------------------
// item status inquiries
// -----------------------------------------------------------------------------

bool wxTreeCtrl::IsVisible(const wxTreeItemId& WXUNUSED(item)) const
{
  wxFAIL_MSG(_T("not implemented"));

  return TRUE;
}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), FALSE, _T("invalid tree item") );

  return !item.m_pItem->GetChildren().IsEmpty();
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), FALSE, _T("invalid tree item") );

  return item.m_pItem->IsExpanded();
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), FALSE, _T("invalid tree item") );

  return item.m_pItem->HasHilight();
}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), FALSE, _T("invalid tree item") );

  return item.m_pItem->IsBold();
}

// -----------------------------------------------------------------------------
// navigation
// -----------------------------------------------------------------------------

wxTreeItemId wxTreeCtrl::GetParent(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), _T("invalid tree item") );

  return item.m_pItem->GetParent();
}

wxTreeItemId wxTreeCtrl::GetFirstChild(const wxTreeItemId& item, long& cookie) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), _T("invalid tree item") );

  cookie = 0;
  return GetNextChild(item, cookie);
}

wxTreeItemId wxTreeCtrl::GetNextChild(const wxTreeItemId& item, long& cookie) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), _T("invalid tree item") );

  wxArrayGenericTreeItems& children = item.m_pItem->GetChildren();
  if ( (size_t)cookie < children.Count() )
  {
    return children.Item(cookie++);
  }
  else
  {
    // there are no more of them
    return wxTreeItemId();
  }
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), _T("invalid tree item") );

  wxArrayGenericTreeItems& children = item.m_pItem->GetChildren();
  return (children.IsEmpty() ? wxTreeItemId() : wxTreeItemId(children.Last()));
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), _T("invalid tree item") );

  wxGenericTreeItem *i = item.m_pItem;
  wxGenericTreeItem *parent = i->GetParent();
  if ( parent == NULL )
  {
    // root item doesn't have any siblings
    return wxTreeItemId();
  }

  wxArrayGenericTreeItems& siblings = parent->GetChildren();
  int index = siblings.Index(i);
  wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

  size_t n = (size_t)(index + 1);
  return n == siblings.Count() ? wxTreeItemId() : wxTreeItemId(siblings[n]);
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), _T("invalid tree item") );

  wxGenericTreeItem *i = item.m_pItem;
  wxGenericTreeItem *parent = i->GetParent();
  if ( parent == NULL )
  {
    // root item doesn't have any siblings
    return wxTreeItemId();
  }

  wxArrayGenericTreeItems& siblings = parent->GetChildren();
  int index = siblings.Index(i);
  wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

  return index == 0 ? wxTreeItemId()
                    : wxTreeItemId(siblings[(size_t)(index - 1)]);
}

wxTreeItemId wxTreeCtrl::GetFirstVisibleItem() const
{
  wxFAIL_MSG(_T("not implemented"));

  return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), _T("invalid tree item") );

  wxFAIL_MSG(_T("not implemented"));

  return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), _T("invalid tree item") );

  wxFAIL_MSG(_T("not implemented"));

  return wxTreeItemId();
}

// -----------------------------------------------------------------------------
// operations
// -----------------------------------------------------------------------------

wxTreeItemId wxTreeCtrl::DoInsertItem(const wxTreeItemId& parentId,
                                      size_t previous,
                                      const wxString& text,
                                      int image, int selImage,
                                      wxTreeItemData *data)
{
  wxGenericTreeItem *parent = parentId.m_pItem;
  if ( !parent )
  {
    // should we give a warning here?
    return AddRoot(text, image, selImage, data);
  }

  wxClientDC dc(this);
  wxGenericTreeItem *item = new wxGenericTreeItem(parent,
                                                  text, dc,
                                                  image, selImage,
                                                  data);

  if ( data != NULL )
  {
    data->m_pItem = item;
  }

  parent->Insert( item, previous );

  m_dirty = TRUE;

  return item;
}

wxTreeItemId wxTreeCtrl::AddRoot(const wxString& text,
                                 int image, int selImage,
                                 wxTreeItemData *data)
{
  wxCHECK_MSG( !m_anchor, wxTreeItemId(), _T("tree can have only one root") );

  wxClientDC dc(this);
  m_anchor = new wxGenericTreeItem((wxGenericTreeItem *)NULL, text, dc,
                                   image, selImage, data);
  if ( data != NULL )
  {
    data->m_pItem = m_anchor;
  }

  Refresh();
  AdjustMyScrollbars();

  return m_anchor;
}

wxTreeItemId wxTreeCtrl::PrependItem(const wxTreeItemId& parent,
                                     const wxString& text,
                                     int image, int selImage,
                                     wxTreeItemData *data)
{
  return DoInsertItem(parent, 0u, text, image, selImage, data);
}

wxTreeItemId wxTreeCtrl::InsertItem(const wxTreeItemId& parentId,
                                    const wxTreeItemId& idPrevious,
                                    const wxString& text,
                                    int image, int selImage,
                                    wxTreeItemData *data)
{
  wxGenericTreeItem *parent = parentId.m_pItem;
  if ( !parent )
  {
    // should we give a warning here?
    return AddRoot(text, image, selImage, data);
  }

  int index = parent->GetChildren().Index(idPrevious.m_pItem);
  wxASSERT_MSG( index != wxNOT_FOUND,
                _T("previous item in wxTreeCtrl::InsertItem() is not a sibling") );
  return DoInsertItem(parentId, (size_t)++index, text, image, selImage, data);
}

wxTreeItemId wxTreeCtrl::AppendItem(const wxTreeItemId& parentId,
                                    const wxString& text,
                                    int image, int selImage,
                                    wxTreeItemData *data)
{
  wxGenericTreeItem *parent = parentId.m_pItem;
  if ( !parent )
  {
    // should we give a warning here?
    return AddRoot(text, image, selImage, data);
  }

  return DoInsertItem(parent, parent->GetChildren().Count(), text,
                      image, selImage, data);
}

void wxTreeCtrl::SendDeleteEvent(wxGenericTreeItem *item)
{
  wxTreeEvent event( wxEVT_COMMAND_TREE_DELETE_ITEM, GetId() );
  event.m_item = item;
  event.SetEventObject( this );
  ProcessEvent( event );
}

void wxTreeCtrl::DeleteChildren(const wxTreeItemId& itemId)
{
    wxGenericTreeItem *item = itemId.m_pItem;
    item->DeleteChildren(this);

    m_dirty = TRUE;
}

void wxTreeCtrl::Delete(const wxTreeItemId& itemId)
{
  wxGenericTreeItem *item = itemId.m_pItem;
  wxGenericTreeItem *parent = item->GetParent();

  if ( parent )
  {
    parent->GetChildren().Remove(item);
  }

  item->DeleteChildren(this);
  SendDeleteEvent(item);
  delete item;

  m_dirty = TRUE;
}

void wxTreeCtrl::DeleteAllItems()
{
  if ( m_anchor )
  {
    m_anchor->DeleteChildren(this);
    delete m_anchor;

    m_anchor = NULL;

    m_dirty = TRUE;
  }
}

void wxTreeCtrl::Expand(const wxTreeItemId& itemId)
{
  wxGenericTreeItem *item = itemId.m_pItem;

  if ( !item->HasPlus() )
    return;

  if ( item->IsExpanded() )
    return;

  wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_EXPANDING, GetId() );
  event.m_item = item;
  event.SetEventObject( this );
  if ( ProcessEvent( event ) && event.m_code )
  {
    // cancelled by program
    return;
  }

  item->Expand();
  CalculatePositions();

  RefreshSubtree(item);

  event.SetEventType(wxEVT_COMMAND_TREE_ITEM_EXPANDED);
  ProcessEvent( event );
}

void wxTreeCtrl::Collapse(const wxTreeItemId& itemId)
{
  wxGenericTreeItem *item = itemId.m_pItem;

  if ( !item->IsExpanded() )
    return;

  wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_COLLAPSING, GetId() );
  event.m_item = item;
  event.SetEventObject( this );
  if ( ProcessEvent( event ) && event.m_code )
  {
    // cancelled by program
    return;
  }

  item->Collapse();

  wxArrayGenericTreeItems& children = item->GetChildren();
  size_t count = children.Count();
  for ( size_t n = 0; n < count; n++ )
  {
    Collapse(children[n]);
  }

  CalculatePositions();

  RefreshSubtree(item);

  event.SetEventType(wxEVT_COMMAND_TREE_ITEM_COLLAPSED);
  ProcessEvent( event );
}

void wxTreeCtrl::CollapseAndReset(const wxTreeItemId& item)
{
  Collapse(item);
  DeleteChildren(item);
}

void wxTreeCtrl::Toggle(const wxTreeItemId& itemId)
{
  wxGenericTreeItem *item = itemId.m_pItem;

  if ( item->IsExpanded() )
    Collapse(itemId);
  else
    Expand(itemId);
}

void wxTreeCtrl::Unselect()
{
  if ( m_current )
  {
    m_current->SetHilight( FALSE );
    RefreshLine( m_current );
  }
}

void wxTreeCtrl::UnselectAllChildren(wxGenericTreeItem *item)
{
  item->SetHilight(FALSE);
  RefreshLine(item);
  
  if (item->HasChildren())
    {
      wxArrayGenericTreeItems& children = item->GetChildren();
      size_t count = children.Count();
      for ( size_t n = 0; n < count; ++n )
	UnselectAllChildren(children[n]);
    }
}

void wxTreeCtrl::UnselectAll()
{
  UnselectAllChildren(GetRootItem().m_pItem);
}

// Recursive function !
// To stop we must have crt_item<last_item
// Algorithm :
// Tag all next children, when no more children,
// Move to parent (not to tag) 
// Keep going... if we found last_item, we stop.
bool wxTreeCtrl::TagNextChildren(wxGenericTreeItem *crt_item, wxGenericTreeItem *last_item, bool select)
{
    wxGenericTreeItem *parent = crt_item->GetParent();

    if ( parent == NULL ) // This is root item
      return TagAllChildrenUntilLast(crt_item, last_item, select);

    wxArrayGenericTreeItems& children = parent->GetChildren();
    int index = children.Index(crt_item);
    wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

    size_t count = children.Count();
    for (size_t n=(size_t)(index+1); n<count; ++n)
      if (TagAllChildrenUntilLast(children[n], last_item, select)) return TRUE;

    return TagNextChildren(parent, last_item, select);
}

bool wxTreeCtrl::TagAllChildrenUntilLast(wxGenericTreeItem *crt_item, wxGenericTreeItem *last_item, bool select)
{
  crt_item->SetHilight(select);
  RefreshLine(crt_item);
  
  if (crt_item==last_item) return TRUE;

  if (crt_item->HasChildren())
    {
      wxArrayGenericTreeItems& children = crt_item->GetChildren();
      size_t count = children.Count();
      for ( size_t n = 0; n < count; ++n )
	if (TagAllChildrenUntilLast(children[n], last_item, select)) return TRUE;
    }
	
  return FALSE;
}

void wxTreeCtrl::SelectItemRange(wxGenericTreeItem *item1, wxGenericTreeItem *item2)
{
  // item2 is not necessary after item1
  wxGenericTreeItem *first=NULL, *last=NULL;

  // choice first' and 'last' between item1 and item2
  if (item1->GetY()<item2->GetY()) 
    {
      first=item1;
      last=item2;
    }
  else 
    {
      first=item2;
      last=item1;
    }

  bool select=m_current->HasHilight();
  
  if (TagAllChildrenUntilLast(first,last,select)) return;

  TagNextChildren(first,last,select);  
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& itemId, 
			    bool unselect_others, 
			    bool extended_select)
{ 
    bool is_single=!(GetWindowStyleFlag() & wxTR_MULTIPLE);

    //wxCHECK_RET( ( (!unselect_others) && is_single),
    //           _T("this is a single selection tree") );

    // to keep going anyhow !!!
    if (is_single) 
    {
        unselect_others=TRUE;
        extended_select=FALSE;
    }

    wxGenericTreeItem *item = itemId.m_pItem;
    
    wxTreeEvent event( wxEVT_COMMAND_TREE_SEL_CHANGING, GetId() );
    event.m_item = item;
    event.m_itemOld = m_current;
    event.SetEventObject( this );
    // TODO : Here we don't send any selection mode yet !
    
    if ( GetEventHandler()->ProcessEvent( event ) && event.WasVetoed() )
      return;

    // ctrl press
    if (unselect_others)
    {
        if (is_single) Unselect(); // to speed up thing
	else UnselectAll();
    }

    // shift press
    if (extended_select) 
    {
        if (m_current == NULL) m_current=m_key_current=GetRootItem().m_pItem;
        // don't change the mark (m_current)
        SelectItemRange(m_current, item);
    }
    else
    {
        bool select=TRUE; // the default

	// Check if we need to toggle hilight (ctrl mode)
	if (!unselect_others)
	  select=!item->HasHilight();

        m_current = m_key_current = item;
	m_current->SetHilight(select);
	RefreshLine( m_current );
    }

    event.SetEventType(wxEVT_COMMAND_TREE_SEL_CHANGED);
    GetEventHandler()->ProcessEvent( event );
}

void wxTreeCtrl::FillArray(wxGenericTreeItem *item, wxArrayTreeItemIds &array) const
{
  if (item->HasHilight()) array.Add(wxTreeItemId(item));

  if (item->HasChildren())
    {
      wxArrayGenericTreeItems& children = item->GetChildren();
      size_t count = children.Count();
      for ( size_t n = 0; n < count; ++n )
	FillArray(children[n],array);
    }
}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds &array) const
{
  array.Empty();
  FillArray(GetRootItem().m_pItem, array);

  return array.Count();
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{  
    if (!item.IsOk()) return;

    wxGenericTreeItem *gitem = item.m_pItem;

    // first expand all parent branches
    wxGenericTreeItem *parent = gitem->GetParent();
    while ( parent && !parent->IsExpanded() )
    {
        Expand(parent);

        parent = parent->GetParent();
    }

    if (parent) CalculatePositions();

    ScrollTo(item);
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId &item)
{
    if (!item.IsOk()) return;

    wxGenericTreeItem *gitem = item.m_pItem;

    // now scroll to the item
    int item_y = gitem->GetY();

    int start_x = 0;
    int start_y = 0;
    ViewStart( &start_x, &start_y );
    start_y *= PIXELS_PER_UNIT;

    int client_h = 0;
    int client_w = 0;
    GetClientSize( &client_w, &client_h );

    if (item_y < start_y+3)
    {
        // going down
        int x = 0;
        int y = 0;
        m_anchor->GetSize( x, y, this );
        y += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        int x_pos = GetScrollPos( wxHORIZONTAL );
	// Item should appear at top
        SetScrollbars( PIXELS_PER_UNIT, PIXELS_PER_UNIT, x/PIXELS_PER_UNIT, y/PIXELS_PER_UNIT, x_pos, item_y/PIXELS_PER_UNIT );
    }
    else if (item_y+GetLineHeight(gitem) > start_y+client_h)
    {
       // going up
       int x = 0;
       int y = 0;
       m_anchor->GetSize( x, y, this );
       y += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
       item_y += PIXELS_PER_UNIT+2;
       int x_pos = GetScrollPos( wxHORIZONTAL );
	// Item should appear at bottom
       SetScrollbars( PIXELS_PER_UNIT, PIXELS_PER_UNIT, x/PIXELS_PER_UNIT, y/PIXELS_PER_UNIT, x_pos, (item_y+GetLineHeight(gitem)-client_h)/PIXELS_PER_UNIT );
    }
}

wxTextCtrl *wxTreeCtrl::EditLabel( const wxTreeItemId& WXUNUSED(item),
                                   wxClassInfo* WXUNUSED(textCtrlClass) )
{
    wxFAIL_MSG(_T("not implemented"));

    return (wxTextCtrl*)NULL;
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
    wxFAIL_MSG(_T("not implemented"));

    return (wxTextCtrl*)NULL;
}

void wxTreeCtrl::EndEditLabel(const wxTreeItemId& WXUNUSED(item), bool WXUNUSED(discardChanges))
{
    wxFAIL_MSG(_T("not implemented"));
}

// FIXME: tree sorting functions are not reentrant and not MT-safe!
static wxTreeCtrl *s_treeBeingSorted = NULL;

static int tree_ctrl_compare_func(wxGenericTreeItem **item1,
                                  wxGenericTreeItem **item2)
{
    wxCHECK_MSG( s_treeBeingSorted, 0, _T("bug in wxTreeCtrl::SortChildren()") );

    return s_treeBeingSorted->OnCompareItems(*item1, *item2);
}

int wxTreeCtrl::OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2)
{
    return wxStrcmp(GetItemText(item1), GetItemText(item2));
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& itemId)
{
    wxCHECK_RET( itemId.IsOk(), _T("invalid tree item") );

    wxGenericTreeItem *item = itemId.m_pItem;

    wxCHECK_RET( !s_treeBeingSorted,
                 _T("wxTreeCtrl::SortChildren is not reentrant") );

    wxArrayGenericTreeItems& children = item->GetChildren();
    if ( children.Count() > 1 )
    {
        s_treeBeingSorted = this;
        children.Sort(tree_ctrl_compare_func);
        s_treeBeingSorted = NULL;

        m_dirty = TRUE;
    }
    //else: don't make the tree dirty as nothing changed
}

wxImageList *wxTreeCtrl::GetImageList() const
{
    return m_imageListNormal;
}

wxImageList *wxTreeCtrl::GetStateImageList() const
{
    return m_imageListState;
}

void wxTreeCtrl::SetImageList(wxImageList *imageList)
{
   m_imageListNormal = imageList;

   // Calculate a m_lineHeight value from the image sizes.
   // May be toggle off. Then wxTreeCtrl will spread when
   // necessary (which might look ugly).
#if 1
   wxPaintDC dc(this);
   m_lineHeight = (int)(dc.GetCharHeight() + 4);
   int
      width = 0,
      height = 0,
      n = m_imageListNormal->GetImageCount();
   for(int i = 0; i < n ; i++)
   {
      m_imageListNormal->GetSize(i, width, height);
      if(height > m_lineHeight) m_lineHeight = height;
   }  

   if (m_lineHeight<40) m_lineHeight+=4; // at least 4 pixels (odd such that a line can be drawn in between)
   else m_lineHeight+=m_lineHeight/10;   // otherwise 10% extra spacing

#endif
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    m_imageListState = imageList;
}

// -----------------------------------------------------------------------------
// helpers
// -----------------------------------------------------------------------------

void wxTreeCtrl::AdjustMyScrollbars()
{
    if (m_anchor)
    {
        int x = 0;
        int y = 0;
        m_anchor->GetSize( x, y, this );
        //y += GetLineHeight(m_anchor);
	y += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        int x_pos = GetScrollPos( wxHORIZONTAL );
        int y_pos = GetScrollPos( wxVERTICAL );
        SetScrollbars( PIXELS_PER_UNIT, PIXELS_PER_UNIT, x/PIXELS_PER_UNIT, y/PIXELS_PER_UNIT, x_pos, y_pos );
    }
    else
    {
        SetScrollbars( 0, 0, 0, 0 );
    }
}

int wxTreeCtrl::GetLineHeight(wxGenericTreeItem *item) const
{
  if (GetWindowStyleFlag() & wxTR_HAS_VARIABLE_ROW_HIGHT)
    return item->GetHeight();
  else
    return m_lineHeight;  
}

void wxTreeCtrl::PaintItem(wxGenericTreeItem *item, wxDC& dc)
{
    // render bold items in bold
    wxFont fontOld;
    wxFont fontNew;

    if (item->IsBold())
    {
        fontOld = dc.GetFont();
        if (fontOld.Ok())
        {
          // VZ: is there any better way to make a bold variant of old font?
          fontNew = wxFont( fontOld.GetPointSize(),
                            fontOld.GetFamily(),
                            fontOld.GetStyle(),
                            wxBOLD,
                            fontOld.GetUnderlined());
          dc.SetFont(fontNew);
        }
        else
        {
            wxFAIL_MSG(_T("wxDC::GetFont() failed!"));
        }
    }

    long text_w = 0;
    long text_h = 0;
    dc.GetTextExtent( item->GetText(), &text_w, &text_h );

    int image_h = 0;
    int image_w = 0;
    if ((item->IsExpanded()) && (item->GetSelectedImage() != -1))
    {
        m_imageListNormal->GetSize( item->GetSelectedImage(), image_w, image_h );
        image_w += 4;
    }
    else if (item->GetImage() != -1)
    {
        m_imageListNormal->GetSize( item->GetImage(), image_w, image_h );
        image_w += 4;
    }

    int total_h = GetLineHeight(item);

    dc.DrawRectangle( item->GetX()-2, item->GetY(), item->GetWidth()+2, total_h );

    if ((item->IsExpanded()) && (item->GetSelectedImage() != -1))
    {
        dc.SetClippingRegion( item->GetX(), item->GetY(), image_w-2, total_h );
        m_imageListNormal->Draw( item->GetSelectedImage(), dc,
                                 item->GetX(),
                                 item->GetY() +((total_h > image_h)?((total_h-image_h)/2):0),
                                 wxIMAGELIST_DRAW_TRANSPARENT );
        dc.DestroyClippingRegion();
    }
    else if (item->GetImage() != -1)
    {
        dc.SetClippingRegion( item->GetX(), item->GetY(), image_w-2, total_h );
        m_imageListNormal->Draw( item->GetImage(), dc,
                                 item->GetX(),
                                 item->GetY() +((total_h > image_h)?((total_h-image_h)/2):0),
                                 wxIMAGELIST_DRAW_TRANSPARENT );
        dc.DestroyClippingRegion();
    }

    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.DrawText( item->GetText(), image_w + item->GetX(), item->GetY()
                 + ((total_h > text_h) ? (total_h - text_h)/2 : 0));

    // restore normal font for bold items
    if (fontOld.Ok())
    {
        dc.SetFont( fontOld);
    }
}

// Now y stands for the top of the item, whereas it used to stand for middle !
void wxTreeCtrl::PaintLevel( wxGenericTreeItem *item, wxDC &dc, int level, int &y )
{
    int horizX = level*m_indent;

    item->SetX( horizX+m_indent+m_spacing );
    item->SetY( y );

    int oldY = y;
    y+=GetLineHeight(item)/2;

    item->SetCross( horizX+m_indent, y );

    int exposed_x = dc.LogicalToDeviceX( 0 );
    int exposed_y = dc.LogicalToDeviceY( item->GetY()-2 );

    if (IsExposed( exposed_x, exposed_y, 10000, GetLineHeight(item)+4 ))  // 10000 = very much
    {
        int startX = horizX;
        int endX = horizX + (m_indent-5);

//        if (!item->HasChildren()) endX += (m_indent+5);
        if (!item->HasChildren()) endX += 20;

        dc.DrawLine( startX, y, endX, y );

        if (item->HasPlus())
        {
            dc.DrawLine( horizX+(m_indent+5), y, horizX+(m_indent+15), y );
            dc.SetPen( *wxGREY_PEN );
            dc.SetBrush( *wxWHITE_BRUSH );
            dc.DrawRectangle( horizX+(m_indent-5), y-4, 11, 9 );
            dc.SetPen( *wxBLACK_PEN );
            dc.DrawLine( horizX+(m_indent-2), y, horizX+(m_indent+3), y );

            if (!item->IsExpanded())
            {
                dc.DrawLine( horizX+m_indent, y-2, horizX+m_indent, y+3 );
            }
        }

        if (item->HasHilight())
        {
            dc.SetTextForeground( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );

            dc.SetBrush( *m_hilightBrush );

            if (m_hasFocus)
                dc.SetPen( *wxBLACK_PEN );
            else
                dc.SetPen( *wxTRANSPARENT_PEN );

            PaintItem(item, dc);

            dc.SetPen( *wxBLACK_PEN );
            dc.SetTextForeground( *wxBLACK );
            dc.SetBrush( *wxWHITE_BRUSH );
        }
        else
        {
            dc.SetBrush( *wxWHITE_BRUSH );
            dc.SetPen( *wxTRANSPARENT_PEN );

            PaintItem(item, dc);

            dc.SetPen( *wxBLACK_PEN );
        }
    }
    
    y = oldY+GetLineHeight(item);

    if (item->IsExpanded())
    {
        oldY+=GetLineHeight(item)/2;
        int semiOldY=y; // (=y) for stupid compilator

        wxArrayGenericTreeItems& children = item->GetChildren();
        size_t n, count = children.Count();
        for ( n = 0; n < count; ++n )
	{
	    semiOldY=y;
	    PaintLevel( children[n], dc, level+1, y );
	}

        // it may happen that the item is expanded but has no items (when you
        // delete all its children for example) - don't draw the vertical line
        // in this case
        if (count > 0)
	  {
	    semiOldY+=GetLineHeight(children[--n])/2;
            dc.DrawLine( horizX+m_indent, oldY+5, horizX+m_indent, semiOldY );
	  }
    }
}

void wxTreeCtrl::DrawBorder(wxTreeItemId &item)
{
  if (!item) return;

    wxGenericTreeItem *i=item.m_pItem;

    wxPaintDC dc(this);
    PrepareDC( dc );
    dc.SetLogicalFunction(wxINVERT);

    int w,h,x;
    ViewStart(&x,&h);     // we only need x
    GetClientSize(&w,&h); // we only need w

    h=GetLineHeight(i)+1;
    // 2 white column at border
    dc.DrawRectangle( PIXELS_PER_UNIT*x+2, i->GetY()-1, w-6, h);
}

void wxTreeCtrl::DrawLine(wxTreeItemId &item, bool below)
{
  if (!item) return;

    wxGenericTreeItem *i=item.m_pItem;

    wxPaintDC dc(this);
    PrepareDC( dc );
    dc.SetLogicalFunction(wxINVERT);
    
    int w,h,y;
    GetSize(&w,&h);

    if (below) y=i->GetY()+GetLineHeight(i)-1;
    else y=i->GetY();

    dc.DrawLine( 0, y, w, y);
}

// -----------------------------------------------------------------------------
// wxWindows callbacks
// -----------------------------------------------------------------------------

void wxTreeCtrl::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    if ( !m_anchor)
        return;

    wxPaintDC dc(this);
    PrepareDC( dc );

    dc.SetFont( wxSystemSettings::GetSystemFont( wxSYS_DEFAULT_GUI_FONT ) );

    dc.SetPen( m_dottedPen );
    //if(GetImageList() == NULL)
    // m_lineHeight = (int)(dc.GetCharHeight() + 4);

    int y = 2;
    PaintLevel( m_anchor, dc, 0, y );
}

void wxTreeCtrl::OnSetFocus( wxFocusEvent &WXUNUSED(event) )
{
    m_hasFocus = TRUE;

    if (m_current) RefreshLine( m_current );
}

void wxTreeCtrl::OnKillFocus( wxFocusEvent &WXUNUSED(event) )
{
    m_hasFocus = FALSE;

    if (m_current) RefreshLine( m_current );
}

void wxTreeCtrl::OnChar( wxKeyEvent &event )
{
    wxTreeEvent te( wxEVT_COMMAND_TREE_KEY_DOWN, GetId() );
    te.m_code = event.KeyCode();
    te.SetEventObject( this );
    GetEventHandler()->ProcessEvent( te );

    if ( (m_current == 0) || (m_key_current == 0) )
    {
        event.Skip();
        return;
    }

    bool is_multiple=(GetWindowStyleFlag() & wxTR_MULTIPLE);
    bool extended_select=(event.ShiftDown() && is_multiple);
    bool unselect_others=!(extended_select || (event.ControlDown() && is_multiple));

    switch (event.KeyCode())
    {
        case '+':
        case WXK_ADD:
            if (m_current->HasPlus() && !IsExpanded(m_current))
            {
                Expand(m_current);
            }
            break;

        case '-':
        case WXK_SUBTRACT:
            if (IsExpanded(m_current))
            {
                Collapse(m_current);
            }
            break;

        case '*':
        case WXK_MULTIPLY:
            Toggle(m_current);
            break;

        case ' ':
        case WXK_RETURN:
            {
                wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, GetId() );
                event.m_item = m_current;
                event.m_code = 0;
                event.SetEventObject( this );
                GetEventHandler()->ProcessEvent( event );
            }
            break;

            // up goes to the previous sibling or to the last of its children if
            // it's expanded
        case WXK_UP:
            {
                wxTreeItemId prev = GetPrevSibling( m_key_current );
                if (!prev)
                {
                    prev = GetParent( m_key_current );
		    if (prev)
		    {
                        long cockie = 0;
                        wxTreeItemId current = m_key_current;
                        if (current == GetFirstChild( prev, cockie ))
                        {
                            // otherwise we return to where we came from
                            SelectItem( prev, unselect_others, extended_select );
			    m_key_current=prev.m_pItem;
			    EnsureVisible( prev );
                            break;
			}
                    }
                }
                if (prev)
                {
                    while ( IsExpanded(prev) && HasChildren(prev) )
                    {
                        wxTreeItemId child = GetLastChild(prev);
                        if ( child )
                        {
                            prev = child;
                        }
                    }

                    SelectItem( prev, unselect_others, extended_select );
		    m_key_current=prev.m_pItem;
                    EnsureVisible( prev );
                }
            }
            break;

            // left arrow goes to the parent
        case WXK_LEFT:
            {
                wxTreeItemId prev = GetParent( m_current );
                if (prev)
                {
                    EnsureVisible( prev );
                    SelectItem( prev, unselect_others, extended_select );
                }
            }
            break;

        case WXK_RIGHT:
            // this works the same as the down arrow except that we also expand the
            // item if it wasn't expanded yet
            Expand(m_current);
            // fall through

        case WXK_DOWN:
            {
                if (IsExpanded(m_key_current) && HasChildren(m_key_current))
                {
                    long cookie = 0;
                    wxTreeItemId child = GetFirstChild( m_key_current, cookie );
                    SelectItem( child, unselect_others, extended_select );
		    m_key_current=child.m_pItem;
                    EnsureVisible( child );
                }
                else
                {
                    wxTreeItemId next = GetNextSibling( m_key_current );
                    if (next == 0)
                    {
                        wxTreeItemId current = m_key_current;
                        while (current && !next)
                        {
                            current = GetParent( current );
                            if (current) next = GetNextSibling( current );
                        }
                    }
                    if (next != 0)
                    {
                        SelectItem( next, unselect_others, extended_select );
			m_key_current=next.m_pItem;
                        EnsureVisible( next );
                    }
                }
            }
            break;

            // <End> selects the last visible tree item
        case WXK_END:
            {
                wxTreeItemId last = GetRootItem();

                while ( last.IsOk() && IsExpanded(last) )
                {
                    wxTreeItemId lastChild = GetLastChild(last);

                    // it may happen if the item was expanded but then all of
                    // its children have been deleted - so IsExpanded() returned
                    // TRUE, but GetLastChild() returned invalid item
                    if ( !lastChild )
                        break;

                    last = lastChild;
                }

                if ( last.IsOk() )
                {
                    EnsureVisible( last );
                    SelectItem( last, unselect_others, extended_select );
                }
            }
            break;

            // <Home> selects the root item
        case WXK_HOME:
            {
                wxTreeItemId prev = GetRootItem();
                if (prev)
                {
                    EnsureVisible( prev );
                    SelectItem( prev, unselect_others, extended_select );
                }
            }
            break;

        default:
            event.Skip();
    }
}

wxTreeItemId wxTreeCtrl::HitTest(const wxPoint& point, int& flags)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    long x = dc.DeviceToLogicalX( (long)point.x );
    long y = dc.DeviceToLogicalY( (long)point.y );
    int w, h;
    GetSize(&w, &h);

    flags=0;
    if (point.x<0) flags|=wxTREE_HITTEST_TOLEFT;
    if (point.x>w) flags|=wxTREE_HITTEST_TORIGHT;
    if (point.y<0) flags|=wxTREE_HITTEST_ABOVE;
    if (point.y>h) flags|=wxTREE_HITTEST_BELOW;
    
    return m_anchor->HitTest( wxPoint(x, y), this, flags);
}

void wxTreeCtrl::OnMouse( wxMouseEvent &event )
{
    if (!event.LeftIsDown()) m_dragCount = 0;

    if ( !(event.LeftUp() || event.LeftDClick() || event.Dragging()) ) return;

    if ( !m_anchor ) return;

    wxClientDC dc(this);
    PrepareDC(dc);
    long x = dc.DeviceToLogicalX( (long)event.GetX() );
    long y = dc.DeviceToLogicalY( (long)event.GetY() );

    int flags=0;
    wxGenericTreeItem *item = m_anchor->HitTest( wxPoint(x,y), this, flags);
    bool onButton = flags & wxTREE_HITTEST_ONITEMBUTTON;

    if (item == NULL) return;  /* we hit the blank area */

    if (event.Dragging())
    {
        if (m_dragCount == 2)  /* small drag latency (3?) */
        {
            m_dragCount = 0;

            wxTreeEvent nevent(wxEVT_COMMAND_TREE_BEGIN_DRAG, GetId());
            nevent.m_item = m_current;
            nevent.SetEventObject(this);
            GetEventHandler()->ProcessEvent(nevent);
        }
        else
        {
            m_dragCount++;
        }
        return;
    }

    bool is_multiple=(GetWindowStyleFlag() & wxTR_MULTIPLE);
    bool extended_select=(event.ShiftDown() && is_multiple);
    bool unselect_others=!(extended_select || (event.ControlDown() && is_multiple));

    if (onButton)
    {
        Toggle( item );
	if (is_multiple) return;
    }

    SelectItem(item, unselect_others, extended_select);

    if (event.LeftDClick())
    {
        wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, GetId() );
        event.m_item = item;
        event.m_code = 0;
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event );
    }
}

void wxTreeCtrl::OnIdle( wxIdleEvent &WXUNUSED(event) )
{
    /* after all changes have been done to the tree control,
     * we actually redraw the tree when everything is over */

    if (!m_dirty)
        return;

    m_dirty = FALSE;

    CalculatePositions();
    Refresh();
    AdjustMyScrollbars();
}

void wxTreeCtrl::CalculateSize( wxGenericTreeItem *item, wxDC &dc )
{  
    long text_w = 0;
    long text_h = 0;
    // TODO : check for boldness. Here with suppose that font normal and bold
    //        have the same height !
    // TODO : bug here, text_w is sometime not the correct answer !!!
    dc.GetTextExtent( item->GetText(), &text_w, &text_h );
    text_h+=4;

    int image_h = 0;
    int image_w = 0;
    if ((item->IsExpanded()) && (item->GetSelectedImage() != -1))
    {
        m_imageListNormal->GetSize( item->GetSelectedImage(), image_w, image_h );
        image_w += 4;
    }
    else if (item->GetImage() != -1)
    {
        m_imageListNormal->GetSize( item->GetImage(), image_w, image_h );
        image_w += 4;
    }

    int total_h = (image_h > text_h) ? image_h : text_h;

    if (total_h<40) total_h+=4; // at least 4 pixels
    else total_h+=total_h/10;   // otherwise 10% extra spacing

    item->SetHeight(total_h);
    if (total_h>m_lineHeight) m_lineHeight=total_h;

    item->SetWidth(image_w+text_w+2);
}

// -----------------------------------------------------------------------------
// for developper : y is now the top of the level
// not the middle of it !
void wxTreeCtrl::CalculateLevel( wxGenericTreeItem *item, wxDC &dc, int level, int &y )
{
    int horizX = level*m_indent;

    CalculateSize( item, dc );
  
    // set its position
    item->SetX( horizX+m_indent+m_spacing );
    item->SetY( y );
    y+=GetLineHeight(item);

    if ( !item->IsExpanded() )
    {
        // we dont need to calculate collapsed branches
        return;
    }

    wxArrayGenericTreeItems& children = item->GetChildren();
    size_t n, count = children.Count();
    for (n = 0; n < count; ++n )
      CalculateLevel( children[n], dc, level+1, y );  // recurse
}

void wxTreeCtrl::CalculatePositions()
{
    if ( !m_anchor ) return;

    wxClientDC dc(this);
    PrepareDC( dc );

    dc.SetFont( wxSystemSettings::GetSystemFont( wxSYS_DEFAULT_GUI_FONT ) );

    dc.SetPen( m_dottedPen );
    //if(GetImageList() == NULL)
    // m_lineHeight = (int)(dc.GetCharHeight() + 4);

    int y = 2; //GetLineHeight(m_anchor) / 2 + 2;
    CalculateLevel( m_anchor, dc, 0, y ); // start recursion
}

void wxTreeCtrl::RefreshSubtree(wxGenericTreeItem *item)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    int cw = 0;
    int ch = 0;
    GetClientSize( &cw, &ch );

    wxRect rect;
    rect.x = dc.LogicalToDeviceX( 0 );
    rect.width = cw;
    rect.y = dc.LogicalToDeviceY( item->GetY() );
    rect.height = ch;

    Refresh( TRUE, &rect );

    AdjustMyScrollbars();
}

void wxTreeCtrl::RefreshLine( wxGenericTreeItem *item )
{
    wxClientDC dc(this);
    PrepareDC( dc );

    wxRect rect;
    rect.x = dc.LogicalToDeviceX( item->GetX() - 2 );
    rect.y = dc.LogicalToDeviceY( item->GetY());
    rect.width = 1000;
    rect.height = GetLineHeight(item); //dc.GetCharHeight() + 6;

    Refresh( TRUE, &rect );
}

