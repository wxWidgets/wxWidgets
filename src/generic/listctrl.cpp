/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "listctrl.h"
#endif

#include "wx/listctrl.h"

//-----------------------------------------------------------------------------
//  wxListItemData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListItemData,wxObject);

wxListItemData::wxListItemData(void)
{
  m_image = -1;
  m_data = 0;
  m_xpos = 0;
  m_ypos = 0;
  m_width = 0;
  m_height = 0;
  m_colour = wxBLACK;
};

wxListItemData::wxListItemData( const wxListItem &info )
{
  m_image = -1;
  m_data = 0;
  m_colour = info.m_colour;
  SetItem( info );
};

void wxListItemData::SetItem( const wxListItem &info )
{
  if (info.m_mask & wxLIST_MASK_TEXT) m_text = info.m_text;
  if (info.m_mask & wxLIST_MASK_IMAGE) m_image = info.m_image;
  if (info.m_mask & wxLIST_MASK_DATA) m_data = info.m_data;
  m_colour = info.m_colour;
  m_xpos = 0;
  m_ypos = 0;
  m_width = info.m_width;
  m_height = 0;
};

void wxListItemData::SetText( const wxString &s )
{
  m_text = s;
};

void wxListItemData::SetImage( int image )
{
  m_image = image;
};

void wxListItemData::SetData( long data )
{
  m_data = data;
};

void wxListItemData::SetPosition( int x, int y )
{
  m_xpos = x;
  m_ypos = y;
};

void wxListItemData::SetSize( int const width, int height )
{
  m_width = width;
  m_height = height;
};

void wxListItemData::SetColour( wxColour *col )
{
  m_colour = col;
};

bool wxListItemData::HasImage(void) const
{
  return (m_image >= 0);
};

bool wxListItemData::HasText(void) const
{
  return (!m_text.IsNull());
};

bool wxListItemData::IsHit( int x, int y ) const
{
  return ((x >= m_xpos) && (x <= m_xpos+m_width) && (y >= m_ypos) && (y <= m_ypos+m_height));
};

void wxListItemData::GetText( wxString &s )
{
  s = m_text;
};

int wxListItemData::GetX( void ) const
{
  return m_xpos;
};

int wxListItemData::GetY( void ) const
{
  return m_ypos;
};

int wxListItemData::GetWidth(void) const
{
  return m_width;
};

int wxListItemData::GetHeight(void) const
{
  return m_height;
};

int wxListItemData::GetImage(void) const
{
  return m_image;
};

void wxListItemData::GetItem( wxListItem &info )
{
  info.m_text = m_text;
  info.m_image = m_image;
  info.m_data = m_data;
};

wxColour *wxListItemData::GetColour(void)
{
  return m_colour;
};

//-----------------------------------------------------------------------------
//  wxListHeaderData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListHeaderData,wxObject);

wxListHeaderData::wxListHeaderData(void)
{
  m_mask = 0;
  m_image = 0;
  m_format = 0;
  m_width = 0;
  m_xpos = 0;
  m_ypos = 0;
  m_height = 0;
};

wxListHeaderData::wxListHeaderData( const wxListItem &item )
{
  SetItem( item );
  m_xpos = 0;
  m_ypos = 0;
  m_height = 0;
};

void wxListHeaderData::SetItem( const wxListItem &item )
{
  m_mask = item.m_mask;
  m_text = item.m_text;
  m_image = item.m_image;
  m_format = item.m_format;
  m_width = item.m_width;
  if (m_width < 0) m_width = 80;
  if (m_width < 6) m_width = 6;
};

void wxListHeaderData::SetPosition( int x, int y )
{
  m_xpos = x;
  m_ypos = y;
};

void wxListHeaderData::SetHeight( int h )
{
  m_height = h;
};

void wxListHeaderData::SetWidth( int w )
{
  m_width = w;
  if (m_width < 0) m_width = 80;
  if (m_width < 6) m_width = 6;
};

void wxListHeaderData::SetFormat( int format )
{
  m_format = format;
};

bool wxListHeaderData::HasImage(void) const
{
  return (m_image != 0);
};

bool wxListHeaderData::HasText(void) const
{
  return (m_text.Length() > 0);
};

bool wxListHeaderData::IsHit( int x, int y ) const
{
  return ((x >= m_xpos) && (x <= m_xpos+m_width) && (y >= m_ypos) && (y <= m_ypos+m_height));
};

void wxListHeaderData::GetItem( wxListItem &item )
{
  item.m_mask = m_mask;
  item.m_text = m_text;
  item.m_image = m_image;
  item.m_format = m_format;
  item.m_width = m_width;
};

void wxListHeaderData::GetText( wxString &s )
{
  s =  m_text;
};

int wxListHeaderData::GetImage(void) const
{
  return m_image;
};

int wxListHeaderData::GetWidth(void) const
{
  return m_width;
};

int wxListHeaderData::GetFormat(void) const
{
  return m_format;
};

//-----------------------------------------------------------------------------
//  wxListLineData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListLineData,wxObject);

wxListLineData::wxListLineData( wxListMainWindow *owner, int mode, wxBrush *hilightBrush )
{
  m_mode = mode;
  m_hilighted = FALSE;
  m_owner = owner;
  m_hilightBrush = hilightBrush;
  m_items.DeleteContents( TRUE );
  m_spacing = 0;
};

void wxListLineData::CalculateSize( wxPaintDC *dc, int spacing )
{
  m_spacing = spacing;
  switch (m_mode)
  {
    case wxLC_ICON:
    {
      m_bound_all.width = m_spacing;
      m_bound_all.height = m_spacing+13;
      wxNode *node = m_items.First();
      if (node)
      {
        wxListItemData *item = (wxListItemData*)node->Data();
        wxString s;
        item->GetText( s );
        long lw,lh;
        dc->GetTextExtent( s, &lw, &lh );
        if (lw > m_spacing) m_bound_all.width = lw;
      };
      break;
    };
    case wxLC_LIST:
    {
      wxNode *node = m_items.First();
      if (node)
      {
        wxListItemData *item = (wxListItemData*)node->Data();
        wxString s;
        item->GetText( s );
        long lw,lh;
        dc->GetTextExtent( s, &lw, &lh );
        m_bound_all.width = lw;
        m_bound_all.height = lh;
      };
      break;
    };
    case wxLC_REPORT:
    {
      m_bound_all.width = 0;
      m_bound_all.height = 0;
      wxNode *node = m_items.First();
      while (node)
      {
        wxListItemData *item = (wxListItemData*)node->Data();
        wxString s;
        item->GetText( s );
        if (s.IsNull()) s = "H";
        long lw,lh;
        dc->GetTextExtent( s, &lw, &lh );
        item->SetSize( item->GetWidth(), lh );
        m_bound_all.width += lw;
        m_bound_all.height = lh;
        node = node->Next();
      };
      break;
    };
  };
};

void wxListLineData::SetPosition( wxPaintDC *dc, int x, int y, int window_width )
{
  m_bound_all.x = x;
  m_bound_all.y = y;
  switch (m_mode)
  {
    case wxLC_ICON:
    {
      AssignRect( m_bound_icon, 0, 0, 0, 0 );
      AssignRect( m_bound_label, 0, 0, 0, 0 );
      AssignRect( m_bound_hilight, m_bound_all );
      wxNode *node = m_items.First();
      if (node)
      {
        wxListItemData *item = (wxListItemData*)node->Data();
        if (item->HasImage())
        {
          wxListItemData *item = (wxListItemData*)node->Data();
          int w = 0;
          int h = 0;
          m_owner->GetImageSize( item->GetImage(), w, h );
          m_bound_icon.x = m_bound_all.x + (m_spacing/2) - (w/2);
          m_bound_icon.y = m_bound_all.y + m_spacing - h - 5;
          m_bound_icon.width = w;
          m_bound_icon.height = h;
          if (!item->HasText())
          {
            AssignRect( m_bound_hilight, m_bound_icon );
            m_bound_hilight.x -= 3;
            m_bound_hilight.y -= 3;
            m_bound_hilight.width += 7;
            m_bound_hilight.height += 7;
          };
        };
        if (item->HasText())
        {
          wxString s;
          item->GetText( s );
          long lw,lh;
          dc->GetTextExtent( s, &lw, &lh );
          if (m_bound_all.width > m_spacing)
            m_bound_label.x = m_bound_all.x;
          else
            m_bound_label.x = m_bound_all.x +  (m_spacing/2) - lw/2;
          m_bound_label.y = m_bound_all.y + m_bound_all.height - lh;
          m_bound_label.width = lw;
          m_bound_label.height = lh;
          AssignRect( m_bound_hilight, m_bound_label );
        };
      };
      break;
    };
    case wxLC_LIST:
    {
      AssignRect( m_bound_label, m_bound_all );
      AssignRect( m_bound_hilight, m_bound_all );
      AssignRect( m_bound_icon, 0, 0, 0, 0 );
      break;
    };
    case wxLC_REPORT:
    {
      long lw,lh;
      dc->GetTextExtent( "H", &lw, &lh );
      m_bound_all.height = lh;
      m_bound_all.width = window_width;
      AssignRect( m_bound_label, m_bound_all );
      AssignRect( m_bound_hilight, m_bound_all );
      AssignRect( m_bound_icon, 0, 0, 0, 0 );
      m_bound_hilight.width = window_width-10;
      m_bound_label.width = window_width-10;
      break;
    };
  };
};

void wxListLineData::SetColumnPosition( int index, int x )
{
  int i = index;
  wxNode *node = m_items.Nth( i );
  if (node)
  {
    wxListItemData *item = (wxListItemData*)node->Data();
    item->SetPosition( x, m_bound_all.y );
  };  
};

void wxListLineData::GetSize( int &width, int &height )
{
  width = m_bound_all.width;
  height = m_bound_all.height;
};

void wxListLineData::GetExtent( int &x, int &y, int &width, int &height )
{
  x = m_bound_all.x;
  y = m_bound_all.y;
  width = m_bound_all.width;
  height = m_bound_all.height;
};

void wxListLineData::GetLabelExtent( int &x, int &y, int &width, int &height )
{
  x = m_bound_label.x;
  y = m_bound_label.y;
  width = m_bound_label.width;
  height = m_bound_label.height;
};

void wxListLineData::GetRect( wxRectangle &rect )
{
  AssignRect( rect, m_bound_all );
};

long wxListLineData::IsHit( int x, int y )
{
  wxNode *node = m_items.First();
  if (node)
  {
    wxListItemData *item = (wxListItemData*)node->Data();
    if (item->HasImage() && IsInRect( x, y, m_bound_icon )) return wxLIST_HITTEST_ONITEMICON;
    if (item->HasText() && IsInRect( x, y, m_bound_label )) return wxLIST_HITTEST_ONITEMLABEL;
    if (!(item->HasImage() || item->HasText())) return 0;
  };
  // if there is no icon or text = empty
  if (IsInRect( x, y, m_bound_all )) return wxLIST_HITTEST_ONITEMICON;
  return 0;
};

void wxListLineData::InitItems( int num )
{
  for (int i = 0; i < num; i++) m_items.Append( new wxListItemData() );
};

void wxListLineData::SetItem( int index, const wxListItem &info )
{
  wxNode *node = m_items.Nth( index );
  if (node)
  {
    wxListItemData *item = (wxListItemData*)node->Data();
    item->SetItem( info );
  };
};

void wxListLineData::GetItem( int const index, wxListItem &info )
{
  int i = index;
  wxNode *node = m_items.Nth( i );
  if (node)
  {
    wxListItemData *item = (wxListItemData*)node->Data();
    item->GetItem( info );
  };
};

void wxListLineData::GetText( int index, wxString &s )
{
  int i = index;
  wxNode *node = m_items.Nth( i );
  s = "";
  if (node)
  {
    wxListItemData *item = (wxListItemData*)node->Data();
    item->GetText( s );
  };
};

void wxListLineData::SetText( int index, const wxString s )
{
  int i = index;
  wxNode *node = m_items.Nth( i );
  if (node)
  {
    wxListItemData *item = (wxListItemData*)node->Data();
    item->SetText( s );
  };
};

int wxListLineData::GetImage( int index )
{
  int i = index;
  wxNode *node = m_items.Nth( i );
  if (node)
  {
    wxListItemData *item = (wxListItemData*)node->Data();
    return item->GetImage();
  };
  return -1;
};

void wxListLineData::DoDraw( wxPaintDC *dc, bool hilight, bool paintBG )
{
  long dev_x = dc->LogicalToDeviceX( m_bound_all.x-2 );
  long dev_y = dc->LogicalToDeviceY( m_bound_all.y-2 );
  long dev_w = dc->LogicalToDeviceXRel( m_bound_all.width+4 );
  long dev_h = dc->LogicalToDeviceYRel( m_bound_all.height+4 );
  if (!m_owner->IsExposed( dev_x, dev_y, dev_w, dev_h ) ) return;
  
  if (paintBG)
  {
    if (hilight)
    {
      dc->SetBrush( m_hilightBrush );
      dc->SetPen( wxTRANSPARENT_PEN );
    }
    else
    {
      dc->SetBrush( wxWHITE_BRUSH );
      dc->SetPen( wxTRANSPARENT_PEN );
    };
    dc->DrawRectangle( m_bound_hilight.x-2, m_bound_hilight.y-2,
                       m_bound_hilight.width+4, m_bound_hilight.height+4 );
  };
  if (m_mode == wxLC_REPORT)
  {
    wxString s;
    wxNode *node = m_items.First();
    while (node)
    {
      wxListItemData *info = (wxListItemData*)node->Data();
      dc->SetClippingRegion( info->GetX(), info->GetY(), info->GetWidth(), info->GetHeight() );
      info->GetText( s );
	if (hilight)
	  dc->SetTextForeground( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	else
          dc->SetTextForeground( info->GetColour() );
      dc->DrawText( s, info->GetX()+2, info->GetY() );
      dc->DestroyClippingRegion();
      node = node->Next();
    };
  }
  else
  {
    wxNode *node = m_items.First();
    if (node)
    {
      wxListItemData *item = (wxListItemData*)node->Data();
      if (item->HasImage())
      {
        m_owner->DrawImage( item->GetImage(), dc, m_bound_icon.x, m_bound_icon.y );
      };
      if (item->HasText())
      {
        wxString s;
        item->GetText( s );
	if (hilight)
	  dc->SetTextForeground( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	else
          dc->SetTextForeground( item->GetColour() );
        dc->DrawText( s, m_bound_label.x, m_bound_label.y );
      };
    };
  };
};

void wxListLineData::Hilight( bool on )
{
  if (on == m_hilighted) return;
  if (on) 
    m_owner->SelectLine( this );
  else
    m_owner->DeselectLine( this );
  m_hilighted = on;
};

void wxListLineData::ReverseHilight( void )
{
  m_hilighted = !m_hilighted;
  if (m_hilighted) 
    m_owner->SelectLine( this );
  else
    m_owner->DeselectLine( this );
};

void wxListLineData::DrawRubberBand( wxPaintDC *dc, bool on )
{
  if (on)
  {
    dc->SetPen( wxBLACK_PEN );
    dc->SetBrush( wxTRANSPARENT_BRUSH );
    dc->DrawRectangle( m_bound_hilight.x-2, m_bound_hilight.y-2,
                       m_bound_hilight.width+4, m_bound_hilight.height+4 );
  };
};

void wxListLineData::Draw( wxPaintDC *dc )
{
  DoDraw( dc, m_hilighted, m_hilighted );
};

bool wxListLineData::IsInRect( int x, int y, const wxRectangle &rect )
{
  return ((x >= rect.x) && (x <= rect.x+rect.width) && (y >= rect.y) && (y <= rect.y+rect.height));
};

bool wxListLineData::IsHilighted( void )
{
  return m_hilighted;
};

void wxListLineData::AssignRect( wxRectangle &dest, int x, int y, int width, int height )
{
  dest.x = x;
  dest.y = y;
  dest.width = width;
  dest.height = height;
};

void wxListLineData::AssignRect( wxRectangle &dest, const wxRectangle &source )
{
  dest.x = source.x;
  dest.y = source.y;
  dest.width = source.width;
  dest.height = source.height;
};

//-----------------------------------------------------------------------------
//  wxListHeaderWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListHeaderWindow,wxWindow);

BEGIN_EVENT_TABLE(wxListHeaderWindow,wxWindow)
  EVT_PAINT         (wxListHeaderWindow::OnPaint)
  EVT_MOUSE_EVENTS  (wxListHeaderWindow::OnMouse)
  EVT_SET_FOCUS     (wxListHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

wxListHeaderWindow::wxListHeaderWindow( void )
{
  m_owner = NULL;
  m_currentCursor = NULL;
  m_resizeCursor = NULL;
};

wxListHeaderWindow::wxListHeaderWindow( wxWindow *win, wxWindowID id, wxListMainWindow *owner, 
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name ) :
  wxWindow( win, id, pos, size, style, name )
{
  m_owner = owner;
//  m_currentCursor = wxSTANDARD_CURSOR;
  m_currentCursor = NULL;
  m_resizeCursor = new wxCursor( wxCURSOR_SIZEWE );
};

void wxListHeaderWindow::DoDrawRect( wxPaintDC *dc, int x, int y, int w, int h )
{
  const m_corner = 1;

  dc->SetBrush( *wxTRANSPARENT_BRUSH );

  dc->SetPen( *wxBLACK_PEN );
  dc->DrawLine( x+w-m_corner+1, y, x+w, y+h );  // right (outer)
  dc->DrawRectangle( x, y+h, w, 1 );            // bottom (outer)
  
  dc->SetPen( *wxMEDIUM_GREY_PEN );
  dc->DrawLine( x+w-m_corner, y, x+w-1, y+h );  // right (inner)
  dc->DrawRectangle( x+1, y+h-1, w-2, 1 );      // bottom (inner)
  
  dc->SetPen( *wxWHITE_PEN );
  dc->DrawRectangle( x, y, w-m_corner+1, 1 );   // top (outer)
//  dc->DrawRectangle( x, y+1, w-m_corner, 1 );   // top (inner)
  dc->DrawRectangle( x, y, 1, h );              // left (outer)
//  dc->DrawRectangle( x+1, y, 1, h-1 );          // left (inner)
};

void wxListHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  wxPaintDC dc( this );
  PrepareDC( dc );
  
  dc.BeginDrawing();
     
  dc.SetFont( wxSystemSettings::GetSystemFont( wxSYS_SYSTEM_FONT ) );

  int w = 0;
  int h = 0;
  int x = 0;
  int y = 0;
  GetClientSize( &w, &h );

  dc.SetTextForeground( *wxBLACK );

  x = 1;
  y = 1;
  int numColumns = m_owner->GetColumnCount();
  wxListItem item;
  for (int i = 0; i < numColumns; i++)
  {
    m_owner->GetColumn( i, item );
    int cw = item.m_width-2;
    if ((i+1 == numColumns) || (x+item.m_width > w-5)) cw = w-x-1;
    dc.SetPen( *wxWHITE_PEN );
    
    DoDrawRect( &dc, x, y, cw, h-2 );
    dc.SetClippingRegion( x, y, cw-5, h-4 );
    dc.DrawText( item.m_text, x+4, y+3 );
    dc.DestroyClippingRegion();
    x += item.m_width;
    if (x > w+5) break;
  }; 
  dc.EndDrawing();
};

void wxListHeaderWindow::OnMouse( wxMouseEvent &event )
{
  float fx = 0;
  float fy = 0;
  event.Position( &fx, &fy );
  int x = (int)fx;
  int y = (int)fy;
  if (event.Moving())
  {
    bool hit = FALSE;
    int xpos = 0;
    for (int j = 0; j < m_owner->GetColumnCount(); j++)
    {
      xpos += m_owner->GetColumnWidth( j );
      if ((abs(x-xpos) < 2) && (y < 14)) { hit = TRUE; break; }
    };
    if (hit)
    {
      if (m_currentCursor == wxSTANDARD_CURSOR) SetCursor( m_resizeCursor );
      m_currentCursor = m_resizeCursor;
    }
    else
    {
      if (m_currentCursor != wxSTANDARD_CURSOR) SetCursor( wxSTANDARD_CURSOR );
      m_currentCursor = wxSTANDARD_CURSOR;
    };
  };
};

void wxListHeaderWindow::OnSetFocus( wxFocusEvent &WXUNUSED(event) )
{
  m_owner->SetFocus();
};

//-----------------------------------------------------------------------------
// wxListRenameTimer (internal)
//-----------------------------------------------------------------------------

wxListRenameTimer::wxListRenameTimer( wxListMainWindow *owner ) 
{ 
  m_owner = owner; 
};

void wxListRenameTimer::Notify() 
{ 
  m_owner->OnRenameTimer();
};

//-----------------------------------------------------------------------------
//  wxListMainWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListMainWindow,wxScrolledWindow);
    
BEGIN_EVENT_TABLE(wxListMainWindow,wxScrolledWindow)
  EVT_PAINT          (wxListMainWindow::OnPaint)
  EVT_SIZE           (wxListMainWindow::OnSize)
  EVT_MOUSE_EVENTS   (wxListMainWindow::OnMouse)
  EVT_CHAR           (wxListMainWindow::OnChar)
  EVT_SET_FOCUS      (wxListMainWindow::OnSetFocus)
  EVT_KILL_FOCUS     (wxListMainWindow::OnKillFocus)
END_EVENT_TABLE()

wxListMainWindow::wxListMainWindow( void )
{
  m_mode = 0;
  m_lines.DeleteContents( TRUE );
  m_columns.DeleteContents( TRUE );
  m_current = NULL;
  m_visibleLines = 0;
  m_hilightBrush = NULL;
  m_myFont = NULL;
  m_xScroll = 0;
  m_yScroll = 0;
  m_dirty = TRUE;
  m_small_image_list = NULL;
  m_normal_image_list = NULL;
  m_small_spacing = 30;
  m_normal_spacing = 40;
  m_hasFocus = FALSE;
  m_usedKeys = TRUE;
  m_lastOnSame = FALSE;
//  m_renameTimer = new wxRenameTimer( this );
  m_isCreated = FALSE;
  m_isDragging = FALSE;
};

wxListMainWindow::wxListMainWindow( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name ) :
  wxScrolledWindow( parent, id, pos, size, style, name )
{
  m_mode = style;
  m_lines.DeleteContents( TRUE );
  m_columns.DeleteContents( TRUE );
  m_current = NULL;
  m_dirty = TRUE;
  m_visibleLines = 0;
  m_hilightBrush = new wxBrush( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT), wxSOLID );
  m_small_image_list = NULL;
  m_normal_image_list = NULL;
  m_small_spacing = 30;
  m_normal_spacing = 40;
//  AllowDoubleClick( TRUE );
  m_myFont = wxNORMAL_FONT;
  m_hasFocus = FALSE;
  m_isDragging = FALSE;
  m_isCreated = FALSE;
  wxSize sz = size;
  sz.y = 25;
  
  if (m_mode & wxLC_REPORT)
  {
    m_xScroll = 0;
    m_yScroll = 15;
  }
  else
  {
    m_xScroll = 15;
    m_yScroll = 0;
  };
  SetScrollbars( m_xScroll, m_yScroll, 0, 0, 0, 0 );
  
  m_usedKeys = TRUE;
  m_lastOnSame = FALSE;
  m_renameTimer = new wxListRenameTimer( this );
  m_renameAccept = FALSE;
//  m_text = new wxRawListTextCtrl( GetParent(), "", &m_renameAccept, &m_renameRes, this, 10, 10, 40, 10 );
//  m_text->Show( FALSE );

  SetBackgroundColour( *wxWHITE );
  
/*
  char *accepted_drop_types[] = { "text/plain" };
  gtk_widget_dnd_drag_set( m_wxwindow, TRUE, accepted_drop_types, 1 );
*/  
};

wxListMainWindow::~wxListMainWindow( void )
{
//  if (m_hilightColour) delete m_hilightColour;
//  if (m_hilightBrush) delete m_hilightBrush;
//  if (m_myFont) delete m_myFont;
  delete m_renameTimer;
//  delete m_text;
};

void wxListMainWindow::RefreshLine( wxListLineData *line )
{
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
  if (line)
  {
    wxClientDC dc(this);
    PrepareDC( dc );
    line->GetExtent( x, y, w, h );
    wxRectangle rect( 
      dc.LogicalToDeviceX(x-3), 
      dc.LogicalToDeviceY(y-3), 
      dc.LogicalToDeviceXRel(w+6), 
      dc.LogicalToDeviceXRel(h+6) );
    Refresh( TRUE, &rect );
  };
};

void wxListMainWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  if (m_dirty) return;
  
  wxPaintDC dc( this );
  PrepareDC( dc );
  
  dc.BeginDrawing();

//  dc.SetFont( *m_myFont );
  dc.SetFont( wxSystemSettings::GetSystemFont( wxSYS_SYSTEM_FONT ) );

  wxNode *node = m_lines.First();
  while (node) 
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    line->Draw( &dc );
    node = node->Next();
  };
  if (m_current) m_current->DrawRubberBand( &dc, m_hasFocus );

  dc.EndDrawing();
};

void wxListMainWindow::HilightAll( bool on )
{
  wxNode *node = m_lines.First();
  while (node)
  {
    wxListLineData *line = (wxListLineData *)node->Data();
    if (line->IsHilighted() != on)
    {
      line->Hilight( on );
      RefreshLine( line );
    };
    node = node->Next();
  };
};

void wxListMainWindow::ActivateLine( wxListLineData *line )
{
  if (!m_parent) return;
  wxListEvent le( wxEVT_COMMAND_LIST_KEY_DOWN, m_parent->GetId() );
  le.SetEventObject( m_parent );
  le.m_code = 0;
  le.m_itemIndex = GetIndexOfLine( line );
  le.m_col = 0;
  line->GetItem( 0, le.m_item );
  OnListNotify( le );
};

void wxListMainWindow::SendNotify( wxListLineData *line, wxEventType command )
{
  if (!m_parent) return;
  wxListEvent le( command, m_parent->GetId() );
  le.SetEventObject( m_parent );
  le.m_code = 0;
  le.m_itemIndex = GetIndexOfLine( line );
  le.m_col = 0;
  line->GetItem( 0, le.m_item );
  OnListNotify( le );
};

void wxListMainWindow::FocusLine( wxListLineData *WXUNUSED(line) )
{
//  SendNotify( line, wxEVT_COMMAND_LIST_ITEM_FOCUSSED );
};

void wxListMainWindow::UnfocusLine( wxListLineData *WXUNUSED(line) )
{
//  SendNotify( line, wxEVT_COMMAND_LIST_ITEM_UNFOCUSSED );
};

void wxListMainWindow::SelectLine( wxListLineData *line )
{
  SendNotify( line, wxEVT_COMMAND_LIST_ITEM_SELECTED );
};

void wxListMainWindow::DeselectLine( wxListLineData *line )
{
  SendNotify( line, wxEVT_COMMAND_LIST_ITEM_DESELECTED );
};

void wxListMainWindow::DeleteLine( wxListLineData *line )
{
  SendNotify( line, wxEVT_COMMAND_LIST_DELETE_ITEM );
};

void wxListMainWindow::RenameLine( wxListLineData *line, const wxString &newName )
{
  wxListEvent le( wxEVT_COMMAND_LIST_END_LABEL_EDIT );
  le.m_code = 0;
  le.m_itemIndex = GetIndexOfLine( line );
  le.m_col = 0;
  line->GetItem( 0, le.m_item );
  le.m_item.m_text = newName;
  OnListNotify( le );
};

void wxListMainWindow::OnRenameTimer()
{
  return;
  wxString s;
  m_current->GetText( 0, s );
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
  m_current->GetLabelExtent( x, y, w, h );
  int dx = 0;
  int dy = 0;
  GetPosition( &dx, &dy );
  x += dx;
  y += dy;
/*
  wxRawListTextCtrl *text = new wxRawListTextCtrl( 
    GetParent(), s, &m_renameAccept, &m_renameRes, this, x+2, y+2, w+8, h+8 );
  text->SetFocus();
*/
/*
  m_text->SetSize( x+3, y+3, w+6, h+6 );
  m_text->SetValue( s );
  m_text->Show( TRUE );
  m_text->SetFocus();
*/
/*
  char *res = wxGetTextFromUser( "Enter new name:", "", s );
  if (res)
  {
    m_dirty = TRUE;
    s = res;
    RenameLine( m_current, s );
  };
  */
};

void wxListMainWindow::OnRenameAccept()
{
  RenameLine( m_current, m_renameRes );
};

void wxListMainWindow::OnMouse( wxMouseEvent &event )
{
  if (!m_current) return;
  if (m_dirty) return;
//  wxDragCanvas::OnEvent( event );

  wxClientDC dc(this);
  PrepareDC(dc);
  long x = dc.DeviceToLogicalX( (long)event.GetX() );
  long y = dc.DeviceToLogicalY( (long)event.GetY() );
  
  long hitResult = 0;
  wxNode *node = m_lines.First();
  wxListLineData *line = NULL;
  while (node)
  {
    line = (wxListLineData*)node->Data();
    hitResult = line->IsHit( x, y );
    if (hitResult) break;
    line = NULL;
    node = node->Next();
  };
  
  if (!event.Dragging()) m_isDragging = FALSE;
  
  if (event.Dragging() && (!m_isDragging))
  {
    m_isDragging = TRUE;
    wxListEvent le( wxEVT_COMMAND_LIST_BEGIN_DRAG, m_parent->GetId() );
    le.SetEventObject( this );
    le.m_code = 0;
    le.m_itemIndex = 0;
    le.m_col = 0;
    OnListNotify( le );
  };
  
  if (!line) return;
  
  if (event.ButtonDClick())
  {
    m_usedKeys = FALSE;
    m_lastOnSame = FALSE;
    m_renameTimer->Stop();
    ActivateLine( line );
    return;
  };
  
  if (event.LeftUp() && m_lastOnSame)
  {
    m_usedKeys = FALSE;
    if ((line == m_current) &&
        (hitResult == wxLIST_HITTEST_ONITEMLABEL) /* && 
	(m_mode & wxLC_ICON) */  )
    {
      m_renameTimer->Start( 330, TRUE );
    };
    m_lastOnSame = FALSE;
    return;
  };
  
  if (event.LeftDown())
  {
    m_usedKeys = FALSE;
    wxListLineData *oldCurrent = m_current;
    m_current = line;
    if (!event.ShiftDown() || (m_mode & wxLC_SINGLE_SEL)) HilightAll( FALSE );
    m_current->ReverseHilight();
    RefreshLine( m_current );
    if (m_current != oldCurrent)
    {
      UnfocusLine( oldCurrent );
      FocusLine( m_current );
      RefreshLine( oldCurrent );
    };
    m_lastOnSame = (m_current == oldCurrent);
    return;
  };
  
};

void wxListMainWindow::MoveToFocus( void )
{
  if (!m_current) return;
/*
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
  m_current->GetExtent( x, y, w, h );
  int w_p = 0;
  int h_p = 0;
  GetClientSize( &w_p, &h_p );
  if (m_mode & wxLC_REPORT)
  {
    if (GetScrollPos( wxHORIZONTAL ) != 0) SetScrollPos( wxHORIZONTAL, 0);
    int y_s = m_yScroll*GetScrollPos( wxVERTICAL );
    if ((y > y_s) && (y+h < y_s+h_p)) return; 
    if (y-y_s < 5) SetScrollPos( wxVERTICAL, (y-5)/m_yScroll );
    if (y+h+5 > y_s+h_p) SetScrollPos( wxVERTICAL, (y+h-h_p+h+5)/m_yScroll );
  }
  else
  {
    if (GetScrollPos( wxVERTICAL ) != 0) SetScrollPos( wxVERTICAL, 0);
    int x_s = m_xScroll*GetScrollPos( wxHORIZONTAL );
    if ((x > x_s) && (x+w < x_s+w_p)) return;
    if (x-x_s < 5) SetScrollPos( wxHORIZONTAL, (x-5)/m_xScroll );
    if (x+w > x_s+w_p)  SetScrollPos( wxHORIZONTAL, (x+w-w_p+5)/m_xScroll );
  };
*/
};

void wxListMainWindow::OnArrowChar( wxListLineData *newCurrent, bool shiftDown )
{
  UnfocusLine( m_current );
  if ((m_mode & wxLC_SINGLE_SEL) || (m_usedKeys == FALSE)) m_current->Hilight( FALSE );
  wxListLineData *oldCurrent = m_current;
  m_current = newCurrent;
  MoveToFocus();
  if (shiftDown || (m_mode & wxLC_SINGLE_SEL)) m_current->Hilight( TRUE );
  FocusLine( m_current );
  RefreshLine( m_current );
  RefreshLine( oldCurrent );
};

void wxListMainWindow::OnChar( wxKeyEvent &event )
{
/*
  if (event.KeyCode() == WXK_TAB)
  {
    if (event.ShiftDown())
      TravPrev( &event );
    else
      TravNext( &event );
    return;
  };
*/
  if (!m_current) return;
  switch (event.KeyCode())
  {
    case WXK_UP:
    {
      wxNode *node = m_lines.Member( m_current )->Previous();
      if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
      break;
    };
    case WXK_DOWN:
    {
      wxNode *node = m_lines.Member( m_current )->Next();
      if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
      break;
    };
    case WXK_END:
    {
      wxNode *node = m_lines.Last();
      OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
      break;
    };
    case WXK_HOME:
    {
      wxNode *node = m_lines.First();
      OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
      break;
    };
    case WXK_PRIOR:
    {
      int steps = 0;
      if (m_mode & wxLC_REPORT) { steps = m_visibleLines-1; }
      else
      {
        int pos = 0; 
        wxNode *node = m_lines.First();
        for (;;) { if (m_current == (wxListLineData*)node->Data()) break; pos++; node = node->Next(); };
        steps = pos % m_visibleLines;
      };
      wxNode *node = m_lines.Member( m_current );
      for (int i = 0; i < steps; i++) if (node->Previous()) node = node->Previous();
      if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
      break;
    };
    case WXK_NEXT:
    {
      int steps = 0;
      if (m_mode & wxLC_REPORT) { steps = m_visibleLines-1; }
      else
      {
        int pos = 0; wxNode *node = m_lines.First();
        for (;;) { if (m_current == (wxListLineData*)node->Data()) break; pos++; node = node->Next(); };
        steps = m_visibleLines-(pos % m_visibleLines)-1;
      };
      wxNode *node = m_lines.Member( m_current );
      for (int i = 0; i < steps; i++) if (node->Next()) node = node->Next();
      if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
      break;
    };
    case WXK_LEFT:
    {
      if (!(m_mode & wxLC_REPORT))
      {
        wxNode *node = m_lines.Member( m_current );
        for (int i = 0; i <m_visibleLines; i++) if (node->Previous()) node = node->Previous();
        if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
      };
      break;
    };
    case WXK_RIGHT:
    {
      if (!(m_mode & wxLC_REPORT))
      {
        wxNode *node = m_lines.Member( m_current );
        for (int i = 0; i <m_visibleLines; i++) if (node->Next()) node = node->Next();
        if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
      };
      break;
    };
    case WXK_INSERT:
    {
      if (!(m_mode & wxLC_SINGLE_SEL))
      {
        wxListLineData *oldCurrent = m_current;
        UnfocusLine( m_current );
        m_current->ReverseHilight();
        wxNode *node = m_lines.Member( m_current )->Next();       
        if (node) m_current = (wxListLineData*)node->Data();
        MoveToFocus();
        FocusLine( m_current );
	RefreshLine( m_current );
	RefreshLine( oldCurrent );
      };
    };
    break;
    case WXK_RETURN:
    case WXK_EXECUTE:
    {
      ActivateLine( m_current );
    };
    break;
    default:
    {
      event.Skip();
      return;
    };
  };
  m_usedKeys = TRUE;
};

void wxListMainWindow::OnSetFocus( wxFocusEvent &WXUNUSED(event) )
{
  m_hasFocus = TRUE;
  RefreshLine( m_current );
  
  if (!m_parent) return;
  
  wxFocusEvent event( wxEVT_SET_FOCUS, m_parent->GetId() );
  event.SetEventObject( m_parent );
  m_parent->ProcessEvent( event );
};

void wxListMainWindow::OnKillFocus( wxFocusEvent &WXUNUSED(event) )
{
  m_hasFocus = FALSE;
  RefreshLine( m_current );
};

void wxListMainWindow::OnSize( wxSizeEvent &WXUNUSED(event) )
{
/*
  We don't even allow the wxScrolledWindow::AdjustScrollbars() call

  CalculatePositions();
  printf( "OnSize::Refresh.\n" );
  Refresh();
  event.Skip();
*/
};

wxFont *wxListMainWindow::GetMyFont( void )
{
  return m_myFont;
};

void wxListMainWindow::DrawImage( int index, wxPaintDC *dc, int x, int y )
{
  if ((m_mode & wxLC_ICON) && (m_normal_image_list))
  {
    m_normal_image_list->Draw( index, *dc, x, y );
    return;
  };
  if ((m_mode & wxLC_SMALL_ICON) && (m_small_image_list))
  {
    m_small_image_list->Draw( index, *dc, x, y );
  };
};

void wxListMainWindow::GetImageSize( int index, int &width, int &height )
{
  if ((m_mode & wxLC_ICON) && (m_normal_image_list))
  {
    m_normal_image_list->GetSize( index, width, height );
    return;
  };
  if ((m_mode & wxLC_SMALL_ICON) && (m_small_image_list))
  {
    m_small_image_list->GetSize( index, width, height );
    return;
  };
  width = 0;
  height = 0;
};

int wxListMainWindow::GetTextLength( wxString &s )
{
  wxPaintDC dc( this );
  long lw = 0;
  long lh = 0;
  dc.GetTextExtent( s, &lw, &lh );
  return lw + 6;
};

int wxListMainWindow::GetIndexOfLine( const wxListLineData *line )
{
  int i = 0;
  wxNode *node = m_lines.First();
  while (node)
  {
    if (line == (wxListLineData*)node->Data()) return i;
    i++;
    node = node->Next();
  };
  return -1;
};

void wxListMainWindow::SetImageList( wxImageList *imageList, int which )
{
  m_dirty = TRUE;
  if (which == wxIMAGE_LIST_NORMAL) m_normal_image_list = imageList;
  if (which == wxIMAGE_LIST_SMALL) m_small_image_list = imageList;
};

void wxListMainWindow::SetItemSpacing( int spacing, bool isSmall )
{
  m_dirty = TRUE;
  if (isSmall)
  { 
    m_small_spacing = spacing;
  }
  else
  {
    m_normal_spacing = spacing;
  };
};

int wxListMainWindow::GetItemSpacing( bool isSmall )
{
  if (isSmall) return m_small_spacing; else return m_normal_spacing;
};

void wxListMainWindow::SetColumn( int col, wxListItem &item )
{
  m_dirty = TRUE;
  wxNode *node = m_columns.Nth( col );
  if (node)
  {
    if (item.m_width == wxLIST_AUTOSIZE_USEHEADER) item.m_width = GetTextLength( item.m_text )+7;
    wxListHeaderData *column = (wxListHeaderData*)node->Data();
    column->SetItem( item );
  };
};

void wxListMainWindow::SetColumnWidth( int col, int width )
{
  m_dirty = TRUE;
  wxNode *node = m_columns.Nth( col );
  if (node)
  {
    wxListHeaderData *column = (wxListHeaderData*)node->Data();
    column->SetWidth( width );
  };
};

void wxListMainWindow::GetColumn( int col, wxListItem &item )
{
  wxNode *node = m_columns.Nth( col );
  if (node)
  {
    wxListHeaderData *column = (wxListHeaderData*)node->Data();
    column->GetItem( item );
  }
  else
  {
    item.m_format = 0;
    item.m_width = 0;
    item.m_text = "";
    item.m_image = 0;
    item.m_data = 0;
  };
};

int wxListMainWindow::GetColumnWidth( int col ) 
{
  wxNode *node = m_columns.Nth( col );
  if (node)
  {
    wxListHeaderData *column = (wxListHeaderData*)node->Data();
    return column->GetWidth();
  }
  else
    return 0;
};

int wxListMainWindow::GetColumnCount( void )
{
  return m_columns.Number();
};

int wxListMainWindow::GetCountPerPage( void )
{
  return m_visibleLines;
};

void wxListMainWindow::SetItem( wxListItem &item )
{
  m_dirty = TRUE;
  wxNode *node = m_lines.Nth( item.m_itemId );
  if (node) 
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    if (m_mode & wxLC_REPORT) item.m_width = GetColumnWidth( item.m_col )-3;
    line->SetItem( item.m_col, item );
  };
};

void wxListMainWindow::SetItemState( long item, long state, long stateMask )
{
  // m_dirty = TRUE; no recalcs needed
  wxListLineData *oldCurrent = m_current;
  if (stateMask & wxLIST_STATE_FOCUSED) 
  {
    wxNode *node = m_lines.Nth( item );
    if (node) 
    {
      wxListLineData *line = (wxListLineData*)node->Data();
      UnfocusLine( m_current );
      m_current = line;
      FocusLine( m_current );
      RefreshLine( m_current );
      RefreshLine( oldCurrent );
    };
  };
  if (stateMask & wxLIST_STATE_SELECTED)
  {
    wxNode *node = m_lines.Nth( item );
    if (node) 
    {
      wxListLineData *line = (wxListLineData*)node->Data();
      bool on = state & wxLIST_STATE_SELECTED;
      line->Hilight( on );
      RefreshLine( m_current );
      RefreshLine( oldCurrent );
    };
  };
};

int wxListMainWindow::GetItemState( long item, long stateMask )
{
  int ret = wxLIST_STATE_DONTCARE;
  if (stateMask & wxLIST_STATE_FOCUSED)
  {
    wxNode *node = m_lines.Nth( item );
    if (node) 
    {
      wxListLineData *line = (wxListLineData*)node->Data();
      if (line == m_current) ret |= wxLIST_STATE_FOCUSED;
    };
  };
  if (stateMask & wxLIST_STATE_SELECTED)
  {
    wxNode *node = m_lines.Nth( item );
    if (node) 
    {
      wxListLineData *line = (wxListLineData*)node->Data();
      if (line->IsHilighted()) ret |= wxLIST_STATE_FOCUSED;
    };
  };
  return ret;
};

void wxListMainWindow::GetItem( wxListItem &item )
{
  wxNode *node = m_lines.Nth( item.m_itemId );
  if (node) 
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    line->GetItem( item.m_col, item );
  }
  else
  {
    item.m_mask = 0;
    item.m_text = "";
    item.m_image = 0;
    item.m_data = 0;
  };
};

int wxListMainWindow::GetItemCount( void )
{
  return m_lines.Number();
};

void wxListMainWindow::GetItemRect( long index, wxRectangle &rect )
{
  wxNode *node = m_lines.Nth( index );
  if (node) 
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    line->GetRect( rect );
  }
  else
  {
    rect.x = 0;
    rect.y = 0;
    rect.width = 0;
    rect.height = 0;
  };
};

int wxListMainWindow::GetSelectedItemCount( void )
{
  int ret = 0;
  wxNode *node = m_lines.First();
  while (node)
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    if (line->IsHilighted()) ret++;
    node = node->Next();
  };
  return 0;
};

void wxListMainWindow::SetMode( long mode )
{
  m_dirty = TRUE;
  m_mode = mode;
  
  DeleteEverything();
  
  if (m_mode & wxLC_REPORT)
  {
    m_xScroll = 0;
    m_yScroll = 15;
  }
  else
  {
    m_xScroll = 15;
    m_yScroll = 0;
  };
};

long wxListMainWindow::GetMode( void ) const
{
  return m_mode;
};

void wxListMainWindow::CalculatePositions( void )
{
  wxPaintDC dc( this );
  dc.SetFont( wxSystemSettings::GetSystemFont( wxSYS_SYSTEM_FONT ) );

  int iconSpacing = 0;
  if (m_mode & wxLC_ICON) iconSpacing = m_normal_spacing;
  if (m_mode & wxLC_SMALL_ICON) iconSpacing = m_small_spacing;
  wxNode *node = m_lines.First();
  while (node) 
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    line->CalculateSize( &dc, iconSpacing );
    node = node->Next();
  };

  int lineWidth = 0;
  int lineHeight = 0;
  int lineSpacing = 0;  

  node = m_lines.First();
  if (node) 
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    int dummy = 0;
    line->GetSize( dummy, lineSpacing );
    lineSpacing += 6;
  }
  else
  {
    // just in case
    lineSpacing = 6 + (int)dc.GetCharHeight();  
  };
  
  int clientWidth = 0;
  int clientHeight = 0;
  
  if (m_mode & wxLC_REPORT)
  {
    int x = 5;
    int y = 6;
    int entireHeight = m_lines.Number() * lineSpacing + 10;
    SetScrollbars( m_xScroll, m_yScroll, 0, (entireHeight+10) / m_yScroll, 0, 0, TRUE );
    GetClientSize( &clientWidth, &clientHeight );
    node = m_lines.First();
    while (node)
    {
      wxListLineData *line = (wxListLineData*)node->Data();
      line->SetPosition( &dc, x, y, clientWidth );
      int col_x = 3;
      for (int i = 0; i < GetColumnCount(); i++)
      {
        line->SetColumnPosition( i, col_x );
        col_x += GetColumnWidth( i );
      };
      y += lineSpacing;
      node = node->Next();
    };
  }
  else
  {
    // At first, we try without any scrollbar
    GetSize( &clientWidth, &clientHeight );
    
    int entireWidth = 0;
    
    for (int tries = 0; tries < 2; tries++)
    {
      entireWidth = 0;
      int x = 5;
      int y = 6;
      int maxWidth = 0;
      node = m_lines.First();
      while (node)
      {
        wxListLineData *line = (wxListLineData*)node->Data();
        line->SetPosition( &dc, x, y, clientWidth );
        line->GetSize( lineWidth, lineHeight );
        if (lineWidth > maxWidth) maxWidth = lineWidth;
        y += lineSpacing;
        if (y+lineHeight > clientHeight-4) 
        {
          y = 6;
          x += maxWidth+13;
          entireWidth += maxWidth+13;
          maxWidth = 0;
        };
        node = node->Next();
	if (!node) entireWidth += maxWidth;
	if ((tries == 0) && (entireWidth > clientWidth))
	{
	  clientHeight -= 14; // scrollbar height
	  break;
	};
	if (!node) tries = 1;
      };
    };
    SetScrollbars( m_xScroll, m_yScroll, (entireWidth+15) / m_xScroll, 0, 0, 0, TRUE );
  };
  m_visibleLines = (clientHeight-4) / (lineSpacing);
};

void wxListMainWindow::RealizeChanges( void )
{
  if (!m_current)
  {
    wxNode *node = m_lines.First();
    if (node) m_current = (wxListLineData*)node->Data();
  };
  if (m_current)
  { 
    FocusLine( m_current );
    if (m_mode & wxLC_SINGLE_SEL) m_current->Hilight( TRUE );
  };
};

long wxListMainWindow::GetNextItem( long item, int WXUNUSED(geometry), int state )
{
  long ret = 0;
  if (item > 0) ret = item;
  wxNode *node = m_lines.Nth( ret );
  while (node)
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    if ((state & wxLIST_STATE_FOCUSED) && (line == m_current)) return ret;
    if ((state & wxLIST_STATE_SELECTED) && (line->IsHilighted())) return ret;
    if (!state) return ret;
    ret++;
    node = node->Next();
  };
  return -1;
};

void wxListMainWindow::DeleteItem( long index )
{
  m_dirty = TRUE;
  wxNode *node = m_lines.Nth( index );
  if (node)
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    DeleteLine( line );
    m_lines.DeleteNode( node );
  };
};

void wxListMainWindow::DeleteColumn( int col )
{
  m_dirty = TRUE;
  wxNode *node = m_columns.Nth( col );
  if (node) m_columns.DeleteNode( node );
};

void wxListMainWindow::DeleteAllItems( void )
{
  m_dirty = TRUE;
  wxNode *node = m_lines.First();
  while (node)
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    DeleteLine( line );
    node = node->Next();
  };
  m_lines.Clear();
  m_current = NULL;
};

void wxListMainWindow::DeleteEverything( void )
{
  m_dirty = TRUE;
  wxNode *node = m_lines.First();
  while (node)
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    DeleteLine( line );
    node = node->Next();
  };
  m_lines.Clear();
  m_current = NULL;
  m_columns.Clear();
};

void wxListMainWindow::EnsureVisible( long index )
{
  wxListLineData *oldCurrent = m_current;
  m_current = NULL;
  int i = index;
  wxNode *node = m_lines.Nth( i );
  if (node) m_current = (wxListLineData*)node->Data();
  if (m_current) MoveToFocus();
  m_current = oldCurrent;
};

long wxListMainWindow::FindItem(long start, const wxString& str, bool WXUNUSED(partial) )
{
  long pos = start;
  wxString tmp = str;
  if (pos < 0) pos = 0;
  wxNode *node = m_lines.Nth( pos );
  while (node)
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    wxString s = "";
    line->GetText( 0, s );
    if (s == tmp) return pos;
    node = node->Next();
    pos++;
  };
  return -1;
};

long wxListMainWindow::FindItem(long start, long data)
{
  long pos = start;
  if (pos < 0) pos = 0;
  wxNode *node = m_lines.Nth( pos );
  while (node)
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    wxListItem item;
    line->GetItem( 0, item );
    if (item.m_data == data) return pos;
    node = node->Next();
    pos++;
  };
  return -1;
};

long wxListMainWindow::HitTest( int x, int y, int &flags )
{
  wxNode *node = m_lines.First();
  int count = 0;
  while (node)
  {
    wxListLineData *line = (wxListLineData*)node->Data();
    long ret = line->IsHit( x, y );
    if (ret & flags)
    {
      flags = ret;
      return count;
    };
    node = node->Next();
    count++;
  };
  return -1;
};

void wxListMainWindow::InsertItem( wxListItem &item )
{
  m_dirty = TRUE;
  int mode = 0;
  if (m_mode & wxLC_REPORT) mode = wxLC_REPORT;
  else if (m_mode & wxLC_LIST) mode = wxLC_LIST;
  else if (m_mode & wxLC_ICON) mode = wxLC_ICON;
  else if (m_mode & wxLC_SMALL_ICON) mode = wxLC_ICON;  // no typo
  wxListLineData *line = new wxListLineData( this, mode, m_hilightBrush );
  if (m_mode & wxLC_REPORT)
  {
    line->InitItems( GetColumnCount() );
    item.m_width = GetColumnWidth( 0 )-3;
  }
  else 
    line->InitItems( 1 );
  line->SetItem( 0, item );
  wxNode *node = m_lines.Nth( item.m_itemId );
  if (node)
    m_lines.Insert( node, line );
  else
    m_lines.Append( line );
};

void wxListMainWindow::InsertColumn( long col, wxListItem &item )
{
  m_dirty = TRUE;
  if (m_mode & wxLC_REPORT)
  {
    if (item.m_width == wxLIST_AUTOSIZE_USEHEADER) item.m_width = GetTextLength( item.m_text );
    wxListHeaderData *column = new wxListHeaderData( item );
    wxNode *node = m_columns.Nth( col );
    if (node)
      m_columns.Insert( node, column );
    else
      m_columns.Append( column );
  };
};

wxListCtrlCompare list_ctrl_compare_func_2;
long              list_ctrl_compare_data;

int list_ctrl_compare_func_1( const void *arg1, const void *arg2 )
{
  wxListLineData *line1 = *((wxListLineData**)arg1);
  wxListLineData *line2 = *((wxListLineData**)arg2);
  wxListItem item;
  line1->GetItem( 0, item );
  long data1 = item.m_data;
  line2->GetItem( 0, item );
  long data2 = item.m_data;
  return list_ctrl_compare_func_2( data1, data2, list_ctrl_compare_data );
};

void wxListMainWindow::SortItems( wxListCtrlCompare fn, long data )
{
  list_ctrl_compare_func_2 = fn;
  list_ctrl_compare_data = data;
  m_lines.Sort( list_ctrl_compare_func_1 );
};

bool wxListMainWindow::OnListNotify( wxListEvent &event )
{
  if (m_parent) m_parent->ProcessEvent( event );
  return FALSE;
};

// -------------------------------------------------------------------------------------
// wxListItem
// -------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListItem, wxObject)

wxListItem::wxListItem(void)
{
  m_mask = 0;
  m_itemId = 0;
  m_col = 0;
  m_state = 0;
  m_stateMask = 0;
  m_image = 0;
  m_data = 0;
  m_format = wxLIST_FORMAT_CENTRE;
  m_width = 0;
  m_colour = wxBLACK;
}

// -------------------------------------------------------------------------------------
// wxListEvent
// -------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListEvent, wxCommandEvent)

wxListEvent::wxListEvent( wxEventType commandType, int id ):
  wxCommandEvent( commandType, id )
{
  m_code = 0;
  m_itemIndex = 0;
  m_col = 0;
  m_cancelled = FALSE;
};

// -------------------------------------------------------------------------------------
// wxListCtrl
// -------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListCtrl, wxControl)

BEGIN_EVENT_TABLE(wxListCtrl,wxControl)
  EVT_SIZE          (wxListCtrl::OnSize)
  EVT_IDLE          (wxListCtrl::OnIdle)
END_EVENT_TABLE()

wxListCtrl::wxListCtrl(void)
{
  m_imageListNormal = NULL;
  m_imageListSmall = NULL;
  m_imageListState = NULL;
}

wxListCtrl::wxListCtrl( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )

{
  Create( parent, id, pos, size, style, name );
};

wxListCtrl::~wxListCtrl(void)
{
}

bool wxListCtrl::Create( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  m_imageListNormal = NULL;
  m_imageListSmall = NULL;
  m_imageListState = NULL;
  
  long s = style;
  
  if ((s & wxLC_REPORT == 0) &&
      (s & wxLC_LIST == 0) &&
      (s & wxLC_ICON == 0))
    s = s | wxLC_LIST;
  
  bool ret = wxControl::Create( parent, id, pos, size, s, name );
  
  m_mainWin = new wxListMainWindow( this, -1, wxPoint(0,0), size, s );
  
  if (GetWindowStyleFlag() & wxLC_REPORT)
    m_headerWin = new wxListHeaderWindow( this, -1, m_mainWin, wxPoint(0,0), wxSize(size.x,23) );
  else
    m_headerWin = NULL;
  
  return ret;
};

void wxListCtrl::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  // handled in OnIdle
  
  if (m_mainWin) m_mainWin->m_dirty = TRUE;
};

void wxListCtrl::SetSingleStyle( long style, bool add )
{
  long flag = GetWindowStyleFlag();
  
  if (add)
  {
    if (style & wxLC_MASK_TYPE)	flag = flag & ~wxLC_MASK_TYPE;
    if (style & wxLC_MASK_ALIGN) flag = flag & ~wxLC_MASK_ALIGN;
    if (style & wxLC_MASK_SORT) flag = flag & ~wxLC_MASK_SORT;
  };

  if (add)
  {
    flag |= style;
  }
  else
  {
    if (flag & style) flag -= style;
  };
  
  SetWindowStyleFlag( flag );
};

void wxListCtrl::SetWindowStyleFlag( long flag )
{
  m_mainWin->DeleteEverything();

  int width = 0;
  int height = 0;
  GetClientSize( &width, &height );

  m_mainWin->SetMode( flag );
  
  if (flag & wxLC_REPORT)
  {
    if (!(GetWindowStyleFlag() & wxLC_REPORT))
    {
//      m_mainWin->SetSize( 0, 24, width, height-24 );
      if (!m_headerWin)
      {
        m_headerWin = new wxListHeaderWindow( this, -1, m_mainWin, wxPoint(0,0), wxSize(width,23) );
      }
      else
      {
//        m_headerWin->SetSize( 0, 0, width, 23 );
        m_headerWin->Show( TRUE );
      };
    };
  }
  else
  {
    if (GetWindowStyleFlag() & wxLC_REPORT)
    {
//      m_mainWin->SetSize( 0, 0, width, height );
      m_headerWin->Show( FALSE );
    };
  };   
  
  wxWindow::SetWindowStyleFlag( flag );
};

void wxListCtrl::SetBackgroundColour(const wxColour& col)
{
  // This is from Julian. You know.
  // Not in wxWin 1.xx ???
  wxWindow::SetBackgroundColour( (wxColour&)col );
};

bool wxListCtrl::GetColumn(int col, wxListItem &item) 
{
  m_mainWin->GetColumn( col, item );
  return TRUE;
};

bool wxListCtrl::SetColumn( int col, wxListItem& item )
{
  m_mainWin->SetColumn( col, item );
  return TRUE;
};

int wxListCtrl::GetColumnWidth( int col ) 
{
  return m_mainWin->GetColumnWidth( col );
};

bool wxListCtrl::SetColumnWidth( int col, int width )
{
  m_mainWin->SetColumnWidth( col, width );
  return TRUE;
};

int wxListCtrl::GetCountPerPage(void) 
{
  return m_mainWin->GetCountPerPage();  // different from Windows ?
};

/*
wxText& wxListCtrl::GetEditControl(void) const
{
};
*/

bool wxListCtrl::GetItem( wxListItem &info ) 
{
  m_mainWin->GetItem( info );
  return TRUE;
};

bool wxListCtrl::SetItem( wxListItem &info )
{
  m_mainWin->SetItem( info );
  return TRUE;
};

long wxListCtrl::SetItem( long index, int col, const wxString& label, int imageId )
{
  wxListItem info;
  info.m_text = label;
  info.m_mask = wxLIST_MASK_TEXT;
  info.m_itemId = index;
  info.m_col = col;
  if ( imageId > -1 )
  {
    info.m_image = imageId;
    info.m_mask |= wxLIST_MASK_IMAGE;
  }
;
  m_mainWin->SetItem(info);
  return TRUE;
};

int wxListCtrl::GetItemState( long item, long stateMask )
{
  return m_mainWin->GetItemState( item, stateMask );  
};

bool wxListCtrl::SetItemState( long item, long state, long stateMask )
{
  m_mainWin->SetItemState( item, state, stateMask );
  return TRUE;
};

bool wxListCtrl::SetItemImage( long item, int image, int WXUNUSED(selImage) )
{
  wxListItem info;
  info.m_image = image;
  info.m_mask = wxLIST_MASK_IMAGE;
  info.m_itemId = item;
  m_mainWin->SetItem( info );
  return TRUE;
};

wxString wxListCtrl::GetItemText( long item ) 
{
  wxListItem info;
  info.m_itemId = item;
  m_mainWin->GetItem( info );
  return info.m_text;
};

void wxListCtrl::SetItemText( long item, const wxString &str )
{
  wxListItem info;
  info.m_mask = wxLIST_MASK_TEXT;
  info.m_itemId = item;
  info.m_text = str;
  m_mainWin->SetItem( info );
};

long wxListCtrl::GetItemData( long item )
{
  wxListItem info;
  info.m_itemId = item;
  m_mainWin->GetItem( info );
  return info.m_data;
};

bool wxListCtrl::SetItemData( long item, long data )
{
  wxListItem info;
  info.m_mask = wxLIST_MASK_DATA;
  info.m_itemId = item;
  info.m_data = data;
  m_mainWin->SetItem( info );
  return TRUE;
};

bool wxListCtrl::GetItemRect( long item, wxRectangle &rect,  int WXUNUSED(code) )
{
  m_mainWin->GetItemRect( item, rect );
  return TRUE;
};

bool wxListCtrl::GetItemPosition( long WXUNUSED(item), wxPoint& WXUNUSED(pos) ) const
{
  return 0;
};

bool wxListCtrl::SetItemPosition( long WXUNUSED(item), const wxPoint& WXUNUSED(pos) )
{
  return 0;
};

int wxListCtrl::GetItemCount(void) 
{
  return m_mainWin->GetItemCount();
};

int wxListCtrl::GetItemSpacing( bool isSmall )
{
  return m_mainWin->GetItemSpacing( isSmall );
};

int wxListCtrl::GetSelectedItemCount(void) 
{
  return m_mainWin->GetSelectedItemCount();
};

/*
wxColour wxListCtrl::GetTextColour(void) const
{
};

void wxListCtrl::SetTextColour(const wxColour& WXUNUSED(col))
{
};
*/

long wxListCtrl::GetTopItem(void)
{
  return 0;
};

long wxListCtrl::GetNextItem( long item, int geom, int state )
{
  return m_mainWin->GetNextItem( item, geom, state );
};

wxImageList *wxListCtrl::GetImageList(int which)
{
  if (which == wxIMAGE_LIST_NORMAL)
  {
    return m_imageListNormal;
  }
  else if (which == wxIMAGE_LIST_SMALL)
  {
    return m_imageListSmall;
  }
  else if (which == wxIMAGE_LIST_STATE)
  {
    return m_imageListState;
  };
  return NULL;
};

void wxListCtrl::SetImageList( wxImageList *imageList, int which )
{
  m_mainWin->SetImageList( imageList, which );
};

bool wxListCtrl::Arrange( int WXUNUSED(flag) )
{
  return 0;
};

bool wxListCtrl::DeleteItem( long item )
{
  m_mainWin->DeleteItem( item );
  return TRUE;
};

bool wxListCtrl::DeleteAllItems(void)
{
  m_mainWin->DeleteAllItems();
  return TRUE;
};

bool wxListCtrl::DeleteColumn( int col )
{
  m_mainWin->DeleteColumn( col );
  return TRUE;
};

/*
wxText& wxListCtrl::Edit( long WXUNUSED(item ) )
{
};
*/

bool wxListCtrl::EnsureVisible( long item )
{
  m_mainWin->EnsureVisible( item );
  return TRUE;
};

long wxListCtrl::FindItem( long start, const wxString& str,  bool partial )
{
  return m_mainWin->FindItem( start, str, partial );
};

long wxListCtrl::FindItem( long start, long data )
{
  return m_mainWin->FindItem( start, data );
};

long wxListCtrl::FindItem( long WXUNUSED(start), const wxPoint& WXUNUSED(pt), 
                           int WXUNUSED(direction))
{
  return 0;
};

long wxListCtrl::HitTest( const wxPoint &point, int &flags )
{
  return m_mainWin->HitTest( (int)point.x, (int)point.y, flags );
};

long wxListCtrl::InsertItem( wxListItem& info )
{
  m_mainWin->InsertItem( info );
  return 0;
};

long wxListCtrl::InsertItem( long index, const wxString &label )
{
  wxListItem info;
  info.m_text = label;
  info.m_mask = wxLIST_MASK_TEXT;
  info.m_itemId = index;
  return InsertItem( info );
};

long wxListCtrl::InsertItem( long index, int imageIndex )
{
  wxListItem info;
  info.m_mask = wxLIST_MASK_IMAGE;
  info.m_image = imageIndex;
  info.m_itemId = index;
  return InsertItem( info );
};

long wxListCtrl::InsertItem( long index, const wxString &label, int imageIndex )
{
  wxListItem info;
  info.m_text = label;
  info.m_image = imageIndex;
  info.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
  info.m_itemId = index;
  return InsertItem( info );
};

long wxListCtrl::InsertColumn( long col, wxListItem &item )
{
  m_mainWin->InsertColumn( col, item );
  return 0;
};

long wxListCtrl::InsertColumn( long col, const wxString &heading,
                               int format, int width )
{
  wxListItem item;
  item.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT;
  item.m_text = heading;
  if (width >= -2)
  {
    item.m_mask |= wxLIST_MASK_WIDTH;
    item.m_width = width;
  }
;
  item.m_format = format;

  return InsertColumn( col, item );
};

bool wxListCtrl::ScrollList( int WXUNUSED(dx), int WXUNUSED(dy) )
{
  return 0;
};

// Sort items.
// fn is a function which takes 3 long arguments: item1, item2, data.
// item1 is the long data associated with a first item (NOT the index).
// item2 is the long data associated with a second item (NOT the index).
// data is the same value as passed to SortItems.
// The return value is a negative number if the first item should precede the second
// item, a positive number of the second item should precede the first,
// or zero if the two items are equivalent.
// data is arbitrary data to be passed to the sort function.

bool wxListCtrl::SortItems( wxListCtrlCompare fn, long data )
{
  m_mainWin->SortItems( fn, data );
  return TRUE;
};

void wxListCtrl::OnIdle( wxIdleEvent &event )
{
  if (!m_mainWin->m_dirty) return;

  int cw = 0;
  int ch = 0;
  GetClientSize( &cw, &ch );
  
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
  
  if (GetWindowStyleFlag() & wxLC_REPORT)
  {
    m_headerWin->GetPosition( &x, &y );
    m_headerWin->GetSize( &w, &h );
    if ((x != 0) || (y != 0) || (w != cw) || (h != 23))
      m_headerWin->SetSize( 0, 0, cw, 23 );
      
    m_mainWin->GetPosition( &x, &y );
    m_mainWin->GetSize( &w, &h );
    if ((x != 0) || (y != 24) || (w != cw) || (h != ch-24))
      m_mainWin->SetSize( 0, 24, cw, ch-24 );
  }
  else
  {
    m_mainWin->GetPosition( &x, &y );
    m_mainWin->GetSize( &w, &h );
    if ((x != 0) || (y != 24) || (w != cw) || (h != ch))
      m_mainWin->SetSize( 0, 0, cw, ch );
  };
  
  m_mainWin->CalculatePositions();
  m_mainWin->RealizeChanges();
  m_mainWin->m_dirty = FALSE;
  m_mainWin->Refresh();
};


