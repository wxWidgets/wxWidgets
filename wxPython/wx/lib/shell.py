
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import shell
_rename(globals(), shell.__dict__, modulename='lib.shell')
del shell
del _rename
