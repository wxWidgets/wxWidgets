
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython import wizard
_rename(globals(), wizard.__dict__, modulename='wizard')
del wizard
del _rename
