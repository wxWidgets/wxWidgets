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

#include "testfile.h"


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

void DoLoadGarbageFile(const wxString& fullname)
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
    CHECK_FALSE( lib.Load(fullname) );

/*
#if wxUSE_MEDIACTRL
    // test wxMediaCtrl
    wxMediaCtrl *media = new wxMediaCtrl(wxTheApp->GetTopWindow());
    CHECK_FALSE( media->Load(fullname) );
#endif

    // test wxHtmlWindow
    wxHtmlWindow *htmlwin = new wxHtmlWindow(wxTheApp->GetTopWindow());
    CHECK_FALSE( htmlwin->LoadFile(fullname) );
    delete htmlwin;
*/
    // test wxXmlResource
#if wxUSE_XRC
    CHECK_FALSE( wxXmlResource::Get()->Load(fullname) );
#endif
}

void DoLoadGarbageStream(wxInputStream& stream)
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
    CHECK_FALSE( htmlwin->LoadFile(fullname) );
    delete htmlwin;
*/
}

TEST_CASE("Load", "[garbage]")
{
    srand(1234);

    wxInitAllImageHandlers();

    for (size_t size = 1; size < GARBAGE_DATA_SIZE; size *= size + 1)
    {
        // first, generate some garbage data
        std::vector<unsigned char> buffer(size);
        unsigned char* data = &buffer[0];
        for (size_t i = 0; i < size; i++)
            data[i] = rand();

        // write it to a file
        TestFile tf(data, size);

        // try to load it by name
        DoLoadGarbageFile(tf.GetName());

        // try to load it from a wxInputStream
        wxMemoryInputStream stream(data, size);
        DoLoadGarbageStream(stream);
    }
}

#undef ASSERT_FUNC_FAILS
#undef ASSERT_FUNC_FAILS_FOR_TYPE
#undef wxFOR_ALL_VALID_ANIMATION_TYPES
#undef wxFOR_ALL_VALID_BITMAP_TYPES
