///////////////////////////////////////////////////////////////////////////////
// Name:        tests/image/image.cpp
// Purpose:     Test wxImage
// Author:      Francesco Montorsi
// Created:     2009-05-31
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Francesco Montorsi
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_IMAGE

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/anidecod.h" // wxImageArray
#include "wx/palette.h"
#include "wx/url.h"
#include "wx/log.h"
#include "wx/mstream.h"
#include "wx/zstream.h"
#include "wx/wfstream.h"

#include "testimage.h"

struct testData {
    const char* file;
    wxBitmapType type;
    unsigned bitDepth;
} g_testfiles[] =
{
    { "horse.ico", wxBITMAP_TYPE_ICO, 4 },
    { "horse.xpm", wxBITMAP_TYPE_XPM, 8 },
    { "horse.png", wxBITMAP_TYPE_PNG, 24 },
    { "horse.ani", wxBITMAP_TYPE_ANI, 24 },
    { "horse.bmp", wxBITMAP_TYPE_BMP, 8 },
    { "horse.cur", wxBITMAP_TYPE_CUR, 1 },
    { "horse.gif", wxBITMAP_TYPE_GIF, 8 },
    { "horse.jpg", wxBITMAP_TYPE_JPEG, 24 },
    { "horse.pcx", wxBITMAP_TYPE_PCX, 8 },
    { "horse.pnm", wxBITMAP_TYPE_PNM, 24 },
    { "horse.tga", wxBITMAP_TYPE_TGA, 8 },
    { "horse.tif", wxBITMAP_TYPE_TIF, 8 }
};


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ImageTestCase : public CppUnit::TestCase
{
public:
    ImageTestCase();
    ~ImageTestCase();

private:
    CPPUNIT_TEST_SUITE( ImageTestCase );
        CPPUNIT_TEST( LoadFromSocketStream );
        CPPUNIT_TEST( LoadFromZipStream );
        CPPUNIT_TEST( LoadFromFile );
        CPPUNIT_TEST( SizeImage );
        CPPUNIT_TEST( CompareLoadedImage );
        CPPUNIT_TEST( CompareSavedImage );
        CPPUNIT_TEST( SavePNG );
        CPPUNIT_TEST( SaveTIFF );
        CPPUNIT_TEST( SaveAnimatedGIF );
        CPPUNIT_TEST( ReadCorruptedTGA );
        CPPUNIT_TEST( GIFComment );
        CPPUNIT_TEST( DibPadding );
        CPPUNIT_TEST( BMPFlippingAndRLECompression );
    CPPUNIT_TEST_SUITE_END();

    void LoadFromSocketStream();
    void LoadFromZipStream();
    void LoadFromFile();
    void SizeImage();
    void CompareLoadedImage();
    void CompareSavedImage();
    void SavePNG();
    void SaveTIFF();
    void SaveAnimatedGIF();
    void ReadCorruptedTGA();
    void GIFComment();
    void DibPadding();
    void BMPFlippingAndRLECompression();

    DECLARE_NO_COPY_CLASS(ImageTestCase)
};

CPPUNIT_TEST_SUITE_REGISTRATION( ImageTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ImageTestCase, "ImageTestCase" );

ImageTestCase::ImageTestCase()
{
    wxSocketBase::Initialize();

    // the formats we're going to test:
    wxImage::AddHandler(new wxICOHandler);
    wxImage::AddHandler(new wxXPMHandler);
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxANIHandler);
    wxImage::AddHandler(new wxBMPHandler);
    wxImage::AddHandler(new wxCURHandler);
    wxImage::AddHandler(new wxGIFHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxPCXHandler);
    wxImage::AddHandler(new wxPNMHandler);
    wxImage::AddHandler(new wxTGAHandler);
    wxImage::AddHandler(new wxTIFFHandler);
}

ImageTestCase::~ImageTestCase()
{
    wxSocketBase::Shutdown();
}

void ImageTestCase::LoadFromFile()
{
    wxImage img;
    for (unsigned int i=0; i<WXSIZEOF(g_testfiles); i++)
        CPPUNIT_ASSERT(img.LoadFile(g_testfiles[i].file));
}

void ImageTestCase::LoadFromSocketStream()
{
    if (!IsNetworkAvailable())      // implemented in test.cpp
    {
        wxLogWarning("No network connectivity; skipping the "
                     "ImageTestCase::LoadFromSocketStream test unit.");
        return;
    }

    struct {
        const char* url;
        wxBitmapType type;
    } testData[] =
    {
        { "http://www.wxwidgets.org/logo9.jpg", wxBITMAP_TYPE_JPEG },
        { "http://www.wxwidgets.org/favicon.ico", wxBITMAP_TYPE_ICO }
    };

    for (unsigned int i=0; i<WXSIZEOF(testData); i++)
    {
        wxURL url(testData[i].url);
        WX_ASSERT_EQUAL_MESSAGE
        (
            ("Constructing URL \"%s\" failed.", testData[i].url),
            wxURL_NOERR,
            url.GetError()
        );

        wxInputStream *in_stream = url.GetInputStream();
        WX_ASSERT_MESSAGE
        (
            ("Opening URL \"%s\" failed.", testData[i].url),
            in_stream && in_stream->IsOk()
        );

        wxImage img;

        // NOTE: it's important to inform wxImage about the type of the image being
        //       loaded otherwise it will try to autodetect the format, but that
        //       requires a seekable stream!
        WX_ASSERT_MESSAGE
        (
            ("Loading image from \"%s\" failed.", testData[i].url),
            img.LoadFile(*in_stream, testData[i].type)
        );

        delete in_stream;
    }
}

void ImageTestCase::LoadFromZipStream()
{
    for (unsigned int i=0; i<WXSIZEOF(g_testfiles); i++)
    {
        switch (g_testfiles[i].type)
        {
            case wxBITMAP_TYPE_XPM:
            case wxBITMAP_TYPE_GIF:
            case wxBITMAP_TYPE_PCX:
            case wxBITMAP_TYPE_TGA:
            case wxBITMAP_TYPE_TIF:
            continue;       // skip testing those wxImageHandlers which cannot
                            // load data from non-seekable streams

            default:
                ; // proceed
        }

        // compress the test file on the fly:
        wxMemoryOutputStream memOut;
        {
            wxFileInputStream file(g_testfiles[i].file);
            CPPUNIT_ASSERT(file.IsOk());

            wxZlibOutputStream compressFilter(memOut, 5, wxZLIB_GZIP);
            CPPUNIT_ASSERT(compressFilter.IsOk());

            file.Read(compressFilter);
            CPPUNIT_ASSERT(file.GetLastError() == wxSTREAM_EOF);
        }

        // now fetch the compressed memory to wxImage, decompressing it on the fly; this
        // allows us to test loading images from non-seekable streams other than socket streams
        wxMemoryInputStream memIn(memOut);
        CPPUNIT_ASSERT(memIn.IsOk());
        wxZlibInputStream decompressFilter(memIn, wxZLIB_GZIP);
        CPPUNIT_ASSERT(decompressFilter.IsOk());

        wxImage img;

        // NOTE: it's important to inform wxImage about the type of the image being
        //       loaded otherwise it will try to autodetect the format, but that
        //       requires a seekable stream!
        WX_ASSERT_MESSAGE(("Could not load file type '%d' after it was zipped", g_testfiles[i].type),
                          img.LoadFile(decompressFilter, g_testfiles[i].type));
    }
}

void ImageTestCase::SizeImage()
{
   // Test the wxImage::Size() function which takes a rectangle from source and
   // places it in a new image at a given position. This test checks, if the
   // correct areas are chosen, and clipping is done correctly.

   // our test image:
   static const char * xpm_orig[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "     .....",
      " ++++@@@@.",
      " +...   @.",
      " +.@@++ @.",
      " +.@ .+ @.",
      ".@ +. @.+ ",
      ".@ ++@@.+ ",
      ".@   ...+ ",
      ".@@@@++++ ",
      ".....     "
   };
   // the expected results for all tests:
   static const char * xpm_l_t[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "...   @.BB",
      ".@@++ @.BB",
      ".@ .+ @.BB",
      " +. @.+ BB",
      " ++@@.+ BB",
      "   ...+ BB",
      "@@@++++ BB",
      "...     BB",
      "BBBBBBBBBB",
      "BBBBBBBBBB"
   };
   static const char * xpm_t[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      " +...   @.",
      " +.@@++ @.",
      " +.@ .+ @.",
      ".@ +. @.+ ",
      ".@ ++@@.+ ",
      ".@   ...+ ",
      ".@@@@++++ ",
      ".....     ",
      "BBBBBBBBBB",
      "BBBBBBBBBB"
   };
   static const char * xpm_r_t[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BB +...   ",
      "BB +.@@++ ",
      "BB +.@ .+ ",
      "BB.@ +. @.",
      "BB.@ ++@@.",
      "BB.@   ...",
      "BB.@@@@+++",
      "BB.....   ",
      "BBBBBBBBBB",
      "BBBBBBBBBB"
   };
   static const char * xpm_l[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "   .....BB",
      "+++@@@@.BB",
      "...   @.BB",
      ".@@++ @.BB",
      ".@ .+ @.BB",
      " +. @.+ BB",
      " ++@@.+ BB",
      "   ...+ BB",
      "@@@++++ BB",
      "...     BB"
   };
   static const char * xpm_r[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BB     ...",
      "BB ++++@@@",
      "BB +...   ",
      "BB +.@@++ ",
      "BB +.@ .+ ",
      "BB.@ +. @.",
      "BB.@ ++@@.",
      "BB.@   ...",
      "BB.@@@@+++",
      "BB.....   "
   };
   static const char * xpm_l_b[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBB",
      "BBBBBBBBBB",
      "   .....BB",
      "+++@@@@.BB",
      "...   @.BB",
      ".@@++ @.BB",
      ".@ .+ @.BB",
      " +. @.+ BB",
      " ++@@.+ BB",
      "   ...+ BB"
   };
   static const char * xpm_b[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBB",
      "BBBBBBBBBB",
      "     .....",
      " ++++@@@@.",
      " +...   @.",
      " +.@@++ @.",
      " +.@ .+ @.",
      ".@ +. @.+ ",
      ".@ ++@@.+ ",
      ".@   ...+ "
   };
   static const char * xpm_r_b[] = {
      "10 10 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBB",
      "BBBBBBBBBB",
      "BB     ...",
      "BB ++++@@@",
      "BB +...   ",
      "BB +.@@++ ",
      "BB +.@ .+ ",
      "BB.@ +. @.",
      "BB.@ ++@@.",
      "BB.@   ..."
   };
   static const char * xpm_sm[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "     .....",
      " ++++@@@",
      " +...   ",
      " +.@@++ ",
      " +.@ .+ ",
      ".@ +. @.",
      ".@ ++@@.",
      ".@   ..."
   };
   static const char * xpm_gt[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "     .....BB",
      " ++++@@@@.BB",
      " +...   @.BB",
      " +.@@++ @.BB",
      " +.@ .+ @.BB",
      ".@ +. @.+ BB",
      ".@ ++@@.+ BB",
      ".@   ...+ BB",
      ".@@@@++++ BB",
      ".....     BB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB"
   };
   static const char * xpm_gt_l_t[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "...   @.BBBB",
      ".@@++ @.BBBB",
      ".@ .+ @.BBBB",
      " +. @.+ BBBB",
      " ++@@.+ BBBB",
      "   ...+ BBBB",
      "@@@++++ BBBB",
      "...     BBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB"
   };
   static const char * xpm_gt_l[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "   .....BBBB",
      "+++@@@@.BBBB",
      "...   @.BBBB",
      ".@@++ @.BBBB",
      ".@ .+ @.BBBB",
      " +. @.+ BBBB",
      " ++@@.+ BBBB",
      "   ...+ BBBB",
      "@@@++++ BBBB",
      "...     BBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB"
   };
   static const char * xpm_gt_l_b[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "   .....BBBB",
      "+++@@@@.BBBB",
      "...   @.BBBB",
      ".@@++ @.BBBB",
      ".@ .+ @.BBBB",
      " +. @.+ BBBB",
      " ++@@.+ BBBB",
      "   ...+ BBBB",
      "@@@++++ BBBB",
      "...     BBBB"
   };
   static const char * xpm_gt_l_bb[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "   .....BBBB",
      "+++@@@@.BBBB",
      "...   @.BBBB",
      ".@@++ @.BBBB",
      ".@ .+ @.BBBB",
      " +. @.+ BBBB",
      " ++@@.+ BBBB",
      "   ...+ BBBB"
   };
   static const char * xpm_gt_t[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      " +...   @.BB",
      " +.@@++ @.BB",
      " +.@ .+ @.BB",
      ".@ +. @.+ BB",
      ".@ ++@@.+ BB",
      ".@   ...+ BB",
      ".@@@@++++ BB",
      ".....     BB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB"
   };
   static const char * xpm_gt_b[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "     .....BB",
      " ++++@@@@.BB",
      " +...   @.BB",
      " +.@@++ @.BB",
      " +.@ .+ @.BB",
      ".@ +. @.+ BB",
      ".@ ++@@.+ BB",
      ".@   ...+ BB",
      ".@@@@++++ BB",
      ".....     BB"
   };
   static const char * xpm_gt_bb[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "     .....BB",
      " ++++@@@@.BB",
      " +...   @.BB",
      " +.@@++ @.BB",
      " +.@ .+ @.BB",
      ".@ +. @.+ BB",
      ".@ ++@@.+ BB",
      ".@   ...+ BB"
   };
   static const char * xpm_gt_r_t[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BB +...   @.",
      "BB +.@@++ @.",
      "BB +.@ .+ @.",
      "BB.@ +. @.+ ",
      "BB.@ ++@@.+ ",
      "BB.@   ...+ ",
      "BB.@@@@++++ ",
      "BB.....     ",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB"
   };
   static const char * xpm_gt_r[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BB     .....",
      "BB ++++@@@@.",
      "BB +...   @.",
      "BB +.@@++ @.",
      "BB +.@ .+ @.",
      "BB.@ +. @.+ ",
      "BB.@ ++@@.+ ",
      "BB.@   ...+ ",
      "BB.@@@@++++ ",
      "BB.....     ",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB"
   };
   static const char * xpm_gt_r_b[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BB     .....",
      "BB ++++@@@@.",
      "BB +...   @.",
      "BB +.@@++ @.",
      "BB +.@ .+ @.",
      "BB.@ +. @.+ ",
      "BB.@ ++@@.+ ",
      "BB.@   ...+ ",
      "BB.@@@@++++ ",
      "BB.....     "
   };
   static const char * xpm_gt_r_bb[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BB     .....",
      "BB ++++@@@@.",
      "BB +...   @.",
      "BB +.@@++ @.",
      "BB +.@ .+ @.",
      "BB.@ +. @.+ ",
      "BB.@ ++@@.+ ",
      "BB.@   ...+ "
   };
   static const char * xpm_gt_rr_t[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBB +...   ",
      "BBBB +.@@++ ",
      "BBBB +.@ .+ ",
      "BBBB.@ +. @.",
      "BBBB.@ ++@@.",
      "BBBB.@   ...",
      "BBBB.@@@@+++",
      "BBBB.....   ",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB"
   };
   static const char * xpm_gt_rr[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBB     ...",
      "BBBB ++++@@@",
      "BBBB +...   ",
      "BBBB +.@@++ ",
      "BBBB +.@ .+ ",
      "BBBB.@ +. @.",
      "BBBB.@ ++@@.",
      "BBBB.@   ...",
      "BBBB.@@@@+++",
      "BBBB.....   ",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB"
   };
   static const char * xpm_gt_rr_b[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBB     ...",
      "BBBB ++++@@@",
      "BBBB +...   ",
      "BBBB +.@@++ ",
      "BBBB +.@ .+ ",
      "BBBB.@ +. @.",
      "BBBB.@ ++@@.",
      "BBBB.@   ...",
      "BBBB.@@@@+++",
      "BBBB.....   "
   };
   static const char * xpm_gt_rr_bb[] = {
      "12 12 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBBBBBBBBBB",
      "BBBB     ...",
      "BBBB ++++@@@",
      "BBBB +...   ",
      "BBBB +.@@++ ",
      "BBBB +.@ .+ ",
      "BBBB.@ +. @.",
      "BBBB.@ ++@@.",
      "BBBB.@   ..."
   };
   static const char * xpm_sm_ll_tt[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      " .+ @.BB",
      ". @.+ BB",
      "+@@.+ BB",
      " ...+ BB",
      "@++++ BB",
      ".     BB",
      "BBBBBBBB",
      "BBBBBBBB"
   };
   static const char * xpm_sm_ll_t[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      ".   @.BB",
      "@++ @.BB",
      " .+ @.BB",
      ". @.+ BB",
      "+@@.+ BB",
      " ...+ BB",
      "@++++ BB",
      ".     BB"
   };
   static const char * xpm_sm_ll[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      " .....BB",
      "+@@@@.BB",
      ".   @.BB",
      "@++ @.BB",
      " .+ @.BB",
      ". @.+ BB",
      "+@@.+ BB",
      " ...+ BB"
   };
   static const char * xpm_sm_ll_b[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBB",
      "BBBBBBBB",
      " .....BB",
      "+@@@@.BB",
      ".   @.BB",
      "@++ @.BB",
      " .+ @.BB",
      ". @.+ BB"
   };
   static const char * xpm_sm_l_tt[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      ".@ .+ @.",
      " +. @.+ ",
      " ++@@.+ ",
      "   ...+ ",
      "@@@++++ ",
      "...     ",
      "BBBBBBBB",
      "BBBBBBBB"
   };
   static const char * xpm_sm_l_t[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "...   @.",
      ".@@++ @.",
      ".@ .+ @.",
      " +. @.+ ",
      " ++@@.+ ",
      "   ...+ ",
      "@@@++++ ",
      "...     "
   };
   static const char * xpm_sm_l[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "   .....",
      "+++@@@@.",
      "...   @.",
      ".@@++ @.",
      ".@ .+ @.",
      " +. @.+ ",
      " ++@@.+ ",
      "   ...+ "
   };
   static const char * xpm_sm_l_b[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBB",
      "BBBBBBBB",
      "   .....",
      "+++@@@@.",
      "...   @.",
      ".@@++ @.",
      ".@ .+ @.",
      " +. @.+ "
   };
   static const char * xpm_sm_tt[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      " +.@ .+ ",
      ".@ +. @.",
      ".@ ++@@.",
      ".@   ...",
      ".@@@@+++",
      ".....   ",
      "BBBBBBBB",
      "BBBBBBBB"
   };
   static const char * xpm_sm_t[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      " +...   ",
      " +.@@++ ",
      " +.@ .+ ",
      ".@ +. @.",
      ".@ ++@@.",
      ".@   ...",
      ".@@@@+++",
      ".....   "
   };
   static const char * xpm_sm_b[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBB",
      "BBBBBBBB",
      "     ...",
      " ++++@@@",
      " +...   ",
      " +.@@++ ",
      " +.@ .+ ",
      ".@ +. @."
   };
   static const char * xpm_sm_r_tt[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BB +.@ .",
      "BB.@ +. ",
      "BB.@ ++@",
      "BB.@   .",
      "BB.@@@@+",
      "BB..... ",
      "BBBBBBBB",
      "BBBBBBBB"
   };
   static const char * xpm_sm_r_t[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BB +... ",
      "BB +.@@+",
      "BB +.@ .",
      "BB.@ +. ",
      "BB.@ ++@",
      "BB.@   .",
      "BB.@@@@+",
      "BB..... "
   };
   static const char * xpm_sm_r[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BB     .",
      "BB ++++@",
      "BB +... ",
      "BB +.@@+",
      "BB +.@ .",
      "BB.@ +. ",
      "BB.@ ++@",
      "BB.@   ."
   };
   static const char * xpm_sm_r_b[] = {
      "8 8 5 1", "B c Black", "  c #00ff00", ". c #0000ff", "+ c #7f7f7f", "@ c #FF0000",
      "BBBBBBBB",
      "BBBBBBBB",
      "BB     .",
      "BB ++++@",
      "BB +... ",
      "BB +.@@+",
      "BB +.@ .",
      "BB.@ +. "
   };

   // this table defines all tests
   struct SizeTestData
   {
      int w, h, dx, dy;                // first parameters for Size()
      const char **ref_xpm;            // expected result
   } sizeTestData[] =
   {
      { 10, 10,  0,  0, xpm_orig},      // same size, same position
      { 12, 12,  0,  0, xpm_gt},       // target larger, same position
      {  8,  8,  0,  0, xpm_sm},       // target smaller, same position
      { 10, 10, -2, -2, xpm_l_t},      // same size, move left up
      { 10, 10, -2,  0, xpm_l},        // same size, move left
      { 10, 10, -2,  2, xpm_l_b},      // same size, move left down
      { 10, 10,  0, -2, xpm_t},        // same size, move up
      { 10, 10,  0,  2, xpm_b},        // same size, move down
      { 10, 10,  2, -2, xpm_r_t},      // same size, move right up
      { 10, 10,  2,  0, xpm_r},        // same size, move right
      { 10, 10,  2,  2, xpm_r_b},      // same size, move right down
      { 12, 12, -2, -2, xpm_gt_l_t},   // target larger, move left up
      { 12, 12, -2,  0, xpm_gt_l},     // target larger, move left
      { 12, 12, -2,  2, xpm_gt_l_b},   // target larger, move left down
      { 12, 12, -2,  4, xpm_gt_l_bb},  // target larger, move left down
      { 12, 12,  0, -2, xpm_gt_t},     // target larger, move up
      { 12, 12,  0,  2, xpm_gt_b},     // target larger, move down
      { 12, 12,  0,  4, xpm_gt_bb},    // target larger, move down
      { 12, 12,  2, -2, xpm_gt_r_t},   // target larger, move right up
      { 12, 12,  2,  0, xpm_gt_r},     // target larger, move right
      { 12, 12,  2,  2, xpm_gt_r_b},   // target larger, move right down
      { 12, 12,  2,  4, xpm_gt_r_bb},  // target larger, move right down
      { 12, 12,  4, -2, xpm_gt_rr_t},  // target larger, move right up
      { 12, 12,  4,  0, xpm_gt_rr},    // target larger, move right
      { 12, 12,  4,  2, xpm_gt_rr_b},  // target larger, move right down
      { 12, 12,  4,  4, xpm_gt_rr_bb}, // target larger, move right down
      {  8,  8, -4, -4, xpm_sm_ll_tt}, // target smaller, move left up
      {  8,  8, -4, -2, xpm_sm_ll_t},  // target smaller, move left up
      {  8,  8, -4,  0, xpm_sm_ll},    // target smaller, move left
      {  8,  8, -4,  2, xpm_sm_ll_b},  // target smaller, move left down
      {  8,  8, -2, -4, xpm_sm_l_tt},  // target smaller, move left up
      {  8,  8, -2, -2, xpm_sm_l_t},   // target smaller, move left up
      {  8,  8, -2,  0, xpm_sm_l},     // target smaller, move left
      {  8,  8, -2,  2, xpm_sm_l_b},   // target smaller, move left down
      {  8,  8,  0, -4, xpm_sm_tt},    // target smaller, move up
      {  8,  8,  0, -2, xpm_sm_t},     // target smaller, move up
      {  8,  8,  0,  2, xpm_sm_b},     // target smaller, move down
      {  8,  8,  2, -4, xpm_sm_r_tt},  // target smaller, move right up
      {  8,  8,  2, -2, xpm_sm_r_t},   // target smaller, move right up
      {  8,  8,  2,  0, xpm_sm_r},     // target smaller, move right
      {  8,  8,  2,  2, xpm_sm_r_b},   // target smaller, move right down
   };

   const wxImage src_img(xpm_orig);
   for ( unsigned i = 0; i < WXSIZEOF(sizeTestData); i++ )
   {
       SizeTestData& st = sizeTestData[i];
       wxImage
           actual(src_img.Size(wxSize(st.w, st.h), wxPoint(st.dx, st.dy), 0, 0, 0)),
           expected(st.ref_xpm);

       // to check results with an image viewer uncomment this:
       //actual.SaveFile(wxString::Format("imagetest-%02d-actual.png", i), wxBITMAP_TYPE_PNG);
       //expected.SaveFile(wxString::Format("imagetest-%02d-exp.png", i), wxBITMAP_TYPE_PNG);

       CPPUNIT_ASSERT_EQUAL( actual.GetSize().x, expected.GetSize().x );
       CPPUNIT_ASSERT_EQUAL( actual.GetSize().y, expected.GetSize().y );

       WX_ASSERT_EQUAL_MESSAGE
       (
         ("Resize test #%u: (%d, %d), (%d, %d)", i, st.w, st.h, st.dx, st.dy),
         expected, actual
       );
   }
}

void ImageTestCase::CompareLoadedImage()
{
    wxImage expected8("horse.xpm");
    CPPUNIT_ASSERT( expected8.IsOk() );

    wxImage expected24("horse.png");
    CPPUNIT_ASSERT( expected24.IsOk() );

    for (size_t i=0; i<WXSIZEOF(g_testfiles); i++)
    {
        if ( !(g_testfiles[i].bitDepth == 8 || g_testfiles[i].bitDepth == 24)
            || g_testfiles[i].type == wxBITMAP_TYPE_JPEG /*skip lossy JPEG*/)
        {
            continue;
        }

        wxImage actual(g_testfiles[i].file);

        if ( actual.GetSize() != expected8.GetSize() )
        {
            continue;
        }


        WX_ASSERT_EQUAL_MESSAGE
        (
            ("Compare test '%s' for loading failed", g_testfiles[i].file),
            g_testfiles[i].bitDepth == 8 ? expected8 : expected24,
            actual
        );
    }

}

enum
{
    wxIMAGE_HAVE_ALPHA = (1 << 0),
    wxIMAGE_HAVE_PALETTE = (1 << 1)
};

static
void CompareImage(const wxImageHandler& handler, const wxImage& image,
    int properties = 0, const wxImage *compareTo = NULL)
{
    wxBitmapType type = handler.GetType();

    const bool testPalette = (properties & wxIMAGE_HAVE_PALETTE) != 0;
    /*
    This is getting messy and should probably be transformed into a table
    with image format features before it gets hairier.
    */
    if ( testPalette
        && ( !(type == wxBITMAP_TYPE_BMP
                || type == wxBITMAP_TYPE_GIF
                || type == wxBITMAP_TYPE_ICO
                || type == wxBITMAP_TYPE_PNG)
            || type == wxBITMAP_TYPE_XPM) )
    {
        return;
    }

    const bool testAlpha = (properties & wxIMAGE_HAVE_ALPHA) != 0;
    if (testAlpha
        && !(type == wxBITMAP_TYPE_PNG || type == wxBITMAP_TYPE_TGA) )
    {
        // don't test images with alpha if this handler doesn't support alpha
        return;
    }

    if (type == wxBITMAP_TYPE_JPEG /* skip lossy JPEG */)
    {
        return;
    }

    wxMemoryOutputStream memOut;
    if ( !image.SaveFile(memOut, type) )
    {
        // Unfortunately we can't know if the handler just doesn't support
        // saving images, or if it failed to save.
        return;
    }

    wxMemoryInputStream memIn(memOut);
    CPPUNIT_ASSERT(memIn.IsOk());

    wxImage actual(memIn);
    CPPUNIT_ASSERT(actual.IsOk());

    const wxImage *expected = compareTo ? compareTo : &image;
    CPPUNIT_ASSERT( actual.GetSize() == expected->GetSize() );

    unsigned bitsPerPixel = testPalette ? 8 : (testAlpha ? 32 : 24);
    WX_ASSERT_EQUAL_MESSAGE
    (
        ("Compare test '%s (%d-bit)' for saving failed",
            handler.GetExtension(), bitsPerPixel),
        *expected,
        actual
    );

#if wxUSE_PALETTE
    CPPUNIT_ASSERT(actual.HasPalette()
        == (testPalette || type == wxBITMAP_TYPE_XPM));
#endif

    CPPUNIT_ASSERT( actual.HasAlpha() == testAlpha);

    if (!testAlpha)
    {
        return;
    }

    WX_ASSERT_EQUAL_MESSAGE
    (
        ("Compare alpha test '%s' for saving failed", handler.GetExtension()),
        *expected,
        actual
    );
}

void ImageTestCase::CompareSavedImage()
{
    // FIXME-VC6: Pre-declare the loop variables for compatibility with
    // pre-standard compilers such as MSVC6 that don't implement proper scope
    // for the variables declared in the for loops.
    int i, x, y;

    wxImage expected24("horse.png");
    CPPUNIT_ASSERT( expected24.IsOk() );
    CPPUNIT_ASSERT( !expected24.HasAlpha() );

    wxImage expected8 = expected24.ConvertToGreyscale();

#if wxUSE_PALETTE
    unsigned char greys[256];
    for (i = 0; i < 256; ++i)
    {
        greys[i] = i;
    }
    wxPalette palette(256, greys, greys, greys);
    expected8.SetPalette(palette);
#endif // #if wxUSE_PALETTE

    expected8.SetOption(wxIMAGE_OPTION_BMP_FORMAT, wxBMP_8BPP_PALETTE);

    // Create an image with alpha based on the loaded image
    wxImage expected32(expected24);
    expected32.SetAlpha();

    int width = expected32.GetWidth();
    int height = expected32.GetHeight();
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            expected32.SetAlpha(x, y, (x*y) & wxIMAGE_ALPHA_OPAQUE);
        }
    }

    const wxList& list = wxImage::GetHandlers();
    for ( wxList::compatibility_iterator node = list.GetFirst();
        node; node = node->GetNext() )
    {
        wxImageHandler *handler = (wxImageHandler *) node->GetData();

#if wxUSE_PALETTE
        CompareImage(*handler, expected8, wxIMAGE_HAVE_PALETTE);
#endif
        CompareImage(*handler, expected24);
        CompareImage(*handler, expected32, wxIMAGE_HAVE_ALPHA);
    }
}

void ImageTestCase::SavePNG()
{
    wxImage expected24("horse.png");
    CPPUNIT_ASSERT( expected24.IsOk() );
#if wxUSE_PALETTE
    CPPUNIT_ASSERT( !expected24.HasPalette() );
#endif // #if wxUSE_PALETTE

    wxImage expected8 = expected24.ConvertToGreyscale();

    /*
    horse.png converted to greyscale should be saved without a palette.
    */
    CompareImage(*wxImage::FindHandler(wxBITMAP_TYPE_PNG), expected8);

    /*
    But if we explicitly ask for trying to save with a palette, it should work.
    */
    expected8.SetOption(wxIMAGE_OPTION_PNG_FORMAT, wxPNG_TYPE_PALETTE);

    CompareImage(*wxImage::FindHandler(wxBITMAP_TYPE_PNG),
        expected8, wxIMAGE_HAVE_PALETTE);


    CPPUNIT_ASSERT( expected8.LoadFile("horse.gif") );
#if wxUSE_PALETTE
    CPPUNIT_ASSERT( expected8.HasPalette() );
#endif // #if wxUSE_PALETTE

    CompareImage(*wxImage::FindHandler(wxBITMAP_TYPE_PNG),
        expected8, wxIMAGE_HAVE_PALETTE);

    /*
    Add alpha to the image in such a way that there will still be a maximum
    of 256 unique RGBA combinations. This should result in a saved
    PNG image still being palettised and having alpha.
    */
    expected8.SetAlpha();

    int x, y;
    const int width = expected8.GetWidth();
    const int height = expected8.GetHeight();
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            expected8.SetAlpha(x, y, expected8.GetRed(x, y));
        }
    }

    CompareImage(*wxImage::FindHandler(wxBITMAP_TYPE_PNG),
        expected8, wxIMAGE_HAVE_ALPHA|wxIMAGE_HAVE_PALETTE);

    /*
    Now change the alpha of the first pixel so that we can't save palettised
    anymore because there will be 256+1 entries which is beyond PNGs limit
    of 256 entries.
    */
    expected8.SetAlpha(0, 0, 1);

    CompareImage(*wxImage::FindHandler(wxBITMAP_TYPE_PNG),
        expected8, wxIMAGE_HAVE_ALPHA);

    /*
    Even if we explicitly ask for saving palettised it should not be done.
    */
    expected8.SetOption(wxIMAGE_OPTION_PNG_FORMAT, wxPNG_TYPE_PALETTE);
    CompareImage(*wxImage::FindHandler(wxBITMAP_TYPE_PNG),
        expected8, wxIMAGE_HAVE_ALPHA);

}

static void TestTIFFImage(const wxString& option, int value)
{
    wxImage image("horse.png");

    wxMemoryOutputStream memOut;
    image.SetOption(option, value);

    CPPUNIT_ASSERT(image.SaveFile(memOut, wxBITMAP_TYPE_TIF));

    wxMemoryInputStream memIn(memOut);
    CPPUNIT_ASSERT(memIn.IsOk());

    wxImage savedImage(memIn);
    CPPUNIT_ASSERT(savedImage.IsOk());

    WX_ASSERT_EQUAL_MESSAGE(("While checking for option %s", option),
        true, savedImage.HasOption(option));

    WX_ASSERT_EQUAL_MESSAGE(("While testing for %s", option),
        value, savedImage.GetOptionInt(option));
}

void ImageTestCase::SaveTIFF()
{
    TestTIFFImage(wxIMAGE_OPTION_TIFF_BITSPERSAMPLE, 1);
    TestTIFFImage(wxIMAGE_OPTION_TIFF_SAMPLESPERPIXEL, 1);
    TestTIFFImage(wxIMAGE_OPTION_TIFF_PHOTOMETRIC, 0/*PHOTOMETRIC_MINISWHITE*/);
    TestTIFFImage(wxIMAGE_OPTION_TIFF_PHOTOMETRIC, 1/*PHOTOMETRIC_MINISBLACK*/);
}

void ImageTestCase::SaveAnimatedGIF()
{
#if wxUSE_PALETTE
    wxImage image("horse.gif");
    CPPUNIT_ASSERT( image.IsOk() );

    wxImageArray images;
    images.Add(image);
    int i;
    for (i = 0; i < 4-1; ++i)
    {
        images.Add( images[i].Rotate90() );

        images[i+1].SetPalette(images[0].GetPalette());
    }

    wxMemoryOutputStream memOut;
    CPPUNIT_ASSERT( wxGIFHandler().SaveAnimation(images, &memOut) );

    wxGIFHandler handler;
    wxMemoryInputStream memIn(memOut);
    CPPUNIT_ASSERT(memIn.IsOk());
    const int imageCount = handler.GetImageCount(memIn);
    CPPUNIT_ASSERT_EQUAL(4, imageCount);

    for (i = 0; i < imageCount; ++i)
    {
        wxFileOffset pos = memIn.TellI();
        CPPUNIT_ASSERT( handler.LoadFile(&image, memIn, true, i) );
        memIn.SeekI(pos);

        WX_ASSERT_EQUAL_MESSAGE
        (
            ("Compare test for GIF frame number %d failed", i),
            images[i],
            image
        );
    }
#endif // #if wxUSE_PALETTE
}

void ImageTestCase::ReadCorruptedTGA()
{
    static unsigned char corruptTGA[18+1+3] =
    {
        0,
        0,
        10, // RLE compressed image.
        0, 0,
        0, 0,
        0,
        0, 0,
        0, 0,
        1, 0, // Width is 1.
        1, 0, // Height is 1.
        24, // Bits per pixel.
        0,

        0xff, // Run length (repeat next pixel 127+1 times).
        0xff, 0xff, 0xff // One 24-bit pixel.
    };

    wxMemoryInputStream memIn(corruptTGA, WXSIZEOF(corruptTGA));
    CPPUNIT_ASSERT(memIn.IsOk());

    wxImage tgaImage;
    CPPUNIT_ASSERT( !tgaImage.LoadFile(memIn) );


    /*
    Instead of repeating a pixel 127+1 times, now tell it there will
    follow 127+1 uncompressed pixels (while we only should have 1 in total).
    */
    corruptTGA[18] = 0x7f;
    CPPUNIT_ASSERT( !tgaImage.LoadFile(memIn) );
}

static void TestGIFComment(const wxString& comment)
{
    wxImage image("horse.gif");

    image.SetOption(wxIMAGE_OPTION_GIF_COMMENT, comment);
    wxMemoryOutputStream memOut;
    CPPUNIT_ASSERT(image.SaveFile(memOut, wxBITMAP_TYPE_GIF));

    wxMemoryInputStream memIn(memOut);
    CPPUNIT_ASSERT( image.LoadFile(memIn) );

    CPPUNIT_ASSERT_EQUAL(comment,
        image.GetOption(wxIMAGE_OPTION_GIF_COMMENT));
}

void ImageTestCase::GIFComment()
{
    // Test reading a comment.
    wxImage image("horse.gif");
    CPPUNIT_ASSERT_EQUAL("  Imported from GRADATION image: gray",
        image.GetOption(wxIMAGE_OPTION_GIF_COMMENT));


    // Test writing a comment and reading it back.
    TestGIFComment("Giving the GIF a gifted giraffe as a gift");


    // Test writing and reading a comment again but with a long comment.
    TestGIFComment(wxString(wxT('a'), 256)
        + wxString(wxT('b'), 256)
        + wxString(wxT('c'), 256));


    // Test writing comments in an animated GIF and reading them back.
    CPPUNIT_ASSERT( image.LoadFile("horse.gif") );

    wxImageArray images;
    int i;
    for (i = 0; i < 4; ++i)
    {
        if (i)
        {
            images.Add( images[i-1].Rotate90() );
            images[i].SetPalette(images[0].GetPalette());
        }
        else
        {
            images.Add(image);
        }

        images[i].SetOption(wxIMAGE_OPTION_GIF_COMMENT,
            wxString::Format("GIF comment for frame #%d", i+1));

    }


    wxMemoryOutputStream memOut;
    CPPUNIT_ASSERT( wxGIFHandler().SaveAnimation(images, &memOut) );

    wxGIFHandler handler;
    wxMemoryInputStream memIn(memOut);
    CPPUNIT_ASSERT(memIn.IsOk());
    const int imageCount = handler.GetImageCount(memIn);
    for (i = 0; i < imageCount; ++i)
    {
        wxFileOffset pos = memIn.TellI();
        CPPUNIT_ASSERT( handler.LoadFile(&image, memIn, true /*verbose?*/, i) );

        CPPUNIT_ASSERT_EQUAL(
            wxString::Format("GIF comment for frame #%d", i+1),
            image.GetOption(wxIMAGE_OPTION_GIF_COMMENT));
        memIn.SeekI(pos);
    }
}

void ImageTestCase::DibPadding()
{
    /*
    There used to be an error with calculating the DWORD aligned scan line
    pitch for a BMP/ICO resulting in buffer overwrites (with at least MSVC9
    Debug this gave a heap corruption assertion when saving the mask of
    an ICO). Test for it here.
    */
    wxImage image("horse.gif");
    CPPUNIT_ASSERT( image.IsOk() );

    image = image.Scale(99, 99);

    wxMemoryOutputStream memOut;
    CPPUNIT_ASSERT( image.SaveFile(memOut, wxBITMAP_TYPE_ICO) );
}

static void CompareBMPImage(const wxString& file1, const wxString& file2)
{
    wxImage image1(file1);
    CPPUNIT_ASSERT( image1.IsOk() );

    wxImage image2(file2);
    CPPUNIT_ASSERT( image2.IsOk() );

    CompareImage(*wxImage::FindHandler(wxBITMAP_TYPE_BMP), image1, 0, &image2);
}

void ImageTestCase::BMPFlippingAndRLECompression()
{
    CompareBMPImage("image/horse_grey.bmp", "image/horse_grey_flipped.bmp");

    CompareBMPImage("image/horse_rle8.bmp", "image/horse_grey.bmp");
    CompareBMPImage("image/horse_rle8.bmp", "image/horse_rle8_flipped.bmp");

    CompareBMPImage("image/horse_rle4.bmp", "image/horse_rle4_flipped.bmp");
}
#endif //wxUSE_IMAGE


/*
    TODO: add lots of more tests to wxImage functions
*/
