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
    including parameter representations like style=wxHSCROLL|wxVSCROLL."""

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

class wxBOTH(Param): pass
wxBOTH = wxBOTH()

class wxDefaultPosition(Param): pass
wxDefaultPosition = wxDefaultPosition()

class wxDefaultSize(Param): pass
wxDefaultSize = wxDefaultSize()

class wxDefaultValidator(Param): pass
wxDefaultValidator = wxDefaultValidator()

class wxEVT_NULL(Param): pass
wxEVT_NULL = wxEVT_NULL()

class wxHSCROLL(Param): pass
wxHSCROLL = wxHSCROLL()

class wxNullColour(Param): pass
wxNullColour = wxNullColour()

class wxPyNOTEBOOK_NAME(Param): pass
wxPyNOTEBOOK_NAME = wxPyNOTEBOOK_NAME()

class wxPyPanelNameStr(Param): pass
wxPyPanelNameStr = wxPyPanelNameStr()

class wxPySTCNameStr(Param): pass
wxPySTCNameStr = wxPySTCNameStr()

class wxSIZE_AUTO(Param): pass
wxSIZE_AUTO = wxSIZE_AUTO()

class wxSIZE_USE_EXISTING(Param): pass
wxSIZE_USE_EXISTING = wxSIZE_USE_EXISTING()

class wxTAB_TRAVERSAL(Param): pass
wxTAB_TRAVERSAL = wxTAB_TRAVERSAL()

class wxVSCROLL(Param): pass
wxVSCROLL = wxVSCROLL()

