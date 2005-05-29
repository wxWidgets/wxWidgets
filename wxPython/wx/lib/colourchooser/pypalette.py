
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib.colourchooser import pypalette
_rename(globals(), pypalette.__dict__, modulename='lib.colourchooser.pypalette')
del pypalette
del _rename
