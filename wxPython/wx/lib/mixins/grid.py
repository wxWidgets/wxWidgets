
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib.mixins import grid
_rename(globals(), grid.__dict__, modulename='lib.mixins.grid')
del grid
del _rename
