#----------------------------------------------------------------------------
# Name:         __init__.py
# Purpose:      The presence of this file turns this directory into a
#               Python package.
#
# Author:       Robin Dunn
#
# Created:      8-Aug-1998
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import __version__
__version__ = __version__.VERSION_STRING


# Load the package namespace with the core classes and such
from wx.core import *
from wx.core import __docfilter__

# wx.core has a 'wx' symbol for internal use.  That's kinda silly for
# this namespace so get rid of it.
del wx

#----------------------------------------------------------------------------

