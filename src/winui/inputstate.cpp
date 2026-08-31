/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/inputstate.cpp
// Purpose:     deterministic state machine for the WinUI pointer bridge
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/private/inputstate.h"

#if wxUSE_WINUI3

#include <algorithm>
#include <array>
#include <cstdlib>
#include <utility>

namespace
{

wxWinUIInputAction MakeAction(
    wxWinUIInputActionKind action,
    const wxWinUIInputTargetKey& target,
    wxWinUIInputArea area,
    wxWinUIInputKind kind,
    wxWinUIInputButton button,
    int zone,
    const wxWinUIPointerSample& sample)
{
    wxWinUIInputAction result;
    result.action = action;
    result.target = target;
    result.area = area;
    result.kind = kind;
    result.button = button;
    result.zone = zone;
    result.screenX = sample.screenX;
    result.screenY = sample.screenY;
    result.wheelDelta = sample.wheelDelta;
    result.horizontalWheel = sample.horizontalWheel;
    result.device = sample.device;
    result.pointerId = sample.pointerId;
    result.timestamp = sample.timestamp;
    result.modifiers = sample.modifiers;
    result.buttonMask = sample.buttonMask;
    return result;
}

bool IsStormEligible(wxWinUIInputKind kind)
{
    return kind == wxWinUIInputKind::Move ||
           kind == wxWinUIInputKind::Enter ||
           kind == wxWinUIInputKind::Leave;
}

int ButtonIndex(wxWinUIInputButton button)
{
    switch ( button )
    {
        case wxWinUIInputButton::Left:
            return 0;
        case wxWinUIInputButton::Right:
            return 1;
        case wxWinUIInputButton::Middle:
            return 2;
        case wxWinUIInputButton::X1:
            return 3;
        case wxWinUIInputButton::X2:
            return 4;
        case wxWinUIInputButton::None:
            return -1;
    }

    return -1;
}

} // namespace

struct wxWinUIInputState::HoverState
{
    wxWinUIInputTargetKey target;
    wxWinUIInputArea area = wxWinUIInputArea::Client;
    int zone = 0;
};

struct wxWinUIInputState::PressState
{
    wxWinUIInputTargetKey target;
    wxWinUIInputArea area = wxWinUIInputArea::Client;
    wxWinUIInputButton button = wxWinUIInputButton::None;
    int zone = 0;
    int downX = 0;
    int downY = 0;
    std::uint64_t downTime = 0;
    bool doubleClick = false;
    bool clickEligible = true;
    wxWinUIInputDevice device = wxWinUIInputDevice::Mouse;
    std::uint32_t pointerId = 0;
    std::uint64_t gestureSerial = 0;

    bool IsActive() const { return target.IsOk(); }
};

struct wxWinUIInputState::PressTable
{
    std::array<PressState, 5> buttons;
    unsigned activeButtons = 0;
};

struct wxWinUIInputState::ClickState
{
    wxWinUIInputTargetKey target;
    wxWinUIInputArea area = wxWinUIInputArea::Client;
    wxWinUIInputButton button = wxWinUIInputButton::None;
    int zone = 0;
    int x = 0;
    int y = 0;
    std::uint64_t time = 0;
    wxWinUIInputDevice device = wxWinUIInputDevice::Mouse;
    std::uint64_t gestureSerial = 0;
};

struct wxWinUIInputState::ClickTable
{
    std::array<ClickState, 5> buttons;
};

struct wxWinUIInputState::CancelState
{
    wxWinUIInputTargetKey target;
    wxWinUIInputButton button = wxWinUIInputButton::None;
    std::uint64_t gestureSerial = 0;
};

struct wxWinUIInputState::CancelTable
{
    std::array<CancelState, 5> buttons;
};

struct wxWinUIInputState::StormSignature
{
    wxWinUIInputKind kind = wxWinUIInputKind::Move;
    wxWinUIInputButton button = wxWinUIInputButton::None;
    wxWinUIInputDevice device = wxWinUIInputDevice::Mouse;
    wxWinUIInputTargetKey target;
    wxWinUIInputArea area = wxWinUIInputArea::Client;
    std::uint32_t pointerId = 0;
    int zone = 0;
    int x = 0;
    int y = 0;
    unsigned modifiers = 0;
    unsigned buttonMask = 0;
    std::uint64_t timestamp = 0;
    wxWinUIInputTargetKey captureTarget;

    bool operator==(const StormSignature& other) const
    {
        return kind == other.kind &&
               button == other.button &&
               device == other.device &&
               target == other.target &&
               area == other.area &&
               pointerId == other.pointerId &&
               zone == other.zone &&
               x == other.x &&
               y == other.y &&
               modifiers == other.modifiers &&
               buttonMask == other.buttonMask &&
               captureTarget == other.captureTarget;
    }
};

class wxWinUIInputState::StormGuard
{
public:
    static constexpr unsigned MaxInterventions = 16;
    static constexpr unsigned MaxRepeatedCycles = 4;
    static constexpr unsigned MaxPeriod = 4;
    static constexpr std::size_t MaxHistory =
        MaxRepeatedCycles * MaxPeriod;

    struct Result
    {
        wxWinUIInputStormDecision decision =
            wxWinUIInputStormDecision::Deliver;
        unsigned period = 0;
    };

    void Arm(unsigned maxInterventions,
             unsigned repeatedCycles,
             unsigned maxPeriod,
             std::uint64_t maxGap)
    {
        m_armed = maxInterventions != 0;
        m_maxInterventions =
            std::min(maxInterventions, MaxInterventions);
        m_repeatedCycles =
            std::min(std::max(2u, repeatedCycles), MaxRepeatedCycles);
        m_maxPeriod = std::min(std::max(1u, maxPeriod), MaxPeriod);
        m_maxGap = std::min<std::uint64_t>(
            std::max<std::uint64_t>(1, maxGap), 100);
        m_interventions = 0;
        m_historySize = 0;
    }

    void Disarm()
    {
        m_armed = false;
        m_historySize = 0;
    }

    bool IsArmed() const { return m_armed; }
    unsigned GetInterventionCount() const { return m_interventions; }
    void ResetHistory() { m_historySize = 0; }

    Result Observe(const wxWinUIPointerSample& sample,
                   const wxWinUIRouteObservation& observation,
                   const wxWinUIInputTargetKey& captureTarget)
    {
        if ( !m_armed )
            return Result();

        // Press, release and wheel are never candidates for suppression.
        // Clearing the history also prevents a pattern from bridging a
        // meaningful state transition.
        if ( !IsStormEligible(sample.kind) )
        {
            m_historySize = 0;
            return Result();
        }

        StormSignature signature;
        signature.kind = sample.kind;
        signature.button = sample.button;
        signature.device = sample.device;
        signature.target = observation.target;
        signature.area = observation.area;
        signature.pointerId = sample.pointerId;
        signature.zone = observation.zone;
        signature.x = sample.screenX;
        signature.y = sample.screenY;
        signature.modifiers = sample.modifiers;
        signature.buttonMask = sample.buttonMask;
        signature.timestamp = sample.timestamp;
        signature.captureTarget = captureTarget;

        // Identity alone is not proof of a storm: ordinary moves can revisit
        // the same point. Only a tightly spaced, monotonic run observed after
        // the guard was explicitly armed participates in cycle detection.
        if ( m_historySize )
        {
            const std::uint64_t previous =
                m_history[m_historySize - 1].timestamp;
            if ( sample.timestamp < previous ||
                 sample.timestamp - previous > m_maxGap )
            {
                m_historySize = 0;
            }
        }

        const std::size_t historyLimit =
            static_cast<std::size_t>(m_repeatedCycles) * m_maxPeriod;
        if ( m_historySize == historyLimit )
        {
            std::move(m_history.begin() + 1,
                      m_history.begin() + m_historySize,
                      m_history.begin());
            --m_historySize;
        }
        m_history[m_historySize++] = signature;

        unsigned period = 0;
        for ( unsigned candidate = 1; candidate <= m_maxPeriod; ++candidate )
        {
            const std::size_t required =
                static_cast<std::size_t>(candidate) * m_repeatedCycles;
            if ( m_historySize < required )
                continue;

            const std::size_t start = m_historySize - required;
            bool repeats = true;
            for ( std::size_t i = start + candidate;
                  i < m_historySize; ++i )
            {
                if ( !(m_history[i] == m_history[
                        start + ((i - start) % candidate)]) )
                {
                    repeats = false;
                    break;
                }
            }
            if ( repeats )
            {
                period = candidate;
                break;
            }
        }

        if ( !period )
            return Result();

        ++m_interventions;
        Result result;
        result.period = period;
        result.decision = period == 1
                            ? wxWinUIInputStormDecision::SuppressRedundant
                            : wxWinUIInputStormDecision::BreakCycle;

        // Bound the breaker itself. If it did not stop the external replay,
        // normal delivery resumes instead of permanently eating input.
        if ( m_interventions >= m_maxInterventions )
            m_armed = false;

        return result;
    }

private:
    bool m_armed = false;
    unsigned m_maxInterventions = 0;
    unsigned m_repeatedCycles = 3;
    unsigned m_maxPeriod = 4;
    unsigned m_interventions = 0;
    std::uint64_t m_maxGap = 8;
    std::array<StormSignature, MaxHistory> m_history;
    std::size_t m_historySize = 0;
};

bool wxWinUIInputTransition::HasAction(wxWinUIInputActionKind kind) const
{
    return std::any_of(actions.begin(), actions.end(),
        [kind](const wxWinUIInputAction& action)
        {
            return action.action == kind;
        });
}

std::size_t
wxWinUIInputTransition::CountActions(wxWinUIInputActionKind kind) const
{
    return static_cast<std::size_t>(std::count_if(
        actions.begin(), actions.end(),
        [kind](const wxWinUIInputAction& action)
        {
            return action.action == kind;
        }));
}

wxWinUIInputMapping
wxWinUIMapPointerUpdate(wxWinUIInputUpdateKind update)
{
    wxWinUIInputMapping mapping;
    switch ( update )
    {
        case wxWinUIInputUpdateKind::LeftPressed:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Press;
            mapping.button = wxWinUIInputButton::Left;
            break;
        case wxWinUIInputUpdateKind::LeftReleased:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Release;
            mapping.button = wxWinUIInputButton::Left;
            break;
        case wxWinUIInputUpdateKind::RightPressed:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Press;
            mapping.button = wxWinUIInputButton::Right;
            break;
        case wxWinUIInputUpdateKind::RightReleased:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Release;
            mapping.button = wxWinUIInputButton::Right;
            break;
        case wxWinUIInputUpdateKind::MiddlePressed:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Press;
            mapping.button = wxWinUIInputButton::Middle;
            break;
        case wxWinUIInputUpdateKind::MiddleReleased:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Release;
            mapping.button = wxWinUIInputButton::Middle;
            break;
        case wxWinUIInputUpdateKind::X1Pressed:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Press;
            mapping.button = wxWinUIInputButton::X1;
            break;
        case wxWinUIInputUpdateKind::X1Released:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Release;
            mapping.button = wxWinUIInputButton::X1;
            break;
        case wxWinUIInputUpdateKind::X2Pressed:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Press;
            mapping.button = wxWinUIInputButton::X2;
            break;
        case wxWinUIInputUpdateKind::X2Released:
            mapping.valid = true;
            mapping.kind = wxWinUIInputKind::Release;
            mapping.button = wxWinUIInputButton::X2;
            break;
        case wxWinUIInputUpdateKind::Other:
            break;
    }
    return mapping;
}

wxWinUIInputMapping wxWinUIMapPointerWheel(bool horizontal)
{
    wxWinUIInputMapping mapping;
    mapping.valid = true;
    mapping.kind = wxWinUIInputKind::Wheel;
    mapping.button = wxWinUIInputButton::None;
    mapping.horizontalWheel = horizontal;
    return mapping;
}

wxWinUIInputState::wxWinUIInputState(
    const wxWinUIClickSettings& clickSettings)
    : m_clickSettings(clickSettings),
      m_hover(new HoverState),
      m_presses(new PressTable),
      m_clicks(new ClickTable),
      m_cancels(new CancelTable),
      m_storm(new StormGuard)
{
}

wxWinUIInputState::~wxWinUIInputState() = default;

void wxWinUIInputState::ArmStormGuard(unsigned maxInterventions,
                                      unsigned repeatedCycles,
                                      unsigned maxPeriod,
                                      std::uint64_t maxGap)
{
    m_storm->Arm(maxInterventions, repeatedCycles, maxPeriod, maxGap);
}

void wxWinUIInputState::DisarmStormGuard()
{
    m_storm->Disarm();
}

bool wxWinUIInputState::IsStormGuardArmed() const
{
    return m_storm->IsArmed();
}

unsigned wxWinUIInputState::GetStormInterventionCount() const
{
    return m_storm->GetInterventionCount();
}

void wxWinUIInputState::LeaveHover(
    wxWinUIInputTransition& transition,
    const wxWinUIPointerSample& sample)
{
    if ( !m_hover->target.IsOk() )
        return;

    transition.actions.push_back(
        MakeAction(wxWinUIInputActionKind::HoverLeave,
                   m_hover->target, m_hover->area,
                   wxWinUIInputKind::Leave, wxWinUIInputButton::None,
                   m_hover->zone, sample));
    transition.actions[transition.actions.size() - 1].expectedHoverVersion =
        m_hoverVersion;
}

void wxWinUIInputState::EnterHover(
    wxWinUIInputTransition& transition,
    const wxWinUIRouteObservation& observation,
    const wxWinUIPointerSample& sample)
{
    if ( m_hover->target == observation.target &&
         m_hover->area == observation.area )
    {
        return;
    }

    const bool leavesCurrent = m_hover->target.IsOk();
    LeaveHover(transition, sample);

    wxWinUIInputAction enter =
        MakeAction(wxWinUIInputActionKind::HoverEnter,
                   observation.target, observation.area,
                   wxWinUIInputKind::Enter, wxWinUIInputButton::None,
                   observation.zone, sample);
    enter.expectedHoverVersion =
        m_hoverVersion + static_cast<std::uint64_t>(leavesCurrent);
    transition.actions.push_back(enter);
}

void wxWinUIInputState::CancelPress(wxWinUIInputTransition& transition,
                                    bool releaseCapture,
                                    bool allPointers,
                                    const wxWinUIPointerSample& sample)
{
    if ( !m_presses->activeButtons &&
         !(allPointers && releaseCapture) )
        return;

    std::array<bool, 5> cancelButton{};
    bool canceledCaptureOwner = false;
    bool survivingCaptureOwner = false;
    for ( unsigned index = 0;
          index < m_presses->buttons.size(); ++index )
    {
        const PressState& press = m_presses->buttons[index];
        if ( !press.IsActive() )
            continue;

        const bool cancel =
            allPointers ||
            (press.device == sample.device &&
             press.pointerId == sample.pointerId);
        cancelButton[index] = cancel;
        if ( m_captureTarget.IsOk() &&
             press.target.HasSameOwner(m_captureTarget) )
        {
            if ( cancel )
                canceledCaptureOwner = true;
            else
                survivingCaptureOwner = true;
        }
    }

    const bool shouldReleaseCapture =
        releaseCapture && m_captureTarget.IsOk() &&
        (allPointers ||
         (canceledCaptureOwner && !survivingCaptureOwner));
    // Capture release is first: it is the only cancellation action whose
    // effect cannot be recovered by a later generation-safe WM_CANCELMODE.
    if ( shouldReleaseCapture )
    {
        transition.actions.push_back(
            MakeAction(wxWinUIInputActionKind::ReleaseCapture,
                       m_captureTarget,
                       wxWinUIInputArea::Client,
                       wxWinUIInputKind::Cancel,
                       wxWinUIInputButton::None, 0, sample));
        m_captureTarget = wxWinUIInputTargetKey();
        // Capture participates in the two-phase preflight of every button,
        // including a button with no committed PressState yet.
        for ( std::uint64_t& version : m_buttonVersions )
            ++version;
    }

    for ( unsigned index = 0;
          index < m_presses->buttons.size(); ++index )
    {
        PressState& press = m_presses->buttons[index];
        if ( !press.IsActive() || !cancelButton[index] )
            continue;

        wxWinUIInputAction cancel =
            MakeAction(wxWinUIInputActionKind::CancelPress,
                       press.target, press.area,
                       wxWinUIInputKind::Cancel, press.button,
                       press.zone, sample);
        cancel.device = press.device;
        cancel.pointerId = press.pointerId;
        cancel.timestamp = press.downTime;
        cancel.gestureSerial = press.gestureSerial;
        transition.actions.push_back(cancel);

        CancelState& canceled = m_cancels->buttons[index];
        canceled.target = press.target;
        canceled.button = press.button;
        canceled.gestureSerial = press.gestureSerial;
        m_clicks->buttons[index] = ClickState();
        press = PressState();
        --m_presses->activeButtons;
        if ( !allPointers )
            ++m_buttonVersions[index];
    }

    if ( allPointers )
    {
        *m_clicks = ClickTable();
        for ( std::uint64_t& version : m_buttonVersions )
            ++version;
        if ( !releaseCapture )
            m_captureTarget = wxWinUIInputTargetKey();
    }
}

wxWinUIInputTransition wxWinUIInputState::NativeCaptureChanged(
    const wxWinUIInputTargetKey& owner,
    const wxWinUIPointerSample& sample)
{
    wxWinUIInputTransition transition;
    if ( owner == m_captureTarget )
        return transition;

    m_storm->ResetHistory();

    // Capture is part of the adapter preflight contract even when no
    // committed press is canceled below. Invalidate every prepared button
    // action before publishing the new owner so an outer Press/Release can
    // never commit using a capture snapshot superseded by reentrant input.
    for ( std::uint64_t& version : m_buttonVersions )
        ++version;

    // A control may receive DOWN on an implementation child and call
    // SetCapture() on its wx shell. They are different dispatch HWNDs but the
    // same logical owner, so preserve that gesture. A transfer cancels only
    // presses which can no longer receive their matching release; a press
    // belonging to the new owner must survive.
    if ( m_presses->activeButtons )
    {
        for ( PressState& press : m_presses->buttons )
        {
            if ( !press.IsActive() ||
                 (owner.IsOk() && press.target.HasSameOwner(owner)) )
            {
                continue;
            }

            wxWinUIInputAction cancel =
                MakeAction(wxWinUIInputActionKind::CancelPress,
                           press.target, press.area,
                           wxWinUIInputKind::Cancel, press.button,
                           press.zone, sample);
            cancel.device = press.device;
            cancel.pointerId = press.pointerId;
            cancel.timestamp = press.downTime;
            cancel.gestureSerial = press.gestureSerial;
            transition.actions.push_back(cancel);

            const int index = ButtonIndex(press.button);
            if ( index >= 0 )
            {
                const unsigned buttonIndex =
                    static_cast<unsigned>(index);
                CancelState& canceled =
                    m_cancels->buttons[buttonIndex];
                canceled.target = press.target;
                canceled.button = press.button;
                canceled.gestureSerial = press.gestureSerial;
                m_clicks->buttons[buttonIndex] = ClickState();
            }
            press = PressState();
            --m_presses->activeButtons;
        }
    }

    m_captureTarget = owner;
    return transition;
}

bool wxWinUIInputState::CommitBeforeDispatch(
    const wxWinUIInputAction& action)
{
    if ( action.action != wxWinUIInputActionKind::Dispatch )
        return false;

    if ( action.kind != wxWinUIInputKind::Press &&
         action.kind != wxWinUIInputKind::DoubleClick &&
         action.kind != wxWinUIInputKind::Release )
    {
        return true;
    }

    const int index = ButtonIndex(action.button);
    if ( index < 0 || !action.gestureSerial )
        return false;

    PressState& press =
        m_presses->buttons[static_cast<unsigned>(index)];
    ClickState& click =
        m_clicks->buttons[static_cast<unsigned>(index)];

    if ( action.kind == wxWinUIInputKind::Press ||
         action.kind == wxWinUIInputKind::DoubleClick )
    {
        if ( press.IsActive() ||
             click.gestureSerial != action.expectedClickSerial ||
             m_buttonVersions[static_cast<unsigned>(index)] !=
                action.expectedStateVersion ||
             m_cancels->buttons[static_cast<unsigned>(index)]
                .gestureSerial != 0 )
        {
            return false;
        }

        press.target = action.target;
        press.area = action.area;
        press.button = action.button;
        press.zone = action.zone;
        press.downX = action.screenX;
        press.downY = action.screenY;
        press.downTime = action.timestamp;
        press.doubleClick =
            action.kind == wxWinUIInputKind::DoubleClick;
        press.device = action.device;
        press.pointerId = action.pointerId;
        press.gestureSerial = action.gestureSerial;
        ++m_presses->activeButtons;
        ++m_buttonVersions[static_cast<unsigned>(index)];
        if ( press.doubleClick )
            click = ClickState();
        return true;
    }

    if ( !action.balancesPress ||
         !press.IsActive() ||
         press.gestureSerial != action.gestureSerial ||
         press.target != action.balanceTarget ||
         press.button != action.button ||
         press.device != action.device ||
         press.pointerId != action.pointerId )
    {
        return false;
    }
    if ( m_buttonVersions[static_cast<unsigned>(index)] !=
            action.expectedStateVersion )
    {
        return false;
    }

    if ( action.completesClick &&
         !press.doubleClick &&
         press.clickEligible )
    {
        click.target = press.target;
        click.area = press.area;
        click.button = press.button;
        click.zone = press.zone;
        click.x = press.downX;
        click.y = press.downY;
        click.time = press.downTime;
        click.device = press.device;
        click.gestureSerial = press.gestureSerial;
    }
    else
    {
        click = ClickState();
    }

    press = PressState();
    --m_presses->activeButtons;
    ++m_buttonVersions[static_cast<unsigned>(index)];
    return true;
}

bool wxWinUIInputState::CommitHoverAction(
    const wxWinUIInputAction& action)
{
    if ( action.expectedHoverVersion != m_hoverVersion )
        return false;

    if ( action.action == wxWinUIInputActionKind::HoverLeave )
    {
        if ( m_hover->target != action.target ||
             m_hover->area != action.area ||
             m_hover->zone != action.zone )
        {
            return false;
        }

        *m_hover = HoverState();
        ++m_hoverVersion;
        return true;
    }

    if ( action.action == wxWinUIInputActionKind::HoverEnter )
    {
        if ( m_hover->target.IsOk() || !action.target.IsOk() )
            return false;

        m_hover->target = action.target;
        m_hover->area = action.area;
        m_hover->zone = action.zone;
        ++m_hoverVersion;
        return true;
    }

    return false;
}

bool wxWinUIInputState::AbortCommittedHoverEnter(
    const wxWinUIInputAction& action)
{
    if ( action.action != wxWinUIInputActionKind::HoverEnter ||
         m_hoverVersion != action.expectedHoverVersion + 1 ||
         m_hover->target != action.target ||
         m_hover->area != action.area ||
         m_hover->zone != action.zone )
    {
        return false;
    }

    *m_hover = HoverState();
    ++m_hoverVersion;
    return true;
}

bool wxWinUIInputState::SuppressClickForGesture(
    const wxWinUIInputAction& action)
{
    if ( action.action != wxWinUIInputActionKind::Dispatch ||
         (action.kind != wxWinUIInputKind::Press &&
          action.kind != wxWinUIInputKind::DoubleClick) ||
         !action.gestureSerial )
    {
        return false;
    }

    const int index = ButtonIndex(action.button);
    if ( index < 0 )
        return false;

    PressState& press =
        m_presses->buttons[static_cast<unsigned>(index)];
    if ( !press.IsActive() ||
         press.gestureSerial != action.gestureSerial ||
         press.target != action.target ||
         press.button != action.button ||
         press.device != action.device ||
         press.pointerId != action.pointerId )
    {
        return false;
    }

    if ( press.clickEligible )
    {
        press.clickEligible = false;
        ++m_buttonVersions[static_cast<unsigned>(index)];
    }
    return true;
}

wxWinUIInputTransition wxWinUIInputState::CancelActiveGesture(
    const wxWinUIInputAction& gesture,
    const wxWinUIPointerSample& sample)
{
    wxWinUIInputTransition transition;
    if ( !HasActiveGesture(gesture) )
        return transition;

    const int index = ButtonIndex(gesture.button);
    if ( index < 0 )
        return transition;

    const unsigned buttonIndex = static_cast<unsigned>(index);
    PressState& press = m_presses->buttons[buttonIndex];
    m_storm->ResetHistory();

    bool survivingCaptureOwner = false;
    if ( m_captureTarget.IsOk() &&
         press.target.HasSameOwner(m_captureTarget) )
    {
        for ( unsigned other = 0;
              other < m_presses->buttons.size(); ++other )
        {
            if ( other == buttonIndex )
                continue;

            const PressState& survivor = m_presses->buttons[other];
            if ( survivor.IsActive() &&
                 survivor.target.HasSameOwner(m_captureTarget) )
            {
                survivingCaptureOwner = true;
                break;
            }
        }

        if ( !survivingCaptureOwner )
        {
            transition.actions.push_back(
                MakeAction(wxWinUIInputActionKind::ReleaseCapture,
                           m_captureTarget,
                           wxWinUIInputArea::Client,
                           wxWinUIInputKind::Cancel,
                           wxWinUIInputButton::None, 0, sample));
            m_captureTarget = wxWinUIInputTargetKey();
            for ( std::uint64_t& version : m_buttonVersions )
                ++version;
        }
    }

    wxWinUIInputAction cancellation =
        MakeAction(wxWinUIInputActionKind::CancelPress,
                   press.target, press.area,
                   wxWinUIInputKind::Cancel, press.button,
                   press.zone, sample);
    cancellation.device = press.device;
    cancellation.pointerId = press.pointerId;
    cancellation.timestamp = press.downTime;
    cancellation.gestureSerial = press.gestureSerial;
    transition.actions.push_back(cancellation);

    CancelState& canceled = m_cancels->buttons[buttonIndex];
    canceled.target = press.target;
    canceled.button = press.button;
    canceled.gestureSerial = press.gestureSerial;
    m_clicks->buttons[buttonIndex] = ClickState();
    press = PressState();
    --m_presses->activeButtons;
    ++m_buttonVersions[buttonIndex];
    transition.cancelSource = true;
    return transition;
}

wxWinUIInputTransition wxWinUIInputState::FinishSynchronousDown(
    const wxWinUIInputAction& gesture,
    const wxWinUIPointerSample& sample,
    std::uint64_t elapsedMilliseconds,
    bool contactActive)
{
    if ( elapsedMilliseconds <= 100 || contactActive )
        return wxWinUIInputTransition();

    wxWinUIPointerSample cancel = sample;
    cancel.kind = wxWinUIInputKind::Cancel;
    cancel.button = gesture.button;
    return CancelActiveGesture(gesture, cancel);
}

bool wxWinUIInputState::AbortUndeliverableRelease(
    const wxWinUIInputAction& action,
    wxWinUIInputAction *cancellation)
{
    if ( cancellation )
        *cancellation = wxWinUIInputAction();
    if ( !cancellation ||
         action.action != wxWinUIInputActionKind::Dispatch ||
         action.kind != wxWinUIInputKind::Release ||
         !action.balancesPress || !action.gestureSerial )
    {
        return false;
    }

    const int index = ButtonIndex(action.button);
    if ( index < 0 )
        return false;

    const unsigned buttonIndex = static_cast<unsigned>(index);
    PressState& press = m_presses->buttons[buttonIndex];
    if ( !press.IsActive() ||
         press.gestureSerial != action.gestureSerial ||
         press.target != action.balanceTarget ||
         press.button != action.button ||
         press.device != action.device ||
         press.pointerId != action.pointerId )
    {
        return false;
    }

    *cancellation = action;
    cancellation->action = wxWinUIInputActionKind::CancelPress;
    cancellation->target = press.target;
    cancellation->area = press.area;
    cancellation->kind = wxWinUIInputKind::Cancel;
    cancellation->zone = press.zone;
    cancellation->timestamp = press.downTime;

    CancelState& canceled = m_cancels->buttons[buttonIndex];
    canceled.target = press.target;
    canceled.button = press.button;
    canceled.gestureSerial = press.gestureSerial;
    m_clicks->buttons[buttonIndex] = ClickState();
    press = PressState();
    --m_presses->activeButtons;
    ++m_buttonVersions[buttonIndex];
    return true;
}

bool wxWinUIInputState::CanDeliverCancellation(
    const wxWinUIInputAction& action) const
{
    if ( action.action != wxWinUIInputActionKind::CancelPress ||
         !action.gestureSerial )
    {
        return false;
    }

    const int index = ButtonIndex(action.button);
    if ( index < 0 )
        return false;

    const CancelState& canceled =
        m_cancels->buttons[static_cast<unsigned>(index)];
    return canceled.gestureSerial == action.gestureSerial &&
           canceled.target == action.target &&
           canceled.button == action.button;
}

void wxWinUIInputState::AcknowledgeCancellation(
    const wxWinUIInputAction& action)
{
    if ( !CanDeliverCancellation(action) )
        return;

    const int index = ButtonIndex(action.button);
    m_cancels->buttons[static_cast<unsigned>(index)] = CancelState();
}

bool wxWinUIInputState::HasActivePress(
    wxWinUIInputDevice device,
    std::uint32_t pointerId,
    wxWinUIInputButton button) const
{
    return std::any_of(
        m_presses->buttons.begin(), m_presses->buttons.end(),
        [device, pointerId, button](const PressState& press)
        {
            return press.IsActive() &&
                   press.device == device &&
                   press.pointerId == pointerId &&
                   (button == wxWinUIInputButton::None ||
                    press.button == button);
        });
}

bool wxWinUIInputState::HasAnyActivePress() const
{
    return m_presses->activeButtons != 0;
}

bool wxWinUIInputState::HasActiveGesture(
    const wxWinUIInputAction& action) const
{
    if ( action.action != wxWinUIInputActionKind::Dispatch ||
         (action.kind != wxWinUIInputKind::Press &&
          action.kind != wxWinUIInputKind::DoubleClick) ||
         !action.gestureSerial )
    {
        return false;
    }

    const int index = ButtonIndex(action.button);
    if ( index < 0 )
        return false;

    const PressState& press =
        m_presses->buttons[static_cast<unsigned>(index)];
    return press.IsActive() &&
           press.gestureSerial == action.gestureSerial &&
           press.target == action.target &&
           press.button == action.button &&
           press.device == action.device &&
           press.pointerId == action.pointerId;
}

bool wxWinUIInputState::HasActivePressForOwner(
    const wxWinUIInputTargetKey& owner) const
{
    return owner.IsOk() &&
           std::any_of(
               m_presses->buttons.begin(), m_presses->buttons.end(),
               [&owner](const PressState& press)
               {
                   return press.IsActive() &&
                          press.target.HasSameOwner(owner);
               });
}

bool wxWinUIInputState::IsDoubleClick(
    const wxWinUIPointerSample& sample,
    const wxWinUIRouteObservation& observation,
    unsigned buttonIndex) const
{
    const ClickState& click = m_clicks->buttons[buttonIndex];
    if ( !click.target.IsOk() ||
         click.target != observation.target ||
         click.button != sample.button ||
         click.device != sample.device ||
         click.area != observation.area ||
         click.zone != observation.zone )
    {
        return false;
    }

    if ( observation.area == wxWinUIInputArea::Client &&
         !observation.clientDoubleClicks )
    {
        return false;
    }

    if ( sample.timestamp < click.time ||
         sample.timestamp - click.time > m_clickSettings.maxInterval )
    {
        return false;
    }

    const auto distance = [](int first, int second)
    {
        const std::int64_t difference =
            static_cast<std::int64_t>(first) - second;
        return static_cast<std::uint64_t>(
            difference < 0 ? -difference : difference);
    };

    // The system metrics describe the full rectangle centred on the first
    // click, hence the factor of two. Widen before subtraction/multiplication
    // so even hostile INT_MIN/INT_MAX samples remain defined.
    return 2 * distance(sample.screenX, click.x) <=
                static_cast<std::uint64_t>(
                    std::max(0, m_clickSettings.rectangleWidth)) &&
           2 * distance(sample.screenY, click.y) <=
                static_cast<std::uint64_t>(
                    std::max(0, m_clickSettings.rectangleHeight));
}

wxWinUIInputTransition wxWinUIInputState::Route(
    const wxWinUIPointerSample& originalSample,
    const wxWinUIRouteObservation& observation)
{
    wxWinUIInputTransition transition;

    // Device policy precedes global bridge termination so capture loss from a
    // secondary contact cannot cancel the primary compatibility gesture.
    if ( originalSample.device != wxWinUIInputDevice::Mouse &&
         !originalSample.isPrimary )
    {
        transition.disposition = wxWinUIInputDisposition::SecondaryPointer;
        return transition;
    }
    if ( originalSample.isCompatibilityMouse )
    {
        transition.disposition =
            wxWinUIInputDisposition::CompatibilityDuplicate;
        return transition;
    }

    // Capture termination is independent of visual hit resolution. USER32
    // can report it after the pointer has left the island or while a visual
    // query is unstable, and filtering it would strand pressed state.
    if ( originalSample.kind == wxWinUIInputKind::Cancel ||
         originalSample.kind == wxWinUIInputKind::CaptureLost )
    {
        m_storm->ResetHistory();
        if ( originalSample.kind == wxWinUIInputKind::Cancel )
        {
            CancelPress(transition, true,
                        originalSample.interruptAll, originalSample);
            // WM_CANCELMODE starts a new click epoch even when it arrives
            // between gestures and there is no pressed/capture state.
        }
        else if ( m_presses->activeButtons )
        {
            CancelPress(transition, false,
                        originalSample.interruptAll, originalSample);
        }
        else if ( originalSample.interruptAll )
        {
            // Normal USER32 capture release after UP must not erase the
            // completed click used to classify the next DOWN.
            m_captureTarget = wxWinUIInputTargetKey();
            for ( std::uint64_t& version : m_buttonVersions )
                ++version;
        }
        transition.cancelSource = !transition.actions.empty();
        return transition;
    }

    // V0 policy: a primary touch or pen point is translated exactly once to
    // mouse compatibility semantics. Secondary contacts are ignored, as are
    // the promoted mouse duplicates explicitly identified by the adapter.
    wxWinUIPointerSample sample = originalSample;
    if ( sample.device != wxWinUIInputDevice::Mouse &&
         (sample.kind == wxWinUIInputKind::Press ||
          sample.kind == wxWinUIInputKind::Release) &&
         sample.button == wxWinUIInputButton::None )
    {
        sample.button = wxWinUIInputButton::Left;
    }

    wxWinUIRouteObservation effective = observation;
    bool releaseRoutesPress = false;
    bool releaseCompletesClick = false;
    if ( sample.kind == wxWinUIInputKind::Release )
    {
        const int index = ButtonIndex(sample.button);
        if ( index >= 0 )
        {
            const PressState& press =
                m_presses->buttons[static_cast<unsigned>(index)];
            releaseRoutesPress =
                press.IsActive() &&
                press.device == sample.device &&
                press.pointerId == sample.pointerId;
            if ( releaseRoutesPress )
            {
                releaseCompletesClick =
                    observation.surface == wxWinUIInputSurface::Native &&
                    observation.target == press.target &&
                    observation.area == press.area &&
                    observation.zone == press.zone &&
                    (!m_captureTarget.IsOk() ||
                     m_captureTarget.HasSameOwner(press.target));
            }
        }
    }

    const bool captureRoutes =
        m_captureTarget.IsOk() &&
        (sample.kind == wxWinUIInputKind::Move ||
         sample.kind == wxWinUIInputKind::Press ||
         sample.kind == wxWinUIInputKind::Release);
    if ( captureRoutes )
    {
        // Native mouse capture is authoritative even outside the target and
        // always produces client messages. This is the intentional exception
        // to XAML/indeterminate pass-through during an active native gesture.
        effective.surface = wxWinUIInputSurface::Native;
        effective.target = m_captureTarget;
        effective.area = wxWinUIInputArea::Client;
        effective.zone = 0;
        effective.clientDoubleClicks = false;
    }
    else if ( releaseRoutesPress )
    {
        const PressState& press =
            m_presses->buttons[
                static_cast<unsigned>(ButtonIndex(sample.button))];
        // The XAML input site owns the physical gesture even when the native
        // target did not call SetCapture(). Balance its DOWN with exactly one
        // UP on the original generation-safe target.
        effective.surface = wxWinUIInputSurface::Native;
        effective.target = press.target;
        effective.area = press.area;
        effective.zone = press.zone;
        effective.clientDoubleClicks = false;
    }

    if ( effective.surface == wxWinUIInputSurface::Indeterminate )
    {
        transition.disposition = wxWinUIInputDisposition::Indeterminate;
        return transition;
    }

    if ( effective.surface == wxWinUIInputSurface::Xaml )
    {
        // A confirmed transition onto XAML can close the old native hover,
        // but never routes the current sample and never cancels XAML.
        if ( sample.kind == wxWinUIInputKind::Move ||
             sample.kind == wxWinUIInputKind::Enter ||
             sample.kind == wxWinUIInputKind::Leave )
        {
            LeaveHover(transition, sample);
        }
        transition.disposition = wxWinUIInputDisposition::Xaml;
        return transition;
    }

    if ( effective.surface == wxWinUIInputSurface::Outside )
    {
        // Unlike an indeterminate query, Outside is authoritative: retire
        // the exact client/non-client hover without claiming the source.
        LeaveHover(transition, sample);
        transition.disposition = wxWinUIInputDisposition::Outside;
        return transition;
    }

    if ( !effective.target.IsOk() )
    {
        transition.disposition = wxWinUIInputDisposition::InvalidTarget;
        return transition;
    }

    const bool closeHoverBeforeDispatch =
        (captureRoutes || releaseRoutesPress) &&
        sample.kind == wxWinUIInputKind::Release &&
        (observation.surface == wxWinUIInputSurface::Xaml ||
         observation.surface == wxWinUIInputSurface::Outside);
    return RouteNative(sample, effective, closeHoverBeforeDispatch,
                       releaseCompletesClick);
}

wxWinUIInputTransition wxWinUIInputState::RouteBoundary(
    const wxWinUIPointerSample& sample)
{
    wxWinUIInputTransition transition;
    transition.disposition = wxWinUIInputDisposition::Outside;

    if ( sample.device != wxWinUIInputDevice::Mouse &&
         !sample.isPrimary )
    {
        transition.disposition = wxWinUIInputDisposition::SecondaryPointer;
        return transition;
    }
    if ( sample.isCompatibilityMouse )
    {
        transition.disposition =
            wxWinUIInputDisposition::CompatibilityDuplicate;
        return transition;
    }
    if ( sample.kind != wxWinUIInputKind::Enter &&
         sample.kind != wxWinUIInputKind::Leave )
    {
        transition.disposition = wxWinUIInputDisposition::InvalidTarget;
        return transition;
    }

    wxWinUIRouteObservation boundary;
    boundary.surface = wxWinUIInputSurface::Outside;
    const StormGuard::Result storm =
        m_storm->Observe(sample, boundary, m_captureTarget);
    transition.stormDecision = storm.decision;
    if ( storm.decision != wxWinUIInputStormDecision::Deliver )
    {
        wxWinUIInputAction action =
            MakeAction(wxWinUIInputActionKind::BreakStorm,
                       wxWinUIInputTargetKey(),
                       wxWinUIInputArea::Client,
                       sample.kind, wxWinUIInputButton::None, 0, sample);
        action.stormPeriod = storm.period;
        transition.actions.push_back(action);
    }

    if ( sample.kind == wxWinUIInputKind::Leave )
        LeaveHover(transition, sample);

    // A root boundary notification is evidence for the breaker and hover,
    // never a native pointer message and never a reason to claim XAML.
    transition.cancelSource = false;
    return transition;
}

wxWinUIInputTransition wxWinUIInputState::RouteNative(
    wxWinUIPointerSample sample,
    const wxWinUIRouteObservation& observation,
    bool closeHoverBeforeDispatch,
    bool releaseCompletesClick)
{
    wxWinUIInputTransition transition;
    transition.disposition = wxWinUIInputDisposition::Routed;

    const StormGuard::Result storm =
        m_storm->Observe(sample, observation, m_captureTarget);
    transition.stormDecision = storm.decision;
    if ( storm.decision != wxWinUIInputStormDecision::Deliver )
    {
        wxWinUIInputAction action =
            MakeAction(wxWinUIInputActionKind::BreakStorm,
                       observation.target, observation.area,
                       sample.kind, sample.button, observation.zone, sample);
        action.stormPeriod = storm.period;
        transition.actions.push_back(action);

        if ( storm.decision ==
                wxWinUIInputStormDecision::SuppressRedundant )
        {
            transition.disposition =
                wxWinUIInputDisposition::StormSuppressed;
            transition.cancelSource = true;
            return transition;
        }
    }

    if ( closeHoverBeforeDispatch &&
         sample.kind != wxWinUIInputKind::Release )
        LeaveHover(transition, sample);

    switch ( sample.kind )
    {
        case wxWinUIInputKind::Move:
            EnterHover(transition, observation, sample);
            transition.actions.push_back(
                MakeAction(wxWinUIInputActionKind::Dispatch,
                           observation.target, observation.area,
                           wxWinUIInputKind::Move,
                           wxWinUIInputButton::None,
                           observation.zone, sample));
            transition.cancelSource = true;
            break;

        case wxWinUIInputKind::Enter:
            EnterHover(transition, observation, sample);
            transition.cancelSource = true;
            break;

        case wxWinUIInputKind::Leave:
            if ( m_hover->target == observation.target &&
                 m_hover->area == observation.area )
            {
                LeaveHover(transition, sample);
                transition.cancelSource = true;
            }
            break;

        case wxWinUIInputKind::Press:
        {
            const int index = ButtonIndex(sample.button);
            if ( index < 0 )
            {
                transition.disposition =
                    wxWinUIInputDisposition::InvalidTarget;
                break;
            }

            PressState& press =
                m_presses->buttons[static_cast<unsigned>(index)];
            if ( press.IsActive() )
            {
                // A physical button cannot be pressed twice without a
                // release/cancel. Fail closed rather than corrupting another
                // button's independent pressed state.
                transition.disposition =
                    wxWinUIInputDisposition::InvalidTarget;
                transition.cancelSource = true;
                break;
            }

            const bool doubleClick =
                IsDoubleClick(sample, observation,
                              static_cast<unsigned>(index));

            // Press alone never implies SetCapture(). The adapter reports any
            // capture established re-entrantly through NativeCaptureChanged.
            wxWinUIInputAction down =
                MakeAction(wxWinUIInputActionKind::Dispatch,
                           observation.target, observation.area,
                           doubleClick ? wxWinUIInputKind::DoubleClick
                                       : wxWinUIInputKind::Press,
                           sample.button, observation.zone, sample);
            if ( ++m_nextGestureSerial == 0 )
                ++m_nextGestureSerial;
            down.gestureSerial = m_nextGestureSerial;
            down.expectedClickSerial =
                m_clicks->buttons[static_cast<unsigned>(index)]
                    .gestureSerial;
            down.expectedStateVersion =
                m_buttonVersions[static_cast<unsigned>(index)];
            transition.actions.push_back(down);
            transition.cancelSource = true;
            break;
        }

        case wxWinUIInputKind::Release:
        {
            const int index = ButtonIndex(sample.button);
            if ( index < 0 )
            {
                transition.disposition =
                    wxWinUIInputDisposition::InvalidTarget;
                break;
            }

            wxWinUIInputTargetKey target = observation.target;
            wxWinUIInputArea area = observation.area;
            int zone = observation.zone;
            PressState& press =
                m_presses->buttons[static_cast<unsigned>(index)];
            const bool matchesPress =
                press.IsActive() &&
                press.device == sample.device &&
                press.pointerId == sample.pointerId;

            // Never synthesize an orphan UP. This also consumes the release
            // of a primary touch/pen press rejected because another device
            // already owned the same mouse-compatible button.
            if ( !matchesPress )
            {
                transition.cancelSource = true;
                break;
            }

            wxWinUIInputAction release =
                MakeAction(wxWinUIInputActionKind::Dispatch,
                           target, area, wxWinUIInputKind::Release,
                           sample.button, zone, sample);
            release.gestureSerial = press.gestureSerial;
            release.expectedStateVersion =
                m_buttonVersions[static_cast<unsigned>(index)];
            release.balanceTarget = press.target;
            release.balancesPress = true;
            release.completesClick =
                releaseCompletesClick && !press.doubleClick;
            transition.actions.push_back(release);
            // Keep the balancing UP ahead of callback-bearing hover cleanup:
            // a WM_MOUSELEAVE handler may pump a nested input event, but the
            // original native DOWN must never be stranded by that reentry.
            if ( closeHoverBeforeDispatch )
                LeaveHover(transition, sample);
            transition.cancelSource = true;
            break;
        }

        case wxWinUIInputKind::Wheel:
            transition.actions.push_back(
                MakeAction(wxWinUIInputActionKind::Dispatch,
                           observation.target, observation.area,
                           wxWinUIInputKind::Wheel,
                           wxWinUIInputButton::None,
                           observation.zone, sample));
            transition.cancelSource = true;
            break;

        case wxWinUIInputKind::Cancel:
            CancelPress(transition, true, sample.interruptAll, sample);
            transition.cancelSource = !transition.actions.empty();
            break;

        case wxWinUIInputKind::CaptureLost:
            CancelPress(transition, false, sample.interruptAll, sample);
            transition.cancelSource = !transition.actions.empty();
            break;

        case wxWinUIInputKind::DoubleClick:
            // DoubleClick is an output semantic; platform adapters feed only
            // raw Press/Release samples into the tracker.
            transition.disposition = wxWinUIInputDisposition::InvalidTarget;
            break;
    }

    return transition;
}

#endif // wxUSE_WINUI3
