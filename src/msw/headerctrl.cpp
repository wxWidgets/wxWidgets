///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/headerctrl.cpp
// Purpose:     implementation of wxHeaderCtrl for wxMSW
// Author:      Vadim Zeitlin
// Created:     2008-12-01
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
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/headerctrl.h"
#include "wx/imaglist.h"

#include "wx/msw/wrapcctl.h"
#include "wx/msw/private.h"

// ============================================================================
// wxHeaderCtrl implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxHeaderCtrl construction/destruction
// ----------------------------------------------------------------------------

void wxHeaderCtrl::Init()
{
    m_imageList = NULL;
}

bool wxHeaderCtrl::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    // notice that we don't need InitCommonControlsEx(ICC_LISTVIEW_CLASSES)
    // here as we already call InitCommonControls() in wxApp initialization
    // code which covers this

    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(WC_HEADER, _T(""), pos, size) )
        return false;

    return true;
}

WXDWORD wxHeaderCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    if ( style & wxHD_DRAGDROP )
        msStyle |= HDS_DRAGDROP;

    // the control looks nicer with these styles and there doesn't seem to be
    // any reason to not use them so we always do (as for HDS_HORZ it is 0
    // anyhow but include it for clarity)
    msStyle |= HDS_HORZ | HDS_BUTTONS | HDS_FLAT | HDS_FULLDRAG | HDS_HOTTRACK;

    return msStyle;
}

wxHeaderCtrl::~wxHeaderCtrl()
{
    delete m_imageList;
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl geometry calculation
// ----------------------------------------------------------------------------

wxSize wxHeaderCtrl::DoGetBestSize() const
{
    RECT rc = wxGetClientRect(GetHwndOf(GetParent()));
    WINDOWPOS wpos;
    HDLAYOUT layout = { &rc, &wpos };
    if ( !Header_Layout(GetHwnd(), &layout) )
    {
        wxLogLastError(_T("Header_Layout"));
        return wxControl::DoGetBestSize();
    }

    return wxSize(wpos.cx, wpos.cy);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl columns managements
// ----------------------------------------------------------------------------

unsigned int wxHeaderCtrl::DoGetCount() const
{
    return Header_GetItemCount(GetHwnd());
}

void wxHeaderCtrl::DoInsert(const wxHeaderColumn& col, unsigned int idx)
{
    // copy the HDITEM because we may modify it below
    HDITEM hdi = col.GetHDI();

    const wxBitmap bmp = col.GetBitmap();
    if ( bmp.IsOk() )
    {
        const int bmpWidth = bmp.GetWidth(),
                  bmpHeight = bmp.GetHeight();

        if ( !m_imageList )
        {
            m_imageList = new wxImageList(bmpWidth, bmpHeight);
            Header_SetImageList(GetHwnd(), GetHimagelistOf(m_imageList));
        }
        else // already have an image list
        {
            // check that all bitmaps we use have the same size
            int imageWidth,
                imageHeight;
            m_imageList->GetSize(0, imageWidth, imageHeight);

            wxASSERT_MSG( imageWidth == bmpWidth && imageHeight == bmpHeight,
                          "all column bitmaps must have the same size" );
        }

        m_imageList->Add(bmp);
        hdi.mask |= HDI_IMAGE;
        hdi.iImage = m_imageList->GetImageCount() - 1;
    }

    if ( col.IsHidden() )
    {
        hdi.mask |= HDI_WIDTH;
        hdi.cxy = 0;
    }

    if ( Header_InsertItem(GetHwnd(), idx, &hdi) == -1 )
    {
        wxLogLastError(_T("Header_InsertItem"));
    }
}

void wxHeaderCtrl::DoDelete(unsigned int idx)
{
    if ( !Header_DeleteItem(GetHwnd(), idx) )
    {
        wxLogLastError(_T("Header_DeleteItem"));
    }
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl columns attributes
// ----------------------------------------------------------------------------

void wxHeaderCtrl::DoShowColumn(unsigned int idx, bool show)
{
    wxHDITEM hdi;
    hdi.mask = HDI_WIDTH;

    if ( !Header_GetItem(GetHwnd(), idx, &hdi) )
    {
        wxLogLastError(_T("Header_GetItem(HDI_WIDTH)"));
        return;
    }

    if ( show )
        hdi.cxy = 80; // FIXME: we don't have the column width here any more
    else
        hdi.cxy = 0;

    if ( !Header_SetItem(GetHwnd(), idx, &hdi) )
    {
        wxLogLastError(_T("Header_SetItem(HDI_WIDTH)"));
        return;
    }
}

void wxHeaderCtrl::DoShowSortIndicator(unsigned int idx, int sortOrder)
{
    wxHDITEM hdi;
    hdi.mask = HDI_FORMAT;

    if ( !Header_GetItem(GetHwnd(), idx, &hdi) )
    {
        wxLogLastError(_T("Header_GetItem(HDI_FORMAT)"));
        return;
    }

    if ( sortOrder == -1 )
        hdi.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
    else
        hdi.fmt |= sortOrder ? HDF_SORTUP : HDF_SORTDOWN;

    if ( !Header_SetItem(GetHwnd(), idx, &hdi) )
    {
        wxLogLastError(_T("Header_SetItem(HDI_FORMAT)"));
    }
}

