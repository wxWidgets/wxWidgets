
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib import throbber
_rename(globals(), throbber.__dict__, modulename='lib.throbber')
del throbber
del _rename
