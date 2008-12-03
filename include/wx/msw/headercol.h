///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/headercol.h
// Purpose:     wxHeaderColumn implementation for MSW
// Author:      Vadim Zeitlin
// Created:     2008-12-02
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_HEADERCOL_H_
#define _WX_MSW_HEADERCOL_H_

struct wxHDITEM;

// ----------------------------------------------------------------------------
// wxHeaderColumn
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxHeaderColumn : public wxHeaderColumnBase
{
public:
    // ctors and dtor
    wxHeaderColumn(const wxString& title,
                       int width = wxCOL_WIDTH_DEFAULT,
                       wxAlignment align = wxALIGN_NOT,
                       int flags = wxCOL_DEFAULT_FLAGS);
    wxHeaderColumn(const wxBitmap &bitmap,
                       int width = wxCOL_WIDTH_DEFAULT,
                       wxAlignment align = wxALIGN_CENTER,
                       int flags = wxCOL_DEFAULT_FLAGS);

    virtual ~wxHeaderColumn();

    // implement base class pure virtuals
    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;

    virtual void SetBitmap(const wxBitmap& bitmap);
    wxBitmap GetBitmap() const;

    virtual void SetWidth(int width);
    virtual int GetWidth() const;

    virtual void SetMinWidth(int minWidth);
    virtual int GetMinWidth() const;

    virtual void SetAlignment(wxAlignment align);
    virtual wxAlignment GetAlignment() const;

    virtual void SetClientData(wxUIntPtr data);
    virtual wxUIntPtr GetClientData() const;

    virtual void SetFlags(int flags);
    virtual int GetFlags() const;


    virtual void SetSortOrder(bool ascending);
    virtual bool IsSortOrderAscending() const;


    // MSW-specific implementation helpers
    wxHDITEM& GetHDI();
    const wxHDITEM& GetHDI() const
    {
        return const_cast<wxHeaderColumn *>(this)->GetHDI();
    }

private:
    // initialize m_impl
    void Init();

    struct wxMSWHeaderColumnImpl *m_impl;
};

#endif // _WX_MSW_HEADERCOL_H_

