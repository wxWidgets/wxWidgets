#
# A distutils script to make a standalone .exe of superdoodle for
# Windows platforms.
#

from distutils.core import setup
import py2exe

setup( name = "superdoodle",
       scripts = ["superdoodle.py"]
       )

