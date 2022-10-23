# Making a New wxWidgets Release

## Checking ABI Compatibility

For the stable (even) releases only, check that binary compatibility hasn't
been broken since the last stable release.

### Checking under Unix systems using `libabigail`.

Instructions:

1. Get [the tools](https://sourceware.org/libabigail/). Under Debian and
   derived systems `apt install abigail-tools` can be used.
1. Build the old (vX.Y.Z-1) library with `-g` option, i.e. configure it
   with `--enable-debug`. For convenience, let's assume it's built in "$old"
   subdirectory.
1. Build the new (vX.Y.Z) library with the same options in "$new".
1. Create directories for temporary files containing the ABI dumps for the old
   and new libraries: `mkdir -p ../compat/{$old,$new}`.
1. Run `abidw` on all libraries: `for l in $old/lib/*.so; do abidw $l
   --out-file ../compat/$old/$(basename $l).abi; done` and the same thing with
   the new libraries.
1. Run `abidiff` on each pair of produced dumps to generate HTML
   reports: `for l in $old/*.abi; do abidiff $l -new $new/$(basename $l); done`.
1. If everything is good, update the ABI files in `$old` with the `$new` ones.

See also `build/elfabi/check_all.sh` which checks the ABI of the newly built
libraries and is simpler to use if there is no need to update the ABI files.

### Checking under MSW systems.

Manually check compatibility by building the widgets samples from the old tree
and then run it using the new DLLs.

## Requesting to Update the Translations

Post to wx-translators@googlegroups.com to ask to update the translations
before the release. This needs to be done some time before making it, of
course.

## Pre-Release Steps

Start by copying all the changes since the previous release to the change log
file as explained in the comment there.

Then update the files below. You can run `build/tools/pre-release.sh` to do
the straightforward changes like updating the dates and checksums
automatically, but please also review and update the contents of the README
and announcement text.

The Post-Release step of the previous release will have updated
the micro version of this release. If this release represents a major
or minor release, these changes will have to be performed manually at
this point.

Note that the best order depends on the release being prepared: for a
development release, `docs/publicity/announce.txt` contains the list of the
major changes since the last stable release and should be updated first, as
this part of it can then be copied verbatim to the corresponding section of
the README file. For the stable releases, it's probably more convenient to
update the README with the details of the changes first.

Here is the list of the files, for reference:
* Update `docs/readme.txt`: version needs to be changed, content updated.
* Update `docs/release.md`: also version and reset SHA-1 sums to zeroes.
* Update `docs/changes.txt`: update the date on the release line.
* Update the date in the manual (`docs/doxygen/mainpages/manual.h`).
* Update the release announcement post in `docs/publicity/announce.txt`.
* Update `docs/msw/binaries.md`: at least the version, but possibly also
  the list of supported compilers.

Commit the changes and tag the release using your GPG key:

    git tag -s -m 'Tag X.Y.Z release' vX.Y.Z

   Don't overwrite existing tags. For non-final releases use e.g. `X.Y.Z-rc1`
   instead of `X.Y.Z`.

## Creating Release On GitHub

Go to https://github.com/wxWidgets/wxWidgets/actions/workflows/make_release.yml
and use the "Run workflow" button to manually run this workflow for the
appropriate branch (either `master` or `3.2` currently). This will create a new
draft release that can be found in the release list or you can see its exact
URL in the output of the "Add Files to the Release" workflow step.

On the release page, use the "Edit" button to manually move the checksums at
the very end of the announcement text to their correct locations (i.e. replace
the all zero checksums with the actual ones).

Also review the announcement for correctness.

Build and upload the binaries to the existing release.

Finally, publish it.

## Update documentation

This requires being able to ssh to docs.wxwidgets.org, please ask Bryan if you
think you should be able to do it, but can't.

Once logged in, copy the contents of either `latest` or `stable` directory to
`public_html/x.y.z`, switch any links, such as `3.1` to point to `x.y.z` by
doing

    $ cd ~/public_html
    $ ln -sfn 3.y.z 3.y

and edit `~/public_html/index.html` to add the link to the new release to it.

If the docs must be generated from the tag itself, run the documentation
generation workflow on GitHub manually providing the tag before doing the
above.

Note that the docs web site currently uses Cloudflare for caching, which means
that it won't update for several hours after the change, unless you purge the
cache manually in the Cloudflare console (which requires an account).

## Announcement

Update https://www.wxwidgets.org:
* Update release information (at least `version` and `released`) in `_data/releases.yml`.
* Update the list of compilers used for making MSW binaries in
  `downloads/index.md` if necessary (note that there is no need to update
  anything else, the page will dynamically show the release files with the
  specified prefixes).
* Add a news item. Usually a news item is enough but something
  more can be called for for major releases
* Push the changes (or create the PR with them) to GitHub. Note that this will
  trigger the site rebuild which will fail if the release statistics are not
  available yet, so make sure to publish the release on GitHub first (or wait
  an hour for the next scheduled site rebuild to happen).

Post `docs/publicity/announce.txt` at least to wx-announce@googlegroups.com and
to wx-users.

Submit a link to https://www.reddit.com/r/cpp or r/programming (depending on
the release importance).

Submit to https://isocpp.org/blog/suggest (need to be logged in to do it).

For major releases, submit the announcement to https://slashdot.org/submission

## Post-Release Steps

* Update the SHA-1 sums in `docs/release.md` using the checksums from the release
  announcement, then commit the changes.

* Mark the milestone corresponding to the release as completed at
  https://github.com/wxWidgets/wxWidgets/milestones

* Update the roadmap at https://wxwidgets.org/develop/roadmap/ to at
least mention the new release there (the text of this page lives in
wxWidgets/website repository).

* Run `misc/scripts/inc_release` to increment micro version,
i.e. replace x.y.z with x.y.z+1. When changing another version component,
all the files updated by this script need to be changed manually.

* Update the C:R:A settings in `build/bakefiles/version.bkl` to C:R+1:A.
Then from the build/bakesfiles directory run

        bakefile_gen

and from the root directory run

        autoconf -B build/autoconf_prepend-include

or, if you're not using Debian Stable version of autoconf, see the instructions
in `build/tools/autoconf/README.md` for regenerating configure on a different
system.

* Restore the description of the Git notes use and create a skeleton section
  for the next release in `docs/changes.txt`.


## MSW Visual Studio Official Builds

To build official x86 and x64 shared binaries the following are prerequisites:

     - Visual Studio 2008, 2010, 2012, 2013 and 2015
     - Windows SDK 6.1, 7.1 (required for x64 builds for Visual Studio 2008, 2010)
     - 7z (required for packaging the files)
     - fciv (required for generating the checksums)

The VSxxxCOMNTOOLS environment variables are used to locate the tools required
for Visual Studio 2012, 2013 and 2015. There are no Microsoft defined variables
for the SDKs used for Visual Studio 2008 and 2010. The build will look for the
following environment variables for the Visual Studio 2008 and 2010 SDK tools:

WINDOWS61SDK
WINDOWS71SDK

If either of these are blank they are set to the default install location.

To build binaries for a single compiler, open a command prompt (for Visual
Studio 2008 only an SDK 6.1 developer's command prompt must be used),
cd to the build\tools\msvs folder and run the batch file 'officialbuild'
with the vcXXX version number:

    Visual Studio 2008  vc90
    Visual Studio 2010  vc100
    Visual Studio 2012  vc110
    Visual Studio 2014  vc120
    Visual Studio 2015  vc14x

The Visual Studio 2015, 2017, 2019 and 2022 are binary compatible, allowing the
vc14x binary to be used with any of them.

This will build all of the x86 and x64 binaries for the selected compiler version,
package them in 7z files and calculate the checksums. The 7z files and the
checksums are output to the build\msw\packages folder.

All of the compiler packages can be built at the same time by executing the
build\tools\msvs\buildall.bat file from a command prompt. Each build will be
launched in its own shell.
