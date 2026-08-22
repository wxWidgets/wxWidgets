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
    // Implementation-only semantic input used by deterministic WinUI tests.
    // Each value names an input source, never a numeric XAML enum value.
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

    enum class WinUISelectionVisual
    {
        Normal,
        Disabled,
        HighContrast,
        HighContrastDisabled
    };

    enum class WinUIActualTheme
    {
        Default,
        Light,
        Dark
    };

    enum class WinUIConvergenceFailure
    {
        None,
        MissingPeer,
        LifetimeChanged,
        NotLoaded,
        EmptyGeometry,
        MissingThumb,
        MissingTrack,
        ThumbNotArranged,
        VisualStateDirty,
        CacheInvalid,
        LayoutNotReady,
        SnapshotRejected
    };

    using WinUIRetemplateHookForTesting =
        void (*)(wxSlider *, void *);

    struct WinUIVisualRect
    {
        double x = 0.0;
        double y = 0.0;
        double width = 0.0;
        double height = 0.0;
    };

    // A snapshot of the realized XAML subtree, not of the wx model. This is
    // intentionally implementation-only and lets WinUI tests catch template,
    // layout, DPI and ThemeResource regressions at the peer boundary.
    struct WinUIVisualState
    {
        double rasterScale = 1.0;
        double rootWidth = 0.0;
        double rootHeight = 0.0;
        double sliderMarginLeft = 0.0;
        double sliderMarginTop = 0.0;
        double sliderMarginRight = 0.0;
        double sliderMarginBottom = 0.0;
        double thumbLengthDIPs = 0.0;
        double axisCross = 0.0;
        std::uintptr_t thumbIdentity = 0;
        std::uintptr_t trackIdentity = 0;
        std::uintptr_t selectionBrushIdentity = 0;
        std::uint32_t selectionColorARGB = 0;
        unsigned renderedTickCount = 0;
        unsigned firstSideTickCount = 0;
        unsigned secondSideTickCount = 0;
        unsigned visibleSelectionCount = 0;
        WinUIVisualRect minimumLabel;
        WinUIVisualRect maximumLabel;
        WinUIVisualRect valueLabel;
        WinUIVisualRect selection;
        WinUIVisualRect nativeTrack;
        WinUIVisualRect nativeThumb;
        double axisStart = 0.0;
        double axisEnd = 0.0;
        double nativeThumbAxis = 0.0;
        double renderedTickAxisMinimum = 0.0;
        double renderedTickAxisMaximum = 0.0;
        WinUISelectionVisual selectionVisual =
            WinUISelectionVisual::Normal;
        WinUIActualTheme actualTheme = WinUIActualTheme::Default;
        bool selectionThemeBound = false;
        bool selectionHasSolidColor = false;
        bool selectionVisible = false;
        bool thumbNamedForOrientation = false;
        bool visualTreeCoherent = false;
        bool sameXamlRoot = false;
        bool vertical = false;
        bool axisReversed = false;
    };

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

    // Implementation-only deterministic seams. They drive the same state
    // machine and peer callback as keyboard, pointer, wheel and UIA input.
    bool WinUIApplyInputForTesting(WinUIInput input,
                                   int value,
                                   bool finishInteraction = true);
    bool WinUIApplyPointerInputForTesting(bool thumb,
                                          int value,
                                          bool valueBeforePress);
    bool WinUISetUnclassifiedPeerValueForTesting(int value);
    bool WinUIGetPeerStateForTesting(double *minimum,
                                     double *maximum,
                                     double *value,
                                     double *smallChange = nullptr,
                                     double *largeChange = nullptr,
                                     bool *vertical = nullptr,
                                     bool *reversed = nullptr) const;
    bool WinUIGetDecorationsForTesting(int *thumbLength,
                                       int *selectionStart,
                                       int *selectionEnd,
                                       unsigned *manualTickCount,
                                       bool *selectionVisible = nullptr,
                                       bool *minMaxLabelsVisible = nullptr,
                                       bool *valueLabelVisible = nullptr,
                                       bool *rightToLeft = nullptr,
                                       bool *rootRightToLeft = nullptr,
                                       bool *sliderRightToLeft = nullptr) const;
    bool WinUIHasTickForTesting(int tickPos) const;
    bool WinUIRefreshForScaleForTesting(double scale);
    bool WinUIGetHostScaleStateForTesting(
        double *scale,
        std::uintptr_t *xamlRootIdentity,
        std::uintptr_t *contentRootIdentity = nullptr) const;
    bool WinUIRefreshVisualStateForTesting();
    WinUIConvergenceFailure
    WinUIGetLastConvergenceFailureForTesting() const;
    bool WinUIDeliverThemeChangedForTesting();
    bool WinUIRetemplatePeerForTesting(
        WinUIRetemplateHookForTesting hook = nullptr,
        void *context = nullptr);
    bool WinUIGetVisualStateForTesting(WinUIVisualState *state) const;

protected:
    void DoSetTickFreq(int freq) override;
    wxSize DoGetBestSize() const override;
    bool MSWOnEffectiveLayoutDirectionChanged() override;

private:
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
    bool ConvergeVisualStateForTesting();

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
