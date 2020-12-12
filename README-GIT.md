Building from Git Sources
-------------------------

Please use `--recurse-submodules` option with the initial `git clone`. If you
had already cloned the repository without this option, please run `git
submodule update --init` to also get all 3rd party libraries code.

If you get errors about "wxUSE_XXX must be defined" when building, it
probably means that you need to copy the new options added to the file
`include/wx/msw/setup0.h` to `include/wx/msw/setup.h`. If you had never
modified the latter, it's safe to just delete it, as it will be created
by copying `setup0.h` during the next build.

Otherwise, building from Git checkout is not different from building
from the sources include into the releases, so please follow the usual
instructions in the port-specific files, such as
[wxMSW](docs/msw/install.md), [wxGTK](docs/gtk/install.md),
[wxOSX](docs/osx/install.md) and so on.
