
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import rcsizer
_rename(globals(), rcsizer.__dict__, modulename='lib.rcsizer')
del rcsizer
del _rename
