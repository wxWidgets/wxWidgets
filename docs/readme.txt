wxWidgets 3.2.0 Release Notes
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

* https://docs.wxwidgets.org/3.2.0/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://www.wxwidgets.org/downloads/

or, for a more more permanent but less convenient to use link, from

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.2.0/

Please see https://docs.wxwidgets.org/3.2.0/overview_install.html for full
installation instructions.



Changes since 3.0
-----------------

This release is a culmination of many years of development since the
last stable 3.0 release. It contains more than 15,000 commits from more
than 400 unique contributors (including more than 200 with multiple
contributions) and has too many improvements to list them all here, but
here is a maximally condensed summary of the most important ones:

- Build system improvements: support for new compilers (up to MSVS 2022,
  g++ 12, clang 14) with an even simpler way of using wxWidgets from
  MSVS, with wxwidgets.props file, and OS versions as well as an
  entirely new CMake build system.
- Support for native dark mode under macOS 10.14 and later, support for
  ARM hardware and macOS versions up to 13.
- High DPI support with the new, but almost perfectly backwards-
  compatible, API based on wxBitmapBundle, including per-monitor DPI and
  dynamic DPI changes.
- New features: HTTPS and HTTP/2 support with wxWebRequest and friends;
  support for freezing rows/columns in wxGrid; mouse gesture events
  (GSoC 2017 project); non-integer font sizes and arbitrary font weights
  in wxFont; fractional pen widths in wxGraphicsContext; arbitrary label
  windows in wxStaticBox; markup in wxDataViewCtrl items text; support
  for ZIP 64 files; LZMA compression; much improved accessibility
  support under MSW; new Edge-based wxWebView implementation.
- New classes: wxActivityIndicator, wxAddRemoveCtrl, wxAppProgressIndicator,
  wxBitmapBundle, wxNativeWindow, wxPersistentComboBox,
  wxPowerResourceBlocker, wxSecretStore, wxTempFFile and many new
  features in the existing classes.
- New XRC handlers for all the new and some of the existing classes.
- Significant improvements to: wxBusyInfo, wxDataViewCtrl, wxDirDialog, wxGrid,
  wxNotificationMessage, wxSpinCtrl, wxStaticBox, wxStyledTextCtrl,
  wxUIActionSimulator.
- Improvements to compile-time safety with the possibility to disable dangerous
  implicit conversions between wxString and "char*" strings.
- Latest versions of all bundled 3rd party libraries, including all the
  security fixes and support for WebKit 2 and GStreamer 1.7 under Unix.
- Revamped OpenGL support better suited to modern OpenGL (3.2+).
- Further C++11 and later support improvements, wxWidgets can be built using
  C++20 compilers.
- New experimental wxQt port.
- Many, many bug fixes.


Note that in spite of all these changes, wxWidgets 3.2 is almost fully
compatible with wxWidgets 3.0 and updating the existing applications to
use it shouldn't require much effort.


Supported Platforms
-------------------

This version of wxWidgets supports the following primary platforms:

* Windows XP, Vista, 7, 8, 10 and 11 (32/64 bits).
* Most Unix variants using the GTK+ toolkit (version 2.6 or newer)
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

The wxWidgets Team, July 2022
