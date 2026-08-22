///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiinputstate.cpp
// Purpose:     deterministic tests for the WinUI pointer state machine
// Author:      wxWidgets development team
// Created:     2026-07-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "wx/winui/private/inputstate.h"

#include <limits>
#include <vector>

namespace
{

wxWinUIInputTargetKey Target(std::uintptr_t leaf,
                             std::uint64_t generation = 1)
{
    wxWinUIInputTargetKey target;
    target.leaf = leaf;
    target.generation = generation;
    return target;
}

wxWinUIInputTargetKey OwnedTarget(std::uintptr_t leaf,
                                  std::uint64_t leafGeneration,
                                  std::uintptr_t owner,
                                  std::uint64_t ownerGeneration)
{
    wxWinUIInputTargetKey target = Target(leaf, leafGeneration);
    target.owner = owner;
    target.ownerGeneration = ownerGeneration;
    return target;
}

wxWinUIRouteObservation Native(
    const wxWinUIInputTargetKey& target,
    wxWinUIInputArea area = wxWinUIInputArea::Client,
    int zone = 1,
    bool clientDoubleClicks = true)
{
    wxWinUIRouteObservation observation;
    observation.surface = wxWinUIInputSurface::Native;
    observation.target = target;
    observation.area = area;
    observation.zone = zone;
    observation.clientDoubleClicks = clientDoubleClicks;
    return observation;
}

wxWinUIRouteObservation Surface(wxWinUIInputSurface surface)
{
    wxWinUIRouteObservation observation;
    observation.surface = surface;
    return observation;
}

wxWinUIPointerSample Sample(
    wxWinUIInputKind kind,
    wxWinUIInputButton button = wxWinUIInputButton::None,
    int x = 10,
    int y = 20,
    std::uint64_t timestamp = 100)
{
    wxWinUIPointerSample sample;
    sample.kind = kind;
    sample.button = button;
    sample.screenX = x;
    sample.screenY = y;
    sample.timestamp = timestamp;
    return sample;
}

const wxWinUIInputAction *FindAction(
    const wxWinUIInputTransition& transition,
    wxWinUIInputActionKind kind)
{
    for ( const wxWinUIInputAction& action : transition.actions )
    {
        if ( action.action == kind )
            return &action;
    }
    return nullptr;
}

const wxWinUIInputAction *FindDispatch(
    const wxWinUIInputTransition& transition)
{
    return FindAction(transition, wxWinUIInputActionKind::Dispatch);
}

wxWinUIInputTransition RouteCommitted(
    wxWinUIInputState& state,
    const wxWinUIPointerSample& sample,
    const wxWinUIRouteObservation& observation)
{
    wxWinUIInputTransition transition = state.Route(sample, observation);
    for ( const wxWinUIInputAction& action : transition.actions )
    {
        if ( action.action == wxWinUIInputActionKind::HoverEnter ||
             action.action == wxWinUIInputActionKind::HoverLeave )
        {
            REQUIRE(state.CommitHoverAction(action));
        }
        else if ( action.action == wxWinUIInputActionKind::Dispatch )
            REQUIRE(state.CommitBeforeDispatch(action));
    }
    return transition;
}

std::vector<wxWinUIInputKind> ClickSequence(
    wxWinUIInputState& state,
    const wxWinUIRouteObservation& first,
    const wxWinUIRouteObservation& second,
    wxWinUIInputButton firstButton = wxWinUIInputButton::Left,
    wxWinUIInputButton secondButton = wxWinUIInputButton::Left,
    int secondX = 11,
    int secondY = 21,
    std::uint64_t secondTime = 300)
{
    std::vector<wxWinUIInputKind> sequence;
    const auto route = [&state, &sequence](
        const wxWinUIPointerSample& sample,
        const wxWinUIRouteObservation& observation)
    {
        const wxWinUIInputTransition transition =
            RouteCommitted(state, sample, observation);
        if ( const wxWinUIInputAction *action = FindDispatch(transition) )
            sequence.push_back(action->kind);
    };

    route(Sample(wxWinUIInputKind::Press, firstButton, 10, 20, 100), first);
    route(Sample(wxWinUIInputKind::Release, firstButton, 10, 20, 120), first);
    route(Sample(wxWinUIInputKind::Press, secondButton,
                 secondX, secondY, secondTime), second);
    route(Sample(wxWinUIInputKind::Release, secondButton,
                 secondX, secondY, secondTime + 20), second);
    return sequence;
}

} // namespace

TEST_CASE("wxWinUI input state mapping", "[winui-input-state]")
{
    struct MappingCase
    {
        wxWinUIInputUpdateKind update;
        wxWinUIInputKind kind;
        wxWinUIInputButton button;
    };

    const MappingCase cases[] =
    {
        { wxWinUIInputUpdateKind::LeftPressed,
          wxWinUIInputKind::Press, wxWinUIInputButton::Left },
        { wxWinUIInputUpdateKind::LeftReleased,
          wxWinUIInputKind::Release, wxWinUIInputButton::Left },
        { wxWinUIInputUpdateKind::RightPressed,
          wxWinUIInputKind::Press, wxWinUIInputButton::Right },
        { wxWinUIInputUpdateKind::RightReleased,
          wxWinUIInputKind::Release, wxWinUIInputButton::Right },
        { wxWinUIInputUpdateKind::MiddlePressed,
          wxWinUIInputKind::Press, wxWinUIInputButton::Middle },
        { wxWinUIInputUpdateKind::MiddleReleased,
          wxWinUIInputKind::Release, wxWinUIInputButton::Middle },
        { wxWinUIInputUpdateKind::X1Pressed,
          wxWinUIInputKind::Press, wxWinUIInputButton::X1 },
        { wxWinUIInputUpdateKind::X1Released,
          wxWinUIInputKind::Release, wxWinUIInputButton::X1 },
        { wxWinUIInputUpdateKind::X2Pressed,
          wxWinUIInputKind::Press, wxWinUIInputButton::X2 },
        { wxWinUIInputUpdateKind::X2Released,
          wxWinUIInputKind::Release, wxWinUIInputButton::X2 }
    };

    for ( const MappingCase& expected : cases )
    {
        const wxWinUIInputMapping mapping =
            wxWinUIMapPointerUpdate(expected.update);
        CHECK(mapping.valid);
        CHECK(mapping.kind == expected.kind);
        CHECK(mapping.button == expected.button);
    }

    CHECK_FALSE(
        wxWinUIMapPointerUpdate(wxWinUIInputUpdateKind::Other).valid);

    const wxWinUIInputMapping vertical = wxWinUIMapPointerWheel(false);
    CHECK(vertical.valid);
    CHECK(vertical.kind == wxWinUIInputKind::Wheel);
    CHECK(vertical.button == wxWinUIInputButton::None);
    CHECK_FALSE(vertical.horizontalWheel);

    const wxWinUIInputMapping horizontal = wxWinUIMapPointerWheel(true);
    CHECK(horizontal.valid);
    CHECK(horizontal.kind == wxWinUIInputKind::Wheel);
    CHECK(horizontal.horizontalWheel);

    wxWinUIInputState state;
    wxWinUIPointerSample wheel = Sample(wxWinUIInputKind::Wheel);
    wheel.device = wxWinUIInputDevice::Pen;
    wheel.pointerId = 42;
    wheel.modifiers = 3;
    wheel.buttonMask = 5;
    wheel.wheelDelta = -120;
    wheel.horizontalWheel = true;
    const wxWinUIInputTransition transition =
        RouteCommitted(state, wheel, Native(Target(1)));
    const wxWinUIInputAction *dispatch = FindDispatch(transition);
    REQUIRE(dispatch);
    CHECK(dispatch->kind == wxWinUIInputKind::Wheel);
    CHECK(dispatch->wheelDelta == -120);
    CHECK(dispatch->horizontalWheel);
    CHECK(dispatch->device == wxWinUIInputDevice::Pen);
    CHECK(dispatch->pointerId == 42);
    CHECK(dispatch->modifiers == 3);
    CHECK(dispatch->buttonMask == 5);
}

TEST_CASE("wxWinUI input state surface and hover authority",
          "[winui-input-state]")
{
    wxWinUIInputState state;
    const wxWinUIInputTargetKey a = Target(0xA);
    const wxWinUIInputTargetKey b = Target(0xB);

    wxWinUIInputTransition transition =
        RouteCommitted(state, Sample(wxWinUIInputKind::Move),
                    Surface(wxWinUIInputSurface::Xaml));
    CHECK(transition.actions.empty());
    CHECK_FALSE(transition.cancelSource);
    CHECK(transition.disposition == wxWinUIInputDisposition::Xaml);

    transition = RouteCommitted(state, Sample(wxWinUIInputKind::Move), Native(a));
    CHECK(transition.CountActions(wxWinUIInputActionKind::HoverEnter) == 1);
    CHECK(transition.CountActions(wxWinUIInputActionKind::Dispatch) == 1);
    CHECK(FindAction(transition,
                     wxWinUIInputActionKind::HoverEnter)->target == a);
    CHECK(transition.cancelSource);

    transition = RouteCommitted(state, Sample(wxWinUIInputKind::Move), Native(a));
    CHECK_FALSE(
        transition.HasAction(wxWinUIInputActionKind::HoverEnter));
    CHECK_FALSE(
        transition.HasAction(wxWinUIInputActionKind::HoverLeave));
    CHECK(transition.CountActions(wxWinUIInputActionKind::Dispatch) == 1);

    transition = RouteCommitted(state,
        Sample(wxWinUIInputKind::Move),
        Native(a, wxWinUIInputArea::NonClient, 8));
    REQUIRE(transition.actions.size() == 3);
    CHECK(transition.actions[0].action ==
          wxWinUIInputActionKind::HoverLeave);
    CHECK(transition.actions[0].target == a);
    CHECK(transition.actions[0].area == wxWinUIInputArea::Client);
    CHECK(transition.actions[1].action ==
          wxWinUIInputActionKind::HoverEnter);
    CHECK(transition.actions[1].target == a);
    CHECK(transition.actions[1].area == wxWinUIInputArea::NonClient);
    CHECK(transition.actions[0].screenX == 10);
    CHECK(transition.actions[0].screenY == 20);
    CHECK(transition.actions[1].screenX == 10);
    CHECK(transition.actions[1].screenY == 20);

    transition = RouteCommitted(state,
        Sample(wxWinUIInputKind::Move),
        Native(b, wxWinUIInputArea::NonClient, 9));
    REQUIRE(transition.actions.size() == 3);
    CHECK(transition.actions[0].action ==
          wxWinUIInputActionKind::HoverLeave);
    CHECK(transition.actions[0].target == a);
    CHECK(transition.actions[0].area == wxWinUIInputArea::NonClient);
    CHECK(transition.actions[1].target == b);

    // An unstable visual query changes no state and cannot cancel the source.
    transition = RouteCommitted(state,
        Sample(wxWinUIInputKind::Move),
        Surface(wxWinUIInputSurface::Indeterminate));
    CHECK(transition.actions.empty());
    CHECK_FALSE(transition.cancelSource);
    CHECK(transition.disposition ==
          wxWinUIInputDisposition::Indeterminate);

    transition = RouteCommitted(state,
        Sample(wxWinUIInputKind::Move),
        Native(b, wxWinUIInputArea::NonClient, 9));
    CHECK_FALSE(
        transition.HasAction(wxWinUIInputActionKind::HoverEnter));
    CHECK_FALSE(
        transition.HasAction(wxWinUIInputActionKind::HoverLeave));

    // A confirmed XAML hit ends the exact old native hover, but the current
    // XAML sample remains unhandled.
    transition = RouteCommitted(state,
        Sample(wxWinUIInputKind::Move),
        Surface(wxWinUIInputSurface::Xaml));
    REQUIRE(transition.actions.size() == 1);
    CHECK(transition.actions[0].action ==
          wxWinUIInputActionKind::HoverLeave);
    CHECK(transition.actions[0].target == b);
    CHECK(transition.actions[0].area == wxWinUIInputArea::NonClient);
    CHECK_FALSE(transition.cancelSource);

    transition = RouteCommitted(state,
        Sample(wxWinUIInputKind::Move),
        Native(a, wxWinUIInputArea::Client, 1));
    REQUIRE(transition.HasAction(wxWinUIInputActionKind::HoverEnter));
    transition = RouteCommitted(state,
        Sample(wxWinUIInputKind::Leave),
        Surface(wxWinUIInputSurface::Outside));
    REQUIRE(transition.actions.size() == 1);
    CHECK(transition.actions[0].action ==
          wxWinUIInputActionKind::HoverLeave);
    CHECK(transition.actions[0].target == a);
    CHECK(transition.actions[0].area == wxWinUIInputArea::Client);
    CHECK_FALSE(transition.cancelSource);
    CHECK(transition.disposition == wxWinUIInputDisposition::Outside);

    // Hover is committed action-by-action, not published while Route() is
    // merely preparing the whole snapshot. After old->none commits, a nested
    // transition may establish C; the stale outer Enter(B) must then fail and
    // must not overwrite C.
    wxWinUIInputState reentrantState;
    const wxWinUIInputTargetKey c = Target(0xC);
    RouteCommitted(reentrantState, Sample(wxWinUIInputKind::Move), Native(a));
    const wxWinUIInputTransition outer =
        reentrantState.Route(Sample(wxWinUIInputKind::Move), Native(b));
    REQUIRE(outer.actions.size() == 3);
    REQUIRE(outer.actions[0].action ==
            wxWinUIInputActionKind::HoverLeave);
    REQUIRE(outer.actions[1].action ==
            wxWinUIInputActionKind::HoverEnter);
    REQUIRE(reentrantState.CommitHoverAction(outer.actions[0]));

    const wxWinUIInputTransition nested =
        RouteCommitted(reentrantState,
                       Sample(wxWinUIInputKind::Move),
                       Native(c));
    REQUIRE(nested.HasAction(wxWinUIInputActionKind::HoverEnter));
    CHECK_FALSE(reentrantState.CommitHoverAction(outer.actions[1]));

    transition =
        RouteCommitted(reentrantState,
                       Sample(wxWinUIInputKind::Move),
                       Native(c));
    CHECK_FALSE(
        transition.HasAction(wxWinUIInputActionKind::HoverEnter));
    CHECK_FALSE(
        transition.HasAction(wxWinUIInputActionKind::HoverLeave));

    // A native adapter can commit Enter, then discover at its final refresh
    // that no dispatch can begin. It may silently roll back only that exact
    // unpublished Enter; a later/nested hover must win.
    wxWinUIInputState abortedEnterState;
    wxWinUIInputTransition abortedEnter =
        abortedEnterState.Route(
            Sample(wxWinUIInputKind::Move), Native(a));
    const wxWinUIInputAction *enterA =
        FindAction(abortedEnter, wxWinUIInputActionKind::HoverEnter);
    REQUIRE(enterA != nullptr);
    const wxWinUIInputAction enterACopy = *enterA;
    REQUIRE(abortedEnterState.CommitHoverAction(enterACopy));
    CHECK(abortedEnterState.AbortCommittedHoverEnter(enterACopy));
    transition = abortedEnterState.Route(
        Sample(wxWinUIInputKind::Move),
        Surface(wxWinUIInputSurface::Xaml));
    CHECK(transition.actions.empty());
    CHECK_FALSE(
        abortedEnterState.AbortCommittedHoverEnter(enterACopy));

    abortedEnter =
        abortedEnterState.Route(
            Sample(wxWinUIInputKind::Move), Native(a));
    enterA = FindAction(abortedEnter, wxWinUIInputActionKind::HoverEnter);
    REQUIRE(enterA != nullptr);
    const wxWinUIInputAction staleEnterA = *enterA;
    REQUIRE(abortedEnterState.CommitHoverAction(staleEnterA));
    abortedEnter = abortedEnterState.Route(
        Sample(wxWinUIInputKind::Move), Native(b));
    REQUIRE(abortedEnter.actions.size() == 3);
    REQUIRE(abortedEnterState.CommitHoverAction(
        abortedEnter.actions[0]));
    REQUIRE(abortedEnterState.CommitHoverAction(
        abortedEnter.actions[1]));
    CHECK_FALSE(
        abortedEnterState.AbortCommittedHoverEnter(staleEnterA));
    transition = abortedEnterState.Route(
        Sample(wxWinUIInputKind::Move), Native(b));
    CHECK_FALSE(
        transition.HasAction(wxWinUIInputActionKind::HoverEnter));
    CHECK_FALSE(
        transition.HasAction(wxWinUIInputActionKind::HoverLeave));
}

TEST_CASE("wxWinUI input state Win32 double-click sequence",
          "[winui-input-state]")
{
    wxWinUIClickSettings settings;
    settings.maxInterval = 500;
    settings.rectangleWidth = 8;
    settings.rectangleHeight = 8;

    SECTION("matching client clicks")
    {
        wxWinUIInputState state(settings);
        const auto sequence =
            ClickSequence(state, Native(Target(1)), Native(Target(1)));
        REQUIRE(sequence.size() == 4);
        CHECK(sequence[0] == wxWinUIInputKind::Press);
        CHECK(sequence[1] == wxWinUIInputKind::Release);
        CHECK(sequence[2] == wxWinUIInputKind::DoubleClick);
        CHECK(sequence[3] == wxWinUIInputKind::Release);
    }

    SECTION("generation is part of the target")
    {
        wxWinUIInputState state(settings);
        const auto sequence =
            ClickSequence(state, Native(Target(1, 1)), Native(Target(1, 2)));
        CHECK(sequence[2] == wxWinUIInputKind::Press);
    }

    SECTION("generation change balances but never completes the old click")
    {
        wxWinUIInputState state(settings);
        const auto oldTarget = Native(Target(1, 1));
        const auto newTarget = Native(Target(1, 2));
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left,
                           10, 20, 100), oldTarget);
        wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left,
                               10, 20, 120), newTarget);
        const wxWinUIInputAction *dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->target == Target(1, 1));

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left,
                               10, 20, 200), newTarget);
        dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->kind == wxWinUIInputKind::Press);
    }

    SECTION("button must match")
    {
        wxWinUIInputState state(settings);
        const auto sequence =
            ClickSequence(state, Native(Target(1)), Native(Target(1)),
                          wxWinUIInputButton::Left,
                          wxWinUIInputButton::X1);
        CHECK(sequence[2] == wxWinUIInputKind::Press);
    }

    SECTION("buttons keep independent click trackers")
    {
        wxWinUIInputState state(settings);
        const auto native = Native(Target(1));
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left,
                           10, 20, 100), native);
        RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                           wxWinUIInputButton::Left,
                           10, 20, 120), native);
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Right,
                           10, 20, 150), native);
        RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                           wxWinUIInputButton::Right,
                           10, 20, 170), native);

        const wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left,
                               11, 21, 250), native);
        const wxWinUIInputAction *dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->kind == wxWinUIInputKind::DoubleClick);
    }

    SECTION("zone must match")
    {
        wxWinUIInputState state(settings);
        const auto sequence =
            ClickSequence(state, Native(Target(1),
                                        wxWinUIInputArea::Client, 1),
                          Native(Target(1),
                                 wxWinUIInputArea::Client, 2));
        CHECK(sequence[2] == wxWinUIInputKind::Press);
    }

    SECTION("time must fit")
    {
        wxWinUIInputState state(settings);
        const auto sequence =
            ClickSequence(state, Native(Target(1)), Native(Target(1)),
                          wxWinUIInputButton::Left,
                          wxWinUIInputButton::Left, 11, 21, 601);
        CHECK(sequence[2] == wxWinUIInputKind::Press);
    }

    SECTION("point must fit the centred system rectangle")
    {
        wxWinUIInputState state(settings);
        const auto sequence =
            ClickSequence(state, Native(Target(1)), Native(Target(1)),
                          wxWinUIInputButton::Left,
                          wxWinUIInputButton::Left, 15, 20, 300);
        CHECK(sequence[2] == wxWinUIInputKind::Press);
    }

    SECTION("hostile coordinate extremes remain a normal click")
    {
        wxWinUIInputState state(settings);
        const auto sequence =
            ClickSequence(
                state, Native(Target(1)), Native(Target(1)),
                wxWinUIInputButton::Left,
                wxWinUIInputButton::Left,
                std::numeric_limits<int>::max(),
                std::numeric_limits<int>::max(), 300);
        CHECK(sequence[2] == wxWinUIInputKind::Press);
    }

    SECTION("client class style is authoritative")
    {
        wxWinUIInputState state(settings);
        const auto sequence =
            ClickSequence(state, Native(Target(1)),
                          Native(Target(1), wxWinUIInputArea::Client,
                                 1, false));
        CHECK(sequence[2] == wxWinUIInputKind::Press);
    }

    SECTION("non-client does not require the client class style")
    {
        wxWinUIInputState state(settings);
        const auto nc = Native(Target(1), wxWinUIInputArea::NonClient,
                               13, false);
        const auto sequence = ClickSequence(state, nc, nc);
        CHECK(sequence[2] == wxWinUIInputKind::DoubleClick);
    }
}

TEST_CASE("wxWinUI input state capture and cancellation",
          "[winui-input-state]")
{
    const wxWinUIInputTargetKey a = Target(0xA);
    const wxWinUIInputTargetKey b = Target(0xB);

    SECTION("observed native capture is authoritative")
    {
        wxWinUIInputState state;
        wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left),
                        Native(a));
        REQUIRE(transition.actions.size() == 1);
        CHECK(transition.actions[0].action ==
              wxWinUIInputActionKind::Dispatch);
        CHECK(state.NativeCaptureChanged(
                    a, Sample(wxWinUIInputKind::Move)).actions.empty());

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left),
                        Native(b));
        const wxWinUIInputAction *dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->target == a);
        CHECK_FALSE(transition.HasAction(
            wxWinUIInputActionKind::ReleaseCapture));
        CHECK(state.NativeCaptureChanged(
                    wxWinUIInputTargetKey(),
                    Sample(wxWinUIInputKind::CaptureLost)).actions.empty());

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left),
                        Native(b));
        CHECK_FALSE(FindDispatch(transition));
        CHECK(transition.cancelSource);
        CHECK_FALSE(transition.HasAction(
            wxWinUIInputActionKind::ReleaseCapture));
    }

    SECTION("press alone does not establish native capture")
    {
        wxWinUIInputState state;
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left), Native(a));
        wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Move), Native(b));
        const wxWinUIInputAction *dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->target == b);

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left),
                        Surface(wxWinUIInputSurface::Outside));
        dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->target == a);
        REQUIRE(transition.actions.size() == 2);
        CHECK(transition.actions[0].action ==
              wxWinUIInputActionKind::Dispatch);
        CHECK(transition.actions[1].action ==
              wxWinUIInputActionKind::HoverLeave);
        CHECK(transition.actions[1].target == b);

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left),
                        Native(b));
        dispatch = FindDispatch(transition);
        CHECK_FALSE(dispatch);
        CHECK(transition.cancelSource);
    }

    SECTION("capture is authoritative outside native hit resolution")
    {
        wxWinUIInputState state;
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left), Native(a));
        state.NativeCaptureChanged(a, Sample(wxWinUIInputKind::Move));

        wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Move),
                        Surface(wxWinUIInputSurface::Xaml));
        const wxWinUIInputAction *dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->target == a);
        CHECK(dispatch->area == wxWinUIInputArea::Client);
        CHECK(transition.cancelSource);

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left),
                        Surface(wxWinUIInputSurface::Indeterminate));
        dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->target == a);
        CHECK_FALSE(transition.HasAction(
            wxWinUIInputActionKind::ReleaseCapture));
        CHECK(transition.cancelSource);
        state.NativeCaptureChanged(
            wxWinUIInputTargetKey(),
            Sample(wxWinUIInputKind::CaptureLost));
    }

    SECTION("five buttons keep independent pressed state")
    {
        wxWinUIInputState state;
        wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left),
                        Native(a));
        state.NativeCaptureChanged(a, Sample(wxWinUIInputKind::Move));

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::X1),
                        Native(b));
        const wxWinUIInputAction *dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->target == a);
        CHECK(dispatch->button == wxWinUIInputButton::X1);
        CHECK_FALSE(transition.HasAction(
            wxWinUIInputActionKind::CancelPress));

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::X1),
                        Surface(wxWinUIInputSurface::Outside));
        dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->target == a);
        CHECK_FALSE(transition.HasAction(
            wxWinUIInputActionKind::ReleaseCapture));

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left),
                        Surface(wxWinUIInputSurface::Indeterminate));
        REQUIRE(transition.actions.size() == 1);
        CHECK(transition.actions[0].action ==
              wxWinUIInputActionKind::Dispatch);
        CHECK_FALSE(transition.HasAction(
            wxWinUIInputActionKind::ReleaseCapture));
        CHECK(state.NativeCaptureChanged(
                    wxWinUIInputTargetKey(),
                    Sample(wxWinUIInputKind::CaptureLost)).actions.empty());
    }

    SECTION("cancel is idempotent")
    {
        wxWinUIInputState state;
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Right), Native(a));
        state.NativeCaptureChanged(a, Sample(wxWinUIInputKind::Move));

        wxWinUIPointerSample cancel = Sample(wxWinUIInputKind::Cancel);
        cancel.interruptAll = true;
        wxWinUIInputTransition transition =
            RouteCommitted(state, cancel,
                        Surface(wxWinUIInputSurface::Indeterminate));
        CHECK(transition.CountActions(
                  wxWinUIInputActionKind::CancelPress) == 1);
        CHECK(transition.CountActions(
                  wxWinUIInputActionKind::ReleaseCapture) == 1);
        REQUIRE(transition.actions.size() == 2);
        CHECK(transition.actions[0].action ==
              wxWinUIInputActionKind::ReleaseCapture);
        CHECK(transition.actions[1].action ==
              wxWinUIInputActionKind::CancelPress);

        transition =
            RouteCommitted(state, cancel,
                        Surface(wxWinUIInputSurface::Xaml));
        CHECK(transition.actions.empty());
        CHECK_FALSE(transition.cancelSource);
    }

    SECTION("capture-lost does not release an already lost capture")
    {
        wxWinUIInputState state;
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::X2), Native(a));
        state.NativeCaptureChanged(a, Sample(wxWinUIInputKind::Move));

        wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::CaptureLost),
                        Surface(wxWinUIInputSurface::Outside));
        CHECK(transition.CountActions(
                  wxWinUIInputActionKind::CancelPress) == 1);
        CHECK_FALSE(transition.HasAction(
            wxWinUIInputActionKind::ReleaseCapture));

        transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::CaptureLost),
                        Surface(wxWinUIInputSurface::Indeterminate));
        CHECK(transition.actions.empty());
    }

    SECTION("capture generation transfer cancels the old press once")
    {
        const wxWinUIInputTargetKey oldTarget = Target(0xA, 1);
        const wxWinUIInputTargetKey newTarget = Target(0xA, 2);
        wxWinUIInputState state;
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left),
                    Native(oldTarget));
        state.NativeCaptureChanged(
            oldTarget, Sample(wxWinUIInputKind::Move));

        wxWinUIInputTransition transition =
            state.NativeCaptureChanged(
                newTarget, Sample(wxWinUIInputKind::CaptureLost));
        const wxWinUIInputAction *cancel =
            FindAction(transition,
                       wxWinUIInputActionKind::CancelPress);
        REQUIRE(cancel);
        CHECK(cancel->target == oldTarget);

        transition = state.NativeCaptureChanged(
            newTarget, Sample(wxWinUIInputKind::CaptureLost));
        CHECK(transition.actions.empty());
    }

    SECTION("capture acquisition on another generation cancels old DOWN")
    {
        const wxWinUIInputTargetKey oldTarget = Target(0xA, 1);
        const wxWinUIInputTargetKey newTarget = Target(0xA, 2);
        wxWinUIInputState state;
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left),
                    Native(oldTarget));

        wxWinUIInputTransition transition =
            state.NativeCaptureChanged(
                newTarget, Sample(wxWinUIInputKind::Move));
        const wxWinUIInputAction *cancel =
            FindAction(transition,
                       wxWinUIInputActionKind::CancelPress);
        REQUIRE(cancel);
        CHECK(cancel->target == oldTarget);

        transition = state.NativeCaptureChanged(
            newTarget, Sample(wxWinUIInputKind::Move));
        CHECK(transition.actions.empty());
    }

    SECTION("implementation child and wx shell are one capture owner")
    {
        const wxWinUIInputTargetKey child =
            OwnedTarget(0xA1, 11, 0xA0, 7);
        const wxWinUIInputTargetKey shell =
            OwnedTarget(0xA0, 7, 0xA0, 7);
        wxWinUIInputState state;

        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left),
                    Native(child));
        CHECK(state.NativeCaptureChanged(
                    shell, Sample(wxWinUIInputKind::Move)).actions.empty());

        const wxWinUIInputTransition release =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left),
                        Native(child));
        const wxWinUIInputAction *dispatch = FindDispatch(release);
        REQUIRE(dispatch);
        CHECK(dispatch->target == shell);
        CHECK(dispatch->balancesPress);
        CHECK(state.NativeCaptureChanged(
                    wxWinUIInputTargetKey(),
                    Sample(wxWinUIInputKind::CaptureLost)).actions.empty());
    }

    SECTION("capture transfer to another logical owner cancels once")
    {
        const wxWinUIInputTargetKey child =
            OwnedTarget(0xA1, 11, 0xA0, 7);
        const wxWinUIInputTargetKey otherShell =
            OwnedTarget(0xB0, 3, 0xB0, 3);
        wxWinUIInputState state;

        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left),
                    Native(child));
        const wxWinUIInputTransition transfer =
            state.NativeCaptureChanged(
                otherShell, Sample(wxWinUIInputKind::Move));
        const wxWinUIInputAction *cancel =
            FindAction(transfer, wxWinUIInputActionKind::CancelPress);
        REQUIRE(cancel);
        CHECK(cancel->target == child);
        CHECK(state.NativeCaptureChanged(
                    otherShell,
                    Sample(wxWinUIInputKind::Move)).actions.empty());
    }

    SECTION("capture transfer preserves presses owned by its new shell")
    {
        const wxWinUIInputTargetKey aChild =
            OwnedTarget(0xA1, 11, 0xA0, 7);
        const wxWinUIInputTargetKey bChild =
            OwnedTarget(0xB1, 12, 0xB0, 8);
        const wxWinUIInputTargetKey bShell =
            OwnedTarget(0xB0, 8, 0xB0, 8);
        wxWinUIInputState state;

        RouteCommitted(state,
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left),
            Native(aChild));
        RouteCommitted(state,
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Right),
            Native(bChild));

        const wxWinUIInputTransition transfer =
            state.NativeCaptureChanged(
                bShell, Sample(wxWinUIInputKind::Move));
        REQUIRE(transfer.CountActions(
                    wxWinUIInputActionKind::CancelPress) == 1);
        CHECK(FindAction(
                  transfer,
                  wxWinUIInputActionKind::CancelPress)->target == aChild);

        const wxWinUIInputTransition bRelease =
            RouteCommitted(state,
                Sample(wxWinUIInputKind::Release,
                       wxWinUIInputButton::Right),
                Native(bChild));
        const wxWinUIInputAction *up = FindDispatch(bRelease);
        REQUIRE(up);
        CHECK(up->target == bShell);
        CHECK(up->balanceTarget == bChild);
    }

    SECTION("owner generation and completeness fail closed")
    {
        const wxWinUIInputTargetKey child =
            OwnedTarget(0xA1, 11, 0xA0, 7);
        wxWinUIInputState state;
        RouteCommitted(state,
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left),
            Native(child));

        const wxWinUIInputTargetKey recycledShell =
            OwnedTarget(0xA0, 8, 0xA0, 8);
        CHECK(state.NativeCaptureChanged(
                  recycledShell,
                  Sample(wxWinUIInputKind::Move)).CountActions(
                    wxWinUIInputActionKind::CancelPress) == 1);

        wxWinUIInputTargetKey partial =
            OwnedTarget(0xB1, 1, 0xB0, 2);
        partial.ownerGeneration = 0;
        CHECK_FALSE(child.HasSameOwner(partial));
        CHECK_FALSE(partial.HasSameOwner(child));
    }

    SECTION("press and release commit only after reentrant preflights")
    {
        wxWinUIClickSettings settings;
        settings.maxInterval = 500;
        settings.rectangleWidth = 8;
        settings.rectangleHeight = 8;
        wxWinUIInputState state(settings);

        const wxWinUIInputTransition firstPrepared =
            state.Route(Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left,
                               10, 20, 100),
                        Native(a));
        const wxWinUIInputAction *firstDown =
            FindDispatch(firstPrepared);
        REQUIRE(firstDown);
        CHECK(firstDown->gestureSerial != 0);

        // The prepared but uncommitted DOWN is invisible: a nested UP cannot
        // balance it or leak an orphan native UP.
        const wxWinUIInputTransition nestedUp =
            state.Route(Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left,
                               10, 20, 101),
                        Native(a));
        CHECK_FALSE(FindDispatch(nestedUp));

        // A nested newer DOWN may win the race. Its state-issued serial makes
        // the stale outer commit fail even with the same pointer id/tick.
        const wxWinUIInputTransition newerPrepared =
            state.Route(Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left,
                               10, 20, 100),
                        Native(b));
        const wxWinUIInputAction *newerDown =
            FindDispatch(newerPrepared);
        REQUIRE(newerDown);
        CHECK(newerDown->gestureSerial != firstDown->gestureSerial);
        REQUIRE(state.CommitBeforeDispatch(*newerDown));
        CHECK_FALSE(state.CommitBeforeDispatch(*firstDown));

        const wxWinUIInputTransition releasePrepared =
            state.Route(Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left,
                               10, 20, 120),
                        Native(b));
        const wxWinUIInputAction *up = FindDispatch(releasePrepared);
        REQUIRE(up);
        CHECK(up->balancesPress);
        CHECK(up->balanceTarget == b);

        // Until UP is committed, another DOWN cannot observe a completed
        // click and cannot be classified as a double click.
        const wxWinUIInputTransition nestedDown =
            state.Route(Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left,
                               10, 20, 121),
                        Native(b));
        CHECK_FALSE(FindDispatch(nestedDown));
        REQUIRE(state.CommitBeforeDispatch(*up));

        const wxWinUIInputTransition next =
            state.Route(Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left,
                               10, 20, 200),
                        Native(b));
        const wxWinUIInputAction *nextDown = FindDispatch(next);
        REQUIRE(nextDown);
        CHECK(nextDown->kind == wxWinUIInputKind::DoubleClick);
        REQUIRE(state.CommitBeforeDispatch(*nextDown));
    }

    SECTION("capture changes invalidate every prepared button action")
    {
        wxWinUIInputState pressState;
        const wxWinUIInputTransition preparedPress =
            pressState.Route(
                Sample(wxWinUIInputKind::Press,
                       wxWinUIInputButton::Left),
                Native(a));
        const wxWinUIInputAction *down = FindDispatch(preparedPress);
        REQUIRE(down);

        // Capture can change during a callback-bearing adapter preflight
        // even though no press has been committed yet.
        CHECK(pressState.NativeCaptureChanged(
                  b, Sample(wxWinUIInputKind::Move)).actions.empty());
        CHECK_FALSE(pressState.CommitBeforeDispatch(*down));

        const wxWinUIInputTargetKey child =
            OwnedTarget(0xA1, 11, 0xA0, 7);
        const wxWinUIInputTargetKey shell =
            OwnedTarget(0xA0, 7, 0xA0, 7);
        wxWinUIInputState releaseState;
        RouteCommitted(
            releaseState,
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left),
            Native(child));
        releaseState.NativeCaptureChanged(
            child, Sample(wxWinUIInputKind::Move));

        const wxWinUIInputTransition preparedRelease =
            releaseState.Route(
                Sample(wxWinUIInputKind::Release,
                       wxWinUIInputButton::Left),
                Native(child));
        const wxWinUIInputAction *up = FindDispatch(preparedRelease);
        REQUIRE(up);

        // A child-to-shell transfer preserves the committed gesture, but it
        // still invalidates the UP prepared against the former capture HWND.
        CHECK(releaseState.NativeCaptureChanged(
                  shell, Sample(wxWinUIInputKind::Move)).actions.empty());
        CHECK_FALSE(releaseState.CommitBeforeDispatch(*up));

        const wxWinUIInputTransition refreshedRelease =
            releaseState.Route(
                Sample(wxWinUIInputKind::Release,
                       wxWinUIInputButton::Left),
                Native(child));
        const wxWinUIInputAction *refreshedUp =
            FindDispatch(refreshedRelease);
        REQUIRE(refreshedUp);
        CHECK(refreshedUp->target == shell);
        REQUIRE(releaseState.CommitBeforeDispatch(*refreshedUp));
    }

    SECTION("modal return with active contact balances without a click")
    {
        wxWinUIClickSettings modalSettings;
        modalSettings.maxInterval = 500;
        modalSettings.rectangleWidth = 8;
        modalSettings.rectangleHeight = 8;
        wxWinUIInputState state(modalSettings);

        const wxWinUIInputTransition prepared =
            state.Route(
                Sample(wxWinUIInputKind::Press,
                       wxWinUIInputButton::Left,
                       10, 20, 100),
                Native(a, wxWinUIInputArea::NonClient, 2));
        const wxWinUIInputAction *down = FindDispatch(prepared);
        REQUIRE(down);
        REQUIRE(state.CommitBeforeDispatch(*down));
        CHECK(state.HasActiveGesture(*down));
        REQUIRE(state.SuppressClickForGesture(*down));

        const wxWinUIInputTransition release =
            state.Route(
                Sample(wxWinUIInputKind::Release,
                       wxWinUIInputButton::Left,
                       10, 20, 120),
                Native(a, wxWinUIInputArea::NonClient, 2));
        const wxWinUIInputAction *up = FindDispatch(release);
        REQUIRE(up);
        REQUIRE(state.CommitBeforeDispatch(*up));
        CHECK_FALSE(state.HasActiveGesture(*down));

        const wxWinUIInputTransition next =
            state.Route(
                Sample(wxWinUIInputKind::Press,
                       wxWinUIInputButton::Left,
                       10, 20, 200),
                Native(a, wxWinUIInputArea::NonClient, 2));
        const wxWinUIInputAction *nextDown = FindDispatch(next);
        REQUIRE(nextDown);
        CHECK(nextDown->kind == wxWinUIInputKind::Press);
        REQUIRE(state.CommitBeforeDispatch(*nextDown));
        CHECK(state.HasActiveGesture(*nextDown));
        CHECK_FALSE(state.HasActiveGesture(*down));
    }

    SECTION("exact modal cancellation preserves another mouse button")
    {
        wxWinUIInputState state;
        const wxWinUIPointerSample left =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left,
                   10, 20, 100);
        const wxWinUIInputTransition preparedLeft =
            state.Route(left, Native(a));
        const wxWinUIInputAction *leftDown =
            FindDispatch(preparedLeft);
        REQUIRE(leftDown);
        const wxWinUIInputAction leftGesture = *leftDown;
        REQUIRE(state.CommitBeforeDispatch(leftGesture));

        const wxWinUIPointerSample right =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Right,
                   12, 22, 101);
        const wxWinUIInputTransition preparedRight =
            state.Route(right, Native(a));
        const wxWinUIInputAction *rightDown =
            FindDispatch(preparedRight);
        REQUIRE(rightDown);
        const wxWinUIInputAction rightGesture = *rightDown;
        REQUIRE(state.CommitBeforeDispatch(rightGesture));
        REQUIRE(state.HasActiveGesture(leftGesture));
        REQUIRE(state.HasActiveGesture(rightGesture));

        wxWinUIPointerSample leave = left;
        leave.kind = wxWinUIInputKind::Cancel;
        leave.timestamp = 120;
        const wxWinUIInputTransition canceled =
            state.CancelActiveGesture(leftGesture, leave);
        CHECK(canceled.CountActions(
                  wxWinUIInputActionKind::CancelPress) == 1);
        CHECK_FALSE(state.HasActiveGesture(leftGesture));
        CHECK(state.HasActiveGesture(rightGesture));

        const wxWinUIInputAction *cancel = FindAction(
            canceled, wxWinUIInputActionKind::CancelPress);
        REQUIRE(cancel);
        CHECK(cancel->gestureSerial == leftGesture.gestureSerial);
        state.AcknowledgeCancellation(*cancel);

        CHECK(state.CancelActiveGesture(leftGesture, leave).actions.empty());
        CHECK(state.HasActiveGesture(rightGesture));

        wxWinUIPointerSample rightRelease = right;
        rightRelease.kind = wxWinUIInputKind::Release;
        rightRelease.timestamp = 130;
        const wxWinUIInputTransition released =
            state.Route(rightRelease, Native(a));
        const wxWinUIInputAction *rightUp = FindDispatch(released);
        REQUIRE(rightUp);
        REQUIRE(state.CommitBeforeDispatch(*rightUp));
        CHECK_FALSE(state.HasAnyActivePress());
    }

    SECTION("slow synchronous DOWN completion is exact and deterministic")
    {
        wxWinUIInputState state;
        const wxWinUIPointerSample left =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left,
                   10, 20, 100);
        const wxWinUIInputTransition preparedLeft =
            state.Route(left, Native(a));
        const wxWinUIInputAction *leftDown =
            FindDispatch(preparedLeft);
        REQUIRE(leftDown);
        const wxWinUIInputAction leftGesture = *leftDown;
        REQUIRE(state.CommitBeforeDispatch(leftGesture));

        const wxWinUIPointerSample right =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Right,
                   12, 22, 101);
        const wxWinUIInputTransition preparedRight =
            state.Route(right, Native(a));
        const wxWinUIInputAction *rightDown =
            FindDispatch(preparedRight);
        REQUIRE(rightDown);
        const wxWinUIInputAction rightGesture = *rightDown;
        REQUIRE(state.CommitBeforeDispatch(rightGesture));

        CHECK(state.FinishSynchronousDown(
                  leftGesture, left, 100, false).actions.empty());
        CHECK(state.FinishSynchronousDown(
                  leftGesture, left, 101, true).actions.empty());
        REQUIRE(state.HasActiveGesture(leftGesture));
        REQUIRE(state.HasActiveGesture(rightGesture));

        const wxWinUIInputTransition finished =
            state.FinishSynchronousDown(
                leftGesture, left, 101, false);
        CHECK(finished.CountActions(
                  wxWinUIInputActionKind::CancelPress) == 1);
        CHECK_FALSE(state.HasActiveGesture(leftGesture));
        CHECK(state.HasActiveGesture(rightGesture));
        const wxWinUIInputAction *cancel =
            FindAction(finished, wxWinUIInputActionKind::CancelPress);
        REQUIRE(cancel);
        CHECK(cancel->gestureSerial == leftGesture.gestureSerial);
        state.AcknowledgeCancellation(*cancel);

        wxWinUIPointerSample rightRelease = right;
        rightRelease.kind = wxWinUIInputKind::Release;
        rightRelease.timestamp = 130;
        const wxWinUIInputTransition released =
            state.Route(rightRelease, Native(a));
        const wxWinUIInputAction *rightUp = FindDispatch(released);
        REQUIRE(rightUp);
        REQUIRE(state.CommitBeforeDispatch(*rightUp));
        CHECK_FALSE(state.HasAnyActivePress());
    }

    SECTION("global interruption invalidates inactive-button preflights")
    {
        wxWinUIInputState state;
        RouteCommitted(
            state,
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left),
            Native(a));

        const wxWinUIInputTransition preparedRight =
            state.Route(
                Sample(wxWinUIInputKind::Press,
                       wxWinUIInputButton::Right),
                Native(a));
        const wxWinUIInputAction *rightDown =
            FindDispatch(preparedRight);
        REQUIRE(rightDown);

        wxWinUIPointerSample lost =
            Sample(wxWinUIInputKind::CaptureLost);
        lost.interruptAll = true;
        const wxWinUIInputTransition interruption =
            state.Route(
                lost, Surface(wxWinUIInputSurface::Indeterminate));
        CHECK(interruption.CountActions(
                  wxWinUIInputActionKind::CancelPress) == 1);
        CHECK_FALSE(state.CommitBeforeDispatch(*rightDown));
    }

    SECTION("unmatched release never creates a native UP")
    {
        wxWinUIInputState state;
        const wxWinUIInputTransition unmatched =
            RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                               wxWinUIInputButton::Left,
                               10, 20, 240),
                        Native(b));
        CHECK_FALSE(FindDispatch(unmatched));
        CHECK(unmatched.cancelSource);
    }

    SECTION("foreign pointer UP cannot retire another device's DOWN")
    {
        wxWinUIInputState state;
        wxWinUIPointerSample mouseDown =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left);
        mouseDown.pointerId = 1;
        RouteCommitted(state, mouseDown, Native(a));
        state.NativeCaptureChanged(a, Sample(wxWinUIInputKind::Move));

        wxWinUIPointerSample touchDown =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::None);
        touchDown.device = wxWinUIInputDevice::Touch;
        touchDown.pointerId = 7;
        CHECK_FALSE(FindDispatch(
            RouteCommitted(state, touchDown, Native(a))));

        touchDown.kind = wxWinUIInputKind::Release;
        CHECK_FALSE(FindDispatch(
            RouteCommitted(state, touchDown, Native(a))));

        mouseDown.kind = wxWinUIInputKind::Release;
        const wxWinUIInputAction *mouseUp = FindDispatch(
            RouteCommitted(state, mouseDown, Native(a)));
        REQUIRE(mouseUp);
        CHECK(mouseUp->balancesPress);

        // The rejected touch release stays suppressed even after the mouse
        // owner completed its gesture.
        CHECK_FALSE(FindDispatch(
            RouteCommitted(state, touchDown, Native(a))));
    }

    SECTION("pointer cancellation preserves unrelated device presses")
    {
        wxWinUIInputState state;
        wxWinUIPointerSample mouse =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left);
        mouse.pointerId = 1;
        RouteCommitted(state, mouse, Native(a));

        wxWinUIPointerSample pen =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Right);
        pen.device = wxWinUIInputDevice::Pen;
        pen.pointerId = 9;
        RouteCommitted(state, pen, Native(b));

        pen.kind = wxWinUIInputKind::Cancel;
        const wxWinUIInputTransition canceled =
            RouteCommitted(state, pen,
                Surface(wxWinUIInputSurface::Indeterminate));
        REQUIRE(canceled.CountActions(
                    wxWinUIInputActionKind::CancelPress) == 1);
        CHECK(FindAction(
                  canceled,
                  wxWinUIInputActionKind::CancelPress)->target == b);

        mouse.kind = wxWinUIInputKind::Release;
        const wxWinUIInputAction *mouseUp = FindDispatch(
            RouteCommitted(state, mouse, Native(a)));
        REQUIRE(mouseUp);
        CHECK(mouseUp->balanceTarget == a);
    }

    SECTION("scoped cancel preserves capture for another pointer")
    {
        wxWinUIInputState state;
        wxWinUIPointerSample mouse =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left);
        mouse.pointerId = 1;
        RouteCommitted(state, mouse, Native(a));

        wxWinUIPointerSample pen =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Right);
        pen.device = wxWinUIInputDevice::Pen;
        pen.pointerId = 9;
        RouteCommitted(state, pen, Native(a));
        state.NativeCaptureChanged(a, Sample(wxWinUIInputKind::Move));

        pen.kind = wxWinUIInputKind::Cancel;
        const wxWinUIInputTransition canceled =
            state.Route(
                pen, Surface(wxWinUIInputSurface::Indeterminate));
        CHECK(canceled.CountActions(
                  wxWinUIInputActionKind::CancelPress) == 1);
        CHECK_FALSE(canceled.HasAction(
            wxWinUIInputActionKind::ReleaseCapture));

        mouse.kind = wxWinUIInputKind::Release;
        const wxWinUIInputAction *mouseUp = FindDispatch(
            RouteCommitted(state, mouse, Native(a)));
        REQUIRE(mouseUp);
        CHECK(mouseUp->target == a);
    }

    SECTION("scoped capture release invalidates other-button preflights")
    {
        wxWinUIInputState state;
        wxWinUIPointerSample pen =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Right);
        pen.device = wxWinUIInputDevice::Pen;
        pen.pointerId = 9;
        RouteCommitted(state, pen, Native(a));
        state.NativeCaptureChanged(a, Sample(wxWinUIInputKind::Move));

        const wxWinUIInputTransition preparedMouse =
            state.Route(
                Sample(wxWinUIInputKind::Press,
                       wxWinUIInputButton::Left),
                Native(a));
        const wxWinUIInputAction *mouseDown =
            FindDispatch(preparedMouse);
        REQUIRE(mouseDown);

        pen.kind = wxWinUIInputKind::Cancel;
        const wxWinUIInputTransition canceled =
            state.Route(
                pen, Surface(wxWinUIInputSurface::Indeterminate));
        CHECK(canceled.CountActions(
                  wxWinUIInputActionKind::ReleaseCapture) == 1);
        CHECK(canceled.CountActions(
                  wxWinUIInputActionKind::CancelPress) == 1);
        CHECK_FALSE(state.CommitBeforeDispatch(*mouseDown));
    }

    SECTION("cancellation tombstones are serial-bound and acknowledged")
    {
        wxWinUIInputState state;
        wxWinUIPointerSample left =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left);
        left.pointerId = 1;
        RouteCommitted(state, left, Native(a));

        wxWinUIPointerSample right =
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Right);
        right.pointerId = 2;
        RouteCommitted(state, right, Native(a));

        left.kind = wxWinUIInputKind::Cancel;
        const wxWinUIInputTransition canceled =
            state.Route(
                left, Surface(wxWinUIInputSurface::Indeterminate));
        const wxWinUIInputAction *cancel =
            FindAction(canceled, wxWinUIInputActionKind::CancelPress);
        REQUIRE(cancel);
        CHECK(cancel->button == wxWinUIInputButton::Left);
        CHECK(state.CanDeliverCancellation(*cancel));

        left.kind = wxWinUIInputKind::Press;
        const wxWinUIInputTransition replacement =
            state.Route(left, Native(a));
        const wxWinUIInputAction *replacementDown =
            FindDispatch(replacement);
        REQUIRE(replacementDown);
        CHECK_FALSE(state.CommitBeforeDispatch(*replacementDown));

        wxWinUIInputAction stale = *cancel;
        ++stale.gestureSerial;
        CHECK_FALSE(state.CanDeliverCancellation(stale));
        state.AcknowledgeCancellation(stale);
        CHECK_FALSE(state.CommitBeforeDispatch(*replacementDown));

        state.AcknowledgeCancellation(*cancel);
        CHECK_FALSE(state.CanDeliverCancellation(*cancel));
        REQUIRE(state.CommitBeforeDispatch(*replacementDown));

        right.kind = wxWinUIInputKind::Release;
        const wxWinUIInputAction *rightUp = FindDispatch(
            state.Route(right, Native(a)));
        REQUIRE(rightUp);
        REQUIRE(state.CommitBeforeDispatch(*rightUp));
    }

    SECTION("failed release abort is serial-safe and gates replacement")
    {
        wxWinUIInputState state;
        RouteCommitted(state,
            Sample(wxWinUIInputKind::Press,
                   wxWinUIInputButton::Left,
                   10, 20, 100),
            Native(a));
        const wxWinUIInputTransition release =
            state.Route(
                Sample(wxWinUIInputKind::Release,
                       wxWinUIInputButton::Left,
                       10, 20, 120),
                Native(a));
        const wxWinUIInputAction *up = FindDispatch(release);
        REQUIRE(up);

        wxWinUIInputAction cancel;
        REQUIRE(state.AbortUndeliverableRelease(*up, &cancel));
        CHECK(cancel.action == wxWinUIInputActionKind::CancelPress);
        CHECK(cancel.target == a);
        CHECK(state.CanDeliverCancellation(cancel));

        const wxWinUIInputTransition replacement =
            state.Route(
                Sample(wxWinUIInputKind::Press,
                       wxWinUIInputButton::Left,
                       10, 20, 200),
                Native(a));
        const wxWinUIInputAction *replacementDown =
            FindDispatch(replacement);
        REQUIRE(replacementDown);
        CHECK(replacementDown->kind == wxWinUIInputKind::Press);
        CHECK_FALSE(state.CommitBeforeDispatch(*replacementDown));

        state.AcknowledgeCancellation(cancel);
        CHECK_FALSE(state.CanDeliverCancellation(cancel));
        REQUIRE(state.CommitBeforeDispatch(*replacementDown));
        CHECK_FALSE(state.AbortUndeliverableRelease(*up, &cancel));
    }

    SECTION("normal capture release preserves the completed click")
    {
        wxWinUIClickSettings settings;
        settings.maxInterval = 500;
        settings.rectangleWidth = 8;
        settings.rectangleHeight = 8;
        wxWinUIInputState state(settings);
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left,
                           10, 20, 100), Native(a));
        state.NativeCaptureChanged(
            a, Sample(wxWinUIInputKind::Move));
        RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                           wxWinUIInputButton::Left,
                           10, 20, 120), Native(a));
        wxWinUIPointerSample lost =
            Sample(wxWinUIInputKind::CaptureLost);
        lost.interruptAll = true;
        CHECK(RouteCommitted(state,
                  lost,
                  Surface(wxWinUIInputSurface::Indeterminate)).actions.empty());

        const wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left,
                               11, 21, 250), Native(a));
        const wxWinUIInputAction *dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->kind == wxWinUIInputKind::DoubleClick);
    }

    SECTION("idle cancel starts a new click epoch")
    {
        wxWinUIInputState state;
        RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                           wxWinUIInputButton::Left,
                           10, 20, 100), Native(a));
        RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                           wxWinUIInputButton::Left,
                           10, 20, 120), Native(a));
        wxWinUIPointerSample cancel = Sample(wxWinUIInputKind::Cancel);
        cancel.interruptAll = true;
        CHECK(RouteCommitted(state,
                  cancel,
                  Surface(wxWinUIInputSurface::Indeterminate)).actions.empty());

        const wxWinUIInputTransition transition =
            RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                               wxWinUIInputButton::Left,
                               10, 20, 200), Native(a));
        const wxWinUIInputAction *dispatch = FindDispatch(transition);
        REQUIRE(dispatch);
        CHECK(dispatch->kind == wxWinUIInputKind::Press);
    }
}

TEST_CASE("wxWinUI input state V0 touch and pen policy",
          "[winui-input-state]")
{
    const auto native = Native(Target(1));
    wxWinUIInputState state;

    wxWinUIPointerSample secondary =
        Sample(wxWinUIInputKind::Press, wxWinUIInputButton::None);
    secondary.device = wxWinUIInputDevice::Touch;
    secondary.pointerId = 2;
    secondary.isPrimary = false;
    wxWinUIInputTransition transition = RouteCommitted(state, secondary, native);
    CHECK(transition.actions.empty());
    CHECK_FALSE(transition.cancelSource);
    CHECK(transition.disposition ==
          wxWinUIInputDisposition::SecondaryPointer);

    wxWinUIPointerSample touch =
        Sample(wxWinUIInputKind::Press, wxWinUIInputButton::None);
    touch.device = wxWinUIInputDevice::Touch;
    touch.pointerId = 1;
    transition = RouteCommitted(state, touch, native);
    const wxWinUIInputAction *dispatch = FindDispatch(transition);
    REQUIRE(dispatch);
    CHECK(dispatch->button == wxWinUIInputButton::Left);

    wxWinUIPointerSample promoted =
        Sample(wxWinUIInputKind::Press, wxWinUIInputButton::Left);
    promoted.device = wxWinUIInputDevice::Mouse;
    promoted.isCompatibilityMouse = true;
    transition = RouteCommitted(state, promoted, native);
    CHECK(transition.actions.empty());
    CHECK_FALSE(transition.cancelSource);
    CHECK(transition.disposition ==
          wxWinUIInputDisposition::CompatibilityDuplicate);

    touch.kind = wxWinUIInputKind::Release;
    transition = RouteCommitted(state, touch, native);
    dispatch = FindDispatch(transition);
    REQUIRE(dispatch);
    CHECK(dispatch->kind == wxWinUIInputKind::Release);
    CHECK(dispatch->button == wxWinUIInputButton::Left);

    wxWinUIPointerSample pen =
        Sample(wxWinUIInputKind::Press, wxWinUIInputButton::None);
    pen.device = wxWinUIInputDevice::Pen;
    pen.pointerId = 9;
    transition = RouteCommitted(state, pen, native);
    dispatch = FindDispatch(transition);
    REQUIRE(dispatch);
    CHECK(dispatch->button == wxWinUIInputButton::Left);

    pen.kind = wxWinUIInputKind::Release;
    RouteCommitted(state, pen, native);
    pen.kind = wxWinUIInputKind::Press;
    pen.button = wxWinUIInputButton::Right;
    transition = RouteCommitted(state, pen, native);
    dispatch = FindDispatch(transition);
    REQUIRE(dispatch);
    CHECK(dispatch->button == wxWinUIInputButton::Right);

    SECTION("pointer identity balances only its own press")
    {
        wxWinUIInputState pointerState;
        wxWinUIPointerSample primary =
            Sample(wxWinUIInputKind::Press, wxWinUIInputButton::None);
        primary.device = wxWinUIInputDevice::Touch;
        primary.pointerId = 7;
        RouteCommitted(pointerState, primary, native);

        wxWinUIPointerSample wrong =
            Sample(wxWinUIInputKind::Release,
                   wxWinUIInputButton::Left);
        CHECK(RouteCommitted(pointerState,
                  wrong,
                  Surface(wxWinUIInputSurface::Outside)).actions.empty());

        wxWinUIPointerSample secondaryCancel =
            Sample(wxWinUIInputKind::CaptureLost);
        secondaryCancel.device = wxWinUIInputDevice::Touch;
        secondaryCancel.pointerId = 8;
        secondaryCancel.isPrimary = false;
        const wxWinUIInputTransition ignored =
            RouteCommitted(pointerState,
                secondaryCancel,
                Surface(wxWinUIInputSurface::Indeterminate));
        CHECK(ignored.actions.empty());
        CHECK(ignored.disposition ==
              wxWinUIInputDisposition::SecondaryPointer);

        primary.kind = wxWinUIInputKind::Release;
        const wxWinUIInputTransition balanced =
            RouteCommitted(pointerState,
                primary,
                Surface(wxWinUIInputSurface::Outside));
        dispatch = FindDispatch(balanced);
        REQUIRE(dispatch);
        CHECK(dispatch->target == Target(1));
    }
}

TEST_CASE("wxWinUI input state proof-driven storm guard",
          "[winui-input-state]")
{
    const auto native = Native(Target(1));

    SECTION("unarmed guard never changes delivery")
    {
        wxWinUIInputState state;
        for ( unsigned i = 0; i < 12; ++i )
        {
            const wxWinUIInputTransition transition =
                RouteCommitted(state, Sample(wxWinUIInputKind::Move), native);
            CHECK(transition.CountActions(
                      wxWinUIInputActionKind::Dispatch) == 1);
            CHECK_FALSE(transition.HasAction(
                wxWinUIInputActionKind::BreakStorm));
        }
        CHECK(state.GetStormInterventionCount() == 0);
    }

    SECTION("identical redundant moves are bounded")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(2, 3, 2);
        unsigned suppressed = 0;
        unsigned delivered = 0;
        for ( unsigned i = 0; i < 6; ++i )
        {
            wxWinUIPointerSample move =
                Sample(wxWinUIInputKind::Move,
                       wxWinUIInputButton::None,
                       10, 20, 100 + i);
            const wxWinUIInputTransition transition =
                RouteCommitted(state, move, native);
            if ( transition.disposition ==
                    wxWinUIInputDisposition::StormSuppressed )
            {
                ++suppressed;
                CHECK(transition.HasAction(
                    wxWinUIInputActionKind::BreakStorm));
            }
            if ( FindDispatch(transition) )
                ++delivered;
        }
        CHECK(suppressed == 2);
        CHECK(delivered == 4);
        CHECK(state.GetStormInterventionCount() == 2);
        CHECK_FALSE(state.IsStormGuardArmed());
    }

    SECTION("a repeating two-position cycle requests a break without loss")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(1, 3, 2);
        wxWinUIInputTransition transition;
        for ( unsigned i = 0; i < 6; ++i )
        {
            transition = RouteCommitted(state,
                Sample(wxWinUIInputKind::Move,
                       wxWinUIInputButton::None,
                       (i % 2) ? 12 : 10, 20, 100 + i), native);
        }
        CHECK(transition.stormDecision ==
              wxWinUIInputStormDecision::BreakCycle);
        CHECK(transition.HasAction(wxWinUIInputActionKind::BreakStorm));
        CHECK(FindDispatch(transition));
        CHECK_FALSE(state.IsStormGuardArmed());
    }

    SECTION("an armed enter-leave cycle is detected and bounded")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(1, 3, 2);
        wxWinUIInputTransition transition;
        for ( unsigned i = 0; i < 6; ++i )
        {
            transition = RouteCommitted(state,
                Sample((i % 2) ? wxWinUIInputKind::Leave
                               : wxWinUIInputKind::Enter,
                       wxWinUIInputButton::None,
                       10, 20, 100 + i),
                native);
        }
        CHECK(transition.stormDecision ==
              wxWinUIInputStormDecision::BreakCycle);
        CHECK(transition.HasAction(wxWinUIInputActionKind::BreakStorm));
        CHECK_FALSE(state.IsStormGuardArmed());
    }

    SECTION("root boundary cycles are detected without native dispatch")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(1, 3, 2);
        wxWinUIInputTransition transition;
        for ( unsigned i = 0; i < 6; ++i )
        {
            const bool entering = (i % 2) == 0;
            transition = state.RouteBoundary(
                Sample(entering ? wxWinUIInputKind::Enter
                                : wxWinUIInputKind::Leave,
                       wxWinUIInputButton::None,
                       10, 20, 100 + i));
            CHECK(transition.disposition ==
                  wxWinUIInputDisposition::Outside);
            CHECK_FALSE(transition.cancelSource);
            CHECK_FALSE(FindDispatch(transition));
            CHECK_FALSE(transition.HasAction(
                wxWinUIInputActionKind::HoverEnter));
        }
        CHECK(transition.stormDecision ==
              wxWinUIInputStormDecision::BreakCycle);
        CHECK(transition.HasAction(wxWinUIInputActionKind::BreakStorm));
        CHECK_FALSE(state.IsStormGuardArmed());
    }

    SECTION("a real revisiting path outside the proven cadence is delivered")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(1, 3, 2, 4);
        for ( unsigned i = 0; i < 12; ++i )
        {
            wxWinUIPointerSample move =
                Sample(wxWinUIInputKind::Move,
                       wxWinUIInputButton::None,
                       (i % 2) ? 12 : 10, 20,
                       100 + 10 * i);
            const wxWinUIInputTransition transition =
                RouteCommitted(state, move, native);
            CHECK(FindDispatch(transition));
            CHECK_FALSE(transition.HasAction(
                wxWinUIInputActionKind::BreakStorm));
        }
        CHECK(state.GetStormInterventionCount() == 0);
    }

    SECTION("significant state changes are not equal signatures")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(1, 3, 2);
        for ( unsigned modifier = 1; modifier <= 3; ++modifier )
        {
            wxWinUIPointerSample move = Sample(wxWinUIInputKind::Move);
            move.modifiers = modifier;
            const wxWinUIInputTransition transition =
                RouteCommitted(state, move, native);
            CHECK(FindDispatch(transition));
            CHECK_FALSE(transition.HasAction(
                wxWinUIInputActionKind::BreakStorm));
        }

        state.ArmStormGuard(1, 3, 2);
        for ( unsigned mask = 1; mask <= 3; ++mask )
        {
            wxWinUIPointerSample move = Sample(wxWinUIInputKind::Move);
            move.buttonMask = mask;
            const wxWinUIInputTransition transition =
                RouteCommitted(state, move, native);
            CHECK(FindDispatch(transition));
            CHECK_FALSE(transition.HasAction(
                wxWinUIInputActionKind::BreakStorm));
        }
    }

    SECTION("press release and wheel are never suppressible")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(20, 2, 4);
        for ( unsigned i = 0; i < 6; ++i )
        {
            wxWinUIInputTransition transition =
                RouteCommitted(state, Sample(wxWinUIInputKind::Press,
                                   wxWinUIInputButton::Left),
                            native);
            CHECK(FindDispatch(transition));
            CHECK_FALSE(transition.HasAction(
                wxWinUIInputActionKind::BreakStorm));

            transition =
                RouteCommitted(state, Sample(wxWinUIInputKind::Release,
                                   wxWinUIInputButton::Left),
                            native);
            CHECK(FindDispatch(transition));
            CHECK_FALSE(transition.HasAction(
                wxWinUIInputActionKind::BreakStorm));

            wxWinUIPointerSample wheel =
                Sample(wxWinUIInputKind::Wheel);
            wheel.wheelDelta = 120;
            transition = RouteCommitted(state, wheel, native);
            CHECK(FindDispatch(transition));
            CHECK_FALSE(transition.HasAction(
                wxWinUIInputActionKind::BreakStorm));
        }
        CHECK(state.GetStormInterventionCount() == 0);
    }

    SECTION("cancel resets an armed partial signature")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(1, 3, 1);
        for ( unsigned i = 0; i < 2; ++i )
        {
            wxWinUIPointerSample move =
                Sample(wxWinUIInputKind::Move,
                       wxWinUIInputButton::None,
                       10, 20, 100 + i);
            CHECK_FALSE(RouteCommitted(state, move, native).HasAction(
                wxWinUIInputActionKind::BreakStorm));
        }

        RouteCommitted(state, Sample(wxWinUIInputKind::Cancel),
                    Surface(wxWinUIInputSurface::Indeterminate));
        for ( unsigned i = 0; i < 2; ++i )
        {
            wxWinUIPointerSample move =
                Sample(wxWinUIInputKind::Move,
                       wxWinUIInputButton::None,
                       10, 20, 102 + i);
            CHECK_FALSE(RouteCommitted(state, move, native).HasAction(
                wxWinUIInputActionKind::BreakStorm));
        }
    }

    SECTION("hostile arm parameters remain hard bounded")
    {
        wxWinUIInputState state;
        state.ArmStormGuard(~0u, ~0u, ~0u, ~std::uint64_t(0));
        unsigned interventions = 0;
        for ( unsigned i = 0; i < 64; ++i )
        {
            const wxWinUIInputTransition transition =
                RouteCommitted(state, Sample(wxWinUIInputKind::Move), native);
            if ( transition.HasAction(
                    wxWinUIInputActionKind::BreakStorm) )
            {
                ++interventions;
            }
        }
        CHECK(interventions <= 16);
        CHECK_FALSE(state.IsStormGuardArmed());
        CHECK(wxWinUIInputActions::Capacity == 12);
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3
