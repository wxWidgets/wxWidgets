wxWidgets 3.2.11 Release Notes
=============================

Welcome to the new stable release of wxWidgets, a free and open source
cross-platform C++ framework for writing advanced GUI applications using
native controls.

wxWidgets allows you to write native-looking GUI applications for all the major
desktop platforms and also helps with abstracting the differences in the non-GUI
aspects between them. It is free for the use in both open source and commercial
applications, comes with the full, easy to read and modify, source and extensive
documentation and a collection of more than a hundred examples. You can learn
more about wxWidgets at:

* https://www.wxwidgets.org/

Documentation is available online at:

* https://docs.wxwidgets.org/3.2.11/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://www.wxwidgets.org/downloads/

or, for a more more permanent but less convenient to use link, from

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.2.11/

Please see https://docs.wxwidgets.org/3.2.11/overview_install.html for full
installation instructions.



Changes since 3.2.10
--------------------

This release contains a big number of important fixes, including for potential
security problems, and upgrading to it is strongly recommended for all
wxWidgets users.

The following bugs were fixed:

- Fix wxCondition and wxSemaphore::WaitTimeout() after 2038 under Unix (#26466).
- Fix buffer overflow in wxGethostby{addr,name}_r() (#26528).
- Fix buffer overflow in wxTarInputStream (#26530).
- Fix out-of-bounds read in wxFileType::ExpandCommand() (#26531).
- Fix buffer overflow in wxVsnprintf() (#26522).
- Fix out-of-bounds read in wxRegEx::Replace() (#26541).
- Fix out-of-bounds read in wxUString (#26548).
- Fix wxDateTime::Format() handling of invalid format (#26543).
- Fix buffer overflow in wxGethostbyxxx_r() on some systems (#26553).
- Fix wxCaret position in scrolled windows (#26282).
- Correct delta-RLE bitmap background colour (#23599).
- Correct loading of `BI_BITFIELDS` bitmaps (#23601).
- Fix loading 32bpp BMP files without valid alpha (#24219).
- Fix buffer overwrite when loading malformed BMPs with invalid RLE data.
- Avoid out-of-bounds palette read in 8bpp BMP decoder (#26438).
- Avoid out-of-bounds palette write in wxIFFDecoder (#26440).
- Avoid out-of-bounds read in wxPCXHandler (#26441).
- Avoid out-of-bounds read in wxXPMDecoder (#26442).
- Fix reading ANI images with invalid frame count (#26492).
- Avoid out-of-bounds write for invalid BMP RLE runs (#26496).
- Fix out-of-bounds read when loading invalid 4bpp BMPs (#26511).
- Avoid out-of-bounds write on bad BMHD chunk in wxIFFDecoder (#26497).
- Fix write overflow in TGA files with invalid colour map (#26493).
- Fix read overflow in wxXPMDecoder on unterminated quote (#26499).
- Fix read overflow in wxXPMDecoder on invalid width (#26519).
- Reject more invalid GIFs and do it without leaking memory (#26501).
- Fix buffer overflows decoding invalid GIFs (#26521, #26524).
- Fix multiple buffer overflows in wxIFFDecoder (#26505, #26518).
- Fix multiple buffer overflows in wxSound (#26506, #26525).
- Fix reading too short extra field in ZIP64 files (#26507).
- Fix out-of-bounds table read in wxMBConvUTF7::ToWChar(#26517).
- Fix buffer overflow reading corrupted message catalogs (#26513).
- Fix buffer overflow for too big tables in wxHTML (#26554).
- Add wxGrid::GetFrozen{Row,Col}LabelWindow() (#26617).
- Fix wxPropertyGrid macros with wxNO_IMPLICIT_WXSTRING_ENCODING (#26651).
- Fix copy/paste in wxGTK when a clipboard manager is running (#26265).
- Fix crash with EGL-based wxGLCanvas in wxNotebook (#26340).
- Fix possible wxTextCtrl crash with GSpell attached (#26464).
- Fix drawing of very large bitmaps in wxGTK (#25656).
- Fix memory leak in wxDataViewChoiceRenderer.
- Fix resource leak when using EGL with X11 (#26341).
- Fix showing title bar under Wayland with wxBORDER_NONE (#26357).
- Remove maximum text length limitation in "picker" controls in wxGTK (#26314).
- Fix appearance of dashed lines drawn by wxDC::DrawLine() in wxGTK (#26449).
- Fix non-resizable TLW size when setting client size in wxGTK (#26480).
- Fix wxBufferedPaintDC when using RTL layout in wxMSW (#26266).
- Fix crash after destroying wxFileSystemWatcher in wxOSX (#26658).
- Fix possible crash in wxFont::SetNativeFontInfo() in wxOSX (#26411).

Please see the full change log for more details:

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.2.11/docs/changes.txt

This release is API and ABI-compatible with the previous 3.2.x releases, so
the existing applications don't even need to be rebuilt to profit from all the
fixes above if they use shared/dynamic libraries. And if they do need to be
recompiled, this can be done without any changes to the code.



Supported Platforms
-------------------

This version of wxWidgets supports the following primary platforms:

* Windows XP, Vista, 7, 8, 10 and 11 (32/64 bits).
* Most Unix variants using the GTK toolkit (version 2.6 or newer or 3.x)
* macOS (10.10 or newer) using Cocoa (x86-64 or ARM).

There is some support for the following platforms:

* Most Unix variants with X11
* Most Unix variants with Motif/Lesstif
* Most Unix variants with GTK+ 1.2
* Most Unix variants with Qt 5 or newer (experimental)

All C++ compilers in common use are supported.


Licence
-------

For licensing information, please see the files:

* docs/preamble.txt
* docs/licence.txt
* docs/licendoc.txt
* docs/gpl.txt
* docs/lgpl.txt
* docs/xserver.txt

Although this may seem complex, it is there to allow authors of proprietary,
commercial applications to use wxWidgets in addition to those writing GPL'ed
applications. In summary, the licence is LGPL plus a clause allowing
unrestricted distribution of application binaries. To answer a FAQ, you don't
have to distribute any source if you wish to write commercial applications using
wxWidgets.

However, if you distribute wxGTK, wxQt or wxMotif (with Lesstif) version of your
application, don't forget that it is linked against GTK+, Qt or Lesstif, which
are covered by LGPL *without* exception notice and so is bound by its
requirements.

If you use TIFF image handler, please see src/tiff/COPYRIGHT for libtiff licence
details.

If you use JPEG image handler, documentation for your program should contain
following sentence: "This software is based in part on the work of the
Independent JPEG Group". See src/jpeg/README for details.

If you use wxRegEx class, please see 3rdparty/pcre/LICENCE for PCRE licence
details.

If you use wxXML classes or XRC, see src/expat/COPYING for licence details.


Reporting Bugs
--------------

The wxWidgets bug tracker can be found here:

* https://github.com/wxWidgets/wxWidgets/issues/

Please use the search function find any possibly relevant bugs before reporting
new ones. Also please notice that often trying to correct the bug yourself is
the quickest way to fix it. Even if you fail to do it, you may discover
valuable information allowing us to fix it while doing it. We also give much
higher priority to bug reports with patches fixing the problems so this ensures
that your report will be addressed sooner.


Further Information
-------------------

If you are looking for community support, you can get it from

* Mailing Lists: https://www.wxwidgets.org/support/mailing-lists/
* Discussion Forums: https://forums.wxwidgets.org/
* #wxwidgets IRC Channel: https://www.wxwidgets.org/support/irc/
* Stack Overflow (tag your questions with "wxwidgets"):
  https://stackoverflow.com/questions/tagged/wxwidgets

Commercial support is also available, please see
https://www.wxwidgets.org/support/commercial/

Finally, keep in mind that wxWidgets is an open source project collaboratively
developed by its users and your contributions to it are always welcome!


Have fun!

The wxWidgets Team, March 2026
