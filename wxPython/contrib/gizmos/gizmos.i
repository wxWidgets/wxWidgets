/////////////////////////////////////////////////////////////////////////////
// Name:        gizmos.i
// Purpose:     Wrappers for the "gizmo" classes in wx/contrib
//
// Author:      Robin Dunn
//
// Created:     23-Nov-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module gizmos


%{
#include "wxPython.h"
#include <wx/gizmos/dynamicsash.h>
#include <wx/gizmos/editlbox.h>
#include <wx/gizmos/splittree.h>
#include <wx/gizmos/ledctrl.h>

#include <wx/gizmos/treelistctrl.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include "pytree.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern events.i
%extern controls.i
%extern controls2.i
%extern gdi.i


//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyDynamicSashNameStr(wxT("dynamicSashWindow"));
    static const wxString wxPyEditableListBoxNameStr(wxT("editableListBox"));
    static const wxString wxPyTreeListCtrlNameStr(wxT("treelistctrl"));
    static const wxString wxPyEmptyString(wxT(""));
%}

///----------------------------------------------------------------------

enum {
    wxEVT_DYNAMIC_SASH_SPLIT,
    wxEVT_DYNAMIC_SASH_UNIFY,

    wxDS_MANAGE_SCROLLBARS,
    wxDS_DRAG_CORNER,
};


/*
    wxDynamicSashSplitEvents are sent to your view by wxDynamicSashWindow
    whenever your view is being split by the user.  It is your
    responsibility to handle this event by creating a new view window as
    a child of the wxDynamicSashWindow.  wxDynamicSashWindow will
    automatically reparent it to the proper place in its window hierarchy.
*/
class wxDynamicSashSplitEvent : public wxCommandEvent {
public:
    wxDynamicSashSplitEvent(wxObject *target);
};


/*
    wxDynamicSashUnifyEvents are sent to your view by wxDynamicSashWindow
    whenever the sash which splits your view and its sibling is being
    reunified such that your view is expanding to replace its sibling.
    You needn't do anything with this event if you are allowing
    wxDynamicSashWindow to manage your view's scrollbars, but it is useful
    if you are managing the scrollbars yourself so that you can keep
    the scrollbars' event handlers connected to your view's event handler
    class.
*/
class wxDynamicSashUnifyEvent : public wxCommandEvent {
public:
    wxDynamicSashUnifyEvent(wxObject *target);
};



/*

    wxDynamicSashWindow

    wxDynamicSashWindow widgets manages the way other widgets are viewed.
    When a wxDynamicSashWindow is first shown, it will contain one child
    view, a viewport for that child, and a pair of scrollbars to allow the
    user to navigate the child view area.  Next to each scrollbar is a small
    tab.  By clicking on either tab and dragging to the appropriate spot, a
    user can split the view area into two smaller views separated by a
    draggable sash.  Later, when the user wishes to reunify the two subviews,
    the user simply drags the sash to the side of the window.
    wxDynamicSashWindow will automatically reparent the appropriate child
    view back up the window hierarchy, and the wxDynamicSashWindow will have
    only one child view once again.

    As an application developer, you will simply create a wxDynamicSashWindow
    using either the Create() function or the more complex constructor
    provided below, and then create a view window whose parent is the
    wxDynamicSashWindow.  The child should respond to
    wxDynamicSashSplitEvents -- perhaps with an OnSplit() event handler -- by
    constructing a new view window whose parent is also the
    wxDynamicSashWindow.  That's it!  Now your users can dynamically split
    and reunify the view you provided.

    If you wish to handle the scrollbar events for your view, rather than
    allowing wxDynamicSashWindow to do it for you, things are a bit more
    complex.  (You might want to handle scrollbar events yourself, if,
    for instance, you wish to scroll a subwindow of the view you add to
    your wxDynamicSashWindow object, rather than scrolling the whole view.)
    In this case, you will need to construct your wxDynamicSashWindow without
    the wxDS_MANAGE_SCROLLBARS style and  you will need to use the
    GetHScrollBar() and GetVScrollBar() methods to retrieve the scrollbar
    controls and call SetEventHanler() on them to redirect the scrolling
    events whenever your window is reparented by wxDyanmicSashWindow.
    You will need to set the scrollbars' event handler at three times:

        *  When your view is created
        *  When your view receives a wxDynamicSashSplitEvent
        *  When your view receives a wxDynamicSashUnifyEvent

    See the dynsash_switch sample application for an example which does this.

*/

class wxDynamicSashWindow : public wxWindow {
public:
    wxDynamicSashWindow(wxWindow *parent, wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                        long style = wxCLIP_CHILDREN | wxDS_MANAGE_SCROLLBARS | wxDS_DRAG_CORNER,
                        const wxString& name = wxPyDynamicSashNameStr);
    %name(wxPreDynamicSashWindow)wxDynamicSashWindow();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxCLIP_CHILDREN | wxDS_MANAGE_SCROLLBARS | wxDS_DRAG_CORNER,
                const wxString& name = wxPyDynamicSashNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreDynamicSashWindow:val._setOORInfo(val)"

    wxScrollBar *GetHScrollBar(const wxWindow *child) const;
    wxScrollBar *GetVScrollBar(const wxWindow *child) const;
};



//----------------------------------------------------------------------
// Python functions to act like the event macros

%pragma(python) code = "
def EVT_DYNAMIC_SASH_SPLIT(win, id, func):
    win.Connect(id, -1, wxEVT_DYNAMIC_SASH_SPLIT, func)

def EVT_DYNAMIC_SASH_UNIFY(win, id, func):
    win.Connect(id, -1, wxEVT_DYNAMIC_SASH_UNIFY, func)
"

//----------------------------------------------------------------------
//----------------------------------------------------------------------

enum {
    wxEL_ALLOW_NEW,
    wxEL_ALLOW_EDIT,
    wxEL_ALLOW_DELETE,
};

// This class provides a composite control that lets the
// user easily enter list of strings
class wxEditableListBox : public wxPanel
{
public:
    wxEditableListBox(wxWindow *parent, wxWindowID id,
                      const wxString& label,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxEL_ALLOW_NEW | wxEL_ALLOW_EDIT | wxEL_ALLOW_DELETE,
                      const wxString& name = wxPyEditableListBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void SetStrings(const wxArrayString& strings);

    //void GetStrings(wxArrayString& strings);
    %addmethods {
        PyObject* GetStrings() {
            wxArrayString strings;
            self->GetStrings(strings);
            return wxArrayString2PyList_helper(strings);
        }
    }

    wxListCtrl* GetListCtrl()       { return m_listCtrl; }
    wxBitmapButton* GetDelButton()  { return m_bDel; }
    wxBitmapButton* GetNewButton()  { return m_bNew; }
    wxBitmapButton* GetUpButton()   { return m_bUp; }
    wxBitmapButton* GetDownButton() { return m_bDown; }
    wxBitmapButton* GetEditButton() { return m_bEdit; }
};



//----------------------------------------------------------------------


/*
 * wxRemotelyScrolledTreeCtrl
 *
 * This tree control disables its vertical scrollbar and catches scroll
 * events passed by a scrolled window higher in the hierarchy.
 * It also updates the scrolled window vertical scrollbar as appropriate.
 */

%{
    typedef wxTreeCtrl wxPyTreeCtrl;
%}

class wxRemotelyScrolledTreeCtrl: public wxPyTreeCtrl
{
public:
    wxRemotelyScrolledTreeCtrl(wxWindow* parent, wxWindowID id,
                               const wxPoint& pos = wxDefaultPosition,
                               const wxSize& size = wxDefaultSize,
                               long style = wxTR_HAS_BUTTONS);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"


    void HideVScrollbar();

    // Adjust the containing wxScrolledWindow's scrollbars appropriately
    void AdjustRemoteScrollbars();

    // Find the scrolled window that contains this control
    wxScrolledWindow* GetScrolledWindow() const;

    // Scroll to the given line (in scroll units where each unit is
    // the height of an item)
    void ScrollToLine(int posHoriz, int posVert);

    // The companion window is one which will get notified when certain
    // events happen such as node expansion
    void SetCompanionWindow(wxWindow* companion);
    wxWindow* GetCompanionWindow() const;
};



/*
 * wxTreeCompanionWindow
 *
 * A window displaying values associated with tree control items.
 */

%{
class wxPyTreeCompanionWindow: public wxTreeCompanionWindow
{
public:
    wxPyTreeCompanionWindow(wxWindow* parent, wxWindowID id = -1,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0)
        : wxTreeCompanionWindow(parent, id, pos, size, style) {}


    virtual void DrawItem(wxDC& dc, wxTreeItemId id, const wxRect& rect) {
        bool found;
        wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DrawItem"))) {
            PyObject* dcobj = wxPyMake_wxObject(&dc);
            PyObject* idobj = wxPyConstructObject((void*)&id, wxT("wxTreeItemId"), FALSE);
            PyObject* recobj= wxPyConstructObject((void*)&rect, wxT("wxRect"), FALSE);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOO)", dcobj, idobj, recobj));
            Py_DECREF(dcobj);
            Py_DECREF(idobj);
            Py_DECREF(recobj);
        }
        wxPyEndBlockThreads();
        if (! found)
            wxTreeCompanionWindow::DrawItem(dc, id, rect);
    }

    PYPRIVATE;
};
%}


%name(wxTreeCompanionWindow) class wxPyTreeCompanionWindow: public wxWindow
{
public:
    wxPyTreeCompanionWindow(wxWindow* parent, wxWindowID id = -1,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0);
    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxTreeCompanionWindow)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    wxRemotelyScrolledTreeCtrl* GetTreeCtrl() const;
    void SetTreeCtrl(wxRemotelyScrolledTreeCtrl* treeCtrl);
};



/*
 * wxThinSplitterWindow
 *
 * Implements a splitter with a less obvious sash
 * than the usual one.
 */

class wxThinSplitterWindow: public wxSplitterWindow
{
public:
    wxThinSplitterWindow(wxWindow* parent, wxWindowID id = -1,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxSP_3D | wxCLIP_CHILDREN);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

};


/*
 * wxSplitterScrolledWindow
 *
 * This scrolled window is aware of the fact that one of its
 * children is a splitter window. It passes on its scroll events
 * (after some processing) to both splitter children for them
 * scroll appropriately.
 */

class wxSplitterScrolledWindow: public wxScrolledWindow
{
public:
    wxSplitterScrolledWindow(wxWindow* parent, wxWindowID id = -1,
                             const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxDefaultSize,
                             long style = 0);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
};


//----------------------------------------------------------------------
//----------------------------------------------------------------------


enum wxLEDValueAlign
{
    wxLED_ALIGN_LEFT,
    wxLED_ALIGN_RIGHT,
    wxLED_ALIGN_CENTER,

    wxLED_ALIGN_MASK,

    wxLED_DRAW_FADED,
};


class wxLEDNumberCtrl :	public wxControl
{
public:
    // Constructors.
    wxLEDNumberCtrl(wxWindow *parent, wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style =  wxLED_ALIGN_LEFT | wxLED_DRAW_FADED);
    %name(wxPreLEDNumberCtrl) wxLEDNumberCtrl();

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreLEDNumberCtrl:val._setOORInfo(val)"

    // Create functions.
    bool Create(wxWindow *parent, wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxLED_ALIGN_LEFT | wxLED_DRAW_FADED);

    wxLEDValueAlign GetAlignment() const { return m_Alignment; }
    bool GetDrawFaded() const { return m_DrawFaded; }
    const wxString &GetValue() const { return m_Value; }

    void SetAlignment(wxLEDValueAlign Alignment, bool Redraw = true);
    void SetDrawFaded(bool DrawFaded, bool Redraw = true);
    void SetValue(const wxString &Value, bool Redraw = true);

};



//----------------------------------------------------------------------------
// wxTreeListCtrl - the multicolumn tree control
//----------------------------------------------------------------------------

enum wxTreeListColumnAlign {
    wxTL_ALIGN_LEFT,
    wxTL_ALIGN_RIGHT,
    wxTL_ALIGN_CENTER
};



enum {
    wxTREE_HITTEST_ONITEMCOLUMN
};




class wxTreeListColumnInfo: public wxObject {
public:
    wxTreeListColumnInfo(const wxString& text = wxPyEmptyString,
			 int image = -1,
			 size_t width = 100,
			 wxTreeListColumnAlign alignment = wxTL_ALIGN_LEFT);

    wxTreeListColumnAlign GetAlignment() const;
    wxString GetText() const;
    int GetImage() const;
    int GetSelectedImage() const;
    size_t GetWidth() const;

    void SetAlignment(wxTreeListColumnAlign alignment);
    void SetText(const wxString& text);
    void SetImage(int image);
    void SetSelectedImage(int image);
    void SetWidth(size_t with);
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

    int OnCompareItems(const wxTreeItemId& item1,
                       const wxTreeItemId& item2) {
        int rval = 0;
        bool found;
        wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "OnCompareItems"))) {
            PyObject *o1 = wxPyConstructObject((void*)&item1, wxT("wxTreeItemId"), 0);
            PyObject *o2 = wxPyConstructObject((void*)&item2, wxT("wxTreeItemId"), 0);
            rval = wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OO)",o1,o2));
            Py_DECREF(o1);
            Py_DECREF(o2);
        }
        wxPyEndBlockThreads();
        if (! found)
            rval = wxTreeListCtrl::OnCompareItems(item1, item2);
        return rval;
    }
    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyTreeListCtrl, wxTreeListCtrl)

%}




// These are for the GetFirstChild/GetNextChild methods below
%{
    static const long longzero = 0;
%}
%typemap(python, in)     long& INOUT = long* INOUT;
%typemap(python, argout) long& INOUT = long* INOUT;


%name(wxTreeListCtrl) class wxPyTreeListCtrl : public wxControl
{
public:
    wxPyTreeListCtrl(wxWindow *parent, wxWindowID id = -1,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxTR_DEFAULT_STYLE,
                   const wxValidator &validator = wxDefaultValidator,
                   const wxString& name = wxPyTreeListCtrlNameStr );
    %name(wxPreTreeListCtrl)wxPyTreeListCtrl();

    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_DEFAULT_STYLE,
                const wxValidator &validator = wxDefaultValidator,
                const wxString& name = wxPyTreeListCtrlNameStr );

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxTreeListCtrl)"

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreTreeListCtrl:val._setOORInfo(val)"



    // get the total number of items in the control
    size_t GetCount() const;

    // indent is the number of pixels the children are indented relative to
    // the parents position. SetIndent() also redraws the control
    // immediately.
    unsigned int GetIndent() const;
    void SetIndent(unsigned int indent);

    // spacing is the number of pixels between the start and the Text
    unsigned int GetSpacing() const;
    void SetSpacing(unsigned int spacing);

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



    // adds a column
    void AddColumn(const wxString& text);
    %name(AddColumnInfo) void AddColumn(const wxTreeListColumnInfo& col);

    // inserts a column before the given one
    void InsertColumn(size_t before, const wxString& text);
    %name(InsertColumnInfo) void InsertColumn(size_t before, const wxTreeListColumnInfo& col);

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



    %addmethods {
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

        void SetItemData(const wxTreeItemId& item, wxPyTreeItemData* data) {
            data->SetId(item); // set the id
            self->SetItemData(item, data);
        }

        // [Get|Set]PyData are short-cuts.  Also made somewhat crash-proof by
        // automatically creating data classes.
        PyObject* GetPyData(const wxTreeItemId& item) {
            wxPyTreeItemData* data = (wxPyTreeItemData*)self->GetItemData(item);
            if (data == NULL) {
                data = new wxPyTreeItemData();
                data->SetId(item); // set the id
                self->SetItemData(item, data);
            }
            return data->GetData();
        }

        void SetPyData(const wxTreeItemId& item, PyObject* obj) {
            wxPyTreeItemData* data = (wxPyTreeItemData*)self->GetItemData(item);
            if (data == NULL) {
                data = new wxPyTreeItemData(obj);
                data->SetId(item); // set the id
                self->SetItemData(item, data);
            } else
                data->SetData(obj);
        }
    }


    // force appearance of [+] button near the item. This is useful to
    // allow the user to expand the items which don't have any children now
    // - but instead add them only when needed, thus minimizing memory
    // usage and loading time.
    void SetItemHasChildren(const wxTreeItemId& item, bool has = TRUE);

    // the item will be shown in bold
    void SetItemBold(const wxTreeItemId& item, bool bold = TRUE);

    // set the item's text colour
    void SetItemTextColour(const wxTreeItemId& item, const wxColour& col);

    // set the item's background colour
    void SetItemBackgroundColour(const wxTreeItemId& item,
				 const wxColour& col);

    // set the item's font (should be of the same height for all items)
    void SetItemFont(const wxTreeItemId& item, const wxFont& font);


    // TODO:  Where are the Getters for item colour, font, etc?


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

    // if 'recursively' is FALSE, only immediate children count, otherwise
    // the returned number is the number of all items in this branch
    size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = TRUE);


    // wxTreeItemId.IsOk() will return FALSE if there is no such item

    // get the root tree item
    wxTreeItemId GetRootItem() const;

    // get the item currently selected (may return NULL if no selection)
    wxTreeItemId GetSelection() const;

    // get the items currently selected, return the number of such item
    size_t GetSelections(wxArrayTreeItemIds&) const;

    // get the parent of this item (may return NULL if root)
    %name(GetItemParent)wxTreeItemId GetParent(const wxTreeItemId& item) const;

    // for this enumeration function you must pass in a "cookie" parameter
    // which is opaque for the application but is necessary for the library
    // to make these functions reentrant (i.e. allow more than one
    // enumeration on one and the same object simultaneously). Of course,
    // the "cookie" passed to GetFirstChild() and GetNextChild() should be
    // the same!

    // get the first child of this item
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& INOUT = longzero) const;

    // get the next child
    wxTreeItemId GetNextChild(const wxTreeItemId& item, long& INOUT) const;

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
    %name(InsertItemBefore)
        wxTreeItemId InsertItem(const wxTreeItemId& parent,
                                size_t index,
                                const wxString& text,
                                int image = -1, int selectedImage = -1,
                                wxPyTreeItemData *data = NULL);

    // insert a new item in as the last child of the parent
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxPyTreeItemData *data = NULL);

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

    // make sure this item is visible (expanding the parent item and/or
    // scrolling to this item if necessary)
    void EnsureVisible(const wxTreeItemId& item);

    // scroll to this item (but don't expand its parent)
    void ScrollTo(const wxTreeItemId& item);

    // Returns wxTreeItemId, flags, and column
    wxTreeItemId HitTest(const wxPoint& point, int& OUTPUT, int& OUTPUT);

    %addmethods {
        // get the bounding rectangle of the item (or of its label only)
        PyObject* GetBoundingRect(const wxTreeItemId& item, bool textOnly = FALSE) {
            wxRect rect;
            if (self->GetBoundingRect(item, rect, textOnly)) {
                wxPyBeginBlockThreads();
                wxRect* r = new wxRect(rect);
                PyObject* val = wxPyConstructObject((void*)r, wxT("wxRect"), 1);
                wxPyEndBlockThreads();
                return val;
            }
            else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }
    }


    // Start editing the item label: this (temporarily) replaces the item
    // with a one line edit control. The item will be selected if it hadn't
    // been before.
    void EditLabel( const wxTreeItemId& item );
    void Edit( const wxTreeItemId& item );

    // sort the children of this item using OnCompareItems
    void SortChildren(const wxTreeItemId& item);

    // get the selected item image
    int GetItemSelectedImage(const wxTreeItemId& item) const;

    // set the selected item image
    void SetItemSelectedImage(const wxTreeItemId& item, int image);


    wxWindow* GetHeaderWindow() const;
    wxWindow* GetMainWindow() const;

%pragma(python) addtoclass = "
    # Redefine some methods that SWIG gets a bit confused on...
    def GetFirstChild(self, *_args, **_kwargs):
        val1,val2 = controls2c.wxTreeCtrl_GetFirstChild(self, *_args, **_kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1,val2)
    def GetNextChild(self, *_args, **_kwargs):
        val1,val2 = controls2c.wxTreeCtrl_GetNextChild(self, *_args, **_kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1,val2)
    def HitTest(self, *_args, **_kwargs):
        val1, val2, val3 = controls2c.wxTreeCtrl_HitTest(self, *_args, **_kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1, val2, val3)
"
};




//----------------------------------------------------------------------
//----------------------------------------------------------------------

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

    wxPyPtrTypeMap_Add("wxTreeCompanionWindow", "wxPyTreeCompanionWindow");
    wxPyPtrTypeMap_Add("wxTreeListCtrl", "wxPyTreeListCtrl");
%}


%pragma(python) include="_gizmoextras.py";

//----------------------------------------------------------------------
//----------------------------------------------------------------------






