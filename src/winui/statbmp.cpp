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

#ifndef WX_PRECOMP
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
    wxWinUIControlHost host;
    MUXC::Image image{ nullptr };
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

wxStaticBitmap::~wxStaticBitmap() = default;

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

    try
    {
        m_winui->image = MUXC::Image();
        m_winui->image.Stretch(winrt::Microsoft::UI::Xaml::Media::Stretch::None);
        UpdateWinUIImage();
        m_winui->host.SetContent(m_winui->image);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Image creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxStaticBitmap::SetBitmap(const wxBitmapBundle& bitmap)
{
    m_bitmapBundle = bitmap;
    m_icon = wxIcon();
    InvalidateBestSize();
    UpdateWinUIImage();
}

wxBitmap wxStaticBitmap::GetBitmap() const
{
    if ( m_bitmapBundle.IsOk() )
        return m_bitmapBundle.GetBitmapFor(this);
    return wxBitmap();
}

void wxStaticBitmap::SetIcon(const wxIcon& icon)
{
    m_icon = icon;
    m_bitmapBundle = wxBitmapBundle();
    InvalidateBestSize();
    UpdateWinUIImage();
}

wxIcon wxStaticBitmap::GetIcon() const
{
    return m_icon;
}

void wxStaticBitmap::SetScaleMode(ScaleMode scaleMode)
{
    m_scaleMode = scaleMode;

    if ( !m_winui || !m_winui->image )
        return;

    using winrt::Microsoft::UI::Xaml::Media::Stretch;
    Stretch stretch = Stretch::None;
    switch ( scaleMode )
    {
        case Scale_None:        stretch = Stretch::None; break;
        case Scale_Fill:        stretch = Stretch::Fill; break;
        case Scale_AspectFit:   stretch = Stretch::Uniform; break;
        case Scale_AspectFill:  stretch = Stretch::UniformToFill; break;
    }

    m_winui->image.Stretch(stretch);
    m_winui->host.ForceRender();
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

void wxStaticBitmap::UpdateWinUIImage()
{
    if ( !m_winui || !m_winui->image )
        return;

    wxBitmap bmp;
    if ( m_icon.IsOk() )
        bmp.CopyFromIcon(m_icon);
    else if ( m_bitmapBundle.IsOk() )
        bmp = m_bitmapBundle.GetBitmapFor(this);

    try
    {
        if ( bmp.IsOk() )
        {
            if ( auto source = wxWinUIWriteableBitmapFromBitmap(bmp) )
                m_winui->image.Source(source);
        }
        else
        {
            m_winui->image.Source(nullptr);
        }
        m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Image update", e);
    }
}

#endif // wxUSE_STATBMP
