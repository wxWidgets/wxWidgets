
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.tools.XRCed import params
_rename(globals(), params.__dict__, modulename='tools.XRCed.params')
del params
del _rename
