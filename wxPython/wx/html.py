
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython import html
_rename(globals(), html.__dict__, modulename='html')
del html
del _rename
