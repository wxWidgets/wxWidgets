
import  wx

from Main import opj

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    bmp = wx.Image(opj('bitmaps/image.bmp'), wx.BITMAP_TYPE_BMP).ConvertToBitmap()
    gif = wx.Image(opj('bitmaps/image.gif'), wx.BITMAP_TYPE_GIF).ConvertToBitmap()
    png = wx.Image(opj('bitmaps/image.png'), wx.BITMAP_TYPE_PNG).ConvertToBitmap()
    jpg = wx.Image(opj('bitmaps/image.jpg'), wx.BITMAP_TYPE_JPEG).ConvertToBitmap()

    panel = wx.Panel(nb, -1)

    pos = 10
    wx.StaticBitmap(panel, -1, bmp, (10, pos), (bmp.GetWidth(), bmp.GetHeight()))

    pos = pos + bmp.GetHeight() + 10
    wx.StaticBitmap(panel, -1, gif, (10, pos), (gif.GetWidth(), gif.GetHeight()))

    pos = pos + gif.GetHeight() + 10
    wx.StaticBitmap(panel, -1, png, (10, pos), (png.GetWidth(), png.GetHeight()))

    pos = pos + png.GetHeight() + 10
    wx.StaticBitmap(panel, -1, jpg, (10, pos), (jpg.GetWidth(), jpg.GetHeight()))

    return panel

#----------------------------------------------------------------------



overview = """\
<html>
<body>
This class encapsulates a platform-independent image. An image can be created 
from data, or using <code>wxBitmap.ConvertToImage</code>. An image can be loaded from 
a file in a variety of formats, and is extensible to new formats via image 
format handlers. Functions are available to set and get image bits, so it can 
be used for basic image manipulation.

<p>The following image handlers are available. wxBMPHandler is always installed 
by default. To use other image formats, install the appropriate handler or use
<code>wx.InitAllImageHandlers()</code>.

<p>
<table>    
<tr><td width=25%>wxBMPHandler</td>  <td>For loading and saving, always installed.</td></tr>
<tr><td>wxPNGHandler</td>  <td>For loading and saving.</td>  </tr>
<tr><td>wxJPEGHandler</td>  <td>For loading and saving.</td>  </tr>
<tr><td>wxGIFHandler</td>  <td>Only for loading, due to legal issues.</td>  </tr>
<tr><td>wxPCXHandler</td>  <td>For loading and saving.</td>  </tr>
<tr><td>wxPNMHandler</td>  <td>For loading and saving.</td>  </tr>
<tr><td>wxTIFFHandler</td>  <td>For loading and saving.</td>  </tr>
<tr><td>wxIFFHandler</td>  <td>For loading only.</td>  </tr>
<tr><td>wxXPMHandler</td>  <td>For loading and saving.</td> </tr> 
<tr><td>wxICOHandler</td>  <td>For loading and saving.</td>  </tr>
<tr><td>wxCURHandler</td>  <td>For loading and saving.</td>  </tr>
<tr><td>wxANIHandler</td>  <td>For loading only.</td> </tr>
</table>

<p>When saving in PCX format, wxPCXHandler will count the number of different 
colours in the image; if there are 256 or less colours, it will save as 8 bit, 
else it will save as 24 bit.

<p>Loading PNMs only works for ASCII or raw RGB images. When saving in PNM format, 
wxPNMHandler will always save as raw RGB.

</body>
</html>"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

