
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython import wizard
_rename(globals(), wizard.__dict__, modulename='wizard')
del wizard
del _rename
