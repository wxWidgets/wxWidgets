
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import analogclock
_rename(globals(), analogclock.__dict__, modulename='lib.analogclock')
del analogclock
del _rename
