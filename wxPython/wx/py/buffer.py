
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import buffer
_rename(globals(), buffer.__dict__, modulename='py.buffer')
del buffer
del _rename
