
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import images
_rename(globals(), images.__dict__, modulename='py.images')
del images
del _rename
