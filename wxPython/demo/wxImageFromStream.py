
from wxPython.wx import *
from Main import opj

from cStringIO import StringIO

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        data = open(opj('bitmaps/image.png'), "rb").read()
        stream = StringIO(data)

        bmp = wxBitmapFromImage( wxImageFromStream( stream ))

        wxStaticText(self, -1,
                     "This image was loaded from a Python file-like object:",
                     (15, 15))
        wxStaticBitmap(self, -1, bmp, (15, 45))#, (bmp.GetWidth(), bmp.GetHeight()))



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
    run.main(['', os.path.basename(sys.argv[0])])

