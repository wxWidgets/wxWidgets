from wxPython.wx import *
from wxPython.lib.maskednumctrl import wxMaskedNumCtrl, EVT_MASKEDNUM
from wxPython.lib.maskednumctrl import __doc__ as overviewdoc
from wxPython.lib.maskededit import wxMaskedTextCtrl
import string, sys, traceback
#----------------------------------------------------------------------

class TestPanel( wxPanel ):
    def __init__( self, parent, log ):

        wxPanel.__init__( self, parent, -1 )
        self.log = log
        panel = wxPanel( self, -1 )

        header = wxStaticText(panel, -1, """\
This shows the various options for wxMaskedNumCtrl.
The controls at the top reconfigure the resulting control at the bottom.
""")
        header.SetForegroundColour( "Blue" )

        intlabel = wxStaticText( panel, -1, "Integer width:" )
        self.integerwidth = wxMaskedNumCtrl(
                                        panel, value=10,
                                        integerWidth=2,
                                        allowNegative=False)

        fraclabel = wxStaticText( panel, -1, "Fraction width:" )
        self.fractionwidth = wxMaskedNumCtrl(
                                        panel, value=0,
                                        integerWidth=2,
                                        allowNegative=False )

        groupcharlabel = wxStaticText( panel,-1, "Grouping char:" )
        self.groupchar = wxMaskedTextCtrl( panel, -1,
                                           value=',',
                                           mask='&',
                                           excludeChars = '-()',
                                           formatcodes='F',
                                           emptyInvalid=True,
                                           validRequired=True)

        decimalcharlabel = wxStaticText( panel,-1, "Decimal char:" )
        self.decimalchar = wxMaskedTextCtrl( panel, -1,
                                             value='.',
                                             mask='&',
                                             excludeChars = '-()',
                                             formatcodes='F',
                                             emptyInvalid=True,
                                             validRequired=True)

        self.set_min = wxCheckBox( panel, -1, "Set minimum value:" )
        # Create this wxMaskedNumCtrl using factory, to show how:
        self.min = wxMaskedNumCtrl( panel, integerWidth=5, fractionWidth=2 )
        self.min.Enable( False )

        self.set_max = wxCheckBox( panel, -1, "Set maximum value:" )
        self.max = wxMaskedNumCtrl( panel, integerWidth=5, fractionWidth=2 )
        self.max.Enable( False )


        self.limit_target = wxCheckBox( panel, -1, "Limit control" )
        self.allow_none = wxCheckBox( panel, -1, "Allow empty control" )
        self.group_digits = wxCheckBox( panel, -1, "Group digits" )
        self.group_digits.SetValue( True )
        self.allow_negative = wxCheckBox( panel, -1, "Allow negative values" )
        self.allow_negative.SetValue( True )
        self.use_parens = wxCheckBox( panel, -1, "Use parentheses" )
        self.select_on_entry = wxCheckBox( panel, -1, "Select on entry" )
        self.select_on_entry.SetValue( True )

        label = wxStaticText( panel, -1, "Resulting numeric control:" )
        font = label.GetFont()
        font.SetWeight(wxBOLD)
        label.SetFont(font)

        self.target_ctl = wxMaskedNumCtrl( panel, -1, name="target control" )

        label_numselect = wxStaticText( panel, -1, """\
Programmatically set the above
value entry ctrl:""")
        self.numselect = wxComboBox(panel, -1, choices = [ '0', '111', '222.22', '-3', '54321.666666666', '-1353.978',
                                                     '1234567', '-1234567', '123456789', '-123456789.1',
                                                     '1234567890.', '-9876543210.9' ])

        grid1 = wxFlexGridSizer( 0, 4, 0, 0 )
        grid1.Add( intlabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5)
        grid1.Add( self.integerwidth, 0, wxALIGN_LEFT|wxALL, 5 )

        grid1.Add( groupcharlabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5)
        grid1.Add( self.groupchar, 0, wxALIGN_LEFT|wxALL, 5 )

        grid1.Add( fraclabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        grid1.Add( self.fractionwidth, 0, wxALIGN_LEFT|wxALL, 5 )

        grid1.Add( decimalcharlabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5)
        grid1.Add( self.decimalchar, 0, wxALIGN_LEFT|wxALL, 5 )

        grid1.Add( self.set_min, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        grid1.Add( self.min, 0, wxALIGN_LEFT|wxALL, 5 )
        grid1.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)
        grid1.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)

        grid1.Add( self.set_max, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        grid1.Add( self.max, 0, wxALIGN_LEFT|wxALL, 5 )
        grid1.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)
        grid1.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)


        grid1.Add( self.limit_target, 0, wxALIGN_LEFT|wxALL, 5 )
        grid1.Add( self.allow_none, 0, wxALIGN_LEFT|wxALL, 5 )
        hbox1 = wxBoxSizer( wxHORIZONTAL )
        hbox1.Add( (17,5), 0, wxALIGN_LEFT|wxALL, 5)
        hbox1.Add( self.group_digits, 0, wxALIGN_LEFT|wxLEFT, 5 )
        grid1.Add( hbox1, 0, wxALIGN_LEFT|wxALL, 5)
        grid1.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)

        grid1.Add( self.allow_negative, 0, wxALIGN_LEFT|wxALL, 5 )
        grid1.Add( self.use_parens, 0, wxALIGN_LEFT|wxALL, 5 )
        hbox2 = wxBoxSizer( wxHORIZONTAL )
        hbox2.Add( (17,5), 0, wxALIGN_LEFT|wxALL, 5)
        hbox2.Add( self.select_on_entry, 0, wxALIGN_LEFT|wxLEFT, 5 )
        grid1.Add( hbox2, 0, wxALIGN_LEFT|wxALL, 5)
        grid1.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)


        grid2 = wxFlexGridSizer( 0, 2, 0, 0 )
        grid2.Add( label, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        grid2.Add( self.target_ctl, 0, wxALIGN_LEFT|wxALL, 5 )
        grid2.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)
        grid2.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)
        grid2.Add( label_numselect, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        grid2.Add( self.numselect, 0, wxALIGN_LEFT|wxALL, 5 )
        grid2.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)
        grid2.Add( (5,5), 0, wxALIGN_LEFT|wxALL, 5)
        grid2.AddGrowableCol(1)

        self.outer_box = wxBoxSizer( wxVERTICAL )
        self.outer_box.Add(header, 0, wxALIGN_LEFT|wxTOP|wxLEFT, 20)
        self.outer_box.Add( grid1, 0, wxALIGN_CENTRE|wxLEFT|wxBOTTOM|wxRIGHT, 20 )
        self.outer_box.Add( grid2, 0, wxALIGN_LEFT|wxALL, 20 )
        self.grid2 = grid2

        panel.SetAutoLayout( True )
        panel.SetSizer( self.outer_box )
        self.outer_box.Fit( panel )
        panel.Move( (50,10) )
        self.panel = panel

        EVT_MASKEDNUM( self, self.integerwidth.GetId(), self.OnSetIntWidth )
        EVT_MASKEDNUM( self, self.fractionwidth.GetId(), self.OnSetFractionWidth )
        EVT_TEXT( self, self.groupchar.GetId(), self.OnSetGroupChar )
        EVT_TEXT( self, self.decimalchar.GetId(), self.OnSetDecimalChar )

        EVT_CHECKBOX( self, self.set_min.GetId(), self.OnSetMin )
        EVT_CHECKBOX( self, self.set_max.GetId(), self.OnSetMax )
        EVT_MASKEDNUM( self, self.min.GetId(), self.SetTargetMinMax )
        EVT_MASKEDNUM( self, self.max.GetId(), self.SetTargetMinMax )

        EVT_CHECKBOX( self, self.limit_target.GetId(), self.SetTargetMinMax )
        EVT_CHECKBOX( self, self.allow_none.GetId(), self.OnSetAllowNone )
        EVT_CHECKBOX( self, self.group_digits.GetId(), self.OnSetGroupDigits )
        EVT_CHECKBOX( self, self.allow_negative.GetId(), self.OnSetAllowNegative )
        EVT_CHECKBOX( self, self.use_parens.GetId(), self.OnSetUseParens )
        EVT_CHECKBOX( self, self.select_on_entry.GetId(), self.OnSetSelectOnEntry )

        EVT_MASKEDNUM( self, self.target_ctl.GetId(), self.OnTargetChange )
        EVT_COMBOBOX( self, self.numselect.GetId(), self.OnNumberSelect )


    def OnSetIntWidth(self, event ):
        width = self.integerwidth.GetValue()
        if width < 1:
            self.log.write("integer width must be positive\n")
            self.integerwidth.SetForegroundColour(wxRED)
        else:
            self.integerwidth.SetForegroundColour(wxBLACK)
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
            self.groupchar.SetForegroundColour(wxRED)
        else:
            self.groupchar.SetForegroundColour(wxBLACK)
            self.log.write("setting group char to %s\n" % char)
            self.target_ctl.SetGroupChar( char )

    def OnSetDecimalChar( self, event ):
        char = self.decimalchar.GetValue()
        if self.target_ctl.GetGroupChar() == char:
            self.log.write("group and decimal chars must be different\n")
            self.decimalchar.SetForegroundColour(wxRED)
        else:
            self.decimalchar.SetForegroundColour(wxBLACK)
            self.log.write("setting decimal char to %s\n" % char)
            self.target_ctl.SetDecimalChar( char )


    def OnSetMin( self, event ):
        self.min.Enable( self.set_min.GetValue() )
        self.SetTargetMinMax()

    def OnSetMax( self, event ):
        self.max.Enable( self.set_max.GetValue() )
        self.SetTargetMinMax()


    def SetTargetMinMax( self, event=None ):
        min = max = None
        self.target_ctl.SetLimited( self.limit_target.GetValue() )

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
            self.min.SetParameters( signedForegroundColour=wxRED, foregroundColour=wxRED )
        else:
            self.min.SetParameters( signedForegroundColour=wxBLACK, foregroundColour=wxBLACK )
        self.min.Refresh()

        if max != cur_max and not self.target_ctl.SetMax( max ):
            if self.target_ctl.GetMax() is None and cur_min < max:
                self.log.write( "max (%d) won't fit in control -- bound not set\n" % max )
            else:
                self.log.write( "max (%d) < current min (%d) -- bound not set\n" % ( max, self.target_ctl.GetMin() ) )
            self.max.SetParameters( signedForegroundColour=wxRED, foregroundColour=wxRED )
        else:
            self.max.SetParameters( signedForegroundColour=wxBLACK, foregroundColour=wxBLACK )
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
overview = overviewdoc

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
