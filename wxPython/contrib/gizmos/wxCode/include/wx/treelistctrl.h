/////////////////////////////////////////////////////////////////////////////
// Name:        treelistctrl.h
// Purpose:     wxTreeListCtrl class
// Author:      Robert Roebling
// Modified by: Alberto Griggio, 2002
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Julian Smart, Alberto Griggio,
//              Vadim Zeitlin, Otto Wyss
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#ifndef TREELISTCTRL_H
#define TREELISTCTRL_H

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "treelistctrl.h"
#endif

#include <wx/treectrl.h>
#include <wx/control.h>
#include <wx/pen.h>
#include <wx/listctrl.h> // for wxListEvent

#ifdef GIZMOISDLL
#define GIZMODLLEXPORT WXDLLEXPORT
#else
#define GIZMODLLEXPORT
#endif


class GIZMODLLEXPORT wxTreeListItem;
class GIZMODLLEXPORT wxTreeListHeaderWindow;
class GIZMODLLEXPORT wxTreeListMainWindow;


// Using this typedef removes an ambiguity when calling Remove()
#ifdef __WXMSW__
#if !wxCHECK_VERSION(2, 5, 0)
typedef long wxTreeItemIdValue;
#else
typedef void *wxTreeItemIdValue;
#endif
#endif


#define wxTR_DONT_ADJUST_MAC    0x0100          // Don't adjust the style for the Mac

//-----------------------------------------------------------------------------
// wxTreeListColumnAttrs
//-----------------------------------------------------------------------------

enum wxTreeListColumnAlign {
    wxTL_ALIGN_LEFT,
    wxTL_ALIGN_RIGHT,
    wxTL_ALIGN_CENTER
};


class GIZMODLLEXPORT wxTreeListColumnInfo: public wxObject {
public:
    enum { DEFAULT_COL_WIDTH = 100 };

    wxTreeListColumnInfo(const wxString &text = wxT(""),
                         int image = -1,
                         size_t width = DEFAULT_COL_WIDTH,
                         bool shown = true,
                         wxTreeListColumnAlign alignment = wxTL_ALIGN_LEFT)
    {
        m_image = image;
        m_selected_image = -1;
        m_text = text;
        m_width = width;
        m_shown = shown;
        m_alignment = alignment;
    }

    wxTreeListColumnInfo(const wxTreeListColumnInfo& other)
    {
        m_image = other.m_image;
        m_selected_image = other.m_selected_image;
        m_text = other.m_text;
        m_width = other.m_width;
        m_shown = other.m_shown;
        m_alignment = other.m_alignment;
    }

    ~wxTreeListColumnInfo() {}

    // getters
    bool GetShown() const { return m_shown; }
    wxTreeListColumnAlign GetAlignment() const { return m_alignment; }
    wxString GetText() const { return m_text; }
    int GetImage() const { return m_image; }
    int GetSelectedImage() const { return m_selected_image; }
    size_t GetWidth() const { return m_width; }

    // setters
    wxTreeListColumnInfo& SetShown(bool shown)
    { m_shown = shown; return *this; }

    wxTreeListColumnInfo& SetAlignment(wxTreeListColumnAlign alignment)
    { m_alignment = alignment; return *this; }

    wxTreeListColumnInfo& SetText(const wxString& text)
    { m_text = text; return *this; }

    wxTreeListColumnInfo& SetImage(int image)
    { m_image = image; return *this; }

    wxTreeListColumnInfo& SetSelectedImage(int image)
    { m_selected_image = image; return *this; }

    wxTreeListColumnInfo& SetWidth(size_t with)
    { m_width = with; return *this; }

private:
    bool m_shown;
    wxTreeListColumnAlign m_alignment;
    wxString m_text;
    int m_image;
    int m_selected_image;
    size_t m_width;
};

//----------------------------------------------------------------------------
// wxTreeListCtrl - the multicolumn tree control
//----------------------------------------------------------------------------

// flags for FindItem
const int wxTL_SEARCH_VISIBLE = 0x0000;
const int wxTL_SEARCH_LEVEL   = 0x0001;
const int wxTL_SEARCH_FULL    = 0x0002;
const int wxTL_SEARCH_PARTIAL = 0x0010;
const int wxTL_SEARCH_NOCASE  = 0x0020;

// additional flag for HitTest
const int wxTREE_HITTEST_ONITEMCOLUMN = 0x2000;
extern GIZMODLLEXPORT const wxChar* wxTreeListCtrlNameStr;


class GIZMODLLEXPORT wxTreeListCtrl : public wxControl
{
public:
    // creation
    // --------
    wxTreeListCtrl()
        : m_header_win(0), m_main_win(0), m_headerHeight(0)
    {}

    wxTreeListCtrl(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_DEFAULT_STYLE,
               const wxValidator &validator = wxDefaultValidator,
               const wxString& name = wxTreeListCtrlNameStr )
        : m_header_win(0), m_main_win(0), m_headerHeight(0)
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    virtual ~wxTreeListCtrl() {}

    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_DEFAULT_STYLE,
                const wxValidator &validator = wxDefaultValidator,
                const wxString& name = wxTreeListCtrlNameStr );

    void Refresh(bool erase=TRUE, const wxRect* rect=NULL);
    void SetFocus();
    // accessors
    // ---------

    // get the total number of items in the control
    size_t GetCount() const;

    // indent is the number of pixels the children are indented relative to
    // the parents position. SetIndent() also redraws the control
    // immediately.
    unsigned int GetIndent() const;
    void SetIndent(unsigned int indent);

    // line spacing is the space above and below the text on each line
    unsigned int GetLineSpacing() const;
    void SetLineSpacing(unsigned int spacing);

    // image list: these functions allow to associate an image list with
    // the control and retrieve it. Note that when assigned with
    // SetImageList, the control does _not_ delete
    // the associated image list when it's deleted in order to allow image
    // lists to be shared between different controls. If you use
    // AssignImageList, the control _does_ delete the image list.
    //
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


    // Functions to work with tree list ctrl columns

    // adds a column
    void AddColumn(const wxString& text)
        { AddColumn(wxTreeListColumnInfo(text)); }
    void AddColumn(const wxString& text,
                   size_t width,
                   wxTreeListColumnAlign alignment = wxTL_ALIGN_LEFT)
        { AddColumn(wxTreeListColumnInfo(text,
                                         -1,
                                         width,
                                         true,
                                         alignment)); }
    void AddColumn(const wxTreeListColumnInfo& col);

    // inserts a column before the given one
    void InsertColumn(size_t before, const wxString& text)
    { InsertColumn(before, wxTreeListColumnInfo(text)); }
    void InsertColumn(size_t before, const wxTreeListColumnInfo& col);

    // deletes the given column - does not delete the corresponding column
    // of each item
    void RemoveColumn(size_t column);

    // returns the number of columns in the ctrl
    size_t GetColumnCount() const;

    void SetColumnWidth(size_t column, size_t width);
    int GetColumnWidth(size_t column) const;

    // tells which column is the "main" one, i.e. the "threaded" one
    void SetMainColumn(size_t column);
    size_t GetMainColumn() const;

    void SetColumnText(size_t column, const wxString& text);
    wxString GetColumnText(size_t column) const;

    void SetColumn(size_t column, const wxTreeListColumnInfo& info);
    wxTreeListColumnInfo& GetColumn(size_t column);
    const wxTreeListColumnInfo& GetColumn(size_t column) const;

    // other column-related methods
    void SetColumnAlignment(size_t column, wxTreeListColumnAlign align);
    wxTreeListColumnAlign GetColumnAlignment(size_t column) const;

    void SetColumnImage(size_t column, int image);
    int GetColumnImage(size_t column) const;

    void ShowColumn(size_t column, bool shown);
    bool IsColumnShown(size_t column) const;

    // Functions to work with tree list ctrl items.

    // accessors
    // ---------

    // retrieve item's label (of the main column)
    wxString GetItemText(const wxTreeItemId& item) const
        { return GetItemText(item, GetMainColumn()); }
    // retrieves item's label of the given column
    wxString GetItemText(const wxTreeItemId& item, size_t column) const;

    // get one of the images associated with the item (normal by default)
    int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const
    { return GetItemImage(item, GetMainColumn(), which); }
    int GetItemImage(const wxTreeItemId& item, size_t column,
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
    void SetItemText(const wxTreeItemId& item, const wxString& text)
    { SetItemText(item, GetMainColumn(), text); }
    void SetItemText(const wxTreeItemId& item, size_t column,
                     const wxString& text);

    // get one of the images associated with the item (normal by default)
    void SetItemImage(const wxTreeItemId& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal)
    { SetItemImage(item, GetMainColumn(), image, which); }
    // the which parameter is ignored for all columns but the main one
    void SetItemImage(const wxTreeItemId& item, size_t column, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);

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

    // set the styles.
    void SetWindowStyle(const long styles);
    long GetWindowStyle() const;
    long GetWindowStyleFlag() const { return GetWindowStyle(); }

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
    wxTreeItemId GetRootItem() const;

    // get the item currently selected (may return NULL if no selection)
    wxTreeItemId GetSelection() const;

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
    //void AdjustMyScrollbars();

    // The first function is more portable (because easier to implement
    // on other platforms), but the second one returns some extra info.
    wxTreeItemId HitTest(const wxPoint& point)
        { int dummy; return HitTest(point, dummy); }
    wxTreeItemId HitTest(const wxPoint& point, int& flags)
    { int col; return HitTest(point, flags, col); }
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

    // overridden base class virtuals
    virtual bool SetBackgroundColour(const wxColour& colour);
    virtual bool SetForegroundColour(const wxColour& colour);


    wxTreeListHeaderWindow* GetHeaderWindow() const
    { return m_header_win; }

    wxTreeListMainWindow* GetMainWindow() const
    { return m_main_win; }

    virtual wxSize DoGetBestSize() const;
    
protected:
    // header window, responsible for column visualization and manipulation
    wxTreeListHeaderWindow* m_header_win;
    // main window, the "true" tree ctrl
    wxTreeListMainWindow*   m_main_win;

//     // the common part of all ctors
//     void Init();

    void OnGetToolTip( wxTreeEvent &event );
    void OnSize(wxSizeEvent& event);
    void CalculateAndSetHeaderHeight();
    void DoHeaderLayout();

private:
    size_t fill_column;
    size_t m_headerHeight;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTreeListCtrl)
};

#endif // TREELISTCTRL_H

