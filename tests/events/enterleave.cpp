///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/enterleave.cpp
// Purpose:     Test wxEVT_ENTER_WINDOW and wxEVT_LEAVE_WINDOW events
// Author:      Ali Kettab
// Created:     2024-10-16
// Copyright:   (c) 2024 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/panel.h"
    #include "wx/textctrl.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/uiaction.h"

#include "asserthelper.h"
#include "testableframe.h"
#include "waitfor.h"

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE("EnterLeaveEvents", "[wxEvent][enter-leave]")
{
    if ( !EnableUITests() )
    {
        WARN("Skipping wxEVT_{ENTER,LEAVE}_WINDOW tests: wxUIActionSimulator not available");
        return;
    }

#ifdef __WINDOWS__
    if ( wxIsRunningUnderWine() )
    {
        WARN("Skipping testing wxEVT_{ENTER,LEAVE}_WINDOW known to fail under Wine");
        return;
    }
#endif // __WINDOWS__

    auto panel = make_unique<wxPanel>(wxTheApp->GetTopWindow(), wxID_ANY);
    auto button = new wxButton(panel.get(), wxID_ANY, "button", {50, 50});
    auto textctrl = new wxTextCtrl(panel.get(), wxID_ANY, "", {160, 50});

    EventCounter enter(panel.get(), wxEVT_ENTER_WINDOW);
    EventCounter leave(panel.get(), wxEVT_LEAVE_WINDOW);

    // Wait for the first paint event to be sure that panel really
    // has its final size.
    WaitForPaint waitForPaint(panel.get());
    panel->SendSizeEventToParent();
    waitForPaint.YieldUntilPainted();

    wxUIActionSimulator sim;

    wxPoint pos = panel->GetScreenPosition() + panel->GetSize();

    sim.MouseMove(pos);
    YieldForAWhile();
    enter.Clear();
    leave.Clear();

    // Make sure the mouse is outside the window before starting the tests.
    REQUIRE(panel.get() != wxFindWindowAtPoint(pos));

    SECTION("Without mouse capture")
    {
        pos = panel->GetScreenPosition() + wxPoint(5, 5);
        sim.MouseMove(pos);
        YieldForAWhile();

        REQUIRE(panel.get() == wxFindWindowAtPoint(pos));

        CHECK( enter.GetCount() == 1 );
        CHECK( leave.GetCount() == 0 );

        enter.Clear();

        pos = button->GetScreenPosition() + wxPoint(5, 5);
        sim.MouseMove(pos);
        YieldForAWhile();

        REQUIRE(panel.get() != wxFindWindowAtPoint(pos));

        // The parent window (panel) should receive wxEVT_LEAVE_WINDOW event
        // when mouse enters the child window (button)
        CHECK( enter.GetCount() == 0 );
        CHECK( leave.GetCount() == 1 );

        leave.Clear();

        pos = panel->GetScreenPosition() + wxPoint(5, 5);
        sim.MouseMove(pos);
        YieldForAWhile();

        REQUIRE(panel.get() == wxFindWindowAtPoint(pos));

        // Now it (panel) should receive wxEVT_ENTER_WINDOW event when
        // the mouse leaves the button and enters the panel again.
        CHECK( enter.GetCount() == 1 );
        CHECK( leave.GetCount() == 0 );
    }

    SECTION("With (implicit) mouse capture")
    {
        // Just to be sure that the button is really shown
        EventCounter clicked(button, wxEVT_BUTTON);

        sim.MouseMove(button->GetScreenPosition() + wxPoint(5, 5));
        YieldForAWhile();

        sim.MouseClick();
        YieldForAWhile();

        CHECK( clicked.GetCount() == 1 );

        enter.Clear();
        leave.Clear();

        sim.MouseDown();
        YieldForAWhile();

#if defined(__WXGTK__) && !defined(__WXGTK3__)
        if ( IsAutomaticTest() )
        {
            WARN("Skipping tests known to fail under GitHub Actions");
            return;
        }
#endif
        sim.MouseMove(button->GetScreenPosition() + wxPoint(10, 5));
        YieldForAWhile();

        // Holding the mouse button down (initiated on the button) and then
        // hovering over the panel should not generate any events (enter/leave)
        // Additionally, entering and leaving another child (textctrl) while the
        // mouse is still held down should also not generate any events.

        sim.MouseMove(panel->GetScreenPosition() + wxPoint(5, 5));
        YieldForAWhile();

        CHECK( enter.GetCount() == 0 );
        CHECK( leave.GetCount() == 0 );

        sim.MouseMove(textctrl->GetScreenPosition() + wxPoint(5, 5));
        YieldForAWhile();

        CHECK( enter.GetCount() == 0 );
        CHECK( leave.GetCount() == 0 );

        sim.MouseMove(panel->GetScreenPosition() + wxPoint(5, 5));
        YieldForAWhile();

        CHECK( enter.GetCount() == 0 );
        CHECK( leave.GetCount() == 0 );

        sim.MouseUp();
        YieldForAWhile();

        // wxGTK behaves differently here, as it does not generate a
        // wxEVT_ENTER_WINDOW event when we release the mouse button.

    #ifndef __WXGTK__
        CHECK( enter.GetCount() == 1 );
    #else
        CHECK( enter.GetCount() == 0 );
    #endif
        CHECK( leave.GetCount() == 0 );
    }
}

#endif // wxUSE_UIACTIONSIMULATOR
