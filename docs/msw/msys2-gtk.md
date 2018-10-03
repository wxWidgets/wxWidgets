Building wxGTK port with Win32 MSys2 GDK backend {#plat_msw_msys2_gtk}
------------------------------------------------

GTK+ widget toolkit has multiple GDK backends and one of them is Win32.
It is a wrapper around Windows API.

These notes don't consider building wxGTK with X11 backend under Windows.

The MSys2 website is http://www.msys2.org/

These building steps are NOT the normal way to build MSys2 MinGW packages.
But, they are a way the wxWidgets developers can test that wxWidgets
can build the wxGTK/Win32 libraries under MSys2 MinGW.

For the MSys2 way please see
  https://github.com/Alexpux/MINGW-packages and
  https://github.com/msys2/msys2/wiki/Creating-packages

Building steps:

**Warning**: At the time these directions were written the GTK version 3
         was NOT able to create wxGTK/Win32 libraries that were usable.

1. Install the mingw32 packages needed to build wxGTK/Win32 using the
   configure/make build method.
   From the MSys2 prompt or MSys2 MinGW prompt:

   The 32 bit Mingw packages are prefixed with "mingw-w64-i686-";
   Change the prefix to "mingw-w64-x86_64-" if you wish to do 64 bit.

        pacman -S --needed --noconfirm make
        pacman -S --needed --noconfirm mingw-w64-i686-libjpeg-turbo
        pacman -S --needed --noconfirm mingw-w64-i686-libpng
        pacman -S --needed --noconfirm mingw-w64-i686-libtiff
        pacman -S --needed --noconfirm mingw-w64-i686-gcc
        pacman -S --needed --noconfirm mingw-w64-i686-pkg-config
        ## gtk2 can take a long time to update/find fonts
        pacman -S --needed --noconfirm mingw-w64-i686-gtk2


    Packages that are needed but are normally installed already.

        pacman -S --needed --noconfirm mingw-w64-i686-gcc-libs
        pacman -S --needed --noconfirm mingw-w64-i686-expat
        pacman -S --needed --noconfirm mingw-w64-i686-xz
        pacman -S --needed --noconfirm mingw-w64-i686-zlib
        pacman -S --needed --noconfirm mingw-w64-i686-gdk-pixbuf2


2. Build the wxGTK/Win32 static library
    1. Open MSys2 MinGW Prompt
       (These steps were tested on MinGW32; but, should work under MinGW64)
    2. Use the cd command to change directory to the wxWidgets top folder.

    3. Create the "build-gtk2-static" folder to build the static libraries

            mkdir -p build-gtk2-static

    4. Configure wxWidgets
       Option "--disable-precomp-headers" is NOT needed.
       It is being used to test for compile issues.

       Remove configure option "--disable-wxdib" to set wxUSE_WXDIB to 1.
       The directions docs/msw/gtk.txt results in wxUSE_WXDIB set to 1.

            cd build-gtk2-static && \
            ../configure --with-gtk=2 \
                --disable-wxdib \
                --disable-shared \
                --disable-precomp-headers \
            && cd ..

    5. clean the wxGTK static libraries

            cd build-gtk2-static && make clean && cd ..

    6. make the wxGTK static libraries

            cd build-gtk2-static && make && cd ..


3.  Build and run the minimal static sample
    1. Clean the minimal sample

            cd build-gtk2-static/samples/minimal && make clean && cd ../../..

    2. Build the minimal sample

            cd build-gtk2-static/samples/minimal && make && cd ../../..

    3. Run the minimal sample

            ./build-gtk2-static/samples/minimal/minimal.exe


4. Build most of the static samples
    1. Clean most of the static samples

            cd build-gtk2-static/samples && make clean && cd ../..

    2. Build most of the static samples

            cd build-gtk2-static/samples && make && cd ../..


5.  Run the drawing static sample

        cd samples/drawing && ../../build-gtk2-static/samples/drawing/drawing.exe && cd ../..

6.  Run the splash static sample

        cd samples/splash && ../../build-gtk2-static/samples/splash/splash.exe && cd ../..

7.  Run the widgets static sample

        cd samples/widgets && ../../build-gtk2-static/samples/widgets/widgets.exe && cd ../..

8.  Run the toolbar static sample

        cd samples/toolbar && ../../build-gtk2-static/samples/toolbar/toolbar.exe && cd ../..

9.  Run the image static sample

        cd samples/image && ../../build-gtk2-static/samples/image/image.exe && cd ../..
