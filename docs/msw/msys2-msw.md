Building wxMSW port with Win32 MSys2 backend {#plat_msw_msys2}
------------------------------------------------

The MSys2 website is http://www.msys2.org/

These building steps are NOT the normal way to build MSys2 MinGW packages.
But, they are a way the wxWidgets developers can test that wxWidgets
can build the wxMSW libraries under MSys2 MinGW.

For the MSys2 way please see https://github.com/Alexpux/MINGW-packages

Building steps:

1. Install the mingw32 packages needed to build wxMSW using the
   configure/make build method.
   From the MSys2 prompt or MSys2 MinGW prompt:

   The 32 bit Mingw packages are prefixed with "mingw-w64-i686-";
   Change the prefix to "mingw-w64-x86_64-" if you wish to do 64 bit.

        pacman -S --needed --noconfirm make
        pacman -S --needed --noconfirm mingw-w64-i686-libjpeg-turbo
        pacman -S --needed --noconfirm mingw-w64-i686-libpng
        pacman -S --needed --noconfirm mingw-w64-i686-libtiff
        pacman -S --needed --noconfirm mingw-w64-i686-gcc

    Packages that are needed but are normally installed already.

        pacman -S --needed --noconfirm mingw-w64-i686-gcc-libs
        pacman -S --needed --noconfirm mingw-w64-i686-expat
        pacman -S --needed --noconfirm mingw-w64-i686-xz
        pacman -S --needed --noconfirm mingw-w64-i686-zlib


2. Build the wxMSW static library
    1. Open MSys2 MinGW Prompt
   (These steps were tested on MinGW32; but, should work under MinGW64)
    2. Use the cd command to change directory to the wxWidgets top folder.

    3. Create the "build-msw-static" folder to build the static libraries

            mkdir -p build-msw-static

    4. Configure wxWidgets

        Option "--disable-precomp-headers" is NOT needed.
        I am doing it to check for compile issues;
        And, I think my old 32 bit Windows machine
        works best with it disabled.

            cd build-msw-static && \
            ../configure --with-msw \
                --disable-shared \
                --disable-precomp-headers \
            && cd ..

    5. make the wxMSW static libraries

            cd build-msw-static && make && cd ..

3. Build and run the minimal static sample

    1. Clean the minimal sample

            cd build-msw-static/samples/minimal && make clean && cd ../../..

    2. Build the minimal sample

            cd build-msw-static/samples/minimal && make && cd ../../..

    3. Run the minimal sample

            ./build-msw-static/samples/minimal/minimal.exe

4. Clean the static samples

        cd build-msw-static/samples && make clean && cd ../..

5.  Build and run the typetest static sample to verify MIME database works

        cd build-msw-static/samples/typetest && make && cd ../../..
        ./build-msw-static/samples/typetest/typetest.exe

6.  Build and run the drawing static sample

        cd build-msw-static/samples/drawing && make && cd ../../..
        cd samples/drawing && ../../build-msw-static/samples/drawing/drawing.exe && cd ../..

7.  Build and run the splash static sample

        cd build-msw-static/samples/splash && make && cd ../../..
        cd samples/splash && ../../build-msw-static/samples/splash/splash.exe && cd ../..

8.  Build and run the widgets static sample

        cd build-msw-static/samples/widgets && make && cd ../../..
        cd samples/widgets && ../../build-msw-static/samples/widgets/widgets.exe && cd ../..

9.  Build all the rest of the static samples

        cd build-msw-static/samples && make && cd ../..
