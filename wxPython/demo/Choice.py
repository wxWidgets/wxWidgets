
import  wx

#---------------------------------------------------------------------------

class TestChoice(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight']

        wx.StaticText(self, -1, "This example uses the wxChoice control.", (15, 10))
        wx.StaticText(self, -1, "Select one:", (15, 50), (75, -1))
        self.ch = wx.Choice(self, -1, (100, 50), choices = sampleList)
        self.Bind(wx.EVT_CHOICE, self.EvtChoice, self.ch)


    def EvtChoice(self, event):
        self.log.WriteText('EvtChoice: %s\n' % event.GetString())
        self.ch.Append("A new item")
        
        if event.GetString() == 'one':
            self.log.WriteText('Well done!\n')


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestChoice(nb, log)
    return win

#---------------------------------------------------------------------------

overview = """
A Choice control is used to select one of a list of strings. Unlike a listbox, 
only the current selection is visible until the user pulls down the menu of 
choices.

This demo illustrates how to set up the Choice control and how to extract the
selected choice once it is selected. 

Note that the syntax of the constructor is different than the C++ implementation.
The number of choices and the choice array are consilidated into one python 
<code>list</code>.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

