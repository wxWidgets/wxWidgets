/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/listctrl.cpp
// Purpose:     generic implementation of wxListCtrl
// Author:      Robert Roebling
//              Vadim Zeitlin (virtual list control support)
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// TODO
//
//   1. we need to implement searching/sorting for virtual controls somehow
//   2. when changing selection the lines are refreshed twice


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_LISTCTRL

#include "wx/listctrl.h"

#ifndef WX_PRECOMP
    #include "wx/scrolwin.h"
    #include "wx/timer.h"
    #include "wx/settings.h"
    #include "wx/dynarray.h"
    #include "wx/dcclient.h"
    #include "wx/dcscreen.h"
    #include "wx/math.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
#endif

#include "wx/imaglist.h"
#include "wx/renderer.h"
#include "wx/weakref.h"
#include "wx/scopeguard.h"

#include "wx/generic/private/drawbitmap.h"
#include "wx/generic/private/listctrl.h"
#include "wx/generic/private/widthcalc.h"

#include "wx/private/access.h"
#if defined(__WXWINUI__) && wxUSE_WINUI3
#include "../winui/listprojection.h"
#include "wx/winui/private/themecolours.h"
#endif

#ifdef __WXMAC__
    #include "wx/osx/private.h"
#endif

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/wrapwin.h"
#endif

// NOTE: If using the wxListBox visual attributes works everywhere then this can
// be removed, as well as the #else case below.
#define _USE_VISATTR 0


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int SCROLL_UNIT_X = 15;

// the spacing between the lines (in report mode)
static const int LINE_SPACING = 0;

// extra margins around the text label
static const int EXTRA_WIDTH = 6;
static const int EXTRA_HEIGHT = 6;

// margin between the window and the items
static const int EXTRA_BORDER_X = 2;
static const int EXTRA_BORDER_Y = 2;

static const int ICON_OFFSET_X = 2;

// offset for the header window
static const int HEADER_OFFSET_X = 0;
static const int HEADER_OFFSET_Y = 0;

// margin between rows of icons in [small] icon view
static const int MARGIN_BETWEEN_ROWS = 6;

// when autosizing the columns, add some slack
static const int AUTOSIZE_COL_MARGIN = 10;

// the space between the image and the text in the report mode
static const int IMAGE_MARGIN_IN_REPORT_MODE = 5;

// the space between the image and the text in the report mode in header
static const int HEADER_IMAGE_MARGIN_IN_REPORT_MODE = 2;

// space after a checkbox
static const int MARGIN_AROUND_CHECKBOX = 5;

// ----------------------------------------------------------------------------
// wxListItemData
// ----------------------------------------------------------------------------

wxListItemData::wxListItemData(wxListItemData&& other)
              : m_image(other.m_image),
                m_data(other.m_data),
                m_owner(other.m_owner),
                m_text(std::move(other.m_text))
{
    // Take ownership of the pointers from the other object and reset them.
    std::swap(m_attr, other.m_attr);
    std::swap(m_rect, other.m_rect);
}

wxListItemData& wxListItemData::operator=(wxListItemData&& other)
{
    m_image = other.m_image;
    m_data = other.m_data;
    m_owner = other.m_owner;
    m_text = std::move(other.m_text);

    // Swap them to let our pointers be deleted by the other object if necessary.
    std::swap(m_attr, other.m_attr);
    std::swap(m_rect, other.m_rect);

    return *this;
}

wxListItemData::~wxListItemData()
{
    // in the virtual list control the attributes are managed by the main
    // program, so don't delete them
    if ( !m_owner->IsVirtual() )
        delete m_attr;

    delete m_rect;
}

wxListItemData::wxListItemData(wxListMainWindow *owner)
{
    m_owner = owner;

    if ( !owner->InReportView() )
        m_rect = new wxRect;
}

// Check if the item is visible
bool wxGenericListCtrl::IsVisible(long item) const
{
    wxRect itemRect;
    GetItemRect( item, itemRect );
    const wxRect clientRect = GetClientRect();
    bool visible = clientRect.Intersects( itemRect );
    if ( visible && m_headerWin )
    {
        const wxRect headerRect = m_headerWin->GetClientRect();
        visible = itemRect.GetBottom() > headerRect.GetBottom();
    }
    return visible;
}

void wxListItemData::SetItem( const wxListItem &info )
{
    if ( info.m_mask & wxLIST_MASK_TEXT )
        SetText(info.m_text);
    if ( info.m_mask & wxLIST_MASK_IMAGE )
        m_image = info.m_image;
    if ( info.m_mask & wxLIST_MASK_DATA )
        m_data = info.m_data;

    if ( info.HasAttributes() )
    {
        if ( m_attr )
            m_attr->AssignFrom(*info.GetAttributes());
        else
            m_attr = new wxItemAttr(*info.GetAttributes());
    }

    if ( m_rect )
    {
        m_rect->x =
        m_rect->y =
        m_rect->height = 0;
        m_rect->width = info.m_width;
    }
}

void wxListItemData::SetPosition( int x, int y )
{
    wxCHECK_RET( m_rect, wxT("unexpected SetPosition() call") );

    m_rect->x = x;
    m_rect->y = y;
}

void wxListItemData::SetSize( int width, int height )
{
    wxCHECK_RET( m_rect, wxT("unexpected SetSize() call") );

    if ( width != -1 )
        m_rect->width = width;
    if ( height != -1 )
        m_rect->height = height;
}

bool wxListItemData::IsHit( int x, int y ) const
{
    wxCHECK_MSG( m_rect, false, wxT("can't be called in this mode") );

    return wxRect(GetX(), GetY(), GetWidth(), GetHeight()).Contains(x, y);
}

int wxListItemData::GetX() const
{
    wxCHECK_MSG( m_rect, 0, wxT("can't be called in this mode") );

    return m_rect->x;
}

int wxListItemData::GetY() const
{
    wxCHECK_MSG( m_rect, 0, wxT("can't be called in this mode") );

    return m_rect->y;
}

int wxListItemData::GetWidth() const
{
    wxCHECK_MSG( m_rect, 0, wxT("can't be called in this mode") );

    return m_rect->width;
}

int wxListItemData::GetHeight() const
{
    wxCHECK_MSG( m_rect, 0, wxT("can't be called in this mode") );

    return m_rect->height;
}

void wxListItemData::GetItem( wxListItem &info ) const
{
    long mask = info.m_mask;
    if ( !mask )
        // by default, get everything for backwards compatibility
        mask = -1;

    if ( mask & wxLIST_MASK_TEXT )
        info.m_text = m_text;
    if ( mask & wxLIST_MASK_IMAGE )
        info.m_image = m_image;
    if ( mask & wxLIST_MASK_DATA )
        info.m_data = m_data;

    if ( m_attr )
    {
        if ( m_attr->HasTextColour() )
            info.SetTextColour(m_attr->GetTextColour());
        if ( m_attr->HasBackgroundColour() )
            info.SetBackgroundColour(m_attr->GetBackgroundColour());
        if ( m_attr->HasFont() )
            info.SetFont(m_attr->GetFont());
    }
}

//-----------------------------------------------------------------------------
//  wxListHeaderData
//-----------------------------------------------------------------------------

void wxListHeaderData::Init()
{
    m_mask = 0;
    m_image = -1;
    m_format = 0;
    m_width = 0;
    m_xpos = 0;
    m_ypos = 0;
    m_height = 0;
    m_state = 0;
}

wxListHeaderData::wxListHeaderData()
{
    Init();
}

wxListHeaderData::wxListHeaderData( const wxListItem &item )
{
    Init();

    SetItem( item );

    // Always give some initial width to the new columns (it's still possible
    // to set the width to 0 explicitly, however).
    if ( !(m_mask & wxLIST_MASK_WIDTH) )
        SetWidth(wxLIST_DEFAULT_COL_WIDTH);
}

void wxListHeaderData::SetItem( const wxListItem &item )
{
    m_mask = item.m_mask;

    if ( m_mask & wxLIST_MASK_TEXT )
        m_text = item.m_text;

    if ( m_mask & wxLIST_MASK_IMAGE )
        m_image = item.m_image;

    if ( m_mask & wxLIST_MASK_FORMAT )
        m_format = item.m_format;

    if ( m_mask & wxLIST_MASK_WIDTH )
        SetWidth(item.m_width);

    if ( m_mask & wxLIST_MASK_STATE )
        SetState(item.m_state);
}

void wxListHeaderData::SetPosition( int x, int y )
{
    m_xpos = x;
    m_ypos = y;
}

void wxListHeaderData::SetHeight( int h )
{
    m_height = h;
}

void wxListHeaderData::SetWidth( int w )
{
    m_width = w < 0 ? wxLIST_DEFAULT_COL_WIDTH : w;
}

void wxListHeaderData::SetState( int flag )
{
    m_state = flag;
}

void wxListHeaderData::SetFormat( int format )
{
    m_format = format;
}

bool wxListHeaderData::HasImage() const
{
    return m_image != -1;
}

bool wxListHeaderData::IsHit( int x, int y ) const
{
    return ((x >= m_xpos) && (x <= m_xpos+m_width) && (y >= m_ypos) && (y <= m_ypos+m_height));
}

void wxListHeaderData::GetItem( wxListItem& item ) const
{
    long mask = item.m_mask;
    if ( !mask )
    {
        // by default, get everything for backwards compatibility
        mask = -1;
    }

    if ( mask & wxLIST_MASK_STATE )
        item.m_state = m_state;
    if ( mask & wxLIST_MASK_TEXT )
        item.m_text = m_text;
    if ( mask & wxLIST_MASK_IMAGE )
        item.m_image = m_image;
    if ( mask & wxLIST_MASK_WIDTH )
        item.m_width = m_width;
    if ( mask & wxLIST_MASK_FORMAT )
        item.m_format = m_format;
}

int wxListHeaderData::GetImage() const
{
    return m_image;
}

int wxListHeaderData::GetWidth() const
{
    return m_width;
}

int wxListHeaderData::GetFormat() const
{
    return m_format;
}

int wxListHeaderData::GetState() const
{
    return m_state;
}

//-----------------------------------------------------------------------------
//  wxListLineData
//-----------------------------------------------------------------------------

inline int wxListLineData::GetMode() const
{
    return m_owner->GetListCtrl()->GetWindowStyleFlag() & wxLC_MASK_TYPE;
}

inline bool wxListLineData::InReportView() const
{
    return m_owner->HasFlag(wxLC_REPORT);
}

inline bool wxListLineData::IsVirtual() const
{
    return m_owner->IsVirtual();
}

wxListLineData::wxListLineData( wxListMainWindow *owner )
{
    m_owner = owner;

    SetReportView(InReportView());

    m_highlighted = false;
    m_checked = false;

    InitItems( GetMode() == wxLC_REPORT ? m_owner->GetColumnCount() : 1 );
}

void wxListLineData::CalculateSize( wxReadOnlyDC *dc, int spacing )
{
    wxCHECK_RET( !m_items.empty(), wxT("no subitems at all??") );

    wxListItemData* const item = &m_items[0];

    wxString s;
    wxCoord lw, lh;

    const auto measureLabel = [&](const wxString& label)
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        wxSize nativeSize;
        const auto* attr = item->GetAttr();
        const wxFont font = attr && attr->HasFont() ? attr->GetFont() : m_owner->GetFont();
        if ( wxWinUIListProjection::MeasureItemText(m_owner, label, font, nativeSize) )
        {
            lw = nativeSize.x;
            lh = nativeSize.y;
        }
        else
#endif
            dc->GetTextExtent(label, &lw, &lh);
    };

    switch ( GetMode() )
    {
        case wxLC_ICON:
        case wxLC_SMALL_ICON:
            m_gi->m_rectAll.width = spacing;

            s = item->GetText();

            if ( s.empty() )
            {
                lh =
                m_gi->m_rectLabel.width =
                m_gi->m_rectLabel.height = 0;
            }
            else // has label
            {
                measureLabel(s);
                lw += EXTRA_WIDTH;
                lh += EXTRA_HEIGHT;

                m_gi->m_rectAll.height = spacing + lh;
                if (lw > spacing)
                    m_gi->m_rectAll.width = lw;

                m_gi->m_rectLabel.width = lw;
                m_gi->m_rectLabel.height = lh;
            }

            if (item->HasImage())
            {
                int w, h;
                m_owner->GetImageSize( w, h );
                m_gi->m_rectIcon.width = w + 8;
                m_gi->m_rectIcon.height = h + 8;

                if ( m_gi->m_rectIcon.width > m_gi->m_rectAll.width )
                    m_gi->m_rectAll.width = m_gi->m_rectIcon.width;
                if ( m_gi->m_rectIcon.height + lh > m_gi->m_rectAll.height - 4 )
                    m_gi->m_rectAll.height = m_gi->m_rectIcon.height + lh + 4;
            }

            if ( item->HasText() )
            {
                m_gi->m_rectHighlight.width = m_gi->m_rectLabel.width;
                m_gi->m_rectHighlight.height = m_gi->m_rectLabel.height;
            }
            else // no text, highlight the icon
            {
                m_gi->m_rectHighlight.width = m_gi->m_rectIcon.width;
                m_gi->m_rectHighlight.height = m_gi->m_rectIcon.height;
            }
            break;

        case wxLC_LIST:
            s = item->GetTextForMeasuring();

            measureLabel(s);
            lw += EXTRA_WIDTH;
            lh += EXTRA_HEIGHT;
#if defined(__WXWINUI__) && wxUSE_WINUI3
            // The room of the selection bar of a WinUI list before the text.
            lw += m_owner->FromDIP(8);
#endif

            m_gi->m_rectLabel.width = lw;
            m_gi->m_rectLabel.height = lh;

            m_gi->m_rectAll.width = lw;
            m_gi->m_rectAll.height = lh;

            if (item->HasImage())
            {
                int w, h;
                m_owner->GetImageSize( w, h );
                m_gi->m_rectIcon.width = w;
                m_gi->m_rectIcon.height = h;

                m_gi->m_rectAll.width += 4 + w;
                if (h > m_gi->m_rectAll.height)
                    m_gi->m_rectAll.height = h;
            }

            m_gi->m_rectHighlight.width = m_gi->m_rectAll.width;
            m_gi->m_rectHighlight.height = m_gi->m_rectAll.height;
            break;

        case wxLC_REPORT:
            wxFAIL_MSG( wxT("unexpected call to SetSize") );
            break;

        default:
            wxFAIL_MSG( wxT("unknown mode") );
            break;
    }
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // TextBlock's measured text plus the native item presenter/content
    // padding. This belongs in wx geometry too: selection, hit testing,
    // column flow and the label editor must agree with what XAML displays.
    m_gi->m_rectAll.width += m_owner->FromDIP(16);
    if ( GetMode() == wxLC_LIST )
        m_gi->m_rectHighlight.width = m_gi->m_rectAll.width;
#endif
}

void wxListLineData::SetPosition( int x, int y, int WXUNUSED(spacing) )
{
    wxCHECK_RET( !m_items.empty(), wxT("no subitems at all??") );

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The native item presenter needs its actual text/padding height. Keep
    // wx geometry and hit testing authoritative instead of clipping a 32-DIP
    // peer to the historical GDI text-only row height.
    m_gi->m_rectAll.height = wxMax(m_gi->m_rectAll.height, m_owner->FromDIP(32));
    if ( GetMode() == wxLC_LIST )
        m_gi->m_rectHighlight.height = m_gi->m_rectAll.height;
#endif

    wxListItemData* const item = &m_items[0];

    switch ( GetMode() )
    {
        case wxLC_ICON:
        case wxLC_SMALL_ICON:
            m_gi->m_rectAll.x = x;
            m_gi->m_rectAll.y = y;

            if ( item->HasImage() )
            {
                m_gi->m_rectIcon.x = m_gi->m_rectAll.x + 4 +
                    (m_gi->m_rectAll.width - m_gi->m_rectIcon.width) / 2;
                m_gi->m_rectIcon.y = m_gi->m_rectAll.y + 4;
            }

            if ( item->HasText() )
            {
                m_gi->m_rectLabel.x = m_gi->m_rectAll.x + (EXTRA_WIDTH/2) +
                    (m_gi->m_rectAll.width - m_gi->m_rectLabel.width) / 2;
                m_gi->m_rectLabel.y = m_gi->m_rectAll.y + m_gi->m_rectAll.height +
                    (EXTRA_HEIGHT/2) - m_gi->m_rectLabel.height;
                m_gi->m_rectHighlight.x = m_gi->m_rectLabel.x - (EXTRA_WIDTH/2);
                m_gi->m_rectHighlight.y = m_gi->m_rectLabel.y - (EXTRA_HEIGHT/2);
            }
            else // no text, highlight the icon
            {
                m_gi->m_rectHighlight.x = m_gi->m_rectIcon.x - 4;
                m_gi->m_rectHighlight.y = m_gi->m_rectIcon.y - 4;
            }
            break;

        case wxLC_LIST:
            m_gi->m_rectAll.x = x;
            m_gi->m_rectAll.y = y;

            m_gi->m_rectHighlight.x = m_gi->m_rectAll.x;
            m_gi->m_rectHighlight.y = m_gi->m_rectAll.y;
            m_gi->m_rectLabel.y = m_gi->m_rectAll.y + 2;

            if (item->HasImage())
            {
                m_gi->m_rectIcon.x = m_gi->m_rectAll.x + 2;
                m_gi->m_rectIcon.y = m_gi->m_rectAll.y + 2;
                m_gi->m_rectLabel.x = m_gi->m_rectAll.x + 4 + (EXTRA_WIDTH/2) + m_gi->m_rectIcon.width;
            }
            else
            {
                m_gi->m_rectLabel.x = m_gi->m_rectAll.x + (EXTRA_WIDTH/2);
            }
            break;

        case wxLC_REPORT:
            wxFAIL_MSG( wxT("unexpected call to SetPosition") );
            break;

        default:
            wxFAIL_MSG( wxT("unknown mode") );
            break;
    }
}

void wxListLineData::InitItems( int num )
{
    for (int i = 0; i < num; i++)
        m_items.emplace_back( m_owner );
}

void wxListLineData::SetItem( int index, const wxListItem &info )
{
    m_items.at(index).SetItem( info );
}

void wxListLineData::GetItem( int index, wxListItem &info ) const
{
    m_items.at(index).GetItem( info );
}

wxString wxListLineData::GetText(int index) const
{
    return m_items.at(index).GetText();
}

void wxListLineData::SetText( int index, const wxString& s )
{
    m_items.at(index).SetText(s);
}

void wxListLineData::SetImage( int index, int image )
{
    m_items.at(index).SetImage(image);
}

int wxListLineData::GetImage( int index ) const
{
    return m_items.at(index).GetImage();
}

wxItemAttr *wxListLineData::GetAttr() const
{
    return m_items.at(0).GetAttr();
}

void wxListLineData::SetAttr(wxItemAttr *attr)
{
    m_items.at(0).SetAttr(attr);
}

void wxListLineData::ApplyAttributes(wxDC *dc,
                                     const wxRect& rectHL,
                                     bool highlighted,
                                     bool current)
{
    const wxItemAttr * const attr = GetAttr();

    wxWindow * const listctrl = m_owner->GetParent();

    const bool hasFocus = listctrl->HasFocus();

    // fg colour

    // don't use foreground colour for drawing highlighted items - this might
    // make them completely invisible (and there is no way to do bit
    // arithmetics on wxColour, unfortunately)
    wxColour colText;
    if ( highlighted )
    {
#ifdef __WXMAC__
        if ( hasFocus )
            colText = *wxWHITE;
        else
            colText = *wxBLACK;
#else
        colText = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
#endif
    }
    else if ( attr && attr->HasTextColour() )
        colText = attr->GetTextColour();
    else
        colText = listctrl->GetForegroundColour();

    dc->SetTextForeground(colText);

    // font
    wxFont font;
    if ( attr && attr->HasFont() )
        font = attr->GetFont();
    else
        font = listctrl->GetFont();

    dc->SetFont(font);

    // background
    if ( highlighted )
    {
        // Use the renderer method to ensure that the selected items use the
        // native look.
        int flags = wxCONTROL_SELECTED;
        if ( hasFocus )
            flags |= wxCONTROL_FOCUSED;
        if (current)
           flags |= wxCONTROL_CURRENT;
        wxRendererNative::Get().
            DrawItemSelectionRect( m_owner, *dc, rectHL, flags );
    }
    else if ( attr && attr->HasBackgroundColour() )
    {
        // Draw the background using the items custom background colour.
        dc->SetBrush(attr->GetBackgroundColour());
        dc->SetPen(*wxTRANSPARENT_PEN);
        dc->DrawRectangle(rectHL);
    }

    // just for debugging to better see where the items are
#if 0
    dc->SetPen(*wxRED_PEN);
    dc->SetBrush(*wxTRANSPARENT_BRUSH);
    dc->DrawRectangle( m_gi->m_rectAll );
    dc->SetPen(*wxGREEN_PEN);
    dc->DrawRectangle( m_gi->m_rectIcon );
#endif
}

void wxListLineData::Draw(wxDC *dc, const wxPoint& origin, bool current)
{
    wxCHECK_RET( !m_items.empty(), wxT("no subitems at all??") );

    wxRect rect(m_gi->m_rectHighlight);
    rect.Offset(origin);

    ApplyAttributes(dc, rect, IsHighlighted(), current);

    wxListItemData* const item = &m_items[0];
    if (item->HasImage())
    {
        // centre the image inside our rectangle, this looks nicer when items
        // ae aligned in a row
        const wxRect& rectIcon = m_gi->m_rectIcon;
        m_owner->DrawImage(
            item->GetImage(), dc, rectIcon.x + origin.x, rectIcon.y + origin.y);
    }

    if (item->HasText())
    {
        wxRect rectLabel = m_gi->m_rectLabel;
        rectLabel.Offset(origin);

        wxDCClipper clipper(*dc, rectLabel);
        dc->DrawText(item->GetText(), rectLabel.x, rectLabel.y);
    }
}

void wxListLineData::DrawInReportMode( wxDC *dc,
                                       const wxRect& rect,
                                       bool highlighted,
                                       bool current,
                                       bool checked )
{
    // TODO: later we should support setting different attributes for
    //       different columns - to do it, just add "col" argument to
    //       GetAttr() and move these lines into the loop below

    // Note: GetSubItemRect() needs to be modified if the layout here changes.

    ApplyAttributes(dc, rect, highlighted, current);

    wxCoord x = rect.x;
    wxCoord yMid = rect.y + rect.height/2;

    for ( int order = 0; order < m_owner->GetColumnCount(); ++order )
    {
        const int col = m_owner->GetColumnIndexFromOrder(order);
        int width = m_owner->GetColumnWidth(col);
        int xOld = x + ICON_OFFSET_X;
        x += width;
        // A control switched from icon to report view can have fewer values
        // in an existing row than columns. Still reserve its displayed width.
        if ( static_cast<size_t>(col) >= m_items.size() )
            continue;
        const auto& item = m_items[col];

        if ( col == 0 && m_owner->HasCheckBoxes() )
        {
            const wxSize cbSize = wxRendererNative::Get().GetCheckBoxSize(m_owner);
            const int yOffset = (rect.height - cbSize.GetHeight()) / 2;
            const wxRect rr(wxPoint(xOld - ICON_OFFSET_X + MARGIN_AROUND_CHECKBOX,
                                   rect.y + yOffset), cbSize);
            wxRendererNative::Get().DrawCheckBox(m_owner, *dc, rr,
                                                checked ? wxCONTROL_CHECKED : 0);
            const int checkWidth = cbSize.GetWidth() + 2 * MARGIN_AROUND_CHECKBOX;
            xOld += checkWidth;
            width -= checkWidth;
        }

        width -= 8;
        const int wText = width;
        wxDCClipper clipper(*dc, xOld, rect.y, wText, rect.height);

        if ( item.HasImage() )
        {
            int ix, iy;
            m_owner->GetImageSize( ix, iy );
            m_owner->DrawImage( item.GetImage(), dc, xOld, yMid - iy/2 );

            ix += IMAGE_MARGIN_IN_REPORT_MODE;

            xOld += ix;
            width -= ix;
        }

        if ( item.HasText() )
            DrawTextFormatted(dc, item.GetText(), col, xOld, yMid, width);
    }
}

void wxListLineData::DrawTextFormatted(wxDC *dc,
                                       const wxString& textOrig,
                                       int col,
                                       int x,
                                       int yMid,
                                       int width)
{
    // we don't support displaying multiple lines currently (and neither does
    // wxMSW FWIW) so just merge all the lines
    wxString text(textOrig);
    text.Replace(wxT("\n"), wxT(" "));

    wxCoord w, h;
    dc->GetTextExtent(text, &w, &h);

    const wxCoord y = yMid - (h + 1)/2;

    wxDCClipper clipper(*dc, x, y, width, h);

    // determine if the string can fit inside the current width
    if (w <= width)
    {
        // it can, draw it using the items alignment
        wxListItem item;
        m_owner->GetColumn(col, item);
        switch ( item.GetAlign() )
        {
            case wxLIST_FORMAT_LEFT:
                // nothing to do
                break;

            case wxLIST_FORMAT_RIGHT:
                x += width - w;
                break;

            case wxLIST_FORMAT_CENTER:
                x += (width - w) / 2;
                break;

            default:
                wxFAIL_MSG( wxT("unknown list item format") );
                break;
        }

        dc->DrawText(text, x, y);
    }
    else // otherwise, truncate and add an ellipsis if possible
    {
        dc->DrawText(wxControl::Ellipsize(text, *dc, wxELLIPSIZE_END, width,
                                          wxELLIPSIZE_FLAGS_NONE),
                     x, y);
    }
}

bool wxListLineData::Highlight( bool on )
{
    wxCHECK_MSG( !IsVirtual(), false, wxT("unexpected call to Highlight") );

    if ( on == m_highlighted )
        return false;

    m_highlighted = on;
    m_owner->UpdateSelectionCount(on);

    return true;
}

void wxListLineData::ReverseHighlight( void )
{
    Highlight(!IsHighlighted());
}

//-----------------------------------------------------------------------------
//  wxListHeaderWindow
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxListHeaderWindow,wxWindow)
    EVT_PAINT         (wxListHeaderWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxListHeaderWindow::OnMouse)
    EVT_SYS_COLOUR_CHANGED(wxListHeaderWindow::OnSysColourChanged)
wxEND_EVENT_TABLE()

void wxListHeaderWindow::Init()
{
    m_currentCursor = nullptr;
    m_isDragging = false;
    m_dirty = false;
    m_sendSetColumnWidth = false;
}

wxListHeaderWindow::wxListHeaderWindow()
{
    Init();

    m_owner = nullptr;
    m_resizeCursor = nullptr;

    m_sortAsc = true;
    m_sortCol = -1;
}

bool wxListHeaderWindow::Create( wxWindow *win,
                                 wxWindowID id,
                                 wxListMainWindow *owner,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString &name )
{
    if ( !wxWindow::Create(win, id, pos, size, style, name) )
        return false;

    Init();

    m_owner = owner;
    m_resizeCursor = new wxCursor( wxCURSOR_SIZEWE );

#if _USE_VISATTR
    wxVisualAttributes attr = wxPanel::GetClassDefaultAttributes();
    SetOwnForegroundColour( attr.colFg );
    SetOwnBackgroundColour( attr.colBg );
    if (!m_hasFont)
        SetOwnFont( attr.font );
#else
    SetOwnForegroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    SetOwnBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    if (!m_hasFont)
        SetOwnFont( wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT ));
#endif

    return true;
}

wxListHeaderWindow::~wxListHeaderWindow()
{
    delete m_resizeCursor;
}

#if defined(__WXWINUI__) && wxUSE_WINUI3
bool wxListHeaderWindow::AcceptsFocus() const
{
    // Container focus bookkeeping also queries surviving siblings while the
    // main pane/list are being destroyed. Never cast through m_owner then:
    // its GetListCtrl() requires a still-live concrete parent class.
    if ( IsBeingDeleted() ) return false;
    const auto* const list = wxDynamicCast(GetParent(), wxGenericListCtrl);
    if ( !list || list->IsBeingDeleted() ) return false;
    const auto projection = list->m_winUIProjection;
    return projection && projection->OwnsWindow(const_cast<wxListHeaderWindow*>(this));
}
#endif

#ifdef __WXUNIVERSAL__
#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"
#endif

// shift the DC origin to match the position of the main window horz
// scrollbar: this allows us to always use logical coords
void wxListHeaderWindow::AdjustDC(wxDC& dc)
{
    wxGenericListCtrl *parent = m_owner->GetListCtrl();

    int xpix;
    parent->GetScrollPixelsPerUnit( &xpix, nullptr );

    int view_start;
    parent->GetViewStart( &view_start, nullptr );


    int org_x = 0;
    int org_y = 0;
    dc.GetDeviceOrigin( &org_x, &org_y );

    // account for the horz scrollbar offset
#if defined(__WXGTK__) && !defined(__WXGTK3__)
    if (GetLayoutDirection() == wxLayout_RightToLeft)
    {
        // Maybe we just have to check for m_signX
        // in the DC, but I leave the #ifdef __WXGTK__
        // for now
        dc.SetDeviceOrigin( org_x + (view_start * xpix), org_y );
    }
    else
#endif
        dc.SetDeviceOrigin( org_x - (view_start * xpix), org_y );
}

void wxListHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxGenericListCtrl *parent = m_owner->GetListCtrl();

    wxPaintDC dc( this );
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( parent->m_winUIProjection && parent->m_winUIProjection->OwnsWindow(this) )
        return;
#endif

    AdjustDC( dc );

    // width and height of the entire header window
    int w, h;
    GetClientSize( &w, &h );
    parent->CalcUnscrolledPosition(w, 0, &w, nullptr);

    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    int x = HEADER_OFFSET_X;
    int numColumns = m_owner->GetColumnCount();
    wxListItem item;
    for ( int order = 0; order < numColumns && x < w; ++order )
    {
        const int i = m_owner->GetColumnIndexFromOrder(order);
        m_owner->GetColumn( i, item );
        int wCol = item.m_width;

        int cw = wCol;
        int ch = h;

        int flags = 0;
        if (!m_parent->IsEnabled())
            flags |= wxCONTROL_DISABLED;

// NB: The code below is not really Mac-specific, but since we are close
// to 2.8 release and I don't have time to test on other platforms, I
// defined this only for wxMac. If this behaviour is desired on
// other platforms, please go ahead and revise or remove the #ifdef.
#ifdef __WXMAC__
        if ( !m_owner->IsVirtual() && (item.m_mask & wxLIST_MASK_STATE) &&
                (item.m_state & wxLIST_STATE_SELECTED) )
            flags |= wxCONTROL_SELECTED;
#endif

        wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE;
        if ( i == m_sortCol )
        {
            if ( m_sortAsc )
                sortArrow = wxHDR_SORT_ICON_UP;
            else
                sortArrow = wxHDR_SORT_ICON_DOWN;
        }

        if (order == 0)
           flags |= wxCONTROL_SPECIAL; // mark as first column

        wxHeaderButtonParams headerBtnParams;
        headerBtnParams.m_arrowColour = GetForegroundColour();

        wxRendererNative::Get().DrawHeaderButton
                                (
                                    this,
                                    dc,
                                    wxRect(x, HEADER_OFFSET_Y, cw, ch),
                                    flags,
                                    sortArrow,
                                    &headerBtnParams
                                );

        // see if we have enough space for the column label

        // for this we need the width of the text
        wxCoord wLabel;
        wxCoord hLabel;
        dc.GetTextExtent(item.GetText(), &wLabel, &hLabel);
        wLabel += 2 * EXTRA_WIDTH;

        // and the width of the icon, if any
        int ix = 0, iy = 0;    // init them just to suppress the compiler warnings
        const int image = item.m_image;
        wxWithImages *smallImages;
        if ( image != -1 )
        {
            smallImages = m_owner->GetSmallImages();
            if ( smallImages )
            {
                smallImages->GetImageLogicalSize(this, ix, iy);
                wLabel += ix + HEADER_IMAGE_MARGIN_IN_REPORT_MODE;
            }
        }
        else
        {
            smallImages = nullptr;
        }

        // ignore alignment if there is not enough space anyhow
        int xAligned;
        switch ( wLabel < cw ? item.GetAlign() : wxLIST_FORMAT_LEFT )
        {
            default:
                wxFAIL_MSG( wxT("unknown list item format") );
                wxFALLTHROUGH;

            case wxLIST_FORMAT_LEFT:
                xAligned = x;
                break;

            case wxLIST_FORMAT_RIGHT:
                xAligned = x + cw - wLabel;
                break;

            case wxLIST_FORMAT_CENTER:
                xAligned = x + (cw - wLabel) / 2;
                break;
        }

        // draw the text and image clipping them so that they
        // don't overwrite the column boundary
        wxDCClipper clipper(dc, x, HEADER_OFFSET_Y, cw, h);

        // if we have an image, draw it on the right of the label
        if ( smallImages )
        {
            wxDrawImageBitmap
            (
                this, *smallImages, image,
                dc,
                xAligned + wLabel - ix - HEADER_IMAGE_MARGIN_IN_REPORT_MODE,
                HEADER_OFFSET_Y + (h - iy)/2
            );
        }

        dc.DrawText( item.GetText(),
                     xAligned + EXTRA_WIDTH, (h - hLabel) / 2 );

        x += wCol;
    }

    // Fill in what's missing to the right of the columns, otherwise we will
    // leave an unpainted area when columns are removed (and it looks better)
    if ( x < w )
    {
        wxRendererNative::Get().DrawHeaderButton
                                (
                                    this,
                                    dc,
                                    wxRect(x, HEADER_OFFSET_Y, w - x, h),
                                    wxCONTROL_DIRTY // mark as last column
                                );
    }
}

void wxListHeaderWindow::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    if (m_sendSetColumnWidth)
    {
        m_owner->SetColumnWidth( m_colToSend, m_widthToSend );
        m_sendSetColumnWidth = false;
    }
}

void wxListHeaderWindow::DrawCurrent()
{
    m_sendSetColumnWidth = true;
    m_colToSend = m_column;
    m_widthToSend = m_currentX - m_minX;
}

void wxListHeaderWindow::OnColumnOrderChanged()
{
    if ( m_isDragging && m_column >= 0 )
    {
        const int width = m_currentX - m_minX;
        m_minX = m_owner->GetColumnStart(m_column);
        m_currentX = m_minX + width;
    }
    m_dirty = true;
    Refresh(false);
}

void wxListHeaderWindow::OnMouse( wxMouseEvent &event )
{
    wxGenericListCtrl *parent = m_owner->GetListCtrl();

    // we want to work with logical coords
    int x;
    parent->CalcUnscrolledPosition(event.GetX(), 0, &x, nullptr);

    if (m_isDragging)
    {
        SendListEvent(wxEVT_LIST_COL_DRAGGING, event.GetPosition());

        // we don't draw the line beyond our window, but we allow dragging it
        // there
        int w = 0;
        GetClientSize( &w, nullptr );
        parent->CalcUnscrolledPosition(w, 0, &w, nullptr);
        w -= 6;

        // erase the line if it was drawn
        if ( m_currentX < w )
            DrawCurrent();

        if (event.ButtonUp())
        {
            ReleaseMouse();
            m_isDragging = false;
            m_dirty = true;
            m_owner->SetColumnWidth( m_column, m_currentX - m_minX );
            SendListEvent(wxEVT_LIST_COL_END_DRAG, event.GetPosition());
        }
        else
        {
            if (x > m_minX + 7)
                m_currentX = x;
            else
                m_currentX = m_minX + 7;

            // draw in the new location
            if ( m_currentX < w )
                DrawCurrent();
        }
    }
    else // not dragging
    {
        m_minX = 0;
        bool hit_border = false;

        // end of the current column
        int xpos = 0;

        // find the column where this event occurred
        int order;
        const int countCol = m_owner->GetColumnCount();
        for (order = 0; order < countCol; ++order)
        {
            const int col = m_owner->GetColumnIndexFromOrder(order);
            xpos += m_owner->GetColumnWidth( col );
            m_column = col;

            if ( abs(x-xpos) < 3 )
            {
                // near the column border
                hit_border = true;
                break;
            }

            if ( x < xpos )
            {
                // inside the column
                break;
            }

            m_minX = xpos;
        }

        if ( order == countCol )
            m_column = -1;

        if (event.LeftDown() || event.RightUp())
        {
            if (hit_border && event.LeftDown())
            {
                if ( SendListEvent(wxEVT_LIST_COL_BEGIN_DRAG,
                                   event.GetPosition()) )
                {
                    m_isDragging = true;
                    m_currentX = x;
                    CaptureMouse();
                    DrawCurrent();
                }
                //else: column resizing was vetoed by the user code
            }
            else // click on a column
            {
                // record the selected state of the columns
                if (event.LeftDown())
                {
                    for (int i=0; i < m_owner->GetColumnCount(); i++)
                    {
                        wxListItem colItem;
                        m_owner->GetColumn(i, colItem);
                        long state = colItem.GetState();
                        if (i == m_column)
                            colItem.SetState(state | wxLIST_STATE_SELECTED);
                        else
                            colItem.SetState(state & ~wxLIST_STATE_SELECTED);
                        m_owner->SetColumn(i, colItem);
                    }
                }

                SendListEvent( event.LeftDown()
                                    ? wxEVT_LIST_COL_CLICK
                                    : wxEVT_LIST_COL_RIGHT_CLICK,
                                event.GetPosition());
            }
        }
        else if ( event.LeftDClick() && hit_border )
        {
            // Autosize the column when the divider is clicked: if there are
            // any items, fit the columns to its contents, otherwise just fit
            // it to its label width.
            parent->SetColumnWidth(m_column,
                                   parent->IsEmpty() ? wxLIST_AUTOSIZE_USEHEADER
                                                     : wxLIST_AUTOSIZE);
        }
        else if (event.Moving())
        {
            bool setCursor;
            if (hit_border)
            {
                setCursor = m_currentCursor == wxSTANDARD_CURSOR;
                m_currentCursor = m_resizeCursor;
            }
            else
            {
                setCursor = m_currentCursor != wxSTANDARD_CURSOR;
                m_currentCursor = wxSTANDARD_CURSOR;
            }

            if ( setCursor )
                SetCursor(*m_currentCursor);
        }
    }
}

void wxListHeaderWindow::OnSysColourChanged(wxSysColourChangedEvent &event)
{
    SetOwnForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    SetOwnBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    Refresh();

    event.Skip();
}

bool wxListHeaderWindow::SendListEvent(wxEventType type, const wxPoint& pos)
{
    wxWindow *parent = GetParent();
    wxListEvent le( type, parent->GetId() );
    le.SetEventObject( parent );
    le.m_pointDrag = pos;
    le.m_col = m_column;
    return !parent->GetEventHandler()->ProcessEvent( le ) || le.IsAllowed();
}

//-----------------------------------------------------------------------------
// wxListRenameTimer (internal)
//-----------------------------------------------------------------------------

wxListRenameTimer::wxListRenameTimer( wxListMainWindow *owner )
{
    m_owner = owner;
}

void wxListRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}

//-----------------------------------------------------------------------------
// wxListFindTimer (internal)
//-----------------------------------------------------------------------------

void wxListFindTimer::Notify()
{
    m_owner->OnFindTimer();
}

//-----------------------------------------------------------------------------
// wxListTextCtrlWrapper (internal)
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxListTextCtrlWrapper, wxEvtHandler)
    EVT_CHAR           (wxListTextCtrlWrapper::OnChar)
    EVT_KEY_UP         (wxListTextCtrlWrapper::OnKeyUp)
    EVT_KILL_FOCUS     (wxListTextCtrlWrapper::OnKillFocus)
wxEND_EVENT_TABLE()

wxListTextCtrlWrapper::wxListTextCtrlWrapper(wxListMainWindow *owner,
                                             wxTextCtrl *text,
                                             size_t itemEdit, const wxString& value)
              : m_startValue(value),
                m_itemEdited(itemEdit)
{
    m_owner = owner;
    m_text = text;
    m_aboutToFinish = false;
    m_finished = false;

}

// Where the editor of a label goes, from the rectangle of the label.
static wxRect wxListEditorRect(const wxWindow *win, const wxRect& rectLabel)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The editor of a cell of a table of WinUI, as in a property grid: a
    // field of the standard height (32) in the middle of its row, which is
    // higher (40), a little inside the cell; its text is then where the
    // text of the cell was. The classic edit control is made to overflow
    // the label instead.
    const int height = wxMin(rectLabel.height, win->FromDIP(32));
    const int inset = win->FromDIP(2);
    return wxRect(rectLabel.x + inset,
                  rectLabel.y + (rectLabel.height - height) / 2,
                  wxMax(0, rectLabel.width - 2 * inset),
                  height);
#else
    wxUnusedVar(win);
    return wxRect(rectLabel.x - 4, rectLabel.y - 4,
                  rectLabel.width + 11, rectLabel.height + 8);
#endif
}

bool wxListTextCtrlWrapper::CreateControl(const wxRect& rectLabel)
{
    const wxWeakRef<wxListTextCtrlWrapper> weakThis(this);
    const wxWeakRef<wxListMainWindow> weakOwner(m_owner);
    const wxWeakRef<wxTextCtrl> weakText(m_text);
    const wxRect rectEditor = wxListEditorRect(m_owner, rectLabel);
    const bool created = m_text->Create(m_owner, wxID_ANY, m_startValue,
                   rectEditor.GetPosition(), rectEditor.GetSize());
    if ( !created || !weakThis || !weakOwner || !weakText || m_finished || !m_itemValid )
        return false;
    m_text->PushEventHandler(this);
    m_handlerInstalled = true;
    m_text->SetFocus();
    return weakThis && weakOwner && weakText && !m_finished && m_itemValid;
}

void wxListTextCtrlWrapper::UpdatePosition()
{
    if ( m_finished || !m_itemValid || m_itemEdited >= m_owner->GetItemCount() )
        return;
    const wxWeakRef<wxListTextCtrlWrapper> weakThis(this);
    const wxWeakRef<wxListMainWindow> weakOwner(m_owner);
    wxListItem info;
    wxRect rectLabel;
    if ( !m_owner->GetLabelEditItem(m_itemEdited, info) || !weakThis || !weakOwner ||
         !m_itemValid || !m_owner->GetLabelEditRect(m_itemEdited, info, rectLabel) ||
         !weakThis || !weakOwner || !m_itemValid )
        return;
    // SetSize may invoke application code: there is no access to the wrapper
    // or owner after this call, and an in-progress edit keeps its value/focus.
    const wxRect rectEditor = wxListEditorRect(m_owner, rectLabel);
    m_text->SetSize(rectEditor.x, rectEditor.y,
                    rectEditor.width, rectEditor.height);
}

bool wxListTextCtrlWrapper::EndEdit(EndReason reason)
{
    if ( m_finished ) return false;
    if( m_aboutToFinish )
    {
        // A validation handler may destroy the whole composite before the
        // outer End_Accept reaches Finish(). Destruction must still detach the
        // pushed handler before wxTextCtrl's destructor runs. Finish itself
        // owns the exactly-once physical cleanup guard.
        if ( reason == End_Destroy )
            Finish(false);
        return false;
    }

    m_aboutToFinish = true;
    const wxWeakRef<wxListTextCtrlWrapper> weakThis(this);
    const wxWeakRef<wxListMainWindow> weakOwner(m_owner);
    const wxWeakRef<wxTextCtrl> weakText(m_text);

    switch ( reason )
    {
        case End_Accept:
        {
            // Notify the owner about the changes
            const bool accepted = AcceptChanges();
            if ( !weakThis || !weakOwner || !weakText || m_finished ) return false;

            // Even if vetoed, close the control (consistent with MSW)
            Finish( true );
            return accepted;
        }

        case End_Discard:
            if ( m_itemValid ) m_owner->OnRenameCancelled(m_itemEdited);
            if ( !weakThis || !weakOwner || !weakText || m_finished ) return false;

            Finish( true );
            break;

        case End_Destroy:
            // Don't generate any notifications for the control being destroyed
            // and don't set focus to it either.
            Finish(false);
            break;
    }
    return false;
}

void wxListTextCtrlWrapper::Finish( bool setfocus )
{
    if ( m_finished ) return;
    m_finished = true;
    const wxWeakRef<wxListMainWindow> weakOwner(m_owner);
    wxTextCtrl* const text = m_text;
    if ( m_handlerInstalled ) m_text->RemoveEventHandler(this);
    m_handlerInstalled = false;
    wxPendingDelete.Append( this );
    m_owner->ResetTextControl(text);

    if ( setfocus && weakOwner )
        weakOwner->SetFocus();
}

bool wxListTextCtrlWrapper::AcceptChanges()
{
    if ( !m_itemValid ) return false;
    const wxWeakRef<wxListTextCtrlWrapper> weakThis(this);
    const wxWeakRef<wxListMainWindow> weakOwner(m_owner);
    const wxWeakRef<wxTextCtrl> weakText(m_text);
    const wxString value = m_text->GetValue();
    if ( !weakThis || !weakOwner || !weakText || m_finished || !m_itemValid ) return false;

    // notice that we should always call OnRenameAccept() to generate the "end
    // label editing" event, even if the user hasn't really changed anything
    const bool accepted = m_owner->OnRenameAccept(m_itemEdited, value);
    if ( !weakThis || !weakOwner || !weakText || m_finished || !m_itemValid || !accepted )
    {
        // vetoed by the user
        return false;
    }

    // accepted, do rename the item (unless nothing changed)
    if ( value != m_startValue )
        m_owner->SetItemText(m_itemEdited, value);

    if ( !weakThis || !weakOwner || !weakText || m_finished || !m_itemValid )
        return false;

    // OnRenameAccept marked geometry dirty, so SetItem() deliberately skipped
    // refreshing this row. A virtual END handler changes the application model,
    // not m_lines, and returning focus need not refresh an offscreen current
    // item. Schedule the committed row explicitly, including native projections
    // that don't rely on the generic paint path to consume m_dirty.
    m_owner->RefreshLine(m_itemEdited);

    return true;
}

void wxListTextCtrlWrapper::OnChar( wxKeyEvent &event )
{
    if ( !CheckForEndEditKey(event) )
        event.Skip();
}

bool wxListTextCtrlWrapper::CheckForEndEditKey(const wxKeyEvent& event)
{
    switch ( event.m_keyCode )
    {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            EndEdit( End_Accept );
            break;

        case WXK_ESCAPE:
            EndEdit( End_Discard );
            break;

        default:
            return false;
    }

    return true;
}

void wxListTextCtrlWrapper::OnKeyUp( wxKeyEvent &event )
{
    if ( !m_aboutToFinish )
    {
        // auto-grow the textctrl:
        wxSize parentSize = m_owner->GetSize();
        wxPoint myPos = m_text->GetPosition();
        wxSize mySize = m_text->GetSize();
        int sx, sy;
        m_text->GetTextExtent(m_text->GetValue() + wxT("MM"), &sx, &sy);
        if (myPos.x + sx > parentSize.x)
            sx = parentSize.x - myPos.x;
       if (mySize.x > sx)
            sx = mySize.x;
       m_text->SetSize(sx, wxDefaultCoord);
    }

    event.Skip();
}

void wxListTextCtrlWrapper::OnKillFocus( wxFocusEvent &event )
{
    if ( !m_aboutToFinish )
    {
        const wxWeakRef<wxListTextCtrlWrapper> weakThis(this);
        const wxWeakRef<wxListMainWindow> weakOwner(m_owner);
        const wxWeakRef<wxTextCtrl> weakText(m_text);
        m_aboutToFinish = true;
        const bool accepted = AcceptChanges();
        if ( !weakThis || !weakOwner || !weakText ) { event.Skip(); return; }
        if ( !accepted && m_itemValid )
            m_owner->OnRenameCancelled( m_itemEdited );
        if ( !weakThis || !weakOwner || !weakText ) { event.Skip(); return; }

        Finish( false );
    }

    // We must let the native text control handle focus
    event.Skip();
}

//-----------------------------------------------------------------------------
//  wxListMainWindow
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxListMainWindow, wxWindow)
  EVT_PAINT          (wxListMainWindow::OnPaint)
  EVT_MOUSE_EVENTS   (wxListMainWindow::OnMouse)
  EVT_CHAR_HOOK      (wxListMainWindow::OnCharHook)
  EVT_CHAR           (wxListMainWindow::OnChar)
  EVT_KEY_DOWN       (wxListMainWindow::OnKeyDown)
  EVT_KEY_UP         (wxListMainWindow::OnKeyUp)
  EVT_SET_FOCUS      (wxListMainWindow::OnSetFocus)
  EVT_KILL_FOCUS     (wxListMainWindow::OnKillFocus)
  EVT_SCROLLWIN      (wxListMainWindow::OnScroll)
  EVT_CHILD_FOCUS    (wxListMainWindow::OnChildFocus)
  EVT_SYS_COLOUR_CHANGED(wxListMainWindow::OnSysColourChanged)
wxEND_EVENT_TABLE()

void wxListMainWindow::Init()
{
    m_dirty = true;
    m_selCount =
    m_countVirt = 0;
    m_structureRevision = 0;
    m_lineFrom =
    m_lineTo = wxNO_ITEM;
    m_linesPerPage = 0;

    m_headerWidth =
    m_lineHeight = 0;

    m_small_images = nullptr;
    m_normal_images = nullptr;

    m_small_spacing = 30;
    m_normal_spacing = 40;

    m_hasFocus = false;
    m_dragCount = 0;
    m_isCreated = false;

    m_lastOnSame = false;
    m_renameTimer = new wxListRenameTimer( this );
    m_findTimer = nullptr;
    m_findBell = 0;  // default is to not ring bell at all
    m_textctrlWrapper = nullptr;

    m_current =
    m_lineLastClicked =
    m_lineSelectSingleOnUp =
    m_lineBeforeLastClicked =
    m_anchor  = wxNO_ITEM;

    m_hasCheckBoxes = false;
    m_extendRulesAndAlternateColour = false;
}

wxListMainWindow::wxListMainWindow()
{
    Init();

    m_highlightBrush =
    m_highlightUnfocusedBrush = nullptr;
}

wxListMainWindow::wxListMainWindow( wxWindow *parent,
                                    wxWindowID id,
                                    const wxPoint& pos,
                                    const wxSize& size )
                : wxWindow( parent, id, pos, size,
                            wxWANTS_CHARS | wxBORDER_NONE )
{
    Init();

    m_highlightBrush = new wxBrush
                         (
                            wxSystemSettings::GetColour
                            (
                                wxSYS_COLOUR_HIGHLIGHT
                            ),
                            wxBRUSHSTYLE_SOLID
                         );

    m_highlightUnfocusedBrush = new wxBrush
                              (
                                 wxSystemSettings::GetColour
                                 (
                                     wxSYS_COLOUR_BTNSHADOW
                                 ),
                                 wxBRUSHSTYLE_SOLID
                              );

    wxVisualAttributes attr = wxGenericListCtrl::GetClassDefaultAttributes();
    SetOwnForegroundColour( attr.colFg );
    SetOwnBackgroundColour( attr.colBg );
    if (!m_hasFont)
        SetOwnFont( attr.font );

#if defined(__WXWINUI__) && wxUSE_WINUI3
    m_winuiScrollTimer.Bind(wxEVT_TIMER, [this](wxTimerEvent&)
    {
        WinUIScrollStep();
    });
#endif
}

#if defined(__WXWINUI__) && wxUSE_WINUI3

void wxListMainWindow::WinUIStopScrolling()
{
    m_winuiScrollTarget = -1;
    m_winuiScrollTimer.Stop();
}

void wxListMainWindow::WinUIScrollByLines(int lines, bool animate)
{
    WinUIScrollBy(lines * GetLineHeight(), animate);
}

void wxListMainWindow::WinUIScrollBy(int pixels, bool animate)
{
    if ( !InReportView() || !pixels )
        return;

    int viewStartY = 0;
    GetListCtrl()->GetViewStart(nullptr, &viewStartY);
    const int entire =
        static_cast<int>(GetItemCount()) * GetLineHeight() + LINE_SPACING;
    const int last = wxMax(0, entire - GetClientSize().y);

    // From where the scroll under way is going, for the turns of a wheel
    // to add up.
    const int from = m_winuiScrollTarget >= 0 ? m_winuiScrollTarget
                                               : viewStartY;
    const int target = wxClip(from + pixels, 0, last);
    if ( !animate )
    {
        WinUIStopScrolling();
        if ( target != viewStartY )
            GetListCtrl()->Scroll(-1, target);
        return;
    }

    m_winuiScrollTarget = target;
    if ( !m_winuiScrollTimer.IsRunning() )
        m_winuiScrollTimer.Start(10);
    WinUIScrollStep();
}

void wxListMainWindow::WinUIScrollStep()
{
    if ( m_winuiScrollTarget < 0 || !InReportView() )
    {
        WinUIStopScrolling();
        return;
    }

    int viewStartY = 0;
    GetListCtrl()->GetViewStart(nullptr, &viewStartY);
    const int left = m_winuiScrollTarget - viewStartY;
    if ( !left )
    {
        WinUIStopScrolling();
        return;
    }

    // A third of what is left at each step: quick at first, then slower.
    int step = left / 3;
    if ( !step )
        step = left > 0 ? 1 : -1;

    const wxWeakRef<wxWindow> alive(this);
    GetListCtrl()->Scroll(-1, viewStartY + step);
    if ( !alive )
        return;

    // Nowhere further to go: the list got shorter meanwhile.
    int now = 0;
    GetListCtrl()->GetViewStart(nullptr, &now);
    if ( now == viewStartY )
        WinUIStopScrolling();
}

void wxListMainWindow::WinUIOnWheel(wxMouseEvent& event)
{
    const int delta = event.GetWheelDelta();
    if ( !delta )
        return;

    if ( !InReportView() )
    {
        // A list of names in columns (wxLC_LIST, the list of a wxFileCtrl)
        // has only a horizontal bar: the wheel moves it, as it does in the
        // list view of the Explorer, where it had no effect at all.
        m_winuiWheelRest += event.GetWheelRotation() *
                            wxMax(1, event.GetLinesPerAction()) *
                            GetLineHeight();
        const int units = m_winuiWheelRest / (delta * SCROLL_UNIT_X);
        m_winuiWheelRest -= units * delta * SCROLL_UNIT_X;
        if ( units )
        {
            int viewStartX = 0;
            GetListCtrl()->GetViewStart(&viewStartX, nullptr);
            GetListCtrl()->Scroll(wxMax(0, viewStartX - units), -1);
        }
        return;
    }

    const int lines = event.IsPageScroll()
        ? wxMax(1, GetCountPerPage())
        : wxMax(1, event.GetLinesPerAction());

    // A notch of a wheel is that many lines. A touch pad sends small parts
    // of a notch, many times: each moves the list by the pixels it is worth,
    // and what is less than a pixel is kept for the next one.
    m_winuiWheelRest += event.GetWheelRotation() * lines * GetLineHeight();
    const int pixels = m_winuiWheelRest / delta;
    m_winuiWheelRest -= pixels * delta;
    if ( !pixels )
        return;

    // The notches of a wheel are far apart: the list glides to where each
    // sends it. What a touch pad sends is followed as it comes.
    const bool notch = abs(event.GetWheelRotation()) >= delta;
    WinUIScrollBy(-pixels, notch);
}

#endif // __WXWINUI__ && wxUSE_WINUI3

wxListMainWindow::~wxListMainWindow()
{
    if ( m_textctrlWrapper )
        m_textctrlWrapper->EndEdit(wxListTextCtrlWrapper::End_Destroy);

    DoDeleteAllItems(!m_deleteAllNotificationActive);

    delete m_highlightBrush;
    delete m_highlightUnfocusedBrush;
    delete m_renameTimer;
    delete m_findTimer;
}

void wxListMainWindow::SetReportView(bool inReportView)
{
    InvalidateLabelEditFrom(0);
#if defined(__WXWINUI__) && wxUSE_WINUI3
    ++m_structureRevision;
#endif
    for ( auto& line : m_lines )
    {
        line.SetReportView(inReportView);
    }
}

void wxListMainWindow::CacheLineData(size_t line)
{
    wxGenericListCtrl *listctrl = GetListCtrl();

    wxListLineData *ld = GetDummyLine();

    size_t countCol = GetColumnCount();
    for ( size_t col = 0; col < countCol; col++ )
    {
        ld->SetText(col, listctrl->OnGetItemText(line, col));
        ld->SetImage(col, listctrl->OnGetItemColumnImage(line, col));
    }

    if ( HasCheckBoxes() )
    {
        ld->Check(listctrl->OnGetItemIsChecked(line));
    }

    ld->SetAttr(listctrl->OnGetItemAttr(line));
}

wxListLineData *wxListMainWindow::GetDummyLine() const
{
    wxASSERT_MSG( !IsEmpty(), wxT("invalid line index") );
    wxASSERT_MSG( IsVirtual(), wxT("GetDummyLine() shouldn't be called") );

    wxListMainWindow *self = wxConstCast(this, wxListMainWindow);

    // we need to recreate the dummy line if the number of columns in the
    // control changed as it would have the incorrect number of fields
    // otherwise
    if ( !m_lines.empty() &&
            m_lines[0].m_items.size() != (size_t)GetColumnCount() )
    {
        self->m_lines.clear();
    }

    if ( m_lines.empty() )
    {
        self->m_lines.emplace_back(self);

        // don't waste extra memory -- there never going to be anything
        // else/more in this array
        self->m_lines.shrink_to_fit();
    }

    return &self->m_lines[0];
}

// ----------------------------------------------------------------------------
// line geometry (report mode only)
// ----------------------------------------------------------------------------

wxCoord wxListMainWindow::GetLineHeight() const
{
    // we cache the line height as calling GetTextExtent() is slow
    if ( !m_lineHeight )
    {
        wxListMainWindow *self = wxConstCast(this, wxListMainWindow);

        wxInfoDC dc( self );

        wxCoord y;
        dc.GetTextExtent(wxT("H"), nullptr, &y);

        if ( m_small_images && m_small_images->GetImageCount() )
        {
            int iw = 0, ih = 0;
            m_small_images->GetImageLogicalSize(this, iw, ih);
            y = wxMax(y, ih);
        }

        y += EXTRA_HEIGHT;
        self->m_lineHeight = y + LINE_SPACING;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        // The rows of a WinUI table.
        self->m_lineHeight = wxMax(self->m_lineHeight,
                                   FromDIP(wxWINUI_TABLE_ROW_HEIGHT));
#endif
    }

    return m_lineHeight;
}

wxCoord wxListMainWindow::GetLineY(size_t line) const
{
    wxASSERT_MSG( InReportView(), wxT("only works in report mode") );

    return LINE_SPACING + line * GetLineHeight();
}

wxRect wxListMainWindow::GetLineRect(size_t line) const
{
    if ( !InReportView() )
        return GetLine(line)->m_gi->m_rectAll;

    wxRect rect;
    rect.x = HEADER_OFFSET_X;
    rect.y = GetLineY(line);
    rect.width = GetHeaderWidth();
    rect.height = GetLineHeight();

    return rect;
}

wxRect wxListMainWindow::GetLineLabelRect(size_t line) const
{
    if ( !InReportView() )
        return GetLine(line)->m_gi->m_rectLabel;

    int contentOffset = ICON_OFFSET_X;
    if ( HasCheckBoxes() )
    {
        const wxSize cbSize =
            wxRendererNative::Get().GetCheckBoxSize(
                const_cast<wxListMainWindow*>(this));
        contentOffset +=
            cbSize.GetWidth() + 2 * MARGIN_AROUND_CHECKBOX;
    }

    wxListLineData *data = GetLine(line);
    if ( !data->m_items.empty() )
    {
        wxListItemData *item = &data->m_items[0];
        if ( item->HasImage() )
        {
            int ix, iy;
            GetImageSize( ix, iy );
            // The public label rectangle includes the visual margin following
            // the icon, so it starts immediately after the icon itself.
            contentOffset += ix;
        }
    }

    wxRect rect = GetLineRect(line);
    rect.width = GetColumnWidth(0);
    rect.x += GetColumnStart(0);
    rect.x += contentOffset;
    rect.width = wxMax(0, rect.width - contentOffset);

    return rect;
}

wxRect wxListMainWindow::GetLineIconRect(size_t line) const
{
    if ( !InReportView() )
        return GetLine(line)->m_gi->m_rectIcon;

    wxASSERT_MSG( GetLine(line)->HasImage(), wxT("should have an image") );

    wxRect rect = GetLineRect(line);
    rect.x += GetColumnStart(0);
    rect.x += ICON_OFFSET_X;

    if ( HasCheckBoxes() )
    {
        wxSize cbSize = wxRendererNative::Get().GetCheckBoxSize(const_cast<wxListMainWindow*>(this));
        rect.x += cbSize.GetWidth() + (2 * MARGIN_AROUND_CHECKBOX);
    }

    // use full height of the line, same as win32 listctrl
    int ix, iy;
    GetImageSize(ix, iy);
    rect.width = ix;

    return rect;
}

wxRect wxListMainWindow::GetLineHighlightRect(size_t line) const
{
    return InReportView() ? GetLineRect(line)
                          : GetLine(line)->m_gi->m_rectHighlight;
}

long wxListMainWindow::HitTestLine(size_t line, int x, int y) const
{
    wxASSERT_MSG( line < GetItemCount(), wxT("invalid line in HitTestLine") );

    if ( IsInsideCheckBox(line, x, y) )
        return wxLIST_HITTEST_ONITEMSTATEICON;

    wxListLineData *ld = GetLine(line);

    if ( ld->HasImage() && GetLineIconRect(line).Contains(x, y) )
        return wxLIST_HITTEST_ONITEMICON;

    // VS: Testing for "ld->HasText() || InReportView()" instead of
    //     "ld->HasText()" is needed to make empty lines in report view
    //     possible
    if ( ld->HasText() || InReportView() )
    {
        wxRect rect = InReportView() ? GetLineRect(line)
                                     : GetLineLabelRect(line);

        if ( rect.Contains(x, y) )
            return wxLIST_HITTEST_ONITEMLABEL;
    }

    return 0;
}

// ----------------------------------------------------------------------------
// highlight (selection) handling
// ----------------------------------------------------------------------------

bool wxListMainWindow::IsHighlighted(size_t line) const
{
    if ( IsVirtual() )
    {
        return m_selStore.IsSelected(line);
    }
    else // !virtual
    {
        wxListLineData *ld = GetLine(line);
        wxCHECK_MSG( ld, false, wxT("invalid index in IsHighlighted") );

        return ld->IsHighlighted();
    }
}

bool wxListMainWindow::HighlightLines( size_t lineFrom,
                                       size_t lineTo,
                                       bool highlight,
                                       SendEvent sendEvent )
{
    // It is safe to swap the bounds here if they are not in order.
    if ( lineFrom > lineTo )
        wxSwap(lineFrom, lineTo);

    if ( IsVirtual() )
    {
        wxArrayInt linesChanged;
        if ( !m_selStore.SelectRange(lineFrom, lineTo, highlight,
                                     &linesChanged) )
        {
            // meny items changed state, refresh everything
            RefreshLines(lineFrom, lineTo);
        }
        else // only a few items changed state, refresh only them
        {
            size_t count = linesChanged.GetCount();
            for ( size_t n = 0; n < count; n++ )
            {
                RefreshLine(linesChanged[n]);
            }
        }
    }
    else // iterate over all items in non report view
    {
        const wxWeakRef<wxWindow> weakThis(this);
        const std::uint64_t structureRevision = m_structureRevision;

        for ( size_t line = lineFrom; line <= lineTo; line++ )
        {
            const bool changed = HighlightLine(line, highlight, sendEvent);
            if ( weakThis.get() != this ||
                    m_structureRevision != structureRevision )
            {
                return false;
            }

            if ( changed )
                RefreshLine(line);
        }
    }

    return true;
}

bool wxListMainWindow::HighlightLine( size_t line, bool highlight, SendEvent sendEvent )
{
    bool changed;

    if ( IsVirtual() )
    {
        changed = m_selStore.SelectItem(line, highlight);
    }
    else // !virtual
    {
        wxListLineData *ld = GetLine(line);
        wxCHECK_MSG( ld, false, wxT("invalid index in HighlightLine") );

        changed = ld->Highlight(highlight);
    }

    if ( changed && sendEvent )
    {
        SendNotify( line, highlight ? wxEVT_LIST_ITEM_SELECTED
                                    : wxEVT_LIST_ITEM_DESELECTED );
    }

    return changed;
}

void wxListMainWindow::RefreshLine( size_t line )
{
    if ( InReportView() )
    {
        size_t visibleFrom, visibleTo;
        GetVisibleLinesRange(&visibleFrom, &visibleTo);

        if ( line < visibleFrom || line > visibleTo )
            return;
    }

    wxRect rect = GetLineRect(line);

    GetListCtrl()->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );
    RefreshRect( rect );
}

void wxListMainWindow::RefreshLines( size_t lineFrom, size_t lineTo )
{
    // we suppose that they are ordered by caller
    wxASSERT_MSG( lineFrom <= lineTo, wxT("indices in disorder") );

    wxASSERT_MSG( lineTo < GetItemCount(), wxT("invalid line range") );

    if ( InReportView() )
    {
        size_t visibleFrom, visibleTo;
        GetVisibleLinesRange(&visibleFrom, &visibleTo);

        if ( lineFrom > visibleTo || lineTo < visibleFrom )
        {
            // None of these lines are currently visible at all, don't bother
            // doing anything.
            return;
        }

        if ( lineFrom < visibleFrom )
            lineFrom = visibleFrom;
        if ( lineTo > visibleTo )
            lineTo = visibleTo;

        wxRect rect;
        rect.x = 0;
        rect.y = GetLineY(lineFrom);
        rect.width = GetClientSize().x;
        rect.height = GetLineY(lineTo) - rect.y + GetLineHeight();

        GetListCtrl()->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );
        RefreshRect( rect );
    }
    else // !report
    {
        // TODO: this should be optimized...
        for ( size_t line = lineFrom; line <= lineTo; line++ )
        {
            RefreshLine(line);
        }
    }
}

void wxListMainWindow::RefreshAfter( size_t lineFrom )
{
    if ( InReportView() )
    {
        // Note that we don't compare lineFrom with the last visible line
        // because we refresh the entire rectangle below it anyhow, so it
        // doesn't matter if it's bigger than it. And we must still refresh
        // even if lineFrom is invalid because it may have been (just) deleted
        // when we're called from DeleteItem().
        size_t visibleFrom;
        GetVisibleLinesRange(&visibleFrom, nullptr);

        if ( lineFrom < visibleFrom )
            lineFrom = visibleFrom;

        wxRect rect;
        rect.x = 0;
        rect.y = GetLineY(lineFrom);
        GetListCtrl()->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

        wxSize size = GetClientSize();
        rect.width = size.x;

        // refresh till the bottom of the window
        rect.height = size.y - rect.y;

        RefreshRect( rect );
    }
    else // !report
    {
        // TODO: how to do it more efficiently?
        m_dirty = true;
    }
}

void wxListMainWindow::RefreshSelected()
{
    if ( IsEmpty() )
        return;

    size_t from, to;
    if ( InReportView() )
    {
        GetVisibleLinesRange(&from, &to);
    }
    else // !virtual
    {
        from = 0;
        to = GetItemCount() - 1;
    }

    if ( HasCurrent() && m_current >= from && m_current <= to )
        RefreshLine(m_current);

    for ( size_t line = from; line <= to; line++ )
    {
        // NB: the test works as expected even if m_current == -1
        if ( line != m_current && IsHighlighted(line) )
            RefreshLine(line);
    }
}

void wxListMainWindow::UpdateAccessibleItems()
{
    wxPrivate::AccessibleRows rows;

    const size_t count = GetItemCount();
    if ( count )
    {
        const bool inReportView = InReportView();

        // Only the shown items are exposed: the control may have thousands of
        // them and the elements of the ones which are not shown would be
        // useless anyhow, as their rectangles would be outside of it.
        size_t visibleFrom, visibleTo;
        if ( inReportView )
        {
            GetVisibleLinesRange(&visibleFrom, &visibleTo);
        }
        else // There is no simple way to find the visible items in this case.
        {
            visibleFrom = 0;
            visibleTo = count - 1;
        }

        const wxRect rectClient(GetClientSize());
        const int numCols = inReportView ? GetColumnCount() : 1;

        for ( size_t line = visibleFrom; line <= visibleTo; ++line )
        {
            wxRect rect;
            GetItemRect(line, rect);
            if ( !inReportView && !rect.Intersects(rectClient) )
                continue;

            wxPrivate::AccessibleRow row(line, rect);
            row.selected = IsHighlighted(line);

            for ( int col = 0; col < numCols; ++col )
            {
                // Only the report view has real columns, in the other ones the
                // single value is the item label.
                wxRect rectCell;
                if ( inReportView )
                {
                    GetSubItemRect(line, col, rectCell);
                }
                else
                {
                    rectCell = GetLineLabelRect(line);
                    GetListCtrl()->CalcScrolledPosition(rectCell.x, rectCell.y,
                                                        &rectCell.x, &rectCell.y);
                }

                row.cells.emplace_back(GetItemText(line, col), rectCell);
            }

            rows.push_back(row);
        }
    }

    wxPrivate::SetAccessibleTable(this, rows, count);

    // Announce the current item if it has changed, as the screen readers have
    // no other way of knowing that the user has moved to another one.
    const long current = HasCurrent() ? (long)m_current : -1;
    if ( current != m_lastAccessibleCurrent )
    {
        m_lastAccessibleCurrent = current;

        wxPrivate::SetAccessibleCurrentRow(this, current);
    }
}

void wxListMainWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    // Note: a wxPaintDC must be constructed even if no drawing is
    // done (a Windows requirement).
    wxPaintDC dc( this );
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( GetListCtrl()->m_winUIProjection &&
         GetListCtrl()->m_winUIProjection->OwnsWindow(this) ) return;
#endif

    if ( IsEmpty() )
    {
        // Remove the accessible elements for the previously shown items.
        UpdateAccessibleItems();

        // nothing to draw or not the moment to draw it
        return;
    }

    if ( m_dirty )
    {
        // Calling Refresh() from inside OnPaint() doesn't work under macOS, so
        // don't do it immediately...
        RecalculatePositions();

        // ... but schedule it for later.
        CallAfter(&wxWindow::Refresh, true, (const wxRect*)nullptr);

        // Don't bother redoing the relayout again the next time nor redrawing
        // now, as we'll be refresh soon anyhow.
        m_dirty = false;
        return;
    }

    const wxPoint origin(GetListCtrl()->CalcScrolledPosition(wxPoint(0, 0)));

    if ( InReportView() )
    {
        int lineHeight = GetLineHeight();

        size_t visibleFrom, visibleTo;
        const size_t linesPerPage = (unsigned int) m_linesPerPage;
        GetVisibleLinesRange(&visibleFrom, &visibleTo);

        // We may need to iterate beyond visibleTo if we want to draw striped
        // background across the entire window.
        size_t visibleEnd;
        wxColour colAlt;
        if ( m_extendRulesAndAlternateColour )
        {
            colAlt = GetListCtrl()->GetAlternateRowColour();
            visibleEnd = wxMax(linesPerPage, visibleTo);
        }
        else
        {
            visibleEnd = visibleTo;
        }

        // tell the caller cache to cache the data
        if ( IsVirtual() )
        {
            wxListEvent evCache(wxEVT_LIST_CACHE_HINT,
                                GetParent()->GetId());
            evCache.SetEventObject( GetParent() );
            evCache.m_oldItemIndex = visibleFrom;
            evCache.m_item.m_itemId =
            evCache.m_itemIndex = visibleTo;
            GetParent()->GetEventHandler()->ProcessEvent( evCache );
        }

        wxRect rectLine(GetLineRect(visibleFrom));
        rectLine.Offset(origin);

        for (size_t line = visibleFrom;
            line <= visibleEnd;
            line++, rectLine.y += rectLine.height)
        {
            if ( !IsExposed(rectLine) )
            {
                // don't redraw unaffected lines to avoid flicker
                continue;
            }

            if ( line > visibleTo )
            {
                // We only iterate beyond visibleTo when we have to draw the
                // odd rows background, so do this if needed.
                if ( line % 2 )
                {
                    dc.SetBrush(colAlt);
                    dc.SetPen(*wxTRANSPARENT_PEN);
                    dc.DrawRectangle(rectLine);
                }

                // But don't do anything else, as there is no valid item.
                continue;
            }

            GetLine(line)->DrawInReportMode( &dc,
                                             rectLine,
                                             IsHighlighted(line),
                                             line == m_current,
                                             IsItemChecked(line) );
        }

        if ( HasFlag(wxLC_HRULES) )
        {
            wxPen pen(GetRuleColour(), 1, wxPENSTYLE_SOLID);
            wxSize clientSize = GetClientSize();

            size_t i = visibleFrom;
            if (i == 0) i = 1; // Don't draw the first one

            int y = (i - visibleFrom) * lineHeight;
            for ( ; i <= visibleEnd; i++, y += lineHeight )
            {
                dc.SetPen(pen);
                dc.SetBrush( *wxTRANSPARENT_BRUSH );
                dc.DrawLine(0, y, clientSize.x, y);
            }

            // Draw last horizontal rule
            if ( visibleEnd == GetItemCount() - 1 )
            {
                dc.SetPen( pen );
                dc.SetBrush( *wxTRANSPARENT_BRUSH );
                dc.DrawLine(0, y, clientSize.x, y);
            }
        }

        // Draw vertical rules if required
        if ( HasFlag(wxLC_VRULES) &&
                (m_extendRulesAndAlternateColour || !IsEmpty()) )
        {
            wxPen pen(GetRuleColour(), 1, wxPENSTYLE_SOLID);
            wxRect firstItemRect, lastItemRect;

            GetItemRect(visibleFrom, firstItemRect);
            GetItemRect(visibleTo, lastItemRect);
            int x = rectLine.x;
            dc.SetPen(pen);
            dc.SetBrush(* wxTRANSPARENT_BRUSH);

            int clientHeight, clientWidth;
            GetSize( &clientWidth, &clientHeight );

            for (int order = 0; order < GetColumnCount(); ++order)
            {
                const int col = GetColumnIndexFromOrder(order);
                int colWidth = GetColumnWidth(col);
                x += colWidth;
                int x_pos = x;
                if (order < GetColumnCount()-1) x_pos -= 2;

                int ruleHeight = m_extendRulesAndAlternateColour && visibleEnd > visibleTo
                                    ? clientHeight
                                    : lastItemRect.GetBottom() + 1;

                dc.DrawLine(x_pos, firstItemRect.GetY() - 1, x_pos, ruleHeight);
            }
        }
    }
    else // !report
    {
        size_t count = GetItemCount();
        for ( size_t i = 0; i < count; i++ )
        {
            GetLine(i)->Draw( &dc, origin, i == m_current );
        }
    }

    // DrawFocusRect() is unusable under Mac, it draws outside of the highlight
    // rectangle somehow and so leaves traces when the item is not selected any
    // more, see #12229.
#ifndef __WXMAC__
    if ( HasCurrent() )
    {
        int flags = 0;
        if ( IsHighlighted(m_current) )
            flags |= wxCONTROL_SELECTED;

        wxRect rect(GetLineHighlightRect(m_current));
        rect.Offset(origin);

        wxRendererNative::Get().DrawFocusRect(this, dc, rect, flags);
    }
#endif // !__WXMAC__

    // The items we've just drawn may be different from the previous ones, e.g.
    // because the control was scrolled, so refresh the elements using them.
    UpdateAccessibleItems();
}

void wxListMainWindow::OnSysColourChanged( wxSysColourChangedEvent &event )
{
    SetOwnForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT));
    SetOwnBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));

    if ( m_highlightBrush )
    {
        m_highlightBrush->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    }

    if ( m_highlightUnfocusedBrush )
    {
        m_highlightUnfocusedBrush->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
    }

    Refresh();

    event.Skip();
}

bool wxListMainWindow::HighlightAll( bool on )
{
    if ( IsSingleSel() )
    {
        wxASSERT_MSG( !on, wxT("can't do this in a single selection control") );

        // we just have one item to turn off
        if ( HasCurrent() && IsHighlighted(m_current) )
        {
            const size_t current = m_current;
            const wxWeakRef<wxWindow> weakThis(this);
            const std::uint64_t structureRevision = m_structureRevision;

            HighlightLine(current, false);
            if ( weakThis.get() != this ||
                    m_structureRevision != structureRevision )
            {
                return false;
            }

            RefreshLine(current);
        }
    }
    else // multi selection
    {
        if ( !IsEmpty() && !HighlightLines(0, GetItemCount() - 1, on) )
            return false;
    }

    return true;
}

void wxListMainWindow::HighlightOnly( size_t line, size_t oldLine )
{
    const unsigned selCount = GetSelectedItemCount();

    if ( selCount == 1 && IsHighlighted(line) )
    {
        return; // Nothing changed.
    }

    if ( oldLine != wxNO_ITEM )
    {
        IsHighlighted(oldLine) ? ReverseHighlight(oldLine)
                               : RefreshLine(oldLine); // refresh the old focus to remove it
    }

    // Deselect the remaining items we may have when using multiple selection
    // (for single selection the only selected item is always the highlighted
    // one, so we would have already returned above).
    if ( selCount != 0 )
    {
        // Deselecting many items at once will generate wxEVT_XXX_DESELECTED event
        // for each one of them. although this may be inefficient if the number of
        // deselected items is too much, we keep doing this (for non-virtual list
        // controls) for backward compatibility concerns. For virtual listctrl (in
        // multi-selection mode), wxMSW sends only a notification to indicate that
        // something has been deselected. Notice that to be fully compatible with
        // wxMSW behaviour, _line_ shouldn't be deselected if it was selected.

        const SendEvent sendEvent = IsVirtual() ? SendEvent_None : SendEvent_Normal;

        size_t lineFrom = 0,
               lineTo   = GetItemCount() - 1;

        if ( line > lineFrom && line < lineTo )
        {
            HighlightLines(lineFrom, line - 1, false, sendEvent);
            HighlightLines(line + 1, lineTo, false, sendEvent);
        }
        else // _line_ is equal to lineFrom or lineTo
        {
            line == lineFrom ? ++lineFrom : --lineTo;
            HighlightLines(lineFrom, lineTo, false, sendEvent);
        }

        // If we didn't send the event for individual items above, send it for all of them now.
        if ( sendEvent == SendEvent_None )
            SendNotify(wxNO_ITEM, wxEVT_LIST_ITEM_DESELECTED);
    }

    // _line_ should be the only selected item.
    HighlightLine(line);
    // refresh the new focus to add it.
    RefreshLine(line);
}

void wxListMainWindow::OnChildFocus(wxChildFocusEvent& WXUNUSED(event))
{
    // Do nothing here.  This prevents the default handler in wxScrolledWindow
    // from needlessly scrolling the window when the edit control is
    // dismissed.  See ticket #9563.
}

wxPoint wxListMainWindow::GetPositionInListCtrl() const
{
    // Don't use GetPosition() as it is only correct after the parent window
    // is laid out, while this function can be called before this happens, e.g.
    // by GetItemRect() called immediately after creating the control. We know
    // that this window is just below the header, if any, so use its height.
    const wxListHeaderWindow* const header = GetListCtrl()->m_headerWin;
    return wxPoint(0, header ? header->GetSize().y : 0);
}

void wxListMainWindow::SendNotify( size_t line,
                                   wxEventType command,
                                   const wxPoint& point )
{
    wxGenericListCtrl* const listctrl = GetListCtrl();

    wxListEvent le( command, listctrl->GetId() );
    le.SetEventObject( listctrl );

    le.m_item.m_itemId =
    le.m_itemIndex = line;

    // set only for events which have position
    if ( point != wxDefaultPosition )
    {
        // the position should be relative to the parent window, not
        // this one for compatibility with MSW and common sense: the
        // user code doesn't know anything at all about this window,
        // so why should it get positions relative to it?
        le.m_pointDrag = GetPositionInListCtrl() + point;
    }

    // provide information about the (first column of the) item in the event if
    // we have a valid item and any columns at all
    if ( line != wxNO_ITEM && listctrl->GetColumnCount() )
    {
        GetLine(line)->GetItem( 0, le.m_item );
    }

    listctrl->GetEventHandler()->ProcessEvent( le );
}

bool wxListMainWindow::ChangeCurrentWithoutEvent(size_t current)
{
    if ( current == m_current )
    {
        return false; // Nothing changed!
    }

    m_current = current;

    // as the current item changed, we shouldn't start editing it when the
    // "slow click" timer expires as the click happened on another item
    if ( m_renameTimer->IsRunning() )
        m_renameTimer->Stop();

    return true;
}

void wxListMainWindow::ChangeCurrent(size_t current)
{
    if ( ChangeCurrentWithoutEvent(current) )
        SendNotify(current, wxEVT_LIST_ITEM_FOCUSED);
}

wxTextCtrl *wxListMainWindow::EditLabel(long item, wxClassInfo* textControlClass)
{
    // Calling this function for control without wxLC_EDIT_LABELS flag set
    // is not portable. i.e. on wxMSW this function cannot edit labels if
    // the flag is not already set on the control.
    wxASSERT_MSG( HasFlag(wxLC_EDIT_LABELS),
                 "should only be called if wxLC_EDIT_LABELS flag is set");

    wxCHECK_MSG( (item >= 0) && ((size_t)item < GetItemCount()), nullptr,
                 wxT("wrong index in wxGenericListCtrl::EditLabel()") );

    wxASSERT_MSG( textControlClass->IsKindOf(wxCLASSINFO(wxTextCtrl)),
                 wxT("EditLabel() needs a text control") );

    size_t itemEdit = (size_t)item;
    // Never replace an existing pushed editor handler from a nested BEGIN.
    if ( m_textctrlWrapper ) return nullptr;
    const wxWeakRef<wxListMainWindow> weakThis(this);
    const auto structure = m_structureRevision;

    wxListEvent le( wxEVT_LIST_BEGIN_LABEL_EDIT, GetParent()->GetId() );
    le.SetEventObject( GetParent() );
    le.m_item.m_itemId =
    le.m_itemIndex = item;
    if ( !GetLabelEditItem(itemEdit, le.m_item) || !weakThis ) return nullptr;
    wxRect labelRect;
    if ( !GetLabelEditRect(itemEdit, le.m_item, labelRect) || !weakThis ||
         m_structureRevision != structure || m_textctrlWrapper ) return nullptr;

    // See comment in EditLabel() (src/msw/listctrl.cpp) for why we create the
    // text control before sending the wxEVT_LIST_BEGIN_LABEL_EDIT event.
    wxTextCtrl * const text = (wxTextCtrl *)textControlClass->CreateObject();
    if ( !weakThis || m_structureRevision != structure || m_textctrlWrapper )
    {
        delete text;
        return nullptr;
    }
    auto* const wrapper = new wxListTextCtrlWrapper(this, text, item, le.m_item.GetText());
    const wxWeakRef<wxListTextCtrlWrapper> weakWrapper(wrapper);
    m_textctrlWrapper = wrapper;
    const auto current = [&]
    { return weakThis && weakWrapper && m_textctrlWrapper == wrapper && wrapper->IsItemValid(); };
    const auto discard = [&]
    {
        if ( weakThis && weakWrapper && m_textctrlWrapper == wrapper )
            wrapper->EndEdit(wxListTextCtrlWrapper::End_Destroy);
    };
    if ( !wrapper->CreateControl(labelRect) || !current() )
    { discard(); return nullptr; }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The label gives way to its editor, which lets what is behind it show
    // through: the native row is shown without its text for as long as the
    // edit lasts.
    RefreshLine(itemEdit);
#endif

    GetParent()->GetEventHandler()->ProcessEvent(le);
    if ( !current() || !le.IsAllowed() )
    { discard(); return nullptr; }

    if ( m_dirty )
    {
        // Ensure the display is updated before we start editing.
        Update();
    }
    if ( !current() ) { discard(); return nullptr; }
    return wrapper->GetText();
}

bool wxListMainWindow::GetLabelEditItem(size_t item, wxListItem& info) const
{
    if ( item >= GetItemCount() ) return false;
    const wxWeakRef<wxListMainWindow> weakThis(const_cast<wxListMainWindow*>(this));
    const auto structure = m_structureRevision;
    const auto current = [&] { return weakThis && m_structureRevision == structure; };
    if ( !IsVirtual() )
    {
        if ( m_lines[item].m_items.empty() ) return false;
        m_lines[item].GetItem(0, info);
        return true;
    }
    // Label events need only column zero. Do not populate the generic dummy
    // line across arbitrary OnGet callbacks or read all hidden columns.
    auto* const list = GetListCtrl();
    const wxString text = list->OnGetItemText(item, 0);
    if ( !current() ) return false;
    const int image = list->OnGetItemColumnImage(item, 0);
    if ( !current() ) return false;
    const auto* const attr = list->OnGetItemAttr(item);
    if ( !current() ) return false;
    info.m_text = text;
    info.m_image = image;
    if ( attr )
    {
        if ( attr->HasTextColour() ) info.SetTextColour(attr->GetTextColour());
        if ( attr->HasBackgroundColour() ) info.SetBackgroundColour(attr->GetBackgroundColour());
        if ( attr->HasFont() ) info.SetFont(attr->GetFont());
    }
    return true;
}

bool wxListMainWindow::GetLabelEditRect(size_t item, const wxListItem& info, wxRect& rect) const
{
    if ( item >= GetItemCount() ) return false;
    const wxWeakRef<wxListMainWindow> weakThis(const_cast<wxListMainWindow*>(this));
    const auto structure = m_structureRevision;
    if ( InReportView() )
    {
        if ( GetColumnCount() == 0 ) return false;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        // From the edge of the cell, see wxListEditorRect().
        int offset = 0;
#else
        int offset = ICON_OFFSET_X;
#endif
        if ( HasCheckBoxes() )
        {
            const auto size = wxRendererNative::Get().GetCheckBoxSize(const_cast<wxListMainWindow*>(this));
            if ( !weakThis || m_structureRevision != structure ) return false;
            offset += size.x + 2 * MARGIN_AROUND_CHECKBOX;
        }
        if ( info.GetImage() >= 0 )
        {
            int width, height;
            GetImageSize(width, height);
            if ( !weakThis || m_structureRevision != structure ) return false;
            offset += width;
        }
        rect = GetLineRect(item);
        rect.x += GetColumnStart(0) + offset;
        rect.width = wxMax(0, GetColumnWidth(0) - offset);
    }
    else rect = GetLineLabelRect(item);
    if ( !weakThis || m_structureRevision != structure ) return false;
    GetListCtrl()->CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
    return true;
}

bool wxListMainWindow::EndEditLabel(bool cancel)
{
    if (!m_textctrlWrapper)
    {
        return false;
    }

    m_textctrlWrapper->EndEdit(cancel ? wxListTextCtrlWrapper::End_Discard : wxListTextCtrlWrapper::End_Accept);
    return true;
}

void wxListMainWindow::OnRenameTimer()
{
    wxCHECK_RET( HasCurrent(), wxT("unexpected rename timer") );

    EditLabel( m_current );
}

bool wxListMainWindow::OnRenameAccept(size_t itemEdit, const wxString& value)
{
    const wxWeakRef<wxListMainWindow> weakThis(this);
    wxListEvent le( wxEVT_LIST_END_LABEL_EDIT, GetParent()->GetId() );
    le.SetEventObject( GetParent() );
    le.m_item.m_itemId =
    le.m_itemIndex = itemEdit;

    auto* const wrapper = m_textctrlWrapper;
    if ( !wrapper || !wrapper->IsItemValid() ||
         !GetLabelEditItem(itemEdit, le.m_item) || !weakThis ) return false;
    if ( m_textctrlWrapper != wrapper || !wrapper->IsItemValid() ) return false;
    le.m_item.m_text = value;

    GetParent()->GetEventHandler()->ProcessEvent( le );

    if ( !weakThis || m_textctrlWrapper != wrapper || !wrapper->IsItemValid() || !le.IsAllowed() )
        return false;

    m_dirty = true;

    return true;
}

void wxListMainWindow::OnRenameCancelled(size_t itemEdit)
{
    const wxWeakRef<wxListMainWindow> weakThis(this);
    // let owner know that the edit was cancelled
    wxListEvent le( wxEVT_LIST_END_LABEL_EDIT, GetParent()->GetId() );

    le.SetEditCanceled(true);

    le.SetEventObject( GetParent() );
    le.m_item.m_itemId =
    le.m_itemIndex = itemEdit;

    if ( !GetLabelEditItem(itemEdit, le.m_item) || !weakThis ) return;
    GetEventHandler()->ProcessEvent( le );
}

void wxListMainWindow::OnFindTimer()
{
    m_findPrefix.clear();
    if ( m_findBell == -1 )
        m_findBell = 1;
}

void wxListMainWindow::EnableBellOnNoMatch( bool on )
{
    m_findBell = on;
}

void wxListMainWindow::OnMouse( wxMouseEvent &event )
{
#ifdef __WXMAC__
    // On wxMac we can't depend on the EVT_KILL_FOCUS event to properly
    // shutdown the edit control when the mouse is clicked elsewhere on the
    // listctrl because the order of events is different (or something like
    // that), so explicitly end the edit if it is active.
    if ( event.LeftDown() && m_textctrlWrapper )
        m_textctrlWrapper->EndEdit(wxListTextCtrlWrapper::End_Accept);
#endif // __WXMAC__

    if ( event.LeftDown() )
    {
        // Ensure we skip the event to let the system set focus to this window.
        event.Skip();
    }

    // Pretend that the event happened in wxListCtrl itself, which means that
    // its position must be in wxListCtrl coordinates too.
    wxMouseEvent me(event);
    me.SetEventObject( GetParent() );
    me.SetId(GetParent()->GetId());
    me.SetPosition(GetPositionInListCtrl() + event.GetPosition());
    if ( GetParent()->GetEventHandler()->ProcessEvent( me ))
        return;

    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
        // let the base class handle mouse wheel events.
        event.Skip();
        return;
    }

    if ( !HasCurrent() || IsEmpty() )
    {
        if (event.RightDown())
        {
            SendNotify( wxNO_ITEM, wxEVT_LIST_ITEM_RIGHT_CLICK, event.GetPosition() );

            wxContextMenuEvent evtCtx(wxEVT_CONTEXT_MENU,
                                      GetParent()->GetId(),
                                      ClientToScreen(event.GetPosition()));
            evtCtx.SetEventObject(GetParent());
            GetParent()->GetEventHandler()->ProcessEvent(evtCtx);
        }

        if ( IsEmpty() )
            return;

        // Continue processing...
    }

    if (m_dirty)
        return;

    if ( !(event.Dragging() || event.ButtonDown() || event.LeftUp() ||
        event.ButtonDClick()) )
        return;

    int x = event.GetX();
    int y = event.GetY();
    GetListCtrl()->CalcUnscrolledPosition( x, y, &x, &y );

    // where did we hit it (if we did)?
    long hitResult = 0;

    size_t count = GetItemCount(),
           current;

    if ( InReportView() )
    {
        current = y / GetLineHeight();
        if ( current < count )
            hitResult = HitTestLine(current, x, y);
    }
    else // !report
    {
        // TODO: optimize it too! this is less simple than for report view but
        //       enumerating all items is still not a way to do it!!
        for ( current = 0; current < count; current++ )
        {
            hitResult = HitTestLine(current, x, y);
            if ( hitResult )
                break;
        }
    }

    // Update drag events counter first as we must do it even if the mouse is
    // not on any item right now as we must keep count in case we started
    // dragging from the empty control area but continued to do it over a valid
    // item -- in this situation we must not start dragging this item.
    if (event.Dragging())
        m_dragCount++;
    else
        m_dragCount = 0;

    // The only mouse events that can be generated without any valid item are
    // wxEVT_LIST_ITEM_DESELECTED for virtual lists, and
    // wxEVT_LIST_ITEM_RIGHT_CLICK as it can be useful to have a global
    // popup menu for the list control itself which should be shown even when
    // the user clicks outside of any item.
    if ( !hitResult )
    {
        // outside of any item
        if (event.RightDown())
        {
            wxContextMenuEvent evtCtx(
                wxEVT_CONTEXT_MENU,
                GetParent()->GetId(),
                ClientToScreen(event.GetPosition()));
            evtCtx.SetEventObject(GetParent());
            GetParent()->GetEventHandler()->ProcessEvent(evtCtx);
        }
        else if (event.LeftDown())
        {
            // reset the selection in multi-selection mode only when Ctrl and
            // Shift keys are not pressed to mimic MSW behaviour
            if ( IsSingleSel() || !(event.ControlDown() || event.ShiftDown()) )
            {
                // reset the selection and bail out
                if ( !HighlightAll(false) )
                    return;
                // generate a DESELECTED event for
                // virtual multi-selection lists
                if ( IsVirtual() && !IsSingleSel() )
                    SendNotify( m_lineLastClicked, wxEVT_LIST_ITEM_DESELECTED );
            }
        }

        return;
    }

    if ( event.Dragging() )
    {
        if (m_dragCount == 1)
        {
            // we have to report the raw, physical coords as we want to be
            // able to call HitTest(event.m_pointDrag) from the user code to
            // get the item being dragged
            m_dragStart = event.GetPosition();
        }

        if (m_dragCount != 3)
            return;

        int command = event.RightIsDown() ? wxEVT_LIST_BEGIN_RDRAG
                                          : wxEVT_LIST_BEGIN_DRAG;

        SendNotify( m_lineLastClicked, command, m_dragStart );

        return;
    }

    bool forceClick = false;
    if (event.ButtonDClick())
    {
        if ( m_renameTimer->IsRunning() )
            m_renameTimer->Stop();

        m_lastOnSame = false;

        if ( current == m_lineLastClicked )
        {
            SendNotify( current, wxEVT_LIST_ITEM_ACTIVATED );

            return;
        }
        else
        {
            // The first click was on another item, so don't interpret this as
            // a double click, but as a simple click instead
            forceClick = true;
        }
    }

    if (event.LeftUp())
    {
        if (m_lineSelectSingleOnUp != wxNO_ITEM)
        {
            // select single line
            HighlightOnly(m_lineSelectSingleOnUp);
        }

        if (m_lastOnSame)
        {
            if ((current == m_current) &&
                (hitResult == wxLIST_HITTEST_ONITEMLABEL) &&
                HasFlag(wxLC_EDIT_LABELS) )
            {
                if ( !InReportView() ||
                        GetLineLabelRect(current).Contains(x, y) )
                {
                    int dclick = wxSystemSettings::GetMetric(wxSYS_DCLICK_MSEC);
                    m_renameTimer->Start(dclick > 0 ? dclick : 250, true);
                }
            }

            m_lastOnSame = false;
        }

        m_lineSelectSingleOnUp = wxNO_ITEM;
    }
    else
    {
        // This is necessary, because after a DnD operation in
        // from and to ourself, the up event is swallowed by the
        // DnD code. So on next non-up event (which means here and
        // now) m_lineSelectSingleOnUp should be reset.
        m_lineSelectSingleOnUp = wxNO_ITEM;
    }
    if (event.RightDown())
    {
        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        // If the item is already selected, do not update the selection.
        // Multi-selections should not be cleared if a selected item is clicked.
        if (!IsHighlighted(current))
        {
            size_t oldCurrent = m_current;
            ChangeCurrent(current);
            HighlightOnly(m_current, oldCurrent);

            // This item is now the only selected one, so it's also the anchor.
            m_anchor = m_current;
        }

        SendNotify( current, wxEVT_LIST_ITEM_RIGHT_CLICK, event.GetPosition() );

        // Allow generation of context menu event
        event.Skip();
    }
    else if (event.MiddleDown())
    {
        SendNotify( current, wxEVT_LIST_ITEM_MIDDLE_CLICK );
    }
    else if ( event.LeftDown() || forceClick )
    {
        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        size_t oldCurrent = m_current;
        bool oldWasSelected = HasCurrent() && IsHighlighted(m_current);

        bool cmdModifierDown = event.CmdDown();
        if ( IsSingleSel() || !(cmdModifierDown || event.ShiftDown()) )
        {
            if (IsInsideCheckBox(current, x, y))
            {
                CheckItem(current, !IsItemChecked(current));
            }
            else
            {
                if (IsSingleSel() || !IsHighlighted(current))
                {
                    ChangeCurrent(current);
                    HighlightOnly(m_current, oldWasSelected ? oldCurrent : wxNO_ITEM);
                }
                else // multi sel & current is highlighted & no mod keys
                {
                    // Don't change the selection immediately to allow dragging
                    // all the selected items, it will be changed when the
                    // mouse button is released.
                    m_lineSelectSingleOnUp = current;
                    ChangeCurrent(current); // change focus
                }

                // In any case, the clicked item becomes the new anchor. Note
                // that we do it here and not when the mouse button is released
                // because we may not get the button up event at all if the
                // items are dragged.
                m_anchor = m_current;
            }
        }
        else // multi sel & either ctrl or shift is down
        {
            if (cmdModifierDown)
            {
                ChangeCurrent(current);

                ReverseHighlight(m_current);

                // Ctrl-click changes the anchor, as in wxMSW.
                m_anchor = m_current;
            }
            else if (event.ShiftDown())
            {
                ChangeCurrent(current);

                if ( oldCurrent == wxNO_ITEM )
                {
                    // Highlight m_current only if there is no previous selection.
                    HighlightLine(m_current);

                    // And use it as the anchor for extending the selection.
                    m_anchor = m_current;
                }
                else if ( oldCurrent != current )
                {
                    ExtendSelection(oldCurrent, current);
                }
            }
            else // !ctrl, !shift
            {
                // test in the enclosing if should make it impossible
                wxFAIL_MSG( wxT("how did we get here?") );
            }
        }

        if (m_current != oldCurrent && oldCurrent != wxNO_ITEM)
            RefreshLine( oldCurrent );

        // Set the flag telling us whether the next click on this item should
        // start editing its label. This should happen if we clicked on the
        // current item and it was already selected, i.e. if this click was not
        // done to select it.
        //
        // It should not happen if this was a double click (forceClick is true)
        // nor if we hadn't had the focus before as then this click was used to
        // give focus to the control.
        m_lastOnSame = (m_current == oldCurrent) && oldWasSelected &&
                            !forceClick && HasFocus();
    }
}

void wxListMainWindow::MoveToItem(size_t item)
{
    if ( item == wxNO_ITEM )
        return;

    wxRect rect = GetLineRect(item);

    int client_w, client_h;
    GetClientSize( &client_w, &client_h );

    const int hLine = GetLineHeight();

    int viewStartX;
    int viewStartY;
    GetListCtrl()->GetViewStart(&viewStartX, &viewStartY);
    const int view_x = SCROLL_UNIT_X * viewStartX;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The vertical bar counts pixels in report view.
    const int unitY = InReportView() ? 1 : hLine;
    WinUIStopScrolling();
#else
    const int unitY = hLine;
#endif
    const int view_y = unitY * viewStartY;

    if ( InReportView() )
    {
        // the next we need the range of lines shown it might be different,
        // so recalculate it
        ResetVisibleLinesRange();

        if (rect.y < view_y)
            GetListCtrl()->Scroll( -1, rect.y / unitY );
        if (rect.y + rect.height + 5 > view_y + client_h)
        {
            GetListCtrl()->Scroll( -1,
                (rect.y + rect.height - client_h + (unitY == 1 ? 5 : hLine))
                    / unitY );
        }

        // Scroll() can synchronously recalculate this cache before publishing
        // its new wxScrollHelper position. Invalidate it after the operation
        // on every generic port so GetTopItem() has one source of truth.
        ResetVisibleLinesRange();
    }
    else // !report
    {
        int sx = -1,
            sy = -1;

        if (rect.x-view_x < 5)
            sx = (rect.x - 5) / SCROLL_UNIT_X;
        if (rect.x + rect.width - 5 > view_x + client_w)
            sx = (rect.x + rect.width - client_w + SCROLL_UNIT_X) / SCROLL_UNIT_X;

        if (rect.y-view_y < 5)
            sy = (rect.y - 5) / hLine;
        if (rect.y + rect.height - 5 > view_y + client_h)
            sy = (rect.y + rect.height - client_h + hLine) / hLine;

        GetListCtrl()->Scroll(sx, sy);
    }
}

bool wxListMainWindow::ScrollList(int WXUNUSED(dx), int dy)
{
    if ( !InReportView() )
    {
        // TODO: this should work in all views but is not implemented now
        return false;
    }

    size_t top, bottom;
    GetVisibleLinesRange(&top, &bottom);

    if ( bottom == wxNO_ITEM )
        return 0;

    ResetVisibleLinesRange();

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The vertical bar counts pixels.
    WinUIStopScrolling();
    int viewStartY = 0;
    GetListCtrl()->GetViewStart(nullptr, &viewStartY);
    GetListCtrl()->Scroll(-1, wxMax(0, viewStartY + dy));
#else
    int hLine = GetLineHeight();

    GetListCtrl()->Scroll(-1, top + dy / hLine);
#endif

    // See the comment in MoveToItem() for why this happens after Scroll().
    ResetVisibleLinesRange();

    return true;
}

// ----------------------------------------------------------------------------
// keyboard handling
// ----------------------------------------------------------------------------

// Helper function used for extending the selection to the given item in
// multi-selection mode, when selecting using mouse or arrows with Shift key
// down: it makes the selection consist of exactly the items between the anchor
// and the new current item (inclusive), as in wxMSW.
//
// Notice that it ensures that wxEVT_LIST_ITEM_{DE}SELECTED events are only
// generated for the items that really changed their state.
void wxListMainWindow::ExtendSelection(size_t oldCurrent, size_t newCurrent)
{
    // If there is no anchor yet, e.g. because the current item was set
    // programmatically and not by the user, use the old current item as
    // anchor, as this is what the user would expect.
    if ( m_anchor == wxNO_ITEM )
        m_anchor = oldCurrent;

    // Refresh the old/new focus to remove/add it
    RefreshLine(oldCurrent);
    RefreshLine(newCurrent);

    size_t lineFrom = m_anchor,
           lineTo = newCurrent;
    if ( lineFrom > lineTo )
        wxSwap(lineFrom, lineTo);

    // Note that we can't assume that the currently selected items are those
    // between the anchor and the old current item, as the current item could
    // have been changed without changing the selection, e.g. using Ctrl-arrows
    // or Ctrl-click, so deselect all the items outside of the new selection.
    const int selCountOld = GetSelectedItemCount();

    if ( lineFrom > 0 )
        HighlightLines(0, lineFrom - 1, false);

    const size_t count = GetItemCount();
    if ( lineTo < count - 1 )
        HighlightLines(lineTo + 1, count - 1, false);

    // For virtual listctrl (in multi-selection mode), wxMSW sends only
    // a notification to indicate that something has been deselected.
    if ( IsVirtual() && GetSelectedItemCount() != selCountOld )
        SendNotify(wxNO_ITEM, wxEVT_LIST_ITEM_DESELECTED);

    HighlightLines(lineFrom, lineTo);
}

void wxListMainWindow::OnArrowChar(size_t newCurrent, const wxKeyEvent& event)
{
    wxCHECK_RET( newCurrent < (size_t)GetItemCount(),
                 wxT("invalid item index in OnArrowChar()") );

    size_t oldCurrent = m_current;

    ChangeCurrent(newCurrent);

    // in single selection we just ignore Shift as we can't select several
    // items anyhow
    if ( event.ShiftDown() && !IsSingleSel() )
    {
        ExtendSelection(oldCurrent, newCurrent);
    }
    else // !shift
    {
        // all previously selected items are unselected unless ctrl is held in
        // a multi-selection control. in single selection mode we must always
        // have a selected item. note that we use the real Ctrl key here and
        // not Cmd under macOS, where Cmd-arrows have a different meaning.
        if ( !event.RawControlDown() || IsSingleSel() )
        {
            HighlightOnly(m_current, oldCurrent);

            // Update anchor
            m_anchor = m_current;
        }
        else
        {
            // refresh the old/new focus to remove/add it
            RefreshLine(oldCurrent);
            RefreshLine(m_current);
        }
    }

    MoveToFocus();
}

void wxListMainWindow::OnKeyDown( wxKeyEvent &event )
{
    wxWindow *parent = GetParent();

    // propagate the key event upwards
    wxKeyEvent ke(event);
    ke.SetEventObject( parent );
    ke.SetId(GetParent()->GetId());
    if (parent->GetEventHandler()->ProcessEvent( ke ))
        return;

    // send a list event
    wxListEvent le( wxEVT_LIST_KEY_DOWN, parent->GetId() );
    const size_t current = ShouldSendEventForCurrent() ? m_current : wxNO_ITEM;
    le.m_item.m_itemId =
    le.m_itemIndex = current;
    if ( current != wxNO_ITEM )
        GetLine(current)->GetItem( 0, le.m_item );
    le.m_code = event.GetKeyCode();
    le.SetEventObject( parent );
    if (parent->GetEventHandler()->ProcessEvent( le ))
        return;

    event.Skip();
}

void wxListMainWindow::OnKeyUp( wxKeyEvent &event )
{
    wxWindow *parent = GetParent();

    // propagate the key event upwards
    wxKeyEvent ke(event);
    ke.SetEventObject( parent );
    ke.SetId(GetParent()->GetId());
    if (parent->GetEventHandler()->ProcessEvent( ke ))
        return;

    event.Skip();
}

void wxListMainWindow::OnCharHook( wxKeyEvent &event )
{
    if ( m_textctrlWrapper )
    {
        // When an in-place editor is active we should ensure that it always
        // gets the key events that are special to it.
        if ( m_textctrlWrapper->CheckForEndEditKey(event) )
        {
            // Skip the call to wxEvent::Skip() below.
            return;
        }
    }

    event.Skip();
}

void wxListMainWindow::OnChar( wxKeyEvent &event )
{
    wxWindow *parent = GetParent();

    // propagate the char event upwards
    wxKeyEvent ke(event);
    ke.SetEventObject( parent );
    ke.SetId(GetParent()->GetId());
    if (parent->GetEventHandler()->ProcessEvent( ke ))
        return;

    if ( HandleAsNavigationKey(event) )
        return;

    // no item -> nothing to do
    if (!HasCurrent())
    {
        event.Skip();
        return;
    }

    // don't use m_linesPerPage directly as it might not be computed yet
    const int pageSize = GetCountPerPage();
    wxCHECK_RET( pageSize, wxT("should have non zero page size") );

    if (GetLayoutDirection() == wxLayout_RightToLeft)
    {
        if (event.GetKeyCode() == WXK_RIGHT)
            event.m_keyCode = WXK_LEFT;
        else if (event.GetKeyCode() == WXK_LEFT)
            event.m_keyCode = WXK_RIGHT;
    }

    int keyCode = event.GetKeyCode();

#ifdef __WXOSX__
    // Cmd-Up and Cmd-Down are the standard shortcuts for going to the first and
    // the last item under macOS, where the keyboards often don't have Home and
    // End keys at all, so handle them in the same way as those keys.
    if ( event.CmdDown() && (keyCode == WXK_UP || keyCode == WXK_DOWN) )
    {
        if ( !IsEmpty() )
            OnArrowChar(keyCode == WXK_UP ? 0 : GetItemCount() - 1, event);

        return;
    }
#endif // __WXOSX__

    switch ( keyCode )
    {
        case WXK_UP:
            if ( m_current > 0 )
                OnArrowChar( m_current - 1, event );
            break;

        case WXK_DOWN:
            if ( m_current < (size_t)GetItemCount() - 1 )
                OnArrowChar( m_current + 1, event );
            break;

        case WXK_END:
            if (!IsEmpty())
                OnArrowChar( GetItemCount() - 1, event );
            break;

        case WXK_HOME:
            if (!IsEmpty())
                OnArrowChar( 0, event );
            break;

        case WXK_PAGEUP:
            {
                int steps = InReportView() ? pageSize - 1
                                           : m_current % pageSize;

                int index = m_current - steps;
                if (index < 0)
                    index = 0;

                OnArrowChar( index, event );
            }
            break;

        case WXK_PAGEDOWN:
            {
                int steps = InReportView()
                                ? pageSize - 1
                                : pageSize - (m_current % pageSize) - 1;

                size_t index = m_current + steps;
                size_t count = GetItemCount();
                if ( index >= count )
                    index = count - 1;

                OnArrowChar( index, event );
            }
            break;

        case WXK_LEFT:
            if ( !InReportView() )
            {
                int index = m_current - pageSize;
                if (index < 0)
                    index = 0;

                OnArrowChar( index, event );
            }
            break;

        case WXK_RIGHT:
            if ( !InReportView() )
            {
                size_t index = m_current + pageSize;

                size_t count = GetItemCount();
                if ( index >= count )
                    index = count - 1;

                OnArrowChar( index, event );
            }
            break;

        case WXK_SPACE:
            if ( IsSingleSel() )
            {
                if ( event.ControlDown() )
                {
                    ReverseHighlight(m_current);
                }
                else if ( ShouldSendEventForCurrent() ) // normal space press
                {
                    SendNotify( m_current, wxEVT_LIST_ITEM_ACTIVATED );
                }
            }
            else // multiple selection
            {
                // Note that, as in wxMSW, this doesn't change the anchor.
                ReverseHighlight(m_current);
            }
            break;

        case WXK_RETURN:
        case WXK_EXECUTE:
            if ( event.HasModifiers() || !ShouldSendEventForCurrent() )
            {
                event.Skip();
                break;
            }

            SendNotify( m_current, wxEVT_LIST_ITEM_ACTIVATED );
            break;

        default:
            if ( !event.HasModifiers() &&
                 ((keyCode >= '0' && keyCode <= '9') ||
                  (keyCode >= 'a' && keyCode <= 'z') ||
                  (keyCode >= 'A' && keyCode <= 'Z') ||
                  (keyCode == '_') ||
                  (keyCode == '+') ||
                  (keyCode == '*') ||
                  (keyCode == '-')))
            {
                // find the next item starting with the given prefix
                wxChar ch = (wxChar)keyCode;
                size_t item;

                // if the same character is typed multiple times then go to the
                // next entry starting with that character instead of searching
                // for an item starting with multiple copies of this character,
                // this is more useful and is how it works under Windows.
                if ( m_findPrefix.length() == 1 && m_findPrefix[0] == ch )
                {
                    item = PrefixFindItem(m_current, ch);
                }
                else
                {
                    const wxString newPrefix(m_findPrefix + ch);
                    item = PrefixFindItem(m_current, newPrefix);
                    if ( item != wxNO_ITEM )
                        m_findPrefix = newPrefix;
                }

                // also start the timer to reset the current prefix if the user
                // doesn't press any more alnum keys soon -- we wouldn't want
                // to use this prefix for a new item search
                if ( !m_findTimer )
                {
                    m_findTimer = new wxListFindTimer( this );
                }

                // Notice that we should start the timer even if we didn't find
                // anything to make sure we reset the search state later.
                m_findTimer->StartOnce(wxListFindTimer::DELAY);

                // restart timer even when there's no match so bell get's reset
                if ( item != wxNO_ITEM )
                {
                    // Select the found item and go to it.
                    if ( !HighlightAll(false) )
                        return;

                    if ( !SetItemState(
                            item,
                            wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED,
                            wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED) )
                    {
                        return;
                    }

                    EnsureVisible(item);

                    // Reset the bell flag if it had been temporarily disabled
                    // before.
                    if ( m_findBell == -1 )
                        m_findBell = 1;
                }
                else // No such item
                {
                    // Signal it with a bell if enabled.
                    if ( m_findBell == 1 )
                    {
                        ::wxBell();

                        // Disable it for the next unsuccessful match, we only
                        // beep once, this is usually enough and continuing to
                        // do it would be annoying.
                        m_findBell = -1;
                    }
                }
            }
            else
            {
                event.Skip();
            }
    }
}

// ----------------------------------------------------------------------------
// focus handling
// ----------------------------------------------------------------------------

void wxListMainWindow::OnSetFocus( wxFocusEvent &WXUNUSED(event) )
{
    if ( GetParent() )
    {
        wxFocusEvent event( wxEVT_SET_FOCUS, GetParent()->GetId() );
        event.SetEventObject( GetParent() );
        if ( GetParent()->GetEventHandler()->ProcessEvent( event) )
            return;
    }

    // wxGTK sends us EVT_SET_FOCUS events even if we had never got
    // EVT_KILL_FOCUS before which means that we finish by redrawing the items
    // which are already drawn correctly resulting in horrible flicker - avoid
    // it
    if ( !m_hasFocus )
    {
        m_hasFocus = true;

        UpdateCurrent();
        RefreshSelected();
    }
}

void wxListMainWindow::OnKillFocus( wxFocusEvent &WXUNUSED(event) )
{
    if ( GetParent() )
    {
        wxFocusEvent event( wxEVT_KILL_FOCUS, GetParent()->GetId() );
        event.SetEventObject( GetParent() );
        if ( GetParent()->GetEventHandler()->ProcessEvent( event) )
            return;
    }

    m_hasFocus = false;
    RefreshSelected();
}

void wxListMainWindow::DrawImage( int index, wxDC *dc, int x, int y )
{
    if ( HasFlag(wxLC_ICON) && m_normal_images )
    {
        wxDrawImageBitmap(this, *m_normal_images, index, *dc, x, y);
    }
    else if ( HasFlag(wxLC_SMALL_ICON | wxLC_LIST | wxLC_REPORT) && m_small_images )
    {
        wxDrawImageBitmap(this, *m_small_images, index, *dc, x, y);
    }
}

void wxListMainWindow::GetImageSize( int &width, int &height ) const
{
    if ( HasFlag(wxLC_ICON) && m_normal_images )
    {
        m_normal_images->GetImageLogicalSize(this, width, height);
    }
    else if ( HasFlag(wxLC_SMALL_ICON | wxLC_LIST | wxLC_REPORT) && m_small_images )
    {
        m_small_images->GetImageLogicalSize(this, width, height);
    }
    else
    {
        width =
        height = 0;
    }
}


void wxListMainWindow::SetImages( wxWithImages *images, const int which )
{
    m_dirty = true;

    // calc the spacing from the icon size
    int width = 0;

    if ((images) && (images->HasImages()) )
    {
        int height;
        images->GetImageLogicalSize(this, width, height);
    }

    if (which == wxIMAGE_LIST_NORMAL)
    {
        m_normal_images = images;
        m_normal_spacing = width + 8;
    }

    if (which == wxIMAGE_LIST_SMALL)
    {
        m_small_images = images;
        m_small_spacing = width + 14;
        m_lineHeight = 0;  // ensure that the line height will be recalc'd
    }
}

void wxListMainWindow::SetItemSpacing( int spacing, bool isSmall )
{
    m_dirty = true;
    if (isSmall)
        m_small_spacing = spacing;
    else
        m_normal_spacing = spacing;
}

int wxListMainWindow::GetItemSpacing( bool isSmall )
{
    return isSmall ? m_small_spacing : m_normal_spacing;
}

// ----------------------------------------------------------------------------
// columns
// ----------------------------------------------------------------------------

int
wxListMainWindow::ComputeMinHeaderWidth(const wxListHeaderData* column) const
{
    wxInfoDC dc(const_cast<wxListMainWindow*>(this));

    int width = dc.GetTextExtent(column->GetText()).x + AUTOSIZE_COL_MARGIN;

    width += 2*EXTRA_WIDTH;

    // check for column header's image availability
    const int image = column->GetImage();
    if ( image != -1 )
    {
        if ( m_small_images )
        {
            int ix = 0, iy = 0;
            m_small_images->GetImageLogicalSize(this, ix, iy);
            width += ix + HEADER_IMAGE_MARGIN_IN_REPORT_MODE;
        }
    }

    return width;
}

void wxListMainWindow::SetColumn( int col, const wxListItem &item )
{
    wxCHECK_RET( col >= 0 && col < (int)m_columns.size(),
                 "invalid column index in SetColumn" );

    wxListHeaderData* const column = &m_columns[col];
    column->SetItem( item );

    if ( item.m_width == wxLIST_AUTOSIZE_USEHEADER )
        column->SetWidth(ComputeMinHeaderWidth(column));

    wxListHeaderWindow *headerWin = GetListCtrl()->m_headerWin;
    if ( headerWin )
        headerWin->m_dirty = true;

    m_dirty = true;

    // invalidate it as it has to be recalculated
    m_headerWidth = 0;
}

class wxListCtrlMaxWidthCalculator : public wxMaxWidthCalculatorBase
{
public:
    wxListCtrlMaxWidthCalculator(wxListMainWindow *listmain, unsigned int column)
        : wxMaxWidthCalculatorBase(column),
          m_listmain(listmain)
    {
    }

    virtual void UpdateWithRow(int row) override
    {
        wxListLineData *line = m_listmain->GetLine( row );

        wxListItem item;
        line->m_items.at(GetColumn()).GetItem(item);

        UpdateWithWidth(m_listmain->GetItemWidthWithImage(&item));
    }

private:
    wxListMainWindow* const m_listmain;
};


void wxListMainWindow::SetColumnWidth( int col, int width )
{
    wxCHECK_RET( col >= 0 && col < GetColumnCount(),
                 wxT("invalid column index") );

    wxCHECK_RET( InReportView(),
                 wxT("SetColumnWidth() can only be called in report mode.") );

    m_dirty = true;

    wxListHeaderWindow *headerWin = GetListCtrl()->m_headerWin;
    if ( headerWin )
        headerWin->m_dirty = true;

    wxCHECK_RET( col >= 0 && col < (int)m_columns.size(),
                 "invalid column index in SetColumnWidth" );

    wxListHeaderData* const column = &m_columns[col];

    if ( width == wxLIST_AUTOSIZE_USEHEADER || width == wxLIST_AUTOSIZE )
    {
        wxListCtrlMaxWidthCalculator calculator(this, col);

        calculator.UpdateWithWidth(AUTOSIZE_COL_MARGIN);

        if ( width == wxLIST_AUTOSIZE_USEHEADER )
            calculator.UpdateWithWidth(ComputeMinHeaderWidth(column));

        //  if the cached column width isn't valid then recalculate it
        wxColWidthInfo& widthInfo = m_aColWidths[col];
        if ( widthInfo.bNeedsUpdate )
        {
            size_t first_visible, last_visible;
            GetVisibleLinesRange(&first_visible, &last_visible);

            calculator.ComputeBestColumnWidth(GetItemCount(),
                                              first_visible, last_visible);
            widthInfo.nMaxWidth = calculator.GetMaxWidth();
            widthInfo.bNeedsUpdate = false;
        }
        else
        {
            calculator.UpdateWithWidth(widthInfo.nMaxWidth);
        }

        width = calculator.GetMaxWidth() + AUTOSIZE_COL_MARGIN;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        // The cells of a table of WinUI keep 12 before and after their text.
        width += wxMax(0, FromDIP(2*wxWINUI_TABLE_CELL_PADDING + 2) -
                              AUTOSIZE_COL_MARGIN);
#endif

        if ( col == 0 && HasCheckBoxes() )
        {
            // also account for the space needed by the checkbox
            width += wxRendererNative::Get().GetCheckBoxSize(this).x
                        + 2*MARGIN_AROUND_CHECKBOX;
        }

        // expand the last column to fit the client size
        // only for AUTOSIZE_USEHEADER to mimic MSW behaviour
        if ( (width == wxLIST_AUTOSIZE_USEHEADER) && (col == GetColumnCount() - 1) )
        {
            int margin = GetClientSize().GetX();
            for ( int i = 0; i < col && margin > 0; ++i )
                margin -= m_columns.at(i).GetWidth();

            if ( margin > width )
                width = margin;
        }
    }

    column->SetWidth( width );

    // invalidate it as it has to be recalculated
    m_headerWidth = 0;
}

int wxListMainWindow::GetHeaderWidth() const
{
    if ( !m_headerWidth )
    {
        wxListMainWindow *self = wxConstCast(this, wxListMainWindow);

        size_t count = GetColumnCount();
        for ( size_t col = 0; col < count; col++ )
        {
            self->m_headerWidth += GetColumnWidth(col);
        }
    }

    return m_headerWidth;
}

void wxListMainWindow::GetColumn( int col, wxListItem &item ) const
{
    wxCHECK_RET( col >= 0 && col < (int)m_columns.size(),
                 "invalid column index in GetColumn" );

    m_columns[col].GetItem( item );
}

int wxListMainWindow::GetColumnWidth( int col ) const
{
    wxCHECK_MSG( col >= 0 && col < (int)m_columns.size(), 0,
                 "invalid column index in GetColumnWidth" );

    return m_columns[col].GetWidth();
}

int wxListMainWindow::GetColumnOrder(int col) const
{
    wxCHECK_MSG( col >= 0 && col < GetColumnCount(), wxNOT_FOUND,
                 "invalid column index in GetColumnOrder" );
    return m_columnsOrder.Index(col);
}

int wxListMainWindow::GetColumnIndexFromOrder(int order) const
{
    wxCHECK_MSG( order >= 0 && order < GetColumnCount(), wxNOT_FOUND,
                 "invalid column position in GetColumnIndexFromOrder" );
    return m_columnsOrder[order];
}

int wxListMainWindow::GetColumnStart(int col) const
{
    wxCHECK_MSG( col >= 0 && col < GetColumnCount(), 0,
                 "invalid column index in GetColumnStart" );
    int x = 0;
    for ( const int index : m_columnsOrder )
    {
        if ( index == col )
            return x;
        x += GetColumnWidth(index);
    }
    wxFAIL_MSG("missing column in display order");
    return 0;
}

bool wxListMainWindow::SetColumnsOrder(const wxArrayInt& orders)
{
    if ( !InReportView() || orders.size() != m_columns.size() )
        return false;
    std::vector<bool> seen(m_columns.size(), false);
    for ( const int col : orders )
    {
        if ( col < 0 || static_cast<size_t>(col) >= seen.size() || seen[col] )
            return false;
        seen[col] = true;
    }
    if ( orders == m_columnsOrder )
        return true;

    // Publish the complete permutation before any refresh or editor callback.
    // Logical identities have not changed: retained native peers stay valid.
    m_columnsOrder = orders;
    m_dirty = true;
    Refresh(false);
    if ( auto* const header = GetListCtrl()->m_headerWin )
        header->OnColumnOrderChanged();
    if ( m_textctrlWrapper )
        m_textctrlWrapper->UpdatePosition();
    return true;
}

// ----------------------------------------------------------------------------
// item state
// ----------------------------------------------------------------------------

bool wxListMainWindow::SetItem( wxListItem &item )
{
    long id = item.m_itemId;
    wxCHECK_MSG( id >= 0 && (size_t)id < GetItemCount(), false,
                 wxT("invalid item index in SetItem") );

    if ( !IsVirtual() )
    {
        wxListLineData *line = GetLine((size_t)id);
        line->SetItem( item.m_col, item );
#if defined(__WXWINUI__) && wxUSE_WINUI3
        if ( !InReportView() &&
             ((item.m_mask & (wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE)) ||
              (item.GetAttributes() && item.GetAttributes()->HasFont())) )
        {
            // A retained native item still needs new wx geometry when the
            // filename/font/image changes. Reflow, scrolling and the editor
            // must all use that same updated rectangle.
            m_dirty = true;
            Refresh(false);
        }
#endif

        // Set item state if user wants
        if ( item.m_mask & wxLIST_MASK_STATE )
        {
            if ( !SetItemState(item.m_itemId, item.m_state, item.m_state) )
                return false;
        }

        if (InReportView())
        {
            //  update the Max Width Cache if needed
            int width = GetItemWidthWithImage(&item);

            wxColWidthInfo& widthInfo = m_aColWidths.at(item.m_col);
            if ( width > widthInfo.nMaxWidth )
            {
                widthInfo.nMaxWidth = width;
                widthInfo.bNeedsUpdate = true;
            }
        }
    }

    // update the item on screen unless we're going to update everything soon
    // anyhow
    if ( !m_dirty )
    {
        wxRect rectItem;
        GetItemRect(id, rectItem);
        RefreshRect(rectItem);
    }

    return true;
}

bool wxListMainWindow::SetItemStateAll(long state, long stateMask)
{
    if ( IsEmpty() )
        return true;

    const wxWeakRef<wxWindow> weakThis(this);
    const std::uint64_t structureRevision = m_structureRevision;
    const auto isAlive = [&]()
    {
        return weakThis.get() == this &&
               m_structureRevision == structureRevision;
    };

    // first deal with selection
    if ( stateMask & wxLIST_STATE_SELECTED )
    {
        // set/clear select state
        if ( IsVirtual() )
        {
            // optimized version for virtual listctrl.
            m_selStore.SelectRange(0, GetItemCount() - 1, state == wxLIST_STATE_SELECTED);
            Refresh();
        }
        else if ( state & wxLIST_STATE_SELECTED )
        {
            const long count = GetItemCount();
            for( long i = 0; i <  count; i++ )
            {
                if ( !SetItemState(i, wxLIST_STATE_SELECTED,
                                   wxLIST_STATE_SELECTED) )
                {
                    return false;
                }
            }

        }
        else
        {
            // clear for non virtual (somewhat optimized by using GetNextItem())
            long i = -1;
            while ( (i = GetNextItem(i, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != -1 )
            {
                if ( !SetItemState(i, 0, wxLIST_STATE_SELECTED) )
                    return false;
            }
        }
    }

    if ( HasCurrent() && (state == 0) && (stateMask & wxLIST_STATE_FOCUSED) )
    {
        // unfocus all: only one item can be focussed, so clearing focus for
        // all items is simply clearing focus of the focussed item.
        if ( !SetItemState(m_current, state, stateMask) )
            return false;
    }
    //(setting focus to all items makes no sense, so it is not handled here.)

    return isAlive();
}

bool wxListMainWindow::SetItemState( long litem, long state, long stateMask )
{
    if ( litem == -1 )
        return SetItemStateAll(state, stateMask);

    wxCHECK_MSG( litem >= 0 && (size_t)litem < GetItemCount(), false,
                 wxT("invalid list ctrl item index in SetItem") );

    size_t oldCurrent = m_current;
    size_t item = (size_t)litem;    // safe because of the check above
    const wxWeakRef<wxWindow> weakThis(this);
    const std::uint64_t structureRevision = m_structureRevision;
    const auto isStable = [&]()
    {
        return weakThis.get() == this &&
               m_structureRevision == structureRevision;
    };

    // do we need to change the focus?
    if ( stateMask & wxLIST_STATE_FOCUSED )
    {
        if ( state & wxLIST_STATE_FOCUSED )
        {
            // don't do anything if this item is already focused
            if ( item != m_current )
            {
                ChangeCurrent(item);
                if ( !isStable() )
                    return false;

                // Also make it the anchor for extending the selection: it
                // wouldn't make sense to keep using the old one, which may
                // be far from the new current item and might not be even
                // visible any more.
                m_anchor = item;

                if ( oldCurrent != wxNO_ITEM )
                {
                    if ( IsSingleSel() )
                    {
                        HighlightLine(oldCurrent, false);
                        if ( !isStable() )
                            return false;
                    }

                    RefreshLine(oldCurrent);
                }

                RefreshLine( m_current );
            }
        }
        else // unfocus
        {
            // don't do anything if this item is not focused
            if ( item == m_current )
            {
                ResetCurrent();
                if ( !isStable() )
                    return false;

                if ( IsSingleSel() )
                {
                    // we must unselect the old current item as well or we
                    // might end up with more than one selected item in a
                    // single selection control
                    HighlightLine(oldCurrent, false);
                    if ( !isStable() )
                        return false;
                }

                RefreshLine( oldCurrent );
            }
        }
    }

    // do we need to change the selection state?
    if ( stateMask & wxLIST_STATE_SELECTED )
    {
        bool on = (state & wxLIST_STATE_SELECTED) != 0;

        if ( IsSingleSel() )
        {
            if ( on )
            {
                // selecting the item also makes it the focused one in the
                // single sel mode
                if ( m_current != item )
                {
                    ChangeCurrent(item);
                    if ( !isStable() )
                        return false;

                    if ( oldCurrent != wxNO_ITEM )
                    {
                        HighlightLine( oldCurrent, false );
                        if ( !isStable() )
                            return false;
                        RefreshLine( oldCurrent );
                    }
                }
            }
            else // off
            {
                // only the current item may be selected anyhow
                if ( item != m_current )
                    return true;
            }
        }

        const bool changed = HighlightLine(item, on);
        if ( !isStable() )
            return false;

        if ( changed )
        {
            RefreshLine(item);
        }
    }

    return true;
}

int wxListMainWindow::GetItemState( long item, long stateMask ) const
{
    wxCHECK_MSG( item >= 0 && (size_t)item < GetItemCount(), 0,
                 wxT("invalid list ctrl item index in GetItemState()") );

    int ret = wxLIST_STATE_DONTCARE;

    if ( stateMask & wxLIST_STATE_FOCUSED )
    {
        if ( (size_t)item == m_current )
            ret |= wxLIST_STATE_FOCUSED;
    }

    if ( stateMask & wxLIST_STATE_SELECTED )
    {
        if ( IsHighlighted(item) )
            ret |= wxLIST_STATE_SELECTED;
    }

    return ret;
}

void wxListMainWindow::GetItem( wxListItem &item ) const
{
    wxCHECK_RET( item.m_itemId >= 0 && (size_t)item.m_itemId < GetItemCount(),
                 wxT("invalid item index in GetItem") );

    wxListLineData *line = GetLine((size_t)item.m_itemId);
    line->GetItem( item.m_col, item );

    // Get item state if user wants it
    if ( item.m_mask & wxLIST_MASK_STATE )
        item.m_state = GetItemState( item.m_itemId, wxLIST_STATE_SELECTED |
                                                 wxLIST_STATE_FOCUSED );
}

// ----------------------------------------------------------------------------
// item count
// ----------------------------------------------------------------------------

size_t wxListMainWindow::GetItemCount() const
{
    return IsVirtual() ? m_countVirt : m_lines.size();
}

void wxListMainWindow::SetItemCount(long count)
{
    const long oldCount = static_cast<long>(GetItemCount());
    const std::uint64_t structureRevision = m_structureRevision;
    const wxWeakRef<wxWindow> weakThis(this);

    // Update the current item if it's not valid any longer (notice that this
    // invalidates it completely if the control is becoming empty, which is the
    // right thing to do).
    if ( HasCurrent() && m_current >= (size_t)count )
    {
        ChangeCurrent(count - 1);
        if ( weakThis.get() != this ||
                m_structureRevision != structureRevision )
        {
            return;
        }
    }

    // And do the same thing for the multiple selection anchor.
    if ( m_anchor != wxNO_ITEM && m_anchor >= (size_t)count )
        m_anchor = count - 1;

    m_selStore.SetItemCount(count);
    m_countVirt = count;
    if ( count < oldCount ) InvalidateLabelEditFrom(static_cast<size_t>(count));
    if ( count != oldCount )
        ++m_structureRevision;

    ResetVisibleLinesRange();

    // scrollbars must be reset
    m_dirty = true;
}

int wxListMainWindow::GetSelectedItemCount() const
{
    // deal with the quick case first
    if ( IsSingleSel() )
        return HasCurrent() ? IsHighlighted(m_current) : false;

    // virtual controls remmebers all its selections itself
    if ( IsVirtual() )
        return m_selStore.GetSelectedCount();

    return m_selCount;
}

// ----------------------------------------------------------------------------
// item position/size
// ----------------------------------------------------------------------------

wxRect wxListMainWindow::GetViewRect() const
{
    wxASSERT_MSG( !HasFlag(wxLC_LIST), "not implemented for list view" );

    // we need to find the longest/tallest label
    wxCoord xMax = 0, yMax = 0;
    const int count = GetItemCount();
    if ( count )
    {
        for ( int i = 0; i < count; i++ )
        {
            // we need logical, not physical, coordinates here, so use
            // GetLineRect() instead of GetItemRect()
            wxRect r = GetLineRect(i);

            wxCoord x = r.GetRight(),
                    y = r.GetBottom();

            if ( x > xMax )
                xMax = x;
            if ( y > yMax )
                yMax = y;
        }
    }

    // some fudge needed to make it look prettier
    xMax += 2 * EXTRA_BORDER_X;
    yMax += 2 * EXTRA_BORDER_Y;

    // account for the scrollbars if necessary
    const wxSize sizeAll = GetClientSize();
    const wxGenericListCtrl* const listctrl = GetListCtrl();
    if ( xMax > sizeAll.x )
        yMax += listctrl->GetScrollbarSize(wxHORIZONTAL);
    if ( yMax > sizeAll.y )
        xMax += listctrl->GetScrollbarSize(wxVERTICAL);

    return wxRect(0, 0, xMax, yMax);
}

bool
wxListMainWindow::GetSubItemRect(long item, long subItem, wxRect& rect,
                                 int code) const
{
    wxCHECK_MSG( subItem == wxLIST_GETSUBITEMRECT_WHOLEITEM || InReportView(),
                 false,
                 wxT("GetSubItemRect only meaningful in report view") );
    wxCHECK_MSG( item >= 0 && (size_t)item < GetItemCount(), false,
                 wxT("invalid item in GetSubItemRect") );

    // ensure that we're laid out, otherwise we could return nonsense
    if ( m_dirty )
    {
        wxConstCast(this, wxListMainWindow)->RecalculatePositions();
    }

    rect = GetLineRect((size_t)item);

    if ( subItem == wxLIST_GETSUBITEMRECT_WHOLEITEM )
    {
        switch ( code )
        {
            case wxLIST_RECT_BOUNDS:
                break;

            case wxLIST_RECT_ICON:
                if ( GetLine(item)->HasImage() )
                    rect = GetLineIconRect(item);
                else
                    rect = wxRect();
                break;

            case wxLIST_RECT_LABEL:
                rect = GetLineLabelRect(item);
                break;

            default:
                wxFAIL_MSG(wxS("Unknown rectangle requested"));
                return false;
        }
    }
    else
    {
        wxCHECK_MSG( subItem >= 0 && subItem < GetColumnCount(), false,
                     wxT("invalid subItem in GetSubItemRect") );

        rect.x += GetColumnStart(subItem);
        rect.width = GetColumnWidth(subItem);

        switch ( code )
        {
            case wxLIST_RECT_BOUNDS:
                // Nothing to do.
                break;

            case wxLIST_RECT_ICON:
            case wxLIST_RECT_LABEL:
                // Note: this needs to be kept in sync with DrawInReportMode().
                {
                    wxListLineData* const line = GetLine(item);
                    if ( subItem == 0 )
                    {
                        if ( code == wxLIST_RECT_ICON )
                        {
                            rect = line->HasImage()
                                ? GetLineIconRect(item)
                                : wxRect();
                        }
                        else
                        {
                            rect = GetLineLabelRect(item);
                        }
                        break;
                    }

                    rect.x += ICON_OFFSET_X;
                    rect.width -= ICON_OFFSET_X;

                    if ( code == wxLIST_RECT_ICON )
                        rect = wxRect();
                }
                break;

            default:
                wxFAIL_MSG(wxS("Unknown rectangle requested"));
                return false;
        }
    }

    GetListCtrl()->CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);

    return true;
}

bool wxListMainWindow::GetItemPosition(long item, wxPoint& pos) const
{
    wxRect rect;
    GetItemRect(item, rect);

    pos.x = rect.x;
    pos.y = rect.y;

    return true;
}

// ----------------------------------------------------------------------------
// checkboxes
// ----------------------------------------------------------------------------

bool wxListMainWindow::HasCheckBoxes() const
{
    return m_hasCheckBoxes;
}

bool wxListMainWindow::EnableCheckBoxes(bool enable)
{
    m_hasCheckBoxes = enable;

    m_dirty = true;
    m_headerWidth = 0;
    Refresh();

    return true;
}

void wxListMainWindow::CheckItem(long item, bool state)
{
    wxCHECK_RET( HasCheckBoxes(), "checkboxes are disabled" );

    if ( !IsVirtual() )
    {
        wxListLineData* line = GetLine((size_t)item);

        // Don't send any events if nothing changes, for consistency with
        // wxMSW, where the native control doesn't send them in this case.
        if ( line->IsChecked() == state )
            return;

        line->Check(state);

        RefreshLine(item);
    }

    SendNotify(item, state ? wxEVT_LIST_ITEM_CHECKED
        : wxEVT_LIST_ITEM_UNCHECKED);
}

bool wxListMainWindow::IsItemChecked(long item) const
{
    if ( !HasCheckBoxes() )
        return false;

    if ( !IsVirtual() )
    {
        wxListLineData* line = GetLine((size_t)item);
        return line->IsChecked();
    }
    else
    {
        wxGenericListCtrl* listctrl = GetListCtrl();
        return listctrl->OnGetItemIsChecked(item);
    }
}

bool wxListMainWindow::IsInsideCheckBox(long item, int x, int y) const
{
    if ( HasCheckBoxes() )
    {
        wxRect lineRect = GetLineRect(item);
        if ( InReportView() )
            lineRect.x += GetColumnStart(0);
        wxSize cbSize = wxRendererNative::Get().GetCheckBoxSize(const_cast<wxListMainWindow*>(this));
        int yOffset = (lineRect.height - cbSize.GetHeight()) / 2;
        wxRect rr(wxPoint(lineRect.x + MARGIN_AROUND_CHECKBOX, lineRect.y + yOffset), cbSize);

        return rr.Contains(wxPoint(x, y));
    }

    return false;
}

// ----------------------------------------------------------------------------
// geometry calculation
// ----------------------------------------------------------------------------

void wxListMainWindow::RecalculatePositions()
{
    const int lineHeight = GetLineHeight();

    wxInfoDC dc( this );

    const size_t count = GetItemCount();

    int iconSpacing;
    if ( HasFlag(wxLC_ICON) && m_normal_images )
        iconSpacing = m_normal_spacing;
    else if ( HasFlag(wxLC_SMALL_ICON) && m_small_images )
        iconSpacing = m_small_spacing;
    else
        iconSpacing = 0;

    // Note that we do not call GetClientSize() here but
    // GetSize() and subtract the border size for sunken
    // borders manually. This is technically incorrect,
    // but we need to know the client area's size WITHOUT
    // scrollbars here. Since we don't know if there are
    // any scrollbars, we use GetSize() instead. Another
    // solution would be to call SetScrollbars() here to
    // remove the scrollbars and call GetClientSize() then,
    // but this might result in flicker and - worse - will
    // reset the scrollbars to 0 which is not good at all
    // if you resize a dialog/window, but don't want to
    // reset the window scrolling. RR.
    // Furthermore, we actually do NOT subtract the border
    // width as 2 pixels is just the extra space which we
    // need around the actual content in the window. Other-
    // wise the text would e.g. touch the upper border. RR.
    int clientWidth,
        clientHeight;
    GetSize( &clientWidth, &clientHeight );

    if ( InReportView() )
    {
        // all lines have the same height and we scroll one line per step
        int entireHeight = count * lineHeight + LINE_SPACING;

        m_linesPerPage = clientHeight / lineHeight;

        ResetVisibleLinesRange();

        int viewStartX;
        int viewStartY;
        GetListCtrl()->GetViewStart(&viewStartX, &viewStartY);
#if defined(__WXWINUI__) && wxUSE_WINUI3
        // One pixel for a step of the vertical bar: the list scrolls as
        // smoothly as a list of WinUI does.
        GetListCtrl()->SetScrollbars( SCROLL_UNIT_X, 1,
                       GetHeaderWidth() / SCROLL_UNIT_X,
                       entireHeight,
                       viewStartX,
                       viewStartY,
                       true );
#else
        GetListCtrl()->SetScrollbars( SCROLL_UNIT_X, lineHeight,
                       GetHeaderWidth() / SCROLL_UNIT_X,
                       (entireHeight + lineHeight - 1) / lineHeight,
                       viewStartX,
                       viewStartY,
                       true );
#endif
    }
    else // !report
    {
        // we have 3 different layout strategies: either layout all items
        // horizontally/vertically (wxLC_ALIGN_XXX styles explicitly given) or
        // to arrange them in top to bottom, left to right (don't ask me why
        // not the other way round...) order
        if ( HasFlag(wxLC_ALIGN_LEFT | wxLC_ALIGN_TOP) )
        {
            int x = EXTRA_BORDER_X;
            int y = EXTRA_BORDER_Y;

            wxCoord widthMax = 0;

            size_t i;
            for ( i = 0; i < count; i++ )
            {
                wxListLineData *line = GetLine(i);
                line->CalculateSize( &dc, iconSpacing );
                line->SetPosition( x, y, iconSpacing );

                wxSize sizeLine = GetLineSize(i);

                if ( HasFlag(wxLC_ALIGN_TOP) )
                {
                    if ( sizeLine.x > widthMax )
                        widthMax = sizeLine.x;

                    y += sizeLine.y;
                }
                else // wxLC_ALIGN_LEFT
                {
                    x += sizeLine.x + MARGIN_BETWEEN_ROWS;
                }
            }

            if ( HasFlag(wxLC_ALIGN_TOP) )
            {
                // traverse the items again and tweak their sizes so that they are
                // all the same in a row
                for ( i = 0; i < count; i++ )
                {
                    wxListLineData *line = GetLine(i);
                    line->m_gi->ExtendWidth(widthMax);
                }
            }

            int viewStartX;
            int viewStartY;
            GetListCtrl()->GetViewStart(&viewStartX, &viewStartY);
            GetListCtrl()->SetScrollbars
            (
                SCROLL_UNIT_X,
                lineHeight,
                (x + SCROLL_UNIT_X) / SCROLL_UNIT_X,
                (y + lineHeight) / lineHeight,
                viewStartX,
                viewStartY,
                true
            );
        }
        else // "flowed" arrangement, the most complicated case
        {
            // at first we try without any scrollbars, if the items don't fit into
            // the window, we recalculate after subtracting the space taken by the
            // scrollbar

            int entireWidth = 0;

            for (int tries = 0; tries < 2; tries++)
            {
                entireWidth = 2 * EXTRA_BORDER_X;

                if (tries == 1)
                {
                    // Now we have decided that the items do not fit into the
                    // client area, so we need a scrollbar
                    entireWidth += SCROLL_UNIT_X;
                }

                int x = EXTRA_BORDER_X;
                int y = EXTRA_BORDER_Y;

                // Note that "row" here is vertical, i.e. what is called
                // "column" in many other places in wxWidgets.
                int maxWidthInThisRow = 0;

                m_linesPerPage = 0;
                int currentlyVisibleLines = 0;

                for (size_t i = 0; i < count; i++)
                {
                    currentlyVisibleLines++;
                    wxListLineData *line = GetLine( i );
                    line->CalculateSize( &dc, iconSpacing );
                    line->SetPosition( x, y, iconSpacing );

                    wxSize sizeLine = GetLineSize( i );

                    if ( maxWidthInThisRow < sizeLine.x )
                        maxWidthInThisRow = sizeLine.x;

                    y += sizeLine.y;
                    if (currentlyVisibleLines > m_linesPerPage)
                        m_linesPerPage = currentlyVisibleLines;

                    // Have we reached the end of the row either because no
                    // more items would fit or because there are simply no more
                    // items?
                    if ( y + sizeLine.y >= clientHeight
                            || i == count - 1)
                    {
                        // Adjust all items in this row to have the same
                        // width to ensure that they all align horizontally in
                        // icon view.
                        if ( HasFlag(wxLC_ICON) || HasFlag(wxLC_SMALL_ICON) )
                        {
                            size_t firstRowLine = i - currentlyVisibleLines + 1;
                            for (size_t j = firstRowLine; j <= i; j++)
                            {
                                GetLine(j)->m_gi->ExtendWidth(maxWidthInThisRow);
                            }
                        }

                        currentlyVisibleLines = 0;
                        y = EXTRA_BORDER_Y;
                        maxWidthInThisRow += MARGIN_BETWEEN_ROWS;
                        x += maxWidthInThisRow;
                        entireWidth += maxWidthInThisRow;
                        maxWidthInThisRow = 0;
                    }

                    if ( (tries == 0) &&
                            (entireWidth + SCROLL_UNIT_X > clientWidth) )
                    {
                        clientHeight -=
                            GetListCtrl()->GetScrollbarSize(wxHORIZONTAL);
                        m_linesPerPage = 0;
                        break;
                    }

                    if ( i == count - 1 )
                        tries = 1;  // Everything fits, no second try required.
                }
            }

            int viewStartX;
            int viewStartY;
            GetListCtrl()->GetViewStart(&viewStartX, &viewStartY);
            GetListCtrl()->SetScrollbars
            (
                SCROLL_UNIT_X,
                lineHeight,
                (entireWidth + SCROLL_UNIT_X) / SCROLL_UNIT_X,
                0,
                viewStartX,
                0,
                true
            );
        }
    }
}

void wxListMainWindow::RecalculatePositionsAndRefresh()
{
    RecalculatePositions();

    // Whether the window was dirty or not before, it won't be any longer after
    // the next refresh.
    m_dirty = false;
    Refresh();

    wxListHeaderWindow *headerWin = GetListCtrl()->m_headerWin;
    if ( headerWin && headerWin->m_dirty )
    {
        headerWin->m_dirty = false;
        headerWin->Refresh();
    }
}

void wxListMainWindow::UpdateCurrent()
{
    if ( !HasCurrent() && !IsEmpty() )
    {
        // Initialise m_current to the first item without sending any
        // wxEVT_LIST_ITEM_FOCUSED event (typicaly when the control gains focus)
        // and this is to allow changing the focused item using the arrow keys.
        // which is the behaviour found in the wxMSW port.
        ChangeCurrentWithoutEvent(0);
    }
}

long wxListMainWindow::GetNextItem( long item,
                                    int WXUNUSED(geometry),
                                    int state ) const
{
    long ret = item,
         max = GetItemCount();
    wxCHECK_MSG( (ret == -1) || (ret < max), -1,
                 wxT("invalid listctrl index in GetNextItem()") );

    // notice that we start with the next item (or the first one if item == -1)
    // and this is intentional to allow writing a simple loop to iterate over
    // all selected items
    ret++;
    if ( ret == max )
        // this is not an error because the index was OK initially,
        // just no such item
        return -1;

    if ( !state )
        // any will do
        return (size_t)ret;

    size_t count = GetItemCount();
    for ( size_t line = (size_t)ret; line < count; line++ )
    {
        if ( (state & wxLIST_STATE_FOCUSED) && (line == m_current) )
            return line;

        if ( (state & wxLIST_STATE_SELECTED) && IsHighlighted(line) )
            return line;
    }

    return -1;
}

// ----------------------------------------------------------------------------
// deleting stuff
// ----------------------------------------------------------------------------

bool wxListMainWindow::DeleteItem( long lindex )
{
    size_t count = GetItemCount();

    wxCHECK_MSG( (lindex >= 0) && ((size_t)lindex < count), false,
                 wxT("invalid item index in DeleteItem") );

    size_t index = (size_t)lindex;

    const std::uint64_t structureRevision = m_structureRevision;
    const wxWeakRef<wxWindow> weakThis(this);
    SendNotify( index, wxEVT_LIST_DELETE_ITEM, wxDefaultPosition );
    if ( weakThis.get() != this )
        return false;

    // A nested writer is authoritative. Applying the stale outer index after
    // it returns could remove a different item and desynchronize composite
    // controls such as wxListbook.
    if ( m_structureRevision != structureRevision )
        return false;

    InvalidateLabelEditFrom(index);

    // Commit all index-derived state only after the synchronous notification
    // boundary has completed. A nested topology writer may otherwise make
    // these adjustments stale even when the actual erase is abandoned.
    if ( HasCurrent() && m_current >= index )
    {
        // if the current item is being deleted, we want the next one to
        // become selected - unless there is no next one - so don't adjust
        // m_current in this case
        if ( m_current != index || m_current == count - 1 )
            m_current--;
    }

    // Also adjust the anchor, but just reset it if it's being deleted, as it
    // doesn't make sense to keep the anchor at some other item.
    if ( m_anchor != wxNO_ITEM )
    {
        if ( m_anchor == index )
            m_anchor = wxNO_ITEM;
        else if ( m_anchor > index )
            m_anchor--;
    }

    if ( InReportView() )
    {
        //  mark the Column Max Width cache as dirty if the items in the line
        //  we're deleting contain the Max Column Width
        wxListLineData * const line = GetLine(index);
        wxListItem      item;

        size_t i = 0;
        for ( const auto& it : line->m_items )
        {
            it.GetItem(item);

            const int itemWidth = GetItemWidthWithImage(&item);

            wxColWidthInfo& widthInfo = m_aColWidths.at(i++);
            if ( itemWidth >= widthInfo.nMaxWidth )
                widthInfo.bNeedsUpdate = true;
        }

        ResetVisibleLinesRange();
    }

    if ( IsVirtual() )
    {
        m_countVirt--;
        m_selStore.OnItemDelete(index);
    }
    else
    {
        auto const iter =  m_lines.begin() + index;
        if ( iter->IsHighlighted() )
            UpdateSelectionCount(false);
        m_lines.erase(iter);
    }
    ++m_structureRevision;

    // we need to refresh the (vert) scrollbar as the number of items changed
    m_dirty = true;

    RefreshAfter(index);

    // This might be a wxGTK bug, but when deleting the last item in a control
    // with many items, the vertical scroll position may change so that the new
    // last item is not visible any longer, which is very annoying from the
    // user point of view. Ensure that whatever happens, this item is visible.
    if ( count > 1 && m_current != wxNO_ITEM )
        EnsureVisible(m_current);

    return true;
}

void wxListMainWindow::DeleteColumn( int col )
{
    wxCHECK_RET( col >= 0 && col < (int)m_columns.size(),
                 "invalid column index in DeleteColumn()" );

    if ( col == 0 ) InvalidateLabelEditFrom(0);
    m_dirty = true;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    ++m_structureRevision;
#endif
    m_columns.erase( m_columns.begin() + col );
    m_columnsOrder.RemoveAt(m_columnsOrder.Index(col));
    for ( int& index : m_columnsOrder )
    {
        if ( index > col )
            --index;
    }
    if ( auto* const header = GetListCtrl()->m_headerWin )
    {
        if ( header->m_sortCol == col )
            header->m_sortCol = -1;
        else if ( header->m_sortCol > col )
            --header->m_sortCol;
        header->m_dirty = true;
    }

    if ( !IsVirtual() )
    {
        // update all the items
        for ( auto& line : m_lines )
        {
            // In the following atypical but possible scenario it can be
            // legal to call DeleteColumn() but the items may not have any
            // values for it:
            //  1. In report view, insert a second column.
            //  2. Still in report view, add an item with 2 values.
            //  3. Switch to an icon (or list) view.
            //  4. Add an item -- necessarily with 1 value only.
            //  5. Switch back to report view.
            //  6. Call DeleteColumn().
            // So we need to check for this as otherwise we would simply crash
            // if this happens.
            if ( line.m_items.size() <= static_cast<unsigned>(col) )
                continue;

            line.m_items.erase(line.m_items.begin() + col);
        }
    }

    if ( InReportView() )   //  we only cache max widths when in Report View
    {
        m_aColWidths.erase(m_aColWidths.begin() + col);
    }

    // invalidate it as it has to be recalculated
    m_headerWidth = 0;
}

bool wxListMainWindow::DoDeleteAllItems(bool notify)
{
    // We will need to update all columns if any items are inserted again.
    if ( InReportView() )
    {
        for ( auto& widthInfo : m_aColWidths )
            widthInfo.bNeedsUpdate = true;
    }

    if ( IsEmpty() )
        // nothing to do - in particular, don't send the event
        return true;

    if ( notify )
    {
        const std::uint64_t structureRevision = m_structureRevision;
        const wxWeakRef<wxWindow> weakThis(this);
        const bool previousNotification = m_deleteAllNotificationActive;
        m_deleteAllNotificationActive = true;
        const auto finishNotification = wxMakeGuard([weakThis, this, previousNotification]()
        {
            if ( weakThis.get() == this )
                m_deleteAllNotificationActive = previousNotification;
        });
        ResetCurrent();
        if ( weakThis.get() != this ||
                m_structureRevision != structureRevision )
        {
            return false;
        }

        // Public DeleteAllItems() sends the single documented notification,
        // not one event per item. A handler can destroy this entire control.
        wxWindow* const parent = GetParent();
        wxListEvent event( wxEVT_LIST_DELETE_ALL_ITEMS, parent->GetId() );
        event.SetEventObject( parent );
        parent->GetEventHandler()->ProcessEvent( event );
        if ( weakThis.get() != this ||
                m_structureRevision != structureRevision )
        {
            return false;
        }
    }
    else
    {
        m_current = (size_t)-1;
    }

    if ( IsVirtual() )
    {
        m_countVirt = 0;
        m_selStore.Clear();
    }
    else
    {
        m_selCount = 0;
    }

    if ( InReportView() )
        ResetVisibleLinesRange();

    InvalidateLabelEditFrom(0);
    m_lines.clear();
    ++m_structureRevision;
    return true;
}

bool wxListMainWindow::DeleteAllItems()
{
    if ( !DoDeleteAllItems() )
        return false;

    RecalculatePositionsAndRefresh();
    return true;
}

void wxListMainWindow::DeleteEverything()
{
    m_columns.clear();
    m_columnsOrder.clear();
    m_aColWidths.clear();
    m_headerWidth = 0;
    if ( auto* const header = GetListCtrl()->m_headerWin )
    {
        header->m_sortCol = -1;
        header->m_dirty = true;
    }

    DeleteAllItems();
}

// ----------------------------------------------------------------------------
// scanning for an item
// ----------------------------------------------------------------------------

void wxListMainWindow::EnsureVisible( long index )
{
    wxCHECK_RET( index >= 0 && (size_t)index < GetItemCount(),
                 wxT("invalid index in EnsureVisible") );

    // We have to call this here because the label in question might just have
    // been added and its position is not known yet
    if ( m_dirty )
        RecalculatePositions();

    MoveToItem((size_t)index);
}

long wxListMainWindow::FindItem(long start, const wxString& str, bool partial )
{
    if (str.empty())
        return wxNOT_FOUND;

    long pos = start;
    wxString str_upper = str.Upper();
    if (pos < 0)
        pos = 0;

    size_t count = GetItemCount();
    for ( size_t i = (size_t)pos; i < count; i++ )
    {
        wxListLineData *line = GetLine(i);
        wxString line_upper = line->GetText(0).Upper();
        if (!partial)
        {
            if (line_upper == str_upper )
                return i;
        }
        else
        {
            if (line_upper.find(str_upper) == 0)
                return i;
        }
    }

    return wxNOT_FOUND;
}

long wxListMainWindow::FindItem(long start, wxUIntPtr data)
{
    long pos = start;
    if (pos < 0)
        pos = 0;

    size_t count = GetItemCount();
    for (size_t i = (size_t)pos; i < count; i++)
    {
        wxListLineData *line = GetLine(i);
        wxListItem item;
        line->GetItem( 0, item );
        if (item.m_data == data)
            return i;
    }

    return wxNOT_FOUND;
}

long wxListMainWindow::FindItem( const wxPoint& pt )
{
    size_t topItem;
    GetVisibleLinesRange( &topItem, nullptr );

    wxPoint p;
    GetItemPosition( GetItemCount() - 1, p );
    if ( p.y == 0 )
        return topItem;

    long id = (long)floor( pt.y * double(GetItemCount() - topItem - 1) / p.y + topItem );
    if ( id >= 0 && id < (long)GetItemCount() )
        return id;

    return wxNOT_FOUND;
}

long wxListMainWindow::HitTest( int x, int y, int &flags ) const
{
    GetListCtrl()->CalcUnscrolledPosition( x, y, &x, &y );

    size_t count = GetItemCount();

    if ( InReportView() )
    {
        size_t current = y / GetLineHeight();
        if ( current < count )
        {
            flags = HitTestLine(current, x, y);
            if ( flags )
                return current;
        }
    }
    else // !report
    {
        // TODO: optimize it too! this is less simple than for report view but
        //       enumerating all items is still not a way to do it!!
        for ( size_t current = 0; current < count; current++ )
        {
            flags = HitTestLine(current, x, y);
            if ( flags )
                return current;
        }
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// adding stuff
// ----------------------------------------------------------------------------

bool wxListMainWindow::InsertItem( wxListItem &item )
{
    wxASSERT_MSG( !IsVirtual(), wxT("can't be used with virtual control") );

    int count = GetItemCount();
    wxCHECK_MSG( item.m_itemId >= 0, false, wxT("invalid item index") );

    if (item.m_itemId > count)
        item.m_itemId = count;

    size_t id = item.m_itemId;

    m_dirty = true;

    if ( InReportView() )
    {
        ResetVisibleLinesRange();

        const unsigned col = item.GetColumn();
        wxCHECK_MSG( col < m_aColWidths.size(), false,
                     "invalid item column" );

        // calculate the width of the item and adjust the max column width
        wxColWidthInfo& widthInfo = m_aColWidths[col];
        int width = GetItemWidthWithImage(&item);
        item.SetWidth(width);
        if (width > widthInfo.nMaxWidth)
        {
            widthInfo.nMaxWidth = width;
            widthInfo.bNeedsUpdate = true;
        }
    }

    wxListLineData line(this);

    line.SetItem( item.m_col, item );
    if ( item.m_mask & wxLIST_MASK_IMAGE )
    {
        // Reset the buffered height if it's not big enough for the new image.
        int image = item.GetImage();
        if ( m_small_images && image != -1 && InReportView() )
        {
            int imageWidth, imageHeight;
            m_small_images->GetImageLogicalSize(this, imageWidth, imageHeight);

            if ( imageHeight > m_lineHeight )
                m_lineHeight = 0;
        }
    }

    InvalidateLabelEditFrom(static_cast<size_t>(id));
    m_lines.insert( m_lines.begin() + id, std::move(line) );
    ++m_structureRevision;

    m_dirty = true;

    // If an item is selected at or below the point of insertion, we need to
    // increment the member variables because the current row's index has gone
    // up by one
    if ( HasCurrent() && m_current >= id )
        m_current++;

    // And the same for the anchor.
    if ( m_anchor != wxNO_ITEM && m_anchor >= id )
        m_anchor++;

    const std::uint64_t structureRevision = m_structureRevision;
    const wxWeakRef<wxWindow> weakThis(this);
    SendNotify(id, wxEVT_LIST_INSERT_ITEM);
    if ( weakThis.get() != this )
        return false;

    // The nested operation has already refreshed its effective topology.
    // Avoid using the outer insertion index against a newer model.
    if ( m_structureRevision != structureRevision )
        return true;

    RefreshLines(id, GetItemCount() - 1);
    return true;
}

long wxListMainWindow::InsertColumn( long col, const wxListItem &item )
{
    long idx = -1;

    m_dirty = true;
    if ( InReportView() )
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        ++m_structureRevision;
#endif
        if ( col == 0 ) InvalidateLabelEditFrom(0);
        wxListHeaderData column( item );
        if (item.m_width == wxLIST_AUTOSIZE_USEHEADER)
            column.SetWidth(ComputeMinHeaderWidth(&column));
#if defined(__WXWINUI__) && wxUSE_WINUI3
        // A column given no width has the default one, 80 pixels whatever
        // the scale of the screen: at 150% a title of seven letters was cut.
        // The same width in DIPs, and never less than its title needs.
        else if ( !(item.m_mask & wxLIST_MASK_WIDTH) || item.m_width < 0 )
        {
            column.SetWidth(wxMax(FromDIP(wxLIST_DEFAULT_COL_WIDTH),
                                  ComputeMinHeaderWidth(&column)));
        }
#endif

        wxColWidthInfo colWidthInfo(0, IsVirtual());

        bool insert = (col >= 0) && ((size_t)col < m_columns.size());
        if ( insert )
        {
            m_columns.insert( m_columns.begin() + col, column );
            m_aColWidths.insert( m_aColWidths.begin() + col, colWidthInfo );
            idx = col;
        }
        else
        {
            idx = m_aColWidths.size();
            m_columns.push_back( column );
            m_aColWidths.push_back( colWidthInfo );
        }

        // Preserve the relative display order of existing columns while the
        // new column takes the visual position corresponding to its index.
        for ( int& index : m_columnsOrder )
        {
            if ( index >= idx )
                ++index;
        }
        m_columnsOrder.Insert(static_cast<int>(idx), static_cast<size_t>(idx));
        if ( auto* const header = GetListCtrl()->m_headerWin )
        {
            if ( header->m_sortCol >= idx )
                ++header->m_sortCol;
            header->m_dirty = true;
        }

        if ( !IsVirtual() )
        {
            // update all the items
            for ( auto& line : m_lines )
            {
                auto& items = line.m_items;
                if ( insert )
                    items.emplace(items.begin() + col, this);
                else
                    items.emplace_back(this);
            }
        }

        // invalidate it as it has to be recalculated
        m_headerWidth = 0;
    }
    return idx;
}

int wxListMainWindow::GetItemWidthWithImage(wxListItem * item)
{
    int width = 0;
    wxInfoDC dc(this);

    if (item->GetImage() != -1)
    {
        int ix, iy;
        GetImageSize( ix, iy );
        width += ix + IMAGE_MARGIN_IN_REPORT_MODE;
    }

    if (!item->GetText().empty())
    {
        wxCoord w;
        dc.GetTextExtent( item->GetText(), &w, nullptr );
        width += w;
    }

    return width;
}

// ----------------------------------------------------------------------------
// sorting
// ----------------------------------------------------------------------------

struct wxListSortEntry
{
    wxUIntPtr data;
    size_t originalIndex;
};

bool wxListMainWindow::SortItems( wxListCtrlCompare fn, wxIntPtr data )
{
    const wxWeakRef<wxWindow> weakThis(this);
    const size_t count = m_lines.size();

    // Allocate all transactional storage before deselection/focus events can
    // make visible state changes. Once those callbacks have run, SortItems()
    // can either publish without allocating or return false.
    std::vector<wxListSortEntry> order;
    order.reserve(count);
    std::vector<wxListSortEntry> scratch(count);
    std::vector<size_t> destination(count);

    // selections won't make sense any more after sorting the items so reset
    // them
    const std::uint64_t structureRevision = m_structureRevision;
    if ( !HighlightAll(false) ||
            weakThis.get() != this ||
            m_structureRevision != structureRevision )
        return false;

    ResetCurrent();
    if ( weakThis.get() != this ||
            m_structureRevision != structureRevision )
        return false;

    if ( count > 1 )
    {
        // Never let the public comparator run while std::sort() holds
        // iterators or references into m_lines: it may synchronously destroy
        // this control or mutate its topology. Sort an independent
        // permutation with an abortable merge pass and publish it only if the
        // control and its item topology are still the original ones.
        for ( size_t i = 0; i < count; ++i )
        {
            wxListItem item;
            item.SetMask(wxLIST_MASK_DATA);
            m_lines[i].GetItem(0, item);
            order.push_back({item.m_data, i});
        }

        for ( size_t width = 1; width < count; )
        {
            for ( size_t left = 0; left < count; )
            {
                const size_t middle =
                    left + wxMin(width, count - left);
                const size_t right =
                    middle + wxMin(width, count - middle);
                size_t first = left;
                size_t second = middle;
                size_t out = left;

                while ( first < middle && second < right )
                {
                    const int compare =
                        fn(order[first].data, order[second].data, data);
                    if ( weakThis.get() != this ||
                            m_structureRevision != structureRevision )
                    {
                        return false;
                    }

                    scratch[out++] = compare <= 0
                        ? order[first++]
                        : order[second++];
                }

                while ( first < middle )
                    scratch[out++] = order[first++];
                while ( second < right )
                    scratch[out++] = order[second++];

                left = right;
            }

            order.swap(scratch);
            if ( width >= count - width )
                break;
            width *= 2;
        }

        for ( size_t newIndex = 0; newIndex < count; ++newIndex )
            destination[order[newIndex].originalIndex] = newIndex;

        // Apply the permutation in place. destination follows the item
        // currently occupying each position, so swapping it together with the
        // line closes one permutation cycle without allocating or losing any
        // wxListLineData attributes.
        for ( size_t current = 0; current < count; ++current )
        {
            while ( destination[current] != current )
            {
                const size_t target = destination[current];
                std::swap(m_lines[current], m_lines[target]);
                std::swap(destination[current], destination[target]);
            }
        }

        InvalidateLabelEditFrom(0);
        ++m_structureRevision;
    }

    m_dirty = true;
    return true;
}

// ----------------------------------------------------------------------------
// scrolling
// ----------------------------------------------------------------------------

void wxListMainWindow::OnScroll(wxScrollWinEvent& event)
{
    // update our idea of which lines are shown when we redraw the window the
    // next time
    ResetVisibleLinesRange();

    if ( event.GetOrientation() == wxHORIZONTAL && HasHeader() )
    {
        wxGenericListCtrl* lc = GetListCtrl();
        wxCHECK_RET( lc, wxT("no listctrl window?") );

        if (lc->m_headerWin) // when we use wxLC_NO_HEADER, m_headerWin==nullptr
        {
            lc->m_headerWin->Refresh();
            lc->m_headerWin->Update();
        }
    }
}

int wxListMainWindow::GetCountPerPage() const
{
    // The number of lines per page may be out of date if we're dirty, so
    // recompute it in this case.
    if ( m_dirty )
        wxConstCast(this, wxListMainWindow)->RecalculatePositions();

    if ( !m_linesPerPage )
    {
        wxConstCast(this, wxListMainWindow)->
            m_linesPerPage = GetClientSize().y / GetLineHeight();
    }

    return m_linesPerPage;
}

void wxListMainWindow::GetVisibleLinesRange(size_t *from, size_t *to)
{
    wxASSERT_MSG( InReportView(), wxT("this is for report mode only") );

    if ( m_lineFrom == wxNO_ITEM )
    {
        size_t count = GetItemCount();
        if ( count )
        {
            int viewStartY;
            GetListCtrl()->GetViewStart(nullptr, &viewStartY);
#if defined(__WXWINUI__) && wxUSE_WINUI3
            // The bar counts pixels: the first line may show in part.
            const int lineHeightNow = wxMax(1, GetLineHeight());
            m_lineFrom = viewStartY / lineHeightNow;
#else
            m_lineFrom = viewStartY;
#endif

            // this may happen if SetScrollbars() hadn't been called yet
            if ( m_lineFrom >= count )
                m_lineFrom = count - 1;

            // we redraw one extra line but this is needed to make the redrawing
            // logic work when there is a fractional number of lines on screen
#if defined(__WXWINUI__) && wxUSE_WINUI3
            m_lineTo = (viewStartY + GetClientSize().y) / lineHeightNow;
#else
            m_lineTo = m_lineFrom + m_linesPerPage;
#endif
            if ( m_lineTo >= count )
                m_lineTo = count - 1;
        }
        else // empty control
        {
            m_lineFrom = 0;
            m_lineTo = wxNO_ITEM;
        }
    }

    wxASSERT_MSG( IsEmpty() ||
                  (m_lineFrom <= m_lineTo && m_lineTo < GetItemCount()),
                  wxT("GetVisibleLinesRange() returns incorrect result") );

    if ( from )
        *from = m_lineFrom;
    if ( to )
        *to = m_lineTo;
}

size_t
wxListMainWindow::PrefixFindItem(size_t idParent,
                                 const wxString& prefixOrig) const
{
    // if no items then just return
    if ( idParent == wxNO_ITEM )
        return idParent;

    // match is case insensitive as this is more convenient to the user: having
    // to press Shift-letter to go to the item starting with a capital letter
    // would be too bothersome
    wxString prefix = prefixOrig.Lower();

    // determine the starting point: we shouldn't take the current item (this
    // allows to switch between two items starting with the same letter just by
    // pressing it) but we shouldn't jump to the next one if the user is
    // continuing to type as otherwise he might easily skip the item he wanted
    size_t itemid = idParent;
    if ( prefix.length() == 1 )
    {
        itemid += 1;
    }

    // look for the item starting with the given prefix after it
    while ( ( itemid < (size_t)GetItemCount() ) &&
            !GetLine(itemid)->GetText(0).Lower().StartsWith(prefix) )
    {
        itemid += 1;
    }

    // if we haven't found anything...
    if ( !( itemid < (size_t)GetItemCount() ) )
    {
        // ... wrap to the beginning
        itemid = 0;

        // and try all the items (stop when we get to the one we started from)
        while ( ( itemid < (size_t)GetItemCount() ) && itemid != idParent &&
                    !GetLine(itemid)->GetText(0).Lower().StartsWith(prefix) )
        {
            itemid += 1;
        }
        // If we haven't found the item, id will be wxNO_ITEM, as per
        // documentation
        if ( !( itemid < (size_t)GetItemCount() ) ||
             ( ( itemid == idParent ) &&
               !GetLine(itemid)->GetText(0).Lower().StartsWith(prefix) ) )
        {
            itemid = wxNO_ITEM;
        }
    }

    return itemid;
}

// -------------------------------------------------------------------------------------
// wxGenericListCtrl
// -------------------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxGenericListCtrl, wxControl);

wxBEGIN_EVENT_TABLE(wxGenericListCtrl,wxListCtrlBase)
  EVT_SIZE(wxGenericListCtrl::OnSize)
  EVT_SCROLLWIN(wxGenericListCtrl::OnScroll)
wxEND_EVENT_TABLE()

void wxGenericListCtrl::Init()
{
    m_mainWin = nullptr;
    m_headerWin = nullptr;
}

wxGenericListCtrl::~wxGenericListCtrl()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( m_winUIProjection ) m_winUIProjection->Close();
    m_winUIProjection.reset();
#endif
    // Don't wait until the base class does it because our subwindows expect
    // their parent window to be a wxListCtrl, but this won't be the case any
    // more when we get to the base class dtor (it will be only a wxWindow).
    DestroyChildren();
}

void wxGenericListCtrl::CreateOrDestroyHeaderWindowAsNeeded()
{
    bool needs_header = HasHeader();
    bool has_header = (m_headerWin != nullptr);

    if (needs_header == has_header)
        return;

    if (needs_header)
    {
        // Notice that we must initialize m_headerWin first, and create the
        // real window only later, so that the test in the beginning of the
        // function blocks repeated creation of the header as it could happen
        // before via wxNavigationEnabled::AddChild() -> ToggleWindowStyle() ->
        // SetWindowStyleFlag().
        m_headerWin = new wxListHeaderWindow();
        m_headerWin->Create
                      (
                        this, wxID_ANY, m_mainWin,
                        wxPoint(0,0),
                        wxSize
                        (
                          GetClientSize().x,
#if defined(__WXWINUI__) && wxUSE_WINUI3
                          wxMax(FromDIP(32), wxRendererNative::Get().GetHeaderButtonHeight(this))
#else
                          wxRendererNative::Get().GetHeaderButtonHeight(this)
#endif
                        ),
                        wxTAB_TRAVERSAL
                      );

#if defined( __WXMAC__ )
        static wxFont font( wxOSX_SYSTEM_FONT_SMALL );
        m_headerWin->SetFont( font );
#endif

        GetSizer()->Prepend( m_headerWin, 0, wxGROW );
    }
    else
    {
        GetSizer()->Detach( m_headerWin );

        wxDELETE(m_headerWin);
    }
}

bool wxGenericListCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint &pos,
                        const wxSize &size,
                        long style,
                        const wxValidator &validator,
                        const wxString &name)
{
    Init();

    // just like in other ports, an assert will fail if the user doesn't give any type style:
    wxASSERT_MSG( (style & wxLC_MASK_TYPE),
                  wxT("wxListCtrl style should have exactly one mode bit set") );

    if ( !wxListCtrlBase::Create( parent, id, pos, size,
                                  style | wxVSCROLL | wxHSCROLL,
                                  validator, name ) )
        return false;

    m_mainWin = new wxListMainWindow(this, wxID_ANY, wxPoint(0, 0), size);

    SetTargetWindow( m_mainWin );

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // In report view the wheel moves the list by pixels: see
    // wxListMainWindow::WinUIOnWheel(). The scrolling of the base class,
    // which is given what is not handled here, waits for a whole notch and
    // moves by lines; the list gives its wheel events to this window first.
    Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent& event)
    {
        if ( m_mainWin && (InReportView() || HasFlag(wxLC_LIST)) &&
             event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL &&
             !event.ControlDown() )
        {
            m_mainWin->WinUIOnWheel(event);
        }
        else
        {
            event.Skip();
        }
    });
#endif

    // We use the cursor keys for moving the selection, not scrolling, so call
    // this method to ensure wxScrollHelperEvtHandler doesn't catch all
    // keyboard events forwarded to us from wxListMainWindow.
    DisableKeyboardScrolling();

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( m_mainWin, 1, wxGROW );
    SetSizer( sizer );

    CreateOrDestroyHeaderWindowAsNeeded();

    SetInitialSize(size);

#if defined(__WXWINUI__) && wxUSE_WINUI3
    const wxWeakRef<wxGenericListCtrl> weakThis(this);
    auto projection = wxWinUIListProjection::Create(this);
    if ( !weakThis ) return false;
    m_winUIProjection = std::move(projection);
#endif
    return true;
}

void wxGenericListCtrl::ExtendRulesAndAlternateColour(bool state)
{
    wxCHECK_RET( m_mainWin, "can't be called before creation" );

    wxASSERT_MSG( InReportView(), "can only be called in report mode" );

    m_mainWin->ExtendRulesAndAlternateColour(state);
    m_mainWin->Refresh();
}

wxBorder wxGenericListCtrl::GetDefaultBorder() const
{
    return wxBORDER_THEME;
}

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
WXLRESULT wxGenericListCtrl::MSWWindowProc(WXUINT nMsg,
                                       WXWPARAM wParam,
                                       WXLPARAM lParam)
{
    WXLRESULT rc = wxListCtrlBase::MSWWindowProc(nMsg, wParam, lParam);

    // we need to process arrows ourselves for scrolling
    if ( nMsg == WM_GETDLGCODE )
    {
        rc |= DLGC_WANTARROWS;
    }

    return rc;
}
#endif // __WXMSW__

wxSize wxGenericListCtrl::GetSizeAvailableForScrollTarget(const wxSize& size)
{
    wxSize newsize = size;
    if (m_headerWin)
       newsize.y -= m_headerWin->GetSize().y;

    return newsize;
}

void wxGenericListCtrl::OnScroll(wxScrollWinEvent& event)
{
    // update our idea of which lines are shown when we redraw
    // the window the next time
    m_mainWin->ResetVisibleLinesRange();

    if ( event.GetOrientation() == wxHORIZONTAL && HasHeader() )
    {
        m_headerWin->Refresh();
        m_headerWin->Update();
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // An arrow of the vertical bar moves the list by a line, not by the
    // pixel which is the unit of the bar in report view.
    if ( event.GetOrientation() == wxVERTICAL && InReportView() )
    {
        const wxEventType type = event.GetEventType();
        if ( type == wxEVT_SCROLLWIN_LINEUP ||
             type == wxEVT_SCROLLWIN_LINEDOWN )
        {
            m_mainWin->WinUIScrollByLines(
                type == wxEVT_SCROLLWIN_LINEUP ? -1 : 1, true);
            return;
        }
    }
#endif

    // Let the window be scrolled as usual by the default handler.
    event.Skip();
}

bool wxGenericListCtrl::HasCheckBoxes() const
{
    if (!InReportView())
        return false;

    return m_mainWin->HasCheckBoxes();
}

bool wxGenericListCtrl::EnableCheckBoxes(bool enable)
{
    if (!InReportView())
        return false;

    return m_mainWin->EnableCheckBoxes(enable);
}

void wxGenericListCtrl::CheckItem(long item, bool state)
{
    if (InReportView())
        m_mainWin->CheckItem(item, state);
}

bool wxGenericListCtrl::IsItemChecked(long item) const
{
    if (!InReportView())
        return false;

    return m_mainWin->IsItemChecked(item);
}

void wxGenericListCtrl::ShowSortIndicator(int idx, bool ascending)
{
    if ( m_headerWin &&
                (idx != m_headerWin->m_sortCol ||
                 (idx != -1 && ascending != m_headerWin->m_sortAsc)) )
    {
        m_headerWin->m_sortCol = idx;
        m_headerWin->m_sortAsc = ascending;
        m_headerWin->Refresh();
    }
}

int wxGenericListCtrl::GetSortIndicator() const
{
    if ( m_headerWin )
        return m_headerWin->m_sortCol;

    return -1;
}

bool wxGenericListCtrl::IsAscendingSortIndicator() const
{
    if ( m_headerWin )
        return m_headerWin->m_sortAsc;

    return true;
}

void wxGenericListCtrl::SetSingleStyle( long style, bool add )
{
    wxASSERT_MSG( !(style & wxLC_VIRTUAL),
                  wxT("wxLC_VIRTUAL can't be [un]set") );

    long flag = GetWindowStyle();

    if (add)
    {
        if (style & wxLC_MASK_TYPE)
            flag &= ~(wxLC_MASK_TYPE | wxLC_VIRTUAL);
        if (style & wxLC_MASK_ALIGN)
            flag &= ~wxLC_MASK_ALIGN;
        if (style & wxLC_MASK_SORT)
            flag &= ~wxLC_MASK_SORT;
    }

    if (add)
        flag |= style;
    else
        flag &= ~style;

    // some styles can be set without recreating everything (as happens in
    // SetWindowStyleFlag() which calls wxListMainWindow::DeleteEverything())
    if ( !(style & ~(wxLC_HRULES | wxLC_VRULES)) )
    {
        Refresh();
        wxWindow::SetWindowStyleFlag(flag);
    }
    else
    {
        SetWindowStyleFlag( flag );
    }
}

void wxGenericListCtrl::SetWindowStyleFlag( long flag )
{
    // we add wxHSCROLL and wxVSCROLL in ctor unconditionally and it never
    // makes sense to remove them as we'll always add scrollbars anyhow when
    // needed
    flag |= wxHSCROLL | wxVSCROLL;

    const bool wasInReportView = HasFlag(wxLC_REPORT);

    // update the window style first so that the header is created or destroyed
    // corresponding to the new style
    wxWindow::SetWindowStyleFlag( flag );

    if (m_mainWin)
    {
        const bool inReportView = (flag & wxLC_REPORT) != 0;
        if ( inReportView != wasInReportView )
        {
            // we need to notify the main window about this change as it must
            // update its data structures
            m_mainWin->SetReportView(inReportView);
        }

        // m_mainWin->DeleteEverything();  wxMSW doesn't do that

        CreateOrDestroyHeaderWindowAsNeeded();

        GetSizer()->Layout();
    }
}

bool wxGenericListCtrl::GetColumn(int col, wxListItem &item) const
{
    m_mainWin->GetColumn( col, item );
    return true;
}

bool wxGenericListCtrl::SetColumn( int col, const wxListItem& item )
{
    m_mainWin->SetColumn( col, item );
    return true;
}

int wxGenericListCtrl::GetColumnWidth( int col ) const
{
    return m_mainWin->GetColumnWidth( col );
}

bool wxGenericListCtrl::SetColumnWidth( int col, int width )
{
    m_mainWin->SetColumnWidth( col, width );
    return true;
}

// Column ordering functions
int wxGenericListCtrl::GetColumnOrder(int col) const
{
    return m_mainWin->GetColumnOrder(col);
}

int wxGenericListCtrl::GetColumnIndexFromOrder(int order) const
{
    return m_mainWin->GetColumnIndexFromOrder(order);
}

wxArrayInt wxGenericListCtrl::GetColumnsOrder() const
{
    return m_mainWin->GetColumnsOrder();
}

bool wxGenericListCtrl::SetColumnsOrder(const wxArrayInt& orders)
{
    return m_mainWin->SetColumnsOrder(orders);
}

int wxGenericListCtrl::GetCountPerPage() const
{
  return m_mainWin->GetCountPerPage();  // different from Windows ?
}

bool wxGenericListCtrl::GetItem( wxListItem &info ) const
{
    m_mainWin->GetItem( info );
    return true;
}

bool wxGenericListCtrl::SetItem( wxListItem &info )
{
    return m_mainWin->SetItem( info );
}

bool wxGenericListCtrl::SetItem( long index, int col, const wxString& label, int imageId )
{
    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    info.m_col = col;
    if ( imageId > -1 )
    {
        info.m_image = imageId;
        info.m_mask |= wxLIST_MASK_IMAGE;
    }

    return m_mainWin->SetItem(info);
}

int wxGenericListCtrl::GetItemState( long item, long stateMask ) const
{
    return m_mainWin->GetItemState( item, stateMask );
}

bool wxGenericListCtrl::SetItemState( long item, long state, long stateMask )
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const wxWeakRef<wxGenericListCtrl> weakThis(this);
    const auto projection = m_winUIProjection;
    const bool succeeded = m_mainWin->SetItemState(item, state, stateMask);
    // Offscreen selection changes don't invalidate a paint rectangle. Publish
    // their accessibility state separately, without rereading visible values.
    if ( weakThis && projection && (stateMask & wxLIST_STATE_SELECTED) )
        projection->NotifySelectionChanged();
    return succeeded;
#else
    return m_mainWin->SetItemState( item, state, stateMask );
#endif
}

bool
wxGenericListCtrl::SetItemImage( long item, int image, int WXUNUSED(selImage) )
{
    return SetItemColumnImage(item, 0, image);
}

bool
wxGenericListCtrl::SetItemColumnImage( long item, long column, int image )
{
    wxListItem info;
    info.m_image = image;
    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_itemId = item;
    info.m_col = column;
    return m_mainWin->SetItem( info );
}

wxString wxGenericListCtrl::GetItemText( long item, int col ) const
{
    return m_mainWin->GetItemText(item, col);
}

void wxGenericListCtrl::SetItemText( long item, const wxString& str )
{
    m_mainWin->SetItemText(item, str);
}

wxUIntPtr wxGenericListCtrl::GetItemData( long item ) const
{
    wxListItem info;
    info.m_mask = wxLIST_MASK_DATA;
    info.m_itemId = item;
    m_mainWin->GetItem( info );
    return info.m_data;
}

bool wxGenericListCtrl::SetItemPtrData( long item, wxUIntPtr data )
{
    wxListItem info;
    info.m_mask = wxLIST_MASK_DATA;
    info.m_itemId = item;
    info.m_data = data;
    return m_mainWin->SetItem( info );
}

wxRect wxGenericListCtrl::GetViewRect() const
{
    return m_mainWin->GetViewRect();
}

bool wxGenericListCtrl::GetItemRect(long item, wxRect& rect, int code) const
{
    return GetSubItemRect(item, wxLIST_GETSUBITEMRECT_WHOLEITEM, rect, code);
}

bool wxGenericListCtrl::GetSubItemRect(long item,
                                       long subItem,
                                       wxRect& rect,
                                       int code) const
{
    if ( !m_mainWin->GetSubItemRect( item, subItem, rect, code ) )
        return false;

    // Convert from the main window coordinates to our own ones, i.e. take
    // into account the header, if any.
    rect.Offset(m_mainWin->GetPositionInListCtrl());

    return true;
}

bool wxGenericListCtrl::GetItemPosition( long item, wxPoint& pos ) const
{
    m_mainWin->GetItemPosition( item, pos );

    // As in GetSubItemRect(), return the position in our coordinates.
    pos += m_mainWin->GetPositionInListCtrl();

    return true;
}

bool wxGenericListCtrl::SetItemPosition( long WXUNUSED(item), const wxPoint& WXUNUSED(pos) )
{
    return false;
}

int wxGenericListCtrl::GetItemCount() const
{
    return m_mainWin->GetItemCount();
}

int wxGenericListCtrl::GetColumnCount() const
{
    // wxLC_LIST is special as we want to return 1 for it, for compatibility
    // with the native wxMSW version and not the real number of columns, which
    // is 0. For the other non-wxLC_REPORT modes returning 0 is fine, however,
    // as wxMSW does it too.
    return HasFlag(wxLC_LIST) ? 1 : m_mainWin->GetColumnCount();
}

void wxGenericListCtrl::SetItemSpacing( int spacing, bool isSmall )
{
    m_mainWin->SetItemSpacing( spacing, isSmall );
}

wxSize wxGenericListCtrl::GetItemSpacing() const
{
    const int spacing = m_mainWin->GetItemSpacing(HasFlag(wxLC_SMALL_ICON));

    return wxSize(spacing, spacing);
}

void wxGenericListCtrl::SetItemTextColour( long item, const wxColour &col )
{
    wxListItem info;
    info.m_itemId = item;
    info.SetTextColour( col );
    m_mainWin->SetItem( info );
}

wxColour wxGenericListCtrl::GetItemTextColour( long item ) const
{
    wxListItem info;
    info.m_itemId = item;
    m_mainWin->GetItem( info );
    return info.GetTextColour();
}

void wxGenericListCtrl::SetItemBackgroundColour( long item, const wxColour &col )
{
    wxListItem info;
    info.m_itemId = item;
    info.SetBackgroundColour( col );
    m_mainWin->SetItem( info );
}

wxColour wxGenericListCtrl::GetItemBackgroundColour( long item ) const
{
    wxListItem info;
    info.m_itemId = item;
    m_mainWin->GetItem( info );
    return info.GetBackgroundColour();
}

void wxGenericListCtrl::SetItemFont( long item, const wxFont &f )
{
    wxListItem info;
    info.m_itemId = item;
    info.SetFont( f );
    m_mainWin->SetItem( info );
}

wxFont wxGenericListCtrl::GetItemFont( long item ) const
{
    wxListItem info;
    info.m_itemId = item;
    m_mainWin->GetItem( info );
    return info.GetFont();
}

int wxGenericListCtrl::GetSelectedItemCount() const
{
    return m_mainWin->GetSelectedItemCount();
}

wxColour wxGenericListCtrl::GetTextColour() const
{
    return GetForegroundColour();
}

void wxGenericListCtrl::SetTextColour(const wxColour& col)
{
    SetForegroundColour(col);
}

long wxGenericListCtrl::GetTopItem() const
{
    size_t top;
    m_mainWin->GetVisibleLinesRange(&top, nullptr);
    return (long)top;
}

long wxGenericListCtrl::GetNextItem( long item, int geom, int state ) const
{
    return m_mainWin->GetNextItem( item, geom, state );
}

void wxGenericListCtrl::DoUpdateImages(int which )
{
    m_mainWin->SetImages( GetImages(which), which );
}

bool wxGenericListCtrl::Arrange( int WXUNUSED(flag) )
{
    return 0;
}

bool wxGenericListCtrl::DeleteItem( long item )
{
    return m_mainWin->DeleteItem( item );
}

bool wxGenericListCtrl::DeleteAllItems()
{
    return m_mainWin->DeleteAllItems();
}

bool wxGenericListCtrl::DeleteAllColumns()
{
    size_t count = m_mainWin->m_columns.size();
    for ( size_t n = 0; n < count; n++ )
        DeleteColumn( 0 );
    return true;
}

void wxGenericListCtrl::ClearAll()
{
    m_mainWin->DeleteEverything();
}

bool wxGenericListCtrl::DeleteColumn( int col )
{
    m_mainWin->DeleteColumn( col );

    // if we don't have the header any longer, we need to relayout the window
    // if ( !GetColumnCount() )


    // Ensure that the non-existent columns are really removed from display.
    Refresh();

    return true;
}

wxTextCtrl *wxGenericListCtrl::EditLabel(long item,
                                         wxClassInfo* textControlClass)
{
    return m_mainWin->EditLabel( item, textControlClass );
}

bool wxGenericListCtrl::EndEditLabel(bool cancel)
{
    return m_mainWin->EndEditLabel(cancel);
}

wxTextCtrl *wxGenericListCtrl::GetEditControl() const
{
    return m_mainWin->GetEditControl();
}

bool wxGenericListCtrl::EnsureVisible( long item )
{
    m_mainWin->EnsureVisible( item );
    return true;
}

long wxGenericListCtrl::FindItem( long start, const wxString& str, bool partial )
{
    return m_mainWin->FindItem( start, str, partial );
}

long wxGenericListCtrl::FindItem( long start, wxUIntPtr data )
{
    return m_mainWin->FindItem( start, data );
}

long wxGenericListCtrl::FindItem( long WXUNUSED(start), const wxPoint& pt,
                           int WXUNUSED(direction))
{
    // As in HitTest(), the point is in our coordinates.
    return m_mainWin->FindItem( pt - m_mainWin->GetPositionInListCtrl() );
}

long wxGenericListCtrl::HitTest(const wxPoint& pointOrig, int& flags, long *col) const
{
    // The point is in our coordinates, as for all the other functions taking
    // or returning positions, such as GetItemRect(), and for consistency with
    // wxMSW, but the main window uses its own coordinates, which are offset
    // by the header height, if any.
    const wxPoint point = pointOrig - m_mainWin->GetPositionInListCtrl();

    if ( col )
    {
        *col = -1;
        if ( InReportView() )
        {
            const wxPoint unscrolled = CalcUnscrolledPosition( point );

            for ( int order = 0, wsum = 0, cols = GetColumnCount();
                  order < cols;
                  ++order )
            {
                const int c = GetColumnIndexFromOrder(order);
                wsum += GetColumnWidth(c);
                if ( wsum > unscrolled.x )
                {
                    *col = c;
                    break;
                }
            }
        }
    }

    return m_mainWin->HitTest(point.x, point.y, flags);
}

long wxGenericListCtrl::InsertItem( wxListItem& info )
{
    return m_mainWin->InsertItem(info) ? info.m_itemId : wxNOT_FOUND;
}

long wxGenericListCtrl::InsertItem( long index, const wxString &label )
{
    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    return InsertItem( info );
}

long wxGenericListCtrl::InsertItem( long index, int imageIndex )
{
    wxListItem info;
    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_image = imageIndex;
    info.m_itemId = index;
    return InsertItem( info );
}

long wxGenericListCtrl::InsertItem( long index, const wxString &label, int imageIndex )
{
    wxListItem info;
    info.m_text = label;
    info.m_image = imageIndex;
    info.m_mask = wxLIST_MASK_TEXT;
    if (imageIndex > -1)
        info.m_mask |= wxLIST_MASK_IMAGE;
    info.m_itemId = index;
    return InsertItem( info );
}

long wxGenericListCtrl::DoInsertColumn( long col, const wxListItem &item )
{
    wxCHECK_MSG( InReportView(), -1, wxT("can't add column in non report mode") );

    long idx = m_mainWin->InsertColumn( col, item );

    // NOTE: if wxLC_NO_HEADER was given, then we are in report view mode but
    //       still have m_headerWin==nullptr
    if (m_headerWin)
        m_headerWin->Refresh();

    return idx;
}

bool wxGenericListCtrl::ScrollList( int dx, int dy )
{
    return m_mainWin->ScrollList(dx, dy);
}

// Sort items.
// fn is a function which takes 3 long arguments: item1, item2, data.
// item1 is the long data associated with a first item (NOT the index).
// item2 is the long data associated with a second item (NOT the index).
// data is the same value as passed to SortItems.
// The return value is a negative number if the first item should precede the second
// item, a positive number of the second item should precede the first,
// or zero if the two items are equivalent.
// data is arbitrary data to be passed to the sort function.

bool wxGenericListCtrl::SortItems( wxListCtrlCompare fn, wxIntPtr data )
{
    return m_mainWin->SortItems( fn, data );
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxGenericListCtrl::OnSize(wxSizeEvent& event)
{
    if (!m_mainWin) return;

    // We need to override OnSize so that our scrolled
    // window a) does call Layout() to use sizers for
    // positioning the controls but b) does not query
    // the sizer for their size and use that for setting
    // the scrollable area as set that ourselves by
    // calling SetScrollbar() further down.

    Layout();

    m_mainWin->RecalculatePositionsAndRefresh();

    AdjustScrollbars();

    event.Skip();
}

void wxGenericListCtrl::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    if (m_mainWin->m_dirty)
        m_mainWin->RecalculatePositionsAndRefresh();
}

// ----------------------------------------------------------------------------
// font/colours
// ----------------------------------------------------------------------------

bool wxGenericListCtrl::SetBackgroundColour( const wxColour &colour )
{
    if ( !wxWindow::SetBackgroundColour( colour ) )
        return false;

    if (m_mainWin)
    {
        m_mainWin->SetBackgroundColour( colour );
        m_mainWin->m_dirty = true;
    }

    return true;
}

bool wxGenericListCtrl::SetForegroundColour( const wxColour &colour )
{
    if ( !wxWindow::SetForegroundColour( colour ) )
        return false;

    if (m_mainWin)
    {
        m_mainWin->SetForegroundColour( colour );
        m_mainWin->m_dirty = true;
    }

    return true;
}

bool wxGenericListCtrl::SetFont( const wxFont &font )
{
    if (!BaseType::SetFont(font))
        return false;

    if (m_mainWin)
    {
        m_mainWin->SetFont( font );
        m_mainWin->m_dirty = true;
    }

    if (m_headerWin)
    {
        m_headerWin->SetFont( font );
        // CalculateAndSetHeaderHeight();
    }

    Refresh();

    return true;
}

// static
wxVisualAttributes
wxGenericListCtrl::GetClassDefaultAttributes(wxWindowVariant variant)
{
#if _USE_VISATTR
    // Use the same color scheme as wxListBox
    return wxListBox::GetClassDefaultAttributes(variant);
#else
    wxUnusedVar(variant);
    wxVisualAttributes attr;
    attr.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
    attr.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    attr.font  = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    return attr;
#endif
}

// ----------------------------------------------------------------------------
// methods forwarded to m_mainWin
// ----------------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP

void wxGenericListCtrl::SetDropTarget( wxDropTarget *dropTarget )
{
    m_mainWin->SetDropTarget( dropTarget );
}

wxDropTarget *wxGenericListCtrl::GetDropTarget() const
{
    return m_mainWin->GetDropTarget();
}

#endif

bool wxGenericListCtrl::SetCursor( const wxCursor &cursor )
{
    return m_mainWin ? m_mainWin->wxWindow::SetCursor(cursor) : false;
}

wxColour wxGenericListCtrl::GetBackgroundColour() const
{
    return m_mainWin ? m_mainWin->GetBackgroundColour() : wxColour();
}

wxColour wxGenericListCtrl::GetForegroundColour() const
{
    return m_mainWin ? m_mainWin->GetForegroundColour() : wxColour();
}

wxSize wxGenericListCtrl::DoGetBestClientSize() const
{
    // The base class version can compute the best size in report view only.
    wxSize sizeBest = wxListCtrlBase::DoGetBestClientSize();

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // Without a header the base class gives an arbitrary width (50
    // characters): a wxListbook then gave 400 pixels to the list of its
    // pages, which the native list of wxMSW makes as wide as their names.
    // Such a list is as wide as its columns.
    if ( InReportView() && HasFlag(wxLC_NO_HEADER) && GetColumnCount() > 0 )
    {
        int width = 0;
        for ( int col = 0; col < GetColumnCount(); ++col )
            width += GetColumnWidth(col);
        sizeBest.x = width;
    }
#endif

    if ( !InReportView() )
    {
        // Ensure that our minimal width is at least big enough to show all our
        // items. This is important for wxListbook to size itself correctly.

        // Remember the offset of the first item: this corresponds to the
        // margins around the item so we will add it to the minimal size below
        // to ensure that we have equal margins on all sides.
        wxPoint ofs;

        // We can iterate over all items as there shouldn't be too many of them
        // in non-report view. If it ever becomes a problem, we could examine
        // just the first few items probably, the determination of the best
        // size is less important if we will need scrollbars anyhow.
        for ( int n = 0; n < GetItemCount(); n++ )
        {
            const wxRect itemRect = m_mainWin->GetLineRect(n);
            if ( !n )
            {
                // Remember the position of the first item as all the rest are
                // offset by at least this number of pixels too.
                ofs = itemRect.GetPosition();
            }

            sizeBest.IncTo(itemRect.GetSize());
        }

        sizeBest.IncBy(2*ofs);


        // If we have the scrollbars we need to account for them too. And to
        // make sure the scrollbars status is up to date we need to call this
        // function to set them.
        m_mainWin->RecalculatePositions();

        // Unfortunately we can't use wxWindow::HasScrollbar() here as we need
        // to use m_mainWin client/virtual size for determination of whether we
        // use scrollbars and not the size of this window itself. Maybe that
        // function should be extended to work correctly in the case when our
        // scrollbars manage a different window from this one but currently it
        // doesn't work.
        const wxSize sizeClient = m_mainWin->GetClientSize();
        const wxSize sizeVirt = m_mainWin->GetVirtualSize();

        if ( sizeVirt.x > sizeClient.x /* HasScrollbar(wxHORIZONTAL) */ )
            sizeBest.y += GetScrollbarSize(wxHORIZONTAL);

        if ( sizeVirt.y > sizeClient.y /* HasScrollbar(wxVERTICAL) */ )
            sizeBest.x += GetScrollbarSize(wxVERTICAL);
    }

    return sizeBest;
}

// ----------------------------------------------------------------------------
// virtual list control support
// ----------------------------------------------------------------------------

void wxGenericListCtrl::SetItemCount(long count)
{
    wxASSERT_MSG( IsVirtual(), wxT("this is for virtual controls only") );

    m_mainWin->SetItemCount(count);
}

void wxGenericListCtrl::RefreshItem(long item)
{
    m_mainWin->RefreshLine(item);
}

void wxGenericListCtrl::RefreshItems(long itemFrom, long itemTo)
{
    m_mainWin->RefreshLines(itemFrom, itemTo);
}

void wxGenericListCtrl::EnableBellOnNoMatch( bool on )
{
    m_mainWin->EnableBellOnNoMatch(on);
}

// Generic wxListCtrl is more or less a container for two other
// windows which drawings are done upon. These are namely
// 'm_headerWin' and 'm_mainWin'.
// Here we override 'virtual wxWindow::Refresh()' to mimic the
// behaviour wxListCtrl has under wxMSW.
//
void wxGenericListCtrl::Refresh(bool eraseBackground, const wxRect *rect)
{
    if (!rect)
    {
        // The easy case, no rectangle specified.
        if (m_headerWin)
            m_headerWin->Refresh(eraseBackground);

        if (m_mainWin)
            m_mainWin->Refresh(eraseBackground);
    }
    else
    {
        // Refresh the header window
        if (m_headerWin)
        {
            wxRect rectHeader = m_headerWin->GetRect();
            rectHeader.Intersect(*rect);
            if (rectHeader.GetWidth() && rectHeader.GetHeight())
            {
                int x, y;
                m_headerWin->GetPosition(&x, &y);
                rectHeader.Offset(-x, -y);
                m_headerWin->Refresh(eraseBackground, &rectHeader);
            }
        }

        // Refresh the main window
        if (m_mainWin)
        {
            wxRect rectMain = m_mainWin->GetRect();
            rectMain.Intersect(*rect);
            if (rectMain.GetWidth() && rectMain.GetHeight())
            {
                int x, y;
                m_mainWin->GetPosition(&x, &y);
                rectMain.Offset(-x, -y);
                m_mainWin->Refresh(eraseBackground, &rectMain);
            }
        }
    }
}

void wxGenericListCtrl::Update()
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const wxWeakRef<wxGenericListCtrl> weakThis(this);
    const auto projection = m_winUIProjection;
#endif
    if ( m_mainWin )
    {
        if ( m_mainWin->m_dirty )
            m_mainWin->RecalculatePositionsAndRefresh();

        m_mainWin->Update();
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( !weakThis ) return;
#endif
    if ( m_headerWin )
        m_headerWin->Update();
#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( weakThis && projection ) projection->Update();
#endif
}

#if defined(__WXWINUI__) && wxUSE_WINUI3
void wxListMainWindow::Refresh(bool eraseBackground, const wxRect* rect)
{
    if ( GetListCtrl()->m_winUIProjection )
        GetListCtrl()->m_winUIProjection->Invalidate();
    wxWindow::Refresh(eraseBackground, rect);
}

void wxListMainWindow::ScrollWindow(int dx, int dy, const wxRect* rect)
{
    const wxWeakRef<wxListMainWindow> weakThis(this);
    ResetVisibleLinesRange();
    wxWindow::ScrollWindow(dx, dy, rect);
    if ( weakThis && GetListCtrl()->m_winUIProjection )
        GetListCtrl()->m_winUIProjection->SyncViewport();
}

void wxListHeaderWindow::Refresh(bool eraseBackground, const wxRect* rect)
{
    if ( m_owner && m_owner->GetListCtrl()->m_winUIProjection )
        m_owner->GetListCtrl()->m_winUIProjection->Invalidate();
    wxWindow::Refresh(eraseBackground, rect);
}
#endif

#endif // wxUSE_LISTCTRL
