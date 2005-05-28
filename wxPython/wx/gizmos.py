
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython import gizmos
_rename(globals(), gizmos.__dict__, modulename='gizmos')
del gizmos
del _rename
