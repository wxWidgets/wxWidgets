The collection of scripts in this directory are an attempt to fully
automate the build of the wxPython source and binary packages on all
build platforms.  It does this through creative use of ssh and scp
commands to the remote build machines, so this will likely only work
in my somewhat unique environment.

The goal here is to be able to start a build on one machine and have
it take care of all the steps, including moving the source tarball to
the other build machines, initiating the build there, and collecting
the results.  Depending on the type of build, (see below) the results
may be copied to a public server for others to play with.

Types of builds:

      dryrun
		Nothing extra is done with the build, this is just for
		my own testing.  

      daily 
		The version number is temporarily adjusted to include a
		datestamp, and if the build is successful the results
		are copied to a daily build folder on starship.

      release
		The results are uploaded to the previews foler on
		starship if the build is successful.


The master script in this folder is build-all (written in Python)
which will setup and control the whole process.  The other scripts
(using bash) are launched from build-all either to do specific tasks
locally, or to run on each individual build machine to manage the
build process there, usually by calling out to other scripts that
already exist.  The build-all script uses the taskrunner.py and
subprocess Python modules.


