
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.tools import dbg
_rename(globals(), dbg.__dict__, modulename='tools.dbg')
del dbg
del _rename
