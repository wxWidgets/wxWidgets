How to add a new sample to wxWidgets.
=====================================

To add a new sample "foo" under directory "samples/foo" you need to do
the following: (note that all this applies also to demos: just replace
samples/ with demos/ where needed).

1. Create the directory samples/foo.

2. "git add" the sample sources (usually based on minimal.cpp) as well.

   Note that unless your sample requires something special you shouldn't
   add foo.rc, foo.ico, ... any more, please just reuse the generic `sample.*`
   files in the samples subdirectory.

3. Create the makefiles:
   - modify samples/samples.bkl (just copy an existing line)
   - create foo.bkl (`sed 's/minimal/foo/g' minimal.bkl > foo.bkl` is usually
   enough, but you may need to add more `<wx-lib>` lines if your sample uses
   anything not in the core lib)
   - generate the makefiles for your sample using bakefile. For this you
   need to:
      - install bakefile (see docs/contributing/how-to-add-files-to-build-system.md)
      - run bakefile_gen in build/bakefiles which will regenerate all
        makefiles which are not up to date

      You may also use `bakefile_gen -b ../../samples/foo/foo.bkl` to
      regenerate only the makefiles for this sample (path must be relative!)
      or even add a `-f<compiler>` to generate just the makefiles for the given
      compiler (run `bakefile --help` to get the list of possible values).
      See how-to-add-files-to-build-system.md for more information.

4. Modify configure.in Unix compilation:
   - if the sample should only be built if `wxUSE_FOO` is enabled, locate
      the test for `wxUSE_FOO = yes` in configure.in and add a line
      `SAMPLES_SUBDIRS="$SAMPLES_SUBDIRS foo"` under it
   b) if it should be always built, locate the line `if test $wxUSE_GUI = yes`
      near the end of configure.in and modify the assignment to
      `SAMPLES_SUBDIRS` to include "foo" (put in alphabetical order)

   After this, regenerate configure from configure.in
   by running "autoconf" on a Unix system in the corresponding directory.

5. Modify `build/cmake/samples/CMakeLists.txt` to include the sample in
   CMake. Add a new line like this:

       wx_add_sample(foo DEPENDS wxUSE_FOO)

   For a complete list of parameters to the `wx_add_sample()` function see
   the description in `build/cmake/functions.cmake`.

6. Add a short description of what the sample does and how does it work
   to the "samples overview" section in the wxWidgets manual. That section
   lives in docs/doxygen/mainpages/samples.h; look at the descriptions for
   other samples, if you are not familiar with Doxygen.

7. Add any non-standard sample's files to `build/bakefiles/make_dist.mk` (the
   makefiles copies all bmp, cpp, h, ico, png, rc, xpm and makefiles by
   default, you only need to update it if the sample uses some other files)
   and run the ./update-manifests.sh script in distrib/scripts.
