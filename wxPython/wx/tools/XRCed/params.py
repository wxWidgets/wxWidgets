
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.tools.XRCed import params
_rename(globals(), params.__dict__, modulename='tools.XRCed.params')
del params
del _rename
