                wxWidgets 3.0.5 Release Notes
                =============================

Welcome to the latest stable release of wxWidgets, a free and open
source cross-platform C++ framework for writing advanced GUI
applications using native controls.


wxWidgets allows you to write native-looking GUI applications for
all the major desktop platforms and also helps with abstracting
the differences in the non-GUI aspects between them. It is free
for the use in both open source and commercial applications, comes
with the full, easy to read and modify, source and extensive
documentation and a collection of more than a hundred examples.
You can learn more about wxWidgets at

        http://www.wxwidgets.org/

and read its documentation online at

        http://docs.wxwidgets.org/3.0.5/


wxWidgets sources and binaries for the selected platforms are
available for download from

	https://github.com/wxWidgets/wxWidgets/releases/tag/v3.0.5/



Changes in this release
-----------------------

This is a bug fix release with no significant new features compared to
the previous 3.0.x releases and compatible with them at both the API
and the ABI level (i.e. all applications linked against earlier 3.0.x
DLLs or shared libraries will continue to work when using 3.0.4
libraries).

The full list of changes in this release is available at

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.0.5/docs/changes.txt

(starting from the line 583, or search for "3.0.5" in this file), here are
some selected ones:

* In all ports:
- Support nullptr in wxPrintf() and other pseudo-vararg functions.
- Fix problem with wx-config installation and use under NetBSD (wiz).
- Avoid spurious errors on thread creation under NetBSD.
- Improve stock items consistency and aesthetics (dhowland).
- Implement wxTextCtrl::HitTest() for single line controls.
- Improve wxSVGFileDC to support more of wxDC API (Maarten Bent).

* In wxGTK:
- Fix position of popup menus shown in wxListCtrl.
- Fix not showing wxInfoBar with GTK+ 3 < 3.22.29.
- Potentially incompatible change: wxDataViewModel::Cleared() now works as
  documented cross-platform.  Previously on wxGTK it just emptied the model
  rather than triggering a reload of the model.  If you are working around the
  broken behaviour with wxGTK-specific code in your application you should
  check that your application works correctly with this change.
- Return appropriate values for some previously hard-coded system colours.

* In wxMSW:
- Fix handling of AUX2 mouse button events (Trylz).
- Honour alignment flags for multiline buttons using custom colours too.
- Fix crash when using wxCHMHelpController() in 64 bit builds (Xlord2).


If you are upgrading from wxWidgets 2.8 or earlier, please read the
"INCOMPATIBLE CHANGES" section in the beginning of this file and the
corresponding part of the manual at

    http://docs.wxwidgets.org/3.0.5/overview_changes_since28.html


Platforms supported
-------------------

wxWidgets 3.0.x supports the following primary platforms:

- Windows 95/98/ME, NT, 2000, XP, Vista, 7, 8, 10 (32/64 bits).
- Most Unix variants using the GTK+ toolkit (version 2.6 or newer)
- Mac OS X (10.5 or newer) using Cocoa (32/64 bits) or Carbon (32 only)

There is some support for the following platforms:

- Most Unix variants with X11
- Most Unix variants with Motif/Lesstif
- Most Unix variants with GTK+ 1.2
- OS/2
- Windows CE (Pocket PC)

Most popular C++ compilers are supported; see the install.txt
file for each platform (available via docs/html/index.htm) and
http://wiki.wxwidgets.org/Supported_Platforms for the most up to
date status.


Files
-----

wxWidgets is distributed in source form in several archive formats.
ZIP and 7z archives are for Microsoft Windows users and contain the
files with DOS/Windows line endings while the compressed tar archives
for Unix systems users (including OS X) and contain the files with
Unix line endings. Please notice that some Windows tools still don't
accept files with Unix line endings and that compiling sources with
DOS line endings under Unix will fail, so please choose the correct
file for your system.

In addition to the sources, documentation in HTML, CHM and HTB
(wxWidgets help viewer) formats is provided as well as an installer
for Microsoft Windows. Notice that you will still need to compile
wxWidgets even when using the installer.

We also supply binaries of wxMSW libraries built with several
versions of Microsoft Visual C++ and GNU g++ compiler for this
release. They are available in the "binaries" subdirectory, see
the description of the files in the README file there.


Installation
------------

Unless you have downloaded the binaries for your compiler, you
will need to build wxWidgets before you can test out the samples
or write your own applications. For installation information,
please see the install.txt file in the docs subdirectory
appropriate for the platform you use, e.g. docs/msw/install.txt.


Licence information
-------------------

For licensing information, please see the files:

  docs/preamble.txt
  docs/licence.txt
  docs/licendoc.txt
  docs/gpl.txt
  docs/lgpl.txt
  docs/xserver.txt

Although this may seem complex, it is there to allow authors of
proprietary/commercial applications to use wxWidgets in addition
to those writing GPL'ed applications. In summary, the licence is
LGPL plus a clause allowing unrestricted distribution of
application binaries. To answer a FAQ, you don't have to
distribute any source if you wish to write commercial
applications using wxWidgets.

However, if you distribute wxGTK or wxMotif (with Lesstif)
version of your application, don't forget that it is linked
against GTK+ (or Lesstif) which is covered by LGPL *without*
exception notice and so is bound by its requirements.

If you use TIFF image handler, please see src/tiff/COPYRIGHT
for libtiff licence details.

If you use JPEG image handler, documentation for your program
should contain following sentence: "This software is based in
part on the work of the Independent JPEG Group". See
src/jpeg/README for details.

If you use wxRegEx class on a system without native regular
expressions support (i.e. MS Windows), see src/regex/COPYRIGHT
file for Henry Spencer's regular expression library copyright.

If you use wxXML classes or XRC, see src/expat/COPYING for licence
details.


Bug reporting
-------------

The wxWidgets bug tracker can be browsed at:

    http://trac.wxwidgets.org/report

and new bugs can be opened from

    http://trac.wxwidgets.org/newticket

Please use the search function of our Trac installation to find
any possibly relevant bugs before reporting new ones. Also please
notice that often trying to correct the bug yourself is the
quickest way to fix it. Even if you fail to do it, you may
discover valuable information allowing us to fix it while doing
it. We also give much higher priority to bug reports with patches
fixing the problems so this ensures that your report will be
addressed sooner.


Further information
-------------------

If you are looking for support, you can get it from

- wxForum at http://forums.wxwidgets.org/
- wx-users mailing list (http://www.wxwidgets.org/support/maillst2.htm)
- #wxwidgets IRC channel
- http://stackoverflow.com/ if you tag your question with "wxwidgets"


Have fun!

The wxWidgets Team, April 2017
