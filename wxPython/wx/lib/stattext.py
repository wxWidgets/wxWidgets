
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import stattext
_rename(globals(), stattext.__dict__, modulename='lib.stattext')
del stattext
del _rename
