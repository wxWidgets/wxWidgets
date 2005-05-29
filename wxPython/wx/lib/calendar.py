
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import calendar
_rename(globals(), calendar.__dict__, modulename='lib.calendar')
del calendar
del _rename
