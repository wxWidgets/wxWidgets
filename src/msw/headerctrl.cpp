///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/headerctrl.cpp
// Purpose:     implementation of wxHeaderCtrl for wxMSW
// Author:      Vadim Zeitlin
// Created:     2008-12-01
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


#if wxUSE_HEADERCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/headerctrl.h"

#ifndef wxHAS_GENERIC_HEADERCTRL

#include "wx/imaglist.h"

#include "wx/msw/wrapcctl.h"
#include "wx/msw/private.h"
#include "wx/msw/private/customdraw.h"
#include "wx/msw/private/winstyle.h"

#ifndef HDM_SETBITMAPMARGIN
    #define HDM_SETBITMAPMARGIN 0x1234
#endif

#ifndef Header_SetBitmapMargin
    #define Header_SetBitmapMargin(hwnd, margin) \
            ::SendMessage((hwnd), HDM_SETBITMAPMARGIN, (WPARAM)(margin), 0)
#endif

// from src/msw/listctrl.cpp
extern int WXDLLIMPEXP_CORE wxMSWGetColumnClicked(NMHDR *nmhdr, POINT *ptClick);

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrlCustomDraw: our custom draw helper
// ----------------------------------------------------------------------------

class wxMSWHeaderCtrlCustomDraw : public wxMSWImpl::CustomDraw
{
public:
    wxMSWHeaderCtrlCustomDraw()
    {
    }

    // Make this field public to let wxHeaderCtrl update it directly when its
    // attributes change.
    wxItemAttr m_attr;

private:
    virtual bool HasCustomDrawnItems() const override
    {
        // We only exist if the header does need to be custom drawn.
        return true;
    }

    virtual const wxItemAttr*
    GetItemAttr(DWORD_PTR WXUNUSED(dwItemSpec)) const override
    {
        // We use the same attribute for all items for now.
        return &m_attr;
    }
};

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrl: the native header control
// ----------------------------------------------------------------------------
class wxMSWHeaderCtrl : public wxControl
{
public:
    explicit wxMSWHeaderCtrl(wxHeaderCtrl& header) :
        m_header(header)
    {
        Init();
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name);

    virtual ~wxMSWHeaderCtrl();

    // Override to implement colours support via custom drawing.
    virtual bool SetBackgroundColour(const wxColour& colour) override;
    virtual bool SetForegroundColour(const wxColour& colour) override;
    virtual bool SetFont(const wxFont& font) override;

    // The implementation of wxHeaderCtrlBase virtual functions
    void SetCount(unsigned int count);
    unsigned int GetCount() const;
    void UpdateHeader(unsigned int idx);

    void ScrollHorz(int dx);

    void SetColumnsOrder(const wxArrayInt& order);
    wxArrayInt GetColumnsOrder() const;

protected:
    // override wxWindow methods which must be implemented by a new control
    virtual wxSize DoGetBestSize() const override;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) override;
    virtual void MSWUpdateFontOnDPIChange(const wxSize& newDPI) override;

    // This function can be used as event handle for wxEVT_DPI_CHANGED event.
    void WXHandleDPIChanged(wxDPIChangedEvent& event);

private:
    // override MSW-specific methods needed for new control
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override;

    // common part of all ctors
    void Init();

    // wrapper around Header_InsertItem(): insert the item using information
    // from the given column at the given index
    void DoInsertItem(const wxHeaderColumn& col, unsigned int idx);

    // get the number of currently visible items: this is also the total number
    // of items contained in the native control
    int GetShownColumnsCount() const;

    // due to the discrepancy for the hidden columns which we know about but
    // the native control does not, there can be a difference between the
    // column indices we use and the ones used by the native control; these
    // functions translate between them
    //
    // notice that MSWToNativeIdx() shouldn't be called for hidden columns and
    // MSWFromNativeIdx() always returns an index of a visible column
    int MSWToNativeIdx(int idx);
    int MSWFromNativeIdx(int item);

    // this is the same as above but for order, not index
    int MSWToNativeOrder(int order);
    int MSWFromNativeOrder(int order);

    // get the event type corresponding to a click or double click event
    // (depending on dblclk value) with the specified (using MSW convention)
    // mouse button
    wxEventType GetClickEventType(bool dblclk, int button);

    // allocate m_customDraw if we need it or free it if it no longer is,
    // return the pointer which can be used to update it if it's non-null
    wxMSWHeaderCtrlCustomDraw* GetCustomDraw();


    // the real wxHeaderCtrl control
    wxHeaderCtrl &m_header;

    // the number of columns in the control, including the hidden ones (not
    // taken into account by the native control, see comment in DoGetCount())
    unsigned int m_numColumns;

    // this is a lookup table allowing us to check whether the column with the
    // given index is currently shown in the native control, in which case the
    // value of this array element with this index is 0, or hidden
    //
    // notice that this may be different from GetColumn(idx).IsHidden() and in
    // fact we need this array precisely because it will be different from it
    // in DoUpdate() when the column hidden flag gets toggled and we need it to
    // handle this transition correctly
    wxArrayInt m_isHidden;

    // the order of our columns: this array contains the index of the column
    // shown at the position n as the n-th element
    //
    // this is necessary only to handle the hidden columns: the native control
    // doesn't know about them and so we can't use Header_GetOrderArray()
    wxArrayInt m_colIndices;

    // the image list: initially nullptr, created on demand
    wxImageList *m_imageList;

    // the offset of the window used to emulate scrolling it
    int m_scrollOffset;

    // actual column we are dragging or -1 if not dragging anything
    int m_colBeingDragged;

    // a column is currently being resized
    bool m_isColBeingResized;

    // the custom draw helper: initially nullptr, created on demand, use
    // GetCustomDraw() to do it
    wxMSWHeaderCtrlCustomDraw *m_customDraw;
};

// ============================================================================
// wxMSWHeaderCtrl implementation
// ============================================================================

extern WXDLLIMPEXP_DATA_CORE(const char) wxMSWHeaderCtrlNameStr[] = "wxMSWHeaderCtrl";

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrl construction/destruction
// ----------------------------------------------------------------------------

void wxMSWHeaderCtrl::Init()
{
    m_numColumns = 0;
    m_imageList = nullptr;
    m_scrollOffset = 0;
    m_colBeingDragged = -1;
    m_isColBeingResized = false;
    m_customDraw = nullptr;

    Bind(wxEVT_DPI_CHANGED, &wxMSWHeaderCtrl::WXHandleDPIChanged, this);
}

bool wxMSWHeaderCtrl::Create(wxWindow *parent,
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

    if ( !MSWCreateControl(WC_HEADER, wxT(""), pos, size) )
        return false;

    // special hack for margins when using comctl32.dll v6 or later: the
    // default margin is too big and results in label truncation when the
    // column width is just about right to show it together with the sort
    // indicator, so reduce it to a smaller value (in principle we could even
    // use 0 here but this starts to look ugly)
    if ( wxApp::GetComCtl32Version() >= 600 )
    {
        (void)Header_SetBitmapMargin(GetHwnd(), wxGetSystemMetrics(SM_CXEDGE, parent));
    }

    return true;
}

WXDWORD wxMSWHeaderCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    if ( style & wxHD_ALLOW_REORDER )
        msStyle |= HDS_DRAGDROP;

    // the control looks nicer with these styles and there doesn't seem to be
    // any reason to not use them so we always do (as for HDS_HORZ it is 0
    // anyhow but include it for clarity)
    // NOTE: don't use however HDS_FLAT because it makes the control look
    //       non-native when running WinXP in classic mode
    msStyle |= HDS_HORZ | HDS_BUTTONS | HDS_FULLDRAG | HDS_HOTTRACK;

    return msStyle;
}

wxMSWHeaderCtrl::~wxMSWHeaderCtrl()
{
    delete m_imageList;
    delete m_customDraw;
}

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrl scrolling
// ----------------------------------------------------------------------------

void wxMSWHeaderCtrl::DoSetSize(int x, int y,
                                int w, int h,
                                int sizeFlags)
{
    wxControl::DoSetSize(x + m_scrollOffset, y, w - m_scrollOffset, h,
                         sizeFlags & wxSIZE_FORCE);
}

void wxMSWHeaderCtrl::ScrollHorz(int dx)
{
    // as the native control doesn't support offsetting its contents, we use a
    // hack here to make it appear correctly when the parent is scrolled:
    // instead of scrolling or repainting we simply move the control window
    // itself: to be precise, offset it by the scroll increment to the left and
    // increment its width to still extend to the right boundary to compensate
    // for it (notice that dx is negative when scrolling to the right)
    m_scrollOffset += dx;

    wxControl::DoSetSize(GetPosition().x + dx, -1,
                         GetSize().x - dx, -1,
                         wxSIZE_USE_EXISTING);
}

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrl geometry calculation
// ----------------------------------------------------------------------------

wxSize wxMSWHeaderCtrl::DoGetBestSize() const
{
    RECT rc = wxGetClientRect(GetHwndOf(m_header.GetParent()));
    WINDOWPOS wpos;
    HDLAYOUT layout = { &rc, &wpos };
    if ( !Header_Layout(GetHwnd(), &layout) )
    {
        wxLogLastError(wxT("Header_Layout"));
        return wxControl::DoGetBestSize();
    }

    return wxSize(wxDefaultCoord, wpos.cy);
}

void wxMSWHeaderCtrl::MSWUpdateFontOnDPIChange(const wxSize& newDPI)
{
    wxControl::MSWUpdateFontOnDPIChange(newDPI);

    if ( wxMSWHeaderCtrlCustomDraw * customDraw = GetCustomDraw() )
    {
        customDraw->m_attr.SetFont(m_font);
    }
}

void wxMSWHeaderCtrl::WXHandleDPIChanged(wxDPIChangedEvent& event)
{
    delete m_imageList;
    m_imageList = nullptr;
    for (unsigned int i = 0; i < m_numColumns; ++i)
    {
        UpdateHeader(i);
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrl columns managements
// ----------------------------------------------------------------------------

unsigned int wxMSWHeaderCtrl::GetCount() const
{
    // we can't use Header_GetItemCount() here because it doesn't take the
    // hidden columns into account and we can't find the hidden columns after
    // the last shown one in MSWFromNativeIdx() without knowing where to stop
    // so we have to store the columns count internally
    return m_numColumns;
}

int wxMSWHeaderCtrl::GetShownColumnsCount() const
{
    const int numItems = Header_GetItemCount(GetHwnd());

    wxASSERT_MSG( numItems >= 0 && (unsigned)numItems <= m_numColumns,
                  "unexpected number of items in the native control" );

    return numItems;
}

void wxMSWHeaderCtrl::SetCount(unsigned int count)
{
    unsigned n;

    // first delete all old columns
    const unsigned countOld = GetShownColumnsCount();
    for ( n = 0; n < countOld; n++ )
    {
        if ( !Header_DeleteItem(GetHwnd(), 0) )
        {
            wxLogLastError(wxT("Header_DeleteItem"));
        }
    }

    // update the column indices order array before changing m_numColumns
    m_header.DoResizeColumnIndices(m_colIndices, count);

    // and add the new ones
    m_numColumns = count;
    m_isHidden.resize(m_numColumns);
    for ( n = 0; n < count; n++ )
    {
        const wxHeaderColumn& col = m_header.GetColumn(n);
        if ( col.IsShown() )
        {
            m_isHidden[n] = false;

            DoInsertItem(col, n);
        }
        else // hidden initially
        {
            m_isHidden[n] = true;
        }
    }
}

void wxMSWHeaderCtrl::UpdateHeader(unsigned int idx)
{
    // the native control does provide Header_SetItem() but it's inconvenient
    // to use it because it sends HDN_ITEMCHANGING messages and we'd have to
    // arrange not to block setting the width from there and the logic would be
    // more complicated as we'd have to reset the old values as well as setting
    // the new ones -- so instead just recreate the column

    const wxHeaderColumn& col = m_header.GetColumn(idx);
    if ( col.IsHidden() )
    {
        // column is hidden now
        if ( !m_isHidden[idx] )
        {
            // but it wasn't hidden before, so remove it
            if ( !Header_DeleteItem(GetHwnd(), MSWToNativeIdx(idx)) )
                wxLogLastError(wxS("Header_DeleteItem()"));

            m_isHidden[idx] = true;
        }
        //else: nothing to do, updating hidden column doesn't have any effect
    }
    else // column is shown now
    {
        if ( m_isHidden[idx] )
        {
            m_isHidden[idx] = false;
        }
        else // and it was shown before as well
        {
            // we need to remove the old column
            if ( !Header_DeleteItem(GetHwnd(), MSWToNativeIdx(idx)) )
                wxLogLastError(wxS("Header_DeleteItem()"));
        }

        DoInsertItem(col, idx);
    }
}

void wxMSWHeaderCtrl::DoInsertItem(const wxHeaderColumn& col, unsigned int idx)
{
    wxASSERT_MSG( !col.IsHidden(), "should only be called for shown columns" );

    wxHDITEM hdi;

    // notice that we need to store the string we use the pointer to until we
    // pass it to the control
    hdi.mask |= HDI_TEXT;
    wxWxCharBuffer buf = col.GetTitle().t_str();
    hdi.pszText = buf.data();
    hdi.cchTextMax = wxStrlen(buf);

    const wxBitmapBundle& bb = col.GetBitmapBundle();
    if ( bb.IsOk() )
    {
        hdi.mask |= HDI_IMAGE;

        if ( HasFlag(wxHD_BITMAP_ON_RIGHT) )
            hdi.fmt |= HDF_BITMAP_ON_RIGHT;

        wxSize bmpSize;
        if ( !m_imageList )
        {
            bmpSize = bb.GetPreferredBitmapSizeFor(this);
            m_imageList = new wxImageList(bmpSize.x, bmpSize.y);
            (void) // suppress mingw32 warning about unused computed value
            Header_SetImageList(GetHwnd(), GetHimagelistOf(m_imageList));
        }
        else // already have an image list
        {
            // use the same size for all bitmaps
            bmpSize = m_imageList->GetSize();
        }

        m_imageList->Add(bb.GetBitmap(bmpSize));
        hdi.iImage = m_imageList->GetImageCount() - 1;
    }

    if ( col.GetAlignment() != wxALIGN_NOT )
    {
        hdi.mask |= HDI_FORMAT;

        // wxALIGN_LEFT is the same as wxALIGN_NOT
        switch ( col.GetAlignment() )
        {
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

    if ( col.GetWidth() != wxCOL_WIDTH_DEFAULT )
    {
        hdi.mask |= HDI_WIDTH;
        hdi.cxy = col.GetWidth();
    }

    hdi.mask |= HDI_ORDER;
    hdi.iOrder = MSWToNativeOrder(m_colIndices.Index(idx));

    if ( ::SendMessage(GetHwnd(), HDM_INSERTITEM,
                       MSWToNativeIdx(idx), (LPARAM)&hdi) == -1 )
    {
        wxLogLastError(wxT("Header_InsertItem()"));
    }

    // Resizing cursor that correctly reflects per-column IsResizable() cannot
    // be implemented, it is per-control rather than per-column in the native
    // control. Enable resizing cursor if at least one column is resizeble.
    bool hasResizableColumns = false;
    for ( unsigned n = 0; n < m_header.GetColumnCount(); n++ )
    {
        const wxHeaderColumn& c = m_header.GetColumn(n);
        if (c.IsShown() && c.IsResizeable())
        {
            hasResizableColumns = true;
            break;
        }
    }

    wxMSWWinStyleUpdater(GetHwnd())
        .TurnOnOrOff(!hasResizableColumns, HDS_NOSIZING);
}

void wxMSWHeaderCtrl::SetColumnsOrder(const wxArrayInt& order)
{
    // This can happen if we don't have any columns at all and "order" is empty
    // anyhow in this case, so we don't have anything to do (note that we
    // already know that the input array contains m_numColumns elements, as
    // it's checked by the public SetColumnsOrder()).
    if ( !m_numColumns )
        return;

    wxArrayInt orderShown;
    orderShown.reserve(m_numColumns);

    for ( unsigned n = 0; n < m_numColumns; n++ )
    {
        const int idx = order[n];
        if ( m_header.GetColumn(idx).IsShown() )
            orderShown.push_back(MSWToNativeIdx(idx));
    }

    if ( !Header_SetOrderArray(GetHwnd(), orderShown.size(), &orderShown[0]) )
    {
        wxLogLastError(wxT("Header_GetOrderArray"));
    }

    m_colIndices = order;
}

wxArrayInt wxMSWHeaderCtrl::GetColumnsOrder() const
{
    // we don't use Header_GetOrderArray() here because it doesn't return
    // information about the hidden columns, instead we just save the columns
    // order array in DoSetColumnsOrder() and update it when they're reordered
    return m_colIndices;
}

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrl indexes and positions translation
// ----------------------------------------------------------------------------

int wxMSWHeaderCtrl::MSWToNativeIdx(int idx)
{
    // don't check for GetColumn(idx).IsShown() as it could have just became
    // false and we may be called from DoUpdate() to delete the old column
    wxASSERT_MSG( !m_isHidden[idx],
                  "column must be visible to have an "
                  "index in the native control" );

    int item = idx;
    for ( int i = 0; i < idx; i++ )
    {
        if ( m_header.GetColumn(i).IsHidden() )
            item--; // one less column the native control knows about
    }

    wxASSERT_MSG( item >= 0 && item <= GetShownColumnsCount(), "logic error" );

    return item;
}

int wxMSWHeaderCtrl::MSWFromNativeIdx(int item)
{
    wxASSERT_MSG( item >= 0 && item < GetShownColumnsCount(),
                  "column index out of range" );

    // reverse the above function

    unsigned idx = item;
    for ( unsigned n = 0; n < m_numColumns; n++ )
    {
        if ( n > idx )
            break;

        if ( m_header.GetColumn(n).IsHidden() )
            idx++;
    }

    wxASSERT_MSG( MSWToNativeIdx(idx) == item, "logic error" );

    return idx;
}

int wxMSWHeaderCtrl::MSWToNativeOrder(int pos)
{
    wxASSERT_MSG( pos >= 0 && static_cast<unsigned>(pos) < m_numColumns,
                  "column position out of range" );

    int order = pos;
    for ( int n = 0; n < pos; n++ )
    {
        if ( m_header.GetColumn(m_colIndices[n]).IsHidden() )
            order--;
    }

    wxASSERT_MSG( order >= 0 && order <= GetShownColumnsCount(), "logic error" );

    return order;
}

int wxMSWHeaderCtrl::MSWFromNativeOrder(int order)
{
    wxASSERT_MSG( order >= 0 && order < GetShownColumnsCount(),
                  "native column position out of range" );

    unsigned pos = order;
    for ( unsigned n = 0; n < m_numColumns; n++ )
    {
        if ( n > pos )
            break;

        if ( m_header.GetColumn(m_colIndices[n]).IsHidden() )
            pos++;
    }

    wxASSERT_MSG( MSWToNativeOrder(pos) == order, "logic error" );

    return pos;
}

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrl appearance
// ----------------------------------------------------------------------------

wxMSWHeaderCtrlCustomDraw* wxMSWHeaderCtrl::GetCustomDraw()
{
    // There is no need to make the control custom drawn just because it has a
    // custom font, the native control handles the font just fine on its own,
    // so if our custom colours were reset, don't bother with custom drawing
    // any longer.
    if ( !m_hasBgCol && !m_hasFgCol )
    {
        if ( m_customDraw )
        {
            delete m_customDraw;
            m_customDraw = nullptr;
        }

        return nullptr;
    }

    // We do have at least one custom colour, so enable custom drawing.
    if ( !m_customDraw )
        m_customDraw = new wxMSWHeaderCtrlCustomDraw();

    return m_customDraw;
}

bool wxMSWHeaderCtrl::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
        return false;

    if ( wxMSWHeaderCtrlCustomDraw* customDraw = GetCustomDraw() )
    {
        customDraw->m_attr.SetBackgroundColour(colour);
    }

    return true;
}

bool wxMSWHeaderCtrl::SetForegroundColour(const wxColour& colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
        return false;

    if ( wxMSWHeaderCtrlCustomDraw* customDraw = GetCustomDraw() )
    {
        customDraw->m_attr.SetTextColour(colour);
    }

    return true;
}

bool wxMSWHeaderCtrl::SetFont(const wxFont& font)
{
    if ( !wxControl::SetFont(font) )
        return false;

    if ( wxMSWHeaderCtrlCustomDraw* customDraw = GetCustomDraw() )
    {
        customDraw->m_attr.SetFont(m_font);
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxMSWHeaderCtrl events
// ----------------------------------------------------------------------------

wxEventType wxMSWHeaderCtrl::GetClickEventType(bool dblclk, int button)
{
    wxEventType evtType;
    switch ( button )
    {
        case 0:
            evtType = dblclk ? wxEVT_HEADER_DCLICK
                             : wxEVT_HEADER_CLICK;
            break;

        case 1:
            evtType = dblclk ? wxEVT_HEADER_RIGHT_DCLICK
                             : wxEVT_HEADER_RIGHT_CLICK;
            break;

        case 2:
            evtType = dblclk ? wxEVT_HEADER_MIDDLE_DCLICK
                             : wxEVT_HEADER_MIDDLE_CLICK;
            break;

        default:
            wxFAIL_MSG( wxS("unexpected event type") );
            evtType = wxEVT_NULL;
    }

    return evtType;
}

bool wxMSWHeaderCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHEADER * const nmhdr = (NMHEADER *)lParam;

    wxEventType evtType = wxEVT_NULL;
    int width = 0;
    int order = -1;
    bool veto = false;
    const UINT code = nmhdr->hdr.code;

    // we don't have the index for all events, e.g. not for NM_RELEASEDCAPTURE
    // so only access for header control events (and yes, the direction of
    // comparisons with FIRST/LAST is correct even if it seems inverted)
    int idx = code <= HDN_FIRST && code > HDN_LAST ? nmhdr->iItem : -1;
    if ( idx != -1 )
    {
        // we also get bogus HDN_BEGINDRAG with -1 index so don't call
        // MSWFromNativeIdx() unconditionally for nmhdr->iItem
        idx = MSWFromNativeIdx(idx);
    }

    switch ( code )
    {
        // click events
        // ------------

        case HDN_ITEMCLICK:
        case HDN_ITEMDBLCLICK:
            evtType = GetClickEventType(code == HDN_ITEMDBLCLICK, nmhdr->iButton);

            // We're not dragging any more.
            m_colBeingDragged = -1;
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
                {
                    idx = MSWFromNativeIdx(idx);

                    // due to a bug in mingw32 headers NM_RDBLCLK is signed
                    // there so we need a cast to avoid warnings about signed/
                    // unsigned comparison
                    evtType = GetClickEventType(
                                code == static_cast<UINT>(NM_RDBLCLK), 1);
                }
                //else: ignore clicks outside any column
            }
            break;

        case HDN_DIVIDERDBLCLICK:
            evtType = wxEVT_HEADER_SEPARATOR_DCLICK;
            break;


        // column resizing events
        // ----------------------

        // see comments in wxListCtrl::MSWOnNotify() for why we catch both
        // ASCII and Unicode versions of this message
        case HDN_BEGINTRACKA:
        case HDN_BEGINTRACKW:
            // non-resizable columns can't be resized no matter what, don't
            // even generate any events for them
            if ( !m_header.GetColumn(idx).IsResizeable() )
            {
                veto = true;
                break;
            }

            m_isColBeingResized = true;
            evtType = wxEVT_HEADER_BEGIN_RESIZE;
            wxFALLTHROUGH;

        case HDN_ENDTRACKA:
        case HDN_ENDTRACKW:
            width = nmhdr->pitem->cxy;

            if ( evtType == wxEVT_NULL )
            {
                evtType = wxEVT_HEADER_END_RESIZE;

                // don't generate events with invalid width
                const int minWidth = m_header.GetColumn(idx).GetMinWidth();
                if ( width < minWidth )
                    width = minWidth;

                m_isColBeingResized = false;
            }
            break;

            // The control is not supposed to send HDN_TRACK when using
            // HDS_FULLDRAG (which we do use) but apparently some versions of
            // comctl32.dll still do it, see #13506, so catch both messages
            // just in case we are dealing with one of these buggy versions.
        case HDN_TRACK:
        case HDN_ITEMCHANGING:
            // With "Show window contents while dragging" option enabled
            // the sequence of notifications is as follows:
            //   HDN_BEGINTRACK
            //   HDN_ITEMCHANGING
            //   HDN_ITEMCHANGED
            //   ...
            //   HDN_ITEMCHANGING
            //   HDN_ITEMCHANGED
            //   HDN_ENDTRACK
            //   HDN_ITEMCHANGING
            //   HDN_ITEMCHANGED
            // With "Show window contents while dragging" option disabled
            // the sequence looks in turn like this:
            //   HDN_BEGINTRACK
            //   HDN_ITEMTRACK
            //   HDN_ITEMCHANGING
            //   ...
            //   HDN_ITEMTRACK
            //   HDN_ITEMCHANGING
            //   HDN_ENDTRACK
            //   HDN_ITEMCHANGING
            //   HDN_ITEMCHANGED
            // In both cases last HDN_ITEMCHANGING notification is sent
            // after HDN_ENDTRACK so we have to skip it.
            if ( nmhdr->pitem && (nmhdr->pitem->mask & HDI_WIDTH) )
            {
                // prevent the column from being shrunk beneath its min width
                width = nmhdr->pitem->cxy;
                if ( width < m_header.GetColumn(idx).GetMinWidth() )
                {
                    // don't generate any events and prevent the change from
                    // happening
                    veto = true;
                }
                // width is acceptable and notification arrived before HDN_ENDTRACK
                else if ( m_isColBeingResized )
                {
                    // generate the resizing event from here as we don't seem
                    // to be getting HDN_TRACK events at all, at least with
                    // comctl32.dll v6
                    evtType = wxEVT_HEADER_RESIZING;
                }
                // else
                // Notification arriving after HDN_ENDTRACK is handled normally
                // by the control but EVT_HEADER_RESIZING event cannot be generated
                // because EVT_HEADER_END_RESIZE finalizing the resizing has been
                // already emitted.
            }
            break;


        // column reordering events
        // ------------------------

        case HDN_BEGINDRAG:
            // Windows sometimes sends us events with invalid column indices
            if ( nmhdr->iItem == -1 )
                break;

            // If we are dragging a column that is not draggable and the mouse
            // is moved over a different column then we get the column number from
            // the column under the mouse. This results in an unexpected behaviour
            // if this column is draggable. To prevent this remember the column we
            // are dragging for the complete drag and drop cycle.
            if ( m_colBeingDragged == -1 )
            {
                m_colBeingDragged = idx;
            }

            // column must have the appropriate flag to be draggable
            if ( !m_header.GetColumn(m_colBeingDragged).IsReorderable() )
            {
                veto = true;
                break;
            }

            evtType = wxEVT_HEADER_BEGIN_REORDER;
            break;

        case HDN_ENDDRAG:
            wxASSERT_MSG( nmhdr->pitem->mask & HDI_ORDER, "should have order" );
            order = nmhdr->pitem->iOrder;

            // we also get messages with invalid order when column reordering
            // is cancelled (e.g. by pressing Esc)
            if ( order == -1 )
                break;

            order = MSWFromNativeOrder(order);

            evtType = wxEVT_HEADER_END_REORDER;

            // We (successfully) ended dragging the column.
            m_colBeingDragged = -1;
            break;

        case NM_RELEASEDCAPTURE:
            evtType = wxEVT_HEADER_DRAGGING_CANCELLED;

            // Dragging the column was cancelled.
            m_colBeingDragged = -1;
            break;

        // other events
        // ------------

        case NM_CUSTOMDRAW:
            if ( m_customDraw )
            {
                *result = m_customDraw->HandleCustomDraw(lParam);
                if ( *result != CDRF_DODEFAULT )
                    return true;
            }
            break;
    }


    // do generate the corresponding wx event
    if ( evtType != wxEVT_NULL )
    {
        wxHeaderCtrlEvent event(evtType, GetId());
        event.SetEventObject(this);
        event.SetColumn(idx);
        event.SetWidth(width);
        if ( order != -1 )
            event.SetNewOrder(order);

        const bool processed = m_header.GetEventHandler()->ProcessEvent(event);

        if ( processed && !event.IsAllowed() )
            veto = true;

        if ( !veto )
        {
            // special post-processing for HDN_ENDDRAG: we need to update the
            // internal column indices array if this is allowed to go ahead as
            // the native control is going to reorder its columns now
            if ( evtType == wxEVT_HEADER_END_REORDER )
                m_header.MoveColumnInOrderArray(m_colIndices, idx, order);

            if ( processed )
            {
                // skip default processing below
                return true;
            }
        }
    }

    if ( veto )
    {
        // all of HDN_BEGIN{DRAG,TRACK}, HDN_TRACK and HDN_ITEMCHANGING
        // interpret TRUE return value as meaning to stop the control
        // default handling of the message
        *result = TRUE;

        return true;
    }

    return wxControl::MSWOnNotify(idCtrl, lParam, result);
}

// ============================================================================
// wxHeaderCtrl implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxHeaderCtrl construction/destruction
// ----------------------------------------------------------------------------

void wxHeaderCtrl::Init()
{
    m_nativeControl = nullptr;
}

bool wxHeaderCtrl::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    long newStyle = style | wxCLIP_CHILDREN | wxTAB_TRAVERSAL;
    if ( !wxWindow::Create(parent, id, pos, size, newStyle, name) )
        return false;

    m_nativeControl = new wxMSWHeaderCtrl(*this);
    if ( !m_nativeControl->Create(this,
                                  wxID_ANY,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxNO_BORDER,
                                  wxMSWHeaderCtrlNameStr) )
        return false;

    SetWindowStyle(newStyle);

    Bind(wxEVT_SIZE, &wxHeaderCtrl::OnSize, this);

    return true;
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl events
// ----------------------------------------------------------------------------

void wxHeaderCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if (m_nativeControl != nullptr) // check whether initialisation has been done
    {
        int cw, ch;
        GetClientSize(&cw, &ch);

        m_nativeControl->SetSize(0, 0, cw, ch);
    }
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl scrolling
// ----------------------------------------------------------------------------

void wxHeaderCtrl::DoScrollHorz(int dx)
{
    m_nativeControl->ScrollHorz(dx);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl geometry calculation
// ----------------------------------------------------------------------------

wxSize wxHeaderCtrl::DoGetBestSize() const
{
    return m_nativeControl->GetBestSize();
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl columns managements
// ----------------------------------------------------------------------------

unsigned int wxHeaderCtrl::DoGetCount() const
{
    return m_nativeControl->GetCount();
}

void wxHeaderCtrl::DoSetCount(unsigned int count)
{
    m_nativeControl->SetCount(count);
}

void wxHeaderCtrl::DoUpdate(unsigned int idx)
{
    m_nativeControl->UpdateHeader(idx);
}

void wxHeaderCtrl::DoSetColumnsOrder(const wxArrayInt& order)
{
    m_nativeControl->SetColumnsOrder(order);
}

wxArrayInt wxHeaderCtrl::DoGetColumnsOrder() const
{
    return m_nativeControl->GetColumnsOrder();
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl composite window
// ----------------------------------------------------------------------------

wxWindowList wxHeaderCtrl::GetCompositeWindowParts() const
{
    wxWindowList parts;
    parts.push_back(m_nativeControl);
    return parts;
}

void wxHeaderCtrl::SetWindowStyleFlag(long style)
{
    wxHeaderCtrlBase::SetWindowStyleFlag(style);

    // Update the native control style.
    long flags = m_nativeControl->GetWindowStyleFlag();

    if ( HasFlag(wxHD_ALLOW_REORDER) )
        flags |= wxHD_ALLOW_REORDER;
    else
        flags &= ~wxHD_ALLOW_REORDER;

    if ( HasFlag(wxHD_BITMAP_ON_RIGHT) )
        flags |= wxHD_BITMAP_ON_RIGHT;
    else
        flags &= ~wxHD_BITMAP_ON_RIGHT;

    m_nativeControl->SetWindowStyleFlag(flags);
}

#endif // wxHAS_GENERIC_HEADERCTRL

#endif // wxUSE_HEADERCTRL
