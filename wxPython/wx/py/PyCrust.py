
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.py import PyCrust
_rename(globals(), PyCrust.__dict__, modulename='py.PyCrust')
del PyCrust
del _rename
