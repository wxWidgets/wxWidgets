
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib.editor import images
_rename(globals(), images.__dict__, modulename='lib.editor.images')
del images
del _rename
