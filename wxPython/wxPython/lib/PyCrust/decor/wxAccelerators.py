"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from wxBase import wxObject
import wxParameters as wx


class wxAcceleratorEntry:
    """"""

    def GetCommand(self):
        """"""
        pass

    def GetFlags(self):
        """"""
        pass

    def GetKeyCode(self):
        """"""
        pass

    def Set(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxAcceleratorTable(wxObject):
    """"""

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


