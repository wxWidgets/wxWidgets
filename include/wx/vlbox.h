///////////////////////////////////////////////////////////////////////////////
// Name:        wx/vlbox.h
// Purpose:     wxVListBox is a virtual listbox with lines of variable height
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.05.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VLBOX_H_
#define _WX_VLBOX_H_

#include "wx/vscroll.h"         // base class

#define wxVListBoxNameStr _T("wxVListBox")

// ----------------------------------------------------------------------------
// wxVListBox
// ----------------------------------------------------------------------------

/*
    This class has two main differences from a regular listbox: it can have an
    arbitrarily huge number of items because it doesn't store them itself but
    uses OnDrawItem() callback to draw them and its items can have variable
    height as determined by OnMeasureItem().

    It emits the same events as wxListBox and the same event macros may be used
    with it.
 */
class WXDLLEXPORT wxVListBox : public wxVScrolledWindow
{
public:
    // constructors and such
    // ---------------------

    // default constructor, you must call Create() later
    wxVListBox() { Init(); }

    // normal constructor which calls Create() internally
    wxVListBox(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               size_t countItems = 0,
               long style = 0,
               const wxString& name = wxVListBoxNameStr)
    {
        Init();

        (void)Create(parent, id, pos, size, countItems, style, name);
    }

    // really creates the control and sets the initial number of items in it
    // (which may be changed later with SetItemCount())
    //
    // there are no special styles defined for wxVListBox
    //
    // returns true on success or false if the control couldn't be created
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                size_t countItems = 0,
                long style = 0,
                const wxString& name = wxVListBoxNameStr);


    // operations
    // ----------

    // set the number of items to be shown in the control
    //
    // this is just a synonym for wxVScrolledWindow::SetLineCount()
    void SetItemCount(size_t count) { SetLineCount(count); }

    // delete all items from the control
    void Clear() { SetItemCount(0); }

    // set the selection to the specified item, if it is -1 the selection is
    // unset
    void SetSelection(int selection) { DoSetSelection(selection, false); }

    // set the margins: horizontal margin is the distance between the window
    // border and the item contents while vertical margin is half of the
    // distance between items
    //
    // by default both margins are 0
    void SetMargins(const wxPoint& pt);
    void SetMargins(wxCoord x, wxCoord y) { SetMargins(wxPoint(x, y)); }


    // accessors
    // ---------

    // get the number of items in the control
    size_t GetItemCount() const { return GetLineCount(); }

    // get the currently selected item or -1 if there is no selection
    int GetSelection() const { return m_selection; }

    // is this item selected?
    bool IsSelected(size_t line) const { return (int)line == m_selection; }

    // get the margins around each item
    wxPoint GetMargins() const { return m_ptMargins; }


protected:
    // the derived class must implement this function to actually draw the item
    // with the given index on the provided DC
    virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const = 0;

    // the derived class must implement this method to return the height of the
    // specified item
    virtual wxCoord OnMeasureItem(size_t n) const = 0;

    // this method may be used to draw separators between the lines; note that
    // the rectangle may be modified, typically to deflate it a bit before
    // passing to OnDrawItem()
    //
    // the base class version doesn't do anything
    virtual void OnDrawSeparator(wxDC& dc, wxRect& rect, size_t n) const;


    // we implement OnGetLineHeight() in terms of OnMeasureItem() because this
    // allows us to add borders to the items easily
    //
    // this function is not upposed to be overridden by the derived classes
    virtual wxCoord OnGetLineHeight(size_t line) const;


    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftDClick(wxMouseEvent& event);


    // common part of all ctors
    void Init();

    // SetSelection() with additional parameter telling it whether to send a
    // notification event or not
    void DoSetSelection(int selection, bool sendEvent = true);

private:
    // the current selection or -1
    int m_selection;

    // margins
    wxPoint m_ptMargins;


    DECLARE_EVENT_TABLE()
};

#endif // _WX_VLBOX_H_

