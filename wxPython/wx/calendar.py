
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython import calendar
_rename(globals(), calendar.__dict__, modulename='calendar')
del calendar
del _rename
