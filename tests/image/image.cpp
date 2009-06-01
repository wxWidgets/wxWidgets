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
    CPPUNIT_TEST_SUITE_END();

    void LoadFromSocketStream();
    void LoadFromZipStream();
    void LoadFromFile();

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

/*
    TODO: add lots of more tests to wxImage functions
*/
