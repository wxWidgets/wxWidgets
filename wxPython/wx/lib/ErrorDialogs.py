
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import ErrorDialogs
_rename(globals(), ErrorDialogs.__dict__, modulename='lib.ErrorDialogs')
del ErrorDialogs
del _rename
