
"""Renamer stub: provides a way to drop the wx prefix from wxPython objects."""

__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wx import _rename
from wxPython.py import PyFilling
_rename(globals(), PyFilling.__dict__, modulename='py.PyFilling')
del PyFilling
del _rename
