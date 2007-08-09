#*******************
#By Daniel Pozmanter

#Thanks to Robin Dunn for taking the time to show me how to do this
#via the mailing list.

#Version 0.0.0

#This is a hacked version of DragAndDrop.py from the wxPython demo 2.5.2.8

import wx, wx.lib.dialogs
from wx.lib.gestures import MouseGestures

#ToDo:

#Add a dialog to record gestures  (Have it showcase the manual mode)
#Allow users to remove gestures

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)

        ID_GESTURE = wx.NewId()
        ID_MOUSE = wx.NewId()
        ID_MODIFIER = wx.NewId()
        ID_VISIBLE = wx.NewId()

        self.log = log

        #Mouse Gestures:
    
        self.mg = MouseGestures(self, Auto=True,
                                MouseButton=wx.MOUSE_BTN_RIGHT)
        
        self.mg.SetGesturesVisible(True)
        
        self.mg.AddGesture('LR', self.ShowSomethingClever, 'Left then Right!')
        self.mg.AddGesture('39', self.ShowSomethingClever, 'You made a V!')
        self.mg.AddGesture('L', self.LogSomethingClever, 'You moved left')
        self.mg.AddGesture('9', self.LogSomethingClever, 'You moved right and up')
        self.mg.AddGesture('U', self.LogSomethingClever, 'You moved up')
        self.mg.AddGesture('DR', self.OnDownThenRight)
        self.mg.AddGesture('LDRU', self.SetToBlue)
        self.mg.AddGesture('RDLU', self.SetToOrange)
        
        #Widgets:
            
        self.btnAddGesture = wx.Button(self, ID_GESTURE, 'Add New Gesture')
        self.btnChangeMouseButton = wx.Button(self, ID_MOUSE, 'Change Mouse Button')
        self.btnChangeModifier = wx.Button(self, ID_MODIFIER, 'Change Modifier')
        self.btnToggleVisible = wx.ToggleButton(self, ID_VISIBLE, 'Toggle Gestures Visible')
        self.btnToggleVisible.SetValue(True)
        
        msg = "Mouse Gestures"
        text = wx.StaticText(self, -1, "", style=wx.ALIGN_CENTRE)
        text.SetFont(wx.Font(24, wx.SWISS, wx.NORMAL, wx.BOLD, False))
        text.SetLabel(msg)

        w,h = text.GetTextExtent(msg)
        text.SetSize(wx.Size(w,h+1))
        text.SetForegroundColour(wx.BLUE)
        
        #Sizer:
        outsideSizer = wx.BoxSizer(wx.VERTICAL)
        
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)
        
        outsideSizer.Add(text, 0, wx.EXPAND|wx.ALL, 5)
        outsideSizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, '   '), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, 'Hold The Middle Mouse Button Down to Gesticulate'), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, 'Left Then Right, Left, The Diagonal Up/Right, Down Then Right, Diagonal Down/Right Then Diagonal Up/Right, and Up are Preset'), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, 'Left,Down,Right,Up Sets the line colour to Blue.'), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, 'Right,Down,Left,Up Sets the line colour to Orange.'), 0, wx.EXPAND)
        outsideSizer.Add(wx.StaticText(self, -1, '   '), 0, wx.EXPAND)
        btnSizer.Add(self.btnAddGesture, 0, wx.SHAPED)
        btnSizer.Add(self.btnChangeMouseButton, 0, wx.SHAPED)
        btnSizer.Add(self.btnChangeModifier, 0, wx.SHAPED)
        btnSizer.Add(self.btnToggleVisible, 0, wx.SHAPED)
        outsideSizer.Add(btnSizer, 0, wx.SHAPED)
        
        self.SetAutoLayout(True)
        self.SetSizer(outsideSizer)

        #Events:
        self.Bind(wx.EVT_BUTTON, self.OnAddGesture, id=ID_GESTURE)
        self.Bind(wx.EVT_BUTTON, self.OnChangeMouseButton, id=ID_MOUSE)
        self.Bind(wx.EVT_BUTTON, self.OnChangeModifiers, id=ID_MODIFIER)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.OnToggleVisible, id=ID_VISIBLE)

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
    
    def OnChangeModifiers(self, event):
        choices = [wx.WXK_CONTROL, wx.WXK_SHIFT, wx.WXK_ALT]
        schoices = ['Control', 'Shift', 'Alt']
        
        d = wx.lib.dialogs.MultipleChoiceDialog(self, 'Select Modifier Keys:\n(Select None if you do not want to use modifier keys\n\n', "Change Modifier Keys", schoices)
        answer = d.ShowModal()
        tuply = d.GetValue()
        d.Destroy()
        
        if (answer == wx.ID_OK):
            if len(tuply) > 0:
                modifiers = []
                modstring = ''
                for x in tuply:
                    modifiers.append(choices[x])
                    modstring += schoices[x] + ' '
                self.mg.SetModifiers(modifiers)
                self.log.WriteText('Set Modifiers to: ' + modstring)
            else:
                self.mg.SetModifiers()
                self.log.WriteText('UnSet All Modifiers')
    
    def OnChangeMouseButton(self, event):
        choices = [wx.MOUSE_BTN_LEFT, wx.MOUSE_BTN_MIDDLE, wx.MOUSE_BTN_RIGHT]
        schoices = ['Left', 'Middle', 'Right']
        d = wx.SingleChoiceDialog(self, "Set Mouse Button To", "Change Mouse Button", schoices,
                                  wx.CHOICEDLG_STYLE|wx.OK|wx.CANCEL)
        d.SetSize(wx.Size(250, 200))
        answer = d.ShowModal()
        i = d.GetSelection()
        d.Destroy()
        if (answer == wx.ID_OK):
            self.mg.SetMouseButton(choices[i])
            self.log.WriteText('Set the Mouse Button to ' + schoices[i])
        
    def OnDownThenRight(self):
        self.log.WriteText('You made an "L"!')

    def OnToggleVisible(self, event):
        visual = self.btnToggleVisible.GetValue()
        self.mg.SetGesturesVisible(visual)
        if visual:
            self.log.WriteText('Made Gestures Visible')
        else:
            self.log.WriteText('Made Gestures Invisible')

    def SetToBlue(self):
        self.mg.SetGesturePen(wx.Colour(0, 144, 255), 5)
        self.log.WriteText('Set Gesture Colour to Blue')
        
    def SetToOrange(self):
        self.mg.SetGesturePen(wx.Colour(255, 156, 0), 5)
        self.log.WriteText('Set Gesture Colour to Orange')

    def ShowSomethingClever(self, somethingclever):
        d = wx.MessageDialog(self, somethingclever, 'Mouse Gesture Action', wx.OK)
        d.ShowModal()
        d.Destroy()

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


