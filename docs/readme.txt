wxWidgets 3.3.3 Release Notes
=============================

Welcome to the latest release of wxWidgets, a free and open source
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

* https://docs.wxwidgets.org/3.3.3/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://www.wxwidgets.org/downloads/

or, for a more more permanent but less convenient to use link, from

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.3.3/

Please see https://docs.wxwidgets.org/3.3.3/overview_install.html for full
installation instructions.



Changes since 3.3.2
-------------------

This release contains more than 600 commits from 55 contributors since 3.3.2,
including a number of new features and improvements:

- Add dark mode support to wxMessageBox and other dialogs in wxMSW (#26570).
- Support switching between light/dark modes in wxMSW (#26516).
- Add support for stylus events to wxMSW (#26223).
- Add accessibility support to wxSVGFileDC (#26379).
- Add wxWebView::Print() and PrintToPDF() (#26239, #26583).
- Improve wxRibbon appearance in high DPI (#26117, #26409).
- Improve wxStaticText label wrapping in wxGTK.
- Add wxMoveToTrash() function (#26256).
- Enable drawing color emoji using Direct2D in wxMSW (#26657).
- Implement C++20 <=> operator for wxString (#26306).
- Add support for private fonts to wxQt (#26403).
- Support building wxOSX under macOS 27 (#26578).

There were also a number of bug fixes, the most important ones being:

- Improve wxTarInputStream robustness (#26607).
- Treat truncated reads as errors in wxDataInputStream (#26600).
- Fix regression when using wxImageList in high DPI in wxGTK (#26349).
- Fix regression with wxCheckBox label in screen readers in wxMSW (#26270).
- Fix regression with wxPD_CAN_SKIP in 3.3.2 in wxMSW (#26287).
- Fix dataview icons rendering under macOS 26 (#26374).
- Fix wxOSX wxOverlay leaving phantoms in Mission Control (#26380).

Please see the change log for the more complete list:

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.3.3/docs/changes.txt

and also see

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.2.11/docs/changes.txt

as the changes in 3.2.11 release are also included in this one and notably
include multiple buffer overflows in different image handlers and other
components.


Note that in spite of all these changes, wxWidgets 3.3.3 is almost fully
compatible with wxWidgets 3.2 and updating the existing applications to
use it shouldn't require much effort. However please make sure to read the
"INCOMPATIBLE CHANGES" section of the change log above when upgrading to be
aware of the breaking changes in it.


Supported Platforms
-------------------

This version of wxWidgets supports the following primary platforms:

* Windows 7, 8, 10 and 11 (32/64 bits).
* Most Unix variants using the GTK+ toolkit (version 2.6 or newer)
* macOS (10.10 or newer, up to 27) using Cocoa (x86-64 or ARM).

There is some support for the following platforms:

* Most Unix variants with X11
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

However, if you distribute wxGTK or wxQt version of your
application, don't forget that it is linked against GTK or Qt, which
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

The wxWidgets Team, July 2026
