
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import popupctl
_rename(globals(), popupctl.__dict__, modulename='lib.popupctl')
del popupctl
del _rename
