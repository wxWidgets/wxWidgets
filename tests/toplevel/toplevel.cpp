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

class DestroyOnScopeExit
{
public:
    explicit DestroyOnScopeExit(wxTopLevelWindow* tlw)
        : m_tlw(tlw)
    {
    }

    ~DestroyOnScopeExit()
    {
        m_tlw->Destroy();
    }

private:
    wxTopLevelWindow* const m_tlw;

    wxDECLARE_NO_COPY_CLASS(DestroyOnScopeExit);
};

static void TopLevelWindowShowTest(wxTopLevelWindow* tlw)
{
    CHECK(!tlw->IsShown());

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

    // Note that at least under MSW, ShowWithoutActivating() still generates
    // wxActivateEvent, so we must only start counting these events after the
    // end of the tests above.
    EventCounter countActivate(tlw, wxEVT_ACTIVATE);

    tlw->Show(true);
    countActivate.WaitEvent();

    // TLWs never become active when running under Xvfb, presumably because
    // there is no WM there.
    if ( !IsRunningUnderXVFB() )
        CHECK(tlw->IsActive());

    CHECK(tlw->IsShown());

    tlw->Hide();
    CHECK(!tlw->IsShown());

    countActivate.WaitEvent();
    CHECK(!tlw->IsActive());
}

TEST_CASE("wxTopLevel::Show", "[tlw][show]")
{
    SECTION("Dialog")
    {
        wxDialog* dialog = new wxDialog(NULL, -1, "Dialog Test");
        DestroyOnScopeExit destroy(dialog);

        TopLevelWindowShowTest(dialog);
    }

    SECTION("Frame")
    {
        wxFrame* frame = new wxFrame(NULL, -1, "Frame test");
        DestroyOnScopeExit destroy(frame);

        TopLevelWindowShowTest(frame);
    }
}

// Check that we receive the expected event when showing the TLW.
TEST_CASE("wxTopLevel::ShowEvent", "[tlw][show][event]")
{
    wxFrame* const frame = new wxFrame(NULL, wxID_ANY, "Maximized frame");
    DestroyOnScopeExit destroy(frame);

    EventCounter countShow(frame, wxEVT_SHOW);

    frame->Maximize();
    frame->Show();

    CHECK( countShow.WaitEvent() );
}
