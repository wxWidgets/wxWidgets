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


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"
    #include "wx/textctrl.h"
    #include "wx/toplevel.h"
#endif // WX_PRECOMP

#include "testableframe.h"

#ifdef __WXQT__
    #include <QtWidgets/QApplication>
    #include <QtWidgets/QWidget>
#endif

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
        wxDialog* dialog = new wxDialog(nullptr, -1, "Dialog Test");
        DestroyOnScopeExit destroy(dialog);

        TopLevelWindowShowTest(dialog);
    }

    SECTION("Frame")
    {
        wxFrame* frame = new wxFrame(nullptr, -1, "Frame test");
        DestroyOnScopeExit destroy(frame);

        TopLevelWindowShowTest(frame);
    }
}

// Check that we receive the expected event when showing the TLW.
TEST_CASE("wxTopLevel::ShowEvent", "[tlw][show][event]")
{
    wxFrame* const frame = new wxFrame(nullptr, wxID_ANY, "Maximized frame");
    DestroyOnScopeExit destroy(frame);

    EventCounter countShow(frame, wxEVT_SHOW);

    frame->Maximize();
    frame->Show();

    CHECK( countShow.WaitEvent() );
}

#ifdef __WXQT__
TEST_CASE("wxTopLevel::SizeHintsPreserveUnspecifiedBounds", "[tlw][size][qt]")
{
    wxDialog dialog(wxTheApp->GetTopWindow(), wxID_ANY, "Size hints");
    QWidget* const widget = dialog.GetHandle();
    REQUIRE(widget);

    const wxSize originalMinSize = dialog.GetMinSize();
    const wxSize originalMaxSize = dialog.GetMaxSize();
    const QSize originalNativeMinSize = widget->minimumSize();
    const QSize originalNativeMaxSize = widget->maximumSize();
    const QSize originalNativeIncrement = widget->sizeIncrement();
    REQUIRE(originalMinSize == wxDefaultSize);
    REQUIRE(originalMaxSize == wxDefaultSize);

    dialog.SetSizeHints(120, 90, 460, 360, 7, 9);
    CHECK(dialog.GetMinSize() == wxSize(120, 90));
    CHECK(dialog.GetMaxSize() == wxSize(460, 360));
    CHECK(widget->minimumSize() == QSize(120, 90));
    CHECK(widget->maximumSize() == QSize(460, 360));
    CHECK(widget->sizeIncrement() == QSize(7, 9));

    // Each coordinate has its own unspecified sentinel, not just the pair.
    dialog.SetSizeHints(wxDefaultCoord, 95, 480, wxDefaultCoord,
                        wxDefaultCoord, 4);
    CHECK(dialog.GetMinSize() == wxSize(wxDefaultCoord, 95));
    CHECK(dialog.GetMaxSize() == wxSize(480, wxDefaultCoord));
    CHECK(widget->minimumSize() == QSize(0, 95));
    CHECK(widget->maximumSize() == QSize(480, QWIDGETSIZE_MAX));
    CHECK(widget->sizeIncrement() == QSize(0, 4));

    // Use the same separate setters as dialog layout rollback. Neither may
    // normalize the other bound as a side effect of restoring its own value.
    dialog.SetMinSize(originalMinSize);
    CHECK(dialog.GetMinSize() == originalMinSize);
    CHECK(dialog.GetMaxSize() == wxSize(480, wxDefaultCoord));
    CHECK(widget->minimumSize() == originalNativeMinSize);
    CHECK(widget->maximumSize() == QSize(480, QWIDGETSIZE_MAX));
    dialog.SetMaxSize(originalMaxSize);
    CHECK(dialog.GetMinSize() == originalMinSize);
    CHECK(dialog.GetMaxSize() == originalMaxSize);
    CHECK(widget->minimumSize() == originalNativeMinSize);
    CHECK(widget->maximumSize() == originalNativeMaxSize);

    dialog.SetSizeHints(originalMinSize, originalMaxSize,
                        wxSize(originalNativeIncrement.width(),
                               originalNativeIncrement.height()));
    CHECK(dialog.GetMinSize() == originalMinSize);
    CHECK(dialog.GetMaxSize() == originalMaxSize);
    CHECK(widget->minimumSize() == originalNativeMinSize);
    CHECK(widget->maximumSize() == originalNativeMaxSize);
    CHECK(widget->sizeIncrement() == originalNativeIncrement);
}

TEST_CASE("wxTopLevel::ShowWithoutActivating", "[tlw][show][qt]")
{
    wxFrame* const active = new wxFrame(nullptr, wxID_ANY, "Active frame");
    DestroyOnScopeExit destroyActive(active);
    REQUIRE(active->Show());
    wxYield();

    // Establish a real Qt active-window baseline, without physical input or
    // assuming that the window manager activated a newly shown window.
    QApplication::setActiveWindow(active->GetHandle());
    REQUIRE(QApplication::activeWindow() == active->GetHandle());

    wxFrame* const frame = new wxFrame(nullptr, wxID_ANY, "Inactive frame");
    DestroyOnScopeExit destroy(frame);
    QWidget* const widget = frame->GetHandle();
    REQUIRE_FALSE(widget->testAttribute(Qt::WA_ShowWithoutActivating));

    frame->ShowWithoutActivating();
    wxYield();
    CHECK(frame->IsShown());
    CHECK(widget->isVisible());
    CHECK(QApplication::activeWindow() == active->GetHandle());
    CHECK_FALSE(widget->isActiveWindow());
    CHECK_FALSE(widget->testAttribute(Qt::WA_ShowWithoutActivating));

    // Calling it on an already visible window must not activate it either.
    frame->ShowWithoutActivating();
    wxYield();
    CHECK(QApplication::activeWindow() == active->GetHandle());
    CHECK_FALSE(widget->testAttribute(Qt::WA_ShowWithoutActivating));

    REQUIRE(frame->Hide());
    widget->setAttribute(Qt::WA_ShowWithoutActivating);
    frame->ShowWithoutActivating();
    wxYield();
    CHECK(frame->IsShown());
    CHECK(widget->testAttribute(Qt::WA_ShowWithoutActivating));
    CHECK(QApplication::activeWindow() == active->GetHandle());

    REQUIRE(frame->Hide());
    widget->setAttribute(Qt::WA_ShowWithoutActivating, false);
    EventCounter countActivate(frame, wxEVT_ACTIVATE);
    REQUIRE(frame->Show());
    CHECK(countActivate.WaitEvent());
    CHECK(frame->IsShown());
    CHECK(QApplication::activeWindow() == widget);
    CHECK_FALSE(widget->testAttribute(Qt::WA_ShowWithoutActivating));
}
#endif // __WXQT__
