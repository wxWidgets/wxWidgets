
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib.mixins import listctrl
_rename(globals(), listctrl.__dict__, modulename='lib.mixins.listctrl')
del listctrl
del _rename
