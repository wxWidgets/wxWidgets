How to use wxMSW binaries              {#plat_msw_binaries}
=========================

[TOC]

Supported Compilers
-------------------
We provide pre-built binary files for the following compilers:

* Microsoft Visual C++ compiler versions 9.0, 10.0, 11.0, 12.0, 14.0 and 14.1
  (corresponding to marketing product names of Microsoft Visual Studio 2008, 2010, 2012, 2013, 2015 and 2017 respectively).
* TDM-GCC version 5.1 and MinGW-w64 version 7.2 (with the default SJLJ
  exceptions propagation method, using C++11). Please note that you need to use
  the very latest MinGW-w64 7.2 compiler release with this version of the
  compiler which can be downloaded from
  [here for 32 bits](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/7.2.0/threads-win32/sjlj/i686-7.2.0-release-win32-sjlj-rt_v5-rev1.7z/download)
  and
  [here for 64 bits](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/7.2.0/threads-win32/seh/x86_64-7.2.0-release-win32-seh-rt_v5-rev1.7z/download),
  the older "rev0" release has a known bug affecting building wxWidgets in
  some scenarios.


Getting the files
-----------------

First, you need to get the correct files. You will always need the
`wxWidgets-3.1.1-headers.7z` one but the rest depends on your compiler version
and architecture: as different versions of MSVC compiler are not binary
compatible, you should select the files with the correct
`vc80`, `vc90`, `vc100`, `vc110`, `vc120`, `vc140` or `vc141`
suffix depending on whether you use
Visual Studio 2005, 2008, 2010, 2012, 2013, 2015 or 2017 respectively.
You also need to decide whether you use the `x64` files for 64-bit development
or the ones without this suffix for the still more common 32-bit builds. After
determining the combination of suffixes you need, you should download the
"Dev" and the "ReleaseDLL" files in addition to the "Headers" one above,
e.g. for 32-bit MSVS 2017 development you need
`wxMSW-3.1.1_vc141_Dev.7z` and `wxMSW-3.1.1_vc141_ReleaseDLL.7z`.

All binaries are available at:

https://github.com/wxWidgets/wxWidgets/releases/v3.1.1

Once you have the files you need, unzip all of them into the same directory, for
example `c:\wx\3.1.1`. You should have only include and lib subdirectories under
it, nothing else. To avoid hard-coding this path into your projects, define
`wxwin` environment variable containing it: although it's a little known fact,
all versions of MSVC support environment variable expansion in the C++ projects
(but not, unfortunately, in the solution files).

Using Binaries with Visual Studio
---------------------------------

Next step is to set up your project to use these files. You need to do the
following:

*   In the compiler options, i.e. "C/C++" properties:
    *   Add `$``(wxwin)/include/msvc;$``(wxwin)/include` to the "Additional Include
        Directories". Notice that the order is important here, putting the
        MSVC-specific directory first ensures that you use `wx/setup.h`
        automatically linking in wxWidgets libraries.
    *   Add `WXUSINGDLL` and `wxMSVC_VERSION_AUTO` to the list of defined
        symbols in "Preprocessor Definitions". The first should be
        self-explanatory (we only provide DLLs, not static libraries) while the
        second one is necessary to use the libraries from e.g. `lib\vc100_dll`
        directory and not the default `lib\vc_dll`.
    *   Also check that `_UNICODE` and `UNICODE` symbols are defined in the same
        "Preprocessor Definitions" section. This should already be the case for
        the newly created projects but it might be necessary to add them if
        you're upgrading an existing one.
    *   Check that you use "Multi-threaded \[Debug\] DLL" in the "Run-time
        library" option under "Code Generation" to ensure that your build uses
        the same CRT version as our binaries.
* In the linker options you only need to add `$``(wxwin)\lib\vc$``(PlatformToolsetVersion)_dll`
  (the standard `PlatformToolsetVersion` macro expands into something like
  `141`, depending on the toolset version being used, and you could also use
  the version number directly if you prefer) to "Additional Library
  Directories" under "Linker\\General" in the options. Thanks to the use of
  MSVC-specific `setup.h` you don't need to list wxWidgets libraries manually,
  i.e. you do **not** need to put anything in the list of "Additional
  Dependencies".

Now you should be able to build your project successfully, both in "Debug" and
"Release" configurations. With MSVS 10 or newer it can also be done from the
command line using `msbuild.exe`. Of course, to run the generated executable
you will need to either add the directory containing wxWidgets DLLs to your PATH
or copy the DLL files to a directory already on it. Finally, if you want to
distribute the binaries created using these options, you will need to install
Microsoft Visual C++ run-time DLLs. Again, MSVC 10 or newer has an advantage
here as you can simply copy `msvcp100.dll` and `msvcr100.dll` as any other DLL,
while you need to install specially for the previous compiler versions that
use WinSxS ("side-by-side") for them.
