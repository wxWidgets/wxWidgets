The collection of scripts in this directory are an attempt to fully
automate the build of the wxPython source and binary packages on all
build platforms.  It does this through creative use of shared folders
on network drives, and ssh commands to the remote machines.  So this
will likly only work in my somewhat unique environment.

The goal here is to be able to start a build on one machine and have
it take care of all the steps, including moving the source tarball to
the other build machines, initiating the build there, and collecting
the results.  Depending on the type of build, (see below) the results
may be copied to a public server for others to play with.

Types of builds:

      dry-run
		Nothing extra is done with the build, this is just for
		my own testing.  

      daily 
		The version number is temporarily adjusted to include a
		datestamp, and if the build is successful the results
		are copied to a daily build folder on starship.

      release-cantidate
		The results are uploaded to the previews foler on
		starship if the build is successful.


The master script in this folder is "make-all" which will setup and
control the whole process.  The other scripts are what are run on each
build machine, most of which will also call out to other scripts that
already exist, etc.


