
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.tools import img2py
_rename(globals(), img2py.__dict__, modulename='tools.img2py')
del img2py
del _rename
