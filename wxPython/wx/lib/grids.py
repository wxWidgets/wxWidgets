
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import grids
_rename(globals(), grids.__dict__, modulename='lib.grids')
del grids
del _rename
