#----------------------------------------------------------------------------
# Name:         maskededit.py
# Authors:      Jeff Childers, Will Sadkin
# Email:        jchilders_98@yahoo.com, wsadkin@nameconnector.com
# Created:      02/11/2003
# Copyright:    (c) 2003 by Jeff Childers, 2003
# Portions:     (c) 2002 by Will Sadkin, 2002-2003
# RCS-ID:       $Id$
# License:      wxWindows license
#----------------------------------------------------------------------------
# NOTE:
#   This was written way it is because of the lack of masked edit controls
#   in wxWindows/wxPython.
#
#   wxMaskedEdit controls are based on a suggestion made on [wxPython-Users] by
#   Jason Hihn, and borrows liberally from Will Sadkin's original masked edit
#   control for time entry (wxTimeCtrl).
#
#   wxMaskedEdit controls do not normally use validators, because they do
#   careful manipulation of the cursor in the text window on each keystroke,
#   and validation is cursor-position specific, so the control intercepts the
#   key codes before the validator would fire.  However, validators can be
#   provided to do data transfer to the controls.
##

"""\
Masked Edit Overview:
=====================
  wxMaskedTextCtrl
      is a sublassed text control that can carefully control
      the user's input based on a mask string you provide.

      General usage example:
          control = wxMaskedTextCtrl( win, -1, '', mask = '(###) ###-####')

  The example above will create a text control that allows only numbers to
  be entered and then only in the positions indicated in the mask by the #
  character.

  wxMaskedComboBox
      is a similar subclass of wxComboBox that allows the
      same sort of masking, but also can do auto-complete of values, and can
      require the value typed to be in the list of choices to be colored
      appropriately.

  wxIpAddrCtrl
      is a special subclass of wxMaskedTextCtrl that handles
      cursor movement and natural typing of IP addresses.


INITILIZATION PARAMETERS
========================
mask=
Allowed mask characters and function:
    Character   Function
            #       Allow numeric only (0-9)
            N       Allow letters (a-z,A-Z) and numbers (0-9)
            A       Allow uppercase letters only (A-Z)
            a       Allow lowercase letters only (a-z)
            X       Allow any typeable character
            C       Allow any letter, upper or lower (a-z,A-Z)

  Using these characters, a variety of template masks can be built. See the
  demo for some other common examples include date+time, social security
  number, etc.  If any of these characters are needed as template rather
  than mask characters, they can be escaped with \, ie. \N means "literal N".
  (use \\ for literal backslash, as in: r'CCC\\NNN'.)

  Note: Changing the mask for a control deletes any previous field classes
        (and any associated validation or formatting constraints) for them.


useFixedWidthFont=
  By default, masked edit controls use a fixed width font, so that
  the mask characters are fixed within the control, regardless of
  subsequent modifications to the value.  Set to False if having
  the control font be the same as other controls is required.


formatcodes=
  These other properties can be passed to the class when instantiating it:
    formatcodes     A string of formatting codes that modify behavior of the control:
            _  Allow spaces
            !  Force upper
            ^  Force lower
            R  right-align field(s)
            r  right-insert in field(s) (implies R)
            &lt;  stay in field until explicit navigation out of it
            ,  Allow comma to be typed as grouping character; auto-group/regroup
               digits in integer fields (if result fits) when leaving such a field.
               (If used, .SetValue() will attempt to auto-group as well.)
               Note: typing '.' in such fields will clip the value to that left
               of the cursor for integer fields of controls with "integer" or
               "floating point" masks.  If the ',' format code is specified, this
               will also cause the resulting digits to be regrouped properly.
            -  Prepend and reserve leading space for sign to mask and allow
               signed values (negative #s shown in red by default)
            0  integer fields get leading zeros
            D  Date[/time] field
            T  Time field
            F  Auto-Fit: the control calulates its size from
               the length of the template mask
            V  validate entered chars against validRegex before allowing them
               to be entered vs. being allowed by basic mask and then having
               the resulting value just colored as invalid.
               (See USSTATE autoformat demo for how this can be used.)
            S  select entire field when navigating to new field

fillChar=
defaultValue=
  These controls have two options for the initial state of the control.
  If a blank control with just the non-editable characters showing
  is desired, simply leave the constructor variable fillChar as its
  default (' ').  If you want some other character there, simply
  change the fillChar to that value.  Note: changing the control's fillChar
  will implicitly reset all of the fields' fillChars to this value.

  If you need different default characters in each mask position,
  you can specify a defaultValue parameter in the constructor, or
  set them for each field individually.
  This value must satisfy the non-editable characters of the mask,
  but need not conform to the replaceable characters.


autoCompleteKeycodes=[]
  By default, DownArrow, PageUp and PageDown will auto-complete a
  partially entered field.  Shift-DownArrow, Shift-UpArrow, PageUp
  and PageDown will also auto-complete, but if the field already
  contains a matched value, these keys will cycle through the list
  of choices forward or backward as appropriate.  Shift-Up and
  Shift-Down also take you to the next/previous field after any
  auto-complete action.

  Additional auto-complete keys can be specified via this parameter.
  Any keys so specified will act like PageDown.



Validating User Input:
======================
  There are a variety of initialization parameters that are used to validate
  user input.  These parameters can apply to the control as a whole, and/or
  to individual fields:

        excludeChars=   A string of characters to exclude even if otherwise allowed
        includeChars=   A string of characters to allow even if otherwise disallowed
        validRegex=     Use a regular expression to validate the contents of the text box
        validRange=     Pass a rangeas list (low,high) to limit numeric fields/values
        choiceRequired= value must be member of choices list
        compareNoCase=  Perform case-insensitive matching when validating against list
        emptyInvalid=   Boolean indicating whether an empty value should be considered invalid

        validFunc=      A function to call of the form: bool = func(candidate_value)
                        which will return True if the candidate_value satisfies some
                        external criteria for the control in addition to the the
                        other validation, or False if not.  (This validation is
                        applied last in the chain of validations.)

        validRequired=  Boolean indicating whether or not keys that are allowed by the
                        mask, but result in an invalid value are allowed to be entered
                        into the control.  Setting this to True implies that a valid
                        default value is set for the control.

        retainFieldValidation=
                        False by default; if True, this allows individual fields to
                        retain their own validation constraints independently of any
                        subsequent changes to the control's overall parameters.

        validator=      Validators are not normally needed for masked controls, because
                        of the nature of the validation and control of input.  However,
                        you can supply one to provide data transfer routines for the
                        controls.


Coloring Behavior:
==================
  The following parameters have been provided to allow you to change the default
  coloring behavior of the control.   These can be set at construction, or via
  the .SetMaskParameters() function.  Pass a color as string e.g. 'Yellow':

        emptyBackgroundColor=       Control Background color when identified as empty. Default=White
        invalidBackgroundColor=     Control Background color when identified as Not valid. Default=Yellow
        validBackgroundColor=       Control Background color when identified as Valid. Default=white


  The following parameters control the default foreground color coloring behavior of the
  control. Pass a color as string e.g. 'Yellow':
        foregroundColor=            Control foreground color when value is not negative.  Default=Black
        signedForegroundColor=      Control foreground color when value is negative. Default=Red


Fields:
=======
  Each part of the mask that allows user input is considered a field.  The fields
  are represented by their own class instances.  You can specify field-specific
  constraints by constructing or accessing the field instances for the control
  and then specifying those constraints via parameters.

fields=
  This parameter allows you to specify Field instances containing
  constraints for the individual fields of a control, eg: local
  choice lists, validation rules, functions, regexps, etc.
  It can be either an ordered list or a dictionary.  If a list,
  the fields will be applied as fields 0, 1, 2, etc.
  If a dictionary, it should be keyed by field index.
  the values should be a instances of maskededit.Field.

  Any field not represented by the list or dictionary will be
  implicitly created by the control.

  eg:
    fields = [ Field(formatcodes='_r'), Field('choices=['a', 'b', 'c']) ]
  or
    fields = {
              1: ( Field(formatcodes='_R', choices=['a', 'b', 'c']),
              3: ( Field(choices=['01', '02', '03'], choiceRequired=True)
             }

  The following parameters are available for individual fields, with the
  same semantics as for the whole control but applied to the field in question:

    fillChar        # if set for a field, it will override the control's fillChar for that field
    defaultValue    # sets field-specific default value; overrides any default from control
    compareNoCase   # overrides control's settings
    emptyInvalid    # determines whether field is required to be filled at all times
    validRequired   # if set, requires field to contain valid value

  If any of the above parameters are subsequently specified for the control as a
  whole, that new value will be propagated to each field, unless the
  retainFieldValidation control-level parameter is set.

    formatcodes     # Augments control's settings
    excludeChars    #     '       '        '
    includeChars    #     '       '        '
    validRegex      #     '       '        '
    validRange      #     '       '        '
    choices         #     '       '        '
    choiceRequired  #     '       '        '
    validFunc       #     '       '        '



Control Class Functions:
========================
  .GetPlainValue(value=None)
                    Returns the value specified (or the control's text value
                    not specified) without the formatting text.
                    In the example above, might return phone no='3522640075',
                    whereas control.GetValue() would return '(352) 264-0075'
  .ClearValue()     Returns the control's value to its default, and places the
                    cursor at the beginning of the control.
  .SetValue()       Does "smart replacement" of passed value into the control, as does
                    the .Paste() method.  As with other text entry controls, the
                    .SetValue() text replacement begins at left-edge of the control,
                    with missing mask characters inserted as appropriate.
                    .SetValue will also adjust integer, float or date mask entry values,
                    adding commas, auto-completing years, etc. as appropriate.
                    If a value does not follow the format of the control's mask, or will
                    not fit into the control, a ValueError exception will be raised.
                    Eg:
                      mask = '(###) ###-####'
                          .SetValue('1234567890')           => '(123) 456-7890'
                          .SetValue('(123)4567890')         => '(123) 456-7890'
                          .SetValue('(123)456-7890')        => '(123) 456-7890'
                          .SetValue('123/4567-890')         => illegal paste; ValueError

                      mask = '#{6}.#{2}', formatcode = '_,-',
                          .SetValue('111')                  => ' 111   .  '
                          .SetValue(' %9.2f' % -111.12345 ) => '   -111.12'
                          .SetValue(' %9.2f' % 1234.00 )    => '  1,234.00'
                          .SetValue(' %9.2f' % -1234567.12345 ) => insufficient room; ValueError


  .IsValid(value=None)
                    Returns True if the value specified (or the value of the control
                    if not specified) passes validation tests
  .IsEmpty(value=None)
                    Returns True if the value specified (or the value of the control
                    if not specified) is equal to an "empty value," ie. all
                    editable characters == the fillChar for their respective fields.
  .IsDefault(value=None)
                    Returns True if the value specified (or the value of the control
                    if not specified) is equal to the initial value of the control.

  .Refresh()        Recolors the control as appropriate to its current settings.

  .SetMaskParameters(**kwargs)
                    This function allows you to set up and/or change the control parameters
                    after construction; it takes a list of key/value pairs as arguments,
                    where the keys can be any of the mask-specific parameters in the constructor.
                    Eg:
                        ctl = wxMaskedTextCtrl( self, -1 )
                        ctl.SetMaskParameters( mask='###-####',
                                               defaultValue='555-1212',
                                               formatcodes='F')

  .GetMaskParameter(parametername)
                    This function allows you to retrieve the current value of a parameter
                    from the control.

  .SetFieldParameters(field_index, **kwargs)
                    This function allows you to specify change individual field
                    parameters after construction. (Indices are 0-based.)

  .GetFieldParameter(field_index, parametername)
                    Allows the retrieval of field parameters after construction


The control detects certain common constructions. In order to use the signed feature
(negative numbers and coloring), the mask has to be all numbers with optionally one
decimal. Without a decimal (e.g. '######', the control will treat it as an integer
value. With a decimal (e.g. '###.##'), the control will act as a decimal control
(i.e. press decimal to 'tab' to the decimal position). Pressing decimal in the
integer control truncates the value.


Check your controls by calling each control's .IsValid() function and the
.IsEmpty() function to determine which controls have been a) filled in and
b) filled in properly.


Regular expression validations can be used flexibly and creatively.
Take a look at the demo; the zip-code validation succeeds as long as the
first five numerals are entered. the last four are optional, but if
any are entered, there must be 4 to be valid.

"""

"""
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DEVELOPER COMMENTS:

Naming Conventions
------------------
  All methods of the Mixin that are not meant to be exposed to the external
  interface are prefaced with '_'.  Those functions that are primarily
  intended to be internal subroutines subsequently start with a lower-case
  letter; those that are primarily intended to be used and/or overridden
  by derived subclasses start with a capital letter.

  The following methods must be used and/or defined when deriving a control
  from wxMaskedEditMixin.  NOTE: if deriving from a *masked edit* control
  (eg. class wxIpAddrCtrl(wxMaskedTextCtrl) ), then this is NOT necessary,
  as it's already been done for you in the base class.

        ._SetInitialValue()
                        This function must be called after the associated base
                        control has been initialized in the subclass __init__
                        function.  It sets the initial value of the control,
                        either to the value specified if non-empty, the
                        default value if specified, or the "template" for
                        the empty control as necessary.  It will also set/reset
                        the font if necessary and apply formatting to the
                        control at this time.

        ._GetSelection()
                        REQUIRED
                        Each class derived from wxMaskedEditMixin must define
                        the function for getting the start and end of the
                        current text selection.  The reason for this is
                        that not all controls have the same function name for
                        doing this; eg. wxTextCtrl uses .GetSelection(),
                        whereas we had to write a .GetMark() function for
                        wxComboBox, because .GetSelection() for the control
                        gets the currently selected list item from the combo
                        box, and the control doesn't (yet) natively provide
                        a means of determining the text selection.
        ._SetSelection()
                        REQUIRED
                        Similarly to _GetSelection, each class derived from
                        wxMaskedEditMixin must define the function for setting
                        the start and end of the current text selection.
                        (eg. .SetSelection() for wxMaskedTextCtrl, and .SetMark() for
                        wxMaskedComboBox.

        ._GetInsertionPoint()
        ._SetInsertionPoint()
                        REQUIRED
                        For consistency, and because the mixin shouldn't rely
                        on fixed names for any manipulations it does of any of
                        the base controls, we require each class derived from
                        wxMaskedEditMixin to define these functions as well.

        ._GetValue()
        ._SetValue()    REQUIRED
                        Each class derived from wxMaskedEditMixin must define
                        the functions used to get and set the raw value of the
                        control.
                        This is necessary so that recursion doesn't take place
                        when setting the value, and so that the mixin can
                        call the appropriate function after doing all its
                        validation and manipulation without knowing what kind
                        of base control it was mixed in with.

        .Cut()
        .Paste()
        .SetValue()     REQUIRED
                        Each class derived from wxMaskedEditMixin must redefine
                        these functions to call the _Cut(), _Paste() and _Paste()
                        methods, respectively for the control, so as to prevent
                        programmatic corruption of the control's value.  This
                        must be done in each derivation, as the mixin cannot
                        itself override a member of a sibling class.

        ._Refresh()     REQUIRED
                        Each class derived from wxMaskedEditMixin must define
                        the function used to refresh the base control.

        .Refresh()      REQUIRED
                        Each class derived from wxMaskedEditMixin must redefine
                        this function so that it checks the validity of the
                        control (via self._CheckValid) and then refreshes
                        control using the base class method.

        ._IsEditable()  REQUIRED
                        Each class derived from wxMaskedEditMixin must define
                        the function used to determine if the base control is
                        editable or not.  (For wxMaskedComboBox, this has to
                        be done with code, rather than specifying the proper
                        function in the base control, as there isn't one...)



Event Handling
--------------
  Event handlers are "chained", and wxMaskedEditMixin usually
  swallows most of the events it sees, thereby preventing any other
  handlers from firing in the chain.  It is therefore required that
  each class derivation using the mixin to have an option to hook up
  the event handlers itself or forego this operation and let a
  subclass of the masked control do so.  For this reason, each
  subclass should probably include the following code:

    if setupEventHandling:
        ## Setup event handlers
        EVT_SET_FOCUS( self, self._OnFocus )        ## defeat automatic full selection
        EVT_KILL_FOCUS( self, self._OnKillFocus )   ## run internal validator
        EVT_LEFT_DCLICK(self, self._OnDoubleClick)  ## select field under cursor on dclick
        EVT_KEY_DOWN( self, self._OnKeyDown )       ## capture control events not normally seen, eg ctrl-tab.
        EVT_CHAR( self, self._OnChar )              ## handle each keypress
        EVT_TEXT( self, self.GetId(), self._OnTextChange )  ## color control appropriately

  where setupEventHandling is an argument to its constructor.

  These 5 handlers must be "wired up" for the wxMaskedEdit
  control to provide default behavior.  (The setupEventHandling
  is an argument to wxMaskedTextCtrl and wxMaskedComboBox, so
  that controls derived from *them* may replace one of these
  handlers if they so choose.)

  If your derived control wants to preprocess events before
  taking action, it should then set up the event handling itself,
  so it can be first in the event handler chain.


  The following routines are available to facilitate changing
  the default behavior of wxMaskedEdit controls:

        ._SetKeycodeHandler(keycode, func)
        ._SetKeyHandler(char, func)
                        Use to replace default handling for any given keycode.
                        func should take the key event as argument and return
                        False if no further action is required to handle the
                        key. Eg:
                            self._SetKeycodeHandler(WXK_UP, self.IncrementValue)
                            self._SetKeyHandler('-', self._OnChangeSign)

        "Navigation" keys are assumed to change the cursor position, and
        therefore don't cause automatic motion of the cursor as insertable
        characters do.

        ._AddNavKeycode(keycode, handler=None)
        ._AddNavKey(char, handler=None)
                        Allows controls to specify other keys (and optional handlers)
                        to be treated as navigational characters. (eg. '.' in wxIpAddrCtrl)

        ._GetNavKeycodes()  Returns the current list of navigational keycodes.

        ._SetNavKeycodes(key_func_tuples)
                        Allows replacement of the current list of keycode
                        processed as navigation keys, and bind associated
                        optional keyhandlers. argument is a list of key/handler
                        tuples.  Passing a value of None for the handler in a
                        given tuple indicates that default processing for the key
                        is desired.

        ._FindField(pos) Returns the Field object associated with this position
                        in the control.

        ._FindFieldExtent(pos, getslice=False, value=None)
                        Returns edit_start, edit_end of the field corresponding
                        to the specified position within the control, and
                        optionally also returns the current contents of that field.
                        If value is specified, it will retrieve the slice the corresponding
                        slice from that value, rather than the current value of the
                        control.

        ._AdjustField(pos)
                        This is, the function that gets called for a given position
                        whenever the cursor is adjusted to leave a given field.
                        By default, it adjusts the year in date fields if mask is a date,
                        It can be overridden by a derived class to
                        adjust the value of the control at that time.
                        (eg. wxIpAddrCtrl reformats the address in this way.)

        ._Change()      Called by internal EVT_TEXT handler. Return False to force
                        skip of the normal class change event.
        ._Keypress(key) Called by internal EVT_CHAR handler. Return False to force
                        skip of the normal class keypress event.
        ._LostFocus()   Called by internal EVT_KILL_FOCUS handler

        ._OnKeyDown(event)
                        This is the default EVT_KEY_DOWN routine; it just checks for
                        "navigation keys", and if event.ControlDown(), it fires the
                        mixin's _OnChar() routine, as such events are not always seen
                        by the "cooked" EVT_CHAR routine.

        ._OnChar(event) This is the main EVT_CHAR handler for the
                        wxMaskedEditMixin.

    The following routines are used to handle standard actions
    for control keys:
        _OnArrow(event)         used for arrow navigation events
        _OnCtrl_A(event)        'select all'
        _OnCtrl_S(event)        'save' (does nothing)
        _OnCtrl_V(event)        'paste' - calls _Paste() method, to do smart paste
        _OnCtrl_X(event)        'cut'   - calls _Cut() method, to "erase" selection

        _OnChangeField(event)   primarily used for tab events, but can be
                                used for other keys (eg. '.' in wxIpAddrCtrl)

        _OnErase(event)         used for backspace and delete
        _OnHome(event)
        _OnEnd(event)

"""

from wxPython.wx import *
import string, re, copy

from wxPython.tools.dbg import Logger
dbg = Logger()
dbg(enable=0)

## ---------- ---------- ---------- ---------- ---------- ---------- ----------

## Constants for identifying control keys and classes of keys:

WXK_CTRL_X = (ord('X')+1) - ord('A')   ## These keys are not already defined in wx
WXK_CTRL_V = (ord('V')+1) - ord('A')
WXK_CTRL_C = (ord('C')+1) - ord('A')
WXK_CTRL_S = (ord('S')+1) - ord('A')
WXK_CTRL_A = (ord('A')+1) - ord('A')

nav =     (WXK_BACK, WXK_LEFT, WXK_RIGHT, WXK_UP, WXK_DOWN, WXK_TAB, WXK_HOME, WXK_END, WXK_RETURN, WXK_PRIOR, WXK_NEXT)
control = (WXK_BACK, WXK_DELETE, WXK_CTRL_A, WXK_CTRL_C, WXK_CTRL_S, WXK_CTRL_V, WXK_CTRL_X)


## ---------- ---------- ---------- ---------- ---------- ---------- ----------

## Constants for masking. This is where mask characters
## are defined.
##  maskchars used to identify valid mask characters from all others
##  masktran  used to replace mask chars with spaces
##  maskchardict  used to defined allowed character sets for each mask char
##   #- allow numeric 0-9 only
##   A- allow uppercase only. Combine with forceupper to force lowercase to upper
##   a- allow lowercase only. Combine with forcelower to force upper to lowercase
##   X- allow any character (letters, punctuation, digits)
##
maskchars = ("#","A","a","X","C","N")
maskchardict  = {
        "#": string.digits,
        "A": string.uppercase,
        "a": string.lowercase,
        "X": string.letters + string.punctuation + string.digits,
        "C": string.letters,
        "N": string.letters + string.digits
        }

months = '(01|02|03|04|05|06|07|08|09|10|11|12)'
charmonths = '(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec|JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC|jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec)'
charmonths_dict = {'jan': 1, 'feb': 2, 'mar': 3, 'apr': 4, 'may': 5, 'jun': 6,
                   'jul': 7, 'aug': 8, 'sep': 9, 'oct': 10, 'nov': 11, 'dec': 12}

days   = '(01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31)'
hours  = '(0\d| \d|1[012])'
milhours = '(00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23)'
minutes = """(00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|\
16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|\
36|37|38|39|40|41|42|43|44|45|46|47|48|49|50|51|52|53|54|55|\
56|57|58|59)"""
seconds = minutes
am_pm_exclude = 'BCDEFGHIJKLMNOQRSTUVWXYZ\x8a\x8c\x8e\x9f\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd8\xd9\xda\xdb\xdc\xdd\xde'

states = "AL,AK,AZ,AR,CA,CO,CT,DE,DC,FL,GA,GU,HI,ID,IL,IN,IA,KS,KY,LA,MA,ME,MD,MI,MN,MS,MO,MT,NE,NV,NH,NJ,NM,NY,NC,ND,OH,OK,OR,PA,PR,RI,SC,SD,TN,TX,UT,VA,VT,VI,WA,WV,WI,WY".split(',')


## ---------- ---------- ---------- ---------- ---------- ---------- ----------

## The following table defines the current set of autoformat codes:

masktags = {
       #  Name: (mask, excludeChars, formatcodes, validRegex, choices, choiceRequired, description)
       "USPHONEFULLEXT":("(###) ###-#### x:###","",'F^-R',"^\(\d{3}\) \d{3}-\d{4}",[], False, "Phone Number w/opt. ext"),
       "USPHONETIGHTEXT":("###-###-#### x:###","",'F^-R',"^\d{3}-\d{3}-\d{4}",[], False, "Phone Number\n (w/hyphens and opt. ext)"),
       "USPHONEFULL":("(###) ###-####","",'F^-R',"^\(\d{3}\) \d{3}-\d{4}",[], False, "Phone Number only"),
       "USPHONETIGHT":("###-###-####","",'F^-R',"^\d{3}-\d{3}-\d{4}",[], False, "Phone Number\n(w/hyphens)"),
       "USSTATE":("AA","",'F!V',"([ACDFGHIKLMNOPRSTUVW] |%s)" % string.join(states,'|'), states, True, "US State"),

       "USDATETIMEMMDDYYYY/HHMMSS":("##/##/#### ##:##:## AM",am_pm_exclude,'DF!','^' + months + '/' + days + '/' + '\d{4} ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',[], False, "US Date + Time"),
       "USDATETIMEMMDDYYYY-HHMMSS":("##-##-#### ##:##:## AM",am_pm_exclude,'DF!','^' + months + '-' + days + '-' + '\d{4} ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',[], False, "US Date + Time\n(w/hypens)"),
       "USDATEMILTIMEMMDDYYYY/HHMMSS":("##/##/#### ##:##:##",'','DF','^' + months + '/' + days + '/' + '\d{4} ' + milhours + ':' + minutes + ':' + seconds,[], False, "US Date + Military Time"),
       "USDATEMILTIMEMMDDYYYY-HHMMSS":("##-##-#### ##:##:##",'','DF','^' + months + '-' + days + '-' + '\d{4} ' + milhours + ':' + minutes + ':' + seconds,[], False, "US Date + Military Time\n(w/hypens)"),
       "USDATETIMEMMDDYYYY/HHMM":("##/##/#### ##:## AM",am_pm_exclude,'DF!','^' + months + '/' + days + '/' + '\d{4} ' + hours + ':' + minutes + ' (A|P)M',[], False, "US Date + Time\n(without seconds)"),
       "USDATEMILTIMEMMDDYYYY/HHMM":("##/##/#### ##:##",'','DF','^' + months + '/' + days + '/' + '\d{4} ' + milhours + ':' + minutes,[], False, "US Date + Military Time\n(without seconds)"),
       "USDATETIMEMMDDYYYY-HHMM":("##-##-#### ##:## AM",am_pm_exclude,'DF!','^' + months + '-' + days + '-' + '\d{4} ' + hours + ':' + minutes + ' (A|P)M',[], False, "US Date + Time\n(w/hypens and w/o secs)"),
       "USDATEMILTIMEMMDDYYYY-HHMM":("##-##-#### ##:##",'','DF','^' + months + '-' + days + '-' + '\d{4} ' + milhours + ':' + minutes,[], False, "US Date + Military Time\n(w/hyphens and w/o seconds)"),
       "USDATEMMDDYYYY/":("##/##/####",'','DF','^' + months + '/' + days + '/' + '\d{4}',[], False, "US Date\n(MMDDYYYY)"),
       "USDATEMMDDYY/":("##/##/##",'','DF','^' + months + '/' + days + '/\d\d',[], False, "US Date\n(MMDDYY)"),
       "USDATEMMDDYYYY-":("##-##-####",'','DF','^' + months + '-' + days + '-' +'\d{4}',[], False, "MM-DD-YYYY"),

       "EUDATEYYYYMMDD/":("####/##/##",'','DF','^' + '\d{4}'+ '/' + months + '/' + days,[], False, "YYYY/MM/DD"),
       "EUDATEYYYYMMDD.":("####.##.##",'','DF','^' + '\d{4}'+ '.' + months + '.' + days,[], False, "YYYY.MM.DD"),
       "EUDATEDDMMYYYY/":("##/##/####",'','DF','^' + days + '/' + months + '/' + '\d{4}',[], False, "DD/MM/YYYY"),
       "EUDATEDDMMYYYY.":("##.##.####",'','DF','^' + days + '.' + months + '.' + '\d{4}',[], False, "DD.MM.YYYY"),
       "EUDATEDDMMMYYYY.":("##.CCC.####",'','DF','^' + days + '.' + charmonths + '.' + '\d{4}',[], False, "DD.Month.YYYY"),
       "EUDATEDDMMMYYYY/":("##/CCC/####",'','DF','^' + days + '/' + charmonths + '/' + '\d{4}',[], False, "DD/Month/YYYY"),

       "EUDATETIMEYYYYMMDD/HHMMSS":("####/##/## ##:##:## AM",am_pm_exclude,'DF!','^' + '\d{4}'+ '/' + months + '/' + days + ' ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',[], False, "YYYY/MM/DD HH:MM:SS"),
       "EUDATETIMEYYYYMMDD.HHMMSS":("####.##.## ##:##:## AM",am_pm_exclude,'DF!','^' + '\d{4}'+ '.' + months + '.' + days + ' ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',[], False, "YYYY.MM.DD HH:MM:SS"),
       "EUDATETIMEDDMMYYYY/HHMMSS":("##/##/#### ##:##:## AM",am_pm_exclude,'DF!','^' + days + '/' + months + '/' + '\d{4} ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',[], False, "DD/MM/YYYY HH:MM:SS"),
       "EUDATETIMEDDMMYYYY.HHMMSS":("##.##.#### ##:##:## AM",am_pm_exclude,'DF!','^' + days + '.' + months + '.' + '\d{4} ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',[], False, "DD.MM.YYYY HH:MM:SS"),

       "EUDATETIMEYYYYMMDD/HHMM":("####/##/## ##:## AM",am_pm_exclude,'DF!','^' + '\d{4}'+ '/' + months + '/' + days + ' ' + hours + ':' + minutes + ' (A|P)M',[], False, "YYYY/MM/DD HH:MM"),
       "EUDATETIMEYYYYMMDD.HHMM":("####.##.## ##:## AM",am_pm_exclude,'DF!','^' + '\d{4}'+ '.' + months + '.' + days + ' ' + hours + ':' + minutes + ' (A|P)M',[], False, "YYYY.MM.DD HH:MM"),
       "EUDATETIMEDDMMYYYY/HHMM":("##/##/#### ##:## AM",am_pm_exclude,'DF!','^' + days + '/' + months + '/' + '\d{4} ' + hours + ':' + minutes + ' (A|P)M',[], False, "DD/MM/YYYY HH:MM"),
       "EUDATETIMEDDMMYYYY.HHMM":("##.##.#### ##:## AM",am_pm_exclude,'DF!','^' + days + '.' + months + '.' + '\d{4} ' + hours + ':' + minutes + ' (A|P)M',[], False, "DD.MM.YYYY HH:MM"),

       "EUDATEMILTIMEYYYYMMDD/HHMMSS":("####/##/## ##:##:##",'','DF','^' + '\d{4}'+ '/' + months + '/' + days + ' ' + milhours + ':' + minutes + ':' + seconds,[], False, "YYYY/MM/DD Mil. Time"),
       "EUDATEMILTIMEYYYYMMDD.HHMMSS":("####.##.## ##:##:##",'','DF','^' + '\d{4}'+ '.' + months + '.' + days + ' ' + milhours + ':' + minutes + ':' + seconds,[], False, "YYYY.MM.DD Mil. Time"),
       "EUDATEMILTIMEDDMMYYYY/HHMMSS":("##/##/#### ##:##:##",'','DF','^' + days + '/' + months + '/' + '\d{4} ' + milhours + ':' + minutes + ':' + seconds,[], False, "DD/MM/YYYY Mil. Time"),
       "EUDATEMILTIMEDDMMYYYY.HHMMSS":("##.##.#### ##:##:##",'','DF','^' + days + '.' + months + '.' + '\d{4} ' + milhours + ':' + minutes + ':' + seconds,[], False, "DD.MM.YYYY Mil. Time"),
       "EUDATEMILTIMEYYYYMMDD/HHMM":("####/##/## ##:##",'','DF','^' + '\d{4}'+ '/' + months + '/' + days + ' ' + milhours + ':' + minutes,[], False, "YYYY/MM/DD Mil. Time\n(w/o seconds)"),
       "EUDATEMILTIMEYYYYMMDD.HHMM":("####.##.## ##:##",'','DF','^' + '\d{4}'+ '.' + months + '.' + days + ' ' + milhours + ':' + minutes,[], False, "YYYY.MM.DD Mil. Time\n(w/o seconds)"),
       "EUDATEMILTIMEDDMMYYYY/HHMM":("##/##/#### ##:##",'','DF','^' + days + '/' + months + '/' + '\d{4} ' + milhours + ':' + minutes,[], False, "DD/MM/YYYY Mil. Time\n(w/o seconds)"),
       "EUDATEMILTIMEDDMMYYYY.HHMM":("##.##.#### ##:##",'','DF','^' + days + '.' + months + '.' + '\d{4} ' + milhours + ':' + minutes,[], False, "DD.MM.YYYY Mil. Time\n(w/o seconds)"),

       "TIMEHHMMSS":("##:##:## AM", am_pm_exclude, 'TF!', '^' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',[], False, "HH:MM:SS (A|P)M\n(see wxTimeCtrl)"),
       "TIMEHHMM":("##:## AM", am_pm_exclude, 'TF!', '^' + hours + ':' + minutes + ' (A|P)M',[], False, "HH:MM (A|P)M\n(see wxTimeCtrl)"),
       "MILTIMEHHMMSS":("##:##:##", "", 'TF', '^' + milhours + ':' + minutes + ':' + seconds,[], False, "Military HH:MM:SS\n(see wxTimeCtrl)"),
       "MILTIMEHHMM":("##:##", "", 'TF', '^' + milhours + ':' + minutes,[], False, "Military HH:MM\n(see wxTimeCtrl)"),
       "USSOCIALSEC":("###-##-####","",'F',"\d{3}-\d{2}-\d{4}",[], False, "Social Sec#"),
       "CREDITCARD":("####-####-####-####","",'F',"\d{4}-\d{4}-\d{4}-\d{4}",[], False, "Credit Card"),
       "EXPDATEMMYY":("##/##", "", "F", "^" + months + "/\d\d",[], False, "Expiration MM/YY"),
       "USZIP":("#####","",'F',"^\d{5}",[], False, "US 5-digit zip code"),
       "USZIPPLUS4":("#####-####","",'F',"\d{5}-(\s{4}|\d{4})",[], False, "US zip+4 code"),
       "PERCENT":("0.##","",'F',"^0.\d\d",[], False, "Percentage"),
       "AGE":("###","","F","^[1-9]{1}  |[1-9][0-9] |1[0|1|2][0-9]",[], False, "Age"),
       "EMAIL":("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"," \\/*&%$#!+='\"","F",
                "[a-zA-Z]\w*(\.\w+)*@\w+\.([a-zA-Z]\w*\.)*(com|org|net|edu|mil|gov|(co\.)?\w\w) *$",[], False, "Email address"),
       "IPADDR":("###.###.###.###", "", 'F_Sr<',
                 "(  \d| \d\d|(1\d\d|2[0-4]\d|25[0-5]))(\.(  \d| \d\d|(1\d\d|2[0-4]\d|25[0-5]))){3}",[], False, "IP Address\n(see wxIpAddrCtrl)")
       }

# build demo-friendly dictionary of descriptions of autoformats
autoformats = []
for key, value in masktags.items():
    autoformats.append((key, value[6]))
autoformats.sort()

## ---------- ---------- ---------- ---------- ---------- ---------- ----------

class Field:
    valid_params = {
              'index': None,                    ## which field of mask; set by parent control.
              'mask': "",                       ## mask chars for this field
              'extent': (),                     ## (edit start, edit_end) of field; set by parent control.
              'formatcodes':  "",               ## codes indicating formatting options for the control
              'fillChar':     ' ',              ## used as initial value for each mask position if initial value is not given
              'defaultValue': "",               ## use if you want different positional defaults vs. all the same fillChar
              'excludeChars': "",               ## optional string of chars to exclude even if main mask type does
              'includeChars': "",               ## optional string of chars to allow even if main mask type doesn't
              'validRegex':   "",               ## optional regular expression to use to validate the control
              'validRange':   (),               ## Optional hi-low range for numerics
              'choices':    [],                 ## Optional list for character expressions
              'choiceRequired': False,          ## If choices supplied this specifies if valid value must be in the list
              'validFunc': None,                ## Optional function for defining additional, possibly dynamic validation constraints on contrl
              'compareNoCase': False,           ## Optional flag to indicate whether or not to use case-insensitive list search
              'validRequired': False,           ## Set to True to disallow input that results in an invalid value
              'emptyInvalid':  False,           ## Set to True to make EMPTY = INVALID
              }


    def __init__(self, **kwargs):
        """
        This is the "constructor" for setting up parameters for fields.
        a field_index of -1 is used to indicate "the entire control."
        """
##        dbg('Field::Field', indent=1)
        # Validate legitimate set of parameters:
        for key in kwargs.keys():
            if key not in Field.valid_params.keys():
                raise TypeError('invalid parameter "%s"' % (key))

        # Set defaults for each parameter for this instance, and fully
        # populate initial parameter list for configuration:
        for key, value in Field.valid_params.items():
            setattr(self, '_' + key, copy.copy(value))
            if not kwargs.has_key(key):
                kwargs[key] = copy.copy(value)

        self._groupchar   = ','     # (this might be changable in a future version, but for now is constant)

        self._SetParameters(**kwargs)

##        dbg(indent=0)


    def _SetParameters(self, **kwargs):
        """
        This function can be used to set individual or multiple parameters for
        a masked edit field parameter after construction.
        """
        dbg(suspend=1)
        dbg('maskededit.Field::_SetParameters', indent=1)
        # Validate keyword arguments:
        for key in kwargs.keys():
            if key not in Field.valid_params.keys():
                raise AttributeError('invalid keyword argument "%s"' % key)

        if self._index is not None: dbg('field index:', self._index)
        dbg('parameters:', indent=1)
        for key, value in kwargs.items():
            dbg('%s:' % key, value)
        dbg(indent=0)

        # First, Assign all parameters specified:
        for key in Field.valid_params.keys():
            if kwargs.has_key(key):
                setattr(self, '_' + key, kwargs[key] )

        # Now go do validation, semantic and inter-dependency parameter processing:
        if kwargs.has_key('choiceRequired'): # (set/changed)
            if self._choiceRequired:
                self._choices = [choice.strip() for choice in self._choices]

        if kwargs.has_key('compareNoCase'): # (set/changed)
            if self._compareNoCase and self._choices:
                self._choices = [item.lower() for item in self._choices]
                dbg('modified choices:', self._choices)

        if kwargs.has_key('formatcodes'):   # (set/changed)
            self._forceupper  = '!' in self._formatcodes
            self._forcelower  = '^' in self._formatcodes
            self._groupdigits = ',' in self._formatcodes
            self._okSpaces    = '_' in self._formatcodes
            self._padZero     = '0' in self._formatcodes
            self._autofit     = 'F' in self._formatcodes
            self._insertRight = 'r' in self._formatcodes
            self._alignRight  = 'R' in self._formatcodes or 'r' in self._formatcodes
            self._moveOnFieldFull = not '<' in self._formatcodes
            self._selectOnFieldEntry = 'S' in self._formatcodes

        if kwargs.has_key('formatcodes') or kwargs.has_key('validRegex'):
            self._regexMask   = 'V' in self._formatcodes and self._validRegex

        if kwargs.has_key('validRegex'):    # (set/changed)
            if self._validRegex:
                try:
                    if self._compareNoCase:
                        self._filter = re.compile(self._validRegex, re.IGNORECASE)
                    else:
                        self._filter = re.compile(self._validRegex)
                except:
                    raise TypeError('%s: validRegex "%s" not a legal regular expression' % (str(self._index), self._validRegex))
            else:
                self._filter = None

        if kwargs.has_key('mask') or kwargs.has_key('validRegex'):  # (set/changed)
            self._isInt = isInteger(self._mask)
            dbg('isInt?', self._isInt)

        if kwargs.has_key('validRange'):    # (set/changed)
            self._hasRange  = False
            self._rangeHigh = 0
            self._rangeLow  = 0
            if self._validRange:
                if type(self._validRange) != type(()) or len( self._validRange )!= 2 or self._validRange[0] >= self._validRange[1]:
                    raise TypeError('%s: validRange %s parameter must be tuple of form (a,b) where a < b'
                                    % (str(self._index), repr(self._validRange)) )

                self._hasRange  = True
                self._rangeLow  = self._validRange[0]
                self._rangeHigh = self._validRange[1]

        if kwargs.has_key('choices'):       # (set/changed)
            self._hasList   = False
            if type(self._choices) not in (type(()), type([])):
                raise TypeError('%s: choices must be a sequence of strings' % str(self._index))
            elif len( self._choices) > 0:
                for choice in self._choices:
                    if type(choice) != type(''):
                        raise TypeError('%s: choices must be a sequence of strings' % str(self._index))

                # Verify each choice specified is valid:
                for choice in self._choices:
                    if not self.IsValid(choice):
                        raise ValueError('%s: "%s" is not a valid value for the control as specified.' % (str(self._index), choice))
                self._hasList = True

        # reset field validity assumption:
        self._valid = True
        dbg(indent=0, suspend=0)


    def _GetParameter(self, paramname):
        """
        Routine for retrieving the value of any given parameter
        """
        if Field.valid_params.has_key(paramname):
            return getattr(self, '_' + paramname)
        else:
            TypeError('Field._GetParameter: invalid parameter "%s"' % key)


    def IsEmpty(self, slice):
        """
        Indicates whether the specified slice is considered empty for the
        field.
        """
        dbg('Field::IsEmpty("%s")' % slice, indent=1)
        if not hasattr(self, '_template'):
            raise AttributeError('_template')

        dbg('self._template: "%s"' % self._template)
        dbg('self._defaultValue: "%s"' % str(self._defaultValue))
        if slice == self._template and not self._defaultValue:
            dbg(indent=0)
            return True

        elif slice == self._template:
            empty = True
            for pos in range(len(self._template)):
##                dbg('slice[%(pos)d] != self._fillChar?' %locals(), slice[pos] != self._fillChar[pos])
                if slice[pos] not in (' ', self._fillChar):
                    empty = False
                    break
            dbg("IsEmpty? %(empty)d (do all mask chars == fillChar?)" % locals(), indent=0)
            return empty
        else:
            dbg("IsEmpty? 0 (slice doesn't match template)", indent=0)
            return False


    def IsValid(self, slice):
        """
        Indicates whether the specified slice is considered a valid value for the
        field.
        """
        dbg(suspend=1)
        dbg('Field[%s]::IsValid("%s")' % (str(self._index), slice), indent=1)
        valid = True    # assume true to start

        if self._emptyInvalid and self.IsEmpty(slice):
            valid = False

        elif self._hasList and self._choiceRequired:
            dbg("(member of list required)")
            # do case-insensitive match on list; strip surrounding whitespace from slice (already done for choices):
            compareStr = slice.strip()
            if self._compareNoCase:
                compareStr = compareStr.lower()
            valid = (compareStr in self._choices)

        elif self._hasRange and not self.IsEmpty(slice):
            dbg('validating against range')
            try:
                valid = self._rangeLow <= int(slice) <= self._rangeHigh
            except:
                valid = False

        elif self._validRegex and self._filter:
            dbg('validating against regex')
            valid = (re.match( self._filter, slice) is not None)

        if valid and self._validFunc:
            dbg('validating against supplied function')
            valid = self._validFunc(slice)
        dbg('valid?', valid, indent=0, suspend=0)
        return valid


    def _AdjustField(self, slice):
        """ 'Fixes' an integer field. Right or left-justifies, as required."""
        dbg('Field::_AdjustField("%s")' % slice, indent=1)
        length = len(self._mask)
        if self._isInt:
            signpos = slice.find('-')
            intStr = slice.replace( '-', '' )                       # drop sign, if any
            intStr = intStr.replace(' ', '')                        # drop extra spaces
            intStr = string.replace(intStr,self._fillChar,"")       # drop extra fillchars
            intStr = string.replace(intStr,"-","")                  # drop sign, if any
            intStr = string.replace(intStr, self._groupchar, "")    # lose commas
            if self._groupdigits:
                new = ''
                cnt = 1
                for i in range(len(intStr)-1, -1, -1):
                    new = intStr[i] + new
                    if (cnt) % 3 == 0:
                        new = self._groupchar + new
                    cnt += 1
                if new and new[0] == self._groupchar:
                    new = new[1:]
                if len(new) <= length:
                    # expanded string will still fit and leave room for sign:
                    intStr = new
                # else... leave it without the commas...

            dbg('padzero?', self._padZero)
            dbg('len(intStr):', len(intStr), 'field length:', length)
            if self._padZero and len(intStr) < length:
                intStr = '0' * (length - len(intStr)) + intStr
                if signpos != -1:
                    intStr = '-' + intStr[1:]
            elif signpos != -1:
                intStr = '-' + intStr
            slice = intStr

        slice = slice.strip() # drop extra spaces

        if self._alignRight:     ## Only if right-alignment is enabled
            slice = slice.rjust( length )
        else:
            slice = slice.ljust( length )
        dbg('adjusted slice: "%s"' % slice, indent=0)
        return slice


## ---------- ---------- ---------- ---------- ---------- ---------- ----------

class wxMaskedEditMixin:
    """
    This class allows us to abstract the masked edit functionality that could
    be associated with any text entry control. (eg. wxTextCtrl, wxComboBox, etc.)
    """
    valid_ctrl_params = {
              'mask': 'XXXXXXXXXXXXX',          ## mask string for formatting this control
              'autoformat':   "",               ## optional auto-format code to set format from masktags dictionary
              'fields': {},                     ## optional list/dictionary of maskededit.Field class instances, indexed by position in mask
              'datestyle':    'MDY',            ## optional date style for date-type values. Can trigger autocomplete year
              'autoCompleteKeycodes': [],       ## Optional list of additional keycodes which will invoke field-auto-complete
              'useFixedWidthFont': True,        ## Use fixed-width font instead of default for base control
              'retainFieldValidation': False,   ## Set this to true if setting control-level parameters independently,
                                                ## from field validation constraints
              'emptyBackgroundColor': "White",
              'validBackgroundColor': "White",
              'invalidBackgroundColor': "Yellow",
              'foregroundColor': "Black",
              'signedForegroundColor': "Red",
              'demo': False}


    def __init__(self, name = 'wxMaskedEdit', **kwargs):
        """
        This is the "constructor" for setting up the mixin variable parameters for the composite class.
        """

        self.name = name

        # set up flag for doing optional things to base control if possible
        if not hasattr(self, 'controlInitialized'):
            self.controlInitialized = False

        # Set internal state var for keeping track of whether or not a character
        # action results in a modification of the control, since .SetValue()
        # doesn't modify the base control's internal state:
        self.modified = False

        # Validate legitimate set of parameters:
        for key in kwargs.keys():
            if key not in wxMaskedEditMixin.valid_ctrl_params.keys() + Field.valid_params.keys():
                raise TypeError('%s: invalid parameter "%s"' % (name, key))

        ## Set up dictionary that can be used by subclasses to override or add to default
        ## behavior for individual characters.  Derived subclasses needing to change
        ## default behavior for keys can either redefine the default functions for the
        ## common keys or add functions for specific keys to this list.  Each function
        ## added should take the key event as argument, and return False if the key
        ## requires no further processing.
        ##
        ## Initially populated with navigation and function control keys:
        self._keyhandlers = {
            # default navigation keys and handlers:
            WXK_BACK:   self._OnErase,
            WXK_LEFT:   self._OnArrow,
            WXK_RIGHT:  self._OnArrow,
            WXK_UP:     self._OnAutoCompleteField,
            WXK_DOWN:   self._OnAutoCompleteField,
            WXK_TAB:    self._OnChangeField,
            WXK_HOME:   self._OnHome,
            WXK_END:    self._OnEnd,
            WXK_RETURN: self._OnReturn,
            WXK_PRIOR:  self._OnAutoCompleteField,
            WXK_NEXT:   self._OnAutoCompleteField,

            # default function control keys and handlers:
            WXK_DELETE: self._OnErase,
            WXK_CTRL_A: self._OnCtrl_A,
            WXK_CTRL_C: self._baseCtrlEventHandler,
            WXK_CTRL_S: self._OnCtrl_S,
            WXK_CTRL_V: self._OnCtrl_V,
            WXK_CTRL_X: self._OnCtrl_X,
            }

        ## bind standard navigational and control keycodes to this instance,
        ## so that they can be augmented and/or changed in derived classes:
        self._nav = list(nav)
        self._control = list(control)

        ## self._ignoreChange is used by wxMaskedComboBox, because
        ## of the hack necessary to determine the selection; it causes
        ## EVT_TEXT messages from the combobox to be ignored if set.
        self._ignoreChange = False
        self._oldvalue  = None

        self._valid     = True

        # Set defaults for each parameter for this instance, and fully
        # populate initial parameter list for configuration:
        for key, value in wxMaskedEditMixin.valid_ctrl_params.items():
            setattr(self, '_' + key, copy.copy(value))
            if not kwargs.has_key(key):
##                dbg('%s: "%s"' % (key, repr(value)))
                kwargs[key] = copy.copy(value)

        # Create a "field" that holds global parameters for control constraints
        self._ctrl_constraints = self._fields[-1] = Field(index=-1)
        self.SetMaskParameters(**kwargs)



    def SetMaskParameters(self, **kwargs):
        """
        This public function can be used to set individual or multiple masked edit
        parameters after construction.
        """
        dbg('wxMaskedEditMixin::SetMaskParameters', indent=1)
        dbg('kwargs:', indent=1)
        for key, value in kwargs.items():
            dbg(key, '=', value)
        dbg(indent=0)

        # Validate keyword arguments:
        constraint_kwargs = {}
        ctrl_kwargs = {}
        for key, value in kwargs.items():
            if key not in wxMaskedEditMixin.valid_ctrl_params.keys() + Field.valid_params.keys():
                raise TypeError('%s: invalid keyword argument "%s"' % (self.name, key))
            elif key in Field.valid_params.keys():
                constraint_kwargs[key] = value
            else:
                ctrl_kwargs[key] = value

        mask = None
        reset_args = {}

        if ctrl_kwargs.has_key('autoformat'):
            autoformat = ctrl_kwargs['autoformat']
        else:
            autoformat = None

        if autoformat != self._autoformat and autoformat in masktags.keys():
            dbg('autoformat:', autoformat)
            self._autoformat                  = autoformat
            mask                              = masktags[self._autoformat][0]
            constraint_kwargs['excludeChars'] = masktags[self._autoformat][1]
            constraint_kwargs['formatcodes']  = masktags[self._autoformat][2]
            constraint_kwargs['validRegex']   = masktags[self._autoformat][3]
            constraint_kwargs['choices']      = masktags[self._autoformat][4]
            if masktags[self._autoformat][4]:
                constraint_kwargs['choiceRequired'] = masktags[self._autoformat][5]

        else:
            dbg('autoformat not selected')
            if kwargs.has_key('mask'):
                mask = kwargs['mask']
                dbg('mask:', mask)

        ## Assign style flags
        if mask is None:
            dbg('preserving previous mask')
            mask = self._previous_mask   # preserve previous mask
        else:
            dbg('mask (re)set')
            reset_args['reset_mask'] = mask
            constraint_kwargs['mask'] = mask

            # wipe out previous fields; preserve new control-level constraints
            self._fields = {-1: self._ctrl_constraints}


        if ctrl_kwargs.has_key('fields'):
            # do field parameter type validation, and conversion to internal dictionary
            # as appropriate:
            fields = ctrl_kwargs['fields']
            if type(fields) in (types.ListType, types.TupleType):
                for i in range(len(fields)):
                    field = fields[i]
                    if not isinstance(field, Field):
                        dbg(indent=0)
                        raise AttributeError('invalid type for field parameter: %s' % repr(field))
                    self._fields[i] = field

            elif type(fields) == types.DictionaryType:
                for index, field in fields.items():
                    if not isinstance(field, Field):
                        dbg(indent=0)
                        raise AttributeError('invalid type for field parameter: %s' % repr(field))
                    self._fields[index] = field
            else:
                dbg(indent=0)
                raise AttributeError('fields parameter must be a list or dictionary; not %s' % repr(fields))

        # Assign constraint parameters for entire control:
##        dbg('control constraints:', indent=1)
##        for key, value in constraint_kwargs.items():
##            dbg('%s:' % key, value)
##        dbg(indent=0)

        # determine if changing parameters that should affect the entire control:
        for key in wxMaskedEditMixin.valid_ctrl_params.keys():
            if key in ( 'mask', 'fields' ): continue    # (processed separately)
            if ctrl_kwargs.has_key(key):
                setattr(self, '_' + key, ctrl_kwargs[key])


        dbg('self._retainFieldValidation:', self._retainFieldValidation)
        if not self._retainFieldValidation:
            # Build dictionary of any changing parameters which should be propagated to the
            # component fields:
            for arg in ('fillChar', 'compareNoCase', 'defaultValue', 'validRequired'):
                dbg('kwargs.has_key(%s)?' % arg, kwargs.has_key(arg))
                dbg('getattr(self._ctrl_constraints, _%s)?' % arg, getattr(self._ctrl_constraints, '_'+arg))
                reset_args[arg] = kwargs.has_key(arg) and kwargs[arg] != getattr(self._ctrl_constraints, '_'+arg)
                dbg('reset_args[%s]?' % arg, reset_args[arg])

        # Set the control-level constraints:
        self._ctrl_constraints._SetParameters(**constraint_kwargs)

        # This routine does the bulk of the interdependent parameter processing, determining
        # the field extents of the mask if changed, resetting parameters as appropriate,
        # determining the overall template value for the control, etc.
        self._configure(mask, **reset_args)

        self._autofit = self._ctrl_constraints._autofit
        self._isNeg      = False

        self._isDate     = 'D' in self._ctrl_constraints._formatcodes and isDateType(mask)
        self._isTime     = 'T' in self._ctrl_constraints._formatcodes and isTimeType(mask)
        if self._isDate:
            # Set _dateExtent, used in date validation to locate date in string;
            # always set as though year will be 4 digits, even if mask only has
            # 2 digits, so we can always properly process the intended year for
            # date validation (leap years, etc.)
            if self._mask.find('CCC') != -1: self._dateExtent = 11
            else:                            self._dateExtent = 10

            self._4digityear = len(self._mask) > 8 and self._mask[9] == '#'

        if self._isDate and self._autoformat:
            # Auto-decide datestyle:
            if self._autoformat.find('MDDY')    != -1: self._datestyle = 'MDY'
            elif self._autoformat.find('YMMD')  != -1: self._datestyle = 'YMD'
            elif self._autoformat.find('YMMMD') != -1: self._datestyle = 'YMD'
            elif self._autoformat.find('DMMY')  != -1: self._datestyle = 'DMY'
            elif self._autoformat.find('DMMMY') != -1: self._datestyle = 'DMY'


        if self.controlInitialized:
            # Then the base control is available for configuration;
            # take action on base control based on new settings, as appropriate.
            if kwargs.has_key('useFixedWidthFont'):
                # Set control font - fixed width by default
                self._setFont()

            if reset_args.has_key('reset_mask') or not self._GetValue().strip():
                self._SetInitialValue()

            if self._autofit:
                self.SetClientSize(self._calcSize())

            # Set value/type-specific formatting
            self._applyFormatting()
        dbg(indent=0)


    def GetMaskParameter(self, paramname):
        """
        Routine for retrieving the value of any given parameter
        """
        if wxMaskedEditMixin.valid_ctrl_params.has_key(paramname):
            return getattr(self, '_' + paramname)
        elif Field.valid_params.has_key(paramname):
            return self._ctrl_constraints._GetParameter(paramname)
        else:
            TypeError('%s.GetMaskParameter: invalid parameter "%s"' % (self.name, paramname))


    def SetFieldParameters(self, field_index, **kwargs):
        """
        Routine provided to modify the parameters of a given field.
        Because changes to fields can affect the overall control,
        direct access to the fields is prevented, and the control
        is always "reconfigured" after setting a field parameter.
        """
        if field_index not in self._field_indices:
            raise IndexError('%s: %s is not a valid field for this control.' % (self.name, str(field_index)))
        # set parameters as requested:
        self._fields[field_index]._SetParameters(**kwargs)

        # Possibly reprogram control template due to resulting changes, and ensure
        # control-level params are still propagated to fields:
        self._configure(self._previous_mask)

        if self.controlInitialized:
            if kwargs.has_key('fillChar') or kwargs.has_key('defaultValue'):
                self._SetInitialValue()

                if self._autofit:
                    self.SetClientSize(self._calcSize())

            # Set value/type-specific formatting
            self._applyFormatting()


    def GetFieldParameter(self, field_index, paramname):
        """
        Routine provided for getting a parameter of an individual field.
        """
        if field_index not in self._field_indices:
            raise IndexError('%s: %s is not a valid field for this control.' % (self.name, str(field_index)))
        elif Field.valid_params.has_key(paramname):
            return self._fields[field_index]._GetParameter(paramname)
        else:
            TypeError('%s.GetMaskParameter: invalid parameter "%s"' % (self.name, paramname))


    def _SetKeycodeHandler(self, keycode, func):
        """
        This function adds and/or replaces key event handling functions
        used by the control.  <func> should take the event as argument
        and return False if no further action on the key is necessary.
        """
        self._keyhandlers[keycode] = func


    def _SetKeyHandler(self, char, func):
        """
        This function adds and/or replaces key event handling functions
        for ascii characters.  <func> should take the event as argument
        and return False if no further action on the key is necessary.
        """
        self._SetKeycodeHandler(ord(char), func)


    def _AddNavKeycode(self, keycode, handler=None):
        """
        This function allows a derived subclass to augment the list of
        keycodes that are considered "navigational" keys.
        """
        self._nav.append(keycode)
        if handler:
            self._keyhandlers[keycode] = handler


    def _AddNavKey(self, char, handler=None):
        """
        This function is a convenience function so you don't have to
        remember to call ord() for ascii chars to be used for navigation.
        """
        self._AddNavKeycode(ord(char), handler)


    def _GetNavKeycodes(self):
        """
        This function retrieves the current list of navigational keycodes for
        the control.
        """
        return self._nav


    def _SetNavKeycodes(self, keycode_func_tuples):
        """
        This function allows you to replace the current list of keycode processed
        as navigation keys, and bind associated optional keyhandlers.
        """
        self._nav = []
        for keycode, func in keycode_func_tuples:
            self._nav.append(keycode)
            if func:
                self._keyhandlers[keycode] = func


    def _processMask(self, mask):
        """
        This subroutine expands {n} syntax in mask strings, and looks for escaped
        special characters and returns the expanded mask, and an dictionary
        of booleans indicating whether or not a given position in the mask is
        a mask character or not.
        """
        dbg('_processMask: mask', mask, indent=1)
        # regular expression for parsing c{n} syntax:
        rex = re.compile('([' +string.join(maskchars,"") + '])\{(\d+)\}')
        s = mask
        match = rex.search(s)
        while match:    # found an(other) occurrence
            maskchr = s[match.start(1):match.end(1)]            # char to be repeated
            repcount = int(s[match.start(2):match.end(2)])      # the number of times
            replacement = string.join( maskchr * repcount, "")  # the resulting substr
            s = s[:match.start(1)] + replacement + s[match.end(2)+1:]   #account for trailing '}'
            match = rex.search(s)                               # look for another such entry in mask

        self._isDec      = isDecimal(s) and not self._ctrl_constraints._validRegex
        self._isInt      = isInteger(s) and not self._ctrl_constraints._validRegex
        self._signOk     = '-' in self._ctrl_constraints._formatcodes and (self._isDec or self._isInt)
##        dbg('isDecimal(%s)?' % s, isDecimal(s), 'ctrl regex:', self._ctrl_constraints._validRegex)

        if self._signOk and s[0] != ' ':
            s = ' ' + s
            if self._ctrl_constraints._defaultValue and self._ctrl_constraints._defaultValue[0] != ' ':
                self._ctrl_constraints._defaultValue = ' ' + self._ctrl_constraints._defaultValue

        # Now, go build up a dictionary of booleans, indexed by position,
        # indicating whether or not a given position is masked or not
        ismasked = {}
        i = 0
        while i < len(s):
            if s[i] == '\\':            # if escaped character:
                ismasked[i] = False     #     mark position as not a mask char
                if i+1 < len(s):        #     if another char follows...
                    s = s[:i] + s[i+1:] #         elide the '\'
                    if i+2 < len(s) and s[i+1] == '\\':
                        # if next char also a '\', char is a literal '\'
                        s = s[:i] + s[i+1:]     # elide the 2nd '\' as well
            else:                       # else if special char, mark position accordingly
                ismasked[i] = s[i] in maskchars
##            dbg('ismasked[%d]:' % i, ismasked[i], s)
            i += 1                      # increment to next char
##        dbg('ismasked:', ismasked)
        dbg(indent=0)
        return s, ismasked


    def _calcFieldExtents(self):
        """
        Subroutine responsible for establishing/configuring field instances with
        indices and editable extents appropriate to the specified mask, and building
        the lookup table mapping each position to the corresponding field.
        """
        if self._mask:

            ## Create dictionary of positions,characters in mask
            self.maskdict = {}
            for charnum in range( len( self._mask)):
                self.maskdict[charnum] = self._mask[charnum:charnum+1]

            # For the current mask, create an ordered list of field extents
            # and a dictionary of positions that map to field indices:
            self._lookupField = {}

            if self._signOk: start = 1
            else: start = 0

            if self._isDec:
                # Skip field "discovery", and just construct a 2-field control with appropriate
                # constraints for a floating-point entry.

                # .setdefault always constructs 2nd argument even if not needed, so we do this
                # the old-fashioned way...
                if not self._fields.has_key(0):
                    self._fields[0] = Field()
                if not self._fields.has_key(1):
                    self._fields[1] = Field()


                self._decimalpos = string.find( self._mask, '.')
                dbg('decimal pos =', self._decimalpos)

                formatcodes = self._fields[0]._GetParameter('formatcodes')
                if 'R' not in formatcodes: formatcodes += 'R'
                self._fields[0]._SetParameters(index=0, extent=(start, self._decimalpos),
                                               mask=self._mask[start:self._decimalpos], formatcodes=formatcodes)

                self._fields[1]._SetParameters(index=1, extent=(self._decimalpos+1, len(self._mask)),
                                               mask=self._mask[self._decimalpos+1:len(self._mask)])

                for i in range(self._decimalpos+1):
                    self._lookupField[i] = 0

                for i in range(self._decimalpos+1, len(self._mask)+1):
                    self._lookupField[i] = 1

            elif self._isInt:
                # Skip field "discovery", and just construct a 1-field control with appropriate
                # constraints for a integer entry.
                if not self._fields.has_key(0):
                    self._fields[0] = Field(index=0)
                self._fields[0]._SetParameters(extent=(start, len(self._mask)),
                                               mask=self._mask[start:len(self._mask)])

                for i in range(len(self._mask)+1):
                    self._lookupField[i] = 0
            else:
                # generic control; parse mask to figure out where the fields are:
                field_index = 0
                pos = 0
                i = self._findNextEntry(pos,adjustInsert=False)  # go to 1st entry point:
                if i < len(self._mask):   # no editable chars!
                    for j in range(pos, i+1):
                        self._lookupField[j] = field_index
                    pos = i       # figure out field for 1st editable space:

                while i <= len(self._mask):
##                    dbg('searching: outer field loop: i = ', i)
                    if self._isMaskChar(i):
##                        dbg('1st char is mask char; recording edit_start=', i)
                        edit_start = i
                        # Skip to end of editable part of current field:
                        while i < len(self._mask) and self._isMaskChar(i):
                            self._lookupField[i] = field_index
                            i += 1
##                        dbg('edit_end =', i)
                        edit_end = i
                        self._lookupField[i] = field_index
##                        dbg('self._fields.has_key(%d)?' % field_index, self._fields.has_key(field_index))
                        if not self._fields.has_key(field_index):
                            self._fields[field_index] = Field()
                        self._fields[field_index]._SetParameters(
                                                                index=field_index,
                                                                extent=(edit_start, edit_end),
                                                                mask=self._mask[edit_start:edit_end])
                    pos = i
                    i = self._findNextEntry(pos, adjustInsert=False)  # go to next field:
                    if i > pos:
                        for j in range(pos, i+1):
                            self._lookupField[j] = field_index
                    if i >= len(self._mask):
                        break           # if past end, we're done
                    else:
                        field_index += 1
##                        dbg('next field:', field_index)

        indices = self._fields.keys()
        indices.sort()
        self._field_indices = indices[1:]
##        dbg('lookupField map:', indent=1)
##        for i in range(len(self._mask)):
##            dbg('pos %d:' % i, self._lookupField[i])
##        dbg(indent=0)

        # Verify that all field indices specified are valid for mask:
        for index in self._fields.keys():
            if index not in [-1] + self._lookupField.values():
                dbg(indent=0)
                raise IndexError('field %d is not a valid field for mask "%s"' % (index, self._mask))


    def _calcTemplate(self, reset_fillchar, reset_default):
        """
        Subroutine for processing current fillchars and default values for
        whole control and individual fields, constructing the resulting
        overall template, and adjusting the current value as necessary.
        """
        default_set = False
        if self._ctrl_constraints._defaultValue:
            default_set = True
        else:
            for field in self._fields.values():
                if field._defaultValue and not reset_default:
                    default_set = True
        dbg('default set?', default_set)

        # Determine overall new template for control, and keep track of previous
        # values, so that current control value can be modified as appropriate:
        if self.controlInitialized: curvalue = list(self._GetValue())
        else:                       curvalue = None

        if hasattr(self, '_fillChar'): old_fillchars = self._fillChar
        else:                          old_fillchars = None

        if hasattr(self, '_template'): old_template = self._template
        else:                          old_template = None

        self._template = ""

        self._fillChar = {}
        reset_value = False

        for field in self._fields.values():
            field._template = ""

        for pos in range(len(self._mask)):
##            dbg('pos:', pos)
            field = self._FindField(pos)
##            dbg('field:', field._index)
            start, end = field._extent

            if pos == 0 and self._signOk:
                self._template = ' ' # always make 1st 1st position blank, regardless of fillchar
            elif self._isMaskChar(pos):
                if field._fillChar != self._ctrl_constraints._fillChar and not reset_fillchar:
                    fillChar = field._fillChar
                else:
                    fillChar = self._ctrl_constraints._fillChar
                self._fillChar[pos] = fillChar

                # Replace any current old fillchar with new one in current value;
                # if action required, set reset_value flag so we can take that action
                # after we're all done
                if self.controlInitialized and old_fillchars and old_fillchars.has_key(pos) and curvalue:
                    if curvalue[pos] == old_fillchars[pos] and old_fillchars[pos] != fillChar:
                        reset_value = True
                        curvalue[pos] = fillChar

                if not field._defaultValue and not self._ctrl_constraints._defaultValue:
##                    dbg('no default value')
                    self._template += fillChar
                    field._template += fillChar

                elif field._defaultValue and not reset_default:
##                    dbg('len(field._defaultValue):', len(field._defaultValue))
##                    dbg('pos-start:', pos-start)
                    if len(field._defaultValue) > pos-start:
##                        dbg('field._defaultValue[pos-start]: "%s"' % field._defaultValue[pos-start])
                        self._template += field._defaultValue[pos-start]
                        field._template += field._defaultValue[pos-start]
                    else:
##                        dbg('field default not long enough; using fillChar')
                        self._template += fillChar
                        field._template += fillChar
                else:
                    if len(self._ctrl_constraints._defaultValue) > pos:
##                        dbg('using control default')
                        self._template += self._ctrl_constraints._defaultValue[pos]
                        field._template += self._ctrl_constraints._defaultValue[pos]
                    else:
##                        dbg('ctrl default not long enough; using fillChar')
                        self._template += fillChar
                        field._template += fillChar
##                dbg('field[%d]._template now "%s"' % (field._index, field._template))
##                dbg('self._template now "%s"' % self._template)
            else:
                self._template += self._mask[pos]

        self._fields[-1]._template = self._template     # (for consistency)

        if curvalue:    # had an old value, put new one back together
            newvalue = string.join(curvalue, "")
        else:
            newvalue = None

        if default_set:
            self._defaultValue = self._template
            dbg('self._defaultValue:', self._defaultValue)
            if not self.IsEmpty(self._defaultValue) and not self.IsValid(self._defaultValue):
                dbg(indent=0)
                raise ValueError('%s: default value of "%s" is not a valid value' % (self.name, self._defaultValue))

            # if no fillchar change, but old value == old template, replace it:
            if newvalue == old_template:
                newvalue = self._template
                reset_value = true
        else:
            self._defaultValue = None

        if reset_value:
            pos = self._GetInsertionPoint()
            sel_start, sel_to = self._GetSelection()
            self._SetValue(newvalue)
            self._SetInsertionPoint(pos)
            self._SetSelection(sel_start, sel_to)


    def _propagateConstraints(self, **reset_args):
        """
        Subroutine for propagating changes to control-level constraints and
        formatting to the individual fields as appropriate.
        """
        parent_codes = self._ctrl_constraints._formatcodes
        for i in self._field_indices:
            field = self._fields[i]
            inherit_args = {}

            field_codes = current_codes = field._GetParameter('formatcodes')
            for c in parent_codes:
                if c not in field_codes: field_codes += c
            if field_codes != current_codes:
                inherit_args['formatcodes'] = field_codes

            include_chars = current_includes = field._GetParameter('includeChars')
            for c in include_chars:
                if not c in include_chars: include_chars += c
            if include_chars != current_includes:
                inherit_args['includeChars'] = include_chars

            exclude_chars = current_excludes = field._GetParameter('excludeChars')
            for c in exclude_chars:
                if not c in exclude_chars: exclude_chars += c
            if exclude_chars != current_excludes:
                inherit_args['excludeChars'] = exclude_chars

            if reset_args.has_key('defaultValue') and reset_args['defaultValue']:
                inherit_args['defaultValue'] = ""   # (reset for field)

            for param in ['fillChar', 'compareNoCase', 'emptyInvalid', 'validRequired']:
                if reset_args.has_key(param) and reset_args[param]:
                    inherit_args[param] = self.GetMaskParameter(param)

            if inherit_args:
                field._SetParameters(**inherit_args)


    def _validateChoices(self):
        """
        Subroutine that validates that all choices for given fields are at
        least of the necessary length, and that they all would be valid pastes
        if pasted into their respective fields.
        """
        for field in self._fields.values():
            if field._choices:
                index = field._index
##                dbg('checking for choices for field', field._index)
                start, end = field._extent
                field_length = end - start
##                dbg('start, end, length:', start, end, field_length)

                for choice in field._choices:
                    valid_paste, ignore, replace_to = self._validatePaste(choice, start, end)
                    if not valid_paste:
                        dbg(indent=0)
                        raise ValueError('%s: "%s" could not be entered into field %d' % (self.name, choice, index))
                    elif replace_to > end:
                        dbg(indent=0)
                        raise ValueError('%s: "%s" will not fit into field %d' (self.name, choice, index))
##                    dbg(choice, 'valid in field', index)


    def _configure(self, mask, **reset_args):
        """
        This function sets flags for automatic styling options.  It is
        called whenever a control or field-level parameter is set/changed.

        This routine does the bulk of the interdependent parameter processing, determining
        the field extents of the mask if changed, resetting parameters as appropriate,
        determining the overall template value for the control, etc.

        reset_args is supplied if called from control's .SetMaskParameters()
        routine, and indicates which if any parameters which can be
        overridden by individual fields have been reset by request for the
        whole control.
        """
        dbg('wxMaskedEditMixin::_configure("%s")' % mask, indent=1)

        # Preprocess specified mask to expand {n} syntax, handle escaped
        # mask characters, etc and build the resulting positionally keyed
        # dictionary for which positions are mask vs. template characters:
        self._mask, self.ismasked = self._processMask(mask)
        dbg('processed mask:', self._mask)

        # Preserve original mask specified, for subsequent reprocessing
        # if parameters change.
        self._previous_mask = mask


        # Set extent of field -1 to width of entire control:
        self._ctrl_constraints._SetParameters(extent=(0,len(self._mask)))

        # Go parse mask to determine where each field is, construct field
        # instances as necessary, configure them with those extents, and
        # build lookup table mapping each position for control to its corresponding
        # field.
        self._calcFieldExtents()

        # Go process defaultValues and fillchars to construct the overall
        # template, and adjust the current value as necessary:
        reset_fillchar = reset_args.has_key('fillChar') and reset_args['fillChar']
        reset_default = reset_args.has_key('defaultValue') and reset_args['defaultValue']

        self._emptyInvalid = False
        for field in self._fields.values():
            if field._emptyInvalid:
                self._emptyInvalid = True   # if any field is required to be filled, color appropriately

        self._calcTemplate(reset_fillchar, reset_default)

        # Propagate control-level formatting and character constraints to each
        # field if they don't already have them:
        self._propagateConstraints(**reset_args)

        # Validate that all choices for given fields are at least of the
        # necessary length, and that they all would be valid pastes if pasted
        # into their respective fields:
        self._validateChoices()

        dbg('fields:', indent=1)
        for i in [-1] + self._field_indices:
            dbg('field %d:' % i, self._fields[i].__dict__)
        dbg(indent=0)

        # Set up special parameters for numeric control, if appropriate:
        if self._signOk:
            self._signpos = 0   # assume it starts here, but it will move around on floats
            self._SetKeyHandler('-', self._OnChangeSign)
            self._SetKeyHandler('+', self._OnChangeSign)
            self._SetKeyHandler(' ', self._OnChangeSign)

        if self._isDec or self._isInt:
            dbg('Registering numeric navigation and control handlers')

            # Replace up/down arrow default handling:
            # make down act like tab, up act like shift-tab:
            self._SetKeycodeHandler(WXK_DOWN, self._OnChangeField)
            self._SetKeycodeHandler(WXK_UP, self._OnUpNumeric)  # (adds "shift" to up arrow, and calls _OnChangeField)

            # On ., truncate contents right of cursor to decimal point (if any)
            # leaves cusor after decimal point if dec, otherwise at 0.
            self._SetKeyHandler('.', self._OnDecimalPoint)
            self._SetKeyHandler('>', self._OnChangeField)   # (Shift-'.')

            # Allow selective insert of commas in numbers:
            self._SetKeyHandler(',', self._OnGroupChar)

        dbg(indent=0)


    def _SetInitialValue(self, value=""):
        """
        fills the control with the generated or supplied default value.
        It will also set/reset the font if necessary and apply
        formatting to the control at this time.
        """
        dbg('wxMaskedEditMixin::_SetInitialValue("%s")' % value, indent=1)
        if not value:
            self._SetValue( self._template )
        else:
            # Apply validation as appropriate to passed value
            self.SetValue(value)

        # Set value/type-specific formatting
        self._applyFormatting()
        dbg(indent=0)


    def _calcSize(self, size=None):
        """ Calculate automatic size if allowed; must be called after the base control is instantiated"""
##        dbg('wxMaskedEditMixin::_calcSize', indent=1)
        cont = (size is None or size == wxDefaultSize)

        if cont and self._autofit:
            sizing_text = 'M' * len(self._mask)
            if wxPlatform != "__WXMSW__":   # give it a little extra space
                sizing_text += 'M'
            if wxPlatform == "__WXMAC__":   # give it even a little more...
                sizing_text += 'M'
##            dbg('len(sizing_text):', len(sizing_text), 'sizing_text: "%s"' % sizing_text)
            w, h = self.GetTextExtent(sizing_text)
            size = (w+4, self.GetClientSize().height)
##            dbg('size:', size, indent=0)
        return size


    def _setFont(self):
        """ Set the control's font typeface -- pass the font name as str."""
##        dbg('wxMaskedEditMixin::_setFont', indent=1)
        if not self._useFixedWidthFont:
            self._font = wxSystemSettings_GetFont(wxSYS_DEFAULT_GUI_FONT)
        else:
            font = self.GetFont()   # get size, weight, etc from current font

            # Set to teletype font (guaranteed to be mappable to all wxWindows
            # platforms:
            self._font = wxFont( font.GetPointSize(), wxTELETYPE, font.GetStyle(),
                                 font.GetWeight(), font.GetUnderlined())
##            dbg('font string: "%s"' % font.GetNativeFontInfo().ToString())

        self.SetFont(self._font)
##        dbg(indent=0)


    def _OnTextChange(self, event):
        """
        Handler for EVT_TEXT event.
        self._Change() is provided for subclasses, and may return False to
        skip this method logic.  This function returns True if the event
        detected was a legitimate event, or False if it was a "bogus"
        EVT_TEXT event.  (NOTE: There is currently an issue with calling
        .SetValue from within the EVT_CHAR handler that causes duplicate
        EVT_TEXT events for the same change.)
        """
        newvalue = self._GetValue()
        dbg('wxMaskedEditMixin::_OnTextChange: value: "%s"' % newvalue, indent=1)
        bValid = False
        if self._ignoreChange:      # ie. if an "intermediate text change event"
            dbg(indent=0)
            return bValid

        ##! WS: For some inexplicable reason, every wxTextCtrl.SetValue
        ## call is generating two (2) EVT_TEXT events.
        ## This is the only mechanism I can find to mask this problem:
        if newvalue == self._oldvalue:
            dbg('ignoring bogus text change event', indent=0)
        else:
            dbg('oldvalue: "%s", newvalue: "%s"' % (self._oldvalue, newvalue))
            if self._Change():
                if self._signOk and self._isNeg and newvalue.find('-') == -1:
                    self._isNeg = False
                    text, self._signpos = self._getSignedValue()
                self._CheckValid()  # Recolor control as appropriate
            event.Skip()
            bValid = True
        self._oldvalue = newvalue   # Save last seen value for next iteration
        dbg(indent=0)
        return bValid


    def _OnKeyDown(self, event):
        """
        This function allows the control to capture Ctrl-events like Ctrl-tab,
        that are not normally seen by the "cooked" EVT_CHAR routine.
        """
        # Get keypress value, adjusted by control options (e.g. convert to upper etc)
        key    = event.GetKeyCode()
        if key in self._nav and event.ControlDown():
            # then this is the only place we will likely see these events;
            # process them now:
            dbg('wxMaskedEditMixin::OnKeyDown: calling _OnChar')
            self._OnChar(event)
            return
        # else allow regular EVT_CHAR key processing
        event.Skip()


    def _OnChar(self, event):
        """
        This is the engine of wxMaskedEdit controls.  It examines each keystroke,
        decides if it's allowed, where it should go or what action to take.
        """
        dbg('wxMaskedEditMixin::_OnChar', indent=1)

        # Get keypress value, adjusted by control options (e.g. convert to upper etc)
        key = event.GetKeyCode()
        orig_pos = self._GetInsertionPoint()
        orig_value = self._GetValue()
        dbg('keycode = ', key)
        dbg('current pos = ', orig_pos)
        dbg('current selection = ', self._GetSelection())

        if not self._Keypress(key):
            dbg(indent=0)
            return

        # If no format string for this control, or the control is marked as "read-only",
        # skip the rest of the special processing, and just "do the standard thing:"
        if not self._mask or not self._IsEditable():
            event.Skip()
            dbg(indent=0)
            return

        # Process navigation and control keys first, with
        # position/selection unadulterated:
        if key in self._nav + self._control:
            if self._keyhandlers.has_key(key):
                keep_processing = self._keyhandlers[key](event)
                if self._GetValue() != orig_value:
                    self.modified = True
                if not keep_processing:
                    dbg(indent=0)
                    return
                self._applyFormatting()
                dbg(indent=0)
                return

        # Else... adjust the position as necessary for next input key,
        # and determine resulting selection:
        pos = self._adjustPos( orig_pos, key )    ## get insertion position, adjusted as needed
        sel_start, sel_to = self._GetSelection()                ## check for a range of selected text
        dbg("pos, sel_start, sel_to:", pos, sel_start, sel_to)

        keep_processing = True
        # Capture user past end of format field
        if pos > len(self.maskdict):
            dbg("field length exceeded:",pos)
            keep_processing = False

        if keep_processing:
            if self._isMaskChar(pos):  ## Get string of allowed characters for validation
                okchars = self._getAllowedChars(pos)
            else:
                dbg('Not a valid position: pos = ', pos,"chars=",maskchars)
                okchars = ""

        key = self._adjustKey(pos, key)     # apply formatting constraints to key:

        if self._keyhandlers.has_key(key):
            # there's an override for default behavior; use override function instead
            dbg('using supplied key handler:', self._keyhandlers[key])
            keep_processing = self._keyhandlers[key](event)
            if self._GetValue() != orig_value:
                self.modified = True
            if not keep_processing:
                dbg(indent=0)
                return
            # else skip default processing, but do final formatting
        if key < WXK_SPACE or key > 255:
            dbg('key < WXK_SPACE or key > 255')
            event.Skip()                # non alphanumeric
            keep_processing = False
        else:
            field = self._FindField(pos)
            dbg("key ='%s'" % chr(key))
            if chr(key) == ' ':
                dbg('okSpaces?', field._okSpaces)

            if chr(key) in field._excludeChars + self._ctrl_constraints._excludeChars:
                keep_processing = False
                if (not wxValidator_IsSilent()) and orig_pos == pos:
                    wxBell()

            if keep_processing and self._isCharAllowed( chr(key), pos, checkRegex = True ):
                dbg("key allowed by mask")
                # insert key into candidate new value, but don't change control yet:
                oldstr = self._GetValue()
                newstr, newpos = self._insertKey(chr(key), pos, sel_start, sel_to, self._GetValue())
                dbg("str with '%s' inserted:" % chr(key), '"%s"' % newstr)
                if self._ctrl_constraints._validRequired and not self.IsValid(newstr):
                    dbg('not valid; checking to see if adjusted string is:')
                    keep_processing = False
                    if self._isDec and newstr != self._template:
                        newstr = self._adjustDec(newstr)
                        dbg('adjusted str:', newstr)
                        if self.IsValid(newstr):
                            dbg("it is!")
                            keep_processing = True
                            wxCallAfter(self._SetInsertionPoint, self._decimalpos)
                    if not keep_processing:
                        dbg("key disallowed by validation")
                        if not wxValidator_IsSilent() and orig_pos == pos:
                            wxBell()

                if keep_processing:
                    unadjusted = newstr

                    # special case: adjust date value as necessary:
                    if self._isDate and newstr != self._template:
                        newstr = self._adjustDate(newstr)
                    dbg('adjusted newstr:', newstr)

                    if newstr != orig_value:
                        self.modified = True

                    wxCallAfter(self._SetValue, newstr)

                    # Adjust insertion point on date if just entered 2 digit year, and there are now 4 digits:
                    if not self.IsDefault() and self._isDate and self._4digityear:
                        year2dig = self._dateExtent - 2
                        if pos == year2dig and unadjusted[year2dig] != newstr[year2dig]:
                            newpos = pos+2

                    wxCallAfter(self._SetInsertionPoint, newpos)
                    newfield = self._FindField(newpos)
                    if newfield != field and newfield._selectOnFieldEntry:
                        wxCallAfter(self._SetSelection, newfield._extent[0], newfield._extent[1])
                    keep_processing = false
            else:
                dbg('char not allowed; orig_pos == pos?', orig_pos == pos)
                keep_processing = False
                if (not wxValidator_IsSilent()) and orig_pos == pos:
                    wxBell()

        self._applyFormatting()

        # Move to next insertion point
        if keep_processing and key not in self._nav:
            pos = self._GetInsertionPoint()
            next_entry = self._findNextEntry( pos )
            if pos != next_entry:
                dbg("moving from %(pos)d to next valid entry: %(next_entry)d" % locals())
                wxCallAfter(self._SetInsertionPoint, next_entry )

            if self._isTemplateChar(pos):
                self._AdjustField(pos)
        dbg(indent=0)


    def _FindFieldExtent(self, pos=None, getslice=False, value=None):
        """ returns editable extent of field corresponding to
        position pos, and, optionally, the contents of that field
        in the control or the value specified.
        Template chars are bound to the preceding field.
        For masks beginning with template chars, these chars are ignored
        when calculating the current field.

        Eg: with template (###) ###-####,
        >>> self._FindFieldExtent(pos=0)
        1, 4
        >>> self._FindFieldExtent(pos=1)
        1, 4
        >>> self._FindFieldExtent(pos=5)
        1, 4
        >>> self._FindFieldExtent(pos=6)
        6, 9
        >>> self._FindFieldExtent(pos=10)
        10, 14
        etc.
        """
        dbg('wxMaskedEditMixin::_FindFieldExtent(pos=%s, getslice=%s)' % (
                str(pos), str(getslice)) ,indent=1)

        field = self._FindField(pos)
        if not field:
            if getslice:
                return None, None, ""
            else:
                return None, None
        edit_start, edit_end = field._extent
        if getslice:
            if value is None: value = self._GetValue()
            slice = value[edit_start:edit_end]
            dbg('edit_start:', edit_start, 'edit_end:', edit_end, 'slice: "%s"' % slice)
            dbg(indent=0)
            return edit_start, edit_end, slice
        else:
            dbg('edit_start:', edit_start, 'edit_end:', edit_end)
            dbg(indent=0)
            return edit_start, edit_end


    def _FindField(self, pos=None):
        """
        Returns the field instance in which pos resides.
        Template chars are bound to the preceding field.
        For masks beginning with template chars, these chars are ignored
        when calculating the current field.

        """
##        dbg('wxMaskedEditMixin::_FindField(pos=%s)' % str(pos) ,indent=1)
        if pos is None: pos = self._GetInsertionPoint()
        elif pos < 0 or pos > len(self._mask):
            raise IndexError('position %s out of range of control' % str(pos))

        if len(self._fields) == 0:
            dbg(indent=0)
            return None

        # else...
##        dbg(indent=0)
        return self._fields[self._lookupField[pos]]


    def ClearValue(self):
        """ Blanks the current control value by replacing it with the default value."""
        dbg("wxMaskedEditMixin::ClearValue - value reset to default value (template)")
        self._SetValue( self._template )
        self._SetInsertionPoint(0)
        self.Refresh()


    def _baseCtrlEventHandler(self, event):
        """
        This function is used whenever a key should be handled by the base control.
        """
        event.Skip()
        return False


    def _OnUpNumeric(self, event):
        """
        Makes up-arrow act like shift-tab should; ie. take you to start of
        previous field.
        """
        dbg('wxMaskedEditMixin::_OnUpNumeric', indent=1)
        event.m_shiftDown = 1
        dbg('event.ShiftDown()?', event.ShiftDown())
        self._OnChangeField(event)
        dbg(indent=0)


    def _OnArrow(self, event):
        """
        Used in response to left/right navigation keys; makes these actions skip
        over mask template chars.
        """
        dbg("wxMaskedEditMixin::_OnArrow", indent=1)
        pos = self._GetInsertionPoint()
        keycode = event.GetKeyCode()
        sel_start, sel_to = self._GetSelection()
        entry_end = self._goEnd(getPosOnly=True)
        if keycode in (WXK_RIGHT, WXK_DOWN):
            if( ( not self._isTemplateChar(pos) and pos+1 > entry_end)
                or ( self._isTemplateChar(pos) and pos >= entry_end) ):
                dbg(indent=0)
                return False
            elif self._isTemplateChar(pos):
                self._AdjustField(pos)
        elif keycode in (WXK_LEFT,WXK_UP) and pos > 0 and self._isTemplateChar(pos-1):
            dbg('adjusting field')
            self._AdjustField(pos)

        # treat as shifted up/down arrows as tab/reverse tab:
        if event.ShiftDown() and keycode in (WXK_UP, WXK_DOWN):
            # remove "shifting" and treat as (forward) tab:
            event.m_shiftDown = False
            keep_processing = self._OnChangeField(event)

        elif self._FindField(pos)._selectOnFieldEntry:
            if( keycode in (WXK_UP, WXK_LEFT)
                and sel_start != 0
                and self._isTemplateChar(sel_start-1)):

                # call _OnChangeField to handle "ctrl-shifted event"
                # (which moves to previous field and selects it.)
                event.m_shiftDown = True
                event.m_ControlDown = True
                keep_processing = self._OnChangeField(event)
            elif( keycode in (WXK_DOWN, WXK_RIGHT)
                  and sel_to != len(self._mask)
                  and self._isTemplateChar(sel_to)):

                # when changing field to the right, ensure don't accidentally go left instead
                event.m_shiftDown = False
                keep_processing = self._OnChangeField(event)
            else:
                # treat arrows as normal, allowing selection
                # as appropriate:
                event.Skip()
        else:
            if( (sel_to == self._fields[0]._extent[0] and keycode == WXK_LEFT)
                or (sel_to == len(self._mask) and keycode == WXK_RIGHT) ):
                if not wxValidator_IsSilent():
                    wxBell()
            else:
                # treat arrows as normal, allowing selection
                # as appropriate:
                dbg('skipping event')
                event.Skip()

        keep_processing = False
        dbg(indent=0)
        return keep_processing


    def _OnCtrl_S(self, event):
        """ Default Ctrl-S handler; prints value information if demo enabled. """
        dbg("wxMaskedEditMixin::_OnCtrl_S")
        if self._demo:
            print 'wxMaskedEditMixin.GetValue()       = "%s"\nwxMaskedEditMixin.GetPlainValue() = "%s"' % (self.GetValue(), self.GetPlainValue())
            print "Valid? => " + str(self.IsValid())
            print "Current field, start, end, value =", str( self._FindFieldExtent(getslice=True))
        return False


    def _OnCtrl_X(self, event):
        """ Handles ctrl-x keypress in control. Should return False to skip other processing. """
        dbg("wxMaskedEditMixin::_OnCtrl_X", indent=1)
        self._Cut()
        dbg(indent=0)
        return False


    def _OnCtrl_V(self, event):
        """ Handles ctrl-V keypress in control. Should return False to skip other processing. """
        dbg("wxMaskedEditMixin::_OnCtrl_V", indent=1)
        self._Paste()
        dbg(indent=0)
        return False


    def _OnCtrl_A(self,event):
        """ Handles ctrl-a keypress in control. Should return False to skip other processing. """
        end = self._goEnd(getPosOnly=True)
        if event.ShiftDown():
            wxCallAfter(self._SetInsertionPoint, 0)
            wxCallAfter(self._SetSelection, 0, len(self._mask))
        else:
            wxCallAfter(self._SetInsertionPoint, 0)
            wxCallAfter(self._SetSelection, 0, end)
        return False


    def _OnErase(self,event):
        """ Handles backspace and delete keypress in control. Should return False to skip other processing."""
        dbg("wxMaskedEditMixin::_OnErase", indent=1)
        sel_start, sel_to = self._GetSelection()                   ## check for a range of selected text
        key = event.GetKeyCode()
        if( ((sel_to == 0 or sel_to == self._fields[0]._extent[0]) and key == WXK_BACK)
            or (sel_to == len(self._mask) and key == WXK_DELETE)):
            if not wxValidator_IsSilent():
                wxBell()
            dbg(indent=0)
            return False

        field = self._FindField(sel_to)
        start, end = field._extent
        value = self._GetValue()

        if( field._insertRight
            and key == WXK_BACK
            and sel_start >= start and sel_to == end            # within field
            and value[start:end] != self._template[start:end]): # and field not empty
            dbg('delete left')
            dbg('sel_start, start:', sel_start, start)
            # special case: backspace at the end of a right insert field shifts contents right to cursor

            if sel_start == end:    # select "last char in field"
                sel_start -= 1

            newfield = value[start:sel_start]
            dbg('cut newfield: "%s"' % newfield)
            left = ""
            for i in range(start, end - len(newfield)):
                if field._padZero:
                    left += '0'
                elif self._signOk and self._isNeg and newfield.find('-') == -1 and i == 1:
                    left += '-'
                else:
                    left += self._template[i]   # this can produce strange results in combination with default values...
            newfield = left + newfield
            dbg('filled newfield: "%s"' % newfield)
            newstr = value[:start] + newfield + value[end:]
            if self._signOk and self._isNeg and newstr[0] == '-':
                newstr = ' ' + newstr[1:]
            pos = end
        else:
            if sel_start == sel_to:
                dbg("current sel_start, sel_to:", sel_start, sel_to)
                if key == WXK_BACK:
                    sel_start, sel_to = sel_to-1, sel_to-1
                    dbg("new sel_start, sel_to:", sel_start, sel_to)
                if field._padZero and not value[start:sel_to].replace('0', '').replace(' ','').replace(field._fillChar, ''):
                    # preceding chars (if any) are zeros, blanks or fillchar; new char should be 0:
                    newchar = '0'
                else:
                    newchar = self._template[sel_to] ## get an original template character to "clear" the current char
                dbg('value = "%s"' % value, 'value[%d] = "%s"' %(sel_start, value[sel_start]))

                if self._isTemplateChar(sel_to):
                    newstr = value
                    newpos = sel_to
                else:
                    newstr, newpos = self._insertKey(newchar, sel_to, sel_start, sel_to, value)

            else:
                newstr = value
                newpos = sel_start
                for i in range(sel_start, sel_to):
                    pos = i
                    newchar = self._template[pos] ## get an original template character to "clear" the current char

                    if not self._isTemplateChar(pos):
                        newstr, newpos = self._insertKey(newchar, pos, sel_start, sel_to, newstr)

            pos = sel_start  # put cursor back at beginning of selection
        dbg('newstr:', newstr)

        # if erasure results in an invalid field, disallow it:
        dbg('field._validRequired?', field._validRequired)
        dbg('field.IsValid("%s")?' % newstr[start:end], field.IsValid(newstr[start:end]))
        if field._validRequired and not field.IsValid(newstr[start:end]):
            if not wxValidator_IsSilent():
                wxBell()
            dbg(indent=0)
            return False

        # if erasure results in an invalid value, disallow it:
        if self._ctrl_constraints._validRequired and not self.IsValid(newstr):
            if not wxValidator_IsSilent():
                wxBell()
            dbg(indent=0)
            return False

        dbg('setting value (later) to', newstr)
        wxCallAfter(self._SetValue, newstr)
        dbg('setting insertion point (later) to', pos)
        wxCallAfter(self._SetInsertionPoint, pos)
        dbg(indent=0)
        return False


    def _OnEnd(self,event):
        """ Handles End keypress in control. Should return False to skip other processing. """
        dbg("wxMaskedEditMixin::_OnEnd", indent=1)
        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
        end = self._goEnd(getPosOnly=True)

        if event.ShiftDown():
            dbg("shift-end; select to end of non-whitespace")
            wxCallAfter(self._SetInsertionPoint, pos)
            wxCallAfter(self._SetSelection, pos, end)
        elif event.ControlDown():
            dbg("control-end; select to end of control")
            wxCallAfter(self._SetInsertionPoint, pos)
            wxCallAfter(self._SetSelection, pos, len(self._mask))
        else:
            onChar = self._goEnd()
        dbg(indent=0)
        return False


    def _OnReturn(self, event):
         """
         Changes the event to look like a tab event, so we can then call
         event.Skip() on it, and have the parent form "do the right thing."
         """
         event.m_keyCode = WXK_TAB
         event.Skip()


    def _OnHome(self,event):
        """ Handles Home keypress in control. Should return False to skip other processing."""
        dbg("wxMaskedEditMixin::_OnHome", indent=1)
        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
        sel_start, sel_to = self._GetSelection()

        if event.ShiftDown():
            dbg("shift-home; select to beginning of non-whitespace")
            if sel_to > pos:
                pos = sel_to
            wxCallAfter(self._SetInsertionPoint, pos)
            wxCallAfter(self._SetSelection, 0, pos)
        else:
            self._goHome()
        dbg(indent=0)
        return False


    def _OnChangeField(self, event):
        """
        Primarily handles TAB events, but can be used for any key that
        designer wants to change fields within a masked edit control.
        NOTE: at the moment, although coded to handle shift-TAB and
        control-shift-TAB, these events are not sent to the controls
        by the framework.
        """
        dbg('wxMaskedEditMixin::_OnChangeField', indent = 1)
        # determine end of current field:
        pos = self._GetInsertionPoint()
        dbg('current pos:', pos)


        masklength = len(self._mask)
        if masklength < 0:   # no fields; process tab normally
            self._AdjustField(pos)
            if event.GetKeyCode() == WXK_TAB:
                dbg('tab to next ctrl')
                event.Skip()
            #else: do nothing
            dbg(indent=0)
            return False

        field = self._FindField(pos)

        if event.ShiftDown():
            # "Go backward"

            # NOTE: doesn't yet work with SHIFT-tab under wx; the control
            # never sees this event! (But I've coded for it should it ever work,
            # and it *does* work for '.' in wxIpAddrCtrl.)

            index = field._index
            begin_field = field._extent[0]
            if event.ControlDown():
                dbg('select to beginning of field:', begin_field, pos)
                wxCallAfter(self._SetInsertionPoint, begin_field)
                wxCallAfter(self._SetSelection, begin_field, pos)
                dbg(indent=0)
                return False

            elif index == 0:
                  # We're already in the 1st field; process shift-tab normally:
                self._AdjustField(pos)
                if event.GetKeyCode() == WXK_TAB:
                    dbg('tab to previous ctrl')
                    event.Skip()
                else:
                    dbg('position at beginning')
                    wxCallAfter(self._SetInsertionPoint, begin_field)
                dbg(indent=0)
                return False
            else:
                # find beginning of previous field:
                begin_prev = self._FindField(begin_field-1)._extent[0]
                self._AdjustField(pos)
                dbg('repositioning to', begin_prev)
                wxCallAfter(self._SetInsertionPoint, begin_prev)
                if self._FindField(begin_prev)._selectOnFieldEntry:
                    edit_start, edit_end = self._FindFieldExtent(begin_prev)
                    wxCallAfter(self._SetSelection, edit_start, edit_end)
                dbg(indent=0)
                return False

        else:
            # "Go forward"

            end_field = field._extent[1]
            if event.ControlDown():
                dbg('select to end of field:', pos, end_field)
                wxCallAfter(self._SetInsertionPoint, pos)
                wxCallAfter(self._SetSelection, pos, end_field)
                dbg(indent=0)
                return False
            else:
                dbg('end of current field:', end_field)
                dbg('go to next field')
                if end_field == self._fields[self._field_indices[-1]]._extent[1]:
                    self._AdjustField(pos)
                    if event.GetKeyCode() == WXK_TAB:
                        dbg('tab to next ctrl')
                        event.Skip()
                    else:
                        dbg('position at end')
                        wxCallAfter(self._SetInsertionPoint, end_field)
                    dbg(indent=0)
                    return False
                else:
                    # we have to find the start of the next field
                    next_pos = self._findNextEntry(end_field)
                    if next_pos == end_field:
                        dbg('already in last field')
                        self._AdjustField(pos)
                        if event.GetKeyCode() == WXK_TAB:
                            dbg('tab to next ctrl')
                            event.Skip()
                        #else: do nothing
                        dbg(indent=0)
                        return False
                    else:
                        self._AdjustField( pos )

                        # move cursor to appropriate point in the next field and select as necessary:
                        field = self._FindField(next_pos)
                        edit_start, edit_end = field._extent
                        if field._selectOnFieldEntry:
                            dbg('move to ', next_pos)
                            wxCallAfter(self._SetInsertionPoint, next_pos)
                            edit_start, edit_end = self._FindFieldExtent(next_pos)
                            dbg('select', edit_start, edit_end)
                            wxCallAfter(self._SetSelection, edit_start, edit_end)
                        else:
                            if field._insertRight:
                                next_pos = field._extent[1]
                            dbg('move to ', next_pos)
                            wxCallAfter(self._SetInsertionPoint, next_pos)
                        dbg(indent=0)
                        return False


    def _OnDecimalPoint(self, event):
        dbg('wxMaskedEditMixin::_OnDecimalPoint', indent=1)

        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())

        if self._isDec:       ## handle decimal value, move to decimal place
            dbg('key == Decimal tab; decimal pos:', self._decimalpos)
            value = self._GetValue()
            if pos < self._decimalpos:
                clipped_text = value[0:pos] + '.' + value[self._decimalpos+1:]
                dbg('value: "%s"' % self._GetValue(), 'clipped_text:', clipped_text)
                newstr = self._adjustDec(clipped_text)
            else:
                newstr = self._adjustDec(value)
            wxCallAfter(self._SetValue, newstr)
            wxCallAfter(self._SetInsertionPoint, self._decimalpos+1)
            keep_processing = False

        if self._isInt:      ## handle integer value, truncate from current position
            dbg('key == Integer decimal event')
            value = self._GetValue()
            clipped_text = value[0:pos]
            dbg('value: "%s"' % self._GetValue(), 'clipped_text:', clipped_text)
            newstr = self._adjustInt(clipped_text)
            dbg('newstr: "%s"' % newstr)
            wxCallAfter(self._SetValue, newstr)
            wxCallAfter(self._SetInsertionPoint, len(newstr.rstrip()))
            keep_processing = False
        dbg(indent=0)


    def _OnChangeSign(self, event):
        dbg('wxMaskedEditMixin::_OnChangeSign', indent=1)
        key = event.GetKeyCode()
        pos = self._adjustPos(self._GetInsertionPoint(), key)
        if chr(key) in ("-","+") or (chr(key) == " " and pos == self._signpos):
            cursign = self._isNeg
            dbg('cursign:', cursign)
            if chr(key) == "-":
                self._isNeg = (not self._isNeg)   ## flip value
            else:
                self._isNeg = False
            dbg('isNeg?', self._isNeg)

            text, signpos = self._getSignedValue()
            if text is not None:
                self._signpos = signpos
                dbg('self._signpos now:', self._signpos)
            else:
                text = self._GetValue()
                field = self._fields[0]
                if field._alignRight and field._fillChar == ' ':
                    self._signpos = text.find('-')
                    if self._signpos == -1:
                        if len(text.lstrip()) < len(text):
                            self._signpos = len(text) - len(text.lstrip()) - 1
                        else:
                            self._signpos = 0
                else:
                    self._signpos = 0
                dbg('self._signpos now:', self._signpos)
            if self._isNeg:
                text = text[:self._signpos] + '-' + text[self._signpos+1:]
            else:
                text = text[:self._signpos] + ' ' + text[self._signpos+1:]

            wxCallAfter(self._SetValue, text)
            wxCallAfter(self._applyFormatting)
            if pos == self._signpos:
                wxCallAfter(self._SetInsertionPoint, self._signpos+1)
            else:
                wxCallAfter(self._SetInsertionPoint, pos)

            keep_processing = False
        else:
            keep_processing = True
        dbg(indent=0)
        return keep_processing


    def _OnGroupChar(self, event):
        """
        This handler is only registered if the mask is a numeric mask.
        It allows the insertion of ',' if appropriate.
        """
        dbg('wxMaskedEditMixin::_OnGroupChar', indent=1)
        keep_processing = True
        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
        sel_start, sel_to = self._GetSelection()
        groupchar = self._fields[0]._groupchar
        if not self._isCharAllowed(groupchar, pos, checkRegex=True):
            keep_processing = False
            if not wxValidator_IsSilent():
                    wxBell()

        if keep_processing:
            newstr, newpos = self._insertKey(groupchar, pos, sel_start, sel_to, self._GetValue() )
            dbg("str with '%s' inserted:" % groupchar, '"%s"' % newstr)
            if self._ctrl_constraints._validRequired and not self.IsValid(newstr):
                keep_processing = False
                if not wxValidator_IsSilent():
                        wxBell()

        if keep_processing:
            wxCallAfter(self._SetValue, newstr)
            wxCallAfter(self._SetInsertionPoint, newpos)
        keep_processing = False
        dbg(indent=0)
        return keep_processing


    def _findNextEntry(self,pos, adjustInsert=True):
        """ Find the insertion point for the next valid entry character position."""
        if self._isTemplateChar(pos):   # if changing fields, pay attn to flag
            adjustInsert = adjustInsert
        else:                           # else within a field; flag not relevant
            adjustInsert = False

        while self._isTemplateChar(pos) and pos < len(self._mask):
            pos += 1

        # if changing fields, and we've been told to adjust insert point,
        # look at new field; if empty and right-insert field,
        # adjust to right edge:
        if adjustInsert and pos < len(self._mask):
            field = self._FindField(pos)
            start, end = field._extent
            slice = self._GetValue()[start:end]
            if field._insertRight and field.IsEmpty(slice):
                pos = end
        return pos


    def _findNextTemplateChar(self, pos):
        """ Find the position of the next non-editable character in the mask."""
        while not self._isTemplateChar(pos) and pos < len(self._mask):
            pos += 1
        return pos


    def _OnAutoCompleteField(self, event):
        dbg('wxMaskedEditMixin::_OnAutoCompleteField', indent =1)
        pos = self._GetInsertionPoint()
        field = self._FindField(pos)
        edit_start, edit_end, slice = self._FindFieldExtent(pos, getslice=True)
        match_index = None
        keycode = event.GetKeyCode()
        text = slice.replace(field._fillChar, '')
        text = text.strip()
        keep_processing = True  # (assume True to start)
        dbg('field._hasList?', field._hasList)
        if field._hasList:
            dbg('choices:', field._choices)
            choices, choice_required = field._choices, field._choiceRequired
            if keycode in (WXK_PRIOR, WXK_UP):
                direction = -1
            else:
                direction = 1
            match_index = self._autoComplete(direction, choices, text, compareNoCase=field._compareNoCase)
            if( match_index is None
                and (keycode in self._autoCompleteKeycodes + [WXK_PRIOR, WXK_NEXT]
                     or (keycode in [WXK_UP, WXK_DOWN] and event.ShiftDown() ) ) ):
                # Select the 1st thing from the list:
                match_index = 0
            if( match_index is not None
                and ( keycode in self._autoCompleteKeycodes + [WXK_PRIOR, WXK_NEXT]
                      or (keycode in [WXK_UP, WXK_DOWN] and event.ShiftDown())
                      or (keycode == WXK_DOWN and len(text) < len(choices[match_index])) ) ):

                    # We're allowed to auto-complete:
                    value = self._GetValue()
                    newvalue = value[:edit_start] + choices[match_index] + value[edit_end:]
                    dbg('match found; setting value to "%s"' % newvalue)
                    self._SetValue(newvalue)
                    self._SetInsertionPoint(edit_end)
                    self._CheckValid()  # recolor as appopriate

        if keycode in (WXK_UP, WXK_DOWN, WXK_LEFT, WXK_RIGHT):
            # treat as left right arrow if unshifted, tab/shift tab if shifted.
            if event.ShiftDown():
                if keycode in (WXK_DOWN, WXK_RIGHT):
                    # remove "shifting" and treat as (forward) tab:
                    event.m_shiftDown = False
                keep_processing = self._OnChangeField(event)
            else:
                keep_processing = self._OnArrow(event)
        # some other key; keep processing the key

        dbg('keep processing?', keep_processing, indent=0)
        return keep_processing


    def _autoComplete(self, direction, choices, value, compareNoCase):
        """
        This function gets called in response to Auto-complete events.
        It attempts to find a match to the specified value against the
        list of choices; if exact match, the index of then next
        appropriate value in the list, based on the given direction.
        If not an exact match, it will return the index of the 1st value from
        the choice list for which the partial value can be extended to match.
        If no match found, it will return None.
        """
        if value is None:
            dbg('nothing to match against', indent=0)
            return None

        if compareNoCase:
            choices = [choice.strip().lower() for choice in choices]
            value = value.lower()
        else:
            choices = [choice.strip() for choice in choices]

        if value in choices:
            index = choices.index(value)
            dbg('matched "%s"' % choices[index])
            if direction == -1:
                if index == 0: index = len(choices) - 1
                else: index -= 1
            else:
                if index == len(choices) - 1: index = 0
                else: index += 1
            dbg('change value to ', index)
            match = index
        else:
            value = value.strip()
            dbg('no match; try to auto-complete:')
            match = None
            dbg('searching for "%s"' % value)
            for index in range(len(choices)):
                choice = choices[index]
                if choice.find(value, 0) == 0:
                    dbg('match found:', choice)
                    match = index
                    break
            if match is not None:
                dbg('matched', match)
            else:
                dbg('no match found')
        dbg(indent=0)
        return match


    def _AdjustField(self, pos):
        """
        This function gets called by default whenever the cursor leaves a field.
        The pos argument given is the char position before leaving that field.
        By default, floating point, integer and date values are adjusted to be
        legal in this function.  Derived classes may override this function
        to modify the value of the control in a different way when changing fields.

        NOTE: these change the value immediately, and restore the cursor to
        the passed location, so that any subsequent code can then move it
        based on the operation being performed.
        """
        newvalue = value = self._GetValue()
        field = self._FindField(pos)
        start, end, slice = self._FindFieldExtent(getslice=True)
        newfield = field._AdjustField(slice)
        newvalue = value[:start] + newfield + value[end:]

        if self._isDec and newvalue != self._template:
            newvalue = self._adjustDec(newvalue)

        if self._ctrl_constraints._isInt and value != self._template:
            newvalue = self._adjustInt(value)

        if self._isDate and value != self._template:
            newvalue = self._adjustDate(value, fixcentury=True)
            if self._4digityear:
                year2dig = self._dateExtent - 2
                if pos == year2dig and value[year2dig] != newvalue[year2dig]:
                    pos = pos+2

        if newvalue != value:
            self._SetValue(newvalue)
            self._SetInsertionPoint(pos)


    def _adjustKey(self, pos, key):
        """ Apply control formatting to the key (e.g. convert to upper etc). """
        field = self._FindField(pos)
        if field._forceupper and key in range(97,123):
            key = ord( chr(key).upper())

        if field._forcelower and key in range(97,123):
            key = ord( chr(key).lower())

        return key


    def _adjustPos(self, pos, key):
        """
        Checks the current insertion point position and adjusts it if
        necessary to skip over non-editable characters.
        """
        dbg('_adjustPos', pos, key, indent=1)
        sel_start, sel_to = self._GetSelection()
        # If a numeric or decimal mask, and negatives allowed, reserve the first space for sign
        if( self._signOk
            and pos == self._signpos
            and key in (ord('-'), ord('+'), ord(' ')) ):
            return pos

        if key not in self._nav:
            field = self._FindField(pos)

            dbg('field._insertRight?', field._insertRight)
            if field._insertRight:              # if allow right-insert
                start, end = field._extent
                slice = self._GetValue()[start:end].strip()
                field_len = end - start
                if pos == end:                      # if cursor at right edge of field
                    # if not filled or supposed to stay in field, keep current position
                    if len(slice) < field_len or not field._moveOnFieldFull:
                        dbg('pos==end; len (slice) < field_len or not field._moveOnFieldFull')
                        pass
                else:
                    # if at start of control, move to right edge
                    if sel_to == sel_start and self._isTemplateChar(pos) and pos != end:
                        pos = end                   # move to right edge
                    elif sel_start <= start and sel_to == end:
                        # select to right edge of field - 1 (to replace char)
                        pos = end - 1
                        self._SetInsertionPoint(pos)
                        # restore selection
                        self._SetSelection(sel_start, pos)

                    elif self._signOk and sel_start == 0:   # if selected to beginning and signed,
                        # adjust to past reserved sign position:
                        pos = self._fields[0]._extent[0]
                        self._SetInsertionPoint(pos)
                        # restore selection
                        self._SetSelection(pos, sel_to)
                    else:
                        pass    # leave position/selection alone

            # else make sure the user is not trying to type over a template character
            # If they are, move them to the next valid entry position
            elif self._isTemplateChar(pos):
                if( not field._moveOnFieldFull
                      and (not self._signOk
                           or (self._signOk
                               and field._index == 0
                               and pos > 0) ) ):      # don't move to next field without explicit cursor movement
                    pass
                else:
                    # find next valid position
                    pos = self._findNextEntry(pos)
                    self._SetInsertionPoint(pos)
                    if pos < sel_to:    # restore selection
                        self._SetSelection(pos, sel_to)
        dbg('adjusted pos:', pos, indent=0)
        return pos


    def _adjustDec(self, candidate=None):
        """
        'Fixes' an floating point control. Collapses spaces, right-justifies, etc.
        """
        dbg('wxMaskedEditMixin::_adjustDec, candidate = "%s"' % candidate, indent=1)
        lenOrd,lenDec  = self._mask.split('.')  ## Get ordinal, decimal lengths
        lenOrd = len(lenOrd)
        lenDec = len(lenDec)
        if candidate is None: value = self._GetValue()
        else: value = candidate
        dbg('value = "%(value)s"' % locals(), 'len(value):', len(value))
        ordStr,decStr = value.split('.')

        ordStr = self._fields[0]._AdjustField(ordStr)
        dbg('adjusted ordStr: "%s"' % ordStr)
        lenOrd = len(ordStr)
        decStr = decStr + ('0'*(lenDec-len(decStr)))  # add trailing spaces to decimal

        dbg('ordStr "%(ordStr)s"' % locals())
        dbg('lenOrd:', lenOrd)

        ordStr = string.rjust( ordStr[-lenOrd:], lenOrd)
        dbg('right-justifed ordStr = "%(ordStr)s"' % locals())
        newvalue = ordStr + '.' + decStr
        if self._signOk:
            if len(newvalue) < len(self._mask):
                newvalue = ' ' + newvalue
            signedvalue = self._getSignedValue(newvalue)[0]
            if signedvalue is not None: newvalue = signedvalue

        # Finally, align string with decimal position, left-padding with
        # fillChar:
        newdecpos = newvalue.find('.')
        if newdecpos < self._decimalpos:
            padlen = self._decimalpos - newdecpos
            newvalue = string.join([' ' * padlen] + [newvalue] ,'')
        dbg('newvalue = "%s"' % newvalue)
        if candidate is None:
            wxCallAfter(self._SetValue, newvalue)
        dbg(indent=0)
        return newvalue


    def _adjustInt(self, candidate=None):
        """ 'Fixes' an integer control. Collapses spaces, right or left-justifies."""
        dbg("wxMaskedEditMixin::_adjustInt")
        lenInt = len(self._mask)
        if candidate is None: value = self._GetValue()
        else: value = candidate
        intStr = self._fields[0]._AdjustField(value)
        intStr = intStr.strip() # drop extra spaces
        if self._isNeg and intStr.find('-') == -1:
            intStr = '-' + intStr
        elif self._signOk and intStr.find('-') == -1:
            intStr = ' ' + intStr

        if self._fields[0]._alignRight:     ## Only if right-alignment is enabled
            intStr = intStr.rjust( lenInt )
        else:
            intStr = intStr.ljust( lenInt )

        if candidate is None:
            wxCallAfter(self._SetValue, intStr )
        return intStr


    def _adjustDate(self, candidate=None, fixcentury=False, force4digit_year=False):
        """
        'Fixes' a date control, expanding the year if it can.
        Applies various self-formatting options.
        """
        dbg("wxMaskedEditMixin::_adjustDate", indent=1)
        if candidate is None: text    = self._GetValue()
        else: text = candidate
        dbg('text=', text)
        if self._datestyle == "YMD":
            year_field = 0
        else:
            year_field = 2

        dbg('getYear: "%s"' % getYear(text, self._datestyle))
        year    = string.replace( getYear( text, self._datestyle),self._fields[year_field]._fillChar,"")  # drop extra fillChars
        month   = getMonth( text, self._datestyle)
        day     = getDay( text, self._datestyle)
        dbg('self._datestyle:', self._datestyle, 'year:', year, 'Month', month, 'day:', day)

        yearVal = None
        yearstart = self._dateExtent - 4
        if( len(year) < 4
            and (fixcentury
                 or force4digit_year
                 or (self._GetInsertionPoint() > yearstart+1 and text[yearstart+2] == ' ')
                 or (self._GetInsertionPoint() > yearstart+2 and text[yearstart+3] == ' ') ) ):
            ## user entered less than four digits and changing fields or past point where we could
            ## enter another digit:
            try:
                yearVal = int(year)
            except:
                dbg('bad year=', year)
                year = text[yearstart:self._dateExtent]

        if len(year) < 4 and yearVal:
            if len(year) == 2:
                # Fix year adjustment to be less "20th century" :-) and to adjust heuristic as the
                # years pass...
                now = wxDateTime_Now()
                century = (now.GetYear() /100) * 100        # "this century"
                twodig_year = now.GetYear() - century       # "this year" (2 digits)
                # if separation between today's 2-digit year and typed value > 50,
                #      assume last century,
                # else assume this century.
                #
                # Eg: if 2003 and yearVal == 30, => 2030
                #     if 2055 and yearVal == 80, => 2080
                #     if 2010 and yearVal == 96, => 1996
                #
                if abs(yearVal - twodig_year) > 50:
                    yearVal = (century - 100) + yearVal
                else:
                    yearVal = century + yearVal
                year = str( yearVal )
            else:   # pad with 0's to make a 4-digit year
                year = "%04d" % yearVal
            if self._4digityear or force4digit_year:
                text = makeDate(year, month, day, self._datestyle, text) + text[self._dateExtent:]
        dbg('newdate: "%s"' % text, indent=0)
        return text


    def _goEnd(self, getPosOnly=False):
        """ Moves the insertion point to the end of user-entry """
        dbg("wxMaskedEditMixin::_goEnd; getPosOnly:", getPosOnly, indent=1)
        text = self._GetValue()
        for i in range( min( len(self._mask)-1, len(text)-1 ), -1, -1):
            if self._isMaskChar(i):
                char = text[i]
                if char != ' ':
                    break

        pos = min(i+1,len(self._mask))
        field = self._FindField(pos)
        start, end = field._extent
        if field._insertRight and pos < end:
            pos = end
        dbg('next pos:', pos)
        dbg(indent=0)
        if getPosOnly:
            return pos
        else:
            self._SetInsertionPoint(pos)


    def _goHome(self):
        """ Moves the insertion point to the beginning of user-entry """
        dbg("wxMaskedEditMixin::_goHome", indent=1)
        text = self._GetValue()
        for i in range(len(self._mask)):
            if self._isMaskChar(i):
                break
        self._SetInsertionPoint(max(i,0))
        dbg(indent=0)


    def _getAllowedChars(self, pos):
        """ Returns a string of all allowed user input characters for the provided
            mask character plus control options
        """
        maskChar = self.maskdict[pos]
        okchars = maskchardict[maskChar]        ## entry, get mask approved characters
        field = self._FindField(pos)
        if okchars and field._okSpaces:          ## Allow spaces?
            okchars += " "
        if okchars and field._includeChars:      ## any additional included characters?
            okchars += field._includeChars
##        dbg('okchars[%d]:' % pos, okchars)
        return okchars


    def _isMaskChar(self, pos):
        """ Returns True if the char at position pos is a special mask character (e.g. NCXaA#)
        """
        if pos < len(self._mask):
            return self.ismasked[pos]
        else:
            return False


    def _isTemplateChar(self,Pos):
        """ Returns True if the char at position pos is a template character (e.g. -not- NCXaA#)
        """
        if Pos < len(self._mask):
            return not self._isMaskChar(Pos)
        else:
            return False


    def _isCharAllowed(self, char, pos, checkRegex=False):
        """ Returns True if character is allowed at the specific position, otherwise False."""
        dbg('_isCharAllowed', char, pos, checkRegex, indent=1)
        field = self._FindField(pos)
        right_insert = False

        if self.controlInitialized:
            sel_start, sel_to = self._GetSelection()
        else:
            sel_start, sel_to = pos, pos

        if (field._insertRight or self._ctrl_constraints._insertRight):
            start, end = field._extent
            if pos == end or (sel_start, sel_to) == field._extent:
                pos = end - 1
                right_insert = True

        if self._isTemplateChar( pos ):  ## if a template character, return empty
            dbg(indent=0)
            return False

        if self._isMaskChar( pos ):
            okChars  = self._getAllowedChars(pos)
            if self._fields[0]._groupdigits and (self._isInt or (self._isDec and pos < self._decimalpos)):
                okChars += self._fields[0]._groupchar
            if self._signOk and (self._isInt or (self._isDec and pos < self._decimalpos)):
                okChars += '-'
##            dbg('%s in %s?' % (char, okChars), char in okChars)
            approved = char in okChars

            if approved and checkRegex:
                dbg("checking appropriate regex's")
                value = self._eraseSelection(self._GetValue())
                if right_insert:
                    newvalue, newpos = self._insertKey(char, pos+1, sel_start, sel_to, value)
                else:
                    newvalue, newpos = self._insertKey(char, pos, sel_start, sel_to, value)
                dbg('newvalue: "%s"' % newvalue)

                fields = [self._FindField(pos)] + [self._ctrl_constraints]
                for field in fields:    # includes fields[-1] == "ctrl_constraints"
                    if field._regexMask and field._filter:
                        dbg('checking vs. regex')
                        start, end = field._extent
                        slice = newvalue[start:end]
                        approved = (re.match( field._filter, slice) is not None)
                        dbg('approved?', approved)
                    if not approved: break
            dbg(indent=0)
            return approved
        else:
            dbg(indent=0)
            return False


    def _applyFormatting(self):
        """ Apply formatting depending on the control's state.
            Need to find a way to call this whenever the value changes, in case the control's
            value has been changed or set programatically.
        """
        dbg(suspend=1)
        dbg('wxMaskedEditMixin::_applyFormatting', indent=1)

        # Handle negative numbers
        if self._signOk:
##            value = self._GetValue()
            text, signpos = self._getSignedValue()
            dbg('text: "%s", signpos:' % text, signpos)
            if not text or text[signpos] != '-':
                self._isNeg = False
                dbg('supposedly negative, but no sign found; new sign:', self._isNeg)
                if text and signpos != self._signpos:
                    self._signpos = signpos
            elif text and self._valid and not self._isNeg and text[signpos] == '-':
                self._isNeg = True

        if self._signOk and self._isNeg:
            dbg('setting foreground to', self._signedForegroundColor)
            self.SetForegroundColour(self._signedForegroundColor)
        else:
            dbg('setting foreground to', self._foregroundColor)
            self.SetForegroundColour(self._foregroundColor)

        if self._valid:
            dbg('valid')
            if self.IsEmpty():
                if self._emptyInvalid:
                    dbg('setting background to', self._invalidBackgroundColor)
                else:
                    dbg('setting background to', self._emptyBackgroundColor)
                    self.SetBackgroundColour(self._emptyBackgroundColor)
            else:
                self.SetBackgroundColour(self._validBackgroundColor)
        else:
            dbg('invalid; coloring', self._invalidBackgroundColor)
            self.SetBackgroundColour(self._invalidBackgroundColor) ## Change BG color if invalid

        self._Refresh()
        dbg(indent=0, suspend=0)


    def _getAbsValue(self, candidate=None):
        """ Return an unsigned value (i.e. strip the '-' prefix if any).
        """
        dbg('wxMaskedEditMixin::_getAbsValue; candidate="%s"' % candidate, indent=1)
        if candidate is None: text = self._GetValue()
        else: text = candidate

        if self._isInt:
            if self._ctrl_constraints._alignRight and self._fields[0]._fillChar == ' ':
                signpos = text.find('-')
                if signpos == -1:
                    rstripped_text = text.rstrip()
                    signpos = rstripped_text.rfind(' ')
                    dbg('signpos:', signpos)
                    if signpos == -1:
                        signpos = 0
                abstext = text[:signpos] + ' ' + text[signpos+1:]
            else:
                signpos = 0
                text = self._template[0] + text[1:]
            groupchar = self._fields[0]._groupchar
            try:
                value = long(text.replace(groupchar,''))
            except:
                dbg('invalid number', indent=0)
                return None, None

        else:   # decimal value
            try:
                groupchar = self._fields[0]._groupchar
                value = float(text.replace(groupchar,''))
                if value < 0:
                    signpos = text.find('-')
                    text = text[:signpos] + self._template[signpos] + text[signpos+1:]
                else:
                    # look backwards from the decimal point for the 1st non-digit
                    dbg('decimal pos:', self._decimalpos)
                    signpos = self._decimalpos-1
                    dbg('text: "%s"' % text)
                    dbg('text[%d]:' % signpos, text[signpos])
                    dbg('text[signpos] in list(string.digits) + [groupchar]?', text[signpos] in list(string.digits) + [groupchar])
                    while text[signpos] in list(string.digits) + [groupchar] and signpos > 0:
                        signpos -= 1
                        dbg('text[%d]:' % signpos, text[signpos])
            except ValueError:
                dbg('invalid number', indent=0)
                return None, None

        dbg('abstext = "%s"' % text, 'signpos:', signpos)
        dbg(indent=0)
        return text, signpos


    def _getSignedValue(self, candidate=None):
        """ Return a signed value by adding a "-" prefix if the value
            is set to negative, or a space if positive.
        """
        dbg('wxMaskedEditMixin::_getSignedValue; candidate="%s"' % candidate, indent=1)
        if candidate is None: text = self._GetValue()
        else: text = candidate


        abstext, signpos = self._getAbsValue(text)
        if self._signOk:
            if abstext is None:
                dbg(indent=0)
                return abstext, signpos

            if self._isNeg or text[signpos] == '-':
                sign = '-'
            else:
                sign = ' '
            if text[signpos] not in string.digits:
                text = text[:signpos] + sign + text[signpos+1:]
            else:
                # this can happen if value passed is too big; sign assumed to be
                # in position 0, but if already filled with a digit, prepend sign...
                text = sign + text
        else:
            text = abstext
        dbg('signedtext = "%s"' % text, 'signpos:', signpos)
        dbg(indent=0)
        return text, signpos


    def GetPlainValue(self, candidate=None):
        """ Returns control's value stripped of the template text.
            plainvalue = wxMaskedEditMixin.GetPlainValue()
        """
        if candidate is None: text = self._GetValue()
        else: text = candidate

        if self.IsEmpty():
            return ""
        else:
            plain = ""
            for idx in range( len( self._template)):
                if self._mask[idx] in maskchars:
                    plain += text[idx]

            if self._isDec or self._isInt:
                if self._fields[0]._alignRight:
                    lpad = plain.count(',')
                    plain = ' ' * lpad + plain.replace(',','')
                else:
                    plain = plain.replace(',','')

                if self._signOk and self._isNeg and plain.count('-') == 0:
                    # must be in reserved position; add to "plain value"
                    plain = '-' + plain

            return plain.rstrip()


    def IsEmpty(self, value=None):
        """
        Returns True if control is equal to an empty value.
        (Empty means all editable positions in the template == fillChar.)
        """
        if value is None: value = self._GetValue()
        if value == self._template and not self._defaultValue:
##            dbg("IsEmpty? 1 (value == self._template and not self._defaultValue)")
            return True     # (all mask chars == fillChar by defn)
        elif value == self._template:
            empty = True
            for pos in range(len(self._template)):
##                dbg('isMaskChar(%(pos)d)?' % locals(), self._isMaskChar(pos))
##                dbg('value[%(pos)d] != self._fillChar?' %locals(), value[pos] != self._fillChar[pos])
                if self._isMaskChar(pos) and value[pos] not in (' ', self._fillChar[pos]):
                    empty = False
##            dbg("IsEmpty? %(empty)d (do all mask chars == fillChar?)" % locals())
            return empty
        else:
##            dbg("IsEmpty? 0 (value doesn't match template)")
            return False


    def IsDefault(self, value=None):
        """
        Returns True if the value specified (or the value of the control if not specified)
        is equal to the default value.
        """
        if value is None: value = self._GetValue()
        return value == self._template


    def IsValid(self, value=None):
        """ Indicates whether the value specified (or the current value of the control
        if not specified) is considered valid."""
##        dbg('wxMaskedEditMixin::IsValid("%s")' % value, indent=1)
        if value is None: value = self._GetValue()
        ret = self._CheckValid(value)
##        dbg(indent=0)
        return ret


    def _eraseSelection(self, value=None, sel_start=None, sel_to=None):
        """ Used to blank the selection when inserting a new character. """
        dbg("wxMaskedEditMixin::_eraseSelection", indent=1)
        if value is None: value = self._GetValue()
        if sel_start is None or sel_to is None:
            sel_start, sel_to = self._GetSelection()                   ## check for a range of selected text
        dbg('value: "%s"' % value)
        dbg("current sel_start, sel_to:", sel_start, sel_to)

        newvalue = list(value)
        for i in range(sel_start, sel_to):
            if self._isMaskChar(i):
                field = self._FindField(i)
                if field._padZero:
                    newvalue[i] = '0'
                else:
                    newvalue[i] = self._template[i]
            elif self._signOk and i == 0 and newvalue[i] == '-':
                newvalue[i] = ' '
        value = string.join(newvalue,"")
        dbg('new value: "%s"' % value)
        dbg(indent=0)
        return value


    def _insertKey(self, char, pos, sel_start, sel_to, value):
        """ Handles replacement of the character at the current insertion point."""
        dbg('wxMaskedEditMixin::_insertKey', "\'" + char + "\'", pos, sel_start, sel_to, '"%s"' % value, indent=1)

        text = self._eraseSelection(value)
        field = self._FindField(pos)
        start, end = field._extent
        newtext = ""
        if field._insertRight:
            # special case; do right insertion if either whole field selected or cursor at right edge of field
            if pos == end or (sel_start, sel_to) == field._extent:   # right edge insert
                fstr = text[start:end]
                erasable_chars = [field._fillChar, ' ']
                if field._padZero: erasable_chars.append('0')
                erased = ''
                if fstr[0] in erasable_chars or (self._signOk and field._index == 0 and fstr[0] == '-'):
                    erased = fstr[0]
                    fstr = fstr[1:] + char
                dbg('field str: "%s"' % fstr)
                newtext = text[:start] + fstr + text[end:]
                if erased == '-' and self._signOk:
                    newtext = '-' + newtext[1:]
                dbg('newtext: "%s"' % newtext)
                if self._signOk and field._index == 0: start -= 1             # account for sign position
                if( field._moveOnFieldFull
                    and len(fstr.lstrip()) == end-start):   # if field now full
                    newpos = self._findNextEntry(end)       #   go to next field
                else:
                    newpos = end                            # else keep cursor at right edge

        if not newtext:
            before  = text[0:pos]
            after   = text[pos+1:]
            newtext = before + char + after
            newpos = pos+1

        dbg('newtext: "%s"' % newtext, 'newpos:', newpos)

        dbg(indent=0)
        return newtext, newpos


    def _OnFocus(self,event):
        """
        This event handler is currently necessary to work around new default
        behavior as of wxPython2.3.3;
        The TAB key auto selects the entire contents of the wxTextCtrl *after*
        the EVT_SET_FOCUS event occurs; therefore we can't query/adjust the selection
        *here*, because it hasn't happened yet.  So to prevent this behavior, and
        preserve the correct selection when the focus event is not due to tab,
        we need to pull the following trick:
        """
        dbg('wxMaskedEditMixin::_OnFocus')
        wxCallAfter(self._fixSelection)
        event.Skip()
        self.Refresh()


    def _CheckValid(self, candidate=None):
        """
        This is the default validation checking routine; It verifies that the
        current value of the control is a "valid value," and has the side
        effect of coloring the control appropriately.
        """
        dbg(suspend=1)
        dbg('wxMaskedEditMixin::_CheckValid: candidate="%s"' % candidate, indent=1)
        oldValid = self._valid
        if candidate is None: value = self._GetValue()
        else: value = candidate
        dbg('value: "%s"' % value)
        oldvalue = value
        valid = True    # assume True

        if not self.IsDefault(value) and self._isDate:                    ## Date type validation
            valid = self._validateDate(value)
            dbg("valid date?", valid)

        elif not self.IsDefault(value) and self._isTime:
            valid = self._validateTime(value)
            dbg("valid time?", valid)

        elif not self.IsDefault(value) and (self._isInt or self._isDec):  ## Numeric type
            valid = self._validateNumeric(value)
            dbg("valid Number?", valid)

        if valid and not self.IsDefault(value):
            ## valid so far; ensure also allowed by any list or regex provided:
            valid = self._validateGeneric(value)
            dbg("valid value?", valid)

        if valid and self._emptyInvalid:
            for field in self._fields.values():
                start, end = field._extent
                if field.IsEmpty(value[start:end]):
                    valid = False
                    break

        dbg('valid?', valid)

        if not candidate:
            self._valid = valid
            self._applyFormatting()
            if self._valid != oldValid:
                dbg('validity changed: oldValid =',oldValid,'newvalid =', self._valid)
                dbg('oldvalue: "%s"' % oldvalue, 'newvalue: "%s"' % self._GetValue())
        dbg(indent=0, suspend=0)
        return valid


    def _validateGeneric(self, candidate=None):
        """ Validate the current value using the provided list or Regex filter (if any).
        """
        if candidate is None:
            text = self._GetValue()
        else:
            text = candidate

        valid = True    # assume true
        for i in [-1] + self._field_indices:   # process global constraints first:
            field = self._fields[i]
            start, end = field._extent
            slice = text[start:end]
            valid = field.IsValid(slice)
            if not valid:
                break

        return valid


    def _validateNumeric(self, candidate=None):
        """ Validate that the value is within the specified range (if specified.)"""
        if candidate is None: value = self._GetValue()
        else: value = candidate
        try:
            groupchar = self._fields[0]._groupchar
            if self._isDec:
                number = float(value.replace(groupchar, ''))
            else:
                number = int( value.replace(groupchar, ''))
            dbg('number:', number)
            if self._ctrl_constraints._hasRange:
                valid = self._ctrl_constraints._rangeLow <= number <= self._ctrl_constraints._rangeHigh
            else:
                valid = True
            groupcharpos = value.rfind(groupchar)
            if groupcharpos != -1:  # group char present
                dbg('groupchar found at', groupcharpos)
                if self._isDec and groupcharpos > self._decimalpos:
                    # 1st one found on right-hand side is past decimal point
                    dbg('groupchar in fraction; illegal')
                    valid = False
                elif self._isDec:
                    ord = value[:self._decimalpos]
                else:
                    ord = value.strip()

                parts = ord.split(groupchar)
                for i in range(len(parts)):
                    if i == 0 and abs(int(parts[0])) > 999:
                        dbg('group 0 too long; illegal')
                        valid = False
                        break
                    elif i > 0 and (len(parts[i]) != 3 or ' ' in parts[i]):
                        dbg('group %i (%s) not right size; illegal' % (i, parts[i]))
                        valid = False
                        break
        except ValueError:
            dbg('value not a valid number')
            valid = False
        return valid


    def _validateDate(self, candidate=None):
        """ Validate the current date value using the provided Regex filter.
            Generally used for character types.BufferType
        """
        dbg('wxMaskedEditMixin::_validateDate', indent=1)
        if candidate is None: value = self._GetValue()
        else: value = candidate
        dbg('value = "%s"' % value)
        text = self._adjustDate(value, force4digit_year=True)     ## Fix the date up before validating it
        dbg('text =', text)
        valid = True   # assume True until proven otherwise

        try:
            # replace fillChar in each field with space:
            datestr = text[0:self._dateExtent]
            for i in range(3):
                field = self._fields[i]
                start, end = field._extent
                fstr = datestr[start:end]
                fstr.replace(field._fillChar, ' ')
                datestr = datestr[:start] + fstr + datestr[end:]

            year, month, day = getDateParts( datestr, self._datestyle)
            year = int(year)
            dbg('self._dateExtent:', self._dateExtent)
            if self._dateExtent == 11:
                month = charmonths_dict[month.lower()]
            else:
                month = int(month)
            day = int(day)
            dbg('year, month, day:', year, month, day)

        except ValueError:
            dbg('cannot convert string to integer parts')
            valid = False
        except KeyError:
            dbg('cannot convert string to integer month')
            valid = False

        if valid:
            # use wxDateTime to unambiguously try to parse the date:
            # ### Note: because wxDateTime is *brain-dead* and expects months 0-11,
            # rather than 1-12, so handle accordingly:
            if month > 12:
                valid = False
            else:
                month -= 1
                try:
                    dbg("trying to create date from values day=%d, month=%d, year=%d" % (day,month,year))
                    dateHandler = wxDateTimeFromDMY(day,month,year)
                    dbg("succeeded")
                    dateOk = True
                except:
                    dbg('cannot convert string to valid date')
                    dateOk = False
                if not dateOk:
                    valid = False

            if valid:
                # wxDateTime doesn't take kindly to leading/trailing spaces when parsing,
                # so we eliminate them here:
                timeStr     = text[self._dateExtent+1:].strip()         ## time portion of the string
                if timeStr:
                    dbg('timeStr: "%s"' % timeStr)
                    try:
                        checkTime    = dateHandler.ParseTime(timeStr)
                        valid = checkTime == len(timeStr)
                    except:
                        valid = False
                    if not valid:
                        dbg('cannot convert string to valid time')
        if valid: dbg('valid date')
        dbg(indent=0)
        return valid


    def _validateTime(self, candidate=None):
        """ Validate the current time value using the provided Regex filter.
            Generally used for character types.BufferType
        """
        dbg('wxMaskedEditMixin::_validateTime', indent=1)
        # wxDateTime doesn't take kindly to leading/trailing spaces when parsing,
        # so we eliminate them here:
        if candidate is None: value = self._GetValue().strip()
        else: value = candidate.strip()
        dbg('value = "%s"' % value)
        valid = True   # assume True until proven otherwise

        dateHandler = wxDateTime_Today()
        try:
            checkTime    = dateHandler.ParseTime(value)
            dbg('checkTime:', checkTime, 'len(value)', len(value))
            valid = checkTime == len(value)
        except:
            valid = False

        if not valid:
            dbg('cannot convert string to valid time')
        if valid: dbg('valid time')
        dbg(indent=0)
        return valid


    def _OnKillFocus(self,event):
        """ Handler for EVT_KILL_FOCUS event.
        """
        dbg('wxMaskedEditMixin::_OnKillFocus', 'isDate=',self._isDate, indent=1)
        if self._mask and self._IsEditable():
            self._AdjustField(self._GetInsertionPoint())
            self._CheckValid()   ## Call valid handler

        self._LostFocus()    ## Provided for subclass use
        event.Skip()
        dbg(indent=0)


    def _fixSelection(self):
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
        dbg('wxMaskedEditMixin::_fixSelection', indent=1)
        if not self._mask or not self._IsEditable():
            dbg(indent=0)
            return

        sel_start, sel_to = self._GetSelection()
        dbg('sel_start, sel_to:', sel_start, sel_to, 'self.IsEmpty()?', self.IsEmpty())

        if( sel_start == 0 and sel_to >= len( self._mask )   #(can be greater in numeric controls because of reserved space)
            or self.IsEmpty() or self.IsDefault()):
            # This isn't normally allowed, and so assume we got here by the new
            # "tab traversal" behavior, so we need to reset the selection
            # and insertion point:
            dbg('entire text selected; resetting selection to start of control')
            self._goHome()
            if self._FindField(0)._selectOnFieldEntry:
                edit_start, edit_end = self._FindFieldExtent(self._GetInsertionPoint())
                self._SetSelection(edit_start, edit_end)
            elif self._fields[0]._insertRight:
                self._SetInsertionPoint(self._fields[0]._extent[1])

        elif sel_start == 0 and self._GetValue()[0] == '-' and (self._isDec or self._isInt) and self._signOk:
            dbg('control is empty; start at beginning after -')
            self._SetInsertionPoint(1)   ## Move past minus sign space if signed
            if self._FindField(0)._selectOnFieldEntry:
                edit_start, edit_end = self._FindFieldExtent(self._GetInsertionPoint())
                self._SetSelection(1, edit_end)
            elif self._fields[0]._insertRight:
                self._SetInsertionPoint(self._fields[0]._extent[1])

        elif sel_start > self._goEnd(getPosOnly=True):
            dbg('cursor beyond the end of the user input; go to end of it')
            self._goEnd()
        else:
            dbg('sel_start, sel_to:', sel_start, sel_to, 'len(self._mask):', len(self._mask))
        dbg(indent=0)


    def _Keypress(self,key):
        """ Method provided to override OnChar routine. Return False to force
            a skip of the 'normal' OnChar process. Called before class OnChar.
        """
        return True


    def _LostFocus(self):
        """ Method provided for subclasses. _LostFocus() is called after
            the class processes its EVT_KILL_FOCUS event code.
        """
        pass


    def _OnDoubleClick(self, event):
        """ selects field under cursor on dclick."""
        pos = self._GetInsertionPoint()
        field = self._FindField(pos)
        start, end = field._extent
        self._SetInsertionPoint(start)
        self._SetSelection(start, end)


    def _Change(self):
        """ Method provided for subclasses. Called by internal EVT_TEXT
            handler. Return False to override the class handler, True otherwise.
        """
        return True


    def _Cut(self):
        """
        Used to override the default Cut() method in base controls, instead
        copying the selection to the clipboard and then blanking the selection,
        leaving only the mask in the selected area behind.
        Note: _Cut (read "undercut" ;-) must be called from a Cut() override in the
        derived control because the mixin functions can't override a method of
        a sibling class.
        """
        dbg("wxMaskedEditMixin::_Cut", indent=1)
        value = self._GetValue()
        dbg('current value: "%s"' % value)
        sel_start, sel_to = self._GetSelection()                   ## check for a range of selected text
        dbg('selected text: "%s"' % value[sel_start:sel_to].strip())
        do = wxTextDataObject()
        do.SetText(value[sel_start:sel_to].strip())
        wxTheClipboard.Open()
        wxTheClipboard.SetData(do)
        wxTheClipboard.Close()

        wxCallAfter(self._SetValue, self._eraseSelection() )
        wxCallAfter(self._SetInsertionPoint, sel_start)
        dbg(indent=0)


# WS Note: overriding Copy is no longer necessary given that you
# can no longer select beyond the last non-empty char in the control.
#
##    def _Copy( self ):
##        """
##        Override the wxTextCtrl's .Copy function, with our own
##        that does validation.  Need to strip trailing spaces.
##        """
##        sel_start, sel_to = self._GetSelection()
##        select_len = sel_to - sel_start
##        textval = wxTextCtrl._GetValue(self)
##
##        do = wxTextDataObject()
##        do.SetText(textval[sel_start:sel_to].strip())
##        wxTheClipboard.Open()
##        wxTheClipboard.SetData(do)
##        wxTheClipboard.Close()


    def _getClipboardContents( self ):
        """ Subroutine for getting the current contents of the clipboard.
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


    def _validatePaste(self, paste_text, sel_start, sel_to, raise_on_invalid=False):
        """
        Used by paste routine and field choice validation to see
        if a given slice of paste text is legal for the area in question:
        returns validity, replacement text, and extent of paste in
        template.
        """
        dbg(suspend=1)
        dbg('wxMaskedEditMixin::_validatePaste("%(paste_text)s", %(sel_start)d, %(sel_to)d), raise_on_invalid? %(raise_on_invalid)d' % locals(), indent=1)
        select_length = sel_to - sel_start
        maxlength = select_length
        dbg('sel_to - sel_start:', maxlength)
        if maxlength == 0:
            maxlength = len(self._mask) - sel_start
        dbg('maxlength:', maxlength)
        length_considered = len(paste_text)
        if length_considered > maxlength:
            dbg('paste text will not fit into the control:', indent=0)
            if raise_on_invalid:
                raise ValueError('"%s" will not fit into the control "%s"' % (paste_text, self.name))
            else:
                return False, None, None

        text = self._template
        dbg('length_considered:', length_considered)

        valid_paste = True
        replacement_text = ""
        replace_to = sel_start
        i = 0
        while valid_paste and i < length_considered and replace_to < len(self._mask):
            char = paste_text[i]
            field = self._FindField(replace_to)
            if field._forceupper:   char = char.upper()
            elif field._forcelower: char = char.lower()

            dbg('char:', "'"+char+"'", 'i =', i, 'replace_to =', replace_to)
            dbg('self._isTemplateChar(%d)?' % replace_to, self._isTemplateChar(replace_to))
            if not self._isTemplateChar(replace_to) and self._isCharAllowed( char, replace_to):
                replacement_text += char
                dbg("not template(%(replace_to)d) and charAllowed('%(char)s',%(replace_to)d)" % locals())
                dbg("replacement_text:", '"'+replacement_text+'"')
                i += 1
                replace_to += 1
            elif char == self._template[replace_to] or (i == 0 and char == '-' and self._signOk):
                replacement_text += char
                dbg("'%(char)s' == template(%(replace_to)d)" % locals())
                dbg("replacement_text:", '"'+replacement_text+'"')
                i += 1
                replace_to += 1
            else:
                next_entry = self._findNextEntry(replace_to, adjustInsert=False)
                if next_entry == replace_to:
                    valid_paste = False
                else:
                    replacement_text += self._template[replace_to:next_entry]
                    dbg("skipping template; next_entry =", next_entry)
                    dbg("replacement_text:", '"'+replacement_text+'"')
                    replace_to = next_entry  # so next_entry will be considered on next loop

        if not valid_paste and raise_on_invalid:
            dbg('raising exception')
            raise ValueError('"%s" cannot be inserted into the control "%s"' % (paste_text, self.name))

        elif i < len(paste_text):
            valid_paste = False
            if raise_on_invalid:
                dbg('raising exception')
                raise ValueError('"%s" will not fit into the control "%s"' % (paste_text, self.name))

        dbg('valid_paste?', valid_paste)
        if valid_paste:
            dbg('replacement_text: "%s"' % replacement_text, 'replace to:', replace_to)
        dbg(indent=0, suspend=0)
        return valid_paste, replacement_text, replace_to


    def _Paste( self, value=None, raise_on_invalid=False, just_return_value=False ):
        """
        Used to override the base control's .Paste() function,
        with our own that does validation.
        Note: _Paste must be called from a Paste() override in the
        derived control because the mixin functions can't override a
        method of a sibling class.
        """
        dbg('wxMaskedEditMixin::_Paste (value = "%s")' % value, indent=1)
        if value is None:
            paste_text = self._getClipboardContents()
        else:
            paste_text = value

        if paste_text is not None:
            dbg('paste text:', paste_text)
            # (conversion will raise ValueError if paste isn't legal)
            sel_start, sel_to = self._GetSelection()
            try:
                valid_paste, replacement_text, replace_to = self._validatePaste(paste_text, sel_start, sel_to, raise_on_invalid)
            except:
                dbg('exception thrown', indent=0)
                raise

            if not valid_paste:
                dbg('paste text not legal for the selection or portion of the control following the cursor;')
                dbg(indent=0)
                return False
            # else...
            text = self._eraseSelection()

            new_text = text[:sel_start] + replacement_text + text[replace_to:]
            if new_text:
                new_text = string.ljust(new_text,len(self._mask))
            dbg("new_text:", '"'+new_text+'"')

            if not just_return_value:
                if new_text == '':
                    self.ClearValue()
                else:
                    wxCallAfter(self._SetValue, )
                    new_pos = sel_start + len(replacement_text)
                    wxCallAfter(self._SetInsertionPoint, new_pos)
            else:
                return new_text
        elif just_return_value:
            return self._GetValue()
        dbg(indent=0)



## ---------- ---------- ---------- ---------- ---------- ---------- ----------

class wxMaskedTextCtrl( wxTextCtrl, wxMaskedEditMixin ):
    """
    This is the primary derivation from wxMaskedEditMixin.  It provides
    a general masked text control that can be configured with different
    masks.
    """

    def __init__( self, parent, id=-1, value = '',
                  pos = wxDefaultPosition,
                  size = wxDefaultSize,
                  style = wxTE_PROCESS_TAB,
                  validator=wxDefaultValidator,     ## placeholder provided for data-transfer logic
                  name = 'maskedTextCtrl',
                  setupEventHandling = True,        ## setup event handling by default
                  **kwargs):

        wxTextCtrl.__init__(self, parent, id, value='',
                            pos=pos, size = size,
                            style=style, validator=validator,
                            name=name)

        self.controlInitialized = True
        wxMaskedEditMixin.__init__( self, name, **kwargs )
        self._SetInitialValue(value)

        if setupEventHandling:
            ## Setup event handlers
            EVT_SET_FOCUS( self, self._OnFocus )        ## defeat automatic full selection
            EVT_KILL_FOCUS( self, self._OnKillFocus )   ## run internal validator
            EVT_LEFT_DCLICK(self, self._OnDoubleClick)  ## select field under cursor on dclick
            EVT_KEY_DOWN( self, self._OnKeyDown )       ## capture control events not normally seen, eg ctrl-tab.
            EVT_CHAR( self, self._OnChar )              ## handle each keypress
            EVT_TEXT( self, self.GetId(), self._OnTextChange )  ## color control appropriately


    def __repr__(self):
        return "<wxMaskedTextCtrl: %s>" % self.GetValue()


    def _GetSelection(self):
        """
        Allow mixin to get the text selection of this control.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        return self.GetSelection()

    def _SetSelection(self, sel_start, sel_to):
        """
        Allow mixin to set the text selection of this control.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        return self.SetSelection( sel_start, sel_to )

    def SetSelection(self, sel_start, sel_to):
        """
        This is just for debugging...
        """
        dbg("wxMaskedTextCtrl::SetSelection(%(sel_start)d, %(sel_to)d)" % locals())
        wxTextCtrl.SetSelection(self, sel_start, sel_to)


    def _GetInsertionPoint(self):
        return self.GetInsertionPoint()

    def _SetInsertionPoint(self, pos):
        self.SetInsertionPoint(pos)

    def SetInsertionPoint(self, pos):
        """
        This is just for debugging...
        """
        dbg("wxMaskedTextCtrl::SetInsertionPoint(%(pos)d)" % locals())
        wxTextCtrl.SetInsertionPoint(self, pos)


    def _GetValue(self):
        """
        Allow mixin to get the raw value of the control with this function.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        return self.GetValue()

    def _SetValue(self, value):
        """
        Allow mixin to set the raw value of the control with this function.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        dbg('wxMaskedTextCtrl::_SetValue("%(value)s")' % locals(), indent=1)
        wxTextCtrl.SetValue(self, value)
        dbg(indent=0)

    def SetValue(self, value):
        """
        This function redefines the externally accessible .SetValue to be
        a smart "paste" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """
        dbg('wxMaskedTextCtrl::SetValue = "%s"' % value, indent=1)

        # empty previous contents, replacing entire value:
        self._SetInsertionPoint(0)
        self._SetSelection(0, len(self._mask))

        # make SetValue behave the same as if you had typed the value in:
        value = self._Paste(value, raise_on_invalid=True, just_return_value=True)
        if self._isDec:
            self._isNeg = False     # (clear current assumptions)
            value = self._adjustDec(value)
        elif self._isInt:
            self._isNeg = False     # (clear current assumptions)
            value = self._adjustInt(value)
        elif self._isDate:
            value = self._adjustDate(value)

        self._SetValue(value)
        dbg(indent=0)


    def _Refresh(self):
        """
        Allow mixin to refresh the base control with this function.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        dbg('wxMaskedTextCtrl::_Refresh', indent=1)
        wxTextCtrl.Refresh(self)
        dbg(indent=0)


    def Refresh(self):
        """
        This function redefines the externally accessible .Refresh() to
        validate the contents of the masked control as it refreshes.
        NOTE: this must be done in the class derived from the base wx control.
        """
        dbg('wxMaskedTextCtrl::Refresh', indent=1)
        self._CheckValid()
        self._Refresh()
        dbg(indent=0)


    def _IsEditable(self):
        """
        Allow mixin to determine if the base control is editable with this function.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        return wxTextCtrl.IsEditable(self)


    def Cut(self):
        """
        This function redefines the externally accessible .Cut to be
        a smart "erase" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """
        self._Cut()     # call the mixin's Cut method


    def Paste(self):
        """
        This function redefines the externally accessible .Paste to be
        a smart "paste" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """
        self._Paste()   # call the mixin's Paste method


    def IsModified(self):
        """
        This function overrides the raw wxTextCtrl method, because the
        masked edit mixin uses SetValue to change the value, which doesn't
        modify the state of this attribute.  So, we keep track on each
        keystroke to see if the value changes, and if so, it's been
        modified.
        """
        return wxTextCtrl.IsModified(self) or self.modified




## ---------- ---------- ---------- ---------- ---------- ---------- ----------
## Because calling SetSelection programmatically does not fire EVT_COMBOBOX
## events, we have to do it ourselves when we auto-complete.
class wxMaskedComboBoxSelectEvent(wxPyCommandEvent):
    def __init__(self, id, selection = 0, object=None):
        wxPyCommandEvent.__init__(self, wxEVT_COMMAND_COMBOBOX_SELECTED, id)

        self.__selection = selection
        self.SetEventObject(object)

    def GetSelection(self):
        """Retrieve the value of the control at the time
        this event was generated."""
        return self.__selection


class wxMaskedComboBox( wxComboBox, wxMaskedEditMixin ):
    """
    This masked edit control adds the ability to use a masked input
    on a combobox, and do auto-complete of such values.
    """
    def __init__( self, parent, id=-1, value = '',
                  pos = wxDefaultPosition,
                  size = wxDefaultSize,
                  choices = [],
                  style = wxCB_DROPDOWN,
                  validator = wxDefaultValidator,
                  name = "maskedComboBox",
                  setupEventHandling = True,        ## setup event handling by default):
                  **kwargs):


        # This is necessary, because wxComboBox currently provides no
        # method for determining later if this was specified in the
        # constructor for the control...
        self.__readonly = style & wxCB_READONLY == wxCB_READONLY

        kwargs['choices'] = choices                 ## set up maskededit to work with choice list too

        ## Since combobox completion is case-insensitive, always validate same way
        if not kwargs.has_key('compareNoCase'):
            kwargs['compareNoCase'] = True

        wxMaskedEditMixin.__init__( self, name, **kwargs )
        self._choices = self._ctrl_constraints._choices
        dbg('self._choices:', self._choices)

        if self._ctrl_constraints._alignRight:
            choices = [choice.rjust(len(self._mask)) for choice in choices]
        else:
            choices = [choice.ljust(len(self._mask)) for choice in choices]

        wxComboBox.__init__(self, parent, id, value='',
                            pos=pos, size = size,
                            choices=choices, style=style|wxWANTS_CHARS,
                            validator=validator,
                            name=name)

        self.controlInitialized = True

        # Set control font - fixed width by default
        self._setFont()

        if self._autofit:
            self.SetClientSize(self.calcSize())

        if value:
            # ensure value is width of the mask of the control:
            if self._ctrl_constraints._alignRight:
                value = value.rjust(len(self._mask))
            else:
                value = value.ljust(len(self._mask))

        if self.__readonly:
            self.SetStringSelection(value)
        else:
            self._SetInitialValue(value)


        self._SetKeycodeHandler(WXK_UP, self.OnSelectChoice)
        self._SetKeycodeHandler(WXK_DOWN, self.OnSelectChoice)

        if setupEventHandling:
            ## Setup event handlers
            EVT_SET_FOCUS( self, self._OnFocus )        ## defeat automatic full selection
            EVT_KILL_FOCUS( self, self._OnKillFocus )   ## run internal validator
            EVT_LEFT_DCLICK(self, self._OnDoubleClick)  ## select field under cursor on dclick
            EVT_CHAR( self, self._OnChar )              ## handle each keypress
            EVT_KEY_DOWN( self, self.OnKeyDown )        ## for special processing of up/down keys
            EVT_KEY_DOWN( self, self._OnKeyDown )       ## for processing the rest of the control keys
                                                           ## (next in evt chain)
            EVT_TEXT( self, self.GetId(), self._OnTextChange )  ## color control appropriately


    def __repr__(self):
        return "<wxMaskedComboBox: %s>" % self.GetValue()


    def calcSize(self, size=None):
        """
        Calculate automatic size if allowed; override base mixin function
        to account for the selector button.
        """
        size = self._calcSize(size)
        return (size[0]+20, size[1])


    def _GetSelection(self):
        """
        Allow mixin to get the text selection of this control.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        return self.GetMark()

    def _SetSelection(self, sel_start, sel_to):
        """
        Allow mixin to set the text selection of this control.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        return self.SetMark( sel_start, sel_to )


    def _GetInsertionPoint(self):
        return self.GetInsertionPoint()

    def _SetInsertionPoint(self, pos):
        self.SetInsertionPoint(pos)


    def _GetValue(self):
        """
        Allow mixin to get the raw value of the control with this function.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        return self.GetValue()

    def _SetValue(self, value):
        """
        Allow mixin to set the raw value of the control with this function.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        # For wxComboBox, ensure that values are properly padded so that
        # if varying length choices are supplied, they always show up
        # in the window properly, and will be the appropriate length
        # to match the mask:
        if self._ctrl_constraints._alignRight:
            value = value.rjust(len(self._mask))
        else:
            value = value.ljust(len(self._mask))
        wxComboBox.SetValue(self, value)

    def SetValue(self, value):
        """
        This function redefines the externally accessible .SetValue to be
        a smart "paste" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """

        # empty previous contents, replacing entire value:
        self._SetInsertionPoint(0)
        self._SetSelection(0, len(self._mask))

        # make SetValue behave the same as if you had typed the value in:
        value = self._Paste(value, raise_on_invalid=True, just_return_value=True)
        if self._isDec:
            self._isNeg = False     # (clear current assumptions)
            value = self._adjustDec(value)
        elif self._isInt:
            self._isNeg = False     # (clear current assumptions)
            value = self._adjustInt(value)
        elif self._isDate:
            value = self._adjustDate(value)

        self._SetValue(value)

    def _Refresh(self):
        """
        Allow mixin to refresh the base control with this function.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        wxComboBox.Refresh(self)

    def Refresh(self):
        """
        This function redefines the externally accessible .Refresh() to
        validate the contents of the masked control as it refreshes.
        NOTE: this must be done in the class derived from the base wx control.
        """
        self._CheckValid()
        self._Refresh()


    def _IsEditable(self):
        """
        Allow mixin to determine if the base control is editable with this function.
        REQUIRED by any class derived from wxMaskedEditMixin.
        """
        return not self.__readonly


    def Cut(self):
        """
        This function redefines the externally accessible .Cut to be
        a smart "erase" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """
        self._Cut()     # call the mixin's Cut method


    def Paste(self):
        """
        This function redefines the externally accessible .Paste to be
        a smart "paste" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """
        self._Paste()   # call the mixin's Paste method


    def Append( self, choice ):
        """
        This function override is necessary so we can keep track of any additions to the list
        of choices, because wxComboBox doesn't have an accessor for the choice list.
        """
        if self._ctrl_constraints._alignRight:
            choice = choice.rjust(len(self._mask))
        else:
            choice = choice.ljust(len(self._mask))

        if self._ctrl_constraints._choiceRequired:
            choice = choice.lower().strip()
        self._choices.append(choice)

        if not self.IsValid(choice):
            raise ValueError('%s: "%s" is not a valid value for the control as specified.' % (self.name, choice))


        wxComboBox.Append(self, choice)


    def Clear( self ):
        """
        This function override is necessary so we can keep track of any additions to the list
        of choices, because wxComboBox doesn't have an accessor for the choice list.
        """
        self._choices = []
        if self._ctrl_constraints._choices:
            self.SetMaskParameters(choices=[])
        wxComboBox.Clear(self)


    def GetMark(self):
        """
        This function is a hack to make up for the fact that wxComboBox has no
        method for returning the selected portion of its edit control.  It
        works, but has the nasty side effect of generating lots of intermediate
        events.
        """
        dbg(suspend=1)  # turn off debugging around this function
        dbg('wxMaskedComboBox::GetMark', indent=1)
        sel_start = sel_to = self.GetInsertionPoint()
        dbg("current sel_start:", sel_start)
        value = self.GetValue()
        dbg('value: "%s"' % value)

        self._ignoreChange = True               # tell _OnTextChange() to ignore next event (if any)

        wxComboBox.Cut(self)
        newvalue = self.GetValue()
        dbg("value after Cut operation:", newvalue)

        if newvalue != value:                   # something was selected; calculate extent
            dbg("something selected")
            sel_to = sel_start + len(value) - len(newvalue)
            wxComboBox.SetValue(self, value)    # restore original value and selection (still ignoring change)
            wxComboBox.SetInsertionPoint(self, sel_start)
            wxComboBox.SetMark(self, sel_start, sel_to)

        self._ignoreChange = False              # tell _OnTextChange() to pay attn again

        dbg('computed selection:', sel_start, sel_to, indent=0)
        dbg(suspend=0)  # resume regular debugging
        return sel_start, sel_to


    def OnKeyDown(self, event):
        """
        This function is necessary because navigation and control key
        events do not seem to normally be seen by the wxComboBox's
        EVT_CHAR routine.  (Tabs don't seem to be visible no matter
        what... {:-( )
        """
        if event.GetKeyCode() in self._nav + self._control:
            self._OnChar(event)
            return
        else:
            event.Skip()    # let mixin default KeyDown behavior occur


    def OnSelectChoice(self, event):
        """
        This function appears to be necessary, because the processing done
        on the text of the control somehow interferes with the combobox's
        selection mechanism for the arrow keys.
        """
        dbg('wxMaskedComboBox::OnSelectChoice', indent=1)

        # force case-insensitive comparison for matching purposes:
        value = self.GetValue().lower().strip()
        if event.GetKeyCode() == WXK_UP:
            direction = -1
        else:
            direction = 1
        match_index = self._autoComplete(direction, self._choices, value, self._ctrl_constraints._compareNoCase)
        if match_index is not None:
            dbg('setting selection to', match_index)
            self.SetSelection(match_index)
            # issue appropriate event to outside:
            self.GetEventHandler().ProcessEvent(
                wxMaskedComboBoxSelectEvent( self.GetId(), match_index, self ) )
            self._CheckValid()
            keep_processing = False
        else:
            pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
            field = self._FindField(pos)
            if self.IsEmpty() or not field._hasList:
                dbg('selecting 1st value in list')
                self.SetSelection(0)
                self.GetEventHandler().ProcessEvent(
                        wxMaskedComboBoxSelectEvent( self.GetId(), 0, self ) )
                self._CheckValid()
                keep_processing = False
            else:
                # attempt field-level auto-complete
                dbg(indent=0)
                keep_processing = self._OnAutoCompleteField(event)
        dbg(indent=0)
        return keep_processing


## ---------- ---------- ---------- ---------- ---------- ---------- ----------

class wxIpAddrCtrl( wxMaskedTextCtrl ):
    """
    This class is a particular type of wxMaskedTextCtrl that accepts
    and understands the semantics of IP addresses, reformats input
    as you move from field to field, and accepts '.' as a navigation
    character, so that typing an IP address can be done naturally.
    """
    def __init__( self, parent, id=-1, value = '',
                  pos = wxDefaultPosition,
                  size = wxDefaultSize,
                  style = wxTE_PROCESS_TAB,
                  validator = wxDefaultValidator,
                  name = 'wxIpAddrCtrl',
                  setupEventHandling = True,        ## setup event handling by default
                  **kwargs):

        if not kwargs.has_key('mask'):
            kwargs['mask'] = mask = "###.###.###.###"
        if not kwargs.has_key('formatcodes'):
            kwargs['formatcodes'] = 'F_Sr<'
        if not kwargs.has_key('validRegex'):
            kwargs['validRegex'] = "(  \d| \d\d|(1\d\d|2[0-4]\d|25[0-5]))(\.(  \d| \d\d|(1\d\d|2[0-4]\d|25[0-5]))){3}"

        if not kwargs.has_key('emptyInvalid'):
            kwargs['emptyInvalid'] = True

        wxMaskedTextCtrl.__init__(
                self, parent, id=id, value = value,
                pos=pos, size=size,
                style = style,
                validator = validator,
                name = name,
                setupEventHandling = setupEventHandling,
                **kwargs)

        field_params = {}
        if not kwargs.has_key('validRequired'):
            field_params['validRequired'] = True

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
        dbg('wxIpAddrCtrl::OnDot', indent=1)
        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
        oldvalue = self.GetValue()
        edit_start, edit_end, slice = self._FindFieldExtent(pos, getslice=True)
        if not event.ShiftDown():
            if pos < edit_end:
                # clip data in field to the right of pos, if adjusting fields
                # when not at delimeter; (assumption == they hit '.')
                newvalue = oldvalue[:pos] + ' ' * (edit_end - pos) + oldvalue[edit_end:]
                self._SetValue(newvalue)
                self._SetInsertionPoint(pos)
        dbg(indent=0)
        return self._OnChangeField(event)



    def GetAddress(self):
        value = wxMaskedTextCtrl.GetValue(self)
        return value.replace(' ','')    # remove spaces from the value


    def _OnCtrl_S(self, event):
        dbg("wxIpAddrCtrl::_OnCtrl_S")
        if self._demo:
            print "value:", self.GetAddress()
        return False



## ---------- ---------- ---------- ---------- ---------- ---------- ----------
## these are helper subroutines:

def movetodec( origvalue, fmtstring, neg, addseparators=False, sepchar = ',',fillchar=' '):
    """ addseparators = add separator character every three numerals if True
    """
    fmt0 = fmtstring.split('.')
    fmt1 = fmt0[0]
    fmt2 = fmt0[1]
    val  = origvalue.split('.')[0].strip()
    ret  = fillchar * (len(fmt1)-len(val)) + val + "." + "0" * len(fmt2)
    if neg:
        ret = '-' + ret[1:]
    return (ret,len(fmt1))


def isDateType( fmtstring ):
    """ Checks the mask and returns True if it fits an allowed
        date or datetime format.
    """
    dateMasks = ("^##/##/####",
                 "^##-##-####",
                 "^##.##.####",
                 "^####/##/##",
                 "^####-##-##",
                 "^####.##.##",
                 "^##/CCC/####",
                 "^##.CCC.####",
                 "^##/##/##$",
                 "^##/##/## ",
                 "^##/CCC/##$",
                 "^##.CCC.## ",)
    reString  = "|".join(dateMasks)
    filter = re.compile( reString)
    if re.match(filter,fmtstring): return True
    return False

def isTimeType( fmtstring ):
    """ Checks the mask and returns True if it fits an allowed
        time format.
    """
    reTimeMask = "^##:##(:##)?( (AM|PM))?"
    filter = re.compile( reTimeMask )
    if re.match(filter,fmtstring): return True
    return False


def isDecimal( fmtstring ):
    filter = re.compile("[ ]?[#]+\.[#]+\n")
    if re.match(filter,fmtstring+"\n"): return True
    return False


def isInteger( fmtstring ):
    filter = re.compile("[#]+\n")
    if re.match(filter,fmtstring+"\n"): return True
    return False


def getDateParts( dateStr, dateFmt ):
    if len(dateStr) > 11: clip = dateStr[0:11]
    else:                 clip = dateStr
    if clip[-2] not in string.digits:
        clip = clip[:-1]    # (got part of time; drop it)

    dateSep = (('/' in clip) * '/') + (('-' in clip) * '-') + (('.' in clip) * '.')
    slices  = clip.split(dateSep)
    if dateFmt == "MDY":
        y,m,d = (slices[2],slices[0],slices[1])  ## year, month, date parts
    elif dateFmt == "DMY":
        y,m,d = (slices[2],slices[1],slices[0])  ## year, month, date parts
    elif dateFmt == "YMD":
        y,m,d = (slices[0],slices[1],slices[2])  ## year, month, date parts
    else:
        y,m,d = None, None, None
    if not y:
        return None
    else:
        return y,m,d


def getDateSepChar(dateStr):
    clip   = dateStr[0:10]
    dateSep = (('/' in clip) * '/') + (('-' in clip) * '-') + (('.' in clip) * '.')
    return dateSep


def makeDate( year, month, day, dateFmt, dateStr):
    sep    = getDateSepChar( dateStr)
    if dateFmt == "MDY":
        return "%s%s%s%s%s" % (month,sep,day,sep,year)  ## year, month, date parts
    elif dateFmt == "DMY":
        return "%s%s%s%s%s" % (day,sep,month,sep,year)  ## year, month, date parts
    elif dateFmt == "YMD":
        return "%s%s%s%s%s" % (year,sep,month,sep,day)  ## year, month, date parts
    else:
        return none


def getYear(dateStr,dateFmt):
    parts = getDateParts( dateStr, dateFmt)
    return parts[0]

def getMonth(dateStr,dateFmt):
    parts = getDateParts( dateStr, dateFmt)
    return parts[1]

def getDay(dateStr,dateFmt):
    parts = getDateParts( dateStr, dateFmt)
    return parts[2]

## ---------- ---------- ---------- ---------- ---------- ---------- ----------
class test(wxPySimpleApp):
        def OnInit(self):
            from wxPython.lib.rcsizer import RowColSizer
            self.frame = wxFrame( NULL, -1, "wxMaskedEditMixin 0.0.7 Demo Page #1", size = (700,600))
            self.panel = wxPanel( self.frame, -1)
            self.sizer = RowColSizer()
            self.labels = []
            self.editList  = []
            rowcount    = 4

            id, id1 = wxNewId(), wxNewId()
            self.command1  = wxButton( self.panel, id, "&Close" )
            self.command2  = wxButton( self.panel, id1, "&AutoFormats" )
            self.sizer.Add(self.command1, row=0, col=0, flag=wxALL, border = 5)
            self.sizer.Add(self.command2, row=0, col=1, colspan=2, flag=wxALL, border = 5)
            EVT_BUTTON( self.panel, id, self.onClick )
##            self.panel.SetDefaultItem(self.command1 )
            EVT_BUTTON( self.panel, id1, self.onClickPage )

            self.check1 = wxCheckBox( self.panel, -1, "Disallow Empty" )
            self.check2 = wxCheckBox( self.panel, -1, "Highlight Empty" )
            self.sizer.Add( self.check1, row=0,col=3, flag=wxALL,border=5 )
            self.sizer.Add( self.check2, row=0,col=4, flag=wxALL,border=5 )
            EVT_CHECKBOX( self.panel, self.check1.GetId(), self._onCheck1 )
            EVT_CHECKBOX( self.panel, self.check2.GetId(), self._onCheck2 )


            label = """Press ctrl-s in any field to output the value and plain value. Press ctrl-x to clear and re-set any field.
Note that all controls have been auto-sized by including F in the format code.
Try entering nonsensical or partial values in validated fields to see what happens (use ctrl-s to test the valid status)."""
            label2 = "\nNote that the State and Last Name fields are list-limited (Name:Smith,Jones,Williams)."

            self.label1 = wxStaticText( self.panel, -1, label)
            self.label2 = wxStaticText( self.panel, -1, "Description")
            self.label3 = wxStaticText( self.panel, -1, "Mask Value")
            self.label4 = wxStaticText( self.panel, -1, "Format")
            self.label5 = wxStaticText( self.panel, -1, "Reg Expr Val. (opt)")
            self.label6 = wxStaticText( self.panel, -1, "wxMaskedEdit Ctrl")
            self.label7 = wxStaticText( self.panel, -1, label2)
            self.label7.SetForegroundColour("Blue")
            self.label1.SetForegroundColour("Blue")
            self.label2.SetFont(wxFont(9,wxSWISS,wxNORMAL,wxBOLD))
            self.label3.SetFont(wxFont(9,wxSWISS,wxNORMAL,wxBOLD))
            self.label4.SetFont(wxFont(9,wxSWISS,wxNORMAL,wxBOLD))
            self.label5.SetFont(wxFont(9,wxSWISS,wxNORMAL,wxBOLD))
            self.label6.SetFont(wxFont(9,wxSWISS,wxNORMAL,wxBOLD))

            self.sizer.Add( self.label1, row=1,col=0,colspan=7, flag=wxALL,border=5)
            self.sizer.Add( self.label7, row=2,col=0,colspan=7, flag=wxALL,border=5)
            self.sizer.Add( self.label2, row=3,col=0, flag=wxALL,border=5)
            self.sizer.Add( self.label3, row=3,col=1, flag=wxALL,border=5)
            self.sizer.Add( self.label4, row=3,col=2, flag=wxALL,border=5)
            self.sizer.Add( self.label5, row=3,col=3, flag=wxALL,border=5)
            self.sizer.Add( self.label6, row=3,col=4, flag=wxALL,border=5)

            # The following list is of the controls for the demo. Feel free to play around with
            # the options!
            controls = [
            #description        mask                    excl format     regexp                              range,list,initial
           ("Phone No",         "(###) ###-#### x:###", "", 'F!^-R',    "^\(\d\d\d\) \d\d\d-\d\d\d\d",    (),[],''),
           ("Last Name Only",   "C{14}",                "", 'F {list}', '^[A-Z][a-zA-Z]+',                  (),('Smith','Jones','Williams'),''),
           ("Full Name",        "C{14}",                "", 'F_',       '^[A-Z][a-zA-Z]+ [A-Z][a-zA-Z]+',   (),[],''),
           ("Social Sec#",      "###-##-####",          "", 'F',        "\d{3}-\d{2}-\d{4}",                (),[],''),
           ("U.S. Zip+4",       "#{5}-#{4}",            "", 'F',        "\d{5}-(\s{4}|\d{4})",(),[],''),
           ("U.S. State (2 char)\n(with default)","AA",                 "", 'F!',       "[A-Z]{2}",                         (),states, 'AZ'),
           ("Customer No",      "\CAA-###",              "", 'F!',      "C[A-Z]{2}-\d{3}",                   (),[],''),
           ("Date (MDY) + Time\n(with default)",      "##/##/#### ##:## AM",  'BCDEFGHIJKLMNOQRSTUVWXYZ','DFR!',"",                (),[], r'03/05/2003 12:00 AM'),
           ("Invoice Total",    "#{9}.##",              "", 'F-R,',     "",                                 (),[], ''),
           ("Integer (signed)\n(with default)", "#{6}",                 "", 'F-R',      "",                                 (),[], '0     '),
           ("Integer (unsigned)\n(with default), 1-399", "######",      "", 'F',        "",                                 (1,399),[], '1     '),
           ("Month selector",   "XXX",                  "", 'F',        "",                                 (),
                ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'],""),
           ("fraction selector","#/##",                 "", 'F',        "^\d\/\d\d?",                       (),
                ['2/3', '3/4', '1/2', '1/4', '1/8', '1/16', '1/32', '1/64'], "")
           ]

            for control in controls:
                self.sizer.Add( wxStaticText( self.panel, -1, control[0]),row=rowcount, col=0,border=5,flag=wxALL)
                self.sizer.Add( wxStaticText( self.panel, -1, control[1]),row=rowcount, col=1,border=5, flag=wxALL)
                self.sizer.Add( wxStaticText( self.panel, -1, control[3]),row=rowcount, col=2,border=5, flag=wxALL)
                self.sizer.Add( wxStaticText( self.panel, -1, control[4][:20]),row=rowcount, col=3,border=5, flag=wxALL)

                if control in controls[:]:#-2]:
                    newControl  = wxMaskedTextCtrl( self.panel, -1, "",
                                                    mask         = control[1],
                                                    excludeChars = control[2],
                                                    formatcodes  = control[3],
                                                    includeChars = "",
                                                    validRegex   = control[4],
                                                    validRange   = control[5],
                                                    choices      = control[6],
                                                    defaultValue = control[7],
                                                    demo         = True)
                    if control[6]: newControl.SetMaskParameters(choiceRequired = True)
                else:
                    newControl = wxMaskedComboBox(  self.panel, -1, "",
                                                    choices = control[7],
                                                    choiceRequired  = True,
                                                    mask         = control[1],
                                                    formatcodes  = control[3],
                                                    excludeChars = control[2],
                                                    includeChars = "",
                                                    validRegex   = control[4],
                                                    validRange   = control[5],
                                                    demo         = True)
                self.editList.append( newControl )

                self.sizer.Add( newControl, row=rowcount,col=4,flag=wxALL,border=5)
                rowcount += 1

            self.sizer.AddGrowableCol(4)

            self.panel.SetSizer(self.sizer)
            self.panel.SetAutoLayout(1)

            self.frame.Show(1)
            self.MainLoop()

            return True

        def onClick(self, event):
            self.frame.Close()

        def onClickPage(self, event):
            self.page2 = test2(self.frame,-1,"")
            self.page2.Show(True)

        def _onCheck1(self,event):
            """ Set required value on/off """
            value = event.Checked()
            if value:
                for control in self.editList:
                    control.SetMaskParameters(emptyInvalid=True)
                    control.Refresh()
            else:
                for control in self.editList:
                    control.SetMaskParameters(emptyInvalid=False)
                    control.Refresh()
            self.panel.Refresh()

        def _onCheck2(self,event):
            """ Highlight empty values"""
            value = event.Checked()
            if value:
                for control in self.editList:
                    control.SetMaskParameters( emptyBackgroundColor = 'Aquamarine')
                    control.Refresh()
            else:
                for control in self.editList:
                    control.SetMaskParameters( emptyBackgroundColor = 'White')
                    control.Refresh()
            self.panel.Refresh()


## ---------- ---------- ---------- ---------- ---------- ---------- ----------

class test2(wxFrame):
        def __init__(self, parent, id, caption):
            wxFrame.__init__( self, parent, id, "wxMaskedEdit control 0.0.7 Demo Page #2 -- AutoFormats", size = (550,600))
            from wxPython.lib.rcsizer import RowColSizer
            self.panel = wxPanel( self, -1)
            self.sizer = RowColSizer()
            self.labels = []
            self.texts  = []
            rowcount    = 4

            label = """\
All these controls have been created by passing a single parameter, the AutoFormat code.
The class contains an internal dictionary of types and formats (autoformats).
To see a great example of validations in action, try entering a bad email address, then tab out."""

            self.label1 = wxStaticText( self.panel, -1, label)
            self.label2 = wxStaticText( self.panel, -1, "Description")
            self.label3 = wxStaticText( self.panel, -1, "AutoFormat Code")
            self.label4 = wxStaticText( self.panel, -1, "wxMaskedEdit Control")
            self.label1.SetForegroundColour("Blue")
            self.label2.SetFont(wxFont(9,wxSWISS,wxNORMAL,wxBOLD))
            self.label3.SetFont(wxFont(9,wxSWISS,wxNORMAL,wxBOLD))
            self.label4.SetFont(wxFont(9,wxSWISS,wxNORMAL,wxBOLD))

            self.sizer.Add( self.label1, row=1,col=0,colspan=3, flag=wxALL,border=5)
            self.sizer.Add( self.label2, row=3,col=0, flag=wxALL,border=5)
            self.sizer.Add( self.label3, row=3,col=1, flag=wxALL,border=5)
            self.sizer.Add( self.label4, row=3,col=2, flag=wxALL,border=5)

            id, id1 = wxNewId(), wxNewId()
            self.command1  = wxButton( self.panel, id, "&Close")
            self.command2  = wxButton( self.panel, id1, "&Print Formats")
            EVT_BUTTON( self.panel, id, self.onClick)
            self.panel.SetDefaultItem(self.command1)
            EVT_BUTTON( self.panel, id1, self.onClickPrint)

            # The following list is of the controls for the demo. Feel free to play around with
            # the options!
            controls = [
           ("Phone No","USPHONEFULLEXT"),
           ("US Date + Time","USDATETIMEMMDDYYYY/HHMM"),
           ("US Date MMDDYYYY","USDATEMMDDYYYY/"),
           ("Time (with seconds)","TIMEHHMMSS"),
           ("Military Time\n(without seconds)","MILTIMEHHMM"),
           ("Social Sec#","USSOCIALSEC"),
           ("Credit Card","CREDITCARD"),
           ("Expiration MM/YY","EXPDATEMMYY"),
           ("Percentage","PERCENT"),
           ("Person's Age","AGE"),
           ("US Zip Code","USZIP"),
           ("US Zip+4","USZIPPLUS4"),
           ("Email Address","EMAIL"),
           ("IP Address", "(derived control wxIpAddrCtrl)")
           ]

            for control in controls:
                self.sizer.Add( wxStaticText( self.panel, -1, control[0]),row=rowcount, col=0,border=5,flag=wxALL)
                self.sizer.Add( wxStaticText( self.panel, -1, control[1]),row=rowcount, col=1,border=5, flag=wxALL)
                if control in controls[:-1]:
                    self.sizer.Add( wxMaskedTextCtrl( self.panel, -1, "",
                                                      autoformat  = control[1],
                                                      demo        = True),
                                row=rowcount,col=2,flag=wxALL,border=5)
                else:
                    self.sizer.Add( wxIpAddrCtrl( self.panel, -1, "", demo=True ),
                                    row=rowcount,col=2,flag=wxALL,border=5)
                rowcount += 1

            self.sizer.Add(self.command1, row=0, col=0, flag=wxALL, border = 5)
            self.sizer.Add(self.command2, row=0, col=1, flag=wxALL, border = 5)
            self.sizer.AddGrowableCol(3)

            self.panel.SetSizer(self.sizer)
            self.panel.SetAutoLayout(1)

        def onClick(self, event):
            self.Close()

        def onClickPrint(self, event):
            for format in masktags.keys():
                sep = "+------------------------+"
                print "%s\n%s  \n  Mask: %s \n  RE Validation string: %s\n" % (sep,format, masktags[format][0], masktags[format][3])

## ---------- ---------- ---------- ---------- ---------- ---------- ----------

if __name__ == "__main__":
    app = test()

i=1
##
## Current Issues:
## ===================================
##
## 1. WS: For some reason I don't understand, the control is generating two (2)
##      EVT_TEXT events for every one (1) .SetValue() of the underlying control.
##      I've been unsuccessful in determining why or in my efforts to make just one
##      occur.  So, I've added a hack to save the last seen value from the
##      control in the EVT_TEXT handler, and if *different*, call event.Skip()
##      to propagate it down the event chain, and let the application see it.
##
## 2. WS: wxMaskedComboBox is deficient in several areas, all having to do with the
##      behavior of the underlying control that I can't fix.  The problems are:
##      a) The background coloring doesn't work in the text field of the control;
##         instead, there's a only border around it that assumes the correct color.
##      b) The control will not pass WXK_TAB to the event handler, no matter what
##         I do, and there's no style wxCB_PROCESS_TAB like wxTE_PROCESS_TAB to
##         indicate that we want these events.  As a result, wxMaskedComboBox
##         doesn't do the nice field-tabbing that wxMaskedTextCtrl does.
##      c) Auto-complete had to be reimplemented for the control because programmatic
##         setting of the value of the text field does not set up the auto complete
##         the way that the control processing keystrokes does.  (But I think I've
##         implemented a fairly decent approximation.)  Because of this the control
##         also won't auto-complete on dropdown, and there's no event I can catch
##         to work around this problem.
##      d) There is no method provided for getting the selection; the hack I've
##         implemented has its flaws, not the least of which is that due to the
##         strategy that I'm using, the paste buffer is always replaced by the
##         contents of the control's selection when in focus, on each keystroke;
##         this makes it impossible to paste anything into a wxMaskedComboBox
##         at the moment... :-(
##      e) The other deficient behavior, likely induced by the workaround for (d),
##         is that you can can't shift-left to select more than one character
##         at a time.
##
##
## 3. WS: Controls on wxPanels don't seem to pass Shift-WXK_TAB to their
##      EVT_KEY_DOWN or EVT_CHAR event handlers.  Until this is fixed in
##      wxWindows, shift-tab won't take you backwards through the fields of
##      a wxMaskedTextCtrl like it should.  Until then Shifted arrow keys will
##      work like shift-tab and tab ought to.
##

## To-Do's:
## =============================##
##  1. Add Popup list for auto-completable fields that simulates combobox on individual
##     fields.  Example: City validates against list of cities, or zip vs zip code list.
##  2. Allow optional monetary symbols (eg. $, pounds, etc.) at front of a "decimal"
##     control.
##  3. Fix shift-left selection for wxMaskedComboBox.


## CHANGELOG:
## ====================
##  Version 1.2
##   1. Fixed .SetValue() to replace the current value, rather than the current
##      selection. Also changed it to generate ValueError if presented with
##      either a value which doesn't follow the format or won't fit.  Also made
##      set value adjust numeric and date controls as if user entered the value.
##      Expanded doc explaining how SetValue() works.
##   2. Fixed EUDATE* autoformats, fixed IsDateType mask list, and added ability to
##      use 3-char months for dates, and EUDATETIME, and EUDATEMILTIME autoformats.
##   3. Made all date autoformats automatically pick implied "datestyle".
##   4. Added IsModified override, since base wxTextCtrl never reports modified if
##      .SetValue used to change the value, which is what the masked edit controls
##      use internally.
##   5. Fixed bug in date position adjustment on 2 to 4 digit date conversion when
##      using tab to "leave field" and auto-adjust.
##   6. Fixed bug in _isCharAllowed() for negative number insertion on pastes,
##      and bug in ._Paste() that didn't account for signs in signed masks either.
##   7. Fixed issues with _adjustPos for right-insert fields causing improper
##      selection/replacement of values
##   8. Fixed _OnHome handler to properly handle extending current selection to
##      beginning of control.
##   9. Exposed all (valid) autoformats to demo, binding descriptions to
##      autoformats.
##  10. Fixed a couple of bugs in email regexp.
##  11. Modified autoformats to be more amenable to international use.
##  12. Clarified meaning of '-' formatcode in doc.
##  13. Fixed a couple of coding bugs being flagged by Python2.1.
##  14. Fixed several issues with sign positioning, erasure and validity
##      checking for "numeric" masked controls.
##
##  Version 1.1
##   1. Changed calling interface to use boolean "useFixedWidthFont" (True by default)
##      vs. literal font facename, and use wxTELETYPE as the font family
##      if so specified.
##   2. Switched to use of dbg module vs. locally defined version.
##   3. Revamped entire control structure to use Field classes to hold constraint
##      and formatting data, to make code more hierarchical, allow for more
##      sophisticated masked edit construction.
##   4. Better strategy for managing options, and better validation on keywords.
##   5. Added 'V' format code, which requires that in order for a character
##      to be accepted, it must result in a string that passes the validRegex.
##   6. Added 'S' format code which means "select entire field when navigating
##      to new field."
##   7. Added 'r' format code to allow "right-insert" fields. (implies 'R'--right-alignment)
##   8. Added '<' format code to allow fields to require explicit cursor movement
##      to leave field.
##   9. Added validFunc option to other validation mechanisms, that allows derived
##      classes to add dynamic validation constraints to the control.
##  10. Fixed bug in validatePaste code causing possible IndexErrors, and also
##      fixed failure to obey case conversion codes when pasting.
##  11. Implemented '0' (zero-pad) formatting code, as it wasn't being done anywhere...
##  12. Removed condition from OnDecimalPoint, so that it always truncates right on '.'
##  13. Enhanced wxIpAddrCtrl to use right-insert fields, selection on field traversal,
##      individual field validation to prevent field values > 255, and require explicit
##      tab/. to change fields.
##  14. Added handler for left double-click to select field under cursor.
##  15. Fixed handling for "Read-only" styles.
##  16. Separated signedForegroundColor from 'R' style, and added foregroundColor
##      attribute, for more consistent and controllable coloring.
##  17. Added retainFieldValidation parameter, allowing top-level constraints
##      such as "validRequired" to be set independently of field-level equivalent.
##      (needed in wxTimeCtrl for bounds constraints.)
##  18. Refactored code a bit, cleaned up and commented code more heavily, fixed
##      some of the logic for setting/resetting parameters, eg. fillChar, defaultValue,
##      etc.
##  19. Fixed maskchar setting for upper/lowercase, to work in all locales.
##
##
##  Version 1.0
##   1. Decimal point behavior restored for decimal and integer type controls:
##      decimal point now trucates the portion > 0.
##   2. Return key now works like the tab character and moves to the next field,
##      provided no default button is set for the form panel on which the control
##      resides.
##   3. Support added in _FindField() for subclasses controls (like timecontrol)
##      to determine where the current insertion point is within the mask (i.e.
##      which sub-'field'). See method documentation for more info and examples.
##   4. Added Field class and support for all constraints to be field-specific
##      in addition to being globally settable for the control.
##      Choices for each field are validated for length and pastability into
##      the field in question, raising ValueError if not appropriate for the control.
##      Also added selective additional validation based on individual field constraints.
##      By default, SHIFT-WXK_DOWN, SHIFT-WXK_UP, WXK_PRIOR and WXK_NEXT all
##      auto-complete fields with choice lists, supplying the 1st entry in
##      the choice list if the field is empty, and cycling through the list in
##      the appropriate direction if already a match.  WXK_DOWN will also auto-
##      complete if the field is partially completed and a match can be made.
##      SHIFT-WXK_UP/DOWN will also take you to the next field after any
##      auto-completion performed.
##   5. Added autoCompleteKeycodes=[] parameters for allowing further
##      customization of the control.  Any keycode supplied as a member
##      of the _autoCompleteKeycodes list will be treated like WXK_NEXT.  If
##      requireFieldChoice is set, then a valid value from each non-empty
##      choice list will be required for the value of the control to validate.
##   6. Fixed "auto-sizing" to be relative to the font actually used, rather
##      than making assumptions about character width.
##   7. Fixed GetMaskParameter(), which was non-functional in previous version.
##   8. Fixed exceptions raised to provide info on which control had the error.
##   9. Fixed bug in choice management of wxMaskedComboBox.
##  10. Fixed bug in wxIpAddrCtrl causing traceback if field value was of
##     the form '# #'.  Modified control code for wxIpAddrCtrl so that '.'
##     in the middle of a field clips the rest of that field, similar to
##     decimal and integer controls.
##
##
##  Version 0.0.7
##   1. "-" is a toggle for sign; "+" now changes - signed numerics to positive.
##   2. ',' in formatcodes now causes numeric values to be comma-delimited (e.g.333,333).
##   3. New support for selecting text within the control.(thanks Will Sadkin!)
##      Shift-End and Shift-Home now select text as you would expect
##      Control-Shift-End selects to the end of the mask string, even if value not entered.
##      Control-A selects all *entered* text, Shift-Control-A selects everything in the control.
##   4. event.Skip() added to onKillFocus to correct remnants when running in Linux (contributed-
##      for some reason I couldn't find the original email but thanks!!!)
##   5. All major key-handling code moved to their own methods for easier subclassing: OnHome,
##      OnErase, OnEnd, OnCtrl_X, OnCtrl_A, etc.
##   6. Email and autoformat validations corrected using regex provided by Will Sadkin (thanks!).
##   (The rest of the changes in this version were done by Will Sadkin with permission from Jeff...)
##   7. New mechanism for replacing default behavior for any given key, using
##      ._SetKeycodeHandler(keycode, func) and ._SetKeyHandler(char, func) now available
##      for easier subclassing of the control.
##   8. Reworked the delete logic, cut, paste and select/replace logic, as well as some bugs
##      with insertion point/selection modification.  Changed Ctrl-X to use standard "cut"
##      semantics, erasing the selection, rather than erasing the entire control.
##   9. Added option for an "default value" (ie. the template) for use when a single fillChar
##      is not desired in every position.  Added IsDefault() function to mean "does the value
##      equal the template?" and modified .IsEmpty() to mean "do all of the editable
##      positions in the template == the fillChar?"
##  10. Extracted mask logic into mixin, so we can have both wxMaskedTextCtrl and wxMaskedComboBox,
##      now included.
##  11. wxMaskedComboBox now adds the capability to validate from list of valid values.
##      Example: City validates against list of cities, or zip vs zip code list.
##  12. Fixed oversight in EVT_TEXT handler that prevented the events from being
##      passed to the next handler in the event chain, causing updates to the
##      control to be invisible to the parent code.
##  13. Added IPADDR autoformat code, and subclass wxIpAddrCtrl for controlling tabbing within
##      the control, that auto-reformats as you move between cells.
##  14. Mask characters [A,a,X,#] can now appear in the format string as literals, by using '\'.
##  15. It is now possible to specify repeating masks, e.g. #{3}-#{3}-#{14}
##  16. Fixed major bugs in date validation, due to the fact that
##      wxDateTime.ParseDate is too liberal, and will accept any form that
##      makes any kind of sense, regardless of the datestyle you specified
##      for the control.  Unfortunately, the strategy used to fix it only
##      works for versions of wxPython post 2.3.3.1, as a C++ assert box
##      seems to show up on an invalid date otherwise, instead of a catchable
##      exception.
##  17. Enhanced date adjustment to automatically adjust heuristic based on
##      current year, making last century/this century determination on
##      2-digit year based on distance between today's year and value;
##      if > 50 year separation, assume last century (and don't assume last
##      century is 20th.)
##  18. Added autoformats and support for including HHMMSS as well as HHMM for
##      date times, and added similar time, and militaray time autoformats.
##  19. Enhanced tabbing logic so that tab takes you to the next field if the
##      control is a multi-field control.
##  20. Added stub method called whenever the control "changes fields", that
##      can be overridden by subclasses (eg. wxIpAddrCtrl.)
##  21. Changed a lot of code to be more functionally-oriented so side-effects
##      aren't as problematic when maintaining code and/or adding features.
##      Eg: IsValid() now does not have side-effects; it merely reflects the
##      validity of the value of the control; to determine validity AND recolor
##      the control, _CheckValid() should be used with a value argument of None.
##      Similarly, made most reformatting function take an optional candidate value
##      rather than just using the current value of the control, and only
##      have them change the value of the control if a candidate is not specified.
##      In this way, you can do validation *before* changing the control.
##  22. Changed validRequired to mean "disallow chars that result in invalid
##      value."  (Old meaning now represented by emptyInvalid.)  (This was
##      possible once I'd made the changes in (19) above.)
##  23. Added .SetMaskParameters and .GetMaskParameter methods, so they
##      can be set/modified/retrieved after construction.  Removed individual
##      parameter setting functions, in favor of this mechanism, so that
##      all adjustment of the control based on changing parameter values can
##      be handled in one place with unified mechanism.
##  24. Did a *lot* of testing and fixing re: numeric values.  Added ability
##      to type "grouping char" (ie. ',') and validate as appropriate.
##  25. Fixed ZIPPLUS4 to allow either 5 or 4, but if > 5 must be 9.
##  26. Fixed assumption about "decimal or integer" masks so that they're only
##      made iff there's no validRegex associated with the field.  (This
##      is so things like zipcodes which look like integers can have more
##      restrictive validation (ie. must be 5 digits.)
##  27. Added a ton more doc strings to explain use and derivation requirements
##      and did regularization of the naming conventions.
##  28. Fixed a range bug in _adjustKey preventing z from being handled properly.
##  29. Changed behavior of '.' (and shift-.) in numeric controls to move to
##      reformat the value and move the next field as appropriate. (shift-'.',
##      ie. '>' moves to the previous field.

##  Version 0.0.6
##   1. Fixed regex bug that caused autoformat AGE to invalidate any age ending
##      in '0'.
##   2. New format character 'D' to trigger date type. If the user enters 2 digits in the
##      year position, the control will expand the value to four digits, using numerals below
##      50 as 21st century (20+nn) and less than 50 as 20th century (19+nn).
##      Also, new optional parameter datestyle = set to one of {MDY|DMY|YDM}
##   3. revalid parameter renamed validRegex to conform to standard for all validation
##      parameters (see 2 new ones below).
##   4. New optional init parameter = validRange. Used only for int/dec (numeric) types.
##      Allows the developer to specify a valid low/high range of values.
##   5. New optional init parameter = validList. Used for character types. Allows developer
##      to send a list of values to the control to be used for specific validation.
##      See the Last Name Only example - it is list restricted to Smith/Jones/Williams.
##   6. Date type fields now use wxDateTime's parser to validate the date and time.
##      This works MUCH better than my kludgy regex!! Thanks to Robin Dunn for pointing
##      me toward this solution!
##   7. Date fields now automatically expand 2-digit years when it can. For example,
##      if the user types "03/10/67", then "67" will auto-expand to "1967". If a two-year
##      date is entered it will be expanded in any case when the user tabs out of the
##      field.
##   8. New class functions: SetValidBackgroundColor, SetInvalidBackgroundColor, SetEmptyBackgroundColor,
##      SetSignedForeColor allow accessto override default class coloring behavior.
##   9. Documentation updated and improved.
##  10. Demo - page 2 is now a wxFrame class instead of a wxPyApp class. Works better.
##      Two new options (checkboxes) - test highlight empty and disallow empty.
##  11. Home and End now work more intuitively, moving to the first and last user-entry
##      value, respectively.
##  12. New class function: SetRequired(bool). Sets the control's entry required flag
##      (i.e. disallow empty values if True).
##
##  Version 0.0.5
##   1. get_plainValue method renamed to GetPlainValue following the wxWindows
##      StudlyCaps(tm) standard (thanks Paul Moore).  ;)
##   2. New format code 'F' causes the control to auto-fit (auto-size) itself
##      based on the length of the mask template.
##   3. Class now supports "autoformat" codes. These can be passed to the class
##      on instantiation using the parameter autoformat="code". If the code is in
##      the dictionary, it will self set the mask, formatting, and validation string.
##      I have included a number of samples, but I am hoping that someone out there
##      can help me to define a whole bunch more.
##   4. I have added a second page to the demo (as well as a second demo class, test2)
##      to showcase how autoformats work. The way they self-format and self-size is,
##      I must say, pretty cool.
##   5. Comments added and some internal cosmetic revisions re: matching the code
##      standards for class submission.
##   6. Regex validation is now done in real time - field turns yellow immediately
##      and stays yellow until the entered value is valid
##   7. Cursor now skips over template characters in a more intuitive way (before the
##      next keypress).
##   8. Change, Keypress and LostFocus methods added for convenience of subclasses.
##      Developer may use these methods which will be called after EVT_TEXT, EVT_CHAR,
##      and EVT_KILL_FOCUS, respectively.
##   9. Decimal and numeric handlers have been rewritten and now work more intuitively.
##
##  Version 0.0.4
##   1. New .IsEmpty() method returns True if the control's value is equal to the
##      blank template string
##   2. Control now supports a new init parameter: revalid. Pass a regular expression
##      that the value will have to match when the control loses focus. If invalid,
##      the control's BackgroundColor will turn yellow, and an internal flag is set (see next).
##   3. Demo now shows revalid functionality. Try entering a partial value, such as a
##      partial social security number.
##   4. New .IsValid() value returns True if the control is empty, or if the value matches
##      the revalid expression. If not, .IsValid() returns False.
##   5. Decimal values now collapse to decimal with '.00' on losefocus if the user never
##      presses the decimal point.
##   6. Cursor now goes to the beginning of the field if the user clicks in an
##      "empty" field intead of leaving the insertion point in the middle of the
##      field.
##   7. New "N" mask type includes upper and lower chars plus digits. a-zA-Z0-9.
##   8. New formatcodes init parameter replaces other init params and adds functions.
##      String passed to control on init controls:
##        _ Allow spaces
##        ! Force upper
##        ^ Force lower
##        R Show negative #s in red
##        , Group digits
##        - Signed numerals
##        0 Numeric fields get leading zeros
##   9. Ctrl-X in any field clears the current value.
##   10. Code refactored and made more modular (esp in OnChar method). Should be more
##       easy to read and understand.
##   11. Demo enhanced.
##   12. Now has _doc_.
##
##  Version 0.0.3
##   1. GetPlainValue() now returns the value without the template characters;
##      so, for example, a social security number (123-33-1212) would return as
##      123331212; also removes white spaces from numeric/decimal values, so
##      "-   955.32" is returned "-955.32". Press ctrl-S to see the plain value.
##   2. Press '.' in an integer style masked control and truncate any trailing digits.
##   3. Code moderately refactored. Internal names improved for clarity. Additional
##      internal documentation.
##   4. Home and End keys now supported to move cursor to beginning or end of field.
##   5. Un-signed integers and decimals now supported.
##   6. Cosmetic improvements to the demo.
##   7. Class renamed to wxMaskedTextCtrl.
##   8. Can now specify include characters that will override the basic
##      controls: for example, includeChars = "@." for email addresses
##   9. Added mask character 'C' -> allow any upper or lowercase character
##   10. .SetSignColor(str:color) sets the foreground color for negative values
##       in signed controls (defaults to red)
##   11. Overview documentation written.
##
##  Version 0.0.2
##   1. Tab now works properly when pressed in last position
##   2. Decimal types now work (e.g. #####.##)
##   3. Signed decimal or numeric values supported (i.e. negative numbers)
##   4. Negative decimal or numeric values now can show in red.
##   5. Can now specify an "exclude list" with the excludeChars parameter.
##      See date/time formatted example - you can only enter A or P in the
##      character mask space (i.e. AM/PM).
##   6. Backspace now works properly, including clearing data from a selected
##      region but leaving template characters intact. Also delete key.
##   7. Left/right arrows now work properly.
##   8. Removed EventManager call from test so demo should work with wxPython 2.3.3
##
