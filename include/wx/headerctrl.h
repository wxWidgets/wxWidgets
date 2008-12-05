///////////////////////////////////////////////////////////////////////////////
// Name:        wx/headerctrl.h
// Purpose:     wxHeaderCtrlBase class: interface of wxHeaderCtrl
// Author:      Vadim Zeitlin
// Created:     2008-12-01
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HEADERCTRL_H_
#define _WX_HEADERCTRL_H_

#include "wx/control.h"

#include "wx/headercol.h"

// notice that the classes in this header are defined in the core library even
// although currently they're only used by wxGrid which is in wxAdv because we
// plan to use it in wxListCtrl which is in core too in the future

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    // allow column drag and drop
    wxHD_DRAGDROP = 0x0001,

    // style used by default when creating the control
    wxHD_DEFAULT_STYLE = wxHD_DRAGDROP
};

extern WXDLLIMPEXP_DATA_CORE(const char) wxHeaderCtrlNameStr[];

class WXDLLIMPEXP_FWD_CORE wxHeaderColumn;

// ----------------------------------------------------------------------------
// wxHeaderCtrlBase defines the interface of a header control
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxHeaderCtrlBase : public wxControl
{
public:
    /*
        Derived classes must provide default ctor as well as a ctor and
        Create() function with the following signatures:

    wxHeaderCtrl(wxWindow *parent,
                 wxWindowID winid = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxHeaderCtrlNameStr);

    bool Create(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxHeaderCtrlNameStr);
     */

    // managing the columns
    // --------------------

    // return the number of columns in the control
    unsigned int GetColumnCount() const { return DoGetCount(); }

    // return whether the control has any columns
    bool IsEmpty() const { return GetColumnCount() == 0; }

    // insert the column at the given position, using GetColumnCount() as
    // position appends it at the end
    void InsertColumn(const wxHeaderColumn& col, unsigned int idx)
    {
        wxCHECK_RET( idx <= GetColumnCount(), "invalid column index" );

        DoInsert(col, idx);
    }

    // append the column to the end of the control
    void AppendColumn(const wxHeaderColumn& col)
    {
        DoInsert(col, GetColumnCount());
    }

    // delete the column at the given index
    void DeleteColumn(unsigned int idx)
    {
        wxCHECK_RET( idx < GetColumnCount(), "invalid column index" );

        DoDelete(idx);
    }

    // delete all the existing columns
    void DeleteAllColumns();


    // modifying columns
    // -----------------

    // show or hide the column, notice that even when a column is hidden we
    // still account for it when using indices
    void ShowColumn(unsigned int idx, bool show = true)
    {
        wxCHECK_RET( idx < GetColumnCount(), "invalid column index" );

        DoShowColumn(idx, show);
    }

    void HideColumn(unsigned int idx)
    {
        ShowColumn(idx, false);
    }

    // indicate that the column is used for sorting in ascending order if
    // sortOrder is true, for sorting in descending order if it is false or not
    // used for sorting at all if it is -1
    void ShowSortIndicator(unsigned int idx, int sortOrder)
    {
        wxCHECK_RET( sortOrder == 0 || sortOrder == 1 || sortOrder == -1,
                     "invalid sort order value" );

        wxCHECK_RET( idx < GetColumnCount(), "invalid column index" );

        DoShowSortIndicator(idx, sortOrder);
    }

    // remove the sort indicator from the given column
    void RemoveSortIndicator(unsigned int idx)
    {
        DoShowSortIndicator(idx, -1);
    }


    // implementation only from now on
    // -------------------------------

    // the user doesn't need to TAB to this control
    virtual bool AcceptsFocusFromKeyboard() const { return false; }

private:
    virtual unsigned int DoGetCount() const = 0;
    virtual void DoInsert(const wxHeaderColumn& col, unsigned int idx) = 0;
    virtual void DoDelete(unsigned int idx) = 0;
    virtual void DoShowColumn(unsigned int idx, bool show) = 0;
    virtual void DoShowSortIndicator(unsigned int idx, int sortOrder) = 0;
};

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/headerctrl.h"
#elif 0 // TODO
    #define wxHAS_GENERIC_HEADERCTRL
    #include "wx/generic/headerctrlg.h"
#endif // platform

#endif // _WX_HEADERCTRL_H_
