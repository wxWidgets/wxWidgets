///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/headercol.cpp
// Purpose:     wxHeaderColumn implementation for wxMSW
// Author:      Vadim Zeitlin
// Created:     2008-12-03
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/headercol.h"

#include "wx/msw/wrapcctl.h"

// ----------------------------------------------------------------------------
// wxMSWHeaderColumnImpl
// ----------------------------------------------------------------------------

struct wxMSWHeaderColumnImpl
{
    wxMSWHeaderColumnImpl()
    {
        flags = wxCOL_DEFAULT_FLAGS;
        minWidth = 0;
        ascending = true;
    }

    wxHDITEM hdi;
    wxWxCharBuffer bufTitle;
    wxBitmap bmp;
    int flags;          // combination of wxCOL_XXX constants
    int minWidth;       // 0 if not set
    bool ascending;     // sort order

    DECLARE_NO_COPY_CLASS(wxMSWHeaderColumnImpl)
};

// ============================================================================
// wxHeaderColumn implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxHeaderColumn construction/destruction
// ----------------------------------------------------------------------------

void wxHeaderColumn::Init()
{
    m_impl = new wxMSWHeaderColumnImpl;
}

wxHeaderColumn::wxHeaderColumn(const wxString& title,
                                       int width,
                                       wxAlignment align,
                                       int flags)
{
    Init();

    SetTitle(title);
    SetWidth(width);
    SetAlignment(align);
    SetFlags(flags);
}

wxHeaderColumn::wxHeaderColumn(const wxBitmap& bitmap,
                                       int width,
                                       wxAlignment align,
                                       int flags)
{
    Init();

    SetBitmap(bitmap);
    SetWidth(width);
    SetAlignment(align);
    SetFlags(flags);
}

wxHeaderColumn::~wxHeaderColumn()
{
    delete m_impl;
}

// ----------------------------------------------------------------------------
// wxHeaderColumn accessors corresponding to HDITEM fields
// ----------------------------------------------------------------------------

wxHDITEM& wxHeaderColumn::GetHDI()
{
    return m_impl->hdi;
}

void wxHeaderColumn::SetTitle(const wxString& title)
{
    HDITEM& hdi = m_impl->hdi;

    hdi.mask |= HDI_TEXT;

    // notice that we need to store the string we use the pointer to
    wxWxCharBuffer& buf = m_impl->bufTitle;
    buf = title.wc_str();
    hdi.pszText = buf.data();
    hdi.cchTextMax = wxStrlen(buf);
}

wxString wxHeaderColumn::GetTitle() const
{
    HDITEM& hdi = m_impl->hdi;

    wxASSERT_MSG( hdi.mask & HDI_TEXT, "title not set" );

    return hdi.pszText;
}

void wxHeaderColumn::SetWidth(int width)
{
    HDITEM& hdi = m_impl->hdi;

    if ( width != wxCOL_WIDTH_DEFAULT )
    {
        hdi.mask |= HDI_WIDTH;
        hdi.cxy = width;
    }
}

int wxHeaderColumn::GetWidth() const
{
    HDITEM& hdi = m_impl->hdi;

    return hdi.mask & HDI_WIDTH ? hdi.cxy : wxCOL_WIDTH_DEFAULT;
}

void wxHeaderColumn::SetAlignment(wxAlignment align)
{
    HDITEM& hdi = m_impl->hdi;

    if ( align != wxALIGN_NOT )
    {
        hdi.mask |= HDI_FORMAT;
        switch ( align )
        {
            case wxALIGN_LEFT:
                hdi.fmt |= HDF_LEFT;
                break;

            case wxALIGN_CENTER:
            case wxALIGN_CENTER_HORIZONTAL:
                hdi.fmt |= HDF_CENTER;
                break;

            case wxALIGN_RIGHT:
                hdi.fmt |= HDF_RIGHT;
                break;

            default:
                wxFAIL_MSG( "invalid column header alignment" );
        }
    }
}

wxAlignment wxHeaderColumn::GetAlignment() const
{
    HDITEM& hdi = m_impl->hdi;

    if ( !(hdi.mask & HDI_FORMAT) )
        return wxALIGN_NOT;

    if ( hdi.fmt & HDF_CENTER )
        return wxALIGN_CENTER;

    if ( hdi.fmt & HDF_RIGHT )
        return wxALIGN_RIGHT;

    // HDF_LEFT == 0 so it doesn't make sense to test for it with bit and
    return wxALIGN_LEFT;
}

void wxHeaderColumn::SetClientData(wxUIntPtr data)
{
    HDITEM& hdi = m_impl->hdi;

    hdi.mask |= HDI_LPARAM;
    hdi.lParam = data;
}

wxUIntPtr wxHeaderColumn::GetClientData() const
{
    HDITEM& hdi = m_impl->hdi;

    wxASSERT_MSG( hdi.mask & HDI_LPARAM, "client data not set" );

    return hdi.lParam;
}

// ----------------------------------------------------------------------------
// wxHeaderColumn trivial accessors for fields stored internally
// ----------------------------------------------------------------------------

void wxHeaderColumn::SetBitmap(const wxBitmap& bitmap)
{
    m_impl->bmp = bitmap;
}

wxBitmap wxHeaderColumn::GetBitmap() const
{
    return m_impl->bmp;
}

void wxHeaderColumn::SetMinWidth(int minWidth)
{
    m_impl->minWidth = minWidth;
}

int wxHeaderColumn::GetMinWidth() const
{
    return m_impl->minWidth;
}

void wxHeaderColumn::SetFlags(int flags)
{
    m_impl->flags = flags;
}

int wxHeaderColumn::GetFlags() const
{
    return m_impl->flags;
}

void wxHeaderColumn::SetSortOrder(bool ascending)
{
    m_impl->ascending = ascending;
}

bool wxHeaderColumn::IsSortOrderAscending() const
{
    return m_impl->ascending;
}


