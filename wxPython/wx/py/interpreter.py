
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import interpreter
_rename(globals(), interpreter.__dict__, modulename='py.interpreter')
del interpreter
del _rename
