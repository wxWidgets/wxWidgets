
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import buttons
_rename(globals(), buttons.__dict__, modulename='lib.buttons')
del buttons
del _rename
