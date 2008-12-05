///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/headercolg.h
// Purpose:     Generic wxHeaderColumn implementation
// Author:      Vadim Zeitlin
// Created:     2008-12-04
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_HEADERCOLG_H_
#define _WX_GENERIC_HEADERCOLG_H_

// ----------------------------------------------------------------------------
// wxHeaderColumn: trivial generic implementation of wxHeaderColumnBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxHeaderColumn : public wxHeaderColumnBase
{
public:
    // ctors and dtor
    wxHeaderColumn(const wxString& title,
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

    wxHeaderColumn(const wxBitmap& bitmap,
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

    virtual void SetSortOrder(bool ascending) { m_sortAscending = ascending; }
    virtual bool IsSortOrderAscending() const { return m_sortAscending; }

private:
    // common part of all ctors
    void Init()
    {
        m_minWidth = 0;
        m_sortAscending = true;
    }

    wxString m_title;
    wxBitmap m_bitmap;
    int m_width,
        m_minWidth;
    wxAlignment m_align;
    int m_flags;
    bool m_sortAscending;
};
#endif // _WX_GENERIC_HEADERCOLG_H_

