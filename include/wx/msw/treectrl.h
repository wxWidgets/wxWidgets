/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:     wxTreeCtrl class
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to be less MSW-specific on 10/10/98
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TREECTRL_H_
#define _WX_TREECTRL_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma interface "treectrl.h"
#endif

#include "wx/control.h"
#include "wx/event.h"

// the type for "untyped" data
typedef long wxDataType;

// fwd decl
class  WXDLLEXPORT wxImageList;
struct WXDLLEXPORT wxTreeViewItem;

// a callback function used for sorting tree items, it should return -1 if the
// first item precedes the second, +1 if the second precedes the first or 0 if
// they're equivalent
class wxTreeItemData;
typedef int (*wxTreeItemCmpFunc)(wxTreeItemData *item1, wxTreeItemData *item2);

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// values for the `flags' parameter of wxTreeCtrl::HitTest() which determine
// where exactly the specified point is situated:
    // above the client area.
static const int wxTREE_HITTEST_ABOVE            = 0x0001;
    // below the client area.
static const int wxTREE_HITTEST_BELOW            = 0x0002;
    // in the client area but below the last item.
static const int wxTREE_HITTEST_NOWHERE          = 0x0004;
    // on the button associated with an item.
static const int wxTREE_HITTEST_ONITEMBUTTON     = 0x0010;
    // on the bitmap associated with an item.
static const int wxTREE_HITTEST_ONITEMICON       = 0x0020;
    // in the indentation associated with an item.
static const int wxTREE_HITTEST_ONITEMINDENT     = 0x0040;
    // on the label (string) associated with an item.
static const int wxTREE_HITTEST_ONITEMLABEL      = 0x0080;
    // in the area to the right of an item.
static const int wxTREE_HITTEST_ONITEMRIGHT      = 0x0100;
    // on the state icon for a tree view item that is in a user-defined state.
static const int wxTREE_HITTEST_ONITEMSTATEICON  = 0x0200;
    // to the right of the client area.
static const int wxTREE_HITTEST_TOLEFT           = 0x0400;
    // to the left of the client area.
static const int wxTREE_HITTEST_TORIGHT          = 0x0800;
    // anywhere on the item
static const int wxTREE_HITTEST_ONITEM  = wxTREE_HITTEST_ONITEMICON |
                                          wxTREE_HITTEST_ONITEMLABEL |
                                          wxTREE_HITTEST_ONITEMSTATEICON;

// NB: all the following flags are for compatbility only and will be removed in the
//     next versions

// flags for deprecated `Expand(int action)'
enum
{
    wxTREE_EXPAND_EXPAND,
    wxTREE_EXPAND_COLLAPSE,
    wxTREE_EXPAND_COLLAPSE_RESET,
    wxTREE_EXPAND_TOGGLE
};

// flags for deprecated InsertItem() variant
#define wxTREE_INSERT_FIRST 0xFFFF0001
#define wxTREE_INSERT_LAST  0xFFFF0002

// ----------------------------------------------------------------------------
// wxTreeItemId identifies an element of the tree. In this implementation, it's
// just a trivial wrapper around Win32 HTREEITEM. It's opaque for the
// application.
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxTreeItemId
{
public:
    // ctors
        // 0 is invalid value for HTREEITEM
    wxTreeItemId() { m_itemId = 0; }

        // default copy ctor/assignment operator are ok for us

    // accessors
        // is this a valid tree item?
    bool IsOk() const { return m_itemId != 0; }

    // conversion to/from either real (system-dependent) tree item id or
    // to "long" which used to be the type for tree item ids in previous
    // versions of wxWindows

    // for wxTreeCtrl usage only
    wxTreeItemId(WXHTREEITEM itemId) { m_itemId = (long)itemId; }
    operator WXHTREEITEM() const { return (WXHTREEITEM)m_itemId; }

    void operator=(WXHTREEITEM item) { m_itemId = (long) item; }

protected:
    long m_itemId;
};

// ----------------------------------------------------------------------------
// wxTreeItemData is some (arbitrary) user class associated with some item. The
// main advantage of having this class (compared to old untyped interface) is
// that wxTreeItemData's are destroyed automatically by the tree and, as this
// class has virtual dtor, it means that the memory will be automatically
// freed. OTOH, we don't just use wxObject instead of wxTreeItemData because
// the size of this class is critical: in any real application, each tree leaf
// will have wxTreeItemData associated with it and number of leaves may be
// quite big.
//
// Because the objects of this class are deleted by the tree, they should
// always be allocated on the heap!
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxTreeItemData : private wxTreeItemId
{
public:
    // default ctor/copy ctor/assignment operator are ok

    // dtor is virtual and all the items are deleted by the tree control when
    // it's deleted, so you normally don't have to care about freeing memory
    // allocated in your wxTreeItemData-derived class
    virtual ~wxTreeItemData() { }

    // accessors: set/get the item associated with this node
    void SetId(const wxTreeItemId& id) { m_itemId = id; }
    const wxTreeItemId& GetId() const { return (wxTreeItemId&) m_itemId; }
};

// ----------------------------------------------------------------------------
// wxTreeCtrl
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxTreeCtrl : public wxControl
{
public:
    // creation
    // --------
    wxTreeCtrl() { Init(); }

    wxTreeCtrl(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = "wxTreeCtrl")
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    virtual ~wxTreeCtrl();

    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = "wxTreeCtrl");

    // accessors
    // ---------

        // get the total number of items in the control
    size_t GetCount() const;

        // indent is the number of pixels the children are indented relative to
        // the parents position. SetIndent() also redraws the control
        // immediately.
    unsigned int GetIndent() const;
    void SetIndent(unsigned int indent);

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

    // Functions to work with tree ctrl items. Unfortunately, they can _not_ be
    // member functions of wxTreeItem because they must know the tree the item
    // belongs to for Windows implementation and storing the pointer to
    // wxTreeCtrl in each wxTreeItem is just too much waste.

    // accessors
    // ---------

        // retrieve items label
    wxString GetItemText(const wxTreeItemId& item) const;
        // get the normal item image
    int GetItemImage(const wxTreeItemId& item) const;
        // get the selected item image
    int GetItemSelectedImage(const wxTreeItemId& item) const;
        // get the data associated with the item
    wxTreeItemData *GetItemData(const wxTreeItemId& item) const;

    // modifiers
    // ---------

        // set items label
    void SetItemText(const wxTreeItemId& item, const wxString& text);
        // set the normal item image
    void SetItemImage(const wxTreeItemId& item, int image);
        // set the selected item image
    void SetItemSelectedImage(const wxTreeItemId& item, int image);
        // associate some data with the item
    void SetItemData(const wxTreeItemId& item, wxTreeItemData *data);

        // force appearance of [+] button near the item. This is useful to
        // allow the user to expand the items which don't have any children now
        // - but instead add them only when needed, thus minimizing memory
        // usage and loading time.
    void SetItemHasChildren(const wxTreeItemId& item, bool has = TRUE);

        // the item will be shown in bold
    void SetItemBold(const wxTreeItemId& item, bool bold = TRUE);

    // item status inquiries
    // ---------------------

        // is the item visible (it might be outside the view or not expanded)?
    bool IsVisible(const wxTreeItemId& item) const;
        // does the item has any children?
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
    wxTreeItemId GetRootItem() const;

        // get the item currently selected (may return NULL if no selection)
    wxTreeItemId GetSelection() const;

        // get the parent of this item (may return NULL if root)
    wxTreeItemId GetParent(const wxTreeItemId& item) const;

        // for this enumeration function you must pass in a "cookie" parameter
        // which is opaque for the application but is necessary for the library
        // to make these functions reentrant (i.e. allow more than one
        // enumeration on one and the same object simultaneously). Of course,
        // the "cookie" passed to GetFirstChild() and GetNextChild() should be
        // the same!

        // get the first child of this item
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& _cookie) const;
        // get the next child
    wxTreeItemId GetNextChild(const wxTreeItemId& item, long& _cookie) const;

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

        // insert a new item in as the last child of the parent
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxTreeItemData *data = NULL);

        // delete this item and associated data if any
    void Delete(const wxTreeItemId& item);
        // delete all items from the tree
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
        // select this item
    void SelectItem(const wxTreeItemId& item);
        // make sure this item is visible (expanding the parent item and/or
        // scrolling to this item if necessary)
    void EnsureVisible(const wxTreeItemId& item);
        // scroll to this item (but don't expand its parent)
    void ScrollTo(const wxTreeItemId& item);

        // start editing the item label: this (temporarily) replaces the item
        // with a one line edit control. The item will be selected if it hadn't
        // been before. textCtrlClass parameter allows you to create an edit
        // control of arbitrary user-defined class deriving from wxTextCtrl.
    wxTextCtrl* EditLabel(const wxTreeItemId& item,
                          wxClassInfo* textCtrlClass = CLASSINFO(wxTextCtrl));
        // returns the same pointer as StartEdit() if the item is being edited,
        // NULL otherwise (it's assumed that no more than one item may be
        // edited simultaneously)
    wxTextCtrl* GetEditControl() const;
        // end editing and accept or discard the changes to item label
    void EndEditLabel(const wxTreeItemId& item, bool discardChanges = FALSE);

        // sort the children of this item using the specified callback function
        // (it should return -1, 0 or +1 as usual), if it's not specified
        // alphabetical comparaison is performed.
        //
        // NB: this function is not reentrant!
    void SortChildren(const wxTreeItemId& item,
                      wxTreeItemCmpFunc *cmpFunction = NULL);

    // helpers
    // -------

        // determine to which item (if any) belongs the given point (the
        // coordinates specified are relative to the client area of tree ctrl)
        // and fill the flags parameter with a bitmask of wxTREE_HITTEST_xxx
        // constants.
        //
        // The first function is more portable (because easier to implement
        // on other platforms), but the second one returns some extra info.
    wxTreeItemId HitTest(const wxPoint& point)
        { int dummy; return HitTest(point, dummy); }
    wxTreeItemId HitTest(const wxPoint& point, int& flags);

        // get the bounding rectangle of the item (or of its label only)
        // @@@ do we really need to expose this functions to the application?
    void GetBoundingRect(const wxTreeItemId& item,
                         wxRectangle& rect,
                         bool textOnly = FALSE) const;

    // deprecated
    // ----------

    // these methods are deprecated and will be removed in future versions of
    // wxWindows, they're here for compatibility only, don't use them in new
    // code (the comments indicate why these methods are now useless and how to
    // replace them)

        // use Expand, Collapse, CollapseAndReset or Toggle
    void ExpandItem(const wxTreeItemId& item, int action);

        // use AddRoot, PrependItem or AppendItem
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1, int selImage = -1,
                            long insertAfter = wxTREE_INSERT_LAST);

        // use Set/GetImageList and Set/GetStateImageList
    wxImageList *GetImageList(int) const
        { return GetImageList(); }
    void SetImageList(wxImageList *imageList, int)
        { SetImageList(imageList); }

    // implementation
    // --------------
    void Command(wxCommandEvent& event) { ProcessCommand(event); };
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual bool MSWNotify(WXWPARAM wParam, WXLPARAM lParam, WXLPARAM *result);

protected:
    // SetImageList helper
    void SetAnyImageList(wxImageList *imageList, int which);

    wxTextCtrl*  m_textCtrl;        // used while editing the item label
    wxImageList *m_imageListNormal, // images for tree elements
                *m_imageListState;  // special images for app defined states

private:
    // the common part of all ctors
    void Init();

    // helper functions
    inline bool DoGetItem(wxTreeViewItem *tvItem) const;
    inline void DoSetItem(wxTreeViewItem *tvItem);

    inline void DoExpand(const wxTreeItemId& item, int flag);

    wxTreeItemId DoInsertItem(const wxTreeItemId& parent,
                              wxTreeItemId hInsertAfter,
                              const wxString& text,
                              int image, int selectedImage,
                              wxTreeItemData *data);

    void DeleteTextCtrl();

    DECLARE_DYNAMIC_CLASS(wxTreeCtrl)
};

// ----------------------------------------------------------------------------
// wxTreeEvent is a special class for all events associated with tree controls
//
// NB: note that not all accessors make sense for all events, see the event
//     descriptions below
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxTreeEvent : public wxNotifyEvent
{
friend wxTreeCtrl;
public:
    wxTreeEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

    // accessors
        // get the item on which the operation was performed or the newly
        // selected item for wxEVT_COMMAND_TREE_SEL_CHANGED/ING events
    wxTreeItemId GetItem() const { return m_item; }

        // for wxEVT_COMMAND_TREE_SEL_CHANGED/ING events, get the previously
        // selected item
    wxTreeItemId GetOldItem() const { return m_itemOld; }

        // the point where the mouse was when the drag operation started (for
        // wxEVT_COMMAND_TREE_BEGIN_(R)DRAG events only)
    wxPoint GetPoint() const { return m_pointDrag; }

        // keyboard code (for wxEVT_COMMAND_TREE_KEY_DOWN only)
    int GetCode() const { return m_code; }

private:
    // @@ we could save some space by using union here
    int           m_code;
    wxTreeItemId  m_item,
                  m_itemOld;
    wxPoint       m_pointDrag;

    DECLARE_DYNAMIC_CLASS(wxTreeEvent)
};

typedef void (wxEvtHandler::*wxTreeEventFunction)(wxTreeEvent&);

// ----------------------------------------------------------------------------
// macros for handling tree control events
// ----------------------------------------------------------------------------

// GetItem() returns the item being dragged, GetPoint() the mouse coords
#define EVT_TREE_BEGIN_DRAG(id, fn) { wxEVT_COMMAND_TREE_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_BEGIN_RDRAG(id, fn) { wxEVT_COMMAND_TREE_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

// GetItem() returns the itme whose label is being edited
#define EVT_TREE_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

// provide/update information about GetItem() item
#define EVT_TREE_GET_INFO(id, fn) { wxEVT_COMMAND_TREE_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_SET_INFO(id, fn) { wxEVT_COMMAND_TREE_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

// GetItem() is the item being expanded/collapsed, the "ING" versions can use 
#define EVT_TREE_ITEM_EXPANDED(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_EXPANDING(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_COLLAPSED(id, fn) { wxEVT_COMMAND_TREE_ITEM_COLLAPSED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_COLLAPSING(id, fn) { wxEVT_COMMAND_TREE_ITEM_COLLAPSING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },

// GetOldItem() is the item which had the selection previously, GetItem() is
// the item which acquires selection
#define EVT_TREE_SEL_CHANGED(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },
#define EVT_TREE_SEL_CHANGING(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },

// GetCode() returns the key code
// NB: this is the only message for which GetItem() is invalid (you may get the
//     item from GetSelection())
#define EVT_TREE_KEY_DOWN(id, fn) { wxEVT_COMMAND_TREE_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, NULL },

// GetItem() returns the item being deleted, the associated data (if any) will
// be deleted just after the return of this event handler (if any)
#define EVT_TREE_DELETE_ITEM(id, fn) { wxEVT_COMMAND_TREE_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

#endif
    // _WX_TREECTRL_H_
