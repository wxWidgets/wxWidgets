
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import layoutf
_rename(globals(), layoutf.__dict__, modulename='lib.layoutf')
del layoutf
del _rename
