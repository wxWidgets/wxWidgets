Building wxQT port with Win32 MSys2 Qt5 backend {#plat_msw_msys2_qt}
------------------------------------------------

The MSys2 website is http://www.msys2.org/

These building steps are NOT the normal way to build MSys2 MinGW packages.
But, they are a way the wxWidgets developers can test that wxWidgets
can build the wxQT/Win32 libraries under MSys2 MinGW.

For the MSys2 way please see
  https://github.com/msys2/MINGW-packages and
  https://www.msys2.org/wiki/Creating-Packages/

Building steps:

A. Install the MSys2 mingw packages needed to build wxQT/Win32 using the
     configure/make build method.
   From the MSys2 MinGW prompt:

        pacman -S --needed --noconfirm make
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-libjpeg-turbo
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-libpng
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-libtiff
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-gcc
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-pkg-config
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-qt5


    Packages that are needed but are normally installed already.

        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-gcc-libs
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-expat
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-xz
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-zlib
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-gdk-pixbuf2


B. Build the wxQT/Win32 static library
    1. Open MSys2 MinGW Prompt
       (These steps were tested on MinGW64; but, should work under MinGW32)
    2. Use the cd command to change directory to the wxWidgets top folder.

    3. Create the "build-${MSYSTEM_CARCH}-qt-static" folder to build the static libraries

            mkdir -p build-${MSYSTEM_CARCH}-qt-static

    4. Configure wxWidgets
       Option "--disable-precomp-headers" is NOT needed.
       It is being used to test for compile issues.

       Remove configure option "--disable-wxdib" to set wxUSE_WXDIB to 1.
       At the time these directions where written wxUSE_WXDIB set to 1
         resulted in build error in dnd sample and run-time errors in
         other samples.

            cd build-${MSYSTEM_CARCH}-qt-static && \
            ../configure --with-qt \
                --disable-wxdib \
                --disable-shared \
                --disable-precomp-headers \
            && cd ..

    5. clean the wxQT static libraries

            cd build-${MSYSTEM_CARCH}-qt-static && make clean && cd ..

    6. make the wxQT static libraries

            cd build-${MSYSTEM_CARCH}-qt-static && make && cd ..


C.  Build and run the minimal static sample
    1. Clean the minimal sample

            cd build-${MSYSTEM_CARCH}-qt-static/samples/minimal && make clean && cd ../../..

    2. Build the minimal sample

            cd build-${MSYSTEM_CARCH}-qt-static/samples/minimal && make && cd ../../..

    3. Run the minimal sample

            ./build-${MSYSTEM_CARCH}-qt-static/samples/minimal/minimal.exe


D. Build most of the static samples
    1. Clean most of the static samples

            cd build-${MSYSTEM_CARCH}-qt-static/samples && make clean && cd ../..

    2. Build most of the static samples

            cd build-${MSYSTEM_CARCH}-qt-static/samples && make && cd ../..


E.  Run the drawing static sample

        cd samples/drawing && ../../build-${MSYSTEM_CARCH}-qt-static/samples/drawing/drawing.exe && cd ../..

F.  Run the splash static sample

        cd samples/splash && ../../build-${MSYSTEM_CARCH}-qt-static/samples/splash/splash.exe && cd ../..

G.  Run the widgets static sample

        cd samples/widgets && ../../build-${MSYSTEM_CARCH}-qt-static/samples/widgets/widgets.exe && cd ../..

H.  Run the toolbar static sample

        cd samples/toolbar && ../../build-${MSYSTEM_CARCH}-qt-static/samples/toolbar/toolbar.exe && cd ../..

I.  Run the image static sample

        cd samples/image && ../../build-${MSYSTEM_CARCH}-qt-static/samples/image/image.exe && cd ../..

J.  Run the dnd static sample

        cd samples/dnd && ../../build-${MSYSTEM_CARCH}-qt-static/samples/dnd/dnd.exe && cd ../..
