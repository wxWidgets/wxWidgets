
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import ErrorDialogs_wdr
_rename(globals(), ErrorDialogs_wdr.__dict__, modulename='lib.ErrorDialogs_wdr')
del ErrorDialogs_wdr
del _rename
