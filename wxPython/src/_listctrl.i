/////////////////////////////////////////////////////////////////////////////
// Name:        _listctrl.i
// Purpose:     SWIG interface file for wxListCtrl and related classes
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
#include <wx/listctrl.h>
%}

MAKE_CONST_WXSTRING(ListCtrlNameStr);

//---------------------------------------------------------------------------
%newgroup

enum {
    // style flags
    wxLC_VRULES,
    wxLC_HRULES,

    wxLC_ICON,
    wxLC_SMALL_ICON,
    wxLC_LIST,
    wxLC_REPORT,

    wxLC_ALIGN_TOP,
    wxLC_ALIGN_LEFT,
    wxLC_AUTOARRANGE,
    wxLC_VIRTUAL,
    wxLC_EDIT_LABELS,
    wxLC_NO_HEADER,
    wxLC_NO_SORT_HEADER,
    wxLC_SINGLE_SEL,
    wxLC_SORT_ASCENDING,
    wxLC_SORT_DESCENDING,

    wxLC_MASK_TYPE,
    wxLC_MASK_ALIGN,
    wxLC_MASK_SORT
};


enum {
    // Mask flags to tell app/GUI what fields of wxListItem are valid
    wxLIST_MASK_STATE,
    wxLIST_MASK_TEXT,
    wxLIST_MASK_IMAGE,
    wxLIST_MASK_DATA,
    wxLIST_SET_ITEM,
    wxLIST_MASK_WIDTH,
    wxLIST_MASK_FORMAT,

// State flags for indicating the state of an item
    wxLIST_STATE_DONTCARE,
    wxLIST_STATE_DROPHILITED,
    wxLIST_STATE_FOCUSED,
    wxLIST_STATE_SELECTED,
    wxLIST_STATE_CUT,
    wxLIST_STATE_DISABLED,
    wxLIST_STATE_FILTERED,
    wxLIST_STATE_INUSE,
    wxLIST_STATE_PICKED,
    wxLIST_STATE_SOURCE,

// Hit test flags, used in HitTest
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

// GetSubItemRect constants    
    wxLIST_GETSUBITEMRECT_WHOLEITEM,
};


// Flags for GetNextItem (MSW only except wxLIST_NEXT_ALL)
enum
{
    wxLIST_NEXT_ABOVE,          // Searches for an item above the specified item
    wxLIST_NEXT_ALL,            // Searches for subsequent item by index
    wxLIST_NEXT_BELOW,          // Searches for an item below the specified item
    wxLIST_NEXT_LEFT,           // Searches for an item to the left of the specified item
    wxLIST_NEXT_RIGHT           // Searches for an item to the right of the specified item
};

// Alignment flags for Arrange (MSW only except wxLIST_ALIGN_LEFT)
enum
{
    wxLIST_ALIGN_DEFAULT,
    wxLIST_ALIGN_LEFT,
    wxLIST_ALIGN_TOP,
    wxLIST_ALIGN_SNAP_TO_GRID
};

// Column format (MSW only except wxLIST_FORMAT_LEFT)
enum wxListColumnFormat
{
    wxLIST_FORMAT_LEFT,
    wxLIST_FORMAT_RIGHT,
    wxLIST_FORMAT_CENTRE,
    wxLIST_FORMAT_CENTER = wxLIST_FORMAT_CENTRE
};

// Autosize values for SetColumnWidth
enum
{
    wxLIST_AUTOSIZE = -1,
    wxLIST_AUTOSIZE_USEHEADER = -2      // partly supported by generic version
};

// Flag values for GetItemRect
enum
{
    wxLIST_RECT_BOUNDS,
    wxLIST_RECT_ICON,
    wxLIST_RECT_LABEL
};

// Flag values for FindItem (MSW only)
enum
{
    wxLIST_FIND_UP,
    wxLIST_FIND_DOWN,
    wxLIST_FIND_LEFT,
    wxLIST_FIND_RIGHT
};



//---------------------------------------------------------------------------
%newgroup

// wxListItemAttr: a structure containing the visual attributes of an item
class wxListItemAttr
{
public:
    // ctors
    //wxListItemAttr();
    wxListItemAttr(const wxColour& colText = wxNullColour,
                   const wxColour& colBack = wxNullColour,
                   const wxFont& font = wxNullFont);
    ~wxListItemAttr();

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

    void AssignFrom(const wxListItemAttr& source);

    %pythonAppend Destroy "args[0].thisown = 0"
    %extend { void Destroy() { delete self; } }

    %property(BackgroundColour, GetBackgroundColour, SetBackgroundColour, doc="See `GetBackgroundColour` and `SetBackgroundColour`");
    %property(Font, GetFont, SetFont, doc="See `GetFont` and `SetFont`");
    %property(TextColour, GetTextColour, SetTextColour, doc="See `GetTextColour` and `SetTextColour`");
};




//---------------------------------------------------------------------------
%newgroup


// wxListItem: the item or column info, used to exchange data with wxListCtrl
class wxListItem : public wxObject {
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

    %property(Align, GetAlign, SetAlign, doc="See `GetAlign` and `SetAlign`");
    %property(Attributes, GetAttributes, doc="See `GetAttributes`");
    %property(BackgroundColour, GetBackgroundColour, SetBackgroundColour, doc="See `GetBackgroundColour` and `SetBackgroundColour`");
    %property(Column, GetColumn, SetColumn, doc="See `GetColumn` and `SetColumn`");
    %property(Data, GetData, SetData, doc="See `GetData` and `SetData`");
    %property(Font, GetFont, SetFont, doc="See `GetFont` and `SetFont`");
    %property(Id, GetId, SetId, doc="See `GetId` and `SetId`");
    %property(Image, GetImage, SetImage, doc="See `GetImage` and `SetImage`");
    %property(Mask, GetMask, SetMask, doc="See `GetMask` and `SetMask`");
    %property(State, GetState, SetState, doc="See `GetState` and `SetState`");
    %property(Text, GetText, SetText, doc="See `GetText` and `SetText`");
    %property(TextColour, GetTextColour, SetTextColour, doc="See `GetTextColour` and `SetTextColour`");
    %property(Width, GetWidth, SetWidth, doc="See `GetWidth` and `SetWidth`");
};


//---------------------------------------------------------------------------
%newgroup


// wxListEvent - the event class for the wxListCtrl notifications
class wxListEvent: public wxNotifyEvent {
public:
    wxListEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

    int           m_code;
    long          m_oldItemIndex;
    long          m_itemIndex;
    int           m_col;
    wxPoint       m_pointDrag;
    %immutable;
    wxListItem    m_item;
    %mutable;

    int GetKeyCode();
    %pythoncode { GetCode = GetKeyCode }
    long GetIndex();
    int GetColumn();
    wxPoint GetPoint();
    %pythoncode { GetPosition = GetPoint }
    const wxString& GetLabel();
    const wxString& GetText();
    int GetImage();
    long GetData();
    long GetMask();
    const wxListItem& GetItem();

    long GetCacheFrom();
    long GetCacheTo();

    // was label editing canceled? (for wxEVT_COMMAND_LIST_END_LABEL_EDIT only)
    bool IsEditCancelled() const;
    void SetEditCanceled(bool editCancelled);

    %property(CacheFrom, GetCacheFrom, doc="See `GetCacheFrom`");
    %property(CacheTo, GetCacheTo, doc="See `GetCacheTo`");
    %property(Column, GetColumn, doc="See `GetColumn`");
    %property(Data, GetData, doc="See `GetData`");
    %property(Image, GetImage, doc="See `GetImage`");
    %property(Index, GetIndex, doc="See `GetIndex`");
    %property(Item, GetItem, doc="See `GetItem`");
    %property(KeyCode, GetKeyCode, doc="See `GetKeyCode`");
    %property(Label, GetLabel, doc="See `GetLabel`");
    %property(Mask, GetMask, doc="See `GetMask`");
    %property(Point, GetPoint, doc="See `GetPoint`");
    %property(Text, GetText, doc="See `GetText`");
    
};

/* List control event types */
%constant wxEventType wxEVT_COMMAND_LIST_BEGIN_DRAG;
%constant wxEventType wxEVT_COMMAND_LIST_BEGIN_RDRAG;
%constant wxEventType wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT;
%constant wxEventType wxEVT_COMMAND_LIST_END_LABEL_EDIT;
%constant wxEventType wxEVT_COMMAND_LIST_DELETE_ITEM;
%constant wxEventType wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS;
%constant wxEventType wxEVT_COMMAND_LIST_ITEM_SELECTED;
%constant wxEventType wxEVT_COMMAND_LIST_ITEM_DESELECTED;
%constant wxEventType wxEVT_COMMAND_LIST_KEY_DOWN;
%constant wxEventType wxEVT_COMMAND_LIST_INSERT_ITEM;
%constant wxEventType wxEVT_COMMAND_LIST_COL_CLICK;
%constant wxEventType wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK;
%constant wxEventType wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK;
%constant wxEventType wxEVT_COMMAND_LIST_ITEM_ACTIVATED;
%constant wxEventType wxEVT_COMMAND_LIST_CACHE_HINT;
%constant wxEventType wxEVT_COMMAND_LIST_COL_RIGHT_CLICK;
%constant wxEventType wxEVT_COMMAND_LIST_COL_BEGIN_DRAG;
%constant wxEventType wxEVT_COMMAND_LIST_COL_DRAGGING;
%constant wxEventType wxEVT_COMMAND_LIST_COL_END_DRAG;
%constant wxEventType wxEVT_COMMAND_LIST_ITEM_FOCUSED;

// WXWIN_COMPATIBILITY_2_4
#if 0
%constant wxEventType wxEVT_COMMAND_LIST_GET_INFO;
%constant wxEventType wxEVT_COMMAND_LIST_SET_INFO;
#endif

%pythoncode {

EVT_LIST_BEGIN_DRAG        = wx.PyEventBinder(wxEVT_COMMAND_LIST_BEGIN_DRAG       , 1)
EVT_LIST_BEGIN_RDRAG       = wx.PyEventBinder(wxEVT_COMMAND_LIST_BEGIN_RDRAG      , 1)
EVT_LIST_BEGIN_LABEL_EDIT  = wx.PyEventBinder(wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT , 1)
EVT_LIST_END_LABEL_EDIT    = wx.PyEventBinder(wxEVT_COMMAND_LIST_END_LABEL_EDIT   , 1)
EVT_LIST_DELETE_ITEM       = wx.PyEventBinder(wxEVT_COMMAND_LIST_DELETE_ITEM      , 1)
EVT_LIST_DELETE_ALL_ITEMS  = wx.PyEventBinder(wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS , 1)
#WXWIN_COMPATIBILITY_2_4
#EVT_LIST_GET_INFO          = wx.PyEventBinder(wxEVT_COMMAND_LIST_GET_INFO         , 1)
#EVT_LIST_SET_INFO          = wx.PyEventBinder(wxEVT_COMMAND_LIST_SET_INFO         , 1)
#END WXWIN_COMPATIBILITY_2_4
EVT_LIST_ITEM_SELECTED     = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_SELECTED    , 1)
EVT_LIST_ITEM_DESELECTED   = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_DESELECTED  , 1)
EVT_LIST_KEY_DOWN          = wx.PyEventBinder(wxEVT_COMMAND_LIST_KEY_DOWN         , 1)
EVT_LIST_INSERT_ITEM       = wx.PyEventBinder(wxEVT_COMMAND_LIST_INSERT_ITEM      , 1)
EVT_LIST_COL_CLICK         = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_CLICK        , 1)
EVT_LIST_ITEM_RIGHT_CLICK  = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK , 1)
EVT_LIST_ITEM_MIDDLE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK, 1)
EVT_LIST_ITEM_ACTIVATED    = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_ACTIVATED   , 1)
EVT_LIST_CACHE_HINT        = wx.PyEventBinder(wxEVT_COMMAND_LIST_CACHE_HINT       , 1)
EVT_LIST_COL_RIGHT_CLICK   = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK  , 1)
EVT_LIST_COL_BEGIN_DRAG    = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG   , 1)
EVT_LIST_COL_DRAGGING      = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_DRAGGING     , 1)
EVT_LIST_COL_END_DRAG      = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_END_DRAG     , 1)
EVT_LIST_ITEM_FOCUSED      = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_FOCUSED     , 1)

#WXWIN_COMPATIBILITY_2_4
#EVT_LIST_GET_INFO = wx._deprecated(EVT_LIST_GET_INFO)
#EVT_LIST_SET_INFO = wx._deprecated(EVT_LIST_SET_INFO)
}

//---------------------------------------------------------------------------
%newgroup


%{ // Python aware sorting function for wxPyListCtrl
    static int wxCALLBACK wxPyListCtrl_SortItems(long item1, long item2, long funcPtr) {
        int retval = 0;
        PyObject* func = (PyObject*)funcPtr;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();

        PyObject* args = Py_BuildValue("(ii)", item1, item2);
        PyObject* result = PyEval_CallObject(func, args);
        Py_DECREF(args);
        if (result) {
            retval = PyInt_AsLong(result);
            Py_DECREF(result);
        }

        wxPyEndBlockThreads(blocked);
        return retval;
    }
%}



%{  // C++ Version of a Python aware class
class wxPyListCtrl : public wxListCtrl {
    DECLARE_ABSTRACT_CLASS(wxPyListCtrl)
public:
    wxPyListCtrl() : wxListCtrl() {}
    wxPyListCtrl(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxValidator& validator,
                 const wxString& name) :
        wxListCtrl(parent, id, pos, size, style, validator, name) {}

    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxValidator& validator,
                const wxString& name) {
        return wxListCtrl::Create(parent, id, pos, size, style, validator, name);
    }

    DEC_PYCALLBACK_STRING_LONGLONG(OnGetItemText);
    DEC_PYCALLBACK_LISTATTR_LONG(OnGetItemAttr);

    // use the virtual version to avoid a confusing assert in the base class
    DEC_PYCALLBACK_INT_LONG_virtual(OnGetItemImage);
    DEC_PYCALLBACK_INT_LONGLONG(OnGetItemColumnImage);

    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyListCtrl, wxListCtrl);

IMP_PYCALLBACK_STRING_LONGLONG(wxPyListCtrl, wxListCtrl, OnGetItemText);
IMP_PYCALLBACK_LISTATTR_LONG(wxPyListCtrl, wxListCtrl, OnGetItemAttr);
IMP_PYCALLBACK_INT_LONG_virtual(wxPyListCtrl, wxListCtrl, OnGetItemImage);
IMP_PYCALLBACK_INT_LONGLONG(wxPyListCtrl, wxListCtrl, OnGetItemColumnImage); 

%}





MustHaveApp(wxPyListCtrl);

%rename(ListCtrl) wxPyListCtrl;
class wxPyListCtrl : public wxControl {
public:

    %pythonAppend wxPyListCtrl         "self._setOORInfo(self);self._setCallbackInfo(self, ListCtrl)"
    %pythonAppend wxPyListCtrl()       ""

    wxPyListCtrl(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxLC_ICON,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyListCtrlNameStr);
    %RenameCtor(PreListCtrl, wxPyListCtrl());

    bool Create(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxLC_ICON,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyListCtrlNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);


    // Set the control colours
    bool SetForegroundColour(const wxColour& col);
    bool SetBackgroundColour(const wxColour& col);

    // Gets information about this column
    %pythonAppend GetColumn "if val is not None: val.thisown = 1";  // %newobject doesn't work with OOR typemap
    %extend {
        wxListItem* GetColumn(int col) {
            wxListItem item;
            item.SetMask( wxLIST_MASK_STATE |
                          wxLIST_MASK_TEXT  |
                          wxLIST_MASK_IMAGE |
                          wxLIST_MASK_DATA  |
                          wxLIST_SET_ITEM   |
                          wxLIST_MASK_WIDTH |
                          wxLIST_MASK_FORMAT
                          );
            if (self->GetColumn(col, item))
                return new wxListItem(item);
            else
                return NULL;
        }
    }

    // Sets information about this column
    bool SetColumn(int col, wxListItem& item) ;

    // Gets the column width
    int GetColumnWidth(int col) const;

    // Sets the column width
    bool SetColumnWidth(int col, int width) ;

    // Gets the number of items that can fit vertically in the
    // visible area of the list control (list or report view)
    // or the total number of items in the list control (icon
    // or small icon view)
    int GetCountPerPage() const;

    // return the total area occupied by all the items (icon/small icon only)
    wxRect GetViewRect() const;

    // Gets the edit control for editing labels.
    wxTextCtrl* GetEditControl() const;

    // Gets information about the item
    %pythonAppend GetItem "if val is not None: val.thisown = 1";  // %newobject doesn't work with OOR typemap
    %extend {
        wxListItem* GetItem(long itemId, int col=0) {
            wxListItem* info = new wxListItem;
            info->m_itemId = itemId;
            info->m_col = col;
            info->m_mask = 0xFFFF;
            self->GetItem(*info);
            return info;
        }
    }

    // Sets information about the item
    bool SetItem(wxListItem& info) ;

    // Sets a string field at a particular column
    %Rename(SetStringItem, long, SetItem(long index, int col, const wxString& label, int imageId = -1));

    // Gets the item state
    int  GetItemState(long item, long stateMask) const ;

    // Sets the item state
    bool SetItemState(long item, long state, long stateMask) ;

    // Sets the item image
    bool SetItemImage(long item, int image, int selImage=-1) ;
    bool SetItemColumnImage( long item, long column, int image );

    // Gets the item text
    wxString GetItemText(long item) const ;

    // Sets the item text
    void SetItemText(long item, const wxString& str) ;

    // Gets the item data
    long GetItemData(long item) const ;

    // Sets the item data
    bool SetItemData(long item, long data) ;


    //bool GetItemRect(long item, wxRect& rect, int code = wxLIST_RECT_BOUNDS) const ;
    //bool GetItemPosition(long item, wxPoint& pos) const ;

    // Gets the item position
    %extend {
        wxPoint GetItemPosition(long item) {
            wxPoint pos;
            self->GetItemPosition(item, pos);
            return pos;
        }
        // Gets the item rectangle
        wxRect GetItemRect(long item, int code = wxLIST_RECT_BOUNDS) {
            wxRect rect;
            self->GetItemRect(item, rect, code);
            return rect;
        }

// MSW only so far...        
//         wxRect GetSubItemRect(long item, long subItem, int code = wxLIST_RECT_BOUNDS) {
//             wxRect rect;
//             self->GetSubItemRect(item, subItem, rect, code);
//             return rect;
//         }
    }


    // Sets the item position
    bool SetItemPosition(long item, const wxPoint& pos) ;

    // Gets the number of items in the list control
    int GetItemCount() const;

    // Gets the number of columns in the list control
    int GetColumnCount() const;

    // get the horizontal and vertical components of the item spacing
    wxSize GetItemSpacing() const;
    %pythoncode { GetItemSpacing = wx._deprecated(GetItemSpacing) }

#ifndef __WXMSW__
    void SetItemSpacing( int spacing, bool isSmall = false );
    %pythoncode { SetItemSpacing = wx._deprecated(SetItemSpacing) }
#endif

    // Gets the number of selected items in the list control
    int GetSelectedItemCount() const;

    // Gets the text colour of the listview
    wxColour GetTextColour() const;

    // Sets the text colour of the listview
    void SetTextColour(const wxColour& col);

    // Gets the index of the topmost visible item when in
    // list or report view
    long GetTopItem() const ;

    // Add or remove a single window style
    void SetSingleStyle(long style, bool add = true) ;

    // Set the whole window style
    void SetWindowStyleFlag(long style) ;

    // Searches for an item, starting from 'item'.
    // item can be -1 to find the first item that matches the
    // specified flags.
    // Returns the item or -1 if unsuccessful.
    long GetNextItem(long item, int geometry = wxLIST_NEXT_ALL, int state = wxLIST_STATE_DONTCARE) const ;

    // Gets one of the three image lists
    wxImageList *GetImageList(int which) const ;

    // Sets the image list
    void SetImageList(wxImageList *imageList, int which);

    %disownarg( wxImageList *imageList );
    void AssignImageList(wxImageList *imageList, int which);
    %cleardisown( wxImageList *imageList );

    // are we in report mode?
    bool InReportView() const;

    // returns True if it is a virtual list control
    bool IsVirtual() const;

    // refresh items selectively (only useful for virtual list controls)
    void RefreshItem(long item);
    void RefreshItems(long itemFrom, long itemTo);

    // Arranges the items
    bool Arrange(int flag = wxLIST_ALIGN_DEFAULT);

    // Deletes an item
    bool DeleteItem(long item);

    // Deletes all items
    bool DeleteAllItems() ;

    // Deletes a column
    bool DeleteColumn(int col);

    // Deletes all columns
    bool DeleteAllColumns();

    // Clears items, and columns if there are any.
    void ClearAll();

#ifdef __WXMSW__
    // Edit the label
    wxTextCtrl* EditLabel(long item /*, wxClassInfo* textControlClass = CLASSINFO(wxTextCtrl)*/);

    // End label editing, optionally cancelling the edit
    bool EndEditLabel(bool cancel);
#else
    void EditLabel(long item);
#endif

    // Ensures this item is visible
    bool EnsureVisible(long item) ;

    // Find an item whose label matches this string, starting from the item after 'start'
    // or the beginning if 'start' is -1.
    long FindItem(long start, const wxString& str, bool partial = false);

    // Find an item whose data matches this data, starting from the item after 'start'
    // or the beginning if 'start' is -1.
    %Rename(FindItemData,  long, FindItem(long start, long data));

    // Find an item nearest this position in the specified direction, starting from
    // the item after 'start' or the beginning if 'start' is -1.
    %Rename(FindItemAtPos,  long, FindItem(long start, const wxPoint& pt, int direction));


    DocDeclAStr(
        long, HitTest(const wxPoint& point, int& OUTPUT),
        "HitTest(Point point) -> (item, where)",
        "Determines which item (if any) is at the specified point, giving
details in the second return value (see wx.LIST_HITTEST flags.)", "");

    DocDeclAStrName(
        long, HitTest(const wxPoint& point, int& OUTPUT, long* OUTPUT),
        "HitTestSubItem(Point point) -> (item, where, subItem)",
        "Determines which item (if any) is at the specified point, giving details in
the second return value (see wx.LIST_HITTEST flags) and also the subItem, if
any.", "",
        HitTestSubItem);

    
    // Inserts an item, returning the index of the new item if successful,
    // -1 otherwise.
    long InsertItem(wxListItem& info);

    // Insert a string item
    %Rename(InsertStringItem,
            long, InsertItem(long index, const wxString& label, int imageIndex=-1));

    // Insert an image item
    %Rename(InsertImageItem,
            long, InsertItem(long index, int imageIndex));

    // Insert an image/string item
    %Rename(InsertImageStringItem,
            long, InsertItem(long index, const wxString& label, int imageIndex));

    // For list view mode (only), inserts a column.
    %Rename(InsertColumnItem,  long, InsertColumn(long col, wxListItem& info));
    %pythoncode { InsertColumnInfo = InsertColumnItem }

    long InsertColumn(long col,
                      const wxString& heading,
                      int format = wxLIST_FORMAT_LEFT,
                      int width = -1);

    // set the number of items in a virtual list control
    void SetItemCount(long count);

    // Scrolls the list control. If in icon, small icon or report view mode,
    // x specifies the number of pixels to scroll. If in list view mode, x
    // specifies the number of columns to scroll.
    // If in icon, small icon or list view mode, y specifies the number of pixels
    // to scroll. If in report view mode, y specifies the number of lines to scroll.
    bool ScrollList(int dx, int dy);

    void SetItemTextColour( long item, const wxColour& col);
    wxColour GetItemTextColour( long item ) const;
    void SetItemBackgroundColour( long item, const wxColour &col);
    wxColour GetItemBackgroundColour( long item ) const;

    // Font of an item.
    void SetItemFont( long item, const wxFont &f);
    wxFont GetItemFont( long item ) const;

    %pythoncode {
    %#
    %# Some helpers...
    def Select(self, idx, on=1):
        '''[de]select an item'''
        if on: state = wx.LIST_STATE_SELECTED
        else: state = 0
        self.SetItemState(idx, state, wx.LIST_STATE_SELECTED)

    def Focus(self, idx):
        '''Focus and show the given item'''
        self.SetItemState(idx, wx.LIST_STATE_FOCUSED, wx.LIST_STATE_FOCUSED)
        self.EnsureVisible(idx)

    def GetFocusedItem(self):
        '''get the currently focused item or -1 if none'''
        return self.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_FOCUSED)

    def GetFirstSelected(self, *args):
        '''return first selected item, or -1 when none'''
        return self.GetNextSelected(-1)

    def GetNextSelected(self, item):
        '''return subsequent selected items, or -1 when no more'''
        return self.GetNextItem(item, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)

    def IsSelected(self, idx):
        '''return True if the item is selected'''
        return (self.GetItemState(idx, wx.LIST_STATE_SELECTED) & wx.LIST_STATE_SELECTED) != 0

    def SetColumnImage(self, col, image):
        item = self.GetColumn(col)
        %# preserve all other attributes too
        item.SetMask( wx.LIST_MASK_STATE |
                      wx.LIST_MASK_TEXT  |
                      wx.LIST_MASK_IMAGE |
                      wx.LIST_MASK_DATA  |
                      wx.LIST_SET_ITEM   |
                      wx.LIST_MASK_WIDTH |
                      wx.LIST_MASK_FORMAT )
        item.SetImage(image)
        self.SetColumn(col, item)

    def ClearColumnImage(self, col):
        self.SetColumnImage(col, -1)

    def Append(self, entry):
        '''Append an item to the list control.  The entry parameter should be a
           sequence with an item for each column'''
        if len(entry):
            if wx.USE_UNICODE:
                cvtfunc = unicode
            else:
                cvtfunc = str
            pos = self.GetItemCount()
            self.InsertStringItem(pos, cvtfunc(entry[0]))
            for i in range(1, len(entry)):
                self.SetStringItem(pos, i, cvtfunc(entry[i]))
            return pos
    }


    // bool SortItems(wxListCtrlCompare fn, long data);
    %extend {
        // Sort items.
        // func is a function which takes 2 long arguments: item1, item2.
        // item1 is the long data associated with a first item (NOT the index).
        // item2 is the long data associated with a second item (NOT the index).
        // The return value is a negative number if the first item should precede the second
        // item, a positive number of the second item should precede the first,
        // or zero if the two items are equivalent.
        bool SortItems(PyObject* func) {
            if (!PyCallable_Check(func))
                return false;
            return self->SortItems((wxListCtrlCompare)wxPyListCtrl_SortItems, (long)func);
        }
    }


    %extend {
        wxWindow* GetMainWindow() {
        #if defined(__WXMSW__) || defined(__WXMAC__)
            return self;
        #else
            return (wxWindow*)self->m_mainWin;
        #endif
        }
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(ColumnCount, GetColumnCount, doc="See `GetColumnCount`");
    %property(CountPerPage, GetCountPerPage, doc="See `GetCountPerPage`");
    %property(EditControl, GetEditControl, doc="See `GetEditControl`");
    %property(FocusedItem, GetFocusedItem, doc="See `GetFocusedItem`");
    %property(ImageList, GetImageList, SetImageList, doc="See `GetImageList` and `SetImageList`");
    %property(ItemCount, GetItemCount, SetItemCount, doc="See `GetItemCount` and `SetItemCount`");
    %property(MainWindow, GetMainWindow, doc="See `GetMainWindow`");
    %property(SelectedItemCount, GetSelectedItemCount, doc="See `GetSelectedItemCount`");
    %property(TextColour, GetTextColour, SetTextColour, doc="See `GetTextColour` and `SetTextColour`");
    %property(TopItem, GetTopItem, doc="See `GetTopItem`");
    %property(ViewRect, GetViewRect, doc="See `GetViewRect`");
};



//---------------------------------------------------------------------------
%newgroup


MustHaveApp(wxListView);

// wxListView: a class which provides a little better API for list control
class wxListView : public wxPyListCtrl
{
public:
    %pythonAppend wxListView         "self._setOORInfo(self)"
    %pythonAppend wxListView()       ""

    wxListView( wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyListCtrlNameStr);
    %RenameCtor(PreListView, wxListView());

    bool Create( wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyListCtrlNameStr);

    // [de]select an item
    void Select(long n, bool on = true);

    // focus and show the given item
    void Focus(long index);

    // get the currently focused item or -1 if none
    long GetFocusedItem() const;

    // get first and subsequent selected items, return -1 when no more
    long GetNextSelected(long item) const;
    long GetFirstSelected() const;

    // return True if the item is selected
    bool IsSelected(long index);

    void SetColumnImage(int col, int image);
    void ClearColumnImage(int col);

    %property(FocusedItem, GetFocusedItem, doc="See `GetFocusedItem`");
};



//---------------------------------------------------------------------------

%init %{
    // Map renamed classes back to their common name for OOR
    wxPyPtrTypeMap_Add("wxListCtrl", "wxPyListCtrl");
%}

//---------------------------------------------------------------------------
