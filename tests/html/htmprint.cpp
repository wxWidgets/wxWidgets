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

    // Use font size in pixels to make it DPI-independent: if we just used a
    // normal (say 12pt) font, it would have different height in pixels on 96
    // and 200 DPI systems, meaning that the text at the end of this test would
    // take different number of (fixed to 1000px height) pages.
    //
    // We could also make the page height proportional to the DPI, but this
    // would be more complicated as we also wouldn't be able to use hardcoded
    // height attribute values in the HTML snippets below then.
    const wxFont fontFixedPixelSize(wxFontInfo(wxSize(10, 16)));
    pr.SetStandardFonts(fontFixedPixelSize.GetPointSize(), "Helvetica");

    // We currently have to do this with wxGTK3 which uses 72 DPI for its
    // wxMemoryDC, resulting in 3/4 scaling (because screen DPI is hardcoded as
    // 96 in src/html/htmprint.cpp), when rendering onto it. This makes the
    // tests pass, but really shouldn't be necessary. Unfortunately it's not
    // clear where and how should this be fixed.
#ifdef __WXGTK3__
    pr.SetPPIPrinter(wxSize(96, 96));
#endif

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
"Early in the morning on the fourteenth of the spring month of Nisan the<br>"
"Procurator of Judea, Pontius Pilate, in a white cloak lined with blood red,<br>"
"emerged with his shuffling cavalryman's walk into the arcade connecting the two<br>"
"wings of the palace of Herod the Great.<br>"
"<br>"
"More than anything else in the world the Procurator hated the smell of attar of<br>"
"roses.  The omens for the day were bad, as this scent had been haunting him<br>"
"since dawn.<br>"
"<br>"
"It seemed to the Procurator that the very cypresses and palms in the garden<br>"
"were exuding the smell of roses, that this damned stench of roses was even<br>"
"mingling with the smell of leather tackle and sweat from his mounted bodyguard.<br>"
"<br>"
"A haze of smoke was drifting toward the arcade across the upper courtyard of<br>"
"the garden, coming from the wing at the rear of the palace, the quarters of the<br>"
"first cohort of the XII Legion; known as the \"Lightning,\" it had been<br>"
"stationed in Jerusalem since the Procurator's arrival.  The same oily perfume<br>"
"of roses was mixed with the acrid smoke that showed that the centuries' cooks<br>"
"had started to prepare breakfast<br>"
"<br>"
"\"Oh, gods, what are you punishing me for?..  No, there's no doubt, I have it<br>"
"again, this terrible incurable pain...  hemicrania, when half the head aches...<br>"
"There's no cure for it, nothing helps...  I must try not to move my head...\"<br>"
"<br>"
"A chair had already been placed on the mosaic floor by the fountain; without a<br>"
"glance around, the Procurator sat in it and stretched out his hand to one side.<br>"
"His secretary deferentially laid a piece of parchment in his hand.  Unable to<br>"
"restrain a grimace of agony, the Procurator gave a fleeting sideways look at<br>"
"its contents, returned the parchment to his secretary and said painfully, \"The<br>"
"accused comes from Galilee, does he?  Was the case sent to the tetrarch?\"<br>"
"<br>"
"\"Yes, Procurator,\" replied the secretary.  \"He declined to confirm the<br>"
"finding of the court and passed the Sanhedrin's sentence of death to you for<br>"
"confirmation.\"<br>"
"<br>"
"The Procurator's cheek twitched, and he said quietly, \"Bring in the accused.\"<br>"
        ;

    pr.SetHtmlText
       (
            wxString::Format
            (
                "<img width=\"100\" height=\"500\" src=\"dummy\"/>"
                "<div>%s</div>"
                "<br/>"
                "<img width=\"100\" height=\"500\" src=\"dummy\"/>",
                text
            )
       );
    CHECK( CountPages(pr) == 2 );

    pr.SetHtmlText
       (
            wxString::Format
            (
                "<img width=\"100\" height=\"500\" src=\"dummy\"/>"
                "<div style=\"page-break-inside:avoid\">%s</div>"
                "<br/>"
                "<img width=\"100\" height=\"500\" src=\"dummy\"/>",
                text
            )
       );
    INFO("Using base font size " << fontFixedPixelSize.GetPointSize());
    CHECK( CountPages(pr) == 3 );
}

#endif //wxUSE_HTML
