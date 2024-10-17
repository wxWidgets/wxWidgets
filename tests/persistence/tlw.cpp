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
#endif // __WXGTK__

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define FRAME_OPTIONS_PREFIX   PO_PREFIX "/Window/frame"

// ----------------------------------------------------------------------------
// local helpers
// ----------------------------------------------------------------------------

// Create the frame used for testing.
static wxFrame* CreatePersistenceTestFrame()
{
    wxFrame* const frame = new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY, "wxTest");
    frame->SetName("frame");

    return frame;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(PersistenceTests, "wxPersistTLW", "[persist][tlw]")
{
    const wxPoint pos(100, 150);
    const wxSize size(450, 350);

    // Save the frame geometry.
    {
        wxFrame* const frame = CreatePersistenceTestFrame();

        // Set the geometry before saving.
        frame->SetPosition(pos);
        frame->SetSize(size);

        CHECK(wxPersistenceManager::Get().Register(frame));

        // Destroy the frame immediately, i.e. don't use Destroy() here.
        delete frame;

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
        wxFrame* const frame = CreatePersistenceTestFrame();

        // Test that the object was registered and restored.
        CHECK(wxPersistenceManager::Get().RegisterAndRestore(frame));

        CHECK(pos.x == frame->GetPosition().x);
        CHECK(pos.y == frame->GetPosition().y);
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
        if ( IsRunningUnderXVFB() )
        {
            checkIconized = false;
        }
        else
        {
            if ( !WaitFor("frame to be iconized", [frame]() {
                        return frame->IsIconized();
                    }) )
            {
                checkIconized = false;
            }
        }
#endif // __WXGTK__

        delete frame;
    }

    // Check geometry after restoring the minimized frame.
    {
        wxFrame* const frame = CreatePersistenceTestFrame();

        CHECK(wxPersistenceManager::Get().RegisterAndRestore(frame));

        // As above, we need to show the frame for it to be actually iconized.
        frame->Show();

        CHECK(!frame->IsMaximized());
        if ( checkIconized )
        {
#ifdef __WXGTK__
            WaitFor("frame to be iconized", [frame]() {
                return frame->IsIconized();
            });
#endif // __WXGTK__

            CHECK(frame->IsIconized());
        }

        frame->Restore();

        CHECK(pos.x == frame->GetPosition().x);
        CHECK(pos.y == frame->GetPosition().y);
        CHECK(size.x == frame->GetSize().GetWidth());
        CHECK(size.y == frame->GetSize().GetHeight());

        // Next try that restoring a maximized frame works correctly: again,
        // for it to be really maximized, it must be shown.
        frame->Maximize();
        frame->Show();

        delete frame;
    }

    // Check geometry after restoring the maximized frame.
    //
    // This test currently fails under non-MSW platforms as they only save the
    // maximized frame size, and its normal size is lost and can't be restored.
#ifdef __WXMSW__
    {
        wxFrame* const frame = CreatePersistenceTestFrame();

        CHECK(wxPersistenceManager::Get().RegisterAndRestore(frame));

        CHECK(frame->IsMaximized());
        CHECK(!frame->IsIconized());

        frame->Restore();

        CHECK(pos.x == frame->GetPosition().x);
        CHECK(pos.y == frame->GetPosition().y);
        CHECK(size.x == frame->GetSize().GetWidth());
        CHECK(size.y == frame->GetSize().GetHeight());

        delete frame;
    }
#endif // __WXMSW__
}
