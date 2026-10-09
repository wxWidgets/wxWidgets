wxWidgets 3.2.12 Release Notes
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

* https://docs.wxwidgets.org/3.2.12/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://www.wxwidgets.org/downloads/

or, for a more more permanent but less convenient to use link, from

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.2.12/

Please see https://docs.wxwidgets.org/3.2.12/overview_install.html for full
installation instructions.



Changes since 3.2.11
--------------------

This is mostly a bug fix release, the only important addition is support for
wxTaskBarIcon under Wayland (#26845). Other changes are:

- Update Unicode mapping tables to current versions (#26700).
- Add wxPATH_RMDIR_PARENTS flag to wxFileName::Rmdir().
- Handle mouse capture loss when using wxContextHelp (#21534).
- Always recognize common file types in wxHTML (#20967).
- Respect background colour when printing in wxHTML (#20652).
- Fix handling of image map coordinates in wxHTML (#3163).
- Don't show modal HTML help window for not found topic (#3219).
- Recognize "Dstrok" as an entity in wxHTML (#3996).
- Fix XRC unknown-control size hints after attaching (#2726).
- Fix handling spacing in wxGridBagSizer (#3105).
- Fix handling TGA files with invalid size (#26760).
- Fix buffer overflow with invalid cached help books (#26765).
- Fix possible hang in wxWebRequestCURL after connection error (#27041).
- Fix using SetBitmap() on wxMenuItem created without a bitmap in wxGTK (#26771).
- Fix memory leak with using custom handlers in WebKit-based wxWebView (#26784).
- Use string passed to wxApp::SetClassName() as Wayland app-id in wxGTK (#24668).
- Fix wrong refresh in wxStaticBitmap::SetBitmap() in wxMSW (#26730).
- Fix drawing of 1x1 rectangles in wxMSW (#3096).
- Account for the column header in wxMSW wxListCtrl::HitTest() (#22239).
- Fix system menu corruption when Explorer is restarted (#26799).
- Fix crash toggling a toolbar item without a bitmap in wxOSX (#26763).

Upgrading to the new version is recommended for all existing 3.2 users.

Please see the full change log for more details:

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.2.12/docs/changes.txt

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
