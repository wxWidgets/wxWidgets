
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.tools import img2xpm
_rename(globals(), img2xpm.__dict__, modulename='tools.img2xpm')
del img2xpm
del _rename
