
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.lib.colourchooser import intl
_rename(globals(), intl.__dict__, modulename='lib.colourchooser.intl')
del intl
del _rename
