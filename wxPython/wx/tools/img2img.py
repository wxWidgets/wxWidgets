
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.tools import img2img
_rename(globals(), img2img.__dict__, modulename='tools.img2img')
del img2img
del _rename
