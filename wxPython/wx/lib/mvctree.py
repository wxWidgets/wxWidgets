
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib import mvctree
_rename(globals(), mvctree.__dict__, modulename='lib.mvctree')
del mvctree
del _rename
