#----------------------------------------------------------------------
# Name:         CreateBatchFiles.py
# Purpose:	Run by the InnoSetup installer to create a DOS batch
#               file for each of the wxPython tool scripts.
#
# Author:       Robin Dunn
#
# Created:      8-Aug-2002
# Copyright:    (c) 2002 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------

import sys, os

python = sys.executable
scriptdir = os.getcwd()

scripts = [ "img2png",
            "img2py",
            "img2xpm",
            "xrced",
            ]

template = """\
@echo off
%(python)s %(scriptdir)s\\%(script)s %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9
"""

for script in scripts:
    batfile = os.path.join(scriptdir, script + '.bat')
    print "Creating", batfile
    f = open(batfile, 'w')
    f.write(template % vars())
    f.close()


