
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.tools import helpviewer
_rename(globals(), helpviewer.__dict__, modulename='tools.helpviewer')
del helpviewer
del _rename
