
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

from wx import _rename
from wxPython.lib import pubsub
_rename(globals(), pubsub.__dict__, modulename='lib.pubsub')
del pubsub
del _rename
