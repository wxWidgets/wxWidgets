#----------------------------------------------------------------------
# Name:         CreateMacScripts.py
# Purpose:	Massages the scripts to be usable with MachoPython
#
# Author:       Robin Dunn
#
# Created:      12-Aug-2002
# Copyright:    (c) 2002 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------

import sys, os

python = sys.executable
destdir = os.path.split(python)[0]
pythonw = os.path.join(destdir, 'pythonw')
scriptdir = os.getcwd()

if len(sys.argv) > 1:
    destdir = sys.argv[1]

from CreateBatchFiles import scripts
repltxt = "#!/usr/bin/env python"

gui_template = """\
#!/bin/sh
exec /Applications/Python.app/Contents/MacOS/python %s.py
"""

def main():
    for script, usegui in scripts:
        destfile = os.path.join(destdir, script)
        thescript = open(script).read()
        if usegui:
            f = open(destfile+'.py', 'w')
            print destfile+'.py'
            f.write(thescript.replace(repltxt, ''))
            f.close()
            f = open(destfile, 'w')
            print destfile
            f.write(gui_template % destfile)
            f.close()

        else:
            thescript = thescript.replace(repltxt, '#!'+python)
            f = open(destfile, 'w')
            print destfile
            f.write(thescript)
            f.close()

        os.chmod(destfile, 0755)


if __name__ == '__main__':
    main()

