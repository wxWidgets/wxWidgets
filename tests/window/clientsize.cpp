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
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/scopedptr.h"

#include "asserthelper.h"
#include "waitforpaint.h"

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
    wxScopedPtr<wxWindow> w(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY,
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
    wxScopedPtr<wxWindow> w0(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY));
#endif // wxGTK 2
    wxScopedPtr<wxWindow> w(new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY));

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
