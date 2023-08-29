Building wxGTK port with Win32 GDK backend {#plat_msw_gtk}
------------------------------------------

GTK+ widget toolkit has multiple GDK backends and one of them is Win32.
It is a wrapper around Windows API.

See http://www.gtk.org/download/win32.php

These notes don't consider building wxGTK with X11 backend under Windows.

Building steps:

1. wxGTK/Win32 build is similar to wxMSW one and you should have configured
and be able to build wxWidgets as described in @ref plat_msw_install

2. wxGTK/Win32 is disabled by default in wxWidgets, you need to enable it
in bakefiles manually. Apply following patch manually or using Cygwin:

        patch -p0 < docs/msw/gtkfix.patch

Regenerate required make/project files:

    cd build\bakefiles
    bakefile_gen

3. Download GTK+ for Windows and uncompress files in the directory
without spaces in the name. We suppose later that GTK+/Win32 is
in the `C:\gtk` directory.

    http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.24/gtk+_2.24.10-1_win32.zip

4. Building

4.1 If you are using Visual C++ project files you need to setup GTK+ 2 include directories

    C:\gtk\lib\include\gtk-2.0
    C:\gtk\lib\include\glib-2.0
    C:\gtk\lib\include\cairo
    C:\gtk\lib\include\pango-1.0
    C:\gtk\lib\include\gdk-pixbuf-2.0
    C:\gtk\lib\include\atk-1.0
    C:\gtk\lib\glib-2.0\include
    C:\gtk\lib\gtk-2.0\include

and library directory

    C:\gtk\lib\

See http://msdn.microsoft.com/en-us/library/t9az1d21(v=vs.90).aspx

Open solution file in `build\msw` directory, select "GTK+ Debug" solution
configuration and build the solution. To be sure that everything is as expected
you can build minimal sample.

GTK+ is linked to wxWidgets always as DLL, so `C:\gtk\bin` directory should be
in %PATH% environment variable before application run.

4.2 You can also build using nmake:

    set INCLUDE=C:\gtk\lib\glib-2.0\include\;C:\gtk\lib\gtk-2.0\include\;C:\gtk\include\cairo\;C:\gtk\include\pango-1.0\;C:\gtk\include\gtk-2.0\;C:\gtk\include\glib-2.0\;C:\gtk\include\gdk-pixbuf-2.0\;C:\gtk\include\atk-1.0\;
    set LIB=C:\gtk\lib\;
    set PATH=C:\gtk\bin\;%PATH%;

    nmake -f makefile.vc "TOOLKIT=GTK" "TOOLKIT_VERSION=2"

or with MinGW:

    set CXXFLAGS=%CXXFLAGS% -IC:\gtk\include\gtk-2.0
    set CXXFLAGS=%CXXFLAGS% -IC:\gtk\include\glib-2.0
    set CXXFLAGS=%CXXFLAGS% -IC:\gtk\include\cairo
    set CXXFLAGS=%CXXFLAGS% -IC:\gtk\include\pango-1.0
    set CXXFLAGS=%CXXFLAGS% -IC:\gtk\include\gdk-pixbuf-2.0
    set CXXFLAGS=%CXXFLAGS% -IC:\gtk\include\atk-1.0
    set CXXFLAGS=%CXXFLAGS% -IC:\gtk\lib\glib-2.0\include
    set CXXFLAGS=%CXXFLAGS% -IC:\gtk\lib\gtk-2.0\include
    set CFLAGS=%CXXFLAGS%
    set LDFLAGS=%LDFLAGS% -LC:\gtk\lib
    set PATH=C:\gtk\bin\;%PATH%;

    mingw32-make -f makefile.gcc "TOOLKIT=GTK" "TOOLKIT_VERSION=2"

5. Now you can use wxGTK/Win32 as wxMSW in your applications.
