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
    wxLIST_FORMAT_LEFT,
    wxLIST_FORMAT_RIGHT,
    wxLIST_FORMAT_CENTRE,
    wxLIST_FORMAT_CENTER,
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


class wxListItem {
public:
    long            m_mask;     // Indicates what fields are valid
    long            m_itemId;   // The zero-based item position
    int             m_col;      // Zero-based column, if in report mode
    long            m_state;    // The state of the item
    long            m_stateMask; // Which flags of m_state are valid (uses same flags)
    wxString        m_text;     // The label/header text
    int             m_image;    // The zero-based index into an image list
    long            m_data;     // App-defined data
//    wxColour       *m_colour;   // only wxGLC, not supported by Windows ;->

    // For columns only
    int             m_format;   // left, right, centre
    int             m_width;    // width of column

    wxListItem();
    ~wxListItem();
};

class wxListEvent: public wxCommandEvent {
public:
    int           m_code;
    long          m_itemIndex;
    long          m_oldItemIndex;
    int           m_col;
    bool          m_cancelled;
    wxPoint       m_pointDrag;
    wxListItem    m_item;
};




class wxListCtrl : public wxControl {
public:
    wxListCtrl(wxWindow* parent, wxWindowID id,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = wxLC_ICON,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "listCtrl");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

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
        %new wxListItem* GetItem() {
            wxListItem* info = new wxListItem;
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
    %name(InsertColumnWithInfo)long InsertColumn(long col, wxListItem& info);
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
    // TODO:  bool SortItems(wxListCtrlCompare fn, long data);
};



//----------------------------------------------------------------------


class wxTreeItemId {
public:
    wxTreeItemId();
    ~wxTreeItemId();
    bool IsOk() const { return m_itemId != 0; }

//    %addmethods {
//        long GetId() { return (long)(*self); }
//    }
};



// ****  This isn't very useful yet.  This needs to be specialized to enable
// derived Python classes...
class wxTreeItemData {
public:
    wxTreeItemData();
    ~wxTreeItemData();

    const wxTreeItemId& GetId();
    void SetId(const wxTreeItemId& id);
};




class wxTreeEvent : public wxCommandEvent {
public:
    wxTreeItemId GetItem();
    wxTreeItemId GetOldItem();
    wxPoint GetPoint();
    int GetCode();
    void Veto();
};


// These are for the GetFirstChild/GetNextChild methods below
%typemap(python, in)     long& INOUT = long* INOUT;
%typemap(python, argout) long& INOUT = long* INOUT;


class wxTreeCtrl : public wxControl {
public:
    wxTreeCtrl(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "wxTreeCtrl");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

    size_t GetCount();
    unsigned int GetIndent();
    void SetIndent(unsigned int indent);
    wxImageList *GetImageList();
    wxImageList *GetStateImageList();
    void SetImageList(wxImageList *imageList);
    void SetStateImageList(wxImageList *imageList);

    wxString GetItemText(const wxTreeItemId& item);
    int GetItemImage(const wxTreeItemId& item);
    int GetItemSelectedImage(const wxTreeItemId& item);
    wxTreeItemData *GetItemData(const wxTreeItemId& item);

    void SetItemText(const wxTreeItemId& item, const wxString& text);
    void SetItemImage(const wxTreeItemId& item, int image);
    void SetItemSelectedImage(const wxTreeItemId& item, int image);
    void SetItemData(const wxTreeItemId& item, wxTreeItemData *data);
    void SetItemHasChildren(const wxTreeItemId& item, bool hasChildren = TRUE);

    bool IsVisible(const wxTreeItemId& item);
    bool ItemHasChildren(const wxTreeItemId& item);
    bool IsExpanded(const wxTreeItemId& item);
    bool IsSelected(const wxTreeItemId& item);

    wxTreeItemId GetRootItem();
    wxTreeItemId GetSelection();
    wxTreeItemId GetParent(const wxTreeItemId& item);

    wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& INOUT);
    wxTreeItemId GetNextChild(const wxTreeItemId& item, long& INOUT);
    wxTreeItemId GetNextSibling(const wxTreeItemId& item);
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item);
    wxTreeItemId GetFirstVisibleItem();
    wxTreeItemId GetNextVisible(const wxTreeItemId& item);
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item);


    wxTreeItemId AddRoot(const wxString& text,
                         int image = -1, int selectedImage = -1,
                         wxTreeItemData *data = NULL);
    wxTreeItemId PrependItem(const wxTreeItemId& parent,
                             const wxString& text,
                             int image = -1, int selectedImage = -1,
                             wxTreeItemData *data = NULL);
    wxTreeItemId InsertItem(const wxTreeItemId& parent,
                            const wxTreeItemId& idPrevious,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxTreeItemData *data = NULL);
    wxTreeItemId AppendItem(const wxTreeItemId& parent,
                            const wxString& text,
                            int image = -1, int selectedImage = -1,
                            wxTreeItemData *data = NULL);

    void Delete(const wxTreeItemId& item);
    void DeleteChildren(const wxTreeItemId& item);
    void DeleteAllItems();

    void Expand(const wxTreeItemId& item);
    void Collapse(const wxTreeItemId& item);
    void CollapseAndReset(const wxTreeItemId& item);
    void Toggle(const wxTreeItemId& item);

    void Unselect();
    void SelectItem(const wxTreeItemId& item);
    void EnsureVisible(const wxTreeItemId& item);
    void ScrollTo(const wxTreeItemId& item);

    wxTextCtrl* EditLabel(const wxTreeItemId& item);
                          // **** figure out how to do this
                          // wxClassInfo* textCtrlClass = CLASSINFO(wxTextCtrl));
    wxTextCtrl* GetEditControl();
    void EndEditLabel(const wxTreeItemId& item, bool discardChanges = FALSE);

//    void SortChildren(const wxTreeItemId& item);
                      // **** And this too
                      // wxTreeItemCmpFunc *cmpFunction = NULL);

    void SetItemBold(const wxTreeItemId& item, bool bold = TRUE);
    bool IsBold(const wxTreeItemId& item) const;
    wxTreeItemId HitTest(const wxPoint& point);
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

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

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


/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.15  1999/02/20 09:02:56  RD
// Added wxWindow_FromHWND(hWnd) for wxMSW to construct a wxWindow from a
// window handle.  If you can get the window handle into the python code,
// it should just work...  More news on this later.
//
// Added wxImageList, wxToolTip.
//
// Re-enabled wxConfig.DeleteAll() since it is reportedly fixed for the
// wxRegConfig class.
//
// As usual, some bug fixes, tweaks, etc.
//
// Revision 1.14  1999/01/30 07:30:10  RD
//
// Added wxSashWindow, wxSashEvent, wxLayoutAlgorithm, etc.
//
// Various cleanup, tweaks, minor additions, etc. to maintain
// compatibility with the current wxWindows.
//
// Revision 1.13  1998/12/17 14:07:34  RR
//
//   Removed minor differences between wxMSW and wxGTK
//
// Revision 1.12  1998/12/16 22:10:52  RD
//
// Tweaks needed to be able to build wxPython with wxGTK.
//
// Revision 1.11  1998/12/15 20:41:16  RD
// Changed the import semantics from "from wxPython import *" to "from
// wxPython.wx import *"  This is for people who are worried about
// namespace pollution, they can use "from wxPython import wx" and then
// prefix all the wxPython identifiers with "wx."
//
// Added wxTaskbarIcon for wxMSW.
//
// Made the events work for wxGrid.
//
// Added wxConfig.
//
// Added wxMiniFrame for wxGTK, (untested.)
//
// Changed many of the args and return values that were pointers to gdi
// objects to references to reflect changes in the wxWindows API.
//
// Other assorted fixes and additions.
//
// Revision 1.10  1998/11/25 08:45:23  RD
//
// Added wxPalette, wxRegion, wxRegionIterator, wxTaskbarIcon
// Added events for wxGrid
// Other various fixes and additions
//
// Revision 1.9  1998/11/16 00:00:54  RD
// Generic treectrl for wxPython/GTK compiles...
//
// Revision 1.8  1998/11/11 04:40:20  RD
// wxTreeCtrl now works (sort of) for wxPython-GTK.  This is the new
// TreeCtrl in src/gtk/treectrl.cpp not the old generic one.
//
// Revision 1.7  1998/11/11 03:12:25  RD
//
// Additions for wxTreeCtrl
//
// Revision 1.6  1998/10/20 06:43:55  RD
// New wxTreeCtrl wrappers (untested)
// some changes in helpers
// etc.
//
// Revision 1.5  1998/10/07 07:34:33  RD
// Version 0.4.1 for wxGTK
//
// Revision 1.4  1998/10/02 06:40:36  RD
//
// Version 0.4 of wxPython for MSW.
//
// Revision 1.3  1998/08/18 19:48:15  RD
// more wxGTK compatibility things.
//
// It builds now but there are serious runtime problems...
//
// Revision 1.2  1998/08/15 07:36:30  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.1  1998/08/09 08:25:49  RD
// Initial version
//
//
