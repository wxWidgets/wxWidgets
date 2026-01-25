///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/statbmptest.cpp
// Purpose:     wxStaticBitmap unit test
// Author:      Vadim Zeitlin
// Created:     2026-01-22
// Copyright:   (c) 2026 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_STATBMP

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/statbmp.h"

#include "wx/dcmemory.h"

#ifdef __WXMSW__
    #include "wx/msw/private/resource_usage.h"
#endif // __WXMSW__

static void TestStaticBitmap(wxWindow* window, double scale)
{
    // Create the bitmaps using the window scale factor to make sure they
    // are used directly by wxStaticBitmap instead, without any rescaling
    // (which would create temporary bitmaps and hide the manifestation of
    // the bug that this test was written to trigger, see #26106).
    wxBitmap bmp1;
    bmp1.CreateWithDIPSize(16, 16, scale);
    wxBitmap bmp2;
    bmp2.CreateWithDIPSize(32, 32, scale);

    wxIcon icon;
    icon.CopyFromBitmap(bmp1);

    wxStaticBitmap statbmp(window, wxID_ANY, bmp1);
    REQUIRE( statbmp.GetBitmap().IsOk() );

    statbmp.SetBitmap(bmp2);
    REQUIRE( statbmp.GetBitmap().IsOk() );

    // Check that the bitmap is valid by selecting it into wxMemoryDC:
    // unlike IsOk() check, this would fail if it had been destroyed.
    REQUIRE_NOTHROW( wxMemoryDC(bmp1) );

    statbmp.SetIcon(icon);
    REQUIRE_NOTHROW( wxMemoryDC(bmp2) );

    statbmp.SetBitmap(bmp2);
    REQUIRE_NOTHROW( wxMemoryDC(bmp1) );
}

TEST_CASE("wxStaticBitmap::Set", "[wxStaticBitmap][bitmap]")
{
    auto* const window = wxTheApp->GetTopWindow();

    TestStaticBitmap(window, window->GetDPIScaleFactor());
}

#ifdef __WXMSW__

TEST_CASE("wxStaticBitmap::ResourceLeak", "[wxStaticBitmap]")
{
    auto* const window = wxTheApp->GetTopWindow();
    auto const scale = window->GetDPIScaleFactor();

    wxGUIObjectUsage usageBefore;
    for ( int n = 0; n < 100; ++n )
    {
        TestStaticBitmap(window, scale);

        // First use of GDI allocates some resources, so consider the state
        // after the first iteration as the baseline: if there are any leaks,
        // we'll see them in the subsequent iterations too and the test below
        // will fail.
        if ( n == 0 )
            usageBefore = wxGetCurrentlyUsedResources();
    }

    auto const usageAfter = wxGetCurrentlyUsedResources();
    CHECK( usageAfter.numGDI == usageBefore.numGDI );
}

#endif // __WXMSW__

#endif // wxUSE_STATBMP
