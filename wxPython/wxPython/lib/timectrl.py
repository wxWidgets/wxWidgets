#----------------------------------------------------------------------------
# Name:         wxTimeCtrl.py
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
#   wxTimeCtrl does not use validators, because it does careful manipulation
#   of the cursor in the text window on each keystroke, and validation is
#   cursor-position specific, so the control intercepts the key codes before the
#   validator would fire.
#

from wxPython.wx import *
import string

# wxWindows' wxTextCtrl translates Composite "control key"
# events into single events before returning them to its OnChar
# routine.  The doc says that this results in 1 for Ctrl-A, 2 for
# Ctrl-B, etc. However, there are no wxPython or wxWindows
# symbols for them, so I'm defining codes for Ctrl-X (cut) and
# Ctrl-V (paste) here for readability:
WXK_CTRL_X = (ord('X')+1) - ord('A')
WXK_CTRL_V = (ord('V')+1) - ord('A')

# The following bit of function is for debugging the subsequent code.
# To turn on debugging output, set _debug to 1
_debug = 0
_indent = 0

if _debug:
    def _dbg(*args, **kwargs):
        global _indent

        if len(args):
            if _indent:      print ' ' * 3 * _indent,
            for arg in args: print arg,
            print
        # else do nothing

        # post process args:
        for kwarg, value in kwargs.items():
            if kwarg == 'indent' and value:         _indent = _indent + 1
            elif kwarg == 'indent' and value == 0:  _indent = _indent - 1
            if _indent < 0: _indent = 0
else:
    def _dbg(*args, **kwargs):
        pass


# This class of event fires whenever the value of the time changes in the control:
wxEVT_TIMEVAL_UPDATED = wxNewId()
def EVT_TIMEUPDATE(win, id, func):
    """Used to trap events indicating that the current time has been changed."""
    win.Connect(id, -1, wxEVT_TIMEVAL_UPDATED, func)

class TimeUpdatedEvent(wxPyCommandEvent):
    def __init__(self, id, value ='12:00:00 AM'):
        wxPyCommandEvent.__init__(self, wxEVT_TIMEVAL_UPDATED, id)
        self.value = value
    def GetValue(self):
        """Retrieve the value of the time control at the time this event was generated"""
        return self.value


# Set up all the positions of the cells in the wxTimeCtrl (once at module import):
# Format of control is:
#               hh:mm:ss xM
#                         1
# positions:    01234567890
_listCells = ['hour', 'minute', 'second', 'am_pm']
_listCellRange =   [(0,1,2), (3,4,5), (6,7,8), (9,10,11)]
_listDelimPos =  [2,5,8]

# Create dictionary of cell ranges, indexed by name or position in the range:
_dictCellRange = {}
for i in range(4):
    _dictCellRange[_listCells[i]] = _listCellRange[i]
for cell in _listCells:
    for i in _dictCellRange[cell]:
        _dictCellRange[i] = _dictCellRange[cell]


# Create lists of starting and ending positions for each range, and a dictionary of starting
# positions indexed by name
_listStartCellPos = []
_listEndCellPos = []
for tup in _listCellRange:
    _listStartCellPos.append(tup[0])  # 1st char of cell
    _listEndCellPos.append(tup[1])    # last char of cell (not including delimiter)

_dictStartCellPos = {}
for i in range(4):
    _dictStartCellPos[_listCells[i]] = _listStartCellPos[i]


class wxTimeCtrl(wxTextCtrl):
    def __init__ (
                self, parent, id=-1, value = '12:00:00 AM',
                pos = wxDefaultPosition, size = wxDefaultSize,
                fmt24hr=0,
                spinButton = None,
                style = wxTE_PROCESS_TAB, name = "time"
        ):
        wxTextCtrl.__init__(self, parent, id, value='',
                            pos=pos, size=size, style=style, name=name)

        self.__fmt24hr = fmt24hr

        if size == wxDefaultSize:
            # set appropriate default sizes depending on format:
            if self.__fmt24hr:
                testText = '00:00:00'
            else:
                testText = '00:00:00 MM'
            _dbg(wxPlatform)

            if wxPlatform != "__WXMSW__":   # give it a little extra space
                testText += 'M'
            if wxPlatform == "__WXMAC__":   # give it even a little more...
                testText += 'M'

            w, h = self.GetTextExtent(testText)
            self.SetClientSize( (w+4, self.GetClientSize().height) )


        if self.__fmt24hr:  self.__lastCell = 'second'
        else:               self.__lastCell = 'am_pm'

        # Validate initial value and set if appropriate
        try:
            self.SetValue(value)
        except:
            self.SetValue('12:00:00 AM')

        # set initial position and selection state
        self.__SetCurrentCell(_dictStartCellPos['hour'])
        self.__OnChangePos(None)

        # Set up internal event handlers to change the event reaction behavior of
        # the base wxTextCtrl:
        EVT_TEXT(self, self.GetId(), self.__OnTextChange)
        EVT_SET_FOCUS(self, self.__OnFocus)
        EVT_LEFT_UP(self, self.__OnChangePos)
        EVT_LEFT_DCLICK(self, self.__OnDoubleClick)
        EVT_CHAR(self, self.__OnChar)

        if spinButton:
            self.BindSpinButton(spinButton)     # bind spin button up/down events to this control


    def BindSpinButton(self, sb):
        """
        This function binds an externally created spin button to the control, so that
        up/down events from the button automatically change the control.
        """
        _dbg('wxTimeCtrl::BindSpinButton')
        self.__spinButton = sb
        if self.__spinButton:
            # bind event handlers to spin ctrl
            EVT_SPIN_UP(self.__spinButton, self.__spinButton.GetId(), self.__OnSpinUp)
            EVT_SPIN_DOWN(self.__spinButton, self.__spinButton.GetId(), self.__OnSpinDown)



    def __repr__(self):
        return "<wxTimeCtrl: %s>" % self.GetValue()



    def SetValue(self, value):
        """
        Validating SetValue function for time strings, doing 12/24 format conversion as appropriate.
        """
        _dbg('wxTimeCtrl::SetValue', indent=1)
        dict_range = _dictCellRange             # (for brevity)
        dict_start = _dictStartCellPos

        fmt12len = dict_range['am_pm'][-1]
        fmt24len = dict_range['second'][-1]
        try:
            separators_correct = value[2] == ':' and value[5] == ':'
            len_ok = len(value) in (fmt12len, fmt24len)

            if len(value) > fmt24len:
                separators_correct = separators_correct and value[8] == ' '
            hour = int(value[dict_range['hour'][0]:dict_range['hour'][-1]])
            hour_ok = ((hour in range(0,24) and len(value) == fmt24len)
                       or (hour in range(1,13) and len(value) == fmt12len
                           and value[dict_start['am_pm']:] in ('AM', 'PM')))

            minute = int(value[dict_range['minute'][0]:dict_range['minute'][-1]])
            min_ok  = minute in range(60)
            second  = int(value[dict_range['second'][0]:dict_range['second'][-1]])
            sec_ok  = second in range(60)

            _dbg('len_ok =', len_ok, 'separators_correct =', separators_correct)
            _dbg('hour =', hour, 'hour_ok =', hour_ok, 'min_ok =', min_ok, 'sec_ok =', sec_ok)

            if len_ok and hour_ok and min_ok and sec_ok and separators_correct:
                _dbg('valid time string')

                self.__hour = hour
                if len(value) == fmt12len:                      # handle 12 hour format conversion for actual hour:
                    am = value[dict_start['am_pm']:] == 'AM'
                    if hour != 12 and not am:
                        self.__hour = hour = (hour+12) % 24
                    elif hour == 12:
                        if am: self.__hour = hour = 0

                self.__minute = minute
                self.__second = second

                # valid time
                need_to_convert = ((self.__fmt24hr and len(value) == fmt12len)
                                   or (not self.__fmt24hr and len(value) == fmt24len))
                _dbg('need_to_convert =', need_to_convert)

                if need_to_convert:     #convert to 12/24 hour format as specified:
                    if self.__fmt24hr and len(value) == fmt12len:
                        text = '%.2d:%.2d:%.2d' % (hour, minute, second)
                    else:
                        if hour > 12:
                            hour = hour - 12
                            am_pm = 'PM'
                        elif hour == 12:
                            am_pm = 'PM'
                        else:
                            if hour == 0: hour = 12
                            am_pm = 'AM'
                        text = '%2d:%.2d:%.2d %s' % (hour, minute, second, am_pm)
                else:
                    text = value
                _dbg('text=', text)
                wxTextCtrl.SetValue(self, text)
                _dbg('firing TimeUpdatedEvent...')
                evt = TimeUpdatedEvent(self.GetId(), text)
                evt.SetEventObject(self)
                self.GetEventHandler().ProcessEvent(evt)
            else:
                _dbg('len_ok:', len_ok, 'separators_correct =', separators_correct)
                _dbg('hour_ok:', hour_ok, 'min_ok:', min_ok, 'sec_ok:', sec_ok, indent=0)
                raise ValueError, 'value is not a valid time string'

        except (TypeError, ValueError):
            _dbg(indent=0)
            raise ValueError, 'value is not a valid time string'
        _dbg(indent=0)

    def SetWxDateTime(self, wxdt):
        value = '%2d:%.2d:%.2d' % (wxdt.GetHour(), wxdt.GetMinute(), wxdt.GetSecond())
        self.SetValue(value)

    def GetWxDateTime(self):
        t = wxDateTimeFromHMS(self.__hour, self.__minute, self.__second)
        return t

    def SetMxDateTime(self, mxdt):
        from mx import DateTime
        value = '%2d:%.2d:%.2d' % (mxdt.hour, mxdt.minute, mxdt.second)
        self.SetValue(value)

    def GetMxDateTime(self):
        from mx import DateTime
        t = DateTime.Time(self.__hour, self.__minute, self.__second)
        return t

#-------------------------------------------------------------------------------------------------------------
# these are private functions and overrides:

    def __SetCurrentCell(self, pos):
        """
        Sets state variables that indicate the current cell and position within the control.
        """
        self.__posCurrent = pos
        self.__cellStart, self.__cellEnd = _dictCellRange[pos][0], _dictCellRange[pos][-1]


    def SetInsertionPoint(self, pos):
        """
        Records the specified position and associated cell before calling base class' function.
        """
        _dbg('wxTimeCtrl::SetInsertionPoint', pos, indent=1)

        # Adjust pos to legal value if not already
        if pos < 0: pos = 0
        elif pos in _listDelimPos + [_dictCellRange[self.__lastCell]]:
            pos = pos - 1
            if self.__lastCell == 'am_pm' and pos in _dictCellRange[self.__lastCell]:
                pos = _dictStartCellPos[self.__lastCell]

        self.__SetCurrentCell(pos)
        wxTextCtrl.SetInsertionPoint(self, pos)                 # (causes EVT_TEXT event to fire)
        _dbg(indent=0)


    def SetSelection(self, sel_start, sel_to):
        _dbg('wxTimeCtrl::SetSelection', sel_start, sel_to, indent=1)

        # Adjust selection range to legal extent if not already
        if sel_start < 0:
            self.SetInsertionPoint(0)
            sel_start = self.__posCurrent

        elif sel_start in _listDelimPos + [_dictCellRange[self.__lastCell]]:
            self.SetInsertionPoint(sel_start - 1)
            sel_start = self.__posCurrent

        if self.__posCurrent != sel_start:                      # force selection and insertion point to match
            self.SetInsertionPoint(sel_start)

        if sel_to not in _dictCellRange[sel_start]:
            sel_to = _dictCellRange[sel_start][-1]              # limit selection to end of current cell

        self.__bSelection = sel_start != sel_to
        self.__posSelectTo = sel_to
        wxTextCtrl.SetSelection(self, sel_start, sel_to)
        _dbg(indent=0)


    def __OnFocus(self,event):
        """
        This event handler is currently necessary to work around new default
        behavior as of wxPython2.3.3;
        The TAB key auto selects the entire contents of the wxTextCtrl *after*
        the EVT_SET_FOCUS event occurs; therefore we can't query/adjust the selection
        *here*, because it hasn't happened yet.  So to prevent this behavior, and
        preserve the correct selection when the focus event is not due to tab,
        we need to pull the following trick:
        """
        _dbg('wxTimeCtrl::OnFocus')
        wxCallAfter(self.__FixSelection)
        event.Skip()


    def __FixSelection(self):
        """
        This gets called after the TAB traversal selection is made, if the
        focus event was due to this, but before the EVT_LEFT_* events if
        the focus shift was due to a mouse event.

        The trouble is that, a priori, there's no explicit notification of
        why the focus event we received.  However, the whole reason we need to
        do this is because the default behavior on TAB traveral in a wxTextCtrl is
        now to select the entire contents of the window, something we don't want.
        So we can *now* test the selection range, and if it's "the whole text"
        we can assume the cause, change the insertion point to the start of
        the control, and deselect.
        """
        _dbg('wxTimeCtrl::FixSelection', indent=1)
        sel_start, sel_to = self.GetSelection()
        if sel_start == 0 and sel_to in _dictCellRange[self.__lastCell]:
            # This isn't normally allowed, and so assume we got here by the new
            # "tab traversal" behavior, so we need to reset the selection
            # and insertion point:
            _dbg('entire text selected; resetting selection to start of control')
            self.SetInsertionPoint(0)
            self.SetSelection(self.__cellStart, self.__cellEnd)
        _dbg(indent=0)


    def __OnTextChange(self, event):
        """
        This private event handler is required to retain the current position information of the cursor
        after update to the underlying text control is done.
        """
        _dbg('wxTimeCtrl::OnTextChange', indent=1)
        self.__SetCurrentCell(self.__posCurrent)                # ensure cell range vars are set

        # Note: must call self.SetSelection here to preserve insertion point cursor after update!
        # (I don't know why, but this does the trick!)
        if self.__bSelection:
            _dbg('reselecting from ', self.__posCurrent, 'to', self.__posSelectTo)
            self.SetSelection(self.__posCurrent, self.__posSelectTo)
        else:
            self.SetSelection(self.__posCurrent, self.__posCurrent)
        event.Skip()
        _dbg(indent=0)

    def __OnSpin(self, key):
        self.__IncrementValue(key, self.__posCurrent)
        self.SetFocus()
        self.SetInsertionPoint(self.__posCurrent)
        self.SetSelection(self.__posCurrent, self.__posSelectTo)

    def __OnSpinUp(self, event):
        """
        Event handler for any bound spin button on EVT_SPIN_UP;
        causes control to behave as if up arrow was pressed.
        """
        _dbg('wxTimeCtrl::OnSpinUp', indent=1)
        self.__OnSpin(WXK_UP)
        _dbg(indent=0)


    def __OnSpinDown(self, event):
        """
        Event handler for any bound spin button on EVT_SPIN_DOWN;
        causes control to behave as if down arrow was pressed.
        """
        _dbg('wxTimeCtrl::OnSpinDown', indent=1)
        self.__OnSpin(WXK_DOWN)
        _dbg(indent=0)


    def __OnChangePos(self, event):
        """
        Event handler for motion events; this handler
        changes limits the selection to the new cell boundaries.
        """
        _dbg('wxTimeCtrl::OnChangePos', indent=1)
        pos = self.GetInsertionPoint()
        self.__SetCurrentCell(pos)
        sel_start, sel_to = self.GetSelection()
        selection = sel_start != sel_to
        if not selection:
            # disallow position at end of field:
            if pos in _listDelimPos + [_dictCellRange[self.__lastCell][-1]]:
                self.SetInsertionPoint(pos-1)
            self.__posSelectTo = self.__cellEnd
        else:
            # only allow selection to end of current cell:
            if sel_to < pos:   self.__posSelectTo = self.__cellStart
            elif sel_to > pos: self.__posSelectTo = self.__cellEnd

        _dbg('new pos =', self.__posCurrent, 'select to ', self.__posSelectTo)
        self.SetSelection(self.__posCurrent, self.__posSelectTo)
        if event: event.Skip()
        _dbg(indent=0)


    def __OnDoubleClick(self, event):
        """
        Event handler for left double-click mouse events; this handler
        causes the cell at the double-click point to be selected.
        """
        _dbg('wxTimeCtrl::OnDoubleClick', indent=1)
        pos = self.GetInsertionPoint()
        self.__SetCurrentCell(pos)
        if self.__posCurrent != self.__cellStart:
            self.SetInsertionPoint(self.__cellStart)
        self.SetSelection(self.__cellStart, self.__cellEnd)
        _dbg(indent=0)


    def __OnChar(self, event):
        """
        This private event handler is the main control point for the wxTimeCtrl.
        It governs whether input characters are accepted and if so, handles them
        so as to provide appropriate cursor and selection behavior for the control.
        """
        _dbg('wxTimeCtrl::OnChar', indent=1)

        # NOTE: Returning without calling event.Skip() eats the event before it
        # gets to the text control...

        key = event.GetKeyCode()
        text = self.GetValue()
        pos = self.GetInsertionPoint()
        if pos != self.__posCurrent:
            _dbg("insertion point has moved; resetting current cell")
            self.__SetCurrentCell(pos)
            self.SetSelection(self.__posCurrent, self.__posCurrent)

        sel_start, sel_to = self.GetSelection()
        selection = sel_start != sel_to
        _dbg('sel_start=', sel_start, 'sel_to =', sel_to)
        if not selection:
            self.__bSelection = False                       # predict unselection of entire region

        _dbg('keycode = ', key)
        _dbg('pos = ', pos)

        # don't allow deletion, cut or paste:
        if key in (WXK_DELETE, WXK_BACK, WXK_CTRL_X, WXK_CTRL_V):
            pass

        elif key == WXK_TAB:                                # skip to next field if applicable:
            _dbg('key == WXK_TAB')
            dict_range = _dictCellRange                     # (for brevity)
            dict_start = _dictStartCellPos
            if event.ShiftDown():                           # tabbing backwords

                ###(NOTE: doesn't work; wxTE_PROCESS_TAB doesn't appear to send us this event!)

                _dbg('event.ShiftDown()')
                if pos in dict_range['hour']:               # already in 1st field
                    self.__SetCurrentCell(dict_start['hour']) # ensure we have our member vars set
                    event.Skip()                            #then do normal tab processing for the form
                    _dbg(indent=0)
                    return

                elif pos in dict_range['minute']:           # skip to hours field
                    new_pos = dict_start['hour']
                elif pos in dict_range['second']:           # skip to minutes field
                    new_pos = dict_start['minute']
                elif pos in dict_range['am_pm']:            # skip to seconds field
                    new_pos = dict_start['second']

                self.SetInsertionPoint(new_pos)             # force insert point to jump to next cell (swallowing TAB)
                self.__OnChangePos(None)                    # update selection accordingly

            else:
                # Tabbing forwards through control...

                if pos in dict_range[self.__lastCell]:      # already in last field; ensure we have our members set
                    self.__SetCurrentCell(dict_start[self.__lastCell])
                    _dbg('tab in last cell')
                    event.Skip()                            # then do normal tab processing for the form
                    _dbg(indent=0)
                    return

                if pos in dict_range['second']:             # skip to AM/PM field (if not last cell)
                    new_pos = dict_start['am_pm']
                elif pos in dict_range['minute']:           # skip to seconds field
                    new_pos = dict_start['second']
                elif pos in dict_range['hour']:             # skip to minutes field
                    new_pos = dict_start['minute']

                self.SetInsertionPoint(new_pos)             # force insert point to jump to next cell (swallowing TAB)
                self.__OnChangePos(None)                    # update selection accordingly

        elif key == WXK_LEFT:                               # move left; set insertion point as appropriate:
            _dbg('key == WXK_LEFT')
            if event.ShiftDown():                           # selecting a range...
                _dbg('event.ShiftDown()')
                if pos in _listStartCellPos:                # can't select pass delimiters
                    if( sel_to == pos+2 and sel_to != _dictCellRange['am_pm'][-1]):
                        self.SetSelection(pos, pos+1)       # allow deselection of 2nd char in cell if not am/pm
                    # else ignore event

                elif pos in _listEndCellPos:                # can't use normal selection, because position ends up
                                                            # at delimeter
                    _dbg('set selection from', pos-1, 'to', self.__posCurrent)
                    self.SetInsertionPoint(pos-1)           # this selects the previous position
                    self.SetSelection(self.__posCurrent, pos)
                else:
                    self.SetInsertionPoint(sel_to - 1)      # this unselects the last digit
                    self.SetSelection(self.__posCurrent, pos)

            else:   # ... not selecting
                if pos == 0:                                # can't position before position 0
                    pass
                elif pos in _listStartCellPos:              # skip (left) OVER the colon/space:
                    self.SetInsertionPoint(pos-2)
                    self.__OnChangePos(None)                # set the selection appropriately
                else:
                    self.SetInsertionPoint(pos-1)           # reposition the cursor and
                    self.__OnChangePos(None)                # set the selection appropriately


        elif key == WXK_RIGHT:                              # move right
            _dbg('key == WXK_RIGHT')
            if event.ShiftDown():
                _dbg('event.ShiftDown()')
                if sel_to in _listDelimPos:                 # can't select pass delimiters
                    pass
                else:
                    self.SetSelection(self.__posCurrent, sel_to+1)
            else:
                if( (self.__lastCell == 'second'
                     and pos == _dictStartCellPos['second']+1)
                    or (self.__lastCell == 'am_pm'
                        and pos == _dictStartCellPos['am_pm']) ):
                    pass                                    # don't allow cursor past last cell
                elif pos in _listEndCellPos:                # skip (right) OVER the colon/space:
                    self.SetInsertionPoint(pos+2)
                    self.__OnChangePos(None)                # set the selection appropriately
                else:
                    self.SetInsertionPoint(pos+1)           # reposition the cursor and
                    self.__OnChangePos(None)                # set the selection appropriately

        elif key in (WXK_UP, WXK_DOWN):
            _dbg('key in (WXK_UP, WXK_DOWN)')
            self.__IncrementValue(key, pos)                   # increment/decrement as appropriate


        elif key < WXK_SPACE or key == WXK_DELETE or key > 255:
            event.Skip()                                    # non alphanumeric; process normally (Right thing to do?)

        elif chr(key) in ['!', 'c', 'C']:                   # Special character; sets the value of the control to "now"
            _dbg("key == '!'; setting time to 'now'")
            now = wxDateTime_Now()
            self.SetWxDateTime(now)

        elif chr(key) in string.digits:                     # let ChangeValue validate and update current position
            self.__ChangeValue(chr(key), pos)               # handle event (and swallow it)

        elif chr(key) in ('a', 'A', 'p', 'P', ' '):         # let ChangeValue validate and update current position
            self.__ChangeValue(chr(key), pos)               # handle event (and swallow it)

        else:                                               # disallowed char; swallow event
            pass
        _dbg(indent=0)


    def __IncrementValue(self, key, pos):
        _dbg('wxTimeCtrl::IncrementValue', key, pos, indent=1)
        text = self.GetValue()

        sel_start, sel_to = self.GetSelection()
        selection = sel_start != sel_to
        cell_selected = selection and sel_to -1 != pos

        dict_start = _dictStartCellPos                      # (for brevity)

        # Determine whether we should change the entire cell or just a portion of it:
        if( cell_selected
            or (pos in _listStartCellPos and not selection)
            or (text[pos] == ' ' and text[pos+1] not in ('1', '2'))
            or (text[pos] == '9' and text[pos-1] == ' ' and key == WXK_UP)
            or (text[pos] == '1' and text[pos-1] == ' ' and key == WXK_DOWN)
            or pos >= dict_start['am_pm']):

            self.__IncrementCell(key, pos)
        else:
            if key == WXK_UP:   inc = 1
            else:               inc = -1

            if pos == dict_start['hour'] and not self.__fmt24hr:
                if text[pos] == ' ': digit = '1'                    # allow ' ' or 1 for 1st digit in 12hr format
                else:                digit = ' '
            else:
                if pos == dict_start['hour']:
                    if int(text[pos + 1]) >3:   mod = 2             # allow for 20-23
                    else:                       mod = 3             # allow 00-19
                elif pos == dict_start['hour'] + 1:
                    if self.__fmt24hr:
                        if text[pos - 1] == '2': mod = 4            # allow hours 20-23
                        else:                    mod = 10           # allow hours 00-19
                    else:
                        if text[pos - 1] == '1': mod = 3            # allow hours 10-12
                        else:                    mod = 10           # allow 0-9

                elif pos in (dict_start['minute'],
                             dict_start['second']): mod = 6         # allow minutes/seconds 00-59
                else:                               mod = 10

                digit = '%d' % ((int(text[pos]) + inc) % mod)

            _dbg("new digit = \'%s\'" % digit)
            self.__ChangeValue(digit, pos)
        _dbg(indent=0)


    def __IncrementCell(self, key, pos):
        _dbg('wxTimeCtrl::IncrementCell', key, pos, indent=1)
        self.__SetCurrentCell(pos)                                  # determine current cell
        hour, minute, second = self.__hour, self.__minute, self.__second
        text = self.GetValue()
        dict_start = _dictStartCellPos                              # (for brevity)
        if key == WXK_UP:   inc = 1
        else:               inc = -1

        if self.__cellStart == dict_start['am_pm']:
            am = text[dict_start['am_pm']:] == 'AM'
            if am: hour = hour + 12
            else:  hour = hour - 12
        else:
            if self.__cellStart == dict_start['hour']:
                hour = (hour + inc) % 24
            elif self.__cellStart == dict_start['minute']:
                minute = (minute + inc) % 60
            elif self.__cellStart == dict_start['second']:
                second = (second + inc) % 60

        newvalue = '%.2d:%.2d:%.2d' % (hour, minute, second)

        self.SetValue(newvalue)
        self.SetInsertionPoint(self.__cellStart)
        self.SetSelection(self.__cellStart, self.__cellEnd)
        _dbg(indent=0)


    def __ChangeValue(self, char, pos):
        _dbg('wxTimeCtrl::ChangeValue', "\'" + char + "\'", pos, indent=1)
        text = self.GetValue()

        self.__SetCurrentCell(pos)
        sel_start, sel_to = self.GetSelection()
        self.__posSelectTo = sel_to
        self.__bSelection = selection = sel_start != sel_to
        cell_selected = selection and sel_to -1 != pos
        _dbg('cell_selected =', cell_selected, indent=0)

        dict_start = _dictStartCellPos                          # (for brevity)

        if pos in _listDelimPos: return                         # don't allow change of punctuation

        elif( 0 < pos < dict_start['am_pm'] and char not in string.digits):
            return                                              # AM/PM not allowed in this position

        # See if we're changing the hour cell, and validate/update appropriately:
        #
        hour_start = dict_start['hour']                         # (ie. 0)

        if pos == hour_start:                                   # if at 1st position,
            if self.__fmt24hr:                                  # and using 24 hour format
                if cell_selected:                               # replace cell contents with hour represented by digit
                    newtext = '%.2d' % int(char) + text[hour_start+2:]
                elif char not in ('0', '1', '2'):               # return if digit not 0,1, or 2
                    return
                else:                                           # relace current position
                    newtext = char + text[pos+1:]
            else:                                               # (12 hour format)
                if cell_selected:
                    if char == ' ': return                      # can't erase entire cell
                    elif char == '0':                           # treat 0 as '12'
                        newtext = '12' + text[hour_start+2:]
                    else:                                       # replace cell contents with hour represented by digit
                        newtext = '%2d' % int(char) + text[hour_start+2:]
                else:
                    if char not in ('1', ' '):                  # can only type a 1 or space
                        return
                    if text[pos+1] not in ('0', '1', '2'):      # and then, only if other column is 0,1, or 2
                        return
                    if char == ' ' and text[pos+1] == '0':      # and char isn't space if 2nd column is 0
                        return
                    else:                                       # ok; replace current position
                        newtext = char + text[pos+1:]
                if char == ' ': self.SetInsertionPoint(pos+1)   # move insert point to legal position

        elif pos == hour_start+1:                               # if editing 2nd position of hour
            if( not self.__fmt24hr                              # and using 12 hour format
                and text[hour_start] == '1'                     # if 1st char is 1,
                and char not in ('0', '1', '2')):               # disallow anything bug 0,1, or 2
                return
            newtext = text[hour_start] + char + text[hour_start+2:]  # else any digit ok

        # Do the same sort of validation for minute and second cells
        elif pos in (dict_start['minute'], dict_start['second']):
            if cell_selected:                                   # if cell selected, replace value
                newtext = text[:pos] + '%.2d' % int(char) + text[pos+2:]
            elif int(char) > 5: return                          # else disallow > 59 for minute and second fields
            else:
                newtext = text[:pos] + char + text[pos+1:]      # else ok

        elif pos in (dict_start['minute']+1, dict_start['second']+1):
            newtext = text[:pos] + char + text[pos+1:]          # all digits ok for 2nd digit of minute/second

        # Process AM/PM cell
        elif pos == dict_start['am_pm']:
            char = char.upper()
            if char not in ('A','P'): return                    # disallow all but A or P as 1st char of column
            newtext = text[:pos] + char + text[pos+1:]
        else: return    # not a valid position

        _dbg(indent=1)
        # update member position vars and set selection to character changed
        if not cell_selected:
            _dbg('reselecting current digit')
            self.__posSelectTo = pos+1

        _dbg('newtext=', newtext)
        self.SetValue(newtext)
        self.SetInsertionPoint(self.__posCurrent)
        self.SetSelection(self.__posCurrent, self.__posSelectTo)
        _dbg(indent=0)


    def Cut(self):
        """
        Override wxTextCtrl::Cut() method, as this operation should not
        be allowed for wxTimeCtrls.
        """
        return


    def Paste(self):
        """
        Override wxTextCtrl::Paste() method, as this operation should not
        be allowed for wxTimeCtrls.
        """
        return


#----------------------------------------------------------------------------
# Test jig for wxTimeCtrl:

if __name__ == '__main__':
    import traceback

    class TestPanel(wxPanel):
        def __init__(self, parent, id,
                     pos = wxPyDefaultPosition, size = wxPyDefaultSize,
                     fmt24hr = 0, test_mx = 0,
                     style = wxTAB_TRAVERSAL ):

            wxPanel.__init__(self, parent, id, pos, size, style)

            self.test_mx = test_mx

            self.tc = wxTimeCtrl(self, 10, fmt24hr = fmt24hr)
            sb = wxSpinButton( self, 20, wxDefaultPosition, wxSize(-1,20), 0 )
            self.tc.BindSpinButton(sb)

            sizer = wxBoxSizer( wxHORIZONTAL )
            sizer.AddWindow( self.tc, 0, wxALIGN_CENTRE|wxLEFT|wxTOP|wxBOTTOM, 5 )
            sizer.AddWindow( sb, 0, wxALIGN_CENTRE|wxRIGHT|wxTOP|wxBOTTOM, 5 )

            self.SetAutoLayout( True )
            self.SetSizer( sizer )
            sizer.Fit( self )
            sizer.SetSizeHints( self )

            EVT_TIMEUPDATE(self, self.tc.GetId(), self.OnTimeChange)

        def OnTimeChange(self, event):
            _dbg('OnTimeChange: value = ', event.GetValue())
            wxdt = self.tc.GetWxDateTime()
            _dbg('wxdt =', wxdt.GetHour(), wxdt.GetMinute(), wxdt.GetSecond())
            if self.test_mx:
                mxdt = self.tc.GetMxDateTime()
                _dbg('mxdt =', mxdt.hour, mxdt.minute, mxdt.second)


    class MyApp(wxApp):
        def OnInit(self):
            import sys
            fmt24hr = '24' in sys.argv
            test_mx = 'mx' in sys.argv
            try:
                frame = wxFrame(NULL, -1, "wxTimeCtrl Test", wxPoint(20,20), wxSize(100,100) )
                panel = TestPanel(frame, -1, wxPoint(-1,-1), fmt24hr=fmt24hr, test_mx = test_mx)
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
