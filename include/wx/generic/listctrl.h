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
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/imaglist.h"
#include "wx/control.h"
#include "wx/timer.h"
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
typedef int (*wxListCtrlCompare)(const long item1, const long item2, long sortData);

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
    wxLIST_NEXT_RIGHT,          // Searches for an item to the right of the specified item
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

// Flag values for Set/GetImageList
enum {
    wxIMAGE_LIST_NORMAL, // Normal icons
    wxIMAGE_LIST_SMALL,  // Small icons
    wxIMAGE_LIST_STATE   // State icons: unimplemented (see WIN32 documentation)
                         // not implemented in wxGLC (see non-existing documentation..)
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
  wxListEvent(WXTYPE commandType = 0, int id = 0);

  int           m_code;
  long          m_itemIndex;
  long          m_oldItemIndex;
  int           m_col;
  bool          m_cancelled;
  wxPoint       m_pointDrag;

  wxListItem    m_item;
};

typedef void (wxEvtHandler::*wxListEventFunction)(wxListEvent&);

#define EVT_LIST_BEGIN_DRAG(id, fn) { wxEVT_COMMAND_LIST_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_BEGIN_RDRAG(id, fn) { wxEVT_COMMAND_LIST_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_LIST_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_DELETE_ITEM(id, fn) { wxEVT_COMMAND_LIST_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_DELETE_ALL_ITEMS(id, fn) { wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_GET_INFO(id, fn) { wxEVT_COMMAND_LIST_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_SET_INFO(id, fn) { wxEVT_COMMAND_LIST_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_ITEM_SELECTED(id, fn) { wxEVT_COMMAND_LIST_ITEM_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_ITEM_DESELECTED(id, fn) { wxEVT_COMMAND_LIST_ITEM_DESELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_KEY_DOWN(id, fn) { wxEVT_COMMAND_LIST_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_INSERT_ITEM(id, fn) { wxEVT_COMMAND_LIST_INSERT_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_COL_CLICK(id, fn) { wxEVT_COMMAND_LIST_COL_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },


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
    void SetImage( const int image );
    void SetData( const long data );
    void SetPosition( const int x, const int y );
    void SetSize( const int width, const int height );
    void SetColour( wxColour *col );
    bool HasImage(void) const;
    bool HasText(void) const;
    bool IsHit( const int x, const int y ) const;
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
    void SetPosition( const int x, const int y );
    void SetWidth( const int w );
    void SetFormat( const int format );
    void SetHeight( const int h );
    bool HasImage(void) const;
    bool HasText(void) const;
    bool IsHit( const int x, const int y ) const;
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

    void DoDraw( wxPaintDC *dc, const bool hilight, const bool paintBG );

  public:
    wxListLineData( void ) {};
    wxListLineData( wxListMainWindow *owner, const int mode, wxBrush *hilightBrush );
    void CalculateSize( wxPaintDC *dc, const int spacing );
    void SetPosition( wxPaintDC *dc, const int x, const int y,  const int window_width );
    void SetColumnPosition( const int index, const int x );
    void GetSize( int &width, int &height );
    void GetExtent( int &x, int &y, int &width, int &height );
    void GetLabelExtent( int &x, int &y, int &width, int &height );
    long IsHit( const int x, const int y );
    void InitItems( const int num );
    void SetItem( const int index, const wxListItem &info );
    void GetItem( const int index, wxListItem &info );
    void GetText( const int index, wxString &s );
    void SetText( const int index, const wxString s );
    int GetImage( const int index );
    void GetRect( wxRectangle &rect );
    void Hilight( const bool on );
    void ReverseHilight( void );
    void DrawRubberBand( wxPaintDC *dc, const bool on );
    void Draw( wxPaintDC *dc );
    bool IsInRect( const int x, const int y, const wxRectangle &rect );
    bool IsHilighted( void );
    void AssignRect( wxRectangle &dest, const int x, const int y, const int width, const int height );
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
    wxListHeaderWindow( wxWindow *win, const wxWindowID id, wxListMainWindow *owner, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = 0, const wxString &name = "columntitles" );
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

/*

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
    wxListTextCtrl(void) : wxTextCtrl() {};
    wxListTextCtrl(  wxWindow *parent, const char *value = "", 
      bool *accept, wxString *res, wxListMainWindow *owner,
      int x = -1, int y = -1, int w = -1, int h = -1, int style = 0, char *name = "rawtext" ) :
      wxTextCtrl( parent, value, x, y, w, h, style, name ) 
      {
        m_res = res;
        m_accept = accept;
	m_owner = owner;
      };
    void OnChar( wxKeyEvent &event )
      {
        if (event.keyCode == WXK_RETURN)
        {
          (*m_accept) = TRUE;
	  (*m_res) = GetValue();
	  m_owner->OnRenameAccept();
//	  Show( FALSE );
          delete this;
  	  return;
	};
        if (event.keyCode == WXK_ESCAPE)
        {
          (*m_accept) = FALSE;
	  (*m_res) = "";
//	  Show( FALSE );
          delete this;
	  return;
	};
      };
    void OnKillFocus(void)
      {
        (*m_accept) = FALSE;
        (*m_res) = "";
//      Show( FALSE );
        delete this;
        return;
      };
};

*/

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
    bool                 m_isDragging;

  public:
    wxListMainWindow(void); 
    wxListMainWindow( wxWindow *parent, const wxWindowID id, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = 0, const wxString &name = "listctrl" );
    ~wxListMainWindow(void);
    void RefreshLine( wxListLineData *line );
    void OnPaint( wxPaintEvent &event );
    void HilightAll( const bool on );
    void ActivateLine( wxListLineData *line );
    void SendNotify( wxListLineData *line, long command );
    void FocusLine( wxListLineData *line );
    void UnfocusLine( wxListLineData *line );
    void SelectLine( wxListLineData *line );
    void DeselectLine( wxListLineData *line );
    void DeleteLine( wxListLineData *line );
    void RenameLine( wxListLineData *line, const wxString &newName );
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

    void SetImageList( wxImageList *imageList, const int which );
    void SetItemSpacing( const int spacing, const bool isSmall = FALSE );
    int GetItemSpacing( const bool isSmall = FALSE );
    void SetColumn( const int col, wxListItem &item );
    void SetColumnWidth( const int col, const int width );
    void GetColumn( const int col, wxListItem &item );
    int GetColumnWidth( const int vol );
    int GetColumnCount( void );
    int GetCountPerPage( void );     
    void SetItem( wxListItem &item );
    void GetItem( wxListItem &item );
    void SetItemState( const long item, const long state, const long stateMask ); 
    int GetItemState( const long item, const long stateMask );
    int GetItemCount( void );
    void GetItemRect( const long index, wxRectangle &rect );
    int GetSelectedItemCount( void );
    void SetMode( const long mode );
    long GetMode( void ) const;
    void CalculatePositions( void );
    void RealizeChanges(void);
    long GetNextItem( const long item, int geometry, int state );
    void DeleteItem( const long index );
    void DeleteAllItems( void );
    void DeleteColumn( const int col );
    void DeleteEverything( void );
    void EnsureVisible( const long index );
    long FindItem(const long start, const wxString& str, const bool partial = FALSE );
    long FindItem(const long start, const long data);
    long HitTest( const int x, const int y, int &flags );
    void InsertItem( wxListItem &item );
    void InsertColumn( const long col, wxListItem &item );
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
    wxListCtrl( wxWindow *parent, const wxWindowID id, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = 0, const wxString &name = "listctrl" );
    ~wxListCtrl(void);
    bool Create( wxWindow *parent, const wxWindowID id, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = 0, const wxString &name = "listctrl" );
    void OnSize( wxSizeEvent &event );
    void SetBackgroundColour( const wxColour& col );
    bool GetColumn( const int col, wxListItem& item );
    bool SetColumn( const int col, wxListItem& item );
    int GetColumnWidth( const int col );
    bool SetColumnWidth( const int col, const int width);
    int GetCountPerPage(void); // not the same in wxGLC as in Windows, I think
//  wxText& GetEditControl(void) const; // not supported in wxGLC
    bool GetItem( wxListItem& info );
    bool SetItem( wxListItem& info ) ;
    long SetItem( const long index, const int col, const wxString& label, const int imageId = -1 );
    int  GetItemState( const long item, const long stateMask );
    bool SetItemState( const long item, const long state, const long stateMask); 
    bool SetItemImage( const long item, const int image, const int selImage); 
    wxString GetItemText( const long item );
    void SetItemText( const long item, const wxString& str );
    long GetItemData( const long item );
    bool SetItemData( const long item, long data );
    bool GetItemRect( const long item, wxRectangle& rect, const int code = wxLIST_RECT_BOUNDS ); // not supported in wxGLC
    bool GetItemPosition( const long item, wxPoint& pos ) const; // not supported in wxGLC
    bool SetItemPosition( const long item, const wxPoint& pos ); // not supported in wxGLC
    int GetItemCount(void);
    int GetItemSpacing( bool isSmall );
    int GetSelectedItemCount(void);
//  wxColour GetTextColour(void) const; // wxGLC has colours for every Item (see wxListItem)
//  void SetTextColour(const wxColour& col);
    long GetTopItem(void);
    void SetSingleStyle( const long style, const bool add = TRUE ) ;
    void SetWindowStyleFlag(const long style);
    void RecreateWindow(void) {};
    void RealizeChanges( void );  // whereas this is much needed in wxGLC
    long GetNextItem(const long item, int geometry = wxLIST_NEXT_ALL, int state = wxLIST_STATE_DONTCARE);
    wxImageList *GetImageList(const int which);
    void SetImageList(wxImageList *imageList, const int which) ;
    bool Arrange( const int flag = wxLIST_ALIGN_DEFAULT ); // always wxLIST_ALIGN_LEFT in wxGLC
    bool DeleteItem( const long item );
    bool DeleteAllItems(void) ;
    bool DeleteColumn( const int col );
//  wxText& Edit(const long item) ;  // not supported in wxGLC
    bool EnsureVisible( const long item );
    long FindItem(const long start, const wxString& str, const bool partial = FALSE );
    long FindItem(const long start, const long data);
    long FindItem(const long start, const wxPoint& pt, const int direction); // not supported in wxGLC
    long HitTest(const wxPoint& point, int& flags);
    long InsertItem(wxListItem& info);
    long InsertItem(const long index, const wxString& label);
    long InsertItem(const long index, const int imageIndex);
    long InsertItem(const long index, const wxString& label, const int imageIndex);
    long InsertColumn(const long col, wxListItem& info);
    long InsertColumn(const long col, const wxString& heading, const int format = wxLIST_FORMAT_LEFT,
      const int width = -1);
    bool ScrollList(const int dx, const int dy);
    bool SortItems(wxListCtrlCompare fn, long data);
    bool Update(const long item);
    virtual bool OnListNotify(wxListEvent& WXUNUSED(event)) { return FALSE; }
    void SetDropTarget( wxDropTarget *dropTarget )
      { m_mainWin->SetDropTarget( dropTarget ); };
    wxDropTarget *GetDropTarget() const
      { return m_mainWin->GetDropTarget(); };

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
