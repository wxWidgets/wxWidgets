wxWidgets 3.2.2.1 Release Notes
===============================

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

* https://docs.wxwidgets.org/3.2.2/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://www.wxwidgets.org/downloads/

or, for a more more permanent but less convenient to use link, from

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.2.2.1/

Please see https://docs.wxwidgets.org/3.2.2/overview_install.html for full
installation instructions.



Changes since 3.2.1
-------------------

This release comes only a few months after the previous 3.2.1, but contains
an important number of bug fixes and enhancements, further improving high DPI
support, including:

- Better window resizing on DPI change in wxMSW.
- Fix using native icons returned by wxArtProvider.
- Fix menu items using custom font in high DPI.
- High resolution icons support in wxGenericTreeCtrl and wxGenericListCtrl.

and also improving locale-related code under Mac and Unix systems:

- wxUILocale::UseDefault() works for locales using different language and
  region under Mac and fails when used for unsupported locale under Unix.
- New wxUILocale::GetSystemLocaleId() allows to retrieve such locales IDs.
- wxUILocale::GetCurrent() works currently for "C" locale under Mac.

Some other user-visible enhancements made in this release:

- Allow selecting and copying text in wxMessageDialog in wxGTK.
- Improve size and behaviour of in-place editor in wxGenericTreeCtrl.
- Fix sometimes missing overwrite prompt in "Save" file dialog in wxMSW.
- Fix glitch in drawing wxStaticBox with a control as label in wxMSW.

There are also some important bug fixes:

- Fix regression in saving TIFF images that could end up truncated.
- Fix long standing bug in parsing wxHTTP responses.
- Fix data race when processing events generated in a worker thread.
- Avoid appending extraneous NUL bytes to wxTextDataObject text in wxMSW.
- Fix handling of fonts with fractional sizes in wxOSX.
- Fix resizing wxGLCanvas with EGL and Wayland in wxGTK.
- Fix display artefacts when using AUI without compositor under X11.
- Work around crashes when using wxTextCtrl with MinGW TDM 64.
- Fix for a possible crash when handling menu events under Mac.
- Third-party libraries have been updated to the latest versions.

All in all, this release includes ~150 fixes from 27 unique contributors,
please see the full change log for more details:

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.2.2.1/docs/changes.txt

This release is API and ABI-compatible with the previous 3.2.x releases, so
the existing applications don't even need to be rebuilt to profit from all the
fixes above if they use shared/dynamic libraries. And if they do need to be
recompiled, this can be done without any changes to the code.



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

The wxWidgets Team, February 2023
