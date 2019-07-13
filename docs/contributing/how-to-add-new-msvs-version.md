How to add support for a new MSVS version
=========================================

MSVS versions
-------------

This is confusing because there are several of them:

- Marketing IDE/product version, which is the year of the release, e.g. 2019.
- Actual internal IDE version, visible in `Help|About` dialog, for example,
  e.g. 16.1.4.
- Compiler version, visible in `cl /?` output, e.g. 19.21.27702.2. This version
  also corresponds to the value of the predefined `_MSC_VER` symbol.
- Toolset version: this is similar to the compiler version, but uses different
  format, e.g. it's 14x for compiler version 19, with `x` being 0, 1 or 2
  depending on the minor compiler version (00, 14 or 21).

Moreover, historically, different IDE versions corresponded to different major
compiler versions, however this hasn't been the case any more for the last few
releases (2015, 2017, 2019 or 14, 15, 16 IDE versions) that all use the same
major compiler version (19) as they are ABI-compatible. Also, historically,
each compiler used its own toolset, but these last few versions can each use
any of 140, 141, 142 toolset.

wxWidgets uses the IDE version for the project/solution files (but notice that
project files are the same for all MSVS 201x versions) and the toolset version
for the names of build directories.


TODO list
---------

Here is a list of things that need to be done when adding support for the next
MSVS version:

- If the major compiler version changed, add a new `__VISUALCxx__` symbol to
  `include/wx/compiler.h` (`xx` here are the first 2 digits of the toolset
  version, i.e. basically correspond to the major compiler version, except
  that 13 was skipped for superstitious reasons, see the comment in that file).
- Add `wxCOMPILER_PREFIX` definition for the new version to
  `include/msvc/wx/setup.h`.
- Add the new solution file `build/msw/wx_vcNN.sln` (where `NN` is the internal
  IDE version), typically by opening an existing solution in the new IDE and
  letting it to update it.
- Discard changes to the project files that were done by the IDE during the
  update and instead update `build/msw/wx_config.props` and add a new condition
  checking `VisualStudioVersion` value for choosing the value of
  `PlatformToolset`.
- Update the lists of supported compiler versions in the different
  documentation files (search for "Microsoft Visual C++" to find the places
  that need updating):
  - `README.md`
  - `docs/doxygen/mainpages/introduction.h`
  - `docs/msw/install.md`
  - `docs/release.md`
- Update `build/tools/msvs/officialbuild.bat` to support the new toolset version
  and also update `build/tools/msvs/buildall.bat` to call it with this new version
  (these files are used to build the official binaries for the releases).
- If necessary, i.e. if the warning C4535 appears when compiling
  `src/msw/main.cpp` with the new version, check that this warning is still
  harmless, i.e. that catching Win32 exceptions still works correctly (this
  can be tested with the "Crash" menu item in the except sample) and  update
  the version check in `include/wx/msw/seh.h` to suppress this warning for the
  new version too.
