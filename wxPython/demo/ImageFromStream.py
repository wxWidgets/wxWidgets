
import  cStringIO

import  wx

from Main import opj

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)

        data = open(opj('bitmaps/image.png'), "rb").read()
        stream = cStringIO.StringIO(data)

        bmp = wx.BitmapFromImage( wx.ImageFromStream( stream ))

        wx.StaticText(
            self, -1, "This image was loaded from a Python file-like object:", 
            (15, 15)
            )

        wx.StaticBitmap(self, -1, bmp, (15, 45))#, (bmp.GetWidth(), bmp.GetHeight()))



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """\
At long last there is finally a way to load any supported image type
directly from any Python file-like object, such as a memory buffer
using StringIO.  """


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
