#----------------------------------------------------------------------------
# Name:         wxPython.lib.maskedctrl.py
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

"""<html><body>
<P>
<B>MaskedCtrl</B> is actually a factory function for several types of
masked edit controls:
<P>
<UL>
    <LI><b>MaskedTextCtrl</b> - standard masked edit text box</LI>
    <LI><b>MaskedComboBox</b> - adds combobox capabilities</LI>
    <LI><b>IpAddrCtrl</b> - adds logical input semantics for IP address entry</LI>
    <LI><b>TimeCtrl</b> - special subclass handling lots of time formats as values</LI>
    <LI><b>MaskedNumCtrl</b> - special subclass handling numeric values</LI>
</UL>
<P>
<B>MaskedCtrl</B> works by looking for a special <b><i>controlType</i></b>
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
    from wxPython.wx.lib.maskedctrl import MaskedCtrl, MASKEDCOMBO, MASKEDTEXT, NUMBER
    from wxPython.wx.lib.maskedctrl import MaskedCtrl, controlTypes
</FONT></PRE>
If not specified as a keyword argument, the default controlType is
controlTypes.MASKEDTEXT.
<P>
Each of the above classes has its own unique arguments, but MaskedCtrl
provides a single "unified" interface for masked controls.  MaskedTextCtrl,
MaskedComboBox and IpAddrCtrl are all documented below; the others have
their own demo pages and interface descriptions.
</body></html>
"""

from wx.lib.maskededit      import MaskedTextCtrl, MaskedComboBox, IpAddrCtrl
from wx.lib.maskednumctrl   import MaskedNumCtrl
from wx.lib.timectrl        import TimeCtrl


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


def MaskedCtrl( *args, **kwargs):
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
        return MaskedTextCtrl(*args, **kwargs)

    elif controlType == MASKEDCOMBO:
        return MaskedComboBox(*args, **kwargs)

    elif controlType == IPADDR:
        return IpAddrCtrl(*args, **kwargs)

    elif controlType == TIME:
        return TimeCtrl(*args, **kwargs)

    elif controlType == NUMBER:
        return MaskedNumCtrl(*args, **kwargs)

    else:
        raise AttributeError(
            "invalid controlType specified: %s" % repr(controlType))


