/////////////////////////////////////////////////////////////////////////////
// Name:        introduction.h
// Purpose:     Introduction page of the Doxygen manual
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page page_introduction Introduction

@tableofcontents



@section page_introduction_whatis What is wxWidgets?

wxWidgets is an open source C++ framework allowing to write cross-platform
GUI applications with native look and feel in C++ and other languages.

wxWidgets was originally developed by Julian Smart at the Artificial
Intelligence Applications Institute, University of Edinburgh, for internal use,
and was first made publicly available in 1992, with a vastly improved version 2
released in 1999. The last major version of the library is 3 and was released
in 2013. Currently wxWidgets is developed and maintained by Julian Smart, Vadim
Zeitlin, Stefan Csomor, Robert Roebling, Vaclav Slavik and many others.

More information about wxWidgets is available on its web site at
https://www.wxwidgets.org.



@section page_introduction_why Why choose wxWidgets?

Compared to the other similar libraries, wxWidgets is:

1. The only C++ GUI library built by wrapping native GUI widgets which results
   in the best user experience on each platform.
2. Written using only the standard C++ and doesn't rely on any custom
   extensions or preprocessing.
3. Open source and free for use in both open source and commercial projects.


wxWidgets provides a simple, easy to learn, yet very rich API, while also
proposing a simple way to define application UI in the external XML files.
It is also mature and stable, and the applications written using wxWidgets 2.0
pre-releases almost 20 years ago can still be built today with wxWidgets 3
almost unchanged. wxWidgets has a large, active and friendly community of
people, including both the users and developers of the library. It is also
available now for more than a dozen other languages, including Python, Perl,
Ruby, Lua, Haskell, D, Erlang, PHP, in addition to C++.

It is impossible to sum up everything included in wxWidgets in a few
paragraphs, but here are some of the benefits:

@li Available on all major desktop platforms.
@li Free for any use.
@li Source is available and easy to read and modify if necessary.
@li Over 100 example programs.
@li Extensive documentation (almost 200,000 lines of it).
@li Straightforward API.
@li Simple but powerful layout system.
@li Run-time loadable or compile-time embeddable resources for UI definition.
@li Flexible event system.
@li All the usual and quite a few of more rare GUI controls.
@li And also all the standard dialogs.
@li 2D path-based drawing API with full support for transparency.
@li Built-in support for many file formats (BMP, PNG, JPEG, GIF, XPM, PNM, PCX, TGA, ...).
@li Printing, help, clipboard, drag-and-drop, document/view, ... support.
@li Integration with the native platform HTML rendering engine.
@li Dockable windows framework.
@li Word processor-like widget.
@li Powerful text editing widget with syntax highlighting.
@li And much, much more...


@section page_introduction_requirements wxWidgets Requirements

wxWidgets first-tier "ports", ie implementations of wxWidgets API, are:

@li wxMSW: This is the native port for Microsoft Windows systems (from Windows
XP up to Windows 11), either 32 or 64 bits. The primarily supported compilers
are Microsoft Visual C++ (versions 2005 up to 2022 are supported, at least 2010
is recommended) and GNU g++ (either from the traditional MinGW, TDM-GCC or
MinGW-w64 distributions).

@li wxGTK: wxGTK2 and wxGTK3 are the ports using GTK+ library version 2.x and
3.x respectively. They are very similar, with wxGTK2 being, however, more
mature. Both ports work on almost any Unix system (Linux, FreeBSD, OpenBSD,
NetBSD, Solaris, AIX, ...) and require GTK+ 2.6 or later or GTK+ 3.x. The
primary supported compiler is GNU g++.

@li wxOSX/Cocoa: This is the native port for Apple computers. wxOSX/Cocoa
supports 32 or 64 bit Intel Macs running macOS 10.10 or later. The port can be
built either with g++ or clang.

Other platforms (e.g. iOS - with a minimum requirement of iOS 13), compilers
(Sun CC, HP-UX aCC, IBM xlC or SGI mipsPro under Unix) and ports (wxQT, wxGTK1,
wxX11, wxDFB...) are also supported but to a lesser extent.
Please see the @subpage page_port "platform details page" for more information.

There are no CPU speed requirements but the faster (and more) CPU(s) you have,
the faster the library will compile. You do need to have enough RAM, especially
under Windows platforms, to avoid running out of memory during link phase.
Depending on the compiler used, you may need at least 4GB to be able to link.
Under all platforms it's recommended to have large amounts of free hard disk
space. The exact amount needed depends on the port, compiler and build
configurations but at least 1GB and possibly more is required.



@section page_introduction_where Where to get wxWidgets and support for it

The download links can be found at https://www.wxwidgets.org/downloads/.
Additionally, the latest version can always be retrieved from our version
control system using Git (https://github.com/wxWidgets/wxWidgets).

The latest version of wxWidgets documentation that you are reading is available
online at https://docs.wxwidgets.org/trunk/ and please also see
https://wiki.wxwidgets.org/ for user-contributed contents.

And if you have any questions, you can join wxWidgets community using

- Web-based <a href="https://forums.wxwidgets.org/">wxForum</a>
- <a href="https://www.wxwidgets.org/support/mailing-lists/">Mailing lists</a>
- <a href="https://www.wxwidgets.org/support/irc/">IRC Channel</a>
- Or asking questions with @c wxwidgets tag on Stack Overflow:
  https://stackoverflow.com/questions/tagged/wxwidgets

*/
