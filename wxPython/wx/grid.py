
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython import grid
_rename(globals(), grid.__dict__, modulename='grid')
del grid
del _rename
