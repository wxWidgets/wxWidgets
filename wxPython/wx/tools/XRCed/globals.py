
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.tools.XRCed import globals
_rename(globals(), globals.__dict__, modulename='tools.XRCed.globals')
del globals
del _rename
