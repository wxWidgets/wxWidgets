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
// wxListCtrl
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxListCtrl: public wxControl
{
public:
    wxListCtrl();
    wxListCtrl( wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = wxLC_ICON,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = "listctrl" )
    {
        Create(parent, id, pos, size, style, validator, name);
    }
    ~wxListCtrl();
    
    bool Create( wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = wxLC_ICON,
                 const wxValidator& validator = wxDefaultValidator,
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
    void AssignImageList( wxImageList *imageList, int which );
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
    void SetDropTarget( wxDropTarget *dropTarget );
    wxDropTarget *GetDropTarget() const;
#endif

    bool SetCursor( const wxCursor &cursor );
    wxColour GetBackgroundColour() const;
    wxColour GetForegroundColour() const;
    bool DoPopupMenu( wxMenu *menu, int x, int y );
    void SetFocus();

    // implementation
    // --------------

    wxImageList         *m_imageListNormal;
    wxImageList         *m_imageListSmall;
    wxImageList         *m_imageListState;  // what's that ?
    bool                 m_ownsImageListNormal, 
                         m_ownsImageListSmall,
                         m_ownsImageListState;
    wxListHeaderWindow  *m_headerWin;
    wxListMainWindow    *m_mainWin;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxListCtrl);
};


#endif // __LISTCTRLH_G__
