///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/garbage.cpp
// Purpose:     test if loading garbage fails
// Author:      Francesco Montorsi
// Created:     2009-01-10
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Francesco Montorsi
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/filename.h"
#include "wx/image.h"
#include "wx/animate.h"
#include "wx/mstream.h"
#include "wx/dynlib.h"
#include "wx/mediactrl.h"
#include "wx/html/htmlwin.h"
#include "wx/xrc/xmlres.h"

#define GARBAGE_DATA_SIZE   1000000             // in bytes; ~ 1MB

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class GarbageTestCase : public CppUnit::TestCase
{
public:
    GarbageTestCase() { }

private:
    CPPUNIT_TEST_SUITE( GarbageTestCase );
        CPPUNIT_TEST( LoadGarbage );
    CPPUNIT_TEST_SUITE_END();

    void LoadGarbage();
    void DoLoadFile(const wxString& fullname);
    void DoLoadStream(wxInputStream& stream);

    DECLARE_NO_COPY_CLASS(GarbageTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( GarbageTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( GarbageTestCase, "GarbageTestCase" );


void GarbageTestCase::LoadGarbage()
{
    srand(1234);

    wxInitAllImageHandlers();

    for (size_t size = 1; size < GARBAGE_DATA_SIZE; size *= size+1)
    {
        // first, generate some garbage data
        unsigned char *data = new unsigned char[size];
        for (size_t i = 0; i < size; i++)
            data[i] = rand();

        // write it to a file
        wxString garbagename = wxFileName::CreateTempFileName("garbage");
        CPPUNIT_ASSERT( !garbagename.empty() );

        wxFile garbage(garbagename, wxFile::write);
        CPPUNIT_ASSERT( garbage.IsOpened() );

        CPPUNIT_ASSERT( garbage.Write(data, size) == size );
        garbage.Close();

        // try to load it by name
        DoLoadFile(garbagename);

        // try to load it from a wxInputStream
        wxMemoryInputStream stream(data, size);
        DoLoadStream(stream);

        wxDELETEA(data);
    }
}

void GarbageTestCase::DoLoadFile(const wxString& fullname)
{
    int type;

    // test wxImage
    wxImage img;
    CPPUNIT_ASSERT( img.LoadFile(fullname) == false );
        // test with the default wxBITMAP_TYPE_ANY

    for (type = wxBITMAP_TYPE_BMP; type < wxBITMAP_TYPE_ANY; type++)
        CPPUNIT_ASSERT( img.LoadFile(fullname, (wxBitmapType)type) == false );
            // test with all other possible wxBITMAP_TYPE_* flags

    // test wxBitmap
    wxBitmap bmp;
    CPPUNIT_ASSERT( bmp.LoadFile(fullname) == false );
        // test with the default wxBITMAP_TYPE_ANY

    for (type = wxBITMAP_TYPE_BMP; type < wxBITMAP_TYPE_ANY; type++)
        CPPUNIT_ASSERT( bmp.LoadFile(fullname, (wxBitmapType)type) == false );
            // test with all other possible wxBITMAP_TYPE_* flags

    // test wxIcon
    wxIcon icon;
    CPPUNIT_ASSERT( icon.LoadFile(fullname) == false );
        // test with the default wxICON_DEFAULT_TYPE

    for (type = wxBITMAP_TYPE_BMP; type < wxBITMAP_TYPE_ANY; type++)
        CPPUNIT_ASSERT( icon.LoadFile(fullname, (wxBitmapType)type) == false );
            // test with all other possible wxBITMAP_TYPE_* flags


    // test wxAnimation
    wxAnimation anim;
    CPPUNIT_ASSERT( anim.LoadFile(fullname) == false );
        // test with the default wxANIMATION_TYPE_ANY

    for (type = wxANIMATION_TYPE_INVALID+1; type < wxANIMATION_TYPE_ANY; type++)
        CPPUNIT_ASSERT( anim.LoadFile(fullname, (wxAnimationType)type) == false );
            // test with all other possible wxANIMATION_TYPE_* flags


    // test wxDynamicLibrary
    wxDynamicLibrary lib;
    CPPUNIT_ASSERT( lib.Load(fullname) == false );
        // test with the default wxANIMATION_TYPE_ANY
/*
#if wxUSE_MEDIACTRL
    // test wxMediaCtrl
    wxMediaCtrl *media = new wxMediaCtrl(wxTheApp->GetTopWindow());
    CPPUNIT_ASSERT( media->Load(fullname) == false );
#endif

    // test wxHtmlWindow
    wxHtmlWindow *htmlwin = new wxHtmlWindow(wxTheApp->GetTopWindow());
    CPPUNIT_ASSERT( htmlwin->LoadFile(fullname) == false );
    delete htmlwin;
*/
    // test wxXmlResource
    CPPUNIT_ASSERT( wxXmlResource::Get()->Load(fullname) == false );
}

void GarbageTestCase::DoLoadStream(wxInputStream& stream)
{
    int type;

    // NOTE: not all classes tested by DoLoadFile() supports loading
    //       from an input stream!

    // test wxImage
    wxImage img;
    CPPUNIT_ASSERT( img.LoadFile(stream) == false );
        // test with the default wxBITMAP_TYPE_ANY

    for (type = wxBITMAP_TYPE_INVALID+1; type < wxBITMAP_TYPE_ANY; type++)
        CPPUNIT_ASSERT( img.LoadFile(stream, (wxBitmapType)type) == false );
            // test with all other possible wxBITMAP_TYPE_* flags


    // test wxAnimation
    wxAnimation anim;
    CPPUNIT_ASSERT( anim.Load(stream) == false );
        // test with the default wxANIMATION_TYPE_ANY

    for (type = wxANIMATION_TYPE_INVALID+1; type < wxANIMATION_TYPE_ANY; type++)
        CPPUNIT_ASSERT( anim.Load(stream, (wxAnimationType)type) == false );
            // test with all other possible wxANIMATION_TYPE_* flags
/*
    // test wxHtmlWindow
    wxHtmlWindow *htmlwin = new wxHtmlWindow(wxTheApp->GetTopWindow());
    CPPUNIT_ASSERT( htmlwin->LoadFile(fullname) == false );
    delete htmlwin;
*/
}

