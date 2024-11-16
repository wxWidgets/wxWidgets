How to add a new sample to wxWidgets.
=====================================

To add a new sample "foo" under directory "samples/foo" you need to do
the following: (note that all this applies also to demos: just replace
samples/ with demos/ where needed).

1. Create the directory samples/foo.

2. "git add" the sample sources (usually based on minimal.cpp) as well.

   Note that unless your sample requires something special you shouldn't
   add foo.rc, foo.ico, ... any more, please just reuse the generic `sample.*`
   files in the samples subdirectory. In particular, the Windows resource
   specification should use the central `.rc` file, i.e. do not use
   `<win32-res>` bakefile tag unless it is really necessary.

3. Create the make and project files:

 - Create MSVS project for the sample and add it to the solution file:
   * In the simplest case, run the following command, which requires some
     version of Perl to be available, from the root of the wxWidgets source
     tree:

     ./build/clone-minimal-vcxproj samples/foo/foo

   * If you need to base the sample on some other project, e.g. console one
     for a new non-GUI sample, you can alternatively run

     ./build/vcxproj-clone samples/console/console.vcxproj samples/foo/foo

   * In either case, open the `samples/samples_vc17.sln` and move the new
     project under the appropriate folder in the solution tree.

   * The following one-liner can be used to update all the other solution files
     (but you may also update them manually if you can't use this command,
      which requires Unix-like environment):

     $ for v in 14 15 16; do
         git diff samples/samples_vc17.sln | \
         sed "s/samples_vc17.sln/samples_vc${v}.sln/g" | \
         git apply -v
       done

   - modify samples/samples.bkl (just copy an existing line)
   - create `foo.bkl`, typically by just copying an existing bakefile from
   another sample (`sed 's/minimal/foo/g' minimal.bkl > foo.bkl` is usually
   enough, but you may need to add more `<wx-lib>` lines if your sample uses
   anything not in the core lib)
   - add `<wx-data>` element with the data files used by the sample that need
   to be copied to its build directory, if any (see e.g.
   `samples/image/image.bkl` for an example of using this tag).
   - generate the makefiles for your sample using bakefile. For this you
   need to:
      - install bakefile (see `how-to-add-files-to-build-system.md`)
      - run `bakefile_gen` in `build/bakefiles` which will regenerate all
        makefiles which are not up to date

      You may also use `bakefile_gen -b ../../samples/foo/foo.bkl` to
      regenerate only the makefiles for this sample (path must be relative!)
      or even add a `-f<compiler>` to generate just the makefiles for the given
      compiler (run `bakefile --help` to get the list of possible values).
      Again, see `how-to-add-files-to-build-system.md` for more information.

4. Modify configure.ac Unix compilation:
   - if the sample should only be built if `wxUSE_FOO` is enabled, locate
      the test for `wxUSE_FOO = yes` in configure.ac and add a line
      `SAMPLES_SUBDIRS="$SAMPLES_SUBDIRS foo"` under it
   - if it should be always built, locate the line `if test $wxUSE_GUI = yes`
     near the end of configure.ac and modify the assignment to
     `SAMPLES_SUBDIRS` to include "foo" (put in alphabetical order)

   After this, regenerate configure from configure.ac
   by running "autoconf" on a Unix system in the corresponding directory.

5. Modify `build/cmake/samples/CMakeLists.txt` to include the sample in
   CMake. Add a new line like this:

        wx_add_sample(foo DEPENDS wxUSE_FOO)

   Use `DATA` CMake variable if the sample uses any data files.

   For a complete list of parameters to the `wx_add_sample()` function see
   the description in `build/cmake/functions.cmake`.

6. Add a short description of what the sample does and how does it work
   to the "samples overview" section in the wxWidgets manual. That section
   lives in `docs/doxygen/mainpages/samples.h`; look at the descriptions for
   other samples, if you are not familiar with Doxygen.

7. Add any non-standard sample's files to `build/bakefiles/make_dist.mk` (the
   makefiles copies all bmp, cpp, h, ico, png, rc, xpm and makefiles by
   default, you only need to update it if the sample uses some other files)
   and run the `./update-manifests.sh` script in distrib/scripts.
