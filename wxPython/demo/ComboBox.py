
import  wx

#---------------------------------------------------------------------------

class TestComboBox(wx.Panel):
    def OnSetFocus(self, evt):
        print "OnSetFocus"
        evt.Skip()

    def OnKillFocus(self, evt):
        print "OnKillFocus"
        evt.Skip()

    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      #'this is a long item that needs a scrollbar...',
                      'six', 'seven', 'eight']

        wx.StaticText(self, -1, "This example uses the wx.ComboBox control.", (8, 10))
        wx.StaticText(self, -1, "Select one:", (15, 50), (75, 18))

        # This combobox is created with a preset list of values.
        cb = wx.ComboBox(
            self, 500, "default value", (90, 50), 
            (95, -1), sampleList, wx.CB_DROPDOWN #|wxTE_PROCESS_ENTER
            )

        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBox, cb)
        self.Bind(wx.EVT_TEXT, self.EvtText, cb)
        self.Bind(wx.EVT_TEXT_ENTER, self.EvtTextEnter, cb)
        cb.Bind(wx.EVT_SET_FOCUS, self.OnSetFocus)
        cb.Bind(wx.EVT_KILL_FOCUS, self.OnKillFocus)

        # Once the combobox is set up, we can append some more data to it.
        cb.Append("foo", "This is some client data for this item")

        # This combobox is created with no values initially.
        cb = wx.ComboBox(
            self, 501, "default value", (90, 80), (95, -1), [], wx.CB_DROPDOWN)

        # Here we dynamically add our values to the second combobox.
        for item in sampleList:
            cb.Append(item, item.upper())

        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBox, cb)
        self.Bind(wx.EVT_COMBOBOX, self.EvtText, cb)

    # When the user selects something, we go here.
    def EvtComboBox(self, evt):
        cb = evt.GetEventObject()
        data = cb.GetClientData(cb.GetSelection())
        self.log.WriteText('EvtComboBox: %s\nClientData: %s\n' % (evt.GetString(), data))

        if evt.GetString() == 'one':
            self.log.WriteText("You follow directions well!\n\n")

    # Capture events every time a user hits a key in the text entry field.
    def EvtText(self, evt):
        self.log.WriteText('EvtText: %s\n' % evt.GetString())

    # Capture events when the user types something into the control then
    # hits ENTER.
    def EvtTextEnter(self, evt):
        self.log.WriteText('EvtTextEnter: %s' % evt.GetString())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestComboBox(nb, log)
    return win

#---------------------------------------------------------------------------


overview = """\
A ComboBox is like a combination of an edit control and a listbox. It can be 
displayed as static list with editable or read-only text field; or a drop-down 
list with text field; or a drop-down list without a text field.

This example shows both a preset ComboBox and one that is dynamically created
(that is, it is initially empty but then we 'grow' it out of program-supplied 
data). The former is common for read-only controls. 

This example also shows the two form factors for the ComboBox. The first is more
common, and resembles a Choice control. The latter, although less common, shows
how all the values in the ComboBox can be visible, yet the functionality is the
same for both.

Finally, this demo shows how event handling can differ. The first ComboBox is set
up to handle EVT_TEXT_ENTER events, in which text is typed in and then ENTER is
hit by the user. This allows the user to enter a line of text which can then be
processed by the program. EVT_TEXT can also be processed, but in that case the
event is generated every time that the user hits a key in the ComboBox entry field.

"""

#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

