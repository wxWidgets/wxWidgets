#----------------------------------------------------------------------------
# Name:         timectrl.py
# Author:       Will Sadkin
# Created:      09/19/2002
# Copyright:    (c) 2002 by Will Sadkin, 2002
# RCS-ID:       $Id$
# License:      wxWindows license
#----------------------------------------------------------------------------
# NOTE:
#   This was written way it is because of the lack of masked edit controls
#   in wxWindows/wxPython.  I would also have preferred to derive this
#   control from a wxSpinCtrl rather than wxTextCtrl, but the wxTextCtrl
#   component of that control is inaccessible through the interface exposed in
#   wxPython.
#
#   TimeCtrl does not use validators, because it does careful manipulation
#   of the cursor in the text window on each keystroke, and validation is
#   cursor-position specific, so the control intercepts the key codes before the
#   validator would fire.
#
#   TimeCtrl now also supports .SetValue() with either strings or wxDateTime
#   values, as well as range limits, with the option of either enforcing them
#   or simply coloring the text of the control if the limits are exceeded.
#
#   Note: this class now makes heavy use of wxDateTime for parsing and
#   regularization, but it always does so with ephemeral instances of
#   wxDateTime, as the C++/Python validity of these instances seems to not
#   persist.  Because "today" can be a day for which an hour can "not exist"
#   or be counted twice (1 day each per year, for DST adjustments), the date
#   portion of all wxDateTimes used/returned have their date portion set to
#   Jan 1, 1970 (the "epoch.")
#----------------------------------------------------------------------------
# 12/13/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for V2.5 compatability
# o wx.SpinCtl has some issues that cause the control to
#   lock up. Noted in other places using it too, it's not this module
#   that's at fault.
#
# 12/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxMaskedTextCtrl -> masked.TextCtrl
# o wxTimeCtrl -> masked.TimeCtrl
#

"""<html><body>
<P>
<B>TimeCtrl</B> provides a multi-cell control that allows manipulation of a time
value.  It supports 12 or 24 hour format, and you can use wxDateTime or mxDateTime
to get/set values from the control.
<P>
Left/right/tab keys to switch cells within a TimeCtrl, and the up/down arrows act
like a spin control.  TimeCtrl also allows for an actual spin button to be attached
to the control, so that it acts like the up/down arrow keys.
<P>
The <B>!</B> or <B>c</B> key sets the value of the control to the current time.
<P>
Here's the API for TimeCtrl:
<DL><PRE>
    <B>TimeCtrl</B>(
         parent, id = -1,
         <B>value</B> = '00:00:00',
         pos = wx.DefaultPosition,
         size = wx.DefaultSize,
         <B>style</B> = wxTE_PROCESS_TAB,
         <B>validator</B> = wx.DefaultValidator,
         name = "time",
         <B>format</B> = 'HHMMSS',
         <B>fmt24hr</B> = False,
         <B>displaySeconds</B> = True,
         <B>spinButton</B> = None,
         <B>min</B> = None,
         <B>max</B> = None,
         <B>limited</B> = None,
         <B>oob_color</B> = "Yellow"
)
</PRE>
<UL>
    <DT><B>value</B>
    <DD>If no initial value is set, the default will be midnight; if an illegal string
    is specified, a ValueError will result.  (You can always later set the initial time
    with SetValue() after instantiation of the control.)
    <DL><B>size</B>
    <DD>The size of the control will be automatically adjusted for 12/24 hour format
    if wx.DefaultSize is specified.  NOTE: due to a problem with wx.DateTime, if the
    locale does not use 'AM/PM' for its values, the default format will automatically
    change to 24 hour format, and an AttributeError will be thrown if a non-24 format
    is specified.
    <DT><B>style</B>
    <DD>By default, TimeCtrl will process TAB events, by allowing tab to the
    different cells within the control.
    <DT><B>validator</B>
    <DD>By default, TimeCtrl just uses the default (empty) validator, as all
    of its validation for entry control is handled internally.  However, a validator
    can be supplied to provide data transfer capability to the control.
    <BR>
    <DT><B>format</B>
    <DD>This parameter can be used instead of the fmt24hr and displaySeconds
    parameters, respectively; it provides a shorthand way to specify the time
    format you want.  Accepted values are 'HHMMSS', 'HHMM', '24HHMMSS', and
    '24HHMM'.  If the format is specified, the other two arguments will be ignored.
    <BR>
    <DT><B>fmt24hr</B>
    <DD>If True, control will display time in 24 hour time format; if False, it will
    use 12 hour AM/PM format.  SetValue() will adjust values accordingly for the
    control, based on the format specified.  (This value is ignored if the <i>format</i>
    parameter is specified.)
    <BR>
    <DT><B>displaySeconds</B>
    <DD>If True, control will include a seconds field; if False, it will
    just show hours and minutes. (This value is ignored if the <i>format</i>
    parameter is specified.)
    <BR>
    <DT><B>spinButton</B>
    <DD>If specified, this button's events will be bound to the behavior of the
    TimeCtrl, working like up/down cursor key events.  (See BindSpinButton.)
    <BR>
    <DT><B>min</B>
    <DD>Defines the lower bound for "valid" selections in the control.
    By default, TimeCtrl doesn't have bounds.  You must set both upper and lower
    bounds to make the control pay attention to them, (as only one bound makes no sense
    with times.) "Valid" times will fall between the min and max "pie wedge" of the
    clock.
    <DT><B>max</B>
    <DD>Defines the upper bound for "valid" selections in the control.
    "Valid" times will fall between the min and max "pie wedge" of the
    clock. (This can be a "big piece", ie. <b>min = 11pm, max= 10pm</b>
    means <I>all but the hour from 10:00pm to 11pm are valid times.</I>)
    <DT><B>limited</B>
    <DD>If True, the control will not permit entry of values that fall outside the
    set bounds.
    <BR>
    <DT><B>oob_color</B>
    <DD>Sets the background color used to indicate out-of-bounds values for the control
    when the control is not limited.  This is set to "Yellow" by default.
    </DL>
</UL>
<BR>
<BR>
<BR>
<DT><B>EVT_TIMEUPDATE(win, id, func)</B>
<DD>func is fired whenever the value of the control changes.
<BR>
<BR>
<DT><B>SetValue(time_string | wxDateTime | wxTimeSpan | mx.DateTime | mx.DateTimeDelta)</B>
<DD>Sets the value of the control to a particular time, given a valid
value; raises ValueError on invalid value.
<EM>NOTE:</EM> This will only allow mx.DateTime or mx.DateTimeDelta if mx.DateTime
was successfully imported by the class module.
<BR>
<DT><B>GetValue(as_wxDateTime = False, as_mxDateTime = False, as_wxTimeSpan=False, as mxDateTimeDelta=False)</B>
<DD>Retrieves the value of the time from the control.  By default this is
returned as a string, unless one of the other arguments is set; args are
searched in the order listed; only one value will be returned.
<BR>
<DT><B>GetWxDateTime(value=None)</B>
<DD>When called without arguments, retrieves the value of the control, and applies
it to the wxDateTimeFromHMS() constructor, and returns the resulting value.
The date portion will always be set to Jan 1, 1970. This form is the same
as GetValue(as_wxDateTime=True).  GetWxDateTime can also be called with any of the
other valid time formats settable with SetValue, to regularize it to a single
wxDateTime form.  The function will raise ValueError on an unconvertable argument.
<BR>
<DT><B>GetMxDateTime()</B>
<DD>Retrieves the value of the control and applies it to the DateTime.Time()
constructor,and returns the resulting value.  (The date portion will always be
set to Jan 1, 1970.) (Same as GetValue(as_wxDateTime=True); provided for backward
compatibility with previous release.)
<BR>
<BR>
<DT><B>BindSpinButton(SpinBtton)</B>
<DD>Binds an externally created spin button to the control, so that up/down spin
events change the active cell or selection in the control (in addition to the
up/down cursor keys.)  (This is primarily to allow you to create a "standard"
interface to time controls, as seen in Windows.)
<BR>
<BR>
<DT><B>SetMin(min=None)</B>
<DD>Sets the expected minimum value, or lower bound, of the control.
(The lower bound will only be enforced if the control is
configured to limit its values to the set bounds.)
If a value of <I>None</I> is provided, then the control will have
explicit lower bound.  If the value specified is greater than
the current lower bound, then the function returns False and the
lower bound will not change from its current setting.  On success,
the function returns True.  Even if set, if there is no corresponding
upper bound, the control will behave as if it is unbounded.
<DT><DD>If successful and the current value is outside the
new bounds, if the control is limited the value will be
automatically adjusted to the nearest bound; if not limited,
the background of the control will be colored with the current
out-of-bounds color.
<BR>
<DT><B>GetMin(as_string=False)</B>
<DD>Gets the current lower bound value for the control, returning
None, if not set, or a wxDateTime, unless the as_string parameter
is set to True, at which point it will return the string
representation of the lower bound.
<BR>
<BR>
<DT><B>SetMax(max=None)</B>
<DD>Sets the expected maximum value, or upper bound, of the control.
(The upper bound will only be enforced if the control is
configured to limit its values to the set bounds.)
If a value of <I>None</I> is provided, then the control will
have no explicit upper bound.  If the value specified is less
than the current lower bound, then the function returns False and
the maximum will not change from its current setting. On success,
the function returns True.  Even if set, if there is no corresponding
lower bound, the control will behave as if it is unbounded.
<DT><DD>If successful and the current value is outside the
new bounds, if the control is limited the value will be
automatically adjusted to the nearest bound; if not limited,
the background of the control will be colored with the current
out-of-bounds color.
<BR>
<DT><B>GetMax(as_string = False)</B>
<DD>Gets the current upper bound value for the control, returning
None, if not set, or a wxDateTime, unless the as_string parameter
is set to True, at which point it will return the string
representation of the lower bound.

<BR>
<BR>
<DT><B>SetBounds(min=None,max=None)</B>
<DD>This function is a convenience function for setting the min and max
values at the same time.  The function only applies the maximum bound
if setting the minimum bound is successful, and returns True
only if both operations succeed.  <B><I>Note: leaving out an argument
will remove the corresponding bound, and result in the behavior of
an unbounded control.</I></B>
<BR>
<DT><B>GetBounds(as_string = False)</B>
<DD>This function returns a two-tuple (min,max), indicating the
current bounds of the control.  Each value can be None if
that bound is not set.  The values will otherwise be wxDateTimes
unless the as_string argument is set to True, at which point they
will be returned as string representations of the bounds.
<BR>
<BR>
<DT><B>IsInBounds(value=None)</B>
<DD>Returns <I>True</I> if no value is specified and the current value
of the control falls within the current bounds.  This function can also
be called with a value to see if that value would fall within the current
bounds of the given control.  It will raise ValueError if the value
specified is not a wxDateTime, mxDateTime (if available) or parsable string.
<BR>
<BR>
<DT><B>IsValid(value)</B>
<DD>Returns <I>True</I>if specified value is a legal time value and
falls within the current bounds of the given control.
<BR>
<BR>
<DT><B>SetLimited(bool)</B>
<DD>If called with a value of True, this function will cause the control
to limit the value to fall within the bounds currently specified.
(Provided both bounds have been set.)
If the control's value currently exceeds the bounds, it will then
be set to the nearest bound.
If called with a value of False, this function will disable value
limiting, but coloring of out-of-bounds values will still take
place if bounds have been set for the control.
<DT><B>IsLimited()</B>
<DD>Returns <I>True</I> if the control is currently limiting the
value to fall within the current bounds.
<BR>
</DL>
</body></html>
"""

import  copy
import  string
import  types

import  wx

from wx.tools.dbg import Logger
from wx.lib.masked import Field, BaseMaskedTextCtrl

dbg = Logger()
##dbg(enable=0)

try:
    from mx import DateTime
    accept_mx = True
except ImportError:
    accept_mx = False

# This class of event fires whenever the value of the time changes in the control:
wxEVT_TIMEVAL_UPDATED = wx.NewEventType()
EVT_TIMEUPDATE = wx.PyEventBinder(wxEVT_TIMEVAL_UPDATED, 1)

class TimeUpdatedEvent(wx.PyCommandEvent):
    def __init__(self, id, value ='12:00:00 AM'):
        wx.PyCommandEvent.__init__(self, wxEVT_TIMEVAL_UPDATED, id)
        self.value = value
    def GetValue(self):
        """Retrieve the value of the time control at the time this event was generated"""
        return self.value

class TimeCtrlAccessorsMixin:
    # Define TimeCtrl's list of attributes having their own
    # Get/Set functions, ignoring those that make no sense for
    # an numeric control.
    exposed_basectrl_params = (
         'defaultValue',
         'description',

         'useFixedWidthFont',
         'emptyBackgroundColour',
         'validBackgroundColour',
         'invalidBackgroundColour',

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


class TimeCtrl(BaseMaskedTextCtrl):

    valid_ctrl_params = {
        'format' :  'HHMMSS',       # default format code
        'displaySeconds' : True,    # by default, shows seconds
        'min': None,                # by default, no bounds set
        'max': None,
        'limited': False,           # by default, no limiting even if bounds set
        'useFixedWidthFont': True,  # by default, use a fixed-width font
        'oob_color': "Yellow"       # by default, the default masked.TextCtrl "invalid" color
        }

    def __init__ (
                self, parent, id=-1, value = '00:00:00',
                pos = wx.DefaultPosition, size = wx.DefaultSize,
                fmt24hr=False,
                spinButton = None,
                style = wx.TE_PROCESS_TAB,
                validator = wx.DefaultValidator,
                name = "time",
                **kwargs ):

        # set defaults for control:
##        dbg('setting defaults:')

        self.__fmt24hr = False
        wxdt = wx.DateTimeFromDMY(1, 0, 1970)
        if wxdt.Format('%p') != 'AM':
            TimeCtrl.valid_ctrl_params['format'] = '24HHMMSS'
            self.__fmt24hr = True
            fmt24hr = True  # force/change default positional argument
                            # (will countermand explicit set to False too.)

        for key, param_value in TimeCtrl.valid_ctrl_params.items():
            # This is done this way to make setattr behave consistently with
            # "private attribute" name mangling
            setattr(self, "_TimeCtrl__" + key, copy.copy(param_value))

        # create locals from current defaults, so we can override if
        # specified in kwargs, and handle uniformly:
        min = self.__min
        max = self.__max
        limited = self.__limited
        self.__posCurrent = 0
        # handle deprecated keword argument name:
        if kwargs.has_key('display_seconds'):
            kwargs['displaySeconds'] = kwargs['display_seconds']
            del kwargs['display_seconds']
        if not kwargs.has_key('displaySeconds'):
            kwargs['displaySeconds'] = True

        # (handle positional arg (from original release) differently from rest of kwargs:)
        if not kwargs.has_key('format'):
            if fmt24hr:
                if kwargs.has_key('displaySeconds') and kwargs['displaySeconds']:
                    kwargs['format'] = '24HHMMSS'
                    del kwargs['displaySeconds']
                else:
                    kwargs['format'] = '24HHMM'
            else:
                if kwargs.has_key('displaySeconds') and kwargs['displaySeconds']:
                    kwargs['format'] = 'HHMMSS'
                    del kwargs['displaySeconds']
                else:
                    kwargs['format'] = 'HHMM'

        if not kwargs.has_key('useFixedWidthFont'):
            # allow control over font selection:
            kwargs['useFixedWidthFont'] = self.__useFixedWidthFont

        maskededit_kwargs = self.SetParameters(**kwargs)

        # allow for explicit size specification:
        if size != wx.DefaultSize:
            # override (and remove) "autofit" autoformat code in standard time formats:
            maskededit_kwargs['formatcodes'] = 'T!'

        # This allows range validation if set
        maskededit_kwargs['validFunc'] = self.IsInBounds

        # This allows range limits to affect insertion into control or not
        # dynamically without affecting individual field constraint validation
        maskededit_kwargs['retainFieldValidation'] = True

        # Now we can initialize the base control:
        BaseMaskedTextCtrl.__init__(
                self, parent, id=id,
                pos=pos, size=size,
                style = style,
                validator = validator,
                name = name,
                setupEventHandling = False,
                **maskededit_kwargs)


        # This makes ':' act like tab (after we fix each ':' key event to remove "shift")
        self._SetKeyHandler(':', self._OnChangeField)


        # This makes the up/down keys act like spin button controls:
        self._SetKeycodeHandler(wx.WXK_UP, self.__OnSpinUp)
        self._SetKeycodeHandler(wx.WXK_DOWN, self.__OnSpinDown)


        # This allows ! and c/C to set the control to the current time:
        self._SetKeyHandler('!', self.__OnSetToNow)
        self._SetKeyHandler('c', self.__OnSetToNow)
        self._SetKeyHandler('C', self.__OnSetToNow)


        # Set up event handling ourselves, so we can insert special
        # processing on the ":' key to remove the "shift" attribute
        # *before* the default handlers have been installed, so
        # that : takes you forward, not back, and so we can issue
        # EVT_TIMEUPDATE events on changes:

        self.Bind(wx.EVT_SET_FOCUS, self._OnFocus )         ## defeat automatic full selection
        self.Bind(wx.EVT_KILL_FOCUS, self._OnKillFocus )    ## run internal validator
        self.Bind(wx.EVT_LEFT_UP, self.__LimitSelection)    ## limit selections to single field
        self.Bind(wx.EVT_LEFT_DCLICK, self._OnDoubleClick ) ## select field under cursor on dclick
        self.Bind(wx.EVT_KEY_DOWN, self._OnKeyDown )        ## capture control events not normally seen, eg ctrl-tab.
        self.Bind(wx.EVT_CHAR, self.__OnChar )              ## remove "shift" attribute from colon key event,
                                                            ## then call BaseMaskedTextCtrl._OnChar with
                                                            ## the possibly modified event.
        self.Bind(wx.EVT_TEXT, self.__OnTextChange, self )  ## color control appropriately and EVT_TIMEUPDATE events


        # Validate initial value and set if appropriate
        try:
            self.SetBounds(min, max)
            self.SetLimited(limited)
            self.SetValue(value)
        except:
            self.SetValue('00:00:00')

        if spinButton:
            self.BindSpinButton(spinButton)     # bind spin button up/down events to this control


    def SetParameters(self, **kwargs):
##        dbg('TimeCtrl::SetParameters(%s)' % repr(kwargs), indent=1)
        maskededit_kwargs = {}
        reset_format = False

        if kwargs.has_key('display_seconds'):
            kwargs['displaySeconds'] = kwargs['display_seconds']
            del kwargs['display_seconds']
        if kwargs.has_key('format') and kwargs.has_key('displaySeconds'):
            del kwargs['displaySeconds']    # always apply format if specified

        # assign keyword args as appropriate:
        for key, param_value in kwargs.items():
            if key not in TimeCtrl.valid_ctrl_params.keys():
                raise AttributeError('invalid keyword argument "%s"' % key)

            if key == 'format':
                wxdt = wx.DateTimeFromDMY(1, 0, 1970)
                if wxdt.Format('%p') != 'AM':
                    require24hr = True
                else:
                    require24hr = False

                # handle both local or generic 'maskededit' autoformat codes:
                if param_value == 'HHMMSS' or param_value == 'TIMEHHMMSS':
                    self.__displaySeconds = True
                    self.__fmt24hr = False
                elif param_value == 'HHMM' or param_value == 'TIMEHHMM':
                    self.__displaySeconds = False
                    self.__fmt24hr = False
                elif param_value == '24HHMMSS' or param_value == '24HRTIMEHHMMSS':
                    self.__displaySeconds = True
                    self.__fmt24hr = True
                elif param_value == '24HHMM' or param_value == '24HRTIMEHHMM':
                    self.__displaySeconds = False
                    self.__fmt24hr = True
                else:
                    raise AttributeError('"%s" is not a valid format' % param_value)

                if require24hr and not self.__fmt24hr:
                    raise AttributeError('"%s" is an unsupported time format for the current locale' % param_value)

                reset_format = True

            elif key in ("displaySeconds",  "display_seconds") and not kwargs.has_key('format'):
                self.__displaySeconds = param_value
                reset_format = True

            elif key == "min":      min = param_value
            elif key == "max":      max = param_value
            elif key == "limited":  limited = param_value

            elif key == "useFixedWidthFont":
                maskededit_kwargs[key] = param_value

            elif key == "oob_color":
                maskededit_kwargs['invalidBackgroundColor'] = param_value

        if reset_format:
            if self.__fmt24hr:
                if self.__displaySeconds:  maskededit_kwargs['autoformat'] = '24HRTIMEHHMMSS'
                else:                      maskededit_kwargs['autoformat'] = '24HRTIMEHHMM'

                # Set hour field to zero-pad, right-insert, require explicit field change,
                # select entire field on entry, and require a resultant valid entry
                # to allow character entry:
                hourfield = Field(formatcodes='0r<SV', validRegex='0\d|1\d|2[0123]', validRequired=True)
            else:
                if self.__displaySeconds:  maskededit_kwargs['autoformat'] = 'TIMEHHMMSS'
                else:                      maskededit_kwargs['autoformat'] = 'TIMEHHMM'

                # Set hour field to allow spaces (at start), right-insert,
                # require explicit field change, select entire field on entry,
                # and require a resultant valid entry to allow character entry:
                hourfield = Field(formatcodes='_0<rSV', validRegex='0[1-9]| [1-9]|1[012]', validRequired=True)
                ampmfield = Field(formatcodes='S', emptyInvalid = True, validRequired = True)

            # Field 1 is always a zero-padded right-insert minute field,
            # similarly configured as above:
            minutefield = Field(formatcodes='0r<SV', validRegex='[0-5]\d', validRequired=True)

            fields = [ hourfield, minutefield ]
            if self.__displaySeconds:
                fields.append(copy.copy(minutefield))    # second field has same constraints as field 1

            if not self.__fmt24hr:
                fields.append(ampmfield)

            # set fields argument:
            maskededit_kwargs['fields'] = fields

            # This allows range validation if set
            maskededit_kwargs['validFunc'] = self.IsInBounds

            # This allows range limits to affect insertion into control or not
            # dynamically without affecting individual field constraint validation
            maskededit_kwargs['retainFieldValidation'] = True

        if hasattr(self, 'controlInitialized') and self.controlInitialized:
            self.SetCtrlParameters(**maskededit_kwargs)   # set appropriate parameters

            # Validate initial value and set if appropriate
            try:
                self.SetBounds(min, max)
                self.SetLimited(limited)
                self.SetValue(value)
            except:
                self.SetValue('00:00:00')
##            dbg(indent=0)
            return {}   # no arguments to return
        else:
##            dbg(indent=0)
            return maskededit_kwargs


    def BindSpinButton(self, sb):
        """
        This function binds an externally created spin button to the control, so that
        up/down events from the button automatically change the control.
        """
##        dbg('TimeCtrl::BindSpinButton')
        self.__spinButton = sb
        if self.__spinButton:
            # bind event handlers to spin ctrl
            self.__spinButton.Bind(wx.EVT_SPIN_UP, self.__OnSpinUp, self.__spinButton)
            self.__spinButton.Bind(wx.EVT_SPIN_DOWN, self.__OnSpinDown, self.__spinButton)


    def __repr__(self):
        return "<TimeCtrl: %s>" % self.GetValue()


    def SetValue(self, value):
        """
        Validating SetValue function for time values:
        This function will do dynamic type checking on the value argument,
        and convert wxDateTime, mxDateTime, or 12/24 format time string
        into the appropriate format string for the control.
        """
##        dbg('TimeCtrl::SetValue(%s)' % repr(value), indent=1)
        try:
            strtime = self._toGUI(self.__validateValue(value))
        except:
##            dbg('validation failed', indent=0)
            raise

##        dbg('strtime:', strtime)
        self._SetValue(strtime)
##        dbg(indent=0)

    def GetValue(self,
                 as_wxDateTime = False,
                 as_mxDateTime = False,
                 as_wxTimeSpan = False,
                 as_mxDateTimeDelta = False):


        if as_wxDateTime or as_mxDateTime or as_wxTimeSpan or as_mxDateTimeDelta:
            value = self.GetWxDateTime()
            if as_wxDateTime:
                pass
            elif as_mxDateTime:
                value = DateTime.DateTime(1970, 1, 1, value.GetHour(), value.GetMinute(), value.GetSecond())
            elif as_wxTimeSpan:
                value = wx.TimeSpan(value.GetHour(), value.GetMinute(), value.GetSecond())
            elif as_mxDateTimeDelta:
                value = DateTime.DateTimeDelta(0, value.GetHour(), value.GetMinute(), value.GetSecond())
        else:
            value = BaseMaskedTextCtrl.GetValue(self)
        return value


    def SetWxDateTime(self, wxdt):
        """
        Because SetValue can take a wxDateTime, this is now just an alias.
        """
        self.SetValue(wxdt)


    def GetWxDateTime(self, value=None):
        """
        This function is the conversion engine for TimeCtrl; it takes
        one of the following types:
            time string
            wxDateTime
            wxTimeSpan
            mxDateTime
            mxDateTimeDelta
        and converts it to a wxDateTime that always has Jan 1, 1970 as its date
        portion, so that range comparisons around values can work using
        wxDateTime's built-in comparison function.  If a value is not
        provided to convert, the string value of the control will be used.
        If the value is not one of the accepted types, a ValueError will be
        raised.
        """
        global accept_mx
##        dbg(suspend=1)
##        dbg('TimeCtrl::GetWxDateTime(%s)' % repr(value), indent=1)
        if value is None:
##            dbg('getting control value')
            value = self.GetValue()
##            dbg('value = "%s"' % value)

        if type(value) == types.UnicodeType:
            value = str(value)  # convert to regular string

        valid = True    # assume true
        if type(value) == types.StringType:

            # Construct constant wxDateTime, then try to parse the string:
            wxdt = wx.DateTimeFromDMY(1, 0, 1970)
##            dbg('attempting conversion')
            value = value.strip()    # (parser doesn't like leading spaces)
            checkTime    = wxdt.ParseTime(value)
            valid = checkTime == len(value)     # entire string parsed?
##            dbg('checkTime == len(value)?', valid)

            if not valid:
                # deal with bug/deficiency in wx.DateTime:
                if wxdt.Format('%p') not in ('AM', 'PM') and checkTime in (5,8):
                    # couldn't parse the AM/PM field
                    raise ValueError('cannot convert string "%s" to valid time for the current locale; please use 24hr time instead' % value)
                else:
##                dbg(indent=0, suspend=0)
                    raise ValueError('cannot convert string "%s" to valid time' % value)

        else:
            if isinstance(value, wx.DateTime):
                hour, minute, second = value.GetHour(), value.GetMinute(), value.GetSecond()
            elif isinstance(value, wx.TimeSpan):
                totalseconds = value.GetSeconds()
                hour = totalseconds / 3600
                minute = totalseconds / 60 - (hour * 60)
                second = totalseconds - ((hour * 3600) + (minute * 60))

            elif accept_mx and isinstance(value, DateTime.DateTimeType):
                hour, minute, second = value.hour, value.minute, value.second
            elif accept_mx and isinstance(value, DateTime.DateTimeDeltaType):
                hour, minute, second = value.hour, value.minute, value.second
            else:
                # Not a valid function argument
                if accept_mx:
                    error = 'GetWxDateTime requires wxDateTime, mxDateTime or parsable time string, passed %s'% repr(value)
                else:
                    error = 'GetWxDateTime requires wxDateTime or parsable time string, passed %s'% repr(value)
##                dbg(indent=0, suspend=0)
                raise ValueError(error)

            wxdt = wx.DateTimeFromDMY(1, 0, 1970)
            wxdt.SetHour(hour)
            wxdt.SetMinute(minute)
            wxdt.SetSecond(second)

##        dbg('wxdt:', wxdt, indent=0, suspend=0)
        return wxdt


    def SetMxDateTime(self, mxdt):
        """
        Because SetValue can take an mxDateTime, (if DateTime is importable),
        this is now just an alias.
        """
        self.SetValue(value)


    def GetMxDateTime(self, value=None):
        if value is None:
            t = self.GetValue(as_mxDateTime=True)
        else:
            # Convert string 1st to wxDateTime, then use components, since
            # mx' DateTime.Parser.TimeFromString() doesn't handle AM/PM:
            wxdt = self.GetWxDateTime(value)
            hour, minute, second = wxdt.GetHour(), wxdt.GetMinute(), wxdt.GetSecond()
            t = DateTime.DateTime(1970,1,1) + DateTimeDelta(0, hour, minute, second)
        return t


    def SetMin(self, min=None):
        """
        Sets the minimum value of the control.  If a value of None
        is provided, then the control will have no explicit minimum value.
        If the value specified is greater than the current maximum value,
        then the function returns 0 and the minimum will not change from
        its current setting.  On success, the function returns 1.

        If successful and the current value is lower than the new lower
        bound, if the control is limited, the value will be automatically
        adjusted to the new minimum value; if not limited, the value in the
        control will be colored as invalid.
        """
##        dbg('TimeCtrl::SetMin(%s)'% repr(min), indent=1)
        if min is not None:
            try:
                min = self.GetWxDateTime(min)
                self.__min = self._toGUI(min)
            except:
##                dbg('exception occurred', indent=0)
                return False
        else:
            self.__min = min

        if self.IsLimited() and not self.IsInBounds():
            self.SetLimited(self.__limited) # force limited value:
        else:
            self._CheckValid()
        ret = True
##        dbg('ret:', ret, indent=0)
        return ret


    def GetMin(self, as_string = False):
        """
        Gets the minimum value of the control.
        If None, it will return None.  Otherwise it will return
        the current minimum bound on the control, as a wxDateTime
        by default, or as a string if as_string argument is True.
        """
##        dbg(suspend=1)
##        dbg('TimeCtrl::GetMin, as_string?', as_string, indent=1)
        if self.__min is None:
##            dbg('(min == None)')
            ret = self.__min
        elif as_string:
            ret = self.__min
##            dbg('ret:', ret)
        else:
            try:
                ret = self.GetWxDateTime(self.__min)
            except:
##                dbg(suspend=0)
##                dbg('exception occurred', indent=0)
                raise
##            dbg('ret:', repr(ret))
##        dbg(indent=0, suspend=0)
        return ret


    def SetMax(self, max=None):
        """
        Sets the maximum value of the control. If a value of None
        is provided, then the control will have no explicit maximum value.
        If the value specified is less than the current minimum value, then
        the function returns False and the maximum will not change from its
        current setting. On success, the function returns True.

        If successful and the current value is greater than the new upper
        bound, if the control is limited the value will be automatically
        adjusted to this maximum value; if not limited, the value in the
        control will be colored as invalid.
        """
##        dbg('TimeCtrl::SetMax(%s)' % repr(max), indent=1)
        if max is not None:
            try:
                max = self.GetWxDateTime(max)
                self.__max = self._toGUI(max)
            except:
##                dbg('exception occurred', indent=0)
                return False
        else:
            self.__max = max
##        dbg('max:', repr(self.__max))
        if self.IsLimited() and not self.IsInBounds():
            self.SetLimited(self.__limited) # force limited value:
        else:
            self._CheckValid()
        ret = True
##        dbg('ret:', ret, indent=0)
        return ret


    def GetMax(self, as_string = False):
        """
        Gets the minimum value of the control.
        If None, it will return None.  Otherwise it will return
        the current minimum bound on the control, as a wxDateTime
        by default, or as a string if as_string argument is True.
        """
##        dbg(suspend=1)
##        dbg('TimeCtrl::GetMin, as_string?', as_string, indent=1)
        if self.__max is None:
##            dbg('(max == None)')
            ret = self.__max
        elif as_string:
            ret = self.__max
##            dbg('ret:', ret)
        else:
            try:
                ret = self.GetWxDateTime(self.__max)
            except:
##                dbg(suspend=0)
##                dbg('exception occurred', indent=0)
                raise
##            dbg('ret:', repr(ret))
##        dbg(indent=0, suspend=0)
        return ret


    def SetBounds(self, min=None, max=None):
        """
        This function is a convenience function for setting the min and max
        values at the same time.  The function only applies the maximum bound
        if setting the minimum bound is successful, and returns True
        only if both operations succeed.
        NOTE: leaving out an argument will remove the corresponding bound.
        """
        ret = self.SetMin(min)
        return ret and self.SetMax(max)


    def GetBounds(self, as_string = False):
        """
        This function returns a two-tuple (min,max), indicating the
        current bounds of the control.  Each value can be None if
        that bound is not set.
        """
        return (self.GetMin(as_string), self.GetMax(as_string))


    def SetLimited(self, limited):
        """
        If called with a value of True, this function will cause the control
        to limit the value to fall within the bounds currently specified.
        If the control's value currently exceeds the bounds, it will then
        be limited accordingly.

        If called with a value of 0, this function will disable value
        limiting, but coloring of out-of-bounds values will still take
        place if bounds have been set for the control.
        """
##        dbg('TimeCtrl::SetLimited(%d)' % limited, indent=1)
        self.__limited = limited

        if not limited:
            self.SetMaskParameters(validRequired = False)
            self._CheckValid()
##            dbg(indent=0)
            return

##        dbg('requiring valid value')
        self.SetMaskParameters(validRequired = True)

        min = self.GetMin()
        max = self.GetMax()
        if min is None or max is None:
##            dbg('both bounds not set; no further action taken')
            return  # can't limit without 2 bounds

        elif not self.IsInBounds():
            # set value to the nearest bound:
            try:
                value = self.GetWxDateTime()
            except:
##                dbg('exception occurred', indent=0)
                raise

            if min <= max:   # valid range doesn't span midnight
##                dbg('min <= max')
                # which makes the "nearest bound" computation trickier...

                # determine how long the "invalid" pie wedge is, and cut
                # this interval in half for comparison purposes:

                # Note: relies on min and max and value date portions
                # always being the same.
                interval = (min + wx.TimeSpan(24, 0, 0, 0)) - max

                half_interval = wx.TimeSpan(
                                    0,      # hours
                                    0,      # minutes
                                    interval.GetSeconds() / 2,  # seconds
                                    0)      # msec

                if value < min: # min is on next day, so use value on
                    # "next day" for "nearest" interval calculation:
                    cmp_value = value + wx.TimeSpan(24, 0, 0, 0)
                else:   # "before midnight; ok
                    cmp_value = value

                if (cmp_value - max) > half_interval:
##                    dbg('forcing value to min (%s)' % min.FormatTime())
                    self.SetValue(min)
                else:
##                    dbg('forcing value to max (%s)' % max.FormatTime())
                    self.SetValue(max)
            else:
##                dbg('max < min')
                # therefore  max < value < min guaranteed to be true,
                # so "nearest bound" calculation is much easier:
                if (value - max) >= (min - value):
                    # current value closer to min; pick that edge of pie wedge
##                    dbg('forcing value to min (%s)' % min.FormatTime())
                    self.SetValue(min)
                else:
##                    dbg('forcing value to max (%s)' % max.FormatTime())
                    self.SetValue(max)

##        dbg(indent=0)



    def IsLimited(self):
        """
        Returns True if the control is currently limiting the
        value to fall within any current bounds.  Note: can
        be set even if there are no current bounds.
        """
        return self.__limited


    def IsInBounds(self, value=None):
        """
        Returns True if no value is specified and the current value
        of the control falls within the current bounds.  As the clock
        is a "circle", both minimum and maximum bounds must be set for
        a value to ever be considered "out of bounds".  This function can
        also be called with a value to see if that value would fall within
        the current bounds of the given control.
        """
        if value is not None:
            try:
                value = self.GetWxDateTime(value)   # try to regularize passed value
            except ValueError:
##                dbg('ValueError getting wxDateTime for %s' % repr(value), indent=0)
                raise

##        dbg('TimeCtrl::IsInBounds(%s)' % repr(value), indent=1)
        if self.__min is None or self.__max is None:
##            dbg(indent=0)
            return True

        elif value is None:
            try:
                value = self.GetWxDateTime()
            except:
##                dbg('exception occurred', indent=0)
                raise

##        dbg('value:', value.FormatTime())

        # Get wxDateTime representations of bounds:
        min = self.GetMin()
        max = self.GetMax()

        midnight = wx.DateTimeFromDMY(1, 0, 1970)
        if min <= max:   # they don't span midnight
            ret = min <= value <= max

        else:
            # have to break into 2 tests; to be in bounds
            # either "min" <= value (<= midnight of *next day*)
            # or midnight <= value <= "max"
            ret = min <= value or (midnight <= value <= max)
##        dbg('in bounds?', ret, indent=0)
        return ret


    def IsValid( self, value ):
        """
        Can be used to determine if a given value would be a legal and
        in-bounds value for the control.
        """
        try:
            self.__validateValue(value)
            return True
        except ValueError:
            return False

    def SetFormat(self, format):
        self.SetParameters(format=format)

    def GetFormat(self):
        if self.__displaySeconds:
            if self.__fmt24hr: return '24HHMMSS'
            else:              return 'HHMMSS'
        else:
            if self.__fmt24hr: return '24HHMM'
            else:              return 'HHMM'

#-------------------------------------------------------------------------------------------------------------
# these are private functions and overrides:


    def __OnTextChange(self, event=None):
##        dbg('TimeCtrl::OnTextChange', indent=1)

        # Allow Maskedtext base control to color as appropriate,
        # and Skip the EVT_TEXT event (if appropriate.)
        ##! WS: For some inexplicable reason, every wxTextCtrl.SetValue()
        ## call is generating two (2) EVT_TEXT events. (!)
        ## The the only mechanism I can find to mask this problem is to
        ## keep track of last value seen, and declare a valid EVT_TEXT
        ## event iff the value has actually changed.  The masked edit
        ## OnTextChange routine does this, and returns True on a valid event,
        ## False otherwise.
        if not BaseMaskedTextCtrl._OnTextChange(self, event):
            return

##        dbg('firing TimeUpdatedEvent...')
        evt = TimeUpdatedEvent(self.GetId(), self.GetValue())
        evt.SetEventObject(self)
        self.GetEventHandler().ProcessEvent(evt)
##        dbg(indent=0)


    def SetInsertionPoint(self, pos):
        """
        Records the specified position and associated cell before calling base class' function.
        This is necessary to handle the optional spin button, because the insertion
        point is lost when the focus shifts to the spin button.
        """
##        dbg('TimeCtrl::SetInsertionPoint', pos, indent=1)
        BaseMaskedTextCtrl.SetInsertionPoint(self, pos)                 # (causes EVT_TEXT event to fire)
        self.__posCurrent = self.GetInsertionPoint()
##        dbg(indent=0)


    def SetSelection(self, sel_start, sel_to):
##        dbg('TimeCtrl::SetSelection', sel_start, sel_to, indent=1)

        # Adjust selection range to legal extent if not already
        if sel_start < 0:
            sel_start = 0

        if self.__posCurrent != sel_start:                      # force selection and insertion point to match
            self.SetInsertionPoint(sel_start)
        cell_start, cell_end = self._FindField(sel_start)._extent
        if not cell_start <= sel_to <= cell_end:
            sel_to = cell_end

        self.__bSelection = sel_start != sel_to
        BaseMaskedTextCtrl.SetSelection(self, sel_start, sel_to)
##        dbg(indent=0)


    def __OnSpin(self, key):
        """
        This is the function that gets called in response to up/down arrow or
        bound spin button events.
        """
        self.__IncrementValue(key, self.__posCurrent)   # changes the value

        # Ensure adjusted control regains focus and has adjusted portion
        # selected:
        self.SetFocus()
        start, end = self._FindField(self.__posCurrent)._extent
        self.SetInsertionPoint(start)
        self.SetSelection(start, end)
##        dbg('current position:', self.__posCurrent)


    def __OnSpinUp(self, event):
        """
        Event handler for any bound spin button on EVT_SPIN_UP;
        causes control to behave as if up arrow was pressed.
        """
##        dbg('TimeCtrl::OnSpinUp', indent=1)
        self.__OnSpin(wx.WXK_UP)
        keep_processing = False
##        dbg(indent=0)
        return keep_processing


    def __OnSpinDown(self, event):
        """
        Event handler for any bound spin button on EVT_SPIN_DOWN;
        causes control to behave as if down arrow was pressed.
        """
##        dbg('TimeCtrl::OnSpinDown', indent=1)
        self.__OnSpin(wx.WXK_DOWN)
        keep_processing = False
##        dbg(indent=0)
        return keep_processing


    def __OnChar(self, event):
        """
        Handler to explicitly look for ':' keyevents, and if found,
        clear the m_shiftDown field, so it will behave as forward tab.
        It then calls the base control's _OnChar routine with the modified
        event instance.
        """
##        dbg('TimeCtrl::OnChar', indent=1)
        keycode = event.GetKeyCode()
##        dbg('keycode:', keycode)
        if keycode == ord(':'):
##            dbg('colon seen! removing shift attribute')
            event.m_shiftDown = False
        BaseMaskedTextCtrl._OnChar(self, event )              ## handle each keypress
##        dbg(indent=0)


    def __OnSetToNow(self, event):
        """
        This is the key handler for '!' and 'c'; this allows the user to
        quickly set the value of the control to the current time.
        """
        self.SetValue(wx.DateTime_Now().FormatTime())
        keep_processing = False
        return keep_processing


    def __LimitSelection(self, event):
        """
        Event handler for motion events; this handler
        changes limits the selection to the new cell boundaries.
        """
##        dbg('TimeCtrl::LimitSelection', indent=1)
        pos = self.GetInsertionPoint()
        self.__posCurrent = pos
        sel_start, sel_to = self.GetSelection()
        selection = sel_start != sel_to
        if selection:
            # only allow selection to end of current cell:
            start, end = self._FindField(sel_start)._extent
            if sel_to < pos:   sel_to = start
            elif sel_to > pos: sel_to = end

##        dbg('new pos =', self.__posCurrent, 'select to ', sel_to)
        self.SetInsertionPoint(self.__posCurrent)
        self.SetSelection(self.__posCurrent, sel_to)
        if event: event.Skip()
##        dbg(indent=0)


    def __IncrementValue(self, key, pos):
##        dbg('TimeCtrl::IncrementValue', key, pos, indent=1)
        text = self.GetValue()
        field = self._FindField(pos)
##        dbg('field: ', field._index)
        start, end = field._extent
        slice = text[start:end]
        if key == wx.WXK_UP: increment = 1
        else:             increment = -1

        if slice in ('A', 'P'):
            if slice == 'A': newslice = 'P'
            elif slice == 'P': newslice = 'A'
            newvalue = text[:start] + newslice + text[end:]

        elif field._index == 0:
            # adjusting this field is trickier, as its value can affect the
            # am/pm setting.  So, we use wxDateTime to generate a new value for us:
            # (Use a fixed date not subject to DST variations:)
            converter = wx.DateTimeFromDMY(1, 0, 1970)
##            dbg('text: "%s"' % text)
            converter.ParseTime(text.strip())
            currenthour = converter.GetHour()
##            dbg('current hour:', currenthour)
            newhour = (currenthour + increment) % 24
##            dbg('newhour:', newhour)
            converter.SetHour(newhour)
##            dbg('converter.GetHour():', converter.GetHour())
            newvalue = converter     # take advantage of auto-conversion for am/pm in .SetValue()

        else:   # minute or second field; handled the same way:
            newslice = "%02d" % ((int(slice) + increment) % 60)
            newvalue = text[:start] + newslice + text[end:]

        try:
            self.SetValue(newvalue)

        except ValueError:  # must not be in bounds:
            if not wx.Validator_IsSilent():
                wx.Bell()
##        dbg(indent=0)


    def _toGUI( self, wxdt ):
        """
        This function takes a wxdt as an unambiguous representation of a time, and
        converts it to a string appropriate for the format of the control.
        """
        if self.__fmt24hr:
            if self.__displaySeconds: strval = wxdt.Format('%H:%M:%S')
            else:                     strval = wxdt.Format('%H:%M')
        else:
            if self.__displaySeconds: strval = wxdt.Format('%I:%M:%S %p')
            else:                     strval = wxdt.Format('%I:%M %p')

        return strval


    def __validateValue( self, value ):
        """
        This function converts the value to a wxDateTime if not already one,
        does bounds checking and raises ValueError if argument is
        not a valid value for the control as currently specified.
        It is used by both the SetValue() and the IsValid() methods.
        """
##        dbg('TimeCtrl::__validateValue(%s)' % repr(value), indent=1)
        if not value:
##            dbg(indent=0)
            raise ValueError('%s not a valid time value' % repr(value))

        valid = True    # assume true
        try:
            value = self.GetWxDateTime(value)   # regularize form; can generate ValueError if problem doing so
        except:
##            dbg('exception occurred', indent=0)
            raise

        if self.IsLimited() and not self.IsInBounds(value):
##            dbg(indent=0)
            raise ValueError (
                'value %s is not within the bounds of the control' % str(value) )
##        dbg(indent=0)
        return value

#----------------------------------------------------------------------------
# Test jig for TimeCtrl:

if __name__ == '__main__':
    import traceback

    class TestPanel(wx.Panel):
        def __init__(self, parent, id,
                     pos = wx.DefaultPosition, size = wx.DefaultSize,
                     fmt24hr = 0, test_mx = 0,
                     style = wx.TAB_TRAVERSAL ):

            wx.Panel.__init__(self, parent, id, pos, size, style)

            self.test_mx = test_mx

            self.tc = TimeCtrl(self, 10, fmt24hr = fmt24hr)
            sb = wx.SpinButton( self, 20, wx.DefaultPosition, (-1,20), 0 )
            self.tc.BindSpinButton(sb)

            sizer = wx.BoxSizer( wx.HORIZONTAL )
            sizer.Add( self.tc, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.TOP|wx.BOTTOM, 5 )
            sizer.Add( sb, 0, wx.ALIGN_CENTRE|wx.RIGHT|wx.TOP|wx.BOTTOM, 5 )

            self.SetAutoLayout( True )
            self.SetSizer( sizer )
            sizer.Fit( self )
            sizer.SetSizeHints( self )

            self.Bind(EVT_TIMEUPDATE, self.OnTimeChange, self.tc)

        def OnTimeChange(self, event):
##            dbg('OnTimeChange: value = ', event.GetValue())
            wxdt = self.tc.GetWxDateTime()
##            dbg('wxdt =', wxdt.GetHour(), wxdt.GetMinute(), wxdt.GetSecond())
            if self.test_mx:
                mxdt = self.tc.GetMxDateTime()
##                dbg('mxdt =', mxdt.hour, mxdt.minute, mxdt.second)


    class MyApp(wx.App):
        def OnInit(self):
            import sys
            fmt24hr = '24' in sys.argv
            test_mx = 'mx' in sys.argv
            try:
                frame = wx.Frame(None, -1, "TimeCtrl Test", (20,20), (100,100) )
                panel = TestPanel(frame, -1, (-1,-1), fmt24hr=fmt24hr, test_mx = test_mx)
                frame.Show(True)
            except:
                traceback.print_exc()
                return False
            return True

    try:
        app = MyApp(0)
        app.MainLoop()
    except:
        traceback.print_exc()
i=0
## Version 1.2
##   1. Changed parameter name display_seconds to displaySeconds, to follow
##      other masked edit conventions.
##   2. Added format parameter, to remove need to use both fmt24hr and displaySeconds.
##   3. Changed inheritance to use BaseMaskedTextCtrl, to remove exposure of
##      nonsensical parameter methods from the control, so it will work
##      properly with Boa.
