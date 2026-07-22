///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuihostlifecycle.cpp
// Purpose:     lifecycle tests of the shared wxWinUI top-level host
// Author:      wxWidgets development team
// Created:     2026-07-22
// Copyright:   (c) wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/winui/private/tlwhost.h"

namespace
{

// Let the coalesced flushes, pending deletes and XAML finalization drain.
void DrainDispatch(int rounds = 20)
{
    for ( int i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(5);
    }
}

} // anonymous namespace

TEST_CASE("HostLifecycle::RegisterUnregisterLoop", "[HostLifecycle]")
{
    const unsigned slots0 = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 = wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    for ( int i = 0; i < 100; ++i )
    {
        wxButton * const button =
            new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "lifecycle");
        delete button;
    }

    DrainDispatch();

    // no slot may leak, and -- the real point -- no callback state may
    // outlive its slot: every handler must have been revoked
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);

    // and not just by the shared state going null: every AddHandler /
    // focus subscription of the loop was individually revoked
    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

TEST_CASE("HostLifecycle::DestroyTLWWithSlots", "[HostLifecycle]")
{
    const unsigned hosts0 = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 = wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "lifecycle-tlw");
    for ( int i = 0; i < 10; ++i )
        new wxButton(frame, wxID_ANY, wxString::Format("b%d", i));
    frame->Show();
    DrainDispatch(5);

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() > hosts0);

    frame->Destroy();
    DrainDispatch();

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);

    // the TLW teardown goes through Shutdown(), not UnregisterSlot(): that
    // path too must revoke every single handler before closing the source
    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

TEST_CASE("HostLifecycle::ReparentAcrossTLW", "[HostLifecycle]")
{
    const unsigned hosts0 = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 = wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    wxFrame * const frameA = new wxFrame(nullptr, wxID_ANY, "lifecycle-A");
    wxFrame * const frameB = new wxFrame(nullptr, wxID_ANY, "lifecycle-B");
    wxButton * const button = new wxButton(frameA, wxID_ANY, "migrant");
    frameA->Show();
    frameB->Show();
    DrainDispatch(5);

    for ( int i = 0; i < 20; ++i )
    {
        wxFrame * const target = i % 2 ? frameA : frameB;
        wxFrame * const other = i % 2 ? frameB : frameA;

        REQUIRE(button->Reparent(target));
        // the migration happens in the coalesced flush
        DrainDispatch(3);
        // the control must stay alive and usable after each migration
        button->SetLabel(wxString::Format("migrant %d", i));

        // exactly one host owns the slot after the migration: the target's
        wxWinUITopLevelHost * const hostTarget =
            wxWinUITopLevelHost::FindForTLW(target);
        REQUIRE(hostTarget != nullptr);
        CHECK(hostTarget->FindSlot(button) != nullptr);

        wxWinUITopLevelHost * const hostOther =
            wxWinUITopLevelHost::FindForTLW(other);
        if ( hostOther )
            CHECK(hostOther->FindSlot(button) == nullptr);
    }

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);

    // every migration is an unregister + register pair: the handler
    // add/revoke balance must close over the whole shuffle
    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

TEST_CASE("HostLifecycle::FreezeThawCoalescedFlush", "[HostLifecycle]")
{
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "lifecycle-freeze");
    wxButton * const button = new wxButton(frame, wxID_ANY, "frozen");
    frame->Show();
    DrainDispatch(5);

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    REQUIRE(host->FindSlot(button) != nullptr);

    frame->Freeze();

    const unsigned sched0 = wxWinUITopLevelHost::GetFlushScheduleCount();
    wxPoint last;
    for ( int i = 0; i < 50; ++i )
    {
        last = wxPoint(10 + i, 10);
        button->Move(last);
    }
    DrainDispatch(5);

    // bounded freeze: a Move storm under Freeze() must not turn into a
    // CallAfter storm (at most one stray pre-freeze flush may still post)
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() - sched0 <= 1);

    const unsigned runs0 = wxWinUITopLevelHost::GetFlushRunCount();
    frame->Thaw();
    DrainDispatch(5);

    // the single owed catch-up flush ran and brought the geometry current
    CHECK(wxWinUITopLevelHost::GetFlushRunCount() > runs0);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    CHECK(slot->GetRectInTLW().x == last.x);
    CHECK(slot->GetRectInTLW().y == last.y);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::DestroyWithFlushInFlight", "[HostLifecycle]")
{
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();

    for ( int i = 0; i < 25; ++i )
    {
        wxButton * const button =
            new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "in-flight");
        // queue a coalesced flush (CallAfter) ...
        button->Move(10 + i, 10);
        // ... and destroy the window before it runs: the flush must cope
        delete button;
    }

    DrainDispatch();
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);
}

#endif // __WXWINUI__ && wxUSE_WINUI3
