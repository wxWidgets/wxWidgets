/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/gauge.cpp
// Purpose:     wxWinUI wxGauge implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GAUGE

#include "wx/appprogress.h"
#include "wx/gauge.h"

#include "private.h"
#include "peerlifetime.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "range-test-access.h"
#endif

#include <algorithm>

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

namespace
{

// Delegates retain this invalidatable state, never the wx control. The
// generation check makes a late notification from a retired peer harmless,
// while invalidating the owner before token revocation covers teardown-time
// synchronous notifications.
using wxWinUIGaugeCallbackState = wxWinUIPeerLifetime<wxGauge>;

} // anonymous namespace

class wxWinUIGaugeImpl
{
public:
    ~wxWinUIGaugeImpl()
    {
        Close();
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();

        if ( panel && sizeChangedToken.value )
        {
            try
            {
                panel.SizeChanged(sizeChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI Gauge SizeChanged removal", e);
            }
        }
        sizeChangedToken = {};

        host.Close();
        panel = nullptr;
        progressBar = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIGaugeCallbackState> callbackState;
    MUXC::ProgressBar progressBar{ nullptr };
    MUXC::Grid panel{ nullptr };
    winrt::event_token sizeChangedToken{};
};

wxGauge::wxGauge()
{
}

wxGauge::wxGauge(wxWindow *parent,
                 wxWindowID id,
                 int range,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxValidator& validator,
                 const wxString& name)
{
    Create(parent, id, range, pos, size, style, validator, name);
}

wxGauge::~wxGauge() = default;

bool wxGauge::Create(wxWindow *parent,
                     wxWindowID id,
                     int range,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxValidator& validator,
                     const wxString& name)
{
    m_indeterminate = false;

    // This is not just a window-creation convenience: the common path
    // initializes the canonical range/value and wxGA_PROGRESS taskbar bridge.
    if ( !wxGaugeBase::Create(parent, id, range, pos, size,
                              style, validator, name) )
    {
        return false;
    }

    m_winui.reset(new wxWinUIGaugeImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIGaugeCallbackState>(this);
    wxWinUIGaugeImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIGaugeCallbackState> callbackState =
        createImpl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->progressBar = MUXC::ProgressBar();
        m_winui->progressBar.Minimum(0);
        ApplyToPeer();

        if ( HasFlag(wxGA_VERTICAL) )
        {
            // ProgressBar is horizontal-only: rotate it a quarter turn around
            // its centre and keep its width equal to the host height so the
            // rotated bar spans the control (filling bottom to top).
            winrt::Microsoft::UI::Xaml::Media::RotateTransform rotate;
            rotate.Angle(-90.0);
            m_winui->progressBar.RenderTransform(rotate);
            m_winui->progressBar.RenderTransformOrigin({ 0.5f, 0.5f });
            m_winui->progressBar.HorizontalAlignment(
                MUX::HorizontalAlignment::Center);
            m_winui->progressBar.VerticalAlignment(
                MUX::VerticalAlignment::Center);

            m_winui->panel = MUXC::Grid();
            m_winui->panel.Children().Append(m_winui->progressBar);
            m_winui->sizeChangedToken = m_winui->panel.SizeChanged(
                [callbackState, callbackGeneration](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::SizeChangedEventArgs const& event)
                {
                    wxGauge * const owner =
                        callbackState->GetOwner(callbackGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         !owner->m_winui->progressBar )
                    {
                        return;
                    }

                    owner->m_winui->progressBar.Width(
                        event.NewSize().Height);
                });
            MUX::Automation::AutomationProperties::SetAccessibilityView(
                createImpl->panel,
                MUX::Automation::Peers::AccessibilityView::Raw);
            const bool contentSet =
                createImpl->host.SetContent(
                    createImpl->panel, createImpl->progressBar);
            wxGauge * const owner =
                callbackState->GetOwner(callbackGeneration);
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
        else
        {
            const bool contentSet =
                createImpl->host.SetContent(createImpl->progressBar);
            wxGauge * const owner =
                callbackState->GetOwner(callbackGeneration);
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
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ProgressBar creation", e);
        wxGauge * const owner =
            callbackState->GetOwner(callbackGeneration);
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxGauge * const owner =
        callbackState->GetOwner(callbackGeneration);
    return owner && owner->m_winui &&
           owner->m_winui.get() == createImpl;
}

void wxGauge::SetRange(int range)
{
    // Like the native MSW control, either determinate setter terminates a
    // previous Pulse() mode. The base call also updates wxGA_PROGRESS.
    m_indeterminate = false;
    wxGaugeBase::SetRange(range);
    ApplyToPeer();
}

int wxGauge::GetRange() const
{
    return m_rangeMax;
}

void wxGauge::SetValue(int pos)
{
    m_indeterminate = false;
    wxGaugeBase::SetValue(pos);
    ApplyToPeer();
}

int wxGauge::GetValue() const
{
    return m_gaugePos;
}

void wxGauge::Pulse()
{
    m_indeterminate = true;
    ApplyToPeer();

    // wxGaugeBase::Pulse() emulates a bouncing gauge on WinUI and calls our
    // virtual SetValue(), which would immediately leave indeterminate mode.
    // Keep the native XAML peer indeterminate and update only the taskbar
    // bridge, exactly as the native MSW implementation does.
    if ( m_appProgressIndicator )
        m_appProgressIndicator->Pulse();
}

wxSize wxGauge::DoGetBestSize() const
{
    // A WinUI ProgressBar is a thin horizontal bar; use a sensible default
    // length and the control's natural (small) thickness.
    const wxSize size = HasFlag(wxGA_VERTICAL) ? wxSize(18, 100)
                                               : wxSize(100, 18);
    return wxWindow::FromDIP(size, const_cast<wxGauge *>(this));
}

void wxGauge::ApplyToPeer()
{
    if ( !m_winui || !m_winui->progressBar )
        return;

    try
    {
        MUXC::ProgressBar const progressBar = m_winui->progressBar;
        progressBar.IsIndeterminate(m_indeterminate);
        if ( !m_indeterminate )
        {
            // XAML rejects a zero maximum and a value outside [Minimum,
            // Maximum], while wxGauge deliberately preserves the caller's
            // model values. Clamp only the peer representation.
            const double maximum = m_rangeMax > 0 ? m_rangeMax : 1;
            const double value =
                std::clamp<double>(m_gaugePos, 0.0, maximum);

            // Lowering Maximum below the old Value throws. Update in the
            // direction that keeps every intermediate peer state valid.
            if ( maximum < progressBar.Maximum() )
            {
                progressBar.Value(value);
                progressBar.Maximum(maximum);
            }
            else
            {
                progressBar.Maximum(maximum);
                progressBar.Value(value);
            }
        }

        m_winui->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Gauge state update", e);
    }
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIRangeTestAccess::GetPeerState(const wxGauge& gauge,
                                          double *maximum,
                                          double *value,
                                          bool *indeterminate,
                                          bool *vertical)
{
    if ( !gauge.m_winui || !gauge.m_winui->progressBar )
        return false;

    try
    {
        if ( maximum )
            *maximum = gauge.m_winui->progressBar.Maximum();
        if ( value )
            *value = gauge.m_winui->progressBar.Value();
        if ( indeterminate )
            *indeterminate = gauge.m_winui->progressBar.IsIndeterminate();
        if ( vertical )
        {
            const auto rotate =
                gauge.m_winui->progressBar.RenderTransform()
                    .try_as<winrt::Microsoft::UI::Xaml::Media::
                                RotateTransform>();
            *vertical = gauge.m_winui->panel && rotate &&
                        rotate.Angle() == -90.0;
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Gauge test snapshot", e);
        return false;
    }
}

bool wxWinUIRangeTestAccess::HasAppProgress(const wxGauge& gauge)
{
    return gauge.m_appProgressIndicator != nullptr;
}
#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_GAUGE
