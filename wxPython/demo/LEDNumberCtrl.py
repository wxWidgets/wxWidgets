
import  time

import  wx
import  wx.gizmos   as  gizmos

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        led = gizmos.LEDNumberCtrl(self, -1, (25,25), (280, 50))
        led.SetValue("01234")

        led = gizmos.LEDNumberCtrl(self, -1, (25,100), (280, 50))
        led.SetValue("56789")
        led.SetAlignment(gizmos.LED_ALIGN_RIGHT)
        led.SetDrawFaded(False)

        led = gizmos.LEDNumberCtrl(self, -1, (25,175), (280, 50),
                              gizmos.LED_ALIGN_CENTER)# | gizmos.LED_DRAW_FADED)
        self.clock = led
        self.OnTimer(None)

        self.timer = wx.Timer(self)
        self.timer.Start(1000)
        self.Bind(wx.EVT_TIMER, self.OnTimer)


    def OnTimer(self, evt):
        t = time.localtime(time.time())
        st = time.strftime("%I-%M-%S", t)
        self.clock.SetValue(st)


    def ShutdownDemo(self):
        self.timer.Stop()
        del self.timer


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------

overview = """\
<html>
<body>
<font size=-1>The following was gleaned as best I could from the wxWindows
source, which was a bit reluctant to reveal its secrets. My appologies if
I missed anything - jmg</font>
<p>
<code><b>LEDNumberCtrl</b>( parent, id=-1, pos=wx.DefaultPosition, 
size=wx.DefaultSize, style=LED_ALIGN_LEFT | LED_DRAW_FADED)</code>

<p>This is a control that simulates an LED clock display. It only accepts 
numeric input. 

<p><b>Styles</b>

<p><dl>
<dt><b>LED_ALIGN_LEFT</b>
<dd>Align to the left.

<dt><b>LED_ALIGN_RIGHT</b>
<dd>Align to the right.

<dt><b>LED_ALIGN_CENTER</b>
<dd>Center on display.

<dt><b>LED_DRAW_FADED</b>
<dd>Not implemented.

</dl>

<p><b>Methods</b> (and best guesses at what they do)

<p><dl>
<dt><b>GetAlignment()</b>
<dd>Returns the alignment attribute for the control.

<dt><b>GetDrawFaded()</b>
<dd>Returns the DrawFaded attribute for the control.

<dt><b>GetValue()</b>
<dd>Returns the current value of the control.

<dt><b>SetAlignment(alignment)</b>
<dd>Set the alignment attribute for the control.

<dt><b>SetDrawFaded(value)</b>
<dd>Set the DrawFaded attribute for the control.

<dt><b>SetValue(number)</b>
<dd>Set the value for the control. Only numeric values are accepted.

</dl>

<p>Additionally, several methods of wx.Window are available as well.

</body>
</html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

