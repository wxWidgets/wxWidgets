#----------------------------------------------------------------------------
# Name:         masked.textctrl.py
# Authors:      Jeff Childers, Will Sadkin
# Email:        jchilders_98@yahoo.com, wsadkin@nameconnector.com
# Created:      02/11/2003
# Copyright:    (c) 2003 by Jeff Childers, Will Sadkin, 2003
# Portions:     (c) 2002 by Will Sadkin, 2002-2003
# RCS-ID:       $Id$
# License:      wxWidgets license
#----------------------------------------------------------------------------
#
# This file contains the most typically used generic masked control,
# masked.TextCtrl.  It also defines the BaseMaskedTextCtrl, which can
# be used to derive other "semantics-specific" classes, like masked.NumCtrl,
# masked.TimeCtrl, and masked.IpAddrCtrl.
#
#----------------------------------------------------------------------------
"""
Provides a generic, fully configurable masked edit text control, as well as
a base class from which you can derive masked controls tailored to a specific
function.  See maskededit module overview for how to configure the control.
"""

import  wx
from wx.lib.masked import *

# jmg 12/9/03 - when we cut ties with Py 2.2 and earlier, this would
# be a good place to implement the 2.3 logger class
from wx.tools.dbg import Logger
##dbg = Logger()
##dbg(enable=1)


class BaseMaskedTextCtrl( wx.TextCtrl, MaskedEditMixin ):
    """
    This is the primary derivation from MaskedEditMixin.  It provides
    a general masked text control that can be configured with different
    masks.

    However, this is done with an extra level of inheritance, so that
    "general" classes like masked.TextCtrl can have all possible attributes,
    while derived classes, like masked.TimeCtrl and masked.NumCtrl
    can prevent exposure of those optional attributes of their base
    class that do not make sense for their derivation.  Therefore,
    we define::
    
        BaseMaskedTextCtrl(TextCtrl, MaskedEditMixin)

    and::
    
        masked.TextCtrl(BaseMaskedTextCtrl, MaskedEditAccessorsMixin).

    This allows us to then derive::
    
        masked.NumCtrl( BaseMaskedTextCtrl )

    and not have to expose all the same accessor functions for the
    derived control when they don't all make sense for it.

    In practice, BaseMaskedTextCtrl should never be instantiated directly,
    but should only be used in derived classes.
    """

    def __init__( self, parent, id=-1, value = '',
                  pos = wx.DefaultPosition,
                  size = wx.DefaultSize,
                  style = wx.TE_PROCESS_TAB,
                  validator=wx.DefaultValidator,     ## placeholder provided for data-transfer logic
                  name = 'maskedTextCtrl',
                  setupEventHandling = True,        ## setup event handling by default
                  **kwargs):

        wx.TextCtrl.__init__(self, parent, id, value='',
                            pos=pos, size = size,
                            style=style, validator=validator,
                            name=name)

        self._PostInit(setupEventHandling = setupEventHandling,
                      name=name, value=value,**kwargs )


    def _PostInit(self,setupEventHandling=True,
                 name='maskedTextCtrl' , value='', **kwargs):

        self.controlInitialized = True
        MaskedEditMixin.__init__( self, name, **kwargs )

        self._SetInitialValue(value)

        if setupEventHandling:
            ## Setup event handlers
            self.Bind(wx.EVT_SET_FOCUS, self._OnFocus )         ## defeat automatic full selection
            self.Bind(wx.EVT_KILL_FOCUS, self._OnKillFocus )    ## run internal validator
            self.Bind(wx.EVT_LEFT_DCLICK, self._OnDoubleClick)  ## select field under cursor on dclick
            self.Bind(wx.EVT_RIGHT_UP, self._OnContextMenu )    ## bring up an appropriate context menu
            self.Bind(wx.EVT_KEY_DOWN, self._OnKeyDown )        ## capture control events not normally seen, eg ctrl-tab.
            self.Bind(wx.EVT_CHAR, self._OnChar )               ## handle each keypress
            self.Bind(wx.EVT_TEXT, self._OnTextChange )         ## color control appropriately & keep
                                                                ## track of previous value for undo


    def __repr__(self):
        return "<BaseMaskedTextCtrl: %s>" % self.GetValue()


    def _GetSelection(self):
        """
        Allow mixin to get the text selection of this control.
        REQUIRED by any class derived from MaskedEditMixin.
        """
        return self.GetSelection()

    def _SetSelection(self, sel_start, sel_to):
        """
        Allow mixin to set the text selection of this control.
        REQUIRED by any class derived from MaskedEditMixin.
        """
####        dbg("MaskedTextCtrl::_SetSelection(%(sel_start)d, %(sel_to)d)" % locals())
        return self.SetSelection( sel_start, sel_to )

##    def SetSelection(self, sel_start, sel_to):
##        """
##        This is just for debugging...
##        """
##        dbg("MaskedTextCtrl::SetSelection(%(sel_start)d, %(sel_to)d)" % locals())
##        wx.TextCtrl.SetSelection(self, sel_start, sel_to)


    def _GetInsertionPoint(self):
        return self.GetInsertionPoint()

    def _SetInsertionPoint(self, pos):
####        dbg("MaskedTextCtrl::_SetInsertionPoint(%(pos)d)" % locals())
        self.SetInsertionPoint(pos)

##    def SetInsertionPoint(self, pos):
##        """
##        This is just for debugging...
##        """
##        dbg("MaskedTextCtrl::SetInsertionPoint(%(pos)d)" % locals())
##        wx.TextCtrl.SetInsertionPoint(self, pos)


    def IsEmpty(*args, **kw):
        return MaskedEditMixin.IsEmpty(*args, **kw)

    def _GetValue(self):
        """
        Allow mixin to get the raw value of the control with this function.
        REQUIRED by any class derived from MaskedEditMixin.
        """
        return self.GetValue()


    def _SetValue(self, value):
        """
        Allow mixin to set the raw value of the control with this function.
        REQUIRED by any class derived from MaskedEditMixin.
        """
##        dbg('MaskedTextCtrl::_SetValue("%(value)s")' % locals(), indent=1)
        # Record current selection and insertion point, for undo
        self._prevSelection = self._GetSelection()
        self._prevInsertionPoint = self._GetInsertionPoint()
        wx.TextCtrl.SetValue(self, value)
##        dbg(indent=0)

    def SetValue(self, value):
        """
        This function redefines the externally accessible .SetValue() to be
        a smart "paste" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """
##        dbg('MaskedTextCtrl::SetValue = "%s"' % value, indent=1)

        if not self._mask:
            wx.TextCtrl.SetValue(self, value)    # revert to base control behavior
            return

        # empty previous contents, replacing entire value:
        self._SetInsertionPoint(0)
        self._SetSelection(0, self._masklength)
        if self._signOk and self._useParens:
            signpos = value.find('-')
            if signpos != -1:
                value = value[:signpos] + '(' + value[signpos+1:].strip() + ')'
            elif value.find(')') == -1 and len(value) < self._masklength:
                value += ' '    # add place holder for reserved space for right paren

        if( len(value) < self._masklength                # value shorter than control
            and (self._isFloat or self._isInt)            # and it's a numeric control
            and self._ctrl_constraints._alignRight ):   # and it's a right-aligned control

##            dbg('len(value)', len(value), ' < self._masklength', self._masklength)
            # try to intelligently "pad out" the value to the right size:
            value = self._template[0:self._masklength - len(value)] + value
            if self._isFloat and value.find('.') == -1:
                value = value[1:]
##            dbg('padded value = "%s"' % value)

        # make SetValue behave the same as if you had typed the value in:
        try:
            value, replace_to = self._Paste(value, raise_on_invalid=True, just_return_value=True)
            if self._isFloat:
                self._isNeg = False     # (clear current assumptions)
                value = self._adjustFloat(value)
            elif self._isInt:
                self._isNeg = False     # (clear current assumptions)
                value = self._adjustInt(value)
            elif self._isDate and not self.IsValid(value) and self._4digityear:
                value = self._adjustDate(value, fixcentury=True)
        except ValueError:
            # If date, year might be 2 digits vs. 4; try adjusting it:
            if self._isDate and self._4digityear:
                dateparts = value.split(' ')
                dateparts[0] = self._adjustDate(dateparts[0], fixcentury=True)
                value = string.join(dateparts, ' ')
##                dbg('adjusted value: "%s"' % value)
                value, replace_to = self._Paste(value, raise_on_invalid=True, just_return_value=True)
            else:
##                dbg('exception thrown', indent=0)
                raise

        self._SetValue(value)   # note: to preserve similar capability, .SetValue()
                                # does not change IsModified()
####        dbg('queuing insertion after .SetValue', replace_to)
        # set selection to last char replaced by paste
        wx.CallAfter(self._SetInsertionPoint, replace_to)
        wx.CallAfter(self._SetSelection, replace_to, replace_to)
##        dbg(indent=0)


    def SetFont(self, *args, **kwargs):
        """ Set the font, then recalculate control size, if appropriate. """
        wx.TextCtrl.SetFont(self, *args, **kwargs)
        if self._autofit:
##            dbg('calculated size:', self._CalcSize())            
            self.SetClientSize(self._CalcSize())
            width = self.GetSize().width
            height = self.GetBestSize().height
##            dbg('setting client size to:', (width, height))
            self.SetBestFittingSize((width, height))


    def Clear(self):
        """ Blanks the current control value by replacing it with the default value."""
##        dbg("MaskedTextCtrl::Clear - value reset to default value (template)")
        if self._mask:
            self.ClearValue()
        else:
            wx.TextCtrl.Clear(self)    # else revert to base control behavior


    def _Refresh(self):
        """
        Allow mixin to refresh the base control with this function.
        REQUIRED by any class derived from MaskedEditMixin.
        """
##        dbg('MaskedTextCtrl::_Refresh', indent=1)
        wx.TextCtrl.Refresh(self)
##        dbg(indent=0)


    def Refresh(self):
        """
        This function redefines the externally accessible .Refresh() to
        validate the contents of the masked control as it refreshes.
        NOTE: this must be done in the class derived from the base wx control.
        """
##        dbg('MaskedTextCtrl::Refresh', indent=1)
        self._CheckValid()
        self._Refresh()
##        dbg(indent=0)


    def _IsEditable(self):
        """
        Allow mixin to determine if the base control is editable with this function.
        REQUIRED by any class derived from MaskedEditMixin.
        """
        return wx.TextCtrl.IsEditable(self)


    def Cut(self):
        """
        This function redefines the externally accessible .Cut to be
        a smart "erase" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """
        if self._mask:
            self._Cut()             # call the mixin's Cut method
        else:
            wx.TextCtrl.Cut(self)    # else revert to base control behavior


    def Paste(self):
        """
        This function redefines the externally accessible .Paste to be
        a smart "paste" of the text in question, so as not to corrupt the
        masked control.  NOTE: this must be done in the class derived
        from the base wx control.
        """
        if self._mask:
            self._Paste()                   # call the mixin's Paste method
        else:
            wx.TextCtrl.Paste(self, value)   # else revert to base control behavior


    def Undo(self):
        """
        This function defines the undo operation for the control. (The default
        undo is 1-deep.)
        """
        if self._mask:
            self._Undo()
        else:
            wx.TextCtrl.Undo(self)   # else revert to base control behavior


    def IsModified(self):
        """
        This function overrides the raw wx.TextCtrl method, because the
        masked edit mixin uses SetValue to change the value, which doesn't
        modify the state of this attribute.  So, the derived control keeps track
        on each keystroke to see if the value changes, and if so, it's been
        modified.
        """
        return wx.TextCtrl.IsModified(self) or self.modified


    def _CalcSize(self, size=None):
        """
        Calculate automatic size if allowed; use base mixin function.
        """
        return self._calcSize(size)


class TextCtrl( BaseMaskedTextCtrl, MaskedEditAccessorsMixin ):
    """
    The "user-visible" masked text control; it is identical to the
    BaseMaskedTextCtrl class it's derived from.
    (This extra level of inheritance allows us to add the generic
    set of masked edit parameters only to this class while allowing
    other classes to derive from the "base" masked text control,
    and provide a smaller set of valid accessor functions.)
    See BaseMaskedTextCtrl for available methods.
    """
    pass


class PreMaskedTextCtrl( BaseMaskedTextCtrl, MaskedEditAccessorsMixin ):
    """
    This class exists to support the use of XRC subclassing.
    """
    # This should really be wx.EVT_WINDOW_CREATE but it is not
    # currently delivered for native controls on all platforms, so
    # we'll use EVT_SIZE instead.  It should happen shortly after the
    # control is created as the control is set to its "best" size.
    _firstEventType = wx.EVT_SIZE

    def __init__(self):
        pre = wx.PreTextCtrl()
        self.PostCreate(pre)
        self.Bind(self._firstEventType, self.OnCreate)


    def OnCreate(self, evt):
        self.Unbind(self._firstEventType)
        self._PostInit()

__i=0
## CHANGELOG:
## ====================
##  Version 1.2
##  - Converted docstrings to reST format, added doc for ePyDoc.
##    removed debugging override functions.
##
##  Version 1.1
##  1. Added .SetFont() method that properly resizes control
##  2. Modified control to support construction via XRC mechanism.
