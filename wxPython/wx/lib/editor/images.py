
# images converted with wxPython's img2py.py tool

# 12/14/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
#

import  cStringIO
import  wx

##----------- Common Functions

def GetBitmap(ImageData):
    return wx.BitmapFromImage(GetImage(ImageData))

def GetImage(ImageData):
    stream = cStringIO.StringIO(ImageData)
    return wx.ImageFromStream(stream)

##----------- Image Data

EofImageData = \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x07\x00\x00\x00\x07\x08\x06\
\x00\x00\x00\xc4RW\xd3\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\
\x00:IDATx\x9c]\x8e\xc1\r\x000\x08\x02\xc5\tn\xff)\xdd\xa0}\xd1`\xf9(\x9c\t\
\n(kf\x0e \xfbN\x90\xf3\xc1\x0c\xd2\xab\xaa\x16Huv\xa4\x00\xb5\x97\x1f\xac\
\x87\x1c\xe4\xe1\x05`2\x15\x9e\xc54\xca\xb4\x00\x00\x00\x00IEND\xaeB`\x82'




