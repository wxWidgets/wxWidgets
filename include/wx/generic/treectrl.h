/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:     wxTreeCtrl class
// Author:      Robert Roebling
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997,1998 Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_TREECTRL_H_
#define _GENERIC_TREECTRL_H_

#ifdef __GNUG__
    #pragma interface "treectrl.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/object.h"
#include "wx/event.h"
#include "wx/scrolwin.h"
#include "wx/textctrl.h"
#include "wx/pen.h"
#include "wx/dynarray.h"
#include "wx/timer.h"

// -----------------------------------------------------------------------------
// forward declaration
// -----------------------------------------------------------------------------

class WXDLLEXPORT wxImageList;
class WXDLLEXPORT wxGenericTreeItem;

class WXDLLEXPORT wxTreeItemData;

class WXDLLEXPORT wxTreeRenameTimer;
class WXDLLEXPORT wxTreeTextCtrl;

// -----------------------------------------------------------------------------
// wxTreeItemId - unique identifier of a tree element
// -----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeItemId
{
friend class wxTreeCtrl;
friend class wxTreeEvent;
public:
    // ctors
        // 0 is invalid value for HTREEITEM
    wxTreeItemId() { m_pItem = 0; }

        // default copy ctor/assignment operator are ok for us

    // accessors
        // is this a valid tree item?
    bool IsOk() const { return m_pItem != 0; }

    // deprecated: only for compatibility
    wxTreeItemId(long itemId) { m_pItem = (wxGenericTreeItem *)itemId; }
    operator long() const { return (long)m_pItem; }

//protected: // not for gcc
    // for wxTreeCtrl usage only
    wxTreeItemId(wxGenericTreeItem *pItem) { m_pItem = pItem; }

    wxGenericTreeItem *m_pItem;
};

WX_DECLARE_OBJARRAY(wxTreeItemId, wxArrayTreeItemIds);

// ----------------------------------------------------------------------------
// wxTreeItemData is some (arbitrary) user class associated with some item.
//
// Because the objects of this class are deleted by the tree, they should
// always be allocated on the heap!
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeItemData: public wxClientData
{
friend class wxTreeCtrl;
public:
    // creation/destruction
    // --------------------
        // default ctor
    wxTreeItemData() { }

        // default copy ctor/assignment operator are ok

    // accessor: get the item associated with us
    const wxTreeItemId& GetId() const { return m_pItem; }
    void SetId(const wxTreeItemId& id) { m_pItem = id; }

protected:
    wxTreeItemId m_pItem;
};

//-----------------------------------------------------------------------------
// wxTreeRenameTimer (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeRenameTimer: public wxTimer
{
 private:
   wxTreeCtrl   *m_owner;

 public:
   wxTreeRenameTimer( wxTreeCtrl *owner );
   void Notify();
};

//-----------------------------------------------------------------------------
//  wxTreeTextCtrl (internal)
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeTextCtrl: public wxTextCtrl
{
  DECLARE_DYNAMIC_CLASS(wxTreeTextCtrl);

  private:
    bool               *m_accept;
    wxString           *m_res;
    wxTreeCtrl         *m_owner;
    wxString            m_startValue;

  public:
    wxTreeTextCtrl(void) {};
    wxTreeTextCtrl( wxWindow *parent, const wxWindowID id,
                    bool *accept, wxString *res, wxTreeCtrl *owner,
                    const wxString &value = wxEmptyString,
                    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
                    int style = 0,
#if wxUSE_VALIDATORS
                    const wxValidator& validator = wxDefaultValidator,
#endif
                    const wxString &name = wxTextCtrlNameStr );
    void OnChar( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );

  DECLARE_EVENT_TABLE()
};

// -----------------------------------------------------------------------------
// wxTreeCtrl - the tree control
// -----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeCtrl : public wxScrolledWindow
{
public:
    // creation
    // --------
    wxTreeCtrl() { Init(); }

    wxTreeCtrl(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
#if wxUSE_VALIDATORS
               const wxValidator &validator = wxDefaultValidator,
#endif
               const wxString& name = wxTreeCtrlNameStr)
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    virtual ~wxTreeCtrl();

    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
#if wxUSE_VALIDATORS
                const wxValidator &validator = wxDefaultValidator,
#endif
                const wxString& name = wxTreeCtrlNameStr);

    // accessors
    // ---------

        // get the total number of items in the control
    size_t GetCount() const;

        // indent is the number of pixels the children are indented relative to
        // the parents position. SetIndent() also redraws the control
        // immediately.
    unsigned int GetIndent() const { return m_indent; }
    void SetIndent(unsigned int indent);

        // spacing is the number of pixels between the start and the Text
    unsigned int GetSpacing() const { return m_spacing; }
    void SetSpacing(unsigned int spacing);

        // image list: these functions allow to associate an image list with
        // the control and retrieve it. Note that the control does _not_ delete
        // the associated image list when it's deleted in order to allow image
        // lists to be shared between different controls.
        //
        // The normal image list is for the icons which correspond to the
        // normal tree item state (whether it is selected or not).
        // Additionally, the application might choose to show a state icon
        // which corresponds to an app-defined item state (for example,
        // checked/unchecked) which are taken from the state image list.
    wxImageList *GetImageList() const;
    wxImageList *GetStateImageList() const;

    void SetImageList(wxImageList *imageList);
    void SetStateImageList(wxImageList *imageList);

    // Functions to work with tree ctrl items.

    // accessors
    // ---------

        // retrieve items label
    wxString GetItemText(const wxTreeItemId& item) const;
        // get one of the images associated with the item (normal by default)
    int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const;
        // get the data associated with the item
    wxTreeItemData *GetItemData(const wxTreeItemId& item) const;

    // modifiers
    // ---------

        // set items label
    void SetItemText(const wxTreeItemId& item, const wxString& text);
        // get one of the images associated with the item (normal by default)
    void SetItemImage(const wxTreeItemId& item, int image,
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

        // set the items text colour
    void SetItemTextColour(const wxTreeItemId& item, const wxColour& col);

        // set the items background colour
    void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col);

        // set the items font (should be of the same height for all items)
    void SetItemFont(const wxTreeItemId& item, const wxFont& font);

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
    wxTreeItemId GetParent(const wxTreeItemId& item) const;

        // for this enumeration function you must pass in a "cookie" parameter
        // which is opaque for the application but is necessary for the library
        // to make these functions reentrant (i.e. allow more than one
        // enumeration on one and the same object simultaneously). Of course,
        // the "cookie" passed to GetFirstChild() and GetNextChild() should be
        // the same!

        // get the first child of this item
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& cookie) const;
        // get the next child
    wxTreeItemId GetNextChild(const wxTreeItemId& item, long& cookie) const;
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

        // insert a new item before a given one
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            size_t before,
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
    void SelectItem(const wxTreeItemId& item, bool unselect_others=TRUE, bool extended_select=FALSE);
        // make sure this item is visible (expanding the parent item and/or
        // scrolling to this item if necessary)
    void EnsureVisible(const wxTreeItemId& item);
        // scroll to this item (but don't expand its parent)
    void ScrollTo(const wxTreeItemId& item);

        // The first function is more portable (because easier to implement
        // on other platforms), but the second one returns some extra info.
    wxTreeItemId HitTest(const wxPoint& point)
        { int dummy; return HitTest(point, dummy); }
    wxTreeItemId HitTest(const wxPoint& point, int& flags);

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

    // deprecated functions: use Set/GetItemImage directly
        // get the selected item image
    int GetItemSelectedImage(const wxTreeItemId& item) const
        { return GetItemImage(item, wxTreeItemIcon_Selected); }
        // set the selected item image
    void SetItemSelectedImage(const wxTreeItemId& item, int image)
        { SetItemImage(item, image, wxTreeItemIcon_Selected); }

    // implementation

    // callbacks
    void OnPaint( wxPaintEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnIdle( wxIdleEvent &event );

    // implementation
    void SendDeleteEvent(wxGenericTreeItem *itemBeingDeleted);

    // Draw Special Information
    void DrawBorder(wxTreeItemId& item);
    void DrawLine(wxTreeItemId& item, bool below);

protected:
    friend class wxGenericTreeItem;
    friend class wxTreeRenameTimer;
    friend class wxTreeTextCtrl;

    wxFont               m_normalFont;
    wxFont               m_boldFont;

    wxGenericTreeItem   *m_anchor;
    wxGenericTreeItem   *m_current, *m_key_current, *m_currentEdit;
    bool                 m_hasFocus;
    bool                 m_dirty;
    int                  m_xScroll,m_yScroll;
    unsigned int         m_indent;
    unsigned int         m_spacing;
    int                  m_lineHeight;
    wxPen                m_dottedPen;
    wxBrush             *m_hilightBrush;
    wxImageList         *m_imageListNormal,
                        *m_imageListState;
    int                  m_dragCount;
    wxPoint              m_dragStart;
    wxTimer             *m_renameTimer;
    bool                 m_renameAccept;
    wxString             m_renameRes;

    // the common part of all ctors
    void Init();

    // misc helpers
    wxTreeItemId DoInsertItem(const wxTreeItemId& parent,
                              size_t previous,
                              const wxString& text,
                              int image, int selectedImage,
                              wxTreeItemData *data);

    void AdjustMyScrollbars();
    int  GetLineHeight(wxGenericTreeItem *item) const;
    void PaintLevel( wxGenericTreeItem *item, wxDC& dc, int level, int &y );
    void PaintItem( wxGenericTreeItem *item, wxDC& dc);

    void CalculateLevel( wxGenericTreeItem *item, wxDC &dc, int level, int &y );
    void CalculatePositions();
    void CalculateSize( wxGenericTreeItem *item, wxDC &dc );

    void RefreshSubtree( wxGenericTreeItem *item );
    void RefreshLine( wxGenericTreeItem *item );

    void OnRenameTimer();
    void OnRenameAccept();

    void FillArray(wxGenericTreeItem*, wxArrayTreeItemIds&) const;
    void SelectItemRange( wxGenericTreeItem *item1, wxGenericTreeItem *item2 );
    bool TagAllChildrenUntilLast(wxGenericTreeItem *crt_item, wxGenericTreeItem *last_item, bool select);
    bool TagNextChildren(wxGenericTreeItem *crt_item, wxGenericTreeItem *last_item, bool select);
    void UnselectAllChildren( wxGenericTreeItem *item );

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTreeCtrl)
};

#endif // _GENERIC_TREECTRL_H_

