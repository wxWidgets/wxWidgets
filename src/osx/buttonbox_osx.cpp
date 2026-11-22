/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/buttonbox_osx.cpp
// Purpose:     wxButtonBox implementation common to all wxOSX ports
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     22.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/buttonbox.h"

// wxHAS_NATIVE_BUTTONBOX is defined only for the ports having a native
// implementation, which currently means only wxOSX/Cocoa.
#if wxUSE_BUTTONBOX && defined(wxHAS_NATIVE_BUTTONBOX)

#include "wx/osx/private.h"
#include "wx/osx/private/buttonbox.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxButtonBox, wxControl);

// ----------------------------------------------------------------------------
// wxButtonBox
// ----------------------------------------------------------------------------

bool wxButtonBox::Create(wxWindow* parent, wxWindowID id,
                         const wxPoint& pos, const wxSize& size,
                         long style, const wxString& name)
{
    DontCreatePeer();

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    SetPeer(wxWidgetImpl::CreateButtonBox(this, parent, id, pos, size, style,
                                          GetExtraStyle()));
    MacPostControlCreate(pos, size);
    return true;
}

wxButtonBoxImpl* wxButtonBox::GetButtonBoxPeer() const
{
    return dynamic_cast<wxButtonBoxImpl*>(GetPeer());
}

size_t wxButtonBox::GetCount() const
{
    return GetButtonBoxPeer()->GetCount();
}

void wxButtonBox::Insert(size_t pos, const wxString& label,
                         const wxBitmapBundle& bitmap)
{
    wxCHECK_RET( pos <= GetCount(), "invalid segment position" );
    m_bitmaps.insert(m_bitmaps.begin() + pos, bitmap);
    GetButtonBoxPeer()->Insert(pos, label, bitmap);
    InvalidateBestSize();
}

void wxButtonBox::Delete(size_t pos)
{
    wxCHECK_RET( pos < GetCount(), "invalid segment index" );
    m_bitmaps.erase(m_bitmaps.begin() + pos);
    GetButtonBoxPeer()->Delete(pos);
    InvalidateBestSize();
}

void wxButtonBox::Clear()
{
    while ( GetCount() )
        Delete(GetCount() - 1);
}

int wxButtonBox::GetSelection() const
{
    return HasFlag(wxBB_MOMENTARY) ? wxNOT_FOUND : GetButtonBoxPeer()->GetSelection();
}

void wxButtonBox::SetSelection(int n)
{
    if ( HasFlag(wxBB_MOMENTARY) )
        return;

    if ( n == wxNOT_FOUND )
        GetButtonBoxPeer()->ClearSelection();
    else
        SetSelected((size_t)n, true);
}

bool wxButtonBox::IsSelected(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), false, "invalid segment index" );
    return GetButtonBoxPeer()->IsSelected(n);
}

void wxButtonBox::SetSelected(size_t n, bool selected)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    if ( HasFlag(wxBB_MOMENTARY) )
        return;
    GetButtonBoxPeer()->SetSelected(n, selected);
}

void wxButtonBox::SetItemLabel(size_t n, const wxString& label)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    GetButtonBoxPeer()->SetItemLabel(n, label);
    InvalidateBestSize();
}

wxString wxButtonBox::GetItemLabel(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxString(), "invalid segment index" );
    return GetButtonBoxPeer()->GetItemLabel(n);
}

void wxButtonBox::SetItemBitmap(size_t n, const wxBitmapBundle& bitmap)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    m_bitmaps[n] = bitmap;
    GetButtonBoxPeer()->SetItemBitmap(n, bitmap);
    InvalidateBestSize();
}

wxBitmapBundle wxButtonBox::GetItemBitmap(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxBitmapBundle(), "invalid segment index" );
    return m_bitmaps[n];
}

void wxButtonBox::SetItemWidth(size_t n, int width)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    GetButtonBoxPeer()->SetItemWidth(n, width);
    InvalidateBestSize();
}

int wxButtonBox::GetItemWidth(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxDefaultCoord, "invalid segment index" );
    return GetButtonBoxPeer()->GetItemWidth(n);
}

void wxButtonBox::EnableItem(size_t n, bool enable)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    GetButtonBoxPeer()->EnableItem(n, enable);
}

bool wxButtonBox::IsItemEnabled(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), false, "invalid segment index" );
    return GetButtonBoxPeer()->IsItemEnabled(n);
}

void wxButtonBox::SetItemToolTip(size_t n, const wxString& tip)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    GetButtonBoxPeer()->SetItemToolTip(n, tip);
}

wxString wxButtonBox::GetItemToolTip(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxString(), "invalid segment index" );
    return GetButtonBoxPeer()->GetItemToolTip(n);
}

// For all three tracking modes NSSegmentedControl's selectedSegment is the
// segment that was just clicked, which is what the event reports.
bool wxButtonBox::OSXHandleClicked(double /* timestampsec */)
{
    wxButtonBoxImpl* const peer = GetButtonBoxPeer();

    const int n = peer->GetSelection();
    if ( n < 0 )
        return false;

    const bool selected = HasFlag(wxBB_MOMENTARY) ? true
                                                  : peer->IsSelected((size_t)n);
    SendButtonBoxEvent(n, selected);
    return true;
}

#endif // wxUSE_BUTTONBOX && wxHAS_NATIVE_BUTTONBOX
