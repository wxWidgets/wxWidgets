"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


class _Param:
    """Used by this module to represent default wxPython parameter values,
    including parameter representations like style=wx.HSCROLL|wx.VSCROLL."""

    def __init__(self, value=None):
        if value is None:
            value = 'wx.' + self.__class__.__name__
        self.value = value

    def __repr__(self):
        return self.value

    def __or__(self, other):
        value = '%s|%s' % (self, other)
        return self.__class__(value)

_params = (
    'BOTH',
    'DEFAULT_FRAME_STYLE',
    'DefaultPosition',
    'DefaultSize',
    'DefaultValidator',
    'EmptyString',
    'EVT_NULL',
    'HORIZONTAL',
    'HSCROLL',
    'NO_BORDER',
    'NULL',
    'NullColour',
    'PyFrameNameStr',
    'PyNOTEBOOK_NAME',
    'PyPanelNameStr',
    'PyStatusLineNameStr',
    'PySTCNameStr',
    'PyToolBarNameStr',
    'SIZE_AUTO',
    'SIZE_USE_EXISTING',
    'ST_SIZEGRIP',
    'TAB_TRAVERSAL',
    'TB_HORIZONTAL',
    'VSCROLL',
    )

## Create classes, then instances, like this:

## class BOTH(Param): pass
## BOTH = BOTH()

for _param in _params:
    exec 'class %s(_Param): pass' % _param
    exec '%s = %s()' % (_param, _param)

del _param
del _params
del _Param
