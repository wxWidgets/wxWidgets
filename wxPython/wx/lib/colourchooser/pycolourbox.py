
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib.colourchooser import pycolourbox
_rename(globals(), pycolourbox.__dict__, modulename='lib.colourchooser.pycolourbox')
del pycolourbox
del _rename
