///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/listbox.h
// Purpose:     the universal listbox
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_LISTBOX_H_
#define _WX_UNIV_LISTBOX_H_

#ifdef __GNUG__
    #pragma implementation "univlistbox.h"
#endif

#include "wx/scrolwin.h"    // for wxScrollHelper

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

// change the current item
#define wxACTION_LISTBOX_SETFOCUS   _T("setfocus")  // select the item
#define wxACTION_LISTBOX_MOVEDOWN   _T("down")      // select item below
#define wxACTION_LISTBOX_MOVEUP     _T("up")        // select item above
#define wxACTION_LISTBOX_PAGEDOWN   _T("pagedown")  // go page down
#define wxACTION_LISTBOX_PAGEUP     _T("pageup")    // go page up
#define wxACTION_LISTBOX_START      _T("start")     // go to first item
#define wxACTION_LISTBOX_END        _T("end")       // go to last item

// do something with the current item
#define wxACTION_LISTBOX_ACTIVATE   _T("activate")  // activate (choose)
#define wxACTION_LISTBOX_TOGGLE     _T("toggle")    // togglee selected state
#define wxACTION_LISTBOX_SELECT     _T("select")    // select/focus
#define wxACTION_LISTBOX_UNSELECT   _T("unselect")  // unselect/unfocus

// ----------------------------------------------------------------------------
// wxListBox: a list of selectable items
// ----------------------------------------------------------------------------

class wxListBox : public wxListBoxBase, public wxScrollHelper
{
public:
    // ctors and such
    wxListBox() { Init(); }
    wxListBox(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int n = 0, const wxString choices[] = (const wxString *) NULL,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxListBoxNameStr )
    {
        Init();

        Create(parent, id, pos, size, n, choices, style, validator, name);
    }

    virtual ~wxListBox();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = (const wxString *) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxListBoxNameStr);

    // implement the listbox interface defined by wxListBoxBase
    virtual void Clear();
    virtual void Delete(int n);

    virtual int GetCount() const { return (int)m_strings.GetCount(); }
    virtual wxString GetString(int n) const { return m_strings[n]; }
    virtual void SetString(int n, const wxString& s);
    virtual int FindString(const wxString& s) const
        { return m_strings.Index(s); }

    virtual bool IsSelected(int n) const
        { return m_selections.Index(n) != wxNOT_FOUND; }
    virtual void SetSelection(int n, bool select = TRUE);
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

protected:
    virtual int DoAppend(const wxString& item);
    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);

    virtual void DoSetFirstItem(int n);

    virtual void DoSetItemClientData(int n, void* clientData);
    virtual void* DoGetItemClientData(int n) const;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(int n) const;

public:
    // override some more base class methods
    virtual bool SetFont(const wxFont& font);

    virtual void Refresh( bool eraseBackground = TRUE,
                          const wxRect *rect = (const wxRect *) NULL );

    // the wxUniversal-specific additions

    // the current item is the same as the selected one for wxLB_SINGLE
    // listboxes but for the other ones it is just the focused item which may
    // be selected or not
    int GetCurrentItem() const { return m_current; }
    void SetCurrentItem(int n);

    // select the item which is diff items below the current one
    void ChangeCurrent(int diff);

    // actions
    void Activate();
    void Select(bool sel = TRUE);
    void EnsureVisible();

    // get, calculating it if necessary, the number of items per page, the
    // height of each line and the max width of an item
    int GetItemsPerPage() const;
    wxCoord GetLineHeight() const;
    wxCoord GetMaxWidth() const;

    // override the wxControl virtual methods
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0l,
                               const wxString& strArg = wxEmptyString);

protected:
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);
    virtual wxBorder GetDefaultBorder() const;

    // common part of all ctors
    void Init();

    // event handlers
    void OnIdle(wxIdleEvent& event);
    void OnSize(wxSizeEvent& event);

    // common part of Clear() and DoSetItems(): clears everything
    void DoClear();

    // refresh the given item(s) or everything
    void RefreshItems(int from, int count);
    void RefreshItem(int n);
    void RefreshAll();

    // send an event of the given type
    bool SendEvent(int item, wxEventType type);

    // calculate the number of items per page using our current size
    void CalcItemsPerPage();

    // can/should we have a horz scrollbar?
    bool HasHorzScrollbar() const
        { return (m_windowStyle & wxLB_HSCROLL) != 0; }

    // the array containing all items (it is sorted if the listbox has
    // wxLB_SORT style)
    wxArrayString m_strings;

    // this array contains the indices of the selected items (for the single
    // selection listboxes only the first element of it is used and contains
    // the current selection)
    wxArrayInt m_selections;

    // and this one the client data (either void or wxClientData)
    wxArrayPtrVoid m_clientData;

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

    // the maximal width of a listbox item
    wxCoord m_maxWidth;

    // the number of items per page
    size_t m_itemsPerPage;

    // if the number of items has changed we may need to show/hide the
    // scrollbar
    bool m_updateScrollbarX, m_updateScrollbarY,
         m_showScrollbarX, m_showScrollbarY;

    // if the current item has changed, we might need to scroll if it went out
    // of the window
    bool m_currentChanged;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxListBox)
};

#endif // _WX_UNIV_LISTBOX_H_
