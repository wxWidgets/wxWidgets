
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython import ogl
_rename(globals(), ogl.__dict__, modulename='ogl')
del ogl
del _rename
