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

#ifndef wxHAS_GENERIC_HEADERCTRL

#include "wx/imaglist.h"

#include "wx/msw/wrapcctl.h"
#include "wx/msw/private.h"

// from src/msw/listctrl.cpp
extern int WXDLLIMPEXP_CORE wxMSWGetColumnClicked(NMHDR *nmhdr, POINT *ptClick);

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
// wxHeaderCtrl scrolling
// ----------------------------------------------------------------------------

void wxHeaderCtrl::DoScrollHorz(int dx)
{
    // as the native control doesn't support offsetting its contents, we use a
    // hack here to make it appear correctly when the parent is scrolled:
    // instead of scrolling or repainting we simply move the control window
    // itself: to be precise, offset it by the scroll increment to the left and
    // increment its width to still extend to the right boundary to compensate
    // for it (notice that dx is negative when scrolling to the right)
    SetSize(GetPosition().x + dx, -1, GetSize().x - dx, -1, wxSIZE_USE_EXISTING);
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

void wxHeaderCtrl::DoSetCount(unsigned int count)
{
    unsigned n;

    // first delete all old columns
    const unsigned countOld = DoGetCount();
    for ( n = 0; n < countOld; n++ )
    {
        if ( !Header_DeleteItem(GetHwnd(), 0) )
        {
            wxLogLastError(_T("Header_DeleteItem"));
        }
    }

    // and add the new ones
    for ( n = 0; n < count; n++ )
    {
        DoSetOrInsertItem(Insert, n);
    }
}

void wxHeaderCtrl::DoUpdate(unsigned int idx)
{
    DoSetOrInsertItem(Set, idx);
}

void wxHeaderCtrl::DoSetOrInsertItem(Operation oper, unsigned int idx)
{
    const wxHeaderColumnBase& col = GetColumn(idx);

    wxHDITEM hdi;

    // notice that we need to store the string we use the pointer to until we
    // pass it to the control
    wxWxCharBuffer buf;
    if ( !col.GetTitle().empty() )
    {
        hdi.mask |= HDI_TEXT;

        buf = col.GetTitle().wx_str();
        hdi.pszText = buf.data();
        hdi.cchTextMax = wxStrlen(buf);
    }

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

    if ( col.GetAlignment() != wxALIGN_NOT )
    {
        hdi.mask |= HDI_FORMAT;
        switch ( col.GetAlignment() )
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

    if ( col.IsSortKey() )
    {
        hdi.mask |= HDI_FORMAT;
        hdi.fmt |= col.IsSortOrderAscending() ? HDF_SORTUP : HDF_SORTDOWN;
    }

    if ( col.GetWidth() != wxCOL_WIDTH_DEFAULT || col.IsHidden() )
    {
        hdi.mask |= HDI_WIDTH;
        hdi.cxy = col.IsHidden() ? 0 : col.GetWidth();
    }

    const LRESULT rc = ::SendMessage(GetHwnd(),
                                     oper == Set ? HDM_SETITEM : HDM_INSERTITEM,
                                     idx,
                                     (LPARAM)&hdi);
    if ( oper == Set )
    {
        if ( !rc )
            wxLogLastError(_T("Header_SetItem()"));
    }
    else // Insert
    {
        if ( rc == -1 )
            wxLogLastError(_T("Header_InsertItem()"));
    }
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl events
// ----------------------------------------------------------------------------

wxEventType wxHeaderCtrl::GetClickEventType(bool dblclk, int button)
{
    wxEventType evtType;
    switch ( button )
    {
        case 0:
            evtType = dblclk ? wxEVT_COMMAND_HEADER_DCLICK
                             : wxEVT_COMMAND_HEADER_CLICK;
            break;

        case 1:
            evtType = dblclk ? wxEVT_COMMAND_HEADER_RIGHT_DCLICK
                             : wxEVT_COMMAND_HEADER_RIGHT_CLICK;
            break;

        case 2:
            evtType = dblclk ? wxEVT_COMMAND_HEADER_MIDDLE_DCLICK
                             : wxEVT_COMMAND_HEADER_MIDDLE_CLICK;
            break;

        default:
            wxFAIL_MSG( wxS("unexpected event type") );
            evtType = wxEVT_NULL;
    }

    return evtType;
}

bool wxHeaderCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHEADER * const nmhdr = (NMHEADER *)lParam;

    wxEventType evtType = wxEVT_NULL;
    int idx = nmhdr->iItem;
    int width = 0;
    bool cancelled = false;
    const UINT code = nmhdr->hdr.code;
    switch ( code )
    {
        // click events
        // ------------

        case HDN_ITEMCLICK:
        case HDN_ITEMDBLCLICK:
            evtType = GetClickEventType(code == HDN_ITEMDBLCLICK, nmhdr->iButton);
            break;

            // although we should get the notifications about the right clicks
            // via HDN_ITEM[DBL]CLICK too according to MSDN this simply doesn't
            // happen in practice on any Windows system up to 2003
        case NM_RCLICK:
        case NM_RDBLCLK:
            {
                POINT pt;
                idx = wxMSWGetColumnClicked(&nmhdr->hdr, &pt);
                if ( idx != wxNOT_FOUND )
                    evtType = GetClickEventType(code == NM_RDBLCLK, 1);
                //else: ignore clicks outside any column
            }
            break;

        case HDN_DIVIDERDBLCLICK:
            evtType = wxEVT_COMMAND_HEADER_SEPARATOR_DCLICK;
            break;


        // column resizing events
        // ----------------------

        // see comments in wxListCtrl::MSWOnNotify() for why we catch both
        // ASCII and Unicode versions of this message
        case HDN_BEGINTRACKA:
        case HDN_BEGINTRACKW:
            // non-resizeable columns can't be resized no matter what, don't
            // even generate any events for them
            if ( !GetColumn(idx).IsResizeable() )
            {
                *result = TRUE;

                return true;
            }

            evtType = wxEVT_COMMAND_HEADER_BEGIN_RESIZE;
            // fall through

        case HDN_TRACKA:
        case HDN_TRACKW:
            if ( evtType == wxEVT_NULL )
                evtType = wxEVT_COMMAND_HEADER_RESIZING;
            // fall through

        case HDN_ENDTRACKA:
        case HDN_ENDTRACKW:
            width = nmhdr->pitem->cxy;

            if ( evtType == wxEVT_NULL )
            {
                evtType = wxEVT_COMMAND_HEADER_END_RESIZE;

                // don't generate events with invalid width
                const int minWidth = GetColumn(idx).GetMinWidth();
                if ( width < minWidth )
                    width = minWidth;
            }
            break;

        case HDN_ITEMCHANGING:
            if ( nmhdr->pitem && (nmhdr->pitem->mask & HDI_WIDTH) )
            {
                // prevent the column from being shrunk beneath its min width
                if ( nmhdr->pitem->cxy < GetColumn(idx).GetMinWidth() )
                {
                    *result = TRUE;

                    return true;
                }
            }
            break;

        case NM_RELEASEDCAPTURE:
            cancelled = true;
            break;
    }


    // do generate the corresponding wx event
    if ( evtType != wxEVT_NULL )
    {
        wxHeaderCtrlEvent event(evtType, GetId());
        event.SetEventObject(this);
        event.SetColumn(idx);
        event.SetWidth(width);
        if ( cancelled )
            event.SetCancelled();

        if ( GetEventHandler()->ProcessEvent(event) )
        {
            if ( !event.IsAllowed() )
            {
                // all of HDN_BEGIN{DRAG,TRACK}, HDN_TRACK and HDN_ITEMCHANGING
                // interpret TRUE return value as meaning to stop the control
                // default handling of the message
                *result = TRUE;
            }

            return true;
        }
    }

    return wxHeaderCtrlBase::MSWOnNotify(idCtrl, lParam, result);
}

#endif // wxHAS_GENERIC_HEADERCTRL
