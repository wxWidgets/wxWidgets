
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import maskededit
_rename(globals(), maskededit.__dict__, modulename='lib.maskededit')
del maskededit
del _rename
