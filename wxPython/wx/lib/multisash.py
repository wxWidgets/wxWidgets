
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import multisash
_rename(globals(), multisash.__dict__, modulename='lib.multisash')
del multisash
del _rename
