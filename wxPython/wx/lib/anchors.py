
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import anchors
_rename(globals(), anchors.__dict__, modulename='lib.anchors')
del anchors
del _rename
