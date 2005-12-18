
import  wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, ID, log):
        wx.Panel.__init__(self, parent, ID)
        self.log = log

        self.process = None
        self.Bind(wx.EVT_IDLE, self.OnIdle)

        # We can either derive from wx.Process and override OnTerminate
        # or we can let wx.Process send this window an event that is
        # caught in the normal way...
        self.Bind(wx.EVT_END_PROCESS, self.OnProcessEnded)


        # Make the controls
        prompt = wx.StaticText(self, -1, 'Command line:')
        self.cmd = wx.TextCtrl(self, -1, 'python -u data/echo.py')
        self.exBtn = wx.Button(self, -1, 'Execute')

        self.out = wx.TextCtrl(self, -1, '',
                               style=wx.TE_MULTILINE|wx.TE_READONLY|wx.TE_RICH2)

        self.inp = wx.TextCtrl(self, -1, '', style=wx.TE_PROCESS_ENTER)
        self.sndBtn = wx.Button(self, -1, 'Send')
        self.termBtn = wx.Button(self, -1, 'Close Stream')
        self.inp.Enable(False)
        self.sndBtn.Enable(False)
        self.termBtn.Enable(False)

        # Hook up the events
        self.Bind(wx.EVT_BUTTON, self.OnExecuteBtn, self.exBtn)
        self.Bind(wx.EVT_BUTTON, self.OnSendText, self.sndBtn)
        self.Bind(wx.EVT_BUTTON, self.OnCloseStream, self.termBtn)
        self.Bind(wx.EVT_TEXT_ENTER, self.OnSendText, self.inp)


        # Do the layout
        box1 = wx.BoxSizer(wx.HORIZONTAL)
        box1.Add(prompt, 0, wx.ALIGN_CENTER)
        box1.Add(self.cmd, 1, wx.ALIGN_CENTER|wx.LEFT|wx.RIGHT, 5)
        box1.Add(self.exBtn, 0)

        box2 = wx.BoxSizer(wx.HORIZONTAL)
        box2.Add(self.inp, 1, wx.ALIGN_CENTER)
        box2.Add(self.sndBtn, 0, wx.LEFT, 5)
        box2.Add(self.termBtn, 0, wx.LEFT, 5)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(box1, 0, wx.EXPAND|wx.ALL, 10)
        sizer.Add(self.out, 1, wx.EXPAND|wx.ALL, 10)
        sizer.Add(box2, 0, wx.EXPAND|wx.ALL, 10)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)


    def __del__(self):
        if self.process is not None:
            self.process.Detach()
            self.process.CloseOutput()
            self.process = None


    def OnExecuteBtn(self, evt):
        cmd = self.cmd.GetValue()

        self.process = wx.Process(self)
        self.process.Redirect();
        pid = wx.Execute(cmd, wx.EXEC_ASYNC, self.process)
        self.log.write('OnExecuteBtn: "%s" pid: %s\n' % (cmd, pid))

        self.inp.Enable(True)
        self.sndBtn.Enable(True)
        self.termBtn.Enable(True)
        self.cmd.Enable(False)
        self.exBtn.Enable(False)
        self.inp.SetFocus()


    def OnSendText(self, evt):
        text = self.inp.GetValue()
        self.inp.SetValue('')
        self.log.write('OnSendText: "%s"\n' % text)
        self.process.GetOutputStream().write(text + '\n')
        self.inp.SetFocus()


    def OnCloseStream(self, evt):
        self.log.write('OnCloseStream\n')
        #print "b4 CloseOutput"
        self.process.CloseOutput()
        #print "after CloseOutput"

    def OnIdle(self, evt):
        if self.process is not None:
            stream = self.process.GetInputStream()

            if stream.CanRead():
                text = stream.read()
                self.out.AppendText(text)


    def OnProcessEnded(self, evt):
        self.log.write('OnProcessEnded, pid:%s,  exitCode: %s\n' %
                       (evt.GetPid(), evt.GetExitCode()))

        stream = self.process.GetInputStream()

        if stream.CanRead():
            text = stream.read()
            self.out.AppendText(text)

        self.process.Destroy()
        self.process = None
        self.inp.Enable(False)
        self.sndBtn.Enable(False)
        self.termBtn.Enable(False)
        self.cmd.Enable(True)
        self.exBtn.Enable(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, -1, log)
    return win


#----------------------------------------------------------------------


overview = """\
<html><body>
<h2>wx.Process</h2>

wx.Process lets you get notified when an asyncronous child process
started by wxExecute terminates, and also to get input/output streams
for the child process's stdout, stderr and stdin.

<p>
This demo launches a simple python script that echos back on stdout
lines that it reads from stdin.  You can send text to the echo
process' stdin by typing in the lower textctrl and clicking Send.

<p>
Clicking the Close Stream button will close the demo's end of the
stdin pipe to the child process.  In our case that will cause the
child process to exit its main loop.

</body><html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
