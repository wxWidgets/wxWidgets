
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import rpcMixin
_rename(globals(), rpcMixin.__dict__, modulename='lib.rpcMixin')
del rpcMixin
del _rename
