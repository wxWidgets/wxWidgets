
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib import infoframe
_rename(globals(), infoframe.__dict__, modulename='lib.infoframe')
del infoframe
del _rename
