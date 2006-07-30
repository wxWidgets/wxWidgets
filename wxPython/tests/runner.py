from wxPython.wx import *
import sys, time

class WaitingProcess(wxProcess):
    def __init__(self):
        wxProcess.__init__(self, None)
        self.terminated = false
    def OnTerminate(self, pid, status):
        print pid, status
        self.terminated = true
    def wait(self):
        while not self.terminated:
            stream = self.GetInputStream()
            if not stream.eof():
                sys.stdout.write(stream.read())
            stream = self.GetErrorStream()
            if not stream.eof():
                sys.stderr.write(stream.read())
            wxYield()

try:
    #raw_input("ready...")
    if 1:
        process = WaitingProcess()
        process.Redirect()
        pid = wxExecute('python -u wxFrame1.py', false, process)
        process.wait()
    else:
        wxExecute('python -u wxFrame1.py')

finally:
    #raw_input("done...")
    pass



