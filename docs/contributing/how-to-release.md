# Making a New wxWidgets Release

Creating a new release requires a few things before getting started:

* Linux or OSX with 7z installed.
* Windows with HTML Help Workshop installed.
* Doxygen 1.8.8, and GraphViz installed on both machines.

Unless mentioned otherwise, all steps should be run on Linux or OSX so that the
repository export used for the release is primarily using LF line endings. Files
that require CRLF line endings will be converted appropriately.

## Pre-Release Steps

* Change the version in `include/wx/version.h`.
* Update `docs/readme.txt`. Please review its contents in addition to just
  changing the version number.
* Put a date on the release line in `docs/changes.txt`.
* Update the date in the manual (`docs/doxygen/mainpages/manual.h`).
* Update the release announcement post in `docs/publicity/announce.txt`.

Finally, tag the release:

    git tag vX.Y.Z -m 'Tag X.Y.Z release.'

## Creating Release Files

Follow these steps assuming the current working directory is the root of git
working copy and you want to prepare distribution for the version x.y.z:

1. Run `./build/tools/git-make-release.sh x.y.z` to create source archives
   `wxWidgets-x.y.z.{7z,tar.bz2,zip}` and `wxWidgets-x.y.z-headers.7z`.

2. Run `./build/tools/make-html-docs.sh x.y.z` to create HTML documentation
   archives `wxWidgets-x.y.z-docs-html.{tar.bz2,zip}`.

3. Run the following commands under Windows since it relies on having `hhc.exe`
   (the HTML Help compiler) in your PATH:

    ```
    cd docs\doxygen
    regen.bat chm
    cd out
    zip ..\..\..\wxWidgets-x.y.z-docs-chm.zip wx.chm
    ```

4. This step also must be done under Windows as it uses Inno Setup to produce
   the `.exe` file and it also requires the CHM file built above:

    ```
    md x.y.z-sources
    cd x.y.z-sources
    7z x ..\wxWidgets-x.y.z.7z
    md docs\htmlhelp
    cp ..\docs\doxygen\out\wx.chm docs\htmlhelp
    set WXW_VER=x.y.z
    iscc build\tools\wxwidgets.iss
    ```

## Uploading

Upload the files to SourceForge. This can be done via the web interface or just
scp to sfusername,wxwindows@frs.sf.net:/home/frs/project/w/wx/wxwindows/x.y.z
The following files need to be uploaded:

    wxMSW-Setup-x.y.z.exe
    wxWidgets-x.y.z.7z
    wxWidgets-x.y.z.tar.bz2
    wxWidgets-x.y.z.zip
    wxWidgets-x.y.z-docs-chm.zip
    wxWidgets-x.y.z-docs-html.tar.bz2
    wxWidgets-x.y.z-docs-html.zip
    wxWidgets-x.y.z-headers.7z

You will need to use the web interface to mark the latest uploaded files as
being "default downloads" for the appropriate platforms (.zip or .exe for MSW,
.tar.bz2 for everything else) as otherwise SourceForge would continue to suggest
people to download old files.

Next, update (at least the versions and SHA1 sums, but maybe more) and upload
docs/release_files.mdwn and docs/release_binaries.mdwn files. They need to be
renamed to README.md on SF to be shown when the directory is viewed, i.e. do:

    scp docs/release_files.mdwn \
        sfuser,wxwindows@frs.sf.net:/home/frs/project/w/wx/wxwindows/x.y.z/README.md
    scp docs/release_binaries.mdwn \
        sfuser,wxwindows@frs.sf.net:/home/frs/project/w/wx/wxwindows/x.y.z/binaries/README.md

And upload the change log too:

    scp docs/changes.txt \
        sfuser,wxwindows@frs.sf.net:/home/frs/project/w/wx/wxwindows/x.y.z

Also upload the files to the FTP mirror at ftp.wxwidgets.org (ask Chris for
access if you don't have it).

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

Also update docs/index.html for the minor or major (i.e. not micro) releases.

Post to wxBlog if necessary.

Create a new post on our official Google+ page here:
https://plus.google.com/+wxwidgets/ (contact Bryan for access)

## Version Updates

Trac: mark the milestone corresponding to the release as completed and add a
new version for it to allow reporting bugs against it and create the next
milestone (ask Vadim or Robin to do it or to get admin password).

Update the roadmap at http://trac.wxwidgets.org/wiki/Roadmap to at least
mention the new release there.

Run `misc/scripts/inc_release` to increment micro version, i.e. replace x.y.z
with x.y.z+1 (minor or major versions updates require manual intervention)
and rerun both `bakefile_gen` and `autoconf` afterwards to update the version
in the generated files too.

Update the definition of the stable and release branches in
`build/buildbot/config/include/defs.xml` after a minor version change.
