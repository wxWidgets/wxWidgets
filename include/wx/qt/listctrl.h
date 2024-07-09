/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/listctrl.h
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_LISTCTRL_H_
#define _WX_QT_LISTCTRL_H_

#include "wx/textctrl.h"

class wxQtListTreeWidget;
class wxQtListModel;
class wxQtVirtualListModel;

class WXDLLIMPEXP_CORE wxListCtrl: public wxListCtrlBase
{
public:
    wxListCtrl() = default;

    wxListCtrl(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxLC_ICON,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxListCtrlNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_ICON,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListCtrlNameStr));

    virtual ~wxListCtrl();

    // Attributes
    ////////////////////////////////////////////////////////////////////////////

    // Set the control colours
    bool SetForegroundColour(const wxColour& col) override;
    bool SetBackgroundColour(const wxColour& col) override;

    // Gets information about this column
    bool GetColumn(int col, wxListItem& info) const override;

    // Sets information about this column
    bool SetColumn(int col, const wxListItem& info) override;

    // Gets the column width
    int GetColumnWidth(int col) const override;

    // Sets the column width
    bool SetColumnWidth(int col, int width) override;


    // Gets the column order from its index or index from its order
    int GetColumnOrder(int col) const override;
    int GetColumnIndexFromOrder(int order) const override;

    // Gets the column order for all columns
    wxArrayInt GetColumnsOrder() const override;

    // Sets the column order for all columns
    bool SetColumnsOrder(const wxArrayInt& orders) override;


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
    bool GetItem(wxListItem& info) const;

    // Sets information about the item
    bool SetItem(wxListItem& info);

    // Sets a string field at a particular column
    long SetItem(long index, int col, const wxString& label, int imageId = -1);

    // Gets the item state
    int  GetItemState(long item, long stateMask) const;

    // Sets the item state
    bool SetItemState(long item, long state, long stateMask);

    // Sets the item image
    bool SetItemImage(long item, int image, int selImage = -1);
    bool SetItemColumnImage(long item, long column, int image);

    // Gets the item text
    wxString GetItemText(long item, int col = 0) const;

    // Sets the item text
    void SetItemText(long item, const wxString& str);

    // Gets the item data
    wxUIntPtr GetItemData(long item) const;

    // Sets the item data
    bool SetItemPtrData(long item, wxUIntPtr data);
    bool SetItemData(long item, long data);

    // Gets the item rectangle
    bool GetItemRect(long item, wxRect& rect, int code = wxLIST_RECT_BOUNDS) const;

    // Gets the subitem rectangle in report mode
    bool GetSubItemRect(long item, long subItem, wxRect& rect, int code = wxLIST_RECT_BOUNDS) const;

    // Gets the item position
    bool GetItemPosition(long item, wxPoint& pos) const;

    // Sets the item position
    bool SetItemPosition(long item, const wxPoint& pos);

    // Gets the number of items in the list control
    int GetItemCount() const override;

    // Gets the number of columns in the list control
    int GetColumnCount() const override;

    // get the horizontal and vertical components of the item spacing
    wxSize GetItemSpacing() const;

    // Foreground colour of an item.
    void SetItemTextColour( long item, const wxColour& col);
    wxColour GetItemTextColour( long item ) const;

    // Background colour of an item.
    void SetItemBackgroundColour( long item, const wxColour &col);
    wxColour GetItemBackgroundColour( long item ) const;

    // Font of an item.
    void SetItemFont( long item, const wxFont &f);
    wxFont GetItemFont( long item ) const;

    // Gets the number of selected items in the list control
    int GetSelectedItemCount() const;

    // Gets the text colour of the listview
    wxColour GetTextColour() const;

    // Sets the text colour of the listview
    void SetTextColour(const wxColour& col);

    // Gets the index of the topmost visible item when in
    // list or report view
    long GetTopItem() const;

    virtual bool HasCheckBoxes() const override;
    virtual bool EnableCheckBoxes(bool enable = true) override;
    virtual bool IsItemChecked(long item) const override;
    virtual void CheckItem(long item, bool check) override;

    // Add or remove a single window style
    void SetSingleStyle(long style, bool add = true);

    // Set the whole window style
    void SetWindowStyleFlag(long style) override;

    // Searches for an item, starting from 'item'.
    // item can be -1 to find the first item that matches the
    // specified flags.
    // Returns the item or -1 if unsuccessful.
    long GetNextItem(long item, int geometry = wxLIST_NEXT_ALL, int state = wxLIST_STATE_DONTCARE) const;

    // refresh items selectively (only useful for virtual list controls)
    void RefreshItem(long item);
    void RefreshItems(long itemFrom, long itemTo);

    // Operations
    ////////////////////////////////////////////////////////////////////////////

    // Arranges the items
    bool Arrange(int flag = wxLIST_ALIGN_DEFAULT);

    // Deletes an item
    bool DeleteItem(long item);

    // Deletes all items
    bool DeleteAllItems();

    // Deletes a column
    bool DeleteColumn(int col) override;

    // Deletes all columns
    bool DeleteAllColumns() override;

    // Clears items, and columns if there are any.
    void ClearAll();

    // Edit the label
    wxTextCtrl* EditLabel(long item, wxClassInfo* textControlClass = CLASSINFO(wxTextCtrl));

    // End label editing, optionally cancelling the edit
    bool EndEditLabel(bool cancel);

    // Ensures this item is visible
    bool EnsureVisible(long item);

    bool IsVisible(long item) const override;

    // Find an item whose label matches this string, starting from the item after 'start'
    // or the beginning if 'start' is -1.
    long FindItem(long start, const wxString& str, bool partial = false);

    // Find an item whose data matches this data, starting from the item after 'start'
    // or the beginning if 'start' is -1.
    long FindItem(long start, wxUIntPtr data);

    // Find an item nearest this position in the specified direction, starting from
    // the item after 'start' or the beginning if 'start' is -1.
    long FindItem(long start, const wxPoint& pt, int direction);

    // Determines which item (if any) is at the specified point,
    // giving details in 'flags' (see wxLIST_HITTEST_... flags above)
    // Request the subitem number as well at the given coordinate.
    long HitTest(const wxPoint& point, int& flags, long* ptrSubItem = nullptr) const;

    // Inserts an item, returning the index of the new item if successful,
    // -1 otherwise.
    long InsertItem(const wxListItem& info);

    // Insert a string item
    long InsertItem(long index, const wxString& label);

    // Insert an image item
    long InsertItem(long index, int imageIndex);

    // Insert an image/string item
    long InsertItem(long index, const wxString& label, int imageIndex);

    // set the number of items in a virtual list control
    void SetItemCount(long count);

    // Scrolls the list control. If in icon, small icon or report view mode,
    // x specifies the number of pixels to scroll. If in list view mode, x
    // specifies the number of columns to scroll.
    // If in icon, small icon or list view mode, y specifies the number of pixels
    // to scroll. If in report view mode, y specifies the number of lines to scroll.
    bool ScrollList(int dx, int dy);

    // Sort items.

    // fn is a function which takes 3 long arguments: item1, item2, data.
    // item1 is the long data associated with a first item (NOT the index).
    // item2 is the long data associated with a second item (NOT the index).
    // data is the same value as passed to SortItems.
    // The return value is a negative number if the first item should precede the second
    // item, a positive number of the second item should precede the first,
    // or zero if the two items are equivalent.

    // data is arbitrary data to be passed to the sort function.
    bool SortItems(wxListCtrlCompare fn, wxIntPtr data);

    // Sort indicator in header.
    virtual void ShowSortIndicator(int col, bool ascending = true) override;
    virtual int GetSortIndicator() const override;
    virtual bool IsAscendingSortIndicator() const override;

    wxQtListTreeWidget* GetQListTreeWidget() const;

protected:
    // Implement base class pure virtual methods.
    virtual long DoInsertColumn(long col, const wxListItem& info) override;
    void DoUpdateImages(int which) override;

    bool m_hasCheckBoxes = false;

private:
    // Allow access to OnGetItemXXX() method from the virtual model class.
    friend class wxQtVirtualListModel;

    wxQtListModel *m_model = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxListCtrl);
};

#endif

