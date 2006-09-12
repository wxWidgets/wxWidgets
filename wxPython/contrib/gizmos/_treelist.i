/////////////////////////////////////////////////////////////////////////////
// Name:        _treelist.i
// Purpose:     wxTreeListCtrl and helpers
//
// Author:      Robin Dunn
//
// Created:     12-Sept-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include "wx/treelistctrl.h"
#include "wx/wxPython/pytree.h"

%}

//---------------------------------------------------------------------------
%newgroup


MAKE_CONST_WXSTRING2(TreeListCtrlNameStr,    wxT("treelistctrl"));


//----------------------------------------------------------------------------
// wxTreeListCtrl - the multicolumn tree control
//----------------------------------------------------------------------------



enum {
    DEFAULT_COL_WIDTH = 100
};

// modes for navigation
enum {
    wxTL_MODE_NAV_FULLTREE,
    wxTL_MODE_NAV_EXPANDED,
    wxTL_MODE_NAV_VISIBLE,
    wxTL_MODE_NAV_LEVEL
};

// modes for FindItem
enum {
    wxTL_MODE_FIND_EXACT,
    wxTL_MODE_FIND_PARTIAL,
    wxTL_MODE_FIND_NOCASE
};

// additional flag for HitTest
enum {
    wxTREE_HITTEST_ONITEMCOLUMN
};
%pythoncode { wx.TREE_HITTEST_ONITEMCOLUMN = TREE_HITTEST_ONITEMCOLUMN }


// additional style flags
enum {
    wxTR_COLUMN_LINES, // put border around items
    wxTR_VIRTUAL      // The application provides items text on demand.
};
%pythoncode {
    wx.TR_COLUMN_LINES = TR_COLUMN_LINES
    wxTR_VIRTUAL = TR_VIRTUAL    
}


%pythoncode {
%#// Compatibility aliases for old names/values
TL_ALIGN_LEFT   = wx.ALIGN_LEFT
TL_ALIGN_RIGHT  = wx.ALIGN_RIGHT
TL_ALIGN_CENTER = wx.ALIGN_CENTER

TL_SEARCH_VISIBLE = TL_MODE_NAV_VISIBLE
TL_SEARCH_LEVEL   = TL_MODE_NAV_LEVEL
TL_SEARCH_FULL    = TL_MODE_FIND_EXACT
TL_SEARCH_PARTIAL = TL_MODE_FIND_PARTIAL
TL_SEARCH_NOCASE  = TL_MODE_FIND_NOCASE

TR_DONT_ADJUST_MAC = 0
wx.TR_DONT_ADJUST_MAC = TR_DONT_ADJUST_MAC
}




class wxTreeListColumnInfo: public wxObject {
public:
    wxTreeListColumnInfo(const wxString& text = wxPyEmptyString,
                         int width = DEFAULT_COL_WIDTH,
                         int flag = wxALIGN_LEFT,
                         int image = -1,
                         bool shown = true,
                         bool edit = false);

    ~wxTreeListColumnInfo();

    int GetAlignment() const;
    wxString GetText() const;
    int GetImage() const;
    int GetSelectedImage() const;
    size_t GetWidth() const;
    bool IsEditable() const { return m_edit; }
    bool IsShown() const { return m_shown; }

    // TODO:  These all actually return wxTreeListColumnInfo&, any problem with doing it for Python too?
    void SetAlignment(int alignment);
    void SetText(const wxString& text);
    void SetImage(int image);
    void SetSelectedImage(int image);
    void SetWidth(size_t with);
    void SetEditable (bool edit);
    void SetShown(bool shown);

};




%{ // C++ version of Python aware control
class wxPyTreeListCtrl : public wxTreeListCtrl {
    DECLARE_ABSTRACT_CLASS(wxPyTreeListCtrl);
public:
    wxPyTreeListCtrl() : wxTreeListCtrl() {}
    wxPyTreeListCtrl(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxValidator &validator,
                     const wxString& name) :
        wxTreeListCtrl(parent, id, pos, size, style, validator, name) {}

    virtual int OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2) {
        int rval = 0;
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "OnCompareItems"))) {
            PyObject *o1 = wxPyConstructObject((void*)&item1, wxT("wxTreeItemId"), 0);
            PyObject *o2 = wxPyConstructObject((void*)&item2, wxT("wxTreeItemId"), 0);
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)",o1,o2));
            Py_DECREF(o1);
            Py_DECREF(o2);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            rval = wxTreeListCtrl::OnCompareItems(item1, item2);
        return rval;
    }

    virtual wxString  OnGetItemText( wxTreeItemData* item, long column ) const {
        wxString rval;
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "OnGetItemText"))) {
            PyObject* ro;
            PyObject* itemo = wxPyConstructObject((void*)&item, wxT("wxTreeItemId"), 0);
            ro = wxPyCBH_callCallbackObj(m_myInst, Py_BuildValue("(Oi)", itemo, column));
            Py_DECREF(itemo);
            if (ro) {
                rval = Py2wxString(ro);
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            rval = wxTreeListCtrl::OnGetItemText(item, column);
        return rval;
    }

    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyTreeListCtrl, wxTreeListCtrl)

%}






MustHaveApp(wxPyTreeListCtrl);

%rename(TreeListCtrl) wxPyTreeListCtrl;
class wxPyTreeListCtrl : public wxControl
{
public:
    %pythonAppend wxPyTreeListCtrl         "self._setOORInfo(self);self._setCallbackInfo(self, TreeListCtrl)"
    %pythonAppend wxPyTreeListCtrl()       ""

    wxPyTreeListCtrl(wxWindow *parent, wxWindowID id = -1,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxTR_DEFAULT_STYLE,
                   const wxValidator &validator = wxDefaultValidator,
                   const wxString& name = wxPyTreeListCtrlNameStr );
    %RenameCtor(PreTreeListCtrl, wxPyTreeListCtrl());

    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_DEFAULT_STYLE,
                const wxValidator &validator = wxDefaultValidator,
                const wxString& name = wxPyTreeListCtrlNameStr );

    void _setCallbackInfo(PyObject* self, PyObject* _class);


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

    %disownarg( wxImageList *imageList );
    void AssignImageList(wxImageList *imageList);
    void AssignStateImageList(wxImageList *imageList);
    void AssignButtonsImageList(wxImageList *imageList);
    %cleardisown( wxImageList *imageList );


    // adds a column
    void AddColumn (const wxString& text,
                    int width = DEFAULT_COL_WIDTH,
                    int flag = wxALIGN_LEFT,
                    int image = -1,
                    bool shown = true,
                    bool edit = false);
    %Rename(AddColumnInfo,  void,  AddColumn(const wxTreeListColumnInfo& col));

    // inserts a column before the given one
    void InsertColumn (int before,
                       const wxString& text,
                       int width = DEFAULT_COL_WIDTH,
                       int flag = wxALIGN_LEFT,
                       int image = -1,
                       bool shown = true,
                       bool edit = false);
    %Rename(InsertColumnInfo,  void,  InsertColumn(size_t before, const wxTreeListColumnInfo& col));

    // deletes the given column - does not delete the corresponding column
    // of each item
    void RemoveColumn(size_t column);

    // returns the number of columns in the ctrl
    size_t GetColumnCount() const;

    // tells which column is the "main" one, i.e. the "threaded" one
    void SetMainColumn(size_t column);
    size_t GetMainColumn() const;

    void SetColumn (int column, const wxTreeListColumnInfo& colInfo);
    wxTreeListColumnInfo& GetColumn (int column);

    void SetColumnText (int column, const wxString& text);
    wxString GetColumnText (int column) const;

    void SetColumnWidth (int column, int width);
    int GetColumnWidth (int column) const;

    void SetColumnAlignment (int column, int flag);
    int GetColumnAlignment (int column) const;

    void SetColumnImage (int column, int image);
    int GetColumnImage (int column) const;

    void SetColumnShown (int column, bool shown = true);
    bool IsColumnShown (int column) const;
    %pythoncode { ShowColumn = SetColumnShown }

    void SetColumnEditable (int column, bool edit = true);
    bool IsColumnEditable (int column) const;




    %extend {
        // retrieves item's label of the given column (main column by default)
        wxString GetItemText(const wxTreeItemId& item, int column = -1) {
            if (column < 0) column = self->GetMainColumn();
            return self->GetItemText(item, column);
        }

        // get one of the images associated with the item (normal by default)
        int GetItemImage(const wxTreeItemId& item, int column = -1,
                         wxTreeItemIcon which = wxTreeItemIcon_Normal) {
            if (column < 0) column = self->GetMainColumn();
            return self->GetItemImage(item, column, which);
        }

        // set item's label (main column by default)
        void SetItemText(const wxTreeItemId& item, const wxString& text, int column = -1) {
            if (column < 0) column = self->GetMainColumn();
            self->SetItemText(item, column, text);
        }

        // set one of the images associated with the item (normal by default)
        // the which parameter is ignored for all columns but the main one
        void SetItemImage(const wxTreeItemId& item, int image, int column = -1,
                          wxTreeItemIcon which = wxTreeItemIcon_Normal) {
            if (column < 0) column = self->GetMainColumn();
            self->SetItemImage(item, column, image, which);
        }


        // [Get|Set]ItemData substitutes.  Automatically create wxPyTreeItemData
        // if needed.
        wxPyTreeItemData* GetItemData(const wxTreeItemId& item) {
            wxPyTreeItemData* data = (wxPyTreeItemData*)self->GetItemData(item);
            if (data == NULL) {
                data = new wxPyTreeItemData();
                data->SetId(item); // set the id
                self->SetItemData(item, data);
            }
            return data;
        }

        %disownarg( wxPyTreeItemData* data );
        void SetItemData(const wxTreeItemId& item, wxPyTreeItemData* data) {
            data->SetId(item); // set the id
            self->SetItemData(item, data);
        }
        %cleardisown(wxPyTreeItemData* data );

        // [Get|Set]ItemPyData are short-cuts.  Also made somewhat crash-proof by
        // automatically creating data classes.
        PyObject* GetItemPyData(const wxTreeItemId& item) {
            wxPyTreeItemData* data = (wxPyTreeItemData*)self->GetItemData(item);
            if (data == NULL) {
                data = new wxPyTreeItemData();
                data->SetId(item); // set the id
                self->SetItemData(item, data);
            }
            return data->GetData();
        }

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
    %pythoncode { GetPyData = GetItemPyData }
    %pythoncode { SetPyData = SetItemPyData }


    bool GetItemBold(const wxTreeItemId& item) const;
    wxColour GetItemTextColour(const wxTreeItemId& item) const;
    wxColour GetItemBackgroundColour(const wxTreeItemId& item) const;
    wxFont GetItemFont(const wxTreeItemId& item) const;


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
    void SetItemBackgroundColour(const wxTreeItemId& item,
				 const wxColour& colour);

    // set the item's font (should be of the same height for all items)
    void SetItemFont(const wxTreeItemId& item, const wxFont& font);



    // is the item visible (it might be outside the view or not expanded)?
    bool IsVisible(const wxTreeItemId& item) const;

    // does the item has any children?
    bool HasChildren(const wxTreeItemId& item) const;
    %pythoncode { ItemHasChildren = HasChildren }

    // is the item expanded (only makes sense if HasChildren())?
    bool IsExpanded(const wxTreeItemId& item) const;

    // is this item currently selected (the same as has focus)?
    bool IsSelected(const wxTreeItemId& item) const;

    // is item text in bold font?
    bool IsBold(const wxTreeItemId& item) const;


    // if 'recursively' is False, only immediate children count, otherwise
    // the returned number is the number of all items in this branch
    size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = true);


    // wxTreeItemId.IsOk() will return False if there is no such item

    // get the root tree item
    wxTreeItemId GetRootItem() const;

    // get the item currently selected (may return NULL if no selection)
    wxTreeItemId GetSelection() const;

    // get the items currently selected, return the number of such item
    //size_t GetSelections(wxArrayTreeItemIds&) const;
    %extend {
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


    // get the parent of this item (may return NULL if root)
    wxTreeItemId GetItemParent(const wxTreeItemId& item) const;

    // for this enumeration function you must pass in a "cookie" parameter
    // which is opaque for the application but is necessary for the library
    // to make these functions reentrant (i.e. allow more than one
    // enumeration on one and the same object simultaneously). Of course,
    // the "cookie" passed to GetFirstChild() and GetNextChild() should be
    // the same!


    // NOTE: These are a copy of the same methods in _treectrl.i, be sure to
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


        PyObject* GetLastChild(const wxTreeItemId& item) {
            void* cookie = 0;
            wxTreeItemId* ritem = new wxTreeItemId(self->GetLastChild(item, cookie));
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, wxPyConstructObject(ritem, wxT("wxTreeItemId"), true));
            PyTuple_SET_ITEM(tup, 1, wxPyMakeSwigPtr(cookie, wxT("void")));
            wxPyEndBlockThreads(blocked);
            return tup;
        }


        PyObject* GetPrevChild(const wxTreeItemId& item, void* cookie) {
            wxTreeItemId* ritem = new wxTreeItemId(self->GetPrevChild(item, cookie));
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, wxPyConstructObject(ritem, wxT("wxTreeItemId"), true));
            PyTuple_SET_ITEM(tup, 1, wxPyMakeSwigPtr(cookie, wxT("void")));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }


    // get the next sibling of this item
    wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;

    // get the previous sibling
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;


    // get item in the full tree (currently only for internal use)
    wxTreeItemId GetNext(const wxTreeItemId& item) const;
    wxTreeItemId GetPrev(const wxTreeItemId& item) const;

     // get expanded item, see IsExpanded()
    wxTreeItemId GetFirstExpandedItem() const;
    wxTreeItemId GetNextExpanded(const wxTreeItemId& item) const;
    wxTreeItemId GetPrevExpanded(const wxTreeItemId& item) const;

    // get visible item, see IsVisible()
    wxTreeItemId GetFirstVisibleItem(bool fullRow = false) const;
    wxTreeItemId GetNextVisible(const wxTreeItemId& item, bool fullRow = false) const;
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item, bool fullRow = false) const;



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
        wxTreeItemId,  InsertItem(const wxTreeItemId& parent,
                                size_t index,
                                const wxString& text,
                                int image = -1, int selectedImage = -1,
                                wxPyTreeItemData *data = NULL));

    // insert a new item in as the last child of the parent
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxPyTreeItemData *data = NULL);

    %cleardisown(wxPyTreeItemData* data );

    // delete this item and associated data if any
    void Delete(const wxTreeItemId& item);

    // delete all children (but don't delete the item itself)
    // NB: this won't send wxEVT_COMMAND_TREE_ITEM_DELETED events
    void DeleteChildren(const wxTreeItemId& item);

    // delete all items from the tree
    // NB: this won't send wxEVT_COMMAND_TREE_ITEM_DELETED events
    void DeleteRoot();
    %pythoncode { DeleteAllItems = DeleteRoot }



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
    void SelectItem(const wxTreeItemId& item,
                    const wxTreeItemId& last = (wxTreeItemId*)NULL,
                    bool unselect_others=true);

    void SelectAll();

    // make sure this item is visible (expanding the parent item and/or
    // scrolling to this item if necessary)
    void EnsureVisible(const wxTreeItemId& item);

    // scroll to this item (but don't expand its parent)
    void ScrollTo(const wxTreeItemId& item);

    // Returns wxTreeItemId, flags, and column
    wxTreeItemId HitTest(const wxPoint& point, int& OUTPUT, int& OUTPUT);

    %extend {
        // get the bounding rectangle of the item (or of its label only)
        PyObject* GetBoundingRect(const wxTreeItemId& item, bool textOnly = false) {
            wxRect rect;
            if (self->GetBoundingRect(item, rect, textOnly)) {
                wxPyBlock_t blocked = wxPyBeginBlockThreads();
                wxRect* r = new wxRect(rect);
                PyObject* val = wxPyConstructObject((void*)r, wxT("wxRect"), 1);
                wxPyEndBlockThreads(blocked);
                return val;
            }
            else {
                RETURN_NONE();
            }
        }
    }


    %extend {
        // Start editing the item label: this (temporarily) replaces the item
        // with a one line edit control. The item will be selected if it hadn't
        // been before.
        void EditLabel(const wxTreeItemId& item, int column = -1) {
            if (column < 0) column = self->GetMainColumn();
            self->EditLabel(item, column);
        }
    }
    %pythoncode { Edit = EditLabel }

    // sort the children of this item using OnCompareItems
    void SortChildren(const wxTreeItemId& item);

    // searching
    wxTreeItemId FindItem (const wxTreeItemId& item, const wxString& str, int flags = 0);

    // drop over item
    void SetDragItem (const wxTreeItemId& item = (wxTreeItemId*)NULL);

    wxWindow* GetHeaderWindow() const;
    wxScrolledWindow* GetMainWindow() const;

};

//----------------------------------------------------------------------

%init %{
   wxPyPtrTypeMap_Add("wxTreeListCtrl", "wxPyTreeListCtrl");
%}

//----------------------------------------------------------------------
