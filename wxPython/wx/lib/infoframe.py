
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import infoframe
_rename(globals(), infoframe.__dict__, modulename='lib.infoframe')
del infoframe
del _rename
