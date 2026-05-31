/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/slider.cpp
// Purpose:     wxWinUI wxSlider implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SLIDER

#include "wx/slider.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "private.h"

#include <cmath>

class wxWinUISliderImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::Slider slider{ nullptr };
    winrt::event_token valueChangedToken{};
};

wxSlider::wxSlider()
    : m_value(0),
      m_rangeMin(0),
      m_rangeMax(100),
      m_lineSize(1),
      m_pageSize(10),
      m_thumbLength(0),
      m_tickFreq(0),
      m_updatingPeer(false)
{
}

wxSlider::wxSlider(wxWindow *parent,
                   wxWindowID id,
                   int value,
                   int minValue,
                   int maxValue,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxSlider()
{
    Create(parent, id, value, minValue, maxValue, pos, size, style, validator, name);
}

wxSlider::~wxSlider() = default;

bool wxSlider::Create(wxWindow *parent,
                      wxWindowID id,
                      int value,
                      int minValue,
                      int maxValue,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxCHECK_MSG( minValue < maxValue, false,
                 wxT("Slider minimum must be strictly less than the maximum.") );

    if ( !(style & (wxSL_HORIZONTAL | wxSL_VERTICAL)) )
        style |= wxSL_HORIZONTAL;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_rangeMin = minValue;
    m_rangeMax = maxValue;
    m_value = ClampValue(value);
    m_pageSize = wxMax(1, (maxValue - minValue) / 10);

    m_winui.reset(new wxWinUISliderImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml::Controls;
        using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;

        m_winui->slider = Slider();
        m_winui->slider.Foreground(wxWinUIBrush(0, 120, 215));
        m_winui->slider.Background(wxWinUIBrush(216, 216, 216));
        m_winui->slider.Orientation(
            HasFlag(wxSL_VERTICAL) ? Orientation::Vertical : Orientation::Horizontal);
        m_winui->slider.IsDirectionReversed(HasFlag(wxSL_INVERSE));
        m_winui->slider.StepFrequency(1);
        m_winui->slider.SnapsTo(SliderSnapsTo::StepValues);
        m_winui->slider.TickPlacement(
            HasFlag(wxSL_TICKS) ? TickPlacement::BottomRight : TickPlacement::None);

        m_winui->valueChangedToken = m_winui->slider.ValueChanged(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   RangeBaseValueChangedEventArgs const& event)
            {
                if ( !m_winui || m_updatingPeer )
                    return;

                m_value = ClampValue(static_cast<int>(std::lround(event.NewValue())));
                SendSliderEvent();
            });

        ApplyRangeToPeer();
        m_winui->host.SetContent(m_winui->slider);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Slider creation", e);
        return false;
    }

    return true;
}

int wxSlider::GetValue() const
{
    return m_value;
}

void wxSlider::SetValue(int value)
{
    m_value = ClampValue(value);
    ApplyValueToPeer();
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    wxCHECK_RET( minValue < maxValue,
                 wxT("Slider minimum must be strictly less than the maximum.") );

    m_rangeMin = minValue;
    m_rangeMax = maxValue;
    m_value = ClampValue(m_value);
    ApplyRangeToPeer();
}

void wxSlider::SetTick(int WXUNUSED(tickPos))
{
}

void wxSlider::Command(wxCommandEvent& event)
{
    SetValue(event.GetInt());
    ProcessCommand(event);
}

void wxSlider::DoSetTickFreq(int freq)
{
    m_tickFreq = freq;
    if ( m_winui && m_winui->slider )
        m_winui->slider.TickFrequency(freq);
}

wxSize wxSlider::DoGetBestSize() const
{
    if ( HasFlag(wxSL_VERTICAL) )
        return wxWindow::FromDIP(wxSize(44, 180), const_cast<wxSlider *>(this));

    return wxWindow::FromDIP(wxSize(220, 44), const_cast<wxSlider *>(this));
}

int wxSlider::ClampValue(int value) const
{
    if ( value < m_rangeMin )
        return m_rangeMin;
    if ( value > m_rangeMax )
        return m_rangeMax;
    return value;
}

void wxSlider::ApplyRangeToPeer()
{
    if ( !m_winui || !m_winui->slider )
        return;

    m_updatingPeer = true;
    m_winui->slider.Minimum(m_rangeMin);
    m_winui->slider.Maximum(m_rangeMax);
    m_winui->slider.Value(m_value);
    if ( m_tickFreq > 0 )
        m_winui->slider.TickFrequency(m_tickFreq);
    m_updatingPeer = false;
}

void wxSlider::ApplyValueToPeer()
{
    if ( !m_winui || !m_winui->slider )
        return;

    m_updatingPeer = true;
    m_winui->slider.Value(m_value);
    m_updatingPeer = false;
}

void wxSlider::SendSliderEvent()
{
    wxCommandEvent event(wxEVT_SLIDER, GetId());
    event.SetEventObject(this);
    event.SetInt(m_value);
    ProcessCommand(event);
}

#endif // wxUSE_SLIDER
