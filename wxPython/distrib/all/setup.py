# setup.py for taskrunner.py

import sys
from distutils.core import setup

deps = []
if sys.version_info < (2, 4):
    deps.append("subprocess")

setup(name='taskrunner',
      version='0.8',
      py_modules=['taskrunner'] + deps,
      )