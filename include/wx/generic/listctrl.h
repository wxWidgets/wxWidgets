/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.h
// Purpose:     Generic list control
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __LISTCTRLH_G__
#define __LISTCTRLH_G__

#ifdef __GNUG__
#pragma interface "listctrl.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/imaglist.h"
#include "wx/control.h"
#include "wx/timer.h"
#include "wx/textctrl.h"
#include "wx/dcclient.h"
#include "wx/scrolwin.h"
#include "wx/settings.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxListItem;
class wxListEvent;
class wxListCtrl;

//-----------------------------------------------------------------------------
// internal classes
//-----------------------------------------------------------------------------

class wxListHeaderData;
class wxListItemData;
class wxListLineData;

class wxListHeaderWindow;
class wxListMainWindow;

class wxListRenameTimer;
//class wxListTextCtrl;

//-----------------------------------------------------------------------------
// types
//-----------------------------------------------------------------------------

// type of compare function for wxListCtrl sort operation
typedef int (*wxListCtrlCompare)(long item1, long item2, long sortData);

//-----------------------------------------------------------------------------
// wxListCtrl flags
//-----------------------------------------------------------------------------

#define wxLC_ICON            0x0004
#define wxLC_SMALL_ICON      0x0008
#define wxLC_LIST            0x0010
#define wxLC_REPORT          0x0020
#define wxLC_ALIGN_TOP       0x0040
#define wxLC_ALIGN_LEFT      0x0080
#define wxLC_AUTOARRANGE     0x0100  // not supported in wxGLC
#define wxLC_USER_TEXT       0x0200  // not supported in wxGLC (how does it work?)
#define wxLC_EDIT_LABELS     0x0400
#define wxLC_NO_HEADER       0x0800  // not supported in wxGLC
#define wxLC_NO_SORT_HEADER  0x1000  // not supported in wxGLC
#define wxLC_SINGLE_SEL      0x2000
#define wxLC_SORT_ASCENDING  0x4000
#define wxLC_SORT_DESCENDING 0x8000  // not supported in wxGLC

#define wxLC_MASK_TYPE       (wxLC_ICON | wxLC_SMALL_ICON | wxLC_LIST | wxLC_REPORT)
#define wxLC_MASK_ALIGN      (wxLC_ALIGN_TOP | wxLC_ALIGN_LEFT)
#define wxLC_MASK_SORT       (wxLC_SORT_ASCENDING | wxLC_SORT_DESCENDING)

// Omitted because (a) too much detail (b) not enough style flags
// #define wxLC_NO_SCROLL
// #define wxLC_NO_LABEL_WRAP
// #define wxLC_OWNERDRAW_FIXED
// #define wxLC_SHOW_SEL_ALWAYS

// Mask flags to tell app/GUI what fields of wxListItem are valid
#define wxLIST_MASK_STATE           0x0001
#define wxLIST_MASK_TEXT            0x0002
#define wxLIST_MASK_IMAGE           0x0004
#define wxLIST_MASK_DATA            0x0008
#define wxLIST_SET_ITEM             0x0010
#define wxLIST_MASK_WIDTH           0x0020
#define wxLIST_MASK_FORMAT          0x0040

// State flags for indicating the state of an item
#define wxLIST_STATE_DONTCARE       0x0000
#define wxLIST_STATE_DROPHILITED    0x0001  // not supported in wxGLC
#define wxLIST_STATE_FOCUSED        0x0002
#define wxLIST_STATE_SELECTED       0x0004
#define wxLIST_STATE_CUT            0x0008  // not supported in wxGLC

// Hit test flags, used in HitTest // wxGLC suppots 20 and 80
#define wxLIST_HITTEST_ABOVE            0x0001  // Above the client area.
#define wxLIST_HITTEST_BELOW            0x0002  // Below the client area.
#define wxLIST_HITTEST_NOWHERE          0x0004  // In the client area but below the last item.
#define wxLIST_HITTEST_ONITEMICON       0x0020  // On the bitmap associated with an item.
#define wxLIST_HITTEST_ONITEMLABEL      0x0080  // On the label (string) associated with an item.
#define wxLIST_HITTEST_ONITEMRIGHT      0x0100  // In the area to the right of an item.
#define wxLIST_HITTEST_ONITEMSTATEICON  0x0200  // On the state icon for a tree view item that is in a user-defined state.
#define wxLIST_HITTEST_TOLEFT           0x0400  // To the right of the client area.
#define wxLIST_HITTEST_TORIGHT          0x0800  // To the left of the client area.

#define wxLIST_HITTEST_ONITEM (wxLIST_HITTEST_ONITEMICON | wxLIST_HITTEST_ONITEMLABEL | wxLIST_HITTEST_ONITEMSTATEICON)



// Flags for GetNextItem  // always wxLIST_NEXT_ALL in wxGLC
enum {
    wxLIST_NEXT_ABOVE,          // Searches for an item above the specified item
    wxLIST_NEXT_ALL,            // Searches for subsequent item by index
    wxLIST_NEXT_BELOW,          // Searches for an item below the specified item
    wxLIST_NEXT_LEFT,           // Searches for an item to the left of the specified item
    wxLIST_NEXT_RIGHT           // Searches for an item to the right of the specified item
};

// Alignment flags for Arrange  // always wxLIST_ALIGN_LEFT in wxGLC
enum {
    wxLIST_ALIGN_DEFAULT,
    wxLIST_ALIGN_LEFT,
    wxLIST_ALIGN_TOP,
    wxLIST_ALIGN_SNAP_TO_GRID
};

// Column format  // always wxLIST_FORMAT_LEFT in wxGLC
enum {
    wxLIST_FORMAT_LEFT,
    wxLIST_FORMAT_RIGHT,
    wxLIST_FORMAT_CENTRE,
    wxLIST_FORMAT_CENTER = wxLIST_FORMAT_CENTRE
};

// Autosize values for SetColumnWidth
enum {
    wxLIST_AUTOSIZE = -1,          // always 80 in wxGLC (what else?)
    wxLIST_AUTOSIZE_USEHEADER = -2
};

// Flag values for GetItemRect
enum {
    wxLIST_RECT_BOUNDS,
    wxLIST_RECT_ICON,
    wxLIST_RECT_LABEL
};

// Flag values for FindItem  // not supported by wxGLC
enum {
    wxLIST_FIND_UP,
    wxLIST_FIND_DOWN,
    wxLIST_FIND_LEFT,
    wxLIST_FIND_RIGHT
};

//-----------------------------------------------------------------------------
// wxListItem
//-----------------------------------------------------------------------------

class wxListItem: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxListItem)

  public:
    long            m_mask;     // Indicates what fields are valid
    long            m_itemId;   // The zero-based item position
    int             m_col;      // Zero-based column, if in report mode
    long            m_state;    // The state of the item
    long            m_stateMask; // Which flags of m_state are valid (uses same flags)
    wxString        m_text;     // The label/header text
    int             m_image;    // The zero-based index into an image list
    long            m_data;     // App-defined data
    wxColour       *m_colour;   // only wxGLC, not supported by Windows ;->

    // For columns only
    int             m_format;   // left, right, centre
    int             m_width;    // width of column

    wxListItem(void);
};

//-----------------------------------------------------------------------------
// wxListEvent
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListEvent: public wxCommandEvent
{
  DECLARE_DYNAMIC_CLASS(wxListEvent)

 public:
  wxListEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

  int           m_code;
  long          m_itemIndex;
  long          m_oldItemIndex;
  int           m_col;
  bool          m_cancelled;
  wxPoint       m_pointDrag;

  wxListItem    m_item;
};

typedef void (wxEvtHandler::*wxListEventFunction)(wxListEvent&);

#define EVT_LIST_BEGIN_DRAG(id, fn) { wxEVT_COMMAND_LIST_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_BEGIN_RDRAG(id, fn) { wxEVT_COMMAND_LIST_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_LIST_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_DELETE_ITEM(id, fn) { wxEVT_COMMAND_LIST_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_DELETE_ALL_ITEMS(id, fn) { wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_GET_INFO(id, fn) { wxEVT_COMMAND_LIST_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_SET_INFO(id, fn) { wxEVT_COMMAND_LIST_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_ITEM_SELECTED(id, fn) { wxEVT_COMMAND_LIST_ITEM_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_ITEM_DESELECTED(id, fn) { wxEVT_COMMAND_LIST_ITEM_DESELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_KEY_DOWN(id, fn) { wxEVT_COMMAND_LIST_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_INSERT_ITEM(id, fn) { wxEVT_COMMAND_LIST_INSERT_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_COL_CLICK(id, fn) { wxEVT_COMMAND_LIST_COL_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },


//-----------------------------------------------------------------------------
//  wxListItemData (internal)
//-----------------------------------------------------------------------------

class wxListItemData : public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxListItemData);

  protected:
    wxString   m_text;
    int        m_image;
    long       m_data;
    int        m_xpos,m_ypos;
    int        m_width,m_height;
    wxColour   *m_colour;

  public:
    wxListItemData(void);
    wxListItemData( const wxListItem &info );
    void SetItem( const wxListItem &info );
    void SetText( const wxString &s );
    void SetImage( int image );
    void SetData( long data );
    void SetPosition( int x, int y );
    void SetSize( int width, int height );
    void SetColour( wxColour *col );
    bool HasImage(void) const;
    bool HasText(void) const;
    bool IsHit( int x, int y ) const;
    void GetText( wxString &s );
    int GetX( void ) const;
    int GetY( void ) const;
    int GetWidth(void) const;
    int GetHeight(void) const;
    int GetImage(void) const;
    void GetItem( wxListItem &info );
    wxColour *GetColour(void);
};

//-----------------------------------------------------------------------------
//  wxListHeaderData (internal)
//-----------------------------------------------------------------------------

class wxListHeaderData : public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxListHeaderData);

  protected:
    long      m_mask;
    int       m_image;
    wxString  m_text;
    int       m_format;
    int       m_width;
    int       m_xpos,m_ypos;
    int       m_height;

  public:
    wxListHeaderData(void);
    wxListHeaderData( const wxListItem &info );
    void SetItem( const wxListItem &item );
    void SetPosition( int x, int y );
    void SetWidth( int w );
    void SetFormat( int format );
    void SetHeight( int h );
    bool HasImage(void) const;
    bool HasText(void) const;
    bool IsHit( int x, int y ) const;
    void GetItem( wxListItem &item );
    void GetText( wxString &s );
    int GetImage(void) const;
    int GetWidth(void) const;
    int GetFormat(void) const;
};

//-----------------------------------------------------------------------------
//  wxListLineData (internal)
//-----------------------------------------------------------------------------

class wxListLineData : public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxListLineData);

  protected:
    wxList              m_items;
    wxRectangle         m_bound_all;
    wxRectangle         m_bound_label;
    wxRectangle         m_bound_icon;
    wxRectangle         m_bound_hilight;
    int                 m_mode;
    bool                m_hilighted;
    wxBrush            *m_hilightBrush;
    int                 m_spacing;
    wxListMainWindow   *m_owner;

    void DoDraw( wxPaintDC *dc, bool hilight, bool paintBG );

  public:
    wxListLineData( void ) {};
    wxListLineData( wxListMainWindow *owner, int mode, wxBrush *hilightBrush );
    void CalculateSize( wxPaintDC *dc, int spacing );
    void SetPosition( wxPaintDC *dc, int x, int y,  int window_width );
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
    void GetRect( wxRectangle &rect );
    void Hilight( bool on );
    void ReverseHilight( void );
    void DrawRubberBand( wxPaintDC *dc, bool on );
    void Draw( wxPaintDC *dc );
    bool IsInRect( int x, int y, const wxRectangle &rect );
    bool IsHilighted( void );
    void AssignRect( wxRectangle &dest, int x, int y, int width, int height );
    void AssignRect( wxRectangle &dest, const wxRectangle &source );
};

//-----------------------------------------------------------------------------
//  wxListHeaderWindow (internal)
//-----------------------------------------------------------------------------

class wxListHeaderWindow : public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxListHeaderWindow)

  protected:
    wxListMainWindow  *m_owner;
    wxCursor          *m_currentCursor;
    wxCursor          *m_resizeCursor;

  public:
    wxListHeaderWindow( void );
    wxListHeaderWindow( wxWindow *win, wxWindowID id, wxListMainWindow *owner,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = 0, const wxString &name = "columntitles" );
    void DoDrawRect( wxPaintDC *dc, int x, int y, int w, int h );
    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );

  DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxListRenameTimer (internal)
//-----------------------------------------------------------------------------

class wxListRenameTimer: public wxTimer
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

class wxListTextCtrl: public wxTextCtrl
{
  DECLARE_DYNAMIC_CLASS(wxListTextCtrl);

  private:
    bool               *m_accept;
    wxString           *m_res;
    wxListMainWindow   *m_owner;

  public:
    wxListTextCtrl(void) {};
    wxListTextCtrl( wxWindow *parent, const wxWindowID id, 
                    bool *accept, wxString *res, wxListMainWindow *owner,
                    const wxString &value = "",
                    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
                    int style = 0, const wxValidator& validator = wxDefaultValidator,
                    const wxString &name = "wxListTextCtrlText" );
    void OnChar( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    
  DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
//  wxListMainWindow (internal)
//-----------------------------------------------------------------------------

class wxListMainWindow: public wxScrolledWindow
{
  DECLARE_DYNAMIC_CLASS(wxListMainWindow);

  public:
    long                 m_mode;
    wxList               m_lines;
    wxList               m_columns;
    wxListLineData      *m_current;
    int                  m_visibleLines;
    wxBrush             *m_hilightBrush;
    wxColour            *m_hilightColour;
    wxFont              *m_myFont;
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
//  wxListTextCtrl      *m_text;
    bool                 m_renameAccept;
    wxString             m_renameRes;
    bool                 m_isCreated;
    int                  m_dragCount;

  public:
    wxListMainWindow(void);
    wxListMainWindow( wxWindow *parent, wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = 0, const wxString &name = "listctrl" );
    ~wxListMainWindow(void);
    void RefreshLine( wxListLineData *line );
    void OnPaint( wxPaintEvent &event );
    void HilightAll( bool on );
    void ActivateLine( wxListLineData *line );
    void SendNotify( wxListLineData *line, wxEventType command );
    void FocusLine( wxListLineData *line );
    void UnfocusLine( wxListLineData *line );
    void SelectLine( wxListLineData *line );
    void DeselectLine( wxListLineData *line );
    void DeleteLine( wxListLineData *line );
    void RenameLine( wxListLineData *line, const wxString &newName );
    void StartLabelEdit( wxListLineData *line );
    void OnRenameTimer(void);
    void OnRenameAccept(void);
    void OnMouse( wxMouseEvent &event );
    void MoveToFocus( void );
    void OnArrowChar( wxListLineData *newCurrent, bool shiftDown );
    void OnChar( wxKeyEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnSize( wxSizeEvent &event );
    wxFont *GetMyFont( void );
    void DrawImage( int index, wxPaintDC *dc, int x, int y );
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
    int GetColumnCount( void );
    int GetCountPerPage( void );
    void SetItem( wxListItem &item );
    void GetItem( wxListItem &item );
    void SetItemState( long item, long state, long stateMask );
    int GetItemState( long item, long stateMask );
    int GetItemCount( void );
    void GetItemRect( long index, wxRectangle &rect );
    bool GetItemPosition(long item, wxPoint& pos);
    int GetSelectedItemCount( void );
    void SetMode( long mode );
    long GetMode( void ) const;
    void CalculatePositions( void );
    void RealizeChanges(void);
    long GetNextItem( long item, int geometry, int state );
    void DeleteItem( long index );
    void DeleteAllItems( void );
    void DeleteColumn( int col );
    void DeleteEverything( void );
    void EnsureVisible( long index );
    long FindItem(long start, const wxString& str, bool partial = FALSE );
    long FindItem(long start, long data);
    long HitTest( int x, int y, int &flags );
    void InsertItem( wxListItem &item );
//    void AddItem( wxListItem &item );
    void InsertColumn( long col, wxListItem &item );
//    void AddColumn( wxListItem &item );
    void SortItems( wxListCtrlCompare fn, long data );
    virtual bool OnListNotify( wxListEvent &event );

  DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxListCtrl
//-----------------------------------------------------------------------------

class wxListCtrl: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxListCtrl);

  public:

    wxListCtrl(void);
    wxListCtrl( wxWindow *parent, wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = 0, const wxString &name = "listctrl" );
    ~wxListCtrl(void);
    bool Create( wxWindow *parent, wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = 0, const wxString &name = "listctrl" );
    void OnSize( wxSizeEvent &event );
    bool GetColumn( int col, wxListItem& item );
    bool SetColumn( int col, wxListItem& item );
    int GetColumnWidth( int col );
    bool SetColumnWidth( int col, int width);
    int GetCountPerPage(void); // not the same in wxGLC as in Windows, I think
//  wxText& GetEditControl(void) const; // not supported in wxGLC
    bool GetItem( wxListItem& info );
    bool SetItem( wxListItem& info ) ;
    long SetItem( long index, int col, const wxString& label, int imageId = -1 );
    int  GetItemState( long item, long stateMask );
    bool SetItemState( long item, long state, long stateMask);
    bool SetItemImage( long item, int image, int selImage);
    wxString GetItemText( long item );
    void SetItemText( long item, const wxString& str );
    long GetItemData( long item );
    bool SetItemData( long item, long data );
    bool GetItemRect( long item, wxRectangle& rect, int code = wxLIST_RECT_BOUNDS );
    bool GetItemPosition( long item, wxPoint& pos );
    bool SetItemPosition( long item, const wxPoint& pos ); // not supported in wxGLC
    int GetItemCount(void);
    void SetItemSpacing( int spacing, bool isSmall = FALSE );
    int GetItemSpacing( bool isSmall );
    int GetSelectedItemCount(void);
//  wxColour GetTextColour(void) const; // wxGLC has colours for every Item (see wxListItem)
//  void SetTextColour(const wxColour& col);
    long GetTopItem(void);
    void SetSingleStyle( long style, bool add = TRUE ) ;
    void SetWindowStyleFlag(long style);
    void RecreateWindow(void) {};
    long GetNextItem(long item, int geometry = wxLIST_NEXT_ALL, int state = wxLIST_STATE_DONTCARE) const;
    wxImageList *GetImageList(int which);
    void SetImageList(wxImageList *imageList, int which) ;
    bool Arrange( int flag = wxLIST_ALIGN_DEFAULT ); // always wxLIST_ALIGN_LEFT in wxGLC
    bool DeleteItem( long item );
    bool DeleteAllItems(void) ;
    bool DeleteColumn( int col );
//  wxText& Edit(long item) ;  // not supported in wxGLC
    bool EnsureVisible( long item );
    long FindItem(long start, const wxString& str, bool partial = FALSE );
    long FindItem(long start, long data);
    long FindItem(long start, const wxPoint& pt, int direction); // not supported in wxGLC
    long HitTest(const wxPoint& point, int& flags);
    long InsertItem(wxListItem& info);
    long InsertItem(long index, const wxString& label);
    long InsertItem(long index, int imageIndex);
    long InsertItem(long index, const wxString& label, int imageIndex);
    long InsertColumn(long col, wxListItem& info);
    long InsertColumn(long col, const wxString& heading, int format = wxLIST_FORMAT_LEFT,
      int width = -1);
    bool ScrollList(int dx, int dy);
    bool SortItems(wxListCtrlCompare fn, long data);
    bool Update(long item);
    virtual bool OnListNotify(wxListEvent& WXUNUSED(event)) { return FALSE; }
    void OnIdle( wxIdleEvent &event );
    
    // We have to hand down a few functions
    
    void SetDropTarget( wxDropTarget *dropTarget )
      { m_mainWin->SetDropTarget( dropTarget ); }
    wxDropTarget *GetDropTarget() const
      { return m_mainWin->GetDropTarget(); }
    void SetCursor( const wxCursor &cursor )
      { m_mainWin->SetCursor( cursor); }
    wxColour GetBackgroundColour() const
      { return m_mainWin->GetBackgroundColour(); }
    void SetBackgroundColour( const wxColour &colour )
      { m_mainWin->SetBackgroundColour( colour ); }
    wxColour GetForegroundColour() const
      { return m_mainWin->GetForegroundColour(); }
    void SetForegroundColour( const wxColour &colour )
      { m_mainWin->SetForegroundColour( colour ); }
    bool PopupMenu( wxMenu *menu, int x, int y )
      { return m_mainWin->PopupMenu( menu, x, y ); }

  protected:

//  wxListTextCtrl       m_textCtrl;
    wxImageList         *m_imageListNormal;
    wxImageList         *m_imageListSmall;
    wxImageList         *m_imageListState;  // what's that ?
    wxListHeaderWindow  *m_headerWin;
    wxListMainWindow    *m_mainWin;

  DECLARE_EVENT_TABLE()

};


#endif // __LISTCTRLH_G__
