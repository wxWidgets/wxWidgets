
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import imageutils
_rename(globals(), imageutils.__dict__, modulename='lib.imageutils')
del imageutils
del _rename
