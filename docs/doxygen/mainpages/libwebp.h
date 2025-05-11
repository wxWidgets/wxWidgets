/////////////////////////////////////////////////////////////////////////////
// Name:        libwebp.h
// Purpose:     Documentation of the use of libwebp with wxWidgets
// Author:      Hermann Höhne <hoehermann@gmx.de>
// Created:     2024-03-08
// Copyright:   (c) Hermann Höhne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page page_build_libwebp wxImage support for the WebP image format with libwebp

To make use of the wxWEBPHandler, libwebp library is required when building wxWidgets.
libwebp is available under a BSD license.

This guide explains how to obtain libwebp and let wxWidgets build system use it.
The exact steps depend on the operating system and compiler used, please refer to the
section appropriate for your environment below.

@section page_build_libwebp_unix Unix-like operating system

Under many Unix or Unix-like systems, libwebp is available as a system package
and the simplest thing to do is to just install it using the system-specific tool
(apt, pacman, ...). Just note that you may need to install the libwebp-dev
package in order to obtain the library headers and not just the library itself.

Building libwebp from source is not part of this guide.

@subsection page_build_libwebp_configure Use libwebp with configure

Provided, @c pkg-config is available and the appropriate files are in the default
locations, @c ./configure will automatically detect the availability of libwebp.

@subsection page_build_libwebp_cmake Use libwebp with CMake

When using CMake, the built-in libwebp is enabled by default. To use the system version,
add @c -DwxUSE_LIBWEBP=sys to the commandline or change it in the GUI.
Use @c -DwxUSE_LIBWEBP=OFF to disable libwebp.

Note: At time of writing, the @c WebPConfig.cmake file supplied by libwebp
is affected by a <a href="https://chromium-review.googlesource.com/c/webm/libwebp/+/4868215">bug</a>.
You may need to add the line <tt>set(WebP_INCLUDE_DIRS ${WebP_INCLUDE_DIR})</tt>
locally.

@subsection page_build_libwebp_msw_vcpkg_msvc MSW with vcpkg and MSVC

You can use Microsoft vcpkg tool (see https://github.com/Microsoft/vcpkg) to
build and install libwebp:

@code
    > git clone https://github.com/Microsoft/vcpkg.git
    > .\vcpkg\bootstrap-vcpkg.bat
    > .\vcpkg\vcpkg.exe install libwebp:x64-windows-static
@endcode

You can then proceed to configure wxWidgets with CMake for building with MSVC,
see @ref page_build_libwebp_cmake.

Dynamic builds have not been tested. x86 builds have not been tested.

@subsection page_build_libwebp_msw_vcpkg_clang MSW with vcpkg and clang

Refer to @ref page_build_libwebp_msw_vcpkg_msvc, but select @c x64-mingw-static
instead of @c x64-windows-static . Then proceed as documented in
@ref page_build_libwebp_cmake or @ref page_build_libwebp_configure . CMake can work
as well as configure since vcpkg provides libwebp with support for both systems.
*/
