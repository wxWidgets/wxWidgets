
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython import iewin
_rename(globals(), iewin.__dict__, modulename='iewin')
del iewin
del _rename
