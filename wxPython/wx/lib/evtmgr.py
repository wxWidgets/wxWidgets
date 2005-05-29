
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import evtmgr
_rename(globals(), evtmgr.__dict__, modulename='lib.evtmgr')
del evtmgr
del _rename
