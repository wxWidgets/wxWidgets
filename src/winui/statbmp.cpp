/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/statbmp.cpp
// Purpose:     wxWinUI wxStaticBitmap implementation (WinUI Image)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATBMP

#include "wx/statbmp.h"
#include "wx/weakref.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/image.h"
#endif

#include "private.h"

#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXMI = winrt::Microsoft::UI::Xaml::Media::Imaging;

// The wxBitmap -> WriteableBitmap conversion is shared via the control host
// (wxWinUIWriteableBitmapFromBitmap, declared in private.h).

class wxWinUIStaticBitmapImpl
{
public:
    ~wxWinUIStaticBitmapImpl()
    {
        Close();
    }

    void Close()
    {
        host.Close();
        image = nullptr;
    }

    wxWinUIControlHost host;
    MUXC::Image image{ nullptr };
    wxSize selectedPixelSize;
    wxSize selectedDIPSize;
    unsigned generation = 0;
    bool hasSource = false;
};

wxStaticBitmap::wxStaticBitmap()
{
}

wxStaticBitmap::wxStaticBitmap(wxWindow *parent,
                               wxWindowID id,
                               const wxBitmapBundle& label,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
{
    Create(parent, id, label, pos, size, style, name);
}

wxStaticBitmap::wxStaticBitmap(wxWindow *parent,
                               wxWindowID id,
                               const wxIcon& label,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
{
    Create(parent, id, label, pos, size, style, name);
}

wxStaticBitmap::~wxStaticBitmap()
{
    Unbind(wxEVT_DPI_CHANGED, &wxStaticBitmap::OnDPIChanged, this);
    if ( m_winui )
        m_winui->Close();
}

bool wxStaticBitmap::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxBitmapBundle& label,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    m_bitmapBundle = label;
    m_icon = wxIcon();
    return DoCreate(parent, id, pos, size, style, name);
}

bool wxStaticBitmap::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxIcon& label,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    m_icon = label;
    m_bitmapBundle = wxBitmapBundle();
    return DoCreate(parent, id, pos, size, style, name);
}

bool wxStaticBitmap::DoCreate(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_winui.reset(new wxWinUIStaticBitmapImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;
    wxWinUIStaticBitmapImpl * const createImpl = m_winui.get();
    const wxWeakRef<wxStaticBitmap> lifetime(this);

    try
    {
        createImpl->image = MUXC::Image();
        createImpl->image.IsHitTestVisible(false);
        if ( !UpdateWinUIImage() )
        {
            wxStaticBitmap * const owner = lifetime.get();
            if ( owner && owner->m_winui.get() == createImpl )
                owner->m_winui.reset();
            return false;
        }
        const bool contentSet =
            createImpl->host.SetContent(createImpl->image);
        wxStaticBitmap * const owner = lifetime.get();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
        if ( !contentSet )
        {
            owner->m_winui.reset();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Image creation", e);
        wxStaticBitmap * const owner = lifetime.get();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxStaticBitmap *owner = lifetime.get();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }
    owner->SetInitialSize(size);
    owner = lifetime.get();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }
    owner->Bind(
        wxEVT_DPI_CHANGED, &wxStaticBitmap::OnDPIChanged, owner);
    return true;
}

void wxStaticBitmap::SetBitmap(const wxBitmapBundle& bitmap)
{
    wxWinUIStaticBitmapImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticBitmap> alive(this);
    m_bitmapBundle = bitmap;
    m_icon = wxIcon();
    InvalidateBestSize();
    if ( !UpdateWinUIImage() )
        return;
    wxStaticBitmap * const owner = alive.get();
    if ( owner && owner->m_winui.get() == impl &&
         owner->GetParent() && owner->GetContainingSizer() )
    {
        owner->GetParent()->Layout();
    }
}

wxBitmap wxStaticBitmap::GetBitmap() const
{
    if ( m_bitmapBundle.IsOk() )
        return m_bitmapBundle.GetBitmapFor(this);
    return wxBitmap();
}

void wxStaticBitmap::SetIcon(const wxIcon& icon)
{
    wxWinUIStaticBitmapImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticBitmap> alive(this);
    m_icon = icon;
    m_bitmapBundle = wxBitmapBundle();
    InvalidateBestSize();
    if ( !UpdateWinUIImage() )
        return;
    wxStaticBitmap * const owner = alive.get();
    if ( owner && owner->m_winui.get() == impl &&
         owner->GetParent() && owner->GetContainingSizer() )
    {
        owner->GetParent()->Layout();
    }
}

wxIcon wxStaticBitmap::GetIcon() const
{
    return m_icon;
}

void wxStaticBitmap::SetScaleMode(ScaleMode scaleMode)
{
    m_scaleMode = scaleMode;
    ApplyWinUIScaleMode();
}

bool wxStaticBitmap::ApplyWinUIScaleMode(bool forceRender)
{
    if ( !m_winui || !m_winui->image )
        return false;

    wxWinUIStaticBitmapImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticBitmap> alive(this);
    const MUXC::Image image = impl->image;
    const ScaleMode scaleMode = m_scaleMode;
    const wxSize selectedDIPSize = impl->selectedDIPSize;
    using winrt::Microsoft::UI::Xaml::Media::Stretch;
    Stretch stretch = Stretch::None;
    switch ( scaleMode )
    {
        case Scale_None:        stretch = Stretch::None; break;
        case Scale_Fill:        stretch = Stretch::Fill; break;
        case Scale_AspectFit:   stretch = Stretch::Uniform; break;
        case Scale_AspectFill:  stretch = Stretch::UniformToFill; break;
    }

    try
    {
        image.Stretch(stretch);

        // WriteableBitmap has no DPI metadata. In Scale_None, constrain the
        // Image to the bitmap's DIP size so a 32px @2x source occupies 16
        // XAML DIPs instead of being scaled a second time by the island.
        if ( scaleMode == Scale_None &&
             selectedDIPSize.x > 0 &&
             selectedDIPSize.y > 0 )
        {
            image.Width(selectedDIPSize.x);
            image.Height(selectedDIPSize.y);
            image.HorizontalAlignment(
                winrt::Microsoft::UI::Xaml::HorizontalAlignment::Left);
            image.VerticalAlignment(
                winrt::Microsoft::UI::Xaml::VerticalAlignment::Top);
        }
        else
        {
            image.ClearValue(
                winrt::Microsoft::UI::Xaml::FrameworkElement::WidthProperty());
            image.ClearValue(
                winrt::Microsoft::UI::Xaml::FrameworkElement::HeightProperty());
            image.HorizontalAlignment(
                winrt::Microsoft::UI::Xaml::HorizontalAlignment::Stretch);
            image.VerticalAlignment(
                winrt::Microsoft::UI::Xaml::VerticalAlignment::Stretch);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Image scale mode", e);
    }

    wxStaticBitmap * const owner = alive.get();
    if ( !owner || owner->m_winui.get() != impl )
        return false;

    if ( forceRender )
    {
        // A shared-host flush may destroy owner. It is deliberately terminal.
        impl->host.ForceRender();
    }
    return alive && alive->m_winui.get() == impl;
}

wxSize wxStaticBitmap::GetImageSize() const
{
    if ( m_icon.IsOk() )
        return m_icon.GetSize();
    if ( m_bitmapBundle.IsOk() )
        return m_bitmapBundle.GetPreferredBitmapSizeFor(this);
    return wxSize(0, 0);
}

wxSize wxStaticBitmap::DoGetBestSize() const
{
    const wxSize size = GetImageSize();
    if ( size.x > 0 && size.y > 0 )
        return size;
    return wxControl::DoGetBestSize();
}

bool wxStaticBitmap::UpdateWinUIImage(double requestedScale)
{
    if ( !m_winui || !m_winui->image )
        return false;

    wxWinUIStaticBitmapImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticBitmap> alive(this);
    const MUXC::Image image = impl->image;
    wxBitmap bmp;
    if ( m_icon.IsOk() )
        bmp.CopyFromIcon(m_icon);
    else if ( m_bitmapBundle.IsOk() )
    {
        if ( requestedScale > 0.0 )
        {
            bmp = m_bitmapBundle.GetBitmap(
                m_bitmapBundle.GetPreferredBitmapSizeAtScale(requestedScale));
        }
        else
        {
            bmp = m_bitmapBundle.GetBitmapFor(this);
        }
    }

    try
    {
        if ( bmp.IsOk() )
        {
            if ( auto source = wxWinUIWriteableBitmapFromBitmap(bmp) )
            {
                image.Source(source);
                if ( !alive || alive->m_winui.get() != impl )
                    return false;
                impl->hasSource = true;
            }
            else
            {
                image.Source(nullptr);
                if ( !alive || alive->m_winui.get() != impl )
                    return false;
                impl->hasSource = false;
            }

            impl->selectedPixelSize = bmp.GetSize();
            impl->selectedDIPSize = bmp.GetDIPSize();
        }
        else
        {
            image.Source(nullptr);
            if ( !alive || alive->m_winui.get() != impl )
                return false;
            impl->hasSource = false;
            impl->selectedPixelSize = wxSize();
            impl->selectedDIPSize = wxSize();
        }

        ++impl->generation;
        wxStaticBitmap * const owner = alive.get();
        if ( !owner || owner->m_winui.get() != impl ||
             !owner->ApplyWinUIScaleMode(false) )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Image update", e);
    }

    wxStaticBitmap * const owner = alive.get();
    if ( !owner || owner->m_winui.get() != impl )
        return false;

    // Exactly one terminal flush for the complete source+scale transaction.
    impl->host.ForceRender();
    return alive && alive->m_winui.get() == impl;
}

void wxStaticBitmap::OnDPIChanged(wxDPIChangedEvent& event)
{
    event.Skip();

    wxWinUIStaticBitmapImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticBitmap> alive(this);
    const wxSize oldBest = GetBestSize();
    InvalidateBestSize();
    if ( !UpdateWinUIImage() )
        return;
    wxStaticBitmap * const owner = alive.get();
    if ( !owner || owner->m_winui.get() != impl )
        return;
    const wxSize newBest = owner->GetBestSize();

    if ( newBest != oldBest && owner->GetParent() &&
         owner->GetContainingSizer() )
    {
        owner->GetParent()->Layout();
    }
}

bool wxStaticBitmap::WinUIRefreshForScaleForTesting(double scale)
{
    if ( scale <= 0.0 || !m_winui || !m_winui->image )
        return false;

    wxWinUIStaticBitmapImpl * const impl = m_winui.get();
    const wxWeakRef<wxStaticBitmap> alive(this);
    if ( !UpdateWinUIImage(scale) )
        return false;
    wxStaticBitmap * const owner = alive.get();
    return owner && owner->m_winui.get() == impl && impl->hasSource;
}

bool wxStaticBitmap::WinUIGetPeerImageStateForTesting(
    wxSize *pixelSize,
    wxSize *dipSize,
    int *stretch,
    unsigned *generation,
    bool *hasSource) const
{
    if ( !m_winui || !m_winui->image )
        return false;

    try
    {
        if ( pixelSize )
            *pixelSize = m_winui->selectedPixelSize;
        if ( dipSize )
            *dipSize = m_winui->selectedDIPSize;
        if ( stretch )
            *stretch = static_cast<int>(m_winui->image.Stretch());
        if ( generation )
            *generation = m_winui->generation;
        if ( hasSource )
            *hasSource = m_winui->hasSource;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

#endif // wxUSE_STATBMP
