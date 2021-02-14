About
-----

wxWidgets is a free and open source cross-platform C++ framework
for writing advanced GUI applications using native controls.

![wxWidgets Logo](https://www.wxwidgets.org/assets/img/header-logo.png)

wxWidgets allows you to write native-looking GUI applications for
all the major desktop platforms and also helps with abstracting
the differences in the non-GUI aspects between them. It is free
for the use in both open source and commercial applications, comes
with the full, easy to read and modify, source and extensive
documentation and a collection of more than a hundred examples.
You can learn more about wxWidgets at https://www.wxwidgets.org/
and read its documentation online at https://docs.wxwidgets.org/


Platforms
---------

[![AppVeyor](https://img.shields.io/appveyor/ci/wxWidgets/wxWidgets/master.svg?label=Windows)](https://ci.appveyor.com/project/wxWidgets/wxwidgets)
[![Travis](https://img.shields.io/travis/wxWidgets/wxWidgets/master.svg?label=Linux)](https://travis-ci.org/wxWidgets/wxWidgets)
[![OSS-Fuzz](https://oss-fuzz-build-logs.storage.googleapis.com/badges/wxwidgets.svg)](https://bugs.chromium.org/p/oss-fuzz/issues/list?sort=-opened&can=1&q=proj:wxwidgets)

This version of wxWidgets supports the following primary platforms:

- Windows XP, Vista, 7, 8 and 10 (32/64 bits).
- Most Unix variants using the GTK+ toolkit (version 2.6 or newer or 3.x).
- macOS (10.10 or newer) using Cocoa under both amd64 and ARM platforms.

Most popular C++ compilers are supported including but not limited to:

- Microsoft Visual C++ 2003 or later (up to 2019).
- g++ 4 or later, including MinGW/MinGW-64/TDM under Windows.
- Clang under macOS and Linux.
- Intel icc compiler.
- Oracle (ex-Sun) CC.


Licence
-------

[wxWidgets licence](https://github.com/wxWidgets/wxWidgets/blob/master/docs/licence.txt)
is a modified version of LGPL explicitly allowing not distributing the sources
of an application using the library even in the case of static linking.


Building
--------

For building the library, please see platform-specific documentation under
`docs/<port>` directory, e.g. here are the instructions for
[wxGTK](docs/gtk/install.md), [wxMSW](docs/msw/install.md) and
[wxOSX](docs/osx/install.md).

If you're building the sources checked out from Git, and not from a released
version, please see these additional [Git-specific notes](README-GIT.md).


Further information
-------------------

If you are looking for community support, you can get it from

- [Mailing Lists](https://www.wxwidgets.org/support/mailing-lists/)
- [Discussion Forums](https://forums.wxwidgets.org/)
- [#wxwidgets IRC channel](https://www.wxwidgets.org/support/irc/)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/wxwidgets)
  (tag your questions with `wxwidgets`)
- And you can report bugs at https://trac.wxwidgets.org/newticket

[Commercial support](https://www.wxwidgets.org/support/commercial/) is also
available.

Finally, keep in mind that wxWidgets is an open source project collaboratively
developed by its users and your contributions to it are always welcome. Please
check [our guidelines](.github/CONTRIBUTING.md) if you'd like to do it.


Have fun!

The wxWidgets Team.
