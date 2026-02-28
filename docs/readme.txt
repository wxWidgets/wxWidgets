wxWidgets 3.2.10 Release Notes
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

* https://docs.wxwidgets.org/3.2.10/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://www.wxwidgets.org/downloads/

or, for a more more permanent but less convenient to use link, from

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.2.10/

Please see https://docs.wxwidgets.org/3.2.10/overview_install.html for full
installation instructions.



Changes since 3.2.9
-------------------

This is a minor bug-fix release with just a single new addition: wxWidgets now
includes project files for Microsoft Visual Studio 2026.

It does contain a number of important bug fixes for macOS port of wxWidgets,
notably:

- Fix install names of the libraries when using "make install" (#25675).
- Fix crash on some key presses in wxDataViewCtrl (#26160).
- Fix memory leaks in a number of controls (#26208).
- Fix regression in wxFileDialog filters in 3.2.9 (#26148).
- Many fixes for macOS 26 Tahoe (#26058, #25767, #26121, #26095).

Some other bugs fixed in this release:

- Don't use first image for wxListCtrl items without images in wxMSW (#26062).
- Fix regression in wxStaticBitmap::SetBitmap() in wxMSW 3.2.9 (#26106).
- Handle font names longer than 31 characters in wxMSW (moi15moi, #25333).
- Fix right/middle click events in vertical wxAuiToolBar (#26242).

Please see the full change log for more details:

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.2.10/docs/changes.txt

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
