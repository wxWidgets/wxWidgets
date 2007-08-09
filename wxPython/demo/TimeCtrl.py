#
# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o presense of spin control causing probs (see spin ctrl demo for details)
#

import wx
import wx.lib.scrolledpanel    as scrolled
import wx.lib.masked           as masked

#----------------------------------------------------------------------

class TestPanel( scrolled.ScrolledPanel ):
    def __init__( self, parent, log ):

        scrolled.ScrolledPanel.__init__( self, parent, -1 )
        self.log = log

        box_label = wx.StaticBox( self, -1, "Change Controls through API" )
        buttonbox = wx.StaticBoxSizer( box_label, wx.HORIZONTAL )
        
        text1 = wx.StaticText( self, -1, "12-hour format:")
        self.time12 = masked.TimeCtrl( self, -1, name="12 hour control" )
        h = self.time12.GetSize().height
        spin1 = wx.SpinButton( self, -1, wx.DefaultPosition, (-1,h), wx.SP_VERTICAL )
        self.time12.BindSpinButton( spin1 )

        text2 = wx.StaticText( self, -1, "24-hour format:")
        spin2 = wx.SpinButton( self, -1, wx.DefaultPosition, (-1,h), wx.SP_VERTICAL )
        self.time24 = masked.TimeCtrl(
                        self, -1, name="24 hour control", fmt24hr=True,
                        spinButton = spin2
                        )

        text3 = wx.StaticText( self, -1, "No seconds\nor spin button:")
        self.spinless_ctrl = masked.TimeCtrl(
                                self, -1, name="spinless control",
                                display_seconds = False
                                )

        grid = wx.FlexGridSizer( 0, 2, 10, 5 )
        grid.Add( text1, 0, wx.ALIGN_RIGHT )
        hbox1 = wx.BoxSizer( wx.HORIZONTAL )
        hbox1.Add( self.time12, 0, wx.ALIGN_CENTRE )
        hbox1.Add( spin1, 0, wx.ALIGN_CENTRE )
        grid.Add( hbox1, 0, wx.LEFT )

        grid.Add( text2, 0, wx.ALIGN_RIGHT|wx.TOP|wx.BOTTOM )
        hbox2 = wx.BoxSizer( wx.HORIZONTAL )
        hbox2.Add( self.time24, 0, wx.ALIGN_CENTRE )
        hbox2.Add( spin2, 0, wx.ALIGN_CENTRE )
        grid.Add( hbox2, 0, wx.LEFT )

        grid.Add( text3, 0, wx.ALIGN_RIGHT|wx.TOP|wx.BOTTOM )
        grid.Add( self.spinless_ctrl, 0, wx.LEFT )


        buttonChange = wx.Button( self, -1, "Change Controls")
        self.radio12to24 = wx.RadioButton(
                            self, -1, "Copy 12-hour time to 24-hour control",
                            wx.DefaultPosition, wx.DefaultSize, wx.RB_GROUP
                            )

        self.radio24to12 = wx.RadioButton(
                            self, -1, "Copy 24-hour time to 12-hour control"
                            )

        self.radioWx = wx.RadioButton( self, -1, "Set controls to 'now' using wxDateTime")
        self.radioMx = wx.RadioButton( self, -1, "Set controls to 'now' using mxDateTime")

        radio_vbox = wx.BoxSizer( wx.VERTICAL )
        radio_vbox.Add( self.radio12to24, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        radio_vbox.Add( self.radio24to12, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        radio_vbox.Add( self.radioWx, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        radio_vbox.Add( self.radioMx, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        
        buttonbox.Add( buttonChange, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )
        buttonbox.Add( radio_vbox, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

        hbox = wx.BoxSizer( wx.HORIZONTAL )
        hbox.Add( grid, 0, wx.ALIGN_LEFT|wx.ALL, 15 )
        hbox.Add( buttonbox, 0, wx.ALIGN_RIGHT|wx.BOTTOM, 20 )


        box_label = wx.StaticBox( self, -1, "Bounds Control" )
        boundsbox = wx.StaticBoxSizer( box_label, wx.HORIZONTAL )
        self.set_bounds = wx.CheckBox( self, -1, "Set time bounds:" )

        minlabel = wx.StaticText( self, -1, "minimum time:" )
        self.min = masked.TimeCtrl( self, -1, name="min", display_seconds = False )
        self.min.Enable( False )

        maxlabel = wx.StaticText( self, -1, "maximum time:" )
        self.max = masked.TimeCtrl( self, -1, name="max", display_seconds = False )
        self.max.Enable( False )

        self.limit_check = wx.CheckBox( self, -1, "Limit control" )

        label = wx.StaticText( self, -1, "Resulting time control:" )
        self.target_ctrl = masked.TimeCtrl( self, -1, name="new" )

        grid2 = wx.FlexGridSizer( 0, 2, 0, 0 )
        grid2.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid2.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid2.Add( self.set_bounds, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid3 = wx.FlexGridSizer( 0, 2, 5, 5 )
        grid3.Add(minlabel, 0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL )
        grid3.Add( self.min, 0, wx.ALIGN_LEFT )
        grid3.Add(maxlabel, 0, wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL )
        grid3.Add( self.max, 0, wx.ALIGN_LEFT )
        grid2.Add(grid3, 0, wx.ALIGN_LEFT )

        grid2.Add( self.limit_check, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid2.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        grid2.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid2.Add( (20, 0), 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        grid2.Add( label, 0, wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
        grid2.Add( self.target_ctrl, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        boundsbox.Add(grid2, 0, wx.ALIGN_CENTER|wx.EXPAND|wx.ALL, 5)

        vbox = wx.BoxSizer( wx.VERTICAL )
        vbox.Add( (20, 20) )
        vbox.Add( hbox, 0, wx.ALIGN_LEFT|wx.ALL, 5)
        vbox.Add( boundsbox, 0, wx.ALIGN_LEFT|wx.ALL, 5 )


        outer_box = wx.BoxSizer( wx.VERTICAL )
        outer_box.Add( vbox, 0, wx.ALIGN_LEFT|wx.ALL, 5)


        # Turn on mxDateTime option only if we can import the module:
        try:
            from mx import DateTime
        except ImportError:
            self.radioMx.Enable( False )


        self.SetAutoLayout( True )
        self.SetSizer( outer_box )
        outer_box.Fit( self )
        self.SetupScrolling()

        self.Bind(wx.EVT_BUTTON, self.OnButtonClick, buttonChange )
        self.Bind(masked.EVT_TIMEUPDATE, self.OnTimeChange, self.time12 )
        self.Bind(masked.EVT_TIMEUPDATE, self.OnTimeChange, self.time24 )
        self.Bind(masked.EVT_TIMEUPDATE, self.OnTimeChange, self.spinless_ctrl )
        self.Bind(wx.EVT_CHECKBOX, self.OnBoundsCheck, self.set_bounds )
        self.Bind(wx.EVT_CHECKBOX, self.SetTargetMinMax, self.limit_check )
        self.Bind(masked.EVT_TIMEUPDATE, self.SetTargetMinMax, self.min )
        self.Bind(masked.EVT_TIMEUPDATE, self.SetTargetMinMax, self.max )
        self.Bind(masked.EVT_TIMEUPDATE, self.OnTimeChange, self.target_ctrl )


    def OnTimeChange( self, event ):
        timectrl = self.FindWindowById( event.GetId() )
        ib_str = [ "  (out of bounds)", "" ]

        self.log.write('%s time = %s%s\n' % ( timectrl.GetName(), timectrl.GetValue(), ib_str[ timectrl.IsInBounds() ] ) )


    def OnButtonClick( self, event ):
        if self.radio12to24.GetValue():
            self.time24.SetValue( self.time12.GetValue() )

        elif self.radio24to12.GetValue():
            self.time12.SetValue( self.time24.GetValue() )

        elif self.radioWx.GetValue():
            now = wx.DateTime_Now()
            self.time12.SetValue( now )
            # (demonstrates that G/SetValue returns/takes a wx.DateTime)
            self.time24.SetValue( self.time12.GetValue(as_wxDateTime=True) )

            # (demonstrates that G/SetValue returns/takes a wx.TimeSpan)
            self.spinless_ctrl.SetValue( self.time12.GetValue(as_wxTimeSpan=True) )

        elif self.radioMx.GetValue():
            from mx import DateTime
            now = DateTime.now()
            self.time12.SetValue( now )

            # (demonstrates that G/SetValue returns/takes a DateTime)
            self.time24.SetValue( self.time12.GetValue(as_mxDateTime=True) )

            # (demonstrates that G/SetValue returns/takes a DateTimeDelta)
            self.spinless_ctrl.SetValue( self.time12.GetValue(as_mxDateTimeDelta=True) )


    def OnBoundsCheck( self, event ):
        self.min.Enable( self.set_bounds.GetValue() )
        self.max.Enable( self.set_bounds.GetValue() )
        self.SetTargetMinMax()


    def SetTargetMinMax( self, event=None ):
        min = None
        max = None

        if self.set_bounds.GetValue():
            min = self.min.GetWxDateTime()
            max = self.max.GetWxDateTime()
        else:
            min, max = None, None

        cur_min, cur_max = self.target_ctrl.GetBounds()
        print cur_min, min
        if min and (min != cur_min): self.target_ctrl.SetMin( min )
        if max and (max != cur_max): self.target_ctrl.SetMax( max )

        self.target_ctrl.SetLimited( self.limit_check.GetValue() )

        if min != cur_min or max != cur_max:
            new_min, new_max = self.target_ctrl.GetBounds()

            if new_min and new_max:
                self.log.write( "current min, max:  (%s, %s)\n" % ( new_min.FormatTime(), new_max.FormatTime() ) )
            else:
                self.log.write( "current min, max:  (None, None)\n" )

#----------------------------------------------------------------------

def runTest( frame, nb, log ):
    win = TestPanel( nb, log )
    return win

#----------------------------------------------------------------------
import wx.lib.masked.timectrl as timectl
overview = """<html>
<PRE><FONT SIZE=-1>
""" + timectl.__doc__ + """
</FONT></PRE>"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

