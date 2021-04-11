///////////////////////////////////////////////////////////////////////////////
// Name:        tests/image/image.cpp
// Purpose:     Test wxImage
// Author:      Francesco Montorsi
// Created:     2009-05-31
// Copyright:   (c) 2009 Francesco Montorsi
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_IMAGE


#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/anidecod.h" // wxImageArray
#include "wx/bitmap.h"
#include "wx/palette.h"
#include "wx/url.h"
#include "wx/log.h"
#include "wx/mstream.h"
#include "wx/zstream.h"
#include "wx/wfstream.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

#include "testimage.h"


#define CHECK_EQUAL_COLOUR_RGB(c1, c2) \
    CHECK( (int)c1.Red()   == (int)c2.Red() ); \
    CHECK( (int)c1.Green() == (int)c2.Green() ); \
    CHECK( (int)c1.Blue()  == (int)c2.Blue() )

#define CHECK_EQUAL_COLOUR_RGBA(c1, c2) \
    CHECK( (int)c1.Red()   == (int)c2.Red() ); \
    CHECK( (int)c1.Green() == (int)c2.Green() ); \
    CHECK( (int)c1.Blue()  == (int)c2.Blue() ); \
    CHECK( (int)c1.Alpha() == (int)c2.Alpha() )

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
    { "horse.tif", wxBITMAP_TYPE_TIFF, 8 }
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
#if wxUSE_LIBTIFF
        CPPUNIT_TEST( SaveTIFF );
#endif // wxUSE_LIBTIFF
        CPPUNIT_TEST( ReadCorruptedTGA );
#if wxUSE_GIF
        CPPUNIT_TEST( SaveAnimatedGIF );
        CPPUNIT_TEST( GIFComment );
#endif // wxUSE_GIF
        CPPUNIT_TEST( DibPadding );
        CPPUNIT_TEST( BMPFlippingAndRLECompression );
        CPPUNIT_TEST( ScaleCompare );
        CPPUNIT_TEST( CreateBitmapFromCursor );
    CPPUNIT_TEST_SUITE_END();

    void LoadFromSocketStream();
    void LoadFromZipStream();
    void LoadFromFile();
    void SizeImage();
    void CompareLoadedImage();
    void CompareSavedImage();
    void SavePNG();
#if wxUSE_LIBTIFF
    void SaveTIFF();
#endif // wxUSE_LIBTIFF
    void ReadCorruptedTGA();
#if wxUSE_GIF
    void SaveAnimatedGIF();
    void GIFComment();
#endif // wxUSE_GIF
    void DibPadding();
    void BMPFlippingAndRLECompression();
    void ScaleCompare();
    void CreateBitmapFromCursor();

    wxDECLARE_NO_COPY_CLASS(ImageTestCase);
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
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif // wxUSE_GIF
    wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxPCXHandler);
    wxImage::AddHandler(new wxPNMHandler);
    wxImage::AddHandler(new wxTGAHandler);
#if wxUSE_LIBTIFF
    wxImage::AddHandler(new wxTIFFHandler);
#endif // wxUSE_LIBTIFF
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
        { "http://www.wxwidgets.org/assets/img/header-logo.png", wxBITMAP_TYPE_PNG },
        { "http://www.wxwidgets.org/assets/ico/favicon-1.ico", wxBITMAP_TYPE_ICO }
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
            case wxBITMAP_TYPE_TIFF:
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

       wxINFO_FMT("Resize test #%u: (%d, %d), (%d, %d)",
                   i, st.w, st.h, st.dx, st.dy);
       CHECK_THAT( actual, RGBSameAs(expected) );
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


        wxINFO_FMT("Compare test '%s' for loading", g_testfiles[i].file);
        CHECK_THAT( actual,
                    RGBSameAs(g_testfiles[i].bitDepth == 8 ? expected8
                                                           : expected24) );
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
        && !(type == wxBITMAP_TYPE_PNG || type == wxBITMAP_TYPE_TGA
            || type == wxBITMAP_TYPE_TIFF) )
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

    unsigned bitsPerPixel = testPalette ? 8 : (testAlpha ? 32 : 24);
    wxINFO_FMT("Compare test '%s (%d-bit)' for saving",
               handler.GetExtension(), bitsPerPixel);
    CHECK_THAT(actual, RGBSameAs(*expected));

#if wxUSE_PALETTE
    CPPUNIT_ASSERT(actual.HasPalette()
        == (testPalette || type == wxBITMAP_TYPE_XPM));
#endif

    CPPUNIT_ASSERT( actual.HasAlpha() == testAlpha);

    if (!testAlpha)
    {
        return;
    }

    wxINFO_FMT("Compare alpha test '%s' for saving", handler.GetExtension());
    CHECK_THAT(actual, RGBSameAs(*expected));
}

static void SetAlpha(wxImage *image)
{
    image->SetAlpha();

    unsigned char *ptr = image->GetAlpha();
    const int width = image->GetWidth();
    const int height = image->GetHeight();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            ptr[y*width + x] = (x*y) & wxIMAGE_ALPHA_OPAQUE;
        }
    }
}

void ImageTestCase::CompareSavedImage()
{
    wxImage expected24("horse.png");
    CPPUNIT_ASSERT( expected24.IsOk() );
    CPPUNIT_ASSERT( !expected24.HasAlpha() );

    wxImage expected8 = expected24.ConvertToGreyscale();

#if wxUSE_PALETTE
    unsigned char greys[256];
    for (int i = 0; i < 256; ++i)
    {
        greys[i] = i;
    }
    wxPalette palette(256, greys, greys, greys);
    expected8.SetPalette(palette);
#endif // #if wxUSE_PALETTE

    expected8.SetOption(wxIMAGE_OPTION_BMP_FORMAT, wxBMP_8BPP_PALETTE);

    // Create an image with alpha based on the loaded image
    wxImage expected32(expected24);

    SetAlpha(&expected32);

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

#if wxUSE_LIBTIFF
static void TestTIFFImage(const wxString& option, int value,
    const wxImage *compareImage = NULL)
{
    wxImage image;
    if (compareImage)
    {
        image = *compareImage;
    }
    else
    {
        (void) image.LoadFile("horse.png");
    }
    CPPUNIT_ASSERT( image.IsOk() );

    wxMemoryOutputStream memOut;
    image.SetOption(option, value);

    CPPUNIT_ASSERT(image.SaveFile(memOut, wxBITMAP_TYPE_TIFF));

    wxMemoryInputStream memIn(memOut);
    CPPUNIT_ASSERT(memIn.IsOk());

    wxImage savedImage(memIn);
    CPPUNIT_ASSERT(savedImage.IsOk());

    WX_ASSERT_EQUAL_MESSAGE(("While checking for option %s", option),
        true, savedImage.HasOption(option));

    WX_ASSERT_EQUAL_MESSAGE(("While testing for %s", option),
        value, savedImage.GetOptionInt(option));

    WX_ASSERT_EQUAL_MESSAGE(("HasAlpha() not equal"), image.HasAlpha(), savedImage.HasAlpha());
}

void ImageTestCase::SaveTIFF()
{
    TestTIFFImage(wxIMAGE_OPTION_TIFF_BITSPERSAMPLE, 1);
    TestTIFFImage(wxIMAGE_OPTION_TIFF_SAMPLESPERPIXEL, 1);
    TestTIFFImage(wxIMAGE_OPTION_TIFF_PHOTOMETRIC, 0/*PHOTOMETRIC_MINISWHITE*/);
    TestTIFFImage(wxIMAGE_OPTION_TIFF_PHOTOMETRIC, 1/*PHOTOMETRIC_MINISBLACK*/);

    wxImage alphaImage("horse.png");
    CPPUNIT_ASSERT( alphaImage.IsOk() );
    SetAlpha(&alphaImage);

    // RGB with alpha
    TestTIFFImage(wxIMAGE_OPTION_TIFF_SAMPLESPERPIXEL, 4, &alphaImage);

    // Grey with alpha
    TestTIFFImage(wxIMAGE_OPTION_TIFF_SAMPLESPERPIXEL, 2, &alphaImage);

    // B/W with alpha
    alphaImage.SetOption(wxIMAGE_OPTION_TIFF_BITSPERSAMPLE, 1);
    TestTIFFImage(wxIMAGE_OPTION_TIFF_SAMPLESPERPIXEL, 2, &alphaImage);
}
#endif // wxUSE_LIBTIFF

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

#if wxUSE_GIF

void ImageTestCase::SaveAnimatedGIF()
{
#if wxUSE_PALETTE
    wxImage image("horse.gif");
    CPPUNIT_ASSERT( image.IsOk() );

    wxImageArray images;
    images.Add(image);
    for (int i = 0; i < 4-1; ++i)
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

    for (int i = 0; i < imageCount; ++i)
    {
        wxFileOffset pos = memIn.TellI();
        CPPUNIT_ASSERT( handler.LoadFile(&image, memIn, true, i) );
        memIn.SeekI(pos);

        wxINFO_FMT("Compare test for GIF frame number %d failed", i);
        CHECK_THAT(image, RGBSameAs(images[i]));
    }
#endif // #if wxUSE_PALETTE
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

#if wxUSE_PALETTE
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
#endif //wxUSE_PALETTE
}

#endif // wxUSE_GIF

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


static int
FindMaxChannelDiff(const wxImage& i1, const wxImage& i2)
{
    if ( i1.GetWidth() != i2.GetWidth() )
        return false;

    if ( i1.GetHeight() != i2.GetHeight() )
        return false;

    const unsigned char* p1 = i1.GetData();
    const unsigned char* p2 = i2.GetData();
    const int numBytes = i1.GetWidth()*i1.GetHeight()*3;
    int maxDiff = 0;
    for ( int n = 0; n < numBytes; n++, p1++, p2++ )
    {
        const int diff = std::abs(*p1 - *p2);
        if ( diff > maxDiff )
            maxDiff = diff;
    }

    return maxDiff;
}

// Note that we accept up to one pixel difference, this happens because of
// different rounding behaviours in different compiler versions
// even under the same architecture, see the example in
// http://thread.gmane.org/gmane.comp.lib.wxwidgets.devel/151149/focus=151154

// The 0 below can be replaced with 1 to generate, instead of comparing with,
// the test files.
#define ASSERT_IMAGE_EQUAL_TO_FILE(image, file) \
    if ( 0 ) \
    { \
        CPPUNIT_ASSERT_MESSAGE( "Failed to save " file, image.SaveFile(file) ); \
    } \
    else \
    { \
        const wxImage imageFromFile(file); \
        if ( imageFromFile.IsOk() ) \
        { \
            INFO("Wrong scaled \"" << file << "\" " << Catch::toString(image)); \
            CHECK(FindMaxChannelDiff(imageFromFile, image) <= 1); \
        } \
        else \
        { \
            FAIL("Failed to load \"" << file << "\""); \
        } \
    }

void ImageTestCase::ScaleCompare()
{
    wxImage original;
    CPPUNIT_ASSERT(original.LoadFile("horse.bmp"));

    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale( 50,  50, wxIMAGE_QUALITY_BICUBIC),
                               "image/horse_bicubic_50x50.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(100, 100, wxIMAGE_QUALITY_BICUBIC),
                               "image/horse_bicubic_100x100.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(150, 150, wxIMAGE_QUALITY_BICUBIC),
                               "image/horse_bicubic_150x150.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(300, 300, wxIMAGE_QUALITY_BICUBIC),
                               "image/horse_bicubic_300x300.png");

    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale( 50,  50, wxIMAGE_QUALITY_BOX_AVERAGE),
                               "image/horse_box_average_50x50.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(100, 100, wxIMAGE_QUALITY_BOX_AVERAGE),
                               "image/horse_box_average_100x100.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(150, 150, wxIMAGE_QUALITY_BOX_AVERAGE),
                               "image/horse_box_average_150x150.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(300, 300, wxIMAGE_QUALITY_BOX_AVERAGE),
                               "image/horse_box_average_300x300.png");

    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale( 50,  50, wxIMAGE_QUALITY_BILINEAR),
                               "image/horse_bilinear_50x50.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(100, 100, wxIMAGE_QUALITY_BILINEAR),
                               "image/horse_bilinear_100x100.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(150, 150, wxIMAGE_QUALITY_BILINEAR),
                               "image/horse_bilinear_150x150.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(original.Scale(300, 300, wxIMAGE_QUALITY_BILINEAR),
                               "image/horse_bilinear_300x300.png");

    // Test scaling symmetric image
    const static char* cross_xpm[] =
    {
        "9 9 5 1",
        "   c None",
        "r  c #FF0000",
        "g  c #00FF00",
        "b  c #0000FF",
        "w  c #FFFFFF",
        "    r    ",
        "    g    ",
        "    b    ",
        "    w    ",
        "rgbw wbgr",
        "    w    ",
        "    b    ",
        "    g    ",
        "    r    "
    };

    wxImage imgCross(cross_xpm);
    ASSERT_IMAGE_EQUAL_TO_FILE(imgCross.Scale(256, 256, wxIMAGE_QUALITY_BILINEAR),
                               "image/cross_bilinear_256x256.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(imgCross.Scale(256, 256, wxIMAGE_QUALITY_BICUBIC),
                               "image/cross_bicubic_256x256.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(imgCross.Scale(256, 256, wxIMAGE_QUALITY_BOX_AVERAGE),
                               "image/cross_box_average_256x256.png");
    ASSERT_IMAGE_EQUAL_TO_FILE(imgCross.Scale(256, 256, wxIMAGE_QUALITY_NEAREST),
                               "image/cross_nearest_neighb_256x256.png");
}

void ImageTestCase::CreateBitmapFromCursor()
{
#if !defined __WXOSX_IPHONE__ && !defined __WXDFB__ && !defined __WXMOTIF__ && !defined __WXX11__

    wxImage image("image/wx.png");
    wxCursor cursor(image);
    wxBitmap bitmap(cursor);

#if defined(__WXGTK__)
    // cursor to bitmap could fail depending on windowing system and cursor (gdk-cursor-get-image)
    if ( !bitmap.IsOk() )
        return;
#endif

    wxImage result = bitmap.ConvertToImage();

    // on Windows the cursor is always scaled to 32x32px (96 DPI)
    // on macOS the resulting bitmap size depends on the DPI
    if ( image.GetSize() == result.GetSize() )
    {
        CHECK_THAT(image, RGBASimilarTo(result, 2));
    }
    else
    {
        wxVector<wxPoint> coords;
        coords.push_back(wxPoint(14, 10)); // blue square
        coords.push_back(wxPoint(8, 22));  // red square
        coords.push_back(wxPoint(26, 18)); // yellow square
        coords.push_back(wxPoint(25, 5));  // empty / tranparent

        for ( size_t i = 0; i < coords.size(); ++i )
        {
            wxPoint const& p1 = coords[i];
            wxPoint p2 = wxPoint(p1.x * (result.GetWidth() / (double)image.GetWidth()), p1.y * (result.GetHeight() / (double)image.GetHeight()));

#if defined(__WXMSW__)
            // when the cursor / result image is larger than the source image, the original image is centered in the result image
            if ( result.GetWidth() > image.GetWidth() )
                p2.x = (result.GetWidth() / 2) + (p1.x - (image.GetWidth() / 2));
            if ( result.GetHeight() > image.GetHeight() )
                p2.y = (result.GetHeight() / 2) + (p1.y - (image.GetHeight() / 2));
#endif

            wxColour cSrc(image.GetRed(p1.x, p1.y), image.GetGreen(p1.x, p1.y), image.GetBlue(p1.x, p1.y), image.GetAlpha(p1.x, p1.y));
            wxColour cRes(result.GetRed(p2.x, p2.y), result.GetGreen(p2.x, p2.y), result.GetBlue(p2.x, p2.y), result.GetAlpha(p2.x, p2.y));

            CHECK_EQUAL_COLOUR_RGBA(cRes, cSrc);
        }
    }
#endif
}

#endif //wxUSE_IMAGE

TEST_CASE("wxImage::Paste", "[image][paste]")
{
    const static char* squares_xpm[] =
    {
        "9 9 7 1",
        "   c None",
        "y  c #FFFF00",
        "r  c #FF0000",
        "g  c #00FF00",
        "b  c #0000FF",
        "o  c #FF6600",
        "w  c #FFFFFF",
        "rrrrwgggg",
        "rrrrwgggg",
        "rrrrwgggg",
        "rrrrwgggg",
        "wwwwwwwww",
        "bbbbwoooo",
        "bbbbwoooo",
        "bbbbwoooo",
        "bbbbwoooo"
    };

    const static char* toggle_equal_size_xpm[] =
    {
        "9 9 2 1",
        "   c None",
        "y  c #FFFF00",
        "y y y y y",
        " y y y y ",
        "y y y y y",
        " y y y y ",
        "y y y y y",
        " y y y y ",
        "y y y y y",
        " y y y y ",
        "y y y y y",
    };

    const static char* transparent_image_xpm[] =
    {
        "5 5 2 1",
        "   c None", // Mask
        "y  c #FFFF00",
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
    };

    const static char* light_image_xpm[] =
    {
        "5 5 2 1",
        "   c None",
        "y  c #FFFF00",
        "yyyyy",
        "yyyyy",
        "yyyyy",
        "yyyyy",
        "yyyyy",
    };

    const static char* black_image_xpm[] =
    {
        "5 5 2 1",
        "   c #000000",
        "y  c None", // Mask
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
    };

    // Execute AddHandler() just once.
    static const bool
        registeredHandler = (wxImage::AddHandler(new wxPNGHandler()), true);

    SECTION("Paste same size image")
    {
        wxImage actual(squares_xpm);
        wxImage paste(toggle_equal_size_xpm);
        wxImage expected(toggle_equal_size_xpm);
        actual.Paste(paste, 0, 0);
        CHECK_THAT(actual, RGBSameAs(expected));

        // Without alpha using "compose" doesn't change anything.
        actual.Paste(paste, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSameAs(expected));
    }

    SECTION("Paste larger image")
    {
        const static char* toggle_larger_size_xpm[] =
        {
            "13 13 2 1",
            "   c None",
            "y  c #FFFF00",
            "y y y y y y y",
            " y y y y y y ",
            "y y y y y y y",
            " y y y y y y ",
            "y y y y y y y",
            " y y y y y y ",
            "y y y y y y y",
            " y y y y y y ",
            "y y y y y y y",
            " y y y y y y ",
            "y y y y y y y",
            " y y y y y y ",
            "y y y y y y y",
        };

        wxImage actual(squares_xpm);
        wxImage paste(toggle_larger_size_xpm);
        wxImage expected(toggle_equal_size_xpm);
        actual.Paste(paste, -2, -2);
        CHECK_THAT(actual, RGBSameAs(expected));
    }

    SECTION("Paste smaller image")
    {
        const static char* toggle_smaller_size_xpm[] =
        {
            "5 5 2 1",
            "   c None",
            "y  c #FFFF00",
            "y y y",
            " y y ",
            "y y y",
            " y y ",
            "y y y",
        };

        const static char* expected_xpm[] =
        {
            "9 9 7 1",
            "   c None",
            "y  c #FFFF00",
            "r  c #FF0000",
            "g  c #00FF00",
            "b  c #0000FF",
            "o  c #FF6600",
            "w  c #FFFFFF",
            "rrrrwgggg",
            "rrrrwgggg",
            "rry y ygg",
            "rr y y gg",
            "wwy y yww",
            "bb y y oo",
            "bby y yoo",
            "bbbbwoooo",
            "bbbbwoooo"
        };

        wxImage actual(squares_xpm);
        wxImage paste(toggle_smaller_size_xpm);
        wxImage expected(expected_xpm);
        actual.Paste(paste, 2, 2);
        CHECK_THAT(actual, RGBSameAs(expected));
    }

    SECTION("Paste beyond top left corner")
    {
        const static char* expected_xpm[] =
        {
            "9 9 7 1",
            "   c None",
            "y  c #FFFF00",
            "r  c #FF0000",
            "g  c #00FF00",
            "b  c #0000FF",
            "o  c #FF6600",
            "w  c #FFFFFF",
            "oooowgggg",
            "oooowgggg",
            "oooowgggg",
            "oooowgggg",
            "wwwwwwwww",
            "bbbbwoooo",
            "bbbbwoooo",
            "bbbbwoooo",
            "bbbbwoooo"
        };

        wxImage actual(squares_xpm);
        wxImage paste(squares_xpm);
        wxImage expected(expected_xpm);
        actual.Paste(paste, -5, -5);
        CHECK_THAT(actual, RGBSameAs(expected));
    }

    SECTION("Paste beyond top right corner")
    {
        const static char* expected_xpm[] =
        {
            "9 9 7 1",
            "   c None",
            "y  c #FFFF00",
            "r  c #FF0000",
            "g  c #00FF00",
            "b  c #0000FF",
            "o  c #FF6600",
            "w  c #FFFFFF",
            "rrrrwbbbb",
            "rrrrwbbbb",
            "rrrrwbbbb",
            "rrrrwbbbb",
            "wwwwwwwww",
            "bbbbwoooo",
            "bbbbwoooo",
            "bbbbwoooo",
            "bbbbwoooo"
        };
        wxImage actual(squares_xpm);
        wxImage paste(squares_xpm);
        wxImage expected(expected_xpm);
        actual.Paste(paste, 5, -5);
        CHECK_THAT(actual, RGBSameAs(expected));
    }

    SECTION("Paste beyond bottom right corner")
    {
        const static char* expected_xpm[] =
        {
            "9 9 7 1",
            "   c None",
            "y  c #FF0000",
            "r  c #FF0000",
            "g  c #00FF00",
            "b  c #0000FF",
            "o  c #FF6600",
            "w  c #FFFFFF",
            "rrrrwgggg",
            "rrrrwgggg",
            "rrrrwgggg",
            "rrrrwgggg",
            "wwwwwwwww",
            "bbbbwrrrr",
            "bbbbwrrrr",
            "bbbbwrrrr",
            "bbbbwrrrr"
        };
        wxImage actual(squares_xpm);
        wxImage paste(squares_xpm);
        wxImage expected(expected_xpm);
        actual.Paste(paste, 5, 5);
        CHECK_THAT(actual, RGBSameAs(expected));
    }

    SECTION("Paste beyond bottom left corner")
    {
        const static char* expected_xpm[] =
        {
            "9 9 7 1",
            "   c None",
            "y  c #FFFF00",
            "r  c #FF0000",
            "g  c #00FF00",
            "b  c #0000FF",
            "o  c #FF6600",
            "w  c #FFFFFF",
            "rrrrwgggg",
            "rrrrwgggg",
            "rrrrwgggg",
            "rrrrwgggg",
            "wwwwwwwww",
            "ggggwoooo",
            "ggggwoooo",
            "ggggwoooo",
            "ggggwoooo"
        };
        wxImage actual(squares_xpm);
        wxImage paste(squares_xpm);
        wxImage expected(expected_xpm);
        actual.Paste(paste, -5, 5);
        CHECK_THAT(actual, RGBSameAs(expected));
    }

    SECTION("Paste fully opaque image onto blank image without alpha")
    {
        const wxImage background("image/paste_input_background.png");
        REQUIRE(background.IsOk());

        wxImage actual(background.GetSize());
        actual.Paste(background, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSameAs(background));
        CHECK(!actual.HasAlpha());
    }
    SECTION("Paste fully opaque image onto blank image with alpha")
    {
        const wxImage background("image/paste_input_background.png");
        REQUIRE(background.IsOk());

        wxImage actual(background.GetSize());
        actual.InitAlpha();
        actual.Paste(background, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSameAs(background));
        CHECK_THAT(actual, CenterAlphaPixelEquals(wxALPHA_OPAQUE));
    }
    SECTION("Paste fully transparent image")
    {
        const wxImage background("image/paste_input_background.png");
        REQUIRE(background.IsOk());

        wxImage actual = background.Copy();
        wxImage transparent(actual.GetSize());
        transparent.InitAlpha();
        memset(transparent.GetAlpha(), 0, transparent.GetWidth() * transparent.GetHeight());
        actual.Paste(transparent, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSameAs(background));
        CHECK_THAT(actual, CenterAlphaPixelEquals(wxALPHA_OPAQUE));
    }
    SECTION("Paste image with transparent region")
    {
        wxImage actual("image/paste_input_background.png");
        REQUIRE(actual.IsOk());

        const wxImage opaque_square("image/paste_input_overlay_transparent_border_opaque_square.png");
        REQUIRE(opaque_square.IsOk());

        actual.Paste(opaque_square, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSameAs(wxImage("image/paste_result_background_plus_overlay_transparent_border_opaque_square.png")));
        CHECK_THAT(actual, CenterAlphaPixelEquals(wxALPHA_OPAQUE));
    }
    SECTION("Paste image with semi transparent region")
    {
        wxImage actual("image/paste_input_background.png");
        REQUIRE(actual.IsOk());

        const wxImage transparent_square("image/paste_input_overlay_transparent_border_semitransparent_square.png");
        REQUIRE(transparent_square.IsOk());

        actual.Paste(transparent_square, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSameAs(wxImage("image/paste_result_background_plus_overlay_transparent_border_semitransparent_square.png")));
        CHECK_THAT(actual, CenterAlphaPixelEquals(wxALPHA_OPAQUE));
    }
    SECTION("Paste two semi transparent images on top of background")
    {
        wxImage actual("image/paste_input_background.png");
        REQUIRE(actual.IsOk());

        const wxImage transparent_square("image/paste_input_overlay_transparent_border_semitransparent_square.png");
        REQUIRE(transparent_square.IsOk());

        const wxImage transparent_circle("image/paste_input_overlay_transparent_border_semitransparent_circle.png");
        REQUIRE(transparent_circle.IsOk());

        actual.Paste(transparent_circle, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        actual.Paste(transparent_square, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSimilarTo(wxImage("image/paste_result_background_plus_circle_plus_square.png"), 1));
        CHECK_THAT(actual, CenterAlphaPixelEquals(wxALPHA_OPAQUE));
    }
    SECTION("Paste two semi transparent images together first, then on top of background")
    {
        wxImage actual("image/paste_input_background.png");
        REQUIRE(actual.IsOk());

        const wxImage transparent_square("image/paste_input_overlay_transparent_border_semitransparent_square.png");
        REQUIRE(transparent_square.IsOk());

        const wxImage transparent_circle("image/paste_input_overlay_transparent_border_semitransparent_circle.png");
        REQUIRE(transparent_circle.IsOk());

        wxImage circle = transparent_circle.Copy();
        circle.Paste(transparent_square, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        actual.Paste(circle, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        // When applied in this order, two times a rounding difference is triggered.
        CHECK_THAT(actual, RGBSimilarTo(wxImage("image/paste_result_background_plus_circle_plus_square.png"), 2));
        CHECK_THAT(actual, CenterAlphaPixelEquals(wxALPHA_OPAQUE));
    }
    SECTION("Paste semitransparent image over transparent image")
    {
        const wxImage transparent_square("image/paste_input_overlay_transparent_border_semitransparent_square.png");
        REQUIRE(transparent_square.IsOk());

        const wxImage transparent_circle("image/paste_input_overlay_transparent_border_semitransparent_circle.png");
        REQUIRE(transparent_circle.IsOk());

        wxImage actual(transparent_circle.GetSize());
        actual.InitAlpha();
        memset(actual.GetAlpha(), 0, actual.GetWidth() * actual.GetHeight());
        actual.Paste(transparent_circle, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, CenterAlphaPixelEquals(192));
        actual.Paste(transparent_square, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSimilarTo(wxImage("image/paste_result_no_background_square_over_circle.png"), 1));
        CHECK_THAT(actual, CenterAlphaPixelEquals(224));
    }
    SECTION("Paste fully transparent (masked) image over light image") // todo make test case for 'blend with mask'
    {
        wxImage actual(light_image_xpm);
        actual.InitAlpha();
        wxImage paste(transparent_image_xpm);
        wxImage expected(light_image_xpm);
        actual.Paste(paste, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSameAs(expected));
    }
    SECTION("Paste fully black (masked) image over light image") // todo make test case for 'blend with mask'
    {
        wxImage actual(light_image_xpm);
        actual.InitAlpha();
        wxImage paste(black_image_xpm);
        wxImage expected(black_image_xpm);
        actual.Paste(paste, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, RGBSameAs(expected));
    }
    SECTION("Paste dark image over light image")
    {
        wxImage black("image/paste_input_black.png");
        wxImage actual("image/paste_input_background.png");
        actual.InitAlpha();
        actual.Paste(black, 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSE);
        CHECK_THAT(actual, CenterAlphaPixelEquals(255));
        CHECK_THAT(actual, RGBSameAs(black));
    }
    SECTION("Paste large image with negative vertical offset")
    {
        wxImage target(442, 249);
        wxImage to_be_pasted(345, 24900);
        target.InitAlpha();
        target.Paste(to_be_pasted, 48, -12325, wxIMAGE_ALPHA_BLEND_COMPOSE);
    }
    SECTION("Paste large image with negative horizontal offset")
    {
        wxImage target(249, 442);
        wxImage to_be_pasted(24900, 345);
        target.InitAlpha();
        target.Paste(to_be_pasted, -12325, 48, wxIMAGE_ALPHA_BLEND_COMPOSE);
    }

}

TEST_CASE("wxImage::RGBtoHSV", "[image][rgb][hsv]")
{
    SECTION("RGB(0,0,0) (Black) to HSV")
    {
       wxImage::RGBValue rgbBlack(0, 0, 0);
       wxImage::HSVValue hsvBlack = wxImage::RGBtoHSV(rgbBlack);

       CHECK(hsvBlack.value == Approx(0.0));
       // saturation and hue are undefined
    }
    SECTION("RGB(255,255,255) (White) to HSV")
    {
       wxImage::RGBValue rgbWhite(255, 255, 255);
       wxImage::HSVValue hsvWhite = wxImage::RGBtoHSV(rgbWhite);

       CHECK(hsvWhite.saturation == Approx(0.0));
       CHECK(hsvWhite.value == Approx(1.0));
       // hue is undefined
    }
    SECTION("RGB(0,255,0) (Green) to HSV")
    {
       wxImage::RGBValue rgbGreen(0, 255, 0);
       wxImage::HSVValue hsvGreen = wxImage::RGBtoHSV(rgbGreen);

       CHECK(hsvGreen.hue == Approx(1.0/3.0));
       CHECK(hsvGreen.saturation == Approx(1.0));
       CHECK(hsvGreen.value == Approx(1.0));
    }

    SECTION("RGB to HSV to RGB")
    {
        const wxImage::RGBValue rgbValues[] =
        {
            wxImage::RGBValue(   0,   0,   0 ),
            wxImage::RGBValue(  10,  10,  10 ),
            wxImage::RGBValue( 255, 255, 255 ),
            wxImage::RGBValue( 255,   0,   0 ),
            wxImage::RGBValue(   0, 255,   0 ),
            wxImage::RGBValue(   0,   0, 255 ),
            wxImage::RGBValue(   1,   2,   3 ),
            wxImage::RGBValue(  10,  20,  30 ),
            wxImage::RGBValue(   0,   1,   6 ),
            wxImage::RGBValue(   9,   0,  99 ),
        };

        for (unsigned i = 0; i < WXSIZEOF(rgbValues); i++)
        {
            wxImage::RGBValue rgbValue = rgbValues[i];
            wxImage::HSVValue hsvValue = wxImage::RGBtoHSV(rgbValue);
            wxImage::RGBValue rgbRoundtrip = wxImage::HSVtoRGB(hsvValue);

            CHECK(rgbRoundtrip.red == rgbValue.red);
            CHECK(rgbRoundtrip.green == rgbValue.green);
            CHECK(rgbRoundtrip.blue == rgbValue.blue);
        }
    }
}

TEST_CASE("wxImage::Clipboard", "[image][clipboard]")
{
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ
    wxInitAllImageHandlers();

    wxImage imgOriginal;
    REQUIRE(imgOriginal.LoadFile("horse.png") == true);

    wxImageDataObject* dobj1 = new wxImageDataObject(imgOriginal);
    {
        wxClipboardLocker lockClip;
        REQUIRE(wxTheClipboard->SetData(dobj1) == true);
    }

    wxImageDataObject dobj2;
    {
        wxClipboardLocker lockClip;
        REQUIRE(wxTheClipboard->GetData(dobj2) == true);
    }
    wxImage imgRetrieved = dobj2.GetImage();
    REQUIRE(imgRetrieved.IsOk());

    CHECK_THAT(imgOriginal, RGBASameAs(imgRetrieved));
#endif // wxUSE_CLIPBOARD && wxUSE_DATAOBJ
}

TEST_CASE("wxImage::InitAlpha", "[image][initalpha]")
{
    const wxColour maskCol(*wxRED);
    const wxColour fillCol(*wxGREEN);

    SECTION("RGB image without mask")
    {
        wxImage img(2, 2);
        img.SetRGB(0, 0, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(0, 1, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(1, 0, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetRGB(1, 1, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        REQUIRE_FALSE(img.HasAlpha());
        REQUIRE_FALSE(img.HasMask());

        wxImage imgRes = img;
        imgRes.InitAlpha();
        REQUIRE(imgRes.HasAlpha() == true);
        REQUIRE_FALSE(imgRes.HasMask());

        for (int y = 0; y < img.GetHeight(); y++)
            for (int x = 0; x < img.GetWidth(); x++)
            {
                wxColour cSrc(img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y));
                wxColour cRes(imgRes.GetRed(x, y), imgRes.GetGreen(x, y), imgRes.GetBlue(x, y), imgRes.GetAlpha(x, y));

                CHECK_EQUAL_COLOUR_RGB(cRes, cSrc);
                CHECK((int)cRes.Alpha() == (int)wxIMAGE_ALPHA_OPAQUE);
            }
    }

    SECTION("RGB image with mask")
    {
        wxImage img(2, 2);
        img.SetRGB(0, 0, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(0, 1, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(1, 0, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetRGB(1, 1, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetMaskColour(maskCol.Red(), maskCol.Green(), maskCol.Blue());
        REQUIRE_FALSE(img.HasAlpha());
        REQUIRE(img.HasMask() == true);

        wxImage imgRes = img;
        imgRes.InitAlpha();
        REQUIRE(imgRes.HasAlpha() == true);
        REQUIRE_FALSE(imgRes.HasMask());

        for ( int y = 0; y < img.GetHeight(); y++ )
            for ( int x = 0; x < img.GetWidth(); x++ )
            {
                wxColour cSrc(img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y));
                wxColour cRes(imgRes.GetRed(x, y), imgRes.GetGreen(x, y), imgRes.GetBlue(x, y), imgRes.GetAlpha(x, y));

                CHECK_EQUAL_COLOUR_RGB(cRes, cSrc);
                if ( cSrc == maskCol )
                {
                    CHECK((int)cRes.Alpha() == (int)wxIMAGE_ALPHA_TRANSPARENT);
                }
                else
                {
                    CHECK((int)cRes.Alpha() == (int)wxIMAGE_ALPHA_OPAQUE);
                }
            }
    }

    SECTION("RGBA image without mask")
    {
        wxImage img(2, 2);
        img.SetRGB(0, 0, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(0, 1, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(1, 0, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetRGB(1, 1, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetAlpha();
        img.SetAlpha(0, 0, 128);
        img.SetAlpha(0, 1, 0);
        img.SetAlpha(1, 0, 128);
        img.SetAlpha(1, 1, 0);
        REQUIRE(img.HasAlpha() == true);
        REQUIRE_FALSE(img.HasMask());

        wxImage imgRes = img;
        CHECK_THROWS(imgRes.InitAlpha());
        REQUIRE(imgRes.HasAlpha() == true);
        REQUIRE_FALSE(imgRes.HasMask());

        for ( int y = 0; y < img.GetHeight(); y++ )
            for ( int x = 0; x < img.GetWidth(); x++ )
            {
                wxColour cSrc(img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y), img.GetAlpha(x, y));
                wxColour cRes(imgRes.GetRed(x, y), imgRes.GetGreen(x, y), imgRes.GetBlue(x, y), imgRes.GetAlpha(x, y));

                CHECK_EQUAL_COLOUR_RGBA(cRes, cSrc);
            }
    }

    SECTION("RGBA image with mask")
    {
        wxImage img(2, 2);
        img.SetRGB(0, 0, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(0, 1, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(1, 0, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetRGB(1, 1, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetAlpha();
        img.SetAlpha(0, 0, 128);
        img.SetAlpha(0, 1, 0);
        img.SetAlpha(1, 0, 128);
        img.SetAlpha(1, 1, 0);
        img.SetMaskColour(maskCol.Red(), maskCol.Green(), maskCol.Blue());
        REQUIRE(img.HasAlpha() == true);
        REQUIRE(img.HasMask() == true);

        wxImage imgRes = img;
        CHECK_THROWS(imgRes.InitAlpha());
        REQUIRE(imgRes.HasAlpha() == true);
        REQUIRE(imgRes.HasMask() == true);

        for ( int y = 0; y < img.GetHeight(); y++ )
            for ( int x = 0; x < img.GetWidth(); x++ )
            {
                wxColour cSrc(img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y), img.GetAlpha(x, y));
                wxColour cRes(imgRes.GetRed(x, y), imgRes.GetGreen(x, y), imgRes.GetBlue(x, y), imgRes.GetAlpha(x, y));

                CHECK_EQUAL_COLOUR_RGBA(cRes, cSrc);
            }
    }
}

/*
    TODO: add lots of more tests to wxImage functions
*/
