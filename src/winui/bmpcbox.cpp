/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/bmpcbox.cpp
// Purpose:     wxWinUI wxBitmapComboBox (WinUI ComboBox with image items)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BITMAPCOMBOBOX

#include "wx/bmpcbox.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/log.h"
#endif

#include "wx/scopeguard.h"
#include "wx/weakref.h"

// wxBitmapComboBox builds on the WinUI wxChoice/wxComboBox image-item hook, so
// no direct WinRT use is needed here.

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapComboBox, wxComboBox);

wxBitmapComboBox::~wxBitmapComboBox()
{
    Unbind(wxEVT_DPI_CHANGED,
           &wxBitmapComboBox::OnDPIChanged, this);
}

bool wxBitmapComboBox::Create(wxWindow *parent, wxWindowID id,
                              const wxString& value, const wxPoint& pos,
                              const wxSize& size, int n, const wxString choices[],
                              long style, const wxValidator& validator,
                              const wxString& name)
{
    if ( !wxComboBox::Create(parent, id, value, pos, size, n, choices,
                             style, validator, name) )
    {
        return false;
    }

    Bind(wxEVT_DPI_CHANGED,
         &wxBitmapComboBox::OnDPIChanged, this);
    return true;
}

bool wxBitmapComboBox::Create(wxWindow *parent, wxWindowID id,
                              const wxString& value, const wxPoint& pos,
                              const wxSize& size, const wxArrayString& choices,
                              long style, const wxValidator& validator,
                              const wxString& name)
{
    if ( !wxComboBox::Create(parent, id, value, pos, size, choices,
                             style, validator, name) )
    {
        return false;
    }

    Bind(wxEVT_DPI_CHANGED,
         &wxBitmapComboBox::OnDPIChanged, this);
    return true;
}

void wxBitmapComboBox::SetItemBitmap(unsigned int n, const wxBitmapBundle& bitmap)
{
    // Make sure the bundle vector is grown to match the item count, then set
    // the requested one (DoSetItemBitmap requires the slot to already exist).
    UpdateInternals();
    if ( n < GetCount() )
    {
        DoSetItemBitmap(n, bitmap);
        OnAddBitmap(bitmap);
        WinUIUpdatePeerItem(n);
        WinUIEnsurePeerConsistent();
    }
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap)
{
    const auto pending = std::make_shared<wxBitmapBundle>(bitmap);
    m_pendingBitmap = pending;
    const wxWeakRef<wxWindow> self(this);
    wxScopeGuard resetPending = wxMakeGuard([self, pending]()
    {
        wxBitmapComboBox * const live =
            wxDynamicCast(self.get(), wxBitmapComboBox);
        if ( live && live->m_pendingBitmap == pending )
            live->m_pendingBitmap.reset();
    });
    wxUnusedVar(resetPending);
    return wxComboBox::Append(item);
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap,
                             void *clientData)
{
    const auto pending = std::make_shared<wxBitmapBundle>(bitmap);
    m_pendingBitmap = pending;
    const wxWeakRef<wxWindow> self(this);
    wxScopeGuard resetPending = wxMakeGuard([self, pending]()
    {
        wxBitmapComboBox * const live =
            wxDynamicCast(self.get(), wxBitmapComboBox);
        if ( live && live->m_pendingBitmap == pending )
            live->m_pendingBitmap.reset();
    });
    wxUnusedVar(resetPending);
    return wxComboBox::Append(item, clientData);
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap,
                             wxClientData *clientData)
{
    const auto pending = std::make_shared<wxBitmapBundle>(bitmap);
    m_pendingBitmap = pending;
    const wxWeakRef<wxWindow> self(this);
    wxScopeGuard resetPending = wxMakeGuard([self, pending]()
    {
        wxBitmapComboBox * const live =
            wxDynamicCast(self.get(), wxBitmapComboBox);
        if ( live && live->m_pendingBitmap == pending )
            live->m_pendingBitmap.reset();
    });
    wxUnusedVar(resetPending);
    return wxComboBox::Append(item, clientData);
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos)
{
    const auto pending = std::make_shared<wxBitmapBundle>(bitmap);
    m_pendingBitmap = pending;
    const wxWeakRef<wxWindow> self(this);
    wxScopeGuard resetPending = wxMakeGuard([self, pending]()
    {
        wxBitmapComboBox * const live =
            wxDynamicCast(self.get(), wxBitmapComboBox);
        if ( live && live->m_pendingBitmap == pending )
            live->m_pendingBitmap.reset();
    });
    wxUnusedVar(resetPending);
    return wxComboBox::Insert(item, pos);
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos, void *clientData)
{
    const auto pending = std::make_shared<wxBitmapBundle>(bitmap);
    m_pendingBitmap = pending;
    const wxWeakRef<wxWindow> self(this);
    wxScopeGuard resetPending = wxMakeGuard([self, pending]()
    {
        wxBitmapComboBox * const live =
            wxDynamicCast(self.get(), wxBitmapComboBox);
        if ( live && live->m_pendingBitmap == pending )
            live->m_pendingBitmap.reset();
    });
    wxUnusedVar(resetPending);
    return wxComboBox::Insert(item, pos, clientData);
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos, wxClientData *clientData)
{
    const auto pending = std::make_shared<wxBitmapBundle>(bitmap);
    m_pendingBitmap = pending;
    const wxWeakRef<wxWindow> self(this);
    wxScopeGuard resetPending = wxMakeGuard([self, pending]()
    {
        wxBitmapComboBox * const live =
            wxDynamicCast(self.get(), wxBitmapComboBox);
        if ( live && live->m_pendingBitmap == pending )
            live->m_pendingBitmap.reset();
    });
    wxUnusedVar(resetPending);
    return wxComboBox::Insert(item, pos, clientData);
}

wxBitmap wxBitmapComboBox::WinUIGetItemBitmap(
    unsigned int n,
    double requestedScale) const
{
    if ( n >= m_bitmapbundles.size() )
        return wxBitmap();

    const wxBitmapBundle& bundle = m_bitmapbundles[n];
    if ( !bundle.IsOk() )
        return wxBitmap();

    if ( requestedScale > 0.0 )
    {
        return bundle.GetBitmap(
            bundle.GetPreferredBitmapSizeAtScale(requestedScale));
    }

    return GetItemBitmap(n);
}

void wxBitmapComboBox::OnDPIChanged(wxDPIChangedEvent& event)
{
    // The event carries the destination DPI even when a deterministic test
    // does not physically move the HWND. Use it for peer reprojection, while
    // UpdateInternals() recomputes the normal physical best-size cache from
    // the control's real DPI during an actual monitor transition.
    const wxSize newDPI = event.GetNewDPI();
    const double requestedScale =
        newDPI.x > 0 ? static_cast<double>(newDPI.x) / 96.0 : 0.0;

    UpdateInternals();
    WinUIRefreshItems(requestedScale);
    InvalidateBestSize();
    if ( GetParent() && GetContainingSizer() )
        GetParent()->Layout();

    event.Skip();
}

wxSize wxBitmapComboBox::DoGetBestSize() const
{
    wxSize size = wxComboBox::DoGetBestSize();
    // GetConsensusSizeFor(), which backs GetBitmapSize(), already returns
    // physical pixels for this control. Applying FromDIP() here scales it a
    // second time at 125/150/200% DPI.
    const wxSize bitmapSize = GetBitmapSize();
    if ( bitmapSize.x > 0 && bitmapSize.y > 0 )
    {
        const wxSize spacing = FromDIP(wxSize(8, 8));
        size.x += bitmapSize.x + spacing.x;
        size.y = wxMax(size.y, bitmapSize.y + spacing.y);
    }
    return size;
}

void wxBitmapComboBox::WinUIOnItemInserted(unsigned int n)
{
    const wxBitmapBundle bitmap =
        m_pendingBitmap ? *m_pendingBitmap : wxBitmapBundle();
    m_bitmapbundles.insert(m_bitmapbundles.begin() + n, bitmap);
    OnAddBitmap(bitmap);
}

void wxBitmapComboBox::WinUIOnItemErased(unsigned int n)
{
    BCBDoDeleteOneItem(n);
    InvalidateBestSize();
}

void wxBitmapComboBox::WinUIOnItemMoved(unsigned int oldIndex,
                                        unsigned int newIndex)
{
    if ( oldIndex >= m_bitmapbundles.size() ||
            oldIndex == newIndex )
    {
        return;
    }

    const wxBitmapBundle bitmap = m_bitmapbundles[oldIndex];
    m_bitmapbundles.erase(m_bitmapbundles.begin() + oldIndex);
    m_bitmapbundles.insert(m_bitmapbundles.begin() + newIndex, bitmap);
    InvalidateBestSize();
}

void wxBitmapComboBox::WinUIOnItemsCleared()
{
    BCBDoClear();
    InvalidateBestSize();
}

#endif // wxUSE_BITMAPCOMBOBOX
