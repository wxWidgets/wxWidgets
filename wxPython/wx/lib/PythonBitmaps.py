
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import PythonBitmaps
_rename(globals(), PythonBitmaps.__dict__, modulename='lib.PythonBitmaps')
del PythonBitmaps
del _rename
