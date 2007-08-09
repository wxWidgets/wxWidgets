
import wx
import wx.combo

import images

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        bcb = wx.combo.BitmapComboBox(self, pos=(25,25), size=(200,-1))

        for x in range(12):
            funcName = 'getLB%02dImage' % (x+1)
            func = getattr(images, funcName)
            img = func()
            img.Rescale(20,20)
            bmp = img.ConvertToBitmap()
            bcb.Append('images.%s()' % funcName, bmp, funcName)

        self.Bind(wx.EVT_COMBOBOX, self.OnCombo, bcb)

    def OnCombo(self, evt):
        bcb = evt.GetEventObject()
        idx = evt.GetInt()
        st  = bcb.GetString(idx)
        cd  = bcb.GetClientData(idx)
        self.log.write("EVT_COMBOBOX: Id %d, string '%s', clientData '%s'" % (idx, st, cd))

        
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.combo.BitmapComboBox</center></h2>

A combobox that displays a bitmap in front of the list items. It
currently only allows using bitmaps of one size, and resizes itself so
that a bitmap can be shown next to the text field.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

