
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython import xrc
_rename(globals(), xrc.__dict__, modulename='xrc')
del xrc
del _rename
