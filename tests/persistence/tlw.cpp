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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "testpersistence.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
#endif // WX_PRECOMP

#include "wx/persist/toplevel.h"

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

        delete frame;
    }
}
