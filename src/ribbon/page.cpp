///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/page.cpp
// Purpose:     Container for ribbon-bar-style interface panels
// Author:      Peter Cawley
// Created:     2009-05-25
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/page.h"
#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/dcbuffer.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

static int GetSizeInOrientation(wxSize size, wxOrientation orientation);

// As scroll buttons need to be rendered on top of a page's child windows, the
// buttons themselves have to be proper child windows (rather than just painted
// onto the page). In order to get proper clipping of a page's children (with
// regard to the scroll button), the scroll buttons are created as children of
// the ribbon bar rather than children of the page. This could not have been
// achieved by creating buttons as children of the page and then doing some Z-order
// manipulation, as this causes problems on win32 due to ribbon panels having the
// transparent flag set.
class wxRibbonPageScrollButton : public wxRibbonControl
{
public:
    explicit wxRibbonPageScrollButton(wxRibbonPage* sibling,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0);

    virtual ~wxRibbonPageScrollButton();

protected:
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    void OnEraseBackground(wxEraseEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void OnMouseEnter(wxMouseEvent& evt);
    void OnMouseLeave(wxMouseEvent& evt);
    void OnMouseDown(wxMouseEvent& evt);
    void OnMouseUp(wxMouseEvent& evt);

    wxRibbonPage* m_sibling = nullptr;
    long m_flags;

    wxDECLARE_CLASS(wxRibbonPageScrollButton);
    wxDECLARE_EVENT_TABLE();
};

wxIMPLEMENT_CLASS(wxRibbonPageScrollButton, wxRibbonControl);

wxBEGIN_EVENT_TABLE(wxRibbonPageScrollButton, wxRibbonControl)
    EVT_ENTER_WINDOW(wxRibbonPageScrollButton::OnMouseEnter)
    EVT_ERASE_BACKGROUND(wxRibbonPageScrollButton::OnEraseBackground)
    EVT_LEAVE_WINDOW(wxRibbonPageScrollButton::OnMouseLeave)
    EVT_LEFT_DOWN(wxRibbonPageScrollButton::OnMouseDown)
    EVT_LEFT_UP(wxRibbonPageScrollButton::OnMouseUp)
    EVT_PAINT(wxRibbonPageScrollButton::OnPaint)
wxEND_EVENT_TABLE()

wxRibbonPageScrollButton::wxRibbonPageScrollButton(wxRibbonPage* sibling,
                 wxWindowID id,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style) : wxRibbonControl(sibling->GetParent(), id, pos, size, wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_sibling = sibling;
    m_flags = (style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK) | wxRIBBON_SCROLL_BTN_FOR_PAGE;
}

wxRibbonPageScrollButton::~wxRibbonPageScrollButton()
{
}

void wxRibbonPageScrollButton::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // Do nothing - all painting done in main paint handler
}

void wxRibbonPageScrollButton::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);
    if ( m_art )
    {
        m_art->DrawScrollButton(dc, this, GetSize(), m_flags);
    }
}

void wxRibbonPageScrollButton::OnMouseEnter(wxMouseEvent& WXUNUSED(evt))
{
    m_flags |= wxRIBBON_SCROLL_BTN_HOVERED;
    Refresh(false);
}

void wxRibbonPageScrollButton::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
    m_flags &= ~wxRIBBON_SCROLL_BTN_HOVERED;
    m_flags &= ~wxRIBBON_SCROLL_BTN_ACTIVE;
    Refresh(false);
}

void wxRibbonPageScrollButton::OnMouseDown(wxMouseEvent& WXUNUSED(evt))
{
    m_flags |= wxRIBBON_SCROLL_BTN_ACTIVE;
    Refresh(false);
}

void wxRibbonPageScrollButton::OnMouseUp(wxMouseEvent& WXUNUSED(evt))
{
    if ( m_flags & wxRIBBON_SCROLL_BTN_ACTIVE )
    {
        m_flags &= ~wxRIBBON_SCROLL_BTN_ACTIVE;
        Refresh(false);
        switch ( m_flags & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
        {
        case wxRIBBON_SCROLL_BTN_DOWN:
        case wxRIBBON_SCROLL_BTN_RIGHT:
            m_sibling->ScrollSections(1);
            break;
        case wxRIBBON_SCROLL_BTN_UP:
        case wxRIBBON_SCROLL_BTN_LEFT:
            m_sibling->ScrollSections(-1);
            break;
        default:
            break;
        }
    }
}

wxIMPLEMENT_CLASS(wxRibbonPage, wxRibbonControl);

wxBEGIN_EVENT_TABLE(wxRibbonPage, wxRibbonControl)
    EVT_ERASE_BACKGROUND(wxRibbonPage::OnEraseBackground)
    EVT_PAINT(wxRibbonPage::OnPaint)
    EVT_SIZE(wxRibbonPage::OnSize)
    EVT_DPI_CHANGED(wxRibbonPage::OnDPIChanged)
wxEND_EVENT_TABLE()

wxRibbonPage::wxRibbonPage()
{
    m_scrollLeftBtn = nullptr;
    m_scrollRightBtn = nullptr;
    m_scrollAmount = 0;
    m_scrollButtonsVisible = false;
}

wxRibbonPage::wxRibbonPage(wxRibbonBar* parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxBitmapBundle& icon,
                   long WXUNUSED(style))
    : wxRibbonControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
    CommonInit(label, icon);
}

wxRibbonPage::~wxRibbonPage()
{
    delete[] m_sizeCalcArray;
    delete m_scrollLeftBtn;
    delete m_scrollRightBtn;
}

bool wxRibbonPage::Create(wxRibbonBar* parent,
                wxWindowID id,
                const wxString& label,
                const wxBitmapBundle& icon,
                long WXUNUSED(style))
{
    if ( !wxRibbonControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE) )
        return false;

    CommonInit(label, icon);

    return true;
}

void wxRibbonPage::CommonInit(const wxString& label, const wxBitmapBundle& icon)
{
    SetName(label);

    SetLabel(label);
    m_icon = icon;
    m_scrollLeftBtn = nullptr;
    m_scrollRightBtn = nullptr;
    m_sizeCalcArray = nullptr;
    m_sizeCalcArraySize = 0;
    m_scrollAmount = 0;
    m_scrollButtonsVisible = false;

    SetBackgroundStyle(wxBG_STYLE_PAINT);

    if ( auto* const bar = wxCheckedStaticCast<wxRibbonBar>(GetParent()) )
        bar->AddPage(this);
}

void wxRibbonPage::SetArtProvider(wxRibbonArtProvider* art)
{
    m_art = art;
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* child = node->GetData();
        wxRibbonControl* ribbonChild = wxDynamicCast(child, wxRibbonControl);
        if ( ribbonChild )
        {
            ribbonChild->SetArtProvider(art);
        }
    }

    // The scroll buttons are children of the parent ribbon control, not the
    // page, so they're not taken into account by the loop above, but they
    // still use the same art provider, so we need to update them too.
    if ( m_scrollLeftBtn )
        m_scrollLeftBtn->SetArtProvider(art);
    if ( m_scrollRightBtn )
        m_scrollRightBtn->SetArtProvider(art);
}

void wxRibbonPage::AdjustRectToIncludeScrollButtons(wxRect* rect) const
{
    if ( m_scrollButtonsVisible )
    {
        if ( GetMajorAxis() == wxVERTICAL )
        {
            if ( m_scrollLeftBtn )
            {
                rect->SetY(rect->GetY() -
                    m_scrollLeftBtn->GetSize().GetHeight());
                rect->SetHeight(rect->GetHeight() +
                    m_scrollLeftBtn->GetSize().GetHeight());
            }
            if ( m_scrollRightBtn )
            {
                rect->SetHeight(rect->GetHeight() +
                    m_scrollRightBtn->GetSize().GetHeight());
            }
        }
        else
        {
            if ( m_scrollLeftBtn )
            {
                rect->SetX(rect->GetX() -
                    m_scrollLeftBtn->GetSize().GetWidth());
                rect->SetWidth(rect->GetWidth() +
                    m_scrollLeftBtn->GetSize().GetWidth());
            }
            if ( m_scrollRightBtn )
            {
                rect->SetWidth(rect->GetWidth() +
                    m_scrollRightBtn->GetSize().GetWidth());
            }
        }
    }
}

void wxRibbonPage::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // All painting done in main paint handler to minimise flicker
}

void wxRibbonPage::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    // No foreground painting done by the page itself, but a paint DC
    // must be created anyway.
    wxAutoBufferedPaintDC dc(this);
    wxRect rect(GetSize());
    AdjustRectToIncludeScrollButtons(&rect);
    m_art->DrawPageBackground(dc, this, rect);
}

wxOrientation wxRibbonPage::GetMajorAxis() const
{
    if ( m_art && (m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL) )
    {
        return wxVERTICAL;
    }
    else
    {
        return wxHORIZONTAL;
    }
}

bool wxRibbonPage::ScrollLines(int lines)
{
    return ScrollPixels(lines * 8);
}

bool wxRibbonPage::ScrollPixels(int pixels)
{
    if ( pixels < 0 )
    {
        if ( m_scrollAmount == 0 )
            return false;
        if ( m_scrollAmount < -pixels )
            pixels = -m_scrollAmount;
    }
    else if ( pixels > 0 )
    {
        if ( m_scrollAmount == m_scrollAmountLimit )
            return false;
        if ( m_scrollAmount + pixels > m_scrollAmountLimit )
            pixels = m_scrollAmountLimit - m_scrollAmount;
    }
    else
        return false;

    m_scrollAmount += pixels;

    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
    {
        wxWindow* child = node->GetData();
        int x, y;
        child->GetPosition(&x, &y);
        if ( GetMajorAxis() == wxHORIZONTAL )
            x -= pixels;
        else
            y -= pixels;
        child->SetPosition(wxPoint(x, y));
    }

    if ( ShowScrollButtons() )
        DoActualLayout();
    Refresh();
    return true;
}

bool wxRibbonPage::ScrollSections(int sections)
{
    // Currently the only valid values are -1 and 1 for scrolling left and
    // right, respectively.
    const bool scrollForward = sections >= 1;

    // Determine by how many pixels to scroll. If something on the page
    // is partially visible, scroll to make it fully visible. Otherwise
    // find the next item that will become visible and scroll to make it
    // fully visible. The ScrollPixel call will correct if we scroll too
    // much if the available width is smaller than the items.

    // Scroll at minimum the same amount as ScrollLines(1):
    int minScroll = sections * 8;
    // How many pixels to scroll:
    int pixels = 0;

    // Determine the scroll position, that is, the page border where items
    // are appearing.
    int scrollPos = 0;

    wxOrientation majorAxis = GetMajorAxis();
    int gap = 0;

    int width = 0;
    int height = 0;
    int x = 0;
    int y = 0;
    GetSize(&width, &height);
    GetPosition(&x, &y);
    if ( majorAxis == wxHORIZONTAL )
    {
        gap = m_art->GetMetric(wxRIBBON_ART_PANEL_X_SEPARATION_SIZE);
        if ( scrollForward )
        {
            scrollPos = width - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
        }
        else
        {
            scrollPos = m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE);
        }
    }
    else
    {
        gap = m_art->GetMetric(wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE);
        if ( scrollForward )
        {
            scrollPos = width - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
        }
        else
        {
            scrollPos = m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE);
        }
    }

    // Find the child that is partially shown or just beyond the scroll position
    for ( wxWindowList::compatibility_iterator
            node = scrollForward ? GetChildren().GetFirst()
                                 : GetChildren().GetLast();
        node;
        node = scrollForward ? node->GetNext()
                             : node->GetPrevious() )
    {
        wxWindow* child = node->GetData();
        child->GetSize(&width, &height);
        child->GetPosition(&x, &y);
        int pos0 = 0;
        int pos1 = 0;
        if ( majorAxis == wxHORIZONTAL )
        {
            pos0 = x;
            pos1 = x + width + gap;
        }
        else
        {
            pos0 = y;
            pos1 = y + height + gap;
        }
        if ( scrollPos >= pos0 && scrollPos <= pos1 )
        {
            // This section is partially visible, scroll to make it fully visible.
            if ( scrollForward )
            {
                pixels += pos1 - scrollPos;
            }
            else
            {
                pixels += pos0 - scrollPos;
            }
            if ( abs(pixels) >= abs(minScroll) )
                break;
        }
        if ( scrollPos <= pos0 && scrollForward )
        {
            // This section is next, scroll the entire section width
            pixels += (pos1 - pos0);
            break;
        }
        if ( scrollPos >= pos1 && !scrollForward )
        {
            // This section is next, scroll the entire section width
            pixels += (pos0 - pos1);
            break;
        }
    }
    // Do a final safety sanity check, should not be necessary, but will not hurt either.
    if ( pixels == 0 )
    {
        pixels = minScroll;
    }
    if ( pixels * minScroll < 0 )
    {
        pixels = -pixels;
    }

    return ScrollPixels(pixels);
}

void wxRibbonPage::SetSizeWithScrollButtonAdjustment(int x, int y, int width, int height)
{
    if ( m_scrollButtonsVisible )
    {
        if ( GetMajorAxis() == wxHORIZONTAL )
        {
            if ( m_scrollLeftBtn )
            {
                int w = m_scrollLeftBtn->GetSize().GetWidth();
                m_scrollLeftBtn->SetPosition(wxPoint(x, y));
                x += w;
                width -= w;
            }
            if ( m_scrollRightBtn )
            {
                int w = m_scrollRightBtn->GetSize().GetWidth();
                width -= w;
                m_scrollRightBtn->SetPosition(wxPoint(x + width, y));
            }
        }
        else
        {
            if ( m_scrollLeftBtn )
            {
                int h = m_scrollLeftBtn->GetSize().GetHeight();
                m_scrollLeftBtn->SetPosition(wxPoint(x, y));
                y += h;
                height -= h;
            }
            if ( m_scrollRightBtn )
            {
                int h = m_scrollRightBtn->GetSize().GetHeight();
                height -= h;
                m_scrollRightBtn->SetPosition(wxPoint(x, y + height));
            }
        }
    }
    if ( width < 0 ) width = 0;
    if ( height < 0 ) height = 0;
    SetSize(x, y, width, height);
}

void wxRibbonPage::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // When a resize triggers the scroll buttons to become visible, the page is resized.
    // This resize from within a resize event can cause (MSW) wxWidgets some confusion,
    // and report the 1st size to the 2nd size event. Hence the most recent size is
    // remembered internally and used in Layout() where appropriate.

    if ( GetMajorAxis() == wxHORIZONTAL )
    {
        m_sizeInMajorAxisForChildren = width;
        if ( m_scrollButtonsVisible )
        {
            if ( m_scrollLeftBtn )
                m_sizeInMajorAxisForChildren += m_scrollLeftBtn->GetSize().GetWidth();
            if ( m_scrollRightBtn )
                m_sizeInMajorAxisForChildren += m_scrollRightBtn->GetSize().GetWidth();
        }
    }
    else
    {
        m_sizeInMajorAxisForChildren = height;
        if ( m_scrollButtonsVisible )
        {
            if ( m_scrollLeftBtn )
                m_sizeInMajorAxisForChildren += m_scrollLeftBtn->GetSize().GetHeight();
            if ( m_scrollRightBtn )
                m_sizeInMajorAxisForChildren += m_scrollRightBtn->GetSize().GetHeight();
        }
    }

    wxRibbonControl::DoSetSize(x, y, width, height, sizeFlags);
}

void wxRibbonPage::OnSize(wxSizeEvent& evt)
{
    wxSize newSize = evt.GetSize();

    if ( m_art )
    {
        wxMemoryDC tempDc;
        wxRect invalidRect = m_art->GetPageBackgroundRedrawArea(tempDc, this, m_oldSize, newSize);
        Refresh(true, &invalidRect);
    }

    m_oldSize = newSize;

    if ( newSize.GetX() > 0 && newSize.GetY() > 0 )
    {
        Layout();
    }
    else
    {
        // Simplify other calculations by pretending new size is zero in both
        // X and Y
        newSize.Set(0, 0);
        // When size == 0, no point in doing any layout
    }

    evt.Skip();
}

void wxRibbonPage::OnDPIChanged(wxDPIChangedEvent& event)
{
    Realize();
    event.Skip();
}

void wxRibbonPage::RemoveChild(wxWindowBase *child)
{
    // Remove all references to the child from the collapse stack
    size_t count = m_collapseStack.GetCount();
    size_t src, dst;
    for ( src = 0, dst = 0; src < count; ++src, ++dst )
    {
        wxRibbonControl *item = m_collapseStack.Item(src);
        if ( item == child )
        {
            ++src;
            if ( src == count )
            {
                break;
            }
        }
        if ( src != dst )
        {
            m_collapseStack.Item(dst) = item;
        }
    }
    if ( src > dst )
    {
        m_collapseStack.RemoveAt(dst, src - dst);
    }

    // ... and then proceed as normal
    wxRibbonControl::RemoveChild(child);
}

bool wxRibbonPage::Realize()
{
    bool status = true;

    m_collapseStack.Clear();
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
                  node;
                  node = node->GetNext() )
    {
        wxRibbonControl* child = wxDynamicCast(node->GetData(), wxRibbonControl);
        if ( child == nullptr )
        {
            continue;
        }
        if ( !child->Realize() )
        {
            status = false;
        }
    }
    PopulateSizeCalcArray(&wxWindow::GetMinSize);

    return DoActualLayout() && status;
}

void wxRibbonPage::PopulateSizeCalcArray(wxSize (wxWindow::*getSize)(void) const)
{
    wxSize parentSize = GetSize();
    parentSize.x -= m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE);
    parentSize.x -= m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
    parentSize.y -= m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE);
    parentSize.y -= m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);

    if ( m_sizeCalcArraySize != GetChildren().GetCount() )
    {
        delete[] m_sizeCalcArray;
        m_sizeCalcArraySize = GetChildren().GetCount();
        m_sizeCalcArray = new wxSize[m_sizeCalcArraySize];
    }

    wxSize* nodeSize = m_sizeCalcArray;
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext(), ++nodeSize )
    {
        wxWindow* child = node->GetData();
        wxRibbonPanel* panel = wxDynamicCast(child, wxRibbonPanel);
        if ( panel && panel->GetFlags() & wxRIBBON_PANEL_FLEXIBLE )
            *nodeSize = panel->GetBestSizeForParentSize(parentSize);
        else
            *nodeSize = (child->*getSize)();
    }
}

bool wxRibbonPage::Layout()
{
    if ( GetChildren().GetCount() == 0 )
    {
        return true;
    }
    else
    {
        PopulateSizeCalcArray(&wxWindow::GetSize);
        return DoActualLayout();
    }
}

bool wxRibbonPage::DoActualLayout()
{
    wxPoint origin(m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE), m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE));
    wxOrientation majorAxis = GetMajorAxis();
    int gap;
    int minorAxisSize;
    int availableSpace;
    if ( majorAxis == wxHORIZONTAL )
    {
        gap = m_art->GetMetric(wxRIBBON_ART_PANEL_X_SEPARATION_SIZE);
        minorAxisSize = GetSize().GetHeight() - origin.y - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
        availableSpace = m_sizeInMajorAxisForChildren - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE) - origin.x;
    }
    else
    {
        gap = m_art->GetMetric(wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE);
        minorAxisSize = GetSize().GetWidth() - origin.x - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
        availableSpace = m_sizeInMajorAxisForChildren - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE) - origin.y;
    }
    if ( minorAxisSize < 0 ) minorAxisSize = 0;
    size_t sizeIndex;
    for ( sizeIndex = 0; sizeIndex < m_sizeCalcArraySize; ++sizeIndex )
    {
        if ( majorAxis == wxHORIZONTAL )
        {
            availableSpace -= m_sizeCalcArray[sizeIndex].GetWidth();
            m_sizeCalcArray[sizeIndex].SetHeight(minorAxisSize);
        }
        else
        {
            availableSpace -= m_sizeCalcArray[sizeIndex].GetHeight();
            m_sizeCalcArray[sizeIndex].SetWidth(minorAxisSize);
        }
        if ( sizeIndex != 0 )
            availableSpace -= gap;
    }
    bool todoHideScrollButtons = false;
    bool todoShowScrollButtons = false;
    if ( availableSpace >= 0 )
    {
        if ( m_scrollButtonsVisible )
            todoHideScrollButtons = true;
        if ( availableSpace > 0 )
            ExpandPanels(majorAxis, availableSpace);
    }
    else
    {
        if ( m_scrollButtonsVisible )
        {
            // Scroll buttons already visible - not going to be able to downsize any more
            m_scrollAmountLimit = -availableSpace;
            if ( m_scrollAmount > m_scrollAmountLimit )
            {
                m_scrollAmount = m_scrollAmountLimit;
                todoShowScrollButtons = true;
            }
        }
        else
        {
            if ( !CollapsePanels(majorAxis, -availableSpace) )
            {
                m_scrollAmount = 0;
                m_scrollAmountLimit = -availableSpace;
                todoShowScrollButtons = true;
            }
        }
    }
    if ( m_scrollButtonsVisible )
    {
        if ( majorAxis == wxHORIZONTAL )
        {
            origin.x -= m_scrollAmount;
            if ( m_scrollLeftBtn )
                origin.x -= m_scrollLeftBtn->GetSize().GetWidth();
        }
        else
        {
            origin.y -= m_scrollAmount;
            if ( m_scrollLeftBtn )
                origin.y -= m_scrollLeftBtn->GetSize().GetHeight();
        }
    }
    sizeIndex = 0;
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
        node;
        node = node->GetNext(), ++sizeIndex )
    {
        wxWindow* child = node->GetData();
        int w = m_sizeCalcArray[sizeIndex].GetWidth();
        int h = m_sizeCalcArray[sizeIndex].GetHeight();
        child->SetSize(origin.x, origin.y, w, h);
        if ( majorAxis == wxHORIZONTAL )
        {
            origin.x += w + (child->IsShown() ? gap : 0);
        }
        else
        {
            origin.y += h + (child->IsShown() ? gap : 0);
        }
    }

    if ( todoShowScrollButtons )
        ShowScrollButtons();
    else if ( todoHideScrollButtons )
        HideScrollButtons();
    else if ( m_scrollButtonsVisible )
        ShowScrollButtons();

    Refresh();
    return true;
}

bool wxRibbonPage::Show(bool show)
{
    if ( m_scrollLeftBtn )
        m_scrollLeftBtn->Show(show);
    if ( m_scrollRightBtn )
        m_scrollRightBtn->Show(show);
    return wxRibbonControl::Show(show);
}

void wxRibbonPage::HideScrollButtons()
{
    m_scrollAmount = 0;
    m_scrollAmountLimit = 0;
    ShowScrollButtons();
}

bool wxRibbonPage::ShowScrollButtons()
{
    bool showLeft = true;
    bool showRight = true;
    bool reposition = false;
    if ( m_scrollAmount == 0 )
    {
        showLeft = false;
    }
    if ( m_scrollAmount >= m_scrollAmountLimit )
    {
        showRight = false;
        m_scrollAmount = m_scrollAmountLimit;
    }
    m_scrollButtonsVisible = showLeft || showRight;

    if ( showLeft )
    {
        wxMemoryDC tempDc;
        wxSize size;
        long direction;
        if ( GetMajorAxis() == wxHORIZONTAL )
        {
              direction = wxRIBBON_SCROLL_BTN_LEFT;
              size = m_art->GetScrollButtonMinimumSize(tempDc, GetParent(), direction);
              size.SetHeight(GetSize().GetHeight());
        }
        else
        {
              direction = wxRIBBON_SCROLL_BTN_UP;
              size = m_art->GetScrollButtonMinimumSize(tempDc, GetParent(), direction);
              size.SetWidth(GetSize().GetWidth());
        }
        if ( m_scrollLeftBtn )
        {
              m_scrollLeftBtn->SetSize(size);
        }
        else
        {
              m_scrollLeftBtn = new wxRibbonPageScrollButton(this, wxID_ANY, GetPosition(), size, direction);
              reposition = true;
        }
        if ( !IsShown() )
        {
              m_scrollLeftBtn->Hide();
        }
    }
    else
    {
        if ( m_scrollLeftBtn != nullptr )
        {
            m_scrollLeftBtn->Destroy();
            m_scrollLeftBtn = nullptr;
            reposition = true;
        }
    }

    if ( showRight )
    {
        wxMemoryDC tempDc;
        wxSize size;
        long direction;
        if ( GetMajorAxis() == wxHORIZONTAL )
        {
              direction = wxRIBBON_SCROLL_BTN_RIGHT;
              size = m_art->GetScrollButtonMinimumSize(tempDc, GetParent(), direction);
              size.SetHeight(GetSize().GetHeight());
        }
        else
        {
              direction = wxRIBBON_SCROLL_BTN_DOWN;
              size = m_art->GetScrollButtonMinimumSize(tempDc, GetParent(), direction);
              size.SetWidth(GetSize().GetWidth());
        }
        wxPoint initialPos = GetPosition() + GetSize() - size;
        if ( m_scrollRightBtn )
        {
              m_scrollRightBtn->SetSize(size);
        }
        else
        {
              m_scrollRightBtn = new wxRibbonPageScrollButton(this, wxID_ANY, initialPos, size, direction);
              reposition = true;
        }
        if ( !IsShown() )
        {
              m_scrollRightBtn->Hide();
        }
    }
    else
    {
        if ( m_scrollRightBtn != nullptr )
        {
            m_scrollRightBtn->Destroy();
            m_scrollRightBtn = nullptr;
            reposition = true;
        }
    }

    if ( reposition )
    {
        wxASSERT_MSG(wxDynamicCast(GetParent(), wxRibbonBar), "pointer of wrong type?");
        static_cast<wxRibbonBar*>(GetParent())->RepositionPage(this);
    }

    return reposition;
}

static int GetSizeInOrientation(wxSize size, wxOrientation orientation)
{
    switch ( orientation )
    {
    case wxHORIZONTAL: return size.GetWidth();
    case wxVERTICAL: return size.GetHeight();
    case wxBOTH: return size.GetWidth() * size.GetHeight();
    default: return 0;
    }
}

bool wxRibbonPage::ExpandPanels(wxOrientation direction, int maximum_amount)
{
    bool expandedSomething = false;
    while ( maximum_amount > 0 )
    {
        int smallestSize = INT_MAX;
        wxRibbonPanel* smallestPanel = nullptr;
        wxSize* smallestPanelSize = nullptr;
        wxSize* panelSize = m_sizeCalcArray;
        for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
                  node;
                  node = node->GetNext(), ++panelSize )
        {
            wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
            if ( panel == nullptr || !panel->IsShown() )
            {
                continue;
            }
            if ( panel->GetFlags() & wxRIBBON_PANEL_FLEXIBLE )
            {
                // Don't change if it's flexible since we already calculated the
                // correct size for the panel.
            }
            else if ( panel->IsSizingContinuous() )
            {
                int size = GetSizeInOrientation(*panelSize, direction);
                if ( size < smallestSize )
                {
                    smallestSize = size;
                    smallestPanel = panel;
                    smallestPanelSize = panelSize;
                }
            }
            else
            {
                int size = GetSizeInOrientation(*panelSize, direction);
                if ( size < smallestSize )
                {
                    wxSize larger = panel->GetNextLargerSize(direction, *panelSize);
                    if ( larger != (*panelSize) && GetSizeInOrientation(larger, direction) > size )
                    {
                        smallestSize = size;
                        smallestPanel = panel;
                        smallestPanelSize = panelSize;
                    }
                }
            }
        }
        if ( smallestPanel != nullptr )
        {
            if ( smallestPanel->IsSizingContinuous() )
            {
                int amount = maximum_amount;
                if ( amount > 32 )
                {
                    // For "large" growth, grow this panel a bit, and then re-allocate
                    // the remainder (which may come to this panel again anyway)
                    amount = 32;
                }
                if ( direction & wxHORIZONTAL )
                {
                    smallestPanelSize->x += amount;
                }
                if ( direction & wxVERTICAL )
                {
                    smallestPanelSize->y += amount;
                }
                maximum_amount -= amount;
                m_collapseStack.Add(smallestPanel);
                expandedSomething = true;
            }
            else
            {
                wxSize larger = smallestPanel->GetNextLargerSize(direction, *smallestPanelSize);
                wxSize delta = larger - (*smallestPanelSize);
                if ( GetSizeInOrientation(delta, direction) <= maximum_amount )
                {
                    *smallestPanelSize = larger;
                    maximum_amount -= GetSizeInOrientation(delta, direction);
                    m_collapseStack.Add(smallestPanel);
                    expandedSomething = true;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }
    return expandedSomething;
}

bool wxRibbonPage::CollapsePanels(wxOrientation direction, int minimum_amount)
{
    while ( minimum_amount > 0 )
    {
        wxRibbonPanel* largestPanel = nullptr;
        wxSize* largestPanelSize = nullptr;
        wxSize* panelSize = m_sizeCalcArray;
        if ( !m_collapseStack.IsEmpty() )
        {
            // For a more consistent panel layout, try to collapse panels which
            // were recently expanded.
            largestPanel = wxDynamicCast(m_collapseStack.Last(), wxRibbonPanel);
            m_collapseStack.RemoveAt(m_collapseStack.GetCount() - 1);
            for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
                      node;
                      node = node->GetNext(), ++panelSize )
            {
                wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
                if ( panel == largestPanel && panel->IsShown() )
                {
                    largestPanelSize = panelSize;
                    break;
                }
            }
        }
        else
        {
            int largestSize = 0;
            for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
                      node;
                      node = node->GetNext(), ++panelSize )
            {
                wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
                if ( panel == nullptr || !panel->IsShown() )
                {
                    continue;
                }
                if ( panel->IsSizingContinuous() )
                {
                    int size = GetSizeInOrientation(*panelSize, direction);
                    if ( size > largestSize )
                    {
                        largestSize = size;
                        largestPanel = panel;
                        largestPanelSize = panelSize;
                    }
                }
                else
                {
                    int size = GetSizeInOrientation(*panelSize, direction);
                    if ( size > largestSize )
                    {
                        wxSize smaller = panel->GetNextSmallerSize(direction, *panelSize);
                        if ( smaller != (*panelSize) &&
                            GetSizeInOrientation(smaller, direction) < size )
                        {
                            largestSize = size;
                            largestPanel = panel;
                            largestPanelSize = panelSize;
                        }
                    }
                }
            }
        }
        if ( largestPanel != nullptr && largestPanelSize != nullptr )
        {
            if ( largestPanel->IsSizingContinuous() )
            {
                int amount = minimum_amount;
                if ( amount > 32 )
                {
                    // For "large" contraction, reduce this panel a bit, and
                    // then re-allocate the remainder of the quota (which may
                    // come to this panel again anyway)
                    amount = 32;
                }
                if ( direction & wxHORIZONTAL )
                {
                    largestPanelSize->x -= amount;
                }
                if ( direction & wxVERTICAL )
                {
                    largestPanelSize->y -= amount;
                }
                minimum_amount -= amount;
            }
            else
            {
                wxSize smaller = largestPanel->GetNextSmallerSize(direction, *largestPanelSize);
                wxSize delta = (*largestPanelSize) - smaller;
                *largestPanelSize = smaller;
                minimum_amount -= GetSizeInOrientation(delta, direction);
            }
        }
        else
        {
            break;
        }
    }
    return minimum_amount <= 0;
}

wxRibbonPanel* wxRibbonPage::GetPanel(int n)
{
    int currentPanelIndex = 0;
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
        if ( panel != nullptr )
        {
            if ( n == currentPanelIndex )
            {
                return panel;
            }
            ++currentPanelIndex;
        }
    }
    return nullptr;
}

wxRibbonPanel* wxRibbonPage::GetPanelById(wxWindowID id)
{
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
        if ( panel != nullptr && panel->GetId() == id )
        {
            return panel;
        }
    }
    return nullptr;
}

size_t wxRibbonPage::GetPanelCount() const
{
    size_t panelCount = 0;
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        if ( node->GetData()->IsKindOf(CLASSINFO(wxRibbonPanel)) )
        {
            ++panelCount;
        }
    }
    return panelCount;
}

bool wxRibbonPage::DismissExpandedPanel()
{
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
    {
        wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
        if ( panel == nullptr || !panel->IsShown() )
        {
            continue;
        }
        if ( panel->GetExpandedPanel() != nullptr )
        {
            return panel->HideExpanded();
        }
    }
    return false;
}

wxSize wxRibbonPage::GetMinSize() const
{
    wxSize min(wxDefaultCoord, wxDefaultCoord);

    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* child = node->GetData();
        wxSize childMin(child->GetMinSize());

        min.x = wxMax(min.x, childMin.x);
        min.y = wxMax(min.y, childMin.y);
    }

    if ( GetMajorAxis() == wxHORIZONTAL )
    {
        min.x = wxDefaultCoord;
        if ( min.y != wxDefaultCoord )
        {
            min.y += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
        }
    }
    else
    {
        if ( min.x != wxDefaultCoord )
        {
            min.x += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
        }
        min.y = wxDefaultCoord;
    }

    return min;
}

wxSize wxRibbonPage::DoGetBestSize() const
{
    wxSize best(0, 0);
    size_t count = 0;

    if ( GetMajorAxis() == wxHORIZONTAL )
    {
        best.y = wxDefaultCoord;

        for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
        {
            wxWindow* child = node->GetData();
            wxSize childBest(child->GetBestSize());

            if ( childBest.x != wxDefaultCoord )
            {
                best.IncBy(childBest.x, 0);
            }
            best.y = wxMax(best.y, childBest.y);

            ++count;
        }

        if ( count > 1 )
        {
            best.IncBy((count - 1) * m_art->GetMetric(wxRIBBON_ART_PANEL_X_SEPARATION_SIZE), 0);
        }
    }
    else
    {
        best.x = wxDefaultCoord;

        for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
        {
            wxWindow* child = node->GetData();
            wxSize childBest(child->GetBestSize());

            best.x = wxMax(best.x, childBest.x);
            if ( childBest.y != wxDefaultCoord )
            {
                best.IncBy(0, childBest.y);
            }

            ++count;
        }

        if ( count > 1 )
        {
            best.IncBy(0, (count - 1) * m_art->GetMetric(wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE));
        }
    }

    if ( best.x != wxDefaultCoord )
    {
        best.x += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
    }
    if ( best.y != wxDefaultCoord )
    {
        best.y += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
    }
    return best;
}

void wxRibbonPage::HideIfExpanded()
{
    if ( auto* const bar = wxCheckedStaticCast<wxRibbonBar>(GetParent()) )
        bar->HideIfExpanded();
}

#endif // wxUSE_RIBBON
