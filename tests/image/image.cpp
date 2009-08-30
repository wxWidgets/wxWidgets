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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/image.h"
#include "wx/url.h"
#include "wx/log.h"
#include "wx/mstream.h"
#include "wx/zstream.h"
#include "wx/wfstream.h"

struct testData {
    const char* file;
    wxBitmapType type;
} g_testfiles[] = 
{
    { "horse.ico", wxBITMAP_TYPE_ICO },
    { "horse.xpm", wxBITMAP_TYPE_XPM },
    { "horse.png", wxBITMAP_TYPE_PNG },
    { "horse.ani", wxBITMAP_TYPE_ANI },
    { "horse.bmp", wxBITMAP_TYPE_BMP },
    { "horse.cur", wxBITMAP_TYPE_CUR },
    { "horse.gif", wxBITMAP_TYPE_GIF },
    { "horse.jpg", wxBITMAP_TYPE_JPEG },
    { "horse.pcx", wxBITMAP_TYPE_PCX },
    { "horse.pnm", wxBITMAP_TYPE_PNM },
    { "horse.tga", wxBITMAP_TYPE_TGA },
    { "horse.tif", wxBITMAP_TYPE_TIF }
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
    CPPUNIT_TEST_SUITE_END();

    void LoadFromSocketStream();
    void LoadFromZipStream();
    void LoadFromFile();
    void SizeImage();

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
        wxLogWarning("No network connectivity; skipping the ImageTestCase::LoadFromSocketStream test unit.");
        return;
    }

    struct {
        const char* url;
        wxBitmapType type;
    } testData[] = 
    {
        { "http://wxwidgets.org/logo9.jpg", wxBITMAP_TYPE_JPEG },
        { "http://wxwidgets.org/favicon.ico", wxBITMAP_TYPE_ICO }
    };
    
    for (unsigned int i=0; i<WXSIZEOF(testData); i++)
    {
        wxURL url(testData[i].url);
        CPPUNIT_ASSERT(url.GetError() == wxURL_NOERR);

        wxInputStream *in_stream = url.GetInputStream();
        CPPUNIT_ASSERT(in_stream && in_stream->IsOk());

        wxImage img;
        
        // NOTE: it's important to inform wxImage about the type of the image being
        //       loaded otherwise it will try to autodetect the format, but that
        //       requires a seekable stream!
        CPPUNIT_ASSERT(img.LoadFile(*in_stream, testData[i].type));
        
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

       const unsigned data_len = 3 * expected.GetHeight() * expected.GetWidth();

       WX_ASSERT_MESSAGE
       (
         ("Resize test #%u: (%d, %d), (%d, %d)", i, st.w, st.h, st.dx, st.dy),
         memcmp(actual.GetData(), expected.GetData(), data_len) == 0
       );
   }
}


/*
    TODO: add lots of more tests to wxImage functions
*/
