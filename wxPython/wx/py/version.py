
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.py import version
_rename(globals(), version.__dict__, modulename='py.version')
del version
del _rename
