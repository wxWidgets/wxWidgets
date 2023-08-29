How to add a new `wxUSE_XXX` preprocessor constant
================================================

Purpose
-------

Detailed description of what needs to be done when you want to add a new
`wxUSE_XXX` compilation flag. The text below assumes you need new `wxUSE_FOO`.


Overview
--------

wxWidgets uses `wxUSE_XXX` macros for conditionally compiling in (or not)
optional components. In general, whenever a new non critical (i.e. not
absolutely required by important parts of the library) class Foo is added it
should use its own `wxUSE_FOO` compilation flag.

`wxUSE_FOO` must be always defined and have value of 0 or 1. Be careful with
testing for it in wx/foo.h: don't do it at the very beginning of the file
because then `wxUSE_FOO` would be not defined at all if the user directly
includes wx/foo.h, include "wx/defs.h" before testing for `wxUSE_FOO`.


Files to update
---------------

The following files need to be modified when adding a new `wxUSE_FOO`:

- `include/wx/setup_inc.h`

   This file contains all common `wxUSE_XXXs`, and is used to update wxMSW, wxMac
   setup.h and Unix setup.h.in using build/update-setup-h. Please try to add
   the new define in a logical place (i.e. near any related ones) and write a
   detailed comment explaining what does it do and why would you want to turn
   it on/off. Choose the appropriate default value: this should be usually 1
   but can be 0 if there are some problems preventing the use of Foo by default
   (e.g. it requires installation of some non standard 3rd party libraries).
   After changing this file, run the update-setup-h script (this is probably
   better done on a Unix machine although it should work under Cygwin too).

- `include/wx/msw/setup_inc.h` for MSW-specific options

   This file contains MSW-specific options, so if the new option is only used
   under MSW, add it here instead of include/wx/setup_inc.h. The rest of the
   instructions is the same as above.

- `include/wx/chkconf.h`

   Add the check for `wxUSE_FOO` definedness in the corresponding (base or GUI)
   section. Please keep the alphabetic order.

   If there are any dependencies, i.e. `wxUSE_FOO` requires `wxUSE_BAR` and
   `wxUSE_BAZ`, check for them here too.

- `include/wx/msw/chkconf.h` for MSW-specific options

   These options won't be defined for the other ports, so shouldn't be added to
   the common `include/wx/chkconf.h` but to this file instead.

- `configure.ac`

   Here you need to add `DEFAULT_wxUSE_FOO` define. It should be added in the
   block beginning after `WX_ARG_CACHE_INIT` line and should default to "no" for
   `if DEBUG_CONFIGURE = 1` branch (this is used for absolutely minimal builds)
   and the same as default value in `setup_inc.h` in the "else" branch.

   You also need to add a `WX_ARG_ENABLE` (or, if new functionality can be
   reasonably described as support for a 3rd party library, `WX_ARG_WITH`)
   line together with all the existing `WX_ARG_ENABLE`s.

   If you have a sample/foo which should be only built when `wxUSE_FOO==1`,
   then only add it to the `SAMPLES_SUBDIRS` if `wxUSE_FOO=yes` in configure.

- `build/cmake/options.cmake`

   To include the option in CMake, add a new line in the appropriate
   section of `options.cmake`.

       wx_option(wxUSE_FOO "enable FOO")

   As an optional third parameter you may specify `OFF` when the option
   should be disabled by default.

- `docs/doxygen/mainpages/const_wxusedef.h`

   Add a brief description of the new constant.
