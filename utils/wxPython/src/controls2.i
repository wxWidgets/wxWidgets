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
#include <wx/tabctrl.h>
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

//----------------------------------------------------------------------

%{
extern wxValidator wxPyDefaultValidator;
%}

//----------------------------------------------------------------------

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

    bool Arrange(int flag = wxLIST_ALIGN_DEFAULT);
    bool DeleteItem(long item);
    bool DeleteAllItems();
    bool DeleteColumn(int col);
    bool DeleteAllColumns(void);
    void ClearAll(void);
    wxTextCtrl* EditLabel(long item);
    bool EndEditLabel(bool cancel);
    bool EnsureVisible(long item);
    long FindItem(long start, const wxString& str, bool partial = FALSE);
    %name(FindItemData)long FindItem(long start, long data);
    %name(FindItemAtPos)long FindItem(long start, const wxPoint& pt,
                                      int direction);
    bool GetColumn(int col, wxListItem& item);
    int GetColumnWidth(int col);
    int GetCountPerPage();
    wxTextCtrl* GetEditControl();
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
    wxColour GetTextColour();
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
    %name(SetItemString)long SetItem(long index, int col, const wxString& label,
                                     int imageId = -1);
    bool SetItemData(long item, long data);
    bool SetItemImage(long item, int image, int selImage);
    bool SetItemPosition(long item, const wxPoint& pos);
    bool SetItemState(long item, long state, long stateMask);
    void SetItemText(long item, const wxString& text);
    void SetSingleStyle(long style, bool add = TRUE);
    void SetTextColour(const wxColour& col);
    void SetWindowStyleFlag(long style);
    // TODO:  bool SortItems(wxListCtrlCompare fn, long data);
};



//----------------------------------------------------------------------


enum {
    wxTREE_MASK_HANDLE,
    wxTREE_MASK_STATE,
    wxTREE_MASK_TEXT,
    wxTREE_MASK_IMAGE,
    wxTREE_MASK_SELECTED_IMAGE,
    wxTREE_MASK_CHILDREN,
    wxTREE_MASK_DATA,

    wxTREE_STATE_BOLD,
    wxTREE_STATE_DROPHILITED,
    wxTREE_STATE_EXPANDED,
    wxTREE_STATE_EXPANDEDONCE,
    wxTREE_STATE_FOCUSED,
    wxTREE_STATE_SELECTED,
    wxTREE_STATE_CUT,

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
    wxTREE_HITTEST_ONITEM,
};


enum {
    wxTREE_NEXT_CARET,
    wxTREE_NEXT_CHILD,
    wxTREE_NEXT_DROPHILITE,
    wxTREE_NEXT_FIRSTVISIBLE,
    wxTREE_NEXT_NEXT,
    wxTREE_NEXT_NEXTVISIBLE,
    wxTREE_NEXT_PARENT,
    wxTREE_NEXT_PREVIOUS,
    wxTREE_NEXT_PREVIOUSVISIBLE,
    wxTREE_NEXT_ROOT
};

enum {
    wxTREE_EXPAND_EXPAND,
    wxTREE_EXPAND_COLLAPSE,
    wxTREE_EXPAND_COLLAPSE_RESET,
    wxTREE_EXPAND_TOGGLE
};

enum {
    wxTREE_INSERT_LAST,
    wxTREE_INSERT_FIRST,
    wxTREE_INSERT_SORT,
};







class wxTreeItem {
public:
    long            m_mask;
    long            m_itemId;
    long            m_state;
    long            m_stateMask;
    wxString        m_text;
    int             m_image;
    int             m_selectedImage;
    int             m_children;
    long            m_data;

    wxTreeItem();
    ~wxTreeItem();
};



class  wxTreeEvent : public wxCommandEvent {
public:
    int           m_code;
    wxTreeItem    m_item;
    long          m_oldItem;
    wxPoint       m_pointDrag;
};




class wxTreeCtrl : public wxControl {
public:
    wxTreeCtrl(wxWindow *parent, wxWindowID id = -1,
            const wxPoint& pos = wxPyDefaultPosition,
            const wxSize& size = wxPyDefaultSize,
            long style = wxTR_HAS_BUTTONS,
            const wxValidator& validator = wxPyDefaultValidator,
            char* name = "wxTreeCtrl");

    bool DeleteAllItems();
    bool DeleteItem(long item);
    wxTextCtrl* EditLabel(long item);
    bool EnsureVisible(long item);
    bool ExpandItem(long item, int action);
    long GetChild(long item);
    int GetCount();
    wxTextCtrl* GetEditControl();
    long GetFirstVisibleItem();
    wxImageList* GetImageList(int which = wxIMAGE_LIST_NORMAL);
    int GetIndent();
    long GetItemData(long item);

    %addmethods {
        %new wxTreeItem* GetItem() {
            wxTreeItem* info = new wxTreeItem;
            self->GetItem(*info);
            return info;
        }
        %new wxRect* GetItemRect(long item, int textOnly = FALSE) {
            wxRect* rect = new wxRect;
            self->GetItemRect(item, *rect, textOnly);
            return rect;
        }
    }

    int GetItemState(long item, long stateMask);
    wxString GetItemText(long item);
    long GetNextItem(long item, int code);
    long GetNextVisibleItem(long item);
    long GetParent(long item);
    long GetRootItem();
    long GetSelection();
    long HitTest(const wxPoint& point, int& OUTPUT); // *** check this
    long InsertItem(long parent, wxTreeItem& info,
                    long insertAfter = wxTREE_INSERT_LAST);
    %name(InsertItemString)
        long InsertItem(long parent, const wxString& label,
                        int image = -1, int selImage = -1,
                        long insertAfter = wxTREE_INSERT_LAST);
    bool ItemHasChildren(long item);
    bool ScrollTo(long item);
    bool SelectItem(long item);
    void SetIndent(int indent);
    void SetImageList(wxImageList* imageList, int which = wxIMAGE_LIST_NORMAL);
    bool SetItem(wxTreeItem& info);
    bool SetItemImage(long item, int image, int selImage);
    bool SetItemState(long item, long state, long stateMask);
    void SetItemText(long item, const wxString& text);
    bool SetItemData(long item, long data);
    bool SortChildren(long item);
};

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
//
// $Log$
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
