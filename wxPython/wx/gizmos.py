
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython import gizmos
_rename(globals(), gizmos.__dict__, modulename='gizmos')
del gizmos
del _rename
