
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib import colourdb
_rename(globals(), colourdb.__dict__, modulename='lib.colourdb')
del colourdb
del _rename
