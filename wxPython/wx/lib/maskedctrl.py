#----------------------------------------------------------------------------
# Name:         wxPython.lib.maskedctrl.py
# Author:       Will Sadkin
# Created:      09/24/2003
# Copyright:   (c) 2003 by Will Sadkin
# RCS-ID:      $Id$
# License:     wxWindows license
#----------------------------------------------------------------------------

"""<html><body>
<P>
<B>wxMaskedCtrl</B> is actually a factory function for several types of
masked edit controls:
<P>
<UL>
    <LI><b>wxMaskedTextCtrl</b> - standard masked edit text box</LI>
    <LI><b>wxMaskedComboBox</b> - adds combobox capabilities</LI>
    <LI><b>wxIpAddrCtrl</b> - adds logical input semantics for IP address entry</LI>
    <LI><b>wxTimeCtrl</b> - special subclass handling lots of time formats as values</LI>
    <LI><b>wxMaskedNumCtrl</b> - special subclass handling numeric values</LI>
</UL>
<P>
<B>wxMaskedCtrl</B> works by looking for a special <b><i>controlType</i></b>
parameter in the variable arguments of the control, to determine
what kind of instance to return.
controlType can be one of:
<PRE><FONT SIZE=-1>
    controlTypes.MASKEDTEXT
    controlTypes.MASKEDCOMBO
    controlTypes.IPADDR
    controlTypes.TIME
    controlTypes.NUMBER
</FONT></PRE>
These constants are also available individually, ie, you can
use either of the following:
<PRE><FONT SIZE=-1>
    from wxPython.wx.lib.maskedctrl import wxMaskedCtrl, MASKEDCOMBO, MASKEDTEXT, NUMBER
    from wxPython.wx.lib.maskedctrl import wxMaskedCtrl, controlTypes
</FONT></PRE>
If not specified as a keyword argument, the default controlType is
controlTypes.MASKEDTEXT.
<P>
Each of the above classes has its own unique arguments, but wxMaskedCtrl
provides a single "unified" interface for masked controls.  wxMaskedTextCtrl,
wxMaskedComboBox and wxIpAddrCtrl are all documented below; the others have
their own demo pages and interface descriptions.
</body></html>
"""

from wxPython.lib.maskededit import wxMaskedTextCtrl, wxMaskedComboBox, wxIpAddrCtrl
from wxPython.lib.maskednumctrl import wxMaskedNumCtrl
from wxPython.lib.timectrl import wxTimeCtrl


# "type" enumeration for class instance factory function
MASKEDTEXT  = 0
MASKEDCOMBO = 1
IPADDR      = 2
TIME        = 3
NUMBER      = 4

# for ease of import
class controlTypes:
    MASKEDTEXT  = MASKEDTEXT
    MASKEDCOMBO = MASKEDCOMBO
    IPADDR      = IPADDR
    TIME        = TIME
    NUMBER      = NUMBER


def wxMaskedCtrl( *args, **kwargs):
    """
    Actually a factory function providing a unifying
    interface for generating masked controls.
    """
    if not kwargs.has_key('controlType'):
        controlType = MASKEDTEXT
    else:
        controlType = kwargs['controlType']
        del kwargs['controlType']

    if controlType == MASKEDTEXT:
        return wxMaskedTextCtrl(*args, **kwargs)

    elif controlType == MASKEDCOMBO:
        return wxMaskedComboBox(*args, **kwargs)

    elif controlType == IPADDR:
        return wxIpAddrCtrl(*args, **kwargs)

    elif controlType == TIME:
        return wxTimeCtrl(*args, **kwargs)

    elif controlType == NUMBER:
        return wxMaskedNumCtrl(*args, **kwargs)

    else:
        raise AttributeError(
            "invalid controlType specified: %s" % repr(controlType))


