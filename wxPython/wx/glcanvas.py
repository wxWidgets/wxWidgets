
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython import glcanvas
_rename(globals(), glcanvas.__dict__, modulename='glcanvas')
del glcanvas
del _rename
