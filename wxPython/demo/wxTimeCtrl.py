from wxPython.wx import *
from wxPython.lib.timectrl import *

import string

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        panel = wxPanel(self, -1)
        grid = wxFlexGridSizer( 0, 2, 20, 0 )

        text1 = wxStaticText( panel, 10, "A 12-hour format wxTimeCtrl:", wxDefaultPosition, wxDefaultSize, 0 )
        grid.AddWindow( text1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 )

        hsizer1 = wxBoxSizer( wxHORIZONTAL )
        self.time12 = wxTimeCtrl(panel, 20, name="12 hour time")
        hsizer1.AddWindow( self.time12, 0, wxALIGN_CENTRE, 5 )
        spin1 = wxSpinButton( panel, 30, wxDefaultPosition, wxSize(-1,20), 0 )
        self.time12.BindSpinButton(spin1)
        hsizer1.AddWindow( spin1, 0, wxALIGN_CENTRE, 5 )
        grid.AddSizer( hsizer1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 )

        text2 = wxStaticText( panel, 40, "A 24-hour format wxTimeCtrl:", wxDefaultPosition, wxDefaultSize, 0 )
        grid.AddWindow( text2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5 )

        hsizer2 = wxBoxSizer( wxHORIZONTAL )
        self.time24 = wxTimeCtrl(panel, 50, fmt24hr=true, name="24 hour time")
        hsizer2.AddWindow( self.time24, 0, wxALIGN_CENTRE, 5 )
        spin2 = wxSpinButton( panel, 60, wxDefaultPosition, wxSize(-1,20), 0 )
        self.time24.BindSpinButton(spin2)
        hsizer2.AddWindow( spin2, 0, wxALIGN_CENTRE, 5 )
        grid.AddSizer( hsizer2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 )

        panel.SetAutoLayout(true)
        panel.SetSizer(grid)
        grid.Fit(panel)
        panel.Move((50,50))
        self.panel = panel

        EVT_TIMEUPDATE(self, self.time12.GetId(), self.OnTimeChange)
        EVT_TIMEUPDATE(self, self.time24.GetId(), self.OnTimeChange)


    def OnTimeChange(self, event):
        timectrl = self.panel.FindWindowById(event.GetId())
        self.log.write('%s = %s\n' % (
                timectrl.GetName(), timectrl.GetValue() ) )


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------

# It's nice to be able to use HTML here, but line breaks in the triple quoted string
# cause the resulting output to look funny, as they seem to be interpreted by the
# parser...

overview = """<html><body>
<P>
<B>wxTimeCtrl</B> provides a multi-cell control that allows manipulation of a time value.  It supports 12 or 24 hour format, and you can use wxDateTime or mxDateTimet o get/set values from the control.
<P>
Left/right/tab keys to switch cells within a wxTimeCtrl, and the up/down arrows act like a spin control.  wxTimeCtrl also allows for an actual spin button to be attached to the control, so that it acts like the up/down arrow keys.
<P>
Here's the interface for wxTimeCtrl:
<DL>

<PRE>
<B>wxTimeCtrl</B>(parent, id = -1,
         <B>value</B> = '12:00:00 AM',
         pos = wxDefaultPosition,
         size = wxDefaultSize,
         <B>fmt24hr</B> = false,
         <B>spinButton</B> = None,
         <B>style</B> = wxTE_PROCESS_TAB,
         name = "time")
</PRE>
<UL>
    <DT><B>value</B><DD>If no initial value is set, the default will be midnight; if an illegal string is specified, a ValueError will result.  (You can always later set the initial time with SetValue() after instantiation of the control.)
    <DL><B>size</B><DD>The size of the control will be automatically adjusted for 12/24 hour format if wxDefaultSize is specified.
    <DT><B>fmt24hr</B><DD>If true, control will display time in 24 hour time format; if false, it will use 12 hour AM/PM format.  SetValue() will adjust values accordingly for the control, based on the format specified.
    <DT><B>spinButton</B><DD>If specified, this button's events will be bound to the behavior of the wxTimeCtrl, working like up/down cursor key events.  (See BindSpinButton.)
    <DT><B>style</B><DD>By default, wxTimeCtrl will process TAB events, by allowing tab to the different cells within the control.
    </DL>
</UL>
<DT><B>SetValue(time_string)</B><DD>Sets the value of the control to a particular time, given a valid time string; raises ValueError on invalid value
<DT><B>GetValue()</B><DD>Retrieves the string value of the time from the control
<BR>
<DT><B>SetWxDateTime(wxDateTime)</B><DD>Uses the time portion of a wxDateTime to construct a value for the control.
<DT><B>GetWxDateTime()</B><DD>Retrieves the value of the control, and applies it to the wxDateTimeFromHMS() constructor, and returns the resulting value.  (This returns the date portion as "today".)
<BR>
<DT><B>SetMxDateTime(mxDateTime)</B><DD>Uses the time portion of an mxDateTime to construct a value for the control.  <EM>NOTE:</EM> This imports mx.DateTime at runtime only if this or the Get function is called.
<DT><B>GetMxDateTime()</B><DD>Retrieves the value of the control and applies it to the DateTime.Time() constructor,  and returns the resulting value.  (mxDateTime is smart enough to know this is just a time value.)
<BR>
<DT><B>BindSpinButton(wxSpinBtton)</B><DD>Binds an externally created spin button to the control, so that up/down spin events change the active cell or selection in the control (in addition to the up/down cursor keys.)  (This is primarily to allow you to create a "standard" interface to time controls, as seen in Windows.)
<BR>
<DT><B>EVT_TIMEUPDATE(win, id, func)</B><DD>func is fired whenever the value of the control changes.
</DL>
</body></html>
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])



