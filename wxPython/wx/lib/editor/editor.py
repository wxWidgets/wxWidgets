
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib.editor import editor
_rename(globals(), editor.__dict__, modulename='lib.editor.editor')
del editor
del _rename
