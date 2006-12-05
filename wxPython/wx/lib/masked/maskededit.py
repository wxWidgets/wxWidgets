#----------------------------------------------------------------------------
# Name:         maskededit.py
# Authors:      Jeff Childers, Will Sadkin
# Email:        jchilders_98@yahoo.com, wsadkin@parlancecorp.com
# Created:      02/11/2003
# Copyright:    (c) 2003 by Jeff Childers, Will Sadkin, 2003
# Portions:     (c) 2002 by Will Sadkin, 2002-2006
# RCS-ID:       $Id$
# License:      wxWindows license
#----------------------------------------------------------------------------
# NOTE:
#   MaskedEdit controls are based on a suggestion made on [wxPython-Users] by
#   Jason Hihn, and borrows liberally from Will Sadkin's original masked edit
#   control for time entry, TimeCtrl (which is now rewritten using this
#   control!).
#
#   MaskedEdit controls do not normally use validators, because they do
#   careful manipulation of the cursor in the text window on each keystroke,
#   and validation is cursor-position specific, so the control intercepts the
#   key codes before the validator would fire.  However, validators can be
#   provided to do data transfer to the controls.
#
#----------------------------------------------------------------------------
#
# This file now contains the bulk of the logic behind all masked controls,
# the MaskedEditMixin class, the Field class, and the autoformat codes.
#
#----------------------------------------------------------------------------
#
# 03/30/2004 - Will Sadkin (wsadkin@nameconnector.com)
#
# o Split out TextCtrl, ComboBox and IpAddrCtrl into their own files,
# o Reorganized code into masked package
#
# 12/09/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace. No guarantees. This is one huge file.
#
# 12/13/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Missed wx.DateTime stuff earlier.
#
# 12/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o MaskedEditMixin -> MaskedEditMixin
# o wxMaskedTextCtrl -> maskedTextCtrl
# o wxMaskedComboBoxSelectEvent -> MaskedComboBoxSelectEvent
# o wxMaskedComboBox -> MaskedComboBox
# o wxIpAddrCtrl -> IpAddrCtrl
# o wxTimeCtrl -> TimeCtrl
#

__doc__ = """\
contains MaskedEditMixin class that drives all the other masked controls.

====================
Masked Edit Overview
====================

masked.TextCtrl:
    is a sublassed text control that can carefully control the user's input
    based on a mask string you provide.

    General usage example::

        control = masked.TextCtrl( win, -1, '', mask = '(###) ###-####')

    The example above will create a text control that allows only numbers to be
    entered and then only in the positions indicated in the mask by the # sign.

masked.ComboBox:
    is a similar subclass of wxComboBox that allows the same sort of masking,
    but also can do auto-complete of values, and can require the value typed
    to be in the list of choices to be colored appropriately.

masked.Ctrl:
    is actually a factory function for several types of masked edit controls:

    =================   ==================================================
    masked.TextCtrl     standard masked edit text box
    masked.ComboBox     adds combobox capabilities
    masked.IpAddrCtrl   adds special semantics for IP address entry
    masked.TimeCtrl     special subclass handling lots of types as values
    masked.NumCtrl      special subclass handling numeric values
    =================   ==================================================

    It works by looking for a *controlType* parameter in the keyword
    arguments of the control, to determine what kind of instance to return.
    If not specified as a keyword argument, the default control type returned
    will be masked.TextCtrl.

    Each of the above classes has its own set of arguments, but masked.Ctrl
    provides a single "unified" interface for masked controls.

What follows is a description of how to configure the generic masked.TextCtrl
and masked.ComboBox;  masked.NumCtrl and masked.TimeCtrl have their own demo 
pages and interface descriptions.

=========================

Initialization Parameters
-------------------------
mask
    Allowed mask characters and function:

    =========  ==========================================================
    Character   Function
    =========  ==========================================================
        #       Allow numeric only (0-9)
        N       Allow letters and numbers (0-9)
        A       Allow uppercase letters only
        a       Allow lowercase letters only
        C       Allow any letter, upper or lower
        X       Allow string.letters, string.punctuation, string.digits
        &       Allow string.punctuation only (doesn't include all unicode symbols)
        \*       Allow any visible character
        |       explicit field boundary (takes no space in the control; allows mix
                of adjacent mask characters to be treated as separate fields,
                eg: '&|###' means "field 0 = '&', field 1 = '###'", but there's
                no fixed characters in between.
    =========  ==========================================================


    These controls define these sets of characters using string.letters,
    string.uppercase, etc.  These sets are affected by the system locale
    setting, so in order to have the masked controls accept characters
    that are specific to your users' language, your application should
    set the locale.
    For example, to allow international characters to be used in the
    above masks, you can place the following in your code as part of
    your application's initialization code::

      import locale
      locale.setlocale(locale.LC_ALL, '')

    The controls now also support (by popular demand) all "visible" characters,
    by use of the * mask character, including unicode characters above
    the standard ANSI keycode range.
    Note:  As string.punctuation doesn't typically include all unicode
    symbols, you will have to use includechars to get some of these into 
    otherwise restricted positions in your control, such as those specified
    with &.

  Using these mask characters, a variety of template masks can be built. See
  the demo for some other common examples include date+time, social security
  number, etc.  If any of these characters are needed as template rather
  than mask characters, they can be escaped with \, ie. \N means "literal N".
  (use \\ for literal backslash, as in: r'CCC\\NNN'.)


  *Note:*
      Masks containing only # characters and one optional decimal point
      character are handled specially, as "numeric" controls.  Such
      controls have special handling for typing the '-' key, handling
      the "decimal point" character as truncating the integer portion,
      optionally allowing grouping characters and so forth.
      There are several parameters and format codes that only make sense
      when combined with such masks, eg. groupChar, decimalChar, and so
      forth (see below).  These allow you to construct reasonable
      numeric entry controls.

  *Note:*
      Changing the mask for a control deletes any previous field classes
      (and any associated validation or formatting constraints) for them.

useFixedWidthFont
  By default, masked edit controls use a fixed width font, so that
  the mask characters are fixed within the control, regardless of
  subsequent modifications to the value.  Set to False if having
  the control font be the same as other controls is required. (This is
  a control-level parameter.)

defaultEncoding
  (Applies to unicode systems only) By default, the default unicode encoding
  used is latin1, or iso-8859-1.  If necessary, you can set this control-level
  parameter to govern the codec used to decode your keyboard inputs.
  (This is a control-level parameter.)

formatcodes
  These other properties can be passed to the class when instantiating it:
    Formatcodes are specified as a string of single character formatting
    codes that modify  behavior of the control::
    
            _  Allow spaces
            !  Force upper
            ^  Force lower
            R  Right-align field(s)
            r  Right-insert in field(s) (implies R)
            <  Stay in field until explicit navigation out of it

            >  Allow insert/delete within partially filled fields (as
               opposed to the default "overwrite" mode for fixed-width
               masked edit controls.)  This allows single-field controls
               or each field within a multi-field control to optionally
               behave more like standard text controls.
               (See EMAIL or phone number autoformat examples.)

               *Note: This also governs whether backspace/delete operations
               shift contents of field to right of cursor, or just blank the
               erased section.

               Also, when combined with 'r', this indicates that the field
               or control allows right insert anywhere within the current
               non-empty value in the field.  (Otherwise right-insert behavior
               is only performed to when the entire right-insertable field is
               selected or the cursor is at the right edge of the field.*


            ,  Allow grouping character in integer fields of numeric controls
               and auto-group/regroup digits (if the result fits) when leaving
               such a field.  (If specified, .SetValue() will attempt to
               auto-group as well.)
               ',' is also the default grouping character.  To change the
               grouping character and/or decimal character, use the groupChar
               and decimalChar parameters, respectively.
               Note: typing the "decimal point" character in such fields will
               clip the value to that left of the cursor for integer
               fields of controls with "integer" or "floating point" masks.
               If the ',' format code is specified, this will also cause the
               resulting digits to be regrouped properly, using the current
               grouping character.
            -  Prepend and reserve leading space for sign to mask and allow
               signed values (negative #s shown in red by default.) Can be
               used with argument useParensForNegatives (see below.)
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

fillChar

defaultValue
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

groupChar

decimalChar
  These parameters govern what character is used to group numbers
  and is used to indicate the decimal point for numeric format controls.
  The default groupChar is ',', the default decimalChar is '.'
  By changing these, you can customize the presentation of numbers
  for your location.

  Eg::

        formatcodes = ',', groupChar='\''                  allows  12'345.34
        formatcodes = ',', groupChar='.', decimalChar=','  allows  12.345,34

  (These are control-level parameters.)

shiftDecimalChar
  The default "shiftDecimalChar" (used for "backwards-tabbing" until
  shift-tab is fixed in wxPython) is '>' (for QUERTY keyboards.) for
  other keyboards, you may want to customize this, eg '?' for shift ',' on
  AZERTY keyboards, ':' or ';' for other European keyboards, etc.
  (This is a control-level parameter.)

useParensForNegatives=False
  This option can be used with signed numeric format controls to
  indicate signs via () rather than '-'.
  (This is a control-level parameter.)

autoSelect=False
  This option can be used to have a field or the control try to
  auto-complete on each keystroke if choices have been specified.

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
  (This is a control-level parameter.)



Validating User Input
=====================
There are a variety of initialization parameters that are used to validate
user input.  These parameters can apply to the control as a whole, and/or
to individual fields:

        =====================  ==================================================================
        excludeChars           A string of characters to exclude even if otherwise allowed
        includeChars           A string of characters to allow even if otherwise disallowed
        validRegex             Use a regular expression to validate the contents of the text box
        validRange             Pass a rangeas list (low,high) to limit numeric fields/values
        choices                A list of strings that are allowed choices for the control.
        choiceRequired         value must be member of choices list
        compareNoCase          Perform case-insensitive matching when validating against list
                               *Note: for masked.ComboBox, this defaults to True.*
        emptyInvalid           Boolean indicating whether an empty value should be considered 
                               invalid

        validFunc              A function to call of the form: bool = func(candidate_value)
                               which will return True if the candidate_value satisfies some
                               external criteria for the control in addition to the the
                               other validation, or False if not.  (This validation is
                               applied last in the chain of validations.)

        validRequired          Boolean indicating whether or not keys that are allowed by the
                               mask, but result in an invalid value are allowed to be entered
                               into the control.  Setting this to True implies that a valid
                               default value is set for the control.

        retainFieldValidation  False by default; if True, this allows individual fields to
                               retain their own validation constraints independently of any
                               subsequent changes to the control's overall parameters.
                               (This is a control-level parameter.)

        validator              Validators are not normally needed for masked controls, because
                               of the nature of the validation and control of input.  However,
                               you can supply one to provide data transfer routines for the
                               controls.
        raiseOnInvalidPaste    False by default; normally a bad paste simply is ignored with a bell;
                               if True, this will cause a ValueError exception to be thrown,
                               with the .value attribute of the exception containing the bad value.
        =====================  ==================================================================


Coloring Behavior
=================
  The following parameters have been provided to allow you to change the default
  coloring behavior of the control.   These can be set at construction, or via
  the .SetCtrlParameters() function.  Pass a color as string e.g. 'Yellow':

        ========================  =======================================================================
        emptyBackgroundColour      Control Background color when identified as empty. Default=White
        invalidBackgroundColour    Control Background color when identified as Not valid. Default=Yellow
        validBackgroundColour      Control Background color when identified as Valid. Default=white
        ========================  =======================================================================


  The following parameters control the default foreground color coloring behavior of the
  control. Pass a color as string e.g. 'Yellow':

        ========================  ======================================================================
        foregroundColour           Control foreground color when value is not negative.  Default=Black
        signedForegroundColour     Control foreground color when value is negative. Default=Red
        ========================  ======================================================================


Fields
======
  Each part of the mask that allows user input is considered a field.  The fields
  are represented by their own class instances.  You can specify field-specific
  constraints by constructing or accessing the field instances for the control
  and then specifying those constraints via parameters.

fields
  This parameter allows you to specify Field instances containing
  constraints for the individual fields of a control, eg: local
  choice lists, validation rules, functions, regexps, etc.
  It can be either an ordered list or a dictionary.  If a list,
  the fields will be applied as fields 0, 1, 2, etc.
  If a dictionary, it should be keyed by field index.
  the values should be a instances of maskededit.Field.

  Any field not represented by the list or dictionary will be
  implicitly created by the control.

  Eg::

    fields = [ Field(formatcodes='_r'), Field('choices=['a', 'b', 'c']) ]
    
  Or::

    fields = {
               1: ( Field(formatcodes='_R', choices=['a', 'b', 'c']),
               3: ( Field(choices=['01', '02', '03'], choiceRequired=True)
             }

  The following parameters are available for individual fields, with the
  same semantics as for the whole control but applied to the field in question:

    ==============  =============================================================================
    fillChar        if set for a field, it will override the control's fillChar for that field
    groupChar       if set for a field, it will override the control's default
    defaultValue    sets field-specific default value; overrides any default from control
    compareNoCase   overrides control's settings
    emptyInvalid    determines whether field is required to be filled at all times
    validRequired   if set, requires field to contain valid value
    ==============  =============================================================================

  If any of the above parameters are subsequently specified for the control as a
  whole, that new value will be propagated to each field, unless the
  retainFieldValidation control-level parameter is set.

    ==============  ==============================
    formatcodes      Augments control's settings
    excludeChars         '       '        '
    includeChars         '       '        '
    validRegex           '       '        '
    validRange           '       '        '
    choices              '       '        '
    choiceRequired       '       '        '
    validFunc            '       '        '
    ==============  ==============================



Control Class Functions
=======================
.GetPlainValue(value=None)
                    Returns the value specified (or the control's text value
                    not specified) without the formatting text.
                    In the example above, might return phone no='3522640075',
                    whereas control.GetValue() would return '(352) 264-0075'
.ClearValue()
                    Returns the control's value to its default, and places the
                    cursor at the beginning of the control.
.SetValue()
                    Does "smart replacement" of passed value into the control, as does
                    the .Paste() method.  As with other text entry controls, the
                    .SetValue() text replacement begins at left-edge of the control,
                    with missing mask characters inserted as appropriate.
                    .SetValue will also adjust integer, float or date mask entry values,
                    adding commas, auto-completing years, etc. as appropriate.
                    For "right-aligned" numeric controls, it will also now automatically
                    right-adjust any value whose length is less than the width of the
                    control before attempting to set the value.
                    If a value does not follow the format of the control's mask, or will
                    not fit into the control, a ValueError exception will be raised.

                    Eg::

                      mask = '(###) ###-####'
                          .SetValue('1234567890')           => '(123) 456-7890'
                          .SetValue('(123)4567890')         => '(123) 456-7890'
                          .SetValue('(123)456-7890')        => '(123) 456-7890'
                          .SetValue('123/4567-890')         => illegal paste; ValueError

                      mask = '#{6}.#{2}', formatcodes = '_,-',
                          .SetValue('111')                  => ' 111   .  '
                          .SetValue(' %9.2f' % -111.12345 ) => '   -111.12'
                          .SetValue(' %9.2f' % 1234.00 )    => '  1,234.00'
                          .SetValue(' %9.2f' % -1234567.12345 ) => insufficient room; ValueError

                      mask = '#{6}.#{2}', formatcodes = '_,-R'  # will right-adjust value for right-aligned control
                          .SetValue('111')                  => padded value misalignment ValueError: "       111" will not fit
                          .SetValue('%.2f' % 111 )          => '    111.00'
                          .SetValue('%.2f' % -111.12345 )   => '   -111.12'


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

.Refresh()
                    Recolors the control as appropriate to its current settings.

.SetCtrlParameters(\*\*kwargs)
                    This function allows you to set up and/or change the control parameters
                    after construction; it takes a list of key/value pairs as arguments,
                    where the keys can be any of the mask-specific parameters in the constructor.

                    Eg::

                        ctl = masked.TextCtrl( self, -1 )
                        ctl.SetCtrlParameters( mask='###-####',
                                               defaultValue='555-1212',
                                               formatcodes='F')

.GetCtrlParameter(parametername)
                    This function allows you to retrieve the current value of a parameter
                    from the control.

  *Note:* Each of the control parameters can also be set using its
      own Set and Get function.  These functions follow a regular form:
      All of the parameter names start with lower case; for their
      corresponding Set/Get function, the parameter name is capitalized.

    Eg::

          ctl.SetMask('###-####')
          ctl.SetDefaultValue('555-1212')
          ctl.GetChoiceRequired()
          ctl.GetFormatcodes()

  *Note:* After any change in parameters, the choices for the
      control are reevaluated to ensure that they are still legal.  If you
      have large choice lists, it is therefore more efficient to set parameters
      before setting the choices available.

.SetFieldParameters(field_index, \*\*kwargs)
                    This function allows you to specify change individual field
                    parameters after construction. (Indices are 0-based.)

.GetFieldParameter(field_index, parametername)
                    Allows the retrieval of field parameters after construction


The control detects certain common constructions. In order to use the signed feature
(negative numbers and coloring), the mask has to be all numbers with optionally one
decimal point. Without a decimal (e.g. '######', the control will treat it as an integer
value. With a decimal (e.g. '###.##'), the control will act as a floating point control
(i.e. press decimal to 'tab' to the decimal position). Pressing decimal in the
integer control truncates the value.  However, for a true numeric control,
masked.NumCtrl provides all this, and true numeric input/output support as well.


Check your controls by calling each control's .IsValid() function and the
.IsEmpty() function to determine which controls have been a) filled in and
b) filled in properly.


Regular expression validations can be used flexibly and creatively.
Take a look at the demo; the zip-code validation succeeds as long as the
first five numerals are entered. the last four are optional, but if
any are entered, there must be 4 to be valid.

masked.Ctrl Configuration
=========================
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

    from wxPython.wx.lib.masked import MaskedCtrl, controlTypes
    from wxPython.wx.lib.masked import MaskedCtrl, COMBO, TEXT, NUMBER, IPADDR

If not specified as a keyword argument, the default controlType is
controlTypes.TEXT.

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
  from MaskedEditMixin.  NOTE: if deriving from a *masked edit* control
  (eg. class IpAddrCtrl(masked.TextCtrl) ), then this is NOT necessary,
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
                        Each class derived from MaskedEditMixin must define
                        the function for getting the start and end of the
                        current text selection.  The reason for this is
                        that not all controls have the same function name for
                        doing this; eg. wx.TextCtrl uses .GetSelection(),
                        whereas we had to write a .GetMark() function for
                        wxComboBox, because .GetSelection() for the control
                        gets the currently selected list item from the combo
                        box, and the control doesn't (yet) natively provide
                        a means of determining the text selection.
        ._SetSelection()
                        REQUIRED
                        Similarly to _GetSelection, each class derived from
                        MaskedEditMixin must define the function for setting
                        the start and end of the current text selection.
                        (eg. .SetSelection() for masked.TextCtrl, and .SetMark() for
                        masked.ComboBox.

        ._GetInsertionPoint()
        ._SetInsertionPoint()
                        REQUIRED
                        For consistency, and because the mixin shouldn't rely
                        on fixed names for any manipulations it does of any of
                        the base controls, we require each class derived from
                        MaskedEditMixin to define these functions as well.

        ._GetValue()
        ._SetValue()    REQUIRED
                        Each class derived from MaskedEditMixin must define
                        the functions used to get and set the raw value of the
                        control.
                        This is necessary so that recursion doesn't take place
                        when setting the value, and so that the mixin can
                        call the appropriate function after doing all its
                        validation and manipulation without knowing what kind
                        of base control it was mixed in with.  To handle undo
                        functionality, the ._SetValue() must record the current
                        selection prior to setting the value.

        .Cut()
        .Paste()
        .Undo()
        .SetValue()     REQUIRED
                        Each class derived from MaskedEditMixin must redefine
                        these functions to call the _Cut(), _Paste(), _Undo()
                        and _SetValue() methods, respectively for the control,
                        so as to prevent programmatic corruption of the control's
                        value.  This must be done in each derivation, as the
                        mixin cannot itself override a member of a sibling class.

        ._Refresh()     REQUIRED
                        Each class derived from MaskedEditMixin must define
                        the function used to refresh the base control.

        .Refresh()      REQUIRED
                        Each class derived from MaskedEditMixin must redefine
                        this function so that it checks the validity of the
                        control (via self._CheckValid) and then refreshes
                        control using the base class method.

        ._IsEditable()  REQUIRED
                        Each class derived from MaskedEditMixin must define
                        the function used to determine if the base control is
                        editable or not.  (For masked.ComboBox, this has to
                        be done with code, rather than specifying the proper
                        function in the base control, as there isn't one...)
        ._CalcSize()    REQUIRED
                        Each class derived from MaskedEditMixin must define
                        the function used to determine how wide the control
                        should be given the mask.  (The mixin function
                        ._calcSize() provides a baseline estimate.)


Event Handling
--------------
  Event handlers are "chained", and MaskedEditMixin usually
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
        EVT_RIGHT_UP(self, self._OnContextMenu )    ## bring up an appropriate context menu
        EVT_KEY_DOWN( self, self._OnKeyDown )       ## capture control events not normally seen, eg ctrl-tab.
        EVT_CHAR( self, self._OnChar )              ## handle each keypress
        EVT_TEXT( self, self.GetId(), self._OnTextChange )  ## color control appropriately & keep
                                                            ## track of previous value for undo

  where setupEventHandling is an argument to its constructor.

  These 5 handlers must be "wired up" for the masked edit
  controls to provide default behavior.  (The setupEventHandling
  is an argument to masked.TextCtrl and masked.ComboBox, so
  that controls derived from *them* may replace one of these
  handlers if they so choose.)

  If your derived control wants to preprocess events before
  taking action, it should then set up the event handling itself,
  so it can be first in the event handler chain.


  The following routines are available to facilitate changing
  the default behavior of masked edit controls:

        ._SetKeycodeHandler(keycode, func)
        ._SetKeyHandler(char, func)
                        Use to replace default handling for any given keycode.
                        func should take the key event as argument and return
                        False if no further action is required to handle the
                        key. Eg:
                            self._SetKeycodeHandler(WXK_UP, self.IncrementValue)
                            self._SetKeyHandler('-', self._OnChangeSign)

        (Setting a func of None removes any keyhandler for the given key.)
        
        "Navigation" keys are assumed to change the cursor position, and
        therefore don't cause automatic motion of the cursor as insertable
        characters do.

        ._AddNavKeycode(keycode, handler=None)
        ._AddNavKey(char, handler=None)
                        Allows controls to specify other keys (and optional handlers)
                        to be treated as navigational characters. (eg. '.' in IpAddrCtrl)

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
                        (eg. IpAddrCtrl reformats the address in this way.)

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
                        MaskedEditMixin.

    The following routines are used to handle standard actions
    for control keys:
        _OnArrow(event)         used for arrow navigation events
        _OnCtrl_A(event)        'select all'
        _OnCtrl_C(event)        'copy' (uses base control function, as copy is non-destructive)
        _OnCtrl_S(event)        'save' (does nothing)
        _OnCtrl_V(event)        'paste' - calls _Paste() method, to do smart paste
        _OnCtrl_X(event)        'cut'   - calls _Cut() method, to "erase" selection
        _OnCtrl_Z(event)        'undo'  - resets value to previous value (if any)

        _OnChangeField(event)   primarily used for tab events, but can be
                                used for other keys (eg. '.' in IpAddrCtrl)

        _OnErase(event)         used for backspace and delete
        _OnHome(event)
        _OnEnd(event)

    The following routine provides a hook back to any class derivations, so that
    they can react to parameter changes before any value is set/reset as a result of
    those changes.  (eg. masked.ComboBox needs to detect when the choices list is
    modified, either implicitly or explicitly, so it can reset the base control
    to have the appropriate choice list *before* the initial value is reset to match.)

        _OnCtrlParametersChanged()

Accessor Functions
------------------
    For convenience, each class derived from MaskedEditMixin should
    define an accessors mixin, so that it exposes only those parameters
    that make sense for the derivation.  This is done with an intermediate
    level of inheritance, ie:

    class BaseMaskedTextCtrl( TextCtrl, MaskedEditMixin ):

    class TextCtrl( BaseMaskedTextCtrl, MaskedEditAccessorsMixin ):
    class ComboBox( BaseMaskedComboBox, MaskedEditAccessorsMixin ):
    class NumCtrl( BaseMaskedTextCtrl, MaskedNumCtrlAccessorsMixin ):
    class IpAddrCtrl( BaseMaskedTextCtrl, IpAddrCtrlAccessorsMixin ):
    class TimeCtrl( BaseMaskedTextCtrl, TimeCtrlAccessorsMixin ):

    etc.

    Each accessors mixin defines Get/Set functions for the base class parameters
    that are appropriate for that derivation.
    This allows the base classes to be "more generic," exposing the widest
    set of options, while not requiring derived classes to be so general.
"""

import  copy
import  difflib
import  re
import  string
import  types

import  wx

# jmg 12/9/03 - when we cut ties with Py 2.2 and earlier, this would
# be a good place to implement the 2.3 logger class
from wx.tools.dbg import Logger

##dbg = Logger()
##dbg(enable=1)

## ---------- ---------- ---------- ---------- ---------- ---------- ----------

## Constants for identifying control keys and classes of keys:

WXK_CTRL_A = (ord('A')+1) - ord('A')   ## These keys are not already defined in wx
WXK_CTRL_C = (ord('C')+1) - ord('A')
WXK_CTRL_S = (ord('S')+1) - ord('A')
WXK_CTRL_V = (ord('V')+1) - ord('A')
WXK_CTRL_X = (ord('X')+1) - ord('A')
WXK_CTRL_Z = (ord('Z')+1) - ord('A')

nav = (
    wx.WXK_BACK, wx.WXK_LEFT, wx.WXK_RIGHT, wx.WXK_UP, wx.WXK_DOWN, wx.WXK_TAB,
    wx.WXK_HOME, wx.WXK_END, wx.WXK_RETURN, wx.WXK_PRIOR, wx.WXK_NEXT
    )

control = (
    wx.WXK_BACK, wx.WXK_DELETE, wx.WXK_INSERT, WXK_CTRL_A, WXK_CTRL_C, WXK_CTRL_S, WXK_CTRL_V,
    WXK_CTRL_X, WXK_CTRL_Z
    )

# Because unicode can go over the ansi character range, we need to explicitly test
# for all non-visible keystrokes, rather than just assuming a particular range for
# visible characters:
wx_control_keycodes = range(32) + list(nav) + list(control) + [
    wx.WXK_START, wx.WXK_LBUTTON, wx.WXK_RBUTTON, wx.WXK_CANCEL, wx.WXK_MBUTTON,
    wx.WXK_CLEAR, wx.WXK_SHIFT, wx.WXK_CONTROL, wx.WXK_MENU, wx.WXK_PAUSE, 
    wx.WXK_CAPITAL, wx.WXK_SELECT, wx.WXK_PRINT, wx.WXK_EXECUTE, wx.WXK_SNAPSHOT,
    wx.WXK_HELP, wx.WXK_NUMPAD0, wx.WXK_NUMPAD1, wx.WXK_NUMPAD2, wx.WXK_NUMPAD3,
    wx.WXK_NUMPAD4, wx.WXK_NUMPAD5, wx.WXK_NUMPAD6, wx.WXK_NUMPAD7, wx.WXK_NUMPAD8,
    wx.WXK_NUMPAD9, wx.WXK_MULTIPLY, wx.WXK_ADD, wx.WXK_SEPARATOR, wx.WXK_SUBTRACT,
    wx.WXK_DECIMAL, wx.WXK_DIVIDE, wx.WXK_F1, wx.WXK_F2, wx.WXK_F3, wx.WXK_F4, 
    wx.WXK_F5, wx.WXK_F6, wx.WXK_F7, wx.WXK_F8, wx.WXK_F9, wx.WXK_F10, wx.WXK_F11,
    wx.WXK_F12, wx.WXK_F13, wx.WXK_F14, wx.WXK_F15, wx.WXK_F16, wx.WXK_F17,
    wx.WXK_F18, wx.WXK_F19, wx.WXK_F20, wx.WXK_F21, wx.WXK_F22, wx.WXK_F23,
    wx.WXK_F24, wx.WXK_NUMLOCK, wx.WXK_SCROLL, wx.WXK_PAGEUP, wx.WXK_PAGEDOWN,
    wx.WXK_NUMPAD_SPACE, wx.WXK_NUMPAD_TAB, wx.WXK_NUMPAD_ENTER, wx.WXK_NUMPAD_F1,
    wx.WXK_NUMPAD_F2, wx.WXK_NUMPAD_F3, wx.WXK_NUMPAD_F4, wx.WXK_NUMPAD_HOME,
    wx.WXK_NUMPAD_LEFT, wx.WXK_NUMPAD_UP, wx.WXK_NUMPAD_RIGHT, wx.WXK_NUMPAD_DOWN,
    wx.WXK_NUMPAD_PRIOR, wx.WXK_NUMPAD_PAGEUP, wx.WXK_NUMPAD_NEXT, wx.WXK_NUMPAD_PAGEDOWN,
    wx.WXK_NUMPAD_END, wx.WXK_NUMPAD_BEGIN, wx.WXK_NUMPAD_INSERT, wx.WXK_NUMPAD_DELETE,
    wx.WXK_NUMPAD_EQUAL, wx.WXK_NUMPAD_MULTIPLY, wx.WXK_NUMPAD_ADD, wx.WXK_NUMPAD_SEPARATOR,
    wx.WXK_NUMPAD_SUBTRACT, wx.WXK_NUMPAD_DECIMAL, wx.WXK_NUMPAD_DIVIDE, wx.WXK_WINDOWS_LEFT,
    wx.WXK_WINDOWS_RIGHT, wx.WXK_WINDOWS_MENU, wx.WXK_COMMAND,
    # Hardware-specific buttons
    wx.WXK_SPECIAL1, wx.WXK_SPECIAL2, wx.WXK_SPECIAL3, wx.WXK_SPECIAL4, wx.WXK_SPECIAL5,
    wx.WXK_SPECIAL6, wx.WXK_SPECIAL7, wx.WXK_SPECIAL8, wx.WXK_SPECIAL9, wx.WXK_SPECIAL10,
    wx.WXK_SPECIAL11, wx.WXK_SPECIAL12, wx.WXK_SPECIAL13, wx.WXK_SPECIAL14, wx.WXK_SPECIAL15,
    wx.WXK_SPECIAL16, wx.WXK_SPECIAL17, wx.WXK_SPECIAL18, wx.WXK_SPECIAL19, wx.WXK_SPECIAL20
    ]
    

## ---------- ---------- ---------- ---------- ---------- ---------- ----------

## Constants for masking. This is where mask characters
## are defined.
##  maskchars used to identify valid mask characters from all others
##   # - allow numeric 0-9 only
##   A - allow uppercase only. Combine with forceupper to force lowercase to upper
##   a - allow lowercase only. Combine with forcelower to force upper to lowercase
##   C - allow any letter, upper or lower
##   X - allow string.letters, string.punctuation, string.digits
##   & - allow string.punctuation only (doesn't include all unicode symbols)
##   * - allow any visible character

## Note: locale settings affect what "uppercase", lowercase, etc comprise.
## Note: '|' is not a maskchar, in that it is a mask processing directive, and so
## does not appear here.
##
maskchars = ("#","A","a","X","C","N",'*','&')
ansichars = ""
for i in xrange(32, 256):
    ansichars += chr(i)

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

state_names = ['Alabama','Alaska','Arizona','Arkansas',
               'California','Colorado','Connecticut',
               'Delaware','District of Columbia',
               'Florida','Georgia','Hawaii',
               'Idaho','Illinois','Indiana','Iowa',
               'Kansas','Kentucky','Louisiana',
               'Maine','Maryland','Massachusetts','Michigan',
               'Minnesota','Mississippi','Missouri','Montana',
               'Nebraska','Nevada','New Hampshire','New Jersey',
               'New Mexico','New York','North Carolina','North Dakokta',
               'Ohio','Oklahoma','Oregon',
               'Pennsylvania','Puerto Rico','Rhode Island',
               'South Carolina','South Dakota',
               'Tennessee','Texas','Utah',
               'Vermont','Virginia',
               'Washington','West Virginia',
               'Wisconsin','Wyoming']

## ---------- ---------- ---------- ---------- ---------- ---------- ----------

## The following dictionary defines the current set of autoformats:

masktags = {
       "USPHONEFULLEXT": {
           'mask': "(###) ###-#### x:###",
           'formatcodes': 'F^->',
           'validRegex': "^\(\d{3}\) \d{3}-\d{4}",
           'description': "Phone Number w/opt. ext"
           },
       "USPHONETIGHTEXT": {
           'mask': "###-###-#### x:###",
           'formatcodes': 'F^->',
           'validRegex': "^\d{3}-\d{3}-\d{4}",
           'description': "Phone Number\n (w/hyphens and opt. ext)"
           },
       "USPHONEFULL": {
           'mask': "(###) ###-####",
           'formatcodes': 'F^->',
           'validRegex': "^\(\d{3}\) \d{3}-\d{4}",
           'description': "Phone Number only"
           },
       "USPHONETIGHT": {
           'mask': "###-###-####",
           'formatcodes': 'F^->',
           'validRegex': "^\d{3}-\d{3}-\d{4}",
           'description': "Phone Number\n(w/hyphens)"
           },
       "USSTATE": {
           'mask': "AA",
           'formatcodes': 'F!V',
           'validRegex': "([ACDFGHIKLMNOPRSTUVW] |%s)" % string.join(states,'|'),
           'choices': states,
           'choiceRequired': True,
           'description': "US State Code"
           },
       "USSTATENAME": {
           'mask': "ACCCCCCCCCCCCCCCCCCC",
           'formatcodes': 'F_',
           'validRegex': "([ACDFGHIKLMNOPRSTUVW] |%s)" % string.join(state_names,'|'),
           'choices': state_names,
           'choiceRequired': True,
           'description': "US State Name"
           },

       "USDATETIMEMMDDYYYY/HHMMSS": {
           'mask': "##/##/#### ##:##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + months + '/' + days + '/' + '\d{4} ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',
           'description': "US Date + Time"
           },
       "USDATETIMEMMDDYYYY-HHMMSS": {
           'mask': "##-##-#### ##:##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + months + '-' + days + '-' + '\d{4} ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',
           'description': "US Date + Time\n(w/hypens)"
           },
       "USDATE24HRTIMEMMDDYYYY/HHMMSS": {
           'mask': "##/##/#### ##:##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + months + '/' + days + '/' + '\d{4} ' + milhours + ':' + minutes + ':' + seconds,
           'description': "US Date + 24Hr (Military) Time"
           },
       "USDATE24HRTIMEMMDDYYYY-HHMMSS": {
           'mask': "##-##-#### ##:##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + months + '-' + days + '-' + '\d{4} ' + milhours + ':' + minutes + ':' + seconds,
           'description': "US Date + 24Hr Time\n(w/hypens)"
           },
       "USDATETIMEMMDDYYYY/HHMM": {
           'mask': "##/##/#### ##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + months + '/' + days + '/' + '\d{4} ' + hours + ':' + minutes + ' (A|P)M',
           'description': "US Date + Time\n(without seconds)"
           },
       "USDATE24HRTIMEMMDDYYYY/HHMM": {
           'mask': "##/##/#### ##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + months + '/' + days + '/' + '\d{4} ' + milhours + ':' + minutes,
           'description': "US Date + 24Hr Time\n(without seconds)"
           },
       "USDATETIMEMMDDYYYY-HHMM": {
           'mask': "##-##-#### ##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + months + '-' + days + '-' + '\d{4} ' + hours + ':' + minutes + ' (A|P)M',
           'description': "US Date + Time\n(w/hypens and w/o secs)"
           },
       "USDATE24HRTIMEMMDDYYYY-HHMM": {
           'mask': "##-##-#### ##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + months + '-' + days + '-' + '\d{4} ' + milhours + ':' + minutes,
           'description': "US Date + 24Hr Time\n(w/hyphens and w/o seconds)"
           },
       "USDATEMMDDYYYY/": {
           'mask': "##/##/####",
           'formatcodes': 'DF',
           'validRegex': '^' + months + '/' + days + '/' + '\d{4}',
           'description': "US Date\n(MMDDYYYY)"
           },
       "USDATEMMDDYY/": {
           'mask': "##/##/##",
           'formatcodes': 'DF',
           'validRegex': '^' + months + '/' + days + '/\d\d',
           'description': "US Date\n(MMDDYY)"
           },
       "USDATEMMDDYYYY-": {
           'mask': "##-##-####",
           'formatcodes': 'DF',
           'validRegex': '^' + months + '-' + days + '-' +'\d{4}',
           'description': "MM-DD-YYYY"
           },

       "EUDATEYYYYMMDD/": {
           'mask': "####/##/##",
           'formatcodes': 'DF',
           'validRegex': '^' + '\d{4}'+ '/' + months + '/' + days,
           'description': "YYYY/MM/DD"
           },
       "EUDATEYYYYMMDD.": {
           'mask': "####.##.##",
           'formatcodes': 'DF',
           'validRegex': '^' + '\d{4}'+ '.' + months + '.' + days,
           'description': "YYYY.MM.DD"
           },
       "EUDATEDDMMYYYY/": {
           'mask': "##/##/####",
           'formatcodes': 'DF',
           'validRegex': '^' + days + '/' + months + '/' + '\d{4}',
           'description': "DD/MM/YYYY"
           },
       "EUDATEDDMMYYYY.": {
           'mask': "##.##.####",
           'formatcodes': 'DF',
           'validRegex': '^' + days + '.' + months + '.' + '\d{4}',
           'description': "DD.MM.YYYY"
           },
       "EUDATEDDMMMYYYY.": {
           'mask': "##.CCC.####",
           'formatcodes': 'DF',
           'validRegex': '^' + days + '.' + charmonths + '.' + '\d{4}',
           'description': "DD.Month.YYYY"
           },
       "EUDATEDDMMMYYYY/": {
           'mask': "##/CCC/####",
           'formatcodes': 'DF',
           'validRegex': '^' + days + '/' + charmonths + '/' + '\d{4}',
           'description': "DD/Month/YYYY"
           },

       "EUDATETIMEYYYYMMDD/HHMMSS": {
           'mask': "####/##/## ##:##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + '\d{4}'+ '/' + months + '/' + days + ' ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',
           'description': "YYYY/MM/DD HH:MM:SS"
           },
       "EUDATETIMEYYYYMMDD.HHMMSS": {
           'mask': "####.##.## ##:##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + '\d{4}'+ '.' + months + '.' + days + ' ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',
           'description': "YYYY.MM.DD HH:MM:SS"
           },
       "EUDATETIMEDDMMYYYY/HHMMSS": {
           'mask': "##/##/#### ##:##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + days + '/' + months + '/' + '\d{4} ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',
           'description': "DD/MM/YYYY HH:MM:SS"
           },
       "EUDATETIMEDDMMYYYY.HHMMSS": {
           'mask': "##.##.#### ##:##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + days + '.' + months + '.' + '\d{4} ' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',
           'description': "DD.MM.YYYY HH:MM:SS"
           },

       "EUDATETIMEYYYYMMDD/HHMM": {
           'mask': "####/##/## ##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + '\d{4}'+ '/' + months + '/' + days + ' ' + hours + ':' + minutes + ' (A|P)M',
           'description': "YYYY/MM/DD HH:MM"
           },
       "EUDATETIMEYYYYMMDD.HHMM": {
           'mask': "####.##.## ##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + '\d{4}'+ '.' + months + '.' + days + ' ' + hours + ':' + minutes + ' (A|P)M',
           'description': "YYYY.MM.DD HH:MM"
           },
       "EUDATETIMEDDMMYYYY/HHMM": {
           'mask': "##/##/#### ##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + days + '/' + months + '/' + '\d{4} ' + hours + ':' + minutes + ' (A|P)M',
           'description': "DD/MM/YYYY HH:MM"
           },
       "EUDATETIMEDDMMYYYY.HHMM": {
           'mask': "##.##.#### ##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'DF!',
           'validRegex': '^' + days + '.' + months + '.' + '\d{4} ' + hours + ':' + minutes + ' (A|P)M',
           'description': "DD.MM.YYYY HH:MM"
           },

       "EUDATE24HRTIMEYYYYMMDD/HHMMSS": {
           'mask': "####/##/## ##:##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + '\d{4}'+ '/' + months + '/' + days + ' ' + milhours + ':' + minutes + ':' + seconds,
           'description': "YYYY/MM/DD 24Hr Time"
           },
       "EUDATE24HRTIMEYYYYMMDD.HHMMSS": {
           'mask': "####.##.## ##:##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + '\d{4}'+ '.' + months + '.' + days + ' ' + milhours + ':' + minutes + ':' + seconds,
           'description': "YYYY.MM.DD 24Hr Time"
           },
       "EUDATE24HRTIMEDDMMYYYY/HHMMSS": {
           'mask': "##/##/#### ##:##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + days + '/' + months + '/' + '\d{4} ' + milhours + ':' + minutes + ':' + seconds,
           'description': "DD/MM/YYYY 24Hr Time"
           },
       "EUDATE24HRTIMEDDMMYYYY.HHMMSS": {
           'mask': "##.##.#### ##:##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + days + '.' + months + '.' + '\d{4} ' + milhours + ':' + minutes + ':' + seconds,
           'description': "DD.MM.YYYY 24Hr Time"
           },
       "EUDATE24HRTIMEYYYYMMDD/HHMM": {
           'mask': "####/##/## ##:##",
           'formatcodes': 'DF','validRegex': '^' + '\d{4}'+ '/' + months + '/' + days + ' ' + milhours + ':' + minutes,
           'description': "YYYY/MM/DD 24Hr Time\n(w/o seconds)"
           },
       "EUDATE24HRTIMEYYYYMMDD.HHMM": {
           'mask': "####.##.## ##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + '\d{4}'+ '.' + months + '.' + days + ' ' + milhours + ':' + minutes,
           'description': "YYYY.MM.DD 24Hr Time\n(w/o seconds)"
           },
       "EUDATE24HRTIMEDDMMYYYY/HHMM": {
           'mask': "##/##/#### ##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + days + '/' + months + '/' + '\d{4} ' + milhours + ':' + minutes,
           'description': "DD/MM/YYYY 24Hr Time\n(w/o seconds)"
           },
       "EUDATE24HRTIMEDDMMYYYY.HHMM": {
           'mask': "##.##.#### ##:##",
           'formatcodes': 'DF',
           'validRegex': '^' + days + '.' + months + '.' + '\d{4} ' + milhours + ':' + minutes,
           'description': "DD.MM.YYYY 24Hr Time\n(w/o seconds)"
           },

       "TIMEHHMMSS": {
           'mask': "##:##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'TF!',
           'validRegex': '^' + hours + ':' + minutes + ':' + seconds + ' (A|P)M',
           'description': "HH:MM:SS (A|P)M\n(see TimeCtrl)"
           },
       "TIMEHHMM": {
           'mask': "##:## AM",
           'excludeChars': am_pm_exclude,
           'formatcodes': 'TF!',
           'validRegex': '^' + hours + ':' + minutes + ' (A|P)M',
           'description': "HH:MM (A|P)M\n(see TimeCtrl)"
           },
       "24HRTIMEHHMMSS": {
           'mask': "##:##:##",
           'formatcodes': 'TF',
           'validRegex': '^' + milhours + ':' + minutes + ':' + seconds,
           'description': "24Hr HH:MM:SS\n(see TimeCtrl)"
           },
       "24HRTIMEHHMM": {
           'mask': "##:##",
           'formatcodes': 'TF',
           'validRegex': '^' + milhours + ':' + minutes,
           'description': "24Hr HH:MM\n(see TimeCtrl)"
           },
       "USSOCIALSEC": {
           'mask': "###-##-####",
           'formatcodes': 'F',
           'validRegex': "\d{3}-\d{2}-\d{4}",
           'description': "Social Sec#"
           },
       "CREDITCARD": {
           'mask': "####-####-####-####",
           'formatcodes': 'F',
           'validRegex': "\d{4}-\d{4}-\d{4}-\d{4}",
           'description': "Credit Card"
           },
       "EXPDATEMMYY": {
           'mask': "##/##",
           'formatcodes': "F",
           'validRegex': "^" + months + "/\d\d",
           'description': "Expiration MM/YY"
           },
       "USZIP": {
           'mask': "#####",
           'formatcodes': 'F',
           'validRegex': "^\d{5}",
           'description': "US 5-digit zip code"
           },
       "USZIPPLUS4": {
           'mask': "#####-####",
           'formatcodes': 'F',
           'validRegex': "\d{5}-(\s{4}|\d{4})",
           'description': "US zip+4 code"
           },
       "PERCENT": {
           'mask': "0.##",
           'formatcodes': 'F',
           'validRegex': "^0.\d\d",
           'description': "Percentage"
           },
       "AGE": {
           'mask': "###",
           'formatcodes': "F",
           'validRegex': "^[1-9]{1}  |[1-9][0-9] |1[0|1|2][0-9]",
           'description': "Age"
           },
       "EMAIL": {
           'mask': "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
           'excludeChars': " \\/*&%$#!+='\"",
           'formatcodes': "F>",
           'validRegex': "^\w+([\-\.]\w+)*@((([a-zA-Z0-9]+(\-[a-zA-Z0-9]+)*\.)+)[a-zA-Z]{2,4}|\[(\d|\d\d|(1\d\d|2[0-4]\d|25[0-5]))(\.(\d|\d\d|(1\d\d|2[0-4]\d|25[0-5]))){3}\]) *$",
           'description': "Email address"
           },
       "IPADDR": {
           'mask': "###.###.###.###",
           'formatcodes': 'F_Sr',
           'validRegex': "(  \d| \d\d|(1\d\d|2[0-4]\d|25[0-5]))(\.(  \d| \d\d|(1\d\d|2[0-4]\d|25[0-5]))){3}",
           'description': "IP Address\n(see IpAddrCtrl)"
           }
       }

# build demo-friendly dictionary of descriptions of autoformats
autoformats = []
for key, value in masktags.items():
    autoformats.append((key, value['description']))
autoformats.sort()

## ---------- ---------- ---------- ---------- ---------- ---------- ----------

class Field:
    """
    This class manages the individual fields in a masked edit control.
    Each field has a zero-based index, indicating its position in the
    control, an extent, an associated mask, and a plethora of optional
    parameters.  Fields can be instantiated and then associated with
    parent masked controls, in order to provide field-specific configuration.
    Alternatively, fields will be implicitly created by the parent control
    if not provided at construction, at which point, the fields can then
    manipulated by the controls .SetFieldParameters() method.
    """
    valid_params = {
              'index': None,                    ## which field of mask; set by parent control.
              'mask': "",                       ## mask chars for this field
              'extent': (),                     ## (edit start, edit_end) of field; set by parent control.
              'formatcodes':  "",               ## codes indicating formatting options for the control
              'fillChar':     ' ',              ## used as initial value for each mask position if initial value is not given
              'groupChar':    ',',              ## used with numeric fields; indicates what char groups 3-tuple digits
              'decimalChar':  '.',              ## used with numeric fields; indicates what char separates integer from fraction
              'shiftDecimalChar': '>',          ## used with numeric fields, indicates what is above the decimal point char on keyboard
              'useParensForNegatives': False,   ## used with numeric fields, indicates that () should be used vs. - to show negative numbers.
              'defaultValue': "",               ## use if you want different positional defaults vs. all the same fillChar
              'excludeChars': "",               ## optional string of chars to exclude even if main mask type does
              'includeChars': "",               ## optional string of chars to allow even if main mask type doesn't
              'validRegex':   "",               ## optional regular expression to use to validate the control
              'validRange':   (),               ## Optional hi-low range for numerics
              'choices':    [],                 ## Optional list for character expressions
              'choiceRequired': False,          ## If choices supplied this specifies if valid value must be in the list
              'compareNoCase': False,           ## Optional flag to indicate whether or not to use case-insensitive list search
              'autoSelect': False,              ## Set to True to try auto-completion on each keystroke:
              'validFunc': None,                ## Optional function for defining additional, possibly dynamic validation constraints on contrl
              'validRequired': False,           ## Set to True to disallow input that results in an invalid value
              'emptyInvalid':  False,           ## Set to True to make EMPTY = INVALID
              'description': "",                ## primarily for autoformats, but could be useful elsewhere
              'raiseOnInvalidPaste': False,     ## if True, paste into field will cause ValueError
              }

    # This list contains all parameters that when set at the control level should
    # propagate down to each field:
    propagating_params = ('fillChar', 'groupChar', 'decimalChar','useParensForNegatives',
                          'compareNoCase', 'emptyInvalid', 'validRequired', 'raiseOnInvalidPaste')

    def __init__(self, **kwargs):
        """
        This is the "constructor" for setting up parameters for fields.
        a field_index of -1 is used to indicate "the entire control."
        """
####        dbg('Field::Field', indent=1)
        # Validate legitimate set of parameters:
        for key in kwargs.keys():
            if key not in Field.valid_params.keys():
####                dbg(indent=0)
                ae = AttributeError('invalid parameter "%s"' % (key))
                ae.attribute = key
                raise ae

        # Set defaults for each parameter for this instance, and fully
        # populate initial parameter list for configuration:
        for key, value in Field.valid_params.items():
            setattr(self, '_' + key, copy.copy(value))
            if not kwargs.has_key(key):
                kwargs[key] = copy.copy(value)

        self._autoCompleteIndex = -1
        self._SetParameters(**kwargs)
        self._ValidateParameters(**kwargs)

####        dbg(indent=0)


    def _SetParameters(self, **kwargs):
        """
        This function can be used to set individual or multiple parameters for
        a masked edit field parameter after construction.
        """
##        dbg(suspend=1)
##        dbg('maskededit.Field::_SetParameters', indent=1)
        # Validate keyword arguments:
        for key in kwargs.keys():
            if key not in Field.valid_params.keys():
##                dbg(indent=0, suspend=0)
                ae = AttributeError('invalid keyword argument "%s"' % key)
                ae.attribute = key
                raise ae

##        if self._index is not None: dbg('field index:', self._index)
##        dbg('parameters:', indent=1)
        for key, value in kwargs.items():
##            dbg('%s:' % key, value)
            pass
##        dbg(indent=0)


        old_fillChar = self._fillChar   # store so we can change choice lists accordingly if it changes

        # First, Assign all parameters specified:
        for key in Field.valid_params.keys():
            if kwargs.has_key(key):
                setattr(self, '_' + key, kwargs[key] )

        if kwargs.has_key('formatcodes'):   # (set/changed)
            self._forceupper  = '!' in self._formatcodes
            self._forcelower  = '^' in self._formatcodes
            self._groupdigits = ',' in self._formatcodes
            self._okSpaces    = '_' in self._formatcodes
            self._padZero     = '0' in self._formatcodes
            self._autofit     = 'F' in self._formatcodes
            self._insertRight = 'r' in self._formatcodes
            self._allowInsert = '>' in self._formatcodes
            self._alignRight  = 'R' in self._formatcodes or 'r' in self._formatcodes
            self._moveOnFieldFull = not '<' in self._formatcodes
            self._selectOnFieldEntry = 'S' in self._formatcodes

            if kwargs.has_key('groupChar'):
                self._groupChar = kwargs['groupChar']
            if kwargs.has_key('decimalChar'):
                self._decimalChar = kwargs['decimalChar']
            if kwargs.has_key('shiftDecimalChar'):
                self._shiftDecimalChar = kwargs['shiftDecimalChar']

        if kwargs.has_key('formatcodes') or kwargs.has_key('validRegex'):
            self._regexMask   = 'V' in self._formatcodes and self._validRegex

        if kwargs.has_key('fillChar'):
            self._old_fillChar = old_fillChar
####            dbg("self._old_fillChar: '%s'" % self._old_fillChar)

        if kwargs.has_key('mask') or kwargs.has_key('validRegex'):  # (set/changed)
            self._isInt = _isInteger(self._mask)
##            dbg('isInt?', self._isInt, 'self._mask:"%s"' % self._mask)

##        dbg(indent=0, suspend=0)


    def _ValidateParameters(self, **kwargs):
        """
        This function can be used to validate individual or multiple parameters for
        a masked edit field parameter after construction.
        """
##        dbg(suspend=1)
##        dbg('maskededit.Field::_ValidateParameters', indent=1)
##        if self._index is not None: dbg('field index:', self._index)
####        dbg('parameters:', indent=1)
##        for key, value in kwargs.items():
####            dbg('%s:' % key, value)
####        dbg(indent=0)
####        dbg("self._old_fillChar: '%s'" % self._old_fillChar)

        # Verify proper numeric format params:
        if self._groupdigits and self._groupChar == self._decimalChar:
##            dbg(indent=0, suspend=0)
            ae = AttributeError("groupChar '%s' cannot be the same as decimalChar '%s'" % (self._groupChar, self._decimalChar))
            ae.attribute = self._groupChar
            raise ae


        # Now go do validation, semantic and inter-dependency parameter processing:
        if kwargs.has_key('choices') or kwargs.has_key('compareNoCase') or kwargs.has_key('choiceRequired'): # (set/changed)

            self._compareChoices = [choice.strip() for choice in self._choices]

            if self._compareNoCase and self._choices:
                self._compareChoices = [item.lower() for item in self._compareChoices]

            if kwargs.has_key('choices'):
                self._autoCompleteIndex = -1


        if kwargs.has_key('validRegex'):    # (set/changed)
            if self._validRegex:
                try:
                    if self._compareNoCase:
                        self._filter = re.compile(self._validRegex, re.IGNORECASE)
                    else:
                        self._filter = re.compile(self._validRegex)
                except:
##                    dbg(indent=0, suspend=0)
                    raise TypeError('%s: validRegex "%s" not a legal regular expression' % (str(self._index), self._validRegex))
            else:
                self._filter = None

        if kwargs.has_key('validRange'):    # (set/changed)
            self._hasRange  = False
            self._rangeHigh = 0
            self._rangeLow  = 0
            if self._validRange:
                if type(self._validRange) != types.TupleType or len( self._validRange )!= 2 or self._validRange[0] > self._validRange[1]:
##                    dbg(indent=0, suspend=0)
                    raise TypeError('%s: validRange %s parameter must be tuple of form (a,b) where a <= b'
                                    % (str(self._index), repr(self._validRange)) )

                self._hasRange  = True
                self._rangeLow  = self._validRange[0]
                self._rangeHigh = self._validRange[1]

        if kwargs.has_key('choices') or (len(self._choices) and len(self._choices[0]) != len(self._mask)):       # (set/changed)
            self._hasList   = False
            if self._choices and type(self._choices) not in (types.TupleType, types.ListType):
##                dbg(indent=0, suspend=0)
                raise TypeError('%s: choices must be a sequence of strings' % str(self._index))
            elif len( self._choices) > 0:
                for choice in self._choices:
                    if type(choice) not in (types.StringType, types.UnicodeType):
##                        dbg(indent=0, suspend=0)
                        raise TypeError('%s: choices must be a sequence of strings' % str(self._index))

                length = len(self._mask)
##                dbg('len(%s)' % self._mask, length, 'len(self._choices):', len(self._choices), 'length:', length, 'self._alignRight?', self._alignRight)
                if len(self._choices) and length:
                    if len(self._choices[0]) > length:
                        # changed mask without respecifying choices; readjust the width as appropriate:
                        self._choices = [choice.strip() for choice in self._choices]
                    if self._alignRight:
                        self._choices = [choice.rjust( length ) for choice in self._choices]
                    else:
                        self._choices = [choice.ljust( length ) for choice in self._choices]
##                    dbg('aligned choices:', self._choices)

                if hasattr(self, '_template'):
                    # Verify each choice specified is valid:
                    for choice in self._choices:
                        if self.IsEmpty(choice) and not self._validRequired:
                            # allow empty values even if invalid, (just colored differently)
                            continue
                        if not self.IsValid(choice):
##                            dbg(indent=0, suspend=0)
                            ve = ValueError('%s: "%s" is not a valid value for the control as specified.' % (str(self._index), choice))
                            ve.value = choice
                            raise ve
                self._hasList = True

####        dbg("kwargs.has_key('fillChar')?", kwargs.has_key('fillChar'), "len(self._choices) > 0?", len(self._choices) > 0)
####        dbg("self._old_fillChar:'%s'" % self._old_fillChar, "self._fillChar: '%s'" % self._fillChar)
        if kwargs.has_key('fillChar') and len(self._choices) > 0:
            if kwargs['fillChar'] != ' ':
                self._choices = [choice.replace(' ', self._fillChar) for choice in self._choices]
            else:
                self._choices = [choice.replace(self._old_fillChar, self._fillChar) for choice in self._choices]
##            dbg('updated choices:', self._choices)


        if kwargs.has_key('autoSelect') and kwargs['autoSelect']:
            if not self._hasList:
##                dbg('no list to auto complete; ignoring "autoSelect=True"')
                self._autoSelect = False

        # reset field validity assumption:
        self._valid = True
##        dbg(indent=0, suspend=0)


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
##        dbg('Field::IsEmpty("%s")' % slice, indent=1)
        if not hasattr(self, '_template'):
##            dbg(indent=0)
            raise AttributeError('_template')

##        dbg('self._template: "%s"' % self._template)
##        dbg('self._defaultValue: "%s"' % str(self._defaultValue))
        if slice == self._template and not self._defaultValue:
##            dbg(indent=0)
            return True

        elif slice == self._template:
            empty = True
            for pos in range(len(self._template)):
####                dbg('slice[%(pos)d] != self._fillChar?' %locals(), slice[pos] != self._fillChar[pos])
                if slice[pos] not in (' ', self._fillChar):
                    empty = False
                    break
##            dbg("IsEmpty? %(empty)d (do all mask chars == fillChar?)" % locals(), indent=0)
            return empty
        else:
##            dbg("IsEmpty? 0 (slice doesn't match template)", indent=0)
            return False


    def IsValid(self, slice):
        """
        Indicates whether the specified slice is considered a valid value for the
        field.
        """
##        dbg(suspend=1)
##        dbg('Field[%s]::IsValid("%s")' % (str(self._index), slice), indent=1)
        valid = True    # assume true to start

        if self.IsEmpty(slice):
##            dbg(indent=0, suspend=0)
            if self._emptyInvalid:
                return False
            else:
                return True

        elif self._hasList and self._choiceRequired:
##            dbg("(member of list required)")
            # do case-insensitive match on list; strip surrounding whitespace from slice (already done for choices):
            if self._fillChar != ' ':
                slice = slice.replace(self._fillChar, ' ')
##                dbg('updated slice:"%s"' % slice)
            compareStr = slice.strip()

            if self._compareNoCase:
                compareStr = compareStr.lower()
            valid = compareStr in self._compareChoices

        elif self._hasRange and not self.IsEmpty(slice):
##            dbg('validating against range')
            try:
                # allow float as well as int ranges (int comparisons for free.)
                valid = self._rangeLow <= float(slice) <= self._rangeHigh
            except:
                valid = False

        elif self._validRegex and self._filter:
##            dbg('validating against regex')
            valid = (re.match( self._filter, slice) is not None)

        if valid and self._validFunc:
##            dbg('validating against supplied function')
            valid = self._validFunc(slice)
##        dbg('valid?', valid, indent=0, suspend=0)
        return valid


    def _AdjustField(self, slice):
        """ 'Fixes' an integer field. Right or left-justifies, as required."""
##        dbg('Field::_AdjustField("%s")' % slice, indent=1)
        length = len(self._mask)
####        dbg('length(self._mask):', length)
####        dbg('self._useParensForNegatives?', self._useParensForNegatives)
        if self._isInt:
            if self._useParensForNegatives:
                signpos = slice.find('(')
                right_signpos = slice.find(')')
                intStr = slice.replace('(', '').replace(')', '')    # drop sign, if any
            else:
                signpos = slice.find('-')
                intStr = slice.replace( '-', '' )                   # drop sign, if any
                right_signpos = -1

            intStr = intStr.replace(' ', '')                        # drop extra spaces
            intStr = string.replace(intStr,self._fillChar,"")       # drop extra fillchars
            intStr = string.replace(intStr,"-","")                  # drop sign, if any
            intStr = string.replace(intStr, self._groupChar, "")    # lose commas/dots
####            dbg('intStr:"%s"' % intStr)
            start, end = self._extent
            field_len = end - start
            if not self._padZero and len(intStr) != field_len and intStr.strip():
                intStr = str(long(intStr))
####            dbg('raw int str: "%s"' % intStr)
####            dbg('self._groupdigits:', self._groupdigits, 'self._formatcodes:', self._formatcodes)
            if self._groupdigits:
                new = ''
                cnt = 1
                for i in range(len(intStr)-1, -1, -1):
                    new = intStr[i] + new
                    if (cnt) % 3 == 0:
                        new = self._groupChar + new
                    cnt += 1
                if new and new[0] == self._groupChar:
                    new = new[1:]
                if len(new) <= length:
                    # expanded string will still fit and leave room for sign:
                    intStr = new
                # else... leave it without the commas...

##            dbg('padzero?', self._padZero)
##            dbg('len(intStr):', len(intStr), 'field length:', length)
            if self._padZero and len(intStr) < length:
                intStr = '0' * (length - len(intStr)) + intStr
                if signpos != -1:   # we had a sign before; restore it
                    if self._useParensForNegatives:
                        intStr = '(' + intStr[1:]
                        if right_signpos != -1:
                            intStr += ')'
                    else:
                        intStr = '-' + intStr[1:]
            elif signpos != -1 and slice[0:signpos].strip() == '':    # - was before digits
                if self._useParensForNegatives:
                    intStr = '(' + intStr
                    if right_signpos != -1:
                        intStr += ')'
                else:
                    intStr = '-' + intStr
            elif right_signpos != -1:
                # must have had ')' but '(' was before field; re-add ')'
                intStr += ')'
            slice = intStr

        slice = slice.strip() # drop extra spaces

        if self._alignRight:     ## Only if right-alignment is enabled
            slice = slice.rjust( length )
        else:
            slice = slice.ljust( length )
        if self._fillChar != ' ':
            slice = slice.replace(' ', self._fillChar)
##        dbg('adjusted slice: "%s"' % slice, indent=0)
        return slice


## ---------- ---------- ---------- ---------- ---------- ---------- ----------

class MaskedEditMixin:
    """
    This class allows us to abstract the masked edit functionality that could
    be associated with any text entry control. (eg. wx.TextCtrl, wx.ComboBox, etc.)
    It forms the basis for all of the lib.masked controls.
    """
    valid_ctrl_params = {
              'mask': 'XXXXXXXXXXXXX',          ## mask string for formatting this control
              'autoformat':   "",               ## optional auto-format code to set format from masktags dictionary
              'fields': {},                     ## optional list/dictionary of maskededit.Field class instances, indexed by position in mask
              'datestyle':    'MDY',            ## optional date style for date-type values. Can trigger autocomplete year
              'autoCompleteKeycodes': [],       ## Optional list of additional keycodes which will invoke field-auto-complete
              'useFixedWidthFont': True,        ## Use fixed-width font instead of default for base control
              'defaultEncoding': 'latin1',      ## optional argument to indicate unicode codec to use (unicode ctrls only)
              'retainFieldValidation': False,   ## Set this to true if setting control-level parameters independently,
                                                ## from field validation constraints
              'emptyBackgroundColour': "White",
              'validBackgroundColour': "White",
              'invalidBackgroundColour': "Yellow",
              'foregroundColour': "Black",
              'signedForegroundColour': "Red",
              'demo': False}


    def __init__(self, name = 'MaskedEdit', **kwargs):
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
        self._previous_mask = None

        # Validate legitimate set of parameters:
        for key in kwargs.keys():
            if key.replace('Color', 'Colour') not in MaskedEditMixin.valid_ctrl_params.keys() + Field.valid_params.keys():
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
            wx.WXK_BACK:   self._OnErase,
            wx.WXK_LEFT:   self._OnArrow,
            wx.WXK_RIGHT:  self._OnArrow,
            wx.WXK_UP:     self._OnAutoCompleteField,
            wx.WXK_DOWN:   self._OnAutoCompleteField,
            wx.WXK_TAB:    self._OnChangeField,
            wx.WXK_HOME:   self._OnHome,
            wx.WXK_END:    self._OnEnd,
            wx.WXK_RETURN: self._OnReturn,
            wx.WXK_PRIOR:  self._OnAutoCompleteField,
            wx.WXK_NEXT:   self._OnAutoCompleteField,

            # default function control keys and handlers:
            wx.WXK_DELETE: self._OnDelete,
            wx.WXK_INSERT: self._OnInsert,
            WXK_CTRL_A: self._OnCtrl_A,
            WXK_CTRL_C: self._OnCtrl_C,
            WXK_CTRL_S: self._OnCtrl_S,
            WXK_CTRL_V: self._OnCtrl_V,
            WXK_CTRL_X: self._OnCtrl_X,
            WXK_CTRL_Z: self._OnCtrl_Z,
            }

        ## bind standard navigational and control keycodes to this instance,
        ## so that they can be augmented and/or changed in derived classes:
        self._nav = list(nav)
        self._control = list(control)

        ## Dynamically evaluate and store string constants for mask chars
        ## so that locale settings can be made after this module is imported
        ## and the controls created after that is done can allow the
        ## appropriate characters:
        self.maskchardict  = {
            '#': string.digits,
            'A': string.uppercase,
            'a': string.lowercase,
            'X': string.letters + string.punctuation + string.digits,
            'C': string.letters,
            'N': string.letters + string.digits,
            '&': string.punctuation,
            '*': ansichars  # to give it a value, but now allows any non-wxcontrol character
        }

        ## self._ignoreChange is used by MaskedComboBox, because
        ## of the hack necessary to determine the selection; it causes
        ## EVT_TEXT messages from the combobox to be ignored if set.
        self._ignoreChange = False

        # These are used to keep track of previous value, for undo functionality:
        self._curValue  = None
        self._prevValue = None

        self._valid     = True

        # Set defaults for each parameter for this instance, and fully
        # populate initial parameter list for configuration:
        for key, value in MaskedEditMixin.valid_ctrl_params.items():
            setattr(self, '_' + key, copy.copy(value))
            if not kwargs.has_key(key):
####                dbg('%s: "%s"' % (key, repr(value)))
                kwargs[key] = copy.copy(value)

        # Create a "field" that holds global parameters for control constraints
        self._ctrl_constraints = self._fields[-1] = Field(index=-1)
        self.SetCtrlParameters(**kwargs)



    def SetCtrlParameters(self, **kwargs):
        """
        This public function can be used to set individual or multiple masked edit
        parameters after construction.  (See maskededit module overview for the list
        of valid parameters.)
        """
##        dbg(suspend=1)
##        dbg('MaskedEditMixin::SetCtrlParameters', indent=1)
####        dbg('kwargs:', indent=1)
##        for key, value in kwargs.items():
####            dbg(key, '=', value)
####        dbg(indent=0)

        # Validate keyword arguments:
        constraint_kwargs = {}
        ctrl_kwargs = {}
        for key, value in kwargs.items():
            key = key.replace('Color', 'Colour')    # for b-c, and standard wxPython spelling
            if key not in MaskedEditMixin.valid_ctrl_params.keys() + Field.valid_params.keys():
##                dbg(indent=0, suspend=0)
                ae = AttributeError('Invalid keyword argument "%s" for control "%s"' % (key, self.name))
                ae.attribute = key
                raise ae
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

        # handle "parochial name" backward compatibility:
        if autoformat and autoformat.find('MILTIME') != -1 and autoformat not in masktags.keys():
            autoformat = autoformat.replace('MILTIME', '24HRTIME')

        if autoformat != self._autoformat and autoformat in masktags.keys():
##            dbg('autoformat:', autoformat)
            self._autoformat                  = autoformat
            mask                              = masktags[self._autoformat]['mask']
            # gather rest of any autoformat parameters:
            for param, value in masktags[self._autoformat].items():
                if param == 'mask': continue    # (must be present; already accounted for)
                constraint_kwargs[param] = value

        elif autoformat and not autoformat in masktags.keys():
            ae = AttributeError('invalid value for autoformat parameter: %s' % repr(autoformat))
            ae.attribute = autoformat
            raise ae
        else:
##            dbg('autoformat not selected')
            if kwargs.has_key('mask'):
                mask = kwargs['mask']
##                dbg('mask:', mask)

        ## Assign style flags
        if mask is None:
##            dbg('preserving previous mask')
            mask = self._previous_mask   # preserve previous mask
        else:
##            dbg('mask (re)set')
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
##                        dbg(indent=0, suspend=0)
                        raise TypeError('invalid type for field parameter: %s' % repr(field))
                    self._fields[i] = field

            elif type(fields) == types.DictionaryType:
                for index, field in fields.items():
                    if not isinstance(field, Field):
##                        dbg(indent=0, suspend=0)
                        raise TypeError('invalid type for field parameter: %s' % repr(field))
                    self._fields[index] = field
            else:
##                dbg(indent=0, suspend=0)
                raise TypeError('fields parameter must be a list or dictionary; not %s' % repr(fields))

        # Assign constraint parameters for entire control:
####        dbg('control constraints:', indent=1)
##        for key, value in constraint_kwargs.items():
####            dbg('%s:' % key, value)
####        dbg(indent=0)

        # determine if changing parameters that should affect the entire control:
        for key in MaskedEditMixin.valid_ctrl_params.keys():
            if key in ( 'mask', 'fields' ): continue    # (processed separately)
            if ctrl_kwargs.has_key(key):
                setattr(self, '_' + key, ctrl_kwargs[key])

        # Validate color parameters, converting strings to named colors and validating
        # result if appropriate:
        for key in ('emptyBackgroundColour', 'invalidBackgroundColour', 'validBackgroundColour',
                    'foregroundColour', 'signedForegroundColour'):
            if ctrl_kwargs.has_key(key):
                if type(ctrl_kwargs[key]) in (types.StringType, types.UnicodeType):
                    c = wx.NamedColour(ctrl_kwargs[key])
                    if c.Get() == (-1, -1, -1):
                        raise TypeError('%s not a legal color specification for %s' % (repr(ctrl_kwargs[key]), key))
                    else:
                        # replace attribute with wxColour object:
                        setattr(self, '_' + key, c)
                        # attach a python dynamic attribute to wxColour for debug printouts
                        c._name = ctrl_kwargs[key]

                elif type(ctrl_kwargs[key]) != type(wx.BLACK):
                    raise TypeError('%s not a legal color specification for %s' % (repr(ctrl_kwargs[key]), key))


##        dbg('self._retainFieldValidation:', self._retainFieldValidation)
        if not self._retainFieldValidation:
            # Build dictionary of any changing parameters which should be propagated to the
            # component fields:
            for arg in Field.propagating_params:
####                dbg('kwargs.has_key(%s)?' % arg, kwargs.has_key(arg))
####                dbg('getattr(self._ctrl_constraints, _%s)?' % arg, getattr(self._ctrl_constraints, '_'+arg))
                reset_args[arg] = kwargs.has_key(arg) and kwargs[arg] != getattr(self._ctrl_constraints, '_'+arg)
####                dbg('reset_args[%s]?' % arg, reset_args[arg])

        # Set the control-level constraints:
        self._ctrl_constraints._SetParameters(**constraint_kwargs)

        # This routine does the bulk of the interdependent parameter processing, determining
        # the field extents of the mask if changed, resetting parameters as appropriate,
        # determining the overall template value for the control, etc.
        self._configure(mask, **reset_args)

        # now that we've propagated the field constraints and mask portions to the
        # various fields, validate the constraints
        self._ctrl_constraints._ValidateParameters(**constraint_kwargs)

        # Validate that all choices for given fields are at least of the
        # necessary length, and that they all would be valid pastes if pasted
        # into their respective fields:
####        dbg('validating choices')
        self._validateChoices()


        self._autofit = self._ctrl_constraints._autofit
        self._isNeg      = False

        self._isDate     = 'D' in self._ctrl_constraints._formatcodes and _isDateType(mask)
        self._isTime     = 'T' in self._ctrl_constraints._formatcodes and _isTimeType(mask)
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

        # Give derived controls a chance to react to parameter changes before
        # potentially changing current value of the control.
        self._OnCtrlParametersChanged()

        if self.controlInitialized:
            # Then the base control is available for configuration;
            # take action on base control based on new settings, as appropriate.
            if kwargs.has_key('useFixedWidthFont'):
                # Set control font - fixed width by default
                self._setFont()

            if reset_args.has_key('reset_mask'):
##                dbg('reset mask')
                curvalue = self._GetValue()
                if curvalue.strip():
                    try:
##                        dbg('attempting to _SetInitialValue(%s)' % self._GetValue())
                        self._SetInitialValue(self._GetValue())
                    except Exception, e:
##                        dbg('exception caught:', e)
##                        dbg("current value doesn't work; attempting to reset to template")
                        self._SetInitialValue()
                else:
##                    dbg('attempting to _SetInitialValue() with template')
                    self._SetInitialValue()

            elif kwargs.has_key('useParensForNegatives'):
                newvalue = self._getSignedValue()[0]

                if newvalue is not None:
                    # Adjust for new mask:
                    if len(newvalue) < len(self._mask):
                        newvalue += ' '
                    elif len(newvalue) > len(self._mask):
                        if newvalue[-1] in (' ', ')'):
                            newvalue = newvalue[:-1]

##                    dbg('reconfiguring value for parens:"%s"' % newvalue)
                    self._SetValue(newvalue)

                    if self._prevValue != newvalue:
                        self._prevValue = newvalue  # disallow undo of sign type

            if self._autofit:
##                dbg('calculated size:', self._CalcSize())
                self.SetClientSize(self._CalcSize())
                width = self.GetSize().width
                height = self.GetBestSize().height
##                dbg('setting client size to:', (width, height))
                self.SetInitialSize((width, height))

            # Set value/type-specific formatting
            self._applyFormatting()
##        dbg(indent=0, suspend=0)

    def SetMaskParameters(self, **kwargs):
        """ old name for the SetCtrlParameters function  (DEPRECATED)"""
        return self.SetCtrlParameters(**kwargs)


    def GetCtrlParameter(self, paramname):
        """
        Routine for retrieving the value of any given parameter
        """
        if MaskedEditMixin.valid_ctrl_params.has_key(paramname.replace('Color','Colour')):
            return getattr(self, '_' + paramname.replace('Color', 'Colour'))
        elif Field.valid_params.has_key(paramname):
            return self._ctrl_constraints._GetParameter(paramname)
        else:
            TypeError('"%s".GetCtrlParameter: invalid parameter "%s"' % (self.name, paramname))

    def GetMaskParameter(self, paramname):
        """ old name for the GetCtrlParameters function  (DEPRECATED)"""
        return self.GetCtrlParameter(paramname)


## This idea worked, but Boa was unable to use this solution...
##    def _attachMethod(self, func):
##        import new
##        setattr(self, func.__name__, new.instancemethod(func, self, self.__class__))
##
##
##    def _DefinePropertyFunctions(exposed_params):
##        for param in exposed_params:
##            propname = param[0].upper() + param[1:]
##
##            exec('def Set%s(self, value): self.SetCtrlParameters(%s=value)' % (propname, param))
##            exec('def Get%s(self): return self.GetCtrlParameter("%s")''' % (propname, param))
##            self._attachMethod(locals()['Set%s' % propname])
##            self._attachMethod(locals()['Get%s' % propname])
##
##            if param.find('Colour') != -1:
##                # add non-british spellings, for backward-compatibility
##                propname.replace('Colour', 'Color')
##
##                exec('def Set%s(self, value): self.SetCtrlParameters(%s=value)' % (propname, param))
##                exec('def Get%s(self): return self.GetCtrlParameter("%s")''' % (propname, param))
##                self._attachMethod(locals()['Set%s' % propname])
##                self._attachMethod(locals()['Get%s' % propname])
##


    def SetFieldParameters(self, field_index, **kwargs):
        """
        Routine provided to modify the parameters of a given field.
        Because changes to fields can affect the overall control,
        direct access to the fields is prevented, and the control
        is always "reconfigured" after setting a field parameter.
        (See maskededit module overview for the list of valid field-level
        parameters.)
        """
        if field_index not in self._field_indices:
            ie = IndexError('%s is not a valid field for control "%s".' % (str(field_index), self.name))
            ie.index = field_index
            raise ie
        # set parameters as requested:
        self._fields[field_index]._SetParameters(**kwargs)

        # Possibly reprogram control template due to resulting changes, and ensure
        # control-level params are still propagated to fields:
        self._configure(self._previous_mask)
        self._fields[field_index]._ValidateParameters(**kwargs)

        if self.controlInitialized:
            if kwargs.has_key('fillChar') or kwargs.has_key('defaultValue'):
                self._SetInitialValue()

                if self._autofit:
                    # this is tricky, because, as Robin explains:
                    # "Basically there are two sizes to deal with, that are potentially 
                    #  different.  The client size is the inside size and may, depending
                    #  on platform, exclude the borders and such.  The normal size is
                    #  the outside size that does include the borders.  What you are
                    #  calculating (in _CalcSize) is the client size, but the sizers
                    #  deal with the full size and so that is the minimum size that
                    #  we need to set with SetInitialSize.  The root of the problem is
                    #  that in _calcSize the current client size height is returned,
                    #  instead of a height based on the current font.  So I suggest using
                    #  _calcSize to just get the width, and then use GetBestSize to
                    #  get the height."
                    self.SetClientSize(self._CalcSize())
                    width = self.GetSize().width
                    height = self.GetBestSize().height
                    self.SetInitialSize((width, height))


            # Set value/type-specific formatting
            self._applyFormatting()


    def GetFieldParameter(self, field_index, paramname):
        """
        Routine provided for getting a parameter of an individual field.
        """
        if field_index not in self._field_indices:
            ie = IndexError('%s is not a valid field for control "%s".' % (str(field_index), self.name))
            ie.index = field_index
            raise ie
        elif Field.valid_params.has_key(paramname):
            return self._fields[field_index]._GetParameter(paramname)
        else:
            ae = AttributeError('"%s".GetFieldParameter: invalid parameter "%s"' % (self.name, paramname))
            ae.attribute = paramname
            raise ae


    def _SetKeycodeHandler(self, keycode, func):
        """
        This function adds and/or replaces key event handling functions
        used by the control.  <func> should take the event as argument
        and return False if no further action on the key is necessary.
        """
        if func:
            self._keyhandlers[keycode] = func
        elif self._keyhandlers.has_key(keycode):
            del self._keyhandlers[keycode]


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
        elif self.keyhandlers.has_key(keycode):
            del self._keyhandlers[keycode]



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
            elif self.keyhandlers.has_key(keycode):
                del self._keyhandlers[keycode]


    def _processMask(self, mask):
        """
        This subroutine expands {n} syntax in mask strings, and looks for escaped
        special characters and returns the expanded mask, and an dictionary
        of booleans indicating whether or not a given position in the mask is
        a mask character or not.
        """
##        dbg('_processMask: mask', mask, indent=1)
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

        self._decimalChar = self._ctrl_constraints._decimalChar
        self._shiftDecimalChar = self._ctrl_constraints._shiftDecimalChar

        self._isFloat      = _isFloatingPoint(s) and not self._ctrl_constraints._validRegex
        self._isInt      = _isInteger(s) and not self._ctrl_constraints._validRegex
        self._signOk     = '-' in self._ctrl_constraints._formatcodes and (self._isFloat or self._isInt)
        self._useParens  = self._ctrl_constraints._useParensForNegatives
        self._isNeg      = False
####        dbg('self._signOk?', self._signOk, 'self._useParens?', self._useParens)
####        dbg('isFloatingPoint(%s)?' % (s), _isFloatingPoint(s),
##            'ctrl regex:', self._ctrl_constraints._validRegex)

        if self._signOk and s[0] != ' ':
            s = ' ' + s
            if self._ctrl_constraints._defaultValue and self._ctrl_constraints._defaultValue[0] != ' ':
                self._ctrl_constraints._defaultValue = ' ' + self._ctrl_constraints._defaultValue
            self._signpos = 0

            if self._useParens:
                s += ' '
                self._ctrl_constraints._defaultValue += ' '


        # Now, go build up a dictionary of booleans, indexed by position,
        # indicating whether or not a given position is masked or not.
        # Also, strip out any '|' chars, adjusting the mask as necessary,
        # marking the appropriate positions for field boundaries:
        ismasked = {}
        explicit_field_boundaries = []
        i = 0
        while i < len(s):
            if s[i] == '\\':            # if escaped character:
                ismasked[i] = False     #     mark position as not a mask char
                if i+1 < len(s):        #     if another char follows...
                    s = s[:i] + s[i+1:] #         elide the '\'
                    if i+2 < len(s) and s[i+1] == '\\':
                        # if next char also a '\', char is a literal '\'
                        s = s[:i] + s[i+1:]     # elide the 2nd '\' as well
                i += 1                      # increment to next char
            elif s[i] == '|':
                s = s[:i] + s[i+1:] #         elide the '|'
                explicit_field_boundaries.append(i)
                # keep index where it is:
            else:                       # else if special char, mark position accordingly
                ismasked[i] = s[i] in maskchars
####                dbg('ismasked[%d]:' % i, ismasked[i], s)
                i += 1                      # increment to next char
####        dbg('ismasked:', ismasked)
##        dbg('new mask: "%s"' % s, indent=0)

        return s, ismasked, explicit_field_boundaries


    def _calcFieldExtents(self):
        """
        Subroutine responsible for establishing/configuring field instances with
        indices and editable extents appropriate to the specified mask, and building
        the lookup table mapping each position to the corresponding field.
        """
        self._lookupField = {}
        if self._mask:

            ## Create dictionary of positions,characters in mask
            self.maskdict = {}
            for charnum in range( len( self._mask)):
                self.maskdict[charnum] = self._mask[charnum:charnum+1]

            # For the current mask, create an ordered list of field extents
            # and a dictionary of positions that map to field indices:

            if self._signOk: start = 1
            else: start = 0

            if self._isFloat:
                # Skip field "discovery", and just construct a 2-field control with appropriate
                # constraints for a floating-point entry.

                # .setdefault always constructs 2nd argument even if not needed, so we do this
                # the old-fashioned way...
                if not self._fields.has_key(0):
                    self._fields[0] = Field()
                if not self._fields.has_key(1):
                    self._fields[1] = Field()

                self._decimalpos = string.find( self._mask, '.')
##                dbg('decimal pos =', self._decimalpos)

                formatcodes = self._fields[0]._GetParameter('formatcodes')
                if 'R' not in formatcodes: formatcodes += 'R'
                self._fields[0]._SetParameters(index=0, extent=(start, self._decimalpos),
                                               mask=self._mask[start:self._decimalpos], formatcodes=formatcodes)
                end = len(self._mask)
                if self._signOk and self._useParens:
                    end -= 1
                self._fields[1]._SetParameters(index=1, extent=(self._decimalpos+1, end),
                                               mask=self._mask[self._decimalpos+1:end])

                for i in range(self._decimalpos+1):
                    self._lookupField[i] = 0

                for i in range(self._decimalpos+1, len(self._mask)+1):
                    self._lookupField[i] = 1

            elif self._isInt:
                # Skip field "discovery", and just construct a 1-field control with appropriate
                # constraints for a integer entry.
                if not self._fields.has_key(0):
                    self._fields[0] = Field(index=0)
                end = len(self._mask)
                if self._signOk and self._useParens:
                    end -= 1
                self._fields[0]._SetParameters(index=0, extent=(start, end),
                                               mask=self._mask[start:end])
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
####                    dbg('searching: outer field loop: i = ', i)
                    if self._isMaskChar(i):
####                        dbg('1st char is mask char; recording edit_start=', i)
                        edit_start = i
                        # Skip to end of editable part of current field:
                        while i < len(self._mask) and self._isMaskChar(i):
                            self._lookupField[i] = field_index
                            i += 1
                            if i in self._explicit_field_boundaries:
                                break
####                        dbg('edit_end =', i)
                        edit_end = i
                        self._lookupField[i] = field_index
####                        dbg('self._fields.has_key(%d)?' % field_index, self._fields.has_key(field_index))
                        if not self._fields.has_key(field_index):
                            kwargs = Field.valid_params.copy()
                            kwargs['index'] = field_index
                            kwargs['extent'] = (edit_start, edit_end)
                            kwargs['mask'] = self._mask[edit_start:edit_end]
                            self._fields[field_index] = Field(**kwargs)
                        else:
                            self._fields[field_index]._SetParameters(
                                                                index=field_index,
                                                                extent=(edit_start, edit_end),
                                                                mask=self._mask[edit_start:edit_end])
                    pos = i
                    i = self._findNextEntry(pos, adjustInsert=False)  # go to next field:
####                    dbg('next entry:', i)
                    if i > pos:
                        for j in range(pos, i+1):
                            self._lookupField[j] = field_index
                    if i >= len(self._mask):
                        break           # if past end, we're done
                    else:
                        field_index += 1
####                        dbg('next field:', field_index)

        indices = self._fields.keys()
        indices.sort()
        self._field_indices = indices[1:]
####        dbg('lookupField map:', indent=1)
##        for i in range(len(self._mask)):
####            dbg('pos %d:' % i, self._lookupField[i])
####        dbg(indent=0)

        # Verify that all field indices specified are valid for mask:
        for index in self._fields.keys():
            if index not in [-1] + self._lookupField.values():
                ie = IndexError('field %d is not a valid field for mask "%s"' % (index, self._mask))
                ie.index = index
                raise ie



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
##        dbg('default set?', default_set)

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
####            dbg('pos:', pos)
            field = self._FindField(pos)
####            dbg('field:', field._index)
            start, end = field._extent

            if pos == 0 and self._signOk:
                self._template = ' ' # always make 1st 1st position blank, regardless of fillchar
            elif self._isFloat and pos == self._decimalpos:
                self._template += self._decimalChar
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
####                    dbg('no default value')
                    self._template += fillChar
                    field._template += fillChar

                elif field._defaultValue and not reset_default:
####                    dbg('len(field._defaultValue):', len(field._defaultValue))
####                    dbg('pos-start:', pos-start)
                    if len(field._defaultValue) > pos-start:
####                        dbg('field._defaultValue[pos-start]: "%s"' % field._defaultValue[pos-start])
                        self._template += field._defaultValue[pos-start]
                        field._template += field._defaultValue[pos-start]
                    else:
####                        dbg('field default not long enough; using fillChar')
                        self._template += fillChar
                        field._template += fillChar
                else:
                    if len(self._ctrl_constraints._defaultValue) > pos:
####                        dbg('using control default')
                        self._template += self._ctrl_constraints._defaultValue[pos]
                        field._template += self._ctrl_constraints._defaultValue[pos]
                    else:
####                        dbg('ctrl default not long enough; using fillChar')
                        self._template += fillChar
                        field._template += fillChar
####                dbg('field[%d]._template now "%s"' % (field._index, field._template))
####                dbg('self._template now "%s"' % self._template)
            else:
                self._template += self._mask[pos]

        self._fields[-1]._template = self._template     # (for consistency)

        if curvalue:    # had an old value, put new one back together
            newvalue = string.join(curvalue, "")
        else:
            newvalue = None

        if default_set:
            self._defaultValue = self._template
##            dbg('self._defaultValue:', self._defaultValue)
            if not self.IsEmpty(self._defaultValue) and not self.IsValid(self._defaultValue):
####                dbg(indent=0)
                ve = ValueError('Default value of "%s" is not a valid value for control "%s"' % (self._defaultValue, self.name))
                ve.value = self._defaultValue
                raise ve

            # if no fillchar change, but old value == old template, replace it:
            if newvalue == old_template:
                newvalue = self._template
                reset_value = True
        else:
            self._defaultValue = None

        if reset_value:
##            dbg('resetting value to: "%s"' % newvalue)
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
        parent_includes = self._ctrl_constraints._includeChars
        parent_excludes = self._ctrl_constraints._excludeChars
        for i in self._field_indices:
            field = self._fields[i]
            inherit_args = {}
            if len(self._field_indices) == 1:
                inherit_args['formatcodes'] = parent_codes
                inherit_args['includeChars'] = parent_includes
                inherit_args['excludeChars'] = parent_excludes
            else:
                field_codes = current_codes = field._GetParameter('formatcodes')
                for c in parent_codes:
                    if c not in field_codes: field_codes += c
                if field_codes != current_codes:
                    inherit_args['formatcodes'] = field_codes

                include_chars = current_includes = field._GetParameter('includeChars')
                for c in parent_includes:
                    if not c in include_chars: include_chars += c
                if include_chars != current_includes:
                    inherit_args['includeChars'] = include_chars

                exclude_chars = current_excludes = field._GetParameter('excludeChars')
                for c in parent_excludes:
                    if not c in exclude_chars: exclude_chars += c
                if exclude_chars != current_excludes:
                    inherit_args['excludeChars'] = exclude_chars

            if reset_args.has_key('defaultValue') and reset_args['defaultValue']:
                inherit_args['defaultValue'] = ""   # (reset for field)

            for param in Field.propagating_params:
####                dbg('reset_args.has_key(%s)?' % param, reset_args.has_key(param))
####                dbg('reset_args.has_key(%(param)s) and reset_args[%(param)s]?' % locals(), reset_args.has_key(param) and reset_args[param])
                if reset_args.has_key(param):
                    inherit_args[param] = self.GetCtrlParameter(param)
####                    dbg('inherit_args[%s]' % param, inherit_args[param])

            if inherit_args:
                field._SetParameters(**inherit_args)
                field._ValidateParameters(**inherit_args)


    def _validateChoices(self):
        """
        Subroutine that validates that all choices for given fields are at
        least of the necessary length, and that they all would be valid pastes
        if pasted into their respective fields.
        """
        for field in self._fields.values():
            if field._choices:
                index = field._index
                if len(self._field_indices) == 1 and index == 0 and field._choices == self._ctrl_constraints._choices:
##                    dbg('skipping (duplicate) choice validation of field 0')
                    continue
####                dbg('checking for choices for field', field._index)
                start, end = field._extent
                field_length = end - start
####                dbg('start, end, length:', start, end, field_length)
                for choice in field._choices:
####                    dbg('testing "%s"' % choice)
                    valid_paste, ignore, replace_to = self._validatePaste(choice, start, end)
                    if not valid_paste:
####                        dbg(indent=0)
                        ve = ValueError('"%s" could not be entered into field %d of control "%s"' % (choice, index, self.name))
                        ve.value = choice
                        ve.index = index
                        raise ve
                    elif replace_to > end:
####                        dbg(indent=0)
                        ve = ValueError('"%s" will not fit into field %d of control "%s"' (choice, index, self.name))
                        ve.value = choice
                        ve.index = index
                        raise ve

####                    dbg(choice, 'valid in field', index)


    def _configure(self, mask, **reset_args):
        """
        This function sets flags for automatic styling options.  It is
        called whenever a control or field-level parameter is set/changed.

        This routine does the bulk of the interdependent parameter processing, determining
        the field extents of the mask if changed, resetting parameters as appropriate,
        determining the overall template value for the control, etc.

        reset_args is supplied if called from control's .SetCtrlParameters()
        routine, and indicates which if any parameters which can be
        overridden by individual fields have been reset by request for the
        whole control.

        """
##        dbg(suspend=1)
##        dbg('MaskedEditMixin::_configure("%s")' % mask, indent=1)

        # Preprocess specified mask to expand {n} syntax, handle escaped
        # mask characters, etc and build the resulting positionally keyed
        # dictionary for which positions are mask vs. template characters:
        self._mask, self._ismasked, self._explicit_field_boundaries = self._processMask(mask)
        self._masklength = len(self._mask)
####        dbg('processed mask:', self._mask)

        # Preserve original mask specified, for subsequent reprocessing
        # if parameters change.
##        dbg('mask: "%s"' % self._mask, 'previous mask: "%s"' % self._previous_mask)
        self._previous_mask = mask    # save unexpanded mask for next time
            # Set expanded mask and extent of field -1 to width of entire control:
        self._ctrl_constraints._SetParameters(mask = self._mask, extent=(0,self._masklength))

        # Go parse mask to determine where each field is, construct field
        # instances as necessary, configure them with those extents, and
        # build lookup table mapping each position for control to its corresponding
        # field.
####        dbg('calculating field extents')

        self._calcFieldExtents()


        # Go process defaultValues and fillchars to construct the overall
        # template, and adjust the current value as necessary:
        reset_fillchar = reset_args.has_key('fillChar') and reset_args['fillChar']
        reset_default = reset_args.has_key('defaultValue') and reset_args['defaultValue']

####        dbg('calculating template')
        self._calcTemplate(reset_fillchar, reset_default)

        # Propagate control-level formatting and character constraints to each
        # field if they don't already have them; if only one field, propagate
        # control-level validation constraints to field as well:
####        dbg('propagating constraints')
        self._propagateConstraints(**reset_args)


        if self._isFloat and self._fields[0]._groupChar == self._decimalChar:
            raise AttributeError('groupChar (%s) and decimalChar (%s) must be distinct.' %
                                 (self._fields[0]._groupChar, self._decimalChar) )

####        dbg('fields:', indent=1)
##        for i in [-1] + self._field_indices:
####            dbg('field %d:' % i, self._fields[i].__dict__)
####        dbg(indent=0)

        # Set up special parameters for numeric control, if appropriate:
        if self._signOk:
            self._signpos = 0   # assume it starts here, but it will move around on floats
            signkeys = ['-', '+', ' ']
            if self._useParens:
                signkeys += ['(', ')']
            for key in signkeys:
                keycode = ord(key)
                if not self._keyhandlers.has_key(keycode):
                    self._SetKeyHandler(key, self._OnChangeSign)
        elif self._isInt or self._isFloat:
            signkeys = ['-', '+', ' ', '(', ')']
            for key in signkeys:
                keycode = ord(key)
                if self._keyhandlers.has_key(keycode) and self._keyhandlers[keycode] == self._OnChangeSign:
                    self._SetKeyHandler(key, None)



        if self._isFloat or self._isInt:
            if self.controlInitialized:
                value = self._GetValue()
####                dbg('value: "%s"' % value, 'len(value):', len(value),
##                    'len(self._ctrl_constraints._mask):',len(self._ctrl_constraints._mask))
                if len(value) < len(self._ctrl_constraints._mask):
                    newvalue = value
                    if self._useParens and len(newvalue) < len(self._ctrl_constraints._mask) and newvalue.find('(') == -1:
                        newvalue += ' '
                    if self._signOk and len(newvalue) < len(self._ctrl_constraints._mask) and newvalue.find(')') == -1:
                        newvalue = ' ' + newvalue
                    if len(newvalue) < len(self._ctrl_constraints._mask):
                        if self._ctrl_constraints._alignRight:
                            newvalue = newvalue.rjust(len(self._ctrl_constraints._mask))
                        else:
                            newvalue = newvalue.ljust(len(self._ctrl_constraints._mask))
##                    dbg('old value: "%s"' % value)
##                    dbg('new value: "%s"' % newvalue)
                    try:
                        self._SetValue(newvalue)
                    except Exception, e:
##                        dbg('exception raised:', e, 'resetting to initial value')
                        self._SetInitialValue()

                elif len(value) > len(self._ctrl_constraints._mask):
                    newvalue = value
                    if not self._useParens and newvalue[-1] == ' ':
                        newvalue = newvalue[:-1]
                    if not self._signOk and len(newvalue) > len(self._ctrl_constraints._mask):
                        newvalue = newvalue[1:]
                    if not self._signOk:
                        newvalue, signpos, right_signpos = self._getSignedValue(newvalue)

##                    dbg('old value: "%s"' % value)
##                    dbg('new value: "%s"' % newvalue)
                    try:
                        self._SetValue(newvalue)
                    except Exception, e:
##                        dbg('exception raised:', e, 'resetting to initial value')
                        self._SetInitialValue()
                elif not self._signOk and ('(' in value or '-' in value):
                    newvalue, signpos, right_signpos = self._getSignedValue(value)
##                    dbg('old value: "%s"' % value)
##                    dbg('new value: "%s"' % newvalue)
                    try:
                        self._SetValue(newvalue)
                    except e:
##                        dbg('exception raised:', e, 'resetting to initial value')
                        self._SetInitialValue()

            # Replace up/down arrow default handling:
            # make down act like tab, up act like shift-tab:

####            dbg('Registering numeric navigation and control handlers (if not already set)')
            if not self._keyhandlers.has_key(wx.WXK_DOWN):
                self._SetKeycodeHandler(wx.WXK_DOWN, self._OnChangeField)
            if not self._keyhandlers.has_key(wx.WXK_UP):
                self._SetKeycodeHandler(wx.WXK_UP, self._OnUpNumeric)  # (adds "shift" to up arrow, and calls _OnChangeField)

            # On ., truncate contents right of cursor to decimal point (if any)
            # leaves cursor after decimal point if floating point, otherwise at 0.
            if not self._keyhandlers.has_key(ord(self._decimalChar)) or self._keyhandlers[ord(self._decimalChar)] != self._OnDecimalPoint:
                self._SetKeyHandler(self._decimalChar, self._OnDecimalPoint)
    
            if not self._keyhandlers.has_key(ord(self._shiftDecimalChar)) or self._keyhandlers[ord(self._shiftDecimalChar)] != self._OnChangeField:
                self._SetKeyHandler(self._shiftDecimalChar, self._OnChangeField)   # (Shift-'.' == '>' on US keyboards)

            # Allow selective insert of groupchar in numbers:
            if not self._keyhandlers.has_key(ord(self._fields[0]._groupChar)) or self._keyhandlers[ord(self._fields[0]._groupChar)] != self._OnGroupChar:
                self._SetKeyHandler(self._fields[0]._groupChar, self._OnGroupChar)

##        dbg(indent=0, suspend=0)


    def _SetInitialValue(self, value=""):
        """
        fills the control with the generated or supplied default value.
        It will also set/reset the font if necessary and apply
        formatting to the control at this time.
        """
##        dbg('MaskedEditMixin::_SetInitialValue("%s")' % value, indent=1)
        if not value:
            self._prevValue = self._curValue = self._template
            # don't apply external validation rules in this case, as template may
            # not coincide with "legal" value...
            try:
                self._SetValue(self._curValue)  # note the use of "raw" ._SetValue()...
            except Exception, e:
##                dbg('exception thrown:', e, indent=0)
                raise
        else:
            # Otherwise apply validation as appropriate to passed value:
####            dbg('value = "%s", length:' % value, len(value))
            self._prevValue = self._curValue = value
            try:
                self.SetValue(value)            # use public (validating) .SetValue()
            except Exception, e:
##                dbg('exception thrown:', e, indent=0)
                raise


        # Set value/type-specific formatting
        self._applyFormatting()
##        dbg(indent=0)


    def _calcSize(self, size=None):
        """ Calculate automatic size if allowed; must be called after the base control is instantiated"""
####        dbg('MaskedEditMixin::_calcSize', indent=1)
        cont = (size is None or size == wx.DefaultSize)

        if cont and self._autofit:
            sizing_text = 'M' * self._masklength
            if wx.Platform != "__WXMSW__":   # give it a little extra space
                sizing_text += 'M'
            if wx.Platform == "__WXMAC__":   # give it even a little more...
                sizing_text += 'M'
####            dbg('len(sizing_text):', len(sizing_text), 'sizing_text: "%s"' % sizing_text)
            w, h = self.GetTextExtent(sizing_text)
            size = (w+4, self.GetSize().height)
####            dbg('size:', size, indent=0)
        return size


    def _setFont(self):
        """ Set the control's font typeface -- pass the font name as str."""
####        dbg('MaskedEditMixin::_setFont', indent=1)
        if not self._useFixedWidthFont:
            self._font = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
        else:
            font = self.GetFont()   # get size, weight, etc from current font

            # Set to teletype font (guaranteed to be mappable to all wxWindows
            # platforms:
            self._font = wx.Font( font.GetPointSize(), wx.TELETYPE, font.GetStyle(),
                                 font.GetWeight(), font.GetUnderlined())
####            dbg('font string: "%s"' % font.GetNativeFontInfo().ToString())

        self.SetFont(self._font)
####        dbg(indent=0)


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
##        dbg('MaskedEditMixin::_OnTextChange: value: "%s"' % newvalue, indent=1)
        bValid = False
        if self._ignoreChange:      # ie. if an "intermediate text change event"
##            dbg(indent=0)
            return bValid

        ##! WS: For some inexplicable reason, every wx.TextCtrl.SetValue
        ## call is generating two (2) EVT_TEXT events.  On certain platforms,
        ## (eg. linux/GTK) the 1st is an empty string value.
        ## This is the only mechanism I can find to mask this problem:
        if newvalue == self._curValue or len(newvalue) == 0:
##            dbg('ignoring bogus text change event', indent=0)
            pass
        else:
##            dbg('curvalue: "%s", newvalue: "%s", len(newvalue): %d' % (self._curValue, newvalue, len(newvalue)))
            if self._Change():
                if self._signOk and self._isNeg and newvalue.find('-') == -1 and newvalue.find('(') == -1:
##                    dbg('clearing self._isNeg')
                    self._isNeg = False
                    text, self._signpos, self._right_signpos = self._getSignedValue()
                self._CheckValid()  # Recolor control as appropriate
##            dbg('calling event.Skip()')
            event.Skip()
            bValid = True
        self._prevValue = self._curValue    # save for undo
        self._curValue = newvalue           # Save last seen value for next iteration
##        dbg(indent=0)
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
##            dbg('MaskedEditMixin::OnKeyDown: calling _OnChar')
            self._OnChar(event)
            return
        # else allow regular EVT_CHAR key processing
        event.Skip()


    def _OnChar(self, event):
        """
        This is the engine of MaskedEdit controls.  It examines each keystroke,
        decides if it's allowed, where it should go or what action to take.
        """
##        dbg('MaskedEditMixin::_OnChar', indent=1)

        # Get keypress value, adjusted by control options (e.g. convert to upper etc)
        key = event.GetKeyCode()
        orig_pos = self._GetInsertionPoint()
        orig_value = self._GetValue()
##        dbg('keycode = ', key)
##        dbg('current pos = ', orig_pos)
##        dbg('current selection = ', self._GetSelection())

        if not self._Keypress(key):
##            dbg(indent=0)
            return

        # If no format string for this control, or the control is marked as "read-only",
        # skip the rest of the special processing, and just "do the standard thing:"
        if not self._mask or not self._IsEditable():
            event.Skip()
##            dbg(indent=0)
            return

        # Process navigation and control keys first, with
        # position/selection unadulterated:
        if key in self._nav + self._control:
            if self._keyhandlers.has_key(key):
                keep_processing = self._keyhandlers[key](event)
                if self._GetValue() != orig_value:
                    self.modified = True
                if not keep_processing:
##                    dbg(indent=0)
                    return
                self._applyFormatting()
##                dbg(indent=0)
                return

        # Else... adjust the position as necessary for next input key,
        # and determine resulting selection:
        pos = self._adjustPos( orig_pos, key )    ## get insertion position, adjusted as needed
        sel_start, sel_to = self._GetSelection()                ## check for a range of selected text
##        dbg("pos, sel_start, sel_to:", pos, sel_start, sel_to)

        keep_processing = True
        # Capture user past end of format field
        if pos > len(self.maskdict):
##            dbg("field length exceeded:",pos)
            keep_processing = False

        key = self._adjustKey(pos, key)     # apply formatting constraints to key:

        if self._keyhandlers.has_key(key):
            # there's an override for default behavior; use override function instead
##            dbg('using supplied key handler:', self._keyhandlers[key])
            keep_processing = self._keyhandlers[key](event)
            if self._GetValue() != orig_value:
                self.modified = True
            if not keep_processing:
##                dbg(indent=0)
                return
            # else skip default processing, but do final formatting
        if key in wx_control_keycodes:
##            dbg('key in wx_control_keycodes')
            event.Skip()                # non-printable; let base control handle it
            keep_processing = False
        else:
            field = self._FindField(pos)

            if 'unicode' in wx.PlatformInfo:
                if key < 256:
                    char = chr(key) # (must work if we got this far)
                    char = char.decode(self._defaultEncoding)
                else:
                    char = unichr(event.GetUnicodeKey())
                    dbg('unicode char:', char)
                excludes = u''
                if type(field._excludeChars) != types.UnicodeType:
                    excludes += field._excludeChars.decode(self._defaultEncoding)
                if type(self._ctrl_constraints) != types.UnicodeType:
                    excludes += self._ctrl_constraints._excludeChars.decode(self._defaultEncoding)
            else:
                char = chr(key) # (must work if we got this far)
                excludes = field._excludeChars + self._ctrl_constraints._excludeChars

##                dbg("key ='%s'" % chr(key))
                if chr(key) == ' ':
##                dbg('okSpaces?', field._okSpaces)
                    pass


            if char in excludes:
                keep_processing = False

            if keep_processing and self._isCharAllowed( char, pos, checkRegex = True ):
##                dbg("key allowed by mask")
                # insert key into candidate new value, but don't change control yet:
                oldstr = self._GetValue()
                newstr, newpos, new_select_to, match_field, match_index = self._insertKey(
                                char, pos, sel_start, sel_to, self._GetValue(), allowAutoSelect = True)
##                dbg("str with '%s' inserted:" % char, '"%s"' % newstr)
                if self._ctrl_constraints._validRequired and not self.IsValid(newstr):
##                    dbg('not valid; checking to see if adjusted string is:')
                    keep_processing = False
                    if self._isFloat and newstr != self._template:
                        newstr = self._adjustFloat(newstr)
##                        dbg('adjusted str:', newstr)
                        if self.IsValid(newstr):
##                            dbg("it is!")
                            keep_processing = True
                            wx.CallAfter(self._SetInsertionPoint, self._decimalpos)
                    if not keep_processing:
##                        dbg("key disallowed by validation")
                        if not wx.Validator_IsSilent() and orig_pos == pos:
                            wx.Bell()

                if keep_processing:
                    unadjusted = newstr

                    # special case: adjust date value as necessary:
                    if self._isDate and newstr != self._template:
                        newstr = self._adjustDate(newstr)
##                    dbg('adjusted newstr:', newstr)

                    if newstr != orig_value:
                        self.modified = True

                    wx.CallAfter(self._SetValue, newstr)

                    # Adjust insertion point on date if just entered 2 digit year, and there are now 4 digits:
                    if not self.IsDefault() and self._isDate and self._4digityear:
                        year2dig = self._dateExtent - 2
                        if pos == year2dig and unadjusted[year2dig] != newstr[year2dig]:
                            newpos = pos+2

##                    dbg('queuing insertion point: (%d)' % newpos)
                    wx.CallAfter(self._SetInsertionPoint, newpos)

                    if match_field is not None:
##                        dbg('matched field')
                        self._OnAutoSelect(match_field, match_index)

                    if new_select_to != newpos:
##                        dbg('queuing selection: (%d, %d)' % (newpos, new_select_to))
                        wx.CallAfter(self._SetSelection, newpos, new_select_to)
                    else:
                        newfield = self._FindField(newpos)
                        if newfield != field and newfield._selectOnFieldEntry:
##                            dbg('queuing insertion point: (%d)' % newfield._extent[0])
                            wx.CallAfter(self._SetInsertionPoint, newfield._extent[0])
##                            dbg('queuing selection: (%d, %d)' % (newfield._extent[0], newfield._extent[1]))
                            wx.CallAfter(self._SetSelection, newfield._extent[0], newfield._extent[1])
                        else:
                            wx.CallAfter(self._SetSelection, newpos, new_select_to)
                    keep_processing = False

            elif keep_processing:
##                dbg('char not allowed')
                keep_processing = False
                if (not wx.Validator_IsSilent()) and orig_pos == pos:
                    wx.Bell()

        self._applyFormatting()

        # Move to next insertion point
        if keep_processing and key not in self._nav:
            pos = self._GetInsertionPoint()
            next_entry = self._findNextEntry( pos )
            if pos != next_entry:
##                dbg("moving from %(pos)d to next valid entry: %(next_entry)d" % locals())
                wx.CallAfter(self._SetInsertionPoint, next_entry )

            if self._isTemplateChar(pos):
                self._AdjustField(pos)
##        dbg(indent=0)


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
##        dbg('MaskedEditMixin::_FindFieldExtent(pos=%s, getslice=%s)' % (str(pos), str(getslice)) ,indent=1)

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
##            dbg('edit_start:', edit_start, 'edit_end:', edit_end, 'slice: "%s"' % slice)
##            dbg(indent=0)
            return edit_start, edit_end, slice
        else:
##            dbg('edit_start:', edit_start, 'edit_end:', edit_end)
##            dbg(indent=0)
            return edit_start, edit_end


    def _FindField(self, pos=None):
        """
        Returns the field instance in which pos resides.
        Template chars are bound to the preceding field.
        For masks beginning with template chars, these chars are ignored
        when calculating the current field.

        """
####        dbg('MaskedEditMixin::_FindField(pos=%s)' % str(pos) ,indent=1)
        if pos is None: pos = self._GetInsertionPoint()
        elif pos < 0 or pos > self._masklength:
            raise IndexError('position %s out of range of control' % str(pos))

        if len(self._fields) == 0:
##            dbg(indent=0)
            return None

        # else...
####        dbg(indent=0)
        return self._fields[self._lookupField[pos]]


    def ClearValue(self):
        """ Blanks the current control value by replacing it with the default value."""
##        dbg("MaskedEditMixin::ClearValue - value reset to default value (template)")
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
##        dbg('MaskedEditMixin::_OnUpNumeric', indent=1)
        event.m_shiftDown = 1
##        dbg('event.ShiftDown()?', event.ShiftDown())
        self._OnChangeField(event)
##        dbg(indent=0)


    def _OnArrow(self, event):
        """
        Used in response to left/right navigation keys; makes these actions skip
        over mask template chars.
        """
##        dbg("MaskedEditMixin::_OnArrow", indent=1)
        pos = self._GetInsertionPoint()
        keycode = event.GetKeyCode()
        sel_start, sel_to = self._GetSelection()
        entry_end = self._goEnd(getPosOnly=True)
        if keycode in (wx.WXK_RIGHT, wx.WXK_DOWN):
            if( ( not self._isTemplateChar(pos) and pos+1 > entry_end)
                or ( self._isTemplateChar(pos) and pos >= entry_end) ):
##                dbg("can't advance", indent=0)
                return False
            elif self._isTemplateChar(pos):
                self._AdjustField(pos)
        elif keycode in (wx.WXK_LEFT,wx.WXK_UP) and sel_start == sel_to and pos > 0 and self._isTemplateChar(pos-1):
##            dbg('adjusting field')
            self._AdjustField(pos)

        # treat as shifted up/down arrows as tab/reverse tab:
        if event.ShiftDown() and keycode in (wx.WXK_UP, wx.WXK_DOWN):
            # remove "shifting" and treat as (forward) tab:
            event.m_shiftDown = False
            keep_processing = self._OnChangeField(event)

        elif self._FindField(pos)._selectOnFieldEntry:
            if( keycode in (wx.WXK_UP, wx.WXK_LEFT)
                and sel_start != 0
                and self._isTemplateChar(sel_start-1)
                and sel_start != self._masklength
                and not self._signOk and not self._useParens):

                # call _OnChangeField to handle "ctrl-shifted event"
                # (which moves to previous field and selects it.)
                event.m_shiftDown = True
                event.m_ControlDown = True
                keep_processing = self._OnChangeField(event)
            elif( keycode in (wx.WXK_DOWN, wx.WXK_RIGHT)
                  and sel_to != self._masklength
                  and self._isTemplateChar(sel_to)):

                # when changing field to the right, ensure don't accidentally go left instead
                event.m_shiftDown = False
                keep_processing = self._OnChangeField(event)
            else:
                # treat arrows as normal, allowing selection
                # as appropriate:
##                dbg('using base ctrl event processing')
                event.Skip()
        else:
            if( (sel_to == self._fields[0]._extent[0] and keycode == wx.WXK_LEFT)
                or (sel_to == self._masklength and keycode == wx.WXK_RIGHT) ):
                if not wx.Validator_IsSilent():
                    wx.Bell()
            else:
                # treat arrows as normal, allowing selection
                # as appropriate:
##                dbg('using base event processing')
                event.Skip()

        keep_processing = False
##        dbg(indent=0)
        return keep_processing


    def _OnCtrl_S(self, event):
        """ Default Ctrl-S handler; prints value information if demo enabled. """
##        dbg("MaskedEditMixin::_OnCtrl_S")
        if self._demo:
            print 'MaskedEditMixin.GetValue()       = "%s"\nMaskedEditMixin.GetPlainValue() = "%s"' % (self.GetValue(), self.GetPlainValue())
            print "Valid? => " + str(self.IsValid())
            print "Current field, start, end, value =", str( self._FindFieldExtent(getslice=True))
        return False


    def _OnCtrl_X(self, event=None):
        """ Handles ctrl-x keypress in control and Cut operation on context menu.
            Should return False to skip other processing. """
##        dbg("MaskedEditMixin::_OnCtrl_X", indent=1)
        self.Cut()
##        dbg(indent=0)
        return False

    def _OnCtrl_C(self, event=None):
        """ Handles ctrl-C keypress in control and Copy operation on context menu.
            Uses base control handling. Should return False to skip other processing."""
        self.Copy()
        return False

    def _OnCtrl_V(self, event=None):
        """ Handles ctrl-V keypress in control and Paste operation on context menu.
            Should return False to skip other processing. """
##        dbg("MaskedEditMixin::_OnCtrl_V", indent=1)
        self.Paste()
##        dbg(indent=0)
        return False

    def _OnInsert(self, event=None):
        """ Handles shift-insert and control-insert operations (paste and copy, respectively)"""
##        dbg("MaskedEditMixin::_OnInsert", indent=1)
        if event and isinstance(event, wx.KeyEvent):
            if event.ShiftDown():
                self.Paste()
            elif event.ControlDown():
                self.Copy()
            # (else do nothing)
        # (else do nothing)
##        dbg(indent=0)
        return False

    def _OnDelete(self, event=None):
        """ Handles shift-delete and delete operations (cut and erase, respectively)"""
##        dbg("MaskedEditMixin::_OnDelete", indent=1)
        if event and isinstance(event, wx.KeyEvent):
            if event.ShiftDown():
                self.Cut()
            else:
                self._OnErase(event)
        else:
            self._OnErase(event)
##        dbg(indent=0)
        return False

    def _OnCtrl_Z(self, event=None):
        """ Handles ctrl-Z keypress in control and Undo operation on context menu.
            Should return False to skip other processing. """
##        dbg("MaskedEditMixin::_OnCtrl_Z", indent=1)
        self.Undo()
##        dbg(indent=0)
        return False

    def _OnCtrl_A(self,event=None):
        """ Handles ctrl-a keypress in control. Should return False to skip other processing. """
        end = self._goEnd(getPosOnly=True)
        if not event or (isinstance(event, wx.KeyEvent) and event.ShiftDown()):
            wx.CallAfter(self._SetInsertionPoint, 0)
            wx.CallAfter(self._SetSelection, 0, self._masklength)
        else:
            wx.CallAfter(self._SetInsertionPoint, 0)
            wx.CallAfter(self._SetSelection, 0, end)
        return False


    def _OnErase(self, event=None, just_return_value=False):
        """ Handles backspace and delete keypress in control. Should return False to skip other processing."""
##        dbg("MaskedEditMixin::_OnErase", indent=1)
        sel_start, sel_to = self._GetSelection()                   ## check for a range of selected text

        if event is None:   # called as action routine from Cut() operation.
            key = wx.WXK_DELETE
        else:
            key = event.GetKeyCode()

        field = self._FindField(sel_to)
        start, end = field._extent
        value = self._GetValue()
        oldstart = sel_start

        # If trying to erase beyond "legal" bounds, disallow operation:
        if( (sel_to == 0 and key == wx.WXK_BACK)
            or (self._signOk and sel_to == 1 and value[0] == ' ' and key == wx.WXK_BACK)
            or (sel_to == self._masklength and sel_start == sel_to and key == wx.WXK_DELETE and not field._insertRight)
            or (self._signOk and self._useParens
                and sel_start == sel_to
                and sel_to == self._masklength - 1
                and value[sel_to] == ' ' and key == wx.WXK_DELETE and not field._insertRight) ):
            if not wx.Validator_IsSilent():
                wx.Bell()
##            dbg(indent=0)
            return False


        if( field._insertRight                                  # an insert-right field
            and value[start:end] != self._template[start:end]   # and field not empty
            and sel_start >= start                              # and selection starts in field
            and ((sel_to == sel_start                           # and no selection
                  and sel_to == end                             # and cursor at right edge
                  and key in (wx.WXK_BACK, wx.WXK_DELETE))            # and either delete or backspace key
                 or                                             # or
                 (key == wx.WXK_BACK                               # backspacing
                    and (sel_to == end                          # and selection ends at right edge
                         or sel_to < end and field._allowInsert)) ) ):  # or allow right insert at any point in field

##            dbg('delete left')
            # if backspace but left of cursor is empty, adjust cursor right before deleting
            while( key == wx.WXK_BACK
                   and sel_start == sel_to
                   and sel_start < end
                   and value[start:sel_start] == self._template[start:sel_start]):
                sel_start += 1
                sel_to = sel_start

##            dbg('sel_start, start:', sel_start, start)

            if sel_start == sel_to:
                keep = sel_start -1
            else:
                keep = sel_start
            newfield = value[start:keep] + value[sel_to:end]

            # handle sign char moving from outside field into the field:
            move_sign_into_field = False
            if not field._padZero and self._signOk and self._isNeg and value[0] in ('-', '('):
                signchar = value[0]
                newfield = signchar + newfield
                move_sign_into_field = True
##            dbg('cut newfield: "%s"' % newfield)

            # handle what should fill in from the left:
            left = ""
            for i in range(start, end - len(newfield)):
                if field._padZero:
                    left += '0'
                elif( self._signOk and self._isNeg and i == 1
                      and ((self._useParens and newfield.find('(') == -1)
                           or (not self._useParens and newfield.find('-') == -1)) ):
                    left += ' '
                else:
                    left += self._template[i]   # this can produce strange results in combination with default values...
            newfield = left + newfield
##            dbg('filled newfield: "%s"' % newfield)

            newstr = value[:start] + newfield + value[end:]

            # (handle sign located in "mask position" in front of field prior to delete)
            if move_sign_into_field:
                newstr = ' ' + newstr[1:]
            pos = sel_to
        else:
            # handle erasure of (left) sign, moving selection accordingly...
            if self._signOk and sel_start == 0:
                newstr = value = ' ' + value[1:]
                sel_start += 1

            if field._allowInsert and sel_start >= start:
                # selection (if any) falls within current insert-capable field:
                select_len = sel_to - sel_start
                # determine where cursor should end up:
                if key == wx.WXK_BACK:
                    if select_len == 0:
                        newpos = sel_start -1
                    else:
                        newpos = sel_start
                    erase_to = sel_to
                else:
                    newpos = sel_start
                    if sel_to == sel_start:
                        erase_to = sel_to + 1
                    else:
                        erase_to = sel_to

                if self._isTemplateChar(newpos) and select_len == 0:
                    if self._signOk:
                        if value[newpos] in ('(', '-'):
                            newpos += 1     # don't move cusor
                            newstr = ' ' + value[newpos:]
                        elif value[newpos] == ')':
                            # erase right sign, but don't move cursor; (matching left sign handled later)
                            newstr = value[:newpos] + ' '
                        else:
                            # no deletion; just move cursor
                            newstr = value
                    else:
                        # no deletion; just move cursor
                        newstr = value
                else:
                    if erase_to > end: erase_to = end
                    erase_len = erase_to - newpos

                    left = value[start:newpos]
##                    dbg("retained ='%s'" % value[erase_to:end], 'sel_to:', sel_to, "fill: '%s'" % self._template[end - erase_len:end])
                    right = value[erase_to:end] + self._template[end-erase_len:end]
                    pos_adjust = 0
                    if field._alignRight:
                        rstripped = right.rstrip()
                        if rstripped != right:
                            pos_adjust = len(right) - len(rstripped)
                        right = rstripped

                    if not field._insertRight and value[-1] == ')' and end == self._masklength - 1:
                        # need to shift ) into the field:
                        right = right[:-1] + ')'
                        value = value[:-1] + ' '

                    newfield = left+right
                    if pos_adjust:
                        newfield = newfield.rjust(end-start)
                        newpos += pos_adjust
##                    dbg("left='%s', right ='%s', newfield='%s'" %(left, right, newfield))
                    newstr = value[:start] + newfield + value[end:]

                pos = newpos

            else:
                if sel_start == sel_to:
##                    dbg("current sel_start, sel_to:", sel_start, sel_to)
                    if key == wx.WXK_BACK:
                        sel_start, sel_to = sel_to-1, sel_to-1
##                        dbg("new sel_start, sel_to:", sel_start, sel_to)

                    if field._padZero and not value[start:sel_to].replace('0', '').replace(' ','').replace(field._fillChar, ''):
                        # preceding chars (if any) are zeros, blanks or fillchar; new char should be 0:
                        newchar = '0'
                    else:
                        newchar = self._template[sel_to] ## get an original template character to "clear" the current char
##                    dbg('value = "%s"' % value, 'value[%d] = "%s"' %(sel_start, value[sel_start]))

                    if self._isTemplateChar(sel_to):
                        if sel_to == 0 and self._signOk and value[sel_to] == '-':   # erasing "template" sign char
                            newstr = ' ' + value[1:]
                            sel_to += 1
                        elif self._signOk and self._useParens and (value[sel_to] == ')' or value[sel_to] == '('):
                            # allow "change sign" by removing both parens:
                            newstr = value[:self._signpos] + ' ' + value[self._signpos+1:-1] + ' '
                        else:
                            newstr = value
                        newpos = sel_to
                    else:
                        if field._insertRight and sel_start == sel_to:
                            # force non-insert-right behavior, by selecting char to be replaced:
                            sel_to += 1
                        newstr, ignore = self._insertKey(newchar, sel_start, sel_start, sel_to, value)

                else:
                    # selection made
                    newstr = self._eraseSelection(value, sel_start, sel_to)

                pos = sel_start  # put cursor back at beginning of selection

        if self._signOk and self._useParens:
            # account for resultant unbalanced parentheses:
            left_signpos = newstr.find('(')
            right_signpos = newstr.find(')')

            if left_signpos == -1 and right_signpos != -1:
                # erased left-sign marker; get rid of right sign marker:
                newstr = newstr[:right_signpos] + ' ' + newstr[right_signpos+1:]

            elif left_signpos != -1 and right_signpos == -1:
                # erased right-sign marker; get rid of left-sign marker:
                newstr = newstr[:left_signpos] + ' ' + newstr[left_signpos+1:]

##        dbg("oldstr:'%s'" % value, 'oldpos:', oldstart)
##        dbg("newstr:'%s'" % newstr, 'pos:', pos)

        # if erasure results in an invalid field, disallow it:
##        dbg('field._validRequired?', field._validRequired)
##        dbg('field.IsValid("%s")?' % newstr[start:end], field.IsValid(newstr[start:end]))
        if field._validRequired and not field.IsValid(newstr[start:end]):
            if not wx.Validator_IsSilent():
                wx.Bell()
##            dbg(indent=0)
            return False

        # if erasure results in an invalid value, disallow it:
        if self._ctrl_constraints._validRequired and not self.IsValid(newstr):
            if not wx.Validator_IsSilent():
                wx.Bell()
##            dbg(indent=0)
            return False

        if just_return_value:
##            dbg(indent=0)
            return newstr

        # else...
##        dbg('setting value (later) to', newstr)
        wx.CallAfter(self._SetValue, newstr)
##        dbg('setting insertion point (later) to', pos)
        wx.CallAfter(self._SetInsertionPoint, pos)
##        dbg(indent=0)
        if newstr != value:
            self.modified = True
        return False


    def _OnEnd(self,event):
        """ Handles End keypress in control. Should return False to skip other processing. """
##        dbg("MaskedEditMixin::_OnEnd", indent=1)
        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
        if not event.ControlDown():
            end = self._masklength  # go to end of control
            if self._signOk and self._useParens:
                end = end - 1       # account for reserved char at end
        else:
            end_of_input = self._goEnd(getPosOnly=True)
            sel_start, sel_to = self._GetSelection()
            if sel_to < pos: sel_to = pos
            field = self._FindField(sel_to)
            field_end = self._FindField(end_of_input)

            # pick different end point if either:
            # - cursor not in same field
            # - or at or past last input already
            # - or current selection = end of current field:
####            dbg('field != field_end?', field != field_end)
####            dbg('sel_to >= end_of_input?', sel_to >= end_of_input)
            if field != field_end or sel_to >= end_of_input:
                edit_start, edit_end = field._extent
####                dbg('edit_end:', edit_end)
####                dbg('sel_to:', sel_to)
####                dbg('sel_to == edit_end?', sel_to == edit_end)
####                dbg('field._index < self._field_indices[-1]?', field._index < self._field_indices[-1])

                if sel_to == edit_end and field._index < self._field_indices[-1]:
                    edit_start, edit_end = self._FindFieldExtent(self._findNextEntry(edit_end))  # go to end of next field:
                    end = edit_end
##                    dbg('end moved to', end)

                elif sel_to == edit_end and field._index == self._field_indices[-1]:
                    # already at edit end of last field; select to end of control:
                    end = self._masklength
##                    dbg('end moved to', end)
                else:
                    end = edit_end  # select to end of current field
##                    dbg('end moved to ', end)
            else:
                # select to current end of input
                end = end_of_input


####        dbg('pos:', pos, 'end:', end)

        if event.ShiftDown():
            if not event.ControlDown():
##                dbg("shift-end; select to end of control")
                pass
            else:
##                dbg("shift-ctrl-end; select to end of non-whitespace")
                pass
            wx.CallAfter(self._SetInsertionPoint, pos)
            wx.CallAfter(self._SetSelection, pos, end)
        else:
            if not event.ControlDown():
##                dbg('go to end of control:')
                pass
            wx.CallAfter(self._SetInsertionPoint, end)
            wx.CallAfter(self._SetSelection, end, end)

##        dbg(indent=0)
        return False


    def _OnReturn(self, event):
         """
         Swallows the return, issues a Navigate event instead, since
         masked controls are "single line" by defn.
         """
##         dbg('MaskedEditMixin::OnReturn')
         self.Navigate(True)
         return False


    def _OnHome(self,event):
        """ Handles Home keypress in control. Should return False to skip other processing."""
##        dbg("MaskedEditMixin::_OnHome", indent=1)
        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
        sel_start, sel_to = self._GetSelection()

        # There are 5 cases here:

        # 1) shift: select from start of control to end of current
        #    selection.
        if event.ShiftDown() and not event.ControlDown():
##            dbg("shift-home; select to start of control")
            start = 0
            end = sel_start

        # 2) no shift, no control: move cursor to beginning of control.
        elif not event.ControlDown():
##            dbg("home; move to start of control")
            start = 0
            end = 0

        # 3) No shift, control: move cursor back to beginning of field; if
        #    there already, go to beginning of previous field.
        # 4) shift, control, start of selection not at beginning of control:
        #    move sel_start back to start of field; if already there, go to
        #    start of previous field.
        elif( event.ControlDown()
              and (not event.ShiftDown()
                   or (event.ShiftDown() and sel_start > 0) ) ):
            if len(self._field_indices) > 1:
                field = self._FindField(sel_start)
                start, ignore = field._extent
                if sel_start == start and field._index != self._field_indices[0]:  # go to start of previous field:
                    start, ignore = self._FindFieldExtent(sel_start-1)
                elif sel_start == start:
                    start = 0   # go to literal beginning if edit start
                                # not at that point
                end_of_field = True

            else:
                start = 0

            if not event.ShiftDown():
##                dbg("ctrl-home; move to beginning of field")
                end = start
            else:
##                dbg("shift-ctrl-home; select to beginning of field")
                end = sel_to

        else:
        # 5) shift, control, start of selection at beginning of control:
        #    unselect by moving sel_to backward to beginning of current field;
        #    if already there, move to start of previous field.
            start = sel_start
            if len(self._field_indices) > 1:
                # find end of previous field:
                field = self._FindField(sel_to)
                if sel_to > start and field._index != self._field_indices[0]:
                    ignore, end = self._FindFieldExtent(field._extent[0]-1)
                else:
                    end = start
                end_of_field = True
            else:
                end = start
                end_of_field = False
##            dbg("shift-ctrl-home; unselect to beginning of field")

##        dbg('queuing new sel_start, sel_to:', (start, end))
        wx.CallAfter(self._SetInsertionPoint, start)
        wx.CallAfter(self._SetSelection, start, end)
##        dbg(indent=0)
        return False


    def _OnChangeField(self, event):
        """
        Primarily handles TAB events, but can be used for any key that
        designer wants to change fields within a masked edit control.
        """
##        dbg('MaskedEditMixin::_OnChangeField', indent = 1)
        # determine end of current field:
        pos = self._GetInsertionPoint()
##        dbg('current pos:', pos)
        sel_start, sel_to = self._GetSelection()

        if self._masklength < 0:   # no fields; process tab normally
            self._AdjustField(pos)
            if event.GetKeyCode() == wx.WXK_TAB:
##                dbg('tab to next ctrl')
                # As of 2.5.2, you don't call event.Skip() to do
                # this, but instead force explicit navigation, if
                # wx.TE_PROCESS_TAB is used (like in the masked edits)
                self.Navigate(True)
            #else: do nothing
##            dbg(indent=0)
            return False


        if event.ShiftDown():

            # "Go backward"

            # NOTE: doesn't yet work with SHIFT-tab under wx; the control
            # never sees this event! (But I've coded for it should it ever work,
            # and it *does* work for '.' in IpAddrCtrl.)
            field = self._FindField(pos)
            index = field._index
            field_start = field._extent[0]
            if pos < field_start:
##                dbg('cursor before 1st field; cannot change to a previous field')
                if not wx.Validator_IsSilent():
                    wx.Bell()
                return False

            if event.ControlDown():
##                dbg('queuing select to beginning of field:', field_start, pos)
                wx.CallAfter(self._SetInsertionPoint, field_start)
                wx.CallAfter(self._SetSelection, field_start, pos)
##                dbg(indent=0)
                return False

            elif index == 0:
                  # We're already in the 1st field; process shift-tab normally:
                self._AdjustField(pos)
                if event.GetKeyCode() == wx.WXK_TAB:
##                    dbg('tab to previous ctrl')
                    # As of 2.5.2, you don't call event.Skip() to do
                    # this, but instead force explicit navigation, if
                    # wx.TE_PROCESS_TAB is used (like in the masked edits)
                    self.Navigate(False)
                else:
##                    dbg('position at beginning')
                    wx.CallAfter(self._SetInsertionPoint, field_start)
##                dbg(indent=0)
                return False
            else:
                # find beginning of previous field:
                begin_prev = self._FindField(field_start-1)._extent[0]
                self._AdjustField(pos)
##                dbg('repositioning to', begin_prev)
                wx.CallAfter(self._SetInsertionPoint, begin_prev)
                if self._FindField(begin_prev)._selectOnFieldEntry:
                    edit_start, edit_end = self._FindFieldExtent(begin_prev)
##                    dbg('queuing selection to (%d, %d)' % (edit_start, edit_end))
                    wx.CallAfter(self._SetInsertionPoint, edit_start)
                    wx.CallAfter(self._SetSelection, edit_start, edit_end)
##                dbg(indent=0)
                return False

        else:
            # "Go forward"
            field = self._FindField(sel_to)
            field_start, field_end = field._extent
            if event.ControlDown():
##                dbg('queuing select to end of field:', pos, field_end)
                wx.CallAfter(self._SetInsertionPoint, pos)
                wx.CallAfter(self._SetSelection, pos, field_end)
##                dbg(indent=0)
                return False
            else:
                if pos < field_start:
##                    dbg('cursor before 1st field; go to start of field')
                    wx.CallAfter(self._SetInsertionPoint, field_start)
                    if field._selectOnFieldEntry:
                        wx.CallAfter(self._SetSelection, field_start, field_end)
                    else:
                        wx.CallAfter(self._SetSelection, field_start, field_start)
                    return False
                # else...
##                dbg('end of current field:', field_end)
##                dbg('go to next field')
                if field_end == self._fields[self._field_indices[-1]]._extent[1]:
                    self._AdjustField(pos)
                    if event.GetKeyCode() == wx.WXK_TAB:
##                        dbg('tab to next ctrl')
                        # As of 2.5.2, you don't call event.Skip() to do
                        # this, but instead force explicit navigation, if
                        # wx.TE_PROCESS_TAB is used (like in the masked edits)
                        self.Navigate(True)
                    else:
##                        dbg('position at end')
                        wx.CallAfter(self._SetInsertionPoint, field_end)
##                    dbg(indent=0)
                    return False
                else:
                    # we have to find the start of the next field
                    next_pos = self._findNextEntry(field_end)
                    if next_pos == field_end:
##                        dbg('already in last field')
                        self._AdjustField(pos)
                        if event.GetKeyCode() == wx.WXK_TAB:
##                            dbg('tab to next ctrl')
                            # As of 2.5.2, you don't call event.Skip() to do
                            # this, but instead force explicit navigation, if
                            # wx.TE_PROCESS_TAB is used (like in the masked edits)
                            self.Navigate(True)
                        #else: do nothing
##                        dbg(indent=0)
                        return False
                    else:
                        self._AdjustField( pos )

                        # move cursor to appropriate point in the next field and select as necessary:
                        field = self._FindField(next_pos)
                        edit_start, edit_end = field._extent
                        if field._selectOnFieldEntry:
##                            dbg('move to ', next_pos)
                            wx.CallAfter(self._SetInsertionPoint, next_pos)
                            edit_start, edit_end = self._FindFieldExtent(next_pos)
##                            dbg('queuing select', edit_start, edit_end)
                            wx.CallAfter(self._SetSelection, edit_start, edit_end)
                        else:
                            if field._insertRight:
                                next_pos = field._extent[1]
##                            dbg('move to ', next_pos)
                            wx.CallAfter(self._SetInsertionPoint, next_pos)
##                        dbg(indent=0)
                        return False


    def _OnDecimalPoint(self, event):
##        dbg('MaskedEditMixin::_OnDecimalPoint', indent=1)

        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())

        if self._isFloat:       ## handle float value, move to decimal place
##            dbg('key == Decimal tab; decimal pos:', self._decimalpos)
            value = self._GetValue()
            if pos < self._decimalpos:
                clipped_text = value[0:pos] + self._decimalChar + value[self._decimalpos+1:]
##                dbg('value: "%s"' % self._GetValue(), "clipped_text:'%s'" % clipped_text)
                newstr = self._adjustFloat(clipped_text)
            else:
                newstr = self._adjustFloat(value)
            wx.CallAfter(self._SetValue, newstr)
            fraction = self._fields[1]
            start, end = fraction._extent
            wx.CallAfter(self._SetInsertionPoint, start)
            if fraction._selectOnFieldEntry:
##                dbg('queuing selection after decimal point to:', (start, end))
                wx.CallAfter(self._SetSelection, start, end)
            else:
                wx.CallAfter(self._SetSelection, start, start)
            keep_processing = False

        if self._isInt:      ## handle integer value, truncate from current position
##            dbg('key == Integer decimal event')
            value = self._GetValue()
            clipped_text = value[0:pos]
##            dbg('value: "%s"' % self._GetValue(), "clipped_text:'%s'" % clipped_text)
            newstr = self._adjustInt(clipped_text)
##            dbg('newstr: "%s"' % newstr)
            wx.CallAfter(self._SetValue, newstr)
            newpos = len(newstr.rstrip())
            if newstr.find(')') != -1:
                newpos -= 1     # (don't move past right paren)
            wx.CallAfter(self._SetInsertionPoint, newpos)
            wx.CallAfter(self._SetSelection, newpos, newpos)
            keep_processing = False
##        dbg(indent=0)


    def _OnChangeSign(self, event):
##        dbg('MaskedEditMixin::_OnChangeSign', indent=1)
        key = event.GetKeyCode()
        pos = self._adjustPos(self._GetInsertionPoint(), key)
        value = self._eraseSelection()
        integer = self._fields[0]
        start, end = integer._extent
        sel_start, sel_to = self._GetSelection()

####        dbg('adjusted pos:', pos)
        if chr(key) in ('-','+','(', ')') or (chr(key) == " " and pos == self._signpos):
            cursign = self._isNeg
##            dbg('cursign:', cursign)
            if chr(key) in ('-','(', ')'):
                if sel_start <= self._signpos:
                    self._isNeg = True
                else:
                    self._isNeg = (not self._isNeg)   ## flip value
            else:
                self._isNeg = False
##            dbg('isNeg?', self._isNeg)

            text, self._signpos, self._right_signpos = self._getSignedValue(candidate=value)
##            dbg('text:"%s"' % text, 'signpos:', self._signpos, 'right_signpos:', self._right_signpos)
            if text is None:
                text = value

            if self._isNeg and self._signpos is not None and self._signpos != -1:
                if self._useParens and self._right_signpos is not None:
                    text = text[:self._signpos] + '(' + text[self._signpos+1:self._right_signpos] + ')' + text[self._right_signpos+1:]
                else:
                    text = text[:self._signpos] + '-' + text[self._signpos+1:]
            else:
####                dbg('self._isNeg?', self._isNeg, 'self.IsValid(%s)' % text, self.IsValid(text))
                if self._useParens:
                    text = text[:self._signpos] + ' ' + text[self._signpos+1:self._right_signpos] + ' ' + text[self._right_signpos+1:]
                else:
                    text = text[:self._signpos] + ' ' + text[self._signpos+1:]
##                dbg('clearing self._isNeg')
                self._isNeg = False

            wx.CallAfter(self._SetValue, text)
            wx.CallAfter(self._applyFormatting)
##            dbg('pos:', pos, 'signpos:', self._signpos)
            if pos == self._signpos or integer.IsEmpty(text[start:end]):
                wx.CallAfter(self._SetInsertionPoint, self._signpos+1)
            else:
                wx.CallAfter(self._SetInsertionPoint, pos)

            keep_processing = False
        else:
            keep_processing = True
##        dbg(indent=0)
        return keep_processing


    def _OnGroupChar(self, event):
        """
        This handler is only registered if the mask is a numeric mask.
        It allows the insertion of ',' or '.' if appropriate.
        """
##        dbg('MaskedEditMixin::_OnGroupChar', indent=1)
        keep_processing = True
        pos = self._adjustPos(self._GetInsertionPoint(), event.GetKeyCode())
        sel_start, sel_to = self._GetSelection()
        groupchar = self._fields[0]._groupChar
        if not self._isCharAllowed(groupchar, pos, checkRegex=True):
            keep_processing = False
            if not wx.Validator_IsSilent():
                wx.Bell()

        if keep_processing:
            newstr, newpos = self._insertKey(groupchar, pos, sel_start, sel_to, self._GetValue() )
##            dbg("str with '%s' inserted:" % groupchar, '"%s"' % newstr)
            if self._ctrl_constraints._validRequired and not self.IsValid(newstr):
                keep_processing = False
                if not wx.Validator_IsSilent():
                        wx.Bell()

        if keep_processing:
            wx.CallAfter(self._SetValue, newstr)
            wx.CallAfter(self._SetInsertionPoint, newpos)
        keep_processing = False
##        dbg(indent=0)
        return keep_processing


    def _findNextEntry(self,pos, adjustInsert=True):
        """ Find the insertion point for the next valid entry character position."""
##        dbg('MaskedEditMixin::_findNextEntry', indent=1)        
        if self._isTemplateChar(pos) or pos in self._explicit_field_boundaries:   # if changing fields, pay attn to flag
            adjustInsert = adjustInsert
        else:                           # else within a field; flag not relevant
            adjustInsert = False

        while self._isTemplateChar(pos) and pos < self._masklength:
            pos += 1

        # if changing fields, and we've been told to adjust insert point,
        # look at new field; if empty and right-insert field,
        # adjust to right edge:
        if adjustInsert and pos < self._masklength:
            field = self._FindField(pos)
            start, end = field._extent
            slice = self._GetValue()[start:end]
            if field._insertRight and field.IsEmpty(slice):
                pos = end
##        dbg('final pos:', pos, indent=0)
        return pos


    def _findNextTemplateChar(self, pos):
        """ Find the position of the next non-editable character in the mask."""
        while not self._isTemplateChar(pos) and pos < self._masklength:
            pos += 1
        return pos


    def _OnAutoCompleteField(self, event):
##        dbg('MaskedEditMixin::_OnAutoCompleteField', indent =1)
        pos = self._GetInsertionPoint()
        field = self._FindField(pos)
        edit_start, edit_end, slice = self._FindFieldExtent(pos, getslice=True)

        match_index = None
        keycode = event.GetKeyCode()

        if field._fillChar != ' ':
            text = slice.replace(field._fillChar, '')
        else:
            text = slice
        text = text.strip()
        keep_processing = True  # (assume True to start)
##        dbg('field._hasList?', field._hasList)
        if field._hasList:
##            dbg('choices:', field._choices)
##            dbg('compareChoices:', field._compareChoices)
            choices, choice_required = field._compareChoices, field._choiceRequired
            if keycode in (wx.WXK_PRIOR, wx.WXK_UP):
                direction = -1
            else:
                direction = 1
            match_index, partial_match = self._autoComplete(direction, choices, text, compareNoCase=field._compareNoCase, current_index = field._autoCompleteIndex)
            if( match_index is None
                and (keycode in self._autoCompleteKeycodes + [wx.WXK_PRIOR, wx.WXK_NEXT]
                     or (keycode in [wx.WXK_UP, wx.WXK_DOWN] and event.ShiftDown() ) ) ):
                # Select the 1st thing from the list:
                match_index = 0

            if( match_index is not None
                and ( keycode in self._autoCompleteKeycodes + [wx.WXK_PRIOR, wx.WXK_NEXT]
                      or (keycode in [wx.WXK_UP, wx.WXK_DOWN] and event.ShiftDown())
                      or (keycode == wx.WXK_DOWN and partial_match) ) ):

                # We're allowed to auto-complete:
##                dbg('match found')
                value = self._GetValue()
                newvalue = value[:edit_start] + field._choices[match_index] + value[edit_end:]
##                dbg('setting value to "%s"' % newvalue)
                self._SetValue(newvalue)
                self._SetInsertionPoint(min(edit_end, len(newvalue.rstrip())))
                self._OnAutoSelect(field, match_index)
                self._CheckValid()  # recolor as appopriate


        if keycode in (wx.WXK_UP, wx.WXK_DOWN, wx.WXK_LEFT, wx.WXK_RIGHT):
            # treat as left right arrow if unshifted, tab/shift tab if shifted.
            if event.ShiftDown():
                if keycode in (wx.WXK_DOWN, wx.WXK_RIGHT):
                    # remove "shifting" and treat as (forward) tab:
                    event.m_shiftDown = False
                keep_processing = self._OnChangeField(event)
            else:
                keep_processing = self._OnArrow(event)
        # else some other key; keep processing the key

##        dbg('keep processing?', keep_processing, indent=0)
        return keep_processing


    def _OnAutoSelect(self, field, match_index = None):
        """
        Function called if autoselect feature is enabled and entire control
        is selected:
        """
##        dbg('MaskedEditMixin::OnAutoSelect', field._index)
        if match_index is not None:
            field._autoCompleteIndex = match_index


    def _autoComplete(self, direction, choices, value, compareNoCase, current_index):
        """
        This function gets called in response to Auto-complete events.
        It attempts to find a match to the specified value against the
        list of choices; if exact match, the index of then next
        appropriate value in the list, based on the given direction.
        If not an exact match, it will return the index of the 1st value from
        the choice list for which the partial value can be extended to match.
        If no match found, it will return None.
        The function returns a 2-tuple, with the 2nd element being a boolean
        that indicates if partial match was necessary.
        """
##        dbg('autoComplete(direction=', direction, 'choices=',choices, 'value=',value,'compareNoCase?', compareNoCase, 'current_index:', current_index, indent=1)
        if value is None:
##            dbg('nothing to match against', indent=0)
            return (None, False)

        partial_match = False

        if compareNoCase:
            value = value.lower()

        last_index = len(choices) - 1
        if value in choices:
##            dbg('"%s" in', choices)
            if current_index is not None and choices[current_index] == value:
                index = current_index
            else:
                index = choices.index(value)

##            dbg('matched "%s" (%d)' % (choices[index], index))
            if direction == -1:
##                dbg('going to previous')
                if index == 0: index = len(choices) - 1
                else: index -= 1
            else:
                if index == len(choices) - 1: index = 0
                else: index += 1
##            dbg('change value to "%s" (%d)' % (choices[index], index))
            match = index
        else:
            partial_match = True
            value = value.strip()
##            dbg('no match; try to auto-complete:')
            match = None
##            dbg('searching for "%s"' % value)
            if current_index is None:
                indices = range(len(choices))
                if direction == -1:
                    indices.reverse()
            else:
                if direction == 1:
                    indices = range(current_index +1, len(choices)) + range(current_index+1)
##                    dbg('range(current_index+1 (%d), len(choices) (%d)) + range(%d):' % (current_index+1, len(choices), current_index+1), indices)
                else:
                    indices = range(current_index-1, -1, -1) + range(len(choices)-1, current_index-1, -1)
##                    dbg('range(current_index-1 (%d), -1) + range(len(choices)-1 (%d)), current_index-1 (%d):' % (current_index-1, len(choices)-1, current_index-1), indices)
####            dbg('indices:', indices)
            for index in indices:
                choice = choices[index]
                if choice.find(value, 0) == 0:
##                    dbg('match found:', choice)
                    match = index
                    break
                else:
##                    dbg('choice: "%s" - no match' % choice)
                    pass
            if match is not None:
##                dbg('matched', match)
                pass
            else:
##                dbg('no match found')
                pass
##        dbg(indent=0)
        return (match, partial_match)


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

        if self._isFloat and newvalue != self._template:
            newvalue = self._adjustFloat(newvalue)

        if self._ctrl_constraints._isInt and value != self._template:
            newvalue = self._adjustInt(value)

        if self._isDate and value != self._template:
            newvalue = self._adjustDate(value, fixcentury=True)
            if self._4digityear:
                year2dig = self._dateExtent - 2
                if pos == year2dig and value[year2dig] != newvalue[year2dig]:
                    pos = pos+2

        if newvalue != value:
##            dbg('old value: "%s"\nnew value: "%s"' % (value, newvalue))
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
##        dbg('_adjustPos', pos, key, indent=1)
        sel_start, sel_to = self._GetSelection()
        # If a numeric or decimal mask, and negatives allowed, reserve the
        # first space for sign, and last one if using parens.
        if( self._signOk
            and ((pos == self._signpos and key in (ord('-'), ord('+'), ord(' ')) )
                 or (self._useParens and pos == self._masklength -1))):
##            dbg('adjusted pos:', pos, indent=0)
            return pos

        if key not in self._nav:
            field = self._FindField(pos)

##            dbg('field._insertRight?', field._insertRight)
##            if self._signOk: dbg('self._signpos:', self._signpos)
            if field._insertRight:              # if allow right-insert
                start, end = field._extent
                slice = self._GetValue()[start:end].strip()
                field_len = end - start
                if pos == end:                      # if cursor at right edge of field
                    # if not filled or supposed to stay in field, keep current position
####                    dbg('pos==end')
####                    dbg('len (slice):', len(slice))
####                    dbg('field_len?', field_len)
####                    dbg('pos==end; len (slice) < field_len?', len(slice) < field_len)
####                    dbg('not field._moveOnFieldFull?', not field._moveOnFieldFull)
                    if len(slice) == field_len and field._moveOnFieldFull:
                        # move cursor to next field:
                        pos = self._findNextEntry(pos)
                        self._SetInsertionPoint(pos)
                        if pos < sel_to:
                            self._SetSelection(pos, sel_to)     # restore selection
                        else:
                            self._SetSelection(pos, pos)        # remove selection
                    else: # leave cursor alone
                        pass
                else:
                    # if at start of control, move to right edge
                    if (sel_to == sel_start
                        and (self._isTemplateChar(pos) or (pos == start and len(slice)+ 1 < field_len))
                        and pos != end):
                        pos = end                   # move to right edge
##                    elif sel_start <= start and sel_to == end:
##                        # select to right edge of field - 1 (to replace char)
##                        pos = end - 1
##                        self._SetInsertionPoint(pos)
##                        # restore selection
##                        self._SetSelection(sel_start, pos)

                    # if selected to beginning and signed, and not changing sign explicitly:
                    elif self._signOk and sel_start == 0 and key not in (ord('-'), ord('+'), ord(' ')):
                        # adjust to past reserved sign position:
                        pos = self._fields[0]._extent[0]
##                        dbg('adjusting field to ', pos)
                        self._SetInsertionPoint(pos)
                        # but keep original selection, to allow replacement of any sign: 
                        self._SetSelection(0, sel_to)
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
                    else:
                        self._SetSelection(pos, pos)
##        dbg('adjusted pos:', pos, indent=0)
        return pos


    def _adjustFloat(self, candidate=None):
        """
        'Fixes' an floating point control. Collapses spaces, right-justifies, etc.
        """
##        dbg('MaskedEditMixin::_adjustFloat, candidate = "%s"' % candidate, indent=1)
        lenInt,lenFraction  = [len(s) for s in self._mask.split('.')]  ## Get integer, fraction lengths

        if candidate is None: value = self._GetValue()
        else: value = candidate
##        dbg('value = "%(value)s"' % locals(), 'len(value):', len(value))
        intStr, fracStr = value.split(self._decimalChar)

        intStr = self._fields[0]._AdjustField(intStr)
##        dbg('adjusted intStr: "%s"' % intStr)
        lenInt = len(intStr)
        fracStr = fracStr + ('0'*(lenFraction-len(fracStr)))  # add trailing spaces to decimal

##        dbg('intStr "%(intStr)s"' % locals())
##        dbg('lenInt:', lenInt)

        intStr = string.rjust( intStr[-lenInt:], lenInt)
##        dbg('right-justifed intStr = "%(intStr)s"' % locals())
        newvalue = intStr + self._decimalChar + fracStr

        if self._signOk:
            if len(newvalue) < self._masklength:
                newvalue = ' ' + newvalue
            signedvalue = self._getSignedValue(newvalue)[0]
            if signedvalue is not None: newvalue = signedvalue

        # Finally, align string with decimal position, left-padding with
        # fillChar:
        newdecpos = newvalue.find(self._decimalChar)
        if newdecpos < self._decimalpos:
            padlen = self._decimalpos - newdecpos
            newvalue = string.join([' ' * padlen] + [newvalue] ,'')

        if self._signOk and self._useParens:
            if newvalue.find('(') != -1:
                newvalue = newvalue[:-1] + ')'
            else:
                newvalue = newvalue[:-1] + ' '

##        dbg('newvalue = "%s"' % newvalue)
        if candidate is None:
            wx.CallAfter(self._SetValue, newvalue)
##        dbg(indent=0)
        return newvalue


    def _adjustInt(self, candidate=None):
        """ 'Fixes' an integer control. Collapses spaces, right or left-justifies."""
##        dbg("MaskedEditMixin::_adjustInt", candidate)
        lenInt = self._masklength
        if candidate is None: value = self._GetValue()
        else: value = candidate

        intStr = self._fields[0]._AdjustField(value)
        intStr = intStr.strip() # drop extra spaces
##        dbg('adjusted field: "%s"' % intStr)

        if self._isNeg and intStr.find('-') == -1 and intStr.find('(') == -1:
            if self._useParens:
                intStr = '(' + intStr + ')'
            else:
                intStr = '-' + intStr
        elif self._isNeg and intStr.find('-') != -1 and self._useParens:
            intStr = intStr.replace('-', '(')

        if( self._signOk and ((self._useParens and intStr.find('(') == -1)
                                or (not self._useParens and intStr.find('-') == -1))):
            intStr = ' ' + intStr
            if self._useParens:
                intStr += ' '   # space for right paren position

        elif self._signOk and self._useParens and intStr.find('(') != -1 and intStr.find(')') == -1:
            # ensure closing right paren:
            intStr += ')'

        if self._fields[0]._alignRight:     ## Only if right-alignment is enabled
            intStr = intStr.rjust( lenInt )
        else:
            intStr = intStr.ljust( lenInt )

        if candidate is None:
            wx.CallAfter(self._SetValue, intStr )
        return intStr


    def _adjustDate(self, candidate=None, fixcentury=False, force4digit_year=False):
        """
        'Fixes' a date control, expanding the year if it can.
        Applies various self-formatting options.
        """
##        dbg("MaskedEditMixin::_adjustDate", indent=1)
        if candidate is None: text    = self._GetValue()
        else: text = candidate
##        dbg('text=', text)
        if self._datestyle == "YMD":
            year_field = 0
        else:
            year_field = 2

##        dbg('getYear: "%s"' % _getYear(text, self._datestyle))
        year    = string.replace( _getYear( text, self._datestyle),self._fields[year_field]._fillChar,"")  # drop extra fillChars
        month   = _getMonth( text, self._datestyle)
        day     = _getDay( text, self._datestyle)
##        dbg('self._datestyle:', self._datestyle, 'year:', year, 'Month', month, 'day:', day)

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
##                dbg('bad year=', year)
                year = text[yearstart:self._dateExtent]

        if len(year) < 4 and yearVal:
            if len(year) == 2:
                # Fix year adjustment to be less "20th century" :-) and to adjust heuristic as the
                # years pass...
                now = wx.DateTime_Now()
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
                text = _makeDate(year, month, day, self._datestyle, text) + text[self._dateExtent:]
##        dbg('newdate: "%s"' % text, indent=0)
        return text


    def _goEnd(self, getPosOnly=False):
        """ Moves the insertion point to the end of user-entry """
##        dbg("MaskedEditMixin::_goEnd; getPosOnly:", getPosOnly, indent=1)
        text = self._GetValue()
####        dbg('text: "%s"' % text)
        i = 0
        if len(text.rstrip()):
            for i in range( min( self._masklength-1, len(text.rstrip())), -1, -1):
####                dbg('i:', i, 'self._isMaskChar(%d)' % i, self._isMaskChar(i))
                if self._isMaskChar(i):
                    char = text[i]
####                    dbg("text[%d]: '%s'" % (i, char))
                    if char != ' ':
                        i += 1
                        break

        if i == 0:
            pos = self._goHome(getPosOnly=True)
        else:
            pos = min(i,self._masklength)

        field = self._FindField(pos)
        start, end = field._extent
        if field._insertRight and pos < end:
            pos = end
##        dbg('next pos:', pos)
##        dbg(indent=0)
        if getPosOnly:
            return pos
        else:
            self._SetInsertionPoint(pos)


    def _goHome(self, getPosOnly=False):
        """ Moves the insertion point to the beginning of user-entry """
##        dbg("MaskedEditMixin::_goHome; getPosOnly:", getPosOnly, indent=1)
        text = self._GetValue()
        for i in range(self._masklength):
            if self._isMaskChar(i):
                break
        pos = max(i, 0)
##        dbg(indent=0)
        if getPosOnly:
            return pos
        else:
            self._SetInsertionPoint(max(i,0))



    def _getAllowedChars(self, pos):
        """ Returns a string of all allowed user input characters for the provided
            mask character plus control options
        """
        maskChar = self.maskdict[pos]
        okchars = self.maskchardict[maskChar]    ## entry, get mask approved characters

        # convert okchars to unicode if required; will force subsequent appendings to
        # result in unicode strings
        if 'unicode' in wx.PlatformInfo and type(okchars) != types.UnicodeType:
            okchars = okchars.decode(self._defaultEncoding)

        field = self._FindField(pos)
        if okchars and field._okSpaces:          ## Allow spaces?
            okchars += " "
        if okchars and field._includeChars:      ## any additional included characters?
            okchars += field._includeChars
####        dbg('okchars[%d]:' % pos, okchars)
        return okchars


    def _isMaskChar(self, pos):
        """ Returns True if the char at position pos is a special mask character (e.g. NCXaA#)
        """
        if pos < self._masklength:
            return self._ismasked[pos]
        else:
            return False


    def _isTemplateChar(self,Pos):
        """ Returns True if the char at position pos is a template character (e.g. -not- NCXaA#)
        """
        if Pos < self._masklength:
            return not self._isMaskChar(Pos)
        else:
            return False


    def _isCharAllowed(self, char, pos, checkRegex=False, allowAutoSelect=True, ignoreInsertRight=False):
        """ Returns True if character is allowed at the specific position, otherwise False."""
##        dbg('_isCharAllowed', char, pos, checkRegex, indent=1)
        field = self._FindField(pos)
        right_insert = False

        if self.controlInitialized:
            sel_start, sel_to = self._GetSelection()
        else:
            sel_start, sel_to = pos, pos

        if (field._insertRight or self._ctrl_constraints._insertRight) and not ignoreInsertRight:
            start, end = field._extent
            field_len = end - start
            if self.controlInitialized:
                value = self._GetValue()
                fstr = value[start:end].strip()
                if field._padZero:
                    while fstr and fstr[0] == '0':
                        fstr = fstr[1:]
                input_len = len(fstr)
                if self._signOk and '-' in fstr or '(' in fstr:
                    input_len -= 1  # sign can move out of field, so don't consider it in length
            else:
                value = self._template
                input_len = 0   # can't get the current "value", so use 0


            # if entire field is selected or position is at end and field is not full,
            # or if allowed to right-insert at any point in field and field is not full and cursor is not at a fillChar
            # or the field is a singleton integer field and is currently 0 and we're at the end:
            if( (sel_start, sel_to) == field._extent
                or (pos == end and ((input_len < field_len)
                                     or (field_len == 1
                                         and input_len == field_len
                                         and field._isInt
                                         and value[end-1] == '0'
                                         )
                                    ) ) ):
                pos = end - 1
##                dbg('pos = end - 1 = ', pos, 'right_insert? 1')
                right_insert = True
            elif( field._allowInsert and sel_start == sel_to
                  and (sel_to == end or (sel_to < self._masklength and value[sel_start] != field._fillChar))
                  and input_len < field_len ):
                pos = sel_to - 1    # where character will go
##                dbg('pos = sel_to - 1 = ', pos, 'right_insert? 1')
                right_insert = True
            # else leave pos alone...
            else:
##                dbg('pos stays ', pos, 'right_insert? 0')
                pass

        if self._isTemplateChar( pos ):  ## if a template character, return empty
##            dbg('%d is a template character; returning False' % pos, indent=0)
            return False

        if self._isMaskChar( pos ):
            okChars  = self._getAllowedChars(pos)

            if self._fields[0]._groupdigits and (self._isInt or (self._isFloat and pos < self._decimalpos)):
                okChars += self._fields[0]._groupChar

            if self._signOk:
                if self._isInt or (self._isFloat and pos < self._decimalpos):
                    okChars += '-'
                    if self._useParens:
                        okChars += '('
                elif self._useParens and (self._isInt or (self._isFloat and pos > self._decimalpos)):
                    okChars += ')'

####            dbg('%s in %s?' % (char, okChars), char in okChars)
            approved = (self.maskdict[pos] == '*' or char in okChars)

            if approved and checkRegex:
##                dbg("checking appropriate regex's")
                value = self._eraseSelection(self._GetValue())
                if right_insert:
                    # move the position to the right side of the insertion:
                    at = pos+1
                else:
                    at = pos
                if allowAutoSelect:
                    newvalue, ignore, ignore, ignore, ignore = self._insertKey(char, at, sel_start, sel_to, value, allowAutoSelect=True)
                else:
                    newvalue, ignore = self._insertKey(char, at, sel_start, sel_to, value)
##                dbg('newvalue: "%s"' % newvalue)

                fields = [self._FindField(pos)] + [self._ctrl_constraints]
                for field in fields:    # includes fields[-1] == "ctrl_constraints"
                    if field._regexMask and field._filter:
##                        dbg('checking vs. regex')
                        start, end = field._extent
                        slice = newvalue[start:end]
                        approved = (re.match( field._filter, slice) is not None)
##                        dbg('approved?', approved)
                    if not approved: break
##            dbg(indent=0)
            return approved
        else:
##            dbg('%d is a !???! character; returning False', indent=0)
            return False


    def _applyFormatting(self):
        """ Apply formatting depending on the control's state.
            Need to find a way to call this whenever the value changes, in case the control's
            value has been changed or set programatically.
        """
##        dbg(suspend=1)
##        dbg('MaskedEditMixin::_applyFormatting', indent=1)

        # Handle negative numbers
        if self._signOk:
            text, signpos, right_signpos = self._getSignedValue()
##            dbg('text: "%s", signpos:' % text, signpos)
            if text and signpos != self._signpos:
                self._signpos = signpos
            if not text or text[signpos] not in ('-','('):
                self._isNeg = False
##                dbg('no valid sign found; new sign:', self._isNeg)
            elif text and self._valid and not self._isNeg and text[signpos] in ('-', '('):
##                dbg('setting _isNeg to True')
                self._isNeg = True
##            dbg('self._isNeg:', self._isNeg)

        if self._signOk and self._isNeg:
            fc = self._signedForegroundColour
        else:
            fc = self._foregroundColour

        if hasattr(fc, '_name'):
            c =fc._name
        else:
            c = fc
##        dbg('setting foreground to', c)
        self.SetForegroundColour(fc)

        if self._valid:
##            dbg('valid')
            if self.IsEmpty():
                bc = self._emptyBackgroundColour
            else:
                bc = self._validBackgroundColour
        else:
##            dbg('invalid')
            bc = self._invalidBackgroundColour
        if hasattr(bc, '_name'):
            c =bc._name
        else:
            c = bc
##        dbg('setting background to', c)
        self.SetBackgroundColour(bc)
        self._Refresh()
##        dbg(indent=0, suspend=0)


    def _getAbsValue(self, candidate=None):
        """ Return an unsigned value (i.e. strip the '-' prefix if any), and sign position(s).
        """
##        dbg('MaskedEditMixin::_getAbsValue; candidate="%s"' % candidate, indent=1)
        if candidate is None: text = self._GetValue()
        else: text = candidate
        right_signpos = text.find(')')

        if self._isInt:
            if self._ctrl_constraints._alignRight and self._fields[0]._fillChar == ' ':
                signpos = text.find('-')
                if signpos == -1:
##                    dbg('no - found; searching for (')
                    signpos = text.find('(')
                elif signpos != -1:
##                    dbg('- found at', signpos)
                    pass

                if signpos == -1:
##                    dbg('signpos still -1')
##                    dbg('len(%s) (%d) < len(%s) (%d)?' % (text, len(text), self._mask, self._masklength), len(text) < self._masklength)
                    if len(text) < self._masklength:
                        text = ' ' + text
                    if len(text) < self._masklength:
                        text += ' '
                    if len(text) > self._masklength and text[-1] in (')', ' '):
                        text = text[:-1]
                    else:
##                        dbg('len(%s) (%d), len(%s) (%d)' % (text, len(text), self._mask, self._masklength))
##                        dbg('len(%s) - (len(%s) + 1):' % (text, text.lstrip()) , len(text) - (len(text.lstrip()) + 1))
                        signpos = len(text) - (len(text.lstrip()) + 1)

                        if self._useParens and not text.strip():
                            signpos -= 1    # empty value; use penultimate space
##                dbg('signpos:', signpos)
                if signpos >= 0:
                    text = text[:signpos] + ' ' + text[signpos+1:]

            else:
                if self._signOk:
                    signpos = 0
                    text = self._template[0] + text[1:]
                else:
                    signpos = -1

            if right_signpos != -1:
                if self._signOk:
                    text = text[:right_signpos] + ' ' + text[right_signpos+1:]
                elif len(text) > self._masklength:
                    text = text[:right_signpos] + text[right_signpos+1:]
                    right_signpos = -1


            elif self._useParens and self._signOk:
                # figure out where it ought to go:
                right_signpos = self._masklength - 1     # initial guess
                if not self._ctrl_constraints._alignRight:
##                    dbg('not right-aligned')
                    if len(text.strip()) == 0:
                        right_signpos = signpos + 1
                    elif len(text.strip()) < self._masklength:
                        right_signpos = len(text.rstrip())
##                dbg('right_signpos:', right_signpos)

            groupchar = self._fields[0]._groupChar
            try:
                value = long(text.replace(groupchar,'').replace('(','-').replace(')','').replace(' ', ''))
            except:
##                dbg('invalid number', indent=0)
                return None, signpos, right_signpos

        else:   # float value
            try:
                groupchar = self._fields[0]._groupChar
                value = float(text.replace(groupchar,'').replace(self._decimalChar, '.').replace('(', '-').replace(')','').replace(' ', ''))
##                dbg('value:', value)
            except:
                value = None

            if value < 0 and value is not None:
                signpos = text.find('-')
                if signpos == -1:
                    signpos = text.find('(')

                text = text[:signpos] + self._template[signpos] + text[signpos+1:]
            else:
                # look forwards up to the decimal point for the 1st non-digit
##                dbg('decimal pos:', self._decimalpos)
##                dbg('text: "%s"' % text)
                if self._signOk:
                    signpos = self._decimalpos - (len(text[:self._decimalpos].lstrip()) + 1)
                    # prevent checking for empty string - Tomo - Wed 14 Jan 2004 03:19:09 PM CET
                    if len(text) >= signpos+1 and  text[signpos+1] in ('-','('):
                        signpos += 1
                else:
                    signpos = -1
##                dbg('signpos:', signpos)

            if self._useParens:
                if self._signOk:
                    right_signpos = self._masklength - 1
                    text = text[:right_signpos] + ' '
                    if text[signpos] == '(':
                        text = text[:signpos] + ' ' + text[signpos+1:]
                else:
                    right_signpos = text.find(')')
                    if right_signpos != -1:
                        text = text[:-1]
                        right_signpos = -1

            if value is None:
##                dbg('invalid number')
                text = None

##        dbg('abstext = "%s"' % text, 'signpos:', signpos, 'right_signpos:', right_signpos)
##        dbg(indent=0)
        return text, signpos, right_signpos


    def _getSignedValue(self, candidate=None):
        """ Return a signed value by adding a "-" prefix if the value
            is set to negative, or a space if positive.
        """
##        dbg('MaskedEditMixin::_getSignedValue; candidate="%s"' % candidate, indent=1)
        if candidate is None: text = self._GetValue()
        else: text = candidate


        abstext, signpos, right_signpos = self._getAbsValue(text)
        if self._signOk:
            if abstext is None:
##                dbg(indent=0)
                return abstext, signpos, right_signpos

            if self._isNeg or text[signpos] in ('-', '('):
                if self._useParens:
                    sign = '('
                else:
                    sign = '-'
            else:
                sign = ' '
            if abstext[signpos] not in string.digits:
                text = abstext[:signpos] + sign + abstext[signpos+1:]
            else:
                # this can happen if value passed is too big; sign assumed to be
                # in position 0, but if already filled with a digit, prepend sign...
                text = sign + abstext
            if self._useParens and text.find('(') != -1:
                text = text[:right_signpos] + ')' + text[right_signpos+1:]
        else:
            text = abstext
##        dbg('signedtext = "%s"' % text, 'signpos:', signpos, 'right_signpos', right_signpos)
##        dbg(indent=0)
        return text, signpos, right_signpos


    def GetPlainValue(self, candidate=None):
        """ Returns control's value stripped of the template text.
            plainvalue = MaskedEditMixin.GetPlainValue()
        """
##        dbg('MaskedEditMixin::GetPlainValue; candidate="%s"' % candidate, indent=1)

        if candidate is None: text = self._GetValue()
        else: text = candidate

        if self.IsEmpty():
##            dbg('returned ""', indent=0)
            return ""
        else:
            plain = ""
            for idx in range( min(len(self._template), len(text)) ):
                if self._mask[idx] in maskchars:
                    plain += text[idx]

            if self._isFloat or self._isInt:
##                dbg('plain so far: "%s"' % plain)
                plain = plain.replace('(', '-').replace(')', ' ')
##                dbg('plain after sign regularization: "%s"' % plain)

                if self._signOk and self._isNeg and plain.count('-') == 0:
                    # must be in reserved position; add to "plain value"
                    plain = '-' + plain.strip()

                if self._fields[0]._alignRight:
                    lpad = plain.count(',')
                    plain = ' ' * lpad + plain.replace(',','')
                else:
                    plain = plain.replace(',','')
##                dbg('plain after pad and group:"%s"' % plain)

##            dbg('returned "%s"' % plain.rstrip(), indent=0)
            return plain.rstrip()


    def IsEmpty(self, value=None):
        """
        Returns True if control is equal to an empty value.
        (Empty means all editable positions in the template == fillChar.)
        """
        if value is None: value = self._GetValue()
        if value == self._template and not self._defaultValue:
####            dbg("IsEmpty? 1 (value == self._template and not self._defaultValue)")
            return True     # (all mask chars == fillChar by defn)
        elif value == self._template:
            empty = True
            for pos in range(len(self._template)):
####                dbg('isMaskChar(%(pos)d)?' % locals(), self._isMaskChar(pos))
####                dbg('value[%(pos)d] != self._fillChar?' %locals(), value[pos] != self._fillChar[pos])
                if self._isMaskChar(pos) and value[pos] not in (' ', self._fillChar[pos]):
                    empty = False
####            dbg("IsEmpty? %(empty)d (do all mask chars == fillChar?)" % locals())
            return empty
        else:
####            dbg("IsEmpty? 0 (value doesn't match template)")
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
####        dbg('MaskedEditMixin::IsValid("%s")' % value, indent=1)
        if value is None: value = self._GetValue()
        ret = self._CheckValid(value)
####        dbg(indent=0)
        return ret


    def _eraseSelection(self, value=None, sel_start=None, sel_to=None):
        """ Used to blank the selection when inserting a new character. """
##        dbg("MaskedEditMixin::_eraseSelection", indent=1)
        if value is None: value = self._GetValue()
        if sel_start is None or sel_to is None:
            sel_start, sel_to = self._GetSelection()                   ## check for a range of selected text
##        dbg('value: "%s"' % value)
##        dbg("current sel_start, sel_to:", sel_start, sel_to)

        newvalue = list(value)
        for i in range(sel_start, sel_to):
            if self._signOk and newvalue[i] in ('-', '(', ')'):
##                dbg('found sign (%s) at' % newvalue[i], i)

                # balance parentheses:
                if newvalue[i] == '(':
                    right_signpos = value.find(')')
                    if right_signpos != -1:
                        newvalue[right_signpos] = ' '

                elif newvalue[i] == ')':
                    left_signpos = value.find('(')
                    if left_signpos != -1:
                        newvalue[left_signpos] = ' '

                newvalue[i] = ' '

            elif self._isMaskChar(i):
                field = self._FindField(i)
                if field._padZero:
                    newvalue[i] = '0'
                else:
                    newvalue[i] = self._template[i]

        value = string.join(newvalue,"")
##        dbg('new value: "%s"' % value)
##        dbg(indent=0)
        return value


    def _insertKey(self, char, pos, sel_start, sel_to, value, allowAutoSelect=False):
        """ Handles replacement of the character at the current insertion point."""
##        dbg('MaskedEditMixin::_insertKey', "\'" + char + "\'", pos, sel_start, sel_to, '"%s"' % value, indent=1)

        text = self._eraseSelection(value)
        field = self._FindField(pos)
        start, end = field._extent
        newtext = ""
        newpos = pos

        # if >= 2 chars selected in a right-insert field, do appropriate erase on field,
        # then set selection to end, and do usual right insert.
        if sel_start != sel_to and sel_to >= sel_start+2:
            field = self._FindField(sel_start)
            if( field._insertRight                          # if right-insert
                and field._allowInsert                      # and allow insert at any point in field
                and field == self._FindField(sel_to) ):     # and selection all in same field
                text = self._OnErase(just_return_value=True)    # remove selection before insert
##                dbg('text after (left)erase: "%s"' % text)
                pos = sel_start = sel_to

        if pos != sel_start and sel_start == sel_to:
            # adjustpos must have moved the position; make selection match:
            sel_start = sel_to = pos

##        dbg('field._insertRight?', field._insertRight)
##        dbg('field._allowInsert?', field._allowInsert)
##        dbg('sel_start, end', sel_start, end)
        if sel_start < end:
##            dbg('text[sel_start] != field._fillChar?', text[sel_start] != field._fillChar)
            pass

        if( field._insertRight                                  # field allows right insert
            and ((sel_start, sel_to) == field._extent           # and whole field selected
                 or (sel_start == sel_to                        # or nothing selected
                     and (sel_start == end                      # and cursor at right edge
                          or (field._allowInsert                # or field allows right-insert
                              and sel_start < end               # next to other char in field:
                              and text[sel_start] != field._fillChar) ) ) ) ):
##            dbg('insertRight')
            fstr = text[start:end]
            erasable_chars = [field._fillChar, ' ']

            # if zero padding field, or a single digit, and currently a value of 0, allow erasure of 0:
            if field._padZero or (field._isInt and (end - start == 1) and fstr[0] == '0'):
                erasable_chars.append('0')

            erased = ''
####            dbg("fstr[0]:'%s'" % fstr[0])
####            dbg('field_index:', field._index)
####            dbg("fstr[0] in erasable_chars?", fstr[0] in erasable_chars)
####            dbg("self._signOk and field._index == 0 and fstr[0] in ('-','(')?", self._signOk and field._index == 0 and fstr[0] in ('-','('))
            if fstr[0] in erasable_chars or (self._signOk and field._index == 0 and fstr[0] in ('-','(')):
                erased = fstr[0]
####                dbg('value:      "%s"' % text)
####                dbg('fstr:       "%s"' % fstr)
####                dbg("erased:     '%s'" % erased)
                field_sel_start = sel_start - start
                field_sel_to = sel_to - start
##                dbg('left fstr:  "%s"' % fstr[1:field_sel_start])
##                dbg('right fstr: "%s"' % fstr[field_sel_to:end])
                fstr = fstr[1:field_sel_start] + char + fstr[field_sel_to:end]
            if field._alignRight and sel_start != sel_to:
                field_len = end - start
##                pos += (field_len - len(fstr))    # move cursor right by deleted amount
                pos = sel_to
##                dbg('setting pos to:', pos)
                if field._padZero:
                    fstr = '0' * (field_len - len(fstr)) + fstr
                else:
                    fstr = fstr.rjust(field_len)   # adjust the field accordingly
##            dbg('field str: "%s"' % fstr)

            newtext = text[:start] + fstr + text[end:]
            if erased in ('-', '(') and self._signOk:
                newtext = erased + newtext[1:]
##            dbg('newtext: "%s"' % newtext)

            if self._signOk and field._index == 0:
                start -= 1             # account for sign position

####            dbg('field._moveOnFieldFull?', field._moveOnFieldFull)
####            dbg('len(fstr.lstrip()) == end-start?', len(fstr.lstrip()) == end-start)
            if( field._moveOnFieldFull and pos == end
                and len(fstr.lstrip()) == end-start):   # if field now full
                newpos = self._findNextEntry(end)       #   go to next field
            else:
                newpos = pos                            # else keep cursor at current position

        if not newtext:
##            dbg('not newtext')
            if newpos != pos:
##                dbg('newpos:', newpos)
                pass
            if self._signOk and self._useParens:
                old_right_signpos = text.find(')')

            if field._allowInsert and not field._insertRight and sel_to <= end and sel_start >= start:
##                dbg('inserting within a left-insert-capable field')
                field_len = end - start
                before = text[start:sel_start]
                after = text[sel_to:end].strip()
####                dbg("current field:'%s'" % text[start:end])
####                dbg("before:'%s'" % before, "after:'%s'" % after)
                new_len = len(before) + len(after) + 1 # (for inserted char)
####                dbg('new_len:', new_len)

                if new_len < field_len:
                    retained = after + self._template[end-(field_len-new_len):end]
                elif new_len > end-start:
                    retained = after[1:]
                else:
                    retained = after

                left = text[0:start] + before
####                dbg("left:'%s'" % left, "retained:'%s'" % retained)
                right   = retained + text[end:]
            else:
                left  = text[0:pos]
                right   = text[pos+1:]

            if 'unicode' in wx.PlatformInfo and type(char) != types.UnicodeType:
                # convert the keyboard constant to a unicode value, to
                # ensure it can be concatenated into the control value:
                char = char.decode(self._defaultEncoding)

            newtext = left + char + right
####            dbg('left:    "%s"' % left)
####            dbg('right:   "%s"' % right)
####            dbg('newtext: "%s"' % newtext)

            if self._signOk and self._useParens:
                # Balance parentheses:
                left_signpos = newtext.find('(')

                if left_signpos == -1:     # erased '('; remove ')'
                    right_signpos = newtext.find(')')
                    if right_signpos != -1:
                        newtext = newtext[:right_signpos] + ' ' + newtext[right_signpos+1:]

                elif old_right_signpos != -1:
                    right_signpos = newtext.find(')')

                    if right_signpos == -1: # just replaced right-paren
                        if newtext[pos] == ' ': # we just erased '); erase '('
                            newtext = newtext[:left_signpos] + ' ' + newtext[left_signpos+1:]
                        else:   # replaced with digit; move ') over
                            if self._ctrl_constraints._alignRight or self._isFloat:
                                newtext = newtext[:-1] + ')'
                            else:
                                rstripped_text = newtext.rstrip()
                                right_signpos = len(rstripped_text)
##                                dbg('old_right_signpos:', old_right_signpos, 'right signpos now:', right_signpos)
                                newtext = newtext[:right_signpos] + ')' + newtext[right_signpos+1:]

            if( field._insertRight                                  # if insert-right field (but we didn't start at right edge)
                and field._moveOnFieldFull                          # and should move cursor when full
                and len(newtext[start:end].strip()) == end-start):  # and field now full
                newpos = self._findNextEntry(end)                   #   go to next field
##                dbg('newpos = nextentry =', newpos)
            else:
##                dbg('pos:', pos, 'newpos:', pos+1)
                newpos = pos+1


        if allowAutoSelect:
            new_select_to = newpos     # (default return values)
            match_field = None
            match_index = None

            if field._autoSelect:
                match_index, partial_match = self._autoComplete(1,  # (always forward)
                                                                field._compareChoices,
                                                                newtext[start:end],
                                                                compareNoCase=field._compareNoCase,
                                                                current_index = field._autoCompleteIndex-1)
                if match_index is not None and partial_match:
                    matched_str = newtext[start:end]
                    newtext = newtext[:start] + field._choices[match_index] + newtext[end:]
                    new_select_to = end
                    match_field = field
                    if field._insertRight:
                        # adjust position to just after partial match in field
                        newpos = end - (len(field._choices[match_index].strip()) - len(matched_str.strip()))

            elif self._ctrl_constraints._autoSelect:
                match_index, partial_match = self._autoComplete(
                                        1,  # (always forward)
                                        self._ctrl_constraints._compareChoices,
                                        newtext,
                                        self._ctrl_constraints._compareNoCase,
                                        current_index = self._ctrl_constraints._autoCompleteIndex - 1)
                if match_index is not None and partial_match:
                    matched_str = newtext
                    newtext = self._ctrl_constraints._choices[match_index]
                    edit_end = self._ctrl_constraints._extent[1]
                    new_select_to = min(edit_end, len(newtext.rstrip()))
                    match_field = self._ctrl_constraints
                    if self._ctrl_constraints._insertRight:
                        # adjust position to just after partial match in control:
                        newpos = self._masklength - (len(self._ctrl_constraints._choices[match_index].strip()) - len(matched_str.strip()))

##            dbg('newtext: "%s"' % newtext, 'newpos:', newpos, 'new_select_to:', new_select_to)
##            dbg(indent=0)
            return newtext, newpos, new_select_to, match_field, match_index
        else:
##            dbg('newtext: "%s"' % newtext, 'newpos:', newpos)
##            dbg(indent=0)
            return newtext, newpos


    def _OnFocus(self,event):
        """
        This event handler is currently necessary to work around new default
        behavior as of wxPython2.3.3;
        The TAB key auto selects the entire contents of the wx.TextCtrl *after*
        the EVT_SET_FOCUS event occurs; therefore we can't query/adjust the selection
        *here*, because it hasn't happened yet.  So to prevent this behavior, and
        preserve the correct selection when the focus event is not due to tab,
        we need to pull the following trick:
        """
##        dbg('MaskedEditMixin::_OnFocus')
        if self.IsBeingDeleted() or self.GetParent().IsBeingDeleted():
            return 
        wx.CallAfter(self._fixSelection)
        event.Skip()
        self.Refresh()


    def _CheckValid(self, candidate=None):
        """
        This is the default validation checking routine; It verifies that the
        current value of the control is a "valid value," and has the side
        effect of coloring the control appropriately.
        """
##        dbg(suspend=1)
##        dbg('MaskedEditMixin::_CheckValid: candidate="%s"' % candidate, indent=1)
        oldValid = self._valid
        if candidate is None: value = self._GetValue()
        else: value = candidate
##        dbg('value: "%s"' % value)
        oldvalue = value
        valid = True    # assume True

        if not self.IsDefault(value) and self._isDate:                    ## Date type validation
            valid = self._validateDate(value)
##            dbg("valid date?", valid)

        elif not self.IsDefault(value) and self._isTime:
            valid = self._validateTime(value)
##            dbg("valid time?", valid)

        elif not self.IsDefault(value) and (self._isInt or self._isFloat):  ## Numeric type
            valid = self._validateNumeric(value)
##            dbg("valid Number?", valid)

        if valid:   # and not self.IsDefault(value):    ## generic validation accounts for IsDefault()
            ## valid so far; ensure also allowed by any list or regex provided:
            valid = self._validateGeneric(value)
##            dbg("valid value?", valid)

##        dbg('valid?', valid)

        if not candidate:
            self._valid = valid
            self._applyFormatting()
            if self._valid != oldValid:
##                dbg('validity changed: oldValid =',oldValid,'newvalid =', self._valid)
##                dbg('oldvalue: "%s"' % oldvalue, 'newvalue: "%s"' % self._GetValue())
                pass
##        dbg(indent=0, suspend=0)
        return valid


    def _validateGeneric(self, candidate=None):
        """ Validate the current value using the provided list or Regex filter (if any).
        """
        if candidate is None:
            text = self._GetValue()
        else:
            text = candidate

        valid = True    # assume True
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
            groupchar = self._fields[0]._groupChar
            if self._isFloat:
                number = float(value.replace(groupchar, '').replace(self._decimalChar, '.').replace('(', '-').replace(')', ''))
            else:
                number = long( value.replace(groupchar, '').replace('(', '-').replace(')', ''))
                if value.strip():
                    if self._fields[0]._alignRight:
                        require_digit_at = self._fields[0]._extent[1]-1
                    else:
                        require_digit_at = self._fields[0]._extent[0]
##                    dbg('require_digit_at:', require_digit_at)
##                    dbg("value[rda]: '%s'" % value[require_digit_at])
                    if value[require_digit_at] not in list(string.digits):
                        valid = False
                        return valid
                # else...
##            dbg('number:', number)
            if self._ctrl_constraints._hasRange:
                valid = self._ctrl_constraints._rangeLow <= number <= self._ctrl_constraints._rangeHigh
            else:
                valid = True
            groupcharpos = value.rfind(groupchar)
            if groupcharpos != -1:  # group char present
##                dbg('groupchar found at', groupcharpos)
                if self._isFloat and groupcharpos > self._decimalpos:
                    # 1st one found on right-hand side is past decimal point
##                    dbg('groupchar in fraction; illegal')
                    return False
                elif self._isFloat:
                    integer = value[:self._decimalpos].strip()
                else:
                    integer = value.strip()
##                dbg("integer:'%s'" % integer)
                if integer[0] in ('-', '('):
                    integer = integer[1:]
                if integer[-1] == ')':
                    integer = integer[:-1]

                parts = integer.split(groupchar)
##                dbg('parts:', parts)
                for i in range(len(parts)):
                    if i == 0 and abs(int(parts[0])) > 999:
##                        dbg('group 0 too long; illegal')
                        valid = False
                        break
                    elif i > 0 and (len(parts[i]) != 3 or ' ' in parts[i]):
##                        dbg('group %i (%s) not right size; illegal' % (i, parts[i]))
                        valid = False
                        break
        except ValueError:
##            dbg('value not a valid number')
            valid = False
        return valid


    def _validateDate(self, candidate=None):
        """ Validate the current date value using the provided Regex filter.
            Generally used for character types.BufferType
        """
##        dbg('MaskedEditMixin::_validateDate', indent=1)
        if candidate is None: value = self._GetValue()
        else: value = candidate
##        dbg('value = "%s"' % value)
        text = self._adjustDate(value, force4digit_year=True)     ## Fix the date up before validating it
##        dbg('text =', text)
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

            year, month, day = _getDateParts( datestr, self._datestyle)
            year = int(year)
##            dbg('self._dateExtent:', self._dateExtent)
            if self._dateExtent == 11:
                month = charmonths_dict[month.lower()]
            else:
                month = int(month)
            day = int(day)
##            dbg('year, month, day:', year, month, day)

        except ValueError:
##            dbg('cannot convert string to integer parts')
            valid = False
        except KeyError:
##            dbg('cannot convert string to integer month')
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
##                    dbg("trying to create date from values day=%d, month=%d, year=%d" % (day,month,year))
                    dateHandler = wx.DateTimeFromDMY(day,month,year)
##                    dbg("succeeded")
                    dateOk = True
                except:
##                    dbg('cannot convert string to valid date')
                    dateOk = False
                if not dateOk:
                    valid = False

            if valid:
                # wxDateTime doesn't take kindly to leading/trailing spaces when parsing,
                # so we eliminate them here:
                timeStr     = text[self._dateExtent+1:].strip()         ## time portion of the string
                if timeStr:
##                    dbg('timeStr: "%s"' % timeStr)
                    try:
                        checkTime    = dateHandler.ParseTime(timeStr)
                        valid = checkTime == len(timeStr)
                    except:
                        valid = False
                    if not valid:
##                        dbg('cannot convert string to valid time')
                        pass
##        if valid: dbg('valid date')
##        dbg(indent=0)
        return valid


    def _validateTime(self, candidate=None):
        """ Validate the current time value using the provided Regex filter.
            Generally used for character types.BufferType
        """
##        dbg('MaskedEditMixin::_validateTime', indent=1)
        # wxDateTime doesn't take kindly to leading/trailing spaces when parsing,
        # so we eliminate them here:
        if candidate is None: value = self._GetValue().strip()
        else: value = candidate.strip()
##        dbg('value = "%s"' % value)
        valid = True   # assume True until proven otherwise

        dateHandler = wx.DateTime_Today()
        try:
            checkTime    = dateHandler.ParseTime(value)
##            dbg('checkTime:', checkTime, 'len(value)', len(value))
            valid = checkTime == len(value)
        except:
            valid = False

        if not valid:
##            dbg('cannot convert string to valid time')
            pass
##        if valid: dbg('valid time')
##        dbg(indent=0)
        return valid


    def _OnKillFocus(self,event):
        """ Handler for EVT_KILL_FOCUS event.
        """
##        dbg('MaskedEditMixin::_OnKillFocus', 'isDate=',self._isDate, indent=1)
        if self.IsBeingDeleted() or self.GetParent().IsBeingDeleted():
            return 
        if self._mask and self._IsEditable():
            self._AdjustField(self._GetInsertionPoint())
            self._CheckValid()   ## Call valid handler

        self._LostFocus()    ## Provided for subclass use
        event.Skip()
##        dbg(indent=0)


    def _fixSelection(self):
        """
        This gets called after the TAB traversal selection is made, if the
        focus event was due to this, but before the EVT_LEFT_* events if
        the focus shift was due to a mouse event.

        The trouble is that, a priori, there's no explicit notification of
        why the focus event we received.  However, the whole reason we need to
        do this is because the default behavior on TAB traveral in a wx.TextCtrl is
        now to select the entire contents of the window, something we don't want.
        So we can *now* test the selection range, and if it's "the whole text"
        we can assume the cause, change the insertion point to the start of
        the control, and deselect.
        """
##        dbg('MaskedEditMixin::_fixSelection', indent=1)
        # can get here if called with wx.CallAfter after underlying 
        # control has been destroyed on close, but after focus
        # events
        if not self or not self._mask or not self._IsEditable():
##            dbg(indent=0)
            return

        sel_start, sel_to = self._GetSelection()
##        dbg('sel_start, sel_to:', sel_start, sel_to, 'self.IsEmpty()?', self.IsEmpty())

        if( sel_start == 0 and sel_to >= len( self._mask )   #(can be greater in numeric controls because of reserved space)
            and (not self._ctrl_constraints._autoSelect or self.IsEmpty() or self.IsDefault() ) ):
            # This isn't normally allowed, and so assume we got here by the new
            # "tab traversal" behavior, so we need to reset the selection
            # and insertion point:
##            dbg('entire text selected; resetting selection to start of control')
            self._goHome()
            field = self._FindField(self._GetInsertionPoint())
            edit_start, edit_end = field._extent
            if field._selectOnFieldEntry:
                if self._isFloat or self._isInt and field == self._fields[0]:
                    edit_start = 0
                self._SetInsertionPoint(edit_start)
                self._SetSelection(edit_start, edit_end)

            elif field._insertRight:
                self._SetInsertionPoint(edit_end)
                self._SetSelection(edit_end, edit_end)

        elif (self._isFloat or self._isInt):

            text, signpos, right_signpos = self._getAbsValue()
            if text is None or text == self._template:
                integer = self._fields[0]
                edit_start, edit_end = integer._extent

                if integer._selectOnFieldEntry:
##                    dbg('select on field entry:')
                    self._SetInsertionPoint(0)
                    self._SetSelection(0, edit_end)

                elif integer._insertRight:
##                    dbg('moving insertion point to end')
                    self._SetInsertionPoint(edit_end)
                    self._SetSelection(edit_end, edit_end)
                else:
##                    dbg('numeric ctrl is empty; start at beginning after sign')
                    self._SetInsertionPoint(signpos+1)   ## Move past minus sign space if signed
                    self._SetSelection(signpos+1, signpos+1)

        elif sel_start > self._goEnd(getPosOnly=True):
##            dbg('cursor beyond the end of the user input; go to end of it')
            self._goEnd()
        else:
##            dbg('sel_start, sel_to:', sel_start, sel_to, 'self._masklength:', self._masklength)
            pass
##        dbg(indent=0)


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
##        dbg("MaskedEditMixin::_Cut", indent=1)
        value = self._GetValue()
##        dbg('current value: "%s"' % value)
        sel_start, sel_to = self._GetSelection()                   ## check for a range of selected text
##        dbg('selected text: "%s"' % value[sel_start:sel_to].strip())
        do = wx.TextDataObject()
        do.SetText(value[sel_start:sel_to].strip())
        wx.TheClipboard.Open()
        wx.TheClipboard.SetData(do)
        wx.TheClipboard.Close()

        if sel_to - sel_start != 0:
            self._OnErase()
##        dbg(indent=0)


# WS Note: overriding Copy is no longer necessary given that you
# can no longer select beyond the last non-empty char in the control.
#
##    def _Copy( self ):
##        """
##        Override the wx.TextCtrl's .Copy function, with our own
##        that does validation.  Need to strip trailing spaces.
##        """
##        sel_start, sel_to = self._GetSelection()
##        select_len = sel_to - sel_start
##        textval = wx.TextCtrl._GetValue(self)
##
##        do = wx.TextDataObject()
##        do.SetText(textval[sel_start:sel_to].strip())
##        wx.TheClipboard.Open()
##        wx.TheClipboard.SetData(do)
##        wx.TheClipboard.Close()


    def _getClipboardContents( self ):
        """ Subroutine for getting the current contents of the clipboard.
        """
        do = wx.TextDataObject()
        wx.TheClipboard.Open()
        success = wx.TheClipboard.GetData(do)
        wx.TheClipboard.Close()

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
##        dbg(suspend=1)
##        dbg('MaskedEditMixin::_validatePaste("%(paste_text)s", %(sel_start)d, %(sel_to)d), raise_on_invalid? %(raise_on_invalid)d' % locals(), indent=1)
        select_length = sel_to - sel_start
        maxlength = select_length
##        dbg('sel_to - sel_start:', maxlength)
        if maxlength == 0:
            maxlength = self._masklength - sel_start
            item = 'control'
        else:
            item = 'selection'
##        dbg('maxlength:', maxlength)
        if 'unicode' in wx.PlatformInfo and type(paste_text) != types.UnicodeType:
            paste_text = paste_text.decode(self._defaultEncoding)

        length_considered = len(paste_text)
        if length_considered > maxlength:
##            dbg('paste text will not fit into the %s:' % item, indent=0)
            if raise_on_invalid:
##                dbg(indent=0, suspend=0)
                if item == 'control':
                    ve = ValueError('"%s" will not fit into the control "%s"' % (paste_text, self.name))
                    ve.value = paste_text
                    raise ve
                else:
                    ve = ValueError('"%s" will not fit into the selection' % paste_text)
                    ve.value = paste_text
                    raise ve
            else:
##                dbg(indent=0, suspend=0)
                return False, None, None

        text = self._template
##        dbg('length_considered:', length_considered)

        valid_paste = True
        replacement_text = ""
        replace_to = sel_start
        i = 0
        while valid_paste and i < length_considered and replace_to < self._masklength:
            if paste_text[i:] == self._template[replace_to:length_considered]:
                # remainder of paste matches template; skip char-by-char analysis
##                dbg('remainder paste_text[%d:] (%s) matches template[%d:%d]' % (i, paste_text[i:], replace_to, length_considered))
                replacement_text += paste_text[i:]
                replace_to = i = length_considered
                continue
            # else:
            char = paste_text[i]
            field = self._FindField(replace_to)
            if not field._compareNoCase:
                if field._forceupper:   char = char.upper()
                elif field._forcelower: char = char.lower()

##            dbg('char:', "'"+char+"'", 'i =', i, 'replace_to =', replace_to)
##            dbg('self._isTemplateChar(%d)?' % replace_to, self._isTemplateChar(replace_to))
            if not self._isTemplateChar(replace_to) and self._isCharAllowed( char, replace_to, allowAutoSelect=False, ignoreInsertRight=True):
                replacement_text += char
##                dbg("not template(%(replace_to)d) and charAllowed('%(char)s',%(replace_to)d)" % locals())
##                dbg("replacement_text:", '"'+replacement_text+'"')
                i += 1
                replace_to += 1
            elif( char == self._template[replace_to]
                  or (self._signOk and
                          ( (i == 0 and (char == '-' or (self._useParens and char == '(')))
                            or (i == self._masklength - 1 and self._useParens and char == ')') ) ) ):
                replacement_text += char
##                dbg("'%(char)s' == template(%(replace_to)d)" % locals())
##                dbg("replacement_text:", '"'+replacement_text+'"')
                i += 1
                replace_to += 1
            else:
                next_entry = self._findNextEntry(replace_to, adjustInsert=False)
                if next_entry == replace_to:
                    valid_paste = False
                else:
                    replacement_text += self._template[replace_to:next_entry]
##                    dbg("skipping template; next_entry =", next_entry)
##                    dbg("replacement_text:", '"'+replacement_text+'"')
                    replace_to = next_entry  # so next_entry will be considered on next loop

        if not valid_paste and raise_on_invalid:
##            dbg('raising exception', indent=0, suspend=0)
            ve = ValueError('"%s" cannot be inserted into the control "%s"' % (paste_text, self.name))
            ve.value = paste_text
            raise ve


        elif i < len(paste_text):
            valid_paste = False
            if raise_on_invalid:
##                dbg('raising exception', indent=0, suspend=0)
                ve = ValueError('"%s" will not fit into the control "%s"' % (paste_text, self.name))
                ve.value = paste_text
                raise ve

##        dbg('valid_paste?', valid_paste)
        if valid_paste:
##            dbg('replacement_text: "%s"' % replacement_text, 'replace to:', replace_to)
            pass
##        dbg(indent=0, suspend=0)
        return valid_paste, replacement_text, replace_to


    def _Paste( self, value=None, raise_on_invalid=False, just_return_value=False ):
        """
        Used to override the base control's .Paste() function,
        with our own that does validation.
        Note: _Paste must be called from a Paste() override in the
        derived control because the mixin functions can't override a
        method of a sibling class.
        """
##        dbg('MaskedEditMixin::_Paste (value = "%s")' % value, indent=1)
        if value is None:
            paste_text = self._getClipboardContents()
        else:
            paste_text = value

        if paste_text is not None:

            if 'unicode' in wx.PlatformInfo and type(paste_text) != types.UnicodeType:
                paste_text = paste_text.decode(self._defaultEncoding)

##            dbg('paste text: "%s"' % paste_text)
            # (conversion will raise ValueError if paste isn't legal)
            sel_start, sel_to = self._GetSelection()
##            dbg('selection:', (sel_start, sel_to))

            # special case: handle allowInsert fields properly
            field = self._FindField(sel_start)
            edit_start, edit_end = field._extent
            new_pos = None
            if field._allowInsert and sel_to <= edit_end and (sel_start + len(paste_text) < edit_end or field._insertRight):
                if field._insertRight:
                    # want to paste to the left; see if it will fit:
                    left_text = self._GetValue()[edit_start:sel_start].lstrip()
##                    dbg('len(left_text):', len(left_text))
##                    dbg('len(paste_text):', len(paste_text))
##                    dbg('sel_start - (len(left_text) + len(paste_text)) >= edit_start?', sel_start - (len(left_text) + len(paste_text)) >= edit_start)
                    if sel_start - (len(left_text) - (sel_to - sel_start) + len(paste_text)) >= edit_start:
                        # will fit! create effective paste text, and move cursor back to do so:
                        paste_text = left_text + paste_text
                        sel_start -= len(left_text)
                        paste_text = paste_text.rjust(sel_to - sel_start)
##                        dbg('modified paste_text to be: "%s"' % paste_text)
##                        dbg('modified selection to:', (sel_start, sel_to))
                    else:
##                        dbg("won't fit left;", 'paste text remains: "%s"' % paste_text)
                        pass
                else:
                    paste_text = paste_text + self._GetValue()[sel_to:edit_end].rstrip()
##                    dbg("allow insert, but not insert right;", 'paste text set to: "%s"' % paste_text)


                new_pos = sel_start + len(paste_text)   # store for subsequent positioning
##                dbg('paste within insertable field; adjusted paste_text: "%s"' % paste_text, 'end:', edit_end)
##                dbg('expanded selection to:', (sel_start, sel_to))

            # Another special case: paste won't fit, but it's a right-insert field where entire
            # non-empty value is selected, and there's room if the selection is expanded leftward:
            if( len(paste_text) > sel_to - sel_start
                and field._insertRight
                and sel_start > edit_start
                and sel_to >= edit_end
                and not self._GetValue()[edit_start:sel_start].strip() ):
                # text won't fit within selection, but left of selection is empty;
                # check to see if we can expand selection to accommodate the value:
                empty_space = sel_start - edit_start
                amount_needed = len(paste_text) - (sel_to - sel_start)
                if amount_needed <= empty_space:
                    sel_start -= amount_needed
##                    dbg('expanded selection to:', (sel_start, sel_to))


            # another special case: deal with signed values properly:
            if self._signOk:
                signedvalue, signpos, right_signpos = self._getSignedValue()
                paste_signpos = paste_text.find('-')
                if paste_signpos == -1:
                    paste_signpos = paste_text.find('(')

                # if paste text will result in signed value:
####                dbg('paste_signpos != -1?', paste_signpos != -1)
####                dbg('sel_start:', sel_start, 'signpos:', signpos)
####                dbg('field._insertRight?', field._insertRight)
####                dbg('sel_start - len(paste_text) >= signpos?', sel_start - len(paste_text) <= signpos)
                if paste_signpos != -1 and (sel_start <= signpos
                                            or (field._insertRight and sel_start - len(paste_text) <= signpos)):
                    signed = True
                else:
                    signed = False
                # remove "sign" from paste text, so we can auto-adjust for sign type after paste:
                paste_text = paste_text.replace('-', ' ').replace('(',' ').replace(')','')
##                dbg('unsigned paste text: "%s"' % paste_text)
            else:
                signed = False

            # another special case: deal with insert-right fields when selection is empty and
            # cursor is at end of field:
####            dbg('field._insertRight?', field._insertRight)
####            dbg('sel_start == edit_end?', sel_start == edit_end)
####            dbg('sel_start', sel_start, 'sel_to', sel_to)
            if field._insertRight and sel_start == edit_end and sel_start == sel_to:
                sel_start -= len(paste_text)
                if sel_start < 0:
                    sel_start = 0
##                dbg('adjusted selection:', (sel_start, sel_to))

            raise_on_invalid = raise_on_invalid or field._raiseOnInvalidPaste
            try:
                valid_paste, replacement_text, replace_to = self._validatePaste(paste_text, sel_start, sel_to, raise_on_invalid)
            except:
##                dbg('exception thrown', indent=0)
                raise

            if not valid_paste:
##                dbg('paste text not legal for the selection or portion of the control following the cursor;')
                if not wx.Validator_IsSilent():
                    wx.Bell()
##                dbg(indent=0)
                return None, -1
            # else...
            text = self._eraseSelection()

            new_text = text[:sel_start] + replacement_text + text[replace_to:]
            if new_text:
                new_text = string.ljust(new_text,self._masklength)
            if signed:
                new_text, signpos, right_signpos = self._getSignedValue(candidate=new_text)
                if new_text:
                    if self._useParens:
                        new_text = new_text[:signpos] + '(' + new_text[signpos+1:right_signpos] + ')' + new_text[right_signpos+1:]
                    else:
                        new_text = new_text[:signpos] + '-' + new_text[signpos+1:]
                    if not self._isNeg:
                        self._isNeg = 1

##            dbg("new_text:", '"'+new_text+'"')

            if not just_return_value:
                if new_text != self._GetValue():
                    self.modified = True
                if new_text == '':
                    self.ClearValue()
                else:
                    wx.CallAfter(self._SetValue, new_text)
                    if new_pos is None:
                        new_pos = sel_start + len(replacement_text)
                    wx.CallAfter(self._SetInsertionPoint, new_pos)
            else:
##                dbg(indent=0)
                return new_text, replace_to
        elif just_return_value:
##            dbg(indent=0)
            return self._GetValue(), sel_to
##        dbg(indent=0)

    def _Undo(self, value=None, prev=None, just_return_results=False):
        """ Provides an Undo() method in base controls. """
##        dbg("MaskedEditMixin::_Undo", indent=1)
        if value is None:
            value = self._GetValue()
        if prev is None:
            prev = self._prevValue
##        dbg('current value:  "%s"' % value)
##        dbg('previous value: "%s"' % prev)
        if prev is None:
##            dbg('no previous value', indent=0)
            return

        elif value != prev:
            # Determine what to select: (relies on fixed-length strings)
            # (This is a lot harder than it would first appear, because
            # of mask chars that stay fixed, and so break up the "diff"...)

            # Determine where they start to differ:
            i = 0
            length = len(value)     # (both are same length in masked control)

            while( value[:i] == prev[:i] ):
                    i += 1
            sel_start = i - 1


            # handle signed values carefully, so undo from signed to unsigned or vice-versa
            # works properly:
            if self._signOk:
                text, signpos, right_signpos = self._getSignedValue(candidate=prev)
                if self._useParens:
                    if prev[signpos] == '(' and prev[right_signpos] == ')':
                        self._isNeg = True
                    else:
                        self._isNeg = False
                    # eliminate source of "far-end" undo difference if using balanced parens:
                    value = value.replace(')', ' ')
                    prev = prev.replace(')', ' ')
                elif prev[signpos] == '-':
                    self._isNeg = True
                else:
                    self._isNeg = False

            # Determine where they stop differing in "undo" result:
            sm = difflib.SequenceMatcher(None, a=value, b=prev)
            i, j, k = sm.find_longest_match(sel_start, length, sel_start, length)
##            dbg('i,j,k = ', (i,j,k), 'value[i:i+k] = "%s"' % value[i:i+k], 'prev[j:j+k] = "%s"' % prev[j:j+k] )

            if k == 0:                              # no match found; select to end
                sel_to = length
            else:
                code_5tuples = sm.get_opcodes()
                for op, i1, i2, j1, j2 in code_5tuples:
##                    dbg("%7s value[%d:%d] (%s) prev[%d:%d] (%s)" % (op, i1, i2, value[i1:i2], j1, j2, prev[j1:j2]))
                    pass

                diff_found = False
                # look backward through operations needed to produce "previous" value;
                # first change wins:
                for next_op in range(len(code_5tuples)-1, -1, -1):
                    op, i1, i2, j1, j2 = code_5tuples[next_op]
##                    dbg('value[i1:i2]: "%s"' % value[i1:i2], 'template[i1:i2] "%s"' % self._template[i1:i2])
                    field = self._FindField(i2)
                    if op == 'insert' and prev[j1:j2] != self._template[j1:j2]:
##                        dbg('insert found: selection =>', (j1, j2))
                        sel_start = j1
                        sel_to = j2
                        diff_found = True
                        break
                    elif op == 'delete' and value[i1:i2] != self._template[i1:i2]:
                        edit_start, edit_end = field._extent
                        if field._insertRight and (field._allowInsert or i2 == edit_end):
                            sel_start = i2
                            sel_to = i2
                        else:
                            sel_start = i1
                            sel_to = j1
##                        dbg('delete found: selection =>', (sel_start, sel_to))
                        diff_found = True
                        break
                    elif op == 'replace':
                        if not prev[i1:i2].strip() and field._insertRight:
                            sel_start = sel_to = j2
                        else:
                            sel_start = j1
                            sel_to = j2
##                        dbg('replace found: selection =>', (sel_start, sel_to))
                        diff_found = True
                        break


                if diff_found:
                    # now go forwards, looking for earlier changes:
##                    dbg('searching forward...')
                    for next_op in range(len(code_5tuples)):
                        op, i1, i2, j1, j2 = code_5tuples[next_op]
                        field = self._FindField(i1)
                        if op == 'equal':
                            continue
                        elif op == 'replace':
                            if field._insertRight:
                                # if replace with spaces in an insert-right control, ignore "forward" replace
                                if not prev[i1:i2].strip():
                                    continue
                                elif j1 < i1:
##                                    dbg('setting sel_start to', j1)
                                    sel_start = j1
                                else:
##                                    dbg('setting sel_start to', i1)
                                    sel_start = i1
                            else:
##                                dbg('setting sel_start to', i1)
                                sel_start = i1
##                            dbg('saw replace; breaking')
                            break
                        elif op == 'insert' and not value[i1:i2]:
##                            dbg('forward %s found' % op)
                            if prev[j1:j2].strip():
##                                dbg('item to insert non-empty; setting sel_start to', j1)
                                sel_start = j1
                                break
                            elif not field._insertRight:
##                                dbg('setting sel_start to inserted space:', j1)
                                sel_start = j1
                                break
                        elif op == 'delete':
##                            dbg('delete; field._insertRight?', field._insertRight, 'value[%d:%d].lstrip: "%s"' % (i1,i2,value[i1:i2].lstrip()))
                            if field._insertRight:
                                if value[i1:i2].lstrip():
##                                    dbg('setting sel_start to ', j1)
                                    sel_start = j1
##                                    dbg('breaking loop')
                                    break
                                else:
                                    continue
                            else:
##                                dbg('saw delete; breaking')
                                break
                        else:
##                            dbg('unknown code!')
                            # we've got what we need
                            break


                if not diff_found:
##                    dbg('no insert,delete or replace found (!)')
                    # do "left-insert"-centric processing of difference based on l.c.s.:
                    if i == j and j != sel_start:         # match starts after start of selection
                        sel_to = sel_start + (j-sel_start)  # select to start of match
                    else:
                        sel_to = j                          # (change ends at j)


            # There are several situations where the calculated difference is
            # not what we want to select.  If changing sign, or just adding
            # group characters, we really don't want to highlight the characters
            # changed, but instead leave the cursor where it is.
            # Also, there a situations in which the difference can be ambiguous;
            # Consider:
            #
            # current value:    11234
            # previous value:   1111234
            #
            # Where did the cursor actually lie and which 1s were selected on the delete
            # operation?
            #
            # Also, difflib can "get it wrong;" Consider:
            #
            # current value:    "       128.66"
            # previous value:   "       121.86"
            #
            # difflib produces the following opcodes, which are sub-optimal:
            #    equal value[0:9] (       12) prev[0:9] (       12)
            #   insert value[9:9] () prev[9:11] (1.)
            #    equal value[9:10] (8) prev[11:12] (8)
            #   delete value[10:11] (.) prev[12:12] ()
            #    equal value[11:12] (6) prev[12:13] (6)
            #   delete value[12:13] (6) prev[13:13] ()
            #
            # This should have been:
            #    equal value[0:9] (       12) prev[0:9] (       12)
            #  replace value[9:11] (8.6) prev[9:11] (1.8)
            #    equal value[12:13] (6) prev[12:13] (6)
            #
            # But it didn't figure this out!
            #
            # To get all this right, we use the previous selection recorded to help us...

            if (sel_start, sel_to) != self._prevSelection:
##                dbg('calculated selection', (sel_start, sel_to), "doesn't match previous", self._prevSelection)

                prev_sel_start, prev_sel_to = self._prevSelection
                field = self._FindField(sel_start)
                if( self._signOk
                      and sel_start < self._masklength
                      and (prev[sel_start] in ('-', '(', ')')
                                     or value[sel_start] in ('-', '(', ')')) ):
                    # change of sign; leave cursor alone...
##                    dbg("prev[sel_start] in ('-', '(', ')')?", prev[sel_start] in ('-', '(', ')'))
##                    dbg("value[sel_start] in ('-', '(', ')')?", value[sel_start] in ('-', '(', ')'))
##                    dbg('setting selection to previous one')
                    sel_start, sel_to = self._prevSelection

                elif field._groupdigits and (value[sel_start:sel_to] == field._groupChar
                                             or prev[sel_start:sel_to] == field._groupChar):
                    # do not highlight grouping changes
##                    dbg('value[sel_start:sel_to] == field._groupChar?', value[sel_start:sel_to] == field._groupChar)
##                    dbg('prev[sel_start:sel_to] == field._groupChar?', prev[sel_start:sel_to] == field._groupChar)
##                    dbg('setting selection to previous one')
                    sel_start, sel_to = self._prevSelection

                else:
                    calc_select_len = sel_to - sel_start
                    prev_select_len = prev_sel_to - prev_sel_start

##                    dbg('sel_start == prev_sel_start', sel_start == prev_sel_start)
##                    dbg('sel_to > prev_sel_to', sel_to > prev_sel_to)

                    if prev_select_len >= calc_select_len:
                        # old selection was bigger; trust it:
##                        dbg('prev_select_len >= calc_select_len?', prev_select_len >= calc_select_len)
                        if not field._insertRight:
##                            dbg('setting selection to previous one')
                            sel_start, sel_to = self._prevSelection
                        else:
                            sel_to = self._prevSelection[1]
##                            dbg('setting selection to', (sel_start, sel_to))

                    elif( sel_to > prev_sel_to                  # calculated select past last selection
                          and prev_sel_to < len(self._template) # and prev_sel_to not at end of control
                          and sel_to == len(self._template) ):  # and calculated selection goes to end of control

                        i, j, k = sm.find_longest_match(prev_sel_to, length, prev_sel_to, length)
##                        dbg('i,j,k = ', (i,j,k), 'value[i:i+k] = "%s"' % value[i:i+k], 'prev[j:j+k] = "%s"' % prev[j:j+k] )
                        if k > 0:
                            # difflib must not have optimized opcodes properly;
                            sel_to = j

                    else:
                        # look for possible ambiguous diff:

                        # if last change resulted in no selection, test from resulting cursor position:
                        if prev_sel_start == prev_sel_to:
                            calc_select_len = sel_to - sel_start
                            field = self._FindField(prev_sel_start)

                            # determine which way to search from last cursor position for ambiguous change:
                            if field._insertRight:
                                test_sel_start = prev_sel_start
                                test_sel_to = prev_sel_start + calc_select_len
                            else:
                                test_sel_start = prev_sel_start - calc_select_len
                                test_sel_to = prev_sel_start
                        else:
                            test_sel_start, test_sel_to = prev_sel_start, prev_sel_to

##                        dbg('test selection:', (test_sel_start, test_sel_to))
##                        dbg('calc change: "%s"' % prev[sel_start:sel_to])
##                        dbg('test change: "%s"' % prev[test_sel_start:test_sel_to])

                        # if calculated selection spans characters, and same characters
                        # "before" the previous insertion point are present there as well,
                        # select the ones related to the last known selection instead.
                        if( sel_start != sel_to
                            and test_sel_to < len(self._template)
                            and prev[test_sel_start:test_sel_to] == prev[sel_start:sel_to] ):

                            sel_start, sel_to = test_sel_start, test_sel_to

                # finally, make sure that the old and new values are
                # different where we say they're different:
                while( sel_to - 1 > 0
                        and sel_to > sel_start
                        and value[sel_to-1:] == prev[sel_to-1:]):
                    sel_to -= 1
                while( sel_start + 1 < self._masklength
                        and sel_start < sel_to
                        and value[:sel_start+1] == prev[:sel_start+1]):
                    sel_start += 1

##            dbg('sel_start, sel_to:', sel_start, sel_to)
##            dbg('previous value: "%s"' % prev)
##            dbg(indent=0)
            if just_return_results:
                return prev, (sel_start, sel_to)
            # else...
            self._SetValue(prev)
            self._SetInsertionPoint(sel_start)
            self._SetSelection(sel_start, sel_to)

        else:
##            dbg('no difference between previous value')
##            dbg(indent=0)
            if just_return_results:
                return prev, self._GetSelection()


    def _OnClear(self, event):
        """ Provides an action for context menu delete operation """
        self.ClearValue()


    def _OnContextMenu(self, event):
##        dbg('MaskedEditMixin::OnContextMenu()', indent=1)
        menu = wx.Menu()
        menu.Append(wx.ID_UNDO, "Undo", "")
        menu.AppendSeparator()
        menu.Append(wx.ID_CUT, "Cut", "")
        menu.Append(wx.ID_COPY, "Copy", "")
        menu.Append(wx.ID_PASTE, "Paste", "")
        menu.Append(wx.ID_CLEAR, "Delete", "")
        menu.AppendSeparator()
        menu.Append(wx.ID_SELECTALL, "Select All", "")

        wx.EVT_MENU(menu, wx.ID_UNDO, self._OnCtrl_Z)
        wx.EVT_MENU(menu, wx.ID_CUT, self._OnCtrl_X)
        wx.EVT_MENU(menu, wx.ID_COPY, self._OnCtrl_C)
        wx.EVT_MENU(menu, wx.ID_PASTE, self._OnCtrl_V)
        wx.EVT_MENU(menu, wx.ID_CLEAR, self._OnClear)
        wx.EVT_MENU(menu, wx.ID_SELECTALL, self._OnCtrl_A)

        # ## WSS: The base control apparently handles
        # enable/disable of wx.ID_CUT, wx.ID_COPY, wx.ID_PASTE
        # and wx.ID_CLEAR menu items even if the menu is one
        # we created.  However, it doesn't do undo properly,
        # so we're keeping track of previous values ourselves.
        # Therefore, we have to override the default update for
        # that item on the menu:
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self._UndoUpdateUI)
        self._contextMenu = menu

        self.PopupMenu(menu, event.GetPosition())
        menu.Destroy()
        self._contextMenu = None
##        dbg(indent=0)

    def _UndoUpdateUI(self, event):
        if self._prevValue is None or self._prevValue == self._curValue:
            self._contextMenu.Enable(wx.ID_UNDO, False)
        else:
            self._contextMenu.Enable(wx.ID_UNDO, True)


    def _OnCtrlParametersChanged(self):
        """
        Overridable function to allow derived classes to take action as a
        result of parameter changes prior to possibly changing the value
        of the control.
        """
        pass

 ## ---------- ---------- ---------- ---------- ---------- ---------- ----------
class MaskedEditAccessorsMixin:
    """
    To avoid a ton of boiler-plate, and to automate the getter/setter generation
    for each valid control parameter so we never forget to add the functions when
    adding parameters, this class programmatically adds the masked edit mixin
    parameters to itself.
    (This makes it easier for Designers like Boa to deal with masked controls.)

    To further complicate matters, this is done with an extra level of inheritance,
    so that "general" classes like masked.TextCtrl can have all possible attributes,
    while derived classes, like masked.TimeCtrl and masked.NumCtrl can prevent
    exposure of those optional attributes of their base class that do not make
    sense for their derivation.

    Therefore, we define:
        BaseMaskedTextCtrl(TextCtrl, MaskedEditMixin)
    and
        masked.TextCtrl(BaseMaskedTextCtrl, MaskedEditAccessorsMixin).

    This allows us to then derive:
        masked.NumCtrl( BaseMaskedTextCtrl )

    and not have to expose all the same accessor functions for the
    derived control when they don't all make sense for it.

    """

    # Define the default set of attributes exposed by the most generic masked controls:
    exposed_basectrl_params = MaskedEditMixin.valid_ctrl_params.keys() + Field.valid_params.keys()
    exposed_basectrl_params.remove('index')
    exposed_basectrl_params.remove('extent')
    exposed_basectrl_params.remove('foregroundColour')   # (base class already has this)

    for param in exposed_basectrl_params:
        propname = param[0].upper() + param[1:]
        exec('def Set%s(self, value): self.SetCtrlParameters(%s=value)' % (propname, param))
        exec('def Get%s(self): return self.GetCtrlParameter("%s")''' % (propname, param))

        if param.find('Colour') != -1:
            # add non-british spellings, for backward-compatibility
            propname.replace('Colour', 'Color')

            exec('def Set%s(self, value): self.SetCtrlParameters(%s=value)' % (propname, param))
            exec('def Get%s(self): return self.GetCtrlParameter("%s")''' % (propname, param))




## ---------- ---------- ---------- ---------- ---------- ---------- ----------
## these are helper subroutines:

def _movetofloat( origvalue, fmtstring, neg, addseparators=False, sepchar = ',',fillchar=' '):
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


def _isDateType( fmtstring ):
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

def _isTimeType( fmtstring ):
    """ Checks the mask and returns True if it fits an allowed
        time format.
    """
    reTimeMask = "^##:##(:##)?( (AM|PM))?"
    filter = re.compile( reTimeMask )
    if re.match(filter,fmtstring): return True
    return False


def _isFloatingPoint( fmtstring):
    filter = re.compile("[ ]?[#]+\.[#]+\n")
    if re.match(filter,fmtstring+"\n"): return True
    return False


def _isInteger( fmtstring ):
    filter = re.compile("[#]+\n")
    if re.match(filter,fmtstring+"\n"): return True
    return False


def _getDateParts( dateStr, dateFmt ):
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


def _getDateSepChar(dateStr):
    clip   = dateStr[0:10]
    dateSep = (('/' in clip) * '/') + (('-' in clip) * '-') + (('.' in clip) * '.')
    return dateSep


def _makeDate( year, month, day, dateFmt, dateStr):
    sep    = _getDateSepChar( dateStr)
    if dateFmt == "MDY":
        return "%s%s%s%s%s" % (month,sep,day,sep,year)  ## year, month, date parts
    elif dateFmt == "DMY":
        return "%s%s%s%s%s" % (day,sep,month,sep,year)  ## year, month, date parts
    elif dateFmt == "YMD":
        return "%s%s%s%s%s" % (year,sep,month,sep,day)  ## year, month, date parts
    else:
        return none


def _getYear(dateStr,dateFmt):
    parts = _getDateParts( dateStr, dateFmt)
    return parts[0]

def _getMonth(dateStr,dateFmt):
    parts = _getDateParts( dateStr, dateFmt)
    return parts[1]

def _getDay(dateStr,dateFmt):
    parts = _getDateParts( dateStr, dateFmt)
    return parts[2]

## ---------- ---------- ---------- ---------- ---------- ---------- ----------
class __test(wx.PySimpleApp):
        def OnInit(self):
            from wx.lib.rcsizer import RowColSizer
            self.frame = wx.Frame( None, -1, "MaskedEditMixin 0.0.7 Demo Page #1", size = (700,600))
            self.panel = wx.Panel( self.frame, -1)
            self.sizer = RowColSizer()
            self.labels = []
            self.editList  = []
            rowcount    = 4

            id, id1 = wx.NewId(), wx.NewId()
            self.command1  = wx.Button( self.panel, id, "&Close" )
            self.command2  = wx.Button( self.panel, id1, "&AutoFormats" )
            self.sizer.Add(self.command1, row=0, col=0, flag=wx.ALL, border = 5)
            self.sizer.Add(self.command2, row=0, col=1, colspan=2, flag=wx.ALL, border = 5)
            self.panel.Bind(wx.EVT_BUTTON, self.onClick, self.command1 )
##            self.panel.SetDefaultItem(self.command1 )
            self.panel.Bind(wx.EVT_BUTTON, self.onClickPage, self.command2)

            self.check1 = wx.CheckBox( self.panel, -1, "Disallow Empty" )
            self.check2 = wx.CheckBox( self.panel, -1, "Highlight Empty" )
            self.sizer.Add( self.check1, row=0,col=3, flag=wx.ALL,border=5 )
            self.sizer.Add( self.check2, row=0,col=4, flag=wx.ALL,border=5 )
            self.panel.Bind(wx.EVT_CHECKBOX, self._onCheck1, self.check1 )
            self.panel.Bind(wx.EVT_CHECKBOX, self._onCheck2, self.check2 )


            label = """Press ctrl-s in any field to output the value and plain value. Press ctrl-x to clear and re-set any field.
Note that all controls have been auto-sized by including F in the format code.
Try entering nonsensical or partial values in validated fields to see what happens (use ctrl-s to test the valid status)."""
            label2 = "\nNote that the State and Last Name fields are list-limited (Name:Smith,Jones,Williams)."

            self.label1 = wx.StaticText( self.panel, -1, label)
            self.label2 = wx.StaticText( self.panel, -1, "Description")
            self.label3 = wx.StaticText( self.panel, -1, "Mask Value")
            self.label4 = wx.StaticText( self.panel, -1, "Format")
            self.label5 = wx.StaticText( self.panel, -1, "Reg Expr Val. (opt)")
            self.label6 = wx.StaticText( self.panel, -1, "MaskedEdit Ctrl")
            self.label7 = wx.StaticText( self.panel, -1, label2)
            self.label7.SetForegroundColour("Blue")
            self.label1.SetForegroundColour("Blue")
            self.label2.SetFont(wx.Font(9,wx.SWISS,wx.NORMAL,wx.BOLD))
            self.label3.SetFont(wx.Font(9,wx.SWISS,wx.NORMAL,wx.BOLD))
            self.label4.SetFont(wx.Font(9,wx.SWISS,wx.NORMAL,wx.BOLD))
            self.label5.SetFont(wx.Font(9,wx.SWISS,wx.NORMAL,wx.BOLD))
            self.label6.SetFont(wx.Font(9,wx.SWISS,wx.NORMAL,wx.BOLD))

            self.sizer.Add( self.label1, row=1,col=0,colspan=7, flag=wx.ALL,border=5)
            self.sizer.Add( self.label7, row=2,col=0,colspan=7, flag=wx.ALL,border=5)
            self.sizer.Add( self.label2, row=3,col=0, flag=wx.ALL,border=5)
            self.sizer.Add( self.label3, row=3,col=1, flag=wx.ALL,border=5)
            self.sizer.Add( self.label4, row=3,col=2, flag=wx.ALL,border=5)
            self.sizer.Add( self.label5, row=3,col=3, flag=wx.ALL,border=5)
            self.sizer.Add( self.label6, row=3,col=4, flag=wx.ALL,border=5)

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
                self.sizer.Add( wx.StaticText( self.panel, -1, control[0]),row=rowcount, col=0,border=5,flag=wx.ALL)
                self.sizer.Add( wx.StaticText( self.panel, -1, control[1]),row=rowcount, col=1,border=5, flag=wx.ALL)
                self.sizer.Add( wx.StaticText( self.panel, -1, control[3]),row=rowcount, col=2,border=5, flag=wx.ALL)
                self.sizer.Add( wx.StaticText( self.panel, -1, control[4][:20]),row=rowcount, col=3,border=5, flag=wx.ALL)

                if control in controls[:]:#-2]:
                    newControl  = MaskedTextCtrl( self.panel, -1, "",
                                                    mask         = control[1],
                                                    excludeChars = control[2],
                                                    formatcodes  = control[3],
                                                    includeChars = "",
                                                    validRegex   = control[4],
                                                    validRange   = control[5],
                                                    choices      = control[6],
                                                    defaultValue = control[7],
                                                    demo         = True)
                    if control[6]: newControl.SetCtrlParameters(choiceRequired = True)
                else:
                    newControl = MaskedComboBox(  self.panel, -1, "",
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

                self.sizer.Add( newControl, row=rowcount,col=4,flag=wx.ALL,border=5)
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
            self.page2 = __test2(self.frame,-1,"")
            self.page2.Show(True)

        def _onCheck1(self,event):
            """ Set required value on/off """
            value = event.IsChecked()
            if value:
                for control in self.editList:
                    control.SetCtrlParameters(emptyInvalid=True)
                    control.Refresh()
            else:
                for control in self.editList:
                    control.SetCtrlParameters(emptyInvalid=False)
                    control.Refresh()
            self.panel.Refresh()

        def _onCheck2(self,event):
            """ Highlight empty values"""
            value = event.IsChecked()
            if value:
                for control in self.editList:
                    control.SetCtrlParameters( emptyBackgroundColour = 'Aquamarine')
                    control.Refresh()
            else:
                for control in self.editList:
                    control.SetCtrlParameters( emptyBackgroundColour = 'White')
                    control.Refresh()
            self.panel.Refresh()


## ---------- ---------- ---------- ---------- ---------- ---------- ----------

class __test2(wx.Frame):
        def __init__(self, parent, id, caption):
            wx.Frame.__init__( self, parent, id, "MaskedEdit control 0.0.7 Demo Page #2 -- AutoFormats", size = (550,600))
            from wx.lib.rcsizer import RowColSizer
            self.panel = wx.Panel( self, -1)
            self.sizer = RowColSizer()
            self.labels = []
            self.texts  = []
            rowcount    = 4

            label = """\
All these controls have been created by passing a single parameter, the AutoFormat code.
The class contains an internal dictionary of types and formats (autoformats).
To see a great example of validations in action, try entering a bad email address, then tab out."""

            self.label1 = wx.StaticText( self.panel, -1, label)
            self.label2 = wx.StaticText( self.panel, -1, "Description")
            self.label3 = wx.StaticText( self.panel, -1, "AutoFormat Code")
            self.label4 = wx.StaticText( self.panel, -1, "MaskedEdit Control")
            self.label1.SetForegroundColour("Blue")
            self.label2.SetFont(wx.Font(9,wx.SWISS,wx.NORMAL,wx.BOLD))
            self.label3.SetFont(wx.Font(9,wx.SWISS,wx.NORMAL,wx.BOLD))
            self.label4.SetFont(wx.Font(9,wx.SWISS,wx.NORMAL,wx.BOLD))

            self.sizer.Add( self.label1, row=1,col=0,colspan=3, flag=wx.ALL,border=5)
            self.sizer.Add( self.label2, row=3,col=0, flag=wx.ALL,border=5)
            self.sizer.Add( self.label3, row=3,col=1, flag=wx.ALL,border=5)
            self.sizer.Add( self.label4, row=3,col=2, flag=wx.ALL,border=5)

            id, id1 = wx.NewId(), wx.NewId()
            self.command1  = wx.Button( self.panel, id, "&Close")
            self.command2  = wx.Button( self.panel, id1, "&Print Formats")
            self.panel.Bind(wx.EVT_BUTTON, self.onClick, self.command1)
            self.panel.SetDefaultItem(self.command1)
            self.panel.Bind(wx.EVT_BUTTON, self.onClickPrint, self.command2)

            # The following list is of the controls for the demo. Feel free to play around with
            # the options!
            controls = [
           ("Phone No","USPHONEFULLEXT"),
           ("US Date + Time","USDATETIMEMMDDYYYY/HHMM"),
           ("US Date MMDDYYYY","USDATEMMDDYYYY/"),
           ("Time (with seconds)","TIMEHHMMSS"),
           ("Military Time\n(without seconds)","24HRTIMEHHMM"),
           ("Social Sec#","USSOCIALSEC"),
           ("Credit Card","CREDITCARD"),
           ("Expiration MM/YY","EXPDATEMMYY"),
           ("Percentage","PERCENT"),
           ("Person's Age","AGE"),
           ("US Zip Code","USZIP"),
           ("US Zip+4","USZIPPLUS4"),
           ("Email Address","EMAIL"),
           ("IP Address", "(derived control IpAddrCtrl)")
           ]

            for control in controls:
                self.sizer.Add( wx.StaticText( self.panel, -1, control[0]),row=rowcount, col=0,border=5,flag=wx.ALL)
                self.sizer.Add( wx.StaticText( self.panel, -1, control[1]),row=rowcount, col=1,border=5, flag=wx.ALL)
                if control in controls[:-1]:
                    self.sizer.Add( MaskedTextCtrl( self.panel, -1, "",
                                                      autoformat  = control[1],
                                                      demo        = True),
                                row=rowcount,col=2,flag=wx.ALL,border=5)
                else:
                    self.sizer.Add( IpAddrCtrl( self.panel, -1, "", demo=True ),
                                    row=rowcount,col=2,flag=wx.ALL,border=5)
                rowcount += 1

            self.sizer.Add(self.command1, row=0, col=0, flag=wx.ALL, border = 5)
            self.sizer.Add(self.command2, row=0, col=1, flag=wx.ALL, border = 5)
            self.sizer.AddGrowableCol(3)

            self.panel.SetSizer(self.sizer)
            self.panel.SetAutoLayout(1)

        def onClick(self, event):
            self.Close()

        def onClickPrint(self, event):
            for format in masktags.keys():
                sep = "+------------------------+"
                print "%s\n%s  \n  Mask: %s \n  RE Validation string: %s\n" % (sep,format, masktags[format]['mask'], masktags[format]['validRegex'])

## ---------- ---------- ---------- ---------- ---------- ---------- ----------

if __name__ == "__main__":
    app = __test(False)

__i=0
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
## 2. WS: MaskedComboBox is deficient in several areas, all having to do with the
##      behavior of the underlying control that I can't fix.  The problems are:
##      a) The background coloring doesn't work in the text field of the control;
##         instead, there's a only border around it that assumes the correct color.
##      b) The control will not pass WXK_TAB to the event handler, no matter what
##         I do, and there's no style wxCB_PROCESS_TAB like wxTE_PROCESS_TAB to
##         indicate that we want these events.  As a result, MaskedComboBox
##         doesn't do the nice field-tabbing that MaskedTextCtrl does.
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
##         this makes it impossible to paste anything into a MaskedComboBox
##         at the moment... :-(
##      e) The other deficient behavior, likely induced by the workaround for (d),
##         is that you can can't shift-left to select more than one character
##         at a time.
##
##
## 3. WS: Controls on wxPanels don't seem to pass Shift-WXK_TAB to their
##      EVT_KEY_DOWN or EVT_CHAR event handlers.  Until this is fixed in
##      wxWindows, shift-tab won't take you backwards through the fields of
##      a MaskedTextCtrl like it should.  Until then Shifted arrow keys will
##      work like shift-tab and tab ought to.
##

## To-Do's:
## =============================##
##  1. Add Popup list for auto-completable fields that simulates combobox on individual
##     fields.  Example: City validates against list of cities, or zip vs zip code list.
##  2. Allow optional monetary symbols (eg. $, pounds, etc.) at front of a "decimal"
##     control.
##  3. Fix shift-left selection for MaskedComboBox.
##  5. Transform notion of "decimal control" to be less "entire control"-centric,
##     so that monetary symbols can be included and still have the appropriate
##     semantics.  (Big job, as currently written, but would make control even
##     more useful for business applications.)


## CHANGELOG:
## ====================
##  Version 1.11
##  1. Added value member to ValueError exceptions, so that people can catch them
##     and then display their own errors, and added attribute raiseOnInvalidPaste,
##     so one doesn't have to subclass the controls simply to force generation of
##     a ValueError on a bad paste operation.
##  2. Fixed handling of unicode charsets by converting to explicit control char
##     set testing for passing those keystrokes to the base control, and then 
##     changing the semantics of the * maskchar to indicate any visible char.
##  3. Added '|' mask specification character, which allows splitting of contiguous
##     mask characters into separate fields, allowing finer control of behavior
##     of a control.
##     
##
##  Version 1.10
##  1. Added handling for WXK_DELETE and WXK_INSERT, such that shift-delete
##     cuts, shift-insert pastes, and ctrl-insert copies.
##
##  Version 1.9
##  1. Now ignores kill focus events when being destroyed.
##  2. Added missing call to set insertion point on changing fields.
##  3. Modified SetKeyHandler() to accept None as means of removing one.
##  4. Fixed keyhandler processing for group and decimal character changes.
##  5. Fixed a problem that prevented input into the integer digit of a 
##     integerwidth=1 numctrl, if the current value was 0.
##  6. Fixed logic involving processing of "_signOk" flag, to remove default
##     sign key handlers if false, so that SetAllowNegative(False) in the
##     NumCtrl works properly.
##  7. Fixed selection logic for numeric controls so that if selectOnFieldEntry
##     is true, and the integer portion of an integer format control is selected
##     and the sign position is selected, the sign keys will always result in a
##     negative value, rather than toggling the previous sign.
##
##
##  Version 1.8
##  1. Fixed bug involving incorrect variable name, causing combobox autocomplete to fail.
##  2. Added proper support for unicode version of wxPython
##  3. Added * as mask char meaning "all ansi chars" (ordinals 32-255).
##  4. Converted doc strings to use reST format, for ePyDoc documentation.
##  5. Renamed helper functions, classes, etc. not intended to be visible in public
##     interface to code.
##
##  Version 1.7
##  1. Fixed intra-right-insert-field erase, such that it doesn't leave a hole, but instead
##     shifts the text to the left accordingly.
##  2. Fixed _SetValue() to place cursor after last character inserted, rather than end of
##     mask.
##  3. Fixed some incorrect undo behavior for right-insert fields, and allowed derived classes
##     (eg. numctrl) to pass modified values for undo processing (to handle/ignore grouping
##     chars properly.)
##  4. Fixed autoselect behavior to work similarly to (2) above, so that combobox
##     selection will only select the non-empty text, as per request.
##  5. Fixed tabbing to work with 2.5.2 semantics.
##  6. Fixed size calculation to handle changing fonts
##
##  Version 1.6
##  1. Reorganized masked controls into separate package, renamed things accordingly
##  2. Split actual controls out of this file into their own files.
##  Version 1.5
##  (Reported) bugs fixed:
##   1. Crash ensues if you attempt to change the mask of a read-only
##      MaskedComboBox after initial construction.
##   2. Changed strategy of defining Get/Set property functions so that
##      these are now generated dynamically at runtime, rather than as
##      part of the class definition.  (This makes it possible to have
##      more general base classes that have many more options for configuration
##      without requiring that derivations support the same options.)
##   3. Fixed IsModified for _Paste() and _OnErase().
##
##   Enhancements:
##   1. Fixed "attribute function inheritance," since base control is more
##      generic than subsequent derivations, not all property functions of a
##      generic control should be exposed in those derivations.  New strategy
##      uses base control classes (eg. BaseMaskedTextCtrl) that should be
##      used to derive new class types, and mixed with their own mixins to
##      only expose those attributes from the generic masked controls that
##      make sense for the derivation.  (This makes Boa happier.)
##   2. Renamed (with b-c) MILTIME autoformats to 24HRTIME, so as to be less
##      "parochial."
##
##  Version 1.4
##  (Reported) bugs fixed:
##   1. Right-click menu allowed "cut" operation that destroyed mask
##      (was implemented by base control)
##   2. MaskedComboBox didn't allow .Append() of mixed-case values; all
##      got converted to lower case.
##   3. MaskedComboBox selection didn't deal with spaces in values
##      properly when autocompleting, and didn't have a concept of "next"
##      match for handling choice list duplicates.
##   4. Size of MaskedComboBox was always default.
##   5. Email address regexp allowed some "non-standard" things, and wasn't
##      general enough.
##   6. Couldn't easily reset MaskedComboBox contents programmatically.
##   7. Couldn't set emptyInvalid during construction.
##   8. Under some versions of wxPython, readonly comboboxes can apparently
##      return a GetInsertionPoint() result (655535), causing masked control
##      to fail.
##   9. Specifying an empty mask caused the controls to traceback.
##  10. Can't specify float ranges for validRange.
##  11. '.' from within a the static portion of a restricted IP address
##      destroyed the mask from that point rightward; tab when cursor is
##      before 1st field takes cursor past that field.
##
##  Enhancements:
##  12. Added Ctrl-Z/Undo handling, (and implemented context-menu properly.)
##  13. Added auto-select option on char input for masked controls with
##      choice lists.
##  14. Added '>' formatcode, allowing insert within a given or each field
##      as appropriate, rather than requiring "overwrite".  This makes single
##      field controls that just have validation rules (eg. EMAIL) much more
##      friendly.  The same flag controls left shift when deleting vs just
##      blanking the value, and for right-insert fields, allows right-insert
##      at any non-blank (non-sign) position in the field.
##  15. Added option to use to indicate negative values for numeric controls.
##  16. Improved OnFocus handling of numeric controls.
##  17. Enhanced Home/End processing to allow operation on a field level,
##      using ctrl key.
##  18. Added individual Get/Set functions for control parameters, for
##      simplified integration with Boa Constructor.
##  19. Standardized "Colour" parameter names to match wxPython, with
##      non-british spellings still supported for backward-compatibility.
##  20. Added '&' mask specification character for punctuation only (no letters
##      or digits).
##  21. Added (in a separate file) wx.MaskedCtrl() factory function to provide
##      unified interface to the masked edit subclasses.
##
##
##  Version 1.3
##   1. Made it possible to configure grouping, decimal and shift-decimal characters,
##      to make controls more usable internationally.
##   2. Added code to smart "adjust" value strings presented to .SetValue()
##      for right-aligned numeric format controls if they are shorter than
##      than the control width,  prepending the missing portion, prepending control
##      template left substring for the missing characters, so that setting
##      numeric values is easier.
##   3. Renamed SetMaskParameters SetCtrlParameters() (with old name preserved
##      for b-c), as this makes more sense.
##
##  Version 1.2
##   1. Fixed .SetValue() to replace the current value, rather than the current
##      selection. Also changed it to generate ValueError if presented with
##      either a value which doesn't follow the format or won't fit.  Also made
##      set value adjust numeric and date controls as if user entered the value.
##      Expanded doc explaining how SetValue() works.
##   2. Fixed EUDATE* autoformats, fixed IsDateType mask list, and added ability to
##      use 3-char months for dates, and EUDATETIME, and EUDATEMILTIME autoformats.
##   3. Made all date autoformats automatically pick implied "datestyle".
##   4. Added IsModified override, since base wx.TextCtrl never reports modified if
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
##  11. Made maskchardict an instance var, to make mask chars to be more
##      amenable to international use.
##  12. Clarified meaning of '-' formatcode in doc.
##  13. Fixed a couple of coding bugs being flagged by Python2.1.
##  14. Fixed several issues with sign positioning, erasure and validity
##      checking for "numeric" masked controls.
##  15. Added validation to IpAddrCtrl.SetValue().
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
##  13. Enhanced IpAddrCtrl to use right-insert fields, selection on field traversal,
##      individual field validation to prevent field values > 255, and require explicit
##      tab/. to change fields.
##  14. Added handler for left double-click to select field under cursor.
##  15. Fixed handling for "Read-only" styles.
##  16. Separated signedForegroundColor from 'R' style, and added foregroundColor
##      attribute, for more consistent and controllable coloring.
##  17. Added retainFieldValidation parameter, allowing top-level constraints
##      such as "validRequired" to be set independently of field-level equivalent.
##      (needed in TimeCtrl for bounds constraints.)
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
##   9. Fixed bug in choice management of MaskedComboBox.
##  10. Fixed bug in IpAddrCtrl causing traceback if field value was of
##     the form '# #'.  Modified control code for IpAddrCtrl so that '.'
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
##  10. Extracted mask logic into mixin, so we can have both MaskedTextCtrl and MaskedComboBox,
##      now included.
##  11. MaskedComboBox now adds the capability to validate from list of valid values.
##      Example: City validates against list of cities, or zip vs zip code list.
##  12. Fixed oversight in EVT_TEXT handler that prevented the events from being
##      passed to the next handler in the event chain, causing updates to the
##      control to be invisible to the parent code.
##  13. Added IPADDR autoformat code, and subclass IpAddrCtrl for controlling tabbing within
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
##      can be overridden by subclasses (eg. IpAddrCtrl.)
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
##   7. Class renamed to MaskedTextCtrl.
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
