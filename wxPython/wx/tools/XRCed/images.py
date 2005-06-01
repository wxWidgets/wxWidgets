
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.tools.XRCed import images
_rename(globals(), images.__dict__, modulename='tools.XRCed.images')
del images
del _rename
