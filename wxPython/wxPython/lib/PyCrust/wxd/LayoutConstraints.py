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


class IndividualLayoutConstraint(Object):
    """"""

    def Above(self):
        """"""
        pass

    def Absolute(self):
        """"""
        pass

    def AsIs(self):
        """"""
        pass

    def Below(self):
        """"""
        pass

    def LeftOf(self):
        """"""
        pass

    def PercentOf(self):
        """"""
        pass

    def RightOf(self):
        """"""
        pass

    def SameAs(self):
        """"""
        pass

    def Set(self):
        """"""
        pass

    def Unconstrained(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class LayoutConstraints(Object):
    """"""

    def __getattr__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def __setattr__(self):
        """"""
        pass
