#
# Throbber.py - Cliff Wells <clifford.wells@attbi.com>
#

from wxPython.wx import *
from wxPython.lib.rcsizer import RowColSizer
from wxPython.lib.throbber import Throbber, __doc__ as docString
import throbImages # this was created using a modified version of img2py

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
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

        self.throbbers['plain']['throbber'] = Throbber(self, -1,
                                                       images, size=(36, 36),
                                                       frameDelay = 0.1)
        self.throbbers['reverse']['throbber'] = Throbber(self, -1, images, #size=(36, 36),
                                                         frameDelay = 0.07)
        self.throbbers['reverse']['throbber'].Reverse()
        self.throbbers['autoreverse']['throbber'] = Throbber(self, -1,
                                                             images, #size=(36, 36),
                                                             frameDelay = 0.1,
                                                             reverse = True)
        self.throbbers['autoreverse']['throbber'].sequence.append(0)
        self.throbbers['label']['throbber'] = Throbber(self, -1,
                                                       images, #size=(36, 36),
                                                       frameDelay = 0.1,
                                                       label = 'Label')
        self.throbbers['label']['throbber'].SetFont(wxFont(pointSize = 10,
                                                           family = wxDEFAULT,
                                                           style = wxNORMAL,
                                                           weight = wxBOLD))
        self.throbbers['overlay']['throbber'] = Throbber(self, -1,
                                                         images, #size=(36, 36),
                                                         frameDelay = 0.1,
                                                         overlay = throbImages.catalog['logo'].getBitmap())
        self.throbbers['overlay+text']['throbber'] = Throbber(self, -1,
                                                              images, #size=(36, 36),
                                                              frameDelay = 0.1,
                                                              overlay = throbImages.catalog['logo'].getBitmap(),
                                                              label = "Python!")
        self.throbbers['overlay+text']['throbber'].SetFont(wxFont(pointSize = 8,
                                                                  family = wxDEFAULT,
                                                                  style = wxNORMAL,
                                                                  weight = wxBOLD))


        # this throbber is created using a single, composite image
        self.otherThrobber = Throbber(self, -1,
                                      throbImages.catalog['eclouds'].getBitmap(), #size=(48, 48),
                                      frameDelay = 0.15,
                                      frames = 12,
                                      frameWidth = 48,
                                      label = "Stop")


        EVT_LEFT_DOWN(self.otherThrobber, self.OnClickThrobber)

        box = wxBoxSizer(wxVERTICAL)
        sizer = RowColSizer()
        box.Add(sizer, 1, wxEXPAND|wxALL, 5)
        sizer.AddGrowableCol(1)

        sizer.Add(self.otherThrobber, row = 0, col = 2, rowspan = 4, flag = wxALIGN_CENTER_VERTICAL)

        row = 2
        # use a list so we can keep our order
        for t in ['plain', 'reverse', 'autoreverse', 'label', 'overlay', 'overlay+text']:
            sizer.Add(self.throbbers[t]['throbber'], row = row, col = 0, flag = wxALIGN_CENTER|wxALL, border=2)
            sizer.Add(wxStaticText(self, -1, self.throbbers[t]['text']), row = row, col = 1,
                      flag = wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT)
            row += 1

        # start and stop buttons
        startButton = wxButton(self, -1, "Start")
        EVT_BUTTON(self, startButton.GetId(), self.OnStartAnimation)
        stopButton = wxButton(self, -1, "Stop")
        EVT_BUTTON(self, stopButton.GetId(), self.OnStopAnimation)

        buttonBox = wxBoxSizer(wxHORIZONTAL)
        buttonBox.AddMany([
            (startButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5),
            (stopButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5),
            ])

        sizer.Add(buttonBox,
                  row = len(self.throbbers) + 3,
                  col = 0,
                  colspan = 3,
                  flag = wxALIGN_CENTER)

        self.SetSizer(box)
        self.SetAutoLayout(True)
        self.Layout()
        sizer.SetSizeHints(self)
        sizer.Fit(self)

        for t in self.throbbers.keys():
            self.throbbers[t]['throbber'].Start()
        self.otherThrobber.Start()
        self.otherThrobber.Reverse()

        EVT_WINDOW_DESTROY(self, self.OnDestroy)

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
    if wxPlatform == "__WXMAC__":
        wxMessageBox("This demo currently fails on the Mac.",
                     "Sorry")
        return
    else:
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
    run.main(['', os.path.basename(sys.argv[0])])
