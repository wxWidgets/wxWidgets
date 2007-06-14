
import  string
import  sys
import  traceback

import  wx
from    wx.lib import masked

#----------------------------------------------------------------------

class TestPanel( wx.Panel ):
    def __init__( self, parent, log ):

        wx.Panel.__init__( self, parent, -1 )
        self.log = log
        panel = wx.Panel( self, -1 )

        header = wx.StaticText(panel, -1, """\
This shows the various options for masked.NumCtrl.
The controls at the top reconfigure the resulting control at the bottom.
""")
        header.SetForegroundColour( "Blue" )

        intlabel = wx.StaticText( panel, -1, "Integer width:" )
        self.integerwidth = masked.NumCtrl(
                                panel, value=10, integerWidth=2, allowNegative=False
                                )

        fraclabel = wx.StaticText( panel, -1, "Fraction width:" )
        self.fractionwidth = masked.NumCtrl(
                                panel, value=0, integerWidth=2, allowNegative=False
                                )

        groupcharlabel = wx.StaticText( panel,-1, "Grouping char:" )
        self.groupchar = masked.TextCtrl(
                                panel, -1, value=',', mask='*', includeChars = ' ', excludeChars = '-()0123456789',
                                formatcodes='F', emptyInvalid=False, validRequired=True
                                )

        decimalcharlabel = wx.StaticText( panel,-1, "Decimal char:" )
        self.decimalchar = masked.TextCtrl(
                                panel, -1, value='.', mask='&', excludeChars = '-()',
                                formatcodes='F', emptyInvalid=True, validRequired=True
                                )

        self.set_min = wx.CheckBox( panel, -1, "Set minimum value:" )
        # Create this masked.NumCtrl using factory, to show how:
        self.min = masked.Ctrl( panel, integerWidth=5, fractionWidth=2, controlType=masked.controlTypes.NUMBER )
        self.min.Enable( False )

        self.set_max = wx.CheckBox( panel, -1, "Set maximum value:" )
        self.max = masked.NumCtrl( panel, integerWidth=5, fractionWidth=2 )
        self.max.Enable( False )


        self.limit_target = wx.CheckBox( panel, -1, "Limit control" )
        self.limit_on_field_change = wx.CheckBox( panel, -1, "Limit on field change" )
        self.allow_none = wx.CheckBox( panel, -1, "Allow empty control" )
        self.group_digits = wx.CheckBox( panel, -1, "Group digits" )
        self.group_digits.SetValue( True )
        self.allow_negative = wx.CheckBox( panel, -1, "Allow negative values" )
        self.allow_negative.SetValue( True )
        self.use_parens = wx.CheckBox( panel, -1, "Use parentheses" )
        self.select_on_entry = wx.CheckBox( panel, -1, "Select on entry" )
        self.select_on_entry.SetValue( True )

        label = wx.StaticText( panel, -1, "Resulting numeric control:" )
        font = label.GetFont()
        font.SetWeight(wx.BOLD)
        label.SetFont(font)

        self.target_ctl = masked.NumCtrl( panel, -1, name="target control" )

        label_numselect = wx.StaticText( panel, -1, """\
Programmatically set the above
value entry ctrl:""")
        self.numselect = wx.ComboBox(panel, -1, choices = [ '0', '111', '222.22', '-3', '54321.666666666', '-1353.978',
                                                     '1234567', '-1234567', '123456789', '-123456789.1',
                                                     '1234567890.', '-9876543210.9' ])

        grid1 = wx.FlexGridSizer( 0, 4, 0, 0 )
        grid1.Add( intlabel, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)
        grid1.Add( self.integerwidth, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid1.Add( groupcharlabel, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)
        grid1.Add( self.groupchar, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid1.Add( fraclabel, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid1.Add( self.fractionwidth, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid1.Add( decimalcharlabel, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)
        grid1.Add( self.decimalchar, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid1.Add( self.set_min, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid1.Add( self.min, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)

        grid1.Add( self.set_max, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid1.Add( self.max, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)


        grid1.Add( self.limit_target, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid1.Add( self.limit_on_field_change, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        
        hbox1 = wx.BoxSizer( wx.HORIZONTAL )
        hbox1.Add( (17,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        hbox1.Add( self.allow_none, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid1.Add( hbox1, 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)

        grid1.Add( self.group_digits, 0, wx.ALIGN_LEFT|wx.LEFT, 5 )
        grid1.Add( self.allow_negative, 0, wx.ALIGN_LEFT|wx.ALL, 5 )        
        hbox2 = wx.BoxSizer( wx.HORIZONTAL )
        hbox2.Add( (17,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        hbox2.Add( self.use_parens, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid1.Add( hbox2, 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        
        grid1.Add( self.select_on_entry, 0, wx.ALIGN_LEFT|wx.LEFT, 5 )
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid1.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)

        grid2 = wx.FlexGridSizer( 0, 2, 0, 0 )
        grid2.Add( label, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid2.Add( self.target_ctl, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid2.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid2.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid2.Add( label_numselect, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid2.Add( self.numselect, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid2.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid2.Add( (5,5), 0, wx.ALIGN_LEFT|wx.ALL, 5)
        grid2.AddGrowableCol(1)

        self.outer_box = wx.BoxSizer( wx.VERTICAL )
        self.outer_box.Add(header, 0, wx.ALIGN_LEFT|wx.TOP|wx.LEFT, 20)
        self.outer_box.Add( grid1, 0, wx.ALIGN_CENTRE|wx.LEFT|wx.BOTTOM|wx.RIGHT, 20 )
        self.outer_box.Add( grid2, 0, wx.ALIGN_LEFT|wx.ALL, 20 )
        self.grid2 = grid2

        panel.SetAutoLayout( True )
        panel.SetSizer( self.outer_box )
        self.outer_box.Fit( panel )
        panel.Move( (50,10) )
        self.panel = panel

        self.Bind(masked.EVT_NUM, self.OnSetIntWidth, self.integerwidth )
        self.Bind(masked.EVT_NUM, self.OnSetFractionWidth, self.fractionwidth )
        self.Bind(wx.EVT_TEXT, self.OnSetGroupChar, self.groupchar )
        self.Bind(wx.EVT_TEXT, self.OnSetDecimalChar, self.decimalchar )

        self.Bind(wx.EVT_CHECKBOX, self.OnSetMin, self.set_min )
        self.Bind(wx.EVT_CHECKBOX, self.OnSetMax, self.set_max )
        self.Bind(masked.EVT_NUM, self.SetTargetMinMax, self.min )
        self.Bind(masked.EVT_NUM, self.SetTargetMinMax, self.max )

        self.Bind(wx.EVT_CHECKBOX, self.OnSetLimited, self.limit_target )
        self.Bind(wx.EVT_CHECKBOX, self.OnSetLimitOnFieldChange, self.limit_on_field_change )
        self.Bind(wx.EVT_CHECKBOX, self.OnSetAllowNone, self.allow_none )
        self.Bind(wx.EVT_CHECKBOX, self.OnSetGroupDigits, self.group_digits )
        self.Bind(wx.EVT_CHECKBOX, self.OnSetAllowNegative, self.allow_negative )
        self.Bind(wx.EVT_CHECKBOX, self.OnSetUseParens, self.use_parens )
        self.Bind(wx.EVT_CHECKBOX, self.OnSetSelectOnEntry, self.select_on_entry )

        self.Bind(masked.EVT_NUM, self.OnTargetChange, self.target_ctl )
        self.Bind(wx.EVT_COMBOBOX, self.OnNumberSelect, self.numselect )


    def OnSetIntWidth(self, event ):
        width = self.integerwidth.GetValue()

        if width < 1:
            self.log.write("integer width must be positive\n")
            self.integerwidth.SetForegroundColour(wx.RED)
        else:
            self.integerwidth.SetForegroundColour(wx.BLACK)
            self.log.write("setting integer width to %d\n" % width)
            self.target_ctl.SetParameters( integerWidth = width)
            # Now resize and fit the dialog as appropriate:
            self.grid2.SetItemMinSize(self.target_ctl, self.target_ctl.GetSize())
            self.outer_box.Fit( self.panel )
            self.outer_box.SetSizeHints( self.panel )


    def OnSetFractionWidth(self, event ):
        width = self.fractionwidth.GetValue()
        self.log.write("setting fraction width to %d\n" % width)
        self.target_ctl.SetParameters( fractionWidth = width)
        # Now resize and fit the dialog as appropriate:
        self.grid2.SetItemMinSize(self.target_ctl, self.target_ctl.GetSize())
        self.outer_box.Fit( self.panel )
        self.outer_box.SetSizeHints( self.panel )


    def OnSetGroupChar( self, event ):
        char = self.groupchar.GetValue()
        if self.target_ctl.GetDecimalChar() == char:
            self.log.write("group and decimal chars must be different\n")
            self.groupchar.SetForegroundColour(wx.RED)
        else:
            self.groupchar.SetForegroundColour(wx.BLACK)
            self.log.write("setting group char to %s\n" % char)
            self.target_ctl.SetGroupChar( char )

    def OnSetDecimalChar( self, event ):
        char = self.decimalchar.GetValue()
        if self.target_ctl.GetGroupChar() == char:
            self.log.write("group and decimal chars must be different\n")
            self.decimalchar.SetForegroundColour(wx.RED)
        else:
            self.decimalchar.SetForegroundColour(wx.BLACK)
            self.log.write("setting decimal char to %s\n" % char)
            self.target_ctl.SetDecimalChar( char )


    def OnSetMin( self, event ):
        self.min.Enable( self.set_min.GetValue() )
        self.SetTargetMinMax()

    def OnSetMax( self, event ):
        self.max.Enable( self.set_max.GetValue() )
        self.SetTargetMinMax()


    def OnSetLimited( self, event ):
        limited = self.limit_target.GetValue()
        self.target_ctl.SetLimited( limited )
        limit_on_field_change = self.limit_on_field_change.GetValue()
        if limited and limit_on_field_change:
            self.limit_on_field_change.SetValue(False)
            self.target_ctl.SetLimitOnFieldChange( False )
        self.SetTargetMinMax()


    def OnSetLimitOnFieldChange( self, event ):
        limit_on_field_change = self.limit_on_field_change.GetValue()
        self.target_ctl.SetLimitOnFieldChange( limit_on_field_change )
        limited = self.limit_target.GetValue()
        if limited and limit_on_field_change:
            self.limit_target.SetValue(False)
            self.target_ctl.SetLimited( False )

    def SetTargetMinMax( self, event=None ):
        min = max = None
        if self.set_min.GetValue():
            min = self.min.GetValue()
        if self.set_max.GetValue():
            max = self.max.GetValue()

        cur_min, cur_max = self.target_ctl.GetBounds()

        if min != cur_min and not self.target_ctl.SetMin( min ):
            if self.target_ctl.GetMax() is None and cur_max > min:
                self.log.write( "min (%d) won't fit in control -- bound not set\n" % min )
            else:
                self.log.write( "min (%d) > current max (%d) -- bound not set\n" % ( min, self.target_ctl.GetMax() ) )
            self.min.SetParameters( signedForegroundColour=wx.RED, foregroundColour=wx.RED )
        else:
            self.min.SetParameters( signedForegroundColour=wx.BLACK, foregroundColour=wx.BLACK )
        self.min.Refresh()

        if max != cur_max and not self.target_ctl.SetMax( max ):
            if self.target_ctl.GetMax() is None and cur_min < max:
                self.log.write( "max (%d) won't fit in control -- bound not set\n" % max )
            else:
                self.log.write( "max (%d) < current min (%d) -- bound not set\n" % ( max, self.target_ctl.GetMin() ) )
            self.max.SetParameters( signedForegroundColour=wx.RED, foregroundColour=wx.RED )
        else:
            self.max.SetParameters( signedForegroundColour=wx.BLACK, foregroundColour=wx.BLACK )
        self.max.Refresh()

        if min != cur_min or max != cur_max:
            new_min, new_max = self.target_ctl.GetBounds()
            self.log.write( "current min, max:  (%s, %s)\n" % ( str(new_min), str(new_max) ) )


    def OnSetAllowNone( self, event ):
        self.target_ctl.SetAllowNone( self.allow_none.GetValue() )


    def OnSetGroupDigits( self, event ):
        self.target_ctl.SetGroupDigits( self.group_digits.GetValue() )
        # Now resize and fit the dialog as appropriate:
        self.grid2.SetItemMinSize(self.target_ctl, self.target_ctl.GetSize())
        self.outer_box.Fit( self.panel )
        self.outer_box.SetSizeHints( self.panel )


    def OnSetAllowNegative( self, event ):
        if self.allow_negative.GetValue():
            self.use_parens.Enable(True)
            self.target_ctl.SetParameters(allowNegative=True,
                                          useParensForNegatives = self.use_parens.GetValue())
        else:
            self.target_ctl.SetAllowNegative(False)
        # Now resize and fit the dialog as appropriate:
        self.grid2.SetItemMinSize(self.target_ctl, self.target_ctl.GetSize())
        self.outer_box.Fit( self.panel )
        self.outer_box.SetSizeHints( self.panel )


    def OnSetUseParens( self, event ):
        self.target_ctl.SetUseParensForNegatives( self.use_parens.GetValue() )
        # Now resize and fit the dialog as appropriate:
        self.grid2.SetItemMinSize(self.target_ctl, self.target_ctl.GetSize())
        self.outer_box.Fit( self.panel )
        self.outer_box.SetSizeHints( self.panel )


    def OnSetSelectOnEntry( self, event ):
        self.target_ctl.SetSelectOnEntry( self.select_on_entry.GetValue() )


    def OnTargetChange( self, event ):
        ctl = event.GetEventObject()
        value = ctl.GetValue()
        ib_str = [ "  (out of bounds)", "" ]
        self.log.write( "value = %s (%s)%s\n" % ( repr(value), repr(type(value)), ib_str[ ctl.IsInBounds(value) ] ) )


    def OnNumberSelect( self, event ):
        value = event.GetString()
        if value:
            if value.find('.') != -1:
                numvalue = float(value)
            else:
                numvalue = long(value)
        else:
            numvalue = value   # try to clear the value again

        try:
            self.target_ctl.SetValue(numvalue)
        except:
            type, value, tb = sys.exc_info()
            for line in traceback.format_exception_only(type, value):
                self.log.write(line)


#----------------------------------------------------------------------

def runTest( frame, nb, log ):
    win = TestPanel( nb, log )
    return win

#----------------------------------------------------------------------
import wx.lib.masked.numctrl as mnum
overview = """<html>
<PRE><FONT SIZE=-1>
""" + mnum.__doc__ + """
</FONT></PRE>"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
