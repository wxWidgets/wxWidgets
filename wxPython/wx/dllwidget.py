
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython import dllwidget
_rename(globals(), dllwidget.__dict__, modulename='dllwidget')
del dllwidget
del _rename
