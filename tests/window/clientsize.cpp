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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/scopedptr.h"

#include "asserthelper.h"

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
