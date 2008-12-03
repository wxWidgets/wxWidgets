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

class WXDLLIMPEXP_CORE wxHeaderColumnBase : public wxObject
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

    // arbitrary client data associated with the column (currently only
    // implemented in MSW because it is used in MSW wxDataViewCtrl
    // implementation)
    virtual void SetClientData(wxUIntPtr WXUNUSED(data))
        { wxFAIL_MSG("not implemented"); }
    virtual wxUIntPtr GetClientData() const
        { return 0; }


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

    // for sortable columns indicate whether we should sort in ascending or
    // descending order
    virtual void SetSortOrder(bool ascending) = 0;
    void ToggleSortOrder() { SetSortOrder(!IsSortOrderAscending()); }
    virtual bool IsSortOrderAscending() const = 0;

protected:
    // helpers for the class overriding Set/IsXXX()
    void SetIndividualFlags(int flags);
    int GetFromIndividualFlags() const;
};

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/headercol.h"
#elif 0 // TODO
    #define wxHAS_GENERIC_HEADERCOL
    #include "wx/generic/headercolg.h"
#endif

#endif // _WX_HEADERCOL_H_

