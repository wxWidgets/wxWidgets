///////////////////////////////////////////////////////////////////////////////
// Name:        tests/persistence/persistence.cpp
// Purpose:     wxTLW persistence support unit test
// Author:      wxWidgets Team
// Created:     2017-08-23
// Copyright:   (c) 2017 wxWidgets Team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#include "testpersistence.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
#endif // WX_PRECOMP

#include "wx/persist/toplevel.h"

#ifdef __WXGTK__
    #include "waitfor.h"
    #include "wx/gtk/private/backend.h"
#endif // __WXGTK__

#ifdef __WXMSW__
    #include "asserthelper.h"

    #include "wx/display.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define FRAME_OPTIONS_PREFIX   PO_PREFIX "/Window/frame"

// ----------------------------------------------------------------------------
// local helpers
// ----------------------------------------------------------------------------

namespace
{

// Create the frame used for testing.
std::unique_ptr<wxFrame> CreatePersistenceTestFrame()
{
    auto frame =
        make_unique<wxFrame>(wxTheApp->GetTopWindow(), wxID_ANY, "wxTest");
    frame->SetName("frame");

    return frame;
}

void SavePersistenceTestFrame(const wxPoint& pos, const wxSize& size)
{
    auto frame = CreatePersistenceTestFrame();
    frame->SetPosition(pos);
    frame->SetSize(size);

    CHECK(wxPersistenceManager::Get().Register(frame.get()));

    // Destroy the frame immediately to cause its geometry to be saved.
}

std::unique_ptr<wxFrame> RestorePersistenceTestFrame()
{
    auto frame = CreatePersistenceTestFrame();

    CHECK(wxPersistenceManager::Get().RegisterAndRestore(frame.get()));

    return frame;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(PersistenceTests, "wxPersistTLW", "[persist][tlw]")
{
    const wxPoint pos(100, 150);
    const wxSize size(450, 350);

    // Wayland doesn't allow clients to position their own top-level windows
    // at all, unlike X11, so don't check the restored position there.
    const bool checkPosition = !IsRunningUnderWayland();

    // Save the frame geometry.
    {
        SavePersistenceTestFrame(pos, size);

        // Test that the relevant keys have been stored correctly.
        int val = -1;

        CHECK(GetConfig().Read(FRAME_OPTIONS_PREFIX "/x", &val));
        CHECK(pos.x == val);

        CHECK(GetConfig().Read(FRAME_OPTIONS_PREFIX "/y", &val));
        CHECK(pos.y == val);

        CHECK(GetConfig().Read(FRAME_OPTIONS_PREFIX "/w", &val));
        CHECK(size.x == val);

        CHECK(GetConfig().Read(FRAME_OPTIONS_PREFIX "/h", &val));
        CHECK(size.y == val);

        CHECK(GetConfig().Read(FRAME_OPTIONS_PREFIX "/Iconized", &val));
        CHECK(0 == val);

        CHECK(GetConfig().Read(FRAME_OPTIONS_PREFIX "/Maximized", &val));
        CHECK(0 == val);
    }

    // Now try recreating the frame using the restored values.
    bool checkIconized = true;
    {
        auto const frame = RestorePersistenceTestFrame();

        // Test that the object was restored.
        if ( checkPosition )
        {
            CHECK(pos.x == frame->GetPosition().x);
            CHECK(pos.y == frame->GetPosition().y);
        }
        CHECK(size.x == frame->GetSize().GetWidth());
        CHECK(size.y == frame->GetSize().GetHeight());
        CHECK(!frame->IsMaximized());
        CHECK(!frame->IsIconized());

        // Next try that restoring a minimized frame works correctly: for
        // Iconize() to have effect, we must show the frame first.
        frame->Iconize();
        frame->Show();

#ifdef __WXGTK__
        // When using Xvfb, the frame will never get iconized, presumably
        // because there is no WM, so don't even bother waiting or warning.
        //
        // Also skip this check under Wayland where we use a headless
        // compositor without WM as well.
        if ( IsRunningUnderXVFB() || IsRunningUnderWayland() )
        {
            checkIconized = false;
        }
        else
        {
            if ( !WaitFor("frame to be iconized", [&]() {
                        return frame->IsIconized();
                    }) )
            {
                checkIconized = false;
            }
        }
#endif // __WXGTK__
    }

    // Check geometry after restoring the minimized frame.
    {
        auto const frame = RestorePersistenceTestFrame();

        // As above, we need to show the frame for it to be actually iconized.
        frame->Show();

        CHECK(!frame->IsMaximized());
        if ( checkIconized )
        {
#ifdef __WXGTK__
            WaitFor("frame to be iconized", [&]() {
                return frame->IsIconized();
            });
#endif // __WXGTK__

            CHECK(frame->IsIconized());
        }

        frame->Restore();

        if ( checkPosition )
        {
            CHECK(pos.x == frame->GetPosition().x);
            CHECK(pos.y == frame->GetPosition().y);
        }
        CHECK(size.x == frame->GetSize().GetWidth());
        CHECK(size.y == frame->GetSize().GetHeight());

        // Next try that restoring a maximized frame works correctly: again,
        // for it to be really maximized, it must be shown.
        frame->Maximize();
        frame->Show();
    }

    // Check geometry after restoring the maximized frame.
    //
    // This test currently fails under non-MSW platforms as they only save the
    // maximized frame size, and its normal size is lost and can't be restored.
#ifdef __WXMSW__
    {
        auto const frame = RestorePersistenceTestFrame();

        CHECK(frame->IsMaximized());
        CHECK(!frame->IsIconized());

        frame->Restore();

        CHECK(pos.x == frame->GetPosition().x);
        CHECK(pos.y == frame->GetPosition().y);
        CHECK(size.x == frame->GetSize().GetWidth());
        CHECK(size.y == frame->GetSize().GetHeight());
    }
#endif // __WXMSW__
}

// This test is MSW-specific because the generic implementation used elsewhere
// has its own, different, check for the window being off screen.
#ifdef __WXMSW__

TEST_CASE_METHOD(PersistenceTests, "wxPersistTLW::OffScreen", "[persist][tlw]")
{
    const wxSize size(450, 350);

    // Find a position at which only a small part of the frame is inside a
    // display: this is what happens when the geometry saved while using a
    // bigger desktop is restored on a smaller one, e.g. after disconnecting a
    // monitor or when connecting to the machine remotely.
    wxPoint pos;
    const unsigned count = wxDisplay::GetCount();
    for ( unsigned n = 0; n < count; n++ )
    {
        pos = wxDisplay(n).GetClientArea().GetBottomRight() - wxPoint(20, 20);

        if ( wxDisplay::GetFromPoint(pos) != wxNOT_FOUND &&
             wxDisplay::GetFromPoint(pos + size) == wxNOT_FOUND )
        {
            // Found a suitable position.
            break;
        }

        pos = wxDefaultPosition;
    }

    if ( pos == wxDefaultPosition )
    {
        WARN("Unexpectedly didn't find a suitable position, skipping the test");
        return;
    }

    // Simulate the geometry saved by a previous version of the program, which
    // didn't record whether the window was off screen at all.
    const auto saveGeometry = [this, size](const wxPoint& posSaved)
    {
        wxConfigBase& config = GetConfig();

        config.Write(FRAME_OPTIONS_PREFIX "/x", posSaved.x);
        config.Write(FRAME_OPTIONS_PREFIX "/y", posSaved.y);
        config.Write(FRAME_OPTIONS_PREFIX "/w", size.x);
        config.Write(FRAME_OPTIONS_PREFIX "/h", size.y);

        // The previous versions didn't save this value at all.
        config.DeleteEntry(FRAME_OPTIONS_PREFIX "/offscreen");
    };

    // Without this entry the window is assumed to have been fully visible
    // when its geometry was saved and so has to be moved back on screen.
    SECTION("Fix up")
    {
        saveGeometry(pos);

        auto const frame = RestorePersistenceTestFrame();

        // The frame shouldn't have been left almost completely off screen.
        const wxRect rect = frame->GetScreenRect();
        CHECK(wxDisplay::GetFromPoint(rect.GetTopLeft()) != wxNOT_FOUND);
        CHECK(wxDisplay::GetFromPoint(rect.GetBottomRight()) != wxNOT_FOUND);

        // The size should have been preserved.
        CHECK(size == rect.GetSize());
    }

    // But if the window had already been off screen when its geometry was
    // saved, it had been put there on purpose and must be left alone.
    SECTION("Preserve")
    {
        saveGeometry(pos);
        GetConfig().Write(FRAME_OPTIONS_PREFIX "/offscreen", 1);

        auto const frame = RestorePersistenceTestFrame();

        CHECK(wxRect(pos, size) == frame->GetScreenRect());
    }

    // Even a window which had been deliberately moved off screen must be
    // moved back if it wouldn't be visible at all, as the user couldn't move
    // it back in this case.
    SECTION("Fix up hidden")
    {
        const wxRect rectPrimary = wxDisplay().GetClientArea();
        const wxPoint posHidden =
            rectPrimary.GetTopLeft() - wxPoint(10000, 10000);
        REQUIRE(wxDisplay::GetFromRect(wxRect(posHidden, size)) == wxNOT_FOUND);

        saveGeometry(posHidden);
        GetConfig().Write(FRAME_OPTIONS_PREFIX "/offscreen", 1);

        auto const frame = RestorePersistenceTestFrame();

        // The frame should have been moved to the position inside the primary
        // display closest to its saved one, i.e. its top left corner.
        CHECK(wxRect(rectPrimary.GetTopLeft(), size) == frame->GetScreenRect());
    }

    // Check that saving the geometry of a window which is off screen does set
    // the flag relied upon by the tests above.
    SECTION("Save")
    {
        SavePersistenceTestFrame(pos, size);

        int val = -1;
        REQUIRE(GetConfig().Read(FRAME_OPTIONS_PREFIX "/offscreen", &val));
        CHECK(val == 1);
    }
}

#endif // __WXMSW__
