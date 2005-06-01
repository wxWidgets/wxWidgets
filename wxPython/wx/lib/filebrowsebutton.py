
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import filebrowsebutton
_rename(globals(), filebrowsebutton.__dict__, modulename='lib.filebrowsebutton')
del filebrowsebutton
del _rename
