
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib.colourchooser import pycolourchooser
_rename(globals(), pycolourchooser.__dict__, modulename='lib.colourchooser.pycolourchooser')
del pycolourchooser
del _rename
