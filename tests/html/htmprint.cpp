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

#include "wx/app.h"
#include "wx/window.h"

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

    // We do need to use DPI-proportional font sizes in order for the text used
    // in the page-break-inside:avoid test to take the same amount of pixels
    // for any DPI (12 being the font size used by wxHtmlDCRenderer by default).
    const wxFont fontFixedPixelSize(wxFontInfo(wxSize(10, 16)));
    pr.SetStandardFonts(fontFixedPixelSize.GetPointSize(), "Helvetica");

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

    // Also test that forbidding page breaks inside a paragraph works: it
    // should move it entirely to the next page, resulting in one extra page
    // compared to the version without "page-break-inside: avoid".
    static const char* const text =
"Early in the morning on the fourteenth of the spring month of Nisan the "
"Procurator of Judea, Pontius Pilate, in a white cloak lined with blood red, "
"emerged with his shuffling cavalryman's walk into the arcade connecting the two "
"wings of the palace of Herod the Great. "
"\n"
"More than anything else in the world the Procurator hated the smell of attar of "
"roses.  The omens for the day were bad, as this scent had been haunting him "
"since dawn. "
"\n"
"It seemed to the Procurator that the very cypresses and palms in the garden "
"were exuding the smell of roses, that this damned stench of roses was even "
"mingling with the smell of leather tackle and sweat from his mounted bodyguard. "
"\n"
"A haze of smoke was drifting toward the arcade across the upper courtyard of "
"the garden, coming from the wing at the rear of the palace, the quarters of the "
"first cohort of the XII Legion; known as the \"Lightning,\" it had been "
"stationed in Jerusalem since the Procurator's arrival.  The same oily perfume "
"of roses was mixed with the acrid smoke that showed that the centuries' cooks "
"had started to prepare breakfast "
"\n"
"\"Oh, gods, what are you punishing me for?..  No, there's no doubt, I have it "
"again, this terrible incurable pain...  hemicrania, when half the head aches... "
"There's no cure for it, nothing helps...  I must try not to move my head...\" "
"\n"
"A chair had already been placed on the mosaic floor by the fountain; without a "
"glance around, the Procurator sat in it and stretched out his hand to one side. "
"His secretary deferentially laid a piece of parchment in his hand.  Unable to "
"restrain a grimace of agony, the Procurator gave a fleeting sideways look at "
"its contents, returned the parchment to his secretary and said painfully, \"The "
"accused comes from Galilee, does he?  Was the case sent to the tetrarch?\" "
"\n"
"\"Yes, Procurator,\" replied the secretary.  \"He declined to confirm the "
"finding of the court and passed the Sanhedrin's sentence of death to you for "
"confirmation.\" "
"\n"
"The Procurator's cheek twitched, and he said quietly, \"Bring in the accused.\" "
        ;

    pr.SetHtmlText
       (
            wxString::Format
            (
                "<img width=\"100\" height=\"600\" src=\"dummy\"/>"
                "<div>%s</div>"
                "<br/>"
                "<img width=\"100\" height=\"600\" src=\"dummy\"/>",
                text
            )
       );
    CHECK( CountPages(pr) == 2 );

    pr.SetHtmlText
       (
            wxString::Format
            (
                "<img width=\"100\" height=\"600\" src=\"dummy\"/>"
                "<div style=\"page-break-inside:avoid\">%s</div>"
                "<br/>"
                "<img width=\"100\" height=\"600\" src=\"dummy\"/>",
                text
            )
       );
    const wxSize ext = dc.GetTextExtent("Something");
    WARN("Using base font size " << fontFixedPixelSize.GetPointSize()
         << ", text extent of \"Something\" is " << ext.x << "x" << ext.y);
    CHECK( CountPages(pr) == 3 );
}

#endif //wxUSE_HTML
