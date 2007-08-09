#----------------------------------------------------------------------
# Name:         CreateMacScripts.py
# Purpose:      Massages the scripts to be usable with MacPython-OSX
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
prefix = destdir
pythonw = os.path.join(destdir, 'pythonw')
scriptdir = os.getcwd()

if len(sys.argv) > 1:
    root = sys.argv[1]
    p = prefix = sys.argv[2]
    if p[0] == '/': p = p[1:]
    destdir = os.path.join(root, p)


from CreateBatchFiles import scripts
repltxt = "#!/usr/bin/env python"

# use the existing pythonw as a template
gui_template = """
#!/bin/sh
exec "%s" %%s.py "$@"
""" % (sys.executable) 

def main():
    for script, usegui in scripts:
        destfile = os.path.join(destdir, script)
        prefixfile = os.path.join(prefix, script)

        thescript = open(script).read()
        if usegui:
            f = open(destfile+'.py', 'w')
            print destfile+'.py'
            f.write(thescript.replace(repltxt, ''))
            f.close()
            f = open(destfile, 'w')
            print destfile
            f.write(gui_template % prefixfile)
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

