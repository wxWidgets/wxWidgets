# A distutils script to make a standalone .exe of superdoodle for
# Windows platforms.  You can get py2exe from
# http://py2exe.sourceforge.net/.  Use this command to build the .exe
# and collect the other needed files:
#
#       python setup.py py2exe
#


from distutils.core import setup
import py2exe

setup( name = "superdoodle",
       #console = ["superdoodle.py"]
       windows = ["superdoodle.py"] #, "doodle.py"]
       )

