#----------------------------------------------------------------------------
# Name:         wxPython.lib.masked.ctrl.py
# Author:       Will Sadkin
# Created:      09/24/2003
# Copyright:   (c) 2003 by Will Sadkin
# RCS-ID:      $Id$
# License:     wxWindows license
#----------------------------------------------------------------------------
# 12/09/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace (minor)
#
# 12/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Removed wx prefix
#

"""

*masked.Ctrl* is actually a factory function for several types of
masked edit controls:

    =================  =========================================================
    masked.TextCtrl     standard masked edit text box
    masked.ComboBox     adds combobox capabilities
    masked.IpAddrCtrl   adds logical input semantics for IP address entry
    masked.TimeCtrl     special subclass handling lots of time formats as values
    masked.NumCtrl      special subclass handling numeric values
    =================  =========================================================

masked.Ctrl works by looking for a special *controlType*
parameter in the variable arguments of the control, to determine
what kind of instance to return.
controlType can be one of::

    controlTypes.TEXT
    controlTypes.COMBO
    controlTypes.IPADDR
    controlTypes.TIME
    controlTypes.NUMBER

These constants are also available individually, ie, you can
use either of the following::

    from wxPython.wx.lib.masked import Ctrl, COMBO, TEXT, NUMBER, TIME
    from wxPython.wx.lib.masked import Ctrl, controlTypes

If not specified as a keyword argument, the default controlType is
controlTypes.TEXT.

Each of the above classes has its own unique arguments, but Masked.Ctrl
provides a single "unified" interface for masked controls.


"""

from wx.lib.masked   import TextCtrl, ComboBox, IpAddrCtrl
from wx.lib.masked   import NumCtrl
from wx.lib.masked   import TimeCtrl


# "type" enumeration for class instance factory function
TEXT        = 0
COMBO       = 1
IPADDR      = 2
TIME        = 3
NUMBER      = 4

# for ease of import
class controlTypes:
    TEXT        = TEXT
    COMBO       = COMBO
    IPADDR      = IPADDR
    TIME        = TIME
    NUMBER      = NUMBER


def Ctrl( *args, **kwargs):
    """
    Actually a factory function providing a unifying
    interface for generating masked controls.
    """
    if not kwargs.has_key('controlType'):
        controlType = TEXT
    else:
        controlType = kwargs['controlType']
        del kwargs['controlType']

    if controlType == TEXT:
        return TextCtrl(*args, **kwargs)

    elif controlType == COMBO:
        return ComboBox(*args, **kwargs)

    elif controlType == IPADDR:
        return IpAddrCtrl(*args, **kwargs)

    elif controlType == TIME:
        return TimeCtrl(*args, **kwargs)

    elif controlType == NUMBER:
        return NumCtrl(*args, **kwargs)

    else:
        raise AttributeError(
            "invalid controlType specified: %s" % repr(controlType))


