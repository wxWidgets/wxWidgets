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



Changes since 3.3.1
-------------------

This release contains almost a 1000 commits since 3.3.1, including a number of
new features and improvements:

- Improve accessibility support: fix it for wxCheckBox in dark mode (#26184),
  add it for wxCheckListBox (#25948), wxStyledTextCtrl (#25956) and minimal
  support for wxRichTextCtrl (#26202).
- Add support for minimizing panes in wxAUI (#23986) and improve splitting
  behaviour in wxAuiNotebook (#26081).
- Add wxStyledTextCtrlMiniMap (#25887).
- Allow using GLX and EGL in the same program in wxGTK (#26023).
- Add support for using LunaSVG for SVG rendering (#25902).
- Include projects and solution files for MSVC 2026 (#26131).
- Many visual fixes for macOS 26 Tahoe (#25766, #25743, #25767)
- Support scrolling, wxListCtrl, wxNotebook in wxiOS (#25827, #25857, #25858).
- Implement printing support and wxArtProvider in wxQt (#26126, #26210).
- Generate suspend/resume events in wxOSX (#25778).
- Make automatic scrolling in wxScrolled<> configurable (#25978).
- Improve number and currency formatting (#25765).
- Significant improvements in wxSVGFileDC (#25723).
- Allow configuring timeouts in wxWebRequest (#25673) and add new
  wxWebRequestDebugLogger class (#26086).
- All bundled 3rd party libraries were updated to latest versions (#26010).

There were, of course, also a number of bug fixes:

- Fix using wxSOCKET_NOWAIT_READ and wxSOCKET_WAITALL_WRITE together (#17114).
- Fix crash on mouse hover after closing tab in wxAuiNotebook (#25959).
- More dark mode fixes in wxMSW: fix or improve rendering of several controls
  (#25835), toolbar (#25892) and menus (#26182).
- More high DPI fixes as well: for wxToolBar (#26038), wxAuiToolBar (#26076),
  wxPGMultiButton (#26069), wxTreeCtrl state images (#26059), generic
  wxCalendarCtrl (#25713) and bitmap position in wxDC::DrawLabel() (#25888).
- Many RTL layout fixes in wxMSW, wxGTK and wxQt (#25426, #25822).
- Fix multiple bugs in gesture handling in wxGTK (#26241).

Please see the change log for the more complete list:

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.3.3/docs/changes.txt

and also see

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.2.10/docs/changes.txt

as the changes in 3.2.9 and 3.2.10 releases are also included in this one.

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
* macOS (10.10 or newer) using Cocoa (x86-64 or ARM).

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

The wxWidgets Team, July 2025
