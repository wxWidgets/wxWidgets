
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.tools.XRCed import panel
_rename(globals(), panel.__dict__, modulename='tools.XRCed.panel')
del panel
del _rename
