
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import wxpTag
_rename(globals(), wxpTag.__dict__, modulename='lib.wxpTag')
del wxpTag
del _rename
