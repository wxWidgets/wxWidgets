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

#include "wx/vector.h"

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
                 long style = wxHD_DEFAULT_STYLE,
                 const wxString& name = wxHeaderCtrlNameStr);

    bool Create(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHD_DEFAULT_STYLE,
                const wxString& name = wxHeaderCtrlNameStr);
     */

    // column-related methods
    // ----------------------

    // set the number of columns in the control
    //
    // this also calls UpdateColumn() for all columns
    void SetColumnCount(unsigned int count) { DoSetCount(count); }

    // return the number of columns in the control as set by SetColumnCount()
    unsigned int GetColumnCount() const { return DoGetCount(); }

    // return whether the control has any columns
    bool IsEmpty() const { return DoGetCount() == 0; }

    // update the column with the given index
    void UpdateColumn(unsigned int idx)
    {
        wxCHECK_RET( idx < GetColumnCount(), "invalid column index" );

        DoUpdate(idx);
    }


    // implementation only from now on
    // -------------------------------

    // the user doesn't need to TAB to this control
    virtual bool AcceptsFocusFromKeyboard() const { return false; }

    // this method is only overridden in order to synchronize the control with
    // the main window when it is scrolled, the derived class must implement
    // DoScrollHorz()
    virtual void ScrollWindow(int dx, int dy, const wxRect *rect = NULL);

protected:
    // this method must be implemented by the derived classes to return the
    // information for the given column
    virtual wxHeaderColumnBase& GetColumn(unsigned int idx) = 0;

private:
    // methods implementing our public API and defined in platform-specific
    // implementations
    virtual void DoSetCount(unsigned int count) = 0;
    virtual unsigned int DoGetCount() const = 0;
    virtual void DoUpdate(unsigned int idx) = 0;

    virtual void DoScrollHorz(int dx) = 0;

    // this window doesn't look nice with the border so don't use it by default
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }
};

// ----------------------------------------------------------------------------
// wxHeaderCtrl: port-specific header control implementation, notice that this
//               is still an ABC which is meant to be used as part of another
//               control, see wxHeaderCtrlSimple for a standalone version
// ----------------------------------------------------------------------------

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/headerctrl.h"
#else
    #define wxHAS_GENERIC_HEADERCTRL
    #include "wx/generic/headerctrlg.h"
#endif // platform

// ----------------------------------------------------------------------------
// wxHeaderCtrlSimple: concrete header control which can be used standalone
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxHeaderCtrlSimple : public wxHeaderCtrl
{
public:
    // control creation
    // ----------------

    wxHeaderCtrlSimple() { Init(); }
    wxHeaderCtrlSimple(wxWindow *parent,
                       wxWindowID winid = wxID_ANY,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxHD_DEFAULT_STYLE,
                       const wxString& name = wxHeaderCtrlNameStr)
    {
        Init();

        Create(parent, winid, pos, size, style, name);
    }

    // managing the columns
    // --------------------

    // insert the column at the given position, using GetColumnCount() as
    // position appends it at the end
    void InsertColumn(const wxHeaderColumnSimple& col, unsigned int idx)
    {
        wxCHECK_RET( idx <= GetColumnCount(), "invalid column index" );

        DoInsert(col, idx);
    }

    // append the column to the end of the control
    void AppendColumn(const wxHeaderColumnSimple& col)
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

    // indicate that the column is used for sorting
    void ShowSortIndicator(unsigned int idx, bool ascending = true)
    {
        wxCHECK_RET( idx < GetColumnCount(), "invalid column index" );

        DoShowSortIndicator(idx, ascending);
    }

    // remove the sort indicator completely
    void RemoveSortIndicator();

protected:
    virtual wxHeaderColumnBase& GetColumn(unsigned int idx);

private:
    // functions implementing our public API
    void DoInsert(const wxHeaderColumnSimple& col, unsigned int idx);
    void DoDelete(unsigned int idx);
    void DoShowColumn(unsigned int idx, bool show);
    void DoShowSortIndicator(unsigned int idx, bool ascending);

    // common part of all ctors
    void Init();

    // bring the column count in sync with the number of columns we store
    void UpdateColumnCount() { SetColumnCount(m_cols.size()); }


    // all our current columns
    typedef wxVector<wxHeaderColumnSimple> Columns;
    Columns m_cols;

    // the column currently used for sorting or -1 if none
    unsigned int m_sortKey;


    DECLARE_NO_COPY_CLASS(wxHeaderCtrlSimple)
};

#endif // _WX_HEADERCTRL_H_
