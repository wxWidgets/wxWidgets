How to use wxMSW binaries              {#plat_msw_binaries}
=========================

[TOC]

Supported Compilers
-------------------

We provide pre-built binary files for x86-64 Windows for the following compilers:

* Microsoft Visual C++ compiler versions 14.0, 14.1, 14.2 and 14.3
  (corresponding to marketing product names of Microsoft Visual Studio 2015, 2017, 2019 and 2022 respectively).
  Please note that MSVC 14.x versions are ABI-compatible and the same set of binaries is used for all of them.
* gcc 15.1 provided by [MSYS2](https://www.msys2.org/) MINGW64 environment.

Getting the files
-----------------

First, you need to get the correct files. You will always need the
`wxWidgets-3.3.0-headers.7z` one but the "Dev" and the "ReleaseDLL"
files depend on your compiler: for example, for MSVS you will need
`wxMSW-3.3.0_vc14x_x64_Dev.7z` and `wxMSW-3.3.0_vc14x_x64_ReleaseDLL.7z`, but
if you are using gcc, you will need `wxMSW-3.3.0_gcc1510_x64_Dev.7z` and
`wxMSW-3.3.0_gcc1510_x64_ReleaseDLL.7z`.

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
