
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

        self.customThrobber = \
            throb.Throbber(self, -1, images, size=(36, 36),
                           frameDelay = 0.1,
                           rest = 4,
                           sequence = [ 1, 5, 2, 7, 3, 6, 4, 4, 4, 4, 7, 2, 2, 0 ]
                           )
                                                
        box = wx.BoxSizer(wx.VERTICAL)
        sizer = wx.GridBagSizer()
        box.Add(sizer, 1, wx.EXPAND|wx.ALL, 5)
        sizer.AddGrowableCol(1)

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

        # Add custom throbber to sizer.
        row += 2
        sizer.Add(
            self.customThrobber, (row, 0), (1, 1), 
            flag = wx.ALIGN_CENTER|wx.ALL, border=2
            )

        sizer.Add(
            wx.StaticText(self, -1, 'with custom & manual sequences'), 
            (row, 1), flag = wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_LEFT
            )

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
            buttonBox, (len(self.throbbers) + 2, 0), (1, 3), flag = wx.ALIGN_CENTER
            )

        # Buttoms for the custom throbber.
        nextButton = wx.Button(self, -1, "Next")
        self.Bind(wx.EVT_BUTTON, self.OnNext, nextButton)

        prevButton = wx.Button(self, -1, "Previous")
        self.Bind(wx.EVT_BUTTON, self.OnPrevious, prevButton)

        incButton = wx.Button(self, -1, "Increment")
        self.Bind(wx.EVT_BUTTON, self.OnIncrement, incButton)

        decButton = wx.Button(self, -1, "Decrement")
        self.Bind(wx.EVT_BUTTON, self.OnDecrement, decButton)

        revButton = wx.Button(self, -1, "Reverse")
        self.Bind(wx.EVT_BUTTON, self.OnReverse, revButton)

        restButton = wx.Button(self, -1, "Rest")
        self.Bind(wx.EVT_BUTTON, self.OnRest, restButton)

        startButton = wx.Button(self, -1, "Start")
        self.Bind(wx.EVT_BUTTON, self.OnStart, startButton)

        stopButton = wx.Button(self, -1, "Stop")
        self.Bind(wx.EVT_BUTTON, self.OnStop, stopButton)

        customBox1 = wx.BoxSizer(wx.HORIZONTAL)
        customBox1.AddMany([
            (nextButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (prevButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (incButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (decButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (revButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            ])

        customBox2 = wx.BoxSizer(wx.HORIZONTAL)
        customBox2.AddMany([
            (restButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (startButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            (stopButton, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5),
            ])

        sizer.Add( customBox1, (len(self.throbbers) + 5, 0), (1, 3), flag = wx.ALIGN_CENTER )
        sizer.Add( customBox2, (len(self.throbbers) + 6, 0), (1, 3), flag = wx.ALIGN_CENTER )

        # Layout.
        self.SetSizer(box)
        self.SetAutoLayout(True)
        self.Layout()
        sizer.SetSizeHints(self)
        sizer.Fit(self)

        for t in self.throbbers.keys():
            self.throbbers[t]['throbber'].Start()

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

    def OnNext(self, event):
        self.customThrobber.Next()

    def OnPrevious(self, event):
        self.customThrobber.Previous()

    def OnIncrement(self, event):
        self.customThrobber.Increment()

    def OnDecrement(self, event):
        self.customThrobber.Decrement()

    def OnReverse(self, event):
        self.customThrobber.Reverse()

    def OnRest(self, event):
        self.customThrobber.Rest()

    def OnStart(self, event):
        self.customThrobber.Start()

    def OnStop(self, event):
        self.customThrobber.Stop()

    def ShutdownDemo(self):
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
