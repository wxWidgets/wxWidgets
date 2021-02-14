Building wxMSW port with Win32 MSys2 backend {#plat_msw_msys2}
------------------------------------------------

The MSys2 website is http://www.msys2.org/

These building steps are NOT the normal way to build MSys2 MinGW packages.
But, they are a way the wxWidgets developers can test that wxWidgets
can build the wxMSW libraries under MSys2 MinGW.

For the MSys2 way please see https://github.com/Alexpux/MINGW-packages

Building steps:

A. Install the mingw32 packages needed to build wxMSW using the
   configure/make build method.
   From the MSys2 MinGW prompt:

        pacman -S --needed --noconfirm make
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-libjpeg-turbo
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-libpng
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-libtiff
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-gcc

    Packages that are needed but are normally installed already.

        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-gcc-libs
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-expat
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-xz
        pacman -S --needed --noconfirm ${MINGW_PACKAGE_PREFIX}-zlib


B. Build the wxMSW static library
    1. Open MSys2 MinGW Prompt
   (These steps were tested on MinGW64; but, should work under MinGW32)
    2. Use the cd command to change directory to the wxWidgets top folder.

    3. Create the "build-${MSYSTEM_CARCH}-msw-static" folder to build the static libraries

            mkdir -p build-${MSYSTEM_CARCH}-msw-static

    4. Configure wxWidgets

        Option "--disable-precomp-headers" is NOT needed.
        I am doing it to check for compile issues;
        And, I think my old 32 bit Windows machine
        works best with it disabled.

            cd build-${MSYSTEM_CARCH}-msw-static && \
            ../configure --with-msw \
                --disable-shared \
                --disable-precomp-headers \
            && cd ..

    5. make the wxMSW static libraries

            cd build-${MSYSTEM_CARCH}-msw-static && make && cd ..

C. Build and run the minimal static sample

    1. Clean the minimal sample

            cd build-${MSYSTEM_CARCH}-msw-static/samples/minimal && make clean && cd ../../..

    2. Build the minimal sample

            cd build-${MSYSTEM_CARCH}-msw-static/samples/minimal && make && cd ../../..

    3. Run the minimal sample

            ./build-${MSYSTEM_CARCH}-msw-static/samples/minimal/minimal.exe

D. Clean the static samples

        cd build-${MSYSTEM_CARCH}-msw-static/samples && make clean && cd ../..

E.  Build and run the typetest static sample to verify MIME database works

        cd build-${MSYSTEM_CARCH}-msw-static/samples/typetest && make && cd ../../..
        ./build-${MSYSTEM_CARCH}-msw-static/samples/typetest/typetest.exe

F.  Build and run the drawing static sample

        cd build-${MSYSTEM_CARCH}-msw-static/samples/drawing && make && cd ../../..
        cd samples/drawing && ../../build-${MSYSTEM_CARCH}-msw-static/samples/drawing/drawing.exe && cd ../..

G.  Build and run the splash static sample

        cd build-${MSYSTEM_CARCH}-msw-static/samples/splash && make && cd ../../..
        cd samples/splash && ../../build-${MSYSTEM_CARCH}-msw-static/samples/splash/splash.exe && cd ../..

H.  Build and run the widgets static sample

        cd build-${MSYSTEM_CARCH}-msw-static/samples/widgets && make && cd ../../..
        cd samples/widgets && ../../build-${MSYSTEM_CARCH}-msw-static/samples/widgets/widgets.exe && cd ../..

I.  Build all the rest of the static samples

        cd build-${MSYSTEM_CARCH}-msw-static/samples && make && cd ../..
