
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython import glcanvas
_rename(globals(), glcanvas.__dict__, modulename='glcanvas')
del glcanvas
del _rename
