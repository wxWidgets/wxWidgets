
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import imagebrowser
_rename(globals(), imagebrowser.__dict__, modulename='lib.imagebrowser')
del imagebrowser
del _rename
