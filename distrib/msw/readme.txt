Distribution scripts and lists
-----------------------------------------------------------------

This directory (distrib\msw) contains some 4DOS batch files
(.bat) and 'response' files (.rsp) to simplify the job of
producing distributions. The .rsp files specify which files are
associated with a particular module, e.g. wx200vc.rsp refers to
the VC++ project files, wx200gen.rsp represents the generic files,
wx200msw.rsp specifies the Windows specific files, etc.

When making a distribution on Windows, I call zipdist.bat to prepare
zip files with everything needed for Windows,
GTK and Motif. zipdist then unzips some of them into
deliver\wx, removes and adds a few files to perfect the
distribution.

zipdist then calls 'makewise.bat' to generate a new wxwin2.wse
script, for WISE Installer. It takes wisetop.txt, wisebott.txt
and adds the section for file installation. (If you've modified
wxwin2.wse using WISE Installer, simply compile and run splitwise.exe
to put back up-to-date wisetop.txt, wisebott.txt files before
running zipdist.)

Finally, zipdist runs WISE Installer using a command line
argument to produce the setup.* files automatically.

Note that although zipdist.bat produces archives for 3 platforms,
I only use a subset of these to produce the Windows-specific
setup.exe. I then have the option of distributing the zip files
as well.

You may need to install 4DOS to run these scripts. If anyone
wishes to remove 4DOS dependency, that's fine with me.

Julian Smart, 11th October 1999

