///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/panel.cpp
// Purpose:     Ribbon-style container for a group of related tools / controls
// Author:      Peter Cawley
// Created:     2009-05-25
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/panel.h"
#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/dcbuffer.h"
#include "wx/display.h"
#include "wx/sizer.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
#endif

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

wxDEFINE_EVENT(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, wxRibbonPanelEvent);

wxIMPLEMENT_DYNAMIC_CLASS(wxRibbonPanelEvent, wxCommandEvent);

wxIMPLEMENT_CLASS(wxRibbonPanel, wxRibbonControl);

wxBEGIN_EVENT_TABLE(wxRibbonPanel, wxRibbonControl)
    EVT_ENTER_WINDOW(wxRibbonPanel::OnMouseEnter)
    EVT_ERASE_BACKGROUND(wxRibbonPanel::OnEraseBackground)
    EVT_KILL_FOCUS(wxRibbonPanel::OnKillFocus)
    EVT_LEAVE_WINDOW(wxRibbonPanel::OnMouseLeave)
    EVT_MOTION(wxRibbonPanel::OnMotion)
    EVT_LEFT_DOWN(wxRibbonPanel::OnMouseClick)
    EVT_PAINT(wxRibbonPanel::OnPaint)
    EVT_SIZE(wxRibbonPanel::OnSize)
    EVT_DPI_CHANGED(wxRibbonPanel::OnDPIChanged)
wxEND_EVENT_TABLE()

wxRibbonPanel::wxRibbonPanel() : m_expandedDummy(nullptr), m_expandedPanel(nullptr)
{
}

wxRibbonPanel::wxRibbonPanel(wxWindow* parent,
                  wxWindowID id,
                  const wxString& label,
                  const wxBitmapBundle& minimisedIcon,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style)
    : wxRibbonControl(parent, id, pos, size, wxBORDER_NONE)
{
    CommonInit(label, minimisedIcon, style);
}

wxRibbonPanel::~wxRibbonPanel()
{
    if ( m_expandedPanel )
    {
        m_expandedPanel->m_expandedDummy = nullptr;
        m_expandedPanel->GetParent()->Destroy();
    }
}

bool wxRibbonPanel::Create(wxWindow* parent,
                wxWindowID id,
                const wxString& label,
                const wxBitmapBundle& icon,
                const wxPoint& pos,
                const wxSize& size,
                long style)
{
    if ( !wxRibbonControl::Create(parent, id, pos, size, wxBORDER_NONE) )
    {
        return false;
    }

    CommonInit(label, icon, style);

    return true;
}

void wxRibbonPanel::SetArtProvider(wxRibbonArtProvider* art)
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
    if ( m_expandedPanel )
        m_expandedPanel->SetArtProvider(art);
}

void wxRibbonPanel::CommonInit(const wxString& label, const wxBitmapBundle& icon, long style)
{
    SetName(label);
    SetLabel(label);

    m_minimisedSize = wxDefaultSize; // Unknown / none
    m_smallestUnminimisedSize = wxDefaultSize;// Unknown / none for IsFullySpecified()
    m_preferredExpandDirection = wxSOUTH;
    m_expandedDummy = nullptr;
    m_expandedPanel = nullptr;
    m_flags = style;
    m_minimisedIcon = icon;
    m_minimised = false;
    m_hovered = false;
    m_extButtonHovered = false;

    if ( m_art == nullptr )
    {
        wxRibbonControl* parent = wxDynamicCast(GetParent(), wxRibbonControl);
        if ( parent != nullptr )
        {
            m_art = parent->GetArtProvider();
        }
    }

    SetAutoLayout(true);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(20, 20));
}

bool wxRibbonPanel::IsMinimised() const
{
    return m_minimised;
}

bool wxRibbonPanel::IsHovered() const
{
    return m_hovered;
}

bool wxRibbonPanel::IsExtButtonHovered() const
{
    return m_extButtonHovered;
}

void wxRibbonPanel::OnMouseEnter(wxMouseEvent& evt)
{
    TestPositionForHover(evt.GetPosition());
}

void wxRibbonPanel::OnMouseEnterChild(wxMouseEvent& evt)
{
    wxPoint pos = evt.GetPosition();
    wxWindow *child = wxDynamicCast(evt.GetEventObject(), wxWindow);
    if ( child )
    {
        pos += child->GetPosition();
        TestPositionForHover(pos);
    }
    evt.Skip();
}

void wxRibbonPanel::OnMouseLeave(wxMouseEvent& evt)
{
    TestPositionForHover(evt.GetPosition());
}

void wxRibbonPanel::OnMouseLeaveChild(wxMouseEvent& evt)
{
    wxPoint pos = evt.GetPosition();
    wxWindow *child = wxDynamicCast(evt.GetEventObject(), wxWindow);
    if ( child )
    {
        pos += child->GetPosition();
        TestPositionForHover(pos);
    }
    evt.Skip();
}

void wxRibbonPanel::OnMotion(wxMouseEvent& evt)
{
    TestPositionForHover(evt.GetPosition());
}

void wxRibbonPanel::TestPositionForHover(const wxPoint& pos)
{
    bool hovered = false, extButtonHovered = false;
    if ( pos.x >= 0 && pos.y >= 0 )
    {
        wxSize size = GetSize();
        if ( pos.x < size.GetWidth() && pos.y < size.GetHeight() )
        {
            hovered = true;
        }
    }
    if ( hovered )
    {
        if ( HasExtButton() )
            extButtonHovered = m_extButtonRect.Contains(pos);
        else
            extButtonHovered = false;
    }
    if ( hovered != m_hovered || extButtonHovered != m_extButtonHovered )
    {
        m_hovered = hovered;
        m_extButtonHovered = extButtonHovered;
        Refresh(false);
    }
}

void wxRibbonPanel::AddChild(wxWindowBase *child)
{
    wxRibbonControl::AddChild(child);

    // Window enter / leave events count for only the window in question, not
    // for children of the window. The panel wants to be in the hovered state
    // whenever the mouse cursor is within its boundary, so the events need to
    // be attached to children too.
    child->Bind(wxEVT_ENTER_WINDOW, &wxRibbonPanel::OnMouseEnterChild, this);
    child->Bind(wxEVT_LEAVE_WINDOW, &wxRibbonPanel::OnMouseLeaveChild, this);
}

void wxRibbonPanel::RemoveChild(wxWindowBase *child)
{
    child->Unbind(wxEVT_ENTER_WINDOW, &wxRibbonPanel::OnMouseEnterChild, this);
    child->Unbind(wxEVT_LEAVE_WINDOW, &wxRibbonPanel::OnMouseLeaveChild, this);

    wxRibbonControl::RemoveChild(child);
}

bool wxRibbonPanel::HasExtButton() const
{
    wxRibbonBar* bar = GetAncestorRibbonBar();
    if ( bar == nullptr )
        return false;
    return (m_flags & wxRIBBON_PANEL_EXT_BUTTON) &&
        (bar->GetWindowStyleFlag() & wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS);
}

void wxRibbonPanel::OnSize(wxSizeEvent& evt)
{
    if ( GetAutoLayout() )
        Layout();

    evt.Skip();
}

void wxRibbonPanel::OnDPIChanged(wxDPIChangedEvent& event)
{
    Realize();
    event.Skip();
}

void wxRibbonPanel::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // At least on MSW, changing the size of a window will cause GetSize() to
    // report the new size, but a size event may not be handled immediately.
    // If this minimised check was performed in the OnSize handler, then
    // GetSize() could return a size much larger than the minimised size while
    // IsMinimised() returns true. This would then affect layout, as the panel
    // will refuse to grow any larger while in limbo between minimised and non.

    bool minimised = (m_flags & wxRIBBON_PANEL_NO_AUTO_MINIMISE) == 0 &&
        IsMinimised(wxSize(width, height)); // check if would be at this size
    if ( minimised != m_minimised )
    {
        m_minimised = minimised;
        // Note that for sizers, this routine disallows the use of mixed shown
        // and hidden controls
        // TODO ? use some list of user set invisible children to restore status.
        for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
                  node;
                  node = node->GetNext() )
        {
            node->GetData()->Show(!minimised);
        }

        Refresh();
    }

    wxRibbonControl::DoSetSize(x, y, width, height, sizeFlags);
}

// Checks if panel would be minimised at (client size) atSize
bool wxRibbonPanel::IsMinimised(wxSize atSize) const
{
    if ( GetSizer() )
    {
        // we have no information on size change direction
        // so check both
        wxSize size = GetMinNotMinimisedSize();
        if ( size.x > atSize.x || size.y > atSize.y )
            return true;

        return false;
    }

    if ( !m_minimisedSize.IsFullySpecified() )
        return false;

    return (atSize.GetX() < m_minimisedSize.GetX() &&
        atSize.GetY() < m_minimisedSize.GetY()) ||
        atSize.GetX() < m_smallestUnminimisedSize.GetX() ||
        atSize.GetY() < m_smallestUnminimisedSize.GetY();
}

void wxRibbonPanel::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // All painting done in main paint handler to minimise flicker
}

void wxRibbonPanel::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);

    if ( m_art != nullptr )
    {
        if ( IsMinimised() )
        {
            m_art->DrawMinimisedPanel(dc, this, GetSize(), m_minimisedIconResized);
        }
        else
        {
            m_art->DrawPanelBackground(dc, this, GetSize());
        }
    }
}

bool wxRibbonPanel::IsSizingContinuous() const
{
    // A panel never sizes continuously, even if all of its children can,
    // as it would appear out of place along side non-continuous panels.

    // JS 2012-03-09: introducing wxRIBBON_PANEL_STRETCH to allow
    // the panel to fill its parent page. For example we might have
    // a list of styles in one of the pages, which should stretch to
    // fill available space.
    return (m_flags & wxRIBBON_PANEL_STRETCH) != 0;
}

// Finds the best width and height given the parent's width and height
wxSize wxRibbonPanel::GetBestSizeForParentSize(const wxSize& parentSize) const
{
    if ( !IsShown() )
    {
        return wxSize(0, 0);
    }
    if ( GetChildren().GetCount() == 1 )
    {
        wxWindow* win = GetChildren().GetFirst()->GetData();
        wxRibbonControl* control = wxDynamicCast(win, wxRibbonControl);
        if ( control )
        {
            wxInfoDC tempDc(const_cast<wxRibbonPanel*>(this));
            wxSize clientParentSize = m_art->GetPanelClientSize(tempDc, this, parentSize, nullptr);
            wxSize childSize = control->GetBestSizeForParentSize(clientParentSize);
            wxSize overallSize = m_art->GetPanelSize(tempDc, this, childSize, nullptr);
            return overallSize;
        }
    }
    return GetSize();
}

wxSize wxRibbonPanel::DoGetNextSmallerSize(wxOrientation direction,
                                         wxSize relativeTo) const
{
    if ( m_expandedPanel != nullptr )
    {
        // Next size depends upon children, who are currently in the
        // expanded panel
        return m_expandedPanel->DoGetNextSmallerSize(direction, relativeTo);
    }

    if ( m_art != nullptr )
    {
        wxInfoDC dc(const_cast<wxRibbonPanel*>(this));
        wxSize childRelative = m_art->GetPanelClientSize(dc, this, relativeTo, nullptr);
        wxSize smaller(-1, -1);
        bool minimise = false;

        if ( GetSizer() )
        {
            // Get smallest non minimised size
            smaller = GetMinSize();
            // and adjust to childRelative for parent page
            if ( m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
            {
                 minimise = (childRelative.y <= smaller.y);
                 if ( smaller.x < childRelative.x )
                    smaller.x = childRelative.x;
            }
            else
            {
                minimise = (childRelative.x <= smaller.x);
                if ( smaller.y < childRelative.y )
                    smaller.y = childRelative.y;
            }
        }
        else if ( GetChildren().GetCount() == 1 )
        {
            // Simple (and common) case of single ribbon child or Sizer
            wxWindow* child = GetChildren().Item(0)->GetData();
            wxRibbonControl* ribbonChild = wxDynamicCast(child, wxRibbonControl);
            if ( ribbonChild != nullptr )
            {
                smaller = ribbonChild->GetNextSmallerSize(direction, childRelative);
                minimise = (smaller == childRelative);
            }
        }

        if ( minimise )
        {
            if ( CanAutoMinimise() )
            {
                wxSize minimised = m_minimisedSize;
                switch ( direction )
                {
                case wxHORIZONTAL:
                    minimised.SetHeight(relativeTo.GetHeight());
                    break;
                case wxVERTICAL:
                    minimised.SetWidth(relativeTo.GetWidth());
                    break;
                default:
                    break;
                }
                return minimised;
            }
            else
            {
                return relativeTo;
            }
        }
        else if ( smaller.IsFullySpecified() ) // Use fallback if !(sizer/child = 1 )
        {
            return m_art->GetPanelSize(dc, this, smaller, nullptr);
        }
    }

    // Fallback: Decrease by 20% (or minimum size, whichever larger)
    wxSize current(relativeTo);
    wxSize minimum(GetMinSize());
    if ( direction & wxHORIZONTAL )
    {
        current.x = (current.x * 4) / 5;
        if ( current.x < minimum.x )
        {
            current.x = minimum.x;
        }
    }
    if ( direction & wxVERTICAL )
    {
        current.y = (current.y * 4) / 5;
        if ( current.y < minimum.y )
        {
            current.y = minimum.y;
        }
    }
    return current;
}

wxSize wxRibbonPanel::DoGetNextLargerSize(wxOrientation direction,
                                        wxSize relativeTo) const
{
    if ( m_expandedPanel != nullptr )
    {
        // Next size depends upon children, who are currently in the
        // expanded panel
        return m_expandedPanel->DoGetNextLargerSize(direction, relativeTo);
    }

    if ( IsMinimised(relativeTo) )
    {
        wxSize current = relativeTo;
        wxSize minSize = GetMinNotMinimisedSize();
        switch ( direction )
        {
        case wxHORIZONTAL:
            if ( minSize.x > current.x && minSize.y == current.y )
                return minSize;
            break;
        case wxVERTICAL:
            if ( minSize.x == current.x && minSize.y > current.y )
                return minSize;
            break;
        case wxBOTH:
            if ( minSize.x > current.x && minSize.y > current.y )
                return minSize;
            break;
        default:
            break;
        }
    }

    if ( m_art != nullptr )
    {
        wxInfoDC dc(const_cast<wxRibbonPanel*>(this));
        wxSize childRelative = m_art->GetPanelClientSize(dc, this, relativeTo, nullptr);
        wxSize larger(-1, -1);

        if ( GetSizer() )
        {
            // We could just let the sizer expand in flow direction but see comment
            // in IsSizingContinuous()
            larger = GetPanelSizerBestSize();
            // and adjust for page in non flow direction
            if ( m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
            {
                 if ( larger.x != childRelative.x )
                    larger.x = childRelative.x;
            }
            else if ( larger.y != childRelative.y )
            {
                larger.y = childRelative.y;
            }
        }
        else if ( GetChildren().GetCount() == 1 )
        {
            // Simple (and common) case of single ribbon child
            wxWindow* child = GetChildren().Item(0)->GetData();
            wxRibbonControl* ribbonChild = wxDynamicCast(child, wxRibbonControl);
            if ( ribbonChild != nullptr )
            {
                larger = ribbonChild->GetNextLargerSize(direction, childRelative);
            }
        }

        if ( larger.IsFullySpecified() ) // Use fallback if !(sizer/child = 1 )
        {
            if ( larger == childRelative )
            {
                return relativeTo;
            }
            else
            {
                return m_art->GetPanelSize(dc, this, larger, nullptr);
            }
        }
    }

    // Fallback: Increase by 25% (equal to a prior or subsequent 20% decrease)
    // Note that due to rounding errors, this increase may not exactly equal a
    // matching decrease - an ideal solution would not have these errors, but
    // avoiding them is non-trivial unless an increase is by 100% rather than
    // a fractional amount. This would then be non-ideal as the resizes happen
    // at very large intervals.
    wxSize current(relativeTo);
    if ( direction & wxHORIZONTAL )
    {
        current.x = (current.x * 5 + 3) / 4;
    }
    if ( direction & wxVERTICAL )
    {
        current.y = (current.y * 5 + 3) / 4;
    }
    return current;
}

bool wxRibbonPanel::CanAutoMinimise() const
{
    return (m_flags & wxRIBBON_PANEL_NO_AUTO_MINIMISE) == 0
        && m_minimisedSize.IsFullySpecified();
}

wxSize wxRibbonPanel::GetMinSize() const
{
    if ( !IsShown() )
    {
        return wxSize(0, 0);
    }
    if ( m_expandedPanel != nullptr )
    {
        // Minimum size depends upon children, who are currently in the
        // expanded panel
        return m_expandedPanel->GetMinSize();
    }

    if ( CanAutoMinimise() )
    {
        return m_minimisedSize;
    }
    else
    {
        return GetMinNotMinimisedSize();
    }
}

wxSize wxRibbonPanel::GetMinNotMinimisedSize() const
{
    if ( !IsShown() )
    {
        return wxSize(0, 0);
    }
    // Ask sizer if present
    if ( GetSizer() )
    {
        wxInfoDC dc(const_cast<wxRibbonPanel*>(this));
        return m_art->GetPanelSize(dc, this, GetPanelSizerMinSize(), nullptr);
    }
    else if ( GetChildren().GetCount() == 1 )
    {
        // Common case of single child taking up the entire panel
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxInfoDC dc(const_cast<wxRibbonPanel*>(this));
        return m_art->GetPanelSize(dc, this, child->GetMinSize(), nullptr);
    }

    return wxRibbonControl::GetMinSize();
}

wxSize wxRibbonPanel::GetPanelSizerMinSize() const
{
    // Called from Realize() to set m_smallestUnminimisedSize and from other
    // functions to get the minimum size.
    // The panel will be invisible when minimised and sizer calcs will be 0
    // Uses m_smallestUnminimisedSize in preference to GetSizer()->CalcMin()
    // to eliminate flicker.

    // Check if is visible and not previously calculated
    if ( IsShown() && !m_smallestUnminimisedSize.IsFullySpecified() )
    {
         return GetSizer()->CalcMin();
    }
    // else use previously calculated m_smallestUnminimisedSize
    wxInfoDC dc(const_cast<wxRibbonPanel*>(this));
    return m_art->GetPanelClientSize(dc,
                                    this,
                                    m_smallestUnminimisedSize,
                                    nullptr);
}

wxSize wxRibbonPanel::GetPanelSizerBestSize() const
{
    wxSize size = GetPanelSizerMinSize();
    // TODO allow panel to increase its size beyond minimum size
    // by steps similarly to ribbon control panels (preferred for aesthetics)
    // or continuously.
    return size;
}

wxSize wxRibbonPanel::DoGetBestSize() const
{
    // Ask sizer if present
    if ( GetSizer() )
    {
        wxInfoDC dc(const_cast<wxRibbonPanel*>(this));
        return m_art->GetPanelSize(dc, this, GetPanelSizerBestSize(), nullptr);
    }
    else if ( GetChildren().GetCount() == 1 )
    {
        // Common case of no sizer and single child taking up the entire panel
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxInfoDC dc(const_cast<wxRibbonPanel*>(this));
        return m_art->GetPanelSize(dc, this, child->GetBestSize(), nullptr);
    }

    return wxRibbonControl::DoGetBestSize();
}

bool wxRibbonPanel::Realize()
{
    bool status = true;

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

    wxSize minimumChildrenSize(0, 0);

    // Reset it before calling GetPanelSizerMinSize() below as it shouldn't use
    // the old value, if we had any.
    m_smallestUnminimisedSize = wxDefaultSize;

    // Ask sizer if there is one present
    if ( GetSizer() )
    {
        minimumChildrenSize = GetPanelSizerMinSize();
    }
    else if ( GetChildren().GetCount() == 1 )
    {
        minimumChildrenSize = GetChildren().GetFirst()->GetData()->GetMinSize();
    }

    if ( m_art != nullptr )
    {
        wxInfoDC tempDc(this);

        m_smallestUnminimisedSize =
            m_art->GetPanelSize(tempDc, this, minimumChildrenSize, nullptr);

        wxSize panelMinSize = GetMinNotMinimisedSize();
        m_minimisedSize = m_art->GetMinimisedPanelMinimumSize(tempDc, this,
            nullptr, &m_preferredExpandDirection);
        if ( m_minimisedIcon.IsOk() )
        {
            m_minimisedIconResized = m_minimisedIcon.GetBitmap(
                m_minimisedIcon.GetPreferredBitmapSizeFor(this));
        }
        if ( m_minimisedSize.x > panelMinSize.x &&
            m_minimisedSize.y > panelMinSize.y )
        {
            // No point in having a minimised size which is larger than the
            // minimum size which the children can go to.
            m_minimisedSize = wxSize(-1, -1);
        }
        else
        {
            if ( m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL )
            {
                m_minimisedSize.x = panelMinSize.x;
            }
            else
            {
                m_minimisedSize.y = panelMinSize.y;
            }
        }
    }
    else
    {
        m_minimisedSize = wxSize(-1, -1);
    }

    return Layout() && status;
}

bool wxRibbonPanel::Layout()
{
    if ( IsMinimised() )
    {
        // Children are all invisible when minimised
        return true;
    }

    // Get wxRibbonPanel client size
    wxPoint position;
    wxInfoDC dc(this);
    wxSize size = m_art->GetPanelClientSize(dc, this, GetSize(), &position);

    // If there is a sizer, use it
    if ( GetSizer() )
    {
        GetSizer()->SetDimension(position, size); // SetSize and Layout()
    }
    else if ( GetChildren().GetCount() == 1 )
    {
        // Common case of no sizer and single child taking up the entire panel
        wxWindow* child = GetChildren().Item(0)->GetData();
        child->SetSize(position.x, position.y, size.GetWidth(), size.GetHeight());
    }

    if ( HasExtButton() )
        m_extButtonRect = m_art->GetPanelExtButtonArea(dc, this, GetSize());

    return true;
}

void wxRibbonPanel::OnMouseClick(wxMouseEvent& WXUNUSED(evt))
{
    if ( IsMinimised() )
    {
        if ( m_expandedPanel != nullptr )
        {
            HideExpanded();
        }
        else
        {
            ShowExpanded();
        }
    }
    else if ( IsExtButtonHovered() )
    {
        wxRibbonPanelEvent notification(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, GetId());
        notification.SetEventObject(this);
        notification.SetPanel(this);
        ProcessEvent(notification);
    }
}

wxRibbonPanel* wxRibbonPanel::GetExpandedDummy()
{
    return m_expandedDummy;
}

wxRibbonPanel* wxRibbonPanel::GetExpandedPanel()
{
    return m_expandedPanel;
}

bool wxRibbonPanel::ShowExpanded()
{
    if ( !IsMinimised() )
    {
        return false;
    }
    if ( m_expandedDummy != nullptr || m_expandedPanel != nullptr )
    {
        return false;
    }

    wxSize size = GetBestSize();

    // Special case for flexible panel layout, where GetBestSize doesn't work
    if ( GetFlags() & wxRIBBON_PANEL_FLEXIBLE )
    {
        size = GetBestSizeForParentSize(wxSize(400, 1000));
    }

    wxPoint pos = GetExpandedPosition(wxRect(GetScreenPosition(), GetSize()),
        size, m_preferredExpandDirection).GetTopLeft();

    // Need a top-level frame to contain the expanded panel
    wxFrame *container = new wxFrame(nullptr, wxID_ANY, GetLabel(),
        pos, size, wxFRAME_NO_TASKBAR | wxBORDER_NONE);

    m_expandedPanel = new wxRibbonPanel(container, wxID_ANY,
        GetLabel(), m_minimisedIcon, wxPoint(0, 0), size, (m_flags /* & ~wxRIBBON_PANEL_FLEXIBLE */));

    m_expandedPanel->SetArtProvider(m_art);
    m_expandedPanel->m_expandedDummy = this;

    // Move all children to the new panel.
    // Conceptually it might be simpler to reparent this entire panel to the
    // container and create a new panel to sit in its place while expanded.
    // This approach has a problem though - when the panel is reinserted into
    // its original parent, it'll be at a different position in the child list
    // and thus assume a new position.
    // NB: Children iterators not used as behaviour is not well defined
    // when iterating over a container which is being emptied
    while ( !GetChildren().IsEmpty() )
    {
        wxWindow *child = GetChildren().GetFirst()->GetData();
        child->Reparent(m_expandedPanel);
        child->Show();
    }

    // Move sizer to new panel
    if ( GetSizer() )
    {
        wxSizer* sizer = GetSizer();
        SetSizer(nullptr, false);
        m_expandedPanel->SetSizer(sizer);
    }

    m_expandedPanel->Realize();
    Refresh();
    container->SetMinClientSize(size);
    container->Show();
    m_expandedPanel->SetFocus();

    return true;
}

bool wxRibbonPanel::ShouldSendEventToDummy(wxEvent& evt)
{
    // For an expanded panel, filter events between being sent up to the
    // floating top level window or to the dummy panel sitting in the ribbon
    // bar.

    // Child focus events should not be redirected, as the child would not be a
    // child of the window the event is redirected to. All other command events
    // seem to be suitable for redirecting.
    return evt.IsCommandEvent() && evt.GetEventType() != wxEVT_CHILD_FOCUS;
}

bool wxRibbonPanel::TryAfter(wxEvent& evt)
{
    if ( m_expandedDummy && ShouldSendEventToDummy(evt) )
    {
        wxPropagateOnce propagateOnce(evt);
        return m_expandedDummy->GetEventHandler()->ProcessEvent(evt);
    }
    else
    {
        return wxRibbonControl::TryAfter(evt);
    }
}

static bool IsAncestorOf(wxWindow *ancestor, wxWindow *window)
{
    while ( window != nullptr )
    {
        wxWindow *parent = window->GetParent();
        if ( parent == ancestor )
            return true;
        else
            window = parent;
    }
    return false;
}

void wxRibbonPanel::OnKillFocus(wxFocusEvent& evt)
{
    if ( m_expandedDummy )
    {
        wxWindow *receiver = evt.GetWindow();
        if ( IsAncestorOf(this, receiver) )
        {
            m_childWithFocus = receiver;
            receiver->Bind(wxEVT_KILL_FOCUS,
                &wxRibbonPanel::OnChildKillFocus, this);
        }
        else if ( receiver == nullptr || receiver != m_expandedDummy )
        {
            HideExpanded();
        }
    }
}

void wxRibbonPanel::OnChildKillFocus(wxFocusEvent& evt)
{
    if ( m_childWithFocus == nullptr )
        return; // Should never happen, but a check can't hurt

    m_childWithFocus->Unbind(wxEVT_KILL_FOCUS,
      &wxRibbonPanel::OnChildKillFocus, this);
    m_childWithFocus = nullptr;

    wxWindow *receiver = evt.GetWindow();
    if ( receiver == this || IsAncestorOf(this, receiver) )
    {
        m_childWithFocus = receiver;
        receiver->Bind(wxEVT_KILL_FOCUS,
            &wxRibbonPanel::OnChildKillFocus, this);
        evt.Skip();
    }
    else if ( receiver == nullptr || receiver != m_expandedDummy )
    {
        HideExpanded();
        // Do not skip event, as the panel has been de-expanded, causing the
        // child with focus to be reparented (and hidden). If the event
        // continues propagation then bad things happen.
    }
    else
    {
        evt.Skip();
    }
}

bool wxRibbonPanel::HideExpanded()
{
    if ( m_expandedDummy == nullptr )
    {
        if ( m_expandedPanel )
        {
            return m_expandedPanel->HideExpanded();
        }
        else
        {
            return false;
        }
    }

    // Move children back to original panel
    // NB: Children iterators not used as behaviour is not well defined
    // when iterating over a container which is being emptied
    while ( !GetChildren().IsEmpty() )
    {
        wxWindow *child = GetChildren().GetFirst()->GetData();
        child->Reparent(m_expandedDummy);
        child->Hide();
    }

    // Move sizer back
    if ( GetSizer() )
    {
        wxSizer* sizer = GetSizer();
        SetSizer(nullptr, false);
        m_expandedDummy->SetSizer(sizer);
    }

    m_expandedDummy->m_expandedPanel = nullptr;
    m_expandedDummy->Realize();
    m_expandedDummy->Refresh();
    wxWindow *parent = GetParent();
    Destroy();
    parent->Destroy();

    return true;
}

wxRect wxRibbonPanel::GetExpandedPosition(wxRect panel,
                                          wxSize expandedSize,
                                          wxDirection direction)
{
    // Strategy:
    // 1) Determine primary position based on requested direction
    // 2) Move the position so that it sits entirely within a display
    //    (for single monitor systems, this moves it into the display region,
    //     but for multiple monitors, it does so without splitting it over
    //     more than one display)
    // 2.1) Move in the primary axis
    // 2.2) Move in the secondary axis

    wxPoint pos;
    bool primaryX = false;
    int secondaryX = 0;
    int secondaryY = 0;
    switch ( direction )
    {
    case wxNORTH:
        pos.x = panel.GetX() + (panel.GetWidth() - expandedSize.GetWidth()) / 2;
        pos.y = panel.GetY() - expandedSize.GetHeight();
        primaryX = true;
        secondaryY = 1;
        break;
    case wxEAST:
        pos.x = panel.GetRight();
        pos.y = panel.GetY() + (panel.GetHeight() - expandedSize.GetHeight()) / 2;
        secondaryX = -1;
        break;
    case wxSOUTH:
        pos.x = panel.GetX() + (panel.GetWidth() - expandedSize.GetWidth()) / 2;
        pos.y = panel.GetBottom();
        primaryX = true;
        secondaryY = -1;
        break;
    case wxWEST:
    default:
        pos.x = panel.GetX() - expandedSize.GetWidth();
        pos.y = panel.GetY() + (panel.GetHeight() - expandedSize.GetHeight()) / 2;
        secondaryX = 1;
        break;
    }
    wxRect expanded(pos, expandedSize);

    wxRect best(expanded);
    int bestDistance = INT_MAX;

    const unsigned displayN = wxDisplay::GetCount();
    for ( unsigned i = 0; i < displayN; ++i )
    {
        wxRect display = wxDisplay(i).GetGeometry();

        if ( display.Contains(expanded) )
        {
            return expanded;
        }
        else if ( display.Intersects(expanded) )
        {
            wxRect newRect(expanded);
            int distance = 0;

            if ( primaryX )
            {
                if ( expanded.GetRight() > display.GetRight() )
                {
                    distance = expanded.GetRight() - display.GetRight();
                    newRect.x -= distance;
                }
                else if ( expanded.GetLeft() < display.GetLeft() )
                {
                    distance = display.GetLeft() - expanded.GetLeft();
                    newRect.x += distance;
                }
            }
            else
            {
                if ( expanded.GetBottom() > display.GetBottom() )
                {
                    distance = expanded.GetBottom() - display.GetBottom();
                    newRect.y -= distance;
                }
                else if ( expanded.GetTop() < display.GetTop() )
                {
                    distance = display.GetTop() - expanded.GetTop();
                    newRect.y += distance;
                }
            }
            if ( !display.Contains(newRect) )
            {
                // Tried moving in primary axis, but failed.
                // Hence try moving in the secondary axis.
                int dx = secondaryX * (panel.GetWidth() + expandedSize.GetWidth());
                int dy = secondaryY * (panel.GetHeight() + expandedSize.GetHeight());
                newRect.x += dx;
                newRect.y += dy;

                // Squaring makes secondary moves more expensive (and also
                // prevents a negative cost)
                distance += dx * dx + dy * dy;
            }
            if ( display.Contains(newRect) && distance < bestDistance )
            {
                best = newRect;
                bestDistance = distance;
            }
        }
    }

    return best;
}

void wxRibbonPanel::HideIfExpanded()
{
    wxRibbonPage* const containingPage = wxDynamicCast(m_parent, wxRibbonPage);
    if ( containingPage )
        containingPage->HideIfExpanded();
}

#endif // wxUSE_RIBBON
