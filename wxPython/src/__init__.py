#----------------------------------------------------------------------------
# Name:         __init__.py
# Purpose:      The presence of this file turns this directory into a
#               Python package.
#
# Author:       Robin Dunn
#
# Created:      8/8/98
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import __version__
__version__ = __version__.ver


# Ensure the main extension module is loaded, in case the add-on modules
# (such as utils,) are used standalone.
import wxc
wxc.__version__ = __version__

#----------------------------------------------------------------------------

