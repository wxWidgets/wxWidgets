
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import CDate
_rename(globals(), CDate.__dict__, modulename='lib.CDate')
del CDate
del _rename
