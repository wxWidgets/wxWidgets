
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import gridmovers
_rename(globals(), gridmovers.__dict__, modulename='lib.gridmovers')
del gridmovers
del _rename
