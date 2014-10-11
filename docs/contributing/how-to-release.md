Making a new wxWidgets release
==============================

Before making the release
-------------------------

Change the version in include/wx/version.h.

Update docs/readme.txt. Please review its contents in addition to just
changing the version number.

Put a date on the release line in docs/changes.txt.

Update the date in the manual (docs/doxygen/mainpages/manual.h).

Update the release announcement post in docs/publicity/announce.txt.

Tag the release:

    svn cp https://svn.wxwidgets.org/svn/wx/wxWidgets/trunk@123456 \
        https://svn.wxwidgets.org/svn/wx/wxWidgets/tags/WX_x_y_z -m 'Tag x.y.z release.'


Creating release files
----------------------

The currently used release scripts need to be used from git-svn checkout and
rely on Git to avoid problems with using non-clean trees and such.

Follow these steps assuming the current working directory is the root of git
working copy and you want to prepare distribution for the version x.y.z:

1. Run `./build/tools/svn-find-native-eols.pl > ../eol-native` (if you have
   an existing svn checkout, pass it to the script to make it run much faster,
   but take care to have up to date sources in the working tree).

2. Run `./build/tools/git-make-release x.y.z` to create source archives
   ../wxWidgets-x.y.z.{7z,tar.bz2,zip} and wxWidgets_x.y.z_Headers.zip.

3. Run `./build/tools/make-html-docs x.y.z` to create HTML documentation
   archives ../wxWidgets-x.y.z.{tar.bz2,zip}

4. This step must be done under Windows as it relies on having hhc.exe, the
   Html Help compiler, in PATH: run the following commands

    ```
    cd docs\doxygen
    regen.bat chm
    cd out
    zip ..\..\..\wxWidgets-x.y.z-docs-chm.zip wx.chm
    ```

5. This step also must be done under Windows as it uses Inno Setup to produce
   the .exe file and it also requires the CHM file built above:

    ```
    md x.y.z-sources
    cd x.y.z-sources
    7z x ..\wxWidgets-x.y.z.7z
    md docs\htmlhelp
    cp ..\docs\doxygen\out\wx.chm docs\htmlhelp
    set WXW_VER=x.y.z
    iscc build\tools\wxwidgets.iss
    ```


Uploading
---------

Upload the files to SourceForge. This can be done via the web interface or just
scp to sfusername,wxwindows@frs.sf.net:/home/frs/project/w/wx/wxwindows/x.y.z
The following files need to be uploaded:

    wxMSW-Setup-x.y.z.exe
    wxWidgets-x.y.z.7z
    wxWidgets-x.y.z.tar.bz2
    wxWidgets-x.y.z.zip
    wxWidgets-docs-chm-x.y.z.zip
    wxWidgets-docs-html-x.y.z.tar.bz2
    wxWidgets-docs-html-x.y.z.zip
    wxWidgets-x.y.z_Headers.7z

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


Announcement
------------

Update http://www.wxwidgets.org, usually a news item is enough but something
more can be called for for major releases.

Post docs/publicity/announce.txt at least to wx-announce@googlegroups.com and
to wx-users for the important releases.

Submit a link to http://www.reddit.com/r/programming

Submit to http://isocpp.org/blog/suggest

For major releases, submit the announcement to http://slashdot.org/submission

Modify the links at downloads/index.html to point to the new release. Also
update the release date on this page.

Also update docs/index.html for the minor or major (i.e. not micro) releases.

Post to wxBlog if necessary.

Announce on Google+/Twitter/whatever the person doing the release prefers (we
don't have "official" wxWidgets account, should we?).


Version updates
---------------

Trac: mark the milestone corresponding to the release as completed and add a
new version for it to allow reporting bugs against it and create the next
milestone (ask Vadim or Robin to do it or to get admin password).

Update the roadmap at http://trac.wxwidgets.org/wiki/Roadmap to at least
mention the new release there.

Run misc/scripts/inc_release to increment micro version, i.e. replace x.y.z
with x.y.z+1 (minor or major versions updates require manual intervention)
and rerun both `bakefile_gen` and `autoconf` afterwards to update the version
in the generated files too.

Update the definition of the stable and release branches in
build/buildbot/config/include/defs.xml after a minor version change.
