/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_bitmap Bitmaps and Icons

@tableofcontents

The wxBitmap class encapsulates the concept of a platform-dependent bitmap,
either monochrome or colour. Platform-specific methods for creating a wxBitmap
object from an existing file are catered for, and this is an occasion where
conditional compilation will sometimes be required.

A bitmap created dynamically or loaded from a file can be selected into a
memory device context (instance of wxMemoryDC). This enables the bitmap to be
copied to a window or memory device context using wxDC::Blit(), or to be used
as a drawing surface.

See wxMemoryDC for an example of drawing onto a bitmap.


@section overview_bitmap_embedding Including Bitmaps in Your Program

It is common to need to embed some small bitmaps to be used for bitmaps or
icons in the program itself, instead of loading them from external files as
would be done for larger images. wxWidgets provides a few helper macros hiding
the differences between the platforms for doing this: wxBITMAP_PNG(), which
loads images from embedded PNG files and resources, or wxICON() and wxBITMAP()
which load images from a Windows resource of the corresponding type or from
XPM file included in the program.

wxBITMAP_PNG() is generally better because PNG images support full alpha
channel, unlike the XPM ones, but requires converting PNG files to a C array
before including it into your program, see its documentation. If you do not
need alpha channel support, XPM images, which can be directly included into the
program are simpler to use as you only need to do the following:

@code
#include "sample.xpm"

MyFrame::MyFrame()
{
    wxIcon icon(wxICON(sample));
}
@endcode

which is equivalent to

@code
#include "sample.xpm"

MyFrame::MyFrame()
{
#if defined(__WXGTK__)
    wxIcon icon(sample_xpm);
#endif

// ... similar checks for other platforms ...

#if defined(__WXMSW__)
    wxIcon icon("sample"); // loaded from Windows resources (.rc file)
#endif
}
@endcode

but is, of course, much simpler and more clear. There is also a corresponding
wxBITMAP() macro which works in the same way but
creates a wxBitmap rather than wxIcon.

Note that including XPM files under Windows is harmless, but not really
necessary, as they are not used there and can be avoided by testing for
`wxHAS_IMAGES_IN_RESOURCES` symbol, i.e. you would typically do

@code
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "bitmaps/north.xpm"
    #include "bitmaps/south.xpm"
    #include "bitmaps/east.xpm"
    #include "bitmaps/west.xpm"
#endif

MyFrame::MyFrame()
{
    wxBitmap bmpN = wxBITMAP(north);
    ...
}
@endcode

Finally note that you may also want to use XPMs under all platforms, including
Windows, for simplicity, and to avoid having to deal with Windows resource
files (however you still need to define the application icon there). In this
case you should _not_ use wxICON() and wxBITMAP() macros as they expect to find
the images in Windows resources when building Windows programs.

@see @ref group_class_gdi



@section overview_bitmap_supportedformats Supported Bitmap File Formats

The following lists the formats handled on different platforms. Note that
missing or partially-implemented formats are automatically supplemented by
using wxImage to load the data, and then converting it to wxBitmap form. Note
that using wxImage is the preferred way to load images in wxWidgets, with the
exception of resources (XPM-files or native Windows resources).

Writing an image format handler for wxImage is also far easier than writing one
for wxBitmap, because wxImage has exactly one format on all platforms whereas
wxBitmap can store pixel data very differently, depending on colour depths and
platform.

@subsection overview_bitmap_supportedformats_bmp wxBitmap

Under Windows, wxBitmap may load the following formats:

@li Windows bitmap resource (wxBITMAP_TYPE_BMP_RESOURCE)
@li Windows bitmap file (wxBITMAP_TYPE_BMP)
@li XPM data and file (wxBITMAP_TYPE_XPM)
@li All formats that are supported by the wxImage class.

Under wxGTK, wxBitmap may load the following formats:

@li XPM data and file (wxBITMAP_TYPE_XPM)
@li All formats that are supported by the wxImage class.

Under wxX11, wxBitmap may load the following formats:

@li XBM data and file (wxBITMAP_TYPE_XBM)
@li XPM data and file (wxBITMAP_TYPE_XPM)
@li All formats that are supported by the wxImage class.

@subsection overview_bitmap_supportedformats_icon wxIcon

Under Windows, wxIcon may load the following formats:

@li Windows icon resource (wxBITMAP_TYPE_ICO_RESOURCE)
@li Windows icon file (wxBITMAP_TYPE_ICO)
@li XPM data and file (wxBITMAP_TYPE_XPM).

Under wxGTK, wxIcon may load the following formats:

@li XPM data and file (wxBITMAP_TYPE_XPM)
@li All formats that are supported by the wxImage class.

Under wxX11, wxIcon may load the following formats:

@li XBM data and file (wxBITMAP_TYPE_XBM)
@li XPM data and file (wxBITMAP_TYPE_XPM)
@li All formats that are supported by the wxImage class.

@subsection overview_bitmap_supportedformats_cursor wxCursor

Under Windows, wxCursor may load the following formats:

@li Windows cursor resource (wxBITMAP_TYPE_CUR_RESOURCE)
@li Windows cursor file (wxBITMAP_TYPE_CUR)
@li Windows icon file (wxBITMAP_TYPE_ICO)
@li Windows bitmap file (wxBITMAP_TYPE_BMP).

Under wxGTK, wxCursor may load the following formats (in addition to stock
cursors):

@li None (stock cursors only).

Under wxX11, wxCursor may load the following formats:

@li XBM data and file (wxBITMAP_TYPE_XBM).


@section overview_bitmap_handlers Bitmap Format Handlers

To provide extensibility, the functionality for loading and saving bitmap
formats is not implemented in the wxBitmap class, but in a number of handler
classes, derived from wxBitmapHandler. There is a static list of handlers which
wxBitmap examines when a file load/save operation is requested.

Some handlers are provided as standard, but if you have special requirements,
you may wish to initialise the wxBitmap class with some extra handlers which
you write yourself or receive from a third party.

To add a handler object to wxBitmap, your application needs to include the
header which implements it, and then call the static function
wxBitmap::AddHandler().

@note Bitmap handlers are not implemented on all platforms, and new ones rarely
need to be implemented since wxImage can be used for loading most formats, as
noted earlier.

*/

