
# images converted with wxPython's img2py.py tool

from wxPython.wx import wxBitmapFromXPMData, wxImageFromBitmap
import cPickle, zlib

##----------- Common Functions

def GetBitmap(ImageData):
    return wxBitmapFromXPMData(ImageData)

def GetImage(ImageData):
    return wxImageFromBitmap(GetBitmap(ImageData))

##----------- Image Data 

EofImageData = cPickle.loads(zlib.decompress(
'x\xda\xd3\xc8)0\xe4\nV7W0W0R0T\xe7J\x0cV\xd7SHVp\xcaIL\xce\x06\xf3\x14\x80<\
\xbf\xfc\xbcT(GA\x0f\x88\xa1l===\x18[\x0f\x04 l=\x08\xc0\x10GQ\x0f7G\x0f\x00\
\xec\xa2\x19\x96' ))
