#!/usr/bin/env python
#---------------------------------------------------------------------------

import sys, os, glob
from wxPython.tools import helpviewer


# Figure out the path where this app is located
if __name__ == '__main__':
    basePath = os.path.dirname(sys.argv[0])
else:
    basePath = os.path.dirname(__file__)


# setup the args
args = ['',
        '--cache='+basePath,
        os.path.join(basePath, 'wx.zip'),
        os.path.join(basePath, 'ogl.zip'),
        ]

# add any other .zip files found
for file in glob.glob(os.path.join(basePath, "*.zip")):
    if file not in args:
        args.append(file)

# launch helpviewer
helpviewer.main(args)

#---------------------------------------------------------------------------

