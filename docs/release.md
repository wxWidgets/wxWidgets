*WARNING* This is a release candidate and not the final 3.1.1 release yet.

Welcome to wxWidgets, a free and open source cross-platform C++ framework for writing advanced GUI applications using native controls.

wxWidgets 3.1.1 is the second release in the 3.1 development branch. This release is a "development" one as it makes (very few) incompatible API changes compared to 3.0 and does not guarantee the ABI stability, unlike the 3.0.x series. It is not inherently more buggy or less stable than the "stable" releases and you're encouraged to use it. If you're already using 3.0, upgrading shouldn't require any special effort, so please try it out.

Please notice that we provide a single source distribution containing files for all platforms, so you only need to download either the archive with the `.zip` (or `.7z` for much smaller size) extension for Microsoft Windows systems or the one with `.tar.bz2` extenstion for Unix ones, including Mac OS X.

See [README](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.1.1-rc/docs/readme.txt) for more information about this release and the [change log](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.1.1-rc/docs/changes.txt) for details of the changes in it.

## Reporting Problems

Please report bugs to the [issue tracker](https://trac.wxwidgets.org/newticket) or the [wx-users mailing list](http://groups.google.com/group/wx-users).

## Download Verification

To verify your download please use the following SHA-1 checksums:

    d3f8a8f5bd7b5dee2176a14fce93b3767d217f00  wxMSW-3.1.1-rc-Setup.exe
    d8de5b7075ba7674e8c0e2cd722259d48e627dc7  wxWidgets-3.1.1-rc-docs-chm.zip
    000c818361eddd403557aacf8b63ac0210335919  wxWidgets-3.1.1-rc-docs-html.tar.bz2
    f5ec0051a4fe620dbeee5262ba1ce30922e5d85c  wxWidgets-3.1.1-rc-docs-html.zip
    f4d1bda81ef27fb232eb58f88730cfeafb35b477  wxWidgets-3.1.1-rc-headers.7z
    36efadd43d9994495d71a698922113a0d8d46b73  wxWidgets-3.1.1-rc.7z
    2968073dffa60c1ad5016d1b0d4eedb5b701efdf  wxWidgets-3.1.1-rc.tar.bz2
    84d7e7a8347fc727801a407302e2801ee7bfc608  wxWidgets-3.1.1-rc.zip

## Binaries

We provide pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 9.0, 10.0, 11.0, 12.0, 14.0 and 14.1 (corresponding to marketing product names of Microsoft Visual Studio 2008, 2010, 2012, 2013, 2015 and 2017 respectively).
* MinGW-TDM versions 5.1 and 7.2 (with the default SJLJ exceptions propagation method, using C++11). Please note that you need to use the very latest MinGW 7.2 compiler release with this version of the compiler which can be downloaded from [here for 32 bits](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/7.2.0/threads-win32/sjlj/i686-7.2.0-release-win32-sjlj-rt_v5-rev1.7z/download) and [here for 64 bits](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/7.2.0/threads-win32/seh/x86_64-7.2.0-release-win32-seh-rt_v5-rev1.7z/download), the older "rev0" release has a known bug affecting building wxWidgets in some scenarios.

### For Developers

For developing applications with wxWidgets you need to download the compiler-independent `wxWidgets-3.1.1_Headers.7z` file and one of `wxMSW-3.1.1-vcXXX_Dev.7z` or `wxMSW-3.1.1_gccXXX_Dev.7z` files depending on your compiler, its version and the target architecture (x86 if not specified or x64).

Unpack both files into the same directory so that `include` and `lib` directories are at the same level after unpacking. You should be able to compile and link applications using wxWidgets in both debug and release modes but the debug symbols are provided only for debug libraries in this archive, see below for the release build debug symbols.

### For End Users

End users may download one of `wxMSW-3.1.1_vcXXX_ReleaseDLL.7z` or `wxMSW-3.1.1_gccXXX_ReleaseDLL.7z` files to get just the DLLs required for running the applications using wxWidgets.

### For Debugging

* Microsoft Visual C++ users: Files `wxMSW-3.1.1_vcXXX_ReleasePDB.7z` contain the debug symbols for the release build of the DLLs. Download them if you want to debug your own applications in release build or if you want to get meaningful information from mini-dumps retrieved from your users machines.
* MinGW-TDM users: Currently the debug symbols are not available for the release build of the DLLs (only the debug versions of the DLLs contains the debug
  symbols).

### Binary File Download Verification

To verify your download please use the following SHA-1 checksums:

    26836356a6e77091caad508d765b60899c30892e wxMSW-3.1.1-rc_gcc510TDM_Dev.7z
    b7eb62bdb152e5539fd5e9ecc105d322fadba3ec wxMSW-3.1.1-rc_gcc510TDM_ReleaseDLL.7z
    77c84112ffcad3b2c649d9fd3adcd4e140945b61 wxMSW-3.1.1-rc_gcc510TDM_x64_Dev.7z
    24e775999ea19b641a11850af0547ac81fd1bc6b wxMSW-3.1.1-rc_gcc510TDM_x64_ReleaseDLL.7z
    ead74e56b9d22e578addd52b8246eadfddbd93d1 wxMSW-3.1.1-rc_gcc720_Dev.7z
    7c43aa6937853d1113430ce1b31a0747e89a6b47 wxMSW-3.1.1-rc_gcc720_ReleaseDLL.7z
    298016eb7cb2a509b4dedc5a8c09eb2b98950698 wxMSW-3.1.1-rc_gcc720_x64_Dev.7z
    741087b3fcbe809ae60563e19cf0db21b571725a wxMSW-3.1.1-rc_gcc720_x64_ReleaseDLL.7z
    7537561f1a93ca06a37be730ca9f81506da665db wxMSW-3.1.1-rc_vc90_Dev.7z
    3df72df959b4ae8279cb27519ea5b114003d17f6 wxMSW-3.1.1-rc_vc90_ReleaseDLL.7z
    1c506db10f555f11ae17ba08430482d81be1c0db wxMSW-3.1.1-rc_vc90_ReleasePDB.7z
    3b45c2680499b3e397264f0ae8f6d2617a139b8a wxMSW-3.1.1-rc_vc90_x64_Dev.7z
    3ba14408edad9b325bc0dd0ba98197954ce2db5e wxMSW-3.1.1-rc_vc90_x64_ReleaseDLL.7z
    e8c5f145a89a556069221886e7644c3b07d2799f wxMSW-3.1.1-rc_vc90_x64_ReleasePDB.7z
    936f454b547ec17e3cefdf4f43c9af8738f803ca wxMSW-3.1.1-rc_vc100_Dev.7z
    940c8c3923be2ad4a71e3de349657bb5443de918 wxMSW-3.1.1-rc_vc100_ReleaseDLL.7z
    8a4dedfc318272b182a83f5947b5224a7f17c532 wxMSW-3.1.1-rc_vc100_ReleasePDB.7z
    c68ccae3a29096e7f78732c65cddabeee5e226b6 wxMSW-3.1.1-rc_vc100_x64_Dev.7z
    ea540496a5875454ff90ba3f6ea65ddcacc9a4a5 wxMSW-3.1.1-rc_vc100_x64_ReleaseDLL.7z
    8100a2bfab8909de47fb483bb8bd78c03ddb4df5 wxMSW-3.1.1-rc_vc100_x64_ReleasePDB.7z
    1fa4c99e48cbf0f50289d07f6ceae8a71be62a19 wxMSW-3.1.1-rc_vc110_Dev.7z
    2e44afb8f7080fc673af2abb456cf176dad4b1a2 wxMSW-3.1.1-rc_vc110_ReleaseDLL.7z
    730e2e1db22e3bc308284562c8ecfbfc3f6a6e9e wxMSW-3.1.1-rc_vc110_ReleasePDB.7z
    5c2578deba24a1daf61ae08a7daa1a8662dbe8c9 wxMSW-3.1.1-rc_vc110_x64_Dev.7z
    57da37116c3b80d4f765b85fafcb30e2cb200dab wxMSW-3.1.1-rc_vc110_x64_ReleaseDLL.7z
    6700dbc853484d385753ad4361c16c48c4bd3e3a wxMSW-3.1.1-rc_vc110_x64_ReleasePDB.7z
    b4dac305e230ae0c9bfcd07fee632a4aad835329 wxMSW-3.1.1-rc_vc120_Dev.7z
    c986f27c6f88b9feb7706f4bff5f50b68b14cec2 wxMSW-3.1.1-rc_vc120_ReleaseDLL.7z
    ca5ca15eadc0d1ca6b79824bb9704215c7945195 wxMSW-3.1.1-rc_vc120_ReleasePDB.7z
    c9a4850228a1b42d1606d1043b9a3d0b334727a9 wxMSW-3.1.1-rc_vc120_x64_Dev.7z
    6a02c05ab72b06e30ef9f9e355956f3957c86eb1 wxMSW-3.1.1-rc_vc120_x64_ReleaseDLL.7z
    2c36680649bf3093536e20067d30e1242a6023d3 wxMSW-3.1.1-rc_vc120_x64_ReleasePDB.7z
    aeede25c3de0f975c811fbce634c5aaf354195fa wxMSW-3.1.1-rc_vc140_Dev.7z
    900203c8f300214e038fbb1ae10ba3eb1a114740 wxMSW-3.1.1-rc_vc140_ReleaseDLL.7z
    788c9b39c0c298735313de425c4ca7986b163908 wxMSW-3.1.1-rc_vc140_ReleasePDB.7z
    68ae8e1c42ff0105ea186c0d32cee95861a1a5a4 wxMSW-3.1.1-rc_vc140_x64_Dev.7z
    550f3d0f52f4d21babf03b66e26145787eb9d922 wxMSW-3.1.1-rc_vc140_x64_ReleaseDLL.7z
    5b57a82a1799ae1a05a385685ce9f729e3fed056 wxMSW-3.1.1-rc_vc140_x64_ReleasePDB.7z
    5990f6bf1d157e2b7e8f254d2a60dfd682971790 wxMSW-3.1.1-rc_vc141_Dev.7z
    fde6140a4348621a1b876ed6c289778d4312f2f1 wxMSW-3.1.1-rc_vc141_ReleaseDLL.7z
    824eef674529ca75e620dac6dce45cc6a0c2328b wxMSW-3.1.1-rc_vc141_ReleasePDB.7z
    a4b4014c1a1289d798597ebc810a01e88edf53bc wxMSW-3.1.1-rc_vc141_x64_Dev.7z
    285afbcc27e18344654183d8b9e6beca9a9eab92 wxMSW-3.1.1-rc_vc141_x64_ReleaseDLL.7z
    82685386d8291dd5012ef00ced5fdc45cb0d8a04 wxMSW-3.1.1-rc_vc141_x64_ReleasePDB.7z
