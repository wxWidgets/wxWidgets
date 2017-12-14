wxWidgets 3.1.1 Release Notes
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

* http://docs.wxwidgets.org/3.1.1/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.1.1/


Changes since 3.1.0
-------------------

There have been more than 2000 commits from more than 130 contributors (70 with
multiple contributions) since 3.1.0 release. New features added since then
include:

- wxWebView can now return JavaScript results to the C++ code.
- wxSecretStore allows to securely store user passwords.

Some of the other improvements:

- wxWidgets can now be built with CMake too.
- Strings can now be translated differently depending on their context.
- Converting between wxString and UTF-8 encoded std::string is now
  simpler and unsafe wxString can now be disabled on the opt-in basis
  (see http://wxwidgets.blogspot.com/2017/02/safer-s.html)
- Many improvements to accessibility support under MSW.
- Support for XDG file layout under Unix.
- Many bug fixes to the appearances in both wxGTK3 and wxOSX, notably
  related to borders (notable wxBORDER_NONE) and colours.
- wxDataViewCtrl items and headers can be formatted using simple markup
  and it is simpler to combine to put items with checkboxes into it.
- Several enhancements to wxStyledTextCtrl including better support for
  custom lexers and auto-completion.

Additionally, the latest versions of compilers (e.g. MSVS 2017) and
operating systems (macOS 10.12) are now supported and all the third
party libraries have been updated to their latest versions.

Please refer to the detailed change log for the full list of changes:

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.1.1/docs/changes.txt


Changes since 3.0
-----------------

Compared to the stable 3.0.x series, this version brings too many
improvements and even more bug fixes to list them them all. Here is the
maximally condensed summary:

- New classes: wxActivityIndicator, wxAddRemoveCtrl,
  wxAppProgressIndicator, wxNativeWindow, wxPowerResourceBlocker,
  wxSecretStore.
- And methods: wxDateTime::GetWeekBasedYear() and GetFirstWeekDay(),
  GetTopItem() and GetCountPerPage() in wxListBox and wxDataViewCtrl,
  wxProcess::Activate(), wxTextEntry::ForceUpper(), several ones in
  wxRendererNative, wxStandardPaths::GetUserDir() and SetFileLayout(),
  wxUIActionSimulator::Select(), wxFontPickerCtrl::SetMinPointSize() and
  many others.
- New events: wxEVT_MAGNIFY, wxEVT_STC_AUTOCOMP_COMPLETED.
- Significant improvements to: wxBusyInfo, wxNotificationMessage.
- All around better support for high DPI monitors.
- Much newer versions of bundled 3rd party libraries (notably libpng)
  and support for GStreamer up to 1.7 under Unix.
- Revamped OpenGL support better suited to modern OpenGL (3.2+).
- Further C++11 support improvements.
- Support for latest compilers: MSVS 2017, g++ 7.2, clang 5.0.
- A lot of bug fixes in wxGTK3 and wxOSX/Cocoa ports.
- New experimental wxQt port.



Platforms Supported
-------------------

wxWidgets currently supports the following primary platforms:

* Windows XP, Vista, 7, 8 and 10 (32/64 bits).
* Most Unix variants using the GTK+ toolkit (version 2.6 or newer)
* OS X (10.7 or newer) using Cocoa (32/64 bits)

There is some support for the following platforms:

* Most Unix variants with X11
* Most Unix variants with Motif/Lesstif
* Most Unix variants with GTK+ 1.2
* Most Unix variants with Qt 5 or newer (experimental)

Most popular C++ compilers are supported; see the install.txt file for each
platform (following the links from docs/index.htm).


Files
-----

wxWidgets is distributed in source form in several archive formats. ZIP and 7z
archives are for Microsoft Windows users and contain the files with DOS/Windows
line endings while the compressed tar archives are for Unix systems (including
OS X) and contain the files with Unix line endings. Please notice that some
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
applications. For installation information, please see the install.txt file in
the docs subdirectory appropriate for the platform you use.


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

If you are looking for support, you can get it from

* Mailing Lists: https://www.wxwidgets.org/support/mailing-lists/
* Discussion Forums: https://forums.wxwidgets.org/
* #wxwidgets IRC Channel: https://www.wxwidgets.org/support/irc/
* Stack Overflow (tag your questions with "wxwidgets"):
  https://stackoverflow.com/questions/tagged/wxwidgets

Have fun!

The wxWidgets Team, February 2016
