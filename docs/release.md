Welcome to wxWidgets, a free and open source cross-platform C++ framework for writing advanced GUI applications using native controls.

wxWidgets 3.1.1 is the second release in the 3.1 development branch. This release is a "development" one as it makes (very few) incompatible API changes compared to 3.0 and does not guarantee the ABI stability, unlike the 3.0.x series. It is not inherently more buggy or less stable than the "stable" releases and you're encouraged to use it. If you're already using 3.0, upgrading shouldn't require any special effort, so please try it out.

Please notice that we provide a single source distribution containing files for all platforms, so you only need to download either the archive with the `.zip` (or `.7z` for much smaller size) extension for Microsoft Windows systems or the one with `.tar.bz2` extenstion for Unix ones, including Mac OS X.

See [README](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.1.1/docs/readme.txt) for more information about this release and the [change log](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.1.1/docs/changes.txt) for details of the changes in it.

## Reporting Problems

Please report bugs to the [issue tracker](https://trac.wxwidgets.org/newticket) or the [wx-users mailing list](http://groups.google.com/group/wx-users).

## Download Verification

To verify your download please use the following SHA-1 checksums:

    0000000000000000000000000000000000000000  wxMSW-3.1.1-Setup.exe
    0000000000000000000000000000000000000000  wxWidgets-3.1.1-docs-chm.zip
    0000000000000000000000000000000000000000  wxWidgets-3.1.1-docs-html.tar.bz2
    0000000000000000000000000000000000000000  wxWidgets-3.1.1-docs-html.zip
    0000000000000000000000000000000000000000  wxWidgets-3.1.1-headers.7z
    0000000000000000000000000000000000000000  wxWidgets-3.1.1.7z
    0000000000000000000000000000000000000000  wxWidgets-3.1.1.tar.bz2
    0000000000000000000000000000000000000000  wxWidgets-3.1.1.zip

## Binaries

We provide pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 9.0, 10.0, 11.0, 12.0, 14.0 and 14.1
  (corresponding to marketing product names of Microsoft Visual Studio 2008,
   2010, 2012, 2013, 2015 and 2017 respectively).
* MinGW-TDM versions 4.9 and 5.1 (with the default SJLJ exceptions propagation
  method, using C++11).

### For Developers

For developing applications with wxWidgets you need to download the compiler-independent `wxWidgets-3.1.1_Headers.7z` file and one of `wxMSW-3.1.1-vcXXX_Dev.7z` or `wxMSW-3.1.1_gccXXX_Dev.7z` files depending on your compiler, its version and the target architecture (x86 if not specified or x64).

Unpack both files into the same directory so that `include` and `lib` directories are at the same level after unpacking. You should be able to compile and link applications using wxWidgets in both debug and release modes but the debug symbols are provided only for debug libraries in this archive, see below for the release build debug symbols.

### For End Users

End users may download one of `wxMSW-3.1.1_vcXXX_ReleaseDLL.7z` or `wxMSW-3.1.1_gccXXX_ReleaseDLL.7z` files to get just the DLLs required for running the applications using wxWidgets.

### For Debugging

* Microsoft Visual C++ users: Files `wxMSW-3.1.1_vcXXX_ReleasePDB.7z` contain
  the debug symbols for the release build of the DLLs. Download them if you want
  to debug your own applications in release build or if you want to get
  meaningful information from mini-dumps retrieved from your users machines.
* MinGW-TDM users: Currently the debug symbols are not available for the release
  build of the DLLs (only the debug versions of the DLLs contains the debug
  symbols).

### Binary File Download Verification

To verify your download please use the following SHA-1 checksums:

    0000000000000000000000000000000000000000 wxMSW-3.1.1_gcc492TDM_Dev.7z*
    0000000000000000000000000000000000000000 wxMSW-3.1.1_gcc492TDM_ReleaseDLL.7z*
    0000000000000000000000000000000000000000 wxMSW-3.1.1_gcc492TDM_x64_Dev.7z*
    0000000000000000000000000000000000000000 wxMSW-3.1.1_gcc492TDM_x64_ReleaseDLL.7z*
    0000000000000000000000000000000000000000 wxMSW-3.1.1_gcc510TDM_Dev.7z*
    0000000000000000000000000000000000000000 wxMSW-3.1.1_gcc510TDM_ReleaseDLL.7z*
    0000000000000000000000000000000000000000 wxMSW-3.1.1_gcc510TDM_x64_Dev.7z*
    0000000000000000000000000000000000000000 wxMSW-3.1.1_gcc510TDM_x64_ReleaseDLL.7z*
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc90_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc90_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc90_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc90_x64_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc90_x64_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc90_x64_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc100_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc100_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc100_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc100_x64_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc100_x64_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc100_x64_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc110_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc110_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc110_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc110_x64_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc110_x64_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc110_x64_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc120_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc120_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc120_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc120_x64_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc120_x64_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc120_x64_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc140_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc140_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc140_ReleasePDB.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc140_x64_Dev.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc140_x64_ReleaseDLL.7z
    0000000000000000000000000000000000000000 wxMSW-3.1.1_vc140_x64_ReleasePDB.7z
