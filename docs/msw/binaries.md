How to use wxMSW binaries              {#plat_msw_binaries}
=========================

[TOC]

Supported Compilers
-------------------
We provide pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 9.0, 10.0, 11.0, 12.0, 14.0, 14.1 and 14.2
  (corresponding to marketing product names of Microsoft Visual Studio 2008, 2010, 2012, 2013, 2015, 2017 and 2019 respectively). Please note that MSVC 14.x versions are ABI-compatible and the same set of binaries is used for all of them.
* MinGW-w64 versions 7.3 and 8.1 (32-bit binaries use SJLJ exceptions, 64-bit ones use SEH, and all binaries use Win32 threads).
* [TDM-GCC](https://jmeubank.github.io/tdm-gcc/) 9.2.0.

Getting the files
-----------------

First, you need to get the correct files. You will always need the
`wxWidgets-3.1.5-headers.7z` one but the rest depends on your compiler version
and architecture: as different versions of MSVC compiler are not binary
compatible, you should select the files with the correct
`vc80`, `vc90`, `vc100`, `vc110`, `vc120`, or `vc14x`
suffix depending on whether you use
Visual Studio 2005, 2008, 2010, 2012, 2013, or 2015/2017/2019 respectively (the Visual Studio 2015/2017/2019 compilers are binary compatible).
You also need to decide whether you use the `x64` files for 64-bit development
or the ones without this suffix for the still more common 32-bit builds. After
determining the combination of suffixes you need, you should download the
"Dev" and the "ReleaseDLL" files in addition to the "Headers" one above,
e.g. for 32-bit MSVS 2017 development you need
`wxMSW-3.1.5_vc14x_Dev.7z` and `wxMSW-3.1.5_vc14x_ReleaseDLL.7z`.

All binaries are available at:

https://www.wxwidgets.org/downloads#v3.1.5_msw

Once you have the files you need, unzip all of them into the same directory, for
example `c:\wx\3.1.5`. You should have only include and lib subdirectories under
it, nothing else. To avoid hard-coding this path into your projects, define
`wxwin` environment variable containing it: although it's a little known fact,
all versions of MSVC support environment variable expansion in the C++ projects
(but not, unfortunately, in the solution files).

Using Binaries with Visual Studio
---------------------------------

Next step is to set up your project to use these files: for this, simply add
the provided `wxwidgets.props` file as a property sheet to your project.
This can be done using `View|Property Manager` menu item in the IDE and
selecting "Add Existing Property Sheet..." from the project popup menu.

Note: your project must use "Unicode Character Set" option.

Now you should be able to build and run your project successfully, both in
"Debug" and "Release" configurations. Please note that during run-time the
executables will require wxWidgets DLLs in addition to MSVC run-time DLLs, so
you should consider adding the directory containing these DLLs to your PATH and
either distributing them with your application or instructing your users to
download them.

Building samples with nmake
---------------------------

When using `makefile.vc` files for building wxWidgets samples using `nmake`
from command line, you need to use `SHARED=1` and also define `COMPILER_PREFIX`
appropriately, e.g. the full command line could be

    > nmake /f makefile.vc BUILD=release SHARED=1 COMPILER_PREFIX=vc14x
