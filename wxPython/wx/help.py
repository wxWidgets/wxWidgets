
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython import help
_rename(globals(), help.__dict__, modulename='help')
del help
del _rename
