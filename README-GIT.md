Building from Git Sources
-------------------------

Please use `--recurse-submodules` option with the initial `git clone`. If you
had already cloned the repository without this option, please run `git
submodule update --init` to also get all 3rd party libraries code.

Otherwise, building from Git checkout is not different from building
from the sources include into the releases, so please follow the usual
instructions in the port-specific files, such as
[wxMSW](docs/msw/install.md), [wxGTK](docs/gtk/install.md),
[wxOSX](docs/osx/install.md) and so on.
