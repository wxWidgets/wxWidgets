/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to WinUI peer test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_SLIDER_TEST_ACCESS_H
#define WX_WINUI_SLIDER_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/slider.h"

#if wxUSE_SLIDER
class WXDLLIMPEXP_CORE wxWinUISliderTestAccess final
{
public:
    using Input = wxSlider::WinUIInput;

    enum class SelectionVisual
    {
        Normal,
        Disabled,
        HighContrast,
        HighContrastDisabled
    };

    enum class ActualTheme
    {
        Default,
        Light,
        Dark
    };

    enum class ConvergenceFailure
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

    using RetemplateHook =
        void (*)(wxSlider *, void *);

    using VisualRect = wxSlider::WinUIVisualRect;

    struct VisualState
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
        VisualRect minimumLabel;
        VisualRect maximumLabel;
        VisualRect valueLabel;
        VisualRect selection;
        VisualRect nativeTrack;
        VisualRect nativeThumb;
        double axisStart = 0.0;
        double axisEnd = 0.0;
        double nativeThumbAxis = 0.0;
        double renderedTickAxisMinimum = 0.0;
        double renderedTickAxisMaximum = 0.0;
        SelectionVisual selectionVisual =
            SelectionVisual::Normal;
        ActualTheme actualTheme = ActualTheme::Default;
        bool selectionThemeBound = false;
        bool selectionHasSolidColor = false;
        bool selectionVisible = false;
        bool thumbNamedForOrientation = false;
        bool visualTreeCoherent = false;
        bool sameXamlRoot = false;
        bool vertical = false;
        bool axisReversed = false;
    };

    static bool ApplyInput(
        wxSlider& control,
        Input input,
        int value,
        bool finishInteraction = true);
    static bool ApplyPointerInput(
        wxSlider& control,
        bool thumb,
        int value,
        bool valueBeforePress);
    static bool SetUnclassifiedPeerValue(
        wxSlider& control,
        int value);
    static bool GetPeerState(
        const wxSlider& control,
        double *minimum,
        double *maximum,
        double *value,
        double *smallChange = nullptr,
        double *largeChange = nullptr,
        bool *vertical = nullptr,
        bool *reversed = nullptr);
    static bool GetDecorations(
        const wxSlider& control,
        int *thumbLength,
        int *selectionStart,
        int *selectionEnd,
        unsigned *manualTickCount,
        bool *selectionVisible = nullptr,
        bool *minMaxLabelsVisible = nullptr,
        bool *valueLabelVisible = nullptr,
        bool *rightToLeft = nullptr,
        bool *rootRightToLeft = nullptr,
        bool *sliderRightToLeft = nullptr);
    static bool HasTick(
        const wxSlider& control,
        int tickPos);
    static bool RefreshForScale(
        wxSlider& control,
        double scale);
    static bool GetHostScaleState(
        const wxSlider& control,
        double *scale,
        std::uintptr_t *xamlRootIdentity,
        std::uintptr_t *contentRootIdentity = nullptr);
    static bool RefreshVisualState(wxSlider& control);
    static ConvergenceFailure GetLastConvergenceFailure(const wxSlider& control);
    static bool DeliverThemeChanged(wxSlider& control);
    static bool RetemplatePeer(
        wxSlider& control,
        RetemplateHook hook = nullptr,
        void *context = nullptr);
    static bool GetVisualState(
        const wxSlider& control,
        VisualState *state);

private:
    static bool ConvergeVisualState(wxSlider& control);
};
#endif

#endif
