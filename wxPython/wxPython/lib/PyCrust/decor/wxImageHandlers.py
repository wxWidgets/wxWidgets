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


class wxImageHandler(wxObject):
    """"""

    def CanRead(self):
        """"""
        pass

    def GetExtension(self):
        """"""
        pass

    def GetMimeType(self):
        """"""
        pass

    def GetName(self):
        """"""
        pass

    def GetType(self):
        """"""
        pass

    def SetExtension(self):
        """"""
        pass

    def SetMimeType(self):
        """"""
        pass

    def SetName(self):
        """"""
        pass

    def SetType(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxBMPHandler(wxImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxGIFHandler(wxImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxICOHandler(wxBMPHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxCURHandler(wxICOHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxANIHandler(wxCURHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxJPEGHandler(wxImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxPCXHandler(wxImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxPNGHandler(wxImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxPNMHandler(wxImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class wxTIFFHandler(wxImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


