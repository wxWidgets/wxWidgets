
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.tools import img2png
_rename(globals(), img2png.__dict__, modulename='tools.img2png')
del img2png
del _rename
