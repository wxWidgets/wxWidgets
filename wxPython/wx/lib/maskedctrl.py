
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import maskedctrl
_rename(globals(), maskedctrl.__dict__, modulename='lib.maskedctrl')
del maskedctrl
del _rename
