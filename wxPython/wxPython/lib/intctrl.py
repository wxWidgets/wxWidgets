#----------------------------------------------------------------------------
# Name:         wxPython.lib.intctrl.py
# Author:       Will Sadkin
# Created:      01/16/2003
# Copyright:   (c) 2003 by Will Sadkin
# RCS-ID:      $Id$
# License:     wxWindows license
#----------------------------------------------------------------------------
# NOTE:
#   This was written to provide a standard integer edit control for wxPython.
#
#   wxIntCtrl permits integer (long) values to be retrieved or  set via
#   .GetValue() and .SetValue(), and provides an EVT_INT() event function
#   for trapping changes to the control.
#
#   It supports negative integers as well as the naturals, and does not
#   permit leading zeros or an empty control; attempting to delete the
#   contents of the control will result in a (selected) value of zero,
#   thus preserving a legitimate integer value, or an empty control
#   (if a value of None is allowed for the control.) Similarly, replacing the
#   contents of the control with '-' will result in a selected (absolute)
#   value of -1.
#
#   wxIntCtrl also supports range limits, with the option of either
#   enforcing them or simply coloring the text of the control if the limits
#   are exceeded.

from wxPython.wx import *
import types, string
from sys import maxint
MAXINT = maxint     # (constants should be in upper case)
MININT = -maxint-1

#----------------------------------------------------------------------------

wxEVT_COMMAND_INT_UPDATED = wxNewEventType()

# wxWindows' wxTextCtrl translates Composite "control key"
# events into single events before returning them to its OnChar
# routine.  The doc says that this results in 1 for Ctrl-A, 2 for
# Ctrl-B, etc. However, there are no wxPython or wxWindows
# symbols for them, so I'm defining codes for Ctrl-X (cut) and
# Ctrl-V (paste) here for readability:
WXK_CTRL_X = (ord('X')+1) - ord('A')
WXK_CTRL_V = (ord('V')+1) - ord('A')


def EVT_INT(win, id, func):
    """Used to trap events indicating that the current
    integer value of the control has been changed."""
    win.Connect(id, -1, wxEVT_COMMAND_INT_UPDATED, func)


class wxIntUpdatedEvent(wxPyCommandEvent):
    def __init__(self, id, value = 0, object=None):
        wxPyCommandEvent.__init__(self, wxEVT_COMMAND_INT_UPDATED, id)

        self.__value = value
        self.SetEventObject(object)

    def GetValue(self):
        """Retrieve the value of the control at the time
        this event was generated."""
        return self.__value


#----------------------------------------------------------------------------

class wxIntValidator( wxPyValidator ):
    """
    Validator class used with wxIntCtrl; handles all validation of input
    prior to changing the value of the underlying wxTextCtrl.
    """
    def __init__(self):
        wxPyValidator.__init__(self)
        EVT_CHAR(self, self.OnChar)

    def Clone (self):
        return self.__class__()

    def Validate(self, window):     # window here is the *parent* of the ctrl
        """
        Because each operation on the control is vetted as it's made,
        the value of the control is always valid.
        """
        return 1


    def OnChar(self, event):
        """
        Validates keystrokes to make sure the resulting value will a legal
        value.  Erasing the value causes it to be set to 0, with the value
        selected, so it can be replaced.  Similarly, replacing the value
        with a '-' sign causes the value to become -1, with the value
        selected.  Leading zeros are removed if introduced by selection,
        and are prevented from being inserted.
        """
        key = event.KeyCode()
        ctrl = event.GetEventObject()


        value = ctrl.GetValue()
        textval = wxTextCtrl.GetValue(ctrl)
        allow_none = ctrl.IsNoneAllowed()

        pos = ctrl.GetInsertionPoint()
        sel_start, sel_to = ctrl.GetSelection()
        select_len = sel_to - sel_start

# (Uncomment for debugging:)
##        print 'keycode:', key
##        print 'pos:', pos
##        print 'sel_start, sel_to:', sel_start, sel_to
##        print 'select_len:', select_len
##        print 'textval:', textval

        # set defaults for processing:
        allow_event = 1
        set_to_none = 0
        set_to_zero = 0
        set_to_minus_one = 0
        paste = 0
        internally_set = 0

        new_value = value
        new_text = textval
        new_pos = pos

        # Validate action, and predict resulting value, so we can
        # range check the result and validate that too.

        if key in (WXK_DELETE, WXK_BACK, WXK_CTRL_X):
            if select_len:
                new_text = textval[:sel_start] + textval[sel_to:]
            elif key == WXK_DELETE and pos < len(textval):
                new_text = textval[:pos] + textval[pos+1:]
            elif key == WXK_BACK and pos > 0:
                new_text = textval[:pos-1] + textval[pos:]
            # (else value shouldn't change)

            if new_text in ('', '-'):
                # Deletion of last significant digit:
                if allow_none and new_text == '':
                    new_value = None
                    set_to_none = 1
                else:
                    new_value = 0
                    set_to_zero = 1
            else:
                try:
                    new_value = ctrl._fromGUI(new_text)
                except ValueError:
                    allow_event = 0


        elif key == WXK_CTRL_V:   # (see comments at top of file)
            # Only allow paste if number:
            paste_text = ctrl._getClipboardContents()
            new_text = textval[:sel_start] + paste_text + textval[sel_to:]
            if new_text == '' and allow_none:
                new_value = None
                set_to_none = 1
            else:
                try:
                    # Convert the resulting strings, verifying they
                    # are legal integers and will fit in proper
                    # size if ctrl limited to int. (if not,
                    # disallow event.)
                    new_value = ctrl._fromGUI(new_text)
                    if paste_text:
                        paste_value = ctrl._fromGUI(paste_text)
                    else:
                        paste_value = 0
                    new_pos = sel_start + len(str(paste_value))

                    # if resulting value is 0, truncate and highlight value:
                    if new_value == 0 and len(new_text) > 1:
                        set_to_zero = 1

                    elif paste_value == 0:
                        # Disallow pasting a leading zero with nothing selected:
                        if( select_len == 0
                            and value is not None
                            and ( (value >= 0 and pos == 0)
                                  or (value < 0 and pos in [0,1]) ) ):
                            allow_event = 0
                    paste = 1

                except ValueError:
                    allow_event = 0


        elif key < WXK_SPACE or key > 255:
            pass    # event ok


        elif chr(key) == '-':
            # Allow '-' to result in -1 if replacing entire contents:
            if( value is None
                or (value == 0 and pos == 0)
                or (select_len >= len(str(abs(value)))) ):
                new_value = -1
                set_to_minus_one = 1

            # else allow negative sign only at start, and only if
            # number isn't already zero or negative:
            elif pos != 0 or (value is not None and value < 0):
                allow_event = 0
            else:
                new_text = '-' + textval
                new_pos = 1
                try:
                    new_value = ctrl._fromGUI(new_text)
                except ValueError:
                    allow_event = 0


        elif chr(key) in string.digits:
            # disallow inserting a leading zero with nothing selected
            if( chr(key) == '0'
                and select_len == 0
                and value is not None
                and ( (value >= 0 and pos == 0)
                      or (value < 0 and pos in [0,1]) ) ):
                allow_event = 0
            # disallow inserting digits before the minus sign:
            elif value is not None and value < 0 and pos == 0:
                allow_event = 0
            else:
                new_text = textval[:sel_start] + chr(key) + textval[sel_to:]
                try:
                    new_value = ctrl._fromGUI(new_text)
                except ValueError:
                    allow_event = 0

        else:
            # not a legal char
            allow_event = 0


        if allow_event:
            # Do range checking for new candidate value:
            if ctrl.IsLimited() and not ctrl.IsInBounds(new_value):
                allow_event = 0
            elif new_value is not None:
                # ensure resulting text doesn't result in a leading 0:
                if not set_to_zero and not set_to_minus_one:
                    if( (new_value > 0 and new_text[0] == '0')
                        or (new_value < 0 and new_text[1] == '0')
                        or (new_value == 0 and select_len > 1 ) ):

                        # Allow replacement of leading chars with
                        # zero, but remove the leading zero, effectively
                        # making this like "remove leading digits"

                        # Account for leading zero when positioning cursor:
                        if( key == WXK_BACK
                            or (paste and paste_value == 0 and new_pos > 0) ):
                            new_pos = new_pos - 1

                        wxCallAfter(ctrl.SetValue, new_value)
                        wxCallAfter(ctrl.SetInsertionPoint, new_pos)
                        internally_set = 1

                    elif paste:
                        # Always do paste numerically, to remove
                        # leading/trailing spaces
                        wxCallAfter(ctrl.SetValue, new_value)
                        wxCallAfter(ctrl.SetInsertionPoint, new_pos)
                        internally_set = 1

                    elif (new_value == 0 and len(new_text) > 1 ):
                        allow_event = 0

                if allow_event:
                    ctrl._colorValue(new_value)   # (one way or t'other)

# (Uncomment for debugging:)
##        if allow_event:
##            print 'new value:', new_value
##            if paste: print 'paste'
##            if set_to_none: print 'set_to_none'
##            if set_to_zero: print 'set_to_zero'
##            if set_to_minus_one: print 'set_to_minus_one'
##            if internally_set: print 'internally_set'
##        else:
##            print 'new text:', new_text
##            print 'disallowed'
##        print

        if allow_event:
            if set_to_none:
                wxCallAfter(ctrl.SetValue, new_value)

            elif set_to_zero:
                # select to "empty" numeric value
                wxCallAfter(ctrl.SetValue, new_value)
                wxCallAfter(ctrl.SetInsertionPoint, 0)
                wxCallAfter(ctrl.SetSelection, 0, 1)

            elif set_to_minus_one:
                wxCallAfter(ctrl.SetValue, new_value)
                wxCallAfter(ctrl.SetInsertionPoint, 1)
                wxCallAfter(ctrl.SetSelection, 1, 2)

            elif not internally_set:
                event.Skip()    # allow base wxTextCtrl to finish processing

        elif not wxValidator_IsSilent():
            wxBell()


    def TransferToWindow(self):
     """ Transfer data from validator to window.

         The default implementation returns False, indicating that an error
         occurred.  We simply return True, as we don't do any data transfer.
     """
     return True # Prevent wxDialog from complaining.


    def TransferFromWindow(self):
     """ Transfer data from window to validator.

         The default implementation returns False, indicating that an error
         occurred.  We simply return True, as we don't do any data transfer.
     """
     return True # Prevent wxDialog from complaining.


#----------------------------------------------------------------------------

class wxIntCtrl(wxTextCtrl):
    """
    This class provides a control that takes and returns integers as
    value, and provides bounds support and optional value limiting.

    wxIntCtrl(
         parent, id = -1,
         value = 0,
         min = None,
         max = None,
         limited = False,
         allow_none = False,
         allow_long = False,
         default_color = wxBLACK,
         oob_color = wxRED,
         pos = wxDefaultPosition,
         size = wxDefaultSize,
         style = 0,
         name = "integer")

    value
        If no initial value is set, the default will be zero, or
        the minimum value, if specified.  If an illegal string is specified,
        a ValueError will result. (You can always later set the initial
        value with SetValue() after instantiation of the control.)
    min
        The minimum value that the control should allow.  This can be
        adjusted with SetMin().  If the control is not limited, any value
        below this bound will be colored with the current out-of-bounds color.
        If min < -sys.maxint-1 and the control is configured to not allow long
        values, the minimum bound will still be set to the long value, but
        the implicit bound will be -sys.maxint-1.
    max
        The maximum value that the control should allow.  This can be
        adjusted with SetMax().  If the control is not limited, any value
        above this bound will be colored with the current out-of-bounds color.
        if max > sys.maxint and the control is configured to not allow long
        values, the maximum bound will still be set to the long value, but
        the implicit bound will be sys.maxint.

    limited
        Boolean indicating whether the control prevents values from
        exceeding the currently set minimum and maximum values (bounds).
        If False and bounds are set, out-of-bounds values will
        be colored with the current out-of-bounds color.

    allow_none
        Boolean indicating whether or not the control is allowed to be
        empty, representing a value of None for the control.

    allow_long
        Boolean indicating whether or not the control is allowed to hold
        and return a long as well as an int.

    default_color
        Color value used for in-bounds values of the control.

    oob_color
        Color value used for out-of-bounds values of the control
        when the bounds are set but the control is not limited.
    """
    def __init__ (
                self, parent, id=-1,
                value = 0, min=None, max=None,
                limited = 0, allow_none = 0, allow_long = 0,
                default_color = wxBLACK, oob_color = wxRED,
                pos = wxDefaultPosition, size = wxDefaultSize,
                style = 0, name = "integer",
        ):

        # Establish attrs required for any operation on value:
        self.__min = None
        self.__max = None
        self.__limited = 0
        self.__default_color = wxBLACK
        self.__oob_color = wxRED
        self.__allow_none = 0
        self.__allow_long = 0

        wxTextCtrl.__init__(
                self, parent, id, self._toGUI(0),
                pos, size, style, wxIntValidator(), name )

        # The following lets us set out our "integer update" events:
        EVT_TEXT( self, self.GetId(), self.OnText )

        # Establish parameters, with appropriate error checking

        self.SetBounds(min, max)
        self.SetLimited(limited)
        self.SetColors(default_color, oob_color)
        self.SetNoneAllowed(allow_none)
        self.SetLongAllowed(allow_long)
        self.SetValue(value)


    def OnText( self, event ):
        """
        Handles an event indicating that the text control's value
        has changed, and issue EVT_INT event.
        """
        try:
            self.GetEventHandler().ProcessEvent(
                wxIntUpdatedEvent( self.GetId(), self.GetValue(), self ) )
        except ValueError:
            return
        # let normal processing of the text continue
        event.Skip()


    def GetValue(self):
        """
        Returns the current integer (long) value of the control.
        """
        return self._fromGUI( wxTextCtrl.GetValue(self) )

    def SetValue(self, value):
        """
        Sets the value of the control to the integer value specified.
        The resulting actual value of the control may be altered to
        conform with the bounds set on the control if limited,
        or colored if not limited but the value is out-of-bounds.
        A ValueError exception will be raised if an invalid value
        is specified.
        """
        wxTextCtrl.SetValue( self, self._toGUI(value) )
        self._colorValue()


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
        control will be colored with the current out-of-bounds color.

        If min > -sys.maxint-1 and the control is configured to not allow longs,
        the function will return 0, and the min will not be set.
        """
        if( self.__max is None
            or min is None
            or (self.__max is not None and self.__max >= min) ):
            self.__min = min

            if self.IsLimited() and min is not None and self.GetValue() < min:
                self.SetValue(min)
            else:
                self._colorValue()
            return 1
        else:
            return 0


    def GetMin(self):
        """
        Gets the minimum value of the control.  It will return the current
        minimum integer, or None if not specified.
        """
        return self.__min


    def SetMax(self, max=None):
        """
        Sets the maximum value of the control. If a value of None
        is provided, then the control will have no explicit maximum value.
        If the value specified is less than the current minimum value, then
        the function returns 0 and the maximum will not change from its
        current setting. On success, the function returns 1.

        If successful and the current value is greater than the new upper
        bound, if the control is limited the value will be automatically
        adjusted to this maximum value; if not limited, the value in the
        control will be colored with the current out-of-bounds color.

        If max > sys.maxint and the control is configured to not allow longs,
        the function will return 0, and the max will not be set.
        """
        if( self.__min is None
            or max is None
            or (self.__min is not None and self.__min <= max) ):
            self.__max = max

            if self.IsLimited() and max is not None and self.GetValue() > max:
                self.SetValue(max)
            else:
                self._colorValue()
            return 1
        else:
            return 0


    def GetMax(self):
        """
        Gets the maximum value of the control.  It will return the current
        maximum integer, or None if not specified.
        """
        return self.__max


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


    def GetBounds(self):
        """
        This function returns a two-tuple (min,max), indicating the
        current bounds of the control.  Each value can be None if
        that bound is not set.
        """
        return (self.__min, self.__max)


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
        self.__limited = limited
        if limited:
            min = self.GetMin()
            max = self.GetMax()
            if not min is None and self.GetValue() < min:
                self.SetValue(min)
            elif not max is None and self.GetValue() > max:
                self.SetValue(max)
        else:
            self._colorValue()


    def IsLimited(self):
        """
        Returns True if the control is currently limiting the
        value to fall within the current bounds.
        """
        return self.__limited


    def IsInBounds(self, value=None):
        """
        Returns True if no value is specified and the current value
        of the control falls within the current bounds.  This function can
        also be called with a value to see if that value would fall within
        the current bounds of the given control.
        """
        if value is None:
            value = self.GetValue()

        if( not (value is None and self.IsNoneAllowed())
            and type(value) not in (types.IntType, types.LongType) ):
            raise ValueError (
                'wxIntCtrl requires integer values, passed %s'% repr(value) )

        min = self.GetMin()
        max = self.GetMax()
        if min is None: min = value
        if max is None: max = value

        # if bounds set, and value is None, return False
        if value == None and (min is not None or max is not None):
            return 0
        else:
            return min <= value <= max


    def SetNoneAllowed(self, allow_none):
        """
        Change the behavior of the validation code, allowing control
        to have a value of None or not, as appropriate.  If the value
        of the control is currently None, and allow_none is 0, the
        value of the control will be set to the minimum value of the
        control, or 0 if no lower bound is set.
        """
        self.__allow_none = allow_none
        if not allow_none and self.GetValue() is None:
            min = self.GetMin()
            if min is not None: self.SetValue(min)
            else:               self.SetValue(0)


    def IsNoneAllowed(self):
        return self.__allow_none


    def SetLongAllowed(self, allow_long):
        """
        Change the behavior of the validation code, allowing control
        to have a long value or not, as appropriate.  If the value
        of the control is currently long, and allow_long is 0, the
        value of the control will be adjusted to fall within the
        size of an integer type, at either the sys.maxint or -sys.maxint-1,
        for positive and negative values, respectively.
        """
        current_value = self.GetValue()
        if not allow_long and type(current_value) is types.LongType:
            if current_value > 0:
                self.SetValue(MAXINT)
            else:
                self.SetValue(MININT)
        self.__allow_long = allow_long


    def IsLongAllowed(self):
        return self.__allow_long



    def SetColors(self, default_color=wxBLACK, oob_color=wxRED):
        """
        Tells the control what colors to use for normal and out-of-bounds
        values.  If the value currently exceeds the bounds, it will be
        recolored accordingly.
        """
        self.__default_color = default_color
        self.__oob_color = oob_color
        self._colorValue()


    def GetColors(self):
        """
        Returns a tuple of (default_color, oob_color), indicating
        the current color settings for the control.
        """
        return self.__default_color, self.__oob_color


    def _colorValue(self, value=None):
        """
        Colors text with oob_color if current value exceeds bounds
        set for control.
        """
        if not self.IsInBounds(value):
            self.SetForegroundColour(self.__oob_color)
        else:
            self.SetForegroundColour(self.__default_color)
        self.Refresh()


    def _toGUI( self, value ):
        """
        Conversion function used to set the value of the control; does
        type and bounds checking and raises ValueError if argument is
        not a valid value.
        """
        if value is None and self.IsNoneAllowed():
            return ''
        elif type(value) == types.LongType and not self.IsLongAllowed():
            raise ValueError (
                'wxIntCtrl requires integer value, passed long' )
        elif type(value) not in (types.IntType, types.LongType):
            raise ValueError (
                'wxIntCtrl requires integer value, passed %s'% repr(value) )

        elif self.IsLimited():
            min = self.GetMin()
            max = self.GetMax()
            if not min is None and value < min:
                raise ValueError (
                    'value is below minimum value of control %d'% value )
            if not max is None and value > max:
                raise ValueError (
                    'value exceeds value of control %d'% value )

        return str(value)


    def _fromGUI( self, value ):
        """
        Conversion function used in getting the value of the control.
        """
        if value == '':
            if not self.IsNoneAllowed():
                return 0
            else:
                return None
        else:
            try:
                return int( value )
            except ValueError:
                if self.IsLongAllowed():
                    return long( value )
                else:
                    raise


    def Cut( self ):
        """
        Override the wxTextCtrl's .Cut function, with our own
        that does validation.  Will result in a value of 0
        if entire contents of control are removed.
        """
        sel_start, sel_to = self.GetSelection()
        select_len = sel_to - sel_start
        textval = wxTextCtrl.GetValue(self)

        do = wxTextDataObject()
        do.SetText(textval[sel_start:sel_to])
        wxTheClipboard.Open()
        wxTheClipboard.SetData(do)
        wxTheClipboard.Close()
        if select_len == len(wxTextCtrl.GetValue(self)):
            if not self.IsNoneAllowed():
                self.SetValue(0)
                self.SetInsertionPoint(0)
                self.SetSelection(0,1)
            else:
                self.SetValue(None)
        else:
            new_value = self._fromGUI(textval[:sel_start] + textval[sel_to:])
            self.SetValue(new_value)


    def _getClipboardContents( self ):
        """
        Subroutine for getting the current contents of the clipboard.
        """
        do = wxTextDataObject()
        wxTheClipboard.Open()
        success = wxTheClipboard.GetData(do)
        wxTheClipboard.Close()

        if not success:
            return None
        else:
            # Remove leading and trailing spaces before evaluating contents
            return do.GetText().strip()


    def Paste( self ):
        """
        Override the wxTextCtrl's .Paste function, with our own
        that does validation.  Will raise ValueError if not a
        valid integerizable value.
        """
        paste_text = self._getClipboardContents()
        if paste_text:
            # (conversion will raise ValueError if paste isn't legal)
            sel_start, sel_to = self.GetSelection()
            text = wxTextCtrl.GetValue( self )
            new_text = text[:sel_start] + paste_text + text[sel_to:]
            if new_text == '' and self.IsNoneAllowed():
                self.SetValue(None)
            else:
                value = self._fromGUI(new_text)
                self.SetValue(value)
                new_pos = sel_start + len(paste_text)
                wxCallAfter(self.SetInsertionPoint, new_pos)



#===========================================================================

if __name__ == '__main__':

    import traceback

    class myDialog(wxDialog):
        def __init__(self, parent, id, title,
            pos = wxPyDefaultPosition, size = wxPyDefaultSize,
            style = wxDEFAULT_DIALOG_STYLE ):
            wxDialog.__init__(self, parent, id, title, pos, size, style)

            self.int_ctrl = wxIntCtrl(self, NewId(), size=(55,20))
            self.OK = wxButton( self, wxID_OK, "OK")
            self.Cancel = wxButton( self, wxID_CANCEL, "Cancel")

            vs = wxBoxSizer( wxVERTICAL )
            vs.AddWindow( self.int_ctrl, 0, wxALIGN_CENTRE|wxALL, 5 )
            hs = wxBoxSizer( wxHORIZONTAL )
            hs.AddWindow( self.OK, 0, wxALIGN_CENTRE|wxALL, 5 )
            hs.AddWindow( self.Cancel, 0, wxALIGN_CENTRE|wxALL, 5 )
            vs.AddSizer(hs, 0, wxALIGN_CENTRE|wxALL, 5 )

            self.SetAutoLayout( True )
            self.SetSizer( vs )
            vs.Fit( self )
            vs.SetSizeHints( self )
            EVT_INT(self, self.int_ctrl.GetId(), self.OnInt)

        def OnInt(self, event):
            print 'int now', event.GetValue()

    class TestApp(wxApp):
        def OnInit(self):
            try:
                self.frame = wxFrame(NULL, -1, "Test",
                                     wxPoint(20,20), wxSize(120,100)  )
                self.panel = wxPanel(self.frame, -1)
                button = wxButton(self.panel, 10, "Push Me",
                                  wxPoint(20, 20))
                EVT_BUTTON(self, 10, self.OnClick)
            except:
                traceback.print_exc()
                return False
            return True

        def OnClick(self, event):
            dlg = myDialog(self.panel, -1, "test wxIntCtrl")
            dlg.int_ctrl.SetValue(501)
            dlg.int_ctrl.SetInsertionPoint(1)
            dlg.int_ctrl.SetSelection(1,2)
            rc = dlg.ShowModal()
            print 'final value', dlg.int_ctrl.GetValue()
            del dlg
            self.frame.Destroy()

        def Show(self):
            self.frame.Show(True)

    try:
        app = TestApp(0)
        app.Show()
        app.MainLoop()
    except:
        traceback.print_exc()
