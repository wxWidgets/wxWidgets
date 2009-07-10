///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/panel.cpp
// Purpose:     Ribbon-style container for a group of related tools / controls
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/panel.h"
#include "wx/dcbuffer.h"

#if wxUSE_RIBBON

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonPanel, wxRibbonControl)

BEGIN_EVENT_TABLE(wxRibbonPanel, wxRibbonControl)
    EVT_ENTER_WINDOW(wxRibbonPanel::OnMouseEnter)
    EVT_ERASE_BACKGROUND(wxRibbonPanel::OnEraseBackground)
    EVT_LEAVE_WINDOW(wxRibbonPanel::OnMouseLeave)
    EVT_PAINT(wxRibbonPanel::OnPaint)
    EVT_SIZE(wxRibbonPanel::OnSize)
END_EVENT_TABLE()

wxRibbonPanel::wxRibbonPanel()
{
}

wxRibbonPanel::wxRibbonPanel(wxWindow* parent,
                  wxWindowID id,
                  const wxString& label,
                  const wxBitmap& minimised_icon,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style)
    : wxRibbonControl(parent, id, pos, size, style)
{
    CommonInit(label, minimised_icon, style);
}

wxRibbonPanel::~wxRibbonPanel()
{
}

bool wxRibbonPanel::Create(wxWindow* parent,
                wxWindowID id,
                const wxString& label,
                const wxBitmap& icon,
                const wxPoint& pos,
                const wxSize& size,
                long style)
{
    if(!wxRibbonControl::Create(parent, id, pos, size, style))
    {
        return false;
    }

    CommonInit(label, icon, style);

    return true;
}

void wxRibbonPanel::SetArtProvider(wxRibbonArtProvider* art)
{
    m_art = art;
}

void wxRibbonPanel::CommonInit(const wxString& label, const wxBitmap& icon, long style)
{
    SetName(label);
    SetLabel(label);

    m_minimised_size = wxDefaultSize; // Unknown / none
    m_flags = style;
    m_minimised_icon = icon;
    m_minimised = false;
    m_hovered_count = false;

    if(m_art == NULL)
    {
        wxRibbonControl* parent = wxDynamicCast(GetParent(), wxRibbonControl);
        if(parent != NULL)
        {
            m_art = parent->GetArtProvider();
        }
    }

    SetAutoLayout(true);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetMinSize(wxSize(20, 20));
}

bool wxRibbonPanel::IsMinimised() const
{
    return m_minimised;
}

bool wxRibbonPanel::IsHovered() const
{
    return m_hovered_count > 0;
}

void wxRibbonPanel::OnMouseEnter(wxMouseEvent& WXUNUSED(evt))
{
    ++m_hovered_count;
    Refresh();
}

void wxRibbonPanel::OnMouseLeave(wxMouseEvent& WXUNUSED(evt))
{
    --m_hovered_count;
    Refresh();
}

void wxRibbonPanel::AddChild(wxWindowBase *child)
{
    wxRibbonControl::AddChild(child);

    // Window enter / leave events count for only the window in question, not
    // for children of the window. The panel wants to be in the hovered state
    // whenever the mouse cursor is within its boundary, so the events need to
    // be attached to children too.
    child->Connect(wxEVT_ENTER_WINDOW, (wxObjectEventFunction)&wxRibbonPanel::OnMouseEnter, NULL, this);
    child->Connect(wxEVT_LEAVE_WINDOW, (wxObjectEventFunction)&wxRibbonPanel::OnMouseLeave, NULL, this);
}

void wxRibbonPanel::RemoveChild(wxWindowBase *child)
{
    child->Disconnect(wxEVT_ENTER_WINDOW, (wxObjectEventFunction)&wxRibbonPanel::OnMouseEnter, NULL, this);
    child->Disconnect(wxEVT_LEAVE_WINDOW, (wxObjectEventFunction)&wxRibbonPanel::OnMouseLeave, NULL, this);

    wxRibbonControl::RemoveChild(child);
}

void wxRibbonPanel::OnSize(wxSizeEvent& evt)
{
    if(GetAutoLayout())
        Layout();

    evt.Skip();
}

void wxRibbonPanel::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // At least on MSW, changing the size of a window will cause GetSize() to
    // report the new size, but a size event may not be handled immediately.
    // If this minimised check was performed in the OnSize handler, then
    // GetSize() could return a size much larger than the minimised size while
    // IsMinimised() returns true. This would then affect layout, as the panel
    // will refuse to grow any larger while in limbo between minimised and non.

    bool minimised = width <= m_minimised_size.GetX() && height <= m_minimised_size.GetY();
    if(minimised != m_minimised)
    {
        m_minimised = minimised;

        for (wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
                  node;
                  node = node->GetNext())
        {
            node->GetData()->Show(!minimised);
        }

        Refresh();
    }
    
    wxRibbonControl::DoSetSize(x, y, width, height, sizeFlags);
}

bool wxRibbonPanel::IsMinimised(wxSize at_size) const
{
    return at_size.GetX() <= m_minimised_size.GetX() &&
        at_size.GetY() <= m_minimised_size.GetY();
}

void wxRibbonPanel::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // All painting done in main paint handler to minimise flicker
}

void wxRibbonPanel::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxAutoBufferedPaintDC dc(this);

    if(m_art != NULL)
    {
        if(IsMinimised())
        {
            m_art->DrawMinimisedPanel(dc, this, GetSize(), m_minimised_icon_resized);
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
    return false;
}

wxSize wxRibbonPanel::DoGetNextSmallerSize(wxOrientation direction,
                                         wxSize relative_to) const
{
    // TODO: Check for, and delegate to, a sizer

    // Simple (and common) case of single ribbon child
    if(GetChildren().GetCount() == 1)
    {
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxRibbonControl* ribbon_child = wxDynamicCast(child, wxRibbonControl);
        if(m_art != NULL && ribbon_child != NULL)
        {
            wxMemoryDC dc;
            wxSize child_relative = m_art->GetPanelClientSize(dc, this, relative_to, NULL);
            wxSize smaller = ribbon_child->GetNextSmallerSize(direction, child_relative);
            if(smaller == child_relative)
            {
                if(CanAutoMinimise())
                {
                    return m_minimised_size;
                }
                else
                {
                    return relative_to;
                }
            }
            else
            {
                return m_art->GetPanelSize(dc, this, smaller, NULL);
            }
        }
    }

    // Fallback: Decrease by 20% (or minimum size, whichever larger)
    wxSize current(relative_to);
    wxSize minimum(GetMinSize());
    if(direction & wxHORIZONTAL)
    {
        current.x = (current.x * 4) / 5;
        if(current.x < minimum.x)
        {
            current.x = minimum.x;
        }
    }
    if(direction & wxVERTICAL)
    {
        current.y = (current.y * 4) / 5;
        if(current.y < minimum.y)
        {
            current.y = minimum.y;
        }
    }
    return current;
}

wxSize wxRibbonPanel::DoGetNextLargerSize(wxOrientation direction,
                                        wxSize relative_to) const
{
    if(IsMinimised(relative_to))
    {
        wxSize current = relative_to;
        wxSize min_size = GetMinNotMinimisedSize();
        switch(direction)
        {
        case wxHORIZONTAL:
            if(min_size.x > current.x && min_size.y == current.y)
                return min_size;
            break;
        case wxVERTICAL:
            if(min_size.x == current.x && min_size.y > current.y)
                return min_size;
            break;
        case wxBOTH:
            if(min_size.x > current.x && min_size.y > current.y)
                return min_size;
            break;
        default:
            break;
        }
    }

    // TODO: Check for, and delegate to, a sizer

    // Simple (and common) case of single ribbon child
    if(GetChildren().GetCount() == 1)
    {
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxRibbonControl* ribbon_child = wxDynamicCast(child, wxRibbonControl);
        if(ribbon_child != NULL)
        {
            wxMemoryDC dc;
            wxSize child_relative = m_art->GetPanelClientSize(dc, this, relative_to, NULL);
            wxSize larger = ribbon_child->GetNextLargerSize(direction, child_relative);
            if(larger == child_relative)
            {
                return relative_to;
            }
            else
            {
                wxMemoryDC dc;
                return m_art->GetPanelSize(dc, this, larger, NULL);
            }
        }
    }

    // Fallback: Increase by 25% (equal to a prior or subsequent 20% decrease)
    // Note that due to rounding errors, this increase may not exactly equal a
    // matching decrease - an ideal solution would not have these errors, but
    // avoiding them is non-trivial unless an increase is by 100% rather than
    // a fractional amount. This would then be non-ideal as the resizes happen
    // at very large intervals.
    wxSize current(relative_to);
    if(direction & wxHORIZONTAL)
    {
        current.x = (current.x * 5 + 3) / 4;
    }
    if(direction & wxVERTICAL)
    {
        current.y = (current.y * 5 + 3) / 4;
    }
    return current;
}

bool wxRibbonPanel::CanAutoMinimise() const
{
    return (m_flags & wxRIBBON_PANEL_NO_AUTO_MINIMISE) == 0
        && m_minimised_size.IsFullySpecified();
}

wxSize wxRibbonPanel::GetMinSize() const
{
    if(CanAutoMinimise())
    {
        return m_minimised_size;
    }
    else
    {
        return GetMinNotMinimisedSize();
    }
}

wxSize wxRibbonPanel::GetMinNotMinimisedSize() const
{
    // TODO: Ask sizer

    // Common case of no sizer and single child taking up the entire panel
    if(GetChildren().GetCount() == 1)
    {
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxMemoryDC dc;
        return m_art->GetPanelSize(dc, this, child->GetMinSize(), NULL);
    }

    return wxRibbonControl::GetMinSize();
}

wxSize wxRibbonPanel::DoGetBestSize() const
{
    // TODO: Ask sizer

    // Common case of no sizer and single child taking up the entire panel
    if(GetChildren().GetCount() == 1)
    {
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxMemoryDC dc;
        return m_art->GetPanelSize(dc, this, child->GetBestSize(), NULL);
    }

    return wxRibbonControl::DoGetBestSize();
}

bool wxRibbonPanel::Realize()
{
    bool status = true;

    for (wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
                  node;
                  node = node->GetNext())
    {
        wxRibbonControl* child = wxDynamicCast(node->GetData(), wxRibbonControl);
        if(child == NULL)
        {
            continue;
        }
        if(!child->Realize())
        {
            status = false;
        }
    }

    if(m_art != NULL)
    {
        wxMemoryDC temp_dc;
        wxSize bitmap_size;
        wxSize panel_min_size = GetMinNotMinimisedSize();
        m_minimised_size = m_art->GetMinimisedPanelMinimumSize(temp_dc, this,
            &bitmap_size);
        if(m_minimised_icon.IsOk() && m_minimised_icon.GetSize() != bitmap_size)
        {
            wxImage img(m_minimised_icon.ConvertToImage());
            img.Rescale(bitmap_size.GetWidth(), bitmap_size.GetHeight(), wxIMAGE_QUALITY_HIGH);
            m_minimised_icon_resized = wxBitmap(img);
        }
        else
        {
            m_minimised_icon_resized = m_minimised_icon;
        }
        if(m_minimised_size.x > panel_min_size.x ||
            m_minimised_size.y > panel_min_size.y)
        {
            m_minimised_size = wxSize(-1, -1);
        }
        else
        {
            if(m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL)
            {
                m_minimised_size.x = panel_min_size.x;
            }
            else
            {
                m_minimised_size.y = panel_min_size.y;
            }
        }
    }
    else
    {
        m_minimised_size = wxSize(-1, -1);
    }

    return Layout() && status;
}

bool wxRibbonPanel::Layout()
{
    if(IsMinimised())
    {
        // Children are all invisible when minimised
        return true;
    }

    // TODO: Delegate to a sizer

    // Common case of no sizer and single child taking up the entire panel
    if(GetChildren().GetCount() == 1)
    {
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxPoint position;
        wxMemoryDC dc;
        wxSize size = m_art->GetPanelClientSize(dc, this, GetSize(), &position);
        child->SetSize(position.x, position.y, size.GetWidth(), size.GetHeight());
    }
    return true;
}

#endif // wxUSE_RIBBON
