/////////////////////////////////////////////////////////////////////////////
// Name:        _treectrl.i
// Purpose:     SWIG interface file for wxTreeCtrl and related classes
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2002 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/treectrl.h>
#include "wx/wxPython/pytree.h"
%}
    
MAKE_CONST_WXSTRING2(TreeCtrlNameStr, _T("wxTreeCtrl"));

//---------------------------------------------------------------------------
%newgroup


// wxTreeCtrl flags
enum {
    wxTR_NO_BUTTONS,
    wxTR_HAS_BUTTONS,
    wxTR_NO_LINES,
    wxTR_LINES_AT_ROOT,

    wxTR_SINGLE,
    wxTR_MULTIPLE,
    wxTR_EXTENDED,
    wxTR_HAS_VARIABLE_ROW_HEIGHT,

    wxTR_EDIT_LABELS,
    wxTR_HIDE_ROOT,
    wxTR_ROW_LINES,

    wxTR_FULL_ROW_HIGHLIGHT,
    wxTR_DEFAULT_STYLE,

    wxTR_TWIST_BUTTONS,
};

%pythoncode {
    %# obsolete
    TR_MAC_BUTTONS = 0
    wxTR_AQUA_BUTTONS = 0
}

enum wxTreeItemIcon
{
    wxTreeItemIcon_Normal,              // not selected, not expanded
    wxTreeItemIcon_Selected,            //     selected, not expanded
    wxTreeItemIcon_Expanded,            // not selected,     expanded
    wxTreeItemIcon_SelectedExpanded,    //     selected,     expanded
    wxTreeItemIcon_Max
};


// constants for HitTest
enum {
    wxTREE_HITTEST_ABOVE,
    wxTREE_HITTEST_BELOW,
    wxTREE_HITTEST_NOWHERE,
    wxTREE_HITTEST_ONITEMBUTTON,
    wxTREE_HITTEST_ONITEMICON,
    wxTREE_HITTEST_ONITEMINDENT,
    wxTREE_HITTEST_ONITEMLABEL,
    wxTREE_HITTEST_ONITEMRIGHT,
    wxTREE_HITTEST_ONITEMSTATEICON,
    wxTREE_HITTEST_TOLEFT,
    wxTREE_HITTEST_TORIGHT,
    wxTREE_HITTEST_ONITEMUPPERPART,
    wxTREE_HITTEST_ONITEMLOWERPART,
    wxTREE_HITTEST_ONITEM
};

//---------------------------------------------------------------------------
%newgroup


// wxTreeItemId identifies an element of the tree. In this implementation, it's
// just a trivial wrapper around Win32 HTREEITEM or a pointer to some private
// data structure in the generic version. It's opaque for the application and
// the only method which can be used by user code is IsOk().
class wxTreeItemId {
public:
    wxTreeItemId();
    ~wxTreeItemId(); 

    // is this a valid tree item?
    bool IsOk() const;

    %extend {
        bool __eq__(const wxTreeItemId* other) { return other ? (*self == *other) : false; }
        bool __ne__(const wxTreeItemId* other) { return other ? (*self != *other) : true;  }
    }

    void*  m_pItem;


    %pythoncode {
        Ok = IsOk
        def __nonzero__(self): return self.IsOk() }
};




// wxTreeItemData is some (arbitrary) user data associated with some tree
// item.  The Python version is just a simple wrapper around a Python object
// that knows how to handle references properly.  Using this class directly in
// Python code should rarely be neccessary.  Just use the GetItemPyData and
// SetItemPyData tree methods instead of the GetItemData and SetItemData
// methods.
%rename(TreeItemData) wxPyTreeItemData;
class wxPyTreeItemData {
public:
    wxPyTreeItemData(PyObject* obj = NULL);
    ~wxPyTreeItemData();
    
    PyObject* GetData();
    void      SetData(PyObject* obj);

    const wxTreeItemId& GetId();
    void                SetId(const wxTreeItemId& id);

    %pythonPrepend Destroy "args[0].this.own(False)"
    %extend { void Destroy() { delete self; } }

    %property(Data, GetData, SetData, doc="See `GetData` and `SetData`");
    %property(Id, GetId, SetId, doc="See `GetId` and `SetId`");
};



#if 0  // it's not currently used anywhere...

// wxTreeItemAttr: a structure containing the visual attributes of an item
class wxTreeItemAttr
{
public:
    // ctors
    //wxTreeItemAttr() { }
    wxTreeItemAttr(const wxColour& colText = wxNullColour,
                   const wxColour& colBack = wxNullColour,
                   const wxFont& font = wxNullFont);
    ~wxTreeItemAttr();
    
    // setters
    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetFont(const wxFont& font);

    // accessors
    bool HasTextColour();
    bool HasBackgroundColour();
    bool HasFont();

    wxColour GetTextColour();
    wxColour GetBackgroundColour();
    wxFont GetFont();

    %pythonAppend Destroy "args[0].thisown = 0"
    %extend { void Destroy() { delete self; } }
};

#endif


//---------------------------------------------------------------------------
%newgroup

/* Tree control event types */
%constant wxEventType wxEVT_COMMAND_TREE_BEGIN_DRAG;
%constant wxEventType wxEVT_COMMAND_TREE_BEGIN_RDRAG;
%constant wxEventType wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT;
%constant wxEventType wxEVT_COMMAND_TREE_END_LABEL_EDIT;
%constant wxEventType wxEVT_COMMAND_TREE_DELETE_ITEM;
%constant wxEventType wxEVT_COMMAND_TREE_GET_INFO;
%constant wxEventType wxEVT_COMMAND_TREE_SET_INFO;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_EXPANDED;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_EXPANDING;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_COLLAPSED;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_COLLAPSING;
%constant wxEventType wxEVT_COMMAND_TREE_SEL_CHANGED;
%constant wxEventType wxEVT_COMMAND_TREE_SEL_CHANGING;
%constant wxEventType wxEVT_COMMAND_TREE_KEY_DOWN;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_ACTIVATED;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK;
%constant wxEventType wxEVT_COMMAND_TREE_END_DRAG;
%constant wxEventType wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP;
%constant wxEventType wxEVT_COMMAND_TREE_ITEM_MENU;

%pythoncode {
EVT_TREE_BEGIN_DRAG        = wx.PyEventBinder(wxEVT_COMMAND_TREE_BEGIN_DRAG       , 1)
EVT_TREE_BEGIN_RDRAG       = wx.PyEventBinder(wxEVT_COMMAND_TREE_BEGIN_RDRAG      , 1)
EVT_TREE_BEGIN_LABEL_EDIT  = wx.PyEventBinder(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT , 1)
EVT_TREE_END_LABEL_EDIT    = wx.PyEventBinder(wxEVT_COMMAND_TREE_END_LABEL_EDIT   , 1)
EVT_TREE_DELETE_ITEM       = wx.PyEventBinder(wxEVT_COMMAND_TREE_DELETE_ITEM      , 1)
EVT_TREE_GET_INFO          = wx.PyEventBinder(wxEVT_COMMAND_TREE_GET_INFO         , 1)
EVT_TREE_SET_INFO          = wx.PyEventBinder(wxEVT_COMMAND_TREE_SET_INFO         , 1)
EVT_TREE_ITEM_EXPANDED     = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_EXPANDED    , 1)
EVT_TREE_ITEM_EXPANDING    = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_EXPANDING   , 1)
EVT_TREE_ITEM_COLLAPSED    = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_COLLAPSED   , 1)
EVT_TREE_ITEM_COLLAPSING   = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_COLLAPSING  , 1)
EVT_TREE_SEL_CHANGED       = wx.PyEventBinder(wxEVT_COMMAND_TREE_SEL_CHANGED      , 1)
EVT_TREE_SEL_CHANGING      = wx.PyEventBinder(wxEVT_COMMAND_TREE_SEL_CHANGING     , 1)
EVT_TREE_KEY_DOWN          = wx.PyEventBinder(wxEVT_COMMAND_TREE_KEY_DOWN         , 1)
EVT_TREE_ITEM_ACTIVATED    = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_ACTIVATED   , 1)
EVT_TREE_ITEM_RIGHT_CLICK  = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK , 1)
EVT_TREE_ITEM_MIDDLE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK, 1)
EVT_TREE_END_DRAG          = wx.PyEventBinder(wxEVT_COMMAND_TREE_END_DRAG         , 1)
EVT_TREE_STATE_IMAGE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, 1)
EVT_TREE_ITEM_GETTOOLTIP   = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP,   1)
EVT_TREE_ITEM_MENU         = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_MENU,         1)
}


%{
    static wxTreeItemId wxNullTreeItemId;
%}
wxTreeItemId wxNullTreeItemId;


// wxTreeEvent is a special class for all events associated with tree controls
//
// NB: note that not all accessors make sense for all events, see the event
//     descriptions below
class wxTreeEvent : public wxNotifyEvent {
public:
    %nokwargs wxTreeEvent;
    wxTreeEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
    wxTreeEvent(wxEventType   commandType,
                wxPyTreeCtrl* tree,
                wxTreeItemId& item = wxNullTreeItemId);

        // get the item on which the operation was performed or the newly
        // selected item for wxEVT_COMMAND_TREE_SEL_CHANGED/ING events
    wxTreeItemId GetItem() const;
    void SetItem(const wxTreeItemId& item);

        // for wxEVT_COMMAND_TREE_SEL_CHANGED/ING events, get the previously
        // selected item
    wxTreeItemId GetOldItem() const;
    void SetOldItem(const wxTreeItemId& item);

        // the point where the mouse was when the drag operation started (for
        // wxEVT_COMMAND_TREE_BEGIN_(R)DRAG events only) or click position
    wxPoint GetPoint() const;
    void SetPoint(const wxPoint& pt);

        // keyboard data (for wxEVT_COMMAND_TREE_KEY_DOWN only)
    const wxKeyEvent& GetKeyEvent() const;
    int GetKeyCode() const;
    void SetKeyEvent(const wxKeyEvent& evt);

        // label (for EVT_TREE_{BEGIN|END}_LABEL_EDIT only)
    const wxString& GetLabel() const;
    void SetLabel(const wxString& label);

        // edit cancel flag (for EVT_TREE_{BEGIN|END}_LABEL_EDIT only)
    bool IsEditCancelled() const;
    void SetEditCanceled(bool editCancelled);

        // Set the tooltip for the item (for EVT_TREE_ITEM_GETTOOLTIP events)
    void SetToolTip(const wxString& toolTip);
    wxString GetToolTip();

    %property(Item, GetItem, SetItem, doc="See `GetItem` and `SetItem`");
    %property(KeyCode, GetKeyCode, doc="See `GetKeyCode`");
    %property(KeyEvent, GetKeyEvent, SetKeyEvent, doc="See `GetKeyEvent` and `SetKeyEvent`");
    %property(Label, GetLabel, SetLabel, doc="See `GetLabel` and `SetLabel`");
    %property(OldItem, GetOldItem, SetOldItem, doc="See `GetOldItem` and `SetOldItem`");
    %property(Point, GetPoint, SetPoint, doc="See `GetPoint` and `SetPoint`");
    %property(ToolTip, GetToolTip, SetToolTip, doc="See `GetToolTip` and `SetToolTip`");
    %property(EditCancelled, IsEditCancelled, SetEditCanceled, doc="See `IsEditCancelled` and `SetEditCanceled`");
};

//---------------------------------------------------------------------------
%newgroup

%{ // C++ version of Python aware wxTreeCtrl
class wxPyTreeCtrl : public wxTreeCtrl {
    DECLARE_ABSTRACT_CLASS(wxPyTreeCtrl)
public:
    wxPyTreeCtrl() : wxTreeCtrl() {}
    wxPyTreeCtrl(wxWindow *parent, wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxValidator& validator,
                 const wxString& name) :
        wxTreeCtrl(parent, id, pos, size, style, validator, name) {}

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString& name) {
        return wxTreeCtrl::Create(parent, id, pos, size, style, validator, name);
    }


    int OnCompareItems(const wxTreeItemId& item1,
                       const wxTreeItemId& item2) {
        int rval = 0;
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "OnCompareItems"))) {
            PyObject *o1 = wxPyConstructObject((void*)&item1, wxT("wxTreeItemId"), false);
            PyObject *o2 = wxPyConstructObject((void*)&item2, wxT("wxTreeItemId"), false);
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)",o1,o2));
            Py_DECREF(o1);
            Py_DECREF(o2);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            rval = wxTreeCtrl::OnCompareItems(item1, item2);
        return rval;
    }
    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyTreeCtrl, wxTreeCtrl);

%}



 
MustHaveApp(wxPyTreeCtrl);

%rename(TreeCtrl) wxPyTreeCtrl;
class wxPyTreeCtrl : public wxControl {
public:
    %pythonAppend wxPyTreeCtrl         "self._setOORInfo(self);self._setCallbackInfo(self, TreeCtrl)"
    %pythonAppend wxPyTreeCtrl()       ""
    %typemap(out) wxPyTreeCtrl*;    // turn off this typemap
   
    wxPyTreeCtrl(wxWindow *parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTR_DEFAULT_STYLE,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyTreeCtrlNameStr);
    %RenameCtor(PreTreeCtrl, wxPyTreeCtrl());

    // Turn it back on again
    %typemap(out) wxPyTreeCtrl* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyTreeCtrlNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    
    // get the total number of items in the control
    unsigned int GetCount() const;

    // indent is the number of pixels the children are indented relative to
    // the parents position. SetIndent() also redraws the control
    // immediately.
    unsigned int GetIndent() const;
    void SetIndent(unsigned int indent);

    // spacing is the number of pixels between the start and the Text
    // not implemented under wxMSW
    unsigned int GetSpacing() const;
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

    %disownarg( wxImageList *imageList );
    void AssignImageList(wxImageList *imageList);
    void AssignStateImageList(wxImageList *imageList);
    %cleardisown( wxImageList *imageList );
    

    // retrieve items label
    wxString GetItemText(const wxTreeItemId& item) const;
    
    // get one of the images associated with the item (normal by default)
    int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const;

    %extend {
        // get the wxPyTreeItemData associated with the tree item
        wxPyTreeItemData* GetItemData(const wxTreeItemId& item) {
            wxPyTreeItemData* data = (wxPyTreeItemData*)self->GetItemData(item);
            if (data == NULL) {
                data = new wxPyTreeItemData();
                data->SetId(item); // set the id
                self->SetItemData(item, data);
            }
            return data;
        }
        // Get the Python object associated with the tree item
        PyObject* GetItemPyData(const wxTreeItemId& item) {
            wxPyTreeItemData* data = (wxPyTreeItemData*)self->GetItemData(item);
            if (data == NULL) {
                data = new wxPyTreeItemData();
                data->SetId(item); // set the id
                self->SetItemData(item, data);
            }
            return data->GetData();
        }
    }
    %pythoncode { GetPyData = GetItemPyData }
        
 
    // get the item's text colour
    wxColour GetItemTextColour(const wxTreeItemId& item) const;

    // get the item's background colour
    wxColour GetItemBackgroundColour(const wxTreeItemId& item) const;

    // get the item's font
    wxFont GetItemFont(const wxTreeItemId& item) const;

    

    // set items label
    void SetItemText(const wxTreeItemId& item, const wxString& text);

    // get one of the images associated with the item (normal by default)
    void SetItemImage(const wxTreeItemId& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);

    %extend {
        // associate a wxPyTreeItemData with the tree item
        %disownarg( wxPyTreeItemData* data );
        void SetItemData(const wxTreeItemId& item, wxPyTreeItemData* data) {
            data->SetId(item); // set the id
            self->SetItemData(item, data);
        }
        %cleardisown( wxPyTreeItemData* data );

        // associate a Python object with the tree item
        void SetItemPyData(const wxTreeItemId& item, PyObject* obj) {
            wxPyTreeItemData* data = (wxPyTreeItemData*)self->GetItemData(item);
            if (data == NULL) {
                data = new wxPyTreeItemData(obj);
                data->SetId(item); // set the id
                self->SetItemData(item, data);
            } else
                data->SetData(obj);
        }
    }
    %pythoncode { SetPyData = SetItemPyData }


    // force appearance of [+] button near the item. This is useful to
    // allow the user to expand the items which don't have any children now
    // - but instead add them only when needed, thus minimizing memory
    // usage and loading time.
    void SetItemHasChildren(const wxTreeItemId& item, bool has = true);

    // the item will be shown in bold
    void SetItemBold(const wxTreeItemId& item, bool bold = true);

    // the item will be shown with a drop highlight
    void SetItemDropHighlight(const wxTreeItemId& item, bool highlight = true);
    
    // set the items text colour
    void SetItemTextColour(const wxTreeItemId& item, const wxColour& col);

    // set the items background colour
    void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col);

    // set the items font (should be of the same height for all items)
    void SetItemFont(const wxTreeItemId& item, const wxFont& font);


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


    // if 'recursively' is False, only immediate children count, otherwise
    // the returned number is the number of all items in this branch
    size_t GetChildrenCount(const wxTreeItemId& item,
                            bool recursively = true) /*const*/;



    // get the root tree item
    // wxTreeItemId.IsOk() will return False if there is no such item
    wxTreeItemId GetRootItem() const;

    // get the item currently selected 
    // wxTreeItemId.IsOk() will return False if there is no such item
    wxTreeItemId GetSelection() const;

    %extend {
        // get the items currently selected, return the number of such item
        //
        // NB: this operation is expensive and can take a long time for a
        //     control with a lot of items (~ O(number of items)).
        PyObject* GetSelections() {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject*           rval = PyList_New(0);
            wxArrayTreeItemIds  array;
            size_t              num, x;
            num = self->GetSelections(array);
            for (x=0; x < num; x++) {
                wxTreeItemId *tii = new wxTreeItemId(array.Item(x));
                PyObject* item = wxPyConstructObject((void*)tii, wxT("wxTreeItemId"), true);
                PyList_Append(rval, item);
                Py_DECREF(item);
            }
            wxPyEndBlockThreads(blocked);
            return rval;
        }
    }

    // get the parent of this item
    // wxTreeItemId.IsOk() will return False if there is no such item
    wxTreeItemId GetItemParent(const wxTreeItemId& item) const;


    // NOTE: These are a copy of the same methods in gizmos.i, be sure to
    // update both at the same time.  (Or find a good way to refactor!)
    %extend {
        // Get the first child of this item.  Returns a wxTreeItemId and an
        // opaque "cookie" value that should be passed to GetNextChild in
        // order to continue the search.
        PyObject* GetFirstChild(const wxTreeItemId& item) {
            void* cookie = 0;
            wxTreeItemId* ritem = new wxTreeItemId(self->GetFirstChild(item, cookie));
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, wxPyConstructObject(ritem, wxT("wxTreeItemId"), true));
            PyTuple_SET_ITEM(tup, 1, wxPyMakeSwigPtr(cookie, wxT("void")));
            wxPyEndBlockThreads(blocked);
            return tup;
        }


        // Get the next child of this item.  The cookie parameter is the 2nd
        // value returned from GetFirstChild or the previous GetNextChild.
        // Returns a wxTreeItemId and an opaque "cookie" value that should be
        // passed to GetNextChild in order to continue the search.
        PyObject* GetNextChild(const wxTreeItemId& item, void* cookie) {
            wxTreeItemId* ritem = new wxTreeItemId(self->GetNextChild(item, cookie));
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, wxPyConstructObject(ritem, wxT("wxTreeItemId"), true));
            PyTuple_SET_ITEM(tup, 1, wxPyMakeSwigPtr(cookie, wxT("void")));
            wxPyEndBlockThreads(blocked);
            return tup;
        }            
    }
    
    // get the last child of this item
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

    
    %disownarg( wxPyTreeItemData* data );
    
    // add the root node to the tree
    wxTreeItemId AddRoot(const wxString& text,
                         int image = -1, int selectedImage = -1,
                         wxPyTreeItemData *data = NULL);

    // insert a new item in as the first child of the parent
    wxTreeItemId PrependItem(const wxTreeItemId& parent,
                             const wxString& text,
                             int image = -1, int selectedImage = -1,
                             wxPyTreeItemData *data = NULL);

    // insert a new item after a given one
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            const wxTreeItemId& idPrevious,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxPyTreeItemData *data = NULL);

    // insert a new item before the one with the given index
    %Rename(InsertItemBefore, 
    wxTreeItemId, InsertItem(const wxTreeItemId& parent,
                            size_t index,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxPyTreeItemData *data = NULL));

        // insert a new item in as the last child of the parent
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxPyTreeItemData *data = NULL);


    %cleardisown( wxPyTreeItemData* data );
    
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

    // expand the item and all its childs and thats childs
    void ExpandAllChildren(const wxTreeItemId& item);

    // expand all items
    void ExpandAll();
   
    // collapse the item without removing its children
    void Collapse(const wxTreeItemId& item);

    // collapse the item and remove all children
    void CollapseAndReset(const wxTreeItemId& item);

    // toggles the current state
    void Toggle(const wxTreeItemId& item);


    // remove the selection from currently selected item (if any)
    void Unselect();

    // remove the selection from the given one (multiselect mode only)
    void UnselectItem(const wxTreeItemId& item);
    
    // unselect all items (only makes sense for multiple selection control)
    void UnselectAll();

    // select this item
    void SelectItem(const wxTreeItemId& item, bool select = true);

    // toggle the item selection
    void ToggleItemSelection(const wxTreeItemId& item);

    
    // make sure this item is visible (expanding the parent item and/or
    // scrolling to this item if necessary)
    void EnsureVisible(const wxTreeItemId& item);

    // scroll to this item (but don't expand its parent)
    void ScrollTo(const wxTreeItemId& item);



    // start editing the item label: this (temporarily) replaces the item
    // with a one line edit control. The item will be selected if it hadn't
    // been before.
    /**wxTextCtrl* */ void  EditLabel(const wxTreeItemId& item);
    
    // returns the same pointer as StartEdit() if the item is being edited,
    // NULL otherwise (it's assumed that no more than one item may be
    // edited simultaneously)
    wxTextCtrl* GetEditControl() const;

#ifdef __WXMSW__
    // end editing and accept or discard the changes to item label
    void EndEditLabel(const wxTreeItemId& item, bool discardChanges = false);
#endif


    // Sort the children of this item using OnCompareItems, a member function
    // that is called to compare 2 items and should return -1, 0 or +1 if the
    // first item is less than, equal to or greater than the second one. The
    // base class version performs alphabetic comparaison of item labels
    // (GetText)
    void SortChildren(const wxTreeItemId& item);



    DocDeclAStr(
        wxTreeItemId, HitTest(const wxPoint& point, int& OUTPUT),
        "HitTest(Point point) -> (item, where)",
        "Determine which item (if any) belongs the given point.  The coordinates
specified are relative to the client area of tree ctrl and the where return
value is set to a bitmask of wxTREE_HITTEST_xxx constants.
", "");
    

    %extend {
        // get the bounding rectangle of the item (or of its label only)
        PyObject* GetBoundingRect(const wxTreeItemId& item,  bool textOnly = false) {
             wxRect rect;
            if (self->GetBoundingRect(item, rect, textOnly)) {
                wxPyBlock_t blocked = wxPyBeginBlockThreads();
                wxRect* r = new wxRect(rect);
                PyObject* val = wxPyConstructObject((void*)r, wxT("wxRect"), true);
                wxPyEndBlockThreads(blocked);
                return val;
            }
            else
                RETURN_NONE();
        }
    }

#ifdef __WXMSW__
    // set/get the item state.image (state == -1 means cycle to the next one)
    void SetState(const wxTreeItemId& node, int state);
    int GetState(const wxTreeItemId& node);
#endif

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    void SetQuickBestSize(bool q);
    bool GetQuickBestSize() const;

    %property(Count, GetCount, doc="See `GetCount`");
    %property(EditControl, GetEditControl, doc="See `GetEditControl`");
    %property(FirstVisibleItem, GetFirstVisibleItem, doc="See `GetFirstVisibleItem`");
    %property(ImageList, GetImageList, SetImageList, doc="See `GetImageList` and `SetImageList`");
    %property(Indent, GetIndent, SetIndent, doc="See `GetIndent` and `SetIndent`");
    %property(QuickBestSize, GetQuickBestSize, SetQuickBestSize, doc="See `GetQuickBestSize` and `SetQuickBestSize`");
    %property(RootItem, GetRootItem, doc="See `GetRootItem`");
    %property(Selection, GetSelection, doc="See `GetSelection`");
    %property(Selections, GetSelections, doc="See `GetSelections`");
    %property(Spacing, GetSpacing, SetSpacing, doc="See `GetSpacing` and `SetSpacing`");
    %property(StateImageList, GetStateImageList, SetStateImageList, doc="See `GetStateImageList` and `SetStateImageList`");
};


//---------------------------------------------------------------------------
%init %{
    // Map renamed classes back to their common name for OOR
    wxPyPtrTypeMap_Add("wxTreeItemData", "wxPyTreeItemData");
    wxPyPtrTypeMap_Add("wxTreeCtrl", "wxPyTreeCtrl");
%}
//---------------------------------------------------------------------------
