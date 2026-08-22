///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/clientsize.cpp
// Purpose:     Client vs. window size handling unit test
// Author:      Vaclav Slavik
// Created:     2008-02-12
// Copyright:   (c) 2008 Vaclav Slavik <vslavik@fastmail.fm>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/scrolwin.h"

#include <memory>

#include "asserthelper.h"
#include "waitfor.h"

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("wxWindow::ClientWindowSizeRoundTrip", "[window][client-size]")
{
    wxWindow* const w = wxTheApp->GetTopWindow();
    REQUIRE( w );

    const wxSize sizeWindow = w->GetSize();
    const wxSize sizeClient = w->GetClientSize();

    INFO("client size: " << sizeClient);
    CHECK( sizeWindow == w->ClientToWindowSize(sizeClient) );

    INFO("window size: " << sizeWindow);
    CHECK( sizeClient == w->WindowToClientSize(sizeWindow) );
}

TEST_CASE("wxWindow::MinClientSize", "[window][client-size]")
{
    std::unique_ptr<wxWindow> w(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY,
                                         wxDefaultPosition, wxDefaultSize,
                                         wxBORDER_THEME));
    w->SetSize(wxSize(1,1));
    const wxSize szw = w->GetClientSize();
    CHECK(szw.GetWidth() >= 0);
    CHECK(szw.GetHeight() >= 0);
}

TEST_CASE("wxWindow::SetClientSize", "[window][client-size]")
{
#if defined(__WXGTK__) && !defined(__WXGTK3__)
    // Under wxGTK2 we need to have two children (at least) because if there
    // is exactly one child its size is set to fill the whole parent frame
    // and the window cannot be resized - see wxTopLevelWindowBase::Layout().
    std::unique_ptr<wxWindow> w0(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY));
#endif // wxGTK 2
    std::unique_ptr<wxWindow> w(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY));

    wxRect reqSize = wxTheApp->GetTopWindow()->GetClientRect();
    reqSize.Deflate(25);
    w->SetClientSize(reqSize.GetSize());

    // Wait for the first paint event to be sure
    // that window really has its final size.
    WaitForPaint waitForPaint(w.get());
    w->Show();
    waitForPaint.YieldUntilPainted();

    // Check if client size has been set as required.
    wxRect r = w->GetClientRect();
    CHECK(r.width == reqSize.width);
    CHECK(r.height == reqSize.height);
}

// Check that the client size of a scrolled window accounts for the scrollbars
// shown in it.
//
// All important checks below fails for wxQt currently, so disable the test for
// it for now.
#ifndef __WXQT__
TEST_CASE("wxScrolled::ClientSize", "[window][client-size][scroll]")
{
    // Check that the borders, if any, are accounted for correctly too.
    const long border = GENERATE(wxBORDER_NONE, wxBORDER_SIMPLE);
    INFO("border style " << (border == wxBORDER_NONE ? "none" : "simple"));

    // This window is not used for anything, but it must exist to prevent the
    // test frame from resizing the panel created below to fill it entirely,
    // which is what would happen if the panel were its unique child.
    std::unique_ptr<wxWindow> const
        sibling(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY));

    // The scrolled window must be really laid out by its parent for the
    // scrollbars to appear, so use a sizer in a panel of our own instead of
    // just creating it as a child of the test frame.
    std::unique_ptr<wxWindow> const
        parent(new wxPanel(wxTheApp->GetTopWindow()));

    // Make the panel big enough for the scrollbars to fit inside it.
    parent->SetSize(wxSize(200, 200));

    wxScrolledWindow* const
        win = new wxScrolledWindow(parent.get(), wxID_ANY,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxHSCROLL | wxVSCROLL | border);

    // Using the scroll rate of 1 pixel makes it simpler to reason about the
    // scroll positions below.
    win->SetScrollRate(1, 1);

    wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(win, wxSizerFlags(1).Expand());
    parent->SetSizer(sizer);
    parent->Layout();

    YieldForAWhile();

    // Don't rely on the window having any particular size, we just need it to
    // be big enough for the scrollbars to fit inside it.
    const wxSize size = win->GetSize();
    INFO("scrolled window size: " << size);
    REQUIRE( size.x > 100 );
    REQUIRE( size.y > 100 );

    // Without any scrollbars the client size is just the total size without
    // the borders.
    const wxSize sizeClientNoScrollbars = win->GetClientSize();

    // GetWindowBorderSize() seems to not return the correct result in wxOSX.
#ifndef __WXOSX__
    CHECK( sizeClientNoScrollbars == size - win->GetWindowBorderSize() );
#endif

    // Now make the virtual size big enough for both scrollbars to appear.
    const wxSize sizeVirtual = 2*size;
    win->SetVirtualSize(sizeVirtual);

    YieldForAWhile();

    REQUIRE( win->GetSize() == size );
    CHECK( win->IsScrollbarShown(wxHORIZONTAL) );
    CHECK( win->IsScrollbarShown(wxVERTICAL) );

    const wxSize sizeClient = win->GetClientSize();
    INFO("client size with scrollbars: " << sizeClient);

    // The scrollbars either take place inside the window, reducing its client
    // area by exactly their size, or are drawn on top of the window contents,
    // in which case the client size doesn't change at all: this is the case of
    // the overlay scrollbars used by default under macOS and also under GTK.
    const wxSize sizeLost = sizeClientNoScrollbars - sizeClient;
    INFO("client size lost to the scrollbars: " << sizeLost);

    if ( sizeLost != wxSize(0, 0) )
    {
        const wxSize sizeScrollbar
                     (
                        wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, win),
                        wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y, win)
                     );

        // Allow for a small tolerance because the scrollbars may be separated
        // from the window contents by a gap under some platforms.
        CHECK( sizeLost.x == Approx(sizeScrollbar.x).margin(4) );
        CHECK( sizeLost.y == Approx(sizeScrollbar.y).margin(4) );
    }

    // In any case, the client size must correspond to the part of the virtual
    // area which is really visible, i.e. scrolling to the end must leave
    // exactly the client size worth of pixels visible.
    win->Scroll(sizeVirtual.x, sizeVirtual.y);

    const wxSize sizeScrolled = sizeVirtual - sizeClient;
    CHECK( win->GetViewStart() == wxPoint(sizeScrolled.x, sizeScrolled.y) );
}
#endif // !__WXQT__
