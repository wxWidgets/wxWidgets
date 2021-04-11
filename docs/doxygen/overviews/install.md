Installing wxWidgets    {#overview_install}
====================

wxWidgets headers and libraries must be available in order to build the
applications using it, so the first step when starting to use wxWidgets is to
install it. This can be done, for all platforms and library versions, by
building wxWidgets from sources, but for the most common platforms pre-built
binaries of the libraries are also provided, so the first decision to make is
whether to use these binaries or build the library yourself. Building the
libraries yourself allows you to compile the latest version using exactly the
options you need, and so is the most flexible solution, but using the binaries
may be simpler and faster -- the choice is yours, just follow the instructions
in the corresponding section below depending on the approach you prefer. Of
course, you only need to do one _or_ the other, _not_ both.


Using Binaries
--------------

How to install binaries depends on your platform:

- For Microsoft Windows (MSW), wxWidgets project provides official binaries
  on the [Downloads page](http://www.wxwidgets.org/downloads/), please see
  the [instructions for using them](@ref plat_msw_binaries).

- For Linux, and other free Unix-like systems, wxWidgets packages are available
  in system repositories under the name "wxGTK". Note that to develop
  applications using wxWidgets you may need to install the "development"
  packages and not just the libraries needed for running the applications using
  wxWidgets. For example, under Debian and Debian-derived systems such as
  Ubuntu, you need to run `apt get libwxgtkX.Y-dev`.

- For macOS, wxWidgets is available in third-party package managers such as
  [brew][1] or [MacPorts][2], and you can install them in the usual way.

[1]: https://brew.sh/
[2]: https://www.macports.org/


Additionally, some third-party C++ package managers also provide wxWidgets
binaries. For example, please see [this post][3] for the instructions about
using [vcpkg][4] C++ package manager for installing wxWidgets.

[3]: https://www.wxwidgets.org/blog/2019/01/wxwidgets-and-vcpkg/
[4]: https://github.com/microsoft/vcpkg


Building from Source
--------------------

### Getting the sources

To build the library you need to get its sources first. The recommended way to
do it is to use Git to check them out from the official wxWidgets repository
using the following command:

    $ git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git

Alternatively, you can download the sources from the [downloads page][1].
Please note that all the source archives in different formats (ZIP, 7z,
tar.bz2) contain the same files, but use different line ending formats: Unix
("LF") for the latter one and DOS ("CR LF") for the two other ones, and it is
usually preferable to choose the format corresponding to the current platform.
When downloading the sources with DOS ends of lines, prefer 7z format for much
smaller file size.


### Selecting the build system

wxWidgets can be built using [CMake](https://cmake.org/) under all platforms.
Please follow [CMake build instructions](@ref overview_cmake) if you prefer to
use it.

Otherwise, please use the appropriate instructions depending on your platform:

- For native wxMSW port under [Microsoft Windows](@ref plat_msw_install).
- For wxGTK under [Linux and other Unix-like systems](@ref plat_gtk_install).
- For native wxOSX port under [macOS](@ref plat_osx_install).

The wxWidgets ports mentioned above are the main ones, however other variants
also exist, see [platform details](@ref page_port) page for the full list.


Verifying the Installation
--------------------------

After installing wxWidgets, it is recommended to check that the library can
actually be used by building a minimal wxWidgets program using it, such as
`samples/minimal/minimal.cpp` included in wxWidgets distributions.

You may choose to build this program in any of the following ways:

- Directly from command line under any Unix-like systems, including macOS and
  Unix-like environments such as Cygwin or MSYS2 under MSW, just run

    $ c++ -o minimal minimal.cpp `wx-config --cxxflags --libs`

  Please note that you *must* use `wx-config` to obtain the compiler and linker
  flags in this case.

- Using CMake, and the provided `samples/minimal/CMakeLists.txt` file.

- Using Microsoft Visual Studio IDE: simply create a new project and add the
  provided `wxwidgets.props` property sheet file to it as explained in the
  [instructions](@ref #msw_build_apps) and build the project as usual.

- Using another IDE with wxWidgets support, such as [Code::Blocks][1] or
  [CodeLite][2], please use the IDE wizards.

[1]: https://www.codeblocks.org/
[2]: https://codelite.org/

- If you use another IDE, under Unix you should run `wx-config --cxxflags` and
  `wx-config --libs` commands separately and copy-and-paste their output to the
  "Additional preprocessor options" and "Additional linker options" fields in
  your IDE, respectively. Under MSW systems you need to configure the IDE using
  the instructions in the ["manual setup"](@ref #msw_build_apps) section.
