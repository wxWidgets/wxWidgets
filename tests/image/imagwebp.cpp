/////////////////////////////////////////////////////////////////////////////
// Name:        tests/image/imagwebp.cpp
// Purpose:     Test wxWEBPHandler
// Author:      Hermann Höhne
// Created:     2024-03-08
// Copyright:   (c) Hermann Höhne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_IMAGE && wxUSE_LIBWEBP

#include "wx/mstream.h"
#include "wx/sstream.h"

#include "testimage.h" // for RGBSimilarTo

TEST_CASE("wxWEBPHandler::LoadCorruptWebP", "[image][webp]")
{
    wxString data("RIFF____WEBP____ThisIsGarbageData");
    wxStringInputStream inputStream(data);
    wxImage loadingImage;
    wxWEBPHandler webpHandler;
    // LoadFile must return false, and will do so reliably.
    // The test exists to check for nullptr dereferences or similar defects.
    CHECK(!webpHandler.LoadFile(&loadingImage, inputStream, false));
}

static bool WebPRoundtrip(wxImage& savingImage, wxImage& loadingImage)
{
    // set quality to maximum so the images hopefully won't differ too much
    wxMemoryOutputStream outputStream;
    wxWEBPHandler webpHandler;
    webpHandler.SaveFile(&savingImage, outputStream, true);
    wxMemoryInputStream inputStream(outputStream);
    webpHandler.LoadFile(&loadingImage, inputStream, true);
    REQUIRE(loadingImage.IsOk());
    return true;
}

TEST_CASE("wxWEBPHandler::LossysRoundtrip", "[image][webp]")
{
    wxImage reference;
    reference.LoadFile("horse.webp");
    REQUIRE(reference.IsOk());
    reference.SetOption(wxIMAGE_OPTION_WEBP_FORMAT, wxWebPImageFormat::Lossy);
    reference.SetOption(wxIMAGE_OPTION_WEBP_QUALITY, 100);

    wxImage loaded;
    REQUIRE(WebPRoundtrip(reference, loaded));

    int tolerance = 32;
    CHECK_THAT(loaded, RGBSimilarTo(reference, tolerance));
}

TEST_CASE("wxWEBPHandler::LosslessRoundtrip", "[image][webp]")
{
    wxImage reference;
    reference.LoadFile("horse.webp");
    REQUIRE(reference.IsOk());
    reference.SetOption(wxIMAGE_OPTION_WEBP_FORMAT, wxWebPImageFormat::Lossless);

    wxImage loaded;
    REQUIRE(WebPRoundtrip(reference, loaded));

    CHECK_THAT(loaded, RGBASameAs(reference));
}

#endif //wxUSE_IMAGE && wxUSE_LIBWEBP
