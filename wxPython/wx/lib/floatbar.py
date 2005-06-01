
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import floatbar
_rename(globals(), floatbar.__dict__, modulename='lib.floatbar')
del floatbar
del _rename
