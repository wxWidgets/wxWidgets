/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/appprog.cpp
// Purpose:     Implementation of wxAppProgressIndicator.
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-09-05
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_TASKBARBUTTON

#ifndef WX_PRECOMP
    #include "wx/toplevel.h"
#endif

#include "wx/appprogress.h"
#include "wx/msw/taskbarbutton.h"
#include "wx/msw/private/taskbarbutton.h"
#include "wx/private/windowlifetime.h"
#include "wx/weakref.h"

#include <cstdint>
#include <memory>
#include <vector>

struct wxMSWAppProgressState
{
    enum class Mode
    {
        Reset,
        Value,
        Pulse
    };

    struct Target
    {
        wxWeakRef<wxWindow> window;
        // A call already inside the native seam owns a copy of this lease even
        // if a nested mutation removes the target or destroys the indicator.
        std::shared_ptr<wxTaskBarButton> taskBarButton;
    };

    bool retired { false };
    bool applying { false };
    bool applyPending { false };
    std::uint64_t desiredRevision { 1 };
    bool allTopLevels { false };
    wxWeakRef<wxWindow> explicitParent;
    int maxValue { 100 };
    int value { 0 };
    Mode mode { Mode::Reset };
    std::vector<Target> targets;
};

namespace
{

std::vector<std::weak_ptr<wxMSWAppProgressState>> gs_appProgressStates;

void wxMSWBumpAppProgressRevision(
    const std::shared_ptr<wxMSWAppProgressState>& state)
{
    if ( ++state->desiredRevision == 0 )
        ++state->desiredRevision;
}

bool wxMSWAppProgressStateIsCurrent(
    const std::shared_ptr<wxMSWAppProgressState>& state,
    std::uint64_t revision)
{
    return state && !state->retired &&
           state->desiredRevision == revision;
}

bool wxMSWAppProgressTargetIsAvailable(
    const wxMSWAppProgressState::Target& target)
{
    wxWindow* const window = target.window.get();
    return window && !wxWindowIsUnavailableForCallbacks(window) &&
           target.taskBarButton &&
           static_cast<wxTaskBarButtonImpl*>(
               target.taskBarButton.get())->IsAvailable();
}

struct wxMSWAppProgressDesiredSnapshot
{
    std::uint64_t revision { 0 };
    int maxValue { 100 };
    int value { 0 };
    wxMSWAppProgressState::Mode mode {
        wxMSWAppProgressState::Mode::Reset
    };
};

void wxMSWApplyAppProgressTarget(
    const std::shared_ptr<wxMSWAppProgressState>& state,
    const wxMSWAppProgressDesiredSnapshot& desired,
    wxMSWAppProgressState::Target target)
{
    if ( !wxMSWAppProgressStateIsCurrent(state, desired.revision) ||
         !wxMSWAppProgressTargetIsAvailable(target) )
    {
        return;
    }

    const std::shared_ptr<wxTaskBarButton> taskbar = target.taskBarButton;
    taskbar->SetProgressRange(desired.maxValue);
    if ( !wxMSWAppProgressStateIsCurrent(state, desired.revision) )
        return;

    // Do not touch target, its weak window, or the indicator wrapper after the
    // following call: every branch crosses the injectable native seam.
    switch ( desired.mode )
    {
        case wxMSWAppProgressState::Mode::Reset:
            taskbar->SetProgressState(wxTASKBAR_BUTTON_NO_PROGRESS);
            break;

        case wxMSWAppProgressState::Mode::Value:
            taskbar->SetProgressValue(desired.value);
            break;

        case wxMSWAppProgressState::Mode::Pulse:
            taskbar->PulseProgress();
            break;
    }
}

bool wxMSWAppProgressHasTarget(
    const wxMSWAppProgressState& state,
    wxWindow* window)
{
    for ( const wxMSWAppProgressState::Target& target : state.targets )
    {
        if ( target.window.get() == window )
            return true;
    }
    return false;
}

bool wxMSWReconcileAppProgressTargets(
    const std::shared_ptr<wxMSWAppProgressState>& state,
    std::uint64_t revision)
{
    if ( !wxMSWAppProgressStateIsCurrent(state, revision) )
        return false;

    // Availability checks are sidecar-only and don't cross the native seam.
    // Still rebuild the vector transactionally so no iterator/reference is
    // held when later controller construction invokes the factory seam.
    std::vector<wxMSWAppProgressState::Target> liveTargets;
    liveTargets.reserve(state->targets.size());
    for ( const wxMSWAppProgressState::Target& target : state->targets )
    {
        if ( wxMSWAppProgressTargetIsAvailable(target) )
            liveTargets.push_back(target);
    }
    if ( !wxMSWAppProgressStateIsCurrent(state, revision) )
        return false;
    state->targets.swap(liveTargets);

    std::vector<wxWeakRef<wxWindow>> candidates;
    if ( state->allTopLevels )
    {
        // Creating the COM seam can dispatch test/application code. Snapshot
        // weak identities before it so a nested TLW destroy can't invalidate a
        // wxTopLevelWindows iterator held across that boundary.
        candidates.reserve(wxTopLevelWindows.size());
        for ( wxWindowList::const_iterator it = wxTopLevelWindows.begin();
              it != wxTopLevelWindows.end(); ++it )
        {
            wxWindow* const window = *it;
            if ( window && !wxWindowIsUnavailableForCallbacks(window) )
                candidates.emplace_back(window);
        }
    }
    else if ( wxWindow* const parent = state->explicitParent.get() )
    {
        if ( !wxWindowIsUnavailableForCallbacks(parent) )
            candidates.emplace_back(parent);
    }

    for ( const wxWeakRef<wxWindow>& candidate : candidates )
    {
        if ( !wxMSWAppProgressStateIsCurrent(state, revision) )
            return false;

        wxWindow* const window = candidate.get();
        if ( !window || wxWindowIsUnavailableForCallbacks(window) ||
             wxMSWAppProgressHasTarget(*state, window) )
        {
            continue;
        }

        std::shared_ptr<wxTaskBarButton> taskbar(
            wxTaskBarButton::New(window));
        if ( !wxMSWAppProgressStateIsCurrent(state, revision) )
        {
            state->applyPending = !state->retired;
            return false;
        }

        wxWindow* const liveWindow = candidate.get();
        if ( !taskbar || liveWindow != window ||
             wxWindowIsUnavailableForCallbacks(liveWindow) ||
             wxMSWAppProgressHasTarget(*state, liveWindow) ||
             !static_cast<wxTaskBarButtonImpl*>(
                 taskbar.get())->IsAvailable() )
        {
            continue;
        }

        wxMSWAppProgressState::Target target;
        target.window = candidate;
        target.taskBarButton = taskbar;
        state->targets.push_back(std::move(target));
    }

    return wxMSWAppProgressStateIsCurrent(state, revision);
}

void wxMSWApplyAppProgress(
    const std::shared_ptr<wxMSWAppProgressState>& state)
{
    if ( !state || state->retired )
        return;

    if ( state->applying )
    {
        state->applyPending = true;
        return;
    }

    state->applying = true;
    unsigned passes = 0;
    do
    {
        state->applyPending = false;
        wxMSWAppProgressDesiredSnapshot desired;
        desired.revision = state->desiredRevision;
        desired.maxValue = state->maxValue;
        desired.value = state->value;
        desired.mode = state->mode;

        if ( !wxMSWReconcileAppProgressTargets(state, desired.revision) )
        {
            state->applyPending = !state->retired;
        }
        else
        {
            // Copy controller leases before the first native call. A nested
            // reconcile may replace state->targets without invalidating this
            // pass or releasing the controller currently inside its seam.
            const std::vector<wxMSWAppProgressState::Target> targets =
                state->targets;
            for ( const wxMSWAppProgressState::Target& target : targets )
            {
                if ( !wxMSWAppProgressStateIsCurrent(
                         state, desired.revision) )
                {
                    state->applyPending = !state->retired;
                    break;
                }

                wxMSWApplyAppProgressTarget(state, desired, target);
                if ( !wxMSWAppProgressStateIsCurrent(
                         state, desired.revision) )
                {
                    state->applyPending = !state->retired;
                    break;
                }
            }
        }

        ++passes;
    }
    while ( !state->retired && state->applyPending && passes < 32 );

    state->applying = false;
}

void wxMSWRegisterAppProgressState(
    const std::shared_ptr<wxMSWAppProgressState>& state)
{
    std::vector<std::weak_ptr<wxMSWAppProgressState>> live;
    live.reserve(gs_appProgressStates.size() + 1);
    for ( const std::weak_ptr<wxMSWAppProgressState>& weak :
          gs_appProgressStates )
    {
        if ( !weak.expired() )
            live.push_back(weak);
    }
    live.push_back(state);
    gs_appProgressStates.swap(live);
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxAppProgressIndicator Implementation.
// ----------------------------------------------------------------------------
wxAppProgressIndicator::wxAppProgressIndicator(wxWindow* parent, int maxValue)
    : m_state(std::make_shared<wxMSWAppProgressState>())
{
    wxCHECK_RET( maxValue > 0,
                 "application progress range must be strictly positive" );
    m_state->allTopLevels = parent == nullptr;
    if ( parent )
        m_state->explicitParent = parent;
    m_state->maxValue = maxValue;
    wxMSWRegisterAppProgressState(m_state);
    wxMSWApplyAppProgress(m_state);
}

wxAppProgressIndicator::~wxAppProgressIndicator()
{
    const std::shared_ptr<wxMSWAppProgressState> state = m_state;
    m_state.reset();
    if ( !state || state->retired )
        return;

    // Publish retirement before the first cleanup seam. A nested callback sees
    // no live wrapper/sidecar, while the target copies below keep their exact
    // controllers alive until SetProgressRange(0) returns.
    state->retired = true;
    state->applyPending = false;
    wxMSWBumpAppProgressRevision(state);
    const std::vector<wxMSWAppProgressState::Target> targets =
        state->targets;
    state->targets.clear();
    for ( const wxMSWAppProgressState::Target& target : targets )
    {
        const std::shared_ptr<wxTaskBarButton> taskbar =
            target.taskBarButton;
        if ( taskbar &&
             static_cast<wxTaskBarButtonImpl*>(
                 taskbar.get())->IsAvailable() )
        {
            taskbar->SetProgressRange(0);
        }
    }
}

bool wxAppProgressIndicator::IsAvailable() const
{
    const std::shared_ptr<wxMSWAppProgressState> state = m_state;
    if ( !state || state->retired )
        return false;
    wxMSWApplyAppProgress(state);
    if ( state->retired )
        return false;
    const std::vector<wxMSWAppProgressState::Target> targets =
        state->targets;
    for ( const wxMSWAppProgressState::Target& target : targets )
    {
        if ( wxMSWAppProgressTargetIsAvailable(target) )
            return true;
    }
    return false;
}

void wxAppProgressIndicator::SetValue(int value)
{
    const std::shared_ptr<wxMSWAppProgressState> state = m_state;
    wxCHECK_RET( state && !state->retired &&
                     value >= 0 && value <= state->maxValue,
                 "invalid application progress value" );
    state->value = value;
    state->mode = wxMSWAppProgressState::Mode::Value;
    wxMSWBumpAppProgressRevision(state);
    wxMSWApplyAppProgress(state);
}

void wxAppProgressIndicator::SetRange(int range)
{
    const std::shared_ptr<wxMSWAppProgressState> state = m_state;
    wxCHECK_RET( state && !state->retired && range > 0,
                 "application progress range must be strictly positive" );
    state->maxValue = range;
    if ( state->value > range )
        state->value = range;
    wxMSWBumpAppProgressRevision(state);
    wxMSWApplyAppProgress(state);
}

void wxAppProgressIndicator::Pulse()
{
    const std::shared_ptr<wxMSWAppProgressState> state = m_state;
    if ( !state || state->retired )
        return;
    state->mode = wxMSWAppProgressState::Mode::Pulse;
    wxMSWBumpAppProgressRevision(state);
    wxMSWApplyAppProgress(state);
}

void wxAppProgressIndicator::Reset()
{
    const std::shared_ptr<wxMSWAppProgressState> state = m_state;
    if ( !state || state->retired )
        return;
    state->value = 0;
    state->mode = wxMSWAppProgressState::Mode::Reset;
    wxMSWBumpAppProgressRevision(state);
    wxMSWApplyAppProgress(state);
}

void wxMSWAppProgressNotifyTaskbarCreated(wxWindow* window)
{
    if ( !window || wxWindowIsUnavailableForCallbacks(window) )
        return;

    // Compact and snapshot before any replay. Reentrant destruction or a new
    // indicator can mutate the registry without invalidating this traversal.
    std::vector<std::weak_ptr<wxMSWAppProgressState>> live;
    std::vector<std::shared_ptr<wxMSWAppProgressState>> states;
    live.reserve(gs_appProgressStates.size());
    states.reserve(gs_appProgressStates.size());
    for ( const std::weak_ptr<wxMSWAppProgressState>& weak :
          gs_appProgressStates )
    {
        const std::shared_ptr<wxMSWAppProgressState> state = weak.lock();
        if ( state && !state->retired )
        {
            live.push_back(state);
            states.push_back(state);
        }
    }
    gs_appProgressStates.swap(live);

    for ( const std::shared_ptr<wxMSWAppProgressState>& state : states )
    {
        const wxWindow* const explicitParent =
            state->explicitParent.get();
        if ( state->allTopLevels || explicitParent == window )
            wxMSWApplyAppProgress(state);
    }
}

#endif // wxUSE_TASKBARBUTTON
