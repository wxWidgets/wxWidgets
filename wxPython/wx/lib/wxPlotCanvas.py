
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import wxPlotCanvas
_rename(globals(), wxPlotCanvas.__dict__, modulename='lib.wxPlotCanvas')
del wxPlotCanvas
del _rename
