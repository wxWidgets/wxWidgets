
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython import stc
_rename(globals(), stc.__dict__, modulename='stc')
del stc
del _rename
