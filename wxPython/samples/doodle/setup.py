# A distutils script to make a standalone .exe of superdoodle for
# Windows platforms.  You can get py2exe from
# http://py2exe.sourceforge.net/.  Use this command to build the .exe
# and collect the other needed files:
#
#       python setup.py py2exe
#


import sys, os
from distutils.core import setup
import py2exe

## if sys.platform == "win32" and sys.version_info > (2, 4):
##     DATA =  [("", os.path.join(sys.exec_prefix, 'msvcr71.dll'))]
## else:
##     DATA = []
    

setup( name = "superdoodle",
       #console = ["superdoodle.py"]
       windows = ["superdoodle.py"],
       #data_files = DATA,
       options = {"py2exe" : { "compressed": 0,
                               "optimize": 2,
                               "bundle_files": 1,
                               }},
       zipfile = None
       )

