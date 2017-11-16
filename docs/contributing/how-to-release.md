# Making a New wxWidgets Release

Creating a new release requires a few things before getting started:

* Linux (or another Unix but GNU tar is required).
* Windows 7+ with HTML Help Workshop, and Inno Setup installed.
* 7-Zip, Doxygen 1.8.8, and GraphViz installed on both machines.

Unless mentioned otherwise, all steps should be run on Linux or OSX so that the
repository export used for the release is primarily using LF line endings. Files
that require CRLF line endings will be converted appropriately.

*Important:* Ensure that 7-Zip, HTML Help Workshop, Doxygen, GraphViz and Inno
Setup have all been added to your Path in Windows. You can confirm this by
running `7z`, `hhc`, `iscc`, `doxygen -v`, and `dot -V` in a command prompt.
Add the missing installed folder locations of any executables to your Path.

## Checking ABI Compatibility

For the stable (even) releases only, check that binary compatibility hasn't
been broken since the last stable release.

### Checking under Unix systems using `abi-complicance-checker` tool.

Instructions:

1. Get [the tool](https://lvc.github.io/abi-compliance-checker/).
1. Build the old (vX.Y.Z-1) library with `-g -Og` options, i.e. configure it
   with `--enable-debug` and `CXXFLAGS=-Og CFLAFS=-Og`. For convenience, let's
   assume it's built in "$old" subdirectory.
1. Build the new (vX.Y.Z) library with the same options in "$new".
1. Create directories for temporary files containing the ABI dumps for the old
   and new libraries: `mkdir -p ../compat/{$old,$new}`.
1. Run abi-dumper on all libraries: `for l in $old/lib/*.so; do abi-dumper $l
   -lver $old -o ../compat/$old/$(basename $l).dump` and the same thing with
   the new libraries.
1. Run abi-compliance-checker on each pair of produced dumps to generate HTML
   reports: `for l in 3.0.2/*dump; abi-compliance-checker -l $(basename $l
   .dump) -old $l -new 3.0.3/$(basename $l)`.
1. Examine these reports, paying attention to the problem summary.

### Checking under MSW systems.

Manually check compatibility by building the widgets samples from the old tree
and then run it using the new DLLs.

## Pre-Release Steps

1. Perform the following steps. You can run `build/tools/pre-release.sh` to do
   the straightforward changes like updating the date and version number
   automatically, but please also review and update the contents of the README
   and announcement text.
    * Update `docs/readme.txt`: version needs to be changed, content updated.
    * Update `docs/release.md`: the release sha1sums should be set to zeroes.
    * Put a date on the release line in `docs/changes.txt`.
    * Update the date in the manual (`docs/doxygen/mainpages/manual.h`).
    * Update the release announcement post in `docs/publicity/announce.txt`.

2. Commit the changes and tag the release using your GPG key:

    git tag -s -m 'Tag X.Y.Z release' vX.Y.Z

   Don't overwrite existing tags. For non-final releases use e.g. `X.Y.Z-rc1`
   instead of `X.Y.Z`.

## Creating Release Files

The release scripts can be run from any working directory, and they will
generate all release package files under `distrib/release/x.y.z`. The scripts
mostly build the release packages based on the current HEAD commit, so always
ensure you have the appropriate tag or commit checked out.

1. Run `./build/tools/release.sh x.y.z` to create source archives
   `wxWidgets-x.y.z.{7z,tar.bz2,zip}`, `wxWidgets-x.y.z-headers.7z`, and
   `wxWidgets-x.y.z-docs-html.{tar.bz2,zip}` packages.

2. Copy just the `wxWidgets-x.y.z.zip` package into the same
   `distrib\release\x.y.z` folder on Windows.

3. Run `build/tools/release.bat x.y.z` in a Windows command prompt. To avoid
   confusion note that, unlike other generated files, the Windows installer is
   created based on files as well as instructions (`build/tools/wxwidgets.iss`)
   contained in the copied release ZIP and not from the current working wx
   directory.

4. Copy these Windows packages back to your Linux or OSX `distrib/release/x.y.z`
   directory so you can continue with the upload step with all packages
   available:

    wxMSW-x.y.z-Setup.exe
    wxWidgets-x.y.z.chm

5. Run `./build/tools/post-release.sh` to update the sha1sums in
   `docs/release.md` and commit the changes.

## Uploading

Create a new release on GitHub using vX.Y.Z tag and title.

Use the content of `docs/release.md` for the release description box.

Attach the following files to it:

    wxMSW-Setup-x.y.z.exe
    wxWidgets-x.y.z.7z
    wxWidgets-x.y.z.tar.bz2
    wxWidgets-x.y.z.zip
    wxWidgets-x.y.z.chm
    wxWidgets-x.y.z-docs-html.tar.bz2
    wxWidgets-x.y.z-docs-html.zip
    wxWidgets-x.y.z-headers.7z

Create http://docs.wxwidgets.org/x.y.z/ (ask Bryan to do it if not done yet).

## Announcement

Update https://www.wxwidgets.org, usually a news item is enough but something
more can be called for for major releases.

Post `docs/publicity/announce.txt` at least to wx-announce@googlegroups.com and
to wx-users for the important releases.

Submit a link to https://www.reddit.com/r/programming

Submit to https://isocpp.org/blog/suggest

For major releases, submit the announcement to https://slashdot.org/submission

Modify the links at downloads/index.html to point to the new release. Also
update the release date on this page.

Also update docs/index.htm for the minor or major (i.e. not micro) releases.

Post to wxBlog if necessary.

Create a new post on our official Google+ page here:
https://plus.google.com/+wxwidgets/ (contact Bryan for access)

## Version Updates

Trac: mark the milestone corresponding to the release as completed and add a
new version for it to allow reporting bugs against it and create the next
milestone (ask Vadim or Robin to do it or to get admin password).

Update the roadmap at https://trac.wxwidgets.org/wiki/Roadmap to at least
mention the new release there.

Run `misc/scripts/inc_release` to increment micro version, i.e. replace x.y.z
with x.y.z+1 (minor or major versions updates require manual intervention)
and rerun both `bakefile_gen` and `autoconf` afterwards to update the version
in the generated files too.

Update the definition of the stable and release branches in
`build/buildbot/config/include/defs.xml` after a minor version change.

## MSW Visual Studio Official Builds

To build official x86 and x64 shared binaries the following are prerequisites:
 - Visual Studio 2012, 2013, 2015
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
cd to the build\msw\tools\msvs folder and run the batch file 'buildofficial'
with the vcXXX version number:

Visual Studio 2008  vc90
Visual Studio 2010  vc100
Visual Studio 2012  vc110
Visual Studio 2014  vc120
Visual Studio 2015  vc140

This will build all of the x86 and x64 binaries for the selected compiler version,
package them in 7z files and calculate the checksums. The 7z files and the
checksums are output to the build\msw\packages folder.

All of the compiler packages can be built at the same time by executing the
build\msw\tools\buildall.bat file (including Visual Studio 2000).
