/////////////////////////////////////////////////////////////////////////////
// Name:        controls2.i
// Purpose:     More control (widget) classes for wxPython
//
// Author:      Robin Dunn
//
// Created:     6/10/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module controls2

%{
#include "helpers.h"
#ifdef __WXMSW__
#include <windows.h>
#endif
#include <wx/listctrl.h>
#include <wx/treectrl.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import windows.i
%import gdi.i
%import events.i
%import controls.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------

%{
extern wxValidator wxPyDefaultValidator;
%}

//----------------------------------------------------------------------

enum {
    wxLIST_MASK_TEXT,
    wxLIST_MASK_IMAGE,
    wxLIST_MASK_DATA,
    wxLIST_MASK_WIDTH,
    wxLIST_MASK_FORMAT,
    wxLIST_MASK_STATE,
    wxLIST_STATE_DONTCARE,
    wxLIST_STATE_DROPHILITED,
    wxLIST_STATE_FOCUSED,
    wxLIST_STATE_SELECTED,
    wxLIST_STATE_CUT,
    wxLIST_HITTEST_ABOVE,
    wxLIST_HITTEST_BELOW,
    wxLIST_HITTEST_NOWHERE,
    wxLIST_HITTEST_ONITEMICON,
    wxLIST_HITTEST_ONITEMLABEL,
    wxLIST_HITTEST_ONITEMRIGHT,
    wxLIST_HITTEST_ONITEMSTATEICON,
    wxLIST_HITTEST_TOLEFT,
    wxLIST_HITTEST_TORIGHT,
    wxLIST_HITTEST_ONITEM,
    wxLIST_NEXT_ABOVE,
    wxLIST_NEXT_ALL,
    wxLIST_NEXT_BELOW,
    wxLIST_NEXT_LEFT,
    wxLIST_NEXT_RIGHT,
    wxLIST_ALIGN_DEFAULT,
    wxLIST_ALIGN_LEFT,
    wxLIST_ALIGN_TOP,
    wxLIST_ALIGN_SNAP_TO_GRID,
    wxLIST_AUTOSIZE,
    wxLIST_AUTOSIZE_USEHEADER,
    wxLIST_RECT_BOUNDS,
    wxLIST_RECT_ICON,
    wxLIST_RECT_LABEL,
    wxLIST_FIND_UP,
    wxLIST_FIND_DOWN,
    wxLIST_FIND_LEFT,
    wxLIST_FIND_RIGHT,
};


enum wxListColumnFormat
{
    wxLIST_FORMAT_LEFT,
    wxLIST_FORMAT_RIGHT,
    wxLIST_FORMAT_CENTRE,
    wxLIST_FORMAT_CENTER = wxLIST_FORMAT_CENTRE
};


class wxListItemAttr
{
public:
    // ctors
    wxListItemAttr();
    //wxListItemAttr(const wxColour& colText,
    //               const wxColour& colBack,
    //               const wxFont& font)
    //    : m_colText(colText), m_colBack(colBack), m_font(font) { }

    // setters
    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetFont(const wxFont& font);

    // accessors
    bool HasTextColour();
    bool HasBackgroundColour();
    bool HasFont();

    const wxColour& GetTextColour();
    const wxColour& GetBackgroundColour();
    const wxFont& GetFont();
};


class wxListItem {
public:
    wxListItem();
    ~wxListItem();

    // resetting
    void Clear();
    void ClearAttributes();

    // setters
    void SetMask(long mask);
    void SetId(long id);
    void SetColumn(int col);
    void SetState(long state);
    void SetStateMask(long stateMask);
    void SetText(const wxString& text);
    void SetImage(int image);
    void SetData(long data);

    void SetWidth(int width);
    void SetAlign(wxListColumnFormat align);

    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetFont(const wxFont& font);

    // accessors
    long GetMask();
    long GetId();
    int GetColumn();
    long GetState();
    const wxString& GetText();
    int GetImage();
    long GetData();

    int GetWidth();
    wxListColumnFormat GetAlign();

    wxListItemAttr *GetAttributes();
    bool HasAttributes();

    wxColour GetTextColour() const;
    wxColour GetBackgroundColour() const;
    wxFont GetFont() const;

    // these members are public for compatibility
    long            m_mask;     // Indicates what fields are valid
    long            m_itemId;   // The zero-based item position
    int             m_col;      // Zero-based column, if in report mode
    long            m_state;    // The state of the item
    long            m_stateMask;// Which flags of m_state are valid (uses same flags)
    wxString        m_text;     // The label/header text
    int             m_image;    // The zero-based index into an image list
    long            m_data;     // App-defined data

    // For columns only
    int             m_format;   // left, right, centre
    int             m_width;    // width of column

};


class wxListEvent: public wxNotifyEvent {
public:
    int           m_code;
    long          m_itemIndex;
    long          m_oldItemIndex;
    int           m_col;
    bool          m_cancelled;
    wxPoint       m_pointDrag;
    wxListItem    m_item;

    int GetCode();
    long GetIndex();
    long GetOldIndex();
    long GetOldItem();
    int GetColumn();
    bool Cancelled();
    wxPoint GetPoint();
    const wxString& GetLabel();
    const wxString& GetText();
    int GetImage();
    long GetData();
    long GetMask();
    const wxListItem& GetItem();
};



class wxListCtrl : public wxControl {
public:
    wxListCtrl(wxWindow* parent, wxWindowID id,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = wxLC_ICON,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "listCtrl");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    bool Arrange(int flag = wxLIST_ALIGN_DEFAULT);
    bool DeleteItem(long item);
    bool DeleteAllItems();
    bool DeleteColumn(int col);
    bool DeleteAllColumns(void);
    void ClearAll(void);
#ifdef __WXMSW__
    wxTextCtrl* EditLabel(long item);
    bool EndEditLabel(bool cancel);
    wxTextCtrl* GetEditControl();
#else
    void EditLabel(long item);
#endif
    bool EnsureVisible(long item);
    long FindItem(long start, const wxString& str, bool partial = FALSE);
    %name(FindItemData)long FindItem(long start, long data);
    %name(FindItemAtPos)long FindItem(long start, const wxPoint& pt,
                                      int direction);
    bool GetColumn(int col, wxListItem& item);
    int GetColumnWidth(int col);
    int GetCountPerPage();
    wxImageList* GetImageList(int which);
    long GetItemData(long item);

    %addmethods {
        %new wxListItem* GetItem(long itemId, int col=0) {
            wxListItem* info = new wxListItem;
            info->m_itemId = itemId;
            info->m_col = col;
            info->m_mask = 0xFFFF;
            self->GetItem(*info);
            return info;
        }
        %new wxPoint* GetItemPosition(long item) {
            wxPoint* pos = new wxPoint;
            self->GetItemPosition(item, *pos);
            return pos;
        }
        %new wxRect* GetItemRect(long item, int code = wxLIST_RECT_BOUNDS) {
            wxRect* rect= new wxRect;
            self->GetItemRect(item, *rect, code);
            return rect;
        }
    }

    int GetItemState(long item, long stateMask);
    int GetItemCount();
    int GetItemSpacing(bool isSmall);
    wxString GetItemText(long item);
    long GetNextItem(long item,
                     int geometry = wxLIST_NEXT_ALL,
                     int state = wxLIST_STATE_DONTCARE);
    int GetSelectedItemCount();
#ifdef __WXMSW__
    wxColour GetTextColour();
    void SetTextColour(const wxColour& col);
#endif
    long GetTopItem();
    long HitTest(const wxPoint& point, int& OUTPUT);
    %name(InsertColumnInfo)long InsertColumn(long col, wxListItem& info);
    long InsertColumn(long col, const wxString& heading,
                      int format = wxLIST_FORMAT_LEFT,
                      int width = -1);

    long InsertItem(wxListItem& info);
    %name(InsertStringItem)     long InsertItem(long index, const wxString& label);
    %name(InsertImageItem)      long InsertItem(long index, int imageIndex);
    %name(InsertImageStringItem)long InsertItem(long index, const wxString& label,
                                                int imageIndex);

    bool ScrollList(int dx, int dy);
    void SetBackgroundColour(const wxColour& col);
    bool SetColumn(int col, wxListItem& item);
    bool SetColumnWidth(int col, int width);
    void SetImageList(wxImageList* imageList, int which);

    bool SetItem(wxListItem& info);
    %name(SetStringItem)long SetItem(long index, int col, const wxString& label,
                                     int imageId = -1);

    bool SetItemData(long item, long data);
    bool SetItemImage(long item, int image, int selImage);
    bool SetItemPosition(long item, const wxPoint& pos);
    bool SetItemState(long item, long state, long stateMask);
    void SetItemText(long item, const wxString& text);
    void SetSingleStyle(long style, bool add = TRUE);
    void SetWindowStyleFlag(long style);

    // bool SortItems(wxListCtrlCompare fn, long data);
    %addmethods {
        bool SortItems(PyObject* func) {
            if (!PyCallable_Check(func))
                return FALSE;

            return self->SortItems(wxPyListCtrl_SortItems, (long)func);
        }
    }
};

%{
    int wxCALLBACK wxPyListCtrl_SortItems(long item1, long item2, long funcPtr) {
        int retval = 0;
        PyObject* func = (PyObject*)funcPtr;
        bool doSave = wxPyRestoreThread();

        PyObject* args = Py_BuildValue("(ii)", item1, item2);
        PyObject* result = PyEval_CallObject(func, args);
        Py_DECREF(args);
        if (result) {
            retval = PyInt_AsLong(result);
            Py_DECREF(result);
        }

        wxPySaveThread(doSave);
        return retval;
    }

%}

//----------------------------------------------------------------------

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


class wxTreeItemId {
public:
    wxTreeItemId();
    ~wxTreeItemId();
    bool IsOk();

    %addmethods {
        int __cmp__(wxTreeItemId* other) {
            if (! other) return -1;
            return *self != *other;
        }
    }
};



%{
class wxPyTreeItemData : public wxTreeItemData {
public:
    wxPyTreeItemData(PyObject* obj = NULL) {
        if (obj == NULL)
            obj = Py_None;
        Py_INCREF(obj);
        m_obj = obj;
    }

    ~wxPyTreeItemData() {
        bool doSave = wxPyRestoreThread();
        Py_DECREF(m_obj);
        wxPySaveThread(doSave);
    }

    PyObject* GetData() {
        Py_INCREF(m_obj);
        return m_obj;
    }

    void SetData(PyObject* obj) {
        bool doSave = wxPyRestoreThread();
        Py_DECREF(m_obj);
        wxPySaveThread(doSave);
        m_obj = obj;
        Py_INCREF(obj);
    }

    PyObject* m_obj;
};
%}



%name(wxTreeItemData) class wxPyTreeItemData {
public:
    wxPyTreeItemData(PyObject* obj = NULL);

    PyObject* GetData();
    void      SetData(PyObject* obj);

    const wxTreeItemId& GetId();
    void                SetId(const wxTreeItemId& id);
};



class wxTreeEvent : public wxNotifyEvent {
public:
    wxTreeItemId GetItem();
    wxTreeItemId GetOldItem();
    wxPoint GetPoint();
    int GetCode();
    const wxString& GetLabel();
};



%{
class wxPyTreeCtrl : public wxTreeCtrl {
public:
    wxPyTreeCtrl(wxWindow *parent, wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxValidator& validator,
                 char* name) :
        wxTreeCtrl(parent, id, pos, size, style, validator, name) {}


    int OnCompareItems(const wxTreeItemId& item1,
                       const wxTreeItemId& item2) {
        int rval = 0;
        bool doSave = wxPyRestoreThread();
        if (m_myInst.findCallback("OnCompareItems"))
            rval = m_myInst.callCallback(Py_BuildValue(
                "(OO)",
                wxPyConstructObject((void*)&item1, "wxTreeItemId"),
                wxPyConstructObject((void*)&item2, "wxTreeItemId")));
        else
            rval = wxTreeCtrl::OnCompareItems(item1, item2);
        wxPySaveThread(doSave);
        return rval;
    }
    PYPRIVATE;
};

%}

// These are for the GetFirstChild/GetNextChild methods below
%typemap(python, in)     long& INOUT = long* INOUT;
%typemap(python, argout) long& INOUT = long* INOUT;


%name(wxTreeCtrl)class wxPyTreeCtrl : public wxControl {
public:
    wxPyTreeCtrl(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "wxTreeCtrl");

    void _setSelf(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxTreeCtrl)"

    size_t GetCount();
    unsigned int GetIndent();
    void SetIndent(unsigned int indent);
    wxImageList *GetImageList();
    wxImageList *GetStateImageList();
    void SetImageList(wxImageList *imageList/*, int which = wxIMAGE_LIST_NORMAL*/);
    void SetStateImageList(wxImageList *imageList);

    unsigned int GetSpacing();
    void SetSpacing(unsigned int spacing);

    wxString GetItemText(const wxTreeItemId& item);
    int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal);
    int GetItemSelectedImage(const wxTreeItemId& item);

    void SetItemText(const wxTreeItemId& item, const wxString& text);
    void SetItemImage(const wxTreeItemId& item, int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal);
    void SetItemSelectedImage(const wxTreeItemId& item, int image);
    void SetItemHasChildren(const wxTreeItemId& item, bool hasChildren = TRUE);

    %addmethods {
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


    bool IsVisible(const wxTreeItemId& item);
    bool ItemHasChildren(const wxTreeItemId& item);
    bool IsExpanded(const wxTreeItemId& item);
    bool IsSelected(const wxTreeItemId& item);

    wxTreeItemId GetRootItem();
    wxTreeItemId GetSelection();
    %name(GetItemParent) wxTreeItemId GetParent(const wxTreeItemId& item);
    //size_t GetSelections(wxArrayTreeItemIds& selection);
    %addmethods {
        PyObject* GetSelections() {
            bool doSave = wxPyRestoreThread();
            PyObject*           rval = PyList_New(0);
            wxArrayTreeItemIds  array;
            size_t              num, x;
            num = self->GetSelections(array);
            for (x=0; x < num; x++) {
                wxTreeItemId *tii = new wxTreeItemId(array.Item(x));
                PyObject* item = wxPyConstructObject((void*)tii, "wxTreeItemId", TRUE);
                PyList_Append(rval, item);
            }
            wxPySaveThread(doSave);
            return rval;
        }
    }



    size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = TRUE);

    wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& INOUT);
    wxTreeItemId GetNextChild(const wxTreeItemId& item, long& INOUT);
    wxTreeItemId GetNextSibling(const wxTreeItemId& item);
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item);
    wxTreeItemId GetFirstVisibleItem();
    wxTreeItemId GetNextVisible(const wxTreeItemId& item);
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item);
    wxTreeItemId GetLastChild(const wxTreeItemId& item);



    wxTreeItemId AddRoot(const wxString& text,
                         int image = -1, int selectedImage = -1,
                         wxPyTreeItemData *data = NULL);
    wxTreeItemId PrependItem(const wxTreeItemId& parent,
                             const wxString& text,
                             int image = -1, int selectedImage = -1,
                             wxPyTreeItemData *data = NULL);
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            const wxTreeItemId& idPrevious,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxPyTreeItemData *data = NULL);
    %name(InsertItemBefore)
        wxTreeItemId InsertItem(const wxTreeItemId& parent,
                                size_t before,
                                const wxString& text,
                                int image = -1, int selectedImage = -1,
                                wxTreeItemData *data = NULL);
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxPyTreeItemData *data = NULL);

    void Delete(const wxTreeItemId& item);
    void DeleteChildren(const wxTreeItemId& item);
    void DeleteAllItems();

    void Expand(const wxTreeItemId& item);
    void Collapse(const wxTreeItemId& item);
    void CollapseAndReset(const wxTreeItemId& item);
    void Toggle(const wxTreeItemId& item);

    void Unselect();
    void UnselectAll();
    void SelectItem(const wxTreeItemId& item);
    void EnsureVisible(const wxTreeItemId& item);
    void ScrollTo(const wxTreeItemId& item);
#ifdef __WXMSW__
    wxTextCtrl* EditLabel(const wxTreeItemId& item);
    wxTextCtrl* GetEditControl();
    void EndEditLabel(const wxTreeItemId& item, int discardChanges = FALSE);
#else
    void EditLabel(const wxTreeItemId& item);
#endif

    void SortChildren(const wxTreeItemId& item);

    void SetItemBold(const wxTreeItemId& item, int bold = TRUE);
    bool IsBold(const wxTreeItemId& item) const;
    wxTreeItemId HitTest(const wxPoint& point, int& OUTPUT);



    void SetItemTextColour(const wxTreeItemId& item, const wxColour& col);
    void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col);
    void SetItemFont(const wxTreeItemId& item, const wxFont& font);

#ifdef __WXMSW__
    void SetItemDropHighlight(const wxTreeItemId& item, int highlight = TRUE);

    //bool GetBoundingRect(const wxTreeItemId& item, wxRect& rect, int textOnly = FALSE)
    %addmethods {
        PyObject* GetBoundingRect(const wxTreeItemId& item, int textOnly = FALSE) {
            wxRect rect;
            if (self->GetBoundingRect(item, rect, textOnly)) {
                bool doSave = wxPyRestoreThread();
                wxRect* r = new wxRect(rect);
                PyObject* val = wxPyConstructObject((void*)r, "wxRect");
                wxPySaveThread(doSave);
                return val;
            }
            else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }
    }
#endif

%pragma(python) addtoclass = "
    # Redefine some methods that SWIG gets a bit confused on...
    def GetFirstChild(self, *_args, **_kwargs):
        val1,val2 = apply(controls2c.wxTreeCtrl_GetFirstChild,(self,) + _args, _kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1,val2)
    def GetNextChild(self, *_args, **_kwargs):
        val1,val2 = apply(controls2c.wxTreeCtrl_GetNextChild,(self,) + _args, _kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1,val2)
    def HitTest(self, *_args, **_kwargs):
        val1, val2 = apply(controls2c.wxTreeCtrl_HitTest,(self,) + _args, _kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1,val2)
"
};


//----------------------------------------------------------------------

#ifdef SKIPTHIS
#ifdef __WXMSW__
class wxTabEvent : public wxCommandEvent {
public:
};



class wxTabCtrl : public wxControl {
public:
    wxTabCtrl(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxPyDefaultPosition,
              const wxSize& size = wxPyDefaultSize,
              long style = 0,
              char* name = "tabCtrl");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    bool DeleteAllItems();
    bool DeleteItem(int item);
    wxImageList* GetImageList();
    int GetItemCount();
    // TODO: void* GetItemData();
    int GetItemImage(int item);

    %addmethods {
        %new wxRect* GetItemRect(int item) {
            wxRect* rect = new wxRect;
            self->GetItemRect(item, *rect);
            return rect;
        }
    }

    wxString GetItemText(int item);
    bool GetRowCount();
    int GetSelection();
    int HitTest(const wxPoint& pt, long& OUTPUT);
    void InsertItem(int item, const wxString& text,
                    int imageId = -1, void* clientData = NULL);
    // TODO: bool SetItemData(int item, void* data);
    bool SetItemImage(int item, int image);
    void SetImageList(wxImageList* imageList);
    void SetItemSize(const wxSize& size);
    bool SetItemText(int item, const wxString& text);
    void SetPadding(const wxSize& padding);
    int SetSelection(int item);

};

#endif
#endif

//----------------------------------------------------------------------


