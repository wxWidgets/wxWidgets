/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/listctrl.h
// Purpose:     Generic list control
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __LISTCTRLH_G__
#define __LISTCTRLH_G__

#ifdef __GNUG__
#pragma interface "listctrl.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/generic/imaglist.h"
#include "wx/control.h"
#include "wx/timer.h"
#include "wx/textctrl.h"
#include "wx/dcclient.h"
#include "wx/scrolwin.h"
#include "wx/settings.h"

#if wxUSE_DRAG_AND_DROP
class WXDLLEXPORT wxDropTarget;
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListItem;
class WXDLLEXPORT wxListEvent;
class WXDLLEXPORT wxListCtrl;

//-----------------------------------------------------------------------------
// internal classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListHeaderData;
class WXDLLEXPORT wxListItemData;
class WXDLLEXPORT wxListLineData;

class WXDLLEXPORT wxListHeaderWindow;
class WXDLLEXPORT wxListMainWindow;

class WXDLLEXPORT wxListRenameTimer;
class WXDLLEXPORT wxListTextCtrl;

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
                       const wxColour& colText, const wxFont& font);

    DECLARE_DYNAMIC_CLASS(wxListLineData);
};

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
    int                m_column;
    int                m_minX;
    int                m_currentX;

public:
    wxListHeaderWindow();
    ~wxListHeaderWindow();
    wxListHeaderWindow( wxWindow *win, wxWindowID id, wxListMainWindow *owner,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = 0, const wxString &name = "columntitles" );
    void DoDrawRect( wxDC *dc, int x, int y, int w, int h );
    void OnPaint( wxPaintEvent &event );
    void DrawCurrent();
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );

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
#if wxUSE_VALIDATORS
                    const wxValidator& validator = wxDefaultValidator,
#endif
                    const wxString &name = "wxListTextCtrlText" );
    void OnChar( wxKeyEvent &event );
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
    wxList               m_lines;
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

public:
    wxListMainWindow();
    wxListMainWindow( wxWindow *parent, wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = 0, const wxString &name = "listctrl" );
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

//-----------------------------------------------------------------------------
// wxListCtrl
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListCtrl: public wxControl
{
public:
    wxListCtrl();
    wxListCtrl( wxWindow *parent, wxWindowID id = -1,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
#if wxUSE_VALIDATORS
      long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator,
#endif
       const wxString &name = "listctrl" )
    {
        Create(parent, id, pos, size, style, validator, name);
    }
    ~wxListCtrl();
    
    bool Create( wxWindow *parent, wxWindowID id = -1,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
#if wxUSE_VALIDATORS
      long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator,
#endif
      const wxString &name = "listctrl" );

    bool GetColumn( int col, wxListItem& item ) const;
    bool SetColumn( int col, wxListItem& item );
    int GetColumnWidth( int col ) const;
    bool SetColumnWidth( int col, int width);
    int GetCountPerPage() const; // not the same in wxGLC as in Windows, I think

    bool GetItem( wxListItem& info ) const;
    bool SetItem( wxListItem& info ) ;
    long SetItem( long index, int col, const wxString& label, int imageId = -1 );
    int  GetItemState( long item, long stateMask ) const;
    bool SetItemState( long item, long state, long stateMask);
    bool SetItemImage( long item, int image, int selImage);
    wxString GetItemText( long item ) const;
    void SetItemText( long item, const wxString& str );
    long GetItemData( long item ) const;
    bool SetItemData( long item, long data );
    bool GetItemRect( long item, wxRect& rect, int code = wxLIST_RECT_BOUNDS ) const;
    bool GetItemPosition( long item, wxPoint& pos ) const;
    bool SetItemPosition( long item, const wxPoint& pos ); // not supported in wxGLC
    int GetItemCount() const;
    int GetColumnCount() const;
    void SetItemSpacing( int spacing, bool isSmall = FALSE );
    int GetItemSpacing( bool isSmall ) const;
    int GetSelectedItemCount() const;
    wxColour GetTextColour() const;
    void SetTextColour(const wxColour& col);
    long GetTopItem() const;

    void SetSingleStyle( long style, bool add = TRUE ) ;
    void SetWindowStyleFlag( long style );
    void RecreateWindow() {}
    long GetNextItem( long item, int geometry = wxLIST_NEXT_ALL, int state = wxLIST_STATE_DONTCARE ) const;
    wxImageList *GetImageList( int which ) const;
    void SetImageList( wxImageList *imageList, int which );
    bool Arrange( int flag = wxLIST_ALIGN_DEFAULT ); // always wxLIST_ALIGN_LEFT in wxGLC

    void ClearAll();
    bool DeleteItem( long item );
    bool DeleteAllItems();
    bool DeleteAllColumns();
    bool DeleteColumn( int col );

    void EditLabel( long item ) { Edit(item); }
    void Edit( long item );

    bool EnsureVisible( long item );
    long FindItem( long start, const wxString& str, bool partial = FALSE );
    long FindItem( long start, long data );
    long FindItem( long start, const wxPoint& pt, int direction ); // not supported in wxGLC
    long HitTest( const wxPoint& point, int& flags);
    long InsertItem(wxListItem& info);
    long InsertItem( long index, const wxString& label );
    long InsertItem( long index, int imageIndex );
    long InsertItem( long index, const wxString& label, int imageIndex );
    long InsertColumn( long col, wxListItem& info );
    long InsertColumn( long col, const wxString& heading, int format = wxLIST_FORMAT_LEFT,
      int width = -1 );
    bool ScrollList( int dx, int dy );
    bool SortItems( wxListCtrlCompare fn, long data );
    bool Update( long item );
    
    void OnIdle( wxIdleEvent &event );
    void OnSize( wxSizeEvent &event );

    // We have to hand down a few functions

    bool SetBackgroundColour( const wxColour &colour );
    bool SetForegroundColour( const wxColour &colour );
    bool SetFont( const wxFont &font );

#if wxUSE_DRAG_AND_DROP
    void SetDropTarget( wxDropTarget *dropTarget )
      { m_mainWin->SetDropTarget( dropTarget ); }
    wxDropTarget *GetDropTarget() const
      { return m_mainWin->GetDropTarget(); }
#endif

    bool SetCursor( const wxCursor &cursor )
      { return m_mainWin ? m_mainWin->wxWindow::SetCursor(cursor) : FALSE; }
    wxColour GetBackgroundColour() const
      { return m_mainWin ? m_mainWin->GetBackgroundColour() : wxColour(); }
    wxColour GetForegroundColour() const
      { return m_mainWin ? m_mainWin->GetForegroundColour() : wxColour(); }
    bool DoPopupMenu( wxMenu *menu, int x, int y )
      { return m_mainWin->PopupMenu( menu, x, y ); }
    void SetFocus()
      {
          /* The test in window.cpp fails as we are a composite
             window, so it checks against "this", but not m_mainWin. */
          if (FindFocus() != this)
              m_mainWin->SetFocus();
      }

    // implementation
    // --------------

    wxImageList         *m_imageListNormal;
    wxImageList         *m_imageListSmall;
    wxImageList         *m_imageListState;  // what's that ?
    wxListHeaderWindow  *m_headerWin;
    wxListMainWindow    *m_mainWin;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxListCtrl);
};


#endif // __LISTCTRLH_G__
