Building from Git Sources
-------------------------

Please use `--recurse-submodules` option with the initial `git clone`. If you
had already cloned the repository without this option, please run `git
submodule update --init` to also get all 3rd party libraries code.

Before building from Git sources under Windows, you need to copy the
file `include/wx/msw/setup0.h` to `include/wx/msw/setup.h`. This is
necessary in order to allow having local modifications to the latter
file without showing it as modified, as it is not under version
control.

After doing this, please proceed with the build as with any official
release, i.e. follow the instructions in the port-specific files, e.g.
[wxMSW](docs/msw/install.md), [wxGTK](docs/gtk/install.md),
[wxOSX](docs/osx/install.md) and so on.
