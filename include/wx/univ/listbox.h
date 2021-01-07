///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/listbox.h
// Purpose:     the universal listbox
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.08.00
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_LISTBOX_H_
#define _WX_UNIV_LISTBOX_H_

#include "wx/scrolwin.h"    // for wxScrollHelper
#include "wx/dynarray.h"
#include "wx/arrstr.h"

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

// change the current item
#define wxACTION_LISTBOX_SETFOCUS   wxT("setfocus")  // select the item
#define wxACTION_LISTBOX_MOVEDOWN   wxT("down")      // select item below
#define wxACTION_LISTBOX_MOVEUP     wxT("up")        // select item above
#define wxACTION_LISTBOX_PAGEDOWN   wxT("pagedown")  // go page down
#define wxACTION_LISTBOX_PAGEUP     wxT("pageup")    // go page up
#define wxACTION_LISTBOX_START      wxT("start")     // go to first item
#define wxACTION_LISTBOX_END        wxT("end")       // go to last item
#define wxACTION_LISTBOX_FIND       wxT("find")      // find item by 1st letter

// do something with the current item
#define wxACTION_LISTBOX_ACTIVATE   wxT("activate")  // activate (choose)
#define wxACTION_LISTBOX_TOGGLE     wxT("toggle")    // togglee selected state
#define wxACTION_LISTBOX_SELECT     wxT("select")    // sel this, unsel others
#define wxACTION_LISTBOX_SELECTADD  wxT("selectadd") // add to selection
#define wxACTION_LISTBOX_UNSELECT   wxT("unselect")  // unselect
#define wxACTION_LISTBOX_ANCHOR     wxT("selanchor") // anchor selection

// do something with the selection globally (not for single selection ones)
#define wxACTION_LISTBOX_SELECTALL   wxT("selectall")   // select all items
#define wxACTION_LISTBOX_UNSELECTALL wxT("unselectall") // unselect all items
#define wxACTION_LISTBOX_SELTOGGLE   wxT("togglesel")   // invert the selection
#define wxACTION_LISTBOX_EXTENDSEL   wxT("extend")      // extend to item

// ----------------------------------------------------------------------------
// wxListBox: a list of selectable items
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxListBox : public wxListBoxBase, public wxScrollHelper
{
public:
    // ctors and such
    wxListBox() : wxScrollHelper(this) { Init(); }
    wxListBox(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int n = 0, const wxString choices[] = (const wxString *) NULL,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxListBoxNameStr) )
        : wxScrollHelper(this)
    {
        Init();

        Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    wxListBox(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              const wxArrayString& choices,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxListBoxNameStr) );

    virtual ~wxListBox();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = (const wxString *) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));

    // implement the listbox interface defined by wxListBoxBase
    virtual void DoClear() wxOVERRIDE;
    virtual void DoDeleteOneItem(unsigned int n) wxOVERRIDE;

    virtual unsigned int GetCount() const wxOVERRIDE;
    virtual wxString GetString(unsigned int n) const wxOVERRIDE;
    virtual void SetString(unsigned int n, const wxString& s) wxOVERRIDE;
    virtual int FindString(const wxString& s, bool bCase = false) const wxOVERRIDE;

    virtual bool IsSelected(int n) const wxOVERRIDE
        { return m_selections.Index(n) != wxNOT_FOUND; }
    virtual int GetSelection() const wxOVERRIDE;
    virtual int GetSelections(wxArrayInt& aSelections) const wxOVERRIDE;

protected:
    virtual void DoSetSelection(int n, bool select) wxOVERRIDE;

    virtual int DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type) wxOVERRIDE;

    virtual int DoListHitTest(const wxPoint& point) const wxOVERRIDE;

    // universal wxComboBox implementation internally uses wxListBox
    friend class WXDLLIMPEXP_FWD_CORE wxComboBox;

    virtual void DoSetFirstItem(int n) wxOVERRIDE;

    virtual void DoSetItemClientData(unsigned int n, void* clientData) wxOVERRIDE;
    virtual void* DoGetItemClientData(unsigned int n) const wxOVERRIDE;

public:
    // override some more base class methods
    virtual bool SetFont(const wxFont& font) wxOVERRIDE;

    // the wxUniversal-specific methods
    // --------------------------------

    // the current item is the same as the selected one for wxLB_SINGLE
    // listboxes but for the other ones it is just the focused item which may
    // be selected or not
    int GetCurrentItem() const { return m_current; }
    void SetCurrentItem(int n);

    // select the item which is diff items below the current one
    void ChangeCurrent(int diff);

    // activate (i.e. send a LISTBOX_DOUBLECLICKED message) the specified or
    // current (if -1) item
    void Activate(int item = -1);

    // select or unselect the specified or current (if -1) item
    void DoSelect(int item = -1, bool sel = true);

    // more readable wrapper
    void DoUnselect(int item) { DoSelect(item, false); }

    // select an item and send a notification about it
    void SelectAndNotify(int item);

    // ensure that the given item is visible by scrolling it into view
    virtual void EnsureVisible(int n) wxOVERRIDE;

    // find the first item [strictly] after the current one which starts with
    // the given string and make it the current one, return true if the current
    // item changed
    bool FindItem(const wxString& prefix, bool strictlyAfter = false);
    bool FindNextItem(const wxString& prefix) { return FindItem(prefix, true); }

    // extend the selection to span the range from the anchor (see below) to
    // the specified or current item
    void ExtendSelection(int itemTo = -1);

    // make this item the new selection anchor: extending selection with
    // ExtendSelection() will work with it
    void AnchorSelection(int itemFrom) { m_selAnchor = itemFrom; }

    // get, calculating it if necessary, the number of items per page, the
    // height of each line and the max width of an item
    int GetItemsPerPage() const;
    wxCoord GetLineHeight() const;
    wxCoord GetMaxWidth() const;

    // override the wxControl virtual methods
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0l,
                               const wxString& strArg = wxEmptyString) wxOVERRIDE;

    static wxInputHandler *GetStdInputHandler(wxInputHandler *handlerDef);
    virtual wxInputHandler *DoGetStdInputHandler(wxInputHandler *handlerDef) wxOVERRIDE
    {
        return GetStdInputHandler(handlerDef);
    }

    // idle processing
    virtual void OnInternalIdle() wxOVERRIDE;

protected:
    // geometry
    virtual wxSize DoGetBestClientSize() const wxOVERRIDE;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;

    virtual void DoDraw(wxControlRenderer *renderer) wxOVERRIDE;
    virtual wxBorder GetDefaultBorder() const wxOVERRIDE;

    // special hook for wxCheckListBox which allows it to update its internal
    // data when a new item is inserted into the listbox
    virtual void OnItemInserted(unsigned int WXUNUSED(pos)) { }


    // common part of all ctors
    void Init();

    // event handlers
    void OnSize(wxSizeEvent& event);

    // refresh the given item(s) or everything
    void RefreshItems(int from, int count);
    void RefreshItem(int n);
    void RefreshFromItemToEnd(int n);
    void RefreshAll();

    // send an event of the given type (using m_current by default)
    bool SendEvent(wxEventType type, int item = -1);

    // calculate the number of items per page using our current size
    void CalcItemsPerPage();

    // can/should we have a horz scrollbar?
    bool HasHorzScrollbar() const
        { return (m_windowStyle & wxLB_HSCROLL) != 0; }

    // redraw the items in the given range only: called from DoDraw()
    virtual void DoDrawRange(wxControlRenderer *renderer,
                             int itemFirst, int itemLast);

    // update the scrollbars and then ensure that the item is visible
    void DoEnsureVisible(int n);

    // mark horz scrollbar for updating
    void RefreshHorzScrollbar();

    // update (show/hide/adjust) the scrollbars
    void UpdateScrollbars();

    // refresh the items specified by m_updateCount and m_updateFrom
    void UpdateItems();

    // the array containing all items (it is sorted if the listbox has
    // wxLB_SORT style)
    union
    {
        wxArrayString *unsorted;
        wxSortedArrayString *sorted;
    } m_strings;

    // this array contains the indices of the selected items (for the single
    // selection listboxes only the first element of it is used and contains
    // the current selection)
    wxArrayInt m_selections;

    // and this one the client data (either void or wxClientData)
    wxArrayPtrVoid m_itemsClientData;

    // this is hold the input handler type. the input handler is different
    // between ListBox and its subclass--CheckListbox
    wxString m_inputHandlerType;

    // the current item
    int m_current;

private:
    // the range of elements which must be updated: if m_updateCount is 0 no
    // update is needed, if it is -1 everything must be updated, otherwise
    // m_updateCount items starting from m_updateFrom have to be redrawn
    int m_updateFrom,
        m_updateCount;

    // the height of one line in the listbox (all lines have the same height)
    wxCoord m_lineHeight;

    // the maximal width of a listbox item and the item which has it
    wxCoord m_maxWidth;
    int m_maxWidthItem;

    // the extents of horz and vert scrollbars
    int m_scrollRangeX,
        m_scrollRangeY;

    // the number of items per page
    size_t m_itemsPerPage;

    // if the number of items has changed we may need to show/hide the
    // scrollbar
    bool m_updateScrollbarX, m_updateScrollbarY,
         m_showScrollbarX, m_showScrollbarY;

    // if the current item has changed, we might need to scroll if it went out
    // of the window
    bool m_currentChanged;

    // the anchor from which the selection is extended for the listboxes with
    // wxLB_EXTENDED style - this is set to the last item which was selected
    // by not extending the selection but by choosing it directly
    int m_selAnchor;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxListBox);
};

#endif // _WX_UNIV_LISTBOX_H_
