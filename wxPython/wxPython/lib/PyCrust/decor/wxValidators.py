"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from wxBase import wxEvtHandler
import wxParameters as wx


class wxValidator(wxEvtHandler):
    """"""

    def __init__(self):
        """"""
        pass

    def Clone(self):
        """"""
        pass

    def GetWindow(self):
        """"""
        pass

    def SetWindow(self, window):
        """"""
        pass


class wxPyValidator(wxValidator):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self, _class, incref=True):
        """"""
        pass


