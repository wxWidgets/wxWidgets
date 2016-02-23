   Welcome to wxWidgets for iOS
   ----------------------------

wxiOS is far from a full supported port, but can be used
as base for simple applications and future improvements.

It requires Xcode with iOS SDK 9.0 or later. Xcode is available
for free in the OS X app store.

More info about the wxWidgets project (including all the
other ports and version of wxWidgets) can be found at the
main wxWidgets homepage at:

    https://www.wxwidgets.org/

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


Please send problems concerning installation, feature requests,
bug reports or comments to the wxWidgets users list. These can
be found at https://www.wxwidgets.org/support/mailing-lists/

wxWidgets doesn't come with any guarantee whatsoever. It
might crash your harddisk or destroy your monitor. It doesn't
claim to be suitable for any special or general purpose.

  Regards,

    The wxWidgets Team
