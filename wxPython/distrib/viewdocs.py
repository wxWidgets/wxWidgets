#!/usr/bin/env python
#---------------------------------------------------------------------------

import sys, os
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

# launch helpviewer
helpviewer.main(args)

#---------------------------------------------------------------------------
