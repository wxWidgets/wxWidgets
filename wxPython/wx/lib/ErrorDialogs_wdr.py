
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib import ErrorDialogs_wdr
_rename(globals(), ErrorDialogs_wdr.__dict__, modulename='lib.ErrorDialogs_wdr')
del ErrorDialogs_wdr
del _rename
