///////////////////////////////////////////////////////////////////////////////
// Name:        tests/toplevel/toplevel.cpp
// Purpose:     Tests for wxTopLevelWindow
// Author:      Kevin Ollivier
// Created:     2008-05-25
// Copyright:   (c) 2009 Kevin Ollivier <kevino@theolliviers.com>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"
    #include "wx/textctrl.h"
    #include "wx/toplevel.h"
#endif // WX_PRECOMP

#include "testableframe.h"

namespace
{
int waitForEvent(EventCounter &countShow)
{
    // Wait for the event to be received for the maximum of 2 seconds.
    int showCount = 0;
    for ( int i = 0; i < 40; i++ )
    {
        wxYield();

        showCount = countShow.GetCount();

        if ( showCount )
            break;

        wxMilliSleep(50);
    }
    return showCount;
}

void TopLevelWindowShowTest(wxTopLevelWindow* tlw)
{
    CHECK(!tlw->IsShown());
#ifdef __WXQT__
    EventCounter countShow(tlw, wxEVT_ACTIVATE);
#endif

    wxTextCtrl* textCtrl = new wxTextCtrl(tlw, -1, "test");
    textCtrl->SetFocus();

// only run this test on platforms where ShowWithoutActivating is implemented.
#if defined(__WXMSW__) || defined(__WXMAC__)
    wxTheApp->GetTopWindow()->SetFocus();
    tlw->ShowWithoutActivating();
    CHECK(tlw->IsShown());
    CHECK(!tlw->IsActive());

    tlw->Hide();
    CHECK(!tlw->IsShown());
    CHECK(!tlw->IsActive());
#endif

    tlw->Show(true);

    // wxQT needs one or two rounds of yield for window to become activated
#ifdef __WXQT__
    waitForEvent(countShow);
#endif

    // wxGTK needs many event loop iterations before the TLW becomes active and
    // this doesn't happen in this test, so avoid checking for it.
#ifndef __WXGTK__
    CHECK(tlw->IsActive());
#endif
    CHECK(tlw->IsShown());

    tlw->Hide();
    CHECK(!tlw->IsShown());
#ifndef __WXGTK__
    CHECK(!tlw->IsActive());
#endif
}
}

TEST_CASE("wxTopLevel::Show", "[tlw][show]")
{
    SECTION("Dialog")
    {
        wxDialog* dialog = new wxDialog(NULL, -1, "Dialog Test");
        TopLevelWindowShowTest(dialog);
        dialog->Destroy();
    }

    SECTION("Frame")
    {
        wxFrame* frame = new wxFrame(NULL, -1, "Frame test");
        TopLevelWindowShowTest(frame);
        frame->Destroy();
    }
}

// Check that we receive the expected event when showing the TLW.
TEST_CASE("wxTopLevel::ShowEvent", "[tlw][show][event]")
{
    wxFrame* const frame = new wxFrame(NULL, wxID_ANY, "Maximized frame");

    EventCounter countShow(frame, wxEVT_SHOW);

    frame->Maximize();
    frame->Show();

    int showCount = waitForEvent(countShow);

    CHECK( showCount == 1 );
}
