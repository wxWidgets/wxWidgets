///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/htmprint.cpp
// Purpose:     wxHtmlPrintout tests
// Author:      Vadim Zeitlin
// Created:     2018-05-22
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_HTML

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
#endif // WX_PRECOMP

#include "wx/html/htmprint.h"

namespace
{

// Return the number of pages in the printout.
int CountPages(wxHtmlPrintout& pr)
{
    REQUIRE_NOTHROW( pr.OnPreparePrinting() );

    int pageMin = -1,
        pageMax = -1,
        selFrom = -1,
        selTo   = -1;
    REQUIRE_NOTHROW( pr.GetPageInfo(&pageMin, &pageMax, &selFrom, &selTo) );

    // This should be always the case.
    CHECK( pageMin == 1 );

    // Return the really interesting value to the caller.
    return pageMax;
}

} // anonymous namespace

TEST_CASE("wxHtmlPrintout::Pagination", "[html][print]")
{
    wxHtmlPrintout pr;

    // Pagination works in terms of printer page size, which is obtained by
    // subtracting margins from the total size and margins are expressed in
    // millimeters, so their conversion to pixels depends on DPI. To ensure
    // that we get the same results for all values of DPI, get just get rid
    // of the margins entirely (it would also be possible to adjust them by
    // the DPI-dependent factor, but it doesn't seem to be worth doing it).
    pr.SetMargins(0, 0, 0, 0, 0);

    wxBitmap bmp(1000, 1000);
    wxMemoryDC dc(bmp);
    pr.SetUp(dc);

    // Empty or short HTML documents should be printed on a single page only.
    CHECK( CountPages(pr) == 1 );

    pr.SetHtmlText("<p>Hello world!</p>");
    CHECK( CountPages(pr) == 1 );

    // This one should be too big to fit on a single page.
    pr.SetHtmlText
       (
            "<img width=\"100\" height=\"600\" src=\"dummy\"/>"
            "<br/>"
            "<img width=\"100\" height=\"600\" src=\"dummy\"/>"
       );
    CHECK( CountPages(pr) == 2 );

    // Special case: normally images are not split, but if the image height is
    // greater than the page height, it should be.
    pr.SetHtmlText
       (
            "<img width=\"100\" height=\"2500\" src=\"dummy\"/>"
       );
    CHECK( CountPages(pr) == 3 );

    // Test explicit page breaks too.
    pr.SetHtmlText
       (
            "First page"
            "<div style=\"page-break-before:always\"/>"
            "Second page"
       );
    CHECK( CountPages(pr) == 2 );

    pr.SetHtmlText
       (
            "Something"
            "<div style=\"page-break-before:always\">"
                "<div style=\"page-break-before:always\">"
                    "<div style=\"page-break-before:always\">"
                        "whatever"
                    "</div>"
                "</div>"
            "</div>"
       );
    CHECK( CountPages(pr) == 2 );
}

#endif //wxUSE_HTML
