
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import timectrl
_rename(globals(), timectrl.__dict__, modulename='lib.timectrl')
del timectrl
del _rename
