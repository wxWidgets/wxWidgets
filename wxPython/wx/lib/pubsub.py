
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib import pubsub
_rename(globals(), pubsub.__dict__, modulename='lib.pubsub')
del pubsub
del _rename
