"""Provides a way to drop the wx prefix from wxPython objects."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import wx
from wx import prefix

from wxPython.lib import infoframe
prefix.rename(d_new=globals(), d_old=infoframe.__dict__)
del infoframe

del prefix
del wx
