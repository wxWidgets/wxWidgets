/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/clrpicker.cpp
// Purpose:     wxWinUI colour picker widget (WinUI DropDownButton + ColorPicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COLOURPICKERCTRL

#include "wx/clrpicker.h"

#include "private.h"

#ifdef WXWINUI_TEST_SUPPORT
#include "picker-test-access.h"
#endif

#include <atomic>
#include <memory>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
std::atomic<unsigned> gs_liveColourButtonCallbackStates{0};
#endif

winrt::Windows::UI::Color wxWinUIToColor(const wxColour& c)
{
    winrt::Windows::UI::Color color{};
    color.A = c.IsOk() ? c.Alpha() : 255;
    color.R = c.IsOk() ? c.Red() : 0;
    color.G = c.IsOk() ? c.Green() : 0;
    color.B = c.IsOk() ? c.Blue() : 0;
    return color;
}

wxColour wxWinUIFromColor(const winrt::Windows::UI::Color& c)
{
    return wxColour(c.R, c.G, c.B, c.A);
}

} // namespace

class wxWinUIColourButtonCallbackState final
{
public:
    explicit wxWinUIColourButtonCallbackState(wxWinUIColourButton *owner)
        : m_owner(owner)
    {
#ifdef WXWINUI_TEST_SUPPORT
        gs_liveColourButtonCallbackStates.fetch_add(
            1, std::memory_order_relaxed);
#endif
    }

    ~wxWinUIColourButtonCallbackState()
    {
#ifdef WXWINUI_TEST_SUPPORT
        gs_liveColourButtonCallbackStates.fetch_sub(
            1, std::memory_order_relaxed);
#endif
    }

    wxWinUIColourButton *GetOwner() const
    {
        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
    }

private:
    std::atomic<wxWinUIColourButton *> m_owner;
};

class wxWinUIColourButtonImpl
{
public:
    ~wxWinUIColourButtonImpl()
    {
        // A flyout can outlive the button visual while its popup is closing.
        // Make every callback inert before revoking its WinRT registration or
        // detaching the visual tree: either operation may synchronously drain
        // a queued notification.
        if ( callbackState )
            callbackState->Invalidate();

        try
        {
            if ( colorChangedToken.value && picker )
                picker.ColorChanged(colorChangedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("ColorPicker::ColorChanged removal", e);
        }
        colorChangedToken = {};

        try
        {
            if ( closedToken.value && flyout )
                flyout.Closed(closedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("ColorPicker Flyout::Closed removal", e);
        }
        closedToken = {};

        host.Close();
    }

    wxWinUIControlHost host;
    MUXC::DropDownButton button{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Border swatch{ nullptr };
    MUXC::TextBlock label{ nullptr };
    MUXC::ColorPicker picker{ nullptr };
    MUXC::Flyout flyout{ nullptr };
    winrt::event_token colorChangedToken{};
    winrt::event_token closedToken{};
    std::shared_ptr<wxWinUIColourButtonCallbackState> callbackState;
    bool updating = false;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWinUIColourButton, wxControl);

wxWinUIColourButton::wxWinUIColourButton()
{
}

wxWinUIColourButton::wxWinUIColourButton(wxWindow *parent, wxWindowID id,
                                         const wxColour& col, const wxPoint& pos,
                                         const wxSize& size, long style,
                                         const wxValidator& validator,
                                         const wxString& name)
{
    Create(parent, id, col, pos, size, style, validator, name);
}

wxWinUIColourButton::~wxWinUIColourButton() = default;

bool wxWinUIColourButton::Create(wxWindow *parent, wxWindowID id,
                                 const wxColour& col, const wxPoint& pos,
                                 const wxSize& size, long style,
                                 const wxValidator& validator,
                                 const wxString& name)
{
    const long ctrlStyle = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, ctrlStyle, validator, name) )
        return false;

    m_colour = col.IsOk() ? col : *wxBLACK;

    m_winui.reset(new wxWinUIColourButtonImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIColourButtonCallbackState>(this);
    wxWinUIColourButtonImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIColourButtonCallbackState> callbackState =
        createImpl->callbackState;
    const bool hostInitialized = createImpl->host.Initialize(this);
    wxWinUIColourButton *ownerAfterInitialize = callbackState->GetOwner();
    if ( !ownerAfterInitialize || !ownerAfterInitialize->m_winui ||
         ownerAfterInitialize->m_winui.get() != createImpl )
    {
        return false;
    }
    if ( !hostInitialized )
    {
        ownerAfterInitialize->m_winui.reset();
        return false;
    }

    try
    {
        // The button face shows a colour swatch.
        m_winui->swatch = winrt::Microsoft::UI::Xaml::Controls::Border();
        m_winui->swatch.Width(28.0);
        m_winui->swatch.Height(16.0);
        m_winui->swatch.CornerRadius(
            winrt::Microsoft::UI::Xaml::CornerRadiusHelper::FromUniformRadius(3));

        // The drop-down hosts a WinUI ColorPicker.
        m_winui->picker = MUXC::ColorPicker();
        m_winui->picker.IsAlphaEnabled((style & wxCLRP_SHOW_ALPHA) != 0);

        m_winui->flyout = MUXC::Flyout();
        m_winui->flyout.Content(m_winui->picker);
        // The control's island is only as large as the button, so the flyout
        // must be allowed to escape the island root bounds (it becomes a
        // windowed popup) instead of being clipped to the button's size.
        m_winui->flyout.ShouldConstrainToRootBounds(false);

        m_winui->button = MUXC::DropDownButton();
        if ( style & wxCLRP_SHOW_LABEL )
        {
            // Show the colour value as text next to the swatch.
            m_winui->label = MUXC::TextBlock();
            m_winui->label.VerticalAlignment(
                winrt::Microsoft::UI::Xaml::VerticalAlignment::Center);

            MUXC::StackPanel face;
            face.Orientation(MUXC::Orientation::Horizontal);
            face.Spacing(8);
            face.Children().Append(m_winui->swatch);
            face.Children().Append(m_winui->label);
            m_winui->button.Content(face);
        }
        else
        {
            m_winui->button.Content(m_winui->swatch);
        }
        m_winui->button.Flyout(m_winui->flyout);

        m_winui->colorChangedToken = m_winui->picker.ColorChanged(
            [callbackState](MUXC::ColorPicker const&,
                            MUXC::ColorChangedEventArgs const& args)
            {
                wxWinUIColourButton * const owner =
                    callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->updating )
                {
                    return;
                }

                owner->m_colour = wxWinUIFromColor(args.NewColor());
                if ( owner->m_winui->swatch )
                {
                    owner->m_winui->swatch.Background(
                        wxWinUIBrush(owner->m_colour.Red(),
                                     owner->m_colour.Green(),
                                     owner->m_colour.Blue(),
                                     owner->m_colour.Alpha()));
                }
                if ( owner->m_winui->label )
                {
                    owner->m_winui->label.Text(
                        wxWinUIToHString(owner->m_colour.GetAsString(
                            wxC2S_HTML_SYNTAX)));
                }

                // SendColourEvent() may synchronously destroy the control.
                // It must remain the final operation in this callback.
                owner->SendColourEvent(wxEVT_COLOURPICKER_CURRENT_CHANGED);
            });

        m_winui->closedToken = m_winui->flyout.Closed(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                wxWinUIColourButton * const owner =
                    callbackState->GetOwner();
                if ( owner )
                    owner->SendColourEvent(wxEVT_COLOURPICKER_CHANGED);
            });

        ApplyColourToPeer();
        const bool contentSet =
            createImpl->host.SetContent(createImpl->button);
        wxWinUIColourButton * const owner = callbackState->GetOwner();
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
        wxWinUILogException("WinUI ColorPicker creation", e);
        wxWinUIColourButton * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxWinUIColourButton * const owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    owner->SetInitialSize(size);
    wxWinUIColourButton * const ownerAfterSize =
        callbackState->GetOwner();
    return ownerAfterSize && ownerAfterSize->m_winui &&
           ownerAfterSize->m_winui.get() == createImpl;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIColourButtonTestAccess::SetPeerColour(
    wxWinUIColourButton& button,
    const wxColour& colour)
{
    if ( !button.m_winui || !button.m_winui->picker )
        return false;

    // Keep the WinRT peer alive independently of the wx owner. Color() can
    // synchronously deliver an application event which destroys this control.
    const MUXC::ColorPicker picker = button.m_winui->picker;
    try
    {
        picker.Color(wxWinUIToColor(colour));
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("ColorPicker testing colour write", e);
        return false;
    }
}

bool wxWinUIColourButtonTestAccess::GetPeerState(
    const wxWinUIColourButton& button,
    wxColour *colour,
    bool *alphaEnabled,
    wxString *label)
{
    if ( !button.m_winui || !button.m_winui->picker )
        return false;

    try
    {
        if ( colour )
            *colour = wxWinUIFromColor(button.m_winui->picker.Color());
        if ( alphaEnabled )
            *alphaEnabled = button.m_winui->picker.IsAlphaEnabled();
        if ( label )
        {
            *label = button.m_winui->label
                ? wxWinUIFromHString(button.m_winui->label.Text())
                : wxString();
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("ColorPicker testing state read", e);
        return false;
    }
}

bool wxWinUIColourButtonTestAccess::DeliverClosed(
    wxWinUIColourButton& button)
{
    if ( !button.m_winui )
        return false;

    // This is the exact final operation of the real Flyout::Closed callback.
    // Its event is allowed to destroy the control.
    button.SendColourEvent(wxEVT_COLOURPICKER_CHANGED);
    return true;
}

unsigned wxWinUIColourButtonTestAccess::GetLiveCallbackStateCount()
{
    return gs_liveColourButtonCallbackStates.load(std::memory_order_relaxed);
}

#endif // WXWINUI_TEST_SUPPORT

void wxWinUIColourButton::UpdateColour()
{
    ApplyColourToPeer();
}

void wxWinUIColourButton::ApplyColourToPeer()
{
    if ( !m_winui )
        return;

    m_winui->updating = true;
    try
    {
        if ( m_winui->swatch )
            m_winui->swatch.Background(wxWinUIBrush(m_colour.Red(),
                m_colour.Green(), m_colour.Blue(), m_colour.Alpha()));
        if ( m_winui->label )
            m_winui->label.Text(wxWinUIToHString(
                m_colour.GetAsString(wxC2S_HTML_SYNTAX)));
        if ( m_winui->picker )
            m_winui->picker.Color(wxWinUIToColor(m_colour));
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_winui->updating = false;
    m_winui->host.ForceRender();
}

void wxWinUIColourButton::SendColourEvent(wxEventType type)
{
    wxColourPickerEvent event(this, GetId(), m_colour, type);
    HandleWindowEvent(event);
}

wxSize wxWinUIColourButton::DoGetBestSize() const
{
    const wxSize size = HasFlag(wxCLRP_SHOW_LABEL) ? wxSize(140, 32)
                                                   : wxSize(72, 32);
    return wxWindow::FromDIP(size, const_cast<wxWinUIColourButton*>(this));
}

#endif // wxUSE_COLOURPICKERCTRL
