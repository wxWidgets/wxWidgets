#----------------------------------------------------------------------------
# Name:         masked.ipaddrctrl.py
# Authors:      Will Sadkin
# Email:        wsadkin@nameconnector.com
# Created:      02/11/2003
# Copyright:    (c) 2003 by Will Sadkin, 2003
# RCS-ID:       $Id$
# License:      wxWidgets license
#----------------------------------------------------------------------------
# NOTE:
#   Masked.IpAddrCtrl is a minor modification to masked.TextCtrl, that is
#   specifically tailored for entering IP addresses.  It allows for
#   right-insert fields and provides an accessor to obtain the entered
#   address with extra whitespace removed.
#
#----------------------------------------------------------------------------

import  wx
from wx.lib.masked import BaseMaskedTextCtrl

# jmg 12/9/03 - when we cut ties with Py 2.2 and earlier, this would
# be a good place to implement the 2.3 logger class
from wx.tools.dbg import Logger
dbg = Logger()
##dbg(enable=0)

class IpAddrCtrlAccessorsMixin:
    # Define IpAddrCtrl's list of attributes having their own
    # Get/Set functions, exposing only those that make sense for
    # an IP address control.

    exposed_basectrl_params = (
        'fields',
        'retainFieldValidation',
        'formatcodes',
        'fillChar',
        'defaultValue',
        'description',

        'useFixedWidthFont',
        'signedForegroundColour',
        'emptyBackgroundColour',
        'validBackgroundColour',
        'invalidBackgroundColour',

        'emptyInvalid',
        'validFunc',
        'validRequired',
        )

    for param in exposed_basectrl_params:
        propname = param[0].upper() + param[1:]
        exec('def Set%s(self, value): self.SetCtrlParameters(%s=value)' % (propname, param))
        exec('def Get%s(self): return self.GetCtrlParameter("%s")''' % (propname, param))

        if param.find('Colour') != -1:
            # add non-british spellings, for backward-compatibility
            propname.replace('Colour', 'Color')

            exec('def Set%s(self, value): self.SetCtrlParameters(%s=value)' % (propname, param))
            exec('def Get%s(self): return self.GetCtrlParameter("%s")''' % (propname, param))


class IpAddrCtrl( BaseMaskedTextCtrl, IpAddrCtrlAccessorsMixin ):
    """
    This class is a particular type of MaskedTextCtrl that accepts
    and understands the semantics of IP addresses, reformats input
    as you move from field to field, and accepts '.' as a navigation
    character, so that typing an IP address can be done naturally.
    """



    def __init__( self, parent, id=-1, value = '',
                  pos = wx.DefaultPosition,
                  size = wx.DefaultSize,
                  style = wx.TE_PROCESS_TAB,
                  validator = wx.DefaultValidator,
                  name = 'IpAddrCtrl',
                  setupEventHandling = True,        ## setup event handling by default
                  **kwargs):

        if not kwargs.has_key('mask'):
           kwargs['mask'] = mask = "###.###.###.###"
        if not kwargs.has_key('formatcodes'):
            kwargs['formatcodes'] = 'F_Sr<>'
        if not kwargs.has_key('validRegex'):
            kwargs['validRegex'] = "(  \d| \d\d|(1\d\d|2[0-4]\d|25[0-5]))(\.(  \d| \d\d|(1\d\d|2[0-4]\d|25[0-5]))){3}"


        BaseMaskedTextCtrl.__init__(
                self, parent, id=id, value = value,
                pos=pos, size=size,
                style = style,
                validator = validator,
                name = name,
                setupEventHandling = setupEventHandling,
                **kwargs)


        # set up individual field parameters as well:
        field_params = {}
        field_params['validRegex'] = "(   |  \d| \d |\d  | \d\d|\d\d |\d \d|(1\d\d|2[0-4]\d|25[0-5]))"

        # require "valid" string; this prevents entry of any value > 255, but allows
        # intermediate constructions; overall control validation requires well-formatted value.
        field_params['formatcodes'] = 'V'

        if field_params:
            for i in self._field_indices:
                self.SetFieldParameters(i, **field_params)

        # This makes '.' act like tab:
        self._AddNavKey('.', handler=self.OnDot)
        self._AddNavKey('>', handler=self.OnDot)    # for "shift-."


    def OnDot(self, event):
##        dbg('IpAddrCtrl::OnDot', indent=1)
        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
        oldvalue = self.GetValue()
        edit_start, edit_end, slice = self._FindFieldExtent(pos, getslice=True)
        if not event.ShiftDown():
            if pos > edit_start and pos < edit_end:
                # clip data in field to the right of pos, if adjusting fields
                # when not at delimeter; (assumption == they hit '.')
                newvalue = oldvalue[:pos] + ' ' * (edit_end - pos) + oldvalue[edit_end:]
                self._SetValue(newvalue)
                self._SetInsertionPoint(pos)
##        dbg(indent=0)
        return self._OnChangeField(event)



    def GetAddress(self):
        value = BaseMaskedTextCtrl.GetValue(self)
        return value.replace(' ','')    # remove spaces from the value


    def _OnCtrl_S(self, event):
##        dbg("IpAddrCtrl::_OnCtrl_S")
        if self._demo:
            print "value:", self.GetAddress()
        return False

    def SetValue(self, value):
##        dbg('IpAddrCtrl::SetValue(%s)' % str(value), indent=1)
        if type(value) not in (types.StringType, types.UnicodeType):
##            dbg(indent=0)
            raise ValueError('%s must be a string', str(value))

        bValid = True   # assume True
        parts = value.split('.')
        if len(parts) != 4:
            bValid = False
        else:
            for i in range(4):
                part = parts[i]
                if not 0 <= len(part) <= 3:
                    bValid = False
                    break
                elif part.strip():  # non-empty part
                    try:
                        j = string.atoi(part)
                        if not 0 <= j <= 255:
                            bValid = False
                            break
                        else:
                            parts[i] = '%3d' % j
                    except:
                        bValid = False
                        break
                else:
                    # allow empty sections for SetValue (will result in "invalid" value,
                    # but this may be useful for initializing the control:
                    parts[i] = '   '    # convert empty field to 3-char length

        if not bValid:
##            dbg(indent=0)
            raise ValueError('value (%s) must be a string of form n.n.n.n where n is empty or in range 0-255' % str(value))
        else:
##            dbg('parts:', parts)
            value = string.join(parts, '.')
            BaseMaskedTextCtrl.SetValue(self, value)
##        dbg(indent=0)

i=0
## Version 1.1
##  Made ipaddrctrls allow right-insert in subfields, now that insert/cut/paste works better
