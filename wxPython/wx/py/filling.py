
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import filling
_rename(globals(), filling.__dict__, modulename='py.filling')
del filling
del _rename
