/////////////////////////////////////////////////////////////////////////////
// Name:        platdetails.h
// Purpose:     Platform details page of the Doxygen manual
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**

@page page_port Platform Details

@tableofcontents

wxWidgets defines a common API across platforms, but uses the native graphical
user interface (GUI) on each platform, so your program will take on the native
look and feel that users are familiar with. Unfortunately native toolkits and
hardware do not always support the functionality that the wxWidgets API
requires. This chapter collects notes about differences among supported
platforms and ports.



@section page_port_wxgtk wxGTK

wxGTK is a port of wxWidgets using the GTK library. It makes use of GTK's
native widgets wherever possible and uses wxWidgets' generic controls when
needed.

You will need GTK 2.6 or higher which is available from:

https://www.gtk.org

The newer version of GTK you use, the more native widgets and features will be
utilized. We have gone to great lengths to allow compiling wxWidgets
applications with the latest version of GTK, with the resulting binary working
on systems even with a much earlier version of GTK. You will have to ensure
that the application is launched with lazy symbol binding for that.

In order to configure wxWidgets to compile wxGTK you will need to use the
@c \--with-gtk argument to the @c configure script. This is the default for many
systems.

GTK 3 is the default starting with wxWidgets 3.1.4.
Use @c configure option @c \--with-gtk=2 to use GTK 2.

@subpage plat_gtk_install "Build and Install Instructions"



@section page_port_wxosx wxOSX/Cocoa

wxOSX/Cocoa is the port of wxWidgets for the macOS platform. It requires
a minimum SDK 10.11, Xcode 7.2.1 or greater (runs under 10.10.5 and higher),
and supports x86_64 (but not i386) and ARM builds and deploying under 10.10.

Note that Apple clang must be used to build wxOSX, due to the use of
Apple-specific extensions ("blocks") in the macOS SDK headers, and hence the
applications using it must be built using clang as well.

@subpage plat_osx_install "Build and Install Instructions"




@section page_port_wxx11 wxX11

wxX11 is a port of wxWidgets using X11 (The X Window System) as the underlying
graphics backend. wxX11 draws its widgets using the wxUniversal widget set
which is now part of wxWidgets. wxX11 is well-suited for a number of special
applications such as those running on systems with few resources (PDAs) or for
applications which need to use a special themed look.

In order to configure wxWidgets to compile wxX11 you will need to type:

@verbatim configure --with-x11 --with-universal @endverbatim

@subpage plat_x11_install "Build Instructions"



@section page_port_wxmsw wxMSW

wxMSW is a port of wxWidgets for the Windows platforms (current version
requires Windows 7 or later). wxMSW provides native look and feel for each Windows version.
This port can be compiled with several compilers including Microsoft
VC++ 2015 or later, MinGW, Cygwin as well as cross-compilation with a
Linux-hosted MinGW tool chain.

@subpage plat_msw_install "Build and Install Instructions"

@subpage plat_msw_binaries "Using pre-built binaries"

@subsection page_port_wxmsw_resources Resources and Application Icon

All applications using wxMSW should have a Windows resource file (@c .rc
extension) and this file should include @c include/wx/msw/wx.rc file which
defines resources used by wxWidgets itself.

Among other things, @c wx.rc defines some standard icons, all of which have
names starting with the "wx" prefix. This normally ensures that any icons
defined in the application's own resource file come before them in alphabetical
order which is important because Explorer (Windows shell) selects the first
icon in alphabetical order to use as the application icon which is displayed
when viewing its file in the file manager. So if all the icons defined in your
application start with "x", "y" or "z", they won't be used by Explorer. To
avoid this, ensure that the icon which is meant to be used as the main
application icon has a name preceding "wxICON" in alphabetical order.


@section page_port_wxQt wxQt

wxQt is a port of wxWidgets using Qt libraries. It requires Qt 5 or later.

@subpage plat_qt_install "Build Instructions"

@subpage plat_qt_architecture "Architecture Overview"

@section page_port_wxiOS wxiOS

wxiOS is a port of wxWidgets using Cocoa touch libraries for iOS. It is very
basic in it current form, but is included for further improvements and very
simple applications. It requires iOS 9 or later and fully supports 64 bit builds.

@subpage plat_ios_install "Build Instructions"

@section page_port_nativedocs Native Toolkit Documentation

It's sometimes useful to interface directly with the underlying toolkit
used by wxWidgets to e.g. use toolkit-specific features.
In such case (or when you want to e.g. write a port-specific patch) it can be
necessary to use the underlying toolkit API directly:

- wxMSW port uses Win32 API: see MSDN docs at https://docs.microsoft.com/en-us/windows/win32/controls/window-controls
- wxGTK port uses GTK and other lower-level libraries; see https://docs.gtk.org/
- wxOSX port uses the Cocoa API: see Cocoa docs at https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/CocoaFundamentals/WhatIsCocoa/WhatIsCocoa.html

*/
