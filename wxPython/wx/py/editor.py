
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import editor
_rename(globals(), editor.__dict__, modulename='py.editor')
del editor
del _rename
