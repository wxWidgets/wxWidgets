
import  wx
import  wx.lib.throbber as  throb

import throbImages # this was created using a modified version of img2py

from wx.lib.throbber import __doc__ as docString

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        # create the throbbers
        self.throbbers = {
            'plain':        { 'throbber': None,
                              'text':     "Plain throbber." },
            'reverse':      { 'throbber': None,
                              'text':     "This throbber runs in reverse and faster." },
            'autoreverse':  { 'throbber': None,
                              'text':     "This throbber switches direction." },
            'label':        { 'throbber': None,
                              'text':     "With a label." },
            'overlay':      { 'throbber': None,
                              'text':     "With an overlayed image." },
            'overlay+text': { 'throbber': None,
                              'text':     "With a label and an overlayed image." },
            }

        images = [throbImages.catalog[i].getBitmap()
                  for i in throbImages.index
                  if i not in ['eclouds', 'logo']]

        self.throbbers['plain']['throbber'] = \
            throb.Throbber(self, -1, images, size=(36, 36),frameDelay = 0.1)
                                                
        self.throbbers['reverse']['throbber'] = \
            throb.Throbber(self, -1, images, frameDelay = 0.07)

        self.throbbers['reverse']['throbber'].Reverse()

        self.throbbers['autoreverse']['throbber'] = \
            throb.Throbber(self, -1, images, frameDelay = 0.1, reverse = True)

        self.throbbers['autoreverse']['throbber'].sequence.append(0)

        self.throbbers['label']['throbber'] = \
            throb.Throbber(self, -1, images, frameDelay = 0.1, label = 'Label')

        self.throbbers['label']['throbber'].SetFont(wx.Font(
            pointSize = 10, family = wx.DEFAULT, style = wx.NORMAL, weight = wx.BOLD
            ))

        self.throbbers['overlay']['throbber'] = \
            throb.Throbber(
                self, -1, images, frameDelay = 0.1, 
                overlay = throbImages.catalog['logo'].getBitmap()
                )

        self.throbbers['overlay+text']['throbber'] = \
            throb.Throbber(
                self, -1, images, frameDelay = 0.1, 
                overlay = throbImages.catalog['logo'].getBitmap(), label = "Python!"
                )

        self.throbbers['overlay+text']['throbber'].SetFont(wx.Font(
            pointSize = 8, family = wx.DEFAULT, style = wx.NORMAL, weight = wx.BOLD
            ))

        # this throbber is created using a single, composite image
        self.otherThrobber = throb.Throbber(
            self, -1, throbImages.catalog['eclouds'].getBitmap(), frameDelay = 0.15, 
            frames = 12, frameWidth = 48, label = "Stop"
            )


        self.otherThrobber.Bind(wx.EVT_LEFT_DOWN, self.OnClickThrobber)

        box = wx.BoxSizer(wx.VERTICAL)
        sizer = wx.GridBagSizer()
        box.Add(sizer, 1, wx.EXPAND|wx.ALL, 5)
        sizer.AddGrowableCol(1)

        sizer.Add(self.otherThrobber, (0, 2), (4, 1),flag = wx.ALIGN_CENTER_VERTICAL)

        row = 2

        # use a list so we can keep our order
        for t in ['plain', 'reverse', 'autoreverse', 'label', 'overlay', 'overlay+text']:
            sizer.Add(
                self.throbbers[t]['throbber'], (row, 0), (1, 1), 
                flag = wx.ALIGN_CENTER|wx.ALL, border=2
                )

            sizer.Add(
                wx.StaticText(self, -1, self.throbbers[t]['text']), 
                (row, 1), flag = wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_LEFT
                )

            row += 1

        # start and stop buttons
        startButton = wx.Button(self, -1, "Start")
        self.Bind(wx.EVT_BUTTON, self.OnStartAnimation, startButton)

        stopButton = wx.Button(self, -1, "Stop")
        self.Bind(wx.EVT_BUTTON, self.OnStopAnimation, stopButton)

        buttonBox = wx.BoxSizer(wx.HORIZONTAL)
        buttonBox.AddMany([
            (startButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (stopButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            ])

        sizer.Add(
            buttonBox, (len(self.throbbers) + 3, 0), (1, 3), flag = wx.ALIGN_CENTER
            )

        self.SetSizer(box)
        self.SetAutoLayout(True)
        self.Layout()
        sizer.SetSizeHints(self)
        sizer.Fit(self)

        for t in self.throbbers.keys():
            self.throbbers[t]['throbber'].Start()

        self.otherThrobber.Start()
        self.otherThrobber.Reverse()

        self.Bind(wx.EVT_WINDOW_DESTROY, self.OnDestroy)

    def OnDestroy(self, event):
        self.log.write("got destroy event")
        event.Skip()

    def OnStartAnimation(self, event):
        for t in self.throbbers.keys():
            self.throbbers[t]['throbber'].Start()

    def OnStopAnimation(self, event):
        for t in self.throbbers.keys():
            self.throbbers[t]['throbber'].Rest()

    def OnClickThrobber(self, event):
        if self.otherThrobber.Running():
            self.otherThrobber.Rest()
            self.otherThrobber.SetLabel("Start")
        else:
            self.otherThrobber.Start()
            self.otherThrobber.SetLabel("Stop")

    def ShutdownDemo(self):
        self.otherThrobber.Rest()
        for t in self.throbbers.keys():
            self.throbbers[t]['throbber'].Rest()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h4><center>Throbber</center></h4>
<p>%s</p>
</body></html>
""" % docString



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
