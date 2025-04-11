How to use wxMSW binaries              {#plat_msw_binaries}
=========================

[TOC]

Supported Compilers
-------------------
We provide pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 14.0, 14.1, 14.2 and 14.3
  (corresponding to marketing product names of Microsoft Visual Studio 2015, 2017, 2019 and 2022 respectively).
  Please note that MSVC 14.x versions are ABI-compatible and the same set of binaries is used for all of them.
* MinGW-w64 versions 7.3 and 8.1 (32-bit binaries use SJLJ exceptions, 64-bit ones use SEH, and all binaries use Win32 threads).
* [TDM-GCC](https://jmeubank.github.io/tdm-gcc/) 10.3.0.
* [MSYS2](https://www.msys2.org/) MinGW 12.2.0 and 13.2.0.

Getting the files
-----------------

First, you need to get the correct files. You will always need the
`wxWidgets-3.3.0-headers.7z` one but the "Dev" and the "ReleaseDLL"
files depend on your compiler and CPU architecture.
For example, for MSVS 2015/2017/2019/2022 and 64-bit (x86_64) architecture
you will need `wxMSW-3.3.0_vc14x_x64_Dev.7z` and `wxMSW-3.3.0_vc14x_x64_ReleaseDLL.7z`.
If you are using MinGW-w64 v8.1 and 32-bit (x86) architecture, you will need
`wxMSW-3.3.0_gcc810_Dev.7z` and `wxMSW-3.3.0_gcc810_ReleaseDLL.7z` (32-bit
packages do not include the architecture suffix in their file name).

All binaries are available at:

https://www.wxwidgets.org/downloads#v3.3.0_msw

Once you have the files you need, unzip all of them into the same directory, for
example `c:\wx\3.3.0`. You should have `include` and `lib` subdirectories under
this directory, as well as files such as `wxwidgets.props`.

Note: To avoid hard-coding this path into your projects, define `wxwin`
environment variable containing it: although it's a little known fact, all
versions of MSVC support environment variable expansion in the C++ projects
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
