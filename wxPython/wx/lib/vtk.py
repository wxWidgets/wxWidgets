
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import vtk
_rename(globals(), vtk.__dict__, modulename='lib.vtk')
del vtk
del _rename
