How to add new bitmaps to wxWidgets UI elements
===============================================

0. Introduction
---------------

Since the introduction of wxArtProvider class, it is no longer desired to
hardcode art resources (e.g. icons and toolbar or button bitmaps) into the
code. This was previously done either by including the bitmap in win32
resource file (include/wx/msw/wx.rc) or by including XPM files in the code.

wxArtProvider should be used instead, to allow users to customize the look of
their wxWidgets app. This technote is a detailed description of steps needed
when adding new bitmap/icon.

1. Adding new resource
----------------------

(Please see wxArtProvider reference documentation for explanation of "art ID"
 and "art client" terms.)

First of all, you have to add new wxArtID constant to include/wx/artprov.h.
Look for "Art IDs" and add new definition to the list, e.g.

    #define wxART_MY_BITMAP     wxART_MAKE_ART_ID(wxART_MY_BITMAP)

Add it to interface/wx/artprov.h, too.

It may happen that the intended use of the new resource doesn't fit into any
of defined client categories (search for "Art clients" in the header). In case
the new resource is part of a larger category, you need to define a new
client. Just add it to the list of existing clients (and don't forget to
update artprov.tex):

    #define wxART_MY_CLIENT wxART_MAKE_CLIENT_ID(wxART_MY_CLIENT)

Alternatively, you may use wxART_OTHER when accessing the resource if the
bitmap is standalone.

Once the header is updated, it's time to add XPM file with the bitmap to
$(wx)/art. Add it to $(wx)/art if it is platform-independent or to
$(wx)/art/$(toolkit) if it is something specific to one of the toolkits. Note
that "specific to one of the toolkits" doesn't mean that the bitmap is *used*
by only one toolkit, but that it doesn't make sense for any of the others! For
example, a GTK wxART_WARNING icon ($(wx)/art/gtk/warning.xpm) is specific to
wxGTK, but new_dir.xpm makes sense even under wxMSW even though it is
currently only used by the generic file dialog. Remember that wxArtProvider
can be used by users, not only the library.

Finally, wxDefaultArtProvider in $(wx)/src/common/artstd.cpp must be updated.
This consists of two steps:

  - add #include line for your XPM file, e.g. #include "../../art/my_bmp.xpm"
  - add ART(...) line to wxDefaultArtProvider::CreateBitmap(). The first
     argument is wxArtID, the other is XPM file name (w/o extension), e.g.
     ART(wxART_MY_BITMAP, my_bmp)

That's all. The bitmap is now available to wxArtProvider users.

Note: there's no difference between icons and bitmaps, always treat them as
bitmaps inside wx(Default)ArtProvider.

1b. Adding Tango version of the resource.
-----------------------------------------

While all the bitmaps are provided in XPM format so that they are available in
all builds of wxWidgets, we also provide most of them in PNG format with full
transparency support that is not available in XPM. Another advantage of the PNG
versions is that the icons used are those of the Tango project and so have the
consistent look, unlike the XPM ones.

So if you an icon exists in http://tango.freedesktop.org/Tango_Icon_Gallery you
should add it too. For this you need to:

1. Convert the PNG to a C array of bytes suitable for inclusion in the code.
   This is done using misc/scripts/png2c.py script, e.g. if the variable "f"
   contains the name of the icon you want to add and you have installed Tango
   icons in a standard location under a Linux system:

    ./misc/scripts/png2c.py -s /usr/share/icons/Tango/{16x16,24x24}/*/$f.png >
        art/tango/${f//-/_}.h

   Of course, the same command may be ran with different paths under Windows.
   Just remember to add both 16 and 24 pixel versions of the bitmap to the
   header and use the "-s" option to embed the image size in its array name.

2. Add #include for the newly created file to src/common/arttango.cpp.

3. Add an entry to s_allBitmaps array in the same file.


2. Accessing the resource
-------------------------

The file that will use the bitmap needs to include "wx/artprov.h". The code to
access the bitmap (or icon) is trivial:

    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_MY_BITMAP, wxART_MY_CLIENT);
        // this would be "wxBitmap bmp(my_bmp_xpm);" before
    wxIcon icon = wxArtProvider::GetIcon(wxART_MY_ICON, wxART_MY_CLIENT);

Substitute wxART_MY_CLIENT in the example with a suitable client ID. If the
client is wxART_OTHER you may write only

    wxArtProvider::GetBitmap(wxART_MY_BITMAP).

3. Providing a demo
-------------------

It is highly desirable to let the users know what stock bitmaps are available
in wxWidgets. The "artprov" sample serves this purpose: it contains a browser
dialog that displays all available art resources.

It has to be updated to accommodate for new bitmaps. Fortunately, this is
trivial: open $(wx)/samples/artprov/artbrows.cpp in text editor and
`ART_ICON(wxART_MY_BITMAP)` line to the FillBitmaps() function.

Similarly, if you add a new client, please update FillClients() by adding new
client to the end of the list.
