wxWidgets for Classic Mac OS (Retro68)                         {#plat_retro68_install}
======================================

wxRetro68 is a port of wxWidgets to classic Mac OS (System 7 to Mac OS 9)
built with the [Retro68](https://github.com/autc04/Retro68) GCC-based cross
compiler. Both 68K (`m68k-apple-macos`) and PowerPC (`powerpc-apple-macos`)
targets are supported.

The port uses the native Window Manager windows (with native title bars, close,
zoom and size boxes), the native menu bar (Apple menu with desk accessories and
a minimal "File" menu with "Quit") and QuickDraw for all drawing, while the
controls themselves are provided by wxUniversal.

[TOC]

Requirements                                      {#plat_retro68_req}
------------

- Retro68 toolchain, by default installed in `/opt/retro68`.
- CMake 3.16 or later.
- At run time: System 7.0 or later (Mac OS 8 or later is recommended) with
  32-bit QuickDraw, i.e. offscreen GWorld support. Both colour and monochrome
  displays are supported.

Building                                          {#plat_retro68_build}
--------

Only the CMake build system is supported. Use the toolchain file provided by
Retro68 for the target you want to build for:

    # 68K
    cmake -S . -B build-m68k -G Ninja -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_TOOLCHAIN_FILE=/opt/retro68/m68k-apple-macos/cmake/retro68.toolchain.cmake
    cmake --build build-m68k

    # PowerPC (classic Toolbox)
    cmake -S . -B build-ppc -G Ninja -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_TOOLCHAIN_FILE=/opt/retro68/powerpc-apple-macos/cmake/retroppc.toolchain.cmake
    cmake --build build-ppc

Use `-DwxUSE_GUI=OFF` to build wxBase only. Only static libraries are
supported. The default values of the build options are adjusted for this
platform in `build/cmake/retro68.cmake`, e.g. threads, sockets and other
unsupported features are disabled.

Samples can be built with `-DwxBUILD_SAMPLES=SOME`, each of them produces
`<name>.bin` (MacBinary file), `<name>.APPL` and `<name>.dsk` (HFS disk image
containing the application, which can be used directly with emulators such as
Basilisk II or SheepShaver).

Building applications                             {#plat_retro68_apps}
---------------------

When using CMake, link with the wxWidgets targets as usual and use Retro68
`add_application()` function to create the application, passing it
`src/retro68/wxretro68.r` file from wxWidgets sources: it defines the `SIZE`
resource with the memory partition sizes and flags required by wxWidgets
applications (by default, 16MB preferred and 8MB minimum partition size, which
can be changed by predefining `WX_RETRO68_PREFERRED_SIZE` and
`WX_RETRO68_MINIMUM_SIZE`). It is strongly recommended to compile with
`-ffunction-sections -fdata-sections` and link with `-Wl,--gc-sections` to
reduce the size of the executable.

Note that under 68K wxWidgets applications need a big stack, so the stack is
enlarged by 512KB when wxApp is initialized. Define
`wxRETRO68_EXTRA_STACK_SIZE` when building wxWidgets to change this.

Platform notes                                    {#plat_retro68_notes}
--------------

- File paths use the classic Mac OS format, i.e. `Volume:Folder:File`, and
  wxFileName uses wxPATH_MAC format by default. The file names are in Mac OS
  Roman encoding, which is also the system encoding used by wxConvLibc.
- As Retro68 runtime only implements opening, reading, writing and seeking in
  files, wx provides its own File Manager-based implementations of stat(),
  mkdir(), rmdir(), rename(), remove() and other functions, see
  `wx/retro68/crt.h`.
- wxStandardPaths uses the Folder Manager to return the standard folders
  (Preferences, Temporary Items etc.).
- wxFileDialog and wxDirDialog use the Standard File Package dialogs. The
  directory dialog uses the "Save" dialog in which the user chooses the folder.
- Command key is mapped to wxMOD_CONTROL, so that the standard accelerators
  such as "Ctrl-C" correspond to "Command-C", while Control key is mapped to
  wxMOD_META. Control-click generates right mouse button events and
  wxEVT_CONTEXT_MENU.
- Apple events for opening and printing documents are forwarded to
  wxApp::MacOpenFiles() and wxApp::MacPrintFile() and the "Quit" Apple event
  closes all the windows.

Limitations                                       {#plat_retro68_limits}
-----------

The following features are not available yet: threads, sockets and networking,
clipboard and drag and drop, tooltips, printing, wxGraphicsContext, rotated
text and dynamic loading. Menus of the application windows are drawn by
wxUniversal inside the windows rather than using the native menu bar. Only
the Roman script system is supported.
