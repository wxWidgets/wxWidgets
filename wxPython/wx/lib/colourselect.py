
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import colourselect
_rename(globals(), colourselect.__dict__, modulename='lib.colourselect')
del colourselect
del _rename
