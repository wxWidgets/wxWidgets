"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object
import Parameters as wx


class ImageHandler(Object):
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


class BMPHandler(ImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class GIFHandler(ImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class ICOHandler(BMPHandler):
    """"""

    def __init__(self):
        """"""
        pass


class CURHandler(ICOHandler):
    """"""

    def __init__(self):
        """"""
        pass


class ANIHandler(CURHandler):
    """"""

    def __init__(self):
        """"""
        pass


class JPEGHandler(ImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class PCXHandler(ImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class PNGHandler(ImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class PNMHandler(ImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


class TIFFHandler(ImageHandler):
    """"""

    def __init__(self):
        """"""
        pass


