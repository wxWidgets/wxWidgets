/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/statbar.cpp
// Purpose:     wxWinUI wxStatusBar implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATUSBAR

#include "wx/statusbr.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "statusbar-test-access.h"
    #include "control-host-test-access.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/app.h"
#include "wx/dcclient.h"
#include "wx/display.h"
#include "wx/log.h"
#include "wx/math.h"
#include "wx/weakref.h"

#include "private.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace MUI = winrt::Microsoft::UI::Input;
namespace WF = winrt::Windows::Foundation;

namespace
{

std::atomic<std::uint64_t> gs_nextStatusBarLifetimeGeneration{0};
constexpr int wxWinUIStatusBarGripSizeDips = 18;

int wxWinUIStatusBarHeightFromDIP(double heightDIPs, const wxSize& dpi)
{
    if ( dpi.y <= 0 )
        return wxDefaultCoord;

    return wxRound(
        heightDIPs * dpi.y / wxDisplay::GetStdPPIValue());
}

class wxWinUIStatusBarCallbackState final
{
public:
    static constexpr unsigned MaxRebuildPassBudget = 16;

    explicit wxWinUIStatusBarCallbackState(wxStatusBar *owner)
        : m_owner(owner),
          m_lifetimeGeneration(
              gs_nextStatusBarLifetimeGeneration.fetch_add(
                  1, std::memory_order_acq_rel) + 1)
    {
    }

    std::uint64_t LifetimeGeneration() const
    {
        return m_lifetimeGeneration.load(std::memory_order_acquire);
    }

    wxStatusBar *GetOwner(std::uint64_t generation) const
    {
        if ( generation == 0 ||
             generation !=
                 m_lifetimeGeneration.load(std::memory_order_acquire) )
        {
            return nullptr;
        }
        return m_owner.load(std::memory_order_acquire);
    }

    std::uint64_t BumpModelRevision()
    {
        // Only a mutation entered outside the active projection starts a new
        // request. Reentrant writes must share one finite budget.
        if ( !m_rebuildActive.load(std::memory_order_acquire) )
        {
            m_rebuildPassesRemaining.store(
                MaxRebuildPassBudget, std::memory_order_release);
            m_rebuildQuarantined.store(false, std::memory_order_release);
            ClearRebuildBudgetWarning();
        }

        std::uint64_t revision =
            m_modelRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
        if ( revision == 0 )
        {
            revision =
                m_modelRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
        }
        return revision;
    }

    std::uint64_t ModelRevision() const
    {
        return m_modelRevision.load(std::memory_order_acquire);
    }

    bool TryBeginRebuild()
    {
        bool expected = false;
        if ( !m_rebuildActive.compare_exchange_strong(
                 expected, true, std::memory_order_acq_rel) )
        {
            m_rebuildRequested.store(true, std::memory_order_release);
            return false;
        }
        return true;
    }

    bool ConsumeRebuildRequest()
    {
        return m_rebuildRequested.exchange(
            false, std::memory_order_acq_rel);
    }

    void RequestRebuild()
    {
        m_rebuildRequested.store(true, std::memory_order_release);
    }

    void EndRebuild()
    {
        m_rebuildActive.store(false, std::memory_order_release);
    }

    bool TryScheduleDeferredRebuild()
    {
        bool expected = false;
        return m_deferredRebuildScheduled.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel);
    }

    bool ConsumeDeferredRebuild()
    {
        return m_deferredRebuildScheduled.exchange(
            false, std::memory_order_acq_rel);
    }

    void CancelDeferredRebuild()
    {
        m_deferredRebuildScheduled.store(
            false, std::memory_order_release);
    }

    bool HasDeferredRebuild() const
    {
        return m_deferredRebuildScheduled.load(
            std::memory_order_acquire);
    }

    bool TryConsumeRebuildPass()
    {
        unsigned remaining =
            m_rebuildPassesRemaining.load(std::memory_order_acquire);
        while ( remaining )
        {
            if ( m_rebuildPassesRemaining.compare_exchange_weak(
                     remaining, remaining - 1,
                     std::memory_order_acq_rel,
                     std::memory_order_acquire) )
            {
                return true;
            }
        }
        return false;
    }

    bool HasRebuildPassesRemaining() const
    {
        return m_rebuildPassesRemaining.load(
                   std::memory_order_acquire) != 0;
    }

    void QuarantineRebuild()
    {
        m_rebuildPassesRemaining.store(0, std::memory_order_release);
        m_rebuildRequested.store(false, std::memory_order_release);
        m_rebuildQuarantined.store(true, std::memory_order_release);
        CancelDeferredRebuild();
    }

    bool IsRebuildQuarantined() const
    {
        return m_rebuildQuarantined.load(std::memory_order_acquire);
    }

    bool MarkRebuildBudgetWarning()
    {
        return !m_rebuildBudgetWarning.exchange(
            true, std::memory_order_acq_rel);
    }

    void ClearRebuildBudgetWarning()
    {
        m_rebuildBudgetWarning.store(false, std::memory_order_release);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_lifetimeGeneration.fetch_add(1, std::memory_order_acq_rel);
        BumpModelRevision();
        QuarantineRebuild();
    }

private:
    std::atomic<wxStatusBar *> m_owner;
    std::atomic<std::uint64_t> m_lifetimeGeneration;
    std::atomic<std::uint64_t> m_modelRevision{1};
    std::atomic<bool> m_rebuildActive{false};
    std::atomic<bool> m_rebuildRequested{false};
    std::atomic<bool> m_deferredRebuildScheduled{false};
    std::atomic<bool> m_rebuildBudgetWarning{false};
    std::atomic<unsigned> m_rebuildPassesRemaining{
        MaxRebuildPassBudget
    };
    std::atomic<bool> m_rebuildQuarantined{false};
};

// C++/WinRT implementation types must remain derivable (make_self enforces
// this), even though this private element is never subclassed by wx code.
struct wxWinUIStatusBarGripGrid
    : MUXC::GridT<wxWinUIStatusBarGripGrid>
{
    bool SetResizeCursor(bool rightToLeft)
    {
        try
        {
            m_cursorShape =
                rightToLeft
                    ? MUI::InputSystemCursorShape::SizeNortheastSouthwest
                    : MUI::InputSystemCursorShape::SizeNorthwestSoutheast;
            ProtectedCursor(MUI::InputSystemCursor::Create(m_cursorShape));
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI StatusBar resize cursor", e);
            return false;
        }
    }

    MUI::InputSystemCursorShape GetCursorShape() const
    {
        return m_cursorShape;
    }

private:
    MUI::InputSystemCursorShape m_cursorShape =
        MUI::InputSystemCursorShape::Arrow;
};

struct wxWinUIStatusBarGripAction final
{
    std::weak_ptr<wxWinUIStatusBarCallbackState> callbackState;
    wxWeakRef<wxWindow> topLevel;
    std::uint64_t lifetimeGeneration = 0;
    unsigned long long peerGeneration = 0;
    HWND hwnd = nullptr;
    int nativeHitTest = HTNOWHERE;
    winrt::weak_ref<MUX::UIElement> grip;
    std::function<wxStatusBar *()> getCurrentOwner;
    std::function<bool (bool, bool, bool, const POINT&,
                        std::uint32_t, std::uint64_t)> invoke;
};

class wxWinUIStatusBarRebuildGuard final
{
public:
    explicit wxWinUIStatusBarRebuildGuard(
        std::shared_ptr<wxWinUIStatusBarCallbackState> state)
        : m_state(std::move(state)),
          m_acquired(m_state->TryBeginRebuild())
    {
    }

    ~wxWinUIStatusBarRebuildGuard()
    {
        if ( m_acquired )
            m_state->EndRebuild();
    }

    explicit operator bool() const { return m_acquired; }

private:
    std::shared_ptr<wxWinUIStatusBarCallbackState> m_state;
    bool m_acquired;
};

void wxWinUIForgetStatusBarManagedToolTips(
    const std::vector<MUXC::TextBlock>& fields)
{
    // Identity-based retirement deliberately preserves a direct application
    // replacement installed after wx registered its own tooltip.
    for ( const MUXC::TextBlock& field : fields )
        (void)wxWinUIForgetManagedToolTip(field);
}

class wxWinUIStatusBarCandidateToolTipGuard final
{
public:
    wxWinUIStatusBarCandidateToolTipGuard(
        std::vector<MUXC::TextBlock> *fields)
        : m_fields(fields)
    {
    }

    ~wxWinUIStatusBarCandidateToolTipGuard()
    {
        if ( m_active && m_fields )
            wxWinUIForgetStatusBarManagedToolTips(*m_fields);
    }

    void Release() { m_active = false; }

private:
    std::vector<MUXC::TextBlock> *m_fields;
    bool m_active = true;
};

struct wxWinUIStatusBarFieldModel final
{
    double columnValue = 1.0;
    MUX::GridUnitType columnUnit = MUX::GridUnitType::Star;
    wxString renderedText;
    wxString fullText;
    bool ellipsized = false;
    int trimming = static_cast<int>(MUX::TextTrimming::None);
    int style = wxSB_NORMAL;
};

} // namespace

class wxWinUIStatusBarImpl
{
public:
#ifdef WXWINUI_TEST_SUPPORT
    using ReentryHook = void (*)(wxStatusBar*, void*);
#endif

    ~wxWinUIStatusBarImpl()
    {
        Close();
    }

    void Close()
    {
        if ( closed )
            return;
        closed = true;

        if ( callbackState )
            callbackState->Invalidate();

        if ( ++rebuildGeneration == 0 )
            ++rebuildGeneration;
        wxWinUIForgetStatusBarManagedToolTips(fields);
        host.Close();
        fields.clear();
        columnValues.clear();
        columnUnits.clear();
        fieldStyles.clear();
        fieldBorderElementCounts.clear();
        root = nullptr;
        automationRoot = nullptr;
        overlay = nullptr;
        grid = nullptr;
        sizeGrip = nullptr;
        sizeGripImpl = nullptr;
        sizeGripAction.reset();
        hasSizeGrip = false;
#ifdef WXWINUI_TEST_SUPPORT
        nextReentryHook = nullptr;
        nextReentryContext = nullptr;
#endif
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState;
    MUXC::ContentControl root{ nullptr };
    MUXC::Grid automationRoot{ nullptr };
    MUXC::Grid overlay{ nullptr };
    MUXC::Grid grid{ nullptr };
    MUXC::Grid sizeGrip{ nullptr };
    winrt::com_ptr<wxWinUIStatusBarGripGrid> sizeGripImpl;
    std::shared_ptr<wxWinUIStatusBarGripAction> sizeGripAction;
    std::vector<MUXC::TextBlock> fields;
    std::vector<double> columnValues;
    std::vector<int> columnUnits;
    std::vector<int> fieldStyles;
    std::vector<int> fieldBorderElementCounts;
    bool usesThemeBorders = false;
    bool hasSizeGrip = false;
#ifdef WXWINUI_TEST_SUPPORT
    int topLevelMaximizedForTesting = -1;
#endif
    unsigned long long minHeightGeneration = 0;
    unsigned long long dpiGeneration = 0;
    unsigned long long rebuildGeneration = 0;
    unsigned long long peerGeneration = 0;
    unsigned long long publishedRevision = 0;
#ifdef WXWINUI_TEST_SUPPORT
    wxWinUIStatusBarReentryPointForTesting nextReentryPoint =
        wxWinUIStatusBarReentryPointForTesting::RebuildLoaded;
    ReentryHook nextReentryHook = nullptr;
    void *nextReentryContext = nullptr;
#endif
    bool closed = false;
};

namespace
{

MUXC::Border wxWinUIStatusThemeBorder(
    wxWinUIThemeBrushProperty property,
    const wxString& preferred = "ControlStrokeColorDefaultBrush")
{
    MUXC::Border border =
        wxWinUICreateThemeBrushBorder(preferred, property);
    if ( !border && preferred != "CardStrokeColorDefaultBrush" )
    {
        border = wxWinUICreateThemeBrushBorder(
            "CardStrokeColorDefaultBrush", property);
    }
    return border;
}

bool wxWinUIAppendStatusEdge(MUXC::Grid const& cell,
                             bool vertical,
                             MUX::HorizontalAlignment horizontal,
                             MUX::VerticalAlignment alignment,
                             const wxString& resource)
{
    MUXC::Border edge = wxWinUIStatusThemeBorder(
        wxWinUIThemeBrushProperty::Background, resource);
    if ( !edge )
        return false;

    edge.IsHitTestVisible(false);
    if ( vertical )
    {
        edge.Width(1.0);
        edge.HorizontalAlignment(horizontal);
        edge.VerticalAlignment(MUX::VerticalAlignment::Stretch);
    }
    else
    {
        edge.Height(1.0);
        edge.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
        edge.VerticalAlignment(alignment);
    }
    cell.Children().Append(edge);
    return true;
}

} // namespace

// Note: wxIMPLEMENT_DYNAMIC_CLASS(wxStatusBar, wxWindow) is already provided by
// the common code in src/common/statbar.cpp, so it must not be repeated here.

wxStatusBar::wxStatusBar()
{
}

wxStatusBar::wxStatusBar(wxWindow *parent,
                         wxWindowID id,
                         long style,
                         const wxString& name)
{
    Create(parent, id, style, name);
}

wxStatusBar::~wxStatusBar()
{
    Unbind(wxEVT_SIZE, &wxStatusBar::OnSize, this);
    Unbind(wxEVT_DPI_CHANGED, &wxStatusBar::OnDPIChanged, this);
    if ( m_winui )
        m_winui->Close();
}

bool wxStatusBar::Create(wxWindow *parent,
                         wxWindowID id,
                         long style,
                         const wxString& name)
{
    if ( !wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize,
                            style | wxFULL_REPAINT_ON_RESIZE,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    m_borderX = FromDIP(2);
    m_borderY = FromDIP(2);

    m_winui.reset(new wxWinUIStatusBarImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIStatusBarCallbackState>(this);
    wxWinUIStatusBarImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        createImpl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->LifetimeGeneration();
    if ( !createImpl->host.Initialize(this) )
        return false;

    wxStatusBar *liveOwner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    liveOwner->Bind(wxEVT_SIZE, &wxStatusBar::OnSize, liveOwner);
    liveOwner->Bind(
        wxEVT_DPI_CHANGED, &wxStatusBar::OnDPIChanged, liveOwner);

    // SetFieldsCount() dispatches through our SetStatusWidths(), which performs
    // the initial transactional install. A failed install leaves root null.
    liveOwner->SetFieldsCount(1);
    liveOwner = callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != callbackState ||
         !createImpl->root )
    {
        return false;
    }

    liveOwner->InvalidateBestSize();
    if ( liveOwner->m_minHeightDIPs >= 0.0 )
    {
        const double minHeightDIPs = liveOwner->m_minHeightDIPs;
        liveOwner->ApplyMinHeight(
            minHeightDIPs, liveOwner->GetMinHeightPixels());
        wxStatusBar * const createdOwner =
            callbackState->GetOwner(lifetimeGeneration);
        return createdOwner && createdOwner->m_winui &&
               createdOwner->m_winui.get() == createImpl &&
               createdOwner->m_winui->callbackState == callbackState;
    }
    return true;
}

wxArrayInt wxStatusBar::GetAbsWidths() const
{
    int width =
        GetClientSize().x - 2 * m_borderX - GetSizeGripReservedWidth();
    if ( width < 0 )
        width = 0;

    return CalculateAbsWidths(width);
}

int wxStatusBar::GetSizeGripReservedWidth() const
{
    if ( !HasFlag(wxSTB_SIZEGRIP) || GetFieldsCount() == 0 )
        return 0;

    wxWindow * const topLevel =
        wxGetTopLevelParent(const_cast<wxStatusBar *>(this));
    if ( !topLevel || !topLevel->HasFlag(wxRESIZE_BORDER) )
        return 0;

    const HWND hwnd = GetHwndOf(topLevel);
    if ( !hwnd || !::IsWindow(hwnd) )
        return 0;

    const bool maximized =
#ifdef WXWINUI_TEST_SUPPORT
        m_winui && m_winui->topLevelMaximizedForTesting >= 0
            ? m_winui->topLevelMaximizedForTesting != 0
            :
#endif
                ::IsZoomed(hwnd) != FALSE;
    return maximized ? 0 : FromDIP(wxWinUIStatusBarGripSizeDips);
}

wxString wxStatusBar::GetDisplayedStatusText(int field,
                                             bool *ellipsized,
                                             int *trimming) const
{
    const wxString full = GetStatusText(field);
    wxString rendered = full;
    bool isEllipsized = false;
    MUX::TextTrimming xamlTrimming = MUX::TextTrimming::None;

    wxRect rect;
    if ( GetFieldRect(field, rect) && !full.empty() )
    {
        const int available =
            std::max(0, rect.width - FromDIP(16));
        wxClientDC dc(const_cast<wxStatusBar *>(this));
        if ( GetFont().IsOk() )
            dc.SetFont(GetFont());

        isEllipsized = dc.GetTextExtent(full).x > available;

        wxEllipsizeMode mode = wxELLIPSIZE_NONE;
        if ( HasFlag(wxSTB_ELLIPSIZE_START) )
            mode = wxELLIPSIZE_START;
        else if ( HasFlag(wxSTB_ELLIPSIZE_MIDDLE) )
            mode = wxELLIPSIZE_MIDDLE;
        else if ( HasFlag(wxSTB_ELLIPSIZE_END) )
            mode = wxELLIPSIZE_END;

        if ( mode == wxELLIPSIZE_START || mode == wxELLIPSIZE_MIDDLE )
        {
            rendered = wxControl::Ellipsize(
                full, dc, mode, available,
                wxELLIPSIZE_FLAGS_EXPAND_TABS);
            isEllipsized = rendered != full;
        }
        else if ( mode == wxELLIPSIZE_END )
        {
            // WinUI implements trailing ellipsis natively. Keeping the full
            // Text value preserves the accessible name and tooltip.
            xamlTrimming = MUX::TextTrimming::CharacterEllipsis;
        }
        else
        {
            // A status bar without an ellipsis style truncates at its field
            // boundary; Clip is the honest XAML equivalent.
            xamlTrimming = MUX::TextTrimming::Clip;
        }
    }

    if ( ellipsized )
        *ellipsized = isEllipsized;
    if ( trimming )
        *trimming = static_cast<int>(xamlTrimming);
    return rendered;
}

bool wxStatusBar::RebuildContent()
{
    if ( !m_winui || !m_winui->callbackState ||
         !m_winui->host.IsOk() )
        return false;

    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->LifetimeGeneration();
    const wxWinUIStatusBarRebuildGuard rebuildGuard(callbackState);
    if ( !rebuildGuard )
    {
        // The active transaction will observe both the request bit and the
        // model revision changed by the nested caller, then rebuild once more
        // from the newest model without recursively entering SetContent().
        return true;
    }

    constexpr unsigned MaxSynchronousRebuildAttempts = 8;
    unsigned rebuildAttempt = 0;
    const auto quarantineLatestRevision =
        [&]() -> bool
        {
            callbackState->QuarantineRebuild();
            if ( callbackState->MarkRebuildBudgetWarning() )
            {
                wxLogWarning(
                    "wxStatusBar WinUI content kept changing during "
                    "realization; quarantining this request until the next "
                    "external model mutation.");
            }
            return false;
        };
    const auto deferLatestRevision =
        [&]() -> bool
        {
            if ( !callbackState->HasRebuildPassesRemaining() )
                return quarantineLatestRevision();

            callbackState->RequestRebuild();
            if ( callbackState->HasDeferredRebuild() )
                return true;

            if ( !wxTheApp )
                return quarantineLatestRevision();

            // A custom log target can yield the event loop. Log before the
            // deferred ticket becomes consumable.
            if ( callbackState->MarkRebuildBudgetWarning() )
            {
                wxLogWarning(
                    "wxStatusBar WinUI content kept changing during "
                    "realization; using its one bounded asynchronous replay.");
            }

            if ( !callbackState->TryScheduleDeferredRebuild() )
            {
                return callbackState->HasDeferredRebuild()
                    ? true
                    : quarantineLatestRevision();
            }

            // Posting is deliberately the final operation before rebuildGuard
            // leaves the active transaction.
            wxTheApp->CallAfter(
                [callbackState, lifetimeGeneration, impl]()
                {
                    if ( !callbackState->ConsumeDeferredRebuild() )
                        return;

                    wxStatusBar * const owner =
                        callbackState->GetOwner(lifetimeGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }
                    owner->RebuildContent();
                });
            return true;
        };

    for ( ;; )
    {
        if ( ++rebuildAttempt > MaxSynchronousRebuildAttempts )
        {
            wxStatusBar * const owner =
                callbackState->GetOwner(lifetimeGeneration);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState )
            {
                return false;
            }
            return deferLatestRevision();
        }

        if ( !callbackState->TryConsumeRebuildPass() )
            return quarantineLatestRevision();

        callbackState->ConsumeRebuildRequest();
        wxStatusBar *iterationOwner =
            callbackState->GetOwner(lifetimeGeneration);
        if ( !iterationOwner || !iterationOwner->m_winui ||
             iterationOwner->m_winui.get() != impl ||
             iterationOwner->m_winui->callbackState != callbackState )
        {
            return false;
        }

        const std::uint64_t modelRevision =
            callbackState->ModelRevision();
        unsigned long long generation = ++impl->rebuildGeneration;
        if ( generation == 0 )
            generation = ++impl->rebuildGeneration;

        // This resolver is the only bridge back from detached XAML writes to
        // the wx object. It intentionally validates lifetime and exact
        // implementation identity without dereferencing the captured raw
        // implementation pointer first.
        const auto resolveCandidateOwner =
            [&]() -> wxStatusBar *
            {
                wxStatusBar * const owner =
                    callbackState->GetOwner(lifetimeGeneration);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return nullptr;
                }
                return owner;
            };

        // Snapshot the complete wx model before creating any XAML object.
        // Detached dependency-property writes below are then allowed to
        // re-enter arbitrary application code without any subsequent access
        // through a possibly destroyed owner. The transaction revalidates
        // this revision before attaching or publishing the candidate.
        const auto validateSnapshotOwner =
            [&]() -> wxStatusBar *
            {
                wxStatusBar * const owner = resolveCandidateOwner();
                return owner &&
                       callbackState->ModelRevision() == modelRevision
                    ? owner
                    : nullptr;
            };

        const int count = iterationOwner->GetFieldsCount();
        iterationOwner = validateSnapshotOwner();
        if ( !iterationOwner )
        {
            if ( resolveCandidateOwner() )
                continue;
            return false;
        }

        const double observedScale =
            iterationOwner->GetDPIScaleFactor();
        iterationOwner = validateSnapshotOwner();
        if ( !iterationOwner )
        {
            if ( resolveCandidateOwner() )
                continue;
            return false;
        }
        const double scale = observedScale > 0.0 ? observedScale : 1.0;
        const int borderX = iterationOwner->m_borderX;
        const int borderY = iterationOwner->m_borderY;
        const bool sameWidthForAllPanes =
            iterationOwner->m_bSameWidthForAllPanes;
        const bool showTips =
            iterationOwner->HasFlag(wxSTB_SHOW_TIPS);
        const bool hasFont = iterationOwner->m_hasFont;
        const bool hasForeground = iterationOwner->m_hasFgCol;
        const bool hasBackground = iterationOwner->m_hasBgCol;
        wxFont font;
        if ( hasFont )
        {
            font = iterationOwner->GetFont();
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( resolveCandidateOwner() )
                    continue;
                return false;
            }
        }
        wxColour foreground;
        if ( hasForeground )
        {
            foreground = iterationOwner->GetForegroundColour();
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( resolveCandidateOwner() )
                    continue;
                return false;
            }
        }
        wxColour background;
        if ( hasBackground )
        {
            background = iterationOwner->GetBackgroundColour();
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( resolveCandidateOwner() )
                    continue;
                return false;
            }
        }
        const bool rightToLeft =
            iterationOwner->GetLayoutDirection() ==
                wxLayout_RightToLeft;
        iterationOwner = validateSnapshotOwner();
        if ( !iterationOwner )
        {
            if ( resolveCandidateOwner() )
                continue;
            return false;
        }

        const int sizeGripReservedWidth =
            iterationOwner->GetSizeGripReservedWidth();
        iterationOwner = validateSnapshotOwner();
        if ( !iterationOwner )
        {
            if ( resolveCandidateOwner() )
                continue;
            return false;
        }
        const bool projectSizeGrip = sizeGripReservedWidth > 0;
        const double sizeGripReservationDips =
            sizeGripReservedWidth / scale;

        wxWeakRef<wxWindow> sizeGripTopLevel;
        HWND sizeGripHwnd = nullptr;
        if ( projectSizeGrip )
        {
            wxWindow * const topLevel =
                wxGetTopLevelParent(iterationOwner);
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( resolveCandidateOwner() )
                    continue;
                return false;
            }
            if ( !topLevel || !topLevel->HasFlag(wxRESIZE_BORDER) )
                return false;

            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( resolveCandidateOwner() )
                    continue;
                return false;
            }
            sizeGripTopLevel = wxWeakRef<wxWindow>(topLevel);
            sizeGripHwnd = GetHwndOf(topLevel);
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( resolveCandidateOwner() )
                    continue;
                return false;
            }
            if ( !sizeGripHwnd || !::IsWindow(sizeGripHwnd) )
                return false;
        }

        const wxString localizedStatusBar = _("status bar");
        const wxString localizedResize = _("Resize");
        const wxString localizedResizeGrip = _("resize grip");
        std::vector<wxWinUIStatusBarFieldModel> fieldModels;
        fieldModels.reserve(count);
        bool snapshotSuperseded = false;
        for ( int i = 0; i < count; ++i )
        {
            wxWinUIStatusBarFieldModel field;
            if ( !sameWidthForAllPanes )
            {
                const int paneWidth =
                    iterationOwner->GetField(i).GetWidth();
                if ( paneWidth >= 0 )
                {
                    field.columnValue = paneWidth / scale;
                    field.columnUnit = MUX::GridUnitType::Pixel;
                }
                else
                {
                    field.columnValue = std::max(1, -paneWidth);
                }
            }
            field.renderedText =
                iterationOwner->GetDisplayedStatusText(
                    i, &field.ellipsized, &field.trimming);
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( !resolveCandidateOwner() )
                    return false;
                snapshotSuperseded = true;
                break;
            }
            field.fullText = iterationOwner->GetStatusText(i);
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( !resolveCandidateOwner() )
                    return false;
                snapshotSuperseded = true;
                break;
            }
            field.style = iterationOwner->GetStatusStyle(i);
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( !resolveCandidateOwner() )
                    return false;
                snapshotSuperseded = true;
                break;
            }
            iterationOwner->SetEllipsizedFlag(i, field.ellipsized);
            iterationOwner = validateSnapshotOwner();
            if ( !iterationOwner )
            {
                if ( !resolveCandidateOwner() )
                    return false;
                snapshotSuperseded = true;
                break;
            }
            fieldModels.push_back(std::move(field));
        }
        if ( snapshotSuperseded )
            continue;

        wxStatusBar *snapshotOwner =
            callbackState->GetOwner(lifetimeGeneration);
        if ( !snapshotOwner || !snapshotOwner->m_winui ||
             snapshotOwner->m_winui.get() != impl ||
             snapshotOwner->m_winui->callbackState != callbackState )
        {
            return false;
        }
        if ( callbackState->ModelRevision() != modelRevision ||
             callbackState->ConsumeRebuildRequest() )
        {
            continue;
        }

        try
        {
            MUXC::ContentControl root;
            root.IsTabStop(false);
            root.HorizontalContentAlignment(MUX::HorizontalAlignment::Stretch);
            root.VerticalContentAlignment(MUX::VerticalAlignment::Stretch);
            MUXC::Grid automationRoot = wxWinUICreateAccessibleGrid(
                MUXAP::AutomationControlType::StatusBar, "wxStatusBar");
            automationRoot.HorizontalAlignment(
                MUX::HorizontalAlignment::Stretch);
            automationRoot.VerticalAlignment(
                MUX::VerticalAlignment::Stretch);
            automationRoot.Children().Append(root);

            MUX::Thickness padding{};
            padding.Left = borderX / scale;
            padding.Right = borderX / scale;
            padding.Top = borderY / scale;
            padding.Bottom = borderY / scale;
            root.Padding(padding);

            MUXC::Grid overlay;
            overlay.VerticalAlignment(MUX::VerticalAlignment::Stretch);
            overlay.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
            // Keep the overlay in physical coordinates: Left is always the
            // bottom-left corner and Right the bottom-right corner.
            overlay.FlowDirection(MUX::FlowDirection::LeftToRight);

            MUXC::Grid grid;
            grid.VerticalAlignment(MUX::VerticalAlignment::Stretch);
            grid.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
            // Field rectangles use wx logical coordinates, as they do in
            // wxMSW. Let XAML mirror the columns physically so column zero
            // and an AddFieldControl(0) child remain in the same RTL pane.
            grid.FlowDirection(
                rightToLeft ? MUX::FlowDirection::RightToLeft
                            : MUX::FlowDirection::LeftToRight);
            MUX::Thickness gridMargin{};
            if ( rightToLeft )
                gridMargin.Left = sizeGripReservationDips;
            else
                gridMargin.Right = sizeGripReservationDips;
            grid.Margin(gridMargin);
            overlay.Children().Append(grid);

            std::vector<MUXC::TextBlock> fields;
            std::vector<MUXC::TextBlock> managedToolTipFields;
            wxWinUIStatusBarCandidateToolTipGuard candidateToolTips(
                &managedToolTipFields);
            std::vector<double> columnValues;
            std::vector<int> columnUnits;
            std::vector<int> fieldStyles;
            std::vector<int> fieldBorderElementCounts;
            fields.reserve(count);
            managedToolTipFields.reserve(count);
            columnValues.reserve(count);
            columnUnits.reserve(count);
            fieldStyles.reserve(count);
            fieldBorderElementCounts.reserve(count);
            bool hasThemeBorder = false;
            bool themeBordersComplete = true;
            bool candidateStale = false;

            for ( int i = 0; i < count; ++i )
            {
                const wxWinUIStatusBarFieldModel& fieldModel =
                    fieldModels[i];
                MUXC::ColumnDefinition column;
                column.Width(
                    MUX::GridLengthHelper::FromValueAndType(
                        fieldModel.columnValue, fieldModel.columnUnit));
                grid.ColumnDefinitions().Append(column);
                columnValues.push_back(fieldModel.columnValue);
                columnUnits.push_back(
                    static_cast<int>(fieldModel.columnUnit));

                MUXC::Grid cell;
                cell.VerticalAlignment(MUX::VerticalAlignment::Stretch);
                cell.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);

                MUXC::TextBlock text;
                text.VerticalAlignment(MUX::VerticalAlignment::Center);
                text.FlowDirection(
                    rightToLeft ? MUX::FlowDirection::RightToLeft
                                : MUX::FlowDirection::LeftToRight);
                text.TextWrapping(MUX::TextWrapping::NoWrap);
                text.TextTrimming(
                    static_cast<MUX::TextTrimming>(fieldModel.trimming));
                MUX::Thickness margin{};
                margin.Left = 8.0;
                margin.Right = 8.0;
                text.Margin(margin);
                text.Text(wxWinUIToHString(fieldModel.renderedText));

                wxStatusBar *liveCandidateOwner =
                    resolveCandidateOwner();
                if ( !liveCandidateOwner )
                    return false;
                if ( callbackState->ModelRevision() != modelRevision )
                {
                    candidateStale = true;
                    break;
                }

#ifdef WXWINUI_TEST_SUPPORT
                if ( i == 0 &&
                     impl->nextReentryPoint ==
                         wxWinUIStatusBarReentryPointForTesting::TextValue &&
                     impl->nextReentryHook )
                {
                    const wxWinUIStatusBarImpl::ReentryHook hook =
                        impl->nextReentryHook;
                    void * const hookContext = impl->nextReentryContext;
                    impl->nextReentryHook = nullptr;
                    impl->nextReentryContext = nullptr;
                    hook(liveCandidateOwner, hookContext);

                    liveCandidateOwner = resolveCandidateOwner();
                    if ( !liveCandidateOwner )
                        return false;
                    if ( callbackState->ModelRevision() != modelRevision )
                    {
                        candidateStale = true;
                        break;
                    }
                }
#endif // WXWINUI_TEST_SUPPORT

                // Status text is data, not a mnemonic-bearing control label:
                // preserve literal ampersands in its accessible name.
                MUXA::AutomationProperties::SetName(
                    text, wxWinUIToHString(fieldModel.fullText));
                liveCandidateOwner = resolveCandidateOwner();
                if ( !liveCandidateOwner )
                    return false;
                if ( callbackState->ModelRevision() != modelRevision )
                {
                    candidateStale = true;
                    break;
                }

#if wxUSE_TOOLTIPS
                if ( showTips && fieldModel.ellipsized )
                {
                    // Track the exact identity before registration: any failure
                    // or re-entry from the attached-property setter must still
                    // release the managed-tooltip registry entry.
                    managedToolTipFields.push_back(text);
                    if ( !wxWinUISetToolTip(
                             text, fieldModel.fullText,
                             liveCandidateOwner) )
                        return false;
                    liveCandidateOwner = resolveCandidateOwner();
                    if ( !liveCandidateOwner )
                        return false;
                    if ( callbackState->ModelRevision() != modelRevision )
                    {
                        candidateStale = true;
                        break;
                    }
                }
#endif

                wxWinUIApplyFont(text, font);
                liveCandidateOwner = resolveCandidateOwner();
                if ( !liveCandidateOwner )
                    return false;
                if ( callbackState->ModelRevision() != modelRevision )
                {
                    candidateStale = true;
                    break;
                }
                wxWinUIApplyForeground(text, foreground);
                liveCandidateOwner = resolveCandidateOwner();
                if ( !liveCandidateOwner )
                    return false;
                if ( callbackState->ModelRevision() != modelRevision )
                {
                    candidateStale = true;
                    break;
                }
                cell.Children().Append(text);

                const int paneStyle = fieldModel.style;
                fieldStyles.push_back(paneStyle);
                int borderElementCount = 0;
                if ( paneStyle == wxSB_NORMAL )
                {
                    MUXC::Border outline = wxWinUIStatusThemeBorder(
                        wxWinUIThemeBrushProperty::BorderBrush);
                    if ( outline )
                    {
                        hasThemeBorder = true;
                        MUX::Thickness thickness{};
                        thickness.Left = 1.0;
                        thickness.Top = 1.0;
                        thickness.Right = 1.0;
                        thickness.Bottom = 1.0;
                        outline.BorderThickness(thickness);
                        outline.IsHitTestVisible(false);
                        cell.Children().InsertAt(0, outline);
                        borderElementCount = 1;
                    }
                    else
                    {
                        themeBordersComplete = false;
                    }
                }
                else if ( paneStyle == wxSB_RAISED ||
                          paneStyle == wxSB_SUNKEN )
                {
                    const wxString first =
                        paneStyle == wxSB_RAISED
                            ? "ControlStrokeColorDefaultBrush"
                            : "ControlStrokeColorSecondaryBrush";
                    const wxString second =
                        paneStyle == wxSB_RAISED
                            ? "ControlStrokeColorSecondaryBrush"
                            : "ControlStrokeColorDefaultBrush";
                    hasThemeBorder = true;
                    const auto appendEdge =
                        [&](bool vertical,
                            MUX::HorizontalAlignment horizontal,
                            MUX::VerticalAlignment verticalAlignment,
                            const wxString& resource)
                        {
                            const bool appended = wxWinUIAppendStatusEdge(
                                cell, vertical, horizontal, verticalAlignment,
                                resource);
                            if ( appended )
                                ++borderElementCount;
                            themeBordersComplete &= appended;
                        };
                    appendEdge(
                        true, MUX::HorizontalAlignment::Left,
                        MUX::VerticalAlignment::Stretch, first);
                    appendEdge(
                        false, MUX::HorizontalAlignment::Stretch,
                        MUX::VerticalAlignment::Top, first);
                    appendEdge(
                        true, MUX::HorizontalAlignment::Right,
                        MUX::VerticalAlignment::Stretch, second);
                    appendEdge(
                        false, MUX::HorizontalAlignment::Stretch,
                        MUX::VerticalAlignment::Bottom, second);
                }
                // wxSB_FLAT intentionally adds no border at all.
                fieldBorderElementCounts.push_back(borderElementCount);

                MUXC::Grid::SetColumn(cell, i);
                grid.Children().Append(cell);
                fields.push_back(text);

                // wxStatusBarBase::OnSize() uses the matching logical field
                // rect for an AddFieldControl() child; wxMSW mirrors that HWND
                // physically in RTL just as this grid mirrors column i. WinUI
                // children already own independent shared-island slots, so
                // duplicating them here would create a second visual and peer.
            }

            if ( candidateStale )
                continue;

            MUXC::Grid sizeGrip{ nullptr };
            winrt::com_ptr<wxWinUIStatusBarGripGrid> sizeGripImpl;
            std::shared_ptr<wxWinUIStatusBarGripAction> sizeGripAction;
            bool hasSizeGrip = false;
            if ( projectSizeGrip && count > 0 )
            {
                sizeGripImpl =
                    winrt::make_self<wxWinUIStatusBarGripGrid>();
                sizeGrip = sizeGripImpl.as<MUXC::Grid>();
                if ( !sizeGripImpl->SetResizeCursor(rightToLeft) )
                    return false;

                sizeGrip.Width(wxWinUIStatusBarGripSizeDips);
                sizeGrip.Height(wxWinUIStatusBarGripSizeDips);
                sizeGrip.HorizontalAlignment(
                    rightToLeft ? MUX::HorizontalAlignment::Left
                                : MUX::HorizontalAlignment::Right);
                sizeGrip.VerticalAlignment(
                    MUX::VerticalAlignment::Bottom);
                sizeGrip.Background(MUXM::SolidColorBrush(
                    winrt::Windows::UI::Color{0, 0, 0, 0}));
                MUXA::AutomationProperties::SetName(
                    sizeGrip, wxWinUIToHString(localizedResize));
                MUXA::AutomationProperties::SetLocalizedControlType(
                    sizeGrip, wxWinUIToHString(localizedResizeGrip));
                MUXA::AutomationProperties::SetAutomationControlType(
                    sizeGrip, MUXAP::AutomationControlType::Thumb);

                MUXC::TextBlock glyph;
                glyph.Text(
                    winrt::hstring(
                        rightToLeft ? L"\u22F1" : L"\u22F0"));
                glyph.FontSize(13.0);
                glyph.Opacity(0.65);
                glyph.IsHitTestVisible(false);
                glyph.HorizontalAlignment(
                    MUX::HorizontalAlignment::Center);
                glyph.VerticalAlignment(
                    MUX::VerticalAlignment::Center);
                sizeGrip.Children().Append(glyph);

                sizeGripAction =
                    std::make_shared<wxWinUIStatusBarGripAction>();
                sizeGripAction->callbackState = callbackState;
                sizeGripAction->topLevel = sizeGripTopLevel;
                sizeGripAction->lifetimeGeneration =
                    lifetimeGeneration;
                sizeGripAction->peerGeneration = generation;
                sizeGripAction->hwnd = sizeGripHwnd;
                sizeGripAction->nativeHitTest =
                    rightToLeft ? HTBOTTOMLEFT : HTBOTTOMRIGHT;
                sizeGripAction->grip =
                    winrt::make_weak(sizeGrip.as<MUX::UIElement>());

                const std::weak_ptr<wxWinUIStatusBarGripAction>
                    weakSizeGripAction(sizeGripAction);
                sizeGripAction->getCurrentOwner =
                    [weakSizeGripAction]() -> wxStatusBar *
                    {
                        const std::shared_ptr<
                            wxWinUIStatusBarGripAction> action =
                                weakSizeGripAction.lock();
                        if ( !action )
                            return nullptr;

                        const std::shared_ptr<
                            wxWinUIStatusBarCallbackState> state =
                                action->callbackState.lock();
                        if ( !state )
                            return nullptr;

                        wxStatusBar * const owner =
                            state->GetOwner(
                                action->lifetimeGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != state ||
                             owner->m_winui->peerGeneration !=
                                 action->peerGeneration ||
                             !owner->m_winui->hasSizeGrip ||
                             !owner->HasFlag(wxSTB_SIZEGRIP) ||
                             owner->m_winui->sizeGripAction != action )
                        {
                            return nullptr;
                        }

                        wxWindow * const topLevel =
                            action->topLevel.get();
                        if ( !topLevel ||
                             wxGetTopLevelParent(owner) != topLevel ||
                             !topLevel->HasFlag(wxRESIZE_BORDER) )
                        {
                            return nullptr;
                        }

                        const HWND hwnd = GetHwndOf(topLevel);
                        if ( !hwnd || hwnd != action->hwnd ||
                             !::IsWindow(hwnd) ||
                             owner->GetSizeGripReservedWidth() == 0 ||
                             action->nativeHitTest !=
                                (owner->GetLayoutDirection() ==
                                     wxLayout_RightToLeft
                                    ? HTBOTTOMLEFT : HTBOTTOMRIGHT) )
                        {
                            return nullptr;
                        }
                        return owner;
                    };

                sizeGripAction->invoke =
                    [weakSizeGripAction](
                        bool isMouse,
                        bool isPrimary,
                        bool isLeftButtonPressed,
                        const POINT& point,
                        std::uint32_t pointerId,
                        std::uint64_t timestamp) -> bool
                    {
                        if ( !isMouse || !isPrimary || !isLeftButtonPressed )
                            return false;
                        const auto action = weakSizeGripAction.lock();
                        wxStatusBar *owner = action && action->getCurrentOwner
                            ? action->getCurrentOwner() : nullptr;
                        if ( !owner )
                            return false;

                        wxWinUITopLevelHost * const host =
                            wxWinUITopLevelHost::FindSlotOwner(owner);
                        const auto grip = action->grip.get();
                        if ( !host || !grip )
                            return false;
                        wxWinUIPointerSample sample;
                        sample.kind = wxWinUIInputKind::Press;
                        sample.button = wxWinUIInputButton::Left;
                        sample.buttonMask = MK_LBUTTON;
                        sample.pointerId = pointerId;
                        sample.timestamp = timestamp;
                        sample.screenX = point.x;
                        sample.screenY = point.y;
                        return host->RequestNativeResize(
                            owner, grip, action->nativeHitTest, sample,
                            [weakSizeGripAction]()
                            {
                                const auto current = weakSizeGripAction.lock();
                                return current && current->getCurrentOwner &&
                                       current->getCurrentOwner() != nullptr;
                            });
                    };

                sizeGrip.PointerPressed(
                    [weakSizeGripAction](
                        const WF::IInspectable& sender,
                        const MUXI::PointerRoutedEventArgs& event)
                    {
                        try
                        {
                            const auto action =
                                weakSizeGripAction.lock();
                            const auto element =
                                sender.try_as<MUX::UIElement>();
                            if ( !action || !element )
                                return;

                            const auto point =
                                event.GetCurrentPoint(element);
                            const auto properties = point.Properties();
                            if ( point.PointerDeviceType() !=
                                     MUI::PointerDeviceType::Mouse ||
                                 !properties.IsPrimary() ||
                                 !properties.IsLeftButtonPressed() )
                            {
                                return;
                            }

                            POINT screenPoint{};
                            if ( !::GetCursorPos(&screenPoint) )
                                return;

                            if ( action->invoke &&
                                 action->invoke(
                                     true, true, true, screenPoint,
                                     point.PointerId(), point.Timestamp() / 1000) )
                            {
                                event.Handled(true);
                            }
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "WinUI StatusBar resize input", e);
                        }
                    });

                // This is a sibling overlay, never a child of the last field
                // column. Very narrow or zero-width last panes therefore
                // cannot clip or move the native resize affordance.
                overlay.Children().Append(sizeGrip);
                hasSizeGrip = true;
            }

            root.Content(overlay);
            wxStatusBar *liveOwner = resolveCandidateOwner();
            if ( !liveOwner )
                return false;
            if ( callbackState->ModelRevision() != modelRevision )
                continue;

            wxWinUIApplyFont(root, font);
            liveOwner = resolveCandidateOwner();
            if ( !liveOwner )
                return false;
            if ( callbackState->ModelRevision() != modelRevision )
                continue;

#ifdef WXWINUI_TEST_SUPPORT
            if ( impl->nextReentryPoint ==
                     wxWinUIStatusBarReentryPointForTesting::
                         AppearanceRootFont &&
                 impl->nextReentryHook )
            {
                const wxWinUIStatusBarImpl::ReentryHook hook =
                    impl->nextReentryHook;
                void * const hookContext = impl->nextReentryContext;
                impl->nextReentryHook = nullptr;
                impl->nextReentryContext = nullptr;
                hook(liveOwner, hookContext);

                liveOwner = resolveCandidateOwner();
                if ( !liveOwner )
                    return false;
                if ( callbackState->ModelRevision() != modelRevision )
                    continue;
            }
#endif // WXWINUI_TEST_SUPPORT

            wxWinUIApplyForeground(root, foreground);
            liveOwner = resolveCandidateOwner();
            if ( !liveOwner )
                return false;
            if ( callbackState->ModelRevision() != modelRevision )
                continue;

            wxWinUIApplyBackground(root, background);
            liveOwner = resolveCandidateOwner();
            if ( !liveOwner )
                return false;
            if ( callbackState->ModelRevision() != modelRevision )
                continue;

            MUXA::AutomationProperties::SetLocalizedControlType(
                automationRoot, wxWinUIToHString(localizedStatusBar));
            liveOwner = resolveCandidateOwner();
            if ( !liveOwner )
                return false;
            if ( callbackState->ModelRevision() != modelRevision )
                continue;

            // Nothing detached may be attached if a dependency-property callback
            // already changed the wx model while this candidate was built.
            if ( callbackState->ModelRevision() != modelRevision ||
                 callbackState->ConsumeRebuildRequest() )
            {
                continue;
            }

#ifdef WXWINUI_TEST_SUPPORT
            const wxWinUIStatusBarImpl::ReentryHook loadedHook =
                impl->nextReentryPoint ==
                        wxWinUIStatusBarReentryPointForTesting::RebuildLoaded
                    ? impl->nextReentryHook
                    : nullptr;
            void * const loadedHookContext =
                loadedHook ? impl->nextReentryContext : nullptr;
            const std::shared_ptr<bool> loadedHookInvoked =
                std::make_shared<bool>(false);
            const auto invokeLoadedHook =
                [callbackState, lifetimeGeneration, impl, loadedHook,
                 loadedHookContext, loadedHookInvoked]()
                {
                    if ( !loadedHook || *loadedHookInvoked )
                        return;

                    wxStatusBar * const owner =
                        callbackState->GetOwner(lifetimeGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }

                    // Mark this before invoking application code: the hook may
                    // destroy the owner, and a late real Loaded event must not
                    // deliver the same one-shot callback a second time.
                    *loadedHookInvoked = true;
                    loadedHook(owner, loadedHookContext);
                };
            if ( loadedHook )
            {
                impl->nextReentryHook = nullptr;
                impl->nextReentryContext = nullptr;
                wxWinUIControlHostTestAccess::SetNextContentLoadedHook(
                    impl->host,
                    invokeLoadedHook);
            }
#endif // WXWINUI_TEST_SUPPORT

            // Publish the candidate model before SetContent(): its Loaded
            // event is allowed to fire synchronously inside RegisterSlot().
            // The old model remains strongly held for exact rollback if the
            // host rejects the swap.
            MUXC::ContentControl oldRoot = std::move(impl->root);
            MUXC::Grid oldAutomationRoot =
                std::move(impl->automationRoot);
            MUXC::Grid oldOverlay = std::move(impl->overlay);
            MUXC::Grid oldGrid = std::move(impl->grid);
            MUXC::Grid oldSizeGrip = std::move(impl->sizeGrip);
            winrt::com_ptr<wxWinUIStatusBarGripGrid>
                oldSizeGripImpl = std::move(impl->sizeGripImpl);
            std::shared_ptr<wxWinUIStatusBarGripAction>
                oldSizeGripAction = std::move(impl->sizeGripAction);
            std::vector<MUXC::TextBlock> oldFields =
                std::move(impl->fields);
            wxWinUIStatusBarCandidateToolTipGuard oldToolTips(
                &oldFields);
            std::vector<double> oldColumnValues =
                std::move(impl->columnValues);
            std::vector<int> oldColumnUnits =
                std::move(impl->columnUnits);
            std::vector<int> oldFieldStyles =
                std::move(impl->fieldStyles);
            std::vector<int> oldFieldBorderElementCounts =
                std::move(impl->fieldBorderElementCounts);
            const bool oldUsesThemeBorders =
                impl->usesThemeBorders;
            const bool oldHasSizeGrip = impl->hasSizeGrip;
            const unsigned long long oldPeerGeneration =
                impl->peerGeneration;
            const unsigned long long oldPublishedRevision =
                impl->publishedRevision;

            impl->root = root;
            impl->automationRoot = automationRoot;
            impl->overlay = overlay;
            impl->grid = grid;
            impl->sizeGrip = sizeGrip;
            impl->sizeGripImpl = sizeGripImpl;
            impl->sizeGripAction = sizeGripAction;
            impl->fields = std::move(fields);
            impl->columnValues = std::move(columnValues);
            impl->columnUnits = std::move(columnUnits);
            impl->fieldStyles = std::move(fieldStyles);
            impl->fieldBorderElementCounts =
                std::move(fieldBorderElementCounts);
            impl->usesThemeBorders =
                hasThemeBorder && themeBordersComplete;
            impl->hasSizeGrip = hasSizeGrip;
            impl->peerGeneration = generation;
            impl->publishedRevision = modelRevision;

            const bool contentSet =
                impl->host.SetContent(automationRoot);
            liveOwner = callbackState->GetOwner(lifetimeGeneration);
            if ( !liveOwner || !liveOwner->m_winui ||
                 liveOwner->m_winui.get() != impl ||
                 liveOwner->m_winui->callbackState != callbackState )
            {
                return false;
            }
            if ( !contentSet )
            {
#ifdef WXWINUI_TEST_SUPPORT
                wxWinUIControlHostTestAccess::SetNextContentLoadedHook(
                    impl->host, {});
#endif
                impl->root = std::move(oldRoot);
                impl->automationRoot =
                    std::move(oldAutomationRoot);
                impl->overlay = std::move(oldOverlay);
                impl->grid = std::move(oldGrid);
                impl->sizeGrip = std::move(oldSizeGrip);
                impl->sizeGripImpl = std::move(oldSizeGripImpl);
                impl->sizeGripAction =
                    std::move(oldSizeGripAction);
                impl->fields = std::move(oldFields);
                impl->columnValues = std::move(oldColumnValues);
                impl->columnUnits = std::move(oldColumnUnits);
                impl->fieldStyles = std::move(oldFieldStyles);
                impl->fieldBorderElementCounts =
                    std::move(oldFieldBorderElementCounts);
                impl->usesThemeBorders = oldUsesThemeBorders;
                impl->hasSizeGrip = oldHasSizeGrip;
                impl->peerGeneration = oldPeerGeneration;
                impl->publishedRevision = oldPublishedRevision;
                oldToolTips.Release();

                if ( callbackState->ModelRevision() != modelRevision ||
                     callbackState->ConsumeRebuildRequest() )
                {
                    continue;
                }
                return false;
            }

            candidateToolTips.Release();

            // Retired TextBlocks must relinquish only wx's exact managed-tooltip
            // entries. Releasing the old visual references is also explicit so
            // any Unloaded callback runs while the transaction can still observe
            // and coalesce the resulting revision.
            wxWinUIForgetStatusBarManagedToolTips(oldFields);
            oldToolTips.Release();
            oldFields.clear();
            oldSizeGripAction.reset();
            oldSizeGripImpl = nullptr;
            oldSizeGrip = nullptr;
            oldGrid = nullptr;
            oldOverlay = nullptr;
            oldRoot = nullptr;
            oldAutomationRoot = nullptr;

            liveOwner = callbackState->GetOwner(lifetimeGeneration);
            if ( !liveOwner || !liveOwner->m_winui ||
                 liveOwner->m_winui.get() != impl ||
                 liveOwner->m_winui->callbackState != callbackState ||
                 impl->peerGeneration != generation )
            {
                return false;
            }

#ifdef WXWINUI_TEST_SUPPORT
            // The normal path remains asynchronous. A test hook explicitly asks
            // to cross the real Loaded boundary while this transaction is active.
            if ( loadedHook )
            {
                impl->host.ForceRender();
                for ( unsigned pump = 0;
                      pump < 16 && !*loadedHookInvoked;
                      ++pump )
                {
                    // A visible slot normally reaches the actual XAML Loaded
                    // callback here. Hidden test slots are allowed to remain
                    // unloaded, so keep this attempt strictly bounded.
                    wxYield();
                    if ( !callbackState->GetOwner(lifetimeGeneration) )
                        break;
                }

                // Match the Toolbar transaction seam: if a hidden slot did
                // not load, invoke the same generation-checked callback at
                // the committed SetContent()/ForceRender boundary. The host
                // hook is cleared below and the shared flag prevents a late
                // Loaded event from duplicating it.
                if ( !*loadedHookInvoked )
                    invokeLoadedHook();

                liveOwner = callbackState->GetOwner(lifetimeGeneration);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui.get() != impl ||
                     liveOwner->m_winui->callbackState != callbackState )
                {
                    return false;
                }
            }
            wxWinUIControlHostTestAccess::SetNextContentLoadedHook(
                impl->host, {});
#endif // WXWINUI_TEST_SUPPORT

            // Loaded/DP callbacks can mutate text, appearance or even field
            // topology. The attached candidate is already truthfully published;
            // rebuild it from the newest revision, with nested calls coalesced by
            // the guard.
            if ( callbackState->ModelRevision() != modelRevision ||
                 callbackState->ConsumeRebuildRequest() )
            {
                continue;
            }
            callbackState->CancelDeferredRebuild();
            callbackState->ClearRebuildBudgetWarning();
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
#ifdef WXWINUI_TEST_SUPPORT
            wxStatusBar * const liveOwner =
                callbackState->GetOwner(lifetimeGeneration);
            if ( liveOwner && liveOwner->m_winui &&
                 liveOwner->m_winui.get() == impl &&
                 liveOwner->m_winui->callbackState == callbackState )
            {
                wxWinUIControlHostTestAccess::SetNextContentLoadedHook(
                    impl->host, {});
            }
#endif
            wxWinUILogException("WinUI StatusBar content", e);
            return false;
        }
    }
}

void wxStatusBar::SetFieldsCount(int number, const int *widths)
{
    wxStatusBarBase::SetFieldsCount(number, widths);
}

void wxStatusBar::SetStatusWidths(int n, const int widths[])
{
    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->LifetimeGeneration() : 0;
    wxStatusBarBase::SetStatusWidths(n, widths);
    if ( !callbackState )
        return;

    wxStatusBar * const liveOwner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return;
    }
    callbackState->BumpModelRevision();
    liveOwner->RebuildContent();
}

void wxStatusBar::SetStatusStyles(int n, const int styles[])
{
    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->LifetimeGeneration() : 0;
    wxStatusBarBase::SetStatusStyles(n, styles);
    if ( !callbackState )
        return;

    wxStatusBar * const liveOwner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return;
    }
    callbackState->BumpModelRevision();
    liveOwner->RebuildContent();
}

bool wxStatusBar::SetFont(const wxFont& font)
{
    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->LifetimeGeneration() : 0;
    const bool changed = wxControl::SetFont(font);
    if ( !callbackState )
        return changed;

    wxStatusBar *liveOwner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return changed;
    }

    const std::uint64_t modelRevision =
        callbackState->BumpModelRevision();
    liveOwner->RebuildContent();

    liveOwner = callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState ||
         callbackState->ModelRevision() != modelRevision )
    {
        return changed;
    }
    liveOwner->InvalidateBestSize();

    liveOwner = callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState ||
         callbackState->ModelRevision() != modelRevision )
    {
        return changed;
    }
    if ( liveOwner->m_minHeightDIPs >= 0.0 )
    {
        const double minHeightDIPs = liveOwner->m_minHeightDIPs;
        // ApplyMinHeight() may synchronously resize and retire this owner.
        liveOwner->ApplyMinHeight(
            minHeightDIPs, liveOwner->GetMinHeightPixels());
    }
    return changed;
}

bool wxStatusBar::SetForegroundColour(const wxColour& colour)
{
    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->LifetimeGeneration() : 0;
    const bool changed = wxControl::SetForegroundColour(colour);
    if ( callbackState )
    {
        wxStatusBar * const liveOwner =
            callbackState->GetOwner(lifetimeGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == impl &&
             liveOwner->m_winui->callbackState == callbackState )
        {
            callbackState->BumpModelRevision();
            liveOwner->RebuildContent();
        }
    }
    return changed;
}

bool wxStatusBar::SetBackgroundColour(const wxColour& colour)
{
    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->LifetimeGeneration() : 0;
    const bool changed = wxControl::SetBackgroundColour(colour);
    if ( callbackState )
    {
        wxStatusBar * const liveOwner =
            callbackState->GetOwner(lifetimeGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == impl &&
             liveOwner->m_winui->callbackState == callbackState )
        {
            callbackState->BumpModelRevision();
            liveOwner->RebuildContent();
        }
    }
    return changed;
}

void wxStatusBar::DoUpdateStatusText(int number)
{
    wxUnusedVar(number);
    if ( !m_winui || !m_winui->callbackState )
        return;

    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->LifetimeGeneration();
    callbackState->BumpModelRevision();
    wxStatusBar * const liveOwner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( liveOwner && liveOwner->m_winui &&
         liveOwner->m_winui.get() == impl &&
         liveOwner->m_winui->callbackState == callbackState )
    {
        liveOwner->RebuildContent();
    }
}

bool wxStatusBar::GetFieldRect(int n, wxRect& rect) const
{
    wxCHECK_MSG( (n >= 0) && (n < GetFieldsCount()), false,
                 wxT("invalid status bar field index") );

    const wxArrayInt widths = GetAbsWidths();
    if ( widths.IsEmpty() )
        return false;

    rect.x = m_borderX;
    for ( int i = 0; i < n; ++i )
        rect.x += widths[i];

    rect.y = m_borderY;
    rect.width = widths[n] - 2 * m_borderX;
    rect.height = GetClientSize().y - 2 * m_borderY;
    return true;
}

int wxStatusBar::GetMinHeightPixels() const
{
    if ( m_minHeightDIPs < 0.0 )
        return wxDefaultCoord;

    return wxWinUIStatusBarHeightFromDIP(
        m_minHeightDIPs, GetDPI());
}

void wxStatusBar::SetMinHeight(int height)
{
    const int heightPixels = std::max(0, height);
    const wxSize dpi = GetDPI();
    const int dpiY =
        dpi.y > 0 ? dpi.y : wxDisplay::GetStdPPIValue();
    const double heightDIPs =
        static_cast<double>(heightPixels) *
        wxDisplay::GetStdPPIValue() / dpiY;
    ApplyMinHeight(heightDIPs, heightPixels);
}

void wxStatusBar::ApplyMinHeight(double heightDIPs, int heightPixels)
{
    wxASSERT_MSG( heightDIPs >= 0.0,
                  "status bar minimum height must be non-negative" );

    heightDIPs = std::max(0.0, heightDIPs);
    heightPixels = std::max(0, heightPixels);
    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->LifetimeGeneration() : 0;
    unsigned long long minHeightGeneration = 0;
    if ( callbackState )
    {
        if ( ++impl->minHeightGeneration == 0 )
            ++impl->minHeightGeneration;
        minHeightGeneration = impl->minHeightGeneration;
    }

    m_minHeightDIPs = heightDIPs;
    InvalidateBestSize();
    if ( !callbackState )
        return;

    const auto resolveOwner =
        [&]() -> wxStatusBar *
        {
            wxStatusBar * const owner =
                callbackState->GetOwner(lifetimeGeneration);
            return owner && owner->m_winui &&
                   owner->m_winui.get() == impl &&
                   owner->m_winui->callbackState == callbackState &&
                   impl->minHeightGeneration == minHeightGeneration
                ? owner
                : nullptr;
        };

    wxStatusBar *liveOwner = resolveOwner();
    if ( !liveOwner )
        return;

    int targetHeight =
        (11 * liveOwner->GetCharHeight()) / 10 +
        2 * liveOwner->m_borderY;
    targetHeight = wxMax(targetHeight, liveOwner->FromDIP(26));
    if ( heightPixels > 0 )
    {
        targetHeight =
            wxMax(targetHeight,
                  heightPixels + 2 * liveOwner->m_borderY);
    }

    wxSize minSize = liveOwner->GetMinSize();
    minSize.y = targetHeight;
    liveOwner->SetMinSize(minSize);
    liveOwner = resolveOwner();
    if ( !liveOwner )
        return;

#ifdef WXWINUI_TEST_SUPPORT
    if ( impl->nextReentryPoint ==
             wxWinUIStatusBarReentryPointForTesting::MinHeightBeforeResize &&
         impl->nextReentryHook )
    {
        const wxWinUIStatusBarImpl::ReentryHook hook =
            impl->nextReentryHook;
        void * const hookContext = impl->nextReentryContext;
        impl->nextReentryHook = nullptr;
        impl->nextReentryContext = nullptr;
        hook(liveOwner, hookContext);

        liveOwner = resolveOwner();
        if ( !liveOwner )
            return;
    }
#endif // WXWINUI_TEST_SUPPORT

    // SetSize() may synchronously dispatch wxEVT_SIZE and arbitrary user code,
    // so this call is terminal.
    liveOwner->SetSize(wxDefaultCoord, targetHeight);
}

wxSize wxStatusBar::DoGetBestSize() const
{
    int width = 80;
    if ( GetParent() )
        GetParent()->GetClientSize(&width, nullptr);

    int height = (11 * GetCharHeight()) / 10 + 2 * m_borderY;
    height = wxMax(height, FromDIP(26));
    if ( m_minHeightDIPs > 0.0 )
        height = wxMax(height, GetMinHeightPixels() + 2 * m_borderY);

    return wxSize(width, height);
}

void wxStatusBar::OnSize(wxSizeEvent& event)
{
    event.Skip();
    if ( !m_winui || !m_winui->callbackState )
        return;

    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->LifetimeGeneration();
    const std::uint64_t modelRevision =
        callbackState->BumpModelRevision();
    wxStatusBar * const liveOwner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( liveOwner && liveOwner->m_winui &&
         liveOwner->m_winui.get() == impl &&
         liveOwner->m_winui->callbackState == callbackState &&
         callbackState->ModelRevision() == modelRevision )
    {
        liveOwner->RebuildContent();
    }
}

void wxStatusBar::OnDPIChanged(wxDPIChangedEvent& event)
{
    event.Skip();
    if ( !m_winui || !m_winui->callbackState )
        return;

    wxWinUIStatusBarImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->LifetimeGeneration();
    if ( ++impl->dpiGeneration == 0 )
        ++impl->dpiGeneration;
    const unsigned long long dpiGeneration = impl->dpiGeneration;
    const int borderX = FromDIP(2);
    wxStatusBar *liveOwner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState ||
         impl->dpiGeneration != dpiGeneration )
    {
        return;
    }

#ifdef WXWINUI_TEST_SUPPORT
    if ( impl->nextReentryPoint ==
             wxWinUIStatusBarReentryPointForTesting::DPIBorderX &&
         impl->nextReentryHook )
    {
        const wxWinUIStatusBarImpl::ReentryHook hook =
            impl->nextReentryHook;
        void * const hookContext = impl->nextReentryContext;
        impl->nextReentryHook = nullptr;
        impl->nextReentryContext = nullptr;
        hook(liveOwner, hookContext);

        liveOwner = callbackState->GetOwner(lifetimeGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != impl ||
             liveOwner->m_winui->callbackState != callbackState ||
             impl->dpiGeneration != dpiGeneration )
        {
            return;
        }
    }
#endif // WXWINUI_TEST_SUPPORT

    const int borderY = liveOwner->FromDIP(2);
    liveOwner = callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState ||
         impl->dpiGeneration != dpiGeneration )
    {
        return;
    }
    liveOwner->m_borderX = borderX;
    liveOwner->m_borderY = borderY;
    const double minHeightDIPs = liveOwner->m_minHeightDIPs;
    const unsigned long long minHeightGeneration =
        impl->minHeightGeneration;
    const std::uint64_t modelRevision =
        callbackState->BumpModelRevision();
    liveOwner->InvalidateBestSize();

    liveOwner = callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState ||
         impl->dpiGeneration != dpiGeneration ||
         callbackState->ModelRevision() != modelRevision )
    {
        return;
    }
    liveOwner->RebuildContent();

    liveOwner = callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState ||
         impl->dpiGeneration != dpiGeneration ||
         impl->minHeightGeneration != minHeightGeneration ||
         liveOwner->m_minHeightDIPs != minHeightDIPs ||
         minHeightDIPs < 0.0 )
    {
        return;
    }

    int minHeightPixels = wxWinUIStatusBarHeightFromDIP(
        minHeightDIPs, event.GetNewDPI());
    if ( minHeightPixels == wxDefaultCoord )
        minHeightPixels = liveOwner->GetMinHeightPixels();

    // ApplyMinHeight() is terminal if its resize dispatch retires the owner.
    liveOwner->ApplyMinHeight(minHeightDIPs, minHeightPixels);
}

#ifdef WXWINUI_TEST_SUPPORT

bool wxWinUIStatusBarTestAccess::HasSizeGrip(const wxStatusBar& bar)
{
    return bar.m_winui && bar.m_winui->hasSizeGrip;
}

bool wxWinUIStatusBarTestAccess::GetSizeGripState(
    const wxStatusBar& bar,
    wxWinUIStatusBarSizeGripSnapshot *snapshot)
{
    if ( !snapshot || !bar.m_winui || !bar.m_winui->hasSizeGrip ||
         !bar.m_winui->overlay || !bar.m_winui->grid ||
         !bar.m_winui->sizeGrip || !bar.m_winui->sizeGripImpl ||
         !bar.m_winui->sizeGripAction )
    {
        return false;
    }

    try
    {
        wxWinUIStatusBarSizeGripSnapshot observed;
        observed.widthDips = bar.m_winui->sizeGrip.Width();
        observed.heightDips = bar.m_winui->sizeGrip.Height();
        observed.horizontalAlignment =
            static_cast<int>(
                bar.m_winui->sizeGrip.HorizontalAlignment());
        observed.cursorShape =
            static_cast<int>(
                bar.m_winui->sizeGripImpl->GetCursorShape());
        observed.nativeHitTest =
            bar.m_winui->sizeGripAction->nativeHitTest;
        observed.automationName = wxString(
            MUXA::AutomationProperties::GetName(
                bar.m_winui->sizeGrip).c_str());
        observed.localizedControlType = wxString(
            MUXA::AutomationProperties::GetLocalizedControlType(
                bar.m_winui->sizeGrip).c_str());
        const MUX::Thickness fieldMargin = bar.m_winui->grid.Margin();
        observed.fieldReservationDips =
            bar.m_winui->sizeGrip.HorizontalAlignment() ==
                    MUX::HorizontalAlignment::Left
                ? fieldMargin.Left
                : fieldMargin.Right;
        observed.reservesFieldSpace =
            observed.fieldReservationDips > 0.0;

        // VisualTreeHelper::GetParent() may legitimately return null for a
        // hidden/unloaded shared-island slot. The overlay contract is a
        // structural one, so inspect the exact logical children instead. The
        // grid margin above means these siblings don't compete for field space.
        const auto children = bar.m_winui->overlay.Children();
        std::uint32_t fieldsIndex = 0;
        std::uint32_t gripIndex = 0;
        observed.overlaysFields =
            children.IndexOf(bar.m_winui->grid, fieldsIndex) &&
            children.IndexOf(bar.m_winui->sizeGrip, gripIndex) &&
            fieldsIndex != gripIndex;
        *snapshot = std::move(observed);
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIStatusBarTestAccess::InvokeSizeGrip(
    wxStatusBar& bar,
    bool isMouse,
    bool isPrimary,
    bool isLeftButtonPressed,
    const wxPoint& screenPoint)
{
    if ( !bar.m_winui || !bar.m_winui->hasSizeGrip ||
         !bar.m_winui->sizeGripAction ||
         !bar.m_winui->sizeGripAction->invoke )
    {
        return false;
    }

    const std::shared_ptr<wxWinUIStatusBarGripAction> action =
        bar.m_winui->sizeGripAction;
    const POINT point{ screenPoint.x, screenPoint.y };
    return action->invoke(
        isMouse, isPrimary, isLeftButtonPressed, point, 0, ::GetTickCount64());
}

void wxWinUIStatusBarTestAccess::SetTopLevelMaximized(
    wxStatusBar& bar, bool maximized)
{
    if ( !bar.m_winui || !bar.m_winui->callbackState )
        return;

    wxWinUIStatusBarImpl * const impl = bar.m_winui.get();
    const std::shared_ptr<wxWinUIStatusBarCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->LifetimeGeneration();
    wxStatusBar * const liveOwner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return;
    }

    impl->topLevelMaximizedForTesting = maximized ? 1 : 0;
    callbackState->BumpModelRevision();

    // This also repositions AddFieldControl() children against the changed
    // reservation. It is deliberately terminal: an event handler may delete
    // the status bar synchronously.
    liveOwner->SendSizeEvent();
}

bool wxWinUIStatusBarTestAccess::GetFieldState(
    const wxStatusBar& bar,
    int field,
    wxString *renderedText,
    int *textTrimming,
    int *fieldStyle,
    bool *hasToolTip,
    double *columnValue,
    int *columnUnitType,
    wxString *automationName,
    int *borderElementCount,
    int *fieldGridFlowDirection,
    int *textFlowDirection)
{
    if ( !bar.m_winui || !bar.m_winui->grid || field < 0 ||
         field >= static_cast<int>(bar.m_winui->fields.size()) ||
         field >= static_cast<int>(bar.m_winui->columnValues.size()) ||
         field >= static_cast<int>(bar.m_winui->columnUnits.size()) ||
         field >= static_cast<int>(bar.m_winui->fieldStyles.size()) ||
         field >= static_cast<int>(
             bar.m_winui->fieldBorderElementCounts.size()) )
    {
        return false;
    }

    try
    {
        const auto text = bar.m_winui->fields[field];
        if ( renderedText )
            *renderedText = wxString(text.Text().c_str());
        if ( textTrimming )
            *textTrimming = static_cast<int>(text.TextTrimming());
        if ( fieldStyle )
            *fieldStyle = bar.m_winui->fieldStyles[field];
        if ( hasToolTip )
        {
#if wxUSE_TOOLTIPS
            *hasToolTip =
                MUXC::ToolTipService::GetToolTip(text) != nullptr;
#else
            *hasToolTip = false;
#endif
        }
        if ( columnValue )
            *columnValue = bar.m_winui->columnValues[field];
        if ( columnUnitType )
            *columnUnitType = bar.m_winui->columnUnits[field];
        if ( automationName )
        {
            *automationName = wxString(
                MUXA::AutomationProperties::GetName(text).c_str());
        }
        if ( borderElementCount )
        {
            *borderElementCount =
                bar.m_winui->fieldBorderElementCounts[field];
        }
        if ( fieldGridFlowDirection )
        {
            *fieldGridFlowDirection =
                static_cast<int>(bar.m_winui->grid.FlowDirection());
        }
        if ( textFlowDirection )
        {
            *textFlowDirection =
                static_cast<int>(text.FlowDirection());
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIStatusBarTestAccess::GetAppearance(
    const wxStatusBar& bar,
    wxWinUIAppearanceSnapshot *snapshot)
{
    if ( !snapshot || !bar.m_winui || !bar.m_winui->root ||
         !bar.m_winui->automationRoot )
        return false;

    try
    {
        *snapshot =
            wxWinUICaptureAppearance(
                bar.m_winui->root, bar.m_winui->automationRoot);
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIStatusBarTestAccess::UsesThemeBorders(const wxStatusBar& bar)
{
    return bar.m_winui && bar.m_winui->usesThemeBorders;
}

void wxWinUIStatusBarTestAccess::SetNextReentryHook(
    wxStatusBar& bar,
    wxWinUIStatusBarReentryPointForTesting point,
    ReentryHook hook,
    void *context)
{
    if ( !bar.m_winui || !bar.m_winui->callbackState )
        return;

    bar.m_winui->nextReentryPoint = point;
    bar.m_winui->nextReentryHook = hook;
    bar.m_winui->nextReentryContext = hook ? context : nullptr;
}

unsigned long long wxWinUIStatusBarTestAccess::GetModelRevision(
    const wxStatusBar& bar)
{
    return bar.m_winui && bar.m_winui->callbackState
               ? bar.m_winui->callbackState->ModelRevision()
               : 0;
}

bool wxWinUIStatusBarTestAccess::HasDeferredRebuild(const wxStatusBar& bar)
{
    return bar.m_winui && bar.m_winui->callbackState &&
           bar.m_winui->callbackState->HasDeferredRebuild();
}

bool wxWinUIStatusBarTestAccess::IsRebuildQuarantined(const wxStatusBar& bar)
{
    return bar.m_winui && bar.m_winui->callbackState &&
           bar.m_winui->callbackState->IsRebuildQuarantined();
}

void wxWinUIStatusBarTestAccess::DeliverDPIChanged(
    wxStatusBar& bar, wxDPIChangedEvent& event)
{
    // OnDPIChanged() is written to permit the implementation-only callback to
    // delete this object and performs no access after detecting retirement.
    // Keep this wrapper equally terminal.
    bar.OnDPIChanged(event);
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_STATUSBAR
