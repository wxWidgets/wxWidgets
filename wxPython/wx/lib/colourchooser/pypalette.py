
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib.colourchooser import pypalette
_rename(globals(), pypalette.__dict__, modulename='lib.colourchooser.pypalette')
del pypalette
del _rename
