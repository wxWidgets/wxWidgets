
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import pyshell
_rename(globals(), pyshell.__dict__, modulename='lib.pyshell')
del pyshell
del _rename
