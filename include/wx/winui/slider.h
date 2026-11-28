/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/slider.h
// Purpose:     wxWinUI wxSlider declaration
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_SLIDER_H_
#define _WX_WINUI_SLIDER_H_

#include <cstdint>
#include <memory>
#include <vector>

class wxWinUISliderImpl;
class wxDPIChangedEvent;

class WXDLLIMPEXP_CORE wxSlider : public wxSliderBase
{
public:
    wxSlider();
    wxSlider(wxWindow *parent,
             wxWindowID id,
             int value,
             int minValue,
             int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxSliderNameStr));
    ~wxSlider() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                int value,
                int minValue,
                int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSliderNameStr));

    int GetValue() const override;
    void SetValue(int value) override;
    void SetRange(int minValue, int maxValue) override;
    int GetMin() const override { return m_rangeMin; }
    int GetMax() const override { return m_rangeMax; }
    void SetLineSize(int lineSize) override;
    void SetPageSize(int pageSize) override;
    int GetLineSize() const override { return m_lineSize; }
    int GetPageSize() const override { return m_pageSize; }
    void SetThumbLength(int lenPixels) override;
    int GetThumbLength() const override;
    int GetTickFreq() const override { return m_tickFreq; }
    void ClearTicks() override;
    void SetTick(int tickPos) override;
    void ClearSel() override;
    int GetSelEnd() const override { return m_selEnd; }
    int GetSelStart() const override { return m_selStart; }
    void SetSelection(int startPos, int endPos) override;

    void Command(wxCommandEvent& event) override;

protected:
    void DoSetTickFreq(int freq) override;
    wxSize DoGetBestSize() const override;
    bool MSWOnEffectiveLayoutDirectionChanged() override;

private:
    friend class wxWinUISliderTestAccess;
    friend class wxWinUISliderImpl;

    // Semantic input and realized geometry are part of the private peer path.
    enum class WinUIInput
    {
        LineDecrement,
        LineIncrement,
        PageDecrement,
        PageIncrement,
        Minimum,
        Maximum,
        ThumbTrack,
        ThumbRelease,
        Wheel,
        Automation
    };

    struct WinUIVisualRect
    {
        double x = 0.0;
        double y = 0.0;
        double width = 0.0;
        double height = 0.0;
    };

    int ClampValue(int value) const;
    void CancelPendingInput();
    void ApplyRangeToPeer();
    void ApplyValueToPeer();
    bool ApplyThumbLengthToPeer();
    void UpdateVisualState();
    double GetEffectiveRasterScale() const;
    WinUIInput GetPageInput(int oldValue, int newValue) const;
    void BeginInput(WinUIInput input);
    void BeginPointerInput(bool thumb);
    void OnPeerValueChanged(int value);
    void FlushPendingInput(WinUIInput input);
    void EndInput(bool pointerInput);
    void OnDPIChanged(wxDPIChangedEvent& event);

    std::unique_ptr<wxWinUISliderImpl> m_winui;
    int m_value = 0;
    int m_rangeMin = 0;
    int m_rangeMax = 100;
    int m_lineSize = 1;
    int m_pageSize = 10;
    // XAML dimensions are DIPs. Keep the requested thumb length in the same
    // unit and convert only at the wx pixel API boundary, so a monitor change
    // scales exactly once.
    double m_thumbLengthDIP = -1.0;
    int m_tickFreq = 0;
    int m_selStart = 0;
    int m_selEnd = 0;
    bool m_ticksCleared = false;
    std::vector<int> m_manualTicks;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSlider);
};

#endif // _WX_WINUI_SLIDER_H_
