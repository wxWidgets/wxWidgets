"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


class Param:
    """Used by this module to represent default wxPython parameter values,
    including parameter representations like style=wx.HSCROLL|wx.VSCROLL."""

    def __init__(self, value=None):
        if value is None:
            value = self.__class__.__name__
        self.value = value

    def __repr__(self):
        return self.value

    def __or__(self, other):
        value = '%s|%s' % (self, other)
        return self.__class__(value)


class NULL(Param): pass
NULL = NULL()

class BOTH(Param): pass
BOTH = BOTH()

class DEFAULT_FRAME_STYLE(Param): pass
DEFAULT_FRAME_STYLE = DEFAULT_FRAME_STYLE()

class DefaultPosition(Param): pass
DefaultPosition = DefaultPosition()

class DefaultSize(Param): pass
DefaultSize = DefaultSize()

class DefaultValidator(Param): pass
DefaultValidator = DefaultValidator()

class EVT_NULL(Param): pass
EVT_NULL = EVT_NULL()

class HSCROLL(Param): pass
HSCROLL = HSCROLL()

class NullColour(Param): pass
NullColour = NullColour()

class PyFrameNameStr(Param): pass
PyFrameNameStr = PyFrameNameStr()

class PyNOTEBOOK_NAME(Param): pass
PyNOTEBOOK_NAME = PyNOTEBOOK_NAME()

class PyPanelNameStr(Param): pass
PyPanelNameStr = PyPanelNameStr()

class PySTCNameStr(Param): pass
PySTCNameStr = PySTCNameStr()

class SIZE_AUTO(Param): pass
SIZE_AUTO = SIZE_AUTO()

class SIZE_USE_EXISTING(Param): pass
SIZE_USE_EXISTING = SIZE_USE_EXISTING()

class TAB_TRAVERSAL(Param): pass
TAB_TRAVERSAL = TAB_TRAVERSAL()

class VSCROLL(Param): pass
VSCROLL = VSCROLL()

