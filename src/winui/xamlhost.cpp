/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/xamlhost.cpp
// Purpose:     wxWinUI XAML island host
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/xamlhost.h"

#if wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/string.h"
#endif

#include "wx/winui/winui.h"

// The content is hosted as one slot of the shared per-TLW island: this class
// no longer owns a DesktopWindowXamlSource of its own, so there is exactly
// one source per top-level window however many hosts and native controls it
// mixes -- and the content gets the island-wide input routing, focus
// arbitration and geometry sync for free.
#include "wx/winui/private/tlwhost.h"

#include <winrt/Microsoft.UI.Xaml.Markup.h>

#include <string>

namespace
{

void wxWinUILogException(const char *what, const winrt::hresult_error& e)
{
    wxLogWarning("%s failed with HRESULT 0x%08lx: %s",
                 what,
                 static_cast<unsigned long>(e.code()),
                 wxString(e.message().c_str()));
}

} // namespace

class wxWinUIXamlHostImpl
{
public:
    winrt::Microsoft::UI::Xaml::UIElement content{ nullptr };
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWinUIXamlHost, wxWindow);

wxWinUIXamlHost::wxWinUIXamlHost()
{
}

wxWinUIXamlHost::wxWinUIXamlHost(wxWindow *parent,
                                 wxWindowID id,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}

wxWinUIXamlHost::~wxWinUIXamlHost()
{
    // The shared host unregisters the slot on our wxEVT_DESTROY anyway; do
    // it explicitly too so the teardown does not depend on handler order.
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(this);
    if ( host )
        host->UnregisterSlot(this);
}

bool wxWinUIXamlHost::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    // wxWindow's generic MSW creation uses its resource name as the initial
    // HWND title. For an arbitrary XAML host this is not visible UI text and
    // must not become its accessible name. Preserve GetName() but start with
    // an empty label; applications can opt in explicitly with SetLabel().
    wxWindow::SetLabel(wxString());

    m_impl.reset(new wxWinUIXamlHostImpl);
    if ( !InitializeXamlSource() )
        return false;

    Bind(wxEVT_SIZE, &wxWinUIXamlHost::OnSize, this);
    return true;
}

bool wxWinUIXamlHost::InitializeXamlSource()
{
    if ( !wxWinUI3Initialize() )
        return false;

    if ( !m_impl )
        m_impl.reset(new wxWinUIXamlHostImpl);

    // "The source" is the top-level window's shared island: just make sure
    // it exists (created on first use).
    return wxWinUITopLevelHost::ForWindow(this, true) != nullptr;
}

bool wxWinUIXamlHost::SetContentFromXaml(const wxString& xaml)
{
    if ( !InitializeXamlSource() )
        return false;

    // Load into a local first: the previous content is only replaced once
    // the whole load + slot installation succeeded.
    winrt::Microsoft::UI::Xaml::UIElement newContent{ nullptr };
    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Markup;

        const std::wstring xamlText = xaml.ToStdWstring();
        const auto loaded = XamlReader::Load(winrt::hstring(xamlText));
        newContent = loaded.as<UIElement>();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("XamlReader::Load", e);
        return false;
    }

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ReconcileSlotOwner(this, true);
    if ( !host || !host->RegisterSlot(this, newContent) )
        return false;

    m_impl->content = newContent;
    return true;
}

void wxWinUIXamlHost::ClearContent()
{
    // Empty the slot first (keeping it: the API stays usable for the next
    // SetContentFromXaml); only forget our own reference once that
    // actually succeeded, so a failed detach leaves a truthful state.
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(this);
    if ( host )
    {
        if ( wxWinUISlot * const slot = host->FindSlot(this) )
        {
            if ( !slot->SetContent(nullptr) )
                return;
        }
    }

    if ( m_impl )
        m_impl->content = nullptr;
}

void wxWinUIXamlHost::SetLabel(const wxString& label)
{
    wxWindow::SetLabel(label);

    // Unlike a native control, this generic host has no component-specific
    // label setter to nudge the shared state adapter. Make the public wx label
    // transaction observable so it updates AutomationProperties.Name and, if
    // application XAML silently acquired an implicit Name style, performs the
    // conservative ownership check before publishing a local wx value.
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(this);
    if ( host )
        host->MarkDirty(this);
}

void wxWinUIXamlHost::MoveAndResizeXamlSource()
{
    // Geometry follows the HWND through the shared host's coalesced sync;
    // just nudge it.
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(this);
    if ( host )
        host->MarkDirty(this);
}

void wxWinUIXamlHost::OnSize(wxSizeEvent& event)
{
    MoveAndResizeXamlSource();
    event.Skip();
}

#endif // wxUSE_WINUI3
