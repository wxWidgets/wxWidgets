Building wxWidgets for iOS           {#plat_ios_install}
----------------------------

wxiOS is far from a full supported port, but can be used
as base for simple applications and future improvements.

It requires Xcode with iOS SDK 9.0 or later. Xcode is available
for free in the macOS app store.

To build wxiOS you should use Xcode to open the minimal samples
Xcode project file located at:

    samples/minimal/minimal_iphone.xcodeproj

That project files includes wxiphone.xcodeproj so you just need
to hit run in order to see the minimal sample running in the
simulator.

The library can also be build via configure/make:

    mkdir build_ios
    cd build_ios
    ../configure --with-osx_iphone --enable-monolithic \
        --with-macosx-version-min=8.0 --disable-shared \
        --enable-macosx_arch=i386 \
        --with-macosx-sdk=$(xcrun --sdk iphonesimulator --show-sdk-path)
    make

On a recent Silicon Mac in the 3.3 branch you need this configure
to compile for the iPhone simulator:

    ../wxWidgets/configure --with-iphonesimulator --enable-monolithic  \
        --disable-shared --with-macosx-sdk=$(xcrun --sdk iphonesimulator --show-sdk-path)  \
        --host=$(../wxWidgets/config.guess) --build=aarch64-apple-darwin --without-libtiff
    make