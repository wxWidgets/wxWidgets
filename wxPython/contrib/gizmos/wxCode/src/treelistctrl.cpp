/////////////////////////////////////////////////////////////////////////////
// Name:        treelistctrl.cpp
// Purpose:     multi column tree control implementation
// Author:      Robert Roebling
// Maintainer:  Otto Wyss
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Robert Roebling, Julian Smart, Alberto Griggio,
//              Vadim Zeitlin, Otto Wyss
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
  #pragma implementation "treelistctrl.h"
#endif

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
#if wxCHECK_VERSION(2, 7, 0)
#include <wx/renderer.h>
#endif

#ifdef __WXMAC__
#include "wx/mac/private.h"
#endif

#include "wx/treelistctrl.h"


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


// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

static const int NO_IMAGE = -1;

static const int LINEHEIGHT = 10;
static const int LINEATROOT = 5;
static const int MARGIN = 2;
static const int MININDENT = 16;
static const int BTNWIDTH = 9;
static const int BTNHEIGHT = 9;
static const int EXTRA_WIDTH = 4;
static const int EXTRA_HEIGHT = 4;
static const int HEADER_OFFSET_X = 1;
static const int HEADER_OFFSET_Y = 1;

static const int DRAG_TIMER_TICKS = 250; // minimum drag wait time in ms
static const int FIND_TIMER_TICKS = 500; // minimum find wait time in ms
static const int RENAME_TIMER_TICKS = 250; // minimum rename wait time in ms

const wxChar* wxTreeListCtrlNameStr = _T("treelistctrl");

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
    const wxCursor *m_currentCursor;
    const wxCursor *m_resizeCursor;
    bool m_isDragging;

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
                            const wxString &name = _T("wxtreelistctrlcolumntitles") );

    virtual ~wxTreeListHeaderWindow();

    void DoDrawRect( wxDC *dc, int x, int y, int w, int h );
    void DrawCurrent();
    void AdjustDC(wxDC& dc);

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );

    // total width of all columns
    int GetWidth() const { return m_total_col_width; }

    // column manipulation
    int GetColumnCount() const { return m_columns.GetCount(); }

    void AddColumn (const wxTreeListColumnInfo& colInfo);

    void InsertColumn (int before, const wxTreeListColumnInfo& colInfo);

    void RemoveColumn (int column);

    // column information manipulation
    const wxTreeListColumnInfo& GetColumn (int column) const{
        wxCHECK_MSG ((column >= 0) && (column < GetColumnCount()),
                     wxInvalidTreeListColumnInfo, _T("Invalid column"));
        return m_columns[column];
    }
    wxTreeListColumnInfo& GetColumn (int column) {
        wxCHECK_MSG ((column >= 0) && (column < GetColumnCount()),
                     wxInvalidTreeListColumnInfo, _T("Invalid column"));
        return m_columns[column];
    }
    void SetColumn (int column, const wxTreeListColumnInfo& info);

    wxString GetColumnText (int column) const {
        wxCHECK_MSG ((column >= 0) && (column < GetColumnCount()),
                     wxEmptyString, _T("Invalid column"));
        return m_columns[column].GetText();
    }
    void SetColumnText (int column, const wxString& text) {
        wxCHECK_RET ((column >= 0) && (column < GetColumnCount()),
                     _T("Invalid column"));
        m_columns[column].SetText (text);
    }

    int GetColumnAlignment (int column) const {
        wxCHECK_MSG ((column >= 0) && (column < GetColumnCount()),
                     wxALIGN_LEFT, _T("Invalid column"));
        return m_columns[column].GetAlignment();
    }
    void SetColumnAlignment (int column, int flag) {
        wxCHECK_RET ((column >= 0) && (column < GetColumnCount()),
                     _T("Invalid column"));
        m_columns[column].SetAlignment (flag);
    }

    int GetColumnWidth (int column) const {
        wxCHECK_MSG ((column >= 0) && (column < GetColumnCount()),
                     -1, _T("Invalid column"));
        return m_columns[column].GetWidth();
    }
    void SetColumnWidth (int column, int width);

    bool IsColumnEditable (int column) const {
        wxCHECK_MSG ((column >= 0) && (column < GetColumnCount()),
                     false, _T("Invalid column"));
        return m_columns[column].IsEditable();
    }

    bool IsColumnShown (int column) const {
        wxCHECK_MSG ((column >= 0) && (column < GetColumnCount()),
                     true, _T("Invalid column"));
        return m_columns[column].IsShown();
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

    wxTreeListMainWindow (wxTreeListCtrl *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_DEFAULT_STYLE,
               const wxValidator &validator = wxDefaultValidator,
               const wxString& name = _T("wxtreelistmainwindow"))
    {
        Init();
        Create (parent, id, pos, size, style, validator, name);
    }

    virtual ~wxTreeListMainWindow();

    bool Create(wxTreeListCtrl *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_DEFAULT_STYLE,
                const wxValidator &validator = wxDefaultValidator,
                const wxString& name = _T("wxtreelistctrl"));

    // accessors
    // ---------

    // return true if this is a virtual list control
    bool IsVirtual() const { return HasFlag(wxTR_VIRTUAL); }

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
    wxImageList *GetImageList() const { return m_imageListNormal; }
    wxImageList *GetStateImageList() const { return m_imageListState; }
    wxImageList *GetButtonsImageList() const { return m_imageListButtons; }

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
    wxString GetItemText (const wxTreeItemId& item) const
    { return GetItemText (item, GetMainColumn()); }
    wxString GetItemText (const wxTreeItemId& item, int column) const;
    wxString GetItemText (wxTreeItemData* item, int column) const;

    // get one of the images associated with the item (normal by default)
    int GetItemImage (const wxTreeItemId& item,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal) const
    { return GetItemImage (item, GetMainColumn(), which); }
    int GetItemImage (const wxTreeItemId& item, int column,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal) const;

    // get the data associated with the item
    wxTreeItemData *GetItemData(const wxTreeItemId& item) const;

    bool GetItemBold(const wxTreeItemId& item) const;
    wxColour GetItemTextColour(const wxTreeItemId& item) const;
    wxColour GetItemBackgroundColour(const wxTreeItemId& item) const;
    wxFont GetItemFont(const wxTreeItemId& item) const;

    // modifiers
    // ---------

    // set item's label
    void SetItemText (const wxTreeItemId& item, const wxString& text)
    { SetItemText (item, GetMainColumn(), text); }
    void SetItemText (const wxTreeItemId& item, int column, const wxString& text);

    // get one of the images associated with the item (normal by default)
    void SetItemImage (const wxTreeItemId& item, int image,
                       wxTreeItemIcon which = wxTreeItemIcon_Normal)
    { SetItemImage (item, GetMainColumn(), image, which); }
    void SetItemImage (const wxTreeItemId& item, int column, int image,
                       wxTreeItemIcon which = wxTreeItemIcon_Normal);

    // associate some data with the item
    void SetItemData(const wxTreeItemId& item, wxTreeItemData *data);

    // force appearance of [+] button near the item. This is useful to
    // allow the user to expand the items which don't have any children now
    // - but instead add them only when needed, thus minimizing memory
    // usage and loading time.
    void SetItemHasChildren(const wxTreeItemId& item, bool has = true);

    // the item will be shown in bold
    void SetItemBold(const wxTreeItemId& item, bool bold = true);

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
    bool IsVisible(const wxTreeItemId& item, bool fullRow) const;
    // does the item has any children?
    bool HasChildren(const wxTreeItemId& item) const;
    // is the item expanded (only makes sense if HasChildren())?
    bool IsExpanded(const wxTreeItemId& item) const;
    // is this item currently selected (the same as has focus)?
    bool IsSelected(const wxTreeItemId& item) const;
    // is item text in bold font?
    bool IsBold(const wxTreeItemId& item) const;
        // does the layout include space for a button?

    // number of children
    // ------------------

    // if 'recursively' is false, only immediate children count, otherwise
    // the returned number is the number of all items in this branch
    size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = true);

    // navigation
    // ----------

    // wxTreeItemId.IsOk() will return false if there is no such item

    // get the root tree item
    wxTreeItemId GetRootItem() const { return m_rootItem; }

    // get the item currently selected, only if a single item is selected
    wxTreeItemId GetSelection() const { return m_selectItem; }

    // get all the items currently selected, return count of items
    size_t GetSelections(wxArrayTreeItemIds&) const;

    // get the parent of this item (may return NULL if root)
    wxTreeItemId GetItemParent(const wxTreeItemId& item) const;

    // for this enumeration function you must pass in a "cookie" parameter
    // which is opaque for the application but is necessary for the library
    // to make these functions reentrant (i.e. allow more than one
    // enumeration on one and the same object simultaneously). Of course,
    // the "cookie" passed to GetFirstChild() and GetNextChild() should be
    // the same!

    // get child of this item
#if !wxCHECK_VERSION(2, 5, 0)
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& cookie) const;
    wxTreeItemId GetNextChild(const wxTreeItemId& item, long& cookie) const;
    wxTreeItemId GetPrevChild(const wxTreeItemId& item, long& cookie) const;
    wxTreeItemId GetLastChild(const wxTreeItemId& item, long& cookie) const;
#else
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeItemId GetPrevChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeItemId GetLastChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
#endif

    // get sibling of this item
    wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;

    // get item in the full tree (currently only for internal use)
    wxTreeItemId GetNext(const wxTreeItemId& item, bool fulltree = true) const;
    wxTreeItemId GetPrev(const wxTreeItemId& item, bool fulltree = true) const;

    // get expanded item, see IsExpanded()
    wxTreeItemId GetFirstExpandedItem() const;
    wxTreeItemId GetNextExpanded(const wxTreeItemId& item) const;
    wxTreeItemId GetPrevExpanded(const wxTreeItemId& item) const;

    // get visible item, see IsVisible()
    wxTreeItemId GetFirstVisibleItem(bool fullRow) const;
    wxTreeItemId GetNextVisible(const wxTreeItemId& item, bool fullRow) const;
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item, bool fullRow) const;

    // operations
    // ----------

    // add the root node to the tree
    wxTreeItemId AddRoot (const wxString& text,
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
    // delete the root and all its children from the tree
    // NB: this won't send wxEVT_COMMAND_TREE_ITEM_DELETED events
    void DeleteRoot();

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
    void SelectItem(const wxTreeItemId& item, const wxTreeItemId& prev = (wxTreeItemId*)NULL,
                    bool unselect_others = true);
    void SelectAll();
    // make sure this item is visible (expanding the parent item and/or
    // scrolling to this item if necessary)
    void EnsureVisible(const wxTreeItemId& item);
    // scroll to this item (but don't expand its parent)
    void ScrollTo(const wxTreeItemId& item);
    void AdjustMyScrollbars();

    // The first function is more portable (because easier to implement
    // on other platforms), but the second one returns some extra info.
    wxTreeItemId HitTest (const wxPoint& point)
        { int flags; int column; return HitTest (point, flags, column); }
    wxTreeItemId HitTest (const wxPoint& point, int& flags)
        { int column; return HitTest (point, flags, column); }
    wxTreeItemId HitTest (const wxPoint& point, int& flags, int& column);


    // get the bounding rectangle of the item (or of its label only)
    bool GetBoundingRect(const wxTreeItemId& item,
                         wxRect& rect,
                         bool textOnly = false) const;

    // Start editing the item label: this (temporarily) replaces the item
    // with a one line edit control. The item will be selected if it hadn't
    // been before.
    void EditLabel (const wxTreeItemId& item, int column);

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
    wxTreeItemId FindItem (const wxTreeItemId& item, const wxString& str, int mode = 0);

    // implementation only from now on

    // overridden base class virtuals
    virtual bool SetBackgroundColour(const wxColour& colour);
    virtual bool SetForegroundColour(const wxColour& colour);

    // drop over item
    void SetDragItem (const wxTreeItemId& item = (wxTreeItemId*)NULL);

    // callbacks
    void OnPaint( wxPaintEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnIdle( wxIdleEvent &event );
    void OnScroll(wxScrollWinEvent& event);

    // implementation helpers
    void SendDeleteEvent(wxTreeListItem *itemBeingDeleted);

    int GetColumnCount() const
    { return m_owner->GetHeaderWindow()->GetColumnCount(); }

    void SetMainColumn (int column)
    { if ((column >= 0) && (column < GetColumnCount())) m_main_column = column; }

    int GetMainColumn() const { return m_main_column; }

    int GetBestColumnWidth (int column, wxTreeItemId parent = wxTreeItemId());
    int GetItemWidth (int column, wxTreeListItem *item);
    wxFont GetItemFont (wxTreeListItem *item);

    void SetFocus();

protected:
    wxTreeListCtrl* m_owner;

    int m_main_column;

    friend class wxTreeListItem;
    friend class wxTreeListRenameTimer;
    friend class wxEditTextCtrl;

    wxFont               m_normalFont;
    wxFont               m_boldFont;

    wxTreeListItem       *m_rootItem; // root item
    wxTreeListItem       *m_curItem; // current item, either selected or marked
    wxTreeListItem       *m_shiftItem; // item, where the shift key was pressed
    wxTreeListItem       *m_editItem; // item, which is currently edited
    wxTreeListItem       *m_selectItem; // current selected item, not with wxTR_MULTIPLE

    int                  m_curColumn;

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
    bool                 m_left_down_selection;

    wxImageList         *m_imageListNormal,
                        *m_imageListState,
                        *m_imageListButtons;

    int                  m_dragCount;
    wxTimer             *m_dragTimer;
    wxTreeListItem      *m_dragItem;

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
        { return (m_imageListButtons) || HasFlag (wxTR_TWIST_BUTTONS|wxTR_HAS_BUTTONS); }

protected:
    void CalculateLineHeight();
    int  GetLineHeight(wxTreeListItem *item) const;
    void PaintLevel( wxTreeListItem *item, wxDC& dc, int level, int &y,
                     int x_maincol);
    void PaintItem( wxTreeListItem *item, wxDC& dc);

    void CalculateLevel( wxTreeListItem *item, wxDC &dc, int level, int &y,
                         int x_maincol);
    void CalculatePositions();
    void CalculateSize( wxTreeListItem *item, wxDC &dc );

    void RefreshSubtree (wxTreeListItem *item);
    void RefreshLine (wxTreeListItem *item);

    // redraw all selected items
    void RefreshSelected();

    // RefreshSelected() recursive helper
    void RefreshSelectedUnder (wxTreeListItem *item);

    void OnRenameTimer();
    void OnRenameAccept();

    void FillArray(wxTreeListItem*, wxArrayTreeItemIds&) const;
    bool TagAllChildrenUntilLast (wxTreeListItem *crt_item, wxTreeListItem *last_item);
    bool TagNextChildren (wxTreeListItem *crt_item, wxTreeListItem *last_item);
    void UnselectAllChildren (wxTreeListItem *item );

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
class  wxEditTextCtrl: public wxTextCtrl
{
public:
    wxEditTextCtrl (wxWindow *parent,
                    const wxWindowID id,
                    bool *accept,
                    wxString *res,
                    wxTreeListMainWindow *owner,
                    const wxString &value = wxEmptyString,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize,
                    int style = 0,
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
        return GetText(0);
    }
    const wxString GetText (int column) const
    {
        if(m_text.GetCount() > 0)
        {
            if( IsVirtual() )   return m_owner->GetItemText( m_data, column );
            else                return m_text[column];
        }
        return wxEmptyString;
    }

    int GetImage (wxTreeItemIcon which = wxTreeItemIcon_Normal) const
        { return m_images[which]; }
    int GetImage (int column, wxTreeItemIcon which=wxTreeItemIcon_Normal) const
    {
        if(column == m_owner->GetMainColumn()) return m_images[which];
        if(column < (int)m_col_images.GetCount()) return m_col_images[column];
        return NO_IMAGE;
    }

    wxTreeItemData *GetData() const { return m_data; }

    // returns the current image for the item (depending on its
    // selected/expanded/whatever state)
    int GetCurrentImage() const;

    void SetText (const wxString &text );
    void SetText (int column, const wxString& text)
    {
        if (column < (int)m_text.GetCount()) {
            m_text[column] = text;
        }else if (column < m_owner->GetColumnCount()) {
            int howmany = m_owner->GetColumnCount();
            for (int i = m_text.GetCount(); i < howmany; ++i) m_text.Add (wxEmptyString);
            m_text[column] = text;
        }
    }
    void SetImage (int image, wxTreeItemIcon which) { m_images[which] = image; }
    void SetImage (int column, int image, wxTreeItemIcon which)
    {
        if (column == m_owner->GetMainColumn()) {
            m_images[which] = image;
        }else if (column < (int)m_col_images.GetCount()) {
            m_col_images[column] = image;
        }else if (column < m_owner->GetColumnCount()) {
            int howmany = m_owner->GetColumnCount();
            for (int i = m_col_images.GetCount(); i < howmany; ++i) m_col_images.Add (NO_IMAGE);
            m_col_images[column] = image;
        }
    }

    void SetData(wxTreeItemData *data) { m_data = data; }

    void SetHasPlus(bool has = true) { m_hasPlus = has; }

    void SetBold(bool bold) { m_isBold = bold; }

    int GetX() const { return m_x; }
    int GetY() const { return m_y; }

    void SetX (int x) { m_x = x; }
    void SetY (int y) { m_y = y; }

    int  GetHeight() const { return m_height; }
    int  GetWidth()  const { return m_width; }

    void SetHeight (int height) { m_height = height; }
    void SetWidth (int width) { m_width = width; }

    int GetTextX() const { return m_text_x; }
    void SetTextX (int text_x) { m_text_x = text_x; }

    wxTreeListItem *GetItemParent() const { return m_parent; }

    // operations
    // deletes all children notifying the treectrl about it if !NULL
    // pointer given
    void DeleteChildren(wxTreeListMainWindow *tree = NULL);

    // get count of all children (and grand children if 'recursively')
    size_t GetChildrenCount(bool recursively = true) const;

    void Insert(wxTreeListItem *child, size_t index)
    { m_children.Insert(child, index); }

    void GetSize( int &x, int &y, const wxTreeListMainWindow* );

    // return the item at given position (or NULL if no item), onButton is
    // true if the point belongs to the item's button, otherwise it lies
    // on the button's label
    wxTreeListItem *HitTest (const wxPoint& point,
                             const wxTreeListMainWindow *,
                             int &flags, int& column, int level);

    void Expand() { m_isCollapsed = false; }
    void Collapse() { m_isCollapsed = true; }

    void SetHilight( bool set = true ) { m_hasHilight = set; }

    // status inquiries
    bool HasChildren() const { return !m_children.IsEmpty(); }
    bool IsSelected()  const { return m_hasHilight != 0; }
    bool IsExpanded()  const { return !m_isCollapsed; }
    bool HasPlus()     const { return m_hasPlus || HasChildren(); }
    bool IsBold()      const { return m_isBold != 0; }
    bool IsVirtual()   const { return m_owner->IsVirtual(); }

    // attributes
    // get them - may be NULL
    wxTreeItemAttr *GetAttributes() const { return m_attr; }
    // get them ensuring that the pointer is not NULL
    wxTreeItemAttr& Attr()
    {
        if ( !m_attr )
        {
            m_attr = new wxTreeItemAttr;
            m_ownsAttr = true;
        }
        return *m_attr;
    }
    // set them
    void SetAttributes(wxTreeItemAttr *attr)
    {
        if ( m_ownsAttr ) delete m_attr;
        m_attr = attr;
        m_ownsAttr = false;
    }
    // set them and delete when done
    void AssignAttributes(wxTreeItemAttr *attr)
    {
        SetAttributes(attr);
        m_ownsAttr = true;
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

    // main column item positions
    wxCoord             m_x;            // (virtual) offset from left (vertical line)
    wxCoord             m_y;            // (virtual) offset from top
    wxCoord             m_text_x;       // item offset from left
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
// wxEditTextCtrl (internal)
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE (wxEditTextCtrl,wxTextCtrl)
    EVT_CHAR           (wxEditTextCtrl::OnChar)
    EVT_KEY_UP         (wxEditTextCtrl::OnKeyUp)
    EVT_KILL_FOCUS     (wxEditTextCtrl::OnKillFocus)
END_EVENT_TABLE()

wxEditTextCtrl::wxEditTextCtrl (wxWindow *parent,
                                const wxWindowID id,
                                bool *accept,
                                wxString *res,
                                wxTreeListMainWindow *owner,
                                const wxString &value,
                                const wxPoint &pos,
                                const wxSize &size,
                                int style,
                                const wxValidator& validator,
                                const wxString &name)
    : wxTextCtrl (parent, id, value, pos, size, style|wxSIMPLE_BORDER, validator, name)
{
    m_res = res;
    m_accept = accept;
    m_owner = owner;
    (*m_accept) = false;
    (*m_res) = wxEmptyString;
    m_startValue = value;
    m_finished = false;
}

void wxEditTextCtrl::OnChar( wxKeyEvent &event )
{
    if (event.GetKeyCode() == WXK_RETURN)
    {
        (*m_accept) = true;
        (*m_res) = GetValue();

        if ((*m_res) != m_startValue)
            m_owner->OnRenameAccept();

        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);

        m_finished = true;
        m_owner->SetFocus(); // This doesn't work. TODO.

        return;
    }
    if (event.GetKeyCode() == WXK_ESCAPE)
    {
        (*m_accept) = false;
        (*m_res) = wxEmptyString;

        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);

        m_finished = true;
        m_owner->SetFocus(); // This doesn't work. TODO.

        return;
    }
    event.Skip();
}

void wxEditTextCtrl::OnKeyUp( wxKeyEvent &event )
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

void wxEditTextCtrl::OnKillFocus( wxFocusEvent &event )
{
    if (m_finished)
    {
        event.Skip();
        return;
    }

    if (!wxPendingDelete.Member(this))
        wxPendingDelete.Append(this);

    (*m_accept) = true;
    (*m_res) = GetValue();

    if ((*m_res) != m_startValue)
        m_owner->OnRenameAccept();
}

//-----------------------------------------------------------------------------
//  wxTreeListHeaderWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeListHeaderWindow,wxWindow);

BEGIN_EVENT_TABLE(wxTreeListHeaderWindow,wxWindow)
    EVT_PAINT         (wxTreeListHeaderWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxTreeListHeaderWindow::OnMouse)
    EVT_SET_FOCUS     (wxTreeListHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

void wxTreeListHeaderWindow::Init()
{
    m_currentCursor = (wxCursor *) NULL;
    m_isDragging = false;
    m_dirty = false;
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

#if !wxCHECK_VERSION(2, 5, 0)
    SetBackgroundColour (wxSystemSettings::GetSystemColour (wxSYS_COLOUR_BTNFACE));
#else
    SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_BTNFACE));
#endif
}

wxTreeListHeaderWindow::~wxTreeListHeaderWindow()
{
    delete m_resizeCursor;
}

void wxTreeListHeaderWindow::DoDrawRect( wxDC *dc, int x, int y, int w, int h )
{
#if !wxCHECK_VERSION(2, 5, 0)
    wxPen pen (wxSystemSettings::GetSystemColour (wxSYS_COLOUR_BTNSHADOW ), 1, wxSOLID);
#else
    wxPen pen (wxSystemSettings::GetColour (wxSYS_COLOUR_BTNSHADOW ), 1, wxSOLID);
#endif

    const int m_corner = 1;

    dc->SetBrush( *wxTRANSPARENT_BRUSH );
#if defined( __WXMAC__  )
    dc->SetPen (pen);
#else // !GTK, !Mac
    dc->SetPen( *wxBLACK_PEN );
#endif
    dc->DrawLine( x+w-m_corner+1, y, x+w, y+h );  // right (outer)
    dc->DrawRectangle( x, y+h, w+1, 1 );          // bottom (outer)

#if defined( __WXMAC__  )
    wxPen pen( wxColour( 0x88 , 0x88 , 0x88 ), 1, wxSOLID );
#endif
    dc->SetPen( pen );
    dc->DrawLine( x+w-m_corner, y, x+w-1, y+h );  // right (inner)
    dc->DrawRectangle( x+1, y+h-1, w-2, 1 );      // bottom (inner)

    dc->SetPen( *wxWHITE_PEN );
    dc->DrawRectangle( x, y, w-m_corner+1, 1 );   // top (outer)
    dc->DrawRectangle( x, y, 1, h );              // left (outer)
    dc->DrawLine( x, y+h-1, x+1, y+h-1 );
    dc->DrawLine( x+w-1, y, x+w-1, y+1 );
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

void wxTreeListHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
#ifdef __WXGTK__
    wxClientDC dc( this );
#else
    wxPaintDC dc( this );
#endif

    PrepareDC( dc );
    AdjustDC( dc );
    dc.SetFont( GetFont() );

    // width and height of the entire header window
    int w, h;
    GetClientSize( &w, &h );
    m_owner->CalcUnscrolledPosition(w, 0, &w, NULL);
    dc.SetBackgroundMode(wxTRANSPARENT);

    // do *not* use the listctrl colour for headers - one day we will have a
    // function to set it separately
    //dc.SetTextForeground( *wxBLACK );
#if !wxCHECK_VERSION(2, 5, 0)
    dc.SetTextForeground (wxSystemSettings::GetSystemColour( wxSYS_COLOUR_WINDOWTEXT ));
#else
    dc.SetTextForeground (wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ));
#endif

    int x = HEADER_OFFSET_X;

    int numColumns = GetColumnCount();
    for ( int i = 0; i < numColumns && x < w; i++ )
    {
        if (!IsColumnShown (i)) continue; // do next colume if not shown

        wxTreeListColumnInfo& column = GetColumn(i);
        int wCol = column.GetWidth();

        // the width of the rect to draw: make it smaller to fit entirely
        // inside the column rect
        int cw = wCol - 2;

#if !wxCHECK_VERSION(2, 7, 0)
        dc.SetPen( *wxWHITE_PEN );
        DoDrawRect( &dc, x, HEADER_OFFSET_Y, cw, h-2 );
#else
        wxRect rect(x, HEADER_OFFSET_Y, cw, h-2);
        wxRendererNative::GetDefault().DrawHeaderButton (this, dc, rect);
#endif

        // if we have an image, draw it on the right of the label
        int image = column.GetImage(); //item.m_image;
        int ix = -2, iy = 0;
        wxImageList* imageList = m_owner->GetImageList();
        if ((image != -1) && imageList) {
            imageList->GetSize (image, ix, iy);
        }

        // extra margins around the text label
        int text_width = 0;
        int text_x = x;
        int image_offset = cw - ix - 1;

        switch(column.GetAlignment()) {
        case wxALIGN_LEFT:
            text_x += EXTRA_WIDTH;
            cw -= ix + 2;
            break;
        case wxALIGN_RIGHT:
            dc.GetTextExtent (column.GetText(), &text_width, NULL);
            text_x += cw - text_width - EXTRA_WIDTH - MARGIN;
            image_offset = 0;
            break;
        case wxALIGN_CENTER:
            dc.GetTextExtent(column.GetText(), &text_width, NULL);
            text_x += (cw - text_width)/2 + ix + 2;
            image_offset = (cw - text_width - ix - 2)/2 - MARGIN;
            break;
        }

        // draw the image
        if ((image != -1) && imageList) {
            imageList->Draw (image, dc, x + image_offset/*cw - ix - 1*/,
                             HEADER_OFFSET_Y + (h - 4 - iy)/2,
                             wxIMAGELIST_DRAW_TRANSPARENT);
        }

        // draw the text clipping it so that it doesn't overwrite the column boundary
        wxDCClipper clipper(dc, x, HEADER_OFFSET_Y, cw, h - 4 );
        dc.DrawText (column.GetText(), text_x, HEADER_OFFSET_Y + EXTRA_HEIGHT );

        // next column
        x += wCol;
    }

    int more_w = m_owner->GetSize().x - x - HEADER_OFFSET_X;
    if (more_w > 0) {
#if !wxCHECK_VERSION(2, 7, 0)
        DoDrawRect (&dc, x, HEADER_OFFSET_Y, more_w, h-2 );
#else
        wxRect rect (x, HEADER_OFFSET_Y, more_w, h-2);
        wxRendererNative::GetDefault().DrawHeaderButton (this, dc, rect);
#endif
    }

}

void wxTreeListHeaderWindow::DrawCurrent()
{
    int x1 = m_currentX;
    int y1 = 0;
    ClientToScreen (&x1, &y1);

    int x2 = m_currentX-1;
#ifdef __WXMSW__
    ++x2; // but why ????
#endif
    int y2 = 0;
    m_owner->GetClientSize( NULL, &y2 );
    m_owner->ClientToScreen( &x2, &y2 );

    wxScreenDC dc;
    dc.SetLogicalFunction (wxINVERT);
    dc.SetPen (wxPen (*wxBLACK, 2, wxSOLID));
    dc.SetBrush (*wxTRANSPARENT_BRUSH);

    AdjustDC(dc);
    dc.DrawLine (x1, y1, x2, y2);
    dc.SetLogicalFunction (wxCOPY);
    dc.SetPen (wxNullPen);
    dc.SetBrush (wxNullBrush);
}

void wxTreeListHeaderWindow::OnMouse (wxMouseEvent &event) {

    // we want to work with logical coords
    int x;
    m_owner->CalcUnscrolledPosition(event.GetX(), 0, &x, NULL);
    int y = event.GetY();

    if (m_isDragging) {

        SendListEvent (wxEVT_COMMAND_LIST_COL_DRAGGING, event.GetPosition());

        // we don't draw the line beyond our window, but we allow dragging it
        // there
        int w = 0;
        GetClientSize( &w, NULL );
        m_owner->CalcUnscrolledPosition(w, 0, &w, NULL);
        w -= 6;

        // erase the line if it was drawn
        if (m_currentX < w) DrawCurrent();

        if (event.ButtonUp()) {
            m_isDragging = false;
            if (HasCapture()) ReleaseMouse();
            m_dirty = true;
            SetColumnWidth (m_column, m_currentX - m_minX);
            Refresh();
            SendListEvent (wxEVT_COMMAND_LIST_COL_END_DRAG, event.GetPosition());
        }else{
            m_currentX = wxMax (m_minX + 7, x);

            // draw in the new location
            if (m_currentX < w) DrawCurrent();
        }

    }else{ // not dragging

        m_minX = 0;
        bool hit_border = false;

        // end of the current column
        int xpos = 0;

        // find the column where this event occured
        int countCol = GetColumnCount();
        for (int column = 0; column < countCol; column++) {
            if (!IsColumnShown (column)) continue; // do next if not shown

            xpos += GetColumnWidth (column);
            m_column = column;
            if ((abs (x-xpos) < 3) && (y < 22)) {
                // near the column border
                hit_border = true;
                break;
            }

            if (x < xpos) {
                // inside the column
                break;
            }

            m_minX = xpos;
        }

        if (event.LeftDown() || event.RightUp()) {
            if (hit_border && event.LeftDown()) {
                m_isDragging = true;
                CaptureMouse();
                m_currentX = x;
                DrawCurrent();
                SendListEvent (wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, event.GetPosition());
            }else{ // click on a column
                wxEventType evt = event.LeftDown()? wxEVT_COMMAND_LIST_COL_CLICK:
                                                    wxEVT_COMMAND_LIST_COL_RIGHT_CLICK;
                SendListEvent (evt, event.GetPosition());
            }
        }else if (event.LeftDClick() && hit_border) {
            SetColumnWidth (m_column, m_owner->GetBestColumnWidth (m_column));
            Refresh();

        }else if (event.Moving()) {
            bool setCursor;
            if (hit_border) {
                setCursor = m_currentCursor == wxSTANDARD_CURSOR;
                m_currentCursor = m_resizeCursor;
            }else{
                setCursor = m_currentCursor != wxSTANDARD_CURSOR;
                m_currentCursor = wxSTANDARD_CURSOR;
            }
            if (setCursor) SetCursor (*m_currentCursor);
        }

    }
}

void wxTreeListHeaderWindow::OnSetFocus (wxFocusEvent &WXUNUSED(event)) {
    m_owner->SetFocus();
}

void wxTreeListHeaderWindow::SendListEvent (wxEventType type, wxPoint pos) {
    wxWindow *parent = GetParent();
    wxListEvent le (type, parent->GetId());
    le.SetEventObject (parent);
    le.m_pointDrag = pos;

    // the position should be relative to the parent window, not
    // this one for compatibility with MSW and common sense: the
    // user code doesn't know anything at all about this header
    // window, so why should it get positions relative to it?
    le.m_pointDrag.y -= GetSize().y;
    le.m_col = m_column;
    parent->GetEventHandler()->ProcessEvent (le);
}

void wxTreeListHeaderWindow::AddColumn (const wxTreeListColumnInfo& colInfo) {
    m_columns.Add (colInfo);
    m_total_col_width += colInfo.GetWidth();
    m_owner->AdjustMyScrollbars();
    m_owner->m_dirty = true;
}

void wxTreeListHeaderWindow::SetColumnWidth (int column, int width) {
    wxCHECK_RET ((column >= 0) && (column < GetColumnCount()), _T("Invalid column"));
    m_total_col_width -= m_columns[column].GetWidth();
    m_columns[column].SetWidth(width);
    m_total_col_width += width;
    m_owner->AdjustMyScrollbars();
    m_owner->m_dirty = true;
}

void wxTreeListHeaderWindow::InsertColumn (int before, const wxTreeListColumnInfo& colInfo) {
    wxCHECK_RET ((before >= 0) && (before < GetColumnCount()), _T("Invalid column"));
    m_columns.Insert (colInfo, before);
    m_total_col_width += colInfo.GetWidth();
    m_owner->AdjustMyScrollbars();
    m_owner->m_dirty = true;
}

void wxTreeListHeaderWindow::RemoveColumn (int column) {
    wxCHECK_RET ((column >= 0) && (column < GetColumnCount()), _T("Invalid column"));
    m_total_col_width -= m_columns[column].GetWidth();
    m_columns.RemoveAt (column);
    m_owner->AdjustMyScrollbars();
    m_owner->m_dirty = true;
}

void wxTreeListHeaderWindow::SetColumn (int column, const wxTreeListColumnInfo& info) {
    wxCHECK_RET ((column >= 0) && (column < GetColumnCount()), _T("Invalid column"));
    int w = m_columns[column].GetWidth();
    m_columns[column] = info;
    if (w != info.GetWidth()) {
        m_total_col_width += info.GetWidth() - w;
        m_owner->AdjustMyScrollbars();
    }
    m_owner->m_dirty = true;
}

// ---------------------------------------------------------------------------
// wxTreeListItem
// ---------------------------------------------------------------------------

wxTreeListItem::wxTreeListItem (wxTreeListMainWindow *owner,
                                wxTreeListItem *parent,
                                const wxArrayString& text,
                                int image, int selImage,
                                wxTreeItemData *data)
              : m_text (text) {

    m_images[wxTreeItemIcon_Normal] = image;
    m_images[wxTreeItemIcon_Selected] = selImage;
    m_images[wxTreeItemIcon_Expanded] = NO_IMAGE;
    m_images[wxTreeItemIcon_SelectedExpanded] = NO_IMAGE;

    m_data = data;
    m_x = 0;
    m_y = 0;
    m_text_x = 0;

    m_isCollapsed = true;
    m_hasHilight = false;
    m_hasPlus = false;
    m_isBold = false;

    m_owner = owner;
    m_parent = parent;

    m_attr = (wxTreeItemAttr *)NULL;
    m_ownsAttr = false;

    // We don't know the height here yet.
    m_width = 0;
    m_height = 0;
}

wxTreeListItem::~wxTreeListItem() {
    delete m_data;
    if (m_ownsAttr) delete m_attr;

    wxASSERT_MSG( m_children.IsEmpty(), _T("please call DeleteChildren() before destructor"));
}

void wxTreeListItem::DeleteChildren (wxTreeListMainWindow *tree) {
    size_t count = m_children.Count();
    for (size_t n = 0; n < count; n++) {
        wxTreeListItem *child = m_children[n];
        if (tree) {
            tree->SendDeleteEvent (child);
            if (tree->m_selectItem == child) tree->m_selectItem = (wxTreeListItem*)NULL;
        }
        child->DeleteChildren (tree);
        delete child;
    }
    m_children.Empty();
}

void wxTreeListItem::SetText (const wxString &text) {
    if (m_text.GetCount() > 0) {
        m_text[0] = text;
    }else{
        m_text.Add (text);
    }
}

size_t wxTreeListItem::GetChildrenCount (bool recursively) const {
    size_t count = m_children.Count();
    if (!recursively) return count;

    size_t total = count;
    for (size_t n = 0; n < count; ++n) {
        total += m_children[n]->GetChildrenCount();
    }
    return total;
}

void wxTreeListItem::GetSize (int &x, int &y, const wxTreeListMainWindow *theButton) {
    int bottomY = m_y + theButton->GetLineHeight (this);
    if (y < bottomY) y = bottomY;
    int width = m_x +  m_width;
    if ( x < width ) x = width;

    if (IsExpanded()) {
        size_t count = m_children.Count();
        for (size_t n = 0; n < count; ++n ) {
            m_children[n]->GetSize (x, y, theButton);
        }
    }
}

wxTreeListItem *wxTreeListItem::HitTest (const wxPoint& point,
                                         const wxTreeListMainWindow *theCtrl,
                                         int &flags, int& column, int level) {

    // for a hidden root node, don't evaluate it, but do evaluate children
    if (!theCtrl->HasFlag(wxTR_HIDE_ROOT) || (level > 0)) {

        // reset any previous hit infos
        flags = 0;
        column = -1;
        wxTreeListHeaderWindow* header_win = theCtrl->m_owner->GetHeaderWindow();

        // check for right of all columns (outside)
        if (point.x > header_win->GetWidth()) return (wxTreeListItem*) NULL;

        // evaluate if y-pos is okay
        int h = theCtrl->GetLineHeight (this);
        if ((point.y >= m_y) && (point.y <= m_y + h)) {

            int maincol = theCtrl->GetMainColumn();

            // check for above/below middle
            int y_mid = m_y + h/2;
            if (point.y < y_mid) {
                flags |= wxTREE_HITTEST_ONITEMUPPERPART;
            }else{
                flags |= wxTREE_HITTEST_ONITEMLOWERPART;
            }

            // check for button hit
            if (HasPlus() && theCtrl->HasButtons()) {
                int bntX = m_x - theCtrl->m_btnWidth2;
                int bntY = y_mid - theCtrl->m_btnHeight2;
                if ((point.x >= bntX) && (point.x <= (bntX + theCtrl->m_btnWidth)) &&
                    (point.y >= bntY) && (point.y <= (bntY + theCtrl->m_btnHeight))) {
                    flags |= wxTREE_HITTEST_ONITEMBUTTON;
                    column = maincol;
                    return this;
                }
            }

            // check for image hit
            if (theCtrl->m_imgWidth > 0) {
                int imgX = m_text_x - theCtrl->m_imgWidth - MARGIN;
                int imgY = y_mid - theCtrl->m_imgHeight2;
                if ((point.x >= imgX) && (point.x <= (imgX + theCtrl->m_imgWidth)) &&
                    (point.y >= imgY) && (point.y <= (imgY + theCtrl->m_imgHeight))) {
                    flags |= wxTREE_HITTEST_ONITEMICON;
                    column = maincol;
                    return this;
                }
            }

            // check for label hit
            if ((point.x >= m_text_x) && (point.x <= (m_text_x + m_width))) {
                flags |= wxTREE_HITTEST_ONITEMLABEL;
                column = maincol;
                return this;
            }

            // check for indent hit after button and image hit
            if (point.x < m_x) {
                flags |= wxTREE_HITTEST_ONITEMINDENT;
                column = -1; // considered not belonging to main column
                return this;
            }

            // check for right of label
            int end = 0;
            for (int i = 0; i <= maincol; ++i) end += header_win->GetColumnWidth (i);
            if ((point.x > (m_text_x + m_width)) && (point.x <= end)) {
                flags |= wxTREE_HITTEST_ONITEMRIGHT;
                column = -1; // considered not belonging to main column
                return this;
            }

            // else check for each column except main
            int x = 0;
            for (int j = 0; j < theCtrl->GetColumnCount(); ++j) {
                if (!header_win->IsColumnShown(j)) continue;
                int w = header_win->GetColumnWidth (j);
                if ((j != maincol) && (point.x >= x && point.x < x+w)) {
                    flags |= wxTREE_HITTEST_ONITEMCOLUMN;
                    column = j;
                    return this;
                }
                x += w;
            }

            // no special flag or column found
            return this;

        }

        // if children not expanded, return no item
        if (!IsExpanded()) return (wxTreeListItem*) NULL;
    }

    // in any case evaluate children
    wxTreeListItem *child;
    size_t count = m_children.Count();
    for (size_t n = 0; n < count; n++) {
        child = m_children[n]->HitTest (point, theCtrl, flags, column, level+1);
        if (child) return child;
    }

    // not found
    return (wxTreeListItem*) NULL;
}

int wxTreeListItem::GetCurrentImage() const {
    int image = NO_IMAGE;
    if (IsExpanded()) {
        if (IsSelected()) {
            image = GetImage (wxTreeItemIcon_SelectedExpanded);
        }else{
            image = GetImage (wxTreeItemIcon_Expanded);
        }
    }else{ // not expanded
        if (IsSelected()) {
            image = GetImage (wxTreeItemIcon_Selected);
        }else{
            image = GetImage (wxTreeItemIcon_Normal);
        }
    }

    // maybe it doesn't have the specific image, try the default one instead
    if (image == NO_IMAGE) image = GetImage();

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

void wxTreeListMainWindow::Init() {

    m_rootItem = (wxTreeListItem*)NULL;
    m_curItem = (wxTreeListItem*)NULL;
    m_shiftItem = (wxTreeListItem*)NULL;
    m_editItem = (wxTreeListItem*)NULL;
    m_selectItem = (wxTreeListItem*)NULL;

    m_curColumn = -1; // no current column

    m_hasFocus = false;
    m_dirty = false;

    m_lineHeight = LINEHEIGHT;
    m_indent = MININDENT; // min. indent
    m_linespacing = 4;

#if !wxCHECK_VERSION(2, 5, 0)
    m_hilightBrush = new wxBrush (wxSystemSettings::GetSystemColour (wxSYS_COLOUR_HIGHLIGHT), wxSOLID);
    m_hilightUnfocusedBrush = new wxBrush (wxSystemSettings::GetSystemColour (wxSYS_COLOUR_BTNSHADOW), wxSOLID);
#else
    m_hilightBrush = new wxBrush (wxSystemSettings::GetColour (wxSYS_COLOUR_HIGHLIGHT), wxSOLID);
    m_hilightUnfocusedBrush = new wxBrush (wxSystemSettings::GetColour (wxSYS_COLOUR_BTNSHADOW), wxSOLID);
#endif

    m_imageListNormal = (wxImageList *) NULL;
    m_imageListButtons = (wxImageList *) NULL;
    m_imageListState = (wxImageList *) NULL;
    m_ownsImageListNormal = m_ownsImageListButtons =
    m_ownsImageListState = false;

    m_imgWidth = 0, m_imgWidth2 = 0;
    m_imgHeight = 0, m_imgHeight2 = 0;
    m_btnWidth = 0, m_btnWidth2 = 0;
    m_btnHeight = 0, m_btnHeight2 = 0;

    m_dragCount = 0;
    m_isDragging = false;
    m_dragTimer = new wxTimer (this, -1);
    m_dragItem = (wxTreeListItem*)NULL;

    m_renameTimer = new wxTreeListRenameTimer (this);
    m_lastOnSame = false;
    m_left_down_selection = false;

    m_findTimer = new wxTimer (this, -1);

#if defined( __WXMAC__ ) && defined(__WXMAC_CARBON__)
    m_normalFont.MacCreateThemeFont (kThemeViewsFont);
#else
    m_normalFont = wxSystemSettings::GetFont (wxSYS_DEFAULT_GUI_FONT);
#endif
    m_boldFont = wxFont( m_normalFont.GetPointSize(),
                         m_normalFont.GetFamily(),
                         m_normalFont.GetStyle(),
                         wxBOLD,
                         m_normalFont.GetUnderlined(),
                         m_normalFont.GetFaceName(),
                         m_normalFont.GetEncoding());
}

bool wxTreeListMainWindow::Create (wxTreeListCtrl *parent,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxValidator &validator,
                                   const wxString& name) {

#ifdef __WXMAC__
    if (style & wxTR_HAS_BUTTONS) style |= wxTR_MAC_BUTTONS;
    if (style & wxTR_HAS_BUTTONS) style &= ~wxTR_HAS_BUTTONS;
    style &= ~wxTR_LINES_AT_ROOT;
    style |= wxTR_NO_LINES;

    int major,minor;
    wxGetOsVersion( &major, &minor );
    if (major < 10) style |= wxTR_ROW_LINES;
#endif

    wxScrolledWindow::Create (parent, id, pos, size, style|wxHSCROLL|wxVSCROLL, name);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

#if !wxCHECK_VERSION(2, 5, 0)
    SetBackgroundColour (wxSystemSettings::GetSystemColour (wxSYS_COLOUR_LISTBOX));
#else
    SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_LISTBOX));
#endif

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
//?    m_dottedPen = wxPen( *wxGREY_PEN, 1, wxDOT );  // too slow under XFree86
    m_dottedPen = wxPen( _T("grey"), 0, 0 ); // Bitmap based pen is not supported by GTK!
#endif

    m_owner = parent;
    m_main_column = 0;

    return true;
}

wxTreeListMainWindow::~wxTreeListMainWindow() {
    delete m_hilightBrush;
    delete m_hilightUnfocusedBrush;

    delete m_dragTimer;
    delete m_renameTimer;
    delete m_findTimer;
    if (m_ownsImageListNormal) delete m_imageListNormal;
    if (m_ownsImageListState) delete m_imageListState;
    if (m_ownsImageListButtons) delete m_imageListButtons;

    DeleteRoot();
}


//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------

size_t wxTreeListMainWindow::GetCount() const {
    return m_rootItem == NULL? 0: m_rootItem->GetChildrenCount();
}

void wxTreeListMainWindow::SetIndent (unsigned int indent) {
    m_indent = wxMax ((unsigned)MININDENT, indent);
    m_dirty = true;
}

void wxTreeListMainWindow::SetLineSpacing (unsigned int spacing) {
    m_linespacing = spacing;
    m_dirty = true;
    CalculateLineHeight();
}

size_t wxTreeListMainWindow::GetChildrenCount (const wxTreeItemId& item,
                                               bool recursively) {
    wxCHECK_MSG (item.IsOk(), 0u, _T("invalid tree item"));
    return ((wxTreeListItem*)item.m_pItem)->GetChildrenCount (recursively);
}

void wxTreeListMainWindow::SetWindowStyle (const long styles) {
    // right now, just sets the styles.  Eventually, we may
    // want to update the inherited styles, but right now
    // none of the parents has updatable styles
    m_windowStyle = styles;
    m_dirty = true;
}

//-----------------------------------------------------------------------------
// functions to work with tree items
//-----------------------------------------------------------------------------

int wxTreeListMainWindow::GetItemImage (const wxTreeItemId& item, int column,
                                        wxTreeItemIcon which) const {
    wxCHECK_MSG (item.IsOk(), -1, _T("invalid tree item"));
    return ((wxTreeListItem*) item.m_pItem)->GetImage (column, which);
}

wxTreeItemData *wxTreeListMainWindow::GetItemData (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), NULL, _T("invalid tree item"));
    return ((wxTreeListItem*) item.m_pItem)->GetData();
}

bool wxTreeListMainWindow::GetItemBold (const wxTreeItemId& item) const {
    wxCHECK_MSG(item.IsOk(), false, _T("invalid tree item"));
    return ((wxTreeListItem *)item.m_pItem)->IsBold();
}

wxColour wxTreeListMainWindow::GetItemTextColour (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), wxNullColour, _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    return pItem->Attr().GetTextColour();
}

wxColour wxTreeListMainWindow::GetItemBackgroundColour (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), wxNullColour, _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    return pItem->Attr().GetBackgroundColour();
}

wxFont wxTreeListMainWindow::GetItemFont (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), wxNullFont, _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    return pItem->Attr().GetFont();
}

void wxTreeListMainWindow::SetItemImage (const wxTreeItemId& item, int column,
                                         int image, wxTreeItemIcon which) {
    wxCHECK_RET (item.IsOk(), _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->SetImage (column, image, which);
    wxClientDC dc (this);
    CalculateSize (pItem, dc);
    RefreshLine (pItem);
}

void wxTreeListMainWindow::SetItemData (const wxTreeItemId& item,
                                        wxTreeItemData *data) {
    wxCHECK_RET (item.IsOk(), _T("invalid tree item"));
    ((wxTreeListItem*) item.m_pItem)->SetData(data);
}

void wxTreeListMainWindow::SetItemHasChildren (const wxTreeItemId& item,
                                               bool has) {
    wxCHECK_RET (item.IsOk(), _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->SetHasPlus (has);
    RefreshLine (pItem);
}

void wxTreeListMainWindow::SetItemBold (const wxTreeItemId& item, bool bold) {
    wxCHECK_RET (item.IsOk(), _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    if (pItem->IsBold() != bold) { // avoid redrawing if no real change
        pItem->SetBold (bold);
        RefreshLine (pItem);
    }
}

void wxTreeListMainWindow::SetItemTextColour (const wxTreeItemId& item,
                                              const wxColour& colour) {
    wxCHECK_RET (item.IsOk(), _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->Attr().SetTextColour (colour);
    RefreshLine (pItem);
}

void wxTreeListMainWindow::SetItemBackgroundColour (const wxTreeItemId& item,
                                                    const wxColour& colour) {
    wxCHECK_RET (item.IsOk(), _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->Attr().SetBackgroundColour (colour);
    RefreshLine (pItem);
}

void wxTreeListMainWindow::SetItemFont (const wxTreeItemId& item,
                                        const wxFont& font) {
    wxCHECK_RET (item.IsOk(), _T("invalid tree item"));
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    pItem->Attr().SetFont (font);
    RefreshLine (pItem);
}

bool wxTreeListMainWindow::SetFont (const wxFont &font) {
    wxScrolledWindow::SetFont (font);
    m_normalFont = font;
    m_boldFont = wxFont (m_normalFont.GetPointSize(),
                         m_normalFont.GetFamily(),
                         m_normalFont.GetStyle(),
                         wxBOLD,
                         m_normalFont.GetUnderlined(),
                         m_normalFont.GetFaceName());
    CalculateLineHeight();
    return true;
}


// ----------------------------------------------------------------------------
// item status inquiries
// ----------------------------------------------------------------------------

bool wxTreeListMainWindow::IsVisible (const wxTreeItemId& item, bool fullRow) const {
    wxCHECK_MSG (item.IsOk(), false, _T("invalid tree item"));

    // An item is only visible if it's not a descendant of a collapsed item
    wxTreeListItem *pItem = (wxTreeListItem*) item.m_pItem;
    wxTreeListItem* parent = pItem->GetItemParent();
    while (parent) {
        if (parent == m_rootItem && HasFlag(wxTR_HIDE_ROOT)) break;
        if (!parent->IsExpanded()) return false;
        parent = parent->GetItemParent();
    }

    wxSize clientSize = GetClientSize();
    wxRect rect;
    if ((!GetBoundingRect (item, rect)) ||
        ((!fullRow && rect.GetWidth() == 0) || rect.GetHeight() == 0) ||
        (rect.GetBottom() < 0 || rect.GetTop() > clientSize.y) ||
        (!fullRow && (rect.GetRight() < 0 || rect.GetLeft() > clientSize.x))) return false;

    return true;
}

bool wxTreeListMainWindow::HasChildren (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), false, _T("invalid tree item"));

    // consider that the item does have children if it has the "+" button: it
    // might not have them (if it had never been expanded yet) but then it
    // could have them as well and it's better to err on this side rather than
    // disabling some operations which are restricted to the items with
    // children for an item which does have them
    return ((wxTreeListItem*) item.m_pItem)->HasPlus();
}

bool wxTreeListMainWindow::IsExpanded (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), false, _T("invalid tree item"));
    return ((wxTreeListItem*) item.m_pItem)->IsExpanded();
}

bool wxTreeListMainWindow::IsSelected (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), false, _T("invalid tree item"));
    return ((wxTreeListItem*) item.m_pItem)->IsSelected();
}

bool wxTreeListMainWindow::IsBold (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), false, _T("invalid tree item"));
    return ((wxTreeListItem*) item.m_pItem)->IsBold();
}

// ----------------------------------------------------------------------------
// navigation
// ----------------------------------------------------------------------------

wxTreeItemId wxTreeListMainWindow::GetItemParent (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    return ((wxTreeListItem*) item.m_pItem)->GetItemParent();
}

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListMainWindow::GetFirstChild (const wxTreeItemId& item,
                                                  long& cookie) const {
#else
wxTreeItemId wxTreeListMainWindow::GetFirstChild (const wxTreeItemId& item,
                                                  wxTreeItemIdValue& cookie) const {
#endif
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    wxArrayTreeListItems& children = ((wxTreeListItem*) item.m_pItem)->GetChildren();
    cookie = 0;
    return (!children.IsEmpty())? wxTreeItemId(children.Item(0)): wxTreeItemId();
}

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListMainWindow::GetNextChild (const wxTreeItemId& item,
                                                 long& cookie) const {
#else
wxTreeItemId wxTreeListMainWindow::GetNextChild (const wxTreeItemId& item,
                                                 wxTreeItemIdValue& cookie) const {
#endif
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    wxArrayTreeListItems& children = ((wxTreeListItem*) item.m_pItem)->GetChildren();
    // it's ok to cast cookie to long, we never have indices which overflow "void*"
    long *pIndex = ((long*)&cookie);
    return ((*pIndex)+1 < (long)children.Count())? wxTreeItemId(children.Item(++(*pIndex))): wxTreeItemId();
}

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListMainWindow::GetPrevChild (const wxTreeItemId& item,
                                                 long& cookie) const {
#else
wxTreeItemId wxTreeListMainWindow::GetPrevChild (const wxTreeItemId& item,
                                                 wxTreeItemIdValue& cookie) const {
#endif
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    wxArrayTreeListItems& children = ((wxTreeListItem*) item.m_pItem)->GetChildren();
    // it's ok to cast cookie to long, we never have indices which overflow "void*"
    long *pIndex = (long*)&cookie;
    return ((*pIndex)-1 >= 0)? wxTreeItemId(children.Item(--(*pIndex))): wxTreeItemId();
}

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListMainWindow::GetLastChild (const wxTreeItemId& item,
                                                 long& cookie) const {
#else
wxTreeItemId wxTreeListMainWindow::GetLastChild (const wxTreeItemId& item,
                                                 wxTreeItemIdValue& cookie) const {
#endif
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    wxArrayTreeListItems& children = ((wxTreeListItem*) item.m_pItem)->GetChildren();
    // it's ok to cast cookie to long, we never have indices which overflow "void*"
    long *pIndex = ((long*)&cookie);
    (*pIndex) = children.Count();
    return (!children.IsEmpty())? wxTreeItemId(children.Last()): wxTreeItemId();
}

wxTreeItemId wxTreeListMainWindow::GetNextSibling (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));

    // get parent
    wxTreeListItem *i = (wxTreeListItem*) item.m_pItem;
    wxTreeListItem *parent = i->GetItemParent();
    if (!parent) return wxTreeItemId(); // root item doesn't have any siblings

    // get index
    wxArrayTreeListItems& siblings = parent->GetChildren();
    size_t index = siblings.Index (i);
    wxASSERT (index != (size_t)wxNOT_FOUND); // I'm not a child of my parent?
    return (index < siblings.Count()-1)? wxTreeItemId(siblings[index+1]): wxTreeItemId();
}

wxTreeItemId wxTreeListMainWindow::GetPrevSibling (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));

    // get parent
    wxTreeListItem *i = (wxTreeListItem*) item.m_pItem;
    wxTreeListItem *parent = i->GetItemParent();
    if (!parent) return wxTreeItemId(); // root item doesn't have any siblings

    // get index
    wxArrayTreeListItems& siblings = parent->GetChildren();
    size_t index = siblings.Index(i);
    wxASSERT (index != (size_t)wxNOT_FOUND); // I'm not a child of my parent?
    return (index >= 1)? wxTreeItemId(siblings[index-1]): wxTreeItemId();
}

// Only for internal use right now, but should probably be public
wxTreeItemId wxTreeListMainWindow::GetNext (const wxTreeItemId& item, bool fulltree) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));

    // if there are any children, return first child
    if (fulltree || ((wxTreeListItem*)item.m_pItem)->IsExpanded()) {
        wxArrayTreeListItems& children = ((wxTreeListItem*)item.m_pItem)->GetChildren();
        if (children.GetCount() > 0) return children.Item (0);
    }

    // get sibling of this item or of the ancestors instead
    wxTreeItemId next;
    wxTreeItemId parent = item;
    do {
        next = GetNextSibling (parent);
        parent = GetItemParent (parent);
    } while (!next.IsOk() && parent.IsOk());
    return next;
}

// Only for internal use right now, but should probably be public
wxTreeItemId wxTreeListMainWindow::GetPrev (const wxTreeItemId& item, bool fulltree) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));

    // if there are any children, return last child
    if (fulltree || ((wxTreeListItem*)item.m_pItem)->IsExpanded()) {
        wxArrayTreeListItems& children = ((wxTreeListItem*)item.m_pItem)->GetChildren();
        if (children.GetCount() > 0) return children.Item (children.GetCount()-1);
    }

    // get sibling of this item or of the ancestors instead
    wxTreeItemId next;
    wxTreeItemId parent = item;
    do {
        next = GetPrevSibling (parent);
        parent = GetItemParent (parent);
    } while (!next.IsOk() && parent.IsOk());
    return next;
}

wxTreeItemId wxTreeListMainWindow::GetFirstExpandedItem() const {
    return GetNextExpanded (GetRootItem());
}

wxTreeItemId wxTreeListMainWindow::GetNextExpanded (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    return GetNext (item, false);
}

wxTreeItemId wxTreeListMainWindow::GetPrevExpanded (const wxTreeItemId& item) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    return GetPrev (item, false);
}

wxTreeItemId wxTreeListMainWindow::GetFirstVisibleItem (bool fullRow) const {
    return GetNextVisible (GetRootItem(), fullRow);
}

wxTreeItemId wxTreeListMainWindow::GetNextVisible (const wxTreeItemId& item, bool fullRow) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    wxTreeItemId id = GetNext (item, false);
    while (id.IsOk()) {
        if (IsVisible (id, fullRow)) return id;
        id = GetNext (id, false);
    }
    return wxTreeItemId();
}

wxTreeItemId wxTreeListMainWindow::GetPrevVisible (const wxTreeItemId& item, bool fullRow) const {
    wxCHECK_MSG (item.IsOk(), wxTreeItemId(), _T("invalid tree item"));
    wxTreeItemId id = GetPrev (item, true);
    while (id.IsOk()) {
        if (IsVisible (id, fullRow)) return id;
        id = GetPrev(id, true);
    }
    return wxTreeItemId();
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

wxTreeItemId wxTreeListMainWindow::DoInsertItem (const wxTreeItemId& parentId,
                                                 size_t previous,
                                                 const wxString& text,
                                                 int image, int selImage,
                                                 wxTreeItemData *data) {
    wxTreeListItem *parent = (wxTreeListItem*)parentId.m_pItem;
    wxCHECK_MSG (parent, wxTreeItemId(), _T("item must have a parent, at least root!") );
    m_dirty = true; // do this first so stuff below doesn't cause flicker

    wxArrayString arr;
    arr.Alloc (GetColumnCount());
    for (int i = 0; i < (int)GetColumnCount(); ++i) arr.Add (wxEmptyString);
    arr[m_main_column] = text;
    wxTreeListItem *item = new wxTreeListItem (this, parent, arr, image, selImage, data);
    if (data != NULL) {
#if !wxCHECK_VERSION(2, 5, 0)
        data->SetId ((long)item);
#else
        data->SetId (item);
#endif
    }
    parent->Insert (item, previous);

    return item;
}

wxTreeItemId wxTreeListMainWindow::AddRoot (const wxString& text,
                                            int image, int selImage,
                                            wxTreeItemData *data) {
    wxCHECK_MSG(!m_rootItem, wxTreeItemId(), _T("tree can have only one root"));
    wxCHECK_MSG(GetColumnCount(), wxTreeItemId(), _T("Add column(s) before adding the root item"));
    m_dirty = true; // do this first so stuff below doesn't cause flicker

    wxArrayString arr;
    arr.Alloc (GetColumnCount());
    for (int i = 0; i < (int)GetColumnCount(); ++i) arr.Add (wxEmptyString);
    arr[m_main_column] = text;
    m_rootItem = new wxTreeListItem (this, (wxTreeListItem *)NULL, arr, image, selImage, data);
    if (data != NULL) {
#if !wxCHECK_VERSION(2, 5, 0)
        data->SetId((long)m_rootItem);
#else
        data->SetId(m_rootItem);
#endif
    }
    if (HasFlag(wxTR_HIDE_ROOT)) {
        // if we will hide the root, make sure children are visible
        m_rootItem->SetHasPlus();
        m_rootItem->Expand();
#if !wxCHECK_VERSION(2, 5, 0)
        long cookie = 0;
#else
        wxTreeItemIdValue cookie = 0;
#endif
        m_curItem = (wxTreeListItem*)GetFirstChild (m_rootItem, cookie).m_pItem;
    }
    return m_rootItem;
}

wxTreeItemId wxTreeListMainWindow::PrependItem (const wxTreeItemId& parent,
                                                const wxString& text,
                                                int image, int selImage,
                                                wxTreeItemData *data) {
    return DoInsertItem (parent, 0u, text, image, selImage, data);
}

wxTreeItemId wxTreeListMainWindow::InsertItem (const wxTreeItemId& parentId,
                                               const wxTreeItemId& idPrevious,
                                               const wxString& text,
                                               int image, int selImage,
                                               wxTreeItemData *data) {
    wxTreeListItem *parent = (wxTreeListItem*)parentId.m_pItem;
    wxCHECK_MSG (parent, wxTreeItemId(), _T("item must have a parent, at least root!") );

    int index = parent->GetChildren().Index((wxTreeListItem*) idPrevious.m_pItem);
    wxASSERT_MSG( index != wxNOT_FOUND,
                  _T("previous item in wxTreeListMainWindow::InsertItem() is not a sibling") );

    return DoInsertItem (parentId, ++index, text, image, selImage, data);
}

wxTreeItemId wxTreeListMainWindow::InsertItem (const wxTreeItemId& parentId,
                                               size_t before,
                                               const wxString& text,
                                               int image, int selImage,
                                               wxTreeItemData *data) {
    wxTreeListItem *parent = (wxTreeListItem*)parentId.m_pItem;
    wxCHECK_MSG (parent, wxTreeItemId(), _T("item must have a parent, at least root!") );

    return DoInsertItem (parentId, before, text, image, selImage, data);
}

wxTreeItemId wxTreeListMainWindow::AppendItem (const wxTreeItemId& parentId,
                                               const wxString& text,
                                               int image, int selImage,
                                               wxTreeItemData *data) {
    wxTreeListItem *parent = (wxTreeListItem*) parentId.m_pItem;
    wxCHECK_MSG (parent, wxTreeItemId(), _T("item must have a parent, at least root!") );

    return DoInsertItem (parent, parent->GetChildren().Count(), text, image, selImage, data);
}

void wxTreeListMainWindow::SendDeleteEvent (wxTreeListItem *item) {
    // send event to user code
    wxTreeEvent event (wxEVT_COMMAND_TREE_DELETE_ITEM, m_owner->GetId());
#if !wxCHECK_VERSION(2, 5, 0)
    event.SetItem ((long)item);
#else
    event.SetItem (item);
#endif
    event.SetEventObject (m_owner);
    m_owner->ProcessEvent (event);
}

void wxTreeListMainWindow::Delete (const wxTreeItemId& itemId) {
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;
    wxCHECK_RET (item != m_rootItem, _T("invalid item, root may not be deleted this way!"));
    m_dirty = true; // do this first so stuff below doesn't cause flicker

    // don't stay with invalid m_shiftItem or we will crash in the next call to OnChar()
    bool changeKeyCurrent = false;
    wxTreeListItem *itemKey = m_shiftItem;
    while (itemKey) {
        if (itemKey == item) { // m_shiftItem is a descendant of the item being deleted
            changeKeyCurrent = true;
            break;
        }
        itemKey = itemKey->GetItemParent();
    }

    wxTreeListItem *parent = item->GetItemParent();
    if (parent) {
        parent->GetChildren().Remove (item);  // remove by value
    }
    if (changeKeyCurrent)  m_shiftItem = parent;

    SendDeleteEvent (item);
    if (m_selectItem == item) m_selectItem = (wxTreeListItem*)NULL;
    item->DeleteChildren (this);
    delete item;
}

void wxTreeListMainWindow::DeleteChildren (const wxTreeItemId& itemId) {
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;
    m_dirty = true; // do this first so stuff below doesn't cause flicker

    item->DeleteChildren (this);
}

void wxTreeListMainWindow::DeleteRoot() {
    if (m_rootItem) {
        m_dirty = true;
        SendDeleteEvent (m_rootItem);
        m_curItem = (wxTreeListItem*)NULL;
        m_selectItem= (wxTreeListItem*)NULL;
        m_rootItem->DeleteChildren (this);
        delete m_rootItem;
        m_rootItem = NULL;
    }
}

void wxTreeListMainWindow::Expand (const wxTreeItemId& itemId) {
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;
    wxCHECK_RET (item, _T("invalid item in wxTreeListMainWindow::Expand") );

    if (!item->HasPlus() || item->IsExpanded()) return;

    // send event to user code
    wxTreeEvent event (wxEVT_COMMAND_TREE_ITEM_EXPANDING, m_owner->GetId());
#if !wxCHECK_VERSION(2, 5, 0)
    event.SetItem ((long)item);
#else
    event.SetItem (item);
#endif
    event.SetEventObject (m_owner);
    if (m_owner->ProcessEvent (event) && !event.IsAllowed()) return; // expand canceled

    item->Expand();
    m_dirty = true;

    // send event to user code
    event.SetEventType (wxEVT_COMMAND_TREE_ITEM_EXPANDED);
    m_owner->ProcessEvent (event);
}

void wxTreeListMainWindow::ExpandAll (const wxTreeItemId& itemId) {
    Expand (itemId);
    if (!IsExpanded (itemId)) return;
#if !wxCHECK_VERSION(2, 5, 0)
    long cookie;
#else
    wxTreeItemIdValue cookie;
#endif
    wxTreeItemId child = GetFirstChild (itemId, cookie);
    while (child.IsOk()) {
        ExpandAll (child);
        child = GetNextChild (itemId, cookie);
    }
}

void wxTreeListMainWindow::Collapse (const wxTreeItemId& itemId) {
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;
    wxCHECK_RET (item, _T("invalid item in wxTreeListMainWindow::Collapse") );

    if (!item->HasPlus() || !item->IsExpanded()) return;

    // send event to user code
    wxTreeEvent event (wxEVT_COMMAND_TREE_ITEM_COLLAPSING, m_owner->GetId() );
#if !wxCHECK_VERSION(2, 5, 0)
    event.SetItem ((long)item);
#else
    event.SetItem (item);
#endif
    event.SetEventObject (m_owner);
    if (m_owner->ProcessEvent (event) && !event.IsAllowed()) return; // collapse canceled

    item->Collapse();
    m_dirty = true;

    // send event to user code
    event.SetEventType (wxEVT_COMMAND_TREE_ITEM_COLLAPSED);
    ProcessEvent (event);
}

void wxTreeListMainWindow::CollapseAndReset (const wxTreeItemId& item) {
    Collapse (item);
    DeleteChildren (item);
}

void wxTreeListMainWindow::Toggle (const wxTreeItemId& itemId) {
    if (IsExpanded (itemId)) {
        Collapse (itemId);
    }else{
        Expand (itemId);
    }
}

void wxTreeListMainWindow::Unselect() {
    if (m_selectItem) {
        m_selectItem->SetHilight (false);
        RefreshLine (m_selectItem);
        m_selectItem = (wxTreeListItem*)NULL;
    }
}

void wxTreeListMainWindow::UnselectAllChildren (wxTreeListItem *item) {
    if (item->IsSelected()) {
        item->SetHilight (false);
        RefreshLine (item);
        if (item == m_selectItem) m_selectItem = (wxTreeListItem*)NULL;
    }
    if (item->HasChildren()) {
        wxArrayTreeListItems& children = item->GetChildren();
        size_t count = children.Count();
        for (size_t n = 0; n < count; ++n) {
            UnselectAllChildren (children[n]);
        }
    }
}

void wxTreeListMainWindow::UnselectAll() {
    UnselectAllChildren ((wxTreeListItem*)GetRootItem().m_pItem);
}

// Recursive function !
// To stop we must have crt_item<last_item
// Algorithm :
// Tag all next children, when no more children,
// Move to parent (not to tag)
// Keep going... if we found last_item, we stop.
bool wxTreeListMainWindow::TagNextChildren (wxTreeListItem *crt_item,
                                            wxTreeListItem *last_item) {
    wxTreeListItem *parent = crt_item->GetItemParent();

    if (!parent) {// This is root item
        return TagAllChildrenUntilLast (crt_item, last_item);
    }

    wxArrayTreeListItems& children = parent->GetChildren();
    int index = children.Index(crt_item);
    wxASSERT (index != wxNOT_FOUND); // I'm not a child of my parent?

    if ((parent->HasChildren() && parent->IsExpanded()) ||
        ((parent == (wxTreeListItem*)GetRootItem().m_pItem) && HasFlag(wxTR_HIDE_ROOT))) {
        size_t count = children.Count();
        for (size_t n = (index+1); n < count; ++n) {
            if (TagAllChildrenUntilLast (children[n], last_item)) return true;
        }
    }

    return TagNextChildren (parent, last_item);
}

bool wxTreeListMainWindow::TagAllChildrenUntilLast (wxTreeListItem *crt_item,
                                                    wxTreeListItem *last_item) {
    crt_item->SetHilight (true);
    RefreshLine(crt_item);

    if (crt_item==last_item) return true;

    if (crt_item->HasChildren() && crt_item->IsExpanded()) {
        wxArrayTreeListItems& children = crt_item->GetChildren();
        size_t count = children.Count();
        for (size_t n = 0; n < count; ++n) {
            if (TagAllChildrenUntilLast (children[n], last_item)) return true;
        }
    }

    return false;
}

void wxTreeListMainWindow::SelectItem (const wxTreeItemId& itemId,
                                       const wxTreeItemId& lastId,
                                       bool unselect_others) {
    wxCHECK_RET (itemId.IsOk(), _T("invalid tree item") );

    bool is_single = !HasFlag(wxTR_MULTIPLE);
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    // single selection requires unselect others
    if (is_single) unselect_others = true;

    // send event to the user code
    wxTreeEvent event( wxEVT_COMMAND_TREE_SEL_CHANGING, m_owner->GetId() );
#if !wxCHECK_VERSION(2, 5, 0)
    event.SetItem ((long)item);
    event.SetOldItem ((long)m_curItem);
#else
    event.SetItem (item);
    event.SetOldItem (m_curItem);
#endif
    event.SetEventObject (m_owner);
    if (m_owner->GetEventHandler()->ProcessEvent (event) && !event.IsAllowed()) return;

    // unselect all if unselect other items
    bool unselected = false; // see that UnselectAll is done only once
    if (unselect_others) {
        if (is_single) {
            Unselect(); // to speed up thing
        }else{
            UnselectAll();
            unselected = true;
        }
    }

    // select item or item range
    if (lastId.IsOk() && (itemId != lastId)) {

        if (!unselected) UnselectAll();
        wxTreeListItem *last = (wxTreeListItem*) lastId.m_pItem;

        // ensure that the position of the item it calculated in any case
        if (m_dirty) CalculatePositions();

        // select item range according Y-position
        if (last->GetY() < item->GetY()) {
            if (!TagAllChildrenUntilLast (last, item)) {
                TagNextChildren (last, item);
            }
        }else{
            if (!TagAllChildrenUntilLast (item, last)) {
                TagNextChildren (item, last);
            }
        }

    }else{

        // select item according its old selection
        item->SetHilight (!item->IsSelected());
        RefreshLine (item);
        if (unselect_others) {
            m_selectItem = (item->IsSelected())? item: (wxTreeListItem*)NULL;
        }

    }

    // send event to user code
    event.SetEventType(wxEVT_COMMAND_TREE_SEL_CHANGED);
    m_owner->GetEventHandler()->ProcessEvent (event);
}

void wxTreeListMainWindow::SelectAll() {
    wxCHECK_RET (HasFlag(wxTR_MULTIPLE), _T("invalid tree style"));

    // send event to user code
    wxTreeEvent event (wxEVT_COMMAND_TREE_SEL_CHANGING, m_owner->GetId());
    event.SetItem (GetRootItem());
#if !wxCHECK_VERSION(2, 5, 0)
    event.SetOldItem ((long)m_curItem);
#else
    event.SetOldItem (m_curItem);
#endif
    event.SetEventObject (m_owner);
    if (m_owner->GetEventHandler()->ProcessEvent (event) && !event.IsAllowed()) return;

#if !wxCHECK_VERSION(2, 5, 0)
    long cookie = 0;
#else
    wxTreeItemIdValue cookie = 0;
#endif
    wxTreeItemId root = GetRootItem();
    wxTreeListItem *first = (wxTreeListItem *)GetFirstChild (root, cookie).m_pItem;
    wxTreeListItem *last = (wxTreeListItem *)GetLastChild (root, cookie).m_pItem;
    if (!TagAllChildrenUntilLast (first, last)) {
        TagNextChildren (first, last);
    }

    // send event to user code
    event.SetEventType (wxEVT_COMMAND_TREE_SEL_CHANGED);
    m_owner->GetEventHandler()->ProcessEvent (event);
}

void wxTreeListMainWindow::FillArray (wxTreeListItem *item,
                                      wxArrayTreeItemIds &array) const {
    if (item->IsSelected()) array.Add (wxTreeItemId(item));

    if (item->HasChildren()) {
        wxArrayTreeListItems& children = item->GetChildren();
        size_t count = children.GetCount();
        for (size_t n = 0; n < count; ++n) FillArray (children[n], array);
    }
}

size_t wxTreeListMainWindow::GetSelections (wxArrayTreeItemIds &array) const {
    array.Empty();
    wxTreeItemId idRoot = GetRootItem();
    if (idRoot.IsOk()) FillArray ((wxTreeListItem*) idRoot.m_pItem, array);
    return array.Count();
}

void wxTreeListMainWindow::EnsureVisible (const wxTreeItemId& item) {
    if (!item.IsOk()) return; // do nothing if no item

    // first expand all parent branches
    wxTreeListItem *gitem = (wxTreeListItem*) item.m_pItem;
    wxTreeListItem *parent = gitem->GetItemParent();
    while (parent) {
        Expand (parent);
        parent = parent->GetItemParent();
    }

    ScrollTo (item);
    RefreshLine (gitem);
}

void wxTreeListMainWindow::ScrollTo (const wxTreeItemId &item) {
    if (!item.IsOk()) return; // do nothing if no item

    // ensure that the position of the item it calculated in any case
    if (m_dirty) CalculatePositions();

    wxTreeListItem *gitem = (wxTreeListItem*) item.m_pItem;

    // now scroll to the item
    int item_y = gitem->GetY();

    int xUnit, yUnit;
    GetScrollPixelsPerUnit (&xUnit, &yUnit);
    int start_x = 0;
    int start_y = 0;
    GetViewStart (&start_x, &start_y);
    start_y *= yUnit;

    int client_h = 0;
    int client_w = 0;
    GetClientSize (&client_w, &client_h);

    int x = 0;
    int y = 0;
    m_rootItem->GetSize (x, y, this);
    x = m_owner->GetHeaderWindow()->GetWidth();
    y += yUnit + 2; // one more scrollbar unit + 2 pixels
    int x_pos = GetScrollPos( wxHORIZONTAL );

    if (item_y < start_y+3) {
        // going down, item should appear at top
        SetScrollbars (xUnit, yUnit, xUnit ? x/xUnit : 0, yUnit ? y/yUnit : 0, x_pos, yUnit ? item_y/yUnit : 0);
    }else if (item_y+GetLineHeight(gitem) > start_y+client_h) {
        // going up, item should appear at bottom
        item_y += yUnit + 2;
        SetScrollbars (xUnit, yUnit, xUnit ? x/xUnit : 0, yUnit ? y/yUnit : 0, x_pos, yUnit ? (item_y+GetLineHeight(gitem)-client_h)/yUnit : 0 );
    }
}

// FIXME: tree sorting functions are not reentrant and not MT-safe!
static wxTreeListMainWindow *s_treeBeingSorted = NULL;

static int LINKAGEMODE tree_ctrl_compare_func(wxTreeListItem **item1,
                                  wxTreeListItem **item2)
{
    wxCHECK_MSG (s_treeBeingSorted, 0, _T("bug in wxTreeListMainWindow::SortChildren()") );

    return s_treeBeingSorted->OnCompareItems(*item1, *item2);
}

int wxTreeListMainWindow::OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2)
{
    return m_owner->OnCompareItems (item1, item2);
}

void wxTreeListMainWindow::SortChildren (const wxTreeItemId& itemId) {
    wxCHECK_RET (itemId.IsOk(), _T("invalid tree item"));

    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    wxCHECK_RET (!s_treeBeingSorted,
                 _T("wxTreeListMainWindow::SortChildren is not reentrant") );

    wxArrayTreeListItems& children = item->GetChildren();
    if ( children.Count() > 1 ) {
        m_dirty = true;
        s_treeBeingSorted = this;
        children.Sort(tree_ctrl_compare_func);
        s_treeBeingSorted = NULL;
    }
}

wxTreeItemId wxTreeListMainWindow::FindItem (const wxTreeItemId& item, const wxString& str, int mode) {
    wxString itemText;
    // determine start item
    wxTreeItemId next = item;
    if (next.IsOk()) {
        if (mode & wxTL_MODE_NAV_LEVEL) {
            next = GetNextSibling (next);
        }else if (mode & wxTL_MODE_NAV_VISIBLE) { //
            next = GetNextVisible (next, false);
        }else if (mode & wxTL_MODE_NAV_EXPANDED) {
            next = GetNextExpanded (next);
        }else{ // (mode & wxTL_MODE_NAV_FULLTREE) default
            next = GetNext (next, true);
        }
    }

#if !wxCHECK_VERSION(2, 5, 0)
    long cookie = 0;
#else
    wxTreeItemIdValue cookie = 0;
#endif
    if (!next.IsOk()) {
        next = (wxTreeListItem*)GetRootItem().m_pItem;
        if (HasFlag(wxTR_HIDE_ROOT)) {
            next = (wxTreeListItem*)GetFirstChild (GetRootItem().m_pItem, cookie).m_pItem;
        }
    }
    if (!next.IsOk()) return (wxTreeItemId*)NULL;

    // start checking the next items
    while (next.IsOk() && (next != item)) {
        if (mode & wxTL_MODE_FIND_PARTIAL) {
            itemText = GetItemText (next).Mid (0, str.Length());
        }else{
            itemText = GetItemText (next);
        }
        if (mode & wxTL_MODE_FIND_NOCASE) {
            if (itemText.CmpNoCase (str) == 0) return next;
        }else{
            if (itemText.Cmp (str) == 0) return next;
        }
        if (mode & wxTL_MODE_NAV_LEVEL) {
            next = GetNextSibling (next);
        }else if (mode & wxTL_MODE_NAV_VISIBLE) { //
            next = GetNextVisible (next, false);
        }else if (mode & wxTL_MODE_NAV_EXPANDED) {
            next = GetNextExpanded (next);
        }else{ // (mode & wxTL_MODE_NAV_FULLTREE) default
            next = GetNext (next, true);
        }
        if (!next.IsOk() && item.IsOk()) {
            next = (wxTreeListItem*)GetRootItem().m_pItem;
            if (HasFlag(wxTR_HIDE_ROOT)) {
                next = (wxTreeListItem*)GetNextChild (GetRootItem().m_pItem, cookie).m_pItem;
            }
        }
    }
    return (wxTreeItemId*)NULL;
}

void wxTreeListMainWindow::SetDragItem (const wxTreeItemId& item) {
    wxTreeListItem *prevItem = m_dragItem;
    m_dragItem = (wxTreeListItem*) item.m_pItem;
    if (prevItem) RefreshLine (prevItem);
    if (m_dragItem) RefreshLine (m_dragItem);
}

void wxTreeListMainWindow::CalculateLineHeight() {
    wxClientDC dc (this);
    dc.SetFont (m_normalFont);
    m_lineHeight = (int)(dc.GetCharHeight() + m_linespacing);

    if (m_imageListNormal) {
        // Calculate a m_lineHeight value from the normal Image sizes.
        // May be toggle off. Then wxTreeListMainWindow will spread when
        // necessary (which might look ugly).
        int n = m_imageListNormal->GetImageCount();
        for (int i = 0; i < n ; i++) {
            int width = 0, height = 0;
            m_imageListNormal->GetSize(i, width, height);
            if (height > m_lineHeight) m_lineHeight = height + m_linespacing;
        }
    }

    if (m_imageListButtons) {
        // Calculate a m_lineHeight value from the Button image sizes.
        // May be toggle off. Then wxTreeListMainWindow will spread when
        // necessary (which might look ugly).
        int n = m_imageListButtons->GetImageCount();
        for (int i = 0; i < n ; i++) {
            int width = 0, height = 0;
            m_imageListButtons->GetSize(i, width, height);
            if (height > m_lineHeight) m_lineHeight = height + m_linespacing;
        }
    }

    if (m_lineHeight < 30) { // add 10% space if greater than 30 pixels
        m_lineHeight += 2; // minimal 2 pixel space
    }else{
        m_lineHeight += m_lineHeight / 10; // otherwise 10% space
    }
}

void wxTreeListMainWindow::SetImageList (wxImageList *imageList) {
    if (m_ownsImageListNormal) delete m_imageListNormal;
    m_imageListNormal = imageList;
    m_ownsImageListNormal = false;
    m_dirty = true;
    CalculateLineHeight();
}

void wxTreeListMainWindow::SetStateImageList (wxImageList *imageList) {
    if (m_ownsImageListState) delete m_imageListState;
    m_imageListState = imageList;
    m_ownsImageListState = false;
}

void wxTreeListMainWindow::SetButtonsImageList (wxImageList *imageList) {
    if (m_ownsImageListButtons) delete m_imageListButtons;
    m_imageListButtons = imageList;
    m_ownsImageListButtons = false;
    m_dirty = true;
    CalculateLineHeight();
}

void wxTreeListMainWindow::AssignImageList (wxImageList *imageList) {
    SetImageList(imageList);
    m_ownsImageListNormal = true;
}

void wxTreeListMainWindow::AssignStateImageList (wxImageList *imageList) {
    SetStateImageList(imageList);
    m_ownsImageListState = true;
}

void wxTreeListMainWindow::AssignButtonsImageList (wxImageList *imageList) {
    SetButtonsImageList(imageList);
    m_ownsImageListButtons = true;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

void wxTreeListMainWindow::AdjustMyScrollbars() {
    if (m_rootItem) {
        int xUnit, yUnit;
        GetScrollPixelsPerUnit (&xUnit, &yUnit);
        if (xUnit == 0) xUnit = GetCharWidth();
        if (yUnit == 0) yUnit = m_lineHeight;
        int x = 0, y = 0;
        m_rootItem->GetSize (x, y, this);
        y += yUnit + 2; // one more scrollbar unit + 2 pixels
        int x_pos = GetScrollPos (wxHORIZONTAL);
        int y_pos = GetScrollPos (wxVERTICAL);
        x = m_owner->GetHeaderWindow()->GetWidth() + 2;
        if (x < GetClientSize().GetWidth()) x_pos = 0;
        SetScrollbars (xUnit, yUnit, x/xUnit, y/yUnit, x_pos, y_pos);
    }else{
        SetScrollbars (0, 0, 0, 0);
    }
}

int wxTreeListMainWindow::GetLineHeight (wxTreeListItem *item) const {
    if (GetWindowStyleFlag() & wxTR_HAS_VARIABLE_ROW_HEIGHT) {
        return item->GetHeight();
    }else{
        return m_lineHeight;
    }
}

void wxTreeListMainWindow::PaintItem (wxTreeListItem *item, wxDC& dc) {

    wxTreeItemAttr *attr = item->GetAttributes();

    dc.SetFont (GetItemFont (item));

    wxColour colText;
    if (attr && attr->HasTextColour()) {
        colText = attr->GetTextColour();
    }else{
        colText = GetForegroundColour();
    }
#if !wxCHECK_VERSION(2, 5, 0)
    wxColour colTextHilight = wxSystemSettings::GetSystemColour (wxSYS_COLOUR_HIGHLIGHTTEXT);
#else
    wxColour colTextHilight = wxSystemSettings::GetColour (wxSYS_COLOUR_HIGHLIGHTTEXT);
#endif

    int total_w = m_owner->GetHeaderWindow()->GetWidth();
    int total_h = GetLineHeight(item);
    int off_h = HasFlag(wxTR_ROW_LINES) ? 1 : 0;
    int off_w = HasFlag(wxTR_COLUMN_LINES) ? 1 : 0;
    wxDCClipper clipper (dc, 0, item->GetY(), total_w, total_h); // only within line

    int text_w = 0, text_h = 0;
    dc.GetTextExtent( item->GetText(GetMainColumn()), &text_w, &text_h );

    // determine background and show it
    wxColour colBg;
    if (attr && attr->HasBackgroundColour()) {
        colBg = attr->GetBackgroundColour();
    }else{
        colBg = m_backgroundColour;
    }
    dc.SetBrush (wxBrush (colBg, wxSOLID));
    dc.SetPen (*wxTRANSPARENT_PEN);
    if (HasFlag (wxTR_FULL_ROW_HIGHLIGHT)) {
        if (item == m_dragItem) {
            dc.SetBrush (*m_hilightBrush);
#ifndef __WXMAC__ // don't draw rect outline if we already have the background color
            dc.SetPen ((item == m_dragItem)? *wxBLACK_PEN: *wxTRANSPARENT_PEN);
#endif // !__WXMAC__
            dc.SetTextForeground (colTextHilight);
        }else if (item->IsSelected()) {
            if (!m_isDragging && m_hasFocus) {
                dc.SetBrush (*m_hilightBrush);
#ifndef __WXMAC__ // don't draw rect outline if we already have the background color
                dc.SetPen (*wxBLACK_PEN);
#endif // !__WXMAC__
            }else{
                dc.SetBrush (*m_hilightUnfocusedBrush);
#ifndef __WXMAC__ // don't draw rect outline if we already have the background color
                dc.SetPen (*wxTRANSPARENT_PEN);
#endif // !__WXMAC__
            }
            dc.SetTextForeground (colTextHilight);
        }else if (item == m_curItem) {
            dc.SetPen (m_hasFocus? *wxBLACK_PEN: *wxTRANSPARENT_PEN);
        }else{
            dc.SetTextForeground (colText);
        }
        dc.DrawRectangle (0, item->GetY() + off_h, total_w, total_h - off_h);
    }else{
        dc.SetTextForeground (colText);
    }

    int text_extraH = (total_h > text_h) ? (total_h - text_h)/2 : 0;
    int img_extraH = (total_h > m_imgHeight)? (total_h-m_imgHeight)/2: 0;
    int x_colstart = 0;
    for (int i = 0; i < GetColumnCount(); ++i ) {
        if (!m_owner->GetHeaderWindow()->IsColumnShown(i)) continue;

        int col_w = m_owner->GetHeaderWindow()->GetColumnWidth(i);
        wxDCClipper clipper (dc, x_colstart, item->GetY(), col_w, total_h); // only within column

        int x = 0;
        int image = NO_IMAGE;
        int image_w = 0;
        if(i == GetMainColumn()) {
            x = item->GetX() + MARGIN;
            if (HasButtons()) {
                x += (m_btnWidth-m_btnWidth2) + LINEATROOT;
            }else{
                x -= m_indent/2;
            }
            if (m_imageListNormal) image = item->GetCurrentImage();
        }else{
            x = x_colstart + MARGIN;
            image = item->GetImage(i);
        }
        if (image != NO_IMAGE) image_w = m_imgWidth + MARGIN;

        // honor text alignment
        wxString text = item->GetText(i);
        int w = 0;
        switch ( m_owner->GetHeaderWindow()->GetColumn(i).GetAlignment() ) {
        case wxALIGN_LEFT:
            // nothing to do, already left aligned
            break;
        case wxALIGN_RIGHT:
            dc.GetTextExtent (text, &text_w, NULL);
            w = col_w - (image_w + text_w + off_w + MARGIN);
            x += (w > 0)? w: 0;
            break;
        case wxALIGN_CENTER:
            dc.GetTextExtent(text, &text_w, NULL);
            w = (col_w - (image_w + text_w + off_w + MARGIN))/2;
            x += (w > 0)? w: 0;
            break;
        }
        int text_x = x + image_w;
        if (i == GetMainColumn()) item->SetTextX (text_x);

        if (!HasFlag (wxTR_FULL_ROW_HIGHLIGHT)) {
            if (i == GetMainColumn()) {
                if (item == m_dragItem) {
                    dc.SetBrush (*m_hilightBrush);
#ifndef __WXMAC__ // don't draw rect outline if we already have the background color
                    dc.SetPen ((item == m_dragItem)? *wxBLACK_PEN: *wxTRANSPARENT_PEN);
#endif // !__WXMAC__
                    dc.SetTextForeground (colTextHilight);
                }else if (item->IsSelected()) {
                    if (!m_isDragging && m_hasFocus) {
                        dc.SetBrush (*m_hilightBrush);
#ifndef __WXMAC__ // don't draw rect outline if we already have the background color
                        dc.SetPen (*wxBLACK_PEN);
#endif // !__WXMAC__
                    }else{
                        dc.SetBrush (*m_hilightUnfocusedBrush);
#ifndef __WXMAC__ // don't draw rect outline if we already have the background color
                      dc.SetPen (*wxTRANSPARENT_PEN);
#endif // !__WXMAC__
                    }
                    dc.SetTextForeground (colTextHilight);
                }else if (item == m_curItem) {
                    dc.SetPen (m_hasFocus? *wxBLACK_PEN: *wxTRANSPARENT_PEN);
                }else{
                    dc.SetTextForeground (colText);
                }
                dc.DrawRectangle (text_x, item->GetY() + off_h, text_w, total_h - off_h);
            }else{
                dc.SetTextForeground (colText);
            }
        }

        if (HasFlag(wxTR_COLUMN_LINES)) { // vertical lines between columns
#if !wxCHECK_VERSION(2, 5, 0)
            wxPen pen (wxSystemSettings::GetSystemColour (wxSYS_COLOUR_3DLIGHT ), 1, wxSOLID);
#else
            wxPen pen (wxSystemSettings::GetColour (wxSYS_COLOUR_3DLIGHT ), 1, wxSOLID);
#endif
            dc.SetPen ((GetBackgroundColour() == *wxWHITE)? pen: *wxWHITE_PEN);
            dc.DrawLine (x_colstart+col_w-1, item->GetY(), x_colstart+col_w-1, item->GetY()+total_h);
        }

        dc.SetBackgroundMode (wxTRANSPARENT);

        if (image != NO_IMAGE) {
            int y = item->GetY() + img_extraH;
            m_imageListNormal->Draw (image, dc, x, y, wxIMAGELIST_DRAW_TRANSPARENT );
        }
        int text_y = item->GetY() + text_extraH;
        dc.DrawText (text, (wxCoord)text_x, (wxCoord)text_y);

        x_colstart += col_w;
    }

    // restore normal font
    dc.SetFont( m_normalFont );
}

// Now y stands for the top of the item, whereas it used to stand for middle !
void wxTreeListMainWindow::PaintLevel (wxTreeListItem *item, wxDC &dc,
                                       int level, int &y, int x_maincol) {

    // Handle hide root (only level 0)
    if (HasFlag(wxTR_HIDE_ROOT) && (level == 0)) {
        wxArrayTreeListItems& children = item->GetChildren();
        for (size_t n = 0; n < children.Count(); n++) {
            PaintLevel (children[n], dc, 1, y, x_maincol);
        }
        // end after expanding root
        return;
    }

    // calculate position of vertical lines
    int x = x_maincol + MARGIN; // start of column
    if (HasFlag(wxTR_LINES_AT_ROOT)) x += LINEATROOT; // space for lines at root
    if (HasButtons()) {
        x += (m_btnWidth-m_btnWidth2); // half button space
    }else{
        x += (m_indent-m_indent/2);
    }
    if (HasFlag(wxTR_HIDE_ROOT)) {
        x += m_indent * (level-1); // indent but not level 1
    }else{
        x += m_indent * level; // indent according to level
    }

    // set position of vertical line
    item->SetX (x);
    item->SetY (y);

    int h = GetLineHeight (item);
    int y_top = y;
    int y_mid = y_top + (h/2);
    y += h;

    int exposed_x = dc.LogicalToDeviceX(0);
    int exposed_y = dc.LogicalToDeviceY(y_top);

    if (IsExposed(exposed_x, exposed_y, 10000, h)) { // 10000 = very much

        if (HasFlag(wxTR_ROW_LINES)) { // horizontal lines between rows
            //dc.DestroyClippingRegion();
            int total_width = m_owner->GetHeaderWindow()->GetWidth();
            // if the background colour is white, choose a
            // contrasting color for the lines
#if !wxCHECK_VERSION(2, 5, 0)
            wxPen pen (wxSystemSettings::GetSystemColour (wxSYS_COLOUR_3DLIGHT ), 1, wxSOLID);
#else
            wxPen pen (wxSystemSettings::GetColour (wxSYS_COLOUR_3DLIGHT ), 1, wxSOLID);
#endif
            dc.SetPen ((GetBackgroundColour() == *wxWHITE)? pen: *wxWHITE_PEN);
            dc.DrawLine (0, y_top, total_width, y_top);
            dc.DrawLine (0, y_top+h, total_width, y_top+h);
        }

        // draw item
        PaintItem (item, dc);

        // restore DC objects
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(m_dottedPen);

        // clip to the column width
        int clip_width = m_owner->GetHeaderWindow()->
                            GetColumn(m_main_column).GetWidth();
        wxDCClipper clipper(dc, x_maincol, y_top, clip_width, 10000);

        if (!HasFlag(wxTR_NO_LINES)) { // connection lines

            // draw the horizontal line here
            dc.SetPen(m_dottedPen);
            int x2 = x - m_indent;
            if (x2 < (x_maincol + MARGIN)) x2 = x_maincol + MARGIN;
            int x3 = x + (m_btnWidth-m_btnWidth2);
            if (HasButtons()) {
                if (item->HasPlus()) {
                    dc.DrawLine (x2, y_mid, x - m_btnWidth2, y_mid);
                    dc.DrawLine (x3, y_mid, x3 + LINEATROOT, y_mid);
                }else{
                    dc.DrawLine (x2, y_mid, x3 + LINEATROOT, y_mid);
                }
            }else{
                dc.DrawLine (x2, y_mid, x - m_indent/2, y_mid);
            }
        }

        if (item->HasPlus() && HasButtons()) { // should the item show a button?

            if (m_imageListButtons) {

                // draw the image button here
                int image = wxTreeItemIcon_Normal;
                if (item->IsExpanded()) image = wxTreeItemIcon_Expanded;
                if (item->IsSelected()) image += wxTreeItemIcon_Selected - wxTreeItemIcon_Normal;
                int xx = x - m_btnWidth2 + MARGIN;
                int yy = y_mid - m_btnHeight2;
                dc.SetClippingRegion(xx, yy, m_btnWidth, m_btnHeight);
                m_imageListButtons->Draw (image, dc, xx, yy, wxIMAGELIST_DRAW_TRANSPARENT);
                dc.DestroyClippingRegion();

            }else if (HasFlag (wxTR_TWIST_BUTTONS)) {

                // draw the twisty button here
                dc.SetPen(*wxBLACK_PEN);
                dc.SetBrush(*m_hilightBrush);
                wxPoint button[3];
                if (item->IsExpanded()) {
                    button[0].x = x - (m_btnWidth2+1);
                    button[0].y = y_mid - (m_btnHeight/3);
                    button[1].x = x + (m_btnWidth2+1);
                    button[1].y = button[0].y;
                    button[2].x = x;
                    button[2].y = button[0].y + (m_btnHeight2+1);
                }else{
                    button[0].x = x - (m_btnWidth/3);
                    button[0].y = y_mid - (m_btnHeight2+1);
                    button[1].x = button[0].x;
                    button[1].y = y_mid + (m_btnHeight2+1);
                    button[2].x = button[0].x + (m_btnWidth2+1);
                    button[2].y = y_mid;
                }
                dc.DrawPolygon(3, button);

            }else{ // if (HasFlag(wxTR_HAS_BUTTONS))

                // draw the plus sign here
#if !wxCHECK_VERSION(2, 7, 0)
                dc.SetPen(*wxGREY_PEN);
                dc.SetBrush(*wxWHITE_BRUSH);
                dc.DrawRectangle (x-m_btnWidth2, y_mid-m_btnHeight2, m_btnWidth, m_btnHeight);
                dc.SetPen(*wxBLACK_PEN);
                dc.DrawLine (x-(m_btnWidth2-2), y_mid, x+(m_btnWidth2-1), y_mid);
                if (!item->IsExpanded()) { // change "-" to "+"
                    dc.DrawLine (x, y_mid-(m_btnHeight2-2), x, y_mid+(m_btnHeight2-1));
                }
#else
                wxRect rect (x-m_btnWidth2, y_mid-m_btnHeight2, m_btnWidth, m_btnHeight);
                int flag = item->IsExpanded()? wxCONTROL_EXPANDED: 0;
                wxRendererNative::GetDefault().DrawTreeItemButton (this, dc, rect, flag);
#endif

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

        // clip to the column width
        int clip_width = m_owner->GetHeaderWindow()->
                            GetColumn(m_main_column).GetWidth();

        // process lower levels
        int oldY;
        if (m_imgWidth > 0) {
            oldY = y_mid + m_imgHeight2;
        }else{
            oldY = y_mid + h/2;
        }
        int y2;
        for (size_t n = 0; n < children.Count(); ++n) {

            y2 = y + h/2;
            PaintLevel (children[n], dc, level+1, y, x_maincol);

            // draw vertical line
            wxDCClipper clipper(dc, x_maincol, y_top, clip_width, 10000);
            if (!HasFlag (wxTR_NO_LINES)) {
                x = item->GetX();
                dc.DrawLine (x, oldY, x, y2);
                oldY = y2;
            }
        }
    }
}


// ----------------------------------------------------------------------------
// wxWindows callbacks
// ----------------------------------------------------------------------------

void wxTreeListMainWindow::OnPaint (wxPaintEvent &WXUNUSED(event)) {

    wxPaintDC dc (this);
    PrepareDC (dc);

    if (!m_rootItem || (GetColumnCount() <= 0)) return;

    // calculate button size
    if (m_imageListButtons) {
        m_imageListButtons->GetSize (0, m_btnWidth, m_btnHeight);
    }else if (HasButtons()) {
        m_btnWidth = BTNWIDTH;
        m_btnHeight = BTNHEIGHT;
    }
    m_btnWidth2 = m_btnWidth/2;
    m_btnHeight2 = m_btnHeight/2;

    // calculate image size
    if (m_imageListNormal) {
        m_imageListNormal->GetSize (0, m_imgWidth, m_imgHeight);
    }
    m_imgWidth2 = m_imgWidth/2;
    m_imgHeight2 = m_imgHeight/2;

    // calculate indent size
    if (m_imageListButtons) {
        m_indent = wxMax (MININDENT, m_btnWidth + MARGIN);
    }else if (HasButtons()) {
        m_indent = wxMax (MININDENT, m_btnWidth + LINEATROOT);
    }

    // set default values
    dc.SetFont( m_normalFont );
    dc.SetPen( m_dottedPen );

    // calculate column start and paint
    int x_maincol = 0;
    int i = 0;
    for (i = 0; i < (int)GetMainColumn(); ++i) {
        if (!m_owner->GetHeaderWindow()->IsColumnShown(i)) continue;
        x_maincol += m_owner->GetHeaderWindow()->GetColumnWidth (i);
    }
    int y = 0;
    PaintLevel (m_rootItem, dc, 0, y, x_maincol);
}

void wxTreeListMainWindow::OnSetFocus (wxFocusEvent &event) {

    m_hasFocus = true;
    RefreshSelected();
    if (m_curItem) RefreshLine (m_curItem);
    event.Skip();
}

void wxTreeListMainWindow::OnKillFocus( wxFocusEvent &event )
{
    m_hasFocus = false;
    RefreshSelected();
    if (m_curItem) RefreshLine (m_curItem);
    event.Skip();
}

void wxTreeListMainWindow::OnChar (wxKeyEvent &event) {
    // send event to user code
    wxTreeEvent nevent (wxEVT_COMMAND_TREE_KEY_DOWN, m_owner->GetId());
    nevent.SetKeyEvent (event);
    nevent.SetEventObject (m_owner);
    if (m_owner->GetEventHandler()->ProcessEvent (nevent)) return; // handled in user code

    // determine first current if none
    bool curItemSet = false;
    if (!m_curItem) {
        m_curItem = (wxTreeListItem*)GetRootItem().m_pItem;
        if (HasFlag(wxTR_HIDE_ROOT)) {
#if !wxCHECK_VERSION(2, 5, 0)
            long cookie = 0;
#else
            wxTreeItemIdValue cookie = 0;
#endif
            m_curItem = (wxTreeListItem*)GetFirstChild (m_curItem, cookie).m_pItem;
        }
        curItemSet = true;
    }
    if (!m_curItem) return; // do nothing if empty tree

    // remember item at shift down
    if (HasFlag(wxTR_MULTIPLE) && event.ShiftDown()) {
        if (!m_shiftItem) m_shiftItem = m_curItem;
    }else{
        m_shiftItem = (wxTreeListItem*)NULL;
    }

    // process all cases
    wxTreeItemId newItem = (wxTreeItemId*)NULL;
    switch (event.GetKeyCode()) {

        // '+': Expand subtree
        case '+':
        case WXK_ADD: {
            if (m_curItem->HasPlus() && !IsExpanded (m_curItem)) Expand (m_curItem);
        }break;

        // '-': collapse subtree
        case '-':
        case WXK_SUBTRACT: {
            if (m_curItem->HasPlus() && IsExpanded (m_curItem)) Collapse (m_curItem);
        }break;

        // '*': expand/collapse all subtrees // TODO: Mak it more useful
        case '*':
        case WXK_MULTIPLY: {
            if (m_curItem->HasPlus() && !IsExpanded (m_curItem)) {
                ExpandAll (m_curItem);
            }else if (m_curItem->HasPlus()) {
                Collapse (m_curItem); // TODO: CollapseAll
            }
        }break;

        // ' ': toggle current item
        case ' ': {
            SelectItem (m_curItem, (wxTreeListItem*)NULL, false);
        }break;

        // <RETURN>: activate current item
        case WXK_RETURN: {
            wxTreeEvent aevent (wxEVT_COMMAND_TREE_ITEM_ACTIVATED, m_owner->GetId());
#if !wxCHECK_VERSION(2, 5, 0)
            aevent.SetItem ((long)m_curItem);
#else
            aevent.SetItem (m_curItem);
#endif
            aevent.SetEventObject (m_owner);
            m_owner->GetEventHandler()->ProcessEvent (aevent);
        }break;

        // <BKSP>: go to the parent without collapsing
        case WXK_BACK: {
            newItem = GetItemParent (m_curItem);
            if ((newItem == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT)) {
                newItem = GetPrevSibling (m_curItem); // get sibling instead of root
            }
        }break;

        // <UP>: go to the previous sibling or to the last of its children, to the parent
        case WXK_UP: {
            newItem = GetPrevSibling (m_curItem);
            if (newItem) {
#if !wxCHECK_VERSION(2, 5, 0)
                long cookie = 0;
#else
                wxTreeItemIdValue cookie = 0;
#endif
                while (IsExpanded (newItem) && HasChildren (newItem)) {
                    newItem = GetLastChild (newItem, cookie);
                }
            }else {
                newItem = GetItemParent (m_curItem);
                if ((newItem == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT)) {
                    newItem = (wxTreeItemId*)NULL; // don't go to root if it is hidden
                }
            }
        }break;

        // <LEFT>: if expanded collapse subtree, else go to the parent
        case WXK_LEFT: {
            if (IsExpanded (m_curItem)) {
                Collapse (m_curItem);
            }else{
                newItem = GetItemParent (m_curItem);
                if ((newItem == GetRootItem()) && HasFlag(wxTR_HIDE_ROOT)) {
                    newItem = GetPrevSibling (m_curItem); // go to sibling if it is hidden
                }
            }
        }break;

        // <RIGHT>: if possible expand subtree, else go go to the first child
        case WXK_RIGHT: {
            if (m_curItem->HasPlus() && !IsExpanded (m_curItem)) {
                Expand (m_curItem);
            }else{
                if (IsExpanded (m_curItem) && HasChildren (m_curItem)) {
#if !wxCHECK_VERSION(2, 5, 0)
                    long cookie = 0;
#else
                    wxTreeItemIdValue cookie = 0;
#endif
                    newItem = GetFirstChild (m_curItem, cookie);
                }
            }
        }break;

        // <DOWN>: if expanded go to the first child, else to the next sibling, ect
        case WXK_DOWN: {
            if (curItemSet) {
                newItem = m_curItem;
            }else{
                if (IsExpanded (m_curItem) && HasChildren (m_curItem)) {
#if !wxCHECK_VERSION(2, 5, 0)
                    long cookie = 0;
#else
                    wxTreeItemIdValue cookie = 0;
#endif
                    newItem = GetFirstChild( m_curItem, cookie );
                }
                if (!newItem) {
                    wxTreeItemId parent = m_curItem;
                    do {
                        newItem = GetNextSibling (parent);
                        parent = GetItemParent (parent);
                    } while (!newItem && parent);
                }
            }
        }break;

        // <END>: go to last item of the root
        case WXK_END: {
#if !wxCHECK_VERSION(2, 5, 0)
            long cookie = 0;
#else
            wxTreeItemIdValue cookie = 0;
#endif
            newItem = GetLastChild (GetRootItem(), cookie);
        }break;

        // <HOME>: go to root
        case WXK_HOME: {
            newItem = GetRootItem();
            if (HasFlag(wxTR_HIDE_ROOT)) {
#if !wxCHECK_VERSION(2, 5, 0)
                long cookie = 0;
#else
                wxTreeItemIdValue cookie = 0;
#endif
                newItem = GetFirstChild (newItem, cookie);
            }
        }break;

        // any char: go to the next matching string
        default:
            if (event.GetKeyCode() >= (int)' ') {
                if (!m_findTimer->IsRunning()) m_findStr.Clear();
                m_findStr.Append (event.GetKeyCode());
                m_findTimer->Start (FIND_TIMER_TICKS, wxTIMER_ONE_SHOT);
                wxTreeItemId prev = m_curItem? (wxTreeItemId*)m_curItem: (wxTreeItemId*)NULL;
                while (true) {
                    newItem = FindItem (prev, m_findStr, wxTL_MODE_NAV_EXPANDED |
                                                         wxTL_MODE_FIND_PARTIAL |
                                                         wxTL_MODE_FIND_NOCASE);
                    if (newItem || (m_findStr.Length() <= 1)) break;
                    m_findStr.RemoveLast();
                };
            }
            event.Skip();

    }

    // select and show the new item
    if (newItem) {
        if (!event.ControlDown()) {
            bool unselect_others = !((event.ShiftDown() || event.ControlDown()) &&
                                      HasFlag(wxTR_MULTIPLE));
            SelectItem (newItem, m_shiftItem, unselect_others);
        }
        EnsureVisible (newItem);
        wxTreeListItem *oldItem = m_curItem;
        m_curItem = (wxTreeListItem*)newItem.m_pItem; // make the new item the current item
        RefreshLine (oldItem);
    }

}

wxTreeItemId wxTreeListMainWindow::HitTest (const wxPoint& point, int& flags, int& column) {

    int w, h;
    GetSize(&w, &h);
    flags=0;
    column = -1;
    if (point.x<0) flags |= wxTREE_HITTEST_TOLEFT;
    if (point.x>w) flags |= wxTREE_HITTEST_TORIGHT;
    if (point.y<0) flags |= wxTREE_HITTEST_ABOVE;
    if (point.y>h) flags |= wxTREE_HITTEST_BELOW;
    if (flags) return wxTreeItemId();

    if (!m_rootItem) {
        flags = wxTREE_HITTEST_NOWHERE;
        column = -1;
        return wxTreeItemId();
    }

    wxTreeListItem *hit = m_rootItem->HitTest (CalcUnscrolledPosition(point),
                                               this, flags, column, 0);
    if (!hit) {
        flags = wxTREE_HITTEST_NOWHERE;
        column = -1;
        return wxTreeItemId();
    }
    return hit;
}

// get the bounding rectangle of the item (or of its label only)
bool wxTreeListMainWindow::GetBoundingRect (const wxTreeItemId& itemId, wxRect& rect,
                                            bool WXUNUSED(textOnly)) const {
    wxCHECK_MSG (itemId.IsOk(), false, _T("invalid item in wxTreeListMainWindow::GetBoundingRect") );

    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;

    int xUnit, yUnit;
    GetScrollPixelsPerUnit (&xUnit, &yUnit);
    int startX, startY;
    GetViewStart(& startX, & startY);

    rect.x = item->GetX() - startX * xUnit;
    rect.y = item->GetY() - startY * yUnit;
    rect.width = item->GetWidth();
    rect.height = GetLineHeight (item);

    return true;
}

/* **** */

void wxTreeListMainWindow::EditLabel (const wxTreeItemId& item, int column) {
    if (!item.IsOk()) return;
    if (!((column >= 0) && (column < GetColumnCount()))) return;

    m_editItem = (wxTreeListItem*) item.m_pItem;

    wxTreeEvent te( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, m_owner->GetId() );
#if !wxCHECK_VERSION(2, 5, 0)
    te.SetItem ((long)m_editItem);
#else
    te.SetItem (m_editItem);
#endif
    te.SetInt (column);
    te.SetEventObject (m_owner );
    m_owner->GetEventHandler()->ProcessEvent (te);

    if (!te.IsAllowed()) return;

    // ensure that the position of the item it calculated in any case
    if (m_dirty) CalculatePositions();

    wxTreeListHeaderWindow* header_win = m_owner->GetHeaderWindow();
    int x = 0;
    int y = m_editItem->GetY() + 1; // wxTextCtrl needs 1 pixels above the text
    int w = 0;
    int h = m_editItem->GetHeight();
    long style = 0;
    if (column == GetMainColumn()) {
        x += m_editItem->GetTextX() - 2; // wxTextCtrl needs 2 pixels before the text
        w = wxMin (m_editItem->GetWidth(), m_owner->GetHeaderWindow()->GetWidth() - x);
    }else{
        for (int i = 0; i < column; ++i) x += header_win->GetColumnWidth (i); // start of column
        switch (header_win->GetColumnAlignment (column)) {
            case wxALIGN_LEFT: {style = wxTE_LEFT; break;}
            case wxALIGN_RIGHT: {style = wxTE_RIGHT; break;}
            case wxALIGN_CENTER: {style = wxTE_CENTER; break;}
        }
        w = header_win->GetColumnWidth (column); // width of column
    }

    wxClientDC dc (this);
    PrepareDC (dc);
    x = dc.LogicalToDeviceX (x);
    y = dc.LogicalToDeviceY (y);

    wxEditTextCtrl *text = new wxEditTextCtrl (this, -1, &m_renameAccept, &m_renameRes,
                                               this, m_editItem->GetText (column),
                                               wxPoint (x, y), wxSize (w, h), style);
    text->SetFocus();
}

void wxTreeListMainWindow::OnRenameTimer() {
    EditLabel (m_curItem, m_curColumn);
}

void wxTreeListMainWindow::OnRenameAccept() {

    // TODO if the validator fails this causes a crash
    wxTreeEvent le( wxEVT_COMMAND_TREE_END_LABEL_EDIT, m_owner->GetId() );
#if !wxCHECK_VERSION(2, 5, 0)
    le.SetItem((long)m_editItem);
#else
    le.SetItem(m_editItem);
#endif
    le.SetEventObject( /*this*/m_owner );
    le.SetLabel( m_renameRes );
    m_owner->GetEventHandler()->ProcessEvent( le );

    if (!le.IsAllowed()) return;

    SetItemText (m_editItem, m_curColumn, m_renameRes);
}

void wxTreeListMainWindow::OnMouse (wxMouseEvent &event) {
    if (!m_rootItem) return;

    // we process left mouse up event (enables in-place edit), right down
    // (pass to the user code), left dbl click (activate item) and
    // dragging/moving events for items drag-and-drop
    if (!(event.LeftDown() ||
          event.LeftUp() ||
          event.RightDown() ||
          event.RightUp() ||
          event.LeftDClick() ||
          event.Dragging() ||
          (event.GetWheelRotation() != 0 )/*? TODO ||
          event.Moving()?*/)) {
        m_owner->GetEventHandler()->ProcessEvent (event);
        return;
    }

    // set focus if window clicked
    if (event.LeftDown() || event.RightDown()) SetFocus();

    // determine event
    wxPoint p = wxPoint (event.GetX(), event.GetY());
    int flags = 0;
    wxTreeListItem *item = m_rootItem->HitTest (CalcUnscrolledPosition (p),
                                                this, flags, m_curColumn, 0);

    // we only process dragging here
    if (event.Dragging()){
        if (m_isDragging) return; // nothing to do, already done
        if (item == NULL) return; // we need an item to dragging

        // determine drag start
        if (m_dragCount == 0) {
            m_dragTimer->Start (DRAG_TIMER_TICKS, wxTIMER_ONE_SHOT);
        }
        m_dragCount++;
        if (m_dragCount < 3) return; // minimum drag 3 pixel
        if (m_dragTimer->IsRunning()) return;

        // we're going to drag
        m_dragCount = 0;
        m_isDragging = true;
        CaptureMouse();
        RefreshSelected();

        // send drag start event
        wxEventType command = event.LeftIsDown()
                              ? wxEVT_COMMAND_TREE_BEGIN_DRAG
                              : wxEVT_COMMAND_TREE_BEGIN_RDRAG;
        wxTreeEvent nevent (command, m_owner->GetId());
        nevent.SetEventObject (m_owner);
#if !wxCHECK_VERSION(2, 5, 0)
        nevent.SetItem ((long)item); // the item the drag is ended
#else
        nevent.SetItem (item); // the item the drag is ended
#endif
        nevent.Veto(); // dragging must be explicit allowed!
        m_owner->GetEventHandler()->ProcessEvent (nevent);

    }else if (m_isDragging) { // any other event but not event.Dragging()

        // end dragging
        m_dragCount = 0;
        m_isDragging = false;
        if (HasCapture()) ReleaseMouse();
        RefreshSelected();

        // send drag end event event
        wxTreeEvent nevent (wxEVT_COMMAND_TREE_END_DRAG, m_owner->GetId());
        nevent.SetEventObject (m_owner);
#if !wxCHECK_VERSION(2, 5, 0)
        nevent.SetItem ((long)item); // the item the drag is started
#else
        nevent.SetItem (item); // the item the drag is started
#endif
        nevent.SetPoint (p);
        m_owner->GetEventHandler()->ProcessEvent (nevent);

    }else if (m_dragCount > 0) { // just in case dragging is initiated

        // end dragging
        m_dragCount = 0;

    }

    // we process only the messages which happen on tree items
    if (item == NULL) {
        m_owner->GetEventHandler()->ProcessEvent (event);
        return;
    }

    // remember item at shift down
    if (event.ShiftDown())  {
        if (!m_shiftItem) m_shiftItem = m_curItem;
    }else{
        m_shiftItem = (wxTreeListItem*)NULL;
    }

    if (event.RightUp()) {

        SetFocus();
        wxTreeEvent nevent (wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, m_owner->GetId());
        nevent.SetEventObject (m_owner);
#if !wxCHECK_VERSION(2, 5, 0)
        nevent.SetItem ((long)item); // the item clicked
#else
        nevent.SetItem (item); // the item clicked
#endif
        nevent.SetInt (m_curColumn); // the colum clicked
        nevent.SetPoint (p);
        m_owner->GetEventHandler()->ProcessEvent (nevent);

    }else if (event.LeftUp()) {

        if (m_lastOnSame) {
            if ((item == m_curItem) && (m_curColumn != -1) &&
                (m_owner->GetHeaderWindow()->IsColumnEditable (m_curColumn)) &&
                (flags & (wxTREE_HITTEST_ONITEMLABEL | wxTREE_HITTEST_ONITEMCOLUMN))){
                m_renameTimer->Start (RENAME_TIMER_TICKS, wxTIMER_ONE_SHOT);
            }
            m_lastOnSame = false;
        }

        if (((flags & wxTREE_HITTEST_ONITEMBUTTON) ||
             (flags & wxTREE_HITTEST_ONITEMICON)) &&
            HasButtons() && item->HasPlus()) {

            // only toggle the item for a single click, double click on
            // the button doesn't do anything (it toggles the item twice)
            if (event.LeftDown()) Toggle (item);

            // don't select the item if the button was clicked
            return;
        }

        // determine the selection if not done by left down
        if (!m_left_down_selection) {
            bool unselect_others = !((event.ShiftDown() || event.ControlDown()) &&
                                     HasFlag(wxTR_MULTIPLE));
            SelectItem (item, m_shiftItem, unselect_others);
            EnsureVisible (item);
            m_curItem = item; // make the new item the current item
        }else{
            m_left_down_selection = false;
        }

    }else if (event.LeftDown() || event.RightDown() || event.LeftDClick()) {

        if (event.LeftDown() || event.RightDown()) {
            SetFocus();
            m_lastOnSame = item == m_curItem;
        }

        if (((flags & wxTREE_HITTEST_ONITEMBUTTON) ||
             (flags & wxTREE_HITTEST_ONITEMICON)) &&
            item->HasPlus()) {

            // only toggle the item for a single click, double click on
            // the button doesn't do anything (it toggles the item twice)
            if (event.LeftDown()) Toggle (item);

            // don't select the item if the button was clicked
            return;
        }

        // determine the selection if the current item is not selected
        if (!item->IsSelected()) {
            bool unselect_others = !((event.ShiftDown() || event.ControlDown()) &&
                                     HasFlag(wxTR_MULTIPLE));
            SelectItem (item, m_shiftItem, unselect_others);
            EnsureVisible (item);
            m_curItem = item; // make the new item the current item
            m_left_down_selection = true;
        }

        // For some reason, Windows isn't recognizing a left double-click,
        // so we need to simulate it here.  Allow 200 milliseconds for now.
        if (event.LeftDClick()) {

            // double clicking should not start editing the item label
            m_renameTimer->Stop();
            m_lastOnSame = false;

            // send activate event first
            wxTreeEvent nevent (wxEVT_COMMAND_TREE_ITEM_ACTIVATED, m_owner->GetId());
            nevent.SetEventObject (m_owner);
#if !wxCHECK_VERSION(2, 5, 0)
            nevent.SetItem ((long)item); // the item clicked
#else
            nevent.SetItem (item); // the item clicked
#endif
            nevent.SetInt (m_curColumn); // the colum clicked
            nevent.SetPoint (p);
            if (!m_owner->GetEventHandler()->ProcessEvent (nevent)) {

                // if the user code didn't process the activate event,
                // handle it ourselves by toggling the item when it is
                // double clicked
                if (item->HasPlus()) Toggle(item);
            }
        }

    }else{ // any other event skip just in case

        event.Skip();

    }
}

void wxTreeListMainWindow::OnIdle (wxIdleEvent &WXUNUSED(event)) {
    /* after all changes have been done to the tree control,
     * we actually redraw the tree when everything is over */

    if (!m_dirty) return;

    m_dirty = false;

    CalculatePositions();
    Refresh();
    AdjustMyScrollbars();
}

void wxTreeListMainWindow::OnScroll (wxScrollWinEvent& event) {
    // FIXME
#if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
    wxScrolledWindow::OnScroll(event);
#else
    HandleOnScroll( event );
#endif

    if(event.GetOrientation() == wxHORIZONTAL) {
        m_owner->GetHeaderWindow()->Refresh();
        m_owner->GetHeaderWindow()->Update();
    }
}

void wxTreeListMainWindow::CalculateSize (wxTreeListItem *item, wxDC &dc) {
    wxCoord text_w = 0;
    wxCoord text_h = 0;

    dc.SetFont (GetItemFont (item));

    dc.GetTextExtent (item->GetText (m_main_column), &text_w, &text_h);

    // restore normal font
    dc.SetFont (m_normalFont);

    int total_h = (m_imgHeight > text_h) ? m_imgHeight : text_h;
    if (total_h < 30) { // add 10% space if greater than 30 pixels
        total_h += 2; // minimal 2 pixel space
    }else{
        total_h += total_h / 10; // otherwise 10% space
    }

    item->SetHeight (total_h);
    if (total_h > m_lineHeight) m_lineHeight = total_h;
    item->SetWidth(m_imgWidth + text_w+2);
}

// -----------------------------------------------------------------------------
void wxTreeListMainWindow::CalculateLevel (wxTreeListItem *item, wxDC &dc,
                                           int level, int &y, int x_colstart) {

    // calculate position of vertical lines
    int x = x_colstart + MARGIN; // start of column
    if (HasFlag(wxTR_LINES_AT_ROOT)) x += LINEATROOT; // space for lines at root
    if (HasButtons()) {
        x += (m_btnWidth-m_btnWidth2); // half button space
    }else{
        x += (m_indent-m_indent/2);
    }
    if (HasFlag(wxTR_HIDE_ROOT)) {
        x += m_indent * (level-1); // indent but not level 1
    }else{
        x += m_indent * level; // indent according to level
    }

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
    long n, count = (long)children.Count();
    ++level;
    for (n = 0; n < count; ++n) {
        CalculateLevel( children[n], dc, level, y, x_colstart );  // recurse
    }
}

void wxTreeListMainWindow::CalculatePositions() {
    if ( !m_rootItem ) return;

    wxClientDC dc(this);
    PrepareDC( dc );

    dc.SetFont( m_normalFont );

    dc.SetPen( m_dottedPen );
    //if(GetImageList() == NULL)
    // m_lineHeight = (int)(dc.GetCharHeight() + 4);

    int y = 2;
    int x_colstart = 0;
    for (int i = 0; i < (int)GetMainColumn(); ++i) {
        if (!m_owner->GetHeaderWindow()->IsColumnShown(i)) continue;
        x_colstart += m_owner->GetHeaderWindow()->GetColumnWidth(i);
    }
    CalculateLevel( m_rootItem, dc, 0, y, x_colstart ); // start recursion
}

void wxTreeListMainWindow::RefreshSubtree (wxTreeListItem *item) {
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

    Refresh (true, &rect );
    AdjustMyScrollbars();
}

void wxTreeListMainWindow::RefreshLine (wxTreeListItem *item) {
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

    Refresh (true, &rect);
}

void wxTreeListMainWindow::RefreshSelected() {
    // TODO: this is awfully inefficient, we should keep the list of all
    //       selected items internally, should be much faster
    if (m_rootItem) {
        RefreshSelectedUnder (m_rootItem);
    }
}

void wxTreeListMainWindow::RefreshSelectedUnder (wxTreeListItem *item) {
    if (item->IsSelected()) {
        RefreshLine (item);
    }

    const wxArrayTreeListItems& children = item->GetChildren();
    long count = children.GetCount();
    for (long n = 0; n < count; n++ ) {
        RefreshSelectedUnder (children[n]);
    }
}

// ----------------------------------------------------------------------------
// changing colours: we need to refresh the tree control
// ----------------------------------------------------------------------------

bool wxTreeListMainWindow::SetBackgroundColour (const wxColour& colour) {
    if (!wxWindow::SetBackgroundColour(colour)) return false;

    Refresh();
    return true;
}

bool wxTreeListMainWindow::SetForegroundColour (const wxColour& colour) {
    if (!wxWindow::SetForegroundColour(colour)) return false;

    Refresh();
    return true;
}

void wxTreeListMainWindow::SetItemText (const wxTreeItemId& itemId, int column,
                                        const wxString& text) {
    wxCHECK_RET (itemId.IsOk(), _T("invalid tree item"));

    wxClientDC dc (this);
    wxTreeListItem *item = (wxTreeListItem*) itemId.m_pItem;
    item->SetText (column, text);
    CalculateSize (item, dc);
    RefreshLine (item);
}

wxString wxTreeListMainWindow::GetItemText (const wxTreeItemId& itemId,
                                            int column) const {
    wxCHECK_MSG (itemId.IsOk(), _T(""), _T("invalid tree item") );

    if( IsVirtual() )   return m_owner->OnGetItemText(((wxTreeListItem*) itemId.m_pItem)->GetData(),column);
    else                return ((wxTreeListItem*) itemId.m_pItem)->GetText (column);
}

wxString wxTreeListMainWindow::GetItemText (wxTreeItemData* item,
int column) const {
   wxASSERT_MSG( IsVirtual(), _T("can be used only with virtual control") );
   return m_owner->OnGetItemText(item,column);
}

void wxTreeListMainWindow::SetFocus() {
    wxWindow::SetFocus();
}

wxFont wxTreeListMainWindow::GetItemFont (wxTreeListItem *item) {
    wxTreeItemAttr *attr = item->GetAttributes();

    if (attr && attr->HasFont()) {
        return attr->GetFont();
    }else if (item->IsBold()) {
        return m_boldFont;
    }else{
        return m_normalFont;
   }
}

int wxTreeListMainWindow::GetItemWidth (int column, wxTreeListItem *item) {
    if (!item) return 0;

    // determine item width
    int w = 0, h = 0;
    wxFont font = GetItemFont (item);
    GetTextExtent (item->GetText (column), &w, &h, NULL, NULL, font.Ok()? &font: NULL);
    w += 2*MARGIN;

    // calculate width
    int width = w + 2*MARGIN;
    if (column == GetMainColumn()) {
        width += MARGIN;
        if (HasFlag(wxTR_LINES_AT_ROOT)) width += LINEATROOT;
        if (HasButtons()) width += m_btnWidth + LINEATROOT;
        if (item->GetCurrentImage() != NO_IMAGE) width += m_imgWidth;

        // count indent level
        int level = 0;
        wxTreeListItem *parent = item->GetItemParent();
        wxTreeListItem *root = (wxTreeListItem*)GetRootItem().m_pItem;
        while (parent && (!HasFlag(wxTR_HIDE_ROOT) || (parent != root))) {
            level++;
            parent = parent->GetItemParent();
        }
        if (level) width += level * GetIndent();
    }

    return width;
}

int wxTreeListMainWindow::GetBestColumnWidth (int column, wxTreeItemId parent) {
    int maxWidth, h;
    GetClientSize (&maxWidth, &h);
    int width = 0;

    // get root if on item
    if (!parent.IsOk()) parent = GetRootItem();

    // add root width
    if (!HasFlag(wxTR_HIDE_ROOT)) {
        int w = GetItemWidth (column, (wxTreeListItem*)parent.m_pItem);
        if (width < w) width = w;
        if (width > maxWidth) return maxWidth;
    }

    wxTreeItemIdValue cookie = 0;
    wxTreeItemId item = GetFirstChild (parent, cookie);
    while (item.IsOk()) {
        int w = GetItemWidth (column, (wxTreeListItem*)item.m_pItem);
        if (width < w) width = w;
        if (width > maxWidth) return maxWidth;

        // check the children of this item
        if (((wxTreeListItem*)item.m_pItem)->IsExpanded()) {
            int w = GetBestColumnWidth (column, item);
            if (width < w) width = w;
            if (width > maxWidth) return maxWidth;
        }

        // next sibling
        item = GetNextChild (parent, cookie);
    }

    return width;
}


//-----------------------------------------------------------------------------
//  wxTreeListCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeListCtrl, wxControl);

BEGIN_EVENT_TABLE(wxTreeListCtrl, wxControl)
    EVT_SIZE(wxTreeListCtrl::OnSize)
END_EVENT_TABLE();

bool wxTreeListCtrl::Create(wxWindow *parent, wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style, const wxValidator &validator,
                            const wxString& name)
{
    long main_style = style & ~(wxSIMPLE_BORDER|wxSUNKEN_BORDER|wxDOUBLE_BORDER|
                                wxRAISED_BORDER|wxSTATIC_BORDER);
    long ctrl_style = style & ~(wxVSCROLL|wxHSCROLL);

    if (!wxControl::Create(parent, id, pos, size, ctrl_style, validator, name)) {
       return false;
    }
    m_main_win = new wxTreeListMainWindow (this, -1, wxPoint(0, 0), size,
                                           main_style, validator);
    m_header_win = new wxTreeListHeaderWindow (this, -1, m_main_win,
                                               wxPoint(0, 0), wxDefaultSize,
                                               wxTAB_TRAVERSAL);
    CalculateAndSetHeaderHeight();
    return true;
}

void wxTreeListCtrl::CalculateAndSetHeaderHeight()
{
    if (m_header_win) {

        // we use 'g' to get the descent, too
        int w, h, d;
        m_header_win->GetTextExtent(_T("Hg"), &w, &h, &d);
        h += d + 2 * HEADER_OFFSET_Y + EXTRA_HEIGHT;

        // only update if changed
        if (h != m_headerHeight) {
            m_headerHeight = h;
            DoHeaderLayout();
        }
    }
}

void wxTreeListCtrl::DoHeaderLayout()
{
    int w, h;
    GetClientSize(&w, &h);
    if (m_header_win) {
        m_header_win->SetSize (0, 0, w, m_headerHeight);
        m_header_win->Refresh();
    }
    if (m_main_win) {
        m_main_win->SetSize (0, m_headerHeight + 1, w, h - m_headerHeight - 1);
    }
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

wxString wxTreeListCtrl::GetItemText(const wxTreeItemId& item, int column) const
{ return m_main_win->GetItemText (item, column); }

int wxTreeListCtrl::GetItemImage(const wxTreeItemId& item, int column,
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


void wxTreeListCtrl::SetItemText(const wxTreeItemId& item, int column,
                                 const wxString& text)
{ m_main_win->SetItemText (item, column, text); }

void wxTreeListCtrl::SetItemImage(const wxTreeItemId& item,
                                  int column,
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
                                       const wxColour& colour)
{ m_main_win->SetItemTextColour(item, colour); }

void wxTreeListCtrl::SetItemBackgroundColour(const wxTreeItemId& item,
                                             const wxColour& colour)
{ m_main_win->SetItemBackgroundColour(item, colour); }

void wxTreeListCtrl::SetItemFont(const wxTreeItemId& item,
                                 const wxFont& font)
{ m_main_win->SetItemFont(item, font); }

bool wxTreeListCtrl::SetFont(const wxFont& font)
{
    if (m_header_win) {
        m_header_win->SetFont(font);
        CalculateAndSetHeaderHeight();
        m_header_win->Refresh();
    }
    if (m_main_win) {
        return m_main_win->SetFont(font);
    }else{
        return false;
    }
}

void wxTreeListCtrl::SetWindowStyle(const long style)
{
    if(m_main_win)
        m_main_win->SetWindowStyle(style);
    m_windowStyle = style;
    // TODO: provide something like wxTL_NO_HEADERS to hide m_header_win
}

long wxTreeListCtrl::GetWindowStyle() const
{
    long style = m_windowStyle;
    if(m_main_win)
        style |= m_main_win->GetWindowStyle();
    return style;
}

bool wxTreeListCtrl::IsVisible(const wxTreeItemId& item, bool fullRow) const
{ return m_main_win->IsVisible(item, fullRow); }

bool wxTreeListCtrl::HasChildren(const wxTreeItemId& item) const
{ return m_main_win->HasChildren(item); }

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
wxTreeItemId wxTreeListCtrl::GetFirstChild (const wxTreeItemId& item,
                                            long& cookie) const
#else
wxTreeItemId wxTreeListCtrl::GetFirstChild (const wxTreeItemId& item,
                                            wxTreeItemIdValue& cookie) const
#endif
{ return m_main_win->GetFirstChild(item, cookie); }

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListCtrl::GetNextChild (const wxTreeItemId& item,
                                           long& cookie) const
#else
wxTreeItemId wxTreeListCtrl::GetNextChild (const wxTreeItemId& item,
                                           wxTreeItemIdValue& cookie) const
#endif
{ return m_main_win->GetNextChild(item, cookie); }

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListCtrl::GetPrevChild (const wxTreeItemId& item,
                                           long& cookie) const
#else
wxTreeItemId wxTreeListCtrl::GetPrevChild (const wxTreeItemId& item,
                                           wxTreeItemIdValue& cookie) const
#endif
{ return m_main_win->GetPrevChild(item, cookie); }

#if !wxCHECK_VERSION(2, 5, 0)
wxTreeItemId wxTreeListCtrl::GetLastChild (const wxTreeItemId& item,
                                           long& cookie) const
#else
wxTreeItemId wxTreeListCtrl::GetLastChild (const wxTreeItemId& item,
                                           wxTreeItemIdValue& cookie) const
#endif
{ return m_main_win->GetLastChild(item, cookie); }


wxTreeItemId wxTreeListCtrl::GetNextSibling(const wxTreeItemId& item) const
{ return m_main_win->GetNextSibling(item); }

wxTreeItemId wxTreeListCtrl::GetPrevSibling(const wxTreeItemId& item) const
{ return m_main_win->GetPrevSibling(item); }

wxTreeItemId wxTreeListCtrl::GetNext(const wxTreeItemId& item) const
{ return m_main_win->GetNext(item, true); }

wxTreeItemId wxTreeListCtrl::GetPrev(const wxTreeItemId& item) const
{ return m_main_win->GetPrev(item, true); }

wxTreeItemId wxTreeListCtrl::GetFirstExpandedItem() const
{ return m_main_win->GetFirstExpandedItem(); }

wxTreeItemId wxTreeListCtrl::GetNextExpanded(const wxTreeItemId& item) const
{ return m_main_win->GetNextExpanded(item); }

wxTreeItemId wxTreeListCtrl::GetPrevExpanded(const wxTreeItemId& item) const
{ return m_main_win->GetPrevExpanded(item); }

wxTreeItemId wxTreeListCtrl::GetFirstVisibleItem(bool fullRow) const
{ return m_main_win->GetFirstVisibleItem(fullRow); }

wxTreeItemId wxTreeListCtrl::GetNextVisible(const wxTreeItemId& item, bool fullRow) const
{ return m_main_win->GetNextVisible(item, fullRow); }

wxTreeItemId wxTreeListCtrl::GetPrevVisible(const wxTreeItemId& item, bool fullRow) const
{ return m_main_win->GetPrevVisible(item, fullRow); }

wxTreeItemId wxTreeListCtrl::AddRoot (const wxString& text, int image,
                                      int selectedImage, wxTreeItemData* data)
{ return m_main_win->AddRoot (text, image, selectedImage, data); }

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

void wxTreeListCtrl::DeleteRoot()
{ m_main_win->DeleteRoot(); }

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

void wxTreeListCtrl::SelectItem(const wxTreeItemId& item, const wxTreeItemId& last,
                                bool unselect_others)
{ m_main_win->SelectItem (item, last, unselect_others); }

void wxTreeListCtrl::SelectAll()
{ m_main_win->SelectAll(); }

void wxTreeListCtrl::EnsureVisible(const wxTreeItemId& item)
{ m_main_win->EnsureVisible(item); }

void wxTreeListCtrl::ScrollTo(const wxTreeItemId& item)
{ m_main_win->ScrollTo(item); }

wxTreeItemId wxTreeListCtrl::HitTest(const wxPoint& pos, int& flags, int& column)
{
    wxPoint p = m_main_win->ScreenToClient (ClientToScreen (pos));
    return m_main_win->HitTest (p, flags, column);
}

bool wxTreeListCtrl::GetBoundingRect(const wxTreeItemId& item, wxRect& rect,
                                     bool textOnly) const
{ return m_main_win->GetBoundingRect(item, rect, textOnly); }

void wxTreeListCtrl::EditLabel (const wxTreeItemId& item, int column)
{ m_main_win->EditLabel (item, column); }

int wxTreeListCtrl::OnCompareItems(const wxTreeItemId& item1,
                                   const wxTreeItemId& item2)
{
    // do the comparison here, and not delegate to m_main_win, in order
    // to let the user override it
    //return m_main_win->OnCompareItems(item1, item2);
    return wxStrcmp(GetItemText(item1), GetItemText(item2));
}

void wxTreeListCtrl::SortChildren(const wxTreeItemId& item)
{ m_main_win->SortChildren(item); }

wxTreeItemId wxTreeListCtrl::FindItem (const wxTreeItemId& item, const wxString& str, int mode)
{ return m_main_win->FindItem (item, str, mode); }

void wxTreeListCtrl::SetDragItem (const wxTreeItemId& item)
{ m_main_win->SetDragItem (item); }

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

int wxTreeListCtrl::GetColumnCount() const
{ return m_main_win->GetColumnCount(); }

void wxTreeListCtrl::SetColumnWidth(int column, int width)
{
    m_header_win->SetColumnWidth (column, width);
    m_header_win->Refresh();
}

int wxTreeListCtrl::GetColumnWidth(int column) const
{ return m_header_win->GetColumnWidth(column); }

void wxTreeListCtrl::SetMainColumn(int column)
{ m_main_win->SetMainColumn(column); }

int wxTreeListCtrl::GetMainColumn() const
{ return m_main_win->GetMainColumn(); }

void wxTreeListCtrl::SetColumnText(int column, const wxString& text)
{
    m_header_win->SetColumnText (column, text);
    m_header_win->Refresh();
}

wxString wxTreeListCtrl::GetColumnText(int column) const
{ return m_header_win->GetColumnText(column); }

void wxTreeListCtrl::AddColumn(const wxTreeListColumnInfo& colInfo)
{
    m_header_win->AddColumn (colInfo);
    DoHeaderLayout();
}

void wxTreeListCtrl::InsertColumn(int before, const wxTreeListColumnInfo& colInfo)
{
    m_header_win->InsertColumn (before, colInfo);
    m_header_win->Refresh();
}

void wxTreeListCtrl::RemoveColumn(int column)
{
    m_header_win->RemoveColumn (column);
    m_header_win->Refresh();
}

void wxTreeListCtrl::SetColumn(int column, const wxTreeListColumnInfo& colInfo)
{
    m_header_win->SetColumn (column, colInfo);
    m_header_win->Refresh();
}

const wxTreeListColumnInfo& wxTreeListCtrl::GetColumn(int column) const
{ return m_header_win->GetColumn(column); }

wxTreeListColumnInfo& wxTreeListCtrl::GetColumn(int column)
{ return m_header_win->GetColumn(column); }

void wxTreeListCtrl::SetColumnImage(int column, int image)
{
    m_header_win->SetColumn (column, GetColumn(column).SetImage(image));
    m_header_win->Refresh();
}

int wxTreeListCtrl::GetColumnImage(int column) const
{
    return m_header_win->GetColumn(column).GetImage();
}

void wxTreeListCtrl::SetColumnEditable(int column, bool shown)
{
    m_header_win->SetColumn (column, GetColumn(column).SetEditable(shown));
}

void wxTreeListCtrl::SetColumnShown(int column, bool shown)
{
    wxASSERT_MSG (column != GetMainColumn(), _T("The main column may not be hidden") );
    m_header_win->SetColumn (column, GetColumn(column).SetShown(GetMainColumn()==column? true: shown));
    m_header_win->Refresh();
}

bool wxTreeListCtrl::IsColumnEditable(int column) const
{
    return m_header_win->GetColumn(column).IsEditable();
}

bool wxTreeListCtrl::IsColumnShown(int column) const
{
    return m_header_win->GetColumn(column).IsShown();
}

void wxTreeListCtrl::SetColumnAlignment (int column, int flag)
{
    m_header_win->SetColumn(column, GetColumn(column).SetAlignment(flag));
    m_header_win->Refresh();
}

int wxTreeListCtrl::GetColumnAlignment(int column) const
{
    return m_header_win->GetColumn(column).GetAlignment();
}

void wxTreeListCtrl::Refresh(bool erase, const wxRect* rect)
{
    m_main_win->Refresh (erase, rect);
    m_header_win->Refresh (erase, rect);
}

void wxTreeListCtrl::SetFocus()
{ m_main_win->SetFocus(); }

wxSize wxTreeListCtrl::DoGetBestSize() const
{
    // something is better than nothing...
    return wxSize (200,200); // but it should be specified values! FIXME
}

wxString wxTreeListCtrl::OnGetItemText( wxTreeItemData* WXUNUSED(item), long WXUNUSED(column)) const
{
    return wxEmptyString;
}
