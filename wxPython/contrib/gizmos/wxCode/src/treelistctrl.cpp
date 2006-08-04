/////////////////////////////////////////////////////////////////////////////
// Name:        treelistctrl.cpp
// Purpose:     multi column tree control implementation
// Author:      Robert Roebling
// Created:     01/02/97
// Modified:    Alberto Griggio, 2002
//              22/10/98 - almost total rewrite, simpler interface (VZ)
// Id:          $Id$
// Copyright:   (c) Robert Roebling, Julian Smart, Alberto Griggio,
//              Vadim Zeitlin, Otto Wyss
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include <wx/treebase.h>
#include <wx/timer.h>
#include <wx/textctrl.h>
#include <wx/imaglist.h>
#include <wx/settings.h>
#include <wx/dcclient.h>
#include <wx/dcscreen.h>
#include <wx/scrolwin.h>
#include <wx/renderer.h>
#include <wx/dcmemory.h>

#include "wx/treelistctrl.h"

#ifdef __WXGTK__
    #include <gtk/gtk.h>
    #include <wx/gtk/win_gtk.h>
#endif

#ifdef __WXMAC__
    #include "wx/mac/private.h"
#endif


// ---------------------------------------------------------------------------
// array types
// ---------------------------------------------------------------------------

class  wxTreeListItem;

#if !wxCHECK_VERSION(2, 5, 0)
WX_DEFINE_ARRAY(wxTreeListItem *, wxArrayTreeListItems);
#else
WX_DEFINE_ARRAY_PTR(wxTreeListItem *, wxArrayTreeListItems);
#endif

#include <wx/dynarray.h>
WX_DECLARE_OBJARRAY(wxTreeListColumnInfo, wxArrayTreeListColumnInfo);
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayTreeListColumnInfo);

#if !wxCHECK_VERSION(2, 3, 3)
WX_DEFINE_ARRAY(short, wxArrayShort);
#endif


// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

static const int NO_IMAGE = -1;

const int LINEHEIGHT = 10;
const int PIXELS_PER_UNIT = 10;
const int LINEATROOT = 5;
const int MARGIN = 2;
const int MININDENT = 10;
const int BTNWIDTH = 9; //11;
const int BTNHEIGHT = 9; //11;

// extra margins around the text label
static const int EXTRA_WIDTH = 4;
static const int EXTRA_HEIGHT = 4;

// offset for the header window
static const int HEADER_OFFSET_X = 1;
static const int HEADER_OFFSET_Y = 1;



const wxChar* wxTreeListCtrlNameStr = wxT("treelistctrl");

static wxTreeListColumnInfo wxInvalidTreeListColumnInfo;


// ---------------------------------------------------------------------------
// private classes
// ---------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  wxTreeListHeaderWindow (internal)
//-----------------------------------------------------------------------------

class  wxTreeListHeaderWindow : public wxWindow
{
protected:
    wxTreeListMainWindow *m_owner;
    const wxCursor       *m_currentCursor;
    wxCursor             *m_resizeCursor;
    bool                 m_isDragging;

    // column being resized
    int m_column;

    // divider line position in logical (unscrolled) coords
    int m_currentX;

    // minimal position beyond which the divider line can't be dragged in
    // logical coords
    int m_minX;

    wxArrayTreeListColumnInfo m_columns;

    // total width of the columns
    int m_total_col_width;


public:
    wxTreeListHeaderWindow();

    wxTreeListHeaderWindow( wxWindow *win,
                            wxWindowID id,
                            wxTreeListMainWindow *owner,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            long style = 0,
                            const wxString &name = wxT("wxtreelistctrlcolumntitles") );

    virtual ~wxTreeListHeaderWindow();

    void DoDrawRect( wxDC *dc, int x, int y, int w, int h );
    void DrawCurrent();
    void AdjustDC(wxDC& dc);

    void OnEraseBackground( wxEraseEvent& event );
    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );


    // columns manipulation

    size_t GetColumnCount() const { return m_columns.GetCount(); }

    void AddColumn(const wxTreeListColumnInfo& col);

    void InsertColumn(size_t before, const wxTreeListColumnInfo& col);

    void RemoveColumn(size_t column);

    void SetColumn(size_t column, const wxTreeListColumnInfo& info);
    const wxTreeListColumnInfo& GetColumn(size_t column) const
    {
        wxCHECK_MSG(column < GetColumnCount(), wxInvalidTreeListColumnInfo, wxT("Invalid column"));
        return m_columns[column];
    }
    wxTreeListColumnInfo& GetColumn(size_t column)
    {
        wxCHECK_MSG(column < GetColumnCount(), wxInvalidTreeListColumnInfo, wxT("Invalid column"));
        return m_columns[column];
    }

    void SetColumnWidth(size_t column, size_t width);

    void SetColumnText(size_t column, const wxString& text)
    {
        wxCHECK_RET(column < GetColumnCount(), wxT("Invalid column"));
        m_columns[column].SetText(text);
    }

    void SetColumnShown(size_t column, bool shown)
    {
        wxCHECK_RET(column < GetColumnCount(), wxT("Invalid column"));
        m_columns[column].SetShown(shown);
    }

    wxString GetColumnText(size_t column) const
    {
        wxCHECK_MSG(column < GetColumnCount(), wxEmptyString, wxT("Invalid column"));
        return m_columns[column].GetText();
    }

    int GetColumnWidth(size_t column) const
    {
        wxCHECK_MSG(column < GetColumnCount(), -1, wxT("Invalid column"));
        return m_columns[column].GetWidth();
    }

    int GetWidth() const { return m_total_col_width; }

    int GetColumnShown(size_t column) const
    {
        wxCHECK_MSG(column < GetColumnCount(), -1, wxT("Invalid column"));
        return m_columns[column].GetShown();
    }

    // needs refresh
    bool m_dirty;

private:
    // common part of all ctors
    void Init();

    void SendListEvent(wxEventType type, wxPoint pos);

    DECLARE_DYNAMIC_CLASS(wxTreeListHeaderWindow)
    DECLARE_EVENT_TABLE()
};


// this is the "true" control
class  wxTreeListMainWindow: public wxScrolledWindow
{
public:
    // creation
    // --------
    wxTreeListMainWindow() { Init(); }

    wxTreeListMainWindow(wxTreeListCtrl *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_DEFAULT_STYLE,
               const wxValidator &validator = wxDefaultValidator,
               const wxString& name = wxT("wxtreelistmainwindow"))
    {
        Init();
        Create(parent, id, pos, size, style, validator, name);
    }

    virtual ~wxTreeListMainWindow();

    bool Create(wxTreeListCtrl *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_DEFAULT_STYLE,
                const wxValidator &validator = wxDefaultValidator,
                const wxString& name = wxT("wxtreelistctrl"));

    // accessors
    // ---------

    // get the total number of items in the control
    size_t GetCount() const;

    // indent is the number of pixels the children are indented relative to
    // the parents position. SetIndent() also redraws the control
    // immediately.
    unsigned int GetIndent() const { return m_indent; }
    void SetIndent(unsigned int indent);

    // see wxTreeListCtrl for the meaning
    unsigned int GetLineSpacing() const { return m_linespacing; }
    void SetLineSpacing(unsigned int spacing);

    // image list: these functions allow to associate an image list with
    // the control and retrieve it. Note that when assigned with
    // SetImageList, the control does _not_ delete
    // the associated image list when it's deleted in order to allow image
    // lists to be shared between different controls. If you use
    // AssignImageList, the control _does_ delete the image list.

    // The normal image list is for the icons which correspond to the
    // normal tree item state (whether it is selected or not).
    // Additionally, the application might choose to show a state icon
    // which corresponds to an app-defined item state (for example,
    // checked/unchecked) which are taken from the state image list.
    wxImageList *GetImageList() const;
    wxImageList *GetStateImageList() const;
    wxImageList *GetButtonsImageList() const;

    void SetImageList(wxImageList *imageList);
    void SetStateImageList(wxImageList *imageList);
    void SetButtonsImageList(wxImageList *imageList);
    void AssignImageList(wxImageList *imageList);
    void AssignStateImageList(wxImageList *imageList);
    void AssignButtonsImageList(wxImageList *imageList);

    // Functions to work with tree ctrl items.

    // accessors
    // ---------

    // retrieve item's label
    wxString GetItemText(const wxTreeItemId& item) const
    { return GetItemText(item, GetMainColumn()); }
    // get one of the images associated with the item (normal by default)
    int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const
    { return GetItemImage(item, GetMainColumn(), which); }

    // get the data associated with the item
    wxTreeItemData *GetItemData(const wxTreeItemId& item) const;

    bool GetItemBold(const wxTreeItemId& item) const;
    wxColour GetItemTextColour(const wxTreeItemId& item) const;
    wxColour GetItemBackgroundColour(const wxTreeItemId& item) const;
    wxFont GetItemFont(const wxTreeItemId& item) const;

    // modifiers
    // ---------

    // set item's label
    void SetItemText(const wxTreeItemId& item, const wxString& text)
    { SetItemText(item, GetMainColumn(), text); }

    // get one of the images associated with the item (normal by default)
    void SetItemImage(const wxTreeItemId& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal)
    { SetItemImage(item, GetMainColumn(), image, which); }

    // associate some data with the item
    void SetItemData(const wxTreeItemId& item, wxTreeItemData *data);

    // force appearance of [+] button near the item. This is useful to
    // allow the user to expand the items which don't have any children now
    // - but instead add them only when needed, thus minimizing memory
    // usage and loading time.
    void SetItemHasChildren(const wxTreeItemId& item, bool has = TRUE);

    // the item will be shown in bold
    void SetItemBold(const wxTreeItemId& item, bool bold = TRUE);

    // set the item's text colour
    void SetItemTextColour(const wxTreeItemId& item, const wxColour& colour);

    // set the item's background colour
    void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& colour);

    // set the item's font (should be of the same height for all items)
    void SetItemFont(const wxTreeItemId& item, const wxFont& font);

    // set the window font
    virtual bool SetFont( const wxFont &font );

    // set the styles.  No need to specify a GetWindowStyle here since
    // the base wxWindow member function will do it for us
    void SetWindowStyle(const long styles);

    // item status inquiries
    // ---------------------

    // is the item visible (it might be outside the view or not expanded)?
    bool IsVisible(const wxTreeItemId& item) const;
    // does the item has any children?
    bool HasChildren(const wxTreeItemId& item) const
    { return ItemHasChildren(item); }
    bool ItemHasChildren(const wxTreeItemId& item) const;
    // is the item expanded (only makes sense if HasChildren())?
    bool IsExpanded(const wxTreeItemId& item) const;
    // is this item currently selected (the same as has focus)?
    bool IsSelected(const wxTreeItemId& item) const;
    // is item text in bold font?
    bool IsBold(const wxTreeItemId& item) const;
        // does the layout include space for a button?

    // number of children
    // ------------------

    // if 'recursively' is FALSE, only immediate children count, otherwise
    // the returned number is the number of all items in this branch
    size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = TRUE);

    // navigation
    // ----------

    // wxTreeItemId.IsOk() will return FALSE if there is no such item

    // get the root tree item
    wxTreeItemId GetRootItem() const { return m_anchor; }

    // get the item currently selected (may return NULL if no selection)
    wxTreeItemId GetSelection() const { return m_current; }

    // get the items currently selected, return the number of such item
    size_t GetSelections(wxArrayTreeItemIds&) const;

    // get the parent of this item (may return NULL if root)
    wxTreeItemId GetItemParent(const wxTreeItemId& item) const;

    // for this enumeration function you must pass in a "cookie" parameter
    // which is opaque for the application but is necessary for the library
    // to make these functions reentrant (i.e. allow more than one
    // enumeration on one and the same object simultaneously). Of course,
    // the "cookie" passed to GetFirstChild() and GetNextChild() should be
    // the same!

    // get the first child of this item
#if !wxCHECK_VERSION(2, 5, 0)
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& cookie) const;
#else
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
#endif
    // get the next child
#if !wxCHECK_VERSION(2, 5, 0)
    wxTreeItemId GetNextChild(const wxTreeItemId& item, long& cookie) const;
#else
    wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
#endif
    // get the prev child
#if !wxCHECK_VERSION(2, 5, 0)
    wxTreeItemId GetPrevChild(const wxTreeItemId& item, long& cookie) const;
#else
    wxTreeItemId GetPrevChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
#endif
    // get the last child of this item - this method doesn't use cookies
    wxTreeItemId GetLastChild(const wxTreeItemId& item) const;

    // get the next sibling of this item
    wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;
    // get the previous sibling
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;

    // get first visible item
    wxTreeItemId GetFirstVisibleItem() const;
    // get the next visible item: item must be visible itself!
    // see IsVisible() and wxTreeCtrl::GetFirstVisibleItem()
    wxTreeItemId GetNextVisible(const wxTreeItemId& item) const;
    // get the previous visible item: item must be visible itself!
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const;

    // Only for internal use right now, but should probably be public
    wxTreeItemId GetNext(const wxTreeItemId& item) const;

    // operations
    // ----------

    // add the root node to the tree
    wxTreeItemId AddRoot(const wxString& text,
                         int image = -1, int selectedImage = -1,
                         wxTreeItemData *data = NULL);

    // insert a new item in as the first child of the parent
    wxTreeItemId PrependItem(const wxTreeItemId& parent,
                             const wxString& text,
                             int image = -1, int selectedImage = -1,
                             wxTreeItemData *data = NULL);

    // insert a new item after a given one
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            const wxTreeItemId& idPrevious,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxTreeItemData *data = NULL);

    // insert a new item before the one with the given index
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            size_t index,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxTreeItemData *data = NULL);

    // insert a new item in as the last child of the parent
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxTreeItemData *data = NULL);

    // delete this item and associated data if any
    void Delete(const wxTreeItemId& item);
    // delete all children (but don't delete the item itself)
    // NB: this won't send wxEVT_COMMAND_TREE_ITEM_DELETED events
    void DeleteChildren(const wxTreeItemId& item);
    // delete all items from the tree
    // NB: this won't send wxEVT_COMMAND_TREE_ITEM_DELETED events
    void DeleteAllItems();

    // expand this item
    void Expand(const wxTreeItemId& item);
    // expand this item and all subitems recursively
    void ExpandAll(const wxTreeItemId& item);
    // collapse the item without removing its children
    void Collapse(const wxTreeItemId& item);
    // collapse the item and remove all children
    void CollapseAndReset(const wxTreeItemId& item);
    // toggles the current state
    void Toggle(const wxTreeItemId& item);

    // remove the selection from currently selected item (if any)
    void Unselect();
    void UnselectAll();
    // select this item
    void SelectItem(const wxTreeItemId& item, bool unselect_others=TRUE,
                    bool extended_select=FALSE);
    void SelectAll(bool extended_select=FALSE);
    // make sure this item is visible (expanding the parent item and/or
    // scrolling to this item if necessary)
    void EnsureVisible(const wxTreeItemId& item);
    // scroll to this item (but don't expand its parent)
    void ScrollTo(const wxTreeItemId& item);
    void AdjustMyScrollbars();

    // The first function is more portable (because easier to implement
    // on other platforms), but the second one returns some extra info.
    wxTreeItemId HitTest(const wxPoint& point)
        { int dummy; return HitTest(point, dummy); }
    wxTreeItemId HitTest(const wxPoint& point, int& flags)
    { int col; return HitTest(point, flags, col); }
    // ALB
    wxTreeItemId HitTest(const wxPoint& point, int& flags, int& column);


    // get the bounding rectangle of the item (or of its label only)
    bool GetBoundingRect(const wxTreeItemId& item,
                         wxRect& rect,
                         bool textOnly = FALSE) const;

    // Start editing the item label: this (temporarily) replaces the item
    // with a one line edit control. The item will be selected if it hadn't
    // been before.
    void EditLabel( const wxTreeItemId& item ) { Edit( item ); }
    void Edit( const wxTreeItemId& item );

    // sorting
    // this function is called to compare 2 items and should return -1, 0
    // or +1 if the first item is less than, equal to or greater than the
    // second one. The base class version performs alphabetic comparaison
    // of item labels (GetText)
    virtual int OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2);
    // sort the children of this item using OnCompareItems
    //
    // NB: this function is not reentrant and not MT-safe (FIXME)!
    void SortChildren(const wxTreeItemId& item);

    // searching
    wxTreeItemId FindItem (const wxTreeItemId& item, const wxString& str, int flags = 0);

    // deprecated functions: use Set/GetItemImage directly
    // get the selected item image
    int GetItemSelectedImage(const wxTreeItemId& item) const
        { return GetItemImage(item, wxTreeItemIcon_Selected); }
    // set the selected item image
    void SetItemSelectedImage(const wxTreeItemId& item, int image)
        { SetItemImage(item, image, wxTreeItemIcon_Selected); }

    // implementation only from now on

    // overridden base class virtuals
    virtual bool SetBackgroundColour(const wxColour& colour);
    virtual bool SetForegroundColour(const wxColour& colour);

    // callbacks
    void OnPaint( wxPaintEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnIdle( wxIdleEvent &event );
    void OnScroll(wxScrollWinEvent& event); // ALB

    // implementation helpers
    void SendDeleteEvent(wxTreeListItem *itemBeingDeleted);

    void DoDirtyProcessing();
    
    void DrawBorder(const wxTreeItemId& item);
    void DrawLine(const wxTreeItemId& item, bool below);

    size_t GetColumnCount() const
    { return m_owner->GetHeaderWindow()->GetColumnCount(); }

    void SetMainColumn(size_t column)
    {
        if(column < GetColumnCount())
            m_main_column = column;
    }
    size_t GetMainColumn() const { return m_main_column; }

    void SetItemText(const wxTreeItemId& item, size_t column,
                     const wxString& text);
    wxString GetItemText(const wxTreeItemId& item, size_t column) const;

    void SetItemImage(const wxTreeItemId& item, size_t column, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);
    int GetItemImage(const wxTreeItemId& item, size_t column,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const;

    void SetFocus();

protected:
    wxTreeListCtrl* m_owner; // ALB

    size_t m_main_column; // ALB

    friend class wxTreeListItem;
    friend class wxTreeListRenameTimer;
    friend class wxTreeListTextCtrl;

    wxFont               m_normalFont;
    wxFont               m_boldFont;

    wxTreeListItem   *m_anchor;
    wxTreeListItem   *m_current, *m_key_current, *m_currentEdit;
    int                  m_btnWidth, m_btnWidth2;
    int                  m_btnHeight, m_btnHeight2;
    int                  m_imgWidth, m_imgWidth2;
    int                  m_imgHeight, m_imgHeight2;
    unsigned short       m_indent;
    int                  m_lineHeight;
    unsigned short       m_linespacing;
    wxPen                m_dottedPen;
    wxBrush             *m_hilightBrush,
                        *m_hilightUnfocusedBrush;
    bool                 m_hasFocus;
public:
    bool                 m_dirty;
protected:
    bool                 m_ownsImageListNormal,
                         m_ownsImageListState,
                         m_ownsImageListButtons;
    bool                 m_isDragging; // true between BEGIN/END drag events
    bool                 m_renameAccept;
    bool                 m_lastOnSame;  // last click on the same item as prev
    wxImageList         *m_imageListNormal,
                        *m_imageListState,
                        *m_imageListButtons;

    int                  m_dragCount;
    wxPoint              m_dragStart;
    wxTreeListItem   *m_dropTarget;
    wxCursor             m_oldCursor;  // cursor is changed while dragging
    wxTreeListItem   *m_oldSelection;
    wxTreeListItem   *m_underMouse; // for visual effects

    wxTimer             *m_renameTimer;
    wxString             m_renameRes;

    // char navigation
    wxTimer             *m_findTimer;
    wxString             m_findStr;

    // the common part of all ctors
    void Init();

    // misc helpers
    wxTreeItemId DoInsertItem(const wxTreeItemId& parent,
                              size_t previous,
                              const wxString& text,
                              int image, int selectedImage,
                              wxTreeItemData *data);
    bool HasButtons(void) const
        { return (m_imageListButtons != NULL) ||
                  HasFlag (wxTR_TWIST_BUTTONS|wxTR_HAS_BUTTONS); }

protected:
    void CalculateLineHeight();
    int  GetLineHeight(wxTreeListItem *item) const;
    void PaintLevel( wxTreeListItem *item, wxDC& dc, int level, int &y,
                     int x_colstart);
    void PaintItem( wxTreeListItem *item, wxDC& dc);

    void CalculateLevel( wxTreeListItem *item, wxDC &dc, int level, int &y,
                         int x_colstart);
    void CalculatePositions();
    void CalculateSize( wxTreeListItem *item, wxDC &dc );

    void RefreshSubtree( wxTreeListItem *item );
    void RefreshLine( wxTreeListItem *item );

    // redraw all selected items
    void RefreshSelected();

    // RefreshSelected() recursive helper
    void RefreshSelectedUnder(wxTreeListItem *item);

    void OnRenameTimer();
    void OnRenameAccept();

    void FillArray(wxTreeListItem*, wxArrayTreeItemIds&) const;
    void SelectItemRange( wxTreeListItem *item1, wxTreeListItem *item2 );
    bool TagAllChildrenUntilLast(wxTreeListItem *crt_item,
                                 wxTreeListItem *last_item, bool select);
    bool TagNextChildren(wxTreeListItem *crt_item, wxTreeListItem *last_item,
                         bool select);
    void UnselectAllChildren( wxTreeListItem *item );

    void DrawDropEffect(wxTreeListItem *item);

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTreeListMainWindow)
};


// timer used for enabling in-place edit
class  wxTreeListRenameTimer: public wxTimer
{
public:
    wxTreeListRenameTimer( wxTreeListMainWindow *owner );

    void Notify();

private:
    wxTreeListMainWindow   *m_owner;
};

// control used for in-place edit
class  wxTreeListTextCtrl: public wxTextCtrl
{
public:
    wxTreeListTextCtrl( wxWindow *parent,
                        const wxWindowID id,
                        bool *accept,
                        wxString *res,
                        wxTreeListMainWindow *owner,
                        const wxString &value = wxEmptyString,
                        const wxPoint &pos = wxDefaultPosition,
                        const wxSize &size = wxDefaultSize,
                        int style = wxSIMPLE_BORDER,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString &name = wxTextCtrlNameStr );

    void OnChar( wxKeyEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );

private:
    bool               *m_accept;
    wxString           *m_res;
    wxTreeListMainWindow  *m_owner;
    wxString            m_startValue;
    bool                m_finished;

    DECLARE_EVENT_TABLE()
};

// a tree item
class  wxTreeListItem
{
public:
    // ctors & dtor
    wxTreeListItem() { m_data = NULL; }
    wxTreeListItem( wxTreeListMainWindow *owner,
                    wxTreeListItem *parent,
                    const wxArrayString& text,
                    int image,
                    int selImage,
                    wxTreeItemData *data );

    ~wxTreeListItem();

    // trivial accessors
    wxArrayTreeListItems& GetChildren() { return m_children; }

    const wxString GetText() const
    {
        if(m_text.GetCount() > 0) return m_text[0];
        return wxEmptyString;
    }
    const wxString GetText(size_t col) const
    {
        if(m_text.GetCount() > col) return m_text[col];
        return wxEmptyString;
    }
    int GetImage(wxTreeItemIcon which = wxTreeItemIcon_Normal) const
        { return m_images[which]; }
    int GetImage(size_t col, wxTreeItemIcon which=wxTreeItemIcon_Normal) const
    {
        if(col == m_owner->GetMainColumn()) return m_images[which];
        if(col < m_col_images.GetCount()) return m_col_images[col];
        return NO_IMAGE;
    }
    wxTreeItemData *GetData() const { return m_data; }

    // returns the current image for the item (depending on its
    // selected/expanded/whatever state)
    int GetCurrentImage() const;

    void SetText( const wxString &text );
    void SetText(size_t col, const wxString& text) // ALB
    {
        if(col < m_text.GetCount())
            m_text[col] = text;
        else if(col < m_owner->GetColumnCount()) {
            int howmany = m_owner->GetColumnCount();
            for(int i = m_text.GetCount(); i < howmany; ++i)
                m_text.Add(wxEmptyString);
            m_text[col] = text;
        }
    }
    void SetImage(int image, wxTreeItemIcon which) { m_images[which] = image; }
    void SetImage(size_t col, int image, wxTreeItemIcon which)
    {
        if(col == m_owner->GetMainColumn()) m_images[which] = image;
        else if(col < m_col_images.GetCount())
            m_col_images[col] = image;
        else if(col < m_owner->GetColumnCount()) {
            int howmany = m_owner->GetColumnCount();
            for(int i = m_col_images.GetCount(); i < howmany; ++i)
                m_col_images.Add(NO_IMAGE);
            m_col_images[col] = image;
        }
    }

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

    wxTreeListItem *GetItemParent() const { return m_parent; }

    // operations
    // deletes all children notifying the treectrl about it if !NULL
    // pointer given
    void DeleteChildren(wxTreeListMainWindow *tree = NULL);

    // get count of all children (and grand children if 'recursively')
    size_t GetChildrenCount(bool recursively = TRUE) const;

    void Insert(wxTreeListItem *child, size_t index)
    { m_children.Insert(child, index); }

    void GetSize( int &x, int &y, const wxTreeListMainWindow* );

    // return the item at given position (or NULL if no item), onButton is
    // TRUE if the point belongs to the item's button, otherwise it lies
    // on the button's label
    wxTreeListItem *HitTest( const wxPoint& point,
                             const wxTreeListMainWindow *,
                             int &flags,
                             int level );
    wxTreeListItem *HitTest( const wxPoint& point,
                             const wxTreeListMainWindow *,
                             int &flags, int& column /*ALB*/,
                             int level );

    void Expand() { m_isCollapsed = FALSE; }
    void Collapse() { m_isCollapsed = TRUE; }

    void SetHilight( bool set = TRUE ) { m_hasHilight = set; }

    // status inquiries
    bool HasChildren() const { return !m_children.IsEmpty(); }
    bool IsSelected()  const { return m_hasHilight != 0; }
    bool IsExpanded()  const { return !m_isCollapsed; }
    bool HasPlus()     const { return m_hasPlus || HasChildren(); }
    bool IsBold()      const { return m_isBold != 0; }

    // attributes
    // get them - may be NULL
    wxTreeItemAttr *GetAttributes() const { return m_attr; }
    // get them ensuring that the pointer is not NULL
    wxTreeItemAttr& Attr()
    {
        if ( !m_attr )
        {
            m_attr = new wxTreeItemAttr;
            m_ownsAttr = TRUE;
        }
        return *m_attr;
    }
    // set them
    void SetAttributes(wxTreeItemAttr *attr)
    {
        if ( m_ownsAttr ) delete m_attr;
        m_attr = attr;
        m_ownsAttr = FALSE;
    }
    // set them and delete when done
    void AssignAttributes(wxTreeItemAttr *attr)
    {
        SetAttributes(attr);
        m_ownsAttr = TRUE;
    }

private:
    wxTreeListMainWindow  *m_owner;        // control the item belongs to

    // since there can be very many of these, we save size by chosing
    // the smallest representation for the elements and by ordering
    // the members to avoid padding.
    wxArrayString      m_text;    // labels to be rendered for item

    wxTreeItemData     *m_data;         // user-provided data

    wxArrayTreeListItems m_children; // list of children
    wxTreeListItem  *m_parent;       // parent of this item

    wxTreeItemAttr     *m_attr;         // attributes???

    // tree ctrl images for the normal, selected, expanded and
    // expanded+selected states
    short               m_images[wxTreeItemIcon_Max];
    wxArrayShort m_col_images; // images for the various columns (!= main)

    wxCoord             m_x;            // (virtual) offset from top
    wxCoord             m_y;            // (virtual) offset from left
    short               m_width;        // width of this item
    unsigned char       m_height;       // height of this item

    // use bitfields to save size
    int                 m_isCollapsed :1;
    int                 m_hasHilight  :1; // same as focused
    int                 m_hasPlus     :1; // used for item which doesn't have
                                          // children but has a [+] button
    int                 m_isBold      :1; // render the label in bold font
    int                 m_ownsAttr    :1; // delete attribute when done
};

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// translate the key or mouse event flags to the type of selection we're
// dealing with
static void EventFlagsToSelType(long style,
                                bool shiftDown,
                                bool ctrlDown,
                                bool &is_multiple,
                                bool &extended_select,
                                bool &unselect_others)
{
    is_multiple = (style & wxTR_MULTIPLE) != 0;
    extended_select = shiftDown && is_multiple;
    unselect_others = !(extended_select || (ctrlDown && is_multiple));
}

// ---------------------------------------------------------------------------
// wxTreeListRenameTimer (internal)
// ---------------------------------------------------------------------------

wxTreeListRenameTimer::wxTreeListRenameTimer( wxTreeListMainWindow *owner )
{
    m_owner = owner;
}

void wxTreeListRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}

//-----------------------------------------------------------------------------
// wxTreeListTextCtrl (internal)
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTreeListTextCtrl,wxTextCtrl)
    EVT_CHAR           (wxTreeListTextCtrl::OnChar)
    EVT_KEY_UP         (wxTreeListTextCtrl::OnKeyUp)
    EVT_KILL_FOCUS     (wxTreeListTextCtrl::OnKillFocus)
END_EVENT_TABLE()

wxTreeListTextCtrl::wxTreeListTextCtrl( wxWindow *parent,
                                        const wxWindowID id,
                                        bool *accept,
                                        wxString *res,
                                        wxTreeListMainWindow *owner,
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
    (*m_res) = wxEmptyString;
    m_startValue = value;
    m_finished = FALSE;
}

void wxTreeListTextCtrl::OnChar( wxKeyEvent &event )
{
    if (event.m_keyCode == WXK_RETURN)
    {
        (*m_accept) = TRUE;
        (*m_res) = GetValue();

        if ((*m_res) != m_startValue)
            m_owner->OnRenameAccept();

        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);

        m_finished = TRUE;
        m_owner->SetFocus(); // This doesn't work. TODO.

        return;
    }
    if (event.m_keyCode == WXK_ESCAPE)
    {
        (*m_accept) = FALSE;
        (*m_res) = wxEmptyString;

        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);

        m_finished = TRUE;
        m_owner->SetFocus(); // This doesn't work. TODO.

        return;
    }
    event.Skip();
}

void wxTreeListTextCtrl::OnKeyUp( wxKeyEvent &event )
{
    if (m_finished)
    {
        event.Skip();
        return;
    }

    // auto-grow the textctrl:
    wxSize parentSize = m_owner->GetSize();
    wxPoint myPos = GetPosition();
    wxSize mySize = GetSize();
    int sx, sy;
    GetTextExtent(GetValue() + _T("M"), &sx, &sy);
    if (myPos.x + sx > parentSize.x) sx = parentSize.x - myPos.x;
    if (mySize.x > sx) sx = mySize.x;
    SetSize(sx, -1);

    event.Skip();
}

void wxTreeListTextCtrl::OnKillFocus( wxFocusEvent &event )
{
    if (m_finished)
    {
        event.Skip();
        return;
    }

    if (!wxPendingDelete.Member(this))
        wxPendingDelete.Append(this);

    (*m_accept) = TRUE;
    (*m_res) = GetValue();

    if ((*m_res) != m_startValue)
        m_owner->OnRenameAccept();
}

//-----------------------------------------------------------------------------
//  wxTreeListHeaderWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeListHeaderWindow,wxWindow)

BEGIN_EVENT_TABLE(wxTreeListHeaderWindow,wxWindow)
    EVT_ERASE_BACKGROUND  (wxTreeListHeaderWindow::OnEraseBackground)
    EVT_PAINT             (wxTreeListHeaderWindow::OnPaint)
    EVT_MOUSE_EVENTS      (wxTreeListHeaderWindow::OnMouse)
    EVT_SET_FOCUS         (wxTreeListHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

void wxTreeListHeaderWindow::Init()
{
    m_currentCursor = (wxCursor *) NULL;
    m_isDragging = FALSE;
    m_dirty = FALSE;
    m_total_col_width = 0;
}

wxTreeListHeaderWindow::wxTreeListHeaderWindow()
{
    Init();

    m_owner = (wxTreeListMainWindow *) NULL;
    m_resizeCursor = (wxCursor *) NULL;
}

wxTreeListHeaderWindow::wxTreeListHeaderWindow( wxWindow *win,
                                                wxWindowID id,
                                                wxTreeListMainWindow *owner,
                                                const wxPoint& pos,
                                                const wxSize& size,
                                                long style,
                                                const wxString &name )
    : wxWindow( win, id, pos, size, style, name )
{
    Init();

    m_owner = owner;
    m_resizeCursor = new wxCursor(wxCURSOR_SIZEWE);

    SetBackgroundColour(wxSystemSettings::GetColour(
                            wxSYS_COLOUR_BTNFACE));
}

wxTreeListHeaderWindow::~wxTreeListHeaderWindow()
{
    delete m_resizeCursor;
}

void wxTreeListHeaderWindow::DoDrawRect( wxDC *dc, int x, int y, int w, int h )
{
#ifdef __WXGTK__
    GtkStateType state = m_parent->IsEnabled() ? GTK_STATE_NORMAL
                                               : GTK_STATE_INSENSITIVE;

    x = dc->XLOG2DEV( x );

    gtk_paint_box (m_wxwindow->style, GTK_PIZZA(m_wxwindow)->bin_window,
                   state, GTK_SHADOW_OUT,
                   (GdkRectangle*) NULL, m_wxwindow, "button",
                   x-1, y-1, w+2, h+2);
#elif defined( __WXMAC__  )
    const int m_corner = 1;

    dc->SetBrush( *wxTRANSPARENT_BRUSH );

    dc->SetPen( wxPen(wxSystemSettings::GetColour(
                          wxSYS_COLOUR_BTNSHADOW), 1, wxSOLID));
    dc->DrawLine( x+w-m_corner+1, y, x+w, y+h );  // right (outer)
    dc->DrawRectangle( x, y+h, w+1, 1 );          // bottom (outer)

    wxPen pen( wxColour( 0x88 , 0x88 , 0x88 ), 1, wxSOLID );

    dc->SetPen( pen );
    dc->DrawLine( x+w-m_corner, y, x+w-1, y+h );  // right (inner)
    dc->DrawRectangle( x+1, y+h-1, w-2, 1 );      // bottom (inner)

    dc->SetPen( *wxWHITE_PEN );
    dc->DrawRectangle( x, y, w-m_corner+1, 1 );   // top (outer)
    dc->DrawRectangle( x, y, 1, h );              // left (outer)
    dc->DrawLine( x, y+h-1, x+1, y+h-1 );
    dc->DrawLine( x+w-1, y, x+w-1, y+1 );
#else // !GTK, !Mac
    const int m_corner = 1;

    dc->SetBrush( *wxTRANSPARENT_BRUSH );

    dc->SetPen( *wxBLACK_PEN );
    dc->DrawLine( x+w-m_corner+1, y, x+w, y+h );  // right (outer)
    dc->DrawRectangle( x, y+h, w+1, 1 );          // bottom (outer)

    wxPen pen(wxSystemSettings::GetColour(
                  wxSYS_COLOUR_BTNSHADOW ), 1, wxSOLID);

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
void wxTreeListHeaderWindow::AdjustDC(wxDC& dc)
{
    int xpix;
    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );

    int x;
    m_owner->GetViewStart( &x, NULL );

    // account for the horz scrollbar offset
    dc.SetDeviceOrigin( -x * xpix, 0 );
}


void wxTreeListHeaderWindow::OnEraseBackground( wxEraseEvent& event )
{
}

void wxTreeListHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
#ifdef __WXGTK__
    wxClientDC dc( this );
#else
    wxPaintDC dc( this );
#endif

    AdjustDC( dc );

    // width and height of the entire header window
    int w, h;
    GetClientSize( &w, &h );
    m_owner->CalcUnscrolledPosition(w, 0, &w, NULL);

    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    
    dc.SetFont( GetFont() );
    dc.SetBackgroundMode(wxTRANSPARENT);

    // do *not* use the listctrl colour for headers - one day we will have a
    // function to set it separately
    //dc.SetTextForeground( *wxBLACK );
    dc.SetTextForeground(wxSystemSettings::
                            GetColour( wxSYS_COLOUR_WINDOWTEXT ));

    int x = HEADER_OFFSET_X;

    int numColumns = GetColumnCount();
    for ( int i = 0; i < numColumns && x < w; i++ )
    {
        if (!GetColumnShown (i)) continue;

        wxTreeListColumnInfo& column = GetColumn(i);
        int wCol = column.GetWidth();

        // the width of the rect to draw: make it smaller to fit entirely
        // inside the column rect
        int cw = wCol - 2;

        dc.SetPen( *wxWHITE_PEN );

        //DoDrawRect( &dc, x, HEADER_OFFSET_Y, cw, h-2 );
        wxRendererNative::Get().DrawHeaderButton(
            this, dc, wxRect(x, HEADER_OFFSET_Y, cw, h - 2),
            m_parent->IsEnabled() ? 0 : wxCONTROL_DISABLED);
        

        // if we have an image, draw it on the right of the label
        int image = column.GetImage(); //item.m_image;
        int ix = -2, iy = 0;
        wxImageList* imageList = m_owner->GetImageList();
        if(image != -1) {
            if(imageList) {
                imageList->GetSize(image, ix, iy);
            }
        //else: ignore the column image
        }

        int text_width = 0;
        int text_x = x;
        int image_offset = cw - ix - 1;

        switch(column.GetAlignment()) {
        case wxTL_ALIGN_LEFT:
            text_x += EXTRA_WIDTH;
            cw -= ix + 2;
            break;
        case wxTL_ALIGN_RIGHT:
            dc.GetTextExtent(column.GetText(), &text_width, NULL);
            text_x += cw - text_width - EXTRA_WIDTH;
            image_offset = 0;
            break;
        case wxTL_ALIGN_CENTER:
            dc.GetTextExtent(column.GetText(), &text_width, NULL);
            text_x += (cw - text_width)/2 + ix + 2;
            image_offset = (cw - text_width - ix - 2)/2;
            break;
        }

        // draw the image
        if(image != -1 && imageList) {
            imageList->Draw(image, dc, x + image_offset/*cw - ix - 1*/,
                            HEADER_OFFSET_Y + (h - 4 - iy)/2,
                            wxIMAGELIST_DRAW_TRANSPARENT);
        }

        // draw the text clipping it so that it doesn't overwrite the column
        // boundary
        wxDCClipper clipper(dc, x, HEADER_OFFSET_Y, cw, h - 4 );

        dc.DrawText( column.GetText(),
                     text_x, HEADER_OFFSET_Y + EXTRA_HEIGHT );

        x += wCol;
    }

    int more_w = m_owner->GetSize().x - x -1;
    if (more_w > 0)
    {
        //DoDrawRect( &dc, x, HEADER_OFFSET_Y, more_w, h-2 );
        wxRendererNative::Get().DrawHeaderButton(
            this, dc, wxRect(x, HEADER_OFFSET_Y, more_w, h-2),
            m_parent->IsEnabled() ? 0 : wxCONTROL_DISABLED);
    }
}

void wxTreeListHeaderWindow::DrawCurrent()
{
    int x1 = m_currentX;
    int y1 = 0;
    ClientToScreen( &x1, &y1 );

    int x2 = m_currentX-1;
#ifdef __WXMSW__
    ++x2; // but why ?
#endif
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

void wxTreeListHeaderWindow::OnMouse( wxMouseEvent &event )
{
    // we want to work with logical coords
    int x;
    m_owner->CalcUnscrolledPosition(event.GetX(), 0, &x, NULL);
    int y = event.GetY();

    if (m_isDragging)
    {
        SendListEvent(wxEVT_COMMAND_LIST_COL_DRAGGING,
                      event.GetPosition());

        // we don't draw the line beyond our window, but we allow dragging it
        // there
        int w = 0;
        GetClientSize( &w, NULL );
        m_owner->CalcUnscrolledPosition(w, 0, &w, NULL);
        w -= 6;

        // erase the line if it was drawn
        if ( m_currentX < w )
            DrawCurrent();

        if (event.ButtonUp())
        {
            ReleaseMouse();
            m_isDragging = FALSE;
            m_dirty = TRUE;
            SetColumnWidth( m_column, m_currentX - m_minX );
            Refresh();
            SendListEvent(wxEVT_COMMAND_LIST_COL_END_DRAG,
                          event.GetPosition());
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

        // find the column where this event occurred
        int countCol = GetColumnCount();
        for (int col = 0; col < countCol; col++)
        {
            if (!GetColumnShown (col)) continue;
            xpos += GetColumnWidth (col);
            m_column = col;

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

        if (event.LeftDown() || event.RightUp())
        {
            if (hit_border && event.LeftDown())
            {
                m_isDragging = TRUE;
                m_currentX = x;
                DrawCurrent();
                CaptureMouse();
                SendListEvent(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG,
                              event.GetPosition());
            }
            else // click on a column
            {
                SendListEvent( event.LeftDown()
                               ? wxEVT_COMMAND_LIST_COL_CLICK
                               : wxEVT_COMMAND_LIST_COL_RIGHT_CLICK,
                               event.GetPosition());
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

void wxTreeListHeaderWindow::OnSetFocus( wxFocusEvent &WXUNUSED(event) )
{
    m_owner->SetFocus();
}

void wxTreeListHeaderWindow::SendListEvent(wxEventType type, wxPoint pos)
{
    wxWindow *parent = GetParent();
    wxListEvent le( type, parent->GetId() );
    le.SetEventObject( parent );
    le.m_pointDrag = pos;

    // the position should be relative to the parent window, not
    // this one for compatibility with MSW and common sense: the
    // user code doesn't know anything at all about this header
    // window, so why should it get positions relative to it?
    le.m_pointDrag.y -= GetSize().y;

    le.m_col = m_column;
    parent->GetEventHandler()->ProcessEvent( le );
}

void wxTreeListHeaderWindow::AddColumn(const wxTreeListColumnInfo& col)
{
    m_columns.Add(col);
    m_total_col_width += col.GetWidth();
    //m_owner->GetHeaderWindow()->Refresh();
    //m_dirty = TRUE;
    m_owner->AdjustMyScrollbars();
    m_owner->m_dirty = TRUE;
    Refresh();
}

void wxTreeListHeaderWindow::SetColumnWidth(size_t column, size_t width)
{
    if(column < GetColumnCount()) {
        m_total_col_width -= m_columns[column].GetWidth();
        m_columns[column].SetWidth(width);
        m_total_col_width += width;
        m_owner->AdjustMyScrollbars();
        m_owner->m_dirty = TRUE;
        //m_dirty = TRUE;
        Refresh();
    }
}


void wxTreeListHeaderWindow::InsertColumn(size_t before,
                                          const wxTreeListColumnInfo& col)
{
    wxCHECK_RET(before < GetColumnCount(), wxT("Invalid column index"));
    m_columns.Insert(col, before);
    m_total_col_width += col.GetWidth();
    //m_dirty = TRUE;
    //m_owner->GetHeaderWindow()->Refresh();
    m_owner->AdjustMyScrollbars();
    m_owner->m_dirty = TRUE;
    Refresh();
}

void wxTreeListHeaderWindow::RemoveColumn(size_t column)
{
    wxCHECK_RET(column < GetColumnCount(), wxT("Invalid column"));
    m_total_col_width -= m_columns[column].GetWidth();
    m_columns.RemoveAt(column);
    //m_dirty = TRUE;
    m_owner->AdjustMyScrollbars();
    m_owner->m_dirty = TRUE;
    Refresh();
}

void wxTreeListHeaderWindow::SetColumn(size_t column,
                                       const wxTreeListColumnInfo& info)
{
    wxCHECK_RET(column < GetColumnCount(), wxT("Invalid column"));
    size_t w = m_columns[column].GetWidth();
    m_columns[column] = info;
    //m_owner->GetHeaderWindow()->Refresh();
    //m_dirty = TRUE;
    if(w != info.GetWidth()) {
        m_total_col_width += info.GetWidth() - w;
        m_owner->AdjustMyScrollbars();
        m_owner->m_dirty = TRUE;
    }
    Refresh();
}

// ---------------------------------------------------------------------------
// wxTreeListItem
// ---------------------------------------------------------------------------

wxTreeListItem::wxTreeListItem(wxTreeListMainWindow *owner,
                                     wxTreeListItem *parent,
                                     const wxArrayString& text,
                                     int image, int selImage,
                                     wxTreeItemData *data)
                 : m_text(text)
{
    m_images[wxTreeItemIcon_Normal] = image;
    m_images[wxTreeItemIcon_Selected] = selImage;
    m_images[wxTreeItemIcon_Expanded] = NO_IMAGE;
    m_images[wxTreeItemIcon_SelectedExpanded] = NO_IMAGE;

    m_data = data;
    m_x = m_y = 0;

    m_isCollapsed = TRUE;
    m_hasHilight = FALSE;
    m_hasPlus = FALSE;
    m_isBold = FALSE;

    m_owner = owner;

    m_parent = parent;

    m_attr = (wxTreeItemAttr *)NULL;
    m_ownsAttr = FALSE;

    // We don't know the height here yet.
    m_width = 0;
    m_height = 0;
}

wxTreeListItem::~wxTreeListItem()
{
    delete m_data;

    if (m_ownsAttr) delete m_attr;

    wxASSERT_MSG( m_children.IsEmpty(),
                  wxT("please call DeleteChildren() before deleting the item") );
}

void wxTreeListItem::DeleteChildren(wxTreeListMainWindow *tree)
{
    size_t count = m_children.Count();
    for ( size_t n = 0; n < count; n++ )
    {
        wxTreeListItem *child = m_children[n];
        if (tree)
            tree->SendDeleteEvent(child);

        child->DeleteChildren(tree);
        delete child;
    }

    m_children.Empty();
}

void wxTreeListItem::SetText( const wxString &text )
{
    if(m_text.GetCount() > 0) m_text[0] = text;
    else {
        m_text.Add(text);
    }
}

size_t wxTreeListItem::GetChildrenCount(bool recursively) const
{
    size_t count = m_children.Count();
    if ( !recursively )
        return count;

    size_t total = count;
    for (size_t n = 0; n < count; ++n)
    {
        total += m_children[n]->GetChildrenCount();
    }

    return total;
}

void wxTreeListItem::GetSize( int &x, int &y,
                                 const wxTreeListMainWindow *theButton )
{
    int bottomY=m_y+theButton->GetLineHeight(this);
    if ( y < bottomY ) y = bottomY;
    int width = m_x +  m_width;
    if ( x < width ) x = width;

    if (IsExpanded())
    {
        size_t count = m_children.Count();
        for ( size_t n = 0; n < count; ++n )
        {
            m_children[n]->GetSize( x, y, theButton );
        }
    }
}

wxTreeListItem *wxTreeListItem::HitTest(const wxPoint& point,
                                        const wxTreeListMainWindow *theCtrl,
                                        int &flags,
                                        int level)
{
    // for a hidden root node, don't evaluate it, but do evaluate children
    if (!(theCtrl->HasFlag(wxTR_HIDE_ROOT) && (level == 0)))
    {
        // evaluate the item
        int h = theCtrl->GetLineHeight(this);
        if ((point.y > m_y) && (point.y <= m_y + h))
        {
            // check for above/below middle
            int y_mid = m_y + h/2;
            if (point.y < y_mid )
                flags |= wxTREE_HITTEST_ONITEMUPPERPART;
            else
                flags |= wxTREE_HITTEST_ONITEMLOWERPART;

            // check for button hit
            int xCross = m_x; // - theCtrl->GetLineSpacing();
#ifdef __WXMAC__
            // according to the drawing code the triangels are drawn
            // at -4 , -4  from the position up to +10/+10 max
            if ((point.x > xCross-4) && (point.x < xCross+10) &&
                (point.y > y_mid-4) && (point.y < y_mid+10) &&
                HasPlus() && theCtrl->HasButtons() )
#else
            // 5 is the size of the plus sign
            if ((point.x > xCross-6) && (point.x < xCross+6) &&
                (point.y > y_mid-6) && (point.y < y_mid+6) &&
                HasPlus() && theCtrl->HasButtons() )
#endif
            {
                flags |= wxTREE_HITTEST_ONITEMBUTTON;
                return this;
            }

            // check for image hit
            if (theCtrl->m_imgWidth > 0 && GetImage() != NO_IMAGE) {
                int imgX = m_x - theCtrl->m_imgWidth2;
                if (HasPlus() && theCtrl->HasButtons())
                    imgX += theCtrl->m_btnWidth + LINEATROOT;
                int imgY = y_mid - theCtrl->m_imgHeight2;
                if ((point.x >= imgX) && (point.x <= (imgX + theCtrl->m_imgWidth)) &&
                    (point.y >= imgY) && (point.y <= (imgY + theCtrl->m_imgHeight))) {
                    flags |= wxTREE_HITTEST_ONITEMICON;
                    return this;
                }
            }

            // check for label hit
            int lblX = m_x - theCtrl->m_imgWidth2 + theCtrl->m_imgWidth + MARGIN;
            if ((point.x >= lblX) && (point.x <= (m_x + m_width)) &&
                (point.y >= m_y) && (point.y <= (m_y + h))) {
                flags |= wxTREE_HITTEST_ONITEMLABEL;
                return this;
            }

            // else check for indent
            if (point.x < m_x) {
                flags |= wxTREE_HITTEST_ONITEMINDENT;
                return this;
            }

            // else check for item right???
            if (point.x > m_x + m_width) {
                flags |= wxTREE_HITTEST_ONITEMRIGHT;
                return this;
            }

        }

        // if children are expanded, fall through to evaluate them
        if (m_isCollapsed) return (wxTreeListItem*) NULL;
    }

    // evaluate children
    size_t count = m_children.Count();
    for ( size_t n = 0; n < count; n++ )
    {
        wxTreeListItem *res = m_children[n]->HitTest(point, theCtrl,
                                                     flags, level + 1);
        if ( res != NULL )
            return res;
    }

    return (wxTreeListItem*) NULL;
}

// ALB
wxTreeListItem *wxTreeListItem::HitTest(const wxPoint& point,
                                        const wxTreeListMainWindow *theCtrl,
                                        int &flags, int& column, int level)
{
    column = theCtrl->GetMainColumn(); //-1;

    wxTreeListItem* res = HitTest(point, theCtrl, flags, level);
    if(!res) {
        column = -1;
        return res;
    }

    wxTreeListHeaderWindow* header_win = theCtrl->m_owner->GetHeaderWindow();
    if (point.x >= header_win->GetWidth())
        column = -1;
    else if(flags & wxTREE_HITTEST_ONITEMINDENT) {
        int x = 0;
        for(size_t i = 0; i < theCtrl->GetMainColumn(); ++i) {
            if (!header_win->GetColumnShown(i)) continue;
            int w = header_win->GetColumnWidth(i);
            if(point.x >= x && point.x < x+w) {
                flags ^= wxTREE_HITTEST_ONITEMINDENT;
                flags |= wxTREE_HITTEST_ONITEMCOLUMN;
                column = i;
                return res;
            }
        }
    }
    else if(flags & wxTREE_HITTEST_ONITEMRIGHT) {
        int x = 0;
        size_t i;
        for(i = 0; i < theCtrl->GetMainColumn()+1; ++i) {
            if (!header_win->GetColumnShown(i)) continue;
            x += header_win->GetColumnWidth(i);
        }
        for(i = theCtrl->GetMainColumn()+1; i < theCtrl->GetColumnCount(); ++i) {
            if (!header_win->GetColumnShown(i)) continue;
            int w = header_win->GetColumnWidth(i);
            if(point.x >= x && point.x < x+w) {
                flags ^= wxTREE_HITTEST_ONITEMRIGHT;
                flags |= wxTREE_HITTEST_ONITEMCOLUMN;
                column = i;
                return res;
            }
            x += w;
        }
    }

    return res;
}


int wxTreeListItem::GetCurrentImage() const
{
    int image = NO_IMAGE;
    if ( IsExpanded() )
    {
        if ( IsSelected() )
        {
            image = GetImage(wxTreeItemIcon_SelectedExpanded);
        }

        if ( image == NO_IMAGE )
        {
            // we usually fall back to the normal item, but try just the
            // expanded one (and not selected) first in this case
            image = GetImage(wxTreeItemIcon_Expanded);
        }
    }
    else // not expanded
    {
        if ( IsSelected() )
            image = GetImage(wxTreeItemIcon_Selected);
    }

    // maybe it doesn't have the specific image we want,
    // try the default one instead
    if ( image == NO_IMAGE ) image = GetImage();

    return image;
}

// ---------------------------------------------------------------------------
// wxTreeListMainWindow implementation
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeListMainWindow, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxTreeListMainWindow, wxScrolledWindow)
    EVT_PAINT          (wxTreeListMainWindow::OnPaint)
    EVT_MOUSE_EVENTS   (wxTreeListMainWindow::OnMouse)
    EVT_CHAR           (wxTreeListMainWindow::OnChar)
    EVT_SET_FOCUS      (wxTreeListMainWindow::OnSetFocus)
    EVT_KILL_FOCUS     (wxTreeListMainWindow::OnKillFocus)
    EVT_IDLE           (wxTreeListMainWindow::OnIdle)
    EVT_SCROLLWIN      (wxTreeListMainWindow::OnScroll)
END_EVENT_TABLE()


// ---------------------------------------------------------------------------
// construction/destruction
// ---------------------------------------------------------------------------

void wxTreeListMainWindow::Init()
{
    m_current = m_key_current = m_anchor = (wxTreeListItem *) NULL;
    m_hasFocus = FALSE;
    m_dirty = FALSE;

    m_lineHeight = LINEHEIGHT;
    m_indent = MININDENT; // min. indent
    m_linespacing = 4;
    m_imgWidth = 0, m_imgWidth2 = 0;
    m_imgHeight = 0, m_imgHeight2 = 0;

    m_hilightBrush = new wxBrush
                         (
                            wxSystemSettings::GetColour
                            (
                                wxSYS_COLOUR_HIGHLIGHT
                            ),
                            wxSOLID
                         );

    m_hilightUnfocusedBrush = new wxBrush
                              (
                                 wxSystemSettings::GetColour
                                 (
                                     wxSYS_COLOUR_BTNSHADOW
                                 ),
                                 wxSOLID
                              );

    m_imageListNormal = m_imageListButtons =
    m_imageListState = (wxImageList *) NULL;
    m_ownsImageListNormal = m_ownsImageListButtons =
    m_ownsImageListState = FALSE;

    m_dragCount = 0;
    m_isDragging = FALSE;
    m_dropTarget = m_oldSelection = (wxTreeListItem *)NULL;

    m_renameTimer = new wxTreeListRenameTimer( this );
    m_lastOnSame = FALSE;

    m_findTimer = new wxTimer (this, -1);

    m_underMouse = NULL;

#ifdef __WXMAC_CARBON__
    m_normalFont.MacCreateThemeFont( kThemeViewsFont ) ;
#else
    m_normalFont = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
#endif
    m_boldFont = wxFont( m_normalFont.GetPointSize(),
                         m_normalFont.GetFamily(),
                         m_normalFont.GetStyle(),
                         wxBOLD,
                         m_normalFont.GetUnderlined(),
                         m_normalFont.GetFaceName(),
                         m_normalFont.GetEncoding());
}


bool wxTreeListMainWindow::Create(wxTreeListCtrl *parent,
                                  wxWindowID id,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
                                  const wxValidator &validator,
                                  const wxString& name )
{
#ifdef __WXMAC__
    if ( !(style & wxTR_DONT_ADJUST_MAC))
    {
        int major,minor;
        wxGetOsVersion( &major, &minor );

        if (style & wxTR_HAS_BUTTONS) style |= wxTR_TWIST_BUTTONS;
        if (style & wxTR_HAS_BUTTONS) style &= ~wxTR_HAS_BUTTONS;
        style &= ~wxTR_LINES_AT_ROOT;
        style |= wxTR_NO_LINES;
        if (major < 10)
            style |= wxTR_ROW_LINES;
    }
#endif
    
    wxScrolledWindow::Create( parent, id, pos, size,
                              style|wxHSCROLL|wxVSCROLL, name );

#if wxUSE_VALIDATORS
    SetValidator( validator );
#endif

    SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_LISTBOX ) );

#ifdef __WXMSW__
    {
        int i, j;
        wxBitmap bmp(8, 8);
        wxMemoryDC bdc;
        bdc.SelectObject(bmp);
        bdc.SetPen(*wxGREY_PEN);
        bdc.DrawRectangle(-1, -1, 10, 10);
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                if (!((i + j) & 1)) {
                    bdc.DrawPoint(i, j);
                }
            }
        }

        m_dottedPen = wxPen(bmp, 1);
    }
#else
    //m_dottedPen = wxPen( *wxGREY_PEN, 1, wxDOT );  // too slow under XFree86
    m_dottedPen = wxPen( wxT("grey"), 0, 0 ); // Bitmap based pen is not supported by GTK!
#endif

    // ALB
    m_owner = parent;
    m_main_column = 0;

    return TRUE;
}

wxTreeListMainWindow::~wxTreeListMainWindow()
{
    delete m_hilightBrush;
    delete m_hilightUnfocusedBrush;

    DeleteAllItems();

    delete m_renameTimer;
    delete m_findTimer;
    if (m_ownsImageListNormal) delete m_imageListNormal;
    if (m_ownsImageListState) delete m_imageListState;
    if (m_ownsImageListButtons) delete m_imageListButtons;
}



//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------

size_t wxTreeListMainWindow::GetCount() const
{
    return m_anchor == NULL ? 0u : m_anchor->GetChildrenCount();
}

void wxTreeListMainWindow::SetIndent(unsigned int indent)
{
    m_indent = indent;
    m_dirty = TRUE;
}

void wxTreeListMainWindow::SetLineSpacing(unsigned int spacing)
{
    m_linespacing = spacing;
    m_dirty = TRUE;
    CalculateLineHeight();
}

size_t wxTreeListMainWindow::GetChildrenCount(const wxTreeItemId& item,
                                              bool recursively)
{
    wxCHECK_MSG( item.IsOk(), 0u, wxT("invalid tree item") );

    return ((wxTreeListItem*) item.m_pItem)->GetChildrenCount(recursively);
}

void wxTreeListMainWindow::SetWindowStyle(const long styles)
{
    // right now, just sets the styles.  Eventually, we may
    // want to update the inherited styles, but right now
    // none of the parents has updatable styles
    m_windowStyle = styles;
    m_dirty = TRUE;
}

//-----------------------------------------------------------------------------
// functions to work with tree items
//-----------------------------------------------------------------------------

int wxTreeListMainWindow::GetItemImage(const wxTreeItemId& item, size_t column,
                                       wxTreeItemIcon which) const
{
    wxCHECK_MSG( item.IsOk(), -1, wxT("invalid tree item") );

    return ((wxTreeListItem*) item.m_pItem)->GetImage(column, which);
}

wxTreeItemData *wxTreeListMainWindow::GetItemData(const wxTreeItemId& item)
    const
{
    wxCHECK_MSG( item.IsOk(), NULL, wxT("invalid tree item") );

    return ((wxTreeListItem*) item.m_pItem)->GetData();
}

bool wxTreeListMainWindow::GetItemBold(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), FALSE, wxT("invalid tree item"));
    return ((wxTreeListItem *)item.m_pItem)->IsBold();
}

wxColour wxTreeListMainWindow::GetItemTextColour(const wxTreeItemId& item)
    const
{
    wxCHECK_MSG( item.IsOk(), wxNullColour, wxT("invalid tree item") );

    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    return pItem->Attr().GetTextColour();
}

wxColour wxTreeListMainWindow::GetItemBackgroundColour(
    const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), wxNullColour, wxT("invalid tree item") );

    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    return pItem->Attr().GetBackgroundColour();
}

wxFont wxTreeListMainWindow::GetItemFont(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), wxNullFont, wxT("invalid tree item") );

    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    return pItem->Attr().GetFont();
}



void wxTreeListMainWindow::SetItemImage(const wxTreeItemId& item,
                                        size_t column,
                                        int image, wxTreeItemIcon which)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->SetImage(column, image, which);

    wxClientDC dc(this);
    CalculateSize(pItem, dc);
    RefreshLine(pItem);
}

void wxTreeListMainWindow::SetItemData(const wxTreeItemId& item,
                                       wxTreeItemData *data)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    ((wxTreeListItem*) item.m_pItem)->SetData(data);
}

void wxTreeListMainWindow::SetItemHasChildren(const wxTreeItemId& item,
                                              bool has)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->SetHasPlus(has);
    RefreshLine(pItem);
}

void wxTreeListMainWindow::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    // avoid redrawing the tree if no real change
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    if ( pItem->IsBold() != bold )
    {
        pItem->SetBold(bold);
        RefreshLine(pItem);
    }
}

void wxTreeListMainWindow::SetItemTextColour(const wxTreeItemId& item,
                                             const wxColour& col)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->Attr().SetTextColour(col);
    RefreshLine(pItem);
}

void wxTreeListMainWindow::SetItemBackgroundColour(const wxTreeItemId& item,
                                                   const wxColour& col)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->Attr().SetBackgroundColour(col);
    RefreshLine(pItem);
}

void wxTreeListMainWindow::SetItemFont(const wxTreeItemId& item,
                                       const wxFont& font)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->Attr().SetFont(font);
    RefreshLine(pItem);
}

bool wxTreeListMainWindow::SetFont( const wxFont &font )
{
    wxScrolledWindow::SetFont(font);

    m_normalFont = font ;
    m_boldFont = wxFont( m_normalFont.GetPointSize(),
                            m_normalFont.GetFamily(),
                            m_normalFont.GetStyle(),
                            wxBOLD,
                            m_normalFont.GetUnderlined());

    return TRUE;
}


// ----------------------------------------------------------------------------
// item status inquiries
// ----------------------------------------------------------------------------

bool wxTreeListMainWindow::IsVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    // An item is only visible if it's not a descendant of a collapsed item
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    wxTreeListItem* parent = pItem->GetItemParent();
    while (parent)
    {
        if (!parent->IsExpanded())
            return FALSE;
        parent = parent->GetItemParent();
    }

    int startX, startY;
    GetViewStart(& startX, & startY);

    wxSize clientSize = GetClientSize();

    wxRect rect;
    if (!GetBoundingRect(item, rect))
        return FALSE;
    if (rect.GetWidth() == 0 || rect.GetHeight() == 0)
        return FALSE;
    if (rect.GetBottom() < 0 || rect.GetTop() > clientSize.y)
        return FALSE;
    if (rect.GetRight() < 0 || rect.GetLeft() > clientSize.x)
        return FALSE;

    return TRUE;
}

bool wxTreeListMainWindow::ItemHasChildren(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    // consider that the item does have children if it has the "+" button: it
    // might not have them (if it had never been expanded yet) but then it
    // could have them as well and it's better to err on this side rather than
    // disabling some operations which are restricted to the items with
    // children for an item which does have them
    return ((wxTreeListItem*) item.m_pItem)->HasPlus();
}

bool wxTreeListMainWindow::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    return ((wxTreeListItem*) item.m_pItem)->IsExpanded();
}

bool wxTreeListMainWindow::IsSelected(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    return ((wxTreeListItem*) item.m_pItem)->IsSelected();
}

bool wxTreeListMainWindow::IsBold(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    return ((wxTreeListItem*) item.m_pItem)->IsBold();
}

// ----------------------------------------------------------------------------
// navigation
// ----------------------------------------------------------------------------

wxTreeItemId wxTreeListMainWindow::GetItemParent(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    return ((wxTreeListItem*) item.m_pItem)->GetItemParent();
}

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListMainWindow::GetFirstChild(const wxTreeItemId& item,
                                                 long& cookie) const
#else
wxTreeItemId wxTreeListMainWindow::GetFirstChild(const wxTreeItemId& item,
                                                 wxTreeItemIdValue& cookie) const
#endif
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    cookie = 0;
    return GetNextChild(item, cookie);
}

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListMainWindow::GetNextChild(const wxTreeItemId& item,
                                                long& cookie) const
#else
wxTreeItemId wxTreeListMainWindow::GetNextChild(const wxTreeItemId& item,
                                                wxTreeItemIdValue& cookie) const
#endif
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    wxArrayTreeListItems& children = ((wxTreeListItem*) item.m_pItem)->GetChildren();

    // it's ok to cast cookie to size_t, we never have indices big enough to
    // overflow "void *"
    size_t *pIndex = (size_t *)&cookie;
    if ( *pIndex < children.Count() )
    {
        return children.Item((*pIndex)++);
    }
    else
    {
        // there are no more of them
        return wxTreeItemId();
    }
}

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListMainWindow::GetPrevChild(const wxTreeItemId& item,
                                                long& cookie) const
#else
wxTreeItemId wxTreeListMainWindow::GetPrevChild(const wxTreeItemId& item,
                                                wxTreeItemIdValue& cookie) const
#endif
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    wxArrayTreeListItems& children = ((wxTreeListItem*) item.m_pItem)->GetChildren();

    // it's ok to cast cookie to size_t, we never have indices big enough to
    // overflow "void *"
    size_t *pIndex = (size_t *)&cookie;
    if ( *pIndex > 0 )
    {
        return children.Item(--(*pIndex));
    }
    else
    {
        // there are no more of them
        return wxTreeItemId();
    }
}

wxTreeItemId wxTreeListMainWindow::GetLastChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    wxArrayTreeListItems& children = ((wxTreeListItem*) item.m_pItem)->GetChildren();
    return (children.IsEmpty() ? wxTreeItemId() : wxTreeItemId(children.Last()));
}

wxTreeItemId wxTreeListMainWindow::GetNextSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    wxTreeListItem *i = (wxTreeListItem*) item.m_pItem;
    wxTreeListItem *parent = i->GetItemParent();
    if ( parent == NULL )
    {
        // root item doesn't have any siblings
        return wxTreeItemId();
    }

    wxArrayTreeListItems& siblings = parent->GetChildren();
    int index = siblings.Index(i);
    wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

    size_t n = (size_t)(index + 1);
    return n == siblings.Count() ? wxTreeItemId() : wxTreeItemId(siblings[n]);
}

wxTreeItemId wxTreeListMainWindow::GetPrevSibling(const wxTreeItemId& item)
    const
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    wxTreeListItem *i = (wxTreeListItem*) item.m_pItem;
    wxTreeListItem *parent = i->GetItemParent();
    if ( parent == NULL )
    {
        // root item doesn't have any siblings
        return wxTreeItemId();
    }

    wxArrayTreeListItems& siblings = parent->GetChildren();
    int index = siblings.Index(i);
    wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

    return index == 0 ? wxTreeItemId()
                      : wxTreeItemId(siblings[(size_t)(index - 1)]);
}

// Only for internal use right now, but should probably be public
wxTreeItemId wxTreeListMainWindow::GetNext(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    wxTreeListItem *i = (wxTreeListItem*) item.m_pItem;

    // First see if there are any children.
    wxArrayTreeListItems& children = i->GetChildren();
    if (children.GetCount() > 0)
    {
         return children.Item(0);
    }
    else
    {
         // Try a sibling of this or ancestor instead
         wxTreeItemId p = item;
         wxTreeItemId toFind;
         do
         {
              toFind = GetNextSibling(p);
              p = GetItemParent(p);
         } while (p.IsOk() && !toFind.IsOk());
         return toFind;
    }
}

wxTreeItemId wxTreeListMainWindow::GetFirstVisibleItem() const
{
    wxTreeItemId id = GetRootItem();
    if (!id.IsOk())
        return id;

    do
    {
        if (IsVisible(id))
              return id;
        id = GetNext(id);
    } while (id.IsOk());

    return wxTreeItemId();
}

wxTreeItemId wxTreeListMainWindow::GetNextVisible(const wxTreeItemId& item)
    const
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    wxTreeItemId id = item;
    if (id.IsOk())
    {
        while (id = GetNext(id), id.IsOk())
        {
            if (IsVisible(id))
                return id;
        }
    }
    return wxTreeItemId();
}

wxTreeItemId wxTreeListMainWindow::GetPrevVisible(const wxTreeItemId& item)
    const
{
    wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

    wxFAIL_MSG(wxT("not implemented"));

    return wxTreeItemId();
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

wxTreeItemId wxTreeListMainWindow::DoInsertItem(const wxTreeItemId& parentId,
                                      size_t previous,
                                      const wxString& text,
                                      int image, int selImage,
                                      wxTreeItemData *data)
{
    wxTreeListItem *parent = (wxTreeListItem*) parentId.m_pItem;
    if ( !parent )
    {
        // should we give a warning here?
        return AddRoot(text, image, selImage, data);
    }

    m_dirty = TRUE;     // do this first so stuff below doesn't cause flicker

    // ALB
    wxArrayString arr;
    arr.Alloc(GetColumnCount());
    for(size_t i = 0; i < GetColumnCount(); ++i) {
        arr.Add(wxEmptyString);
    }
    arr[m_main_column] = text;
    wxTreeListItem *item =
        new wxTreeListItem( this, parent, arr, image, selImage, data );

    if ( data != NULL )
    {
        data->SetId((void*)item);
    }

    parent->Insert( item, previous );

    return item;
}

wxTreeItemId wxTreeListMainWindow::AddRoot(const wxString& text,
                                 int image, int selImage,
                                 wxTreeItemData *data)
{
    wxCHECK_MSG(!m_anchor, wxTreeItemId(), wxT("tree can have only one root"));
    wxCHECK_MSG(GetColumnCount(), wxTreeItemId(), wxT("Add column(s) before adding the root item"));

    m_dirty = TRUE;     // do this first so stuff below doesn't cause flicker

    // ALB
    wxArrayString arr;
    arr.Alloc(GetColumnCount());
    for(size_t i = 0; i < GetColumnCount(); ++i) {
        arr.Add(wxEmptyString);
    }
    arr[m_main_column] = text;
    m_anchor = new wxTreeListItem( this, (wxTreeListItem *)NULL, arr,
                                      image, selImage, data);
#if 0
    if (HasFlag(wxTR_HIDE_ROOT))
    {
        // if root is hidden, make sure we can navigate
        // into children
        m_anchor->SetHasPlus();
        Expand(m_anchor);
    }
#endif
    if ( data != NULL )
    {
        data->SetId((void*)m_anchor);
    }

    if (!HasFlag(wxTR_MULTIPLE))
    {
        m_current = m_key_current = m_anchor;
        m_current->SetHilight( TRUE );
    }

    return m_anchor;
}

wxTreeItemId wxTreeListMainWindow::PrependItem(const wxTreeItemId& parent,
                                     const wxString& text,
                                     int image, int selImage,
                                     wxTreeItemData *data)
{
    return DoInsertItem(parent, 0u, text, image, selImage, data);
}

wxTreeItemId wxTreeListMainWindow::InsertItem(const wxTreeItemId& parentId,
                                    const wxTreeItemId& idPrevious,
                                    const wxString& text,
                                    int image, int selImage,
                                    wxTreeItemData *data)
{
    wxTreeListItem *parent = (wxTreeListItem*) parentId.m_pItem;
    if ( !parent )
    {
        // should we give a warning here?
        return AddRoot(text, image, selImage, data);
    }

    int index = parent->GetChildren().Index((wxTreeListItem*) idPrevious.m_pItem);
    wxASSERT_MSG( index != wxNOT_FOUND,
                  wxT("previous item in wxTreeListMainWindow::InsertItem() is not a sibling") );

    return DoInsertItem(parentId, (size_t)++index, text, image, selImage, data);
}

wxTreeItemId wxTreeListMainWindow::InsertItem(const wxTreeItemId& parentId,
                                    size_t before,
                                    const wxString& text,
                                    int image, int selImage,
                                    wxTreeItemData *data)
{
    wxTreeListItem *parent = (wxTreeListItem*) parentId.m_pItem;
    if ( !parent )
    {
        // should we give a warning here?
        return AddRoot(text, image, selImage, data);
    }

    return DoInsertItem(parentId, before, text, image, selImage, data);
}

wxTreeItemId wxTreeListMainWindow::AppendItem(const wxTreeItemId& parentId,
                                    const wxString& text,
                                    int image, int selImage,
                                    wxTreeItemData *data)
{
    wxTreeListItem *parent = (wxTreeListItem*) parentId.m_pItem;
    if ( !parent )
    {
        // should we give a warning here?
        return AddRoot(text, image, selImage, data);
    }

    return DoInsertItem( parent, parent->GetChildren().Count(), text,
                         image, selImage, data);
}

void wxTreeListMainWindow::SendDeleteEvent(wxTreeListItem *item)
{
    wxTreeEvent event( wxEVT_COMMAND_TREE_DELETE_ITEM, m_owner->GetId() );
    event.SetItem((void*)item);
    event.SetEventObject( /*this*/m_owner );
    m_owner->ProcessEvent( event );
}

void wxTreeListMainWindow::DeleteChildren(const wxTreeItemId& itemId)
{
    m_dirty = TRUE;     // do this first so stuff below doesn't cause flicker

    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;
    item->DeleteChildren(this);
}

void wxTreeListMainWindow::Delete(const wxTreeItemId& itemId)
{
    m_dirty = TRUE;     // do this first so stuff below doesn't cause flicker

    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    // don't stay with invalid m_key_current or we will crash in
    // the next call to OnChar()
    bool changeKeyCurrent = FALSE;
    wxTreeListItem *itemKey = m_key_current;
    while ( itemKey )
    {
        if ( itemKey == item )
        {
            // m_key_current is a descendant of the item being deleted
            changeKeyCurrent = TRUE;
            break;
        }
        itemKey = itemKey->GetItemParent();
    }

    wxTreeListItem *parent = item->GetItemParent();
    if ( parent )
    {
        parent->GetChildren().Remove( item );  // remove by value
    }

    if ( changeKeyCurrent )
    {
        // may be NULL or not
        m_key_current = parent;
    }

    item->DeleteChildren(this);
    SendDeleteEvent(item);
    delete item;
}

void wxTreeListMainWindow::DeleteAllItems()
{
    if ( m_anchor )
    {
        m_dirty = TRUE;

        m_anchor->DeleteChildren(this);
        delete m_anchor;

        m_anchor = NULL;
    }
}

void wxTreeListMainWindow::Expand(const wxTreeItemId& itemId)
{
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    wxCHECK_RET( item, _T("invalid item in wxTreeListMainWindow::Expand") );

    if ( !item->HasPlus() )
        return;

    if ( item->IsExpanded() )
        return;

    wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_EXPANDING, m_owner->GetId() );
    event.SetItem( (void*)item );
    event.SetEventObject( /*this*/m_owner );

    if ( m_owner->ProcessEvent( event ) && !event.IsAllowed() )
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

void wxTreeListMainWindow::ExpandAll(const wxTreeItemId& item)
{
    Expand(item);
    if ( IsExpanded(item) )
    {
#if !wxCHECK_VERSION(2, 5, 0)
        long cookie;
#else
        wxTreeItemIdValue cookie;
#endif
        wxTreeItemId child = GetFirstChild(item, cookie);
        while ( child.IsOk() )
        {
            ExpandAll(child);

            child = GetNextChild(item, cookie);
        }
    }
}

void wxTreeListMainWindow::Collapse(const wxTreeItemId& itemId)
{
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    if ( !item->IsExpanded() )
        return;

    wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_COLLAPSING, m_owner->GetId() );
    event.SetItem( (void*)item );
    event.SetEventObject( /*this*/m_owner );
    if ( m_owner->ProcessEvent( event ) && !event.IsAllowed() )
    {
        // cancelled by program
        return;
    }

    item->Collapse();

#if 0  // TODO why should items be collapsed recursively?
    wxArrayTreeListItems& children = item->GetChildren();
    size_t count = children.Count();
    for ( size_t n = 0; n < count; n++ )
    {
        Collapse(children[n]);
    }
#endif

    CalculatePositions();

    RefreshSubtree(item);

    event.SetEventType(wxEVT_COMMAND_TREE_ITEM_COLLAPSED);
    ProcessEvent( event );
}

void wxTreeListMainWindow::CollapseAndReset(const wxTreeItemId& item)
{
    Collapse(item);
    DeleteChildren(item);
}

void wxTreeListMainWindow::Toggle(const wxTreeItemId& itemId)
{
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    if (item->IsExpanded())
        Collapse(itemId);
    else
        Expand(itemId);
}

void wxTreeListMainWindow::Unselect()
{
    if (m_current)
    {
        m_current->SetHilight( FALSE );
        RefreshLine( m_current );
    }
}

void wxTreeListMainWindow::UnselectAllChildren(wxTreeListItem *item)
{
    if (item->IsSelected())
    {
        item->SetHilight(FALSE);
        RefreshLine(item);
    }

    if (item->HasChildren())
    {
        wxArrayTreeListItems& children = item->GetChildren();
        size_t count = children.Count();
        for ( size_t n = 0; n < count; ++n )
        {
            UnselectAllChildren(children[n]);
        }
    }
}

void wxTreeListMainWindow::UnselectAll()
{
    UnselectAllChildren((wxTreeListItem*)GetRootItem().m_pItem);
}

// Recursive function !
// To stop we must have crt_item<last_item
// Algorithm :
// Tag all next children, when no more children,
// Move to parent (not to tag)
// Keep going... if we found last_item, we stop.
bool wxTreeListMainWindow::TagNextChildren(wxTreeListItem *crt_item, wxTreeListItem *last_item, bool select)
{
    wxTreeListItem *parent = crt_item->GetItemParent();

    if (parent == NULL) // This is root item
        return TagAllChildrenUntilLast(crt_item, last_item, select);

    wxArrayTreeListItems& children = parent->GetChildren();
    int index = children.Index(crt_item);
    wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

    size_t count = children.Count();
    for (size_t n=(size_t)(index+1); n<count; ++n)
    {
        if (TagAllChildrenUntilLast(children[n], last_item, select)) return TRUE;
    }

    return TagNextChildren(parent, last_item, select);
}

bool wxTreeListMainWindow::TagAllChildrenUntilLast(wxTreeListItem *crt_item, wxTreeListItem *last_item, bool select)
{
    crt_item->SetHilight(select);
    RefreshLine(crt_item);

    if (crt_item==last_item)
        return TRUE;

    if (crt_item->HasChildren())
    {
        wxArrayTreeListItems& children = crt_item->GetChildren();
        size_t count = children.Count();
        for ( size_t n = 0; n < count; ++n )
        {
            if (TagAllChildrenUntilLast(children[n], last_item, select))
                return TRUE;
        }
    }

  return FALSE;
}

void wxTreeListMainWindow::SelectItemRange(wxTreeListItem *item1, wxTreeListItem *item2)
{
    // item2 is not necessary after item1
    wxTreeListItem *first=NULL, *last=NULL;

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

    bool select = m_current->IsSelected();

    if ( TagAllChildrenUntilLast(first,last,select) )
        return;

    TagNextChildren(first,last,select);
}

void wxTreeListMainWindow::SelectItem(const wxTreeItemId& itemId,
                            bool unselect_others,
                            bool extended_select)
{
    wxCHECK_RET( itemId.IsOk(), wxT("invalid tree item") );

    bool is_single=!(GetWindowStyleFlag() & wxTR_MULTIPLE);
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    //wxCHECK_RET( ( (!unselect_others) && is_single),
    //           wxT("this is a single selection tree") );

    // to keep going anyhow !!!
    if (is_single)
    {
        if (item->IsSelected())
            return; // nothing to do
        unselect_others = TRUE;
        extended_select = FALSE;
    }
    else if ( unselect_others && item->IsSelected() )
    {
        // selection change if there is more than one item currently selected
        wxArrayTreeItemIds selected_items;
        if ( GetSelections(selected_items) == 1 )
            return;
    }

    wxTreeEvent event( wxEVT_COMMAND_TREE_SEL_CHANGING, m_owner->GetId() );
    event.SetItem( (void*)item );
    event.SetOldItem( (void*)m_current );
    event.SetEventObject( /*this*/m_owner );
    // TODO : Here we don't send any selection mode yet !

    if(m_owner->GetEventHandler()->ProcessEvent( event ) && !event.IsAllowed())
        return;

    wxTreeItemId parent = GetItemParent( itemId );
    while (parent.IsOk())
    {
        if (!IsExpanded(parent))
            Expand( parent );

        parent = GetItemParent( parent );
    }

    EnsureVisible( itemId );

    // ctrl press
    if (unselect_others)
    {
        if (is_single) Unselect(); // to speed up thing
        else UnselectAll();
    }

    // shift press
    if (extended_select)
    {
        if ( !m_current )
        {
            m_current = m_key_current = (wxTreeListItem*)GetRootItem().m_pItem;
        }

        // don't change the mark (m_current)
        SelectItemRange(m_current, item);
    }
    else
    {
        bool select=TRUE; // the default

        // Check if we need to toggle hilight (ctrl mode)
        if (!unselect_others)
            select=!item->IsSelected();

        m_current = m_key_current = item;
        m_current->SetHilight(select);
        RefreshLine( m_current );
    }

    event.SetEventType(wxEVT_COMMAND_TREE_SEL_CHANGED);
    GetEventHandler()->ProcessEvent( event );
}

void wxTreeListMainWindow::SelectAll(bool extended_select)
{
    wxCHECK_RET( GetWindowStyleFlag() & wxTR_MULTIPLE, wxT("invalid tree style, must have wxTR_MULTIPLE style to select all items") );

    wxTreeEvent event( wxEVT_COMMAND_TREE_SEL_CHANGING, m_owner->GetId() );
    event.SetItem( GetRootItem() );
    event.SetOldItem( (void*) m_current );
    event.SetEventObject( /*this*/m_owner );
    // TODO : Here we don't send any selection mode yet !

    if(m_owner->GetEventHandler()->ProcessEvent( event ) && !event.IsAllowed())
        return;

    // shift press
    if (!extended_select)
    {

    }
    else
    {

    }
#if !wxCHECK_VERSION(2, 5, 0)
    long cookie = 0;
#else
    wxTreeItemIdValue cookie = 0;
#endif
    wxTreeItemId root = GetRootItem();
    wxTreeListItem *first = (wxTreeListItem *)GetFirstChild (root, cookie).m_pItem;
    wxTreeListItem *last = (wxTreeListItem *)GetLastChild (GetRootItem()).m_pItem;
    if (!first || !last) return;
    if (TagAllChildrenUntilLast (first, last, true)) return;
    TagNextChildren (first, last, true);

    event.SetEventType(wxEVT_COMMAND_TREE_SEL_CHANGED);
    GetEventHandler()->ProcessEvent( event );
}

void wxTreeListMainWindow::FillArray(wxTreeListItem *item,
                           wxArrayTreeItemIds &array) const
{
    if ( item->IsSelected() )
        array.Add(wxTreeItemId(item));

    if ( item->HasChildren() )
    {
        wxArrayTreeListItems& children = item->GetChildren();
        size_t count = children.GetCount();
        for ( size_t n = 0; n < count; ++n )
            FillArray(children[n], array);
    }
}

size_t wxTreeListMainWindow::GetSelections(wxArrayTreeItemIds &array) const
{
    array.Empty();
    wxTreeItemId idRoot = GetRootItem();
    if ( idRoot.IsOk() )
    {
        FillArray((wxTreeListItem*) idRoot.m_pItem, array);
    }
    //else: the tree is empty, so no selections

    return array.Count();
}

void wxTreeListMainWindow::EnsureVisible(const wxTreeItemId& item)
{
    if (!item.IsOk()) return;

    wxTreeListItem *gitem = (wxTreeListItem*) item.m_pItem;

    // first expand all parent branches
    wxTreeListItem *parent = gitem->GetItemParent();
    while ( parent )
    {
        Expand(parent);
        parent = parent->GetItemParent();
    }

    //if (parent) CalculatePositions();

    ScrollTo(item);
}

void wxTreeListMainWindow::ScrollTo(const wxTreeItemId &item)
{
    if (!item.IsOk()) return;

    // We have to call this here because the label in
    // question might just have been added and no screen
    // update taken place.
    if (m_dirty) 
        DoDirtyProcessing();

    wxTreeListItem *gitem = (wxTreeListItem*) item.m_pItem;

    // now scroll to the item
    int item_y = gitem->GetY();

    int start_x = 0;
    int start_y = 0;
    GetViewStart( &start_x, &start_y );
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
        x = m_owner->GetHeaderWindow()->GetWidth(); //m_total_col_width; // ALB
        y += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        //x += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
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
        //x += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        x = m_owner->GetHeaderWindow()->GetWidth(); //m_total_col_width; // ALB
        item_y += PIXELS_PER_UNIT+2;
        int x_pos = GetScrollPos( wxHORIZONTAL );
        // Item should appear at bottom
        SetScrollbars( PIXELS_PER_UNIT, PIXELS_PER_UNIT, x/PIXELS_PER_UNIT, y/PIXELS_PER_UNIT, x_pos, (item_y+GetLineHeight(gitem)-client_h)/PIXELS_PER_UNIT );
    }
}

// FIXME: tree sorting functions are not reentrant and not MT-safe!
static wxTreeListMainWindow *s_treeBeingSorted = NULL;

static int LINKAGEMODE tree_ctrl_compare_func(wxTreeListItem **item1,
                                  wxTreeListItem **item2)
{
    wxCHECK_MSG( s_treeBeingSorted, 0, wxT("bug in wxTreeListMainWindow::SortChildren()") );

    return s_treeBeingSorted->OnCompareItems(*item1, *item2);
}

int wxTreeListMainWindow::OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2)
{
    // ALB: delegate to m_owner, to let the user overrride the comparison
    //return wxStrcmp(GetItemText(item1), GetItemText(item2));
    return m_owner->OnCompareItems(item1, item2);
}

void wxTreeListMainWindow::SortChildren(const wxTreeItemId& itemId)
{
    wxCHECK_RET( itemId.IsOk(), wxT("invalid tree item") );

    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    wxCHECK_RET( !s_treeBeingSorted,
                 wxT("wxTreeListMainWindow::SortChildren is not reentrant") );

    wxArrayTreeListItems& children = item->GetChildren();
    if ( children.Count() > 1 )
    {
        m_dirty = TRUE;

        s_treeBeingSorted = this;
        children.Sort(tree_ctrl_compare_func);
        s_treeBeingSorted = NULL;
    }
    //else: don't make the tree dirty as nothing changed
}

wxTreeItemId wxTreeListMainWindow::FindItem (const wxTreeItemId& item, const wxString& str, int flags) {
#if !wxCHECK_VERSION(2, 5, 0)
    long cookie = 0;
#else
    wxTreeItemIdValue cookie = 0;
#endif
    wxTreeItemId next = item;
    if (!next.IsOk()) next = GetSelection();
    if (!next.IsOk()) {
        if (HasFlag(wxTR_HIDE_ROOT)) {
            next = (wxTreeListItem*)GetFirstChild (GetRootItem().m_pItem, cookie).m_pItem;
        } else {
            next = (wxTreeListItem*)GetRootItem().m_pItem;
        }
    }
    if (!next.IsOk()) return item;

    // start checking the next items
    wxString itemText;
    while (next.IsOk()) {
        itemText = GetItemText (next);
        if (flags & wxTL_SEARCH_LEVEL) {
            next = GetNextSibling (next);
        }else if (flags & wxTL_SEARCH_FULL) {
            wxTreeItemId n = GetFirstChild (next, cookie);
            if (!n.IsOk())
                n = GetNextSibling (next);
            if (!n.IsOk())
                n = GetNextSibling (GetItemParent (next));
            next = n;
        }else{ // wxTL_SEARCH_VISIBLE
            next = GetNextVisible (next);
        }
        if (!next.IsOk()) break; // done
        if (flags & wxTL_SEARCH_PARTIAL) {
            itemText = GetItemText (next).Mid (0, str.Length());
        }else{
            itemText = GetItemText (next);
        }
        if (flags & wxTL_SEARCH_NOCASE) {
            if (itemText.CmpNoCase (str) == 0) return next;
        }else{
            if (itemText.Cmp (str) == 0) return next;
        }
    }
    return item;
}

wxImageList *wxTreeListMainWindow::GetImageList() const
{
    return m_imageListNormal;
}

wxImageList *wxTreeListMainWindow::GetButtonsImageList() const
{
    return m_imageListButtons;
}

wxImageList *wxTreeListMainWindow::GetStateImageList() const
{
    return m_imageListState;
}

void wxTreeListMainWindow::CalculateLineHeight()
{
    wxClientDC dc(this);
    dc.SetFont( m_normalFont );
    m_lineHeight = (int)(dc.GetCharHeight() + m_linespacing);

    if ( m_imageListNormal )
    {
        // Calculate a m_lineHeight value from the normal Image sizes.
        // May be toggle off. Then wxTreeListMainWindow will spread when
        // necessary (which might look ugly).
        int n = m_imageListNormal->GetImageCount();
        for (int i = 0; i < n ; i++)
        {
            int width = 0, height = 0;
            m_imageListNormal->GetSize(i, width, height);
            if (height > m_lineHeight) m_lineHeight = height + m_linespacing;
        }
    }

    if (m_imageListButtons)
    {
        // Calculate a m_lineHeight value from the Button image sizes.
        // May be toggle off. Then wxTreeListMainWindow will spread when
        // necessary (which might look ugly).
        int n = m_imageListButtons->GetImageCount();
        for (int i = 0; i < n ; i++)
        {
            int width = 0, height = 0;
            m_imageListButtons->GetSize(i, width, height);
            if (height > m_lineHeight) m_lineHeight = height + m_linespacing;
        }
    }

/*? FIXME: Don't get what this code is for... Adding a line space is already done!!!
    if (m_lineHeight < 30)
        m_lineHeight += 2;                 // at least 2 pixels
    else
        m_lineHeight += m_lineHeight/10;   // otherwise 10% extra spacing
?*/
}

void wxTreeListMainWindow::SetImageList(wxImageList *imageList)
{
    if (m_ownsImageListNormal) delete m_imageListNormal;
    m_imageListNormal = imageList;
    m_ownsImageListNormal = FALSE;
    m_dirty = TRUE;
    CalculateLineHeight();
}

void wxTreeListMainWindow::SetStateImageList(wxImageList *imageList)
{
    if (m_ownsImageListState) delete m_imageListState;
    m_imageListState = imageList;
    m_ownsImageListState = FALSE;
}

void wxTreeListMainWindow::SetButtonsImageList(wxImageList *imageList)
{
    if (m_ownsImageListButtons) delete m_imageListButtons;
    m_imageListButtons = imageList;
    m_ownsImageListButtons = FALSE;
    m_dirty = TRUE;
    CalculateLineHeight();
}

void wxTreeListMainWindow::AssignImageList(wxImageList *imageList)
{
    SetImageList(imageList);
    m_ownsImageListNormal = TRUE;
}

void wxTreeListMainWindow::AssignStateImageList(wxImageList *imageList)
{
    SetStateImageList(imageList);
    m_ownsImageListState = TRUE;
}

void wxTreeListMainWindow::AssignButtonsImageList(wxImageList *imageList)
{
    SetButtonsImageList(imageList);
    m_ownsImageListButtons = TRUE;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

void wxTreeListMainWindow::AdjustMyScrollbars()
{
    if (m_anchor)
    {
        int x = 0, y = 0;
        m_anchor->GetSize( x, y, this );
        y += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        //x += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        int x_pos = GetScrollPos( wxHORIZONTAL );
        int y_pos = GetScrollPos( wxVERTICAL );
        x = m_owner->GetHeaderWindow()->GetWidth() + 2;
        if(x < GetClientSize().GetWidth()) x_pos = 0;
        //m_total_col_width + 2; // ALB
        SetScrollbars( PIXELS_PER_UNIT, PIXELS_PER_UNIT, x/PIXELS_PER_UNIT,
                       y/PIXELS_PER_UNIT, x_pos, y_pos );
    }
    else
    {
        SetScrollbars( 0, 0, 0, 0 );
    }
}

int wxTreeListMainWindow::GetLineHeight(wxTreeListItem *item) const
{
    if (GetWindowStyleFlag() & wxTR_HAS_VARIABLE_ROW_HEIGHT)
        return item->GetHeight();
    else
        return m_lineHeight;
}

void wxTreeListMainWindow::PaintItem(wxTreeListItem *item, wxDC& dc)
{
    wxTreeItemAttr *attr = item->GetAttributes();
    if (attr && attr->HasFont()) {
        dc.SetFont(attr->GetFont());
    }else if (item->IsBold()) {
        dc.SetFont(m_boldFont);
    }
    wxColour colText;
    if (attr && attr->HasTextColour()) {
        colText = attr->GetTextColour();
    }else{
        colText = GetForegroundColour();
    }

    dc.SetPen(*wxTRANSPARENT_PEN);

    long text_w = 0, text_h = 0;

    dc.GetTextExtent( item->GetText(GetMainColumn()), &text_w, &text_h );

    int total_h = GetLineHeight(item);

    if (item->IsSelected() && HasFlag(wxTR_FULL_ROW_HIGHLIGHT)) {
            dc.SetBrush(*(m_hasFocus ? m_hilightBrush : m_hilightUnfocusedBrush));
            dc.SetPen(*wxBLACK_PEN);
            colText = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    } else {
        wxColour colBg;
        if (attr && attr->HasBackgroundColour()) {
            colBg = attr->GetBackgroundColour();
        } else {
            colBg = GetBackgroundColour();
        }
        dc.SetBrush(wxBrush(colBg, wxSOLID));
    }

    int offset = HasFlag(wxTR_ROW_LINES) ? 1 : 0;
    dc.DrawRectangle(0, item->GetY() + offset,
                     m_owner->GetHeaderWindow()->GetWidth(), total_h-offset);

    dc.SetBackgroundMode(wxTRANSPARENT);
    int text_extraH = (total_h > text_h) ? (total_h - text_h)/2 : 0;
    int img_extraH = (total_h > m_imgHeight)? (total_h-m_imgHeight)/2: 0;
    int x_colstart = 0;
    for ( size_t i = 0; i < GetColumnCount(); ++i ) {
        if (!m_owner->GetHeaderWindow()->GetColumnShown(i)) continue;
        int colwidth = m_owner->GetHeaderWindow()->GetColumnWidth(i);
        int image;
        int image_x = 0;
        int image_w = 0;
        if (i == GetMainColumn()) {
            image = item->GetCurrentImage();
            if (item->HasPlus()) {
                 image_x = item->GetX() + (m_btnWidth-m_btnWidth2) + LINEATROOT;
            }else{
                 image_x = item->GetX() - m_imgWidth2;
            }
        }
        else
        {
            image = item->GetImage(i);
            image_x = x_colstart + MARGIN;
        }
        if (image != NO_IMAGE) image_w = m_imgWidth + MARGIN;

        // honor text alignment
        wxString text = item->GetText(i);
        switch ( m_owner->GetHeaderWindow()->GetColumn(i).GetAlignment() ) {
        case wxTL_ALIGN_LEFT:
            // already left aligned
            break;
        case wxTL_ALIGN_RIGHT:
            dc.GetTextExtent(text, &text_w, NULL);
            image_x = x_colstart + colwidth - (image_w + text_w + MARGIN);
            break;
        case wxTL_ALIGN_CENTER:
            dc.GetTextExtent(text, &text_w, NULL);
            int w = colwidth - image_w - text_w;
            image_x = x_colstart + (w > 0)? w: 0;
            break;
        }
        int text_x = image_x + image_w;

        if (item->IsSelected() && (i==GetMainColumn()) && !HasFlag(wxTR_FULL_ROW_HIGHLIGHT))
        {
            dc.SetPen(*wxBLACK_PEN);
            dc.SetBrush(*(m_hasFocus ? m_hilightBrush : m_hilightUnfocusedBrush));
            int offset = HasFlag (wxTR_ROW_LINES) ? 1 : 0;
            int width = wxMin(text_w+2, colwidth - text_x - x_colstart);
            dc.DrawRectangle(text_x-1, item->GetY() + offset, width, total_h-offset);
            dc.SetBackgroundMode(wxTRANSPARENT);
            dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        }else{
            dc.SetTextForeground(colText);
        }

        wxDCClipper clipper (dc, x_colstart, item->GetY(), colwidth, total_h);
        if (image != NO_IMAGE)
        {
            int image_y = item->GetY() + img_extraH;
            m_imageListNormal->Draw ( image, dc, image_x, image_y,
                                      wxIMAGELIST_DRAW_TRANSPARENT );
        }
        int text_y = item->GetY() + text_extraH;
        dc.DrawText ( text, (wxCoord)text_x, (wxCoord)text_y );

        x_colstart += colwidth;
    }

    // restore normal font
    dc.SetFont( m_normalFont );
}

// Now y stands for the top of the item, whereas it used to stand for middle !
void wxTreeListMainWindow::PaintLevel (wxTreeListItem *item, wxDC &dc,
                                       int level, int &y, int x_colstart )
{
    // Handle hide root (only level 0)
    if (HasFlag(wxTR_HIDE_ROOT) && (level == 0)) {
        // always expand hidden root
        wxArrayTreeListItems& children = item->GetChildren();
        int n;
        for (n = 0; n < (int)children.Count(); n++) {
            PaintLevel (children[n], dc, 1, y, x_colstart);
        }
        // end after expanding root
        return;
    }

    // calculate position of vertical lines
    int x = x_colstart + MARGIN; // start of column
    if (HasFlag (wxTR_LINES_AT_ROOT)) x += LINEATROOT; // space for lines at root
    if (HasButtons()) {
        x += m_btnWidth2; // middle of button
    }else{
        if (m_imgWidth > 0) x += m_imgWidth2; // middle of image
    }
    if (!HasFlag (wxTR_HIDE_ROOT)) {
        x += m_indent * level; // indent according to level
    }else{
        if (level > 0) x += m_indent * (level-1); // but not level 1
    }

    // handle column text
    item->SetX (x);
    item->SetY (y);

    int h = GetLineHeight(item);
    int y_top = y;
    int y_mid = y_top + (h/2);
    y += h;

    int exposed_x = dc.LogicalToDeviceX(0);
    int exposed_y = dc.LogicalToDeviceY(y_top);

    if (IsExposed(exposed_x, exposed_y, 10000, h))  // 10000 = very much
    {
        // draw item
        PaintItem(item, dc);

        if (HasFlag(wxTR_ROW_LINES))
        {
            //dc.DestroyClippingRegion();
            int total_width = m_owner->GetHeaderWindow()->GetWidth();
            // if the background colour is white, choose a
            // contrasting color for the lines
            dc.SetPen(((GetBackgroundColour() == *wxWHITE) ?
                       wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT) : *wxWHITE_PEN));
            dc.DrawLine(0, y_top, total_width, y_top);
            dc.DrawLine(0, y, total_width, y);
        }

        // restore DC objects
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(m_dottedPen);

        if (((level == 0) || ((level == 1) && HasFlag(wxTR_HIDE_ROOT))) &&
            HasFlag(wxTR_LINES_AT_ROOT) && !HasFlag(wxTR_NO_LINES)) {
            int rootPos = x_colstart + MARGIN;
            dc.DrawLine (rootPos, y_mid, rootPos+LINEATROOT, y_mid);
        }

        size_t clip_width = m_owner->GetHeaderWindow()->
                            GetColumn(m_main_column).GetWidth();

        if (item->HasPlus() && HasButtons())  // should the item show a button?
        {
            // clip to the column width
            wxDCClipper clipper(dc, x_colstart, y_top, clip_width, 10000);

            if ( !HasFlag(wxTR_NO_LINES) )
            {
                // draw the horizontal line here
                int x_start = x;
                if (x > (signed)m_indent)
                    x_start -= m_indent;
                else if (HasFlag(wxTR_LINES_AT_ROOT))
                    x_start = 3;
                dc.DrawLine(x_start, y_mid, x /*+ m_spacing*/, y_mid);
            }
            
            if (m_imageListButtons != NULL)
            {
                // draw the image button here
                int image = wxTreeItemIcon_Normal;
                if (item->IsExpanded()) image = wxTreeItemIcon_Expanded;
                if (item->IsSelected())
                    image += wxTreeItemIcon_Selected - wxTreeItemIcon_Normal;
                int xx = x + m_btnWidth2;
                int yy = y_mid - m_btnHeight2;
                dc.SetClippingRegion(xx, yy, m_btnWidth, m_btnHeight);
                m_imageListButtons->Draw(image, dc, xx, yy,
                                         wxIMAGELIST_DRAW_TRANSPARENT);
                dc.DestroyClippingRegion();
            }
            else // no custom buttons
            {
                static const int wImage = 9;
                static const int hImage = 9;

                int flag = 0;
                if (item->IsExpanded())
                    flag |= wxCONTROL_EXPANDED;
                if (item == m_underMouse)
                    flag |= wxCONTROL_CURRENT;

                wxRendererNative::Get().DrawTreeItemButton(
                    this, dc,
                    wxRect(x - wImage/2, y_mid - hImage/2, wImage, hImage),
                    flag);
            }

            if (!HasFlag(wxTR_NO_LINES)) {
                if (/*!(level == 0) &&*/ !((level == 1) && HasFlag(wxTR_HIDE_ROOT))) {
                    if (m_imgWidth > 0) {
                        dc.DrawLine(x+m_btnWidth2, y_mid, x+m_indent-m_imgWidth2, y_mid);
                    }else{
                        dc.DrawLine(x+m_btnWidth2, y_mid, x+m_btnWidth2+LINEATROOT-MARGIN, y_mid);
                    }
                }
            }
        }
        else if (!HasFlag(wxTR_NO_LINES))  // no button; maybe a line?
        {
            // clip to the column width
            wxDCClipper clipper(dc, x_colstart, y_top, clip_width, 10000);

            // draw the horizontal line here
            if (/*!(level == 0) &&*/ !((level == 1) && HasFlag(wxTR_HIDE_ROOT))) {
                int x2 = x - m_indent;
                if (m_imgWidth > 0) {
                    dc.DrawLine(x2, y_mid, x2+m_indent-m_imgWidth2, y_mid);
                }else{
                    dc.DrawLine(x2, y_mid, x2+m_btnWidth2+LINEATROOT+MARGIN, y_mid);
                }
            }
        }
    }

    // restore DC objects
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(m_dottedPen);
    dc.SetTextForeground(*wxBLACK);

    if (item->IsExpanded())
    {
        wxArrayTreeListItems& children = item->GetChildren();
        int count = children.Count();
        int n, oldY = 0;

        // paint sublevel items first
        for (n=0; n<count; ++n) {
            oldY = y;
            PaintLevel(children[n], dc, level+1, y, x_colstart);
        }

        // then draw the connecting lines
        if (!HasFlag(wxTR_NO_LINES) && count > 0)
        {
            // clip to the column width
            size_t clip_width = m_owner->GetHeaderWindow()->GetColumn(m_main_column).GetWidth();
            wxDCClipper clipper(dc, x_colstart, y_top, clip_width, 10000);

            // draw line down to last child
            oldY += GetLineHeight(children[n-1]) >> 1;
            if (HasButtons()) y_mid += 5;
            dc.DrawLine(x, y_mid, x, oldY);
        }
    }
}

void wxTreeListMainWindow::DrawDropEffect(wxTreeListItem *item)
{
    if ( item )
    {
        if ( item->HasPlus() )
        {
            // it's a folder, indicate it by a border
            DrawBorder(item);
        }
        else
        {
            // draw a line under the drop target because the item will be
            // dropped there
            DrawLine(item, TRUE /* below */);
        }

        SetCursor(wxCURSOR_BULLSEYE);
    }
    else
    {
        // can't drop here
        SetCursor(wxCURSOR_NO_ENTRY);
    }
}

void wxTreeListMainWindow::DrawBorder(const wxTreeItemId &item)
{
    wxCHECK_RET( item.IsOk(), _T("invalid item in wxTreeListMainWindow::DrawLine") );

    wxTreeListItem *i = (wxTreeListItem*) item.m_pItem;

    wxClientDC dc(this);
    PrepareDC( dc );
    dc.SetLogicalFunction(wxINVERT);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    int w = i->GetWidth() + 2;
    int h = GetLineHeight(i) + 2;

    dc.DrawRectangle( i->GetX() - 1, i->GetY() - 1, w, h);
}

void wxTreeListMainWindow::DrawLine(const wxTreeItemId &item, bool below)
{
    wxCHECK_RET( item.IsOk(), _T("invalid item in wxTreeListMainWindow::DrawLine") );

    wxTreeListItem *i = (wxTreeListItem*) item.m_pItem;

    wxClientDC dc(this);
    PrepareDC( dc );
    dc.SetLogicalFunction(wxINVERT);

    int x = i->GetX(),
        y = i->GetY();
    if ( below )
    {
        y += GetLineHeight(i) - 1;
    }

    dc.DrawLine( x, y, x + i->GetWidth(), y);
}

// ----------------------------------------------------------------------------
// wxWindows callbacks
// ----------------------------------------------------------------------------

void wxTreeListMainWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc(this);

    PrepareDC( dc );

    if(!GetColumnCount()) return; // ALB

    if ( !m_anchor)
        return;

    // calculate button size
    m_btnWidth = 0, m_btnWidth2 = 0;
    m_btnHeight = 0, m_btnHeight2 = 0;
    if (m_imageListButtons) {
        m_imageListButtons->GetSize (0, m_btnWidth, m_btnHeight);
    }else if (HasButtons()) {
        m_btnWidth = BTNWIDTH;
        m_btnHeight = BTNHEIGHT;
    }
    m_btnWidth2 = m_btnWidth/2;
    m_btnHeight2 = m_btnHeight/2;

    // calculate image size
    m_imgWidth = 0, m_imgWidth2 = 0;
    m_imgHeight = 0, m_imgHeight2 = 0;
    if (m_imageListNormal) {
        m_imageListNormal->GetSize (0, m_imgWidth, m_imgHeight);
        m_imgWidth += 4; //? ToDo: Why + 4?
    }
    m_imgWidth2 = m_imgWidth/2;
    m_imgHeight2 = m_imgHeight/2;

    // calculate indent size
    int btnIndent = HasButtons()? m_btnWidth + LINEATROOT: 0;
    m_indent = wxMax (MININDENT, wxMax (m_imgWidth, btnIndent)) + MARGIN;

    // set default values
    dc.SetFont( m_normalFont );
    dc.SetPen( m_dottedPen );

    // this is now done dynamically
    //if(GetImageList() == NULL)
    // m_lineHeight = (int)(dc.GetCharHeight() + 4);

    // calculate column start and paint
    int x_colstart = 0;
    int i = 0;
    for (i = 0; i < (int)GetMainColumn(); ++i) {
        if (!m_owner->GetHeaderWindow()->GetColumnShown(i)) continue;
        x_colstart += m_owner->GetHeaderWindow()->GetColumnWidth (i);
    }
    int y = 0;
    PaintLevel ( m_anchor, dc, 0, y, x_colstart );
}

void wxTreeListMainWindow::OnSetFocus( wxFocusEvent &event )
{
    m_hasFocus = TRUE;

    RefreshSelected();

    event.Skip();
}

void wxTreeListMainWindow::OnKillFocus( wxFocusEvent &event )
{
    m_hasFocus = FALSE;

    RefreshSelected();

    event.Skip();
}

void wxTreeListMainWindow::OnChar( wxKeyEvent &event )
{
    wxTreeEvent te( wxEVT_COMMAND_TREE_KEY_DOWN, m_owner->GetId() );
    te.SetKeyEvent( event );
    te.SetEventObject( /*this*/m_owner );
    if ( m_owner->GetEventHandler()->ProcessEvent( te ) )
    {
        // intercepted by the user code
        return;
    }

    if ( !m_current )
    {
        if (HasFlag(wxTR_HIDE_ROOT)) {
#if !wxCHECK_VERSION(2, 5, 0)
            long cookie = 0;
#else
            wxTreeItemIdValue cookie = 0;
#endif
            m_current = m_key_current = (wxTreeListItem*)GetFirstChild (GetRootItem().m_pItem, cookie).m_pItem;
        }
        else
        {
            m_current = m_key_current = (wxTreeListItem*)GetRootItem().m_pItem;
        }
    }

    // how should the selection work for this event?
    bool is_multiple, extended_select, unselect_others;
    EventFlagsToSelType(GetWindowStyleFlag(),
                        event.ShiftDown(),
                        event.ControlDown(),
                        is_multiple, extended_select, unselect_others);

    // + : Expand (not on Win32)
    // - : Collaspe (not on Win32)
    // * : Expand all/Collapse all
    // ' ' | return : activate
    // up    : go up (not last children!)
    // down  : go down
    // left  : go to parent (or collapse on Win32)
    // right : open if parent and go next (or expand on Win32)
    // home  : go to root
    // end   : go to last item without opening parents
    switch (event.GetKeyCode())
    {
#ifndef __WXMSW__ // mimic the standard win32 tree ctrl
        case '+':
        case WXK_ADD:
            if (m_current->HasPlus() && !IsExpanded(m_current))
            {
                Expand (m_current);
            }
            break;
#endif // __WXMSW__

        case '*':
        case WXK_MULTIPLY:
            if ( !IsExpanded(m_current) )
            {
                // expand all
                ExpandAll (m_current);
                break;
            }
            //else: fall through to Collapse() it

#ifndef __WXMSW__ // mimic the standard wxTreeCtrl behaviour
        case '-':
        case WXK_SUBTRACT:
            if (IsExpanded(m_current))
            {
                Collapse (m_current);
            }
            break;
#endif // __WXMSW__

        case ' ':
        case WXK_RETURN:
            {
                wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
                                   m_owner->GetId() );
                event.SetItem( (void*) m_current);
                event.SetEventObject( /*this*/m_owner );
                m_owner->GetEventHandler()->ProcessEvent( event );
            }
            break;

        // backspace goes to the parent, sends "root" activation
        case WXK_BACK:
            {
                wxTreeItemId prev = GetItemParent( m_current );
                if ((prev == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT))
                {
                    // don't go to root if it is hidden
                    prev = GetPrevSibling( m_current );
                }
                if (prev)
                {
                    SelectItem( prev, unselect_others, extended_select );
                    EnsureVisible( prev );
                }
            }
            break;

        // up goes to the previous sibling or to the last
        // of its children if it's expanded
        case WXK_UP:
            {
                wxTreeItemId prev = GetPrevSibling( m_key_current );
                if (!prev)
                {
                    prev = GetItemParent( m_key_current );
                    if ((prev == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT))
                    {
                        break;  // don't go to root if it is hidden
                    }
                    if (prev)
                    {
#if !wxCHECK_VERSION(2, 5, 0)
                        long cookie = 0;
#else
                        wxTreeItemIdValue cookie = 0;
#endif
                        wxTreeItemId current = m_key_current;
                        // TODO: Huh?  If we get here, we'd better be the first child of our parent.  How else could it be?
                        if (current == GetFirstChild( prev, cookie ))
                        {
                            // otherwise we return to where we came from
                            SelectItem( prev, unselect_others, extended_select );
                            m_key_current= (wxTreeListItem*) prev.m_pItem;
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
                        if ( !child )
                        {
                            break;
                        }
                        prev = child;
                    }

                    SelectItem( prev, unselect_others, extended_select );
                    m_key_current=(wxTreeListItem*) prev.m_pItem;
                    EnsureVisible( prev );
                }
            }
            break;

        // left arrow goes to the parent
        case WXK_LEFT:
            if (IsExpanded(m_current))
            {
                Collapse(m_current);
            }
            else
            {
                wxTreeItemId prev = GetItemParent( m_current );
                if ((prev == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT))
                {
                    // don't go to root if it is hidden
                    prev = GetPrevSibling( m_current );
                }
                if (prev)
                {
                    SelectItem( prev, unselect_others, extended_select );
                    EnsureVisible( prev );
                }
            }
            break;

        case WXK_RIGHT:
#if defined(__WXMSW__) // mimic the standard win32 tree ctrl
            if (m_current->HasPlus() && !IsExpanded(m_current))
            {
                Expand(m_current);
                break;
            }
#endif // __WXMSW__

            // this works the same as the down arrow except that we
            // also expand the item if it wasn't expanded yet
            Expand(m_current);
            // fall through

        case WXK_DOWN:
            {
                if (IsExpanded(m_key_current) && HasChildren(m_key_current))
                {
#if !wxCHECK_VERSION(2, 5, 0)
                    long cookie = 0;
#else
                    wxTreeItemIdValue cookie = 0;
#endif
                    wxTreeItemId child = GetFirstChild( m_key_current, cookie );
                    if (child) {
                        SelectItem( child, unselect_others, extended_select );
                        m_key_current=(wxTreeListItem*) child.m_pItem;
                        EnsureVisible( child );
                        break;
                    }
                }
                wxTreeItemId next = GetNextSibling( m_key_current );
                if (!next)
                {
                    wxTreeItemId current = m_key_current;
                    while (current && !next)
                    {
                        current = GetItemParent( current );
                        if (current) next = GetNextSibling( current );
                    }
                }
                if (next)
                {
                    SelectItem( next, unselect_others, extended_select );
                    m_key_current=(wxTreeListItem*) next.m_pItem;
                    EnsureVisible( next );
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
                    SelectItem( last, unselect_others, extended_select );
                    EnsureVisible( last );
                }
            }
            break;

        // <Home> selects the root item
        case WXK_HOME:
            {
                wxTreeItemId prev = GetRootItem();
                if (!prev) break;
                if (HasFlag(wxTR_HIDE_ROOT))
                {
#if !wxCHECK_VERSION(2, 5, 0)
                    long cookie = 0;
#else
                    wxTreeItemIdValue cookie = 0;
#endif
                    prev = GetFirstChild(prev, cookie);
                    if (!prev) break;
                }
                SelectItem( prev, unselect_others, extended_select );
                EnsureVisible( prev );
            }
            break;

        default:
            if (event.m_keyCode >= (int)' ') {
                if (!m_findTimer->IsRunning()) m_findStr.Clear();
                m_findStr.Append (event.m_keyCode);
                m_findTimer->Start (500, wxTIMER_ONE_SHOT);
                wxTreeItemId dummy = (wxTreeItemId*)NULL;
                wxTreeItemId item = FindItem (dummy, m_findStr, wxTL_SEARCH_VISIBLE |
                                                                wxTL_SEARCH_PARTIAL |
                                                                wxTL_SEARCH_NOCASE);
                if (item.IsOk()) {
                    EnsureVisible (item);
                    SelectItem (item);
                }
            }
            event.Skip();
    }
}

wxTreeItemId wxTreeListMainWindow::HitTest(const wxPoint& point, int& flags,
                                           int& column)
{
    // JACS: removed wxYieldIfNeeded() because it can cause the window
    // to be deleted from under us if a close window event is pending

    int w, h;
    GetSize(&w, &h);
    flags=0;
    column = -1;
    if (point.x<0) flags |= wxTREE_HITTEST_TOLEFT;
    if (point.x>w) flags |= wxTREE_HITTEST_TORIGHT;
    if (point.y<0) flags |= wxTREE_HITTEST_ABOVE;
    if (point.y>h) flags |= wxTREE_HITTEST_BELOW;
    if (flags) return wxTreeItemId();

    if (m_anchor == NULL)
    {
        flags = wxTREE_HITTEST_NOWHERE;
        return wxTreeItemId();
    }

    wxTreeListItem *hit = m_anchor->HitTest(CalcUnscrolledPosition(point),
                                            this, flags, column, 0);
    if (hit == NULL)
    {
        flags = wxTREE_HITTEST_NOWHERE;
        return wxTreeItemId();
    }
    return hit;
}

// get the bounding rectangle of the item (or of its label only)
bool wxTreeListMainWindow::GetBoundingRect(const wxTreeItemId& item,
                         wxRect& rect,
                         bool WXUNUSED(textOnly)) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, _T("invalid item in wxTreeListMainWindow::GetBoundingRect") );

    wxTreeListItem *i = (wxTreeListItem*) item.m_pItem;

    int startX, startY;
    GetViewStart(& startX, & startY);

    rect.x = i->GetX() - startX*PIXELS_PER_UNIT;
    rect.y = i->GetY() - startY*PIXELS_PER_UNIT;
    rect.width = i->GetWidth();
    //rect.height = i->GetHeight();
    rect.height = GetLineHeight(i);

    return TRUE;
}

/* **** */

void wxTreeListMainWindow::Edit( const wxTreeItemId& item )
{
    if (!item.IsOk()) return;

    m_currentEdit = (wxTreeListItem*) item.m_pItem;

    wxTreeEvent te( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, m_owner->GetId() );
    te.SetItem( (void*) m_currentEdit);
    te.SetEventObject( /*this*/m_owner );
    m_owner->GetEventHandler()->ProcessEvent( te );

    if (!te.IsAllowed()) return;

    // We have to call this here because the label in
    // question might just have been added and no screen
    // update taken place.
    if (m_dirty) 
        DoDirtyProcessing();

    wxString s = m_currentEdit->GetText(/*ALB*/m_main_column);
    int x = m_currentEdit->GetX() + m_imgWidth2;
    int y = m_currentEdit->GetY();
    int w = wxMin (m_currentEdit->GetWidth(),
                   m_owner->GetHeaderWindow()->GetWidth()) - m_imgWidth2;
    int h = m_currentEdit->GetHeight() + 2;
    wxClientDC dc(this);
    PrepareDC( dc );
    x = dc.LogicalToDeviceX( x );
    y = dc.LogicalToDeviceY( y );

    wxTreeListTextCtrl *text = new wxTreeListTextCtrl(this, -1,
                                              &m_renameAccept,
                                              &m_renameRes,
                                              this,
                                              s,
                                              wxPoint (x,y),
                                              wxSize (w,h));
    text->SetFocus();
}

void wxTreeListMainWindow::OnRenameTimer()
{
    Edit( m_current );
}

void wxTreeListMainWindow::OnRenameAccept()
{
    // TODO if the validator fails this causes a crash
    wxTreeEvent le( wxEVT_COMMAND_TREE_END_LABEL_EDIT, m_owner->GetId() );
    le.SetItem( (void*)m_currentEdit );
    le.SetEventObject( /*this*/m_owner );
    le.SetLabel( m_renameRes );
    m_owner->GetEventHandler()->ProcessEvent( le );

    if (!le.IsAllowed()) return;

    SetItemText( m_currentEdit, m_renameRes );
}

void wxTreeListMainWindow::OnMouse( wxMouseEvent &event )
{
    if ( !m_anchor ) return;

    wxPoint pt = CalcUnscrolledPosition(event.GetPosition());

    // Is the mouse over a tree item button?
    int flags = 0;
    wxTreeListItem *item = m_anchor->HitTest(pt, this, flags, 0);
    wxTreeListItem *underMouse = item;
#if wxUSE_TOOLTIPS
    bool underMouseChanged = (underMouse != m_underMouse) ;
#endif // wxUSE_TOOLTIPS

    if (underMouse && (flags & wxTREE_HITTEST_ONITEMBUTTON) &&
        !event.LeftIsDown() && !m_isDragging &&
        (!m_renameTimer || !m_renameTimer->IsRunning()))
    {
    }
    else
    {
        underMouse = NULL;
    }

    if (underMouse != m_underMouse)
    {
         if (m_underMouse)
         {
            // unhighlight old item
            wxTreeListItem *tmp = m_underMouse;
            m_underMouse = NULL;
            RefreshLine( tmp );
         }

         m_underMouse = underMouse;
         if (m_underMouse)
            RefreshLine( m_underMouse );
    }

#if wxUSE_TOOLTIPS
    // Determines what item we are hovering over and need a tooltip for
    wxTreeItemId hoverItem = item;

    // We do not want a tooltip if we are dragging, or if the rename timer is running
    if (underMouseChanged && hoverItem.IsOk() && !m_isDragging && (!m_renameTimer || !m_renameTimer->IsRunning()))
    {
        // Ask the tree control what tooltip (if any) should be shown
        wxTreeEvent hevent(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, GetId());
        hevent.SetItem(hoverItem);
        hevent.SetEventObject(this);

        if ( GetEventHandler()->ProcessEvent(hevent) && hevent.IsAllowed() )
        {
            SetToolTip(hevent.GetLabel());
        }
    }
#endif

    // we process left mouse up event (enables in-place edit), right down
    // (pass to the user code), left dbl click (activate item) and
    // dragging/moving events for items drag-and-drop
    if ( !(event.LeftDown() ||
           event.LeftUp() ||
           event.RightDown() ||
           event.LeftDClick() ||
           event.Dragging() ||
           ((event.Moving() || event.RightUp()) && m_isDragging)) )
    {
        event.Skip();
        return;
    }

    if ( event.LeftDown() )
        SetFocus();

//     wxClientDC dc(this);
//     PrepareDC(dc);
//     wxCoord x = dc.DeviceToLogicalX( event.GetX() );
//     wxCoord y = dc.DeviceToLogicalY( event.GetY() );
    wxCoord &x = pt.x;
    wxCoord &y = pt.y;

    if ( event.Dragging() && !m_isDragging )
    {
        if (m_dragCount == 0)
            m_dragStart = wxPoint(x,y);

        m_dragCount++;

        if (m_dragCount != 3)
        {
            // wait until user drags a bit further...
            return;
        }

        wxEventType command = event.RightIsDown()
                              ? wxEVT_COMMAND_TREE_BEGIN_RDRAG
                              : wxEVT_COMMAND_TREE_BEGIN_DRAG;

        wxTreeEvent nevent( command,/*ALB*/ m_owner->GetId() );
        nevent.SetItem( (void*)m_current);
        nevent.SetEventObject(/*this*/m_owner); // ALB
        nevent.SetPoint(pt);

        // by default the dragging is not supported, the user code must
        // explicitly allow the event for it to take place
        nevent.Veto();

        if ( m_owner->GetEventHandler()->ProcessEvent(nevent) &&
             nevent.IsAllowed() )
        {
            // we're going to drag this item
            m_isDragging = TRUE;

            // remember the old cursor because we will change it while
            // dragging
            m_oldCursor = m_cursor;

            // in a single selection control, hide the selection temporarily
            if ( !(GetWindowStyleFlag() & wxTR_MULTIPLE) )
            {
                m_oldSelection = (wxTreeListItem*) GetSelection().m_pItem;

                if ( m_oldSelection )
                {
                    m_oldSelection->SetHilight(FALSE);
                    RefreshLine(m_oldSelection);
                }
            }

            CaptureMouse();
        }
    }
    else if ( event.Moving() )
    {
        if ( item != m_dropTarget )
        {
            // unhighlight the previous drop target
            DrawDropEffect(m_dropTarget);

            m_dropTarget = item;

            // highlight the current drop target if any
            DrawDropEffect(m_dropTarget);

            DoDirtyProcessing();
        }
    }
    else if ( (event.LeftUp() || event.RightUp()) && m_isDragging )
    {
        // erase the highlighting
        DrawDropEffect(m_dropTarget);

        if ( m_oldSelection )
        {
            m_oldSelection->SetHilight(TRUE);
            RefreshLine(m_oldSelection);
            m_oldSelection = (wxTreeListItem *)NULL;
        }

        // generate the drag end event
        wxTreeEvent event(wxEVT_COMMAND_TREE_END_DRAG,/*ALB*/m_owner->GetId());

        event.SetItem( (void*)item );
        event.SetPoint( wxPoint(x, y) );
        event.SetEventObject(/*this*/m_owner);

        (void)m_owner->GetEventHandler()->ProcessEvent(event);

        m_isDragging = FALSE;
        m_dropTarget = (wxTreeListItem *)NULL;

        ReleaseMouse();

        SetCursor(m_oldCursor);

        DoDirtyProcessing();
    }
    else
    {
        // here we process only the messages which happen on tree items

        m_dragCount = 0;

        if ( item == NULL ) return;  /* we hit the blank area */

        if ( event.RightDown() )
        {
            SetFocus();
            wxTreeEvent nevent(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
                               m_owner->GetId());
            nevent.SetItem( (void*)item );
            int nx, ny;
            CalcScrolledPosition(x, y, &nx, &ny);
            nevent.SetPoint( wxPoint(nx, ny));
            nevent.SetEventObject(/*this*/m_owner);
            m_owner->GetEventHandler()->ProcessEvent(nevent);
        }
        else if ( event.LeftUp() )
        {
            if ( m_lastOnSame )
            {
                if ( ( item == m_current ) &&
                     ( flags & wxTREE_HITTEST_ONITEMLABEL ) &&
                     HasFlag(wxTR_EDIT_LABELS ) )
                {
                    if ( m_renameTimer->IsRunning() )
                        m_renameTimer->Stop();

                    m_renameTimer->Start( 100, TRUE );
                }

                m_lastOnSame = FALSE;
            }
        }
        else // !RightDown() && !LeftUp() ==> LeftDown() || LeftDClick()
        {
            if ( event.LeftDown() )
            {
                SetFocus();
                m_lastOnSame = item == m_current;
            }

            if ((flags & wxTREE_HITTEST_ONITEMBUTTON) ||
                ((flags & wxTREE_HITTEST_ONITEMICON)) &&
                 !HasButtons() && item->HasPlus())
            {
                // only toggle the item for a single click, double click on
                // the button doesn't do anything (it toggles the item twice)
                if ( event.LeftDown() )
                {
                    Toggle( item );
                }

                // don't select the item if the button was clicked
                return;
            }

            // how should the selection work for this event?
            bool is_multiple, extended_select, unselect_others;
            EventFlagsToSelType(GetWindowStyleFlag(),
                                event.ShiftDown(),
                                event.ControlDown(),
                                is_multiple, extended_select, unselect_others);

            SelectItem (item, unselect_others, extended_select);

            // For some reason, Windows isn't recognizing a left double-click,
            // so we need to simulate it here.  Allow 200 milliseconds for now.
            if ( event.LeftDClick() )
            {
                // double clicking should not start editing the item label
                m_renameTimer->Stop();
                m_lastOnSame = FALSE;

                // send activate event first
                wxTreeEvent nevent( wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
                                    m_owner->GetId() );
                nevent.SetItem( (void*)item );
                int nx, ny;
                CalcScrolledPosition(x, y, &nx, &ny);
                nevent.SetPoint( wxPoint(nx, ny) );
                nevent.SetEventObject( /*this*/m_owner );
                if ( !m_owner->GetEventHandler()->ProcessEvent( nevent ) )
                {
                    // if the user code didn't process the activate event,
                    // handle it ourselves by toggling the item when it is
                    // double clicked
                    if ( item->HasPlus() )
                    {
                        Toggle(item);
                    }
                }
            }
        }
    }
}

void wxTreeListMainWindow::OnIdle( wxIdleEvent &WXUNUSED(event) )
{
    DoDirtyProcessing();
}

void wxTreeListMainWindow::DoDirtyProcessing()
{
    /* after all changes have been done to the tree control,
     * we actually redraw the tree when everything is over */

    if (!m_dirty) return;

    m_dirty = FALSE;

    CalculatePositions();
    Refresh();
    AdjustMyScrollbars();
}

void wxTreeListMainWindow::OnScroll(wxScrollWinEvent& event)
{
    // FIXME
#if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
    wxScrolledWindow::OnScroll(event);
#else
    HandleOnScroll( event );
#endif

    if(event.GetOrientation() == wxHORIZONTAL)
    {
        m_owner->GetHeaderWindow()->Refresh();
        m_owner->GetHeaderWindow()->Update();
    }
}


void wxTreeListMainWindow::CalculateSize( wxTreeListItem *item, wxDC &dc )
{
    wxCoord text_w = 0;
    wxCoord text_h = 0;

    if (item->IsBold())
        dc.SetFont(m_boldFont);

    dc.GetTextExtent( item->GetText(/*ALB*/m_main_column), &text_w, &text_h );
    text_h+=2;

    // restore normal font
    dc.SetFont( m_normalFont );

    int total_h = (m_imgHeight > text_h) ? m_imgHeight : text_h;

    item->SetHeight(total_h);
    if (total_h>m_lineHeight)
        m_lineHeight=total_h;

    item->SetWidth(m_imgWidth + text_w+2);
}

// -----------------------------------------------------------------------------
// for developper : y is now the top of the level
// not the middle of it !
void wxTreeListMainWindow::CalculateLevel( wxTreeListItem *item, wxDC &dc,
                                        int level, int &y, int x_colstart )
{
    // calculate position of vertical lines
    int x = x_colstart + MARGIN; // start of column
    if (HasFlag(wxTR_LINES_AT_ROOT)) x += LINEATROOT; // space for lines at root
    if (HasButtons()) x += m_btnWidth2; // space for buttons etc.
    if (!HasFlag(wxTR_HIDE_ROOT)) x += m_indent; // indent root as well
    x += m_indent * level; // indent according to level

    // a hidden root is not evaluated, but its children are always
    if (HasFlag(wxTR_HIDE_ROOT) && (level == 0)) goto Recurse;

    CalculateSize( item, dc );

    // set its position
    item->SetX (x);
    item->SetY (y);
    y += GetLineHeight(item);

    // we don't need to calculate collapsed branches
    if ( !item->IsExpanded() ) return;

Recurse:
    wxArrayTreeListItems& children = item->GetChildren();
    size_t n, count = children.Count();
    ++level;
    for (n = 0; n < count; ++n )
        CalculateLevel( children[n], dc, level, y, x_colstart );  // recurse
}

void wxTreeListMainWindow::CalculatePositions()
{
    if ( !m_anchor ) return;

    wxClientDC dc(this);
    PrepareDC( dc );

    dc.SetFont( m_normalFont );

    dc.SetPen( m_dottedPen );
    //if(GetImageList() == NULL)
    // m_lineHeight = (int)(dc.GetCharHeight() + 4);

    int y = 2;
    int x_colstart = 0;
    for(size_t i = 0; i < GetMainColumn(); ++i) {
        if (!m_owner->GetHeaderWindow()->GetColumnShown(i)) continue;
        x_colstart += m_owner->GetHeaderWindow()->GetColumnWidth(i);
    }
    CalculateLevel( m_anchor, dc, 0, y, x_colstart ); // start recursion
}

void wxTreeListMainWindow::RefreshSubtree(wxTreeListItem *item)
{
    if (m_dirty) return;

    wxClientDC dc(this);
    PrepareDC(dc);

    int cw = 0;
    int ch = 0;
    GetVirtualSize( &cw, &ch ); 

    wxRect rect;
    rect.x = dc.LogicalToDeviceX( 0 );
    rect.width = cw;
    rect.y = dc.LogicalToDeviceY( item->GetY() - 2 );
    rect.height = ch;

    Refresh( TRUE, &rect );

    AdjustMyScrollbars();
}

void wxTreeListMainWindow::RefreshLine( wxTreeListItem *item )
{
    if (m_dirty) return;

    wxClientDC dc(this);
    PrepareDC( dc );

    int cw = 0;
    int ch = 0;
    GetVirtualSize( &cw, &ch );  

    wxRect rect;
    rect.x = dc.LogicalToDeviceX( 0 );
    rect.y = dc.LogicalToDeviceY( item->GetY() );
    rect.width = cw;
    rect.height = GetLineHeight(item); //dc.GetCharHeight() + 6;

    Refresh( TRUE, &rect );
}

void wxTreeListMainWindow::RefreshSelected()
{
    // TODO: this is awfully inefficient, we should keep the list of all
    //       selected items internally, should be much faster
    if ( m_anchor )
        RefreshSelectedUnder(m_anchor);
}

void wxTreeListMainWindow::RefreshSelectedUnder(wxTreeListItem *item)
{
    if ( item->IsSelected() )
        RefreshLine(item);

    const wxArrayTreeListItems& children = item->GetChildren();
    size_t count = children.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        RefreshSelectedUnder(children[n]);
    }
}

// ----------------------------------------------------------------------------
// changing colours: we need to refresh the tree control
// ----------------------------------------------------------------------------

bool wxTreeListMainWindow::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxWindow::SetBackgroundColour(colour) )
        return FALSE;

    Refresh();

    return TRUE;
}

bool wxTreeListMainWindow::SetForegroundColour(const wxColour& colour)
{
    if ( !wxWindow::SetForegroundColour(colour) )
        return FALSE;

    Refresh();

    return TRUE;
}

//----------- ALB -------------
void wxTreeListMainWindow::SetItemText(const wxTreeItemId& item, size_t column,
                                    const wxString& text)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxClientDC dc(this);
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->SetText(column, text);
    CalculateSize(pItem, dc);
    RefreshLine(pItem);
}

wxString wxTreeListMainWindow::GetItemText(const wxTreeItemId& item,
                                     size_t column) const
{
    wxCHECK_MSG( item.IsOk(), wxT(""), wxT("invalid tree item") );

    return ((wxTreeListItem*) item.m_pItem)->GetText(column);
}

void wxTreeListMainWindow::SetFocus()
{
    wxWindow::SetFocus();
}


//-----------------------------------------------------------------------------
//  wxTreeListCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeListCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTreeListCtrl, wxControl)
    EVT_SIZE(wxTreeListCtrl::OnSize)
    EVT_TREE_ITEM_GETTOOLTIP(wxID_ANY, wxTreeListCtrl::OnGetToolTip)
END_EVENT_TABLE();

bool wxTreeListCtrl::Create(wxWindow *parent, wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style, const wxValidator &validator,
                            const wxString& name)
{
    long main_style = style & ~(wxRAISED_BORDER|wxSUNKEN_BORDER
                                |wxSIMPLE_BORDER|wxNO_BORDER|wxDOUBLE_BORDER
                                |wxSTATIC_BORDER);
    long ctrl_style = style & ~(wxVSCROLL|wxHSCROLL);

    if (!wxControl::Create(parent, id, pos, size, ctrl_style, validator, name)) {
       return false;
    }
    m_main_win = new wxTreeListMainWindow(this, -1, wxPoint(0, 0), size,
                                          main_style, validator);
    m_header_win = new wxTreeListHeaderWindow(this, -1, m_main_win,
                                              wxPoint(0, 0), wxDefaultSize,
                                              wxTAB_TRAVERSAL);
    CalculateAndSetHeaderHeight();
    return TRUE;
}

void wxTreeListCtrl::CalculateAndSetHeaderHeight()
{
    if ( m_header_win )
    {
        // we use 'g' to get the descent, too
        int w, h, d;
        m_header_win->GetTextExtent(wxT("Hg"), &w, &h, &d);
        h += d + 2 * HEADER_OFFSET_Y + EXTRA_HEIGHT;

        // only update if changed
        if ( h != (int)m_headerHeight )
        {
            m_headerHeight = (size_t)h;
            m_header_win->SetSize(m_header_win->GetSize().x, m_headerHeight);
        }
    }
}

void wxTreeListCtrl::DoHeaderLayout()
{
    int w, h;
    GetClientSize(&w, &h);
    if (m_header_win)
    {
        m_header_win->SetSize(0, 0, w, m_headerHeight);
        m_header_win->Refresh(false);
    }
    if (m_main_win)
        m_main_win->SetSize(0, m_headerHeight + 1, w, h - m_headerHeight - 1);
}    

void wxTreeListCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    DoHeaderLayout();
}


size_t wxTreeListCtrl::GetCount() const { return m_main_win->GetCount(); }

unsigned int wxTreeListCtrl::GetIndent() const
{ return m_main_win->GetIndent(); }

void wxTreeListCtrl::SetIndent(unsigned int indent)
{ m_main_win->SetIndent(indent); }

unsigned int wxTreeListCtrl::GetLineSpacing() const
{ return m_main_win->GetLineSpacing(); }

void wxTreeListCtrl::SetLineSpacing(unsigned int spacing)
{ m_main_win->SetLineSpacing(spacing); }

wxImageList* wxTreeListCtrl::GetImageList() const
{ return m_main_win->GetImageList(); }

wxImageList* wxTreeListCtrl::GetStateImageList() const
{ return m_main_win->GetStateImageList(); }

wxImageList* wxTreeListCtrl::GetButtonsImageList() const
{ return m_main_win->GetButtonsImageList(); }

void wxTreeListCtrl::SetImageList(wxImageList* imageList)
{ m_main_win->SetImageList(imageList); }

void wxTreeListCtrl::SetStateImageList(wxImageList* imageList)
{ m_main_win->SetStateImageList(imageList); }

void wxTreeListCtrl::SetButtonsImageList(wxImageList* imageList)
{ m_main_win->SetButtonsImageList(imageList); }

void wxTreeListCtrl::AssignImageList(wxImageList* imageList)
{ m_main_win->AssignImageList(imageList); }

void wxTreeListCtrl::AssignStateImageList(wxImageList* imageList)
{ m_main_win->AssignStateImageList(imageList); }

void wxTreeListCtrl::AssignButtonsImageList(wxImageList* imageList)
{ m_main_win->AssignButtonsImageList(imageList); }

wxString wxTreeListCtrl::GetItemText(const wxTreeItemId& item, size_t column)
    const
{ return m_main_win->GetItemText(item, column); }

int wxTreeListCtrl::GetItemImage(const wxTreeItemId& item, size_t column,
                                 wxTreeItemIcon which) const
{ return m_main_win->GetItemImage(item, column, which); }

wxTreeItemData* wxTreeListCtrl::GetItemData(const wxTreeItemId& item) const
{ return m_main_win->GetItemData(item); }

bool wxTreeListCtrl::GetItemBold(const wxTreeItemId& item) const
{ return m_main_win->GetItemBold(item); }

wxColour wxTreeListCtrl::GetItemTextColour(const wxTreeItemId& item) const
{ return m_main_win->GetItemTextColour(item); }

wxColour wxTreeListCtrl::GetItemBackgroundColour(const wxTreeItemId& item)
    const
{ return m_main_win->GetItemBackgroundColour(item); }

wxFont wxTreeListCtrl::GetItemFont(const wxTreeItemId& item) const
{ return m_main_win->GetItemFont(item); }


void wxTreeListCtrl::SetItemText(const wxTreeItemId& item, size_t column,
                                 const wxString& text)
{ m_main_win->SetItemText(item, column, text); }

void wxTreeListCtrl::SetItemImage(const wxTreeItemId& item,
                                  size_t column,
                                  int image,
                                  wxTreeItemIcon which)
{ m_main_win->SetItemImage(item, column, image, which); }

void wxTreeListCtrl::SetItemData(const wxTreeItemId& item,
                                 wxTreeItemData* data)
{ m_main_win->SetItemData(item, data); }

void wxTreeListCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{ m_main_win->SetItemHasChildren(item, has); }

void wxTreeListCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{ m_main_win->SetItemBold(item, bold); }

void wxTreeListCtrl::SetItemTextColour(const wxTreeItemId& item,
                                       const wxColour& col)
{ m_main_win->SetItemTextColour(item, col); }

void wxTreeListCtrl::SetItemBackgroundColour(const wxTreeItemId& item,
                                             const wxColour& col)
{ m_main_win->SetItemBackgroundColour(item, col); }

void wxTreeListCtrl::SetItemFont(const wxTreeItemId& item,
                                 const wxFont& font)
{ m_main_win->SetItemFont(item, font); }

bool wxTreeListCtrl::SetFont(const wxFont& font)
{
    if (m_header_win)
    {
        m_header_win->SetFont(font);
        CalculateAndSetHeaderHeight();
    }
    if (m_main_win)
        return m_main_win->SetFont(font);
    else return FALSE;
}

void wxTreeListCtrl::SetWindowStyle(const long style)
{
    if(m_main_win)
        m_main_win->SetWindowStyle(style);
    // TODO: provide something like wxTL_NO_HEADERS to hide m_header_win
}

long wxTreeListCtrl::GetWindowStyle() const
{
    long style = m_windowStyle;
    if(m_main_win)
        style |= m_main_win->GetWindowStyle();
    return style;
}

bool wxTreeListCtrl::IsVisible(const wxTreeItemId& item) const
{ return m_main_win->IsVisible(item); }

bool wxTreeListCtrl::ItemHasChildren(const wxTreeItemId& item) const
{ return m_main_win->ItemHasChildren(item); }

bool wxTreeListCtrl::IsExpanded(const wxTreeItemId& item) const
{ return m_main_win->IsExpanded(item); }

bool wxTreeListCtrl::IsSelected(const wxTreeItemId& item) const
{ return m_main_win->IsSelected(item); }

bool wxTreeListCtrl::IsBold(const wxTreeItemId& item) const
{ return m_main_win->IsBold(item); }

size_t wxTreeListCtrl::GetChildrenCount(const wxTreeItemId& item, bool rec)
{ return m_main_win->GetChildrenCount(item, rec); }

wxTreeItemId wxTreeListCtrl::GetRootItem() const
{ return m_main_win->GetRootItem(); }

wxTreeItemId wxTreeListCtrl::GetSelection() const
{ return m_main_win->GetSelection(); }

size_t wxTreeListCtrl::GetSelections(wxArrayTreeItemIds& arr) const
{ return m_main_win->GetSelections(arr); }

wxTreeItemId wxTreeListCtrl::GetItemParent(const wxTreeItemId& item) const
{ return m_main_win->GetItemParent(item); }

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListCtrl::GetFirstChild(const wxTreeItemId& item,
                                           long& cookie) const
#else
wxTreeItemId wxTreeListCtrl::GetFirstChild(const wxTreeItemId& item,
                                           wxTreeItemIdValue& cookie) const
#endif
{ return m_main_win->GetFirstChild(item, cookie); }

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListCtrl::GetNextChild(const wxTreeItemId& item,
                                          long& cookie) const
#else
wxTreeItemId wxTreeListCtrl::GetNextChild(const wxTreeItemId& item,
                                          wxTreeItemIdValue& cookie) const
#endif
{ return m_main_win->GetNextChild(item, cookie); }

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListCtrl::GetPrevChild(const wxTreeItemId& item,
                                          long& cookie) const
#else
wxTreeItemId wxTreeListCtrl::GetPrevChild(const wxTreeItemId& item,
                                          wxTreeItemIdValue& cookie) const
#endif
{ return m_main_win->GetPrevChild(item, cookie); }

wxTreeItemId wxTreeListCtrl::GetLastChild(const wxTreeItemId& item) const
{ return m_main_win->GetLastChild(item); }

wxTreeItemId wxTreeListCtrl::GetNextSibling(const wxTreeItemId& item) const
{ return m_main_win->GetNextSibling(item); }

wxTreeItemId wxTreeListCtrl::GetPrevSibling(const wxTreeItemId& item) const
{ return m_main_win->GetPrevSibling(item); }

wxTreeItemId wxTreeListCtrl::GetFirstVisibleItem() const
{ return m_main_win->GetFirstVisibleItem(); }

wxTreeItemId wxTreeListCtrl::GetNextVisible(const wxTreeItemId& item) const
{ return m_main_win->GetNextVisible(item); }

wxTreeItemId wxTreeListCtrl::GetPrevVisible(const wxTreeItemId& item) const
{ return m_main_win->GetPrevVisible(item); }

wxTreeItemId wxTreeListCtrl::GetNext(const wxTreeItemId& item) const
{ return m_main_win->GetNext(item); }

wxTreeItemId wxTreeListCtrl::AddRoot(const wxString& text, int image,
                                     int selectedImage, wxTreeItemData* data)
{ return m_main_win->AddRoot(text, image, selectedImage, data); }

wxTreeItemId wxTreeListCtrl::PrependItem(const wxTreeItemId& parent,
                                         const wxString& text, int image,
                                         int selectedImage,
                                         wxTreeItemData* data)
{ return m_main_win->PrependItem(parent, text, image, selectedImage, data); }

wxTreeItemId wxTreeListCtrl::InsertItem(const wxTreeItemId& parent,
                                        const wxTreeItemId& previous,
                                        const wxString& text, int image,
                                        int selectedImage,
                                        wxTreeItemData* data)
{
    return m_main_win->InsertItem(parent, previous, text, image,
                                  selectedImage, data);
}

wxTreeItemId wxTreeListCtrl::InsertItem(const wxTreeItemId& parent,
                                        size_t index,
                                        const wxString& text, int image,
                                        int selectedImage,
                                        wxTreeItemData* data)
{
    return m_main_win->InsertItem(parent, index, text, image,
                                  selectedImage, data);
}

wxTreeItemId wxTreeListCtrl::AppendItem(const wxTreeItemId& parent,
                                        const wxString& text, int image,
                                        int selectedImage,
                                        wxTreeItemData* data)
{ return m_main_win->AppendItem(parent, text, image, selectedImage, data); }

void wxTreeListCtrl::Delete(const wxTreeItemId& item)
{ m_main_win->Delete(item); }

void wxTreeListCtrl::DeleteChildren(const wxTreeItemId& item)
{ m_main_win->DeleteChildren(item); }

void wxTreeListCtrl::DeleteAllItems()
{ m_main_win->DeleteAllItems(); }

void wxTreeListCtrl::Expand(const wxTreeItemId& item)
{ m_main_win->Expand(item); }

void wxTreeListCtrl::ExpandAll(const wxTreeItemId& item)
{ m_main_win->ExpandAll(item); }

void wxTreeListCtrl::Collapse(const wxTreeItemId& item)
{ m_main_win->Collapse(item); }

void wxTreeListCtrl::CollapseAndReset(const wxTreeItemId& item)
{ m_main_win->CollapseAndReset(item); }

void wxTreeListCtrl::Toggle(const wxTreeItemId& item)
{ m_main_win->Toggle(item); }

void wxTreeListCtrl::Unselect()
{ m_main_win->Unselect(); }

void wxTreeListCtrl::UnselectAll()
{ m_main_win->UnselectAll(); }

void wxTreeListCtrl::SelectItem(const wxTreeItemId& item, bool unselect_others,
                                bool extended_select)
{ m_main_win->SelectItem(item, unselect_others, extended_select); }

void wxTreeListCtrl::SelectAll(bool extended_select)
{ m_main_win->SelectAll(extended_select); }

void wxTreeListCtrl::EnsureVisible(const wxTreeItemId& item)
{ m_main_win->EnsureVisible(item); }

void wxTreeListCtrl::ScrollTo(const wxTreeItemId& item)
{ m_main_win->ScrollTo(item); }

wxTreeItemId wxTreeListCtrl::HitTest(const wxPoint& pos, int& flags,
                                     int& column)
{
    return m_main_win->HitTest(pos, flags, column);
}

bool wxTreeListCtrl::GetBoundingRect(const wxTreeItemId& item, wxRect& rect,
                                     bool textOnly) const
{ return m_main_win->GetBoundingRect(item, rect, textOnly); }

void wxTreeListCtrl::Edit(const wxTreeItemId& item)
{ m_main_win->Edit(item); }

int wxTreeListCtrl::OnCompareItems(const wxTreeItemId& item1,
                                   const wxTreeItemId& item2)
{
    // ALB: do the comparison here, and not delegate to m_main_win, in order
    // to let the user override it
    //return m_main_win->OnCompareItems(item1, item2);
    return wxStrcmp(GetItemText(item1), GetItemText(item2));
}

void wxTreeListCtrl::SortChildren(const wxTreeItemId& item)
{ m_main_win->SortChildren(item); }

wxTreeItemId wxTreeListCtrl::FindItem (const wxTreeItemId& item, const wxString& str, int flags)
{ return m_main_win->FindItem (item, str, flags); }

bool wxTreeListCtrl::SetBackgroundColour(const wxColour& colour)
{ 
    if (!m_main_win) return false;
    return m_main_win->SetBackgroundColour(colour); 
}

bool wxTreeListCtrl::SetForegroundColour(const wxColour& colour)
{ 
    if (!m_main_win) return false;
    return m_main_win->SetForegroundColour(colour); 
}

size_t wxTreeListCtrl::GetColumnCount() const
{ return m_main_win->GetColumnCount(); }

void wxTreeListCtrl::SetColumnWidth(size_t column, size_t width)
{ m_header_win->SetColumnWidth(column, width); }

int wxTreeListCtrl::GetColumnWidth(size_t column) const
{ return m_header_win->GetColumnWidth(column); }

void wxTreeListCtrl::SetMainColumn(size_t column)
{ m_main_win->SetMainColumn(column); }

size_t wxTreeListCtrl::GetMainColumn() const
{ return m_main_win->GetMainColumn(); }

void wxTreeListCtrl::SetColumnText(size_t column, const wxString& text)
{
    m_header_win->SetColumnText(column, text);
    m_header_win->Refresh();
}

wxString wxTreeListCtrl::GetColumnText(size_t column) const
{ return m_header_win->GetColumnText(column); }

void wxTreeListCtrl::AddColumn(const wxTreeListColumnInfo& col)
{
    m_header_win->AddColumn(col);
    DoHeaderLayout();
}

void wxTreeListCtrl::InsertColumn(size_t before,
                                  const wxTreeListColumnInfo& col)
{ m_header_win->InsertColumn(before, col); }

void wxTreeListCtrl::RemoveColumn(size_t column)
{ m_header_win->RemoveColumn(column); }

void wxTreeListCtrl::SetColumn(size_t column, const wxTreeListColumnInfo& col)
{ m_header_win->SetColumn(column, col); }

const wxTreeListColumnInfo& wxTreeListCtrl::GetColumn(size_t column) const
{ return m_header_win->GetColumn(column); }

wxTreeListColumnInfo& wxTreeListCtrl::GetColumn(size_t column)
{ return m_header_win->GetColumn(column); }

void wxTreeListCtrl::SetColumnImage(size_t column, int image)
{
    m_header_win->SetColumn(column, GetColumn(column).SetImage(image));
}

int wxTreeListCtrl::GetColumnImage(size_t column) const
{
    return m_header_win->GetColumn(column).GetImage();
}

void wxTreeListCtrl::ShowColumn(size_t column, bool shown)
{
    wxASSERT_MSG( column != GetMainColumn(),
                  wxT("The main column may not be hidden") );
    m_header_win->SetColumn(column, GetColumn(column).SetShown(GetMainColumn()? true: shown));
}

bool wxTreeListCtrl::IsColumnShown(size_t column) const
{
    return m_header_win->GetColumn(column).GetShown();
}

void wxTreeListCtrl::SetColumnAlignment(size_t column,
                                        wxTreeListColumnAlign align)
{
    m_header_win->SetColumn(column, GetColumn(column).SetAlignment(align));
}

wxTreeListColumnAlign wxTreeListCtrl::GetColumnAlignment(size_t column) const
{
    return m_header_win->GetColumn(column).GetAlignment();
}

void wxTreeListCtrl::Refresh(bool erase, const wxRect* rect)
{
    m_main_win->Refresh(erase, rect);
    m_header_win->Refresh(erase, rect);
}

void wxTreeListCtrl::SetFocus()
{ m_main_win->SetFocus(); }


wxSize wxTreeListCtrl::DoGetBestSize() const
{
    // something is better than nothing...
    return wxSize(100,80);
}

// Process the tooltip event, to speed up event processing.
// Doesn't actually get a tooltip.
void wxTreeListCtrl::OnGetToolTip( wxTreeEvent &event )
{
    event.Veto();
}

