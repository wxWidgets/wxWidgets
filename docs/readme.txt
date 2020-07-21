wxWidgets 3.1.4 Release Notes
=============================

Welcome to the latest development release of wxWidgets, a free and open source
cross-platform C++ framework for writing advanced GUI applications using native
controls.

wxWidgets allows you to write native-looking GUI applications for all the major
desktop platforms and also helps with abstracting the differences in the non-GUI
aspects between them. It is free for the use in both open source and commercial
applications, comes with the full, easy to read and modify, source and extensive
documentation and a collection of more than a hundred examples. You can learn
more about wxWidgets at:

* https://www.wxwidgets.org/

Documentation is available online at:

* https://docs.wxwidgets.org/3.1.4/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://www.wxwidgets.org/downloads/

or, for a more more permanent but less convenient to use link, from

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.1.4/


Changes since 3.1.3
-------------------

In the nine months since the previous release, there have been almost 1700
commits from 74 unique contributors (37 with multiple contributions), so it is
impossible to summarize all the changes in this document without making it too
long, please see the fuller changelog at

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.1.4/docs/changes.txt

for more (but still not all) details, but some of the most important changes are:

- wxOSX port now supports the latest macOS 11 on ARM hardware.
- wxMSW comes with a new Edge-based wxWebView implementation.
- Better, although still not perfect, per-monitor DPI support in wxMSW.
- wxGrid was has been significantly improved and is now much user friendlier.
- AUI appearance enhancements for non-default GTK themes and macOS dark mode.
- wxWidgets may now be built with C++20 compiler.
- Improvements to CMake build system.
- Allow optionally forbidding dangerous implicit wxString conversions.

And, of course, there were many, many bug fixes in all ports, notably including
fixing several long-standing problems in wxOSX.

Notice that in spite of all the changes, this release remains almost completely
compatible with 3.1.3 at the API level, so upgrading to it if you're already
using wxWidgets 3 should be straightforward.


Changes since 3.0
-----------------

Compared to the stable 3.0.x series, this version brings too many
improvements and even more bug fixes to list them all, but here is the
maximally condensed summary:

- Build system improvements: support for new compilers (up to MSVS 2019, g++ 10)
  with an even simpler way of using wxWidgets from MSVS, with wxwidgets.props
  file, and OS versions as well as an entirely new CMake build system.
- Support for native dark mode under macOS 10.14 Mojave and later.
- New features: support for mouse gesture events (GSoC 2017 project);
  fractional pen widths in wxGraphicsContext; arbitrary label windows in
  wxStaticBox; markup in wxDataViewCtrl items text; better support for high DPI
  monitors; support for ZIP 64 files; much improved accessibility support under
  MSW; Support for non-integer font sizes and arbitrary font weights; LZMA
  compression support (using liblzma).
- New classes: wxActivityIndicator, wxAddRemoveCtrl,
  wxAppProgressIndicator, wxNativeWindow, wxPowerResourceBlocker,
  wxSecretStore.
- And methods: wxDateTime::GetWeekBasedYear(), wxListBox::GetTopItem(),
  wxProcess::Activate(), wxTextEntry::ForceUpper(), several ones in
  wxRendererNative, wxStandardPaths::GetUserDir(), wxUIActionSimulator
  ::Select() and many others.
- Significant improvements to: wxBusyInfo, wxDataViewCtrl,
  wxNotificationMessage, wxStaticBox, wxStyledTextCtrl.
- Latest versions of all bundled 3rd party libraries, including all the
  security fixed and support for WebKit 2 and GStreamer 1.7 under Unix.
- Revamped OpenGL support better suited to modern OpenGL (3.2+).
- Further C++11 support improvements.
- A lot of bug fixes, especially in wxGTK3 and wxOSX/Cocoa ports.
- New experimental wxQt port.



Platforms Supported
-------------------

This version of wxWidgets supports the following primary platforms:

* Windows XP, Vista, 7, 8 and 10 (32/64 bits).
* Most Unix variants using the GTK+ toolkit (version 2.6 or newer)
* macOS (10.10 or newer) using Cocoa (32/64 bits)

There is some support for the following platforms:

* Most Unix variants with X11
* Most Unix variants with Motif/Lesstif
* Most Unix variants with GTK+ 1.2
* Most Unix variants with Qt 5 or newer (experimental)

All C++ compilers in common use are supported; see the install.txt file for
each platform (following the links from docs/index.htm).


Files
-----

wxWidgets is distributed in source form in several archive formats. ZIP and 7z
archives are for Microsoft Windows users and contain the files with DOS/Windows
line endings while the compressed tar archives are for Unix systems (including
macOS) and contain the files with Unix line endings. Please notice that some
Windows tools still don't accept files with Unix line endings and that compiling
sources with DOS line endings under Unix will fail, so please choose the correct
file for your system.

In addition to the sources, documentation in HTML, CHM and HTB (wxWidgets help
viewer) formats is provided as well as an installer for Microsoft Windows.
Notice that you will still need to compile wxWidgets even when using the
installer.

We also supply binaries of wxMSW libraries built with several versions of
Microsoft Visual C++ and GNU g++ compiler for this release.


Installation
------------

Unless you have downloaded the binaries for your compiler, you will need to
build wxWidgets before you can test out the samples or write your own
applications. For installation information, please see the install.md files in
the docs subdirectory appropriate for the platform you use or the "Platform
Details" page of the manual, which contains links to the rendered versions of
these files.


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

If you use wxRegEx class on a system without native regular expressions support
(i.e. MS Windows), see src/regex/COPYRIGHT file for Henry Spencer's regular
expression library copyright.

If you use wxXML classes or XRC, see src/expat/COPYING for licence details.


Reporting Bugs
--------------

The wxWidgets bug tracker can be found here:

* https://trac.wxwidgets.org/newticket

Please use the search function of our Trac installation to find any possibly
relevant bugs before reporting new ones. Also please notice that often trying to
correct the bug yourself is the quickest way to fix it. Even if you fail to do
it, you may discover valuable information allowing us to fix it while doing it.
We also give much higher priority to bug reports with patches fixing the
problems so this ensures that your report will be addressed sooner.


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

The wxWidgets Team, July 2020
