
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import printout
_rename(globals(), printout.__dict__, modulename='lib.printout')
del printout
del _rename
