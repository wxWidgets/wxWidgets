
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib.colourchooser import intl
_rename(globals(), intl.__dict__, modulename='lib.colourchooser.intl')
del intl
del _rename
