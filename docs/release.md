Welcome to wxWidgets, a free and open source cross-platform C++ framework for writing advanced GUI applications using native controls.

Please notice that since the 2.9 series, only a single source distribution is provided instead of the multiple distributions containing only the files for each port as was done before. So you only need to download wxWidgets-3.0.0.zip (or .7z for much smaller archive size) for Microsoft Windows systems or wxWidgets-3.0.0.tar.bz2 for Unix ones, including Mac OS X. The only difference between these files is that the ZIP (and 7z) archives contain files with DOS line endings while the tar balls contain files with Unix line endings.

See [README](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.0.5/docs/readme.txt) for more information about this release and the [change log](https://raw.githubusercontent.com/wxWidgets/wxWidgets/v3.0.5/docs/changes.txt) for details of the changes in it.

## Reporting Problems

Please report bugs to the [issue tracker](http://trac.wxwidgets.org/newticket) or the [wx-users mailing list](http://groups.google.com/group/wx-users).

## Download Verification

To verify your download please use the following sha1sums:

```
0000000000000000000000000000000000000000  wxMSW-3.0.5-Setup.exe
0000000000000000000000000000000000000000  wxWidgets-3.0.5-docs-chm.zip
0000000000000000000000000000000000000000  wxWidgets-3.0.5-docs-html.tar.bz2
0000000000000000000000000000000000000000  wxWidgets-3.0.5-docs-html.zip
0000000000000000000000000000000000000000  wxWidgets-3.0.5-headers.7z
0000000000000000000000000000000000000000  wxWidgets-3.0.5.7z
0000000000000000000000000000000000000000  wxWidgets-3.0.5.tar.bz2
0000000000000000000000000000000000000000  wxWidgets-3.0.5.zip
```

## Binaries

We have provided pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 9.0, 10.0, 11.0, 12.0, 14.0 and 14.1, corresponding to marketing product names of Microsoft Visual Studio 2008, 2010, 2012, 2013, 2015 and 2017 respectively.
* TDM-GCC version 4.92, 5.1 and MinGW-w64 version 7.2 (with the default SJLJ exceptions propagation method, using C++11). Please note that you need to use the very latest MinGW-w64 7.2 compiler release which can be downloaded from [here for 32 bits](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/7.2.0/threads-win32/sjlj/i686-7.2.0-release-win32-sjlj-rt_v5-rev1.7z/download) and [here for 64 bits](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/7.2.0/threads-win32/seh/x86_64-7.2.0-release-win32-seh-rt_v5-rev1.7z/download), the older "rev0" release has a known bug affecting building wxWidgets in some scenarios.

### For Developers

For developing applications with wxWidgets you need to download the compiler-independent `wxWidgets-3.0.5_Headers.7z` file and one of `wxMSW-3.0.5-vcXXX_Dev.7z` or `wxMSW-3.0.5_gccXXX_Dev.7z` files depending on your compiler, its version and the target architecture (x86 if not specified or x64).

Unpack both files into the same directory so that `include` and `lib` directories are at the same level after unpacking. You should be able to compile and link applications using wxWidgets in both debug and release modes but the debug symbols are provided only for debug libraries in this archive, see below for the release build debug symbols.

### For End Users

End users may download one of `wxMSW-3.0.5_vcXXX_ReleaseDLL.7z` or `wxMSW-3.0.5_gccXXX_ReleaseDLL.7z` files to get just the DLLs required for running the applications using wxWidgets.

### For Debugging

* Microsoft Visual C++ users: Files `wxMSW-3.0.5_vcXXX_ReleasePDB.7z` contain the debug symbols for the release build of the DLLs. Download them if you want to debug your own applications in release build or if you want to get meaningful information from mini-dumps retrieved from your users machines.
* MinGW-TDM users: Currently the debug symbols are not available for the release build of the DLLs (only the debug versions of the DLLs contains the debug symbols).

### Binary File Download Verification

To verify your download please use the following sha1sums:

```
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc492TDM_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc492TDM_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc492TDM_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc492TDM_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc510TDM_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc510TDM_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc510TDM_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc510TDM_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc492TDM_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc492TDM_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc492TDM_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc492TDM_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc720_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc720_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc720_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_gcc720_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc90_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc90_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc90_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc90_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc90_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc90_x64_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc100_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc100_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc100_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc100_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc100_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc100_x64_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc110_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc110_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc110_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc110_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc110_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc110_x64_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc120_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc120_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc120_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc120_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc120_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc120_x64_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc140_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc140_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc140_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc140_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc140_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc140_x64_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc141_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc141_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc141_ReleasePDB.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc141_x64_Dev.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc141_x64_ReleaseDLL.7z
0000000000000000000000000000000000000000  wxMSW-3.0.5_vc141_x64_ReleasePDB.7z
```
