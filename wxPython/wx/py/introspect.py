
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import introspect
_rename(globals(), introspect.__dict__, modulename='py.introspect')
del introspect
del _rename
