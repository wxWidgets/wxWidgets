from wxPython.wx import *
#----------------------------------------------------------------------

class TestPanel( wxPanel ):
    def __init__( self, parent, log ):

        wxPanel.__init__( self, parent, -1 )
        self.log = log
        panel = wxPanel( self, -1 )

        # 1st group of controls:
        self.group1_ctrls = []
        radio1 = wxRadioButton( panel, -1, " Radio1 ", style = wxRB_GROUP )
        text1 = wxTextCtrl( panel, -1, "" )
        radio2 = wxRadioButton( panel, -1, " Radio2 " )
        text2 = wxTextCtrl( panel, -1, "" )
        radio3 = wxRadioButton( panel, -1, " Radio3 " )
        text3 = wxTextCtrl( panel, -1, "" )
        self.group1_ctrls.append((radio1, text1))
        self.group1_ctrls.append((radio2, text2))
        self.group1_ctrls.append((radio3, text3))

        # 2nd group of controls:
        self.group2_ctrls = []
        radio4 = wxRadioButton( panel, -1, " Radio1 ", style = wxRB_GROUP )
        text4 = wxTextCtrl( panel, -1, "" )
        radio5 = wxRadioButton( panel, -1, " Radio2 " )
        text5 = wxTextCtrl( panel, -1, "" )
        radio6 = wxRadioButton( panel, -1, " Radio3 " )
        text6 = wxTextCtrl( panel, -1, "" )
        self.group2_ctrls.append((radio4, text4))
        self.group2_ctrls.append((radio5, text5))
        self.group2_ctrls.append((radio6, text6))

        # Layout controls on panel:
        vs = wxBoxSizer( wxVERTICAL )

        box1_title = wxStaticBox( panel, -1, "Group 1" )
        box1 = wxStaticBoxSizer( box1_title, wxVERTICAL )
        grid1 = wxFlexGridSizer( 0, 2, 0, 0 )
        for radio, text in self.group1_ctrls:
            grid1.AddWindow( radio, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxTOP, 5 )
            grid1.AddWindow( text, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxTOP, 5 )
        box1.AddSizer( grid1, 0, wxALIGN_CENTRE|wxALL, 5 )
        vs.AddSizer( box1, 0, wxALIGN_CENTRE|wxALL, 5 )

        box2_title = wxStaticBox( panel, -1, "Group 2" )
        box2 = wxStaticBoxSizer( box2_title, wxVERTICAL )
        grid2 = wxFlexGridSizer( 0, 2, 0, 0 )
        for radio, text in self.group2_ctrls:
            grid2.AddWindow( radio, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxTOP, 5 )
            grid2.AddWindow( text, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxTOP, 5 )
        box2.AddSizer( grid2, 0, wxALIGN_CENTRE|wxALL, 5 )
        vs.AddSizer( box2, 0, wxALIGN_CENTRE|wxALL, 5 )

        panel.SetSizer( vs )
        vs.Fit( panel )
        panel.Move( (50,50) )
        self.panel = panel

        # Setup event handling and initial state for controls:
        for radio, text in self.group1_ctrls:
            EVT_RADIOBUTTON( self, radio.GetId(), self.OnGroup1Select )

        for radio, text in self.group2_ctrls:
            EVT_RADIOBUTTON( self, radio.GetId(), self.OnGroup2Select )

        for radio, text in self.group1_ctrls + self.group2_ctrls:
            radio.SetValue(0)
            text.Enable(False)


    def OnGroup1Select( self, event ):
        radio_selected = event.GetEventObject()
        self.log.write('Group1 %s selected\n' % radio_selected.GetLabel() )
        for radio, text in self.group1_ctrls:
            if radio is radio_selected:
                text.Enable(True)
            else:
                text.Enable(False)

    def OnGroup2Select( self, event ):
        radio_selected = event.GetEventObject()
        self.log.write('Group2 %s selected\n' % radio_selected.GetLabel() )
        for radio, text in self.group2_ctrls:
            if radio is radio_selected:
                text.Enable(True)
            else:
                text.Enable(False)

#----------------------------------------------------------------------

def runTest( frame, nb, log ):
    win = TestPanel( nb, log )
    return win

#----------------------------------------------------------------------


overview = """\
<html><body>
<P>
This demo shows how individual radio buttons can be used to build
more complicated selection mechanisms...
<P>
It uses 2 groups of wxRadioButtons, where the groups are defined by
instantiation.  When a wxRadioButton is created with the <I>wxRB_GROUP</I>
style, all subsequent wxRadioButtons created without it are implicitly
added to that group by the framework.
</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

