
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.tools.XRCed import tree
_rename(globals(), tree.__dict__, modulename='tools.XRCed.tree')
del tree
del _rename
