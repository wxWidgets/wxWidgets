
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import crust
_rename(globals(), crust.__dict__, modulename='py.crust')
del crust
del _rename
