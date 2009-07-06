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
    EVT_ERASE_BACKGROUND(wxRibbonPanel::OnEraseBackground)
    EVT_PAINT(wxRibbonPanel::OnPaint)
    EVT_SIZE(wxRibbonPanel::OnSize)
END_EVENT_TABLE()

wxRibbonPanel::wxRibbonPanel()
{
}

#define FORCED_STYLE 0

wxRibbonPanel::wxRibbonPanel(wxWindow* parent,
                  wxWindowID id,
                  const wxString& label,
                  const wxBitmap& minimised_icon,
                  const wxPoint& pos,
                  const wxSize& size,
                  long style)
    : wxRibbonControl(parent, id, pos, size, style | FORCED_STYLE)
{
    CommonInit(label, minimised_icon, style | FORCED_STYLE);
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
    style |= FORCED_STYLE;

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

    m_flags = style;
    m_minimised_icon = icon;

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

void wxRibbonPanel::OnSize(wxSizeEvent& evt)
{
    if(GetAutoLayout())
        Layout();

    evt.Skip();
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
        m_art->DrawPanelBackground(dc, this, GetSize());
    }
}

bool wxRibbonPanel::IsSizingContinuous() const
{
    // A panel never sizes continuously, even if all of its children can,
    // as it would appear out of place along side non-continuous panels.
    return false;
}

wxSize wxRibbonPanel::GetNextSmallerSize(wxOrientation direction) const
{
    // TODO: Check for, and delegate to, a sizer

    // Simple (and common) case of single ribbon child
    if(GetChildren().GetCount() == 1)
    {
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxRibbonControl* ribbon_child = wxDynamicCast(child, wxRibbonControl);
        if(ribbon_child != NULL)
        {
            wxSize smaller = ribbon_child->GetNextSmallerSize(direction);
            if(smaller == ribbon_child->GetSize())
            {
                return GetSize();
            }
            else
            {
                wxMemoryDC dc;
                return m_art->GetPanelSize(dc, this, smaller, NULL);
            }
        }
    }

    // Fallback: Decrease by 20% (or minimum size, whichever larger)
    wxSize current(GetSize());
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

wxSize wxRibbonPanel::GetNextLargerSize(wxOrientation direction) const
{
    // TODO: Check for, and delegate to, a sizer

    // Simple (and common) case of single ribbon child
    if(GetChildren().GetCount() == 1)
    {
        wxWindow* child = GetChildren().Item(0)->GetData();
        wxRibbonControl* ribbon_child = wxDynamicCast(child, wxRibbonControl);
        if(ribbon_child != NULL)
        {
            wxSize larger = ribbon_child->GetNextLargerSize(direction);
            if(larger == ribbon_child->GetSize())
            {
                return GetSize();
            }
            else
            {
                wxMemoryDC dc;
                return m_art->GetPanelSize(dc, this, larger, NULL);
            }
        }
    }

    // Fallback: Increase by 25% (equal to a prior or subsequent 20% decrease)
    wxSize current(GetSize());
    if(direction & wxHORIZONTAL)
    {
        current.x = (current.x * 5) / 4;
    }
    if(direction & wxVERTICAL)
    {
        current.y = (current.y * 5) / 4;
    }
    return current;
}

wxSize wxRibbonPanel::GetMinSize() const
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

    return Layout() && status;
}

bool wxRibbonPanel::Layout()
{
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
