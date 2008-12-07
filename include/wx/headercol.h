///////////////////////////////////////////////////////////////////////////////
// Name:        wx/headercol.h
// Purpose:     declaration of wxHeaderColumn class
// Author:      Vadim Zeitlin
// Created:     2008-12-02
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HEADERCOL_H_
#define _WX_HEADERCOL_H_

#include "wx/bitmap.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    // special value for column width meaning unspecified/default
    wxCOL_WIDTH_DEFAULT = -1
};

// bit masks for the various column attributes
enum
{
    // column can be resized (included in default flags)
    wxCOL_RESIZABLE   = 1,

    // column can be clicked to toggle the sort order by its contents
    wxCOL_SORTABLE    = 2,

    // column can be dragged to change its order (included in default)
    wxCOL_REORDERABLE = 4,

    // column is not shown at all
    wxCOL_HIDDEN      = 8,

    // default flags for wxHeaderColumn ctor
    wxCOL_DEFAULT_FLAGS = wxCOL_RESIZABLE | wxCOL_REORDERABLE
};

// ----------------------------------------------------------------------------
// wxHeaderColumnBase: interface for a column in a header of controls such as
//                     wxListCtrl, wxDataViewCtrl or wxGrid
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxHeaderColumnBase
{
public:
    // ctors and dtor
    // --------------

    /*
       Derived classes must provide ctors with the following signatures
       (notice that they shouldn't be explicit to allow passing strings/bitmaps
       directly to methods such wxHeaderCtrl::AppendColumn()):
    wxHeaderColumn(const wxString& title,
                   int width = wxCOL_WIDTH_DEFAULT,
                   wxAlignment align = wxALIGN_NOT,
                   int flags = wxCOL_DEFAULT_FLAGS);
    wxHeaderColumn(const wxBitmap &bitmap,
                   int width = wxDVC_DEFAULT_WIDTH,
                   wxAlignment align = wxALIGN_CENTER,
                   int flags = wxCOL_DEFAULT_FLAGS);
    */


    // setters and getters for various attributes
    // ------------------------------------------

    // title is the string shown for this column
    virtual void SetTitle(const wxString& title) = 0;
    virtual wxString GetTitle() const = 0;

    // bitmap shown (instead of text) in the column header
    virtual void SetBitmap(const wxBitmap& bitmap) = 0;
    virtual wxBitmap GetBitmap() const = 0;                                   \

    // width of the column in pixels, can be set to wxCOL_WIDTH_DEFAULT meaning
    // unspecified/default
    virtual void SetWidth(int width) = 0;
    virtual int GetWidth() const = 0;

    // minimal width can be set for resizeable columns to forbid resizing them
    // below the specified size (set to 0 to remove)
    virtual void SetMinWidth(int minWidth) = 0;
    virtual int GetMinWidth() const = 0;

    // alignment of the text: wxALIGN_CENTRE, wxALIGN_LEFT or wxALIGN_RIGHT
    virtual void SetAlignment(wxAlignment align) = 0;
    virtual wxAlignment GetAlignment() const = 0;


    // flags manipulations:
    // --------------------

    // notice that while we make Set/GetFlags() pure virtual here and implement
    // the individual flags access in terms of them, for some derived classes
    // it is more natural to implement access to each flag individually, in
    // which case they can use SetIndividualFlags() and GetFromIndividualFlags() 
    // below to implement Set/GetFlags()

    // set or retrieve all column flags at once: combination of wxCOL_XXX
    // values above
    virtual void SetFlags(int flags) = 0;
    virtual int GetFlags() const = 0;

    // change, set, clear, toggle or test for any individual flag
    void ChangeFlag(int flag, bool set);
    void SetFlag(int flag);
    void ClearFlag(int flag);
    void ToggleFlag(int flag);

    bool HasFlag(int flag) const { return (GetFlags() & flag) != 0; }


    // wxCOL_RESIZABLE
    virtual void SetResizeable(bool resizeable)
        { ChangeFlag(wxCOL_RESIZABLE, resizeable); }
    virtual bool IsResizeable() const
        { return HasFlag(wxCOL_RESIZABLE); }

    // wxCOL_SORTABLE
    virtual void SetSortable(bool sortable)
        { ChangeFlag(wxCOL_SORTABLE, sortable); }
    virtual bool IsSortable() const
        { return HasFlag(wxCOL_SORTABLE); }

    // wxCOL_REORDERABLE
    virtual void SetReorderable(bool reorderable)
        { ChangeFlag(wxCOL_REORDERABLE, reorderable); }
    virtual bool IsReorderable() const
        { return HasFlag(wxCOL_REORDERABLE); }

    // wxCOL_HIDDEN
    virtual void SetHidden(bool hidden)
        { ChangeFlag(wxCOL_HIDDEN, hidden); }
    virtual bool IsHidden() const
        { return HasFlag(wxCOL_HIDDEN); }
    bool IsShown() const
        { return !IsHidden(); }


    // sorting
    // -------

    // set this column as the one used to sort the control
    virtual void SetAsSortKey(bool sort = true) = 0;
    void UnsetAsSortKey() { SetAsSortKey(false); }

    // return true if the column is used for sorting
    virtual bool IsSortKey() const = 0;

    // for sortable columns indicate whether we should sort in ascending or
    // descending order (this should only be taken into account if IsSortKey())
    virtual void SetSortOrder(bool ascending) = 0;
    void ToggleSortOrder() { SetSortOrder(!IsSortOrderAscending()); }
    virtual bool IsSortOrderAscending() const = 0;

protected:
    // helpers for the class overriding Set/IsXXX()
    void SetIndividualFlags(int flags);
    int GetFromIndividualFlags() const;
};

// ----------------------------------------------------------------------------
// wxHeaderColumnSimple: trivial generic implementation of wxHeaderColumnBase
// ----------------------------------------------------------------------------

class wxHeaderColumnSimple : public wxHeaderColumnBase
{
public:
    // ctors and dtor
    wxHeaderColumnSimple(const wxString& title,
                         int width = wxCOL_WIDTH_DEFAULT,
                         wxAlignment align = wxALIGN_NOT,
                         int flags = wxCOL_DEFAULT_FLAGS)
        : m_title(title),
          m_width(width),
          m_align(align),
          m_flags(flags)
    {
        Init();
    }

    wxHeaderColumnSimple(const wxBitmap& bitmap,
                         int width = wxCOL_WIDTH_DEFAULT,
                         wxAlignment align = wxALIGN_CENTER,
                         int flags = wxCOL_DEFAULT_FLAGS)
        : m_bitmap(bitmap),
          m_width(width),
          m_align(align),
          m_flags(flags)
    {
        Init();
    }

    // implement base class pure virtuals
    virtual void SetTitle(const wxString& title) { m_title = title; }
    virtual wxString GetTitle() const { return m_title; }

    virtual void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    wxBitmap GetBitmap() const { return m_bitmap; }

    virtual void SetWidth(int width) { m_width = width; }
    virtual int GetWidth() const { return m_width; }

    virtual void SetMinWidth(int minWidth) { m_minWidth = minWidth; }
    virtual int GetMinWidth() const { return m_minWidth; }

    virtual void SetAlignment(wxAlignment align) { m_align = align; }
    virtual wxAlignment GetAlignment() const { return m_align; }

    virtual void SetFlags(int flags) { m_flags = flags; }
    virtual int GetFlags() const { return m_flags; }

    virtual void SetAsSortKey(bool sort = true) { m_sort = sort; }
    virtual bool IsSortKey() const { return m_sort; }

    virtual void SetSortOrder(bool ascending) { m_sortAscending = ascending; }
    virtual bool IsSortOrderAscending() const { return m_sortAscending; }

private:
    // common part of all ctors
    void Init()
    {
        m_minWidth = 0;
        m_sort = false;
        m_sortAscending = true;
    }

    wxString m_title;
    wxBitmap m_bitmap;
    int m_width,
        m_minWidth;
    wxAlignment m_align;
    int m_flags;
    bool m_sort,
         m_sortAscending;
};
#endif // _WX_HEADERCOL_H_

