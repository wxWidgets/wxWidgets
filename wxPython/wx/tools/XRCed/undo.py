
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.tools.XRCed import undo
_rename(globals(), undo.__dict__, modulename='tools.XRCed.undo')
del undo
del _rename
