wxWidgets 3.3.2 Release Notes
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

* https://docs.wxwidgets.org/3.3.2/

wxWidgets sources and binaries for the selected platforms are available for
download from:

* https://www.wxwidgets.org/downloads/

or, for a more more permanent but less convenient to use link, from

* https://github.com/wxWidgets/wxWidgets/releases/tag/v3.3.2/

Please see https://docs.wxwidgets.org/3.3.2/overview_install.html for full
installation instructions.



Changes since 3.3.0
-------------------

This release comes only a few weeks after 3.3.0, but still has more than 200
changes including several important fixes for bugs found in 3.3.0, notably:

- Restore Windows date/time pickers behaviour under non-English OS (#25511).
- Fix crash on startup under macOS when using Farsi as system language (#25561).
- Fix losing wxGenericListCtrl contents after AppendColumn() (#25519).
- Fix crash when sorting wxDataViewCtrl in wxGTK in some circumstances (#25625).
- Fix crash when using per-window menu in Windows MDI applications (#25522).
- Restore inadvertently broken API compatibility in wxPropertyGrid (#25627).

There were also a number of fixes to the new Windows dark mode support:

- Fix crash with wxStaticBitmap inside wxNotebook in dark mode (#25499).
- Fix disabled wxButton bitmaps appearance in dark mode (#25575).
- Fix disabled wxStaticText appearance in dark mode (#25574).
- Fix notebook background in dark high contrast themes (#25542).
- Fix regression in wxDataViewCtrl border in light mode (#25532).
- Fix selected toolbar buttons in dark mode (Stefan Ziegler, #25616).
- Fix wxComboCtrl appearance in dark mode (#23766).
- Fix wxTE_RICH wxTextCtrl appearance in dark mode (#25602).

And, finally, this release also contains a small number of new features:

- Allow setting "app id" under supported platforms (Windows/Wayland) (#25548).
- Add persistence support for wxRadioButtons (#25530) and wxCheckBox (#25515).
- Add support for PNG "Description" text chunk to wxImage (#25556).
- wxNumberFormatter improvements (#25614, #25635).

Please see the change log for the more complete list:

https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.3.2/docs/changes.txt

Note that in spite of all these changes, wxWidgets 3.3.2 is almost fully
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
