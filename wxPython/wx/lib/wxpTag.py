
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib import wxpTag
_rename(globals(), wxpTag.__dict__, modulename='lib.wxpTag')
del wxpTag
del _rename
