
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import splashscreen
_rename(globals(), splashscreen.__dict__, modulename='lib.splashscreen')
del splashscreen
del _rename
