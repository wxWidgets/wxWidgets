Doodle
------

This little sample is a doodle application.  It shows you how to draw
on a canvas, deal with mouse events, popup menus, update UI events,
and much more.

    doodle.py	    A class for the main drawing window.  You can also
                    run it directly to see just this window.


    superdoodle.py  Takes the DoodleWindow from doodle.py and puts it
                    in a more full featured application with a control
                    panel, and the ability to save and load doodles.

    setup.py        A distutils script to make a standalone .exe of
                    superdoodle for Windows platforms.  You can get
                    py2exe from http://py2exe.sourceforge.net/.  Use
                    this command to build the .exe and collect the
                    other needed files:

		        python setup.py py2exe


    superdoodle.iss  NOTE: This .iss file needs updating!
		     An InnoSetup script to build a self-installer of
                     the superdoodle standalone executable created
                     with py2exe.  You can get InnoSetup from
                     http://www.jrsoftware.org/.  You may have to edit
                     this file to make it work for your environment
                     and python installation.		     
		     

    cxfreeze.bat     An example of creating an executable with the
                     cx_Freeze utility, downloadable from 
		     http://starship.python.net/crew/atuining/cx_Freeze/

    installer.bat    An example of creating an executable with Gordon
                     McMillan's Installer utility.
