/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/buttonbox.cpp
// Purpose:     wxGenericButtonBox: segmented control made of toggle buttons
// Author:      Wlodzimierz ABX Skiba, LLM supported
// Created:     12.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BUTTONBOX

#include "wx/buttonbox.h"
#include "wx/generic/buttonbox.h"        // allow build even if native control is built

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
#endif

#include "wx/tglbtn.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxGenericButtonBox, wxControl);

bool wxGenericButtonBox::Create(wxWindow* parent, wxWindowID id,
                                const wxPoint& pos, const wxSize& size,
                                long style, const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style | wxBORDER_NONE,
                            wxDefaultValidator, name) )
        return false;

    m_sizer = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(m_sizer);
    SetInitialSize(size);
    return true;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

wxToggleButton* wxGenericButtonBox::CreateButton(const Segment& seg)
{
    wxToggleButton* btn;
    if ( seg.label.empty() && seg.bitmap.IsOk() )
    {
        btn = new wxBitmapToggleButton(this, wxID_ANY, seg.bitmap,
                                       wxDefaultPosition, wxDefaultSize,
                                       wxBU_EXACTFIT);
    }
    else
    {
        btn = new wxToggleButton(this, wxID_ANY, seg.label,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxBU_EXACTFIT);
        if ( seg.bitmap.IsOk() )
            btn->SetBitmap(seg.bitmap);
    }

    if ( !seg.tooltip.empty() )
        btn->SetToolTip(seg.tooltip);

    btn->Bind(wxEVT_TOGGLEBUTTON, &wxGenericButtonBox::OnToggle, this);
    return btn;
}

int wxGenericButtonBox::FindSegment(const wxObject* button) const
{
    for ( size_t i = 0; i < m_segments.size(); ++i )
    {
        if ( m_segments[i].button == button )
            return (int)i;
    }
    return wxNOT_FOUND;
}

void wxGenericButtonBox::Relayout()
{
    // The best size depends on the segments, so it must be recomputed.
    //
    // Note that SetInitialSize(GetSize()) must not be used here: it turns the
    // current size into the minimum size, and while there are no segments yet
    // this is 0x0, which would keep the control invisible forever.
    InvalidateBestSize();

    if ( GetContainingSizer() )
    {
        // Let the sizer give us the new best size and lay out our children
        // from our size event handler.
        GetParent()->Layout();
    }
    else
    {
        SetSize(GetBestSize());
        Layout();
    }
}

// Label/bitmap changes may switch between wxToggleButton and
// wxBitmapToggleButton, so the button is simply rebuilt in place.
void wxGenericButtonBox::ReplaceButton(size_t n)
{
    Segment& seg = m_segments[n];
    const bool selected = seg.button->GetValue();
    const bool enabled = seg.button->IsEnabled();
    const int width = seg.button->GetMinWidth();

    m_sizer->Detach(seg.button);
    seg.button->Destroy();

    seg.button = CreateButton(seg);
    seg.button->SetValue(selected);
    seg.button->Enable(enabled);
    if ( width != wxDefaultCoord )
        seg.button->SetMinSize(wxSize(width, wxDefaultCoord));
    m_sizer->Insert(n, seg.button, 0, wxEXPAND);
    Relayout();
}

// ----------------------------------------------------------------------------
// segments
// ----------------------------------------------------------------------------

void wxGenericButtonBox::Insert(size_t pos, const wxString& label,
                                const wxBitmapBundle& bitmap)
{
    wxCHECK_RET( pos <= GetCount(), "invalid segment position" );

    Segment seg;
    seg.label = label;
    seg.bitmap = bitmap;
    seg.button = CreateButton(seg);

    m_segments.insert(m_segments.begin() + pos, seg);
    m_sizer->Insert(pos, seg.button, 0, wxEXPAND);
    Relayout();
}

void wxGenericButtonBox::Delete(size_t pos)
{
    wxCHECK_RET( pos < GetCount(), "invalid segment index" );

    wxToggleButton* btn = m_segments[pos].button;
    m_segments.erase(m_segments.begin() + pos);
    m_sizer->Detach(btn);
    btn->Destroy();
    Relayout();
}

void wxGenericButtonBox::Clear()
{
    while ( GetCount() )
        Delete(GetCount() - 1);
    m_lastSelected = wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxGenericButtonBox::GetSelection() const
{
    if ( HasFlag(wxBB_MOMENTARY) )
        return wxNOT_FOUND;

    if ( m_lastSelected != wxNOT_FOUND &&
            m_lastSelected < (int)GetCount() &&
                IsSelected(m_lastSelected) )
        return m_lastSelected;

    for ( size_t i = 0; i < GetCount(); ++i )
    {
        if ( IsSelected(i) )
            return (int)i;
    }
    return wxNOT_FOUND;
}

void wxGenericButtonBox::SetSelection(int n)
{
    if ( HasFlag(wxBB_MOMENTARY) )
        return;

    if ( HasFlag(wxBB_SELECT_ANY) )
    {
        if ( n == wxNOT_FOUND )
        {
            for ( size_t i = 0; i < GetCount(); ++i )
                SetSelected(i, false);
        }
        else
        {
            SetSelected((size_t)n, true);
        }
        return;
    }

    for ( size_t i = 0; i < GetCount(); ++i )
        m_segments[i].button->SetValue((int)i == n);
    m_lastSelected = n;
}

bool wxGenericButtonBox::IsSelected(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), false, "invalid segment index" );
    return m_segments[n].button->GetValue();
}

void wxGenericButtonBox::SetSelected(size_t n, bool selected)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );

    if ( HasFlag(wxBB_MOMENTARY) )
        return;

    // Note that wxBB_SELECT_ONE is 0, so it can't be tested with HasFlag().
    if ( !HasFlag(wxBB_SELECT_ANY) )
    {
        if ( selected )
            SetSelection((int)n);
        else if ( IsSelected(n) )
            SetSelection(wxNOT_FOUND);
        return;
    }

    m_segments[n].button->SetValue(selected);
    if ( selected )
        m_lastSelected = (int)n;
}

void wxGenericButtonBox::OnToggle(wxCommandEvent& event)
{
    const int n = FindSegment(event.GetEventObject());
    if ( n == wxNOT_FOUND )
        return;

    wxToggleButton* btn = m_segments[n].button;
    bool selected = btn->GetValue();

    if ( HasFlag(wxBB_MOMENTARY) )
    {
        btn->SetValue(false);
        selected = true;        // "was clicked", as NSSegmentedControl does
    }
    else if ( HasFlag(wxBB_SELECT_ANY) )
    {
        if ( selected )
            m_lastSelected = n;
    }
    else // wxBB_SELECT_ONE: clicking the selected segment keeps it selected
    {
        for ( size_t i = 0; i < GetCount(); ++i )
            m_segments[i].button->SetValue((int)i == n);
        selected = true;
        m_lastSelected = n;
    }

    SendButtonBoxEvent(n, selected);
}

// ----------------------------------------------------------------------------
// per-segment attributes
// ----------------------------------------------------------------------------

void wxGenericButtonBox::SetItemLabel(size_t n, const wxString& label)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    m_segments[n].label = label;
    ReplaceButton(n);
}

wxString wxGenericButtonBox::GetItemLabel(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxString(), "invalid segment index" );
    return m_segments[n].label;
}

void wxGenericButtonBox::SetItemBitmap(size_t n, const wxBitmapBundle& bitmap)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    m_segments[n].bitmap = bitmap;
    ReplaceButton(n);
}

wxBitmapBundle wxGenericButtonBox::GetItemBitmap(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxBitmapBundle(), "invalid segment index" );
    return m_segments[n].bitmap;
}

void wxGenericButtonBox::SetItemWidth(size_t n, int width)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    m_segments[n].button->SetMinSize(
        wxSize(width > 0 ? width : wxDefaultCoord, wxDefaultCoord));
    Relayout();
}

int wxGenericButtonBox::GetItemWidth(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxDefaultCoord, "invalid segment index" );
    return m_segments[n].button->GetMinWidth();
}

void wxGenericButtonBox::EnableItem(size_t n, bool enable)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    m_segments[n].button->Enable(enable);
}

bool wxGenericButtonBox::IsItemEnabled(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), false, "invalid segment index" );
    return m_segments[n].button->IsEnabled();
}

void wxGenericButtonBox::SetItemToolTip(size_t n, const wxString& tip)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    m_segments[n].tooltip = tip;
    m_segments[n].button->SetToolTip(tip);
}

wxString wxGenericButtonBox::GetItemToolTip(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxString(), "invalid segment index" );
    return m_segments[n].tooltip;
}

wxSize wxGenericButtonBox::DoGetBestSize() const
{
    wxSize best = m_sizer ? m_sizer->CalcMin() : wxSize(0, 0);
    CacheBestSize(best);
    return best;
}

#endif // wxUSE_BUTTONBOX
