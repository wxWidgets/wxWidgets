#*******************
#By Daniel Pozmanter
#Under the GPL, etc, etc.

#Thanks to Robin Dunn for taking the time to show me how to do this
#via the mailing list.

#Version 0.0.0 ALPHA

#This is a hacked version of DragAndDrop.py from the wxPython demo 2.5.2.8

import  wx, wx.stc
from wx.lib.gestures import MouseGestures

#ToDo:

#Add a dialog to record gestures  (Have it showcase the manual mode)
#Allow users to remove gestures

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)

        ID_GESTURE = wx.NewId()

        self.log = log

        #Mouse Gestures:
    
        self.mg = MouseGestures(self, Auto=True)
        
        self.mg.SetGesturesVisible(True)
        
        self.mg.AddGesture('L', self.LogSomethingClever, 'You moved left')
        self.mg.AddGesture('9', self.LogSomethingClever, 'You moved right and up')
        self.mg.AddGesture('U', self.LogSomethingClever, 'You moved up')
        self.mg.AddGesture('DR', self.OnDownThenRight)        
        
        #Widgets:
            
        self.btnAddGesture = wx.Button(self, ID_GESTURE, 'Add New Gesture')
        
        msg = "Mouse Gestures"
        text = wx.StaticText(self, -1, "", style=wx.ALIGN_CENTRE)
        text.SetFont(wx.Font(24, wx.SWISS, wx.NORMAL, wx.BOLD, False))
        text.SetLabel(msg)

        w,h = text.GetTextExtent(msg)
        text.SetSize(wx.Size(w,h+1))
        text.SetForegroundColour(wx.BLUE)        
        
        #Sizer:
        outsideSizer = wx.BoxSizer(wx.VERTICAL)
        
        outsideSizer.Add(text, 0, wx.EXPAND|wx.ALL, 5)
        outsideSizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, '   '), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, 'Hold The Middle Mouse Button Down to Gesticulate'), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, 'Left, The Diagonal Up/Right, Down Then Right, and Up are Preset'), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, '   '), 0, wx.EXPAND)
        outsideSizer.Add(self.btnAddGesture, 0, wx.SHAPED)
        
        self.SetAutoLayout(True)
        self.SetSizer(outsideSizer)


        #Events:
        self.Bind(wx.EVT_BUTTON, self.OnAddGesture, id=ID_GESTURE)        

    def LogSomethingClever(self, somethingclever):
        self.log.WriteText(somethingclever)
    
    def OnAddGesture(self, event):
        d = wx.TextEntryDialog(self, "Enter Gesture (LRUD1379) (EG Right Then Up Then DownLeft is RU1):", "Add New Gesture", "")
        answer1 = d.ShowModal()
        gesture = d.GetValue()
        d.Destroy()
        d = wx.TextEntryDialog(self, 'Print the following text on "%s":' % gesture, "Gesture Action", "")
        answer2 = d.ShowModal()
        text = d.GetValue()
        d.Destroy()
        if (answer1 == wx.ID_OK) and (answer2 == wx.ID_OK):
            self.mg.AddGesture(gesture.upper(), self.LogSomethingClever, text)
        
    def OnDownThenRight(self):
        self.log.WriteText('You made an "L"!')

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """\
<html>
<body>
This demo shows how to add MouseGestures
to your program, and showcases the MouseGestures
class in all it's mousey glory.
<p><p>
</body>
</html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

