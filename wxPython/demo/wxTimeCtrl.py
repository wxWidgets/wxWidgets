from wxPython.wx import *
from wxPython.lib.timectrl import *

#----------------------------------------------------------------------

class TestPanel( wxPanel ):
    def __init__( self, parent, log ):

        wxPanel.__init__( self, parent, -1 )
        self.log = log
        panel = wxPanel( self, -1 )

        grid = wxFlexGridSizer( 0, 2, 20, 0 )

        text1 = wxStaticText( panel, 10, "A 12-hour format wxTimeCtrl:")
        self.time12 = wxTimeCtrl( panel, 20, name="12 hour control" )
        spin1 = wxSpinButton( panel, 30, wxDefaultPosition, wxSize(-1,20), 0 )
        self.time12.BindSpinButton( spin1 )

        grid.AddWindow( text1, 0, wxALIGN_RIGHT, 5 )
        hbox1 = wxBoxSizer( wxHORIZONTAL )
        hbox1.AddWindow( self.time12, 0, wxALIGN_CENTRE, 5 )
        hbox1.AddWindow( spin1, 0, wxALIGN_CENTRE, 5 )
        grid.AddSizer( hbox1, 0, wxLEFT, 5 )


        text2 = wxStaticText( panel, 40, "A 24-hour format wxTimeCtrl:")
        self.time24 = wxTimeCtrl( panel, 50, fmt24hr=True, name="24 hour control" )
        spin2 = wxSpinButton( panel, 60, wxDefaultPosition, wxSize(-1,20), 0 )
        self.time24.BindSpinButton( spin2 )

        grid.AddWindow( text2, 0, wxALIGN_RIGHT|wxTOP|wxBOTTOM, 5 )
        hbox2 = wxBoxSizer( wxHORIZONTAL )
        hbox2.AddWindow( self.time24, 0, wxALIGN_CENTRE, 5 )
        hbox2.AddWindow( spin2, 0, wxALIGN_CENTRE, 5 )
        grid.AddSizer( hbox2, 0, wxLEFT, 5 )


        text3 = wxStaticText( panel, 70, "A wxTimeCtrl without a spin button:")
        self.spinless_ctrl = wxTimeCtrl( panel, 80, name="spinless control" )

        grid.AddWindow( text3, 0, wxALIGN_RIGHT|wxTOP|wxBOTTOM, 5 )
        grid.AddWindow( self.spinless_ctrl, 0, wxLEFT, 5 )


        buttonChange = wxButton( panel, 100, "Change Controls")
        self.radio12to24 = wxRadioButton( panel, 110, "Copy 12-hour time to 24-hour control", wxDefaultPosition, wxDefaultSize, wxRB_GROUP )
        self.radio24to12 = wxRadioButton( panel, 120, "Copy 24-hour time to 12-hour control")
        self.radioWx = wxRadioButton( panel, 130, "Set controls to 'now' using wxDateTime")
        self.radioMx = wxRadioButton( panel, 140, "Set controls to 'now' using mxDateTime")

        radio_vbox = wxBoxSizer( wxVERTICAL )
        radio_vbox.AddWindow( self.radio12to24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        radio_vbox.AddWindow( self.radio24to12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        radio_vbox.AddWindow( self.radioWx, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )
        radio_vbox.AddWindow( self.radioMx, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 )

        box_label = wxStaticBox( panel, 90, "Change Controls through API" )
        buttonbox = wxStaticBoxSizer( box_label, wxHORIZONTAL )
        buttonbox.AddWindow( buttonChange, 0, wxALIGN_CENTRE|wxALL, 5 )
        buttonbox.AddSizer( radio_vbox, 0, wxALIGN_CENTRE|wxALL, 5 )

        outer_box = wxBoxSizer( wxVERTICAL )
        outer_box.AddSizer( grid, 0, wxALIGN_CENTRE|wxBOTTOM, 20 )
        outer_box.AddSizer( buttonbox, 0, wxALIGN_CENTRE|wxALL, 5 )


        # Turn on mxDateTime option only if we can import the module:
        try:
            from mx import DateTime
        except ImportError:
            self.radioMx.Enable( False )


        panel.SetAutoLayout( True )
        panel.SetSizer( outer_box )
        outer_box.Fit( panel )
        panel.Move( (50,50) )
        self.panel = panel


        EVT_TIMEUPDATE( self, self.time12.GetId(), self.OnTimeChange )
        EVT_TIMEUPDATE( self, self.time24.GetId(), self.OnTimeChange )
        EVT_TIMEUPDATE( self, self.spinless_ctrl.GetId(), self.OnTimeChange )

        EVT_BUTTON( self, buttonChange.GetId(), self.OnButtonClick )


    def OnTimeChange( self, event ):
        timectrl = self.panel.FindWindowById( event.GetId() )
        self.log.write('%s time = %s\n' % ( timectrl.GetName(), timectrl.GetValue() ) )

    def OnButtonClick( self, event ):
        if self.radio12to24.GetValue():
            self.time24.SetValue( self.time12.GetValue() )

        elif self.radio24to12.GetValue():
            self.time12.SetValue( self.time24.GetValue() )

        elif self.radioWx.GetValue():
            now = wxDateTime_Now()
            self.time12.SetWxDateTime( now )
            self.time24.SetWxDateTime( now )
            self.spinless_ctrl.SetWxDateTime( now )

        elif self.radioMx.GetValue():
            from mx import DateTime
            now = DateTime.now()
            self.time12.SetMxDateTime( now )
            self.time24.SetMxDateTime( now )
            self.spinless_ctrl.SetMxDateTime( now )

#----------------------------------------------------------------------

def runTest( frame, nb, log ):
    win = TestPanel( nb, log )
    return win

#----------------------------------------------------------------------

overview = """<html><body>
<P>
<B>wxTimeCtrl</B> provides a multi-cell control that allows manipulation of a time
value.  It supports 12 or 24 hour format, and you can use wxDateTime or mxDateTime
to get/set values from the control.
<P>
Left/right/tab keys to switch cells within a wxTimeCtrl, and the up/down arrows act
like a spin control.  wxTimeCtrl also allows for an actual spin button to be attached
to the control, so that it acts like the up/down arrow keys.
<P>
The <B>!</B> or <B>c</B> key sets the value of the control to <B><I>now.</I></B>
<P>
Here's the API for wxTimeCtrl:
<DL><PRE>
    <B>wxTimeCtrl</B>(
         parent, id = -1,
         <B>value</B> = '12:00:00 AM',
         pos = wxDefaultPosition,
         size = wxDefaultSize,
         <B>fmt24hr</B> = False,
         <B>spinButton</B> = None,
         <B>style</B> = wxTE_PROCESS_TAB,
         name = "time")
</PRE>
<UL>
    <DT><B>value</B>
    <DD>If no initial value is set, the default will be midnight; if an illegal string
    is specified, a ValueError will result.  (You can always later set the initial time
    with SetValue() after instantiation of the control.)
    <DL><B>size</B>
    <DD>The size of the control will be automatically adjusted for 12/24 hour format
    if wxDefaultSize is specified.
    <BR>
    <DT><B>fmt24hr</B>
    <DD>If True, control will display time in 24 hour time format; if False, it will
    use 12 hour AM/PM format.  SetValue() will adjust values accordingly for the
    control, based on the format specified.
    <BR>
    <DT><B>spinButton</B>
    <DD>If specified, this button's events will be bound to the behavior of the
    wxTimeCtrl, working like up/down cursor key events.  (See BindSpinButton.)
    <BR>
    <DT><B>style</B>
    <DD>By default, wxTimeCtrl will process TAB events, by allowing tab to the
    different cells within the control.
    </DL>
</UL>
<BR>
<BR>
<DT><B>SetValue(time_string)</B>
<DD>Sets the value of the control to a particular time, given a valid time string;
raises ValueError on invalid value
<BR>
<DT><B>GetValue()</B>
<DD>Retrieves the string value of the time from the control
<BR>
<DT><B>SetWxDateTime(wxDateTime)</B>
<DD>Uses the time portion of a wxDateTime to construct a value for the control.
<BR>
<DT><B>GetWxDateTime()</B>
<DD>Retrieves the value of the control, and applies it to the wxDateTimeFromHMS()
constructor, and returns the resulting value.  (This returns the date portion as
"today".)
<BR>
<DT><B>SetMxDateTime(mxDateTime)</B>
<DD>Uses the time portion of an mxDateTime to construct a value for the control.
<EM>NOTE:</EM> This imports mx.DateTime at runtime only if this or the Get function
is called.
<BR>
<DT><B>GetMxDateTime()</B>
<DD>Retrieves the value of the control and applies it to the DateTime.Time()
constructor,  and returns the resulting value.  (mxDateTime is smart enough to
know this is just a time value.)
<BR>
<DT><B>BindSpinButton(wxSpinBtton)</B>
<DD>Binds an externally created spin button to the control, so that up/down spin
events change the active cell or selection in the control (in addition to the
up/down cursor keys.)  (This is primarily to allow you to create a "standard"
interface to time controls, as seen in Windows.)
<BR>
<DT><B>EVT_TIMEUPDATE(win, id, func)</B>
<DD>func is fired whenever the value of the control changes.
</DL>
</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

