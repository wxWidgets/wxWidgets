/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "listctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/dcscreen.h"
#include "wx/app.h"
#include "wx/listctrl.h"
#include "wx/generic/imaglist.h"

//-----------------------------------------------------------------------------
//  wxListItemData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListItemData,wxObject);

wxListItemData::wxListItemData()
{
    m_image = -1;
    m_data = 0;
    m_xpos = 0;
    m_ypos = 0;
    m_width = 0;
    m_height = 0;
    m_colour = wxBLACK;
}

wxListItemData::wxListItemData( const wxListItem &info )
{
    m_image = -1;
    m_data = 0;
    m_colour = info.m_colour;
    SetItem( info );
}

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
}

void wxListItemData::SetText( const wxString &s )
{
    m_text = s;
}

void wxListItemData::SetImage( int image )
{
    m_image = image;
}

void wxListItemData::SetData( long data )
{
    m_data = data;
}

void wxListItemData::SetPosition( int x, int y )
{
    m_xpos = x;
    m_ypos = y;
}

void wxListItemData::SetSize( int width, int height )
{
    if (width != -1) m_width = width;
    if (height != -1) m_height = height;
}

void wxListItemData::SetColour( wxColour *col )
{
    m_colour = col;
}

bool wxListItemData::HasImage() const
{
    return (m_image >= 0);
}

bool wxListItemData::HasText() const
{
    return (!m_text.IsNull());
}

bool wxListItemData::IsHit( int x, int y ) const
{
    return ((x >= m_xpos) && (x <= m_xpos+m_width) && (y >= m_ypos) && (y <= m_ypos+m_height));
}

void wxListItemData::GetText( wxString &s )
{
    s = m_text;
}

int wxListItemData::GetX() const
{
    return m_xpos;
}

int wxListItemData::GetY() const
{
    return m_ypos;
}

int wxListItemData::GetWidth() const
{
    return m_width;
}

int wxListItemData::GetHeight() const
{
    return m_height;
}

int wxListItemData::GetImage() const
{
    return m_image;
}

void wxListItemData::GetItem( wxListItem &info )
{
    info.m_text = m_text;
    info.m_image = m_image;
    info.m_data = m_data;
    info.m_colour = m_colour;
}

wxColour *wxListItemData::GetColour()
{
    return m_colour;
}

//-----------------------------------------------------------------------------
//  wxListHeaderData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListHeaderData,wxObject);

wxListHeaderData::wxListHeaderData()
{
    m_mask = 0;
    m_image = 0;
    m_format = 0;
    m_width = 0;
    m_xpos = 0;
    m_ypos = 0;
    m_height = 0;
}

wxListHeaderData::wxListHeaderData( const wxListItem &item )
{
    SetItem( item );
    m_xpos = 0;
    m_ypos = 0;
    m_height = 0;
}

void wxListHeaderData::SetItem( const wxListItem &item )
{
    m_mask = item.m_mask;
    m_text = item.m_text;
    m_image = item.m_image;
    m_format = item.m_format;
    m_width = item.m_width;
    if (m_width < 0) m_width = 80;
    if (m_width < 6) m_width = 6;
}

void wxListHeaderData::SetPosition( int x, int y )
{
    m_xpos = x;
    m_ypos = y;
}

void wxListHeaderData::SetHeight( int h )
{
    m_height = h;
}

void wxListHeaderData::SetWidth( int w )
{
    m_width = w;
    if (m_width < 0) m_width = 80;
    if (m_width < 6) m_width = 6;
}

void wxListHeaderData::SetFormat( int format )
{
    m_format = format;
}

bool wxListHeaderData::HasImage() const
{
    return (m_image != 0);
}

bool wxListHeaderData::HasText() const
{
    return (m_text.Length() > 0);
}

bool wxListHeaderData::IsHit( int x, int y ) const
{
    return ((x >= m_xpos) && (x <= m_xpos+m_width) && (y >= m_ypos) && (y <= m_ypos+m_height));
}

void wxListHeaderData::GetItem( wxListItem &item )
{
    item.m_mask = m_mask;
    item.m_text = m_text;
    item.m_image = m_image;
    item.m_format = m_format;
    item.m_width = m_width;
}

void wxListHeaderData::GetText( wxString &s )
{
    s =  m_text;
}

int wxListHeaderData::GetImage() const
{
    return m_image;
}

int wxListHeaderData::GetWidth() const
{
    return m_width;
}

int wxListHeaderData::GetFormat() const
{
    return m_format;
}

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
}

void wxListLineData::CalculateSize( wxDC *dc, int spacing )
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
            }
            break;
        }
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
                if (item->HasImage())
                {
                    int w = 0;
                    int h = 0;
                    m_owner->GetImageSize( item->GetImage(), w, h );
		    m_bound_all.width += 4 + w;
		    if (h > m_bound_all.height) m_bound_all.height = h;
		}
            }
            break;
        }
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
                long lh;
                dc->GetTextExtent( s, (long*) NULL, &lh );
                item->SetSize( item->GetWidth(), lh );
                m_bound_all.height = lh;
                node = node->Next();
            }
            break;
        }
    }
}

void wxListLineData::SetPosition( wxDC *dc, int x, int y, int window_width )
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
                        m_bound_hilight.x -= 5;
                        m_bound_hilight.y -= 5;
                        m_bound_hilight.width += 9;
                        m_bound_hilight.height += 9;
                    }
                }
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
                    m_bound_hilight.x -= 2;
                    m_bound_hilight.y -= 2;
                    m_bound_hilight.width += 4;
                    m_bound_hilight.height += 4;
                }
            }
            break;
        }
        case wxLC_LIST:
        {
            AssignRect( m_bound_label, m_bound_all );
            m_bound_all.x -= 2;
            m_bound_all.y -= 2;
            m_bound_all.width += 4;
            m_bound_all.height += 3;
            AssignRect( m_bound_hilight, m_bound_all );
            AssignRect( m_bound_icon, 0, 0, 0, 0 );
            wxNode *node = m_items.First();
            if (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();
                if (item->HasImage())
		{
                    m_bound_icon.x = m_bound_all.x + 2;
                    m_bound_icon.y = m_bound_all.y + 2;
		    int w;
		    int h;
                    m_owner->GetImageSize( item->GetImage(), w, h );
                    m_bound_icon.width = w;
                    m_bound_icon.height = h;
		    m_bound_label.x += 4 + w;
		    m_bound_label.width -= 4 + w;
		}
	    }
            break;
        }
        case wxLC_REPORT:
        {
            long lw,lh;
            dc->GetTextExtent( "H", &lw, &lh );
            m_bound_all.x = 0;
            m_bound_all.y -= 0;
            m_bound_all.height = lh+3;
            m_bound_all.width = window_width;
            AssignRect( m_bound_hilight, m_bound_all );
            AssignRect( m_bound_label, m_bound_all );
            AssignRect( m_bound_icon, 0, 0, 0, 0 );
            wxNode *node = m_items.First();
            if (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();
                wxString s;
                item->GetText( s );
		if (s.IsEmpty()) s = _T("H");
                long lw,lh;
                dc->GetTextExtent( s, &lw, &lh );
		m_bound_label.width = lw;
		m_bound_label.height = lh;
		if (item->HasImage())
		{
                    m_bound_icon.x = m_bound_all.x + 2;
                    m_bound_icon.y = m_bound_all.y + 2;
		    int w;
		    int h;
                    m_owner->GetImageSize( item->GetImage(), w, h );
                    m_bound_icon.width = w;
                    m_bound_icon.height = h;
		    m_bound_label.x += 4 + w;
		}
	    }
            break;
        }
    }
}

void wxListLineData::SetColumnPosition( int index, int x )
{
    int i = index;
    wxNode *node = m_items.Nth( i );
    if (node)
    {
        wxListItemData *item = (wxListItemData*)node->Data();
        item->SetPosition( x, m_bound_all.y+1 );
    }
}

void wxListLineData::GetSize( int &width, int &height )
{
    width = m_bound_all.width;
    height = m_bound_all.height;
}

void wxListLineData::GetExtent( int &x, int &y, int &width, int &height )
{
    x = m_bound_all.x;
    y = m_bound_all.y;
    width = m_bound_all.width;
    height = m_bound_all.height;
}

void wxListLineData::GetLabelExtent( int &x, int &y, int &width, int &height )
{
    x = m_bound_label.x;
    y = m_bound_label.y;
    width = m_bound_label.width;
    height = m_bound_label.height;
}

void wxListLineData::GetRect( wxRect &rect )
{
    AssignRect( rect, m_bound_all );
}

long wxListLineData::IsHit( int x, int y )
{
    wxNode *node = m_items.First();
    if (node)
    {
        wxListItemData *item = (wxListItemData*)node->Data();
        if (item->HasImage() && IsInRect( x, y, m_bound_icon )) return wxLIST_HITTEST_ONITEMICON;
        if (item->HasText() && IsInRect( x, y, m_bound_label )) return wxLIST_HITTEST_ONITEMLABEL;
//      if (!(item->HasImage() || item->HasText())) return 0;
    }
    // if there is no icon or text = empty
    if (IsInRect( x, y, m_bound_all )) return wxLIST_HITTEST_ONITEMICON;
    return 0;
}

void wxListLineData::InitItems( int num )
{
    for (int i = 0; i < num; i++) m_items.Append( new wxListItemData() );
}

void wxListLineData::SetItem( int index, const wxListItem &info )
{
    wxNode *node = m_items.Nth( index );
    if (node)
    {
       wxListItemData *item = (wxListItemData*)node->Data();
       item->SetItem( info );
    }
}

void wxListLineData::GetItem( int index, wxListItem &info )
{
    int i = index;
    wxNode *node = m_items.Nth( i );
    if (node)
    {
        wxListItemData *item = (wxListItemData*)node->Data();
        item->GetItem( info );
    }
}

void wxListLineData::GetText( int index, wxString &s )
{
    int i = index;
    wxNode *node = m_items.Nth( i );
    s = "";
    if (node)
    {
        wxListItemData *item = (wxListItemData*)node->Data();
        item->GetText( s );
    }
}

void wxListLineData::SetText( int index, const wxString s )
{
    int i = index;
    wxNode *node = m_items.Nth( i );
    if (node)
    {
        wxListItemData *item = (wxListItemData*)node->Data();
        item->SetText( s );
    }
}

int wxListLineData::GetImage( int index )
{
    int i = index;
    wxNode *node = m_items.Nth( i );
    if (node)
    {
        wxListItemData *item = (wxListItemData*)node->Data();
        return item->GetImage();
    }
    return -1;
}

void wxListLineData::DoDraw( wxDC *dc, bool hilight, bool paintBG )
{
    long dev_x = dc->LogicalToDeviceX( m_bound_all.x-2 );
    long dev_y = dc->LogicalToDeviceY( m_bound_all.y-2 );
    long dev_w = dc->LogicalToDeviceXRel( m_bound_all.width+4 );
    long dev_h = dc->LogicalToDeviceYRel( m_bound_all.height+4 );
    
    if (!m_owner->IsExposed( dev_x, dev_y, dev_w, dev_h ))
    { 
        return;
    }

    if (paintBG)
    {
        if (hilight)
        {
            dc->SetBrush( * m_hilightBrush );
            dc->SetPen( * wxTRANSPARENT_PEN );
        }
        else
        {
            dc->SetBrush( * wxWHITE_BRUSH );
            dc->SetPen( * wxTRANSPARENT_PEN );
        }
        dc->DrawRectangle( m_bound_hilight.x, m_bound_hilight.y,
                           m_bound_hilight.width, m_bound_hilight.height );
    }
    
    dc->SetBackgroundMode(wxTRANSPARENT);
    if (m_mode == wxLC_REPORT)
    {
        wxString s;
        wxNode *node = m_items.First();
        while (node)
        {
            wxListItemData *item = (wxListItemData*)node->Data();
            dc->SetClippingRegion( item->GetX(), item->GetY(), item->GetWidth()-3, item->GetHeight() );
            int x = item->GetX();
            if (item->HasImage())
            {
                int y = 0;
                m_owner->DrawImage( item->GetImage(), dc, x, item->GetY() );
                m_owner->GetImageSize( item->GetImage(), x, y );
                x += item->GetX() + 5;
            }
            if (item->HasText())
            {
                item->GetText( s );
                if (hilight)
                    dc->SetTextForeground( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
                else
                    dc->SetTextForeground( *item->GetColour() );
                dc->DrawText( s, x, item->GetY() );
            }
            dc->DestroyClippingRegion();
            node = node->Next();
        }
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
            }
            if (item->HasText())
            {
                wxString s;
                item->GetText( s );
                if (hilight)
                    dc->SetTextForeground( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
                else
                    dc->SetTextForeground( * item->GetColour() );
                dc->DrawText( s, m_bound_label.x, m_bound_label.y );
            }
        }
    }
}

void wxListLineData::Hilight( bool on )
{
    if (on == m_hilighted) return;
    if (on)
        m_owner->SelectLine( this );
    else
        m_owner->DeselectLine( this );
    m_hilighted = on;
}

void wxListLineData::ReverseHilight( void )
{
    m_hilighted = !m_hilighted;
    if (m_hilighted)
        m_owner->SelectLine( this );
    else
        m_owner->DeselectLine( this );
}

void wxListLineData::DrawRubberBand( wxDC *dc, bool on )
{
    if (on)
    {
        dc->SetPen( * wxBLACK_PEN );
        dc->SetBrush( * wxTRANSPARENT_BRUSH );
        dc->DrawRectangle( m_bound_hilight.x, m_bound_hilight.y,
                           m_bound_hilight.width, m_bound_hilight.height );
    }
}

void wxListLineData::Draw( wxDC *dc )
{
    DoDraw( dc, m_hilighted, m_hilighted );
}

bool wxListLineData::IsInRect( int x, int y, const wxRect &rect )
{
    return ((x >= rect.x) && (x <= rect.x+rect.width) && 
            (y >= rect.y) && (y <= rect.y+rect.height));
}

bool wxListLineData::IsHilighted( void )
{
    return m_hilighted;
}

void wxListLineData::AssignRect( wxRect &dest, int x, int y, int width, int height )
{
    dest.x = x;
    dest.y = y;
    dest.width = width;
    dest.height = height;
}

void wxListLineData::AssignRect( wxRect &dest, const wxRect &source )
{
    dest.x = source.x;
    dest.y = source.y;
    dest.width = source.width;
    dest.height = source.height;
}

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
    m_owner = (wxListMainWindow *) NULL;
    m_currentCursor = (wxCursor *) NULL;
    m_resizeCursor = (wxCursor *) NULL;
    m_isDragging = FALSE;
}

wxListHeaderWindow::wxListHeaderWindow( wxWindow *win, wxWindowID id, wxListMainWindow *owner,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name ) :
  wxWindow( win, id, pos, size, style, name )
{
    m_owner = owner;
//  m_currentCursor = wxSTANDARD_CURSOR;
    m_currentCursor = (wxCursor *) NULL;
    m_resizeCursor = new wxCursor( wxCURSOR_SIZEWE );
    m_isDragging = FALSE;
    SetBackgroundColour( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_BTNFACE ) );
}

wxListHeaderWindow::~wxListHeaderWindow( void )
{
    delete m_resizeCursor;
}

void wxListHeaderWindow::DoDrawRect( wxDC *dc, int x, int y, int w, int h )
{
    const int m_corner = 1;

    dc->SetBrush( *wxTRANSPARENT_BRUSH );

    dc->SetPen( *wxBLACK_PEN );
    dc->DrawLine( x+w-m_corner+1, y, x+w, y+h );  // right (outer)
    dc->DrawRectangle( x, y+h, w+1, 1 );          // bottom (outer)

    wxPen pen( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_BTNSHADOW ), 1, wxSOLID );
  
    dc->SetPen( pen );
    dc->DrawLine( x+w-m_corner, y, x+w-1, y+h );  // right (inner)
    dc->DrawRectangle( x+1, y+h-1, w-2, 1 );      // bottom (inner)

    dc->SetPen( *wxWHITE_PEN );
    dc->DrawRectangle( x, y, w-m_corner+1, 1 );   // top (outer)
    dc->DrawRectangle( x, y, 1, h );              // left (outer)
    dc->DrawLine( x, y+h-1, x+1, y+h-1 );
    dc->DrawLine( x+w-1, y, x+w-1, y+1 );
}

void wxListHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    dc.BeginDrawing();

    dc.SetFont( GetFont() );

    int w = 0;
    int h = 0;
    int x = 0;
    int y = 0;
    GetClientSize( &w, &h );

    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetTextForeground( *wxBLACK );
    if (m_foregroundColour.Ok()) dc.SetTextForeground( m_foregroundColour );

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
    }
    dc.EndDrawing();
}

void wxListHeaderWindow::DrawCurrent()
{
    int x1 = m_currentX;
    int y1 = 0;
    int x2 = m_currentX-1;
    int y2 = 0;
    int dummy;
    m_owner->GetClientSize( &dummy, &y2 );
    ClientToScreen( &x1, &y1 );
    m_owner->ClientToScreen( &x2, &y2 );

    wxScreenDC dc;
    dc.SetLogicalFunction( wxINVERT );
    dc.SetPen( wxPen( *wxBLACK, 2, wxSOLID ) );
    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    dc.DrawLine( x1, y1, x2, y2 );

    dc.SetLogicalFunction( wxCOPY );

    dc.SetPen( wxNullPen );
    dc.SetBrush( wxNullBrush );
}

void wxListHeaderWindow::OnMouse( wxMouseEvent &event )
{
    int x = event.GetX();
    int y = event.GetY();
    if (m_isDragging)
    {
        DrawCurrent();
        if (event.ButtonUp())
        {
            ReleaseMouse();
            m_isDragging = FALSE;
            m_owner->SetColumnWidth( m_column, m_currentX-m_minX );
        }
        else
        {
            int size_x = 0;
            int dummy;
            GetClientSize( &size_x, & dummy );
            if (x > m_minX+7)
                m_currentX = x;
            else
                m_currentX = m_minX+7;
            if (m_currentX > size_x-7) m_currentX = size_x-7;
            DrawCurrent();
        }
        return;
    }

    m_minX = 0;
    bool hit_border = FALSE;
    int xpos = 0;
    for (int j = 0; j < m_owner->GetColumnCount(); j++)
    {
        xpos += m_owner->GetColumnWidth( j );
        m_column = j;
        if ((abs(x-xpos) < 3) && (y < 22))
        {
            hit_border = TRUE;
            break;
        }
	if (x-xpos < 0)
	{
	    break;
	}
        m_minX = xpos;
    }

    if (event.LeftDown())
    {
        if (hit_border)
	{
            m_isDragging = TRUE;
            m_currentX = x;
            DrawCurrent();
            CaptureMouse();
            return;
	}
	else
	{
            wxListEvent le( wxEVT_COMMAND_LIST_COL_CLICK, GetParent()->GetId() );
            le.SetEventObject( GetParent() );
	    le.m_col = m_column;
            GetParent()->GetEventHandler()->ProcessEvent( le );
	    return;
	}
    }

    if (event.Moving())
    {
        if (hit_border)
        {
            if (m_currentCursor == wxSTANDARD_CURSOR) SetCursor( * m_resizeCursor );
            m_currentCursor = m_resizeCursor;
        }
        else
        {
            if (m_currentCursor != wxSTANDARD_CURSOR) SetCursor( * wxSTANDARD_CURSOR );
            m_currentCursor = wxSTANDARD_CURSOR;
        }
    }
}

void wxListHeaderWindow::OnSetFocus( wxFocusEvent &WXUNUSED(event) )
{
    m_owner->SetFocus();
}

//-----------------------------------------------------------------------------
// wxListRenameTimer (internal)
//-----------------------------------------------------------------------------

wxListRenameTimer::wxListRenameTimer( wxListMainWindow *owner )
{
    m_owner = owner;
}

void wxListRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}

//-----------------------------------------------------------------------------
// wxListTextCtrl (internal)
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListTextCtrl,wxTextCtrl);

BEGIN_EVENT_TABLE(wxListTextCtrl,wxTextCtrl)
    EVT_CHAR           (wxListTextCtrl::OnChar)
    EVT_KILL_FOCUS     (wxListTextCtrl::OnKillFocus)
END_EVENT_TABLE()

wxListTextCtrl::wxListTextCtrl( wxWindow *parent, const wxWindowID id,
    bool *accept, wxString *res, wxListMainWindow *owner,
    const wxString &value, const wxPoint &pos, const wxSize &size,
    int style, const wxValidator& validator, const wxString &name ) :
  wxTextCtrl( parent, id, value, pos, size, style, validator, name )
{
    m_res = res;
    m_accept = accept;
    m_owner = owner;
    (*m_accept) = FALSE;
    (*m_res) = "";
    m_startValue = value;
}

void wxListTextCtrl::OnChar( wxKeyEvent &event )
{
    if (event.m_keyCode == WXK_RETURN)
    {
        (*m_accept) = TRUE;
        (*m_res) = GetValue();
	m_owner->SetFocus();
        return;
    }
    if (event.m_keyCode == WXK_ESCAPE)
    {
        (*m_accept) = FALSE;
        (*m_res) = "";
	m_owner->SetFocus();
        return;
    }
    event.Skip();
}

void wxListTextCtrl::OnKillFocus( wxFocusEvent &WXUNUSED(event) )
{
    if (wxPendingDelete.Member(this)) return;

    wxPendingDelete.Append(this);
    
    if ((*m_accept) && ((*m_res) != m_startValue))
        m_owner->OnRenameAccept();
}

//-----------------------------------------------------------------------------
//  wxListMainWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListMainWindow,wxScrolledWindow);

BEGIN_EVENT_TABLE(wxListMainWindow,wxScrolledWindow)
  EVT_PAINT          (wxListMainWindow::OnPaint)
  EVT_SIZE           (wxListMainWindow::OnSize)
  EVT_MOUSE_EVENTS   (wxListMainWindow::OnMouse)
  EVT_CHAR           (wxListMainWindow::OnChar)
  EVT_KEY_DOWN       (wxListMainWindow::OnKeyDown)
  EVT_SET_FOCUS      (wxListMainWindow::OnSetFocus)
  EVT_KILL_FOCUS     (wxListMainWindow::OnKillFocus)
END_EVENT_TABLE()

wxListMainWindow::wxListMainWindow()
{
    m_mode = 0;
    m_lines.DeleteContents( TRUE );
    m_columns.DeleteContents( TRUE );
    m_current = (wxListLineData *) NULL;
    m_visibleLines = 0;
    m_hilightBrush = (wxBrush *) NULL;
    m_xScroll = 0;
    m_yScroll = 0;
    m_dirty = TRUE;
    m_small_image_list = (wxImageList *) NULL;
    m_normal_image_list = (wxImageList *) NULL;
    m_small_spacing = 30;
    m_normal_spacing = 40;
    m_hasFocus = FALSE;
    m_usedKeys = TRUE;
    m_lastOnSame = FALSE;
    m_renameTimer = new wxListRenameTimer( this );
    m_isCreated = FALSE;
    m_dragCount = 0;
}

wxListMainWindow::wxListMainWindow( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name ) :
  wxScrolledWindow( parent, id, pos, size, style|wxHSCROLL|wxVSCROLL, name )
{
    m_mode = style;
    m_lines.DeleteContents( TRUE );
    m_columns.DeleteContents( TRUE );
    m_current = (wxListLineData *) NULL;
    m_dirty = TRUE;
    m_visibleLines = 0;
    m_hilightBrush = new wxBrush( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT), wxSOLID );
    m_small_image_list = (wxImageList *) NULL;
    m_normal_image_list = (wxImageList *) NULL;
    m_small_spacing = 30;
    m_normal_spacing = 40;
    m_hasFocus = FALSE;
    m_dragCount = 0;
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
    }
    SetScrollbars( m_xScroll, m_yScroll, 0, 0, 0, 0 );

    m_usedKeys = TRUE;
    m_lastOnSame = FALSE;
    m_renameTimer = new wxListRenameTimer( this );
    m_renameAccept = FALSE;

    SetBackgroundColour( *wxWHITE );
}

wxListMainWindow::~wxListMainWindow()
{
    if (m_hilightBrush) delete m_hilightBrush;
    
    delete m_renameTimer;
}

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
        wxRect rect(
          dc.LogicalToDeviceX(x-3),
          dc.LogicalToDeviceY(y-3),
          dc.LogicalToDeviceXRel(w+6),
          dc.LogicalToDeviceXRel(h+6) );
        Refresh( TRUE, &rect );
    }
}

void wxListMainWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    // Note: a wxPaintDC must be constructed even if no drawing is
    // done (a Windows requirement).
    wxPaintDC dc( this );
    PrepareDC( dc );

    if (m_dirty) return;
    
    if (m_lines.GetCount() == 0) return;

    dc.BeginDrawing();

    dc.SetFont( GetFont() );
    
    if (m_mode & wxLC_REPORT)
    {
        int lineSpacing = 0;
        wxListLineData *line = (wxListLineData*)m_lines.First()->Data();
        int dummy = 0;
        line->GetSize( dummy, lineSpacing );
        lineSpacing += 1;
	
        int y_s = m_yScroll*GetScrollPos( wxVERTICAL );
	
        wxNode *node = m_lines.Nth( y_s / lineSpacing );
	for (int i = 0; i < m_visibleLines+2; i++)
	{
	    if (!node) break;
	    
            line = (wxListLineData*)node->Data();
            line->Draw( &dc );
            node = node->Next();
	}
    }
    else
    {
        wxNode *node = m_lines.First();
        while (node)
        {
            wxListLineData *line = (wxListLineData*)node->Data();
            line->Draw( &dc );
            node = node->Next();
        }
    }
    
    if (m_current) m_current->DrawRubberBand( &dc, m_hasFocus );

    dc.EndDrawing();
}

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
        }
        node = node->Next();
    }
}

void wxListMainWindow::SendNotify( wxListLineData *line, wxEventType command )
{
    wxListEvent le( command, GetParent()->GetId() );
    le.SetEventObject( GetParent() );
    le.m_itemIndex = GetIndexOfLine( line );
    line->GetItem( 0, le.m_item );
    GetParent()->GetEventHandler()->ProcessEvent( le );
}

void wxListMainWindow::FocusLine( wxListLineData *WXUNUSED(line) )
{
//  SendNotify( line, wxEVT_COMMAND_LIST_ITEM_FOCUSSED );
}

void wxListMainWindow::UnfocusLine( wxListLineData *WXUNUSED(line) )
{
//  SendNotify( line, wxEVT_COMMAND_LIST_ITEM_UNFOCUSSED );
}

void wxListMainWindow::SelectLine( wxListLineData *line )
{
    SendNotify( line, wxEVT_COMMAND_LIST_ITEM_SELECTED );
}

void wxListMainWindow::DeselectLine( wxListLineData *line )
{
    SendNotify( line, wxEVT_COMMAND_LIST_ITEM_DESELECTED );
}

void wxListMainWindow::DeleteLine( wxListLineData *line )
{
    SendNotify( line, wxEVT_COMMAND_LIST_DELETE_ITEM );
}

/* *** */

void wxListMainWindow::EditLabel( long item )
{
    wxNode *node = m_lines.Nth( item );
    wxCHECK_RET( node, _T("wrong index in wxListCtrl::Edit()") );
    
    m_currentEdit = (wxListLineData*) node->Data();

    wxListEvent le( wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, GetParent()->GetId() );
    le.SetEventObject( GetParent() );
    le.m_itemIndex = GetIndexOfLine( m_currentEdit );
    m_currentEdit->GetItem( 0, le.m_item );
    GetParent()->GetEventHandler()->ProcessEvent( le );
    
    if (!le.IsAllowed())
        return;
    
    // We have to call this here because the label in
    // question might just have been added and no screen
    // update taken place.
    if (m_dirty) wxYield();

    wxString s;
    m_currentEdit->GetText( 0, s );
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    m_currentEdit->GetLabelExtent( x, y, w, h );
    
    wxClientDC dc(this);
    PrepareDC( dc );
    x = dc.LogicalToDeviceX( x );
    y = dc.LogicalToDeviceY( y );

    wxListTextCtrl *text = new wxListTextCtrl(
      this, -1, &m_renameAccept, &m_renameRes, this, s, wxPoint(x-4,y-4), wxSize(w+11,h+8) );
    text->SetFocus();
}

void wxListMainWindow::OnRenameTimer()
{
    wxCHECK_RET( m_current, _T("invalid m_current") );
    
    Edit( m_lines.IndexOf( m_current ) );
}

void wxListMainWindow::OnRenameAccept()
{
    wxListEvent le( wxEVT_COMMAND_LIST_END_LABEL_EDIT, GetParent()->GetId() );
    le.SetEventObject( GetParent() );
    le.m_itemIndex = GetIndexOfLine( m_currentEdit );
    m_currentEdit->GetItem( 0, le.m_item );
    le.m_item.m_text = m_renameRes;
    GetParent()->GetEventHandler()->ProcessEvent( le );
    
    if (!le.IsAllowed()) return;
    
    wxListItem info;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = le.m_itemIndex;
    info.m_text = m_renameRes;
    info.m_colour = le.m_item.m_colour;
    SetItem( info );
}

void wxListMainWindow::OnMouse( wxMouseEvent &event )
{
    if (GetParent()->GetEventHandler()->ProcessEvent( event)) return;

    if (!m_current) return;
    if (m_dirty) return;
    if ( !(event.Dragging() || event.ButtonDown() || event.LeftUp() || event.ButtonDClick()) ) return;

    wxClientDC dc(this);
    PrepareDC(dc);
    long x = dc.DeviceToLogicalX( (long)event.GetX() );
    long y = dc.DeviceToLogicalY( (long)event.GetY() );
    
    /* Did we actually hit an item ? */
    long hitResult = 0;
    wxNode *node = m_lines.First();
    wxListLineData *line = (wxListLineData *) NULL;
    while (node)
    {
        line = (wxListLineData*)node->Data();
        hitResult = line->IsHit( x, y );
        if (hitResult) break;
        line = (wxListLineData *) NULL;
        node = node->Next();
    }

    if (event.Dragging())
    {
        if (m_dragCount == 0)
	    m_dragStart = wxPoint(x,y);
	
        m_dragCount++;
	
	if (m_dragCount != 3) return;
	
	int command = wxEVT_COMMAND_LIST_BEGIN_DRAG;
	if (event.RightIsDown()) command = wxEVT_COMMAND_LIST_BEGIN_RDRAG;
	
        wxListEvent le( command, GetParent()->GetId() );
        le.SetEventObject( GetParent() );
	le.m_pointDrag = m_dragStart;
	GetParent()->GetEventHandler()->ProcessEvent( le );
	
	return;
    }
    else
    {
        m_dragCount = 0;
    }

    if (!line) return;

    if (event.ButtonDClick())
    {
        m_usedKeys = FALSE;
        m_lastOnSame = FALSE;
        m_renameTimer->Stop();
    
        SendNotify( line, wxEVT_COMMAND_LIST_ITEM_ACTIVATED );
    
       return;
    }

    if (event.LeftUp() && m_lastOnSame)
    {
        m_usedKeys = FALSE;
        if ((line == m_current) &&
            (hitResult == wxLIST_HITTEST_ONITEMLABEL) &&
            (m_mode & wxLC_EDIT_LABELS)  )
        {
            m_renameTimer->Start( 100, TRUE );
        }
        m_lastOnSame = FALSE;
        return;
    }

    if (event.RightDown())
    {
        SendNotify( line, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK );
        return;
    }
  
    if (event.MiddleDown())
    {
        SendNotify( line, wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK );
        return;
    }
  
    if (event.LeftDown())
    {
        m_usedKeys = FALSE;
        wxListLineData *oldCurrent = m_current;
        if (m_mode & wxLC_SINGLE_SEL)
        {
            m_current = line;
            HilightAll( FALSE );
            m_current->ReverseHilight();
            RefreshLine( m_current );
        }
        else
        {
            if (event.ShiftDown())
            {
                m_current = line;
                m_current->ReverseHilight();
                RefreshLine( m_current );
            }
            else if (event.ControlDown())
            {
                m_current = line;
		
                int numOfCurrent = -1;
                node = m_lines.First();
                while (node)
                {
                    wxListLineData *test_line = (wxListLineData*)node->Data();
                    numOfCurrent++;
                    if (test_line == oldCurrent) break;
                    node = node->Next();
                }
		
                int numOfLine = -1;
                node = m_lines.First();
                while (node)
                {
                    wxListLineData *test_line = (wxListLineData*)node->Data();
                    numOfLine++;
                    if (test_line == line) break;
                    node = node->Next();
                }

                if (numOfLine < numOfCurrent)
                {  
	            int i = numOfLine; 
		    numOfLine = numOfCurrent; 
		    numOfCurrent = i; 
	        }
	    
                wxNode *node = m_lines.Nth( numOfCurrent );
                for (int i = 0; i <= numOfLine-numOfCurrent; i++)
                {
                    wxListLineData *test_line= (wxListLineData*)node->Data();
                    test_line->Hilight(TRUE);
                    RefreshLine( test_line );
                    node = node->Next();
                }
            }
            else
            {
                m_current = line;
                HilightAll( FALSE );
                m_current->ReverseHilight();
                RefreshLine( m_current );
            }
        }
        if (m_current != oldCurrent)
        {
            RefreshLine( oldCurrent );
            UnfocusLine( oldCurrent );
            FocusLine( m_current );
        }
        m_lastOnSame = (m_current == oldCurrent);
        return;
    }
}

void wxListMainWindow::MoveToFocus()
{
    if (!m_current) return;
  
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
        int y_s = m_yScroll*GetScrollPos( wxVERTICAL );
        if ((y > y_s) && (y+h < y_s+h_p)) return;
        if (y-y_s < 5) { Scroll( -1, (y-5-h_p/2)/m_yScroll ); Refresh(); }
        if (y+h+5 > y_s+h_p) { Scroll( -1, (y+h-h_p/2+h+15)/m_yScroll); Refresh(); }
    }
    else
    {
        int x_s = m_xScroll*GetScrollPos( wxHORIZONTAL );
        if ((x > x_s) && (x+w < x_s+w_p)) return;
        if (x-x_s < 5) { Scroll( (x-5)/m_xScroll, -1 ); Refresh(); }
        if (x+w-5 > x_s+w_p) { Scroll( (x+w-w_p+15)/m_xScroll, -1 ); Refresh(); }
    }
}

void wxListMainWindow::OnArrowChar( wxListLineData *newCurrent, bool shiftDown )
{
    if ((m_mode & wxLC_SINGLE_SEL) || (m_usedKeys == FALSE)) m_current->Hilight( FALSE );
    wxListLineData *oldCurrent = m_current;
    m_current = newCurrent;
    MoveToFocus();
    if (shiftDown || (m_mode & wxLC_SINGLE_SEL)) m_current->Hilight( TRUE );
    RefreshLine( m_current );
    RefreshLine( oldCurrent );
    FocusLine( m_current );
    UnfocusLine( oldCurrent );
}

void wxListMainWindow::OnKeyDown( wxKeyEvent &event )
{
    wxWindow *parent = GetParent();
  
    /* we propagate the key event up */
    wxKeyEvent ke( wxEVT_KEY_DOWN );
    ke.m_shiftDown = event.m_shiftDown;
    ke.m_controlDown = event.m_controlDown;
    ke.m_altDown = event.m_altDown;
    ke.m_metaDown = event.m_metaDown;
    ke.m_keyCode = event.m_keyCode;
    ke.m_x = event.m_x;
    ke.m_y = event.m_y;
    ke.SetEventObject( parent );
    if (parent->GetEventHandler()->ProcessEvent( ke )) return;
    
    event.Skip();
}
  
void wxListMainWindow::OnChar( wxKeyEvent &event )
{
    wxWindow *parent = GetParent();
  
    /* we send a list_key event up */
    wxListEvent le( wxEVT_COMMAND_LIST_KEY_DOWN, GetParent()->GetId() );
    le.m_code = event.KeyCode();
    le.SetEventObject( parent );
    parent->GetEventHandler()->ProcessEvent( le );

    /* we propagate the char event up */
    wxKeyEvent ke( wxEVT_CHAR );
    ke.m_shiftDown = event.m_shiftDown;
    ke.m_controlDown = event.m_controlDown;
    ke.m_altDown = event.m_altDown;
    ke.m_metaDown = event.m_metaDown;
    ke.m_keyCode = event.m_keyCode;
    ke.m_x = event.m_x;
    ke.m_y = event.m_y;
    ke.SetEventObject( parent );
    if (parent->GetEventHandler()->ProcessEvent( ke )) return;
  
    if (event.KeyCode() == WXK_TAB)
    {
        wxNavigationKeyEvent nevent;
        nevent.SetDirection( !event.ShiftDown() );
        nevent.SetCurrentFocus( m_parent );
        if (m_parent->GetEventHandler()->ProcessEvent( nevent )) return;
    }
  
    /* no item -> nothing to do */
    if (!m_current)
    {
        event.Skip();
        return;
    }

    switch (event.KeyCode())
    {
        case WXK_UP:
        {
            wxNode *node = m_lines.Member( m_current )->Previous();
            if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
            break;
        }
        case WXK_DOWN:
        {
            wxNode *node = m_lines.Member( m_current )->Next();
            if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
            break;
        }
        case WXK_END:
        {
            wxNode *node = m_lines.Last();
            OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
            break;
        }
        case WXK_HOME:
        {
            wxNode *node = m_lines.First();
            OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
            break;
        }
        case WXK_PRIOR:
        {
            int steps = 0;
            if (m_mode & wxLC_REPORT) 
	    { 
	        steps = m_visibleLines-1; 
	    }
            else
            {
                int pos = 0;
                wxNode *node = m_lines.First();
                for (;;) { if (m_current == (wxListLineData*)node->Data()) break; pos++; node = node->Next(); }
                steps = pos % m_visibleLines;
            }
            wxNode *node = m_lines.Member( m_current );
            for (int i = 0; i < steps; i++) if (node->Previous()) node = node->Previous();
            if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
            break;
        }
        case WXK_NEXT:
        {
            int steps = 0;
            if (m_mode & wxLC_REPORT) 
	    { 
	        steps = m_visibleLines-1; 
	    }
            else
            {
                int pos = 0; wxNode *node = m_lines.First();
                for (;;) { if (m_current == (wxListLineData*)node->Data()) break; pos++; node = node->Next(); }
                steps = m_visibleLines-(pos % m_visibleLines)-1;
            }
            wxNode *node = m_lines.Member( m_current );
            for (int i = 0; i < steps; i++) if (node->Next()) node = node->Next();
            if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
            break;
        }
        case WXK_LEFT:
        {
            if (!(m_mode & wxLC_REPORT))
            {
                wxNode *node = m_lines.Member( m_current );
                for (int i = 0; i <m_visibleLines; i++) if (node->Previous()) node = node->Previous();
                if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
            }
            break;
        }
        case WXK_RIGHT:
        {
            if (!(m_mode & wxLC_REPORT))
            {
                wxNode *node = m_lines.Member( m_current );
                for (int i = 0; i <m_visibleLines; i++) if (node->Next()) node = node->Next();
                if (node) OnArrowChar( (wxListLineData*)node->Data(), event.ShiftDown() );
            }
            break;
        }
        case WXK_SPACE:
        {
            m_current->ReverseHilight();
            RefreshLine( m_current );
            break;
        }
        case WXK_INSERT:
        {
            if (!(m_mode & wxLC_SINGLE_SEL))
            {
                wxListLineData *oldCurrent = m_current;
                m_current->ReverseHilight();
                wxNode *node = m_lines.Member( m_current )->Next();
                if (node) m_current = (wxListLineData*)node->Data();
                MoveToFocus();
                RefreshLine( oldCurrent );
                RefreshLine( m_current );
                UnfocusLine( oldCurrent );
                FocusLine( m_current );
            }
            break;
        }
        case WXK_RETURN:
        case WXK_EXECUTE:
        {
            wxListEvent le( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, GetParent()->GetId() );
            le.SetEventObject( GetParent() );
            le.m_itemIndex = GetIndexOfLine( m_current );
            m_current->GetItem( 0, le.m_item );
            GetParent()->GetEventHandler()->ProcessEvent( le );
            break;
        }
        default:
        {
            event.Skip();
            return;
        }
    }
    m_usedKeys = TRUE;
}

#ifdef __WXGTK__
extern wxWindow *g_focusWindow;
#endif

void wxListMainWindow::OnSetFocus( wxFocusEvent &WXUNUSED(event) )
{
    m_hasFocus = TRUE;
    RefreshLine( m_current );

    if (!GetParent()) return;
    
#ifdef __WXGTK__
    g_focusWindow = GetParent();
#endif

    wxFocusEvent event( wxEVT_SET_FOCUS, GetParent()->GetId() );
    event.SetEventObject( GetParent() );
    GetParent()->GetEventHandler()->ProcessEvent( event );
}

void wxListMainWindow::OnKillFocus( wxFocusEvent &WXUNUSED(event) )
{
    m_hasFocus = FALSE;
    RefreshLine( m_current );
}

void wxListMainWindow::OnSize( wxSizeEvent &WXUNUSED(event) )
{
/*
  We don't even allow the wxScrolledWindow::AdjustScrollbars() call
  
*/
}

void wxListMainWindow::DrawImage( int index, wxDC *dc, int x, int y )
{
    if ((m_mode & wxLC_ICON) && (m_normal_image_list))
    {
        m_normal_image_list->Draw( index, *dc, x, y, wxIMAGELIST_DRAW_TRANSPARENT );
        return;
    }
    if ((m_mode & wxLC_SMALL_ICON) && (m_small_image_list))
    {
        m_small_image_list->Draw( index, *dc, x, y, wxIMAGELIST_DRAW_TRANSPARENT );
    }
    if ((m_mode & wxLC_LIST) && (m_small_image_list))
    {
        m_small_image_list->Draw( index, *dc, x, y, wxIMAGELIST_DRAW_TRANSPARENT );
    }
    if ((m_mode & wxLC_REPORT) && (m_small_image_list))
    {
        m_small_image_list->Draw( index, *dc, x, y, wxIMAGELIST_DRAW_TRANSPARENT );
        return;
    }
}

void wxListMainWindow::GetImageSize( int index, int &width, int &height )
{
    if ((m_mode & wxLC_ICON) && (m_normal_image_list))
    {
        m_normal_image_list->GetSize( index, width, height );
        return;
    }
    if ((m_mode & wxLC_SMALL_ICON) && (m_small_image_list))
    {
        m_small_image_list->GetSize( index, width, height );
        return;
    }
    if ((m_mode & wxLC_LIST) && (m_small_image_list))
    {
        m_small_image_list->GetSize( index, width, height );
        return;
    }
    if ((m_mode & wxLC_REPORT) && (m_small_image_list))
    {
        m_small_image_list->GetSize( index, width, height );
        return;
    }
    width = 0;
    height = 0;
}

int wxListMainWindow::GetTextLength( wxString &s )
{
    wxClientDC dc( this );
    long lw = 0;
    long lh = 0;
    dc.GetTextExtent( s, &lw, &lh );
    return lw + 6;
}

int wxListMainWindow::GetIndexOfLine( const wxListLineData *line )
{
    int i = 0;
    wxNode *node = m_lines.First();
    while (node)
    {
        if (line == (wxListLineData*)node->Data()) return i;
        i++;
        node = node->Next();
    }
    return -1;
}

void wxListMainWindow::SetImageList( wxImageList *imageList, int which )
{
    m_dirty = TRUE;
    if (which == wxIMAGE_LIST_NORMAL) m_normal_image_list = imageList;
    if (which == wxIMAGE_LIST_SMALL) m_small_image_list = imageList;
}

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
    }
}

int wxListMainWindow::GetItemSpacing( bool isSmall )
{
    if (isSmall) return m_small_spacing; else return m_normal_spacing;
}

void wxListMainWindow::SetColumn( int col, wxListItem &item )
{
    m_dirty = TRUE;
    wxNode *node = m_columns.Nth( col );
    if (node)
    {
        if (item.m_width == wxLIST_AUTOSIZE_USEHEADER) item.m_width = GetTextLength( item.m_text )+7;
        wxListHeaderData *column = (wxListHeaderData*)node->Data();
        column->SetItem( item );
    }
    wxListCtrl *lc = (wxListCtrl*) GetParent();
    if (lc->m_headerWin) lc->m_headerWin->Refresh();
}

void wxListMainWindow::SetColumnWidth( int col, int width )
{
    if (!(m_mode & wxLC_REPORT)) return;

    m_dirty = TRUE;

    wxNode *node = (wxNode*) NULL;

    if (width == wxLIST_AUTOSIZE_USEHEADER) width = 80;
    if (width == wxLIST_AUTOSIZE)
    {
        wxClientDC dc(this);
        dc.SetFont( GetFont() );
        int max = 10;
        node = m_lines.First();
        while (node)
        {
            wxListLineData *line = (wxListLineData*)node->Data();
            wxNode *n = line->m_items.Nth( col );
            if (n)
            {
                wxListItemData *item = (wxListItemData*)n->Data();
		int current = 0, ix = 0, iy = 0;
		long lx = 0, ly = 0;
		if (item->HasImage())
		{
                    GetImageSize( item->GetImage(), ix, iy );
		    current = ix + 5;
		}
		if (item->HasText())
		{
		    wxString str;
		    item->GetText( str );
		    dc.GetTextExtent( str, &lx, &ly );
		    current += lx;
		}
		if (current > max) max = current;
            }
            node = node->Next();
        }
	width = max+10;
    }

    node = m_columns.Nth( col );
    if (node)
    {
        wxListHeaderData *column = (wxListHeaderData*)node->Data();
        column->SetWidth( width );
    }

    node = m_lines.First();
    while (node)
    {
        wxListLineData *line = (wxListLineData*)node->Data();
        wxNode *n = line->m_items.Nth( col );
        if (n)
        {
            wxListItemData *item = (wxListItemData*)n->Data();
            item->SetSize( width, -1 );
        }
        node = node->Next();
    }

    wxListCtrl *lc = (wxListCtrl*) GetParent();
    if (lc->m_headerWin) lc->m_headerWin->Refresh();
}

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
    }
}

int wxListMainWindow::GetColumnWidth( int col )
{
    wxNode *node = m_columns.Nth( col );
    if (node)
    {
        wxListHeaderData *column = (wxListHeaderData*)node->Data();
        return column->GetWidth();
    }
    else
    {
        return 0; 
    }
}

int wxListMainWindow::GetColumnCount()
{
    return m_columns.Number();
}

int wxListMainWindow::GetCountPerPage()
{
    return m_visibleLines;
}

void wxListMainWindow::SetItem( wxListItem &item )
{
    m_dirty = TRUE;
    wxNode *node = m_lines.Nth( item.m_itemId );
    if (node)
    {
        wxListLineData *line = (wxListLineData*)node->Data();
        if (m_mode & wxLC_REPORT) item.m_width = GetColumnWidth( item.m_col )-3;
        line->SetItem( item.m_col, item );
    }
}

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
            if (oldCurrent) RefreshLine( oldCurrent );
        }
    }

    if (stateMask & wxLIST_STATE_SELECTED)
    {
        bool on = state & wxLIST_STATE_SELECTED;
        if (!on && (m_mode & wxLC_SINGLE_SEL)) return;

        wxNode *node = m_lines.Nth( item );
        if (node)
        {
            wxListLineData *line = (wxListLineData*)node->Data();
            if (m_mode & wxLC_SINGLE_SEL)
            {
                UnfocusLine( m_current );
                m_current = line;
                FocusLine( m_current );
                if (oldCurrent) oldCurrent->Hilight( FALSE );
                RefreshLine( m_current );
                if (oldCurrent) RefreshLine( oldCurrent );
            }
            bool on = state & wxLIST_STATE_SELECTED;
	    if (on != line->IsHilighted())
	    {
                line->Hilight( on );
                RefreshLine( line );
	    }
        }
    }
}

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
        }
    }
    if (stateMask & wxLIST_STATE_SELECTED)
    {
        wxNode *node = m_lines.Nth( item );
        if (node)
        {
            wxListLineData *line = (wxListLineData*)node->Data();
            if (line->IsHilighted()) ret |= wxLIST_STATE_FOCUSED;
        }
    }
    return ret;
}

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
    }
}

int wxListMainWindow::GetItemCount()
{
    return m_lines.Number();
}

void wxListMainWindow::GetItemRect( long index, wxRect &rect )
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
    }
}

bool wxListMainWindow::GetItemPosition(long item, wxPoint& pos)
{
    wxNode *node = m_lines.Nth( item );
    if (node)
    {
        wxRect rect;
        wxListLineData *line = (wxListLineData*)node->Data();
        line->GetRect( rect );
        pos.x = rect.x;
        pos.y = rect.y;
    }
    else
    {
       pos.x = 0;
       pos.y = 0;
    }
    return TRUE;
}

int wxListMainWindow::GetSelectedItemCount()
{
    int ret = 0;
    wxNode *node = m_lines.First();
    while (node)
    {
        wxListLineData *line = (wxListLineData*)node->Data();
        if (line->IsHilighted()) ret++;
        node = node->Next();
    }
    return ret;
}

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
    }
}

long wxListMainWindow::GetMode() const
{
    return m_mode;
}

void wxListMainWindow::CalculatePositions()
{
    if (!m_lines.First()) return;

    wxClientDC dc( this );
    dc.SetFont( GetFont() );

    int iconSpacing = 0;
    if (m_mode & wxLC_ICON) iconSpacing = m_normal_spacing;
    if (m_mode & wxLC_SMALL_ICON) iconSpacing = m_small_spacing;
  
    // we take the first line (which also can be an icon or
    // an a text item in wxLC_ICON and wxLC_LIST modes) to
    // measure the size of the line
  
    int lineWidth = 0;
    int lineHeight = 0;
    int lineSpacing = 0;

    wxListLineData *line = (wxListLineData*)m_lines.First()->Data();
    line->CalculateSize( &dc, iconSpacing );
    int dummy = 0;
    line->GetSize( dummy, lineSpacing );
    lineSpacing += 4;

    int clientWidth = 0;
    int clientHeight = 0;

    if (m_mode & wxLC_REPORT)
    {
        int x = 4;
        int y = 1;
        int entireHeight = m_lines.Number() * lineSpacing + 2;
        int scroll_pos = GetScrollPos( wxVERTICAL );
        SetScrollbars( m_xScroll, m_yScroll, 0, (entireHeight+15) / m_yScroll, 0, scroll_pos, TRUE );
        GetClientSize( &clientWidth, &clientHeight );

        wxNode* node = m_lines.First();
        while (node)
        {
            wxListLineData *line = (wxListLineData*)node->Data();
            line->CalculateSize( &dc, iconSpacing );
            line->SetPosition( &dc, x, y, clientWidth );
            int col_x = 2;
            for (int i = 0; i < GetColumnCount(); i++)
            {
                line->SetColumnPosition( i, col_x );
                col_x += GetColumnWidth( i );
            }
            y += lineSpacing;  // one pixel blank line between items
            node = node->Next();
        }
	m_visibleLines = clientHeight / lineSpacing;
    }
    else
    {
        // at first we try without any scrollbar. if the items don't
        // fit into the window, we recalculate after subtracting an
        // approximated 15 pt for the horizontal scrollbar
    
        GetSize( &clientWidth, &clientHeight );
	clientHeight -= 4;  // sunken frame

        int entireWidth = 0;

        for (int tries = 0; tries < 2; tries++)
        {
            entireWidth = 0;
            int x = 5;  // painting is done at x-2
            int y = 5;  // painting is done at y-2
            int maxWidth = 0;
            m_visibleLines = 0;
	    int m_currentVisibleLines = 0;
            wxNode *node = m_lines.First();
            while (node)
            {
	        m_currentVisibleLines++;
                wxListLineData *line = (wxListLineData*)node->Data();
                line->CalculateSize( &dc, iconSpacing );
                line->SetPosition( &dc, x, y, clientWidth );
                line->GetSize( lineWidth, lineHeight );
                if (lineWidth > maxWidth) maxWidth = lineWidth;
                y += lineSpacing;
                if (y+lineSpacing-6 >= clientHeight) // -6 for earlier "line breaking"
                {
	            if (m_currentVisibleLines > m_visibleLines)
		        m_visibleLines = m_currentVisibleLines;
	            m_currentVisibleLines = 0;
                    y = 5;
                    x += maxWidth+6;
                    entireWidth += maxWidth+6;
                    maxWidth = 0;
                }
                node = node->Next();
                if (!node) entireWidth += maxWidth;
                if ((tries == 0) && (entireWidth > clientWidth))
                {
                    clientHeight -= 15; // scrollbar height
                    m_visibleLines = 0;
	            m_currentVisibleLines = 0;
                    break;
                }
                if (!node) tries = 1;  // everything fits, no second try required
            }
        }
//        m_visibleLines = (5+clientHeight+6) / (lineSpacing); // +6 for earlier "line breaking"
	
        int scroll_pos = GetScrollPos( wxHORIZONTAL );
        SetScrollbars( m_xScroll, m_yScroll, (entireWidth+15) / m_xScroll, 0, scroll_pos, 0, TRUE );
    }
}

void wxListMainWindow::RealizeChanges( void )
{
    if (!m_current)
    {
        wxNode *node = m_lines.First();
        if (node) m_current = (wxListLineData*)node->Data();
    }
    if (m_current)
    {
        FocusLine( m_current );
        if (m_mode & wxLC_SINGLE_SEL) m_current->Hilight( TRUE );
    }
}

long wxListMainWindow::GetNextItem( long item, int WXUNUSED(geometry), int state )
{
    long ret = 0;
    if (item > 0) ret = item;
    if(ret >= GetItemCount()) return -1;
    wxNode *node = m_lines.Nth( ret );
    while (node)
    {
        wxListLineData *line = (wxListLineData*)node->Data();
        if ((state & wxLIST_STATE_FOCUSED) && (line == m_current)) return ret;
        if ((state & wxLIST_STATE_SELECTED) && (line->IsHilighted())) return ret;
        if (!state) return ret;
        ret++;
        node = node->Next();
    }
    return -1;
}

void wxListMainWindow::DeleteItem( long index )
{
    m_dirty = TRUE;
    wxNode *node = m_lines.Nth( index );
    if (node)
    {
        wxListLineData *line = (wxListLineData*)node->Data();
        if (m_current == line) m_current = (wxListLineData *) NULL;
        DeleteLine( line );
        m_lines.DeleteNode( node );
    }
}

void wxListMainWindow::DeleteColumn( int col )
{
    wxCHECK_RET( col < (int)m_columns.GetCount(),
               _T("attempting to delete inexistent column in wxListView") );

    m_dirty = TRUE;
    wxNode *node = m_columns.Nth( col );
    if (node) m_columns.DeleteNode( node );
}

void wxListMainWindow::DeleteAllItems( void )
{
    m_dirty = TRUE;
    m_current = (wxListLineData *) NULL;
    wxNode *node = m_lines.First();
    while (node)
    {
        wxListLineData *line = (wxListLineData*)node->Data();
        DeleteLine( line );
        node = node->Next();
    }
    m_lines.Clear();
}

void wxListMainWindow::DeleteEverything( void )
{
    m_dirty = TRUE;
    m_current = (wxListLineData *) NULL;
    wxNode *node = m_lines.First();
    while (node)
    {
        wxListLineData *line = (wxListLineData*)node->Data();
        DeleteLine( line );
        node = node->Next();
    }
    m_lines.Clear();
    m_current = (wxListLineData *) NULL;
    m_columns.Clear();
}

void wxListMainWindow::EnsureVisible( long index )
{
    // We have to call this here because the label in
    // question might just have been added and no screen
    // update taken place.
    if (m_dirty) wxYield();

    wxListLineData *oldCurrent = m_current;
    m_current = (wxListLineData *) NULL;
    int i = index;
    wxNode *node = m_lines.Nth( i );
    if (node) m_current = (wxListLineData*)node->Data();
    if (m_current) MoveToFocus();
    m_current = oldCurrent;
}

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
    }
    return -1;
}

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
    }
    return -1;
}

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
        }
        node = node->Next();
        count++;
    }
    return -1;
}

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
    {
        line->InitItems( 1 );
    }
    
    line->SetItem( 0, item );
    if ((item.m_itemId >= 0) && (item.m_itemId < (int)m_lines.GetCount()))
    {
        wxNode *node = m_lines.Nth( item.m_itemId );
        if (node) m_lines.Insert( node, line );
    }
    else
    {
        m_lines.Append( line );
    }
}

void wxListMainWindow::InsertColumn( long col, wxListItem &item )
{
    m_dirty = TRUE;
    if (m_mode & wxLC_REPORT)
    {
        if (item.m_width == wxLIST_AUTOSIZE_USEHEADER) item.m_width = GetTextLength( item.m_text );
        wxListHeaderData *column = new wxListHeaderData( item );
        if ((col >= 0) && (col < (int)m_columns.GetCount()))
        {
            wxNode *node = m_columns.Nth( col );
            if (node)
                 m_columns.Insert( node, column );
        }
        else
        {
            m_columns.Append( column );
        }
    }
}

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
}

void wxListMainWindow::SortItems( wxListCtrlCompare fn, long data )
{
    list_ctrl_compare_func_2 = fn;
    list_ctrl_compare_data = data;
    m_lines.Sort( list_ctrl_compare_func_1 );
}

// -------------------------------------------------------------------------------------
// wxListItem
// -------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListItem, wxObject)

wxListItem::wxListItem()
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

IMPLEMENT_DYNAMIC_CLASS(wxListEvent, wxNotifyEvent)

wxListEvent::wxListEvent( wxEventType commandType, int id ):
  wxNotifyEvent( commandType, id )
{
    m_code = 0;
    m_itemIndex = 0;
    m_oldItemIndex = 0;
    m_col = 0;
    m_cancelled = FALSE;
    m_pointDrag.x = 0;
    m_pointDrag.y = 0;
}

// -------------------------------------------------------------------------------------
// wxListCtrl
// -------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListCtrl, wxControl)

BEGIN_EVENT_TABLE(wxListCtrl,wxControl)
  EVT_SIZE          (wxListCtrl::OnSize)
  EVT_IDLE          (wxListCtrl::OnIdle)
END_EVENT_TABLE()

wxListCtrl::wxListCtrl()
{
    m_imageListNormal = (wxImageList *) NULL;
    m_imageListSmall = (wxImageList *) NULL;
    m_imageListState = (wxImageList *) NULL;
    m_mainWin = (wxListMainWindow*) NULL;
    m_headerWin = (wxListHeaderWindow*) NULL;
}

wxListCtrl::~wxListCtrl()
{
}

bool wxListCtrl::Create( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      long style, const wxValidator &validator,
      const wxString &name )
{
    m_imageListNormal = (wxImageList *) NULL;
    m_imageListSmall = (wxImageList *) NULL;
    m_imageListState = (wxImageList *) NULL;
    m_mainWin = (wxListMainWindow*) NULL;
    m_headerWin = (wxListHeaderWindow*) NULL;

    long s = style;

    if ((s & wxLC_REPORT == 0) &&
        (s & wxLC_LIST == 0) &&
        (s & wxLC_ICON == 0))
    {
        s = s | wxLC_LIST;
    }

    bool ret = wxControl::Create( parent, id, pos, size, s, name );

#if wxUSE_VALIDATORS
    SetValidator( validator );
#endif
  
    if (s & wxSUNKEN_BORDER) s -= wxSUNKEN_BORDER;

    m_mainWin = new wxListMainWindow( this, -1, wxPoint(0,0), size, s );

    if (HasFlag(wxLC_REPORT))
        m_headerWin = new wxListHeaderWindow( this, -1, m_mainWin, wxPoint(0,0), wxSize(size.x,23), wxTAB_TRAVERSAL );
    else
        m_headerWin = (wxListHeaderWindow *) NULL;

    SetBackgroundColour( *wxWHITE );

    return ret;
}

void wxListCtrl::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    /* handled in OnIdle */

    if (m_mainWin) m_mainWin->m_dirty = TRUE;
}

void wxListCtrl::SetSingleStyle( long style, bool add )
{
    long flag = GetWindowStyle();

    if (add)
    {
        if (style & wxLC_MASK_TYPE)  flag = flag & ~wxLC_MASK_TYPE;
        if (style & wxLC_MASK_ALIGN) flag = flag & ~wxLC_MASK_ALIGN;
        if (style & wxLC_MASK_SORT) flag = flag & ~wxLC_MASK_SORT;
    }

    if (add)
    {
        flag |= style;
    }
    else
    {
        if (flag & style) flag -= style;
    }

    SetWindowStyleFlag( flag );
}

void wxListCtrl::SetWindowStyleFlag( long flag )
{
    if (m_mainWin)
    {
        m_mainWin->DeleteEverything();

        int width = 0;
        int height = 0;
        GetClientSize( &width, &height );

        m_mainWin->SetMode( flag );

        if (flag & wxLC_REPORT)
        {
            if (!HasFlag(wxLC_REPORT))
            {
                if (!m_headerWin)
                {
                    m_headerWin = new wxListHeaderWindow( this, -1, m_mainWin, 
		      wxPoint(0,0), wxSize(width,23), wxTAB_TRAVERSAL );
		    if (HasFlag(wxLC_NO_HEADER))
		        m_headerWin->Show( FALSE );
                }
                else
                {  
		    if (flag & wxLC_NO_HEADER)
		        m_headerWin->Show( FALSE );
		    else
                        m_headerWin->Show( TRUE );
                } 
            }
        }
        else
        {
            if (HasFlag(wxLC_REPORT) && !(HasFlag(wxLC_NO_HEADER)))
            {
                m_headerWin->Show( FALSE );
            }
	}
    }
    
    wxWindow::SetWindowStyleFlag( flag );
}

bool wxListCtrl::GetColumn(int col, wxListItem &item) const
{
    m_mainWin->GetColumn( col, item );
    return TRUE;
}

bool wxListCtrl::SetColumn( int col, wxListItem& item )
{
    m_mainWin->SetColumn( col, item );
    return TRUE;
}

int wxListCtrl::GetColumnWidth( int col ) const
{
    return m_mainWin->GetColumnWidth( col );
}

bool wxListCtrl::SetColumnWidth( int col, int width )
{
    m_mainWin->SetColumnWidth( col, width );
    return TRUE;
}

int wxListCtrl::GetCountPerPage() const
{
  return m_mainWin->GetCountPerPage();  // different from Windows ?
}

bool wxListCtrl::GetItem( wxListItem &info ) const
{
    m_mainWin->GetItem( info );
    return TRUE;
}

bool wxListCtrl::SetItem( wxListItem &info )
{
    m_mainWin->SetItem( info );
    return TRUE;
}

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
    };
    m_mainWin->SetItem(info);
    return TRUE;
}

int wxListCtrl::GetItemState( long item, long stateMask ) const
{
    return m_mainWin->GetItemState( item, stateMask );
}

bool wxListCtrl::SetItemState( long item, long state, long stateMask )
{
    m_mainWin->SetItemState( item, state, stateMask );
    return TRUE;
}

bool wxListCtrl::SetItemImage( long item, int image, int WXUNUSED(selImage) )
{
    wxListItem info;
    info.m_image = image;
    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_itemId = item;
    m_mainWin->SetItem( info );
    return TRUE;
}

wxString wxListCtrl::GetItemText( long item ) const
{
    wxListItem info;
    info.m_itemId = item;
    m_mainWin->GetItem( info );
    return info.m_text;
}

void wxListCtrl::SetItemText( long item, const wxString &str )
{
    wxListItem info;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = item;
    info.m_text = str;
    m_mainWin->SetItem( info );
}

long wxListCtrl::GetItemData( long item ) const
{
    wxListItem info;
    info.m_itemId = item;
    m_mainWin->GetItem( info );
    return info.m_data;
}

bool wxListCtrl::SetItemData( long item, long data )
{
    wxListItem info;
    info.m_mask = wxLIST_MASK_DATA;
    info.m_itemId = item;
    info.m_data = data;
    m_mainWin->SetItem( info );
    return TRUE;
}

bool wxListCtrl::GetItemRect( long item, wxRect &rect,  int WXUNUSED(code) ) const
{
    m_mainWin->GetItemRect( item, rect );
    return TRUE;
}

bool wxListCtrl::GetItemPosition( long item, wxPoint& pos ) const
{
    m_mainWin->GetItemPosition( item, pos );
    return TRUE;
}

bool wxListCtrl::SetItemPosition( long WXUNUSED(item), const wxPoint& WXUNUSED(pos) )
{
    return 0;
}

int wxListCtrl::GetItemCount() const
{
    return m_mainWin->GetItemCount();
}

int wxListCtrl::GetColumnCount() const
{
    return m_mainWin->GetColumnCount();
}

void wxListCtrl::SetItemSpacing( int spacing, bool isSmall )
{
    m_mainWin->SetItemSpacing( spacing, isSmall );
}

int wxListCtrl::GetItemSpacing( bool isSmall ) const
{
    return m_mainWin->GetItemSpacing( isSmall );
}

int wxListCtrl::GetSelectedItemCount() const
{
    return m_mainWin->GetSelectedItemCount();
}

/*
wxColour wxListCtrl::GetTextColour() const
{
}

void wxListCtrl::SetTextColour(const wxColour& WXUNUSED(col))
{
}
*/

long wxListCtrl::GetTopItem() const
{
    return 0;
}

long wxListCtrl::GetNextItem( long item, int geom, int state ) const
{
    return m_mainWin->GetNextItem( item, geom, state );
}

wxImageList *wxListCtrl::GetImageList(int which) const
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
    }
    return (wxImageList *) NULL;
}

void wxListCtrl::SetImageList( wxImageList *imageList, int which )
{
    m_mainWin->SetImageList( imageList, which );
}

bool wxListCtrl::Arrange( int WXUNUSED(flag) )
{
    return 0;
}

bool wxListCtrl::DeleteItem( long item )
{
    m_mainWin->DeleteItem( item );
    return TRUE;
}

bool wxListCtrl::DeleteAllItems()
{
    m_mainWin->DeleteAllItems();
    return TRUE;
}

bool wxListCtrl::DeleteAllColumns()
{
    for ( size_t n = 0; n < m_mainWin->m_columns.GetCount(); n++ )
        DeleteColumn(n);
	
    return TRUE;
}

void wxListCtrl::ClearAll()
{
    m_mainWin->DeleteEverything();
}

bool wxListCtrl::DeleteColumn( int col )
{
    m_mainWin->DeleteColumn( col );
    return TRUE;
}

void wxListCtrl::Edit( long item )
{
    m_mainWin->Edit( item );
}

bool wxListCtrl::EnsureVisible( long item )
{
    m_mainWin->EnsureVisible( item );
    return TRUE;
}

long wxListCtrl::FindItem( long start, const wxString& str,  bool partial )
{
    return m_mainWin->FindItem( start, str, partial );
}

long wxListCtrl::FindItem( long start, long data )
{
    return m_mainWin->FindItem( start, data );
}

long wxListCtrl::FindItem( long WXUNUSED(start), const wxPoint& WXUNUSED(pt),
                           int WXUNUSED(direction))
{
    return 0;
}

long wxListCtrl::HitTest( const wxPoint &point, int &flags )
{
    return m_mainWin->HitTest( (int)point.x, (int)point.y, flags );
}

long wxListCtrl::InsertItem( wxListItem& info )
{
    m_mainWin->InsertItem( info );
    return 0;
}

long wxListCtrl::InsertItem( long index, const wxString &label )
{
    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    return InsertItem( info );
}

long wxListCtrl::InsertItem( long index, int imageIndex )
{
    wxListItem info;
    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_image = imageIndex;
    info.m_itemId = index;
    return InsertItem( info );
}

long wxListCtrl::InsertItem( long index, const wxString &label, int imageIndex )
{
    wxListItem info;
    info.m_text = label;
    info.m_image = imageIndex;
    info.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
    info.m_itemId = index;
    return InsertItem( info );
}

long wxListCtrl::InsertColumn( long col, wxListItem &item )
{
    m_mainWin->InsertColumn( col, item );
    return 0;
}

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
    item.m_format = format;

    return InsertColumn( col, item );
}

bool wxListCtrl::ScrollList( int WXUNUSED(dx), int WXUNUSED(dy) )
{
    return 0;
}

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
}

void wxListCtrl::OnIdle( wxIdleEvent &WXUNUSED(event) )
{
    if (!m_mainWin->m_dirty) return;

    int cw = 0;
    int ch = 0;
    GetClientSize( &cw, &ch );

    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    if (HasFlag(wxLC_REPORT) && !HasFlag(wxLC_NO_HEADER))
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
    }

    m_mainWin->CalculatePositions();
    m_mainWin->RealizeChanges();
    m_mainWin->m_dirty = FALSE;
    m_mainWin->Refresh();
}

bool wxListCtrl::SetBackgroundColour( const wxColour &colour )
{
    if ( !wxWindow::SetBackgroundColour( colour ) )
        return FALSE;

    if (m_mainWin)
    {
        m_mainWin->SetBackgroundColour( colour );
        m_mainWin->m_dirty = TRUE;
    }
    
    if (m_headerWin)
    {
//        m_headerWin->SetBackgroundColour( colour );
    }

    return TRUE;
}

bool wxListCtrl::SetForegroundColour( const wxColour &colour )
{
    if ( !wxWindow::SetForegroundColour( colour ) )
        return FALSE;
    
    if (m_mainWin)
    {
        m_mainWin->SetForegroundColour( colour );
        m_mainWin->m_dirty = TRUE;
    }
    
    if (m_headerWin)
    {
        m_headerWin->SetForegroundColour( colour );
    }

    return TRUE;
}

bool wxListCtrl::SetFont( const wxFont &font )
{
    if ( !wxWindow::SetFont( font ) )
        return FALSE;
    
    if (m_mainWin)
    {
        m_mainWin->SetFont( font );
        m_mainWin->m_dirty = TRUE;
    }
    
    if (m_headerWin)
    {
        m_headerWin->SetFont( font );
    }

    return TRUE;
}

