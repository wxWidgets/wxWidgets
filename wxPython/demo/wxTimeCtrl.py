from wxPython.wx import *
from wxPython.lib.timectrl import *
from wxPython.lib.timectrl import __doc__ as overviewdoc
from wxPython.lib.scrolledpanel import wxScrolledPanel

#----------------------------------------------------------------------

class TestPanel( wxScrolledPanel ):
    def __init__( self, parent, log ):

        wxScrolledPanel.__init__( self, parent, -1 )
        self.log = log


        text1 = wxStaticText( self, -1, "12-hour format:")
        self.time12 = wxTimeCtrl( self, -1, name="12 hour control" )
        spin1 = wxSpinButton( self, -1, wxDefaultPosition, wxSize(-1,20), 0 )
        self.time12.BindSpinButton( spin1 )

        text2 = wxStaticText( self, -1, "24-hour format:")
        spin2 = wxSpinButton( self, -1, wxDefaultPosition, wxSize(-1,20), 0 )
        self.time24 = wxTimeCtrl( self, -1, name="24 hour control", fmt24hr=True, spinButton = spin2 )

        text3 = wxStaticText( self, -1, "No seconds\nor spin button:")
        self.spinless_ctrl = wxTimeCtrl( self, -1, name="spinless control", display_seconds = False )

        grid = wxFlexGridSizer( 0, 2, 10, 5 )
        grid.Add( text1, 0, wxALIGN_RIGHT )
        hbox1 = wxBoxSizer( wxHORIZONTAL )
        hbox1.Add( self.time12, 0, wxALIGN_CENTRE )
        hbox1.Add( spin1, 0, wxALIGN_CENTRE )
        grid.Add( hbox1, 0, wxLEFT )

        grid.Add( text2, 0, wxALIGN_RIGHT|wxTOP|wxBOTTOM )
        hbox2 = wxBoxSizer( wxHORIZONTAL )
        hbox2.Add( self.time24, 0, wxALIGN_CENTRE )
        hbox2.Add( spin2, 0, wxALIGN_CENTRE )
        grid.Add( hbox2, 0, wxLEFT )

        grid.Add( text3, 0, wxALIGN_RIGHT|wxTOP|wxBOTTOM )
        grid.Add( self.spinless_ctrl, 0, wxLEFT )


        buttonChange = wxButton( self, -1, "Change Controls")
        self.radio12to24 = wxRadioButton( self, -1, "Copy 12-hour time to 24-hour control", wxDefaultPosition, wxDefaultSize, wxRB_GROUP )
        self.radio24to12 = wxRadioButton( self, -1, "Copy 24-hour time to 12-hour control")
        self.radioWx = wxRadioButton( self, -1, "Set controls to 'now' using wxDateTime")
        self.radioMx = wxRadioButton( self, -1, "Set controls to 'now' using mxDateTime")

        radio_vbox = wxBoxSizer( wxVERTICAL )
        radio_vbox.Add( self.radio12to24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        radio_vbox.Add( self.radio24to12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        radio_vbox.Add( self.radioWx, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        radio_vbox.Add( self.radioMx, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )

        box_label = wxStaticBox( self, -1, "Change Controls through API" )
        buttonbox = wxStaticBoxSizer( box_label, wxHORIZONTAL )
        buttonbox.Add( buttonChange, 0, wxALIGN_CENTRE|wxALL, 5 )
        buttonbox.Add( radio_vbox, 0, wxALIGN_CENTRE|wxALL, 5 )

        hbox = wxBoxSizer( wxHORIZONTAL )
        hbox.Add( grid, 0, wxALIGN_LEFT|wxALL, 15 )
        hbox.Add( buttonbox, 0, wxALIGN_RIGHT|wxBOTTOM, 20 )


        box_label = wxStaticBox( self, -1, "Bounds Control" )
        boundsbox = wxStaticBoxSizer( box_label, wxHORIZONTAL )
        self.set_bounds = wxCheckBox( self, -1, "Set time bounds:" )

        minlabel = wxStaticText( self, -1, "minimum time:" )
        self.min = wxTimeCtrl( self, -1, name="min", display_seconds = False )
        self.min.Enable( False )

        maxlabel = wxStaticText( self, -1, "maximum time:" )
        self.max = wxTimeCtrl( self, -1, name="max", display_seconds = False )
        self.max.Enable( False )

        self.limit_check = wxCheckBox( self, -1, "Limit control" )

        label = wxStaticText( self, -1, "Resulting time control:" )
        self.target_ctrl = wxTimeCtrl( self, -1, name="new" )

        grid2 = wxFlexGridSizer( 0, 2, 0, 0 )
        grid2.Add( 20, 0, 0, wxALIGN_LEFT|wxALL, 5 )
        grid2.Add( 20, 0, 0, wxALIGN_LEFT|wxALL, 5 )

        grid2.Add( self.set_bounds, 0, wxALIGN_LEFT|wxALL, 5 )
        grid3 = wxFlexGridSizer( 0, 2, 5, 5 )
        grid3.Add(minlabel, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL )
        grid3.Add( self.min, 0, wxALIGN_LEFT )
        grid3.Add(maxlabel, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL )
        grid3.Add( self.max, 0, wxALIGN_LEFT )
        grid2.Add(grid3, 0, wxALIGN_LEFT )

        grid2.Add( self.limit_check, 0, wxALIGN_LEFT|wxALL, 5 )
        grid2.Add( 20, 0, 0, wxALIGN_LEFT|wxALL, 5 )

        grid2.Add( 20, 0, 0, wxALIGN_LEFT|wxALL, 5 )
        grid2.Add( 20, 0, 0, wxALIGN_LEFT|wxALL, 5 )
        grid2.Add( label, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        grid2.Add( self.target_ctrl, 0, wxALIGN_LEFT|wxALL, 5 )
        boundsbox.Add(grid2, 0, wxALIGN_CENTER|wxEXPAND|wxALL, 5)

        vbox = wxBoxSizer( wxVERTICAL )
        vbox.AddSpacer(20, 20)
        vbox.Add( hbox, 0, wxALIGN_LEFT|wxALL, 5)
        vbox.Add( boundsbox, 0, wxALIGN_LEFT|wxALL, 5 )


        outer_box = wxBoxSizer( wxVERTICAL )
        outer_box.Add( vbox, 0, wxALIGN_LEFT|wxALL, 5)


        # Turn on mxDateTime option only if we can import the module:
        try:
            from mx import DateTime
        except ImportError:
            self.radioMx.Enable( False )


        self.SetAutoLayout( True )
        self.SetSizer( outer_box )
        outer_box.Fit( self )
        self.SetupScrolling()

        EVT_BUTTON( self, buttonChange.GetId(), self.OnButtonClick )
        EVT_TIMEUPDATE( self, self.time12.GetId(), self.OnTimeChange )
        EVT_TIMEUPDATE( self, self.time24.GetId(), self.OnTimeChange )
        EVT_TIMEUPDATE( self, self.spinless_ctrl.GetId(), self.OnTimeChange )


        EVT_CHECKBOX( self, self.set_bounds.GetId(), self.OnBoundsCheck )
        EVT_CHECKBOX( self, self.limit_check.GetId(), self.SetTargetMinMax )
        EVT_TIMEUPDATE( self, self.min.GetId(), self.SetTargetMinMax )
        EVT_TIMEUPDATE( self, self.max.GetId(), self.SetTargetMinMax )
        EVT_TIMEUPDATE( self, self.target_ctrl.GetId(), self.OnTimeChange )



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
            now = wxDateTime_Now()
            self.time12.SetValue( now )
            # (demonstrates that G/SetValue returns/takes a wxDateTime)
            self.time24.SetValue( self.time12.GetValue(as_wxDateTime=True) )

            # (demonstrates that G/SetValue returns/takes a wxTimeSpan)
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
        min = max = None

        if self.set_bounds.GetValue():
            min = self.min.GetWxDateTime()
            max = self.max.GetWxDateTime()
        else:
            min, max = None, None

        cur_min, cur_max = self.target_ctrl.GetBounds()

        if min != cur_min: self.target_ctrl.SetMin( min )
        if max != cur_max: self.target_ctrl.SetMax( max )

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

overview = overviewdoc

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

