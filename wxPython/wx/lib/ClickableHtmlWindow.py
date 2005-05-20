
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import ClickableHtmlWindow
_rename(globals(), ClickableHtmlWindow.__dict__, modulename='lib.ClickableHtmlWindow')
del ClickableHtmlWindow
del _rename
