
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import floatcanvas
_rename(globals(), floatcanvas.__dict__, modulename='lib.floatcanvas')
del floatcanvas
del _rename
