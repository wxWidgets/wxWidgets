///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/garbage.cpp
// Purpose:     test if loading garbage fails
// Author:      Francesco Montorsi
// Created:     2009-01-10
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
#include "wx/icon.h"
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

    wxDECLARE_NO_COPY_CLASS(GarbageTestCase);
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

// Execute the given macro with the given first and second parameters and
// bitmap type as its third parameter for all bitmap types.
#define wxFOR_ALL_VALID_BITMAP_TYPES(m, p1, p2) \
    for ( wxBitmapType type = wxBitmapType(wxBITMAP_TYPE_INVALID + 1); \
          type < wxBITMAP_TYPE_MAX; \
          type = (wxBitmapType)(type + 1) ) \
        m(p1, p2, type)

// Similar to above but for animation types.
#define wxFOR_ALL_VALID_ANIMATION_TYPES(m, p1, p2) \
    for ( wxAnimationType type = wxAnimationType(wxANIMATION_TYPE_INVALID + 1); \
          type < wxANIMATION_TYPE_ANY; \
          type = (wxAnimationType)(type + 1) ) \
        m(p1, p2, type)

// This macro is used as an argument to wxFOR_ALL_VALID_BITMAP_TYPES() to
// include the information about the type for which the test failed.
#define ASSERT_FUNC_FAILS_FOR_TYPE(func, arg, type) \
    WX_ASSERT_MESSAGE \
    ( \
      (#func "() unexpectedly succeeded for type %d", type), \
      !func(arg) \
    )

// And this one exists mostly just for consistency with the one above.
#define ASSERT_FUNC_FAILS(func, arg) \
    WX_ASSERT_MESSAGE \
    ( \
      (#func "() unexpectedly succeeded for default type"), \
      !func(arg) \
    )

void GarbageTestCase::DoLoadFile(const wxString& fullname)
{
    // test wxImage
    wxImage img;
    ASSERT_FUNC_FAILS(img.LoadFile, fullname);
    wxFOR_ALL_VALID_BITMAP_TYPES(ASSERT_FUNC_FAILS_FOR_TYPE, img.LoadFile, fullname);

    // test wxBitmap
    wxBitmap bmp;
    ASSERT_FUNC_FAILS(bmp.LoadFile, fullname);
    wxFOR_ALL_VALID_BITMAP_TYPES(ASSERT_FUNC_FAILS_FOR_TYPE, bmp.LoadFile, fullname);

    // test wxIcon
    wxIcon icon;
    ASSERT_FUNC_FAILS(icon.LoadFile, fullname);
    wxFOR_ALL_VALID_BITMAP_TYPES(ASSERT_FUNC_FAILS_FOR_TYPE, icon.LoadFile, fullname);

#if wxUSE_ANIMATIONCTRL
    // test wxAnimation
    wxAnimation anim;
    ASSERT_FUNC_FAILS(anim.LoadFile, fullname);
    wxFOR_ALL_VALID_ANIMATION_TYPES(ASSERT_FUNC_FAILS_FOR_TYPE, anim.LoadFile, fullname);
#endif

    // test wxDynamicLibrary
    wxDynamicLibrary lib;
    CPPUNIT_ASSERT( lib.Load(fullname) == false );

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
#if wxUSE_XRC
    CPPUNIT_ASSERT( wxXmlResource::Get()->Load(fullname) == false );
#endif
}

void GarbageTestCase::DoLoadStream(wxInputStream& stream)
{
    // NOTE: not all classes tested by DoLoadFile() supports loading
    //       from an input stream!

    // test wxImage
    wxImage img;
    ASSERT_FUNC_FAILS(img.LoadFile, stream);
    wxFOR_ALL_VALID_BITMAP_TYPES(ASSERT_FUNC_FAILS_FOR_TYPE, img.LoadFile, stream);

#if wxUSE_ANIMATIONCTRL
    // test wxAnimation
    wxAnimation anim;
    ASSERT_FUNC_FAILS(anim.Load, stream);
    wxFOR_ALL_VALID_BITMAP_TYPES(ASSERT_FUNC_FAILS_FOR_TYPE, anim.Load, stream);
#endif
/*
    // test wxHtmlWindow
    wxHtmlWindow *htmlwin = new wxHtmlWindow(wxTheApp->GetTopWindow());
    CPPUNIT_ASSERT( htmlwin->LoadFile(fullname) == false );
    delete htmlwin;
*/
}

#undef ASSERT_FUNC_FAILS
#undef ASSERT_FUNC_FAILS_FOR_TYPE
#undef wxFOR_ALL_VALID_ANIMATION_TYPES
#undef wxFOR_ALL_VALID_BITMAP_TYPES
