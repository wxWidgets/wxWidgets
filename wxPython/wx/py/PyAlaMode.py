
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.py import PyAlaMode
_rename(globals(), PyAlaMode.__dict__, modulename='py.PyAlaMode')
del PyAlaMode
del _rename
