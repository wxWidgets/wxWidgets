
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import mvctree
_rename(globals(), mvctree.__dict__, modulename='lib.mvctree')
del mvctree
del _rename
