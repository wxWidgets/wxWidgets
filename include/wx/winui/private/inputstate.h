/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/inputstate.h
// Purpose:     deterministic state machine for the WinUI pointer bridge
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_INPUTSTATE_H_
#define _WX_WINUI_PRIVATE_INPUTSTATE_H_

#include "wx/defs.h"
#include "wx/debug.h"

#if wxUSE_WINUI3

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

// These types deliberately don't expose any WinRT or HWND type. The adapter
// in tlwhost.cpp owns conversion to and from platform events; this layer owns
// only the ordering and lifetime-independent parts of the input contract.
enum class wxWinUIInputDevice
{
    Mouse,
    Touch,
    Pen
};

enum class wxWinUIInputKind
{
    Move,
    Enter,
    Leave,
    Press,
    Release,
    DoubleClick,
    Wheel,
    CaptureLost,
    Cancel
};

enum class wxWinUIInputButton
{
    None,
    Left,
    Right,
    Middle,
    X1,
    X2
};

enum class wxWinUIInputSurface
{
    Native,
    Xaml,
    Outside,
    Indeterminate
};

enum class wxWinUIInputArea
{
    Client,
    NonClient
};

// The leaf value is opaque to the state machine. The adapter normally stores
// a HWND converted to uintptr_t and the generation captured by inputrouter.
// Both fields are required: a recycled handle must never inherit hover,
// capture or click state from its predecessor.
struct WXDLLIMPEXP_CORE wxWinUIInputTargetKey
{
    std::uintptr_t leaf = 0;
    std::uint64_t generation = 0;
    // Logical wx shell identity. It lets an implementation child receiving
    // DOWN and its parent shell receiving SetCapture remain one gesture,
    // while leaf/generation still identify the exact dispatch HWND.
    std::uintptr_t owner = 0;
    std::uint64_t ownerGeneration = 0;

    bool IsOk() const { return leaf != 0 && generation != 0; }
    bool HasSameOwner(const wxWinUIInputTargetKey& other) const
    {
        const bool thisHasOwner = owner != 0 || ownerGeneration != 0;
        const bool otherHasOwner =
            other.owner != 0 || other.ownerGeneration != 0;
        if ( thisHasOwner || otherHasOwner )
        {
            // A half-populated owner identity is never allowed to borrow the
            // leaf's generation: that could alias a recycled shell HWND.
            if ( !owner || !ownerGeneration ||
                 !other.owner || !other.ownerGeneration )
            {
                return false;
            }
            return owner == other.owner &&
                   ownerGeneration == other.ownerGeneration;
        }

        return IsOk() && other.IsOk() &&
               leaf == other.leaf && generation == other.generation;
    }

    bool operator==(const wxWinUIInputTargetKey& other) const
    {
        return leaf == other.leaf &&
               generation == other.generation &&
               owner == other.owner &&
               ownerGeneration == other.ownerGeneration;
    }

    bool operator!=(const wxWinUIInputTargetKey& other) const
    {
        return !(*this == other);
    }
};

struct WXDLLIMPEXP_CORE wxWinUIPointerSample
{
    wxWinUIInputDevice device = wxWinUIInputDevice::Mouse;
    wxWinUIInputKind kind = wxWinUIInputKind::Move;
    wxWinUIInputButton button = wxWinUIInputButton::None;
    std::uint32_t pointerId = 0;
    bool isPrimary = true;

    // Set by the platform adapter for the compatibility mouse event promoted
    // from a touch/pen event already routed through this state machine.
    bool isCompatibilityMouse = false;
    // WM_CAPTURECHANGED/WM_CANCELMODE are thread-wide USER32 interruptions.
    // XAML PointerCanceled/PointerCaptureLost are pointer-specific.
    bool interruptAll = false;

    int screenX = 0;
    int screenY = 0;
    // The exact XAML-root position carried by a routed pointer event. Win32
    // dispatch still needs integer screen pixels, but XAML hit classification
    // must not round DIP -> px -> DIP at fractional rasterization scales.
    bool hasRootDips = false;
    double rootXDIP = 0.0;
    double rootYDIP = 0.0;
    std::uint64_t timestamp = 0;
    unsigned modifiers = 0;
    unsigned buttonMask = 0;
    int wheelDelta = 0;
    bool horizontalWheel = false;
};

struct WXDLLIMPEXP_CORE wxWinUIRouteObservation
{
    wxWinUIInputSurface surface = wxWinUIInputSurface::Indeterminate;
    wxWinUIInputTargetKey target;
    wxWinUIInputArea area = wxWinUIInputArea::Client;

    // A stable semantic hit zone. The Win32 adapter uses the HT* value.
    int zone = 0;

    // CS_DBLCLKS is required for client double-click messages. USER32's
    // non-client double-click messages explicitly don't require this style.
    bool clientDoubleClicks = false;
};

enum class wxWinUIInputActionKind
{
    HoverEnter,
    HoverLeave,
    Dispatch,
    ReleaseCapture,
    CancelPress,
    BreakStorm
};

struct WXDLLIMPEXP_CORE wxWinUIInputAction
{
    wxWinUIInputActionKind action = wxWinUIInputActionKind::Dispatch;
    wxWinUIInputTargetKey target;
    wxWinUIInputArea area = wxWinUIInputArea::Client;
    wxWinUIInputKind kind = wxWinUIInputKind::Move;
    wxWinUIInputButton button = wxWinUIInputButton::None;
    int zone = 0;
    int screenX = 0;
    int screenY = 0;
    int wheelDelta = 0;
    bool horizontalWheel = false;
    wxWinUIInputDevice device = wxWinUIInputDevice::Mouse;
    std::uint32_t pointerId = 0;
    std::uint64_t timestamp = 0;
    unsigned modifiers = 0;
    unsigned buttonMask = 0;
    // State-issued identity of the gesture represented by a Press/Release.
    // It is independent of platform timestamps and never zero.
    std::uint64_t gestureSerial = 0;
    // Click serial observed while preparing a Press. Commit requires the same
    // serial so a preflight that re-entered input cannot create a stale
    // double-click decision.
    std::uint64_t expectedClickSerial = 0;
    std::uint64_t expectedStateVersion = 0;
    // Hover transitions are also two-phase. A Leave commits old -> none and
    // an Enter commits none -> new immediately before the adapter mirrors the
    // corresponding native state. This prevents a callback from observing a
    // future hover target whose Enter action has not run yet.
    std::uint64_t expectedHoverVersion = 0;
    // Exact HWND generation that received the matching DOWN. It can differ
    // from target when an implementation child delegates capture to its wx
    // shell.
    wxWinUIInputTargetKey balanceTarget;
    // True only for a Release prepared from a matching committed DOWN.
    bool balancesPress = false;
    // True when committing this Release may establish the first click of a
    // future double-click pair.
    bool completesClick = false;

    // Non-zero only for BreakStorm: the proven repeating period.
    unsigned stormPeriod = 0;
};

// Pointer routing is a hot path. Keep the transition self-contained without
// allocating: its largest possible transition is a five-button cancel plus
// capture/hover bookkeeping, comfortably below this fixed capacity.
class WXDLLIMPEXP_CORE wxWinUIInputActions
{
public:
    static constexpr std::size_t Capacity = 12;

    using iterator = wxWinUIInputAction *;
    using const_iterator = const wxWinUIInputAction *;

    bool empty() const { return m_count == 0; }
    std::size_t size() const { return m_count; }
    iterator begin() { return m_actions.data(); }
    iterator end() { return m_actions.data() + m_count; }
    const_iterator begin() const { return m_actions.data(); }
    const_iterator end() const { return m_actions.data() + m_count; }
    const wxWinUIInputAction& operator[](std::size_t index) const
        { return m_actions[index]; }
    wxWinUIInputAction& operator[](std::size_t index)
        { return m_actions[index]; }

    void push_back(const wxWinUIInputAction& action)
    {
        wxASSERT_MSG(m_count < Capacity,
                     "WinUI input transition action capacity exceeded");
        if ( m_count < Capacity )
            m_actions[m_count++] = action;
    }

private:
    std::array<wxWinUIInputAction, Capacity> m_actions;
    std::size_t m_count = 0;
};

enum class wxWinUIInputDisposition
{
    Routed,
    Xaml,
    Outside,
    Indeterminate,
    InvalidTarget,
    SecondaryPointer,
    CompatibilityDuplicate,
    StormSuppressed
};

enum class wxWinUIInputStormDecision
{
    Deliver,
    SuppressRedundant,
    BreakCycle
};

struct WXDLLIMPEXP_CORE wxWinUIInputTransition
{
    wxWinUIInputActions actions;
    wxWinUIInputDisposition disposition = wxWinUIInputDisposition::Routed;

    // The adapter may mark the originating routed event handled only when
    // this is true. XAML and indeterminate observations always leave it false.
    bool cancelSource = false;

    wxWinUIInputStormDecision stormDecision =
        wxWinUIInputStormDecision::Deliver;

    bool HasAction(wxWinUIInputActionKind kind) const;
    std::size_t CountActions(wxWinUIInputActionKind kind) const;
};

enum class wxWinUIInputUpdateKind
{
    Other,
    LeftPressed,
    LeftReleased,
    RightPressed,
    RightReleased,
    MiddlePressed,
    MiddleReleased,
    X1Pressed,
    X1Released,
    X2Pressed,
    X2Released
};

struct WXDLLIMPEXP_CORE wxWinUIInputMapping
{
    bool valid = false;
    wxWinUIInputKind kind = wxWinUIInputKind::Move;
    wxWinUIInputButton button = wxWinUIInputButton::None;
    bool horizontalWheel = false;
};

WXDLLIMPEXP_CORE wxWinUIInputMapping
wxWinUIMapPointerUpdate(wxWinUIInputUpdateKind update);

WXDLLIMPEXP_CORE wxWinUIInputMapping
wxWinUIMapPointerWheel(bool horizontal);

struct WXDLLIMPEXP_CORE wxWinUIClickSettings
{
    std::uint64_t maxInterval = 500;
    int rectangleWidth = 4;
    int rectangleHeight = 4;
};

class WXDLLIMPEXP_CORE wxWinUIInputState
{
public:
    explicit wxWinUIInputState(
        const wxWinUIClickSettings& clickSettings = wxWinUIClickSettings());
    ~wxWinUIInputState();

    wxWinUIInputTransition Route(
        const wxWinUIPointerSample& sample,
        const wxWinUIRouteObservation& observation);

    // Root PointerEntered/PointerExited events are input-site boundaries, not
    // native hits. Observe them for the bounded storm proof and retire hover
    // on Leave, without ever producing a native Dispatch or claiming XAML.
    wxWinUIInputTransition RouteBoundary(
        const wxWinUIPointerSample& sample);

    // Press and Release are prepared without changing semantic state because
    // the adapter still has callback-bearing hit-test preflights to perform.
    // Call this exactly once after all preflights and immediately before the
    // corresponding native/wx delivery. It fails closed if input re-entered.
    bool CommitBeforeDispatch(const wxWinUIInputAction& action);
    // Commit a prepared HoverLeave/HoverEnter in action order. The adapter
    // calls this immediately before changing native hover state; stale
    // snapshots fail without overwriting a hover established re-entrantly.
    bool CommitHoverAction(const wxWinUIInputAction& action);
    // Undo only the exact HoverEnter that was just committed but whose native
    // dispatch never began. This is deliberately version-bound: a nested
    // transition that replaced the hover makes the rollback a no-op.
    bool AbortCommittedHoverEnter(const wxWinUIInputAction& action);
    // A modal non-client DOWN can return while the physical contact is still
    // active (e.g. caption double-click or an aborted tracking loop). Keep
    // the exact committed press so its real UP can balance, but make that UP
    // ineligible to seed another double-click.
    bool SuppressClickForGesture(const wxWinUIInputAction& action);
    // Cancel exactly the committed DOWN identified by gesture, without
    // touching another button carried by the same physical pointer. The
    // returned CancelPress action follows the normal tombstone protocol.
    wxWinUIInputTransition CancelActiveGesture(
        const wxWinUIInputAction& gesture,
        const wxWinUIPointerSample& sample);
    // Complete a synchronous native DOWN that returned after its matching
    // physical contact was consumed inside the native loop. The elapsed/contact
    // policy and exact-serial cancellation live here so the production adapter
    // can be tested without Sleep(), SendInput or physical key state.
    wxWinUIInputTransition FinishSynchronousDown(
        const wxWinUIInputAction& gesture,
        const wxWinUIPointerSample& sample,
        std::uint64_t elapsedMilliseconds,
        bool contactActive);
    bool AbortUndeliverableRelease(
        const wxWinUIInputAction& action,
        wxWinUIInputAction *cancellation);

    // CancelPress uses a serial-bound tombstone: the adapter must first
    // reject stale cancellation actions with CanDeliverCancellation(), then
    // call AcknowledgeCancellation() exactly once after WM_CANCELMODE (or
    // equivalent semantic cleanup). It must acknowledge even when the native
    // target has already disappeared; until then a replacement DOWN for the
    // same button deliberately cannot commit.
    bool CanDeliverCancellation(
        const wxWinUIInputAction& action) const;
    void AcknowledgeCancellation(
        const wxWinUIInputAction& action);
    bool HasActivePress(wxWinUIInputDevice device,
                        std::uint32_t pointerId,
                        wxWinUIInputButton button =
                            wxWinUIInputButton::None) const;
    bool HasAnyActivePress() const;
    // Verify that this exact prepared/committed DOWN still owns its button.
    // Modal adapters use this after their final callback-bearing hit test,
    // before synthesizing the release consumed inside the native loop.
    bool HasActiveGesture(const wxWinUIInputAction& action) const;
    bool HasActivePressForOwner(
        const wxWinUIInputTargetKey& owner) const;

    // Feed the observed USER32 capture owner after GetCapture() changes.
    // Press alone never implies capture. Passing an invalid key reports loss
    // of the previously observed capture and cancels its pressed state once.
    wxWinUIInputTransition NativeCaptureChanged(
        const wxWinUIInputTargetKey& owner,
        const wxWinUIPointerSample& sample);

    // The guard is never armed implicitly. It is a proof-driven emergency
    // mechanism: identical redundant events may be suppressed and short
    // repeating cycles request a breaker action, but only for a bounded
    // number of interventions.
    void ArmStormGuard(unsigned maxInterventions,
                       unsigned repeatedCycles = 3,
                       unsigned maxPeriod = 4,
                       std::uint64_t maxGap = 8);
    void DisarmStormGuard();
    bool IsStormGuardArmed() const;
    unsigned GetStormInterventionCount() const;

private:
    struct HoverState;
    struct PressState;
    struct PressTable;
    struct ClickState;
    struct ClickTable;
    struct CancelState;
    struct CancelTable;
    struct StormSignature;
    class StormGuard;

    wxWinUIInputTransition RouteNative(
        wxWinUIPointerSample sample,
        const wxWinUIRouteObservation& observation,
        bool closeHoverBeforeDispatch,
        bool releaseCompletesClick);
    void LeaveHover(wxWinUIInputTransition& transition,
                    const wxWinUIPointerSample& sample);
    void EnterHover(wxWinUIInputTransition& transition,
                    const wxWinUIRouteObservation& observation,
                    const wxWinUIPointerSample& sample);
    void CancelPress(wxWinUIInputTransition& transition,
                     bool releaseCapture,
                     bool allPointers,
                     const wxWinUIPointerSample& sample);
    bool IsDoubleClick(const wxWinUIPointerSample& sample,
                       const wxWinUIRouteObservation& observation,
                       unsigned buttonIndex) const;

    wxWinUIClickSettings m_clickSettings;
    std::unique_ptr<HoverState> m_hover;
    std::unique_ptr<PressTable> m_presses;
    std::unique_ptr<ClickTable> m_clicks;
    std::unique_ptr<CancelTable> m_cancels;
    std::unique_ptr<StormGuard> m_storm;
    wxWinUIInputTargetKey m_captureTarget;
    std::uint64_t m_nextGestureSerial = 0;
    std::uint64_t m_hoverVersion = 0;
    std::array<std::uint64_t, 5> m_buttonVersions{};

    wxDECLARE_NO_COPY_CLASS(wxWinUIInputState);
};

#endif // wxUSE_WINUI3

#endif // _WX_WINUI_PRIVATE_INPUTSTATE_H_
