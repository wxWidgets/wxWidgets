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
    #pragma implementation "listctrlbase.h"
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
#include "wx/dynarray.h"

#ifdef __WXGTK__
#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"
#endif

#ifndef wxUSE_GENERIC_LIST_EXTENSIONS
#define wxUSE_GENERIC_LIST_EXTENSIONS 1
#endif

// ============================================================================
// private classes
// ============================================================================

//-----------------------------------------------------------------------------
//  wxListItemData (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListItemData : public wxObject
{
public:
    wxString   m_text;
    int        m_image;
    long       m_data;
    int        m_xpos,m_ypos;
    int        m_width,m_height;

    wxListItemAttr *m_attr;

public:
    wxListItemData();
    ~wxListItemData() { delete m_attr; }

    wxListItemData( const wxListItem &info );
    void SetItem( const wxListItem &info );
    void SetText( const wxString &s );
    void SetImage( int image );
    void SetData( long data );
    void SetPosition( int x, int y );
    void SetSize( int width, int height );
    bool HasImage() const;
    bool HasText() const;
    bool IsHit( int x, int y ) const;
    void GetText( wxString &s );
    const wxString& GetText() { return m_text; }
    int GetX( void ) const;
    int GetY( void ) const;
    int GetWidth() const;
    int GetHeight() const;
    int GetImage() const;
    void GetItem( wxListItem &info ) const;

    wxListItemAttr *GetAttributes() const { return m_attr; }

private:
    DECLARE_DYNAMIC_CLASS(wxListItemData);
};

//-----------------------------------------------------------------------------
//  wxListHeaderData (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListHeaderData : public wxObject
{
protected:
    long      m_mask;
    int       m_image;
    wxString  m_text;
    int       m_format;
    int       m_width;
    int       m_xpos,m_ypos;
    int       m_height;

public:
    wxListHeaderData();
    wxListHeaderData( const wxListItem &info );
    void SetItem( const wxListItem &item );
    void SetPosition( int x, int y );
    void SetWidth( int w );
    void SetFormat( int format );
    void SetHeight( int h );
    bool HasImage() const;
    bool HasText() const;
    bool IsHit( int x, int y ) const;
    void GetItem( wxListItem &item );
    void GetText( wxString &s );
    int GetImage() const;
    int GetWidth() const;
    int GetFormat() const;

private:
    DECLARE_DYNAMIC_CLASS(wxListHeaderData);
};

//-----------------------------------------------------------------------------
//  wxListLineData (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListLineData : public wxObject
{
public:
    wxList              m_items;
    wxRect              m_bound_all;
    wxRect              m_bound_label;
    wxRect              m_bound_icon;
    wxRect              m_bound_hilight;
    int                 m_mode;
    bool                m_hilighted;
    wxBrush            *m_hilightBrush;
    int                 m_spacing;
    wxListMainWindow   *m_owner;

    void DoDraw( wxDC *dc, bool hilight, bool paintBG );

public:
    wxListLineData() {}
    wxListLineData( wxListMainWindow *owner, int mode, wxBrush *hilightBrush );
    void CalculateSize( wxDC *dc, int spacing );
    void SetPosition( wxDC *dc, int x, int y,  int window_width );
    void SetColumnPosition( int index, int x );
    void GetSize( int &width, int &height );
    void GetExtent( int &x, int &y, int &width, int &height );
    void GetLabelExtent( int &x, int &y, int &width, int &height );
    long IsHit( int x, int y );
    void InitItems( int num );
    void SetItem( int index, const wxListItem &info );
    void GetItem( int index, wxListItem &info );
    void GetText( int index, wxString &s );
    void SetText( int index, const wxString s );
    int GetImage( int index );
    void GetRect( wxRect &rect );
    void Hilight( bool on );
    void ReverseHilight();
    void DrawRubberBand( wxDC *dc, bool on );
    void Draw( wxDC *dc );
    bool IsInRect( int x, int y, const wxRect &rect );
    bool IsHilighted();
    void AssignRect( wxRect &dest, int x, int y, int width, int height );
    void AssignRect( wxRect &dest, const wxRect &source );

private:
    void SetAttributes(wxDC *dc,
                       const wxListItemAttr *attr,
                       const wxColour& colText, const wxFont& font,
                       bool hilight);

    DECLARE_DYNAMIC_CLASS(wxListLineData);
};


WX_DECLARE_EXPORTED_OBJARRAY(wxListLineData, wxListLineDataArray);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxListLineDataArray);

//-----------------------------------------------------------------------------
//  wxListHeaderWindow (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListHeaderWindow : public wxWindow
{
protected:
    wxListMainWindow  *m_owner;
    wxCursor          *m_currentCursor;
    wxCursor          *m_resizeCursor;
    bool               m_isDragging;

    // column being resized
    int m_column;

    // divider line position in logical (unscrolled) coords
    int m_currentX;

    // minimal position beyond which the divider line can't be dragged in
    // logical coords
    int m_minX;

public:
    wxListHeaderWindow();
    virtual ~wxListHeaderWindow();

    wxListHeaderWindow( wxWindow *win,
                        wxWindowID id,
                        wxListMainWindow *owner,
                        const wxPoint &pos = wxDefaultPosition,
                        const wxSize &size = wxDefaultSize,
                        long style = 0,
                        const wxString &name = "wxlistctrlcolumntitles" );

    void DoDrawRect( wxDC *dc, int x, int y, int w, int h );
    void DrawCurrent();
    void AdjustDC(wxDC& dc);

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );

    // needs refresh
    bool m_dirty;

private:
    DECLARE_DYNAMIC_CLASS(wxListHeaderWindow)
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxListRenameTimer (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListRenameTimer: public wxTimer
{
private:
    wxListMainWindow   *m_owner;

public:
    wxListRenameTimer( wxListMainWindow *owner );
    void Notify();
};

//-----------------------------------------------------------------------------
//  wxListTextCtrl (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListTextCtrl: public wxTextCtrl
{
private:
    bool               *m_accept;
    wxString           *m_res;
    wxListMainWindow   *m_owner;
    wxString            m_startValue;

public:
    wxListTextCtrl() {}
    wxListTextCtrl( wxWindow *parent, const wxWindowID id,
                    bool *accept, wxString *res, wxListMainWindow *owner,
                    const wxString &value = "",
                    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
                    int style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString &name = "listctrltextctrl" );
    void OnChar( wxKeyEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(wxListTextCtrl);
    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
//  wxListMainWindow (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListMainWindow: public wxScrolledWindow
{
public:
    long                 m_mode;
    wxListLineDataArray  m_lines;
    wxList               m_columns;
    wxListLineData      *m_current;
    wxListLineData      *m_currentEdit;
    int                  m_visibleLines;
    wxBrush             *m_hilightBrush;
    wxColour            *m_hilightColour;
    int                  m_xScroll,m_yScroll;
    bool                 m_dirty;
    wxImageList         *m_small_image_list;
    wxImageList         *m_normal_image_list;
    int                  m_small_spacing;
    int                  m_normal_spacing;
    bool                 m_hasFocus;
    bool                 m_usedKeys;
    bool                 m_lastOnSame;
    wxTimer             *m_renameTimer;
    bool                 m_renameAccept;
    wxString             m_renameRes;
    bool                 m_isCreated;
    int                  m_dragCount;
    wxPoint              m_dragStart;

    // for double click logic
    wxListLineData      *m_lineLastClicked,
                        *m_lineBeforeLastClicked;

public:
    wxListMainWindow();
    wxListMainWindow( wxWindow *parent, wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = 0, const wxString &name = "listctrlmainwindow" );
    ~wxListMainWindow();
    void RefreshLine( wxListLineData *line );
    void OnPaint( wxPaintEvent &event );
    void HilightAll( bool on );
    void SendNotify( wxListLineData *line, wxEventType command );
    void FocusLine( wxListLineData *line );
    void UnfocusLine( wxListLineData *line );
    void SelectLine( wxListLineData *line );
    void DeselectLine( wxListLineData *line );
    void DeleteLine( wxListLineData *line );

    void EditLabel( long item );
    void Edit( long item ) { EditLabel(item); }         // deprecated
    void OnRenameTimer();
    void OnRenameAccept();

    void OnMouse( wxMouseEvent &event );
    void MoveToFocus();
    void OnArrowChar( wxListLineData *newCurrent, bool shiftDown );
    void OnChar( wxKeyEvent &event );
    void OnKeyDown( wxKeyEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnScroll(wxScrollWinEvent& event) ;

    void DrawImage( int index, wxDC *dc, int x, int y );
    void GetImageSize( int index, int &width, int &height );
    int GetIndexOfLine( const wxListLineData *line );
    int GetTextLength( wxString &s );  // should be const

    void SetImageList( wxImageList *imageList, int which );
    void SetItemSpacing( int spacing, bool isSmall = FALSE );
    int GetItemSpacing( bool isSmall = FALSE );
    void SetColumn( int col, wxListItem &item );
    void SetColumnWidth( int col, int width );
    void GetColumn( int col, wxListItem &item );
    int GetColumnWidth( int vol );
    int GetColumnCount();
    int GetCountPerPage();
    void SetItem( wxListItem &item );
    void GetItem( wxListItem &item );
    void SetItemState( long item, long state, long stateMask );
    int GetItemState( long item, long stateMask );
    int GetItemCount();
    void GetItemRect( long index, wxRect &rect );
    bool GetItemPosition( long item, wxPoint& pos );
    int GetSelectedItemCount();
    void SetMode( long mode );
    long GetMode() const;
    void CalculatePositions();
    void RealizeChanges();
    long GetNextItem( long item, int geometry, int state );
    void DeleteItem( long index );
    void DeleteAllItems();
    void DeleteColumn( int col );
    void DeleteEverything();
    void EnsureVisible( long index );
    long FindItem( long start, const wxString& str, bool partial = FALSE );
    long FindItem( long start, long data);
    long HitTest( int x, int y, int &flags );
    void InsertItem( wxListItem &item );
//    void AddItem( wxListItem &item );
    void InsertColumn( long col, wxListItem &item );
//    void AddColumn( wxListItem &item );
    void SortItems( wxListCtrlCompare fn, long data );

private:
    DECLARE_DYNAMIC_CLASS(wxListMainWindow);
    DECLARE_EVENT_TABLE()
};

// ============================================================================
// implementation
// ============================================================================

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
    m_attr = NULL;
}

wxListItemData::wxListItemData( const wxListItem &info )
{
    m_image = -1;
    m_data = 0;
    m_attr = NULL;

    SetItem( info );
}

void wxListItemData::SetItem( const wxListItem &info )
{
    if (info.m_mask & wxLIST_MASK_TEXT) m_text = info.m_text;
    if (info.m_mask & wxLIST_MASK_IMAGE) m_image = info.m_image;
    if (info.m_mask & wxLIST_MASK_DATA) m_data = info.m_data;

    if ( info.HasAttributes() )
    {
        if ( m_attr )
            *m_attr = *info.GetAttributes();
        else
            m_attr = new wxListItemAttr(*info.GetAttributes());
    }

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

void wxListItemData::GetItem( wxListItem &info ) const
{
    info.m_text = m_text;
    info.m_image = m_image;
    info.m_data = m_data;

    if ( m_attr )
    {
        if ( m_attr->HasTextColour() )
            info.SetTextColour(m_attr->GetTextColour());
        if ( m_attr->HasBackgroundColour() )
            info.SetBackgroundColour(m_attr->GetBackgroundColour());
        if ( m_attr->HasFont() )
            info.SetFont(m_attr->GetFont());
    }
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
            wxNode *node = m_items.First();
            if (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();
                wxString s = item->GetText();
                if (s.IsEmpty()) s = wxT("H");
                wxCoord lw,lh;
                dc->GetTextExtent( s, &lw, &lh );
                if (lh < 15) lh = 15;
                lw += 4;
                lh += 3;

                m_bound_all.height = m_spacing+lh;
                if (lw > m_spacing) m_bound_all.width = lw;
                m_bound_label.width = lw;
                m_bound_label.height = lh;

                if (item->HasImage())
                {
                    int w = 0;
                    int h = 0;
                    m_owner->GetImageSize( item->GetImage(), w, h );
                    m_bound_icon.width = w + 8;
                    m_bound_icon.height = h + 8;

                    if ( m_bound_icon.width > m_bound_all.width )
                        m_bound_all.width = m_bound_icon.width;
                    if ( h + lh > m_bound_all.height - 4 )
                        m_bound_all.height = h + lh + 4;
                }

                if (!item->HasText())
                {
                    m_bound_hilight.width = m_bound_icon.width;
                    m_bound_hilight.height = m_bound_icon.height;
                }
                else
                {
                    m_bound_hilight.width = m_bound_label.width;
                    m_bound_hilight.height = m_bound_label.height;
                }
            }
            break;
        }
        case wxLC_LIST:
        {
            wxNode *node = m_items.First();
            if (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();

                wxString s = item->GetText();
                if (s.IsEmpty()) s = wxT("H");
                wxCoord lw,lh;
                dc->GetTextExtent( s, &lw, &lh );
                if (lh < 15) lh = 15;
                lw += 4;
                lh += 3;
                m_bound_label.width = lw;
                m_bound_label.height = lh;

                m_bound_all.width = lw;
                m_bound_all.height = lh;

                if (item->HasImage())
                {
                    int w = 0;
                    int h = 0;
                    m_owner->GetImageSize( item->GetImage(), w, h );
                    m_bound_icon.width = w;
                    m_bound_icon.height = h;

                    m_bound_all.width += 4 + w;
                    if (h > m_bound_all.height) m_bound_all.height = h;
                }

                m_bound_hilight.width = m_bound_all.width;
                m_bound_hilight.height = m_bound_all.height;
            }
            break;
        }
        case wxLC_REPORT:
        {
            m_bound_all.width = 0;
            m_bound_all.height = 0;
            wxNode *node = m_items.First();
            if (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();
                if (item->HasImage())
                {
                    int w = 0;
                    int h = 0;
                    m_owner->GetImageSize( item->GetImage(), w, h );
                    m_bound_icon.width = w;
                    m_bound_icon.height = h;
                }
                else
                {
                    m_bound_icon.width = 0;
                    m_bound_icon.height = 0;
                }
            }
            while (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();
                wxString s = item->GetText();
                if (s.IsEmpty()) s = wxT("H");
                wxCoord lw,lh;
                dc->GetTextExtent( s, &lw, &lh );
                if (lh < 15) lh = 15;
                lw += 4;
                lh += 3;

                item->SetSize( item->GetWidth(), lh );
                m_bound_all.width += lw;
                m_bound_all.height = lh;
                node = node->Next();
            }
            m_bound_label.width = m_bound_all.width;
            m_bound_label.height = m_bound_all.height;
            break;
        }
    }
}

void wxListLineData::SetPosition( wxDC * WXUNUSED(dc),
                                  int x, int y, int window_width )
{
    m_bound_all.x = x;
    m_bound_all.y = y;
    switch (m_mode)
    {
        case wxLC_ICON:
        {
            wxNode *node = m_items.First();
            if (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();
                if (item->HasImage())
                {
                    m_bound_icon.x = m_bound_all.x + 4
                                        + (m_spacing - m_bound_icon.width)/2;
                    m_bound_icon.y = m_bound_all.y + 4;
                }
                if (item->HasText())
                {
                    if (m_bound_all.width > m_spacing)
                        m_bound_label.x = m_bound_all.x + 2;
                    else
                        m_bound_label.x = m_bound_all.x + 2 + (m_spacing/2) - (m_bound_label.width/2);
                    m_bound_label.y = m_bound_all.y + m_bound_all.height + 2 - m_bound_label.height;
                    m_bound_hilight.x = m_bound_label.x - 2;
                    m_bound_hilight.y = m_bound_label.y - 2;
                }
                else
                {
                    m_bound_hilight.x = m_bound_icon.x - 4;
                    m_bound_hilight.y = m_bound_icon.y - 4;
                }
            }
            break;
        }
        case wxLC_LIST:
        {
            m_bound_hilight.x = m_bound_all.x;
            m_bound_hilight.y = m_bound_all.y;
            m_bound_label.y = m_bound_all.y + 2;
            wxNode *node = m_items.First();
            if (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();
                if (item->HasImage())
                {
                    m_bound_icon.x = m_bound_all.x + 2;
                    m_bound_icon.y = m_bound_all.y + 2;
                    m_bound_label.x = m_bound_all.x + 6 + m_bound_icon.width;
                }
                else
                {
                    m_bound_label.x = m_bound_all.x + 2;
                }
            }
            break;
        }
        case wxLC_REPORT:
        {
            m_bound_all.x = 0;
            m_bound_all.width = window_width;
            AssignRect( m_bound_hilight, m_bound_all );
            m_bound_label.x = m_bound_all.x + 2;
            m_bound_label.y = m_bound_all.y + 2;
            wxNode *node = m_items.First();
            if (node)
            {
                wxListItemData *item = (wxListItemData*)node->Data();
                if (item->HasImage())
                {
                    m_bound_icon.x = m_bound_all.x + 2;
                    m_bound_icon.y = m_bound_all.y + 2;
                    m_bound_label.x += 4 + m_bound_icon.width;
                }
            }
            break;
        }
    }
}

void wxListLineData::SetColumnPosition( int index, int x )
{
    wxNode *node = m_items.Nth( (size_t)index );
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

void wxListLineData::SetAttributes(wxDC *dc,
                                   const wxListItemAttr *attr,
                                   const wxColour& colText,
                                   const wxFont& font,
                                   bool hilight)
{
    // don't use foregroud colour for drawing highlighted items - this might
    // make them completely invisible (and there is no way to do bit
    // arithmetics on wxColour, unfortunately)
    if ( !hilight && attr && attr->HasTextColour() )
    {
        dc->SetTextForeground(attr->GetTextColour());
    }
    else
    {
        dc->SetTextForeground(colText);
    }

    if ( attr && attr->HasFont() )
    {
        dc->SetFont(attr->GetFont());
    }
    else
    {
        dc->SetFont(font);
    }
}

void wxListLineData::DoDraw( wxDC *dc, bool hilight, bool paintBG )
{
    int dev_x = 0;
    int dev_y = 0;
    m_owner->CalcScrolledPosition( m_bound_all.x, m_bound_all.y, &dev_x, &dev_y );
    wxCoord dev_w = m_bound_all.width;
    wxCoord dev_h = m_bound_all.height;

    if (!m_owner->IsExposed( dev_x, dev_y, dev_w, dev_h ))
        return;

    wxWindow *listctrl = m_owner->GetParent();

    // default foreground colour
    wxColour colText;
    if ( hilight )
    {
        colText = wxSystemSettings::GetSystemColour( wxSYS_COLOUR_HIGHLIGHTTEXT );
    }
    else
    {
        colText = listctrl->GetForegroundColour();
    }

    // default font
    wxFont font = listctrl->GetFont();

    // VZ: currently we set the colours/fonts only once, but like this (i.e.
    //     using SetAttributes() inside the loop), it will be trivial to
    //     customize the subitems (in report mode) too.
    wxListItemData *item = (wxListItemData*)m_items.First()->Data();
    wxListItemAttr *attr = item->GetAttributes();
    SetAttributes(dc, attr, colText, font, hilight);

    bool hasBgCol = attr && attr->HasBackgroundColour();
    if ( paintBG || hasBgCol )
    {
        if (hilight)
        {
            dc->SetBrush( * m_hilightBrush );
        }
        else
        {
            if ( hasBgCol )
                dc->SetBrush(wxBrush(attr->GetBackgroundColour(), wxSOLID));
            else
                dc->SetBrush( * wxWHITE_BRUSH );
        }

        dc->SetPen( * wxTRANSPARENT_PEN );
        dc->DrawRectangle( m_bound_hilight.x, m_bound_hilight.y,
                           m_bound_hilight.width, m_bound_hilight.height );
    }

    if (m_mode == wxLC_REPORT)
    {
        wxNode *node = m_items.First();
        while (node)
        {
            wxListItemData *item = (wxListItemData*)node->Data();
            int x = item->GetX();
            if (item->HasImage())
            {
                int y = 0;
                m_owner->DrawImage( item->GetImage(), dc, x, item->GetY() );
                m_owner->GetImageSize( item->GetImage(), x, y );
                x += item->GetX() + 5;
            }
            dc->SetClippingRegion( item->GetX(), item->GetY(), item->GetWidth()-3, item->GetHeight() );
            if (item->HasText())
            {
                dc->DrawText( item->GetText(), x, item->GetY()+1 );
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
                dc->DrawText( item->GetText(), m_bound_label.x, m_bound_label.y );
            }
        }
    }
}

void wxListLineData::Hilight( bool on )
{
    if (on == m_hilighted) return;
    m_hilighted = on;
    if (on)
        m_owner->SelectLine( this );
    else
        m_owner->DeselectLine( this );
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
    m_dirty = FALSE;

    SetBackgroundColour( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_BTNFACE ) );
}

wxListHeaderWindow::~wxListHeaderWindow( void )
{
    delete m_resizeCursor;
}

void wxListHeaderWindow::DoDrawRect( wxDC *dc, int x, int y, int w, int h )
{
#ifdef __WXGTK__
    GtkStateType state = GTK_STATE_NORMAL;
    if (!m_parent->IsEnabled()) state = GTK_STATE_INSENSITIVE;
    
    x = dc->XLOG2DEV( x );
    
	gtk_paint_box (m_wxwindow->style, GTK_PIZZA(m_wxwindow)->bin_window, state, GTK_SHADOW_OUT,
		(GdkRectangle*) NULL, m_wxwindow, "button", x-1, y-1, w+2, h+2);
#else
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
#endif
}

// shift the DC origin to match the position of the main window horz
// scrollbar: this allows us to always use logical coords
void wxListHeaderWindow::AdjustDC(wxDC& dc)
{
#if wxUSE_GENERIC_LIST_EXTENSIONS
    int xpix;
    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );

    int x;
    m_owner->GetViewStart( &x, NULL );

    // account for the horz scrollbar offset
    dc.SetDeviceOrigin( -x * xpix, 0 );
#endif // wxUSE_GENERIC_LIST_EXTENSIONS
}

void wxListHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );
    AdjustDC( dc );

    dc.BeginDrawing();

    dc.SetFont( GetFont() );

    // width and height of the entire header window
    int w, h;
    GetClientSize( &w, &h );
#if wxUSE_GENERIC_LIST_EXTENSIONS
    m_owner->CalcUnscrolledPosition(w, 0, &w, NULL);
#endif // wxUSE_GENERIC_LIST_EXTENSIONS

    dc.SetBackgroundMode(wxTRANSPARENT);

    // do *not* use the listctrl colour for headers - one day we will have a
    // function to set it separately
    //dc.SetTextForeground( *wxBLACK );
    dc.SetTextForeground(wxSystemSettings::GetSystemColour( wxSYS_COLOUR_WINDOWTEXT ));

    int x = 1;          // left of the header rect
    const int y = 1;    // top
    int numColumns = m_owner->GetColumnCount();
    wxListItem item;
    for (int i = 0; i < numColumns; i++)
    {
        m_owner->GetColumn( i, item );
        int wCol = item.m_width;
        int cw = wCol - 2; // the width of the rect to draw

        int xEnd = x + wCol;

        // VZ: no, draw it normally - this is better now as we allow resizing
        //     of the last column as well
#if 0
        // let the last column occupy all available space
        if ( i == numColumns - 1 )
            cw = w-x-1;
#endif // 0

        dc.SetPen( *wxWHITE_PEN );

        DoDrawRect( &dc, x, y, cw, h-2 );
        dc.SetClippingRegion( x, y, cw-5, h-4 );
        dc.DrawText( item.m_text, x+4, y+3 );
        dc.DestroyClippingRegion();
        x += wCol;

        if (xEnd > w+5)
            break;
    }
    dc.EndDrawing();
}

void wxListHeaderWindow::DrawCurrent()
{
    int x1 = m_currentX;
    int y1 = 0;
    ClientToScreen( &x1, &y1 );

    int x2 = m_currentX-1;
    int y2 = 0;
    m_owner->GetClientSize( NULL, &y2 );
    m_owner->ClientToScreen( &x2, &y2 );

    wxScreenDC dc;
    dc.SetLogicalFunction( wxINVERT );
    dc.SetPen( wxPen( *wxBLACK, 2, wxSOLID ) );
    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    AdjustDC(dc);

    dc.DrawLine( x1, y1, x2, y2 );

    dc.SetLogicalFunction( wxCOPY );

    dc.SetPen( wxNullPen );
    dc.SetBrush( wxNullBrush );
}

void wxListHeaderWindow::OnMouse( wxMouseEvent &event )
{
    // we want to work with logical coords
#if wxUSE_GENERIC_LIST_EXTENSIONS
    int x;
    m_owner->CalcUnscrolledPosition(event.GetX(), 0, &x, NULL);
#else // !wxUSE_GENERIC_LIST_EXTENSIONS
    int x = event.GetX();
#endif // wxUSE_GENERIC_LIST_EXTENSIONS
    int y = event.GetY();

    if (m_isDragging)
    {
        // we don't draw the line beyond our window, but we allow dragging it
        // there
        int w = 0;
        GetClientSize( &w, NULL );
#if wxUSE_GENERIC_LIST_EXTENSIONS
        m_owner->CalcUnscrolledPosition(w, 0, &w, NULL);
#endif // wxUSE_GENERIC_LIST_EXTENSIONS
        w -= 6;

        // erase the line if it was drawn
        if ( m_currentX < w )
            DrawCurrent();

        if (event.ButtonUp())
        {
            ReleaseMouse();
            m_isDragging = FALSE;
            m_dirty = TRUE;
            m_owner->SetColumnWidth( m_column, m_currentX - m_minX );
        }
        else
        {
            if (x > m_minX + 7)
                m_currentX = x;
            else
                m_currentX = m_minX + 7;

            // draw in the new location
            if ( m_currentX < w )
                DrawCurrent();
        }
    }
    else // not dragging
    {
        m_minX = 0;
        bool hit_border = FALSE;

        // end of the current column
        int xpos = 0;

        // find the column where this event occured
        int countCol = m_owner->GetColumnCount();
        for (int j = 0; j < countCol; j++)
        {
            xpos += m_owner->GetColumnWidth( j );
            m_column = j;

            if ( (abs(x-xpos) < 3) && (y < 22) )
            {
                // near the column border
                hit_border = TRUE;
                break;
            }

            if ( x < xpos )
            {
                // inside the column
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
            }
            else
            {
                wxWindow *parent = GetParent();
                wxListEvent le( wxEVT_COMMAND_LIST_COL_CLICK, parent->GetId() );
                le.SetEventObject( parent );
                le.m_col = m_column;
                parent->GetEventHandler()->ProcessEvent( le );
            }
        }
        else if (event.Moving())
        {
            bool setCursor;
            if (hit_border)
            {
                setCursor = m_currentCursor == wxSTANDARD_CURSOR;
                m_currentCursor = m_resizeCursor;
            }
            else
            {
                setCursor = m_currentCursor != wxSTANDARD_CURSOR;
                m_currentCursor = wxSTANDARD_CURSOR;
            }

            if ( setCursor )
                SetCursor(*m_currentCursor);
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
    EVT_KEY_UP         (wxListTextCtrl::OnKeyUp)    
    EVT_KILL_FOCUS     (wxListTextCtrl::OnKillFocus)
END_EVENT_TABLE()

wxListTextCtrl::wxListTextCtrl( wxWindow *parent,
                                const wxWindowID id,
                                bool *accept,
                                wxString *res,
                                wxListMainWindow *owner,
                                const wxString &value,
                                const wxPoint &pos,
                                const wxSize &size,
                                int style,
                                const wxValidator& validator,
                                const wxString &name )
              : wxTextCtrl( parent, id, value, pos, size, style, validator, name )
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

        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);

        if ((*m_accept) && ((*m_res) != m_startValue))
            m_owner->OnRenameAccept();

        return;
    }
    if (event.m_keyCode == WXK_ESCAPE)
    {
        (*m_accept) = FALSE;
        (*m_res) = "";

        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);

        return;
    }

    event.Skip();
}

void wxListTextCtrl::OnKeyUp( wxKeyEvent &event )
{
    // auto-grow the textctrl:
    wxSize parentSize = m_owner->GetSize();
    wxPoint myPos = GetPosition();
    wxSize mySize = GetSize();
    int sx, sy;
    GetTextExtent(GetValue() + _T("MM"), &sx, &sy);
    if (myPos.x + sx > parentSize.x) sx = parentSize.x - myPos.x;
    if (mySize.x > sx) sx = mySize.x;
    SetSize(sx, -1);
    
    event.Skip();
}

void wxListTextCtrl::OnKillFocus( wxFocusEvent &WXUNUSED(event) )
{
    if (!wxPendingDelete.Member(this))
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
  EVT_SCROLLWIN      (wxListMainWindow::OnScroll)
END_EVENT_TABLE()

wxListMainWindow::wxListMainWindow()
{
    m_mode = 0;
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

    m_lineLastClicked =
    m_lineBeforeLastClicked = (wxListLineData *)NULL;
}

wxListMainWindow::wxListMainWindow( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name ) :
  wxScrolledWindow( parent, id, pos, size, style|wxHSCROLL|wxVSCROLL, name )
{
    m_mode = style;
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
#if wxUSE_GENERIC_LIST_EXTENSIONS
        m_xScroll = 15;
#else
        m_xScroll = 0;
#endif
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

    SetBackgroundColour( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_LISTBOX ) );
}

wxListMainWindow::~wxListMainWindow()
{
    DeleteEverything();

    if (m_hilightBrush) delete m_hilightBrush;

    delete m_renameTimer;
}

void wxListMainWindow::RefreshLine( wxListLineData *line )
{
    if (m_dirty) return;

    if (!line) return;

    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    line->GetExtent( x, y, w, h );
    CalcScrolledPosition( x, y, &x, &y );
    wxRect rect( x, y, w, h );
    Refresh( TRUE, &rect );
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
        wxPen pen(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID);
        dc.SetPen(pen);
        dc.SetBrush(* wxTRANSPARENT_BRUSH);

        wxSize clientSize = GetClientSize();

        int lineSpacing = 0;
        wxListLineData *line = &m_lines[0];
        int dummy = 0;
        line->GetSize( dummy, lineSpacing );
        lineSpacing += 1;

        int y_s = m_yScroll*GetScrollPos( wxVERTICAL );

        size_t i_to = y_s / lineSpacing + m_visibleLines+2;
        if (i_to >= m_lines.GetCount()) i_to = m_lines.GetCount();
        size_t i;
        for (i = y_s / lineSpacing; i < i_to; i++)
        {
            m_lines[i].Draw( &dc );
            // Draw horizontal rule if required
            if (GetWindowStyle() & wxLC_HRULES)
                dc.DrawLine(0, i*lineSpacing, clientSize.x, i*lineSpacing);
        }

        // Draw last horizontal rule
        if ((i > (size_t) (y_s / lineSpacing)) && (GetWindowStyle() & wxLC_HRULES))
            dc.DrawLine(0, i*lineSpacing, clientSize.x, i*lineSpacing);

        // Draw vertical rules if required
        if ((GetWindowStyle() & wxLC_VRULES) && (GetItemCount() > 0))
        {
            int col = 0;
            wxRect firstItemRect;
            wxRect lastItemRect;
            GetItemRect(0, firstItemRect);
            GetItemRect(GetItemCount() - 1, lastItemRect);
            int x = firstItemRect.GetX();
            for (col = 0; col < GetColumnCount(); col++)
            {
                int colWidth = GetColumnWidth(col);
                x += colWidth ;
                dc.DrawLine(x, firstItemRect.GetY() - 1, x, lastItemRect.GetBottom() + 1);
            }
        }
    }
    else
    {
        for (size_t i = 0; i < m_lines.GetCount(); i++)
            m_lines[i].Draw( &dc );
    }

    if (m_current) m_current->DrawRubberBand( &dc, m_hasFocus );

    dc.EndDrawing();
}

void wxListMainWindow::HilightAll( bool on )
{
    for (size_t i = 0; i < m_lines.GetCount(); i++)
    {
        wxListLineData *line = &m_lines[i];
        if (line->IsHilighted() != on)
        {
            line->Hilight( on );
            RefreshLine( line );
        }
    }
}

void wxListMainWindow::SendNotify( wxListLineData *line, wxEventType command )
{
    wxListEvent le( command, GetParent()->GetId() );
    le.SetEventObject( GetParent() );
    le.m_itemIndex = GetIndexOfLine( line );
    line->GetItem( 0, le.m_item );
    GetParent()->GetEventHandler()->ProcessEvent( le );
//    GetParent()->GetEventHandler()->AddPendingEvent( le );
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
    wxCHECK_RET( ((size_t)item < m_lines.GetCount()),
                 wxT("wrong index in wxListCtrl::Edit()") );

    m_currentEdit = &m_lines[(size_t)item];

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
    wxCHECK_RET( m_current, wxT("invalid m_current") );

    Edit( m_lines.Index( *m_current ) );
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
    info.SetTextColour(le.m_item.GetTextColour());
    SetItem( info );
}

void wxListMainWindow::OnMouse( wxMouseEvent &event )
{
    event.SetEventObject( GetParent() );
    if (GetParent()->GetEventHandler()->ProcessEvent( event)) return;

    if (!m_current) return;
    if (m_dirty) return;
    if ( !(event.Dragging() || event.ButtonDown() || event.LeftUp() || event.ButtonDClick()) ) return;

    int x = event.GetX();
    int y = event.GetY();
    CalcUnscrolledPosition( x, y, &x, &y );

    /* Did we actually hit an item ? */
    long hitResult = 0;
    wxListLineData *line = (wxListLineData *) NULL;
    for (size_t i = 0; i < m_lines.GetCount(); i++)
    {
        line = &m_lines[i];
        hitResult = line->IsHit( x, y );
        if (hitResult) break;
        line = (wxListLineData *) NULL;
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

    bool forceClick = FALSE;
    if (event.ButtonDClick())
    {
        m_renameTimer->Stop();
        m_lastOnSame = FALSE;

        if ( line == m_lineBeforeLastClicked )
        {
            m_usedKeys = FALSE;

            SendNotify( line, wxEVT_COMMAND_LIST_ITEM_ACTIVATED );

            return;
        }
        else
        {
            // the first click was on another item, so don't interpret this as
            // a double click, but as a simple click instead
            forceClick = TRUE;
        }
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

    if ( event.LeftDown() || forceClick )
    {
        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = line;

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
            if (event.ControlDown())
            {
                m_current = line;
                m_current->ReverseHilight();
                RefreshLine( m_current );
            }
            else if (event.ShiftDown())
            {
                size_t j;

                m_current = line;

                int numOfCurrent = -1;
                for (j = 0; j < m_lines.GetCount(); j++)
                {
                    wxListLineData *test_line = &m_lines[j];
                    numOfCurrent++;
                    if (test_line == oldCurrent) break;
                }

                int numOfLine = -1;

                for (j = 0; j < m_lines.GetCount(); j++)
                {
                    wxListLineData *test_line = &m_lines[j];
                    numOfLine++;
                    if (test_line == line) break;
                }

                if (numOfLine < numOfCurrent)
                {
                    int i = numOfLine;
                    numOfLine = numOfCurrent;
                    numOfCurrent = i;
                }

                for (int i = 0; i <= numOfLine-numOfCurrent; i++)
                {
                    wxListLineData *test_line= &m_lines[numOfCurrent + i];
                    test_line->Hilight(TRUE);
                    RefreshLine( test_line );
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

        // forceClick is only set if the previous click was on another item
        m_lastOnSame = !forceClick && (m_current == oldCurrent);

        return;
    }
}

void wxListMainWindow::MoveToFocus()
{
    if (!m_current) return;

    int item_x = 0;
    int item_y = 0;
    int item_w = 0;
    int item_h = 0;
    m_current->GetExtent( item_x, item_y, item_w, item_h );

    int client_w = 0;
    int client_h = 0;
    GetClientSize( &client_w, &client_h );

    int view_x = m_xScroll*GetScrollPos( wxHORIZONTAL );
    int view_y = m_yScroll*GetScrollPos( wxVERTICAL );

    if (m_mode & wxLC_REPORT)
    {
        if (item_y-5 < view_y )
            Scroll( -1, (item_y-5)/m_yScroll );
        if (item_y+item_h+5 > view_y+client_h)
            Scroll( -1, (item_y+item_h-client_h+15)/m_yScroll );
    }
    else
    {
        if (item_x-view_x < 5)
            Scroll( (item_x-5)/m_xScroll, -1 );
        if (item_x+item_w-5 > view_x+client_w)
            Scroll( (item_x+item_w-client_w+15)/m_xScroll, -1 );
    }
}

void wxListMainWindow::OnArrowChar( wxListLineData *newCurrent, bool shiftDown )
{
    if ((m_mode & wxLC_SINGLE_SEL) || (m_usedKeys == FALSE)) m_current->Hilight( FALSE );
    wxListLineData *oldCurrent = m_current;
    m_current = newCurrent;
    if (shiftDown || (m_mode & wxLC_SINGLE_SEL)) m_current->Hilight( TRUE );
    RefreshLine( m_current );
    RefreshLine( oldCurrent );
    FocusLine( m_current );
    UnfocusLine( oldCurrent );
    MoveToFocus();
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
    if ( m_current )
    {
        wxListEvent le( wxEVT_COMMAND_LIST_KEY_DOWN, GetParent()->GetId() );
        le.m_itemIndex = GetIndexOfLine( m_current );
        m_current->GetItem( 0, le.m_item );
        le.m_code = (int)event.KeyCode();
        le.SetEventObject( parent );
        parent->GetEventHandler()->ProcessEvent( le );
    }

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
        nevent.SetWindowChange( event.ControlDown() );
        nevent.SetDirection( !event.ShiftDown() );
        nevent.SetEventObject( GetParent()->GetParent() );
        nevent.SetCurrentFocus( m_parent );
        if (GetParent()->GetParent()->GetEventHandler()->ProcessEvent( nevent )) return;
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
            int index = m_lines.Index(*m_current);
            if (index != wxNOT_FOUND && index > 0)
                OnArrowChar( &m_lines[index-1], event.ShiftDown() );
            break;
        }
        case WXK_DOWN:
        {
            int index = m_lines.Index(*m_current);
            if (index != wxNOT_FOUND && (size_t)index < m_lines.GetCount()-1)
                OnArrowChar( &m_lines[index+1], event.ShiftDown() );
            break;
        }
        case WXK_END:
        {
            if (!m_lines.IsEmpty())
                OnArrowChar( &m_lines.Last(), event.ShiftDown() );
            break;
        }
        case WXK_HOME:
        {
            if (!m_lines.IsEmpty())
                OnArrowChar( &m_lines[0], event.ShiftDown() );
            break;
        }
        case WXK_PRIOR:
        {
            int steps = 0;
            int index = m_lines.Index(*m_current);
            if (m_mode & wxLC_REPORT)
            {
                steps = m_visibleLines-1;
            }
            else
            {
                steps = index % m_visibleLines;
            }
            if (index != wxNOT_FOUND)
            {
                index -= steps;
                if (index < 0) index = 0;
                OnArrowChar( &m_lines[index], event.ShiftDown() );
            }
            break;
        }
        case WXK_NEXT:
        {
            int steps = 0;
            int index = m_lines.Index(*m_current);
            if (m_mode & wxLC_REPORT)
            {
                steps = m_visibleLines-1;
            }
            else
            {
                steps = m_visibleLines-(index % m_visibleLines)-1;
            }

            if (index != wxNOT_FOUND)
            {
                index += steps;
                if ((size_t)index >= m_lines.GetCount())
                    index = m_lines.GetCount()-1;
                OnArrowChar( &m_lines[index], event.ShiftDown() );
            }
            break;
        }
        case WXK_LEFT:
        {
            if (!(m_mode & wxLC_REPORT))
            {
                int index = m_lines.Index(*m_current);
                if (index != wxNOT_FOUND)
                {
                    index -= m_visibleLines;
                    if (index < 0) index = 0;
                    OnArrowChar( &m_lines[index], event.ShiftDown() );
                }
            }
            break;
        }
        case WXK_RIGHT:
        {
            if (!(m_mode & wxLC_REPORT))
            {
                int index = m_lines.Index(*m_current);
                if (index != wxNOT_FOUND)
                {
                    index += m_visibleLines;
                    if ((size_t)index >= m_lines.GetCount())
                        index = m_lines.GetCount()-1;
                    OnArrowChar( &m_lines[index], event.ShiftDown() );
                }
            }
            break;
        }
        case WXK_SPACE:
        {
            if (m_mode & wxLC_SINGLE_SEL)
            {
                wxListEvent le( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, GetParent()->GetId() );
                le.SetEventObject( GetParent() );
                le.m_itemIndex = GetIndexOfLine( m_current );
                m_current->GetItem( 0, le.m_item );
                GetParent()->GetEventHandler()->ProcessEvent( le );
            }
            else
            {
                m_current->ReverseHilight();
                RefreshLine( m_current );
            }
            break;
        }
        case WXK_INSERT:
        {
            if (!(m_mode & wxLC_SINGLE_SEL))
            {
                wxListLineData *oldCurrent = m_current;
                m_current->ReverseHilight();
                int index = m_lines.Index( *m_current ) + 1;
                if ( (size_t)index < m_lines.GetCount() )
                    m_current = &m_lines[index];
                RefreshLine( oldCurrent );
                RefreshLine( m_current );
                UnfocusLine( oldCurrent );
                FocusLine( m_current );
                MoveToFocus();
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
    m_dirty = TRUE;
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
    wxCoord lw = 0;
    wxCoord lh = 0;
    dc.GetTextExtent( s, &lw, &lh );
    return lw + 6;
}

int wxListMainWindow::GetIndexOfLine( const wxListLineData *line )
{
    int i = m_lines.Index(*line);
    if (i == wxNOT_FOUND) return -1;
    else return i;
}

void wxListMainWindow::SetImageList( wxImageList *imageList, int which )
{
    m_dirty = TRUE;

    // calc the spacing from the icon size
    int width = 0,
        height = 0;
    if ((imageList) && (imageList->GetImageCount()) )
    {
        imageList->GetSize(0, width, height);
    }

    if (which == wxIMAGE_LIST_NORMAL)
    {
        m_normal_image_list = imageList;
        m_normal_spacing = width + 8;
    }

    if (which == wxIMAGE_LIST_SMALL)
    {
        m_small_image_list = imageList;
        m_small_spacing = width + 14;
    }
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
    return isSmall ? m_small_spacing : m_normal_spacing;
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

    wxListHeaderWindow *headerWin = ((wxListCtrl*) GetParent())->m_headerWin;
    if ( headerWin )
        headerWin->m_dirty = TRUE;
}

void wxListMainWindow::SetColumnWidth( int col, int width )
{
    wxCHECK_RET( m_mode & wxLC_REPORT,
                 _T("SetColumnWidth() can only be called in report mode.") );

    m_dirty = TRUE;

    wxNode *node = (wxNode*) NULL;

    if (width == wxLIST_AUTOSIZE_USEHEADER)
    {
        // TODO do use the header
        width = 80;
    }
    else if (width == wxLIST_AUTOSIZE)
    {
        wxClientDC dc(this);
        dc.SetFont( GetFont() );
        int max = 10;

        for (size_t i = 0; i < m_lines.GetCount(); i++)
        {
            wxListLineData *line = &m_lines[i];
            wxNode *n = line->m_items.Nth( col );
            if (n)
            {
                wxListItemData *item = (wxListItemData*)n->Data();
                int current = 0, ix = 0, iy = 0;
                wxCoord lx = 0, ly = 0;
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
        }
        width = max+10;
    }

    node = m_columns.Nth( col );
    if (node)
    {
        wxListHeaderData *column = (wxListHeaderData*)node->Data();
        column->SetWidth( width );
    }

    for (size_t i = 0; i < m_lines.GetCount(); i++)
    {
        wxListLineData *line = &m_lines[i];
        wxNode *n = line->m_items.Nth( col );
        if (n)
        {
            wxListItemData *item = (wxListItemData*)n->Data();
            item->SetSize( width, -1 );
        }
    }

    wxListHeaderWindow *headerWin = ((wxListCtrl*) GetParent())->m_headerWin;
    if ( headerWin )
        headerWin->m_dirty = TRUE;
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
    if (item.m_itemId >= 0 && (size_t)item.m_itemId < m_lines.GetCount())
    {
        wxListLineData *line = &m_lines[(size_t)item.m_itemId];
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
        if (item >= 0 && (size_t)item < m_lines.GetCount())
        {
            wxListLineData *line = &m_lines[(size_t)item];
            UnfocusLine( m_current );
            m_current = line;
            FocusLine( m_current );
            if ((m_mode & wxLC_SINGLE_SEL) && oldCurrent) oldCurrent->Hilight( FALSE );
            RefreshLine( m_current );
            if (oldCurrent) RefreshLine( oldCurrent );
        }
    }

    if (stateMask & wxLIST_STATE_SELECTED)
    {
        bool on = (state & wxLIST_STATE_SELECTED) != 0;
        if (!on && (m_mode & wxLC_SINGLE_SEL)) return;

        if (item >= 0 && (size_t)item < m_lines.GetCount())
        {
            wxListLineData *line = &m_lines[(size_t)item];
            if (m_mode & wxLC_SINGLE_SEL)
            {
                UnfocusLine( m_current );
                m_current = line;
                FocusLine( m_current );
                if (oldCurrent) oldCurrent->Hilight( FALSE );
                RefreshLine( m_current );
                if (oldCurrent) RefreshLine( oldCurrent );
            }
            bool on = (state & wxLIST_STATE_SELECTED) != 0;
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
        if (item >= 0 && (size_t)item < m_lines.GetCount())
        {
            wxListLineData *line = &m_lines[(size_t)item];
            if (line == m_current) ret |= wxLIST_STATE_FOCUSED;
        }
    }
    if (stateMask & wxLIST_STATE_SELECTED)
    {
        if (item >= 0 && (size_t)item < m_lines.GetCount())
        {
            wxListLineData *line = &m_lines[(size_t)item];
            if (line->IsHilighted()) ret |= wxLIST_STATE_FOCUSED;
        }
    }
    return ret;
}

void wxListMainWindow::GetItem( wxListItem &item )
{
    if (item.m_itemId >= 0 && (size_t)item.m_itemId < m_lines.GetCount())
    {
        wxListLineData *line = &m_lines[(size_t)item.m_itemId];
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
    return m_lines.GetCount();
}

void wxListMainWindow::GetItemRect( long index, wxRect &rect )
{
    if (index >= 0 && (size_t)index < m_lines.GetCount())
    {
        m_lines[(size_t)index].GetRect( rect );
        this->CalcScrolledPosition(rect.x,rect.y,&rect.x,&rect.y);
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
    wxRect rect;
    this->GetItemRect(item,rect);
    pos.x=rect.x; pos.y=rect.y;
    return TRUE;
}

int wxListMainWindow::GetSelectedItemCount()
{
    int ret = 0;
    for (size_t i = 0; i < m_lines.GetCount(); i++)
    {
        if (m_lines[i].IsHilighted()) ret++;
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
#if wxUSE_GENERIC_LIST_EXTENSIONS
        m_xScroll = 15;
#else
        m_xScroll = 0;
#endif
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
    if (m_lines.IsEmpty()) return;

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

    wxListLineData *line = &m_lines[0];
    line->CalculateSize( &dc, iconSpacing );
    int dummy = 0;
    line->GetSize( dummy, lineSpacing );
    lineSpacing += 1;

    int clientWidth = 0;
    int clientHeight = 0;

    if (m_mode & wxLC_REPORT)
    {
        int x = 4;
        int y = 1;
        int entireHeight = m_lines.GetCount() * lineSpacing + 2;
        int scroll_pos = GetScrollPos( wxVERTICAL );
#if wxUSE_GENERIC_LIST_EXTENSIONS
        int x_scroll_pos = GetScrollPos( wxHORIZONTAL );
#else
        SetScrollbars( m_xScroll, m_yScroll, 0, (entireHeight+15) / m_yScroll, 0, scroll_pos, TRUE );
#endif
        GetClientSize( &clientWidth, &clientHeight );

        int entireWidth = 0 ;
        for (size_t j = 0; j < m_lines.GetCount(); j++)
        {
            wxListLineData *line = &m_lines[j];
            line->CalculateSize( &dc, iconSpacing );
            line->SetPosition( &dc, x, y, clientWidth );
            int col_x = 2;
            for (int i = 0; i < GetColumnCount(); i++)
            {
                line->SetColumnPosition( i, col_x );
                col_x += GetColumnWidth( i );
            }
            entireWidth = wxMax( entireWidth , col_x ) ;
#if wxUSE_GENERIC_LIST_EXTENSIONS
            line->SetPosition( &dc, x, y, col_x );
#endif
            y += lineSpacing;  // one pixel blank line between items
        }
                m_visibleLines = clientHeight / lineSpacing;
#if wxUSE_GENERIC_LIST_EXTENSIONS
                SetScrollbars( m_xScroll, m_yScroll, entireWidth / m_xScroll , (entireHeight+15) / m_yScroll, x_scroll_pos  , scroll_pos, TRUE );
#endif
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
            int x = 2;
            int y = 2;
            int maxWidth = 0;
            m_visibleLines = 0;
            int m_currentVisibleLines = 0;
            for (size_t i = 0; i < m_lines.GetCount(); i++)
            {
                m_currentVisibleLines++;
                wxListLineData *line = &m_lines[i];
                line->CalculateSize( &dc, iconSpacing );
                line->SetPosition( &dc, x, y, clientWidth );
                line->GetSize( lineWidth, lineHeight );
                if (lineWidth > maxWidth) maxWidth = lineWidth;
                y += lineSpacing;
                if (m_currentVisibleLines > m_visibleLines)
                    m_visibleLines = m_currentVisibleLines;
                if (y+lineSpacing-6 >= clientHeight) // -6 for earlier "line breaking"
                {
                    m_currentVisibleLines = 0;
                    y = 2;
                    x += maxWidth+6;
                    entireWidth += maxWidth+6;
                    maxWidth = 0;
                }
                if (i == m_lines.GetCount()-1) entireWidth += maxWidth;
                if ((tries == 0) && (entireWidth > clientWidth))
                {
                    clientHeight -= 15; // scrollbar height
                    m_visibleLines = 0;
                    m_currentVisibleLines = 0;
                    break;
                }
                if (i == m_lines.GetCount()-1) tries = 1;  // everything fits, no second try required
            }
        }

        int scroll_pos = GetScrollPos( wxHORIZONTAL );
        SetScrollbars( m_xScroll, m_yScroll, (entireWidth+15) / m_xScroll, 0, scroll_pos, 0, TRUE );
    }
}

void wxListMainWindow::RealizeChanges()
{
    if (!m_current)
    {
        if (!m_lines.IsEmpty())
            m_current = &m_lines[0];
    }
    if (m_current)
    {
        FocusLine( m_current );
        // TODO: MSW doesn't automatically hilight the
        //       first item.
        // if (m_mode & wxLC_SINGLE_SEL) m_current->Hilight( TRUE );
    }
}

long wxListMainWindow::GetNextItem( long item,
                                    int WXUNUSED(geometry),
                                    int state )
{
    long ret = item,
         max = GetItemCount();
    wxCHECK_MSG( (ret == -1) || (ret < max), -1,
                 _T("invalid listctrl index in GetNextItem()") );

    // notice that we start with the next item (or the first one if item == -1)
    // and this is intentional to allow writing a simple loop to iterate over
    // all selected items
    ret++;
    if ( ret == max )
    {
        // this is not an error because the index was ok initially, just no
        // such item
        return -1;
    }

    for (size_t i = (size_t)ret; i < m_lines.GetCount(); i++)
    {
        wxListLineData *line = &m_lines[i];
        if ((state & wxLIST_STATE_FOCUSED) && (line == m_current))
            return ret;
        if ((state & wxLIST_STATE_SELECTED) && (line->IsHilighted()))
            return ret;
        if (!state)
            return ret;
        ret++;
    }

    return -1;
}

void wxListMainWindow::DeleteItem( long index )
{
    m_dirty = TRUE;
    if (index >= 0 && (size_t)index < m_lines.GetCount())
    {
        wxListLineData *line = &m_lines[(size_t)index];
        if (m_current == line) m_current = (wxListLineData *) NULL;
        DeleteLine( line );
        m_lines.RemoveAt( (size_t)index );
    }
}

void wxListMainWindow::DeleteColumn( int col )
{
    wxCHECK_RET( col < (int)m_columns.GetCount(),
               wxT("attempting to delete inexistent column in wxListView") );

    m_dirty = TRUE;
    wxNode *node = m_columns.Nth( col );
    if (node) m_columns.DeleteNode( node );
}

void wxListMainWindow::DeleteAllItems()
{
    m_dirty = TRUE;
    m_current = (wxListLineData *) NULL;

    // to make the deletion of all items faster, we don't send the
    // notifications in this case: this is compatible with wxMSW and
    // documented in DeleteAllItems() description

    wxListEvent event( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, GetParent()->GetId() );
    event.SetEventObject( GetParent() );
    GetParent()->GetEventHandler()->ProcessEvent( event );

    m_lines.Clear();
}

void wxListMainWindow::DeleteEverything()
{
    DeleteAllItems();

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
    if (index >= 0 && (size_t)index < m_lines.GetCount())
        m_current = &m_lines[(size_t)index];
    if (m_current) MoveToFocus();
    m_current = oldCurrent;
}

long wxListMainWindow::FindItem(long start, const wxString& str, bool WXUNUSED(partial) )
{
    long pos = start;
    wxString tmp = str;
    if (pos < 0) pos = 0;
    for (size_t i = (size_t)pos; i < m_lines.GetCount(); i++)
    {
        wxListLineData *line = &m_lines[i];
        wxString s = "";
        line->GetText( 0, s );
        if (s == tmp) return pos;
        pos++;
    }
    return -1;
}

long wxListMainWindow::FindItem(long start, long data)
{
    long pos = start;
    if (pos < 0) pos = 0;
    for (size_t i = (size_t)pos; i < m_lines.GetCount(); i++)
    {
        wxListLineData *line = &m_lines[i];
        wxListItem item;
        line->GetItem( 0, item );
        if (item.m_data == data) return pos;
        pos++;
    }
    return -1;
}

long wxListMainWindow::HitTest( int x, int y, int &flags )
{
    CalcUnscrolledPosition( x, y, &x, &y );

    int count = 0;
    for (size_t i = 0; i < m_lines.GetCount(); i++)
    {
        wxListLineData *line = &m_lines[i];
        long ret = line->IsHit( x, y );
        if (ret) //  & flags) // No: flags is output-only so may be garbage at this point
        {
            flags = (int)ret;
            return count;
        }
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
    if ((item.m_itemId >= 0) && ((size_t)item.m_itemId < m_lines.GetCount()))
    {
        m_lines.Insert( line, (size_t)item.m_itemId );
    }
    else
    {
        m_lines.Add( line );
        item.m_itemId = m_lines.GetCount()-1;
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
            wxNode *node = m_columns.Nth( (size_t)col );
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

int LINKAGEMODE list_ctrl_compare_func_1( wxListLineData **arg1, wxListLineData **arg2 )
{
    wxListLineData *line1 = *arg1;
    wxListLineData *line2 = *arg2;
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
    m_dirty = TRUE;
}

void wxListMainWindow::OnScroll(wxScrollWinEvent& event)
{
        wxScrolledWindow::OnScroll( event ) ;
#if wxUSE_GENERIC_LIST_EXTENSIONS

    if (event.GetOrientation() == wxHORIZONTAL && ( m_mode & wxLC_REPORT ))
    {
            wxListCtrl* lc = wxDynamicCast( GetParent() , wxListCtrl ) ;
            if ( lc )
            {
                    lc->m_headerWin->Refresh() ;
#ifdef __WXMAC__
                        lc->m_headerWin->MacUpdateImmediately() ;
#endif
            }
    }
#endif
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

    m_attr = NULL;
}

void wxListItem::Clear()
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
    m_text = wxEmptyString;

    if (m_attr) delete m_attr;
    m_attr = NULL;
}

void wxListItem::ClearAttributes()
{
    if (m_attr) delete m_attr;
    m_attr = NULL;
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

void wxListEvent::CopyObject(wxObject& object_dest) const
{
    wxListEvent *obj = (wxListEvent *)&object_dest;

    wxNotifyEvent::CopyObject(object_dest);

    obj->m_code = m_code;
    obj->m_itemIndex = m_itemIndex;
    obj->m_oldItemIndex = m_oldItemIndex;
    obj->m_col = m_col;
    obj->m_cancelled = m_cancelled;
    obj->m_pointDrag = m_pointDrag;
    obj->m_item.m_mask = m_item.m_mask;
    obj->m_item.m_itemId = m_item.m_itemId;
    obj->m_item.m_col = m_item.m_col;
    obj->m_item.m_state = m_item.m_state;
    obj->m_item.m_stateMask = m_item.m_stateMask;
    obj->m_item.m_text = m_item.m_text;
    obj->m_item.m_image = m_item.m_image;
    obj->m_item.m_data = m_item.m_data;
    obj->m_item.m_format = m_item.m_format;
    obj->m_item.m_width = m_item.m_width;

    if ( m_item.HasAttributes() )
    {
        obj->m_item.SetTextColour(m_item.GetTextColour());
    }
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
    m_ownsImageListNormal = m_ownsImageListSmall = m_ownsImageListState = FALSE;
    m_mainWin = (wxListMainWindow*) NULL;
    m_headerWin = (wxListHeaderWindow*) NULL;
}

wxListCtrl::~wxListCtrl()
{
    if (m_ownsImageListNormal) delete m_imageListNormal;
    if (m_ownsImageListSmall) delete m_imageListSmall;
    if (m_ownsImageListState) delete m_imageListState;
}

bool wxListCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint &pos,
                        const wxSize &size,
                        long style,
                        const wxValidator &validator,
                        const wxString &name)
{
    m_imageListNormal = (wxImageList *) NULL;
    m_imageListSmall = (wxImageList *) NULL;
    m_imageListState = (wxImageList *) NULL;
    m_ownsImageListNormal = m_ownsImageListSmall = m_ownsImageListState = FALSE;
    m_mainWin = (wxListMainWindow*) NULL;
    m_headerWin = (wxListHeaderWindow*) NULL;

    if ( !(style & (wxLC_REPORT | wxLC_LIST | wxLC_ICON)) )
    {
        style = style | wxLC_LIST;
    }

    bool ret = wxControl::Create( parent, id, pos, size, style, validator, name );


    if (style & wxSUNKEN_BORDER)
        style -= wxSUNKEN_BORDER;

    m_mainWin = new wxListMainWindow( this, -1, wxPoint(0,0), size, style );

    if (HasFlag(wxLC_REPORT))
    {
        m_headerWin = new wxListHeaderWindow( this, -1, m_mainWin, wxPoint(0,0), wxSize(size.x,23), wxTAB_TRAVERSAL );
        if (HasFlag(wxLC_NO_HEADER))
            m_headerWin->Show( FALSE );
    }
    else
    {
        m_headerWin = (wxListHeaderWindow *) NULL;
    }

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

wxColour wxListCtrl::GetTextColour() const
{
    return GetForegroundColour();
}

void wxListCtrl::SetTextColour(const wxColour& col)
{
    SetForegroundColour(col);
}

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
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        if (m_ownsImageListNormal) delete m_imageListNormal;
        m_imageListNormal = imageList;
        m_ownsImageListNormal = FALSE;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        if (m_ownsImageListSmall) delete m_imageListSmall;
        m_imageListSmall = imageList;
        m_ownsImageListSmall = FALSE;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        if (m_ownsImageListState) delete m_imageListState;
        m_imageListState = imageList;
        m_ownsImageListState = FALSE;
    }

    m_mainWin->SetImageList( imageList, which );
}

void wxListCtrl::AssignImageList(wxImageList *imageList, int which)
{
    SetImageList(imageList, which);
    if ( which == wxIMAGE_LIST_NORMAL )
        m_ownsImageListNormal = TRUE;
    else if ( which == wxIMAGE_LIST_SMALL )
        m_ownsImageListSmall = TRUE;
    else if ( which == wxIMAGE_LIST_STATE )
        m_ownsImageListState = TRUE;
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
    return info.m_itemId;
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
    wxASSERT( m_headerWin );
    m_mainWin->InsertColumn( col, item );
    m_headerWin->Refresh();

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

    if ( m_headerWin && m_headerWin->m_dirty )
    {
        m_headerWin->m_dirty = FALSE;
        m_headerWin->Refresh();
    }
}

bool wxListCtrl::SetBackgroundColour( const wxColour &colour )
{
    if (m_mainWin)
    {
        m_mainWin->SetBackgroundColour( colour );
        m_mainWin->m_dirty = TRUE;
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

#if wxUSE_DRAG_AND_DROP

void wxListCtrl::SetDropTarget( wxDropTarget *dropTarget )
{
    m_mainWin->SetDropTarget( dropTarget );
}

wxDropTarget *wxListCtrl::GetDropTarget() const
{
    return m_mainWin->GetDropTarget();
}

#endif // wxUSE_DRAG_AND_DROP

bool wxListCtrl::SetCursor( const wxCursor &cursor )
{
    return m_mainWin ? m_mainWin->wxWindow::SetCursor(cursor) : FALSE;
}

wxColour wxListCtrl::GetBackgroundColour() const
{
    return m_mainWin ? m_mainWin->GetBackgroundColour() : wxColour();
}

wxColour wxListCtrl::GetForegroundColour() const
{
    return m_mainWin ? m_mainWin->GetForegroundColour() : wxColour();
}

bool wxListCtrl::DoPopupMenu( wxMenu *menu, int x, int y )
{
    return m_mainWin->PopupMenu( menu, x, y );
}

void wxListCtrl::SetFocus()
{
    /* The test in window.cpp fails as we are a composite
       window, so it checks against "this", but not m_mainWin. */
    if ( FindFocus() != this )
        m_mainWin->SetFocus();
}
